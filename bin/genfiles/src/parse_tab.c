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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void
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
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void*
Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_starb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ( void*
t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_primop_exp( void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
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
struct _tuple8 Cyc_Parse_att_map[ 15u]={{{ _temp3, _temp3, _temp3 + 8u},( void*)
0u},{{ _temp6, _temp6, _temp6 + 6u},( void*) 1u},{{ _temp9, _temp9, _temp9 + 9u},(
void*) 2u},{{ _temp12, _temp12, _temp12 + 6u},( void*) 3u},{{ _temp15, _temp15,
_temp15 + 8u},( void*)& Cyc_Parse_att_aligned},{{ _temp18, _temp18, _temp18 + 7u},(
void*) 4u},{{ _temp21, _temp21, _temp21 + 7u},( void*) 6u},{{ _temp24, _temp24,
_temp24 + 7u},( void*) 7u},{{ _temp27, _temp27, _temp27 + 5u},( void*) 8u},{{
_temp30, _temp30, _temp30 + 10u},( void*) 9u},{{ _temp33, _temp33, _temp33 + 10u},(
void*) 10u},{{ _temp36, _temp36, _temp36 + 23u},( void*) 11u},{{ _temp39,
_temp39, _temp39 + 12u},( void*) 12u},{{ _temp42, _temp42, _temp42 + 11u},( void*)
13u},{{ _temp45, _temp45, _temp45 + 22u},( void*) 14u}}; static void Cyc_Parse_err(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Position_post_error(
Cyc_Position_mk_err_parse( sg, msg));} static void* Cyc_Parse_abort( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err( msg, sg);(
void) _throw(( void*) Cyc_Position_Exit);} static void Cyc_Parse_warn( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp46= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp47= msg;
fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n", _temp46.last_plus_one -
_temp46.curr, _temp46.curr, _temp47.last_plus_one - _temp47.curr, _temp47.curr);});
return;} static void* Cyc_Parse_unimp( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ return(( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp48= msg; xprintf("%.*s unimplemented",
_temp48.last_plus_one - _temp48.curr, _temp48.curr);}), sg);} static void Cyc_Parse_unimp2(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){({ struct
_tagged_string _temp49= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp50= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp49.last_plus_one - _temp49.curr, _temp49.curr, _temp50.last_plus_one -
_temp50.curr, _temp50.curr);}); return;} struct _tuple9{ struct _tuple7* f1;
struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct Cyc_Core_Opt*
_temp53; struct _tuple7* _temp55; struct _tuple9 _temp51=* field_info; _LL56:
_temp55= _temp51.f1; goto _LL54; _LL54: _temp53= _temp51.f2; goto _LL52; _LL52:
if((* _temp55).f4 != 0){ Cyc_Parse_err(({ char* _temp57="bad type params in struct field";
struct _tagged_string _temp58; _temp58.curr= _temp57; _temp58.base= _temp57;
_temp58.last_plus_one= _temp57 + 32; _temp58;}), loc);}{ struct _tuple1* _temp59=(*
_temp55).f1; goto _LL60; _LL60:{ void* _temp61=(* _temp59).f1; struct Cyc_List_List*
_temp71; struct Cyc_List_List* _temp73; _LL63: if(( unsigned int) _temp61 > 1u?*((
int*) _temp61) == Cyc_Absyn_Rel_n: 0){ _LL72: _temp71=(( struct Cyc_Absyn_Rel_n_struct*)
_temp61)->f1; if( _temp71 == 0){ goto _LL64;} else{ goto _LL65;}} else{ goto
_LL65;} _LL65: if(( unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Abs_n:
0){ _LL74: _temp73=(( struct Cyc_Absyn_Abs_n_struct*) _temp61)->f1; if( _temp73
== 0){ goto _LL66;} else{ goto _LL67;}} else{ goto _LL67;} _LL67: if( _temp61 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70;
_LL64: goto _LL62; _LL66: goto _LL62; _LL68: goto _LL62; _LL70: Cyc_Parse_err(({
char* _temp75="struct field cannot be qualified with a module name"; struct
_tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75; _temp76.last_plus_one=
_temp75 + 52; _temp76;}), loc); goto _LL62; _LL62:;}{ struct Cyc_List_List*
_temp77=(* _temp55).f5; goto _LL78; _LL78: return({ struct Cyc_Absyn_Structfield*
_temp79=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp79->name=(* _temp59).f2; _temp79->tq=(* _temp55).f2; _temp79->type=( void*)(*
_temp55).f3; _temp79->width= _temp53; _temp79->attributes= _temp77; _temp79;});}}}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp82; struct Cyc_List_List* _temp83; void* _temp85; struct Cyc_Absyn_Tqual
_temp87; struct Cyc_Core_Opt* _temp89; struct _tuple10* _temp80= field; _temp82=*
_temp80; _LL90: _temp89= _temp82.f1; goto _LL88; _LL88: _temp87= _temp82.f2;
goto _LL86; _LL86: _temp85= _temp82.f3; goto _LL84; _LL84: _temp83= _temp82.f4;
goto _LL81; _LL81: { struct Cyc_Core_Opt* _temp91= 0; goto _LL92; _LL92: if(
_temp89 != 0){ _temp91=({ struct Cyc_Core_Opt* _temp93=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp93->v=( void*)(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp89))->v)).f2; _temp93;});{ void* _temp94=(*((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp89))->v)).f1; struct
Cyc_List_List* _temp104; struct Cyc_List_List* _temp106; _LL96: if((
unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Rel_n: 0){ _LL105:
_temp104=(( struct Cyc_Absyn_Rel_n_struct*) _temp94)->f1; if( _temp104 == 0){
goto _LL97;} else{ goto _LL98;}} else{ goto _LL98;} _LL98: if(( unsigned int)
_temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Abs_n: 0){ _LL107: _temp106=((
struct Cyc_Absyn_Abs_n_struct*) _temp94)->f1; if( _temp106 == 0){ goto _LL99;}
else{ goto _LL100;}} else{ goto _LL100;} _LL100: if( _temp94 ==( void*) Cyc_Absyn_Loc_n){
goto _LL101;} else{ goto _LL102;} _LL102: goto _LL103; _LL97: goto _LL95; _LL99:
goto _LL95; _LL101: goto _LL95; _LL103: Cyc_Parse_err(({ char* _temp108="parameter cannot be qualified with a module name";
struct _tagged_string _temp109; _temp109.curr= _temp108; _temp109.base= _temp108;
_temp109.last_plus_one= _temp108 + 49; _temp109;}), loc); goto _LL95; _LL95:;}}
if( _temp83 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ char* _temp110="parameter should have no type parameters";
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
_temp115)->f1; goto _LL124; _LL124: _temp123=(( struct Cyc_Absyn_ArrayType_struct*)
_temp115)->f2; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_ArrayType_struct*)
_temp115)->f3; goto _LL118;} else{ goto _LL119;} _LL119: goto _LL120; _LL118:
return Cyc_Absyn_starb_typ( _temp125,( void*) Cyc_Absyn_HeapRgn, _temp123,
_temp121 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp127=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp127[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp128; _temp128.tag= Cyc_Absyn_Upper_b;
_temp128.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp121); _temp128;});
_temp127;})); _LL120: return t; _LL116:;} static void Cyc_Parse_arg_array2ptr(
struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple11*
_temp129=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp129->f1=(*
t).f2; _temp129->f2=(* t).f3; _temp129;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp130=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp156;
int _temp158; struct Cyc_Absyn_Exp* _temp160; struct Cyc_Core_Opt* _temp162;
struct Cyc_Core_Opt* _temp164; struct Cyc_Absyn_Pat* _temp166; struct Cyc_List_List*
_temp168; struct Cyc_Absyn_Fndecl* _temp170; struct Cyc_Absyn_Structdecl*
_temp172; struct Cyc_Absyn_Uniondecl* _temp174; struct Cyc_Absyn_Tuniondecl*
_temp176; struct Cyc_Absyn_Typedefdecl* _temp178; struct Cyc_Absyn_Enumdecl*
_temp180; struct Cyc_List_List* _temp182; struct _tagged_string* _temp184;
struct Cyc_List_List* _temp186; struct _tuple1* _temp188; struct Cyc_List_List*
_temp190; _LL132: if(*(( int*) _temp130) == Cyc_Absyn_Var_d){ _LL157: _temp156=((
struct Cyc_Absyn_Var_d_struct*) _temp130)->f1; goto _LL133;} else{ goto _LL134;}
_LL134: if(*(( int*) _temp130) == Cyc_Absyn_Let_d){ _LL167: _temp166=(( struct
Cyc_Absyn_Let_d_struct*) _temp130)->f1; goto _LL165; _LL165: _temp164=(( struct
Cyc_Absyn_Let_d_struct*) _temp130)->f2; goto _LL163; _LL163: _temp162=(( struct
Cyc_Absyn_Let_d_struct*) _temp130)->f3; goto _LL161; _LL161: _temp160=(( struct
Cyc_Absyn_Let_d_struct*) _temp130)->f4; goto _LL159; _LL159: _temp158=(( struct
Cyc_Absyn_Let_d_struct*) _temp130)->f5; goto _LL135;} else{ goto _LL136;} _LL136:
if(*(( int*) _temp130) == Cyc_Absyn_Letv_d){ _LL169: _temp168=(( struct Cyc_Absyn_Letv_d_struct*)
_temp130)->f1; goto _LL137;} else{ goto _LL138;} _LL138: if(*(( int*) _temp130)
== Cyc_Absyn_Fn_d){ _LL171: _temp170=(( struct Cyc_Absyn_Fn_d_struct*) _temp130)->f1;
goto _LL139;} else{ goto _LL140;} _LL140: if(*(( int*) _temp130) == Cyc_Absyn_Struct_d){
_LL173: _temp172=(( struct Cyc_Absyn_Struct_d_struct*) _temp130)->f1; goto
_LL141;} else{ goto _LL142;} _LL142: if(*(( int*) _temp130) == Cyc_Absyn_Union_d){
_LL175: _temp174=(( struct Cyc_Absyn_Union_d_struct*) _temp130)->f1; goto _LL143;}
else{ goto _LL144;} _LL144: if(*(( int*) _temp130) == Cyc_Absyn_Tunion_d){
_LL177: _temp176=(( struct Cyc_Absyn_Tunion_d_struct*) _temp130)->f1; goto
_LL145;} else{ goto _LL146;} _LL146: if(*(( int*) _temp130) == Cyc_Absyn_Typedef_d){
_LL179: _temp178=(( struct Cyc_Absyn_Typedef_d_struct*) _temp130)->f1; goto
_LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp130) == Cyc_Absyn_Enum_d){
_LL181: _temp180=(( struct Cyc_Absyn_Enum_d_struct*) _temp130)->f1; goto _LL149;}
else{ goto _LL150;} _LL150: if(*(( int*) _temp130) == Cyc_Absyn_Namespace_d){
_LL185: _temp184=(( struct Cyc_Absyn_Namespace_d_struct*) _temp130)->f1; goto
_LL183; _LL183: _temp182=(( struct Cyc_Absyn_Namespace_d_struct*) _temp130)->f2;
goto _LL151;} else{ goto _LL152;} _LL152: if(*(( int*) _temp130) == Cyc_Absyn_Using_d){
_LL189: _temp188=(( struct Cyc_Absyn_Using_d_struct*) _temp130)->f1; goto _LL187;
_LL187: _temp186=(( struct Cyc_Absyn_Using_d_struct*) _temp130)->f2; goto _LL153;}
else{ goto _LL154;} _LL154: if(*(( int*) _temp130) == Cyc_Absyn_ExternC_d){
_LL191: _temp190=(( struct Cyc_Absyn_ExternC_d_struct*) _temp130)->f1; goto
_LL155;} else{ goto _LL131;} _LL133: if( _temp156->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp192="initializers are not allowed in parameter declarations"; struct
_tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 55; _temp193;}), x->loc);}{ void* _temp194=(*
_temp156->name).f1; struct Cyc_List_List* _temp204; struct Cyc_List_List*
_temp206; _LL196: if( _temp194 ==( void*) Cyc_Absyn_Loc_n){ goto _LL197;} else{
goto _LL198;} _LL198: if(( unsigned int) _temp194 > 1u?*(( int*) _temp194) ==
Cyc_Absyn_Rel_n: 0){ _LL205: _temp204=(( struct Cyc_Absyn_Rel_n_struct*)
_temp194)->f1; if( _temp204 == 0){ goto _LL199;} else{ goto _LL200;}} else{ goto
_LL200;} _LL200: if(( unsigned int) _temp194 > 1u?*(( int*) _temp194) == Cyc_Absyn_Abs_n:
0){ _LL207: _temp206=(( struct Cyc_Absyn_Abs_n_struct*) _temp194)->f1; if(
_temp206 == 0){ goto _LL201;} else{ goto _LL202;}} else{ goto _LL202;} _LL202:
goto _LL203; _LL197: goto _LL195; _LL199: goto _LL195; _LL201: goto _LL195;
_LL203: Cyc_Parse_err(({ char* _temp208="module names not allowed on parameter declarations";
struct _tagged_string _temp209; _temp209.curr= _temp208; _temp209.base= _temp208;
_temp209.last_plus_one= _temp208 + 51; _temp209;}), x->loc); goto _LL195; _LL195:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp156->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp210=*(*
_temp156->name).f2; xprintf("%.*s is not listed as a parameter", _temp210.last_plus_one
- _temp210.curr, _temp210.curr);}), x->loc);} return;} _LL135: decl_kind=({ char*
_temp211="let declaration"; struct _tagged_string _temp212; _temp212.curr=
_temp211; _temp212.base= _temp211; _temp212.last_plus_one= _temp211 + 16;
_temp212;}); goto _LL131; _LL137: decl_kind=({ char* _temp213="let declaration";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 16; _temp214;}); goto _LL131; _LL139:
decl_kind=({ char* _temp215="function declaration"; struct _tagged_string
_temp216; _temp216.curr= _temp215; _temp216.base= _temp215; _temp216.last_plus_one=
_temp215 + 21; _temp216;}); goto _LL131; _LL141: decl_kind=({ char* _temp217="struct declaration";
struct _tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 19; _temp218;}); goto _LL131; _LL143:
decl_kind=({ char* _temp219="union declaration"; struct _tagged_string _temp220;
_temp220.curr= _temp219; _temp220.base= _temp219; _temp220.last_plus_one=
_temp219 + 18; _temp220;}); goto _LL131; _LL145: decl_kind=({ char* _temp221="tunion declaration";
struct _tagged_string _temp222; _temp222.curr= _temp221; _temp222.base= _temp221;
_temp222.last_plus_one= _temp221 + 19; _temp222;}); goto _LL131; _LL147:
decl_kind=({ char* _temp223="typedef"; struct _tagged_string _temp224; _temp224.curr=
_temp223; _temp224.base= _temp223; _temp224.last_plus_one= _temp223 + 8;
_temp224;}); goto _LL131; _LL149: decl_kind=({ char* _temp225="enum declaration";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 17; _temp226;}); goto _LL131; _LL151:
decl_kind=({ char* _temp227="namespace declaration"; struct _tagged_string
_temp228; _temp228.curr= _temp227; _temp228.base= _temp227; _temp228.last_plus_one=
_temp227 + 22; _temp228;}); goto _LL131; _LL153: decl_kind=({ char* _temp229="using declaration";
struct _tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 18; _temp230;}); goto _LL131; _LL155:
decl_kind=({ char* _temp231="extern C declaration"; struct _tagged_string
_temp232; _temp232.curr= _temp231; _temp232.base= _temp231; _temp232.last_plus_one=
_temp231 + 21; _temp232;}); goto _LL131; _LL131:;}(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp233= decl_kind; xprintf("%.*s appears in parameter type",
_temp233.last_plus_one - _temp233.curr, _temp233.curr);}), x->loc); return;}
struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_string* x){ struct _tuple12 _temp236; struct Cyc_Position_Segment*
_temp237; struct Cyc_List_List* _temp239; struct _tuple12* _temp234= env;
_temp236=* _temp234; _LL240: _temp239= _temp236.f1; goto _LL238; _LL238:
_temp237= _temp236.f2; goto _LL235; _LL235: if( _temp239 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp241=* x; xprintf("missing type for parameter %.*s",
_temp241.last_plus_one - _temp241.curr, _temp241.curr);}), _temp237);}{ void*
_temp242=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp239))->hd)->r; struct Cyc_Absyn_Vardecl* _temp248; _LL244: if(*(( int*)
_temp242) == Cyc_Absyn_Var_d){ _LL249: _temp248=(( struct Cyc_Absyn_Var_d_struct*)
_temp242)->f1; goto _LL245;} else{ goto _LL246;} _LL246: goto _LL247; _LL245:{
void* _temp250=(* _temp248->name).f1; struct Cyc_List_List* _temp260; struct Cyc_List_List*
_temp262; _LL252: if( _temp250 ==( void*) Cyc_Absyn_Loc_n){ goto _LL253;} else{
goto _LL254;} _LL254: if(( unsigned int) _temp250 > 1u?*(( int*) _temp250) ==
Cyc_Absyn_Rel_n: 0){ _LL261: _temp260=(( struct Cyc_Absyn_Rel_n_struct*)
_temp250)->f1; if( _temp260 == 0){ goto _LL255;} else{ goto _LL256;}} else{ goto
_LL256;} _LL256: if(( unsigned int) _temp250 > 1u?*(( int*) _temp250) == Cyc_Absyn_Abs_n:
0){ _LL263: _temp262=(( struct Cyc_Absyn_Abs_n_struct*) _temp250)->f1; if(
_temp262 == 0){ goto _LL257;} else{ goto _LL258;}} else{ goto _LL258;} _LL258:
goto _LL259; _LL253: goto _LL251; _LL255: goto _LL251; _LL257: goto _LL251;
_LL259: Cyc_Parse_err(({ char* _temp264="module name not allowed on parameter";
struct _tagged_string _temp265; _temp265.curr= _temp264; _temp265.base= _temp264;
_temp265.last_plus_one= _temp264 + 37; _temp265;}), _temp237); goto _LL251;
_LL251:;} if( Cyc_String_zstrptrcmp((* _temp248->name).f2, x) == 0){ return({
struct _tuple2* _temp266=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp266[ 0]=({ struct _tuple2 _temp267; _temp267.f1=({ struct Cyc_Core_Opt*
_temp268=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp268[ 0]=({ struct Cyc_Core_Opt _temp269; _temp269.v=( void*)(* _temp248->name).f2;
_temp269;}); _temp268;}); _temp267.f2= _temp248->tq; _temp267.f3=( void*)
_temp248->type; _temp267;}); _temp266;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp270=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp270->f1=(( struct Cyc_List_List*) _check_null( _temp239))->tl; _temp270->f2=
_temp237; _temp270;}), x);} _LL247: return(( struct _tuple2*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ char*
_temp271="non-variable declaration"; struct _tagged_string _temp272; _temp272.curr=
_temp271; _temp272.base= _temp271; _temp272.last_plus_one= _temp271 + 25;
_temp272;}),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp239))->hd)->loc); _LL243:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp273= tm; int _temp279; struct Cyc_Position_Segment* _temp281; struct
Cyc_List_List* _temp283; _LL275: if(( unsigned int) _temp273 > 1u?*(( int*)
_temp273) == Cyc_Absyn_TypeParams_mod: 0){ _LL284: _temp283=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp273)->f1; goto _LL282; _LL282: _temp281=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp273)->f2; goto _LL280; _LL280: _temp279=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp273)->f3; goto _LL276;} else{ goto _LL277;} _LL277: goto _LL278; _LL276:
return 1; _LL278: return 0; _LL274:;} static void* Cyc_Parse_id2type( struct
_tagged_string s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,({ char*
_temp285="`H"; struct _tagged_string _temp286; _temp286.curr= _temp285; _temp286.base=
_temp285; _temp286.last_plus_one= _temp285 + 3; _temp286;})) == 0){ return( void*)
Cyc_Absyn_HeapRgn;} else{ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp287=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp287[ 0]=({ struct Cyc_Absyn_VarType_struct _temp288; _temp288.tag= Cyc_Absyn_VarType;
_temp288.f1=({ struct Cyc_Absyn_Tvar* _temp289=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp289->name=({ struct
_tagged_string* _temp290=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp290[ 0]= s; _temp290;}); _temp289->kind= k; _temp289;});
_temp288;}); _temp287;});}} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment* loc, void* t){ void* _temp291= t; struct Cyc_Absyn_Tvar*
_temp297; _LL293: if(( unsigned int) _temp291 > 4u?*(( int*) _temp291) == Cyc_Absyn_VarType:
0){ _LL298: _temp297=(( struct Cyc_Absyn_VarType_struct*) _temp291)->f1; goto
_LL294;} else{ goto _LL295;} _LL295: goto _LL296; _LL294: return _temp297;
_LL296: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ char* _temp299="expecting a list of type variables, not types";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 46; _temp300;}), loc); _LL292:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp301=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp301[ 0]=({ struct Cyc_Absyn_VarType_struct _temp302; _temp302.tag= Cyc_Absyn_VarType;
_temp302.f1= pr; _temp302;}); _temp301;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp303=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp309; _LL305:
if(( unsigned int) _temp303 > 1u?*(( int*) _temp303) == Cyc_Absyn_Function_mod:
0){ _LL310: _temp309=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp303)->f1;
goto _LL306;} else{ goto _LL307;} _LL307: goto _LL308; _LL306: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp311= _temp309; struct Cyc_Core_Opt* _temp317; int _temp319;
struct Cyc_List_List* _temp321; struct Cyc_Position_Segment* _temp323; struct
Cyc_List_List* _temp325; _LL313: if(*(( int*) _temp311) == Cyc_Absyn_WithTypes){
_LL322: _temp321=(( struct Cyc_Absyn_WithTypes_struct*) _temp311)->f1; goto
_LL320; _LL320: _temp319=(( struct Cyc_Absyn_WithTypes_struct*) _temp311)->f2;
goto _LL318; _LL318: _temp317=(( struct Cyc_Absyn_WithTypes_struct*) _temp311)->f3;
goto _LL314;} else{ goto _LL315;} _LL315: if(*(( int*) _temp311) == Cyc_Absyn_NoTypes){
_LL326: _temp325=(( struct Cyc_Absyn_NoTypes_struct*) _temp311)->f1; goto _LL324;
_LL324: _temp323=(( struct Cyc_Absyn_NoTypes_struct*) _temp311)->f2; goto _LL316;}
else{ goto _LL312;} _LL314: Cyc_Parse_warn(({ char* _temp327="function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp328; _temp328.curr= _temp327; _temp328.base= _temp327;
_temp328.last_plus_one= _temp327 + 92; _temp328;}), loc); return tms; _LL316:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp325, tds);{ struct _tuple12* _temp330=({ struct _tuple12* _temp329=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp329->f1= tds; _temp329->f2=
loc; _temp329;}); goto _LL331; _LL331: return({ struct Cyc_List_List* _temp332=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp332->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp333=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp333[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp334; _temp334.tag= Cyc_Absyn_Function_mod;
_temp334.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp335=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp335[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp336; _temp336.tag= Cyc_Absyn_WithTypes;
_temp336.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_string*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp330, _temp325); _temp336.f2= 0; _temp336.f3= 0;
_temp336;}); _temp335;})); _temp334;}); _temp333;})); _temp332->tl= 0; _temp332;});}
_LL312:;} else{ return({ struct Cyc_List_List* _temp337=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp337->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( tms))->hd); _temp337->tl= Cyc_Parse_oldstyle2newstyle(((
struct Cyc_List_List*) _check_null( tms))->tl, tds, loc); _temp337;});} _LL308:
return({ struct Cyc_List_List* _temp338=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp338->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd); _temp338->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*)
_check_null( tms))->tl, tds, loc); _temp338;}); _LL304:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp339=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp339->id= d->id; _temp339->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp339;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp340=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL342: if( _temp340 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL343;} else{ goto _LL344;} _LL344: if(
_temp340 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL345;} else{ goto _LL346;}
_LL346: if( _temp340 ==( void*) Cyc_Parse_Static_sc){ goto _LL347;} else{ goto
_LL348;} _LL348: goto _LL349; _LL343: sc=( void*) Cyc_Absyn_Extern; goto _LL341;
_LL345: sc=( void*) Cyc_Absyn_ExternC; goto _LL341; _LL347: sc=( void*) Cyc_Absyn_Static;
goto _LL341; _LL349: Cyc_Parse_err(({ char* _temp350="bad storage class on function";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 30; _temp351;}), loc); goto _LL341; _LL341:;}}{
struct Cyc_Core_Opt* _temp354; void* _temp356; struct _tuple5 _temp352= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL357: _temp356= _temp352.f1; goto _LL355; _LL355: _temp354=
_temp352.f2; goto _LL353; _LL353: { struct Cyc_List_List* _temp360; struct Cyc_List_List*
_temp362; void* _temp364; struct Cyc_Absyn_Tqual _temp366; struct _tuple6
_temp358= Cyc_Parse_apply_tms( tq, _temp356, atts, d->tms); _LL367: _temp366=
_temp358.f1; goto _LL365; _LL365: _temp364= _temp358.f2; goto _LL363; _LL363:
_temp362= _temp358.f3; goto _LL361; _LL361: _temp360= _temp358.f4; goto _LL359;
_LL359: if( _temp354 != 0){ Cyc_Parse_warn(({ char* _temp368="nested type declaration within function prototype";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 50; _temp369;}), loc);} if( _temp362 != 0){
Cyc_Parse_warn(({ char* _temp370="bad type params, ignoring"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 26; _temp371;}), loc);}{ void* _temp372=
_temp364; struct Cyc_Absyn_FnInfo _temp378; struct Cyc_List_List* _temp380; int
_temp382; struct Cyc_List_List* _temp384; void* _temp386; struct Cyc_Core_Opt*
_temp388; struct Cyc_List_List* _temp390; _LL374: if(( unsigned int) _temp372 >
4u?*(( int*) _temp372) == Cyc_Absyn_FnType: 0){ _LL379: _temp378=(( struct Cyc_Absyn_FnType_struct*)
_temp372)->f1; _LL391: _temp390= _temp378.tvars; goto _LL389; _LL389: _temp388=
_temp378.effect; goto _LL387; _LL387: _temp386=( void*) _temp378.ret_typ; goto
_LL385; _LL385: _temp384= _temp378.args; goto _LL383; _LL383: _temp382= _temp378.varargs;
goto _LL381; _LL381: _temp380= _temp378.attributes; goto _LL375;} else{ goto
_LL376;} _LL376: goto _LL377; _LL375: { struct Cyc_List_List* _temp392=(( struct
Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_fnargspec_to_arg, loc, _temp384); goto _LL393; _LL393: return({ struct
Cyc_Absyn_Fndecl* _temp394=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct
Cyc_Absyn_Fndecl)); _temp394->sc=( void*) sc; _temp394->name= d->id; _temp394->tvs=
_temp390; _temp394->is_inline= is_inline; _temp394->effect= _temp388; _temp394->ret_type=(
void*) _temp386; _temp394->args= _temp392; _temp394->varargs= _temp382; _temp394->body=
body; _temp394->cached_typ= 0; _temp394->param_vardecls= 0; _temp394->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp380, _temp360); _temp394;});} _LL377: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp395="declarator is not a function prototype"; struct _tagged_string
_temp396; _temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 39; _temp396;}), loc); _LL373:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(({
char* _temp397="missing argument variable in function prototype"; struct
_tagged_string _temp398; _temp398.curr= _temp397; _temp398.base= _temp397;
_temp398.last_plus_one= _temp397 + 48; _temp398;}), loc); return({ struct
_tuple4* _temp399=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp399->f1=({ struct _tagged_string* _temp400=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp400[ 0]=({ char* _temp401="?";
struct _tagged_string _temp402; _temp402.curr= _temp401; _temp402.base= _temp401;
_temp402.last_plus_one= _temp401 + 2; _temp402;}); _temp400;}); _temp399->f2=(*
t).f2; _temp399->f3=(* t).f3; _temp399;});} else{ return({ struct _tuple4*
_temp403=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp403->f1=(
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp403->f2=(* t).f2; _temp403->f3=(* t).f3; _temp403;});}} static char
_temp406[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1={ _temp406, _temp406, _temp406 + 52u};
static char _temp409[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2={ _temp409, _temp409, _temp409 + 63u};
static char _temp412[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3={ _temp412, _temp412, _temp412 + 50u};
static char _temp415[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4={ _temp415, _temp415, _temp415 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp416=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp430; void* _temp432; struct Cyc_Position_Segment*
_temp434; struct Cyc_Position_Segment* _temp436; struct Cyc_Position_Segment*
_temp438; struct Cyc_Position_Segment* _temp440; struct Cyc_Absyn_Decl* _temp442;
_LL418: if(*(( int*) _temp416) == Cyc_Parse_Type_spec){ _LL433: _temp432=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp416)->f1; goto _LL431; _LL431: _temp430=((
struct Cyc_Parse_Type_spec_struct*) _temp416)->f2; goto _LL419;} else{ goto
_LL420;} _LL420: if(*(( int*) _temp416) == Cyc_Parse_Signed_spec){ _LL435:
_temp434=(( struct Cyc_Parse_Signed_spec_struct*) _temp416)->f1; goto _LL421;}
else{ goto _LL422;} _LL422: if(*(( int*) _temp416) == Cyc_Parse_Unsigned_spec){
_LL437: _temp436=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp416)->f1; goto
_LL423;} else{ goto _LL424;} _LL424: if(*(( int*) _temp416) == Cyc_Parse_Short_spec){
_LL439: _temp438=(( struct Cyc_Parse_Short_spec_struct*) _temp416)->f1; goto
_LL425;} else{ goto _LL426;} _LL426: if(*(( int*) _temp416) == Cyc_Parse_Long_spec){
_LL441: _temp440=(( struct Cyc_Parse_Long_spec_struct*) _temp416)->f1; goto
_LL427;} else{ goto _LL428;} _LL428: if(*(( int*) _temp416) == Cyc_Parse_Decl_spec){
_LL443: _temp442=(( struct Cyc_Parse_Decl_spec_struct*) _temp416)->f1; goto
_LL429;} else{ goto _LL417;} _LL419: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp430);} last_loc= _temp430; seen_type= 1; t= _temp432; goto _LL417; _LL421:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp434);} if( seen_type){ Cyc_Parse_err(({
char* _temp444="signed qualifier must come before type"; struct _tagged_string
_temp445; _temp445.curr= _temp444; _temp445.base= _temp444; _temp445.last_plus_one=
_temp444 + 39; _temp445;}), _temp434);} last_loc= _temp434; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Signed; goto _LL417; _LL423: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4,
_temp436);} if( seen_type){ Cyc_Parse_err(({ char* _temp446="signed qualifier must come before type";
struct _tagged_string _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 39; _temp447;}), _temp436);} last_loc=
_temp436; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL417; _LL425:
if( seen_size){ Cyc_Parse_err(({ char* _temp448="integral size may appear only once within a type specifier";
struct _tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 59; _temp449;}), _temp438);} if( seen_type){
Cyc_Parse_err(({ char* _temp450="short modifier must come before base type";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 42; _temp451;}), _temp438);} last_loc=
_temp438; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL417; _LL427: if(
seen_type){ Cyc_Parse_err(({ char* _temp452="long modifier must come before base type";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 41; _temp453;}), _temp440);} if( seen_size){
void* _temp454= sz; _LL456: if( _temp454 ==( void*) Cyc_Absyn_B4){ goto _LL457;}
else{ goto _LL458;} _LL458: goto _LL459; _LL457: sz=( void*) Cyc_Absyn_B8; goto
_LL455; _LL459: Cyc_Parse_err(({ char* _temp460="extra long in type specifier";
struct _tagged_string _temp461; _temp461.curr= _temp460; _temp461.base= _temp460;
_temp461.last_plus_one= _temp460 + 29; _temp461;}), _temp440); goto _LL455;
_LL455:;} last_loc= _temp440; seen_size= 1; goto _LL417; _LL429: last_loc=
_temp442->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp462=(
void*) _temp442->r; struct Cyc_Absyn_Structdecl* _temp474; struct Cyc_Absyn_Tuniondecl*
_temp476; struct Cyc_Absyn_Uniondecl* _temp478; struct Cyc_Absyn_Enumdecl*
_temp480; _LL464: if(*(( int*) _temp462) == Cyc_Absyn_Struct_d){ _LL475:
_temp474=(( struct Cyc_Absyn_Struct_d_struct*) _temp462)->f1; goto _LL465;}
else{ goto _LL466;} _LL466: if(*(( int*) _temp462) == Cyc_Absyn_Tunion_d){
_LL477: _temp476=(( struct Cyc_Absyn_Tunion_d_struct*) _temp462)->f1; goto
_LL467;} else{ goto _LL468;} _LL468: if(*(( int*) _temp462) == Cyc_Absyn_Union_d){
_LL479: _temp478=(( struct Cyc_Absyn_Union_d_struct*) _temp462)->f1; goto _LL469;}
else{ goto _LL470;} _LL470: if(*(( int*) _temp462) == Cyc_Absyn_Enum_d){ _LL481:
_temp480=(( struct Cyc_Absyn_Enum_d_struct*) _temp462)->f1; goto _LL471;} else{
goto _LL472;} _LL472: goto _LL473; _LL465: { struct Cyc_List_List* _temp482=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp474->tvs); goto _LL483; _LL483: t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp484=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp484[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp485; _temp485.tag= Cyc_Absyn_StructType;
_temp485.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp474->name))->v); _temp485.f2= _temp482; _temp485.f3= 0;
_temp485;}); _temp484;}); if( _temp474->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp486=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp486->v=( void*) _temp442; _temp486;});} goto _LL463;} _LL467: { struct Cyc_List_List*
_temp487=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp476->tvs);
goto _LL488; _LL488: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp489=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp489[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp490; _temp490.tag= Cyc_Absyn_TunionType;
_temp490.f1=({ struct Cyc_Absyn_TunionInfo _temp491; _temp491.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp492=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp492[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp493; _temp493.tag= Cyc_Absyn_KnownTunion;
_temp493.f1= _temp476; _temp493;}); _temp492;})); _temp491.targs= _temp487;
_temp491.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp491;}); _temp490;});
_temp489;}); if( _temp476->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp494=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp494->v=( void*) _temp442; _temp494;});} goto _LL463;} _LL469: { struct Cyc_List_List*
_temp495=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp478->tvs);
goto _LL496; _LL496: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp497=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp497[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp498; _temp498.tag= Cyc_Absyn_UnionType;
_temp498.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp478->name))->v); _temp498.f2= _temp495; _temp498.f3= 0;
_temp498;}); _temp497;}); if( _temp478->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp499=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp499->v=( void*) _temp442; _temp499;});} goto _LL463;} _LL471: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp500=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp500[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp501; _temp501.tag= Cyc_Absyn_EnumType; _temp501.f1=
_temp480->name; _temp501.f2= 0; _temp501;}); _temp500;}); declopt=({ struct Cyc_Core_Opt*
_temp502=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp502->v=( void*) _temp442; _temp502;}); goto _LL463; _LL473:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp503="bad declaration within type specifier"; struct _tagged_string
_temp504; _temp504.curr= _temp503; _temp504.base= _temp503; _temp504.last_plus_one=
_temp503 + 38; _temp504;}), _temp442->loc); goto _LL463; _LL463:;}} else{ Cyc_Parse_err(
Cyc_Parse_msg3, _temp442->loc);} goto _LL417; _LL417:;} if( ! seen_type){ if( !
seen_sign? ! seen_size: 0){ Cyc_Parse_err(({ char* _temp505="missing type withing specifier";
struct _tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 31; _temp506;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp507=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp507[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp508; _temp508.tag= Cyc_Absyn_IntType; _temp508.f1=( void*) sgn; _temp508.f2=(
void*) sz; _temp508;}); _temp507;});} else{ if( seen_sign){ void* _temp509= t;
void* _temp515; void* _temp517; _LL511: if(( unsigned int) _temp509 > 4u?*(( int*)
_temp509) == Cyc_Absyn_IntType: 0){ _LL518: _temp517=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp509)->f1; goto _LL516; _LL516: _temp515=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp509)->f2; goto _LL512;} else{ goto _LL513;} _LL513: goto _LL514; _LL512: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp519=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp519[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp520; _temp520.tag= Cyc_Absyn_IntType; _temp520.f1=( void*) sgn; _temp520.f2=(
void*) _temp515; _temp520;}); _temp519;}); goto _LL510; _LL514: Cyc_Parse_err(({
char* _temp521="sign specification on non-integral type"; struct _tagged_string
_temp522; _temp522.curr= _temp521; _temp522.base= _temp521; _temp522.last_plus_one=
_temp521 + 40; _temp522;}), last_loc); goto _LL510; _LL510:;} if( seen_size){
void* _temp523= t; void* _temp529; void* _temp531; _LL525: if(( unsigned int)
_temp523 > 4u?*(( int*) _temp523) == Cyc_Absyn_IntType: 0){ _LL532: _temp531=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp523)->f1; goto _LL530; _LL530:
_temp529=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp523)->f2; goto _LL526;}
else{ goto _LL527;} _LL527: goto _LL528; _LL526: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp533=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp533[ 0]=({ struct Cyc_Absyn_IntType_struct _temp534; _temp534.tag= Cyc_Absyn_IntType;
_temp534.f1=( void*) _temp531; _temp534.f2=( void*) sz; _temp534;}); _temp533;});
goto _LL524; _LL528: Cyc_Parse_err(({ char* _temp535="size qualifier on non-integral type";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 36; _temp536;}), last_loc); goto _LL524;
_LL524:;}} return({ struct _tuple5 _temp537; _temp537.f1= t; _temp537.f2=
declopt; _temp537;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp538=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL539; _LL539: { struct _tuple1* _temp540= _temp538->id; goto _LL541;
_LL541: { struct Cyc_List_List* _temp544; struct Cyc_List_List* _temp546; void*
_temp548; struct Cyc_Absyn_Tqual _temp550; struct _tuple6 _temp542= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp538->tms); _LL551: _temp550= _temp542.f1; goto _LL549;
_LL549: _temp548= _temp542.f2; goto _LL547; _LL547: _temp546= _temp542.f3; goto
_LL545; _LL545: _temp544= _temp542.f4; goto _LL543; _LL543: return({ struct Cyc_List_List*
_temp552=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp552->hd=( void*)({ struct _tuple7* _temp553=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp553->f1= _temp540; _temp553->f2= _temp550;
_temp553->f3= _temp548; _temp553->f4= _temp546; _temp553->f5= _temp544; _temp553;});
_temp552->tl= Cyc_Parse_apply_tmss( _temp550, t,(( struct Cyc_List_List*)
_check_null( ds))->tl, shared_atts); _temp552;});}}}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp554= a; int _temp568; _LL556: if(( unsigned int) _temp554 >
15u?*(( int*) _temp554) == Cyc_Absyn_Regparm_att: 0){ _LL569: _temp568=(( struct
Cyc_Absyn_Regparm_att_struct*) _temp554)->f1; goto _LL557;} else{ goto _LL558;}
_LL558: if( _temp554 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL559;} else{ goto
_LL560;} _LL560: if( _temp554 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL561;}
else{ goto _LL562;} _LL562: if( _temp554 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL563;} else{ goto _LL564;} _LL564: if( _temp554 ==( void*) Cyc_Absyn_Const_att){
goto _LL565;} else{ goto _LL566;} _LL566: goto _LL567; _LL557: goto _LL559;
_LL559: goto _LL561; _LL561: goto _LL563; _LL563: goto _LL565; _LL565: return 1;
_LL567: return 0; _LL555:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp570; _temp570.f1= tq; _temp570.f2=
t; _temp570.f3= 0; _temp570.f4= atts; _temp570;});}{ void* _temp571=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp585;
void* _temp587; int _temp589; struct Cyc_Position_Segment* _temp591; struct Cyc_List_List*
_temp593; struct Cyc_Absyn_Tqual _temp595; void* _temp597; void* _temp599;
struct Cyc_List_List* _temp601; struct Cyc_Position_Segment* _temp603; _LL573:
if( _temp571 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL574;} else{ goto _LL575;}
_LL575: if(( unsigned int) _temp571 > 1u?*(( int*) _temp571) == Cyc_Absyn_ConstArray_mod:
0){ _LL586: _temp585=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp571)->f1;
goto _LL576;} else{ goto _LL577;} _LL577: if(( unsigned int) _temp571 > 1u?*((
int*) _temp571) == Cyc_Absyn_Function_mod: 0){ _LL588: _temp587=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp571)->f1; goto _LL578;} else{ goto
_LL579;} _LL579: if(( unsigned int) _temp571 > 1u?*(( int*) _temp571) == Cyc_Absyn_TypeParams_mod:
0){ _LL594: _temp593=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp571)->f1;
goto _LL592; _LL592: _temp591=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp571)->f2; goto _LL590; _LL590: _temp589=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp571)->f3; goto _LL580;} else{ goto _LL581;} _LL581: if(( unsigned int)
_temp571 > 1u?*(( int*) _temp571) == Cyc_Absyn_Pointer_mod: 0){ _LL600: _temp599=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp571)->f1; goto _LL598;
_LL598: _temp597=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp571)->f2;
goto _LL596; _LL596: _temp595=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp571)->f3;
goto _LL582;} else{ goto _LL583;} _LL583: if(( unsigned int) _temp571 > 1u?*((
int*) _temp571) == Cyc_Absyn_Attributes_mod: 0){ _LL604: _temp603=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp571)->f1; goto _LL602; _LL602: _temp601=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp571)->f2; goto _LL584;} else{ goto _LL572;} _LL574: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp605=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp605[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp606; _temp606.tag= Cyc_Absyn_ArrayType;
_temp606.f1=( void*) t; _temp606.f2= tq; _temp606.f3= 0; _temp606;}); _temp605;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL576: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp607=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp607[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp608; _temp608.tag= Cyc_Absyn_ArrayType;
_temp608.f1=( void*) t; _temp608.f2= tq; _temp608.f3=( struct Cyc_Absyn_Exp*)
_temp585; _temp608;}); _temp607;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL578: { void* _temp609= _temp587; struct Cyc_Core_Opt* _temp615;
int _temp617; struct Cyc_List_List* _temp619; struct Cyc_Position_Segment*
_temp621; struct Cyc_List_List* _temp623; _LL611: if(*(( int*) _temp609) == Cyc_Absyn_WithTypes){
_LL620: _temp619=(( struct Cyc_Absyn_WithTypes_struct*) _temp609)->f1; goto
_LL618; _LL618: _temp617=(( struct Cyc_Absyn_WithTypes_struct*) _temp609)->f2;
goto _LL616; _LL616: _temp615=(( struct Cyc_Absyn_WithTypes_struct*) _temp609)->f3;
goto _LL612;} else{ goto _LL613;} _LL613: if(*(( int*) _temp609) == Cyc_Absyn_NoTypes){
_LL624: _temp623=(( struct Cyc_Absyn_NoTypes_struct*) _temp609)->f1; goto _LL622;
_LL622: _temp621=(( struct Cyc_Absyn_NoTypes_struct*) _temp609)->f2; goto _LL614;}
else{ goto _LL610;} _LL612: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as != 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Parse_fn_type_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp625=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp625->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp625->tl= fn_atts; _temp625;});} else{ new_atts=({ struct Cyc_List_List*
_temp626=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp626->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp626->tl= new_atts; _temp626;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp627=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp633; struct Cyc_Position_Segment*
_temp635; struct Cyc_List_List* _temp637; _LL629: if(( unsigned int) _temp627 >
1u?*(( int*) _temp627) == Cyc_Absyn_TypeParams_mod: 0){ _LL638: _temp637=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp627)->f1; goto _LL636; _LL636:
_temp635=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp627)->f2; goto _LL634;
_LL634: _temp633=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp627)->f3; goto
_LL630;} else{ goto _LL631;} _LL631: goto _LL632; _LL630: typvars= _temp637; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL628; _LL632: goto _LL628;
_LL628:;} if(((( ! _temp617? _temp619 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp619))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp619))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp619))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp619= 0;
_temp617= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp619);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp615, t, _temp619, _temp617, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL614:( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ char*
_temp639="function declaration without parameter types"; struct _tagged_string
_temp640; _temp640.curr= _temp639; _temp640.base= _temp639; _temp640.last_plus_one=
_temp639 + 45; _temp640;}), _temp621)); _LL610:;} _LL580: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0){ return({ struct _tuple6 _temp641; _temp641.f1= tq;
_temp641.f2= t; _temp641.f3= _temp593; _temp641.f4= atts; _temp641;});}( void)
_throw((( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(({ char* _temp642="type parameters must appear before function arguments in declarator";
struct _tagged_string _temp643; _temp643.curr= _temp642; _temp643.base= _temp642;
_temp643.last_plus_one= _temp642 + 68; _temp643;}), _temp591)); _LL582: { void*
_temp644= _temp599; struct Cyc_Absyn_Exp* _temp652; struct Cyc_Absyn_Exp*
_temp654; _LL646: if(( unsigned int) _temp644 > 1u?*(( int*) _temp644) == Cyc_Absyn_NonNullable_ps:
0){ _LL653: _temp652=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp644)->f1;
goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int) _temp644 > 1u?*((
int*) _temp644) == Cyc_Absyn_Nullable_ps: 0){ _LL655: _temp654=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp644)->f1; goto _LL649;} else{ goto _LL650;} _LL650: if( _temp644 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL651;} else{ goto _LL645;} _LL647: return Cyc_Parse_apply_tms(
_temp595, Cyc_Absyn_atb_typ( t, _temp597, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp656=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp656[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp657; _temp657.tag= Cyc_Absyn_Upper_b;
_temp657.f1= _temp652; _temp657;}); _temp656;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL649: return Cyc_Parse_apply_tms( _temp595, Cyc_Absyn_starb_typ(
t, _temp597, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp658=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp658[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp659; _temp659.tag= Cyc_Absyn_Upper_b;
_temp659.f1= _temp654; _temp659;}); _temp658;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL651: return Cyc_Parse_apply_tms( _temp595, Cyc_Absyn_tagged_typ(
t, _temp597, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL645:;}
_LL584: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp601),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL572:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp662; void* _temp664; struct _tuple5 _temp660= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL665: _temp664= _temp660.f1; goto _LL663; _LL663: _temp662=
_temp660.f2; goto _LL661; _LL661: if( _temp662 != 0){ Cyc_Parse_warn(({ char*
_temp666="ignoring nested type declaration(s) in specifier list"; struct
_tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
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
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(({ char* _temp670="inline is only allowed on function definitions";
struct _tagged_string _temp671; _temp671.curr= _temp670; _temp671.base= _temp670;
_temp671.last_plus_one= _temp670 + 47; _temp671;}), loc);} if( tss == 0){ Cyc_Parse_err(({
char* _temp672="missing type specifiers in declaration"; struct _tagged_string
_temp673; _temp673.curr= _temp672; _temp673.base= _temp672; _temp673.last_plus_one=
_temp672 + 39; _temp673;}), loc); return 0;} if( ds->sc != 0){ void* _temp674=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL676: if( _temp674 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL677;} else{ goto _LL678;} _LL678: if(
_temp674 ==( void*) Cyc_Parse_Extern_sc){ goto _LL679;} else{ goto _LL680;}
_LL680: if( _temp674 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL681;} else{ goto
_LL682;} _LL682: if( _temp674 ==( void*) Cyc_Parse_Static_sc){ goto _LL683;}
else{ goto _LL684;} _LL684: if( _temp674 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL685;} else{ goto _LL686;} _LL686: if( _temp674 ==( void*) Cyc_Parse_Register_sc){
goto _LL687;} else{ goto _LL688;} _LL688: if( _temp674 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL689;} else{ goto _LL675;} _LL677: istypedef= 1; goto _LL675; _LL679: s=(
void*) Cyc_Absyn_Extern; goto _LL675; _LL681: s=( void*) Cyc_Absyn_ExternC; goto
_LL675; _LL683: s=( void*) Cyc_Absyn_Static; goto _LL675; _LL685: s=( void*) Cyc_Absyn_Public;
goto _LL675; _LL687: s=( void*) Cyc_Absyn_Public; goto _LL675; _LL689: s=( void*)
Cyc_Absyn_Abstract; goto _LL675; _LL675:;}{ struct Cyc_List_List* _temp692;
struct Cyc_List_List* _temp694; struct _tuple0 _temp690=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL695: _temp694= _temp690.f1;
goto _LL693; _LL693: _temp692= _temp690.f2; goto _LL691; _LL691: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp692; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp696= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL697; _LL697: if( _temp694 == 0){ struct Cyc_Core_Opt* _temp700; void*
_temp702; struct _tuple5 _temp698= _temp696; _LL703: _temp702= _temp698.f1; goto
_LL701; _LL701: _temp700= _temp698.f2; goto _LL699; _LL699: if( _temp700 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp700))->v;{ void* _temp704=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp716; struct Cyc_Absyn_Structdecl* _temp718; struct Cyc_Absyn_Uniondecl*
_temp720; struct Cyc_Absyn_Tuniondecl* _temp722; _LL706: if(*(( int*) _temp704)
== Cyc_Absyn_Enum_d){ _LL717: _temp716=(( struct Cyc_Absyn_Enum_d_struct*)
_temp704)->f1; goto _LL707;} else{ goto _LL708;} _LL708: if(*(( int*) _temp704)
== Cyc_Absyn_Struct_d){ _LL719: _temp718=(( struct Cyc_Absyn_Struct_d_struct*)
_temp704)->f1; goto _LL709;} else{ goto _LL710;} _LL710: if(*(( int*) _temp704)
== Cyc_Absyn_Union_d){ _LL721: _temp720=(( struct Cyc_Absyn_Union_d_struct*)
_temp704)->f1; goto _LL711;} else{ goto _LL712;} _LL712: if(*(( int*) _temp704)
== Cyc_Absyn_Tunion_d){ _LL723: _temp722=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp704)->f1; goto _LL713;} else{ goto _LL714;} _LL714: goto _LL715; _LL707:(
void*)( _temp716->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(({ char*
_temp724="bad attributes on enum"; struct _tagged_string _temp725; _temp725.curr=
_temp724; _temp725.base= _temp724; _temp725.last_plus_one= _temp724 + 23;
_temp725;}), loc);} goto _LL705; _LL709:( void*)( _temp718->sc=( void*) s);
_temp718->attributes= atts; goto _LL705; _LL711:( void*)( _temp720->sc=( void*)
s); _temp720->attributes= atts; goto _LL705; _LL713:( void*)( _temp722->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(({ char* _temp726="bad attributes on tunion";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 25; _temp727;}), loc);} goto _LL705; _LL715:
Cyc_Parse_err(({ char* _temp728="bad declaration"; struct _tagged_string
_temp729; _temp729.curr= _temp728; _temp729.base= _temp728; _temp729.last_plus_one=
_temp728 + 16; _temp729;}), loc); return 0; _LL705:;} return({ struct Cyc_List_List*
_temp730=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp730->hd=( void*) d; _temp730->tl= 0; _temp730;});} else{ void* _temp731=
_temp702; struct Cyc_Absyn_Structdecl** _temp745; struct Cyc_List_List* _temp747;
struct _tuple1* _temp749; struct Cyc_Absyn_TunionInfo _temp751; void* _temp753;
struct Cyc_List_List* _temp755; void* _temp757; struct Cyc_Absyn_Tuniondecl*
_temp759; struct Cyc_Absyn_TunionInfo _temp761; void* _temp763; struct Cyc_List_List*
_temp765; void* _temp767; struct Cyc_Absyn_UnknownTunionInfo _temp769; int
_temp771; struct _tuple1* _temp773; struct Cyc_Absyn_Uniondecl** _temp775;
struct Cyc_List_List* _temp777; struct _tuple1* _temp779; struct Cyc_Absyn_Enumdecl*
_temp781; struct _tuple1* _temp783; _LL733: if(( unsigned int) _temp731 > 4u?*((
int*) _temp731) == Cyc_Absyn_StructType: 0){ _LL750: _temp749=(( struct Cyc_Absyn_StructType_struct*)
_temp731)->f1; goto _LL748; _LL748: _temp747=(( struct Cyc_Absyn_StructType_struct*)
_temp731)->f2; goto _LL746; _LL746: _temp745=(( struct Cyc_Absyn_StructType_struct*)
_temp731)->f3; goto _LL734;} else{ goto _LL735;} _LL735: if(( unsigned int)
_temp731 > 4u?*(( int*) _temp731) == Cyc_Absyn_TunionType: 0){ _LL752: _temp751=((
struct Cyc_Absyn_TunionType_struct*) _temp731)->f1; _LL758: _temp757=( void*)
_temp751.tunion_info; if(*(( int*) _temp757) == Cyc_Absyn_KnownTunion){ _LL760:
_temp759=(( struct Cyc_Absyn_KnownTunion_struct*) _temp757)->f1; goto _LL756;}
else{ goto _LL737;} _LL756: _temp755= _temp751.targs; goto _LL754; _LL754:
_temp753=( void*) _temp751.rgn; goto _LL736;} else{ goto _LL737;} _LL737: if((
unsigned int) _temp731 > 4u?*(( int*) _temp731) == Cyc_Absyn_TunionType: 0){
_LL762: _temp761=(( struct Cyc_Absyn_TunionType_struct*) _temp731)->f1; _LL768:
_temp767=( void*) _temp761.tunion_info; if(*(( int*) _temp767) == Cyc_Absyn_UnknownTunion){
_LL770: _temp769=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp767)->f1;
_LL774: _temp773= _temp769.name; goto _LL772; _LL772: _temp771= _temp769.is_xtunion;
goto _LL766;} else{ goto _LL739;} _LL766: _temp765= _temp761.targs; goto _LL764;
_LL764: _temp763=( void*) _temp761.rgn; goto _LL738;} else{ goto _LL739;} _LL739:
if(( unsigned int) _temp731 > 4u?*(( int*) _temp731) == Cyc_Absyn_UnionType: 0){
_LL780: _temp779=(( struct Cyc_Absyn_UnionType_struct*) _temp731)->f1; goto
_LL778; _LL778: _temp777=(( struct Cyc_Absyn_UnionType_struct*) _temp731)->f2;
goto _LL776; _LL776: _temp775=(( struct Cyc_Absyn_UnionType_struct*) _temp731)->f3;
goto _LL740;} else{ goto _LL741;} _LL741: if(( unsigned int) _temp731 > 4u?*((
int*) _temp731) == Cyc_Absyn_EnumType: 0){ _LL784: _temp783=(( struct Cyc_Absyn_EnumType_struct*)
_temp731)->f1; goto _LL782; _LL782: _temp781=(( struct Cyc_Absyn_EnumType_struct*)
_temp731)->f2; goto _LL742;} else{ goto _LL743;} _LL743: goto _LL744; _LL734: {
struct Cyc_List_List* _temp785=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp747);
goto _LL786; _LL786: { struct Cyc_Absyn_Structdecl* _temp789=({ struct Cyc_Absyn_Structdecl*
_temp787=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp787->sc=( void*) s; _temp787->name=({ struct Cyc_Core_Opt* _temp788=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp788->v=(
void*)(( struct _tuple1*) _check_null( _temp749)); _temp788;}); _temp787->tvs=
_temp785; _temp787->fields= 0; _temp787->attributes= 0; _temp787;}); goto _LL790;
_LL790: if( atts != 0){ Cyc_Parse_err(({ char* _temp791="bad attributes on struct";
struct _tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 25; _temp792;}), loc);} return({ struct Cyc_List_List*
_temp793=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp793->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp794=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp794[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp795; _temp795.tag= Cyc_Absyn_Struct_d;
_temp795.f1= _temp789; _temp795;}); _temp794;}), loc); _temp793->tl= 0; _temp793;});}}
_LL736: if( atts != 0){ Cyc_Parse_err(({ char* _temp796="bad attributes on tunion";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 25; _temp797;}), loc);} return({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp798->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp799=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp799[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp800; _temp800.tag= Cyc_Absyn_Tunion_d;
_temp800.f1= _temp759; _temp800;}); _temp799;}), loc); _temp798->tl= 0; _temp798;});
_LL738: { struct Cyc_List_List* _temp801=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp765);
goto _LL802; _LL802: { struct Cyc_Absyn_Decl* _temp803= Cyc_Absyn_tunion_decl( s,
_temp773, _temp801, 0, _temp771, loc); goto _LL804; _LL804: if( atts != 0){ Cyc_Parse_err(({
char* _temp805="bad attributes on tunion"; struct _tagged_string _temp806;
_temp806.curr= _temp805; _temp806.base= _temp805; _temp806.last_plus_one=
_temp805 + 25; _temp806;}), loc);} return({ struct Cyc_List_List* _temp807=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp807->hd=(
void*) _temp803; _temp807->tl= 0; _temp807;});}} _LL740: { struct Cyc_List_List*
_temp808=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp777); goto _LL809; _LL809: { struct Cyc_Absyn_Uniondecl*
_temp812=({ struct Cyc_Absyn_Uniondecl* _temp810=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp810->sc=( void*) s;
_temp810->name=({ struct Cyc_Core_Opt* _temp811=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp811->v=( void*)(( struct _tuple1*)
_check_null( _temp779)); _temp811;}); _temp810->tvs= _temp808; _temp810->fields=
0; _temp810->attributes= 0; _temp810;}); goto _LL813; _LL813: if( atts != 0){
Cyc_Parse_err(({ char* _temp814="bad attributes on union"; struct _tagged_string
_temp815; _temp815.curr= _temp814; _temp815.base= _temp814; _temp815.last_plus_one=
_temp814 + 24; _temp815;}), loc);} return({ struct Cyc_List_List* _temp816=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp816->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp817=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp817->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp818=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp818[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp819; _temp819.tag= Cyc_Absyn_Union_d;
_temp819.f1= _temp812; _temp819;}); _temp818;})); _temp817->loc= loc; _temp817;});
_temp816->tl= 0; _temp816;});}} _LL742: { struct Cyc_Absyn_Enumdecl* _temp821=({
struct Cyc_Absyn_Enumdecl* _temp820=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp820->sc=( void*) s; _temp820->name=
_temp783; _temp820->fields= 0; _temp820;}); goto _LL822; _LL822: if( atts != 0){
Cyc_Parse_err(({ char* _temp823="bad attributes on enum"; struct _tagged_string
_temp824; _temp824.curr= _temp823; _temp824.base= _temp823; _temp824.last_plus_one=
_temp823 + 23; _temp824;}), loc);} return({ struct Cyc_List_List* _temp825=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp825->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp826=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp826->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp827=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp827[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp828; _temp828.tag= Cyc_Absyn_Enum_d;
_temp828.f1= _temp821; _temp828;}); _temp827;})); _temp826->loc= loc; _temp826;});
_temp825->tl= 0; _temp825;});} _LL744: Cyc_Parse_err(({ char* _temp829="missing declarator";
struct _tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 19; _temp830;}), loc); return 0; _LL732:;}}
else{ void* t= _temp696.f1; struct Cyc_List_List* _temp831= Cyc_Parse_apply_tmss(
tq, t, _temp694, atts); goto _LL832; _LL832: if( istypedef){ if( ! exps_empty){
Cyc_Parse_err(({ char* _temp833="initializer in typedef declaration"; struct
_tagged_string _temp834; _temp834.curr= _temp833; _temp834.base= _temp833;
_temp834.last_plus_one= _temp833 + 35; _temp834;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp831); if( _temp696.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp696.f2))->v;{ void* _temp835=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp847; struct Cyc_Absyn_Tuniondecl* _temp849; struct Cyc_Absyn_Uniondecl*
_temp851; struct Cyc_Absyn_Enumdecl* _temp853; _LL837: if(*(( int*) _temp835) ==
Cyc_Absyn_Struct_d){ _LL848: _temp847=(( struct Cyc_Absyn_Struct_d_struct*)
_temp835)->f1; goto _LL838;} else{ goto _LL839;} _LL839: if(*(( int*) _temp835)
== Cyc_Absyn_Tunion_d){ _LL850: _temp849=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp835)->f1; goto _LL840;} else{ goto _LL841;} _LL841: if(*(( int*) _temp835)
== Cyc_Absyn_Union_d){ _LL852: _temp851=(( struct Cyc_Absyn_Union_d_struct*)
_temp835)->f1; goto _LL842;} else{ goto _LL843;} _LL843: if(*(( int*) _temp835)
== Cyc_Absyn_Enum_d){ _LL854: _temp853=(( struct Cyc_Absyn_Enum_d_struct*)
_temp835)->f1; goto _LL844;} else{ goto _LL845;} _LL845: goto _LL846; _LL838:(
void*)( _temp847->sc=( void*) s); _temp847->attributes= atts; atts= 0; goto
_LL836; _LL840:( void*)( _temp849->sc=( void*) s); goto _LL836; _LL842:( void*)(
_temp851->sc=( void*) s); goto _LL836; _LL844:( void*)( _temp853->sc=( void*) s);
goto _LL836; _LL846: Cyc_Parse_err(({ char* _temp855="declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp856; _temp856.curr= _temp855; _temp856.base= _temp855;
_temp856.last_plus_one= _temp855 + 70; _temp856;}), loc); goto _LL836; _LL836:;}
decls=({ struct Cyc_List_List* _temp857=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp857->hd=( void*) d; _temp857->tl= decls;
_temp857;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp858=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s in typedef", _temp858.last_plus_one - _temp858.curr,
_temp858.curr);}), loc);} return decls;}} else{ if( _temp696.f2 != 0){ Cyc_Parse_unimp2(({
char* _temp859="nested type declaration within declarator"; struct
_tagged_string _temp860; _temp860.curr= _temp859; _temp860.base= _temp859;
_temp860.last_plus_one= _temp859 + 42; _temp860;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp861= _temp831; goto _LL862; _LL862: for( 0;
_temp861 != 0;( _temp861=(( struct Cyc_List_List*) _check_null( _temp861))->tl,
_temp692=(( struct Cyc_List_List*) _check_null( _temp692))->tl)){ struct _tuple7
_temp865; struct Cyc_List_List* _temp866; struct Cyc_List_List* _temp868; void*
_temp870; struct Cyc_Absyn_Tqual _temp872; struct _tuple1* _temp874; struct
_tuple7* _temp863=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp861))->hd; _temp865=* _temp863; _LL875: _temp874= _temp865.f1; goto _LL873;
_LL873: _temp872= _temp865.f2; goto _LL871; _LL871: _temp870= _temp865.f3; goto
_LL869; _LL869: _temp868= _temp865.f4; goto _LL867; _LL867: _temp866= _temp865.f5;
goto _LL864; _LL864: if( _temp868 != 0){ Cyc_Parse_warn(({ char* _temp876="bad type params, ignoring";
struct _tagged_string _temp877; _temp877.curr= _temp876; _temp877.base= _temp876;
_temp877.last_plus_one= _temp876 + 26; _temp877;}), loc);} if( _temp692 == 0){((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp878="unexpected null in parse!"; struct _tagged_string _temp879;
_temp879.curr= _temp878; _temp879.base= _temp878; _temp879.last_plus_one=
_temp878 + 26; _temp879;}), loc);}{ struct Cyc_Absyn_Exp* _temp880=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp692))->hd; goto _LL881; _LL881: {
struct Cyc_Absyn_Vardecl* _temp882= Cyc_Absyn_new_vardecl( _temp874, _temp870,
_temp880); goto _LL883; _LL883: _temp882->tq= _temp872;( void*)( _temp882->sc=(
void*) s); _temp882->attributes= _temp866;{ struct Cyc_Absyn_Decl* _temp887=({
struct Cyc_Absyn_Decl* _temp884=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp884->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp885=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp885[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp886; _temp886.tag= Cyc_Absyn_Var_d;
_temp886.f1= _temp882; _temp886;}); _temp885;})); _temp884->loc= loc; _temp884;});
goto _LL888; _LL888: decls=({ struct Cyc_List_List* _temp889=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp889->hd=( void*) _temp887;
_temp889->tl= decls; _temp889;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp890= s; xprintf("bad kind: %.*s",
_temp890.last_plus_one - _temp890.curr, _temp890.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), 0)){
case 'A': _LL891: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL892: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL893: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL894: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL895: return(
void*) Cyc_Absyn_EffKind; default: _LL896: Cyc_Parse_err(({ struct
_tagged_string _temp898= s; xprintf("bad kind: %.*s", _temp898.last_plus_one -
_temp898.curr, _temp898.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp899=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp899->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp900=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp900[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp901; _temp901.tag= Cyc_Absyn_Attributes_mod;
_temp901.f1= loc; _temp901.f2= atts; _temp901;}); _temp900;})); _temp899->tl=
tms; _temp899;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp902= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp902.last_plus_one - _temp902.curr, _temp902.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp903=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp903[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp904; _temp904.tag= Cyc_Absyn_Typedef_d; _temp904.f1=({
struct Cyc_Absyn_Typedefdecl* _temp905=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp905->name= x; _temp905->tvs=(*
t).f4; _temp905->defn=( void*)(* t).f3; _temp905;}); _temp904;}); _temp903;}),
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
} ; static char _temp909[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp909, _temp909, _temp909 + 8u}};
struct _tuple16* Cyc_yyget_Int_tok( void* yy1){ void* _temp910= yy1; struct
_tuple16* _temp916; _LL912: if(*(( void**) _temp910) == Cyc_Int_tok){ _LL917:
_temp916=(( struct Cyc_Int_tok_struct*) _temp910)->f1; goto _LL913;} else{ goto
_LL914;} _LL914: goto _LL915; _LL913: return _temp916; _LL915:( void) _throw((
void*)& Cyc_yyfail_Int_tok); _LL911:;} static char _temp921[ 11u]="String_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={ Cyc_Core_Failure,{
_temp921, _temp921, _temp921 + 11u}}; struct _tagged_string Cyc_yyget_String_tok(
void* yy1){ void* _temp922= yy1; struct _tagged_string _temp928; _LL924: if(*((
void**) _temp922) == Cyc_String_tok){ _LL929: _temp928=(( struct Cyc_String_tok_struct*)
_temp922)->f1; goto _LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925:
return _temp928; _LL927:( void) _throw(( void*)& Cyc_yyfail_String_tok); _LL923:;}
static char _temp933[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{ _temp933, _temp933, _temp933 + 9u}}; char Cyc_yyget_Char_tok(
void* yy1){ void* _temp934= yy1; char _temp940; _LL936: if(*(( void**) _temp934)
== Cyc_Char_tok){ _LL941: _temp940=(( struct Cyc_Char_tok_struct*) _temp934)->f1;
goto _LL937;} else{ goto _LL938;} _LL938: goto _LL939; _LL937: return _temp940;
_LL939:( void) _throw(( void*)& Cyc_yyfail_Char_tok); _LL935:;} static char
_temp945[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={
Cyc_Core_Failure,{ _temp945, _temp945, _temp945 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
void* yy1){ void* _temp946= yy1; void* _temp952; _LL948: if(*(( void**) _temp946)
== Cyc_Pointer_Sort_tok){ _LL953: _temp952=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp946)->f1; goto _LL949;} else{ goto _LL950;} _LL950: goto _LL951; _LL949:
return _temp952; _LL951:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL947:;} static char _temp957[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp957, _temp957, _temp957 + 8u}};
struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){ void* _temp958= yy1; struct
Cyc_Absyn_Exp* _temp964; _LL960: if(*(( void**) _temp958) == Cyc_Exp_tok){
_LL965: _temp964=(( struct Cyc_Exp_tok_struct*) _temp958)->f1; goto _LL961;}
else{ goto _LL962;} _LL962: goto _LL963; _LL961: return _temp964; _LL963:( void)
_throw(( void*)& Cyc_yyfail_Exp_tok); _LL959:;} static char _temp969[ 12u]="ExpList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{
_temp969, _temp969, _temp969 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok(
void* yy1){ void* _temp970= yy1; struct Cyc_List_List* _temp976; _LL972: if(*((
void**) _temp970) == Cyc_ExpList_tok){ _LL977: _temp976=(( struct Cyc_ExpList_tok_struct*)
_temp970)->f1; goto _LL973;} else{ goto _LL974;} _LL974: goto _LL975; _LL973:
return _temp976; _LL975:( void) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL971:;}
static char _temp981[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{ _temp981, _temp981, _temp981
+ 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok( void* yy1){ void*
_temp982= yy1; struct Cyc_List_List* _temp988; _LL984: if(*(( void**) _temp982)
== Cyc_InitializerList_tok){ _LL989: _temp988=(( struct Cyc_InitializerList_tok_struct*)
_temp982)->f1; goto _LL985;} else{ goto _LL986;} _LL986: goto _LL987; _LL985:
return _temp988; _LL987:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL983:;} static char _temp993[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp993, _temp993, _temp993 + 11u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* _temp994= yy1; void* _temp1000;
_LL996: if(*(( void**) _temp994) == Cyc_Primop_tok){ _LL1001: _temp1000=( void*)((
struct Cyc_Primop_tok_struct*) _temp994)->f1; goto _LL997;} else{ goto _LL998;}
_LL998: goto _LL999; _LL997: return _temp1000; _LL999:( void) _throw(( void*)&
Cyc_yyfail_Primop_tok); _LL995:;} static char _temp1005[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{
_temp1005, _temp1005, _temp1005 + 14u}}; struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok(
void* yy1){ void* _temp1006= yy1; struct Cyc_Core_Opt* _temp1012; _LL1008: if(*((
void**) _temp1006) == Cyc_Primopopt_tok){ _LL1013: _temp1012=(( struct Cyc_Primopopt_tok_struct*)
_temp1006)->f1; goto _LL1009;} else{ goto _LL1010;} _LL1010: goto _LL1011;
_LL1009: return _temp1012; _LL1011:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL1007:;} static char _temp1017[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{ _temp1017, _temp1017, _temp1017 + 11u}};
struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){ void* _temp1018= yy1; struct
_tuple1* _temp1024; _LL1020: if(*(( void**) _temp1018) == Cyc_QualId_tok){
_LL1025: _temp1024=(( struct Cyc_QualId_tok_struct*) _temp1018)->f1; goto
_LL1021;} else{ goto _LL1022;} _LL1022: goto _LL1023; _LL1021: return _temp1024;
_LL1023:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1019:;} static char
_temp1029[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{ _temp1029, _temp1029, _temp1029 + 9u}}; struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1030= yy1; struct Cyc_Absyn_Stmt*
_temp1036; _LL1032: if(*(( void**) _temp1030) == Cyc_Stmt_tok){ _LL1037:
_temp1036=(( struct Cyc_Stmt_tok_struct*) _temp1030)->f1; goto _LL1033;} else{
goto _LL1034;} _LL1034: goto _LL1035; _LL1033: return _temp1036; _LL1035:( void)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1031:;} static char _temp1041[ 14u]="BlockItem_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure,{
_temp1041, _temp1041, _temp1041 + 14u}}; void* Cyc_yyget_BlockItem_tok( void*
yy1){ void* _temp1042= yy1; void* _temp1048; _LL1044: if(*(( void**) _temp1042)
== Cyc_BlockItem_tok){ _LL1049: _temp1048=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp1042)->f1; goto _LL1045;} else{ goto _LL1046;} _LL1046: goto _LL1047;
_LL1045: return _temp1048; _LL1047:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL1043:;} static char _temp1053[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp1053, _temp1053,
_temp1053 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ void* _temp1054= yy1; struct Cyc_List_List* _temp1060; _LL1056: if(*((
void**) _temp1054) == Cyc_SwitchClauseList_tok){ _LL1061: _temp1060=(( struct
Cyc_SwitchClauseList_tok_struct*) _temp1054)->f1; goto _LL1057;} else{ goto
_LL1058;} _LL1058: goto _LL1059; _LL1057: return _temp1060; _LL1059:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1055:;} static char
_temp1065[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={
Cyc_Core_Failure,{ _temp1065, _temp1065, _temp1065 + 12u}}; struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1066= yy1; struct Cyc_Absyn_Pat*
_temp1072; _LL1068: if(*(( void**) _temp1066) == Cyc_Pattern_tok){ _LL1073:
_temp1072=(( struct Cyc_Pattern_tok_struct*) _temp1066)->f1; goto _LL1069;}
else{ goto _LL1070;} _LL1070: goto _LL1071; _LL1069: return _temp1072; _LL1071:(
void) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL1067:;} static char _temp1077[
16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp1077, _temp1077, _temp1077 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp1078= yy1; struct Cyc_List_List*
_temp1084; _LL1080: if(*(( void**) _temp1078) == Cyc_PatternList_tok){ _LL1085:
_temp1084=(( struct Cyc_PatternList_tok_struct*) _temp1078)->f1; goto _LL1081;}
else{ goto _LL1082;} _LL1082: goto _LL1083; _LL1081: return _temp1084; _LL1083:(
void) _throw(( void*)& Cyc_yyfail_PatternList_tok); _LL1079:;} static char
_temp1089[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp1089, _temp1089, _temp1089 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp1090= yy1; struct _tuple14* _temp1096; _LL1092: if(*((
void**) _temp1090) == Cyc_FieldPattern_tok){ _LL1097: _temp1096=(( struct Cyc_FieldPattern_tok_struct*)
_temp1090)->f1; goto _LL1093;} else{ goto _LL1094;} _LL1094: goto _LL1095;
_LL1093: return _temp1096; _LL1095:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1091:;} static char _temp1101[ 21u]="FieldPatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{ _temp1101, _temp1101,
_temp1101 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok( void*
yy1){ void* _temp1102= yy1; struct Cyc_List_List* _temp1108; _LL1104: if(*((
void**) _temp1102) == Cyc_FieldPatternList_tok){ _LL1109: _temp1108=(( struct
Cyc_FieldPatternList_tok_struct*) _temp1102)->f1; goto _LL1105;} else{ goto
_LL1106;} _LL1106: goto _LL1107; _LL1105: return _temp1108; _LL1107:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1103:;} static char
_temp1113[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,{ _temp1113, _temp1113, _temp1113 + 11u}}; struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1114= yy1; struct Cyc_Absyn_Fndecl*
_temp1120; _LL1116: if(*(( void**) _temp1114) == Cyc_FnDecl_tok){ _LL1121:
_temp1120=(( struct Cyc_FnDecl_tok_struct*) _temp1114)->f1; goto _LL1117;} else{
goto _LL1118;} _LL1118: goto _LL1119; _LL1117: return _temp1120; _LL1119:( void)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL1115:;} static char _temp1125[ 13u]="DeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp1125, _temp1125, _temp1125 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ void* _temp1126= yy1; struct Cyc_List_List* _temp1132; _LL1128: if(*((
void**) _temp1126) == Cyc_DeclList_tok){ _LL1133: _temp1132=(( struct Cyc_DeclList_tok_struct*)
_temp1126)->f1; goto _LL1129;} else{ goto _LL1130;} _LL1130: goto _LL1131;
_LL1129: return _temp1132; _LL1131:( void) _throw(( void*)& Cyc_yyfail_DeclList_tok);
_LL1127:;} static char _temp1137[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp1137, _temp1137, _temp1137 +
13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){
void* _temp1138= yy1; struct Cyc_Parse_Declaration_spec* _temp1144; _LL1140: if(*((
void**) _temp1138) == Cyc_DeclSpec_tok){ _LL1145: _temp1144=(( struct Cyc_DeclSpec_tok_struct*)
_temp1138)->f1; goto _LL1141;} else{ goto _LL1142;} _LL1142: goto _LL1143;
_LL1141: return _temp1144; _LL1143:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1139:;} static char _temp1149[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp1149, _temp1149, _temp1149 +
13u}}; struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp1150= yy1;
struct _tuple15* _temp1156; _LL1152: if(*(( void**) _temp1150) == Cyc_InitDecl_tok){
_LL1157: _temp1156=(( struct Cyc_InitDecl_tok_struct*) _temp1150)->f1; goto
_LL1153;} else{ goto _LL1154;} _LL1154: goto _LL1155; _LL1153: return _temp1156;
_LL1155:( void) _throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL1151:;} static
char _temp1161[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{ _temp1161, _temp1161, _temp1161
+ 17u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( void* yy1){ void*
_temp1162= yy1; struct Cyc_List_List* _temp1168; _LL1164: if(*(( void**)
_temp1162) == Cyc_InitDeclList_tok){ _LL1169: _temp1168=(( struct Cyc_InitDeclList_tok_struct*)
_temp1162)->f1; goto _LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167;
_LL1165: return _temp1168; _LL1167:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1163:;} static char _temp1173[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp1173, _temp1173, _temp1173
+ 17u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* _temp1174= yy1;
void* _temp1180; _LL1176: if(*(( void**) _temp1174) == Cyc_StorageClass_tok){
_LL1181: _temp1180=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp1174)->f1;
goto _LL1177;} else{ goto _LL1178;} _LL1178: goto _LL1179; _LL1177: return
_temp1180; _LL1179:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1175:;} static char _temp1185[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{ _temp1185, _temp1185,
_temp1185 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void* yy1){ void*
_temp1186= yy1; void* _temp1192; _LL1188: if(*(( void**) _temp1186) == Cyc_TypeSpecifier_tok){
_LL1193: _temp1192=( void*)(( struct Cyc_TypeSpecifier_tok_struct*) _temp1186)->f1;
goto _LL1189;} else{ goto _LL1190;} _LL1190: goto _LL1191; _LL1189: return
_temp1192; _LL1191:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1187:;} static char _temp1197[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{ _temp1197, _temp1197,
_temp1197 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void* yy1){ void*
_temp1198= yy1; void* _temp1204; _LL1200: if(*(( void**) _temp1198) == Cyc_StructOrUnion_tok){
_LL1205: _temp1204=( void*)(( struct Cyc_StructOrUnion_tok_struct*) _temp1198)->f1;
goto _LL1201;} else{ goto _LL1202;} _LL1202: goto _LL1203; _LL1201: return
_temp1204; _LL1203:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1199:;} static char _temp1209[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp1209, _temp1209, _temp1209 +
13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ void*
_temp1210= yy1; struct Cyc_Absyn_Tqual _temp1216; _LL1212: if(*(( void**)
_temp1210) == Cyc_TypeQual_tok){ _LL1217: _temp1216=(( struct Cyc_TypeQual_tok_struct*)
_temp1210)->f1; goto _LL1213;} else{ goto _LL1214;} _LL1214: goto _LL1215;
_LL1213: return _temp1216; _LL1215:( void) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1211:;} static char _temp1221[ 24u]="StructFieldDeclList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp1221, _temp1221, _temp1221 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp1222= yy1; struct Cyc_List_List* _temp1228; _LL1224: if(*((
void**) _temp1222) == Cyc_StructFieldDeclList_tok){ _LL1229: _temp1228=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1222)->f1; goto _LL1225;} else{ goto
_LL1226;} _LL1226: goto _LL1227; _LL1225: return _temp1228; _LL1227:( void)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1223:;} static char
_temp1233[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1233, _temp1233,
_temp1233 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1234= yy1; struct Cyc_List_List* _temp1240; _LL1236: if(*((
void**) _temp1234) == Cyc_StructFieldDeclListList_tok){ _LL1241: _temp1240=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1234)->f1; goto _LL1237;}
else{ goto _LL1238;} _LL1238: goto _LL1239; _LL1237: return _temp1240; _LL1239:(
void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1235:;}
static char _temp1245[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1245, _temp1245,
_temp1245 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1246= yy1; struct Cyc_List_List* _temp1252; _LL1248: if(*((
void**) _temp1246) == Cyc_TypeModifierList_tok){ _LL1253: _temp1252=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1246)->f1; goto _LL1249;} else{ goto
_LL1250;} _LL1250: goto _LL1251; _LL1249: return _temp1252; _LL1251:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1247:;} static char
_temp1257[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgn_tok={
Cyc_Core_Failure,{ _temp1257, _temp1257, _temp1257 + 8u}}; void* Cyc_yyget_Rgn_tok(
void* yy1){ void* _temp1258= yy1; void* _temp1264; _LL1260: if(*(( void**)
_temp1258) == Cyc_Rgn_tok){ _LL1265: _temp1264=( void*)(( struct Cyc_Rgn_tok_struct*)
_temp1258)->f1; goto _LL1261;} else{ goto _LL1262;} _LL1262: goto _LL1263;
_LL1261: return _temp1264; _LL1263:( void) _throw(( void*)& Cyc_yyfail_Rgn_tok);
_LL1259:;} static char _temp1269[ 15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp1269, _temp1269, _temp1269 +
15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( void* yy1){ void*
_temp1270= yy1; struct Cyc_Parse_Declarator* _temp1276; _LL1272: if(*(( void**)
_temp1270) == Cyc_Declarator_tok){ _LL1277: _temp1276=(( struct Cyc_Declarator_tok_struct*)
_temp1270)->f1; goto _LL1273;} else{ goto _LL1274;} _LL1274: goto _LL1275;
_LL1273: return _temp1276; _LL1275:( void) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1271:;} static char _temp1281[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1281, _temp1281,
_temp1281 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1282= yy1; struct _tuple13* _temp1288; _LL1284: if(*(( void**)
_temp1282) == Cyc_DeclaratorExpopt_tok){ _LL1289: _temp1288=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1282)->f1; goto _LL1285;} else{ goto _LL1286;} _LL1286: goto _LL1287;
_LL1285: return _temp1288; _LL1287:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1283:;} static char _temp1293[ 25u]="DeclaratorExpoptList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1293, _temp1293, _temp1293 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1294= yy1; struct Cyc_List_List* _temp1300; _LL1296: if(*((
void**) _temp1294) == Cyc_DeclaratorExpoptList_tok){ _LL1301: _temp1300=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1294)->f1; goto _LL1297;}
else{ goto _LL1298;} _LL1298: goto _LL1299; _LL1297: return _temp1300; _LL1299:(
void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1295:;} static
char _temp1305[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1305, _temp1305,
_temp1305 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1306= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1312;
_LL1308: if(*(( void**) _temp1306) == Cyc_AbstractDeclarator_tok){ _LL1313:
_temp1312=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1306)->f1; goto
_LL1309;} else{ goto _LL1310;} _LL1310: goto _LL1311; _LL1309: return _temp1312;
_LL1311:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1307:;}
static char _temp1317[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={
Cyc_Core_Failure,{ _temp1317, _temp1317, _temp1317 + 9u}}; int Cyc_yyget_Bool_tok(
void* yy1){ void* _temp1318= yy1; int _temp1324; _LL1320: if(*(( void**)
_temp1318) == Cyc_Bool_tok){ _LL1325: _temp1324=(( struct Cyc_Bool_tok_struct*)
_temp1318)->f1; goto _LL1321;} else{ goto _LL1322;} _LL1322: goto _LL1323;
_LL1321: return _temp1324; _LL1323:( void) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1319:;} static char _temp1329[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1329, _temp1329, _temp1329 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1330= yy1; void* _temp1336;
_LL1332: if(*(( void**) _temp1330) == Cyc_Scope_tok){ _LL1337: _temp1336=( void*)((
struct Cyc_Scope_tok_struct*) _temp1330)->f1; goto _LL1333;} else{ goto _LL1334;}
_LL1334: goto _LL1335; _LL1333: return _temp1336; _LL1335:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1331:;} static char _temp1341[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1341, _temp1341, _temp1341 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1342= yy1; struct Cyc_Absyn_Tunionfield* _temp1348;
_LL1344: if(*(( void**) _temp1342) == Cyc_TunionField_tok){ _LL1349: _temp1348=((
struct Cyc_TunionField_tok_struct*) _temp1342)->f1; goto _LL1345;} else{ goto
_LL1346;} _LL1346: goto _LL1347; _LL1345: return _temp1348; _LL1347:( void)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1343:;} static char _temp1353[
20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1353, _temp1353, _temp1353 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1354= yy1; struct Cyc_List_List*
_temp1360; _LL1356: if(*(( void**) _temp1354) == Cyc_TunionFieldList_tok){
_LL1361: _temp1360=(( struct Cyc_TunionFieldList_tok_struct*) _temp1354)->f1;
goto _LL1357;} else{ goto _LL1358;} _LL1358: goto _LL1359; _LL1357: return
_temp1360; _LL1359:( void) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1355:;} static char _temp1365[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{ _temp1365, _temp1365, _temp1365
+ 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok( void* yy1){ void*
_temp1366= yy1; struct _tuple18* _temp1372; _LL1368: if(*(( void**) _temp1366)
== Cyc_QualSpecList_tok){ _LL1373: _temp1372=(( struct Cyc_QualSpecList_tok_struct*)
_temp1366)->f1; goto _LL1369;} else{ goto _LL1370;} _LL1370: goto _LL1371;
_LL1369: return _temp1372; _LL1371:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1367:;} static char _temp1377[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1377, _temp1377, _temp1377 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1378= yy1;
struct Cyc_List_List* _temp1384; _LL1380: if(*(( void**) _temp1378) == Cyc_IdList_tok){
_LL1385: _temp1384=(( struct Cyc_IdList_tok_struct*) _temp1378)->f1; goto
_LL1381;} else{ goto _LL1382;} _LL1382: goto _LL1383; _LL1381: return _temp1384;
_LL1383:( void) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1379:;} static char
_temp1389[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure,{ _temp1389, _temp1389, _temp1389 + 14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ void* _temp1390= yy1; struct _tuple2* _temp1396; _LL1392: if(*((
void**) _temp1390) == Cyc_ParamDecl_tok){ _LL1397: _temp1396=(( struct Cyc_ParamDecl_tok_struct*)
_temp1390)->f1; goto _LL1393;} else{ goto _LL1394;} _LL1394: goto _LL1395;
_LL1393: return _temp1396; _LL1395:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL1391:;} static char _temp1401[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1401, _temp1401,
_temp1401 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1402= yy1; struct Cyc_List_List* _temp1408; _LL1404: if(*(( void**)
_temp1402) == Cyc_ParamDeclList_tok){ _LL1409: _temp1408=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1402)->f1; goto _LL1405;} else{ goto _LL1406;} _LL1406: goto _LL1407;
_LL1405: return _temp1408; _LL1407:( void) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1403:;} static char _temp1413[ 22u]="ParamDeclListBool_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1413, _temp1413, _temp1413 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1414= yy1; struct _tuple17* _temp1420; _LL1416: if(*((
void**) _temp1414) == Cyc_ParamDeclListBool_tok){ _LL1421: _temp1420=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1414)->f1; goto _LL1417;} else{ goto
_LL1418;} _LL1418: goto _LL1419; _LL1417: return _temp1420; _LL1419:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1415:;} static char
_temp1425[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1425, _temp1425, _temp1425 + 13u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1426= yy1; struct Cyc_List_List*
_temp1432; _LL1428: if(*(( void**) _temp1426) == Cyc_TypeList_tok){ _LL1433:
_temp1432=(( struct Cyc_TypeList_tok_struct*) _temp1426)->f1; goto _LL1429;}
else{ goto _LL1430;} _LL1430: goto _LL1431; _LL1429: return _temp1432; _LL1431:(
void) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1427:;} static char
_temp1437[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={
Cyc_Core_Failure,{ _temp1437, _temp1437, _temp1437 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1438= yy1; struct Cyc_List_List*
_temp1444; _LL1440: if(*(( void**) _temp1438) == Cyc_DesignatorList_tok){
_LL1445: _temp1444=(( struct Cyc_DesignatorList_tok_struct*) _temp1438)->f1;
goto _LL1441;} else{ goto _LL1442;} _LL1442: goto _LL1443; _LL1441: return
_temp1444; _LL1443:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1439:;} static char _temp1449[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1449, _temp1449, _temp1449 +
15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void* _temp1450= yy1; void*
_temp1456; _LL1452: if(*(( void**) _temp1450) == Cyc_Designator_tok){ _LL1457:
_temp1456=( void*)(( struct Cyc_Designator_tok_struct*) _temp1450)->f1; goto
_LL1453;} else{ goto _LL1454;} _LL1454: goto _LL1455; _LL1453: return _temp1456;
_LL1455:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1451:;} static
char _temp1461[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,{ _temp1461, _temp1461, _temp1461 + 9u}}; void* Cyc_yyget_Kind_tok(
void* yy1){ void* _temp1462= yy1; void* _temp1468; _LL1464: if(*(( void**)
_temp1462) == Cyc_Kind_tok){ _LL1469: _temp1468=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1462)->f1; goto _LL1465;} else{ goto _LL1466;} _LL1466: goto _LL1467;
_LL1465: return _temp1468; _LL1467:( void) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1463:;} static char _temp1473[ 9u]="Type_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{ _temp1473, _temp1473, _temp1473 + 9u}};
void* Cyc_yyget_Type_tok( void* yy1){ void* _temp1474= yy1; void* _temp1480;
_LL1476: if(*(( void**) _temp1474) == Cyc_Type_tok){ _LL1481: _temp1480=( void*)((
struct Cyc_Type_tok_struct*) _temp1474)->f1; goto _LL1477;} else{ goto _LL1478;}
_LL1478: goto _LL1479; _LL1477: return _temp1480; _LL1479:( void) _throw(( void*)&
Cyc_yyfail_Type_tok); _LL1475:;} static char _temp1485[ 18u]="AttributeList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1485, _temp1485, _temp1485 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1486= yy1; struct Cyc_List_List* _temp1492; _LL1488: if(*((
void**) _temp1486) == Cyc_AttributeList_tok){ _LL1493: _temp1492=(( struct Cyc_AttributeList_tok_struct*)
_temp1486)->f1; goto _LL1489;} else{ goto _LL1490;} _LL1490: goto _LL1491;
_LL1489: return _temp1492; _LL1491:( void) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1487:;} static char _temp1497[ 14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1497, _temp1497, _temp1497 +
14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1498= yy1; void*
_temp1504; _LL1500: if(*(( void**) _temp1498) == Cyc_Attribute_tok){ _LL1505:
_temp1504=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1498)->f1; goto
_LL1501;} else{ goto _LL1502;} _LL1502: goto _LL1503; _LL1501: return _temp1504;
_LL1503:( void) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1499:;} static
char _temp1509[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={
Cyc_Core_Failure,{ _temp1509, _temp1509, _temp1509 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1510= yy1; struct Cyc_Absyn_Enumfield*
_temp1516; _LL1512: if(*(( void**) _temp1510) == Cyc_Enumfield_tok){ _LL1517:
_temp1516=(( struct Cyc_Enumfield_tok_struct*) _temp1510)->f1; goto _LL1513;}
else{ goto _LL1514;} _LL1514: goto _LL1515; _LL1513: return _temp1516; _LL1515:(
void) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1511:;} static char
_temp1521[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={
Cyc_Core_Failure,{ _temp1521, _temp1521, _temp1521 + 18u}}; struct Cyc_List_List*
Cyc_yyget_EnumfieldList_tok( void* yy1){ void* _temp1522= yy1; struct Cyc_List_List*
_temp1528; _LL1524: if(*(( void**) _temp1522) == Cyc_EnumfieldList_tok){ _LL1529:
_temp1528=(( struct Cyc_EnumfieldList_tok_struct*) _temp1522)->f1; goto _LL1525;}
else{ goto _LL1526;} _LL1526: goto _LL1527; _LL1525: return _temp1528; _LL1527:(
void) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1523:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1530; _temp1530.timestamp=
0; _temp1530.first_line= 0; _temp1530.first_column= 0; _temp1530.last_line= 0;
_temp1530.last_column= 0; _temp1530.text=({ char* _temp1531=""; struct
_tagged_string _temp1532; _temp1532.curr= _temp1531; _temp1532.base= _temp1531;
_temp1532.last_plus_one= _temp1531 + 1; _temp1532;}); _temp1530;});} static char
_temp1535[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{ _temp1535,
_temp1535, _temp1535 + 1u}}; static short Cyc_yytranslate[ 352u]={ (short)0,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)123, (short)2, (short)2, (short)105, (short)121, (short)117, (short)2,
(short)101, (short)102, (short)112, (short)115, (short)103, (short)116, (short)109,
(short)120, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)108, (short)97, (short)106, (short)100,
(short)107, (short)114, (short)113, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)110, (short)2,
(short)111, (short)119, (short)104, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)98,
(short)118, (short)99, (short)122, (short)2, (short)2, (short)2, (short)2,
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
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95,
(short)96}; static char _temp1538[ 2u]="$"; static char _temp1541[ 6u]="error";
static char _temp1544[ 12u]="$undefined."; static char _temp1547[ 5u]="AUTO";
static char _temp1550[ 9u]="REGISTER"; static char _temp1553[ 7u]="STATIC";
static char _temp1556[ 7u]="EXTERN"; static char _temp1559[ 8u]="TYPEDEF";
static char _temp1562[ 5u]="VOID"; static char _temp1565[ 5u]="CHAR"; static
char _temp1568[ 6u]="SHORT"; static char _temp1571[ 4u]="INT"; static char
_temp1574[ 5u]="LONG"; static char _temp1577[ 6u]="FLOAT"; static char _temp1580[
7u]="DOUBLE"; static char _temp1583[ 7u]="SIGNED"; static char _temp1586[ 9u]="UNSIGNED";
static char _temp1589[ 6u]="CONST"; static char _temp1592[ 9u]="VOLATILE";
static char _temp1595[ 9u]="RESTRICT"; static char _temp1598[ 7u]="STRUCT";
static char _temp1601[ 6u]="UNION"; static char _temp1604[ 5u]="CASE"; static
char _temp1607[ 8u]="DEFAULT"; static char _temp1610[ 7u]="INLINE"; static char
_temp1613[ 3u]="IF"; static char _temp1616[ 5u]="ELSE"; static char _temp1619[ 7u]="SWITCH";
static char _temp1622[ 6u]="WHILE"; static char _temp1625[ 3u]="DO"; static char
_temp1628[ 4u]="FOR"; static char _temp1631[ 5u]="GOTO"; static char _temp1634[
9u]="CONTINUE"; static char _temp1637[ 6u]="BREAK"; static char _temp1640[ 7u]="RETURN";
static char _temp1643[ 7u]="SIZEOF"; static char _temp1646[ 5u]="ENUM"; static
char _temp1649[ 8u]="NULL_kw"; static char _temp1652[ 4u]="LET"; static char
_temp1655[ 6u]="THROW"; static char _temp1658[ 4u]="TRY"; static char _temp1661[
6u]="CATCH"; static char _temp1664[ 4u]="NEW"; static char _temp1667[ 9u]="ABSTRACT";
static char _temp1670[ 9u]="FALLTHRU"; static char _temp1673[ 6u]="USING";
static char _temp1676[ 10u]="NAMESPACE"; static char _temp1679[ 7u]="TUNION";
static char _temp1682[ 8u]="XTUNION"; static char _temp1685[ 5u]="FILL"; static
char _temp1688[ 8u]="CODEGEN"; static char _temp1691[ 4u]="CUT"; static char
_temp1694[ 7u]="SPLICE"; static char _temp1697[ 7u]="PRINTF"; static char
_temp1700[ 8u]="FPRINTF"; static char _temp1703[ 8u]="XPRINTF"; static char
_temp1706[ 6u]="SCANF"; static char _temp1709[ 7u]="FSCANF"; static char
_temp1712[ 7u]="SSCANF"; static char _temp1715[ 7u]="MALLOC"; static char
_temp1718[ 9u]="REGION_T"; static char _temp1721[ 7u]="REGION"; static char
_temp1724[ 5u]="RNEW"; static char _temp1727[ 8u]="RMALLOC"; static char
_temp1730[ 7u]="PTR_OP"; static char _temp1733[ 7u]="INC_OP"; static char
_temp1736[ 7u]="DEC_OP"; static char _temp1739[ 8u]="LEFT_OP"; static char
_temp1742[ 9u]="RIGHT_OP"; static char _temp1745[ 6u]="LE_OP"; static char
_temp1748[ 6u]="GE_OP"; static char _temp1751[ 6u]="EQ_OP"; static char
_temp1754[ 6u]="NE_OP"; static char _temp1757[ 7u]="AND_OP"; static char
_temp1760[ 6u]="OR_OP"; static char _temp1763[ 11u]="MUL_ASSIGN"; static char
_temp1766[ 11u]="DIV_ASSIGN"; static char _temp1769[ 11u]="MOD_ASSIGN"; static
char _temp1772[ 11u]="ADD_ASSIGN"; static char _temp1775[ 11u]="SUB_ASSIGN";
static char _temp1778[ 12u]="LEFT_ASSIGN"; static char _temp1781[ 13u]="RIGHT_ASSIGN";
static char _temp1784[ 11u]="AND_ASSIGN"; static char _temp1787[ 11u]="XOR_ASSIGN";
static char _temp1790[ 10u]="OR_ASSIGN"; static char _temp1793[ 9u]="ELLIPSIS";
static char _temp1796[ 11u]="LEFT_RIGHT"; static char _temp1799[ 12u]="COLON_COLON";
static char _temp1802[ 11u]="IDENTIFIER"; static char _temp1805[ 17u]="INTEGER_CONSTANT";
static char _temp1808[ 7u]="STRING"; static char _temp1811[ 19u]="CHARACTER_CONSTANT";
static char _temp1814[ 18u]="FLOATING_CONSTANT"; static char _temp1817[ 9u]="TYPE_VAR";
static char _temp1820[ 16u]="QUAL_IDENTIFIER"; static char _temp1823[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1826[ 10u]="ATTRIBUTE"; static char _temp1829[ 4u]="';'";
static char _temp1832[ 4u]="'{'"; static char _temp1835[ 4u]="'}'"; static char
_temp1838[ 4u]="'='"; static char _temp1841[ 4u]="'('"; static char _temp1844[ 4u]="')'";
static char _temp1847[ 4u]="','"; static char _temp1850[ 4u]="'_'"; static char
_temp1853[ 4u]="'$'"; static char _temp1856[ 4u]="'<'"; static char _temp1859[ 4u]="'>'";
static char _temp1862[ 4u]="':'"; static char _temp1865[ 4u]="'.'"; static char
_temp1868[ 4u]="'['"; static char _temp1871[ 4u]="']'"; static char _temp1874[ 4u]="'*'";
static char _temp1877[ 4u]="'@'"; static char _temp1880[ 4u]="'?'"; static char
_temp1883[ 4u]="'+'"; static char _temp1886[ 4u]="'-'"; static char _temp1889[ 4u]="'&'";
static char _temp1892[ 4u]="'|'"; static char _temp1895[ 4u]="'^'"; static char
_temp1898[ 4u]="'/'"; static char _temp1901[ 4u]="'%'"; static char _temp1904[ 4u]="'~'";
static char _temp1907[ 4u]="'!'"; static char _temp1910[ 5u]="prog"; static char
_temp1913[ 17u]="translation_unit"; static char _temp1916[ 21u]="external_declaration";
static char _temp1919[ 20u]="function_definition"; static char _temp1922[ 21u]="function_definition2";
static char _temp1925[ 13u]="using_action"; static char _temp1928[ 15u]="unusing_action";
static char _temp1931[ 17u]="namespace_action"; static char _temp1934[ 19u]="unnamespace_action";
static char _temp1937[ 12u]="declaration"; static char _temp1940[ 17u]="declaration_list";
static char _temp1943[ 23u]="declaration_specifiers"; static char _temp1946[ 24u]="storage_class_specifier";
static char _temp1949[ 15u]="attributes_opt"; static char _temp1952[ 11u]="attributes";
static char _temp1955[ 15u]="attribute_list"; static char _temp1958[ 10u]="attribute";
static char _temp1961[ 15u]="type_specifier"; static char _temp1964[ 5u]="kind";
static char _temp1967[ 15u]="type_qualifier"; static char _temp1970[ 15u]="enum_specifier";
static char _temp1973[ 11u]="enum_field"; static char _temp1976[ 22u]="enum_declaration_list";
static char _temp1979[ 26u]="struct_or_union_specifier"; static char _temp1982[
16u]="type_params_opt"; static char _temp1985[ 16u]="struct_or_union"; static
char _temp1988[ 24u]="struct_declaration_list"; static char _temp1991[ 25u]="struct_declaration_list0";
static char _temp1994[ 21u]="init_declarator_list"; static char _temp1997[ 22u]="init_declarator_list0";
static char _temp2000[ 16u]="init_declarator"; static char _temp2003[ 19u]="struct_declaration";
static char _temp2006[ 25u]="specifier_qualifier_list"; static char _temp2009[
23u]="struct_declarator_list"; static char _temp2012[ 24u]="struct_declarator_list0";
static char _temp2015[ 18u]="struct_declarator"; static char _temp2018[ 17u]="tunion_specifier";
static char _temp2021[ 18u]="tunion_or_xtunion"; static char _temp2024[ 17u]="tunionfield_list";
static char _temp2027[ 18u]="tunionfield_scope"; static char _temp2030[ 12u]="tunionfield";
static char _temp2033[ 11u]="declarator"; static char _temp2036[ 18u]="direct_declarator";
static char _temp2039[ 8u]="pointer"; static char _temp2042[ 13u]="pointer_char";
static char _temp2045[ 8u]="rgn_opt"; static char _temp2048[ 4u]="rgn"; static
char _temp2051[ 20u]="type_qualifier_list"; static char _temp2054[ 20u]="parameter_type_list";
static char _temp2057[ 11u]="effect_set"; static char _temp2060[ 14u]="atomic_effect";
static char _temp2063[ 11u]="region_set"; static char _temp2066[ 15u]="parameter_list";
static char _temp2069[ 22u]="parameter_declaration"; static char _temp2072[ 16u]="identifier_list";
static char _temp2075[ 17u]="identifier_list0"; static char _temp2078[ 12u]="initializer";
static char _temp2081[ 18u]="array_initializer"; static char _temp2084[ 17u]="initializer_list";
static char _temp2087[ 12u]="designation"; static char _temp2090[ 16u]="designator_list";
static char _temp2093[ 11u]="designator"; static char _temp2096[ 10u]="type_name";
static char _temp2099[ 14u]="any_type_name"; static char _temp2102[ 15u]="type_name_list";
static char _temp2105[ 20u]="abstract_declarator"; static char _temp2108[ 27u]="direct_abstract_declarator";
static char _temp2111[ 10u]="statement"; static char _temp2114[ 18u]="labeled_statement";
static char _temp2117[ 21u]="expression_statement"; static char _temp2120[ 19u]="compound_statement";
static char _temp2123[ 16u]="block_item_list"; static char _temp2126[ 11u]="block_item";
static char _temp2129[ 20u]="selection_statement"; static char _temp2132[ 15u]="switch_clauses";
static char _temp2135[ 20u]="iteration_statement"; static char _temp2138[ 15u]="jump_statement";
static char _temp2141[ 8u]="pattern"; static char _temp2144[ 19u]="tuple_pattern_list";
static char _temp2147[ 20u]="tuple_pattern_list0"; static char _temp2150[ 14u]="field_pattern";
static char _temp2153[ 19u]="field_pattern_list"; static char _temp2156[ 20u]="field_pattern_list0";
static char _temp2159[ 11u]="expression"; static char _temp2162[ 22u]="assignment_expression";
static char _temp2165[ 20u]="assignment_operator"; static char _temp2168[ 23u]="conditional_expression";
static char _temp2171[ 20u]="constant_expression"; static char _temp2174[ 22u]="logical_or_expression";
static char _temp2177[ 23u]="logical_and_expression"; static char _temp2180[ 24u]="inclusive_or_expression";
static char _temp2183[ 24u]="exclusive_or_expression"; static char _temp2186[ 15u]="and_expression";
static char _temp2189[ 20u]="equality_expression"; static char _temp2192[ 22u]="relational_expression";
static char _temp2195[ 17u]="shift_expression"; static char _temp2198[ 20u]="additive_expression";
static char _temp2201[ 26u]="multiplicative_expression"; static char _temp2204[
16u]="cast_expression"; static char _temp2207[ 17u]="unary_expression"; static
char _temp2210[ 14u]="format_primop"; static char _temp2213[ 15u]="unary_operator";
static char _temp2216[ 19u]="postfix_expression"; static char _temp2219[ 19u]="primary_expression";
static char _temp2222[ 25u]="argument_expression_list"; static char _temp2225[
26u]="argument_expression_list0"; static char _temp2228[ 9u]="constant"; static
char _temp2231[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
232u]={{ _temp1538, _temp1538, _temp1538 + 2u},{ _temp1541, _temp1541, _temp1541
+ 6u},{ _temp1544, _temp1544, _temp1544 + 12u},{ _temp1547, _temp1547, _temp1547
+ 5u},{ _temp1550, _temp1550, _temp1550 + 9u},{ _temp1553, _temp1553, _temp1553
+ 7u},{ _temp1556, _temp1556, _temp1556 + 7u},{ _temp1559, _temp1559, _temp1559
+ 8u},{ _temp1562, _temp1562, _temp1562 + 5u},{ _temp1565, _temp1565, _temp1565
+ 5u},{ _temp1568, _temp1568, _temp1568 + 6u},{ _temp1571, _temp1571, _temp1571
+ 4u},{ _temp1574, _temp1574, _temp1574 + 5u},{ _temp1577, _temp1577, _temp1577
+ 6u},{ _temp1580, _temp1580, _temp1580 + 7u},{ _temp1583, _temp1583, _temp1583
+ 7u},{ _temp1586, _temp1586, _temp1586 + 9u},{ _temp1589, _temp1589, _temp1589
+ 6u},{ _temp1592, _temp1592, _temp1592 + 9u},{ _temp1595, _temp1595, _temp1595
+ 9u},{ _temp1598, _temp1598, _temp1598 + 7u},{ _temp1601, _temp1601, _temp1601
+ 6u},{ _temp1604, _temp1604, _temp1604 + 5u},{ _temp1607, _temp1607, _temp1607
+ 8u},{ _temp1610, _temp1610, _temp1610 + 7u},{ _temp1613, _temp1613, _temp1613
+ 3u},{ _temp1616, _temp1616, _temp1616 + 5u},{ _temp1619, _temp1619, _temp1619
+ 7u},{ _temp1622, _temp1622, _temp1622 + 6u},{ _temp1625, _temp1625, _temp1625
+ 3u},{ _temp1628, _temp1628, _temp1628 + 4u},{ _temp1631, _temp1631, _temp1631
+ 5u},{ _temp1634, _temp1634, _temp1634 + 9u},{ _temp1637, _temp1637, _temp1637
+ 6u},{ _temp1640, _temp1640, _temp1640 + 7u},{ _temp1643, _temp1643, _temp1643
+ 7u},{ _temp1646, _temp1646, _temp1646 + 5u},{ _temp1649, _temp1649, _temp1649
+ 8u},{ _temp1652, _temp1652, _temp1652 + 4u},{ _temp1655, _temp1655, _temp1655
+ 6u},{ _temp1658, _temp1658, _temp1658 + 4u},{ _temp1661, _temp1661, _temp1661
+ 6u},{ _temp1664, _temp1664, _temp1664 + 4u},{ _temp1667, _temp1667, _temp1667
+ 9u},{ _temp1670, _temp1670, _temp1670 + 9u},{ _temp1673, _temp1673, _temp1673
+ 6u},{ _temp1676, _temp1676, _temp1676 + 10u},{ _temp1679, _temp1679, _temp1679
+ 7u},{ _temp1682, _temp1682, _temp1682 + 8u},{ _temp1685, _temp1685, _temp1685
+ 5u},{ _temp1688, _temp1688, _temp1688 + 8u},{ _temp1691, _temp1691, _temp1691
+ 4u},{ _temp1694, _temp1694, _temp1694 + 7u},{ _temp1697, _temp1697, _temp1697
+ 7u},{ _temp1700, _temp1700, _temp1700 + 8u},{ _temp1703, _temp1703, _temp1703
+ 8u},{ _temp1706, _temp1706, _temp1706 + 6u},{ _temp1709, _temp1709, _temp1709
+ 7u},{ _temp1712, _temp1712, _temp1712 + 7u},{ _temp1715, _temp1715, _temp1715
+ 7u},{ _temp1718, _temp1718, _temp1718 + 9u},{ _temp1721, _temp1721, _temp1721
+ 7u},{ _temp1724, _temp1724, _temp1724 + 5u},{ _temp1727, _temp1727, _temp1727
+ 8u},{ _temp1730, _temp1730, _temp1730 + 7u},{ _temp1733, _temp1733, _temp1733
+ 7u},{ _temp1736, _temp1736, _temp1736 + 7u},{ _temp1739, _temp1739, _temp1739
+ 8u},{ _temp1742, _temp1742, _temp1742 + 9u},{ _temp1745, _temp1745, _temp1745
+ 6u},{ _temp1748, _temp1748, _temp1748 + 6u},{ _temp1751, _temp1751, _temp1751
+ 6u},{ _temp1754, _temp1754, _temp1754 + 6u},{ _temp1757, _temp1757, _temp1757
+ 7u},{ _temp1760, _temp1760, _temp1760 + 6u},{ _temp1763, _temp1763, _temp1763
+ 11u},{ _temp1766, _temp1766, _temp1766 + 11u},{ _temp1769, _temp1769,
_temp1769 + 11u},{ _temp1772, _temp1772, _temp1772 + 11u},{ _temp1775, _temp1775,
_temp1775 + 11u},{ _temp1778, _temp1778, _temp1778 + 12u},{ _temp1781, _temp1781,
_temp1781 + 13u},{ _temp1784, _temp1784, _temp1784 + 11u},{ _temp1787, _temp1787,
_temp1787 + 11u},{ _temp1790, _temp1790, _temp1790 + 10u},{ _temp1793, _temp1793,
_temp1793 + 9u},{ _temp1796, _temp1796, _temp1796 + 11u},{ _temp1799, _temp1799,
_temp1799 + 12u},{ _temp1802, _temp1802, _temp1802 + 11u},{ _temp1805, _temp1805,
_temp1805 + 17u},{ _temp1808, _temp1808, _temp1808 + 7u},{ _temp1811, _temp1811,
_temp1811 + 19u},{ _temp1814, _temp1814, _temp1814 + 18u},{ _temp1817, _temp1817,
_temp1817 + 9u},{ _temp1820, _temp1820, _temp1820 + 16u},{ _temp1823, _temp1823,
_temp1823 + 18u},{ _temp1826, _temp1826, _temp1826 + 10u},{ _temp1829, _temp1829,
_temp1829 + 4u},{ _temp1832, _temp1832, _temp1832 + 4u},{ _temp1835, _temp1835,
_temp1835 + 4u},{ _temp1838, _temp1838, _temp1838 + 4u},{ _temp1841, _temp1841,
_temp1841 + 4u},{ _temp1844, _temp1844, _temp1844 + 4u},{ _temp1847, _temp1847,
_temp1847 + 4u},{ _temp1850, _temp1850, _temp1850 + 4u},{ _temp1853, _temp1853,
_temp1853 + 4u},{ _temp1856, _temp1856, _temp1856 + 4u},{ _temp1859, _temp1859,
_temp1859 + 4u},{ _temp1862, _temp1862, _temp1862 + 4u},{ _temp1865, _temp1865,
_temp1865 + 4u},{ _temp1868, _temp1868, _temp1868 + 4u},{ _temp1871, _temp1871,
_temp1871 + 4u},{ _temp1874, _temp1874, _temp1874 + 4u},{ _temp1877, _temp1877,
_temp1877 + 4u},{ _temp1880, _temp1880, _temp1880 + 4u},{ _temp1883, _temp1883,
_temp1883 + 4u},{ _temp1886, _temp1886, _temp1886 + 4u},{ _temp1889, _temp1889,
_temp1889 + 4u},{ _temp1892, _temp1892, _temp1892 + 4u},{ _temp1895, _temp1895,
_temp1895 + 4u},{ _temp1898, _temp1898, _temp1898 + 4u},{ _temp1901, _temp1901,
_temp1901 + 4u},{ _temp1904, _temp1904, _temp1904 + 4u},{ _temp1907, _temp1907,
_temp1907 + 4u},{ _temp1910, _temp1910, _temp1910 + 5u},{ _temp1913, _temp1913,
_temp1913 + 17u},{ _temp1916, _temp1916, _temp1916 + 21u},{ _temp1919, _temp1919,
_temp1919 + 20u},{ _temp1922, _temp1922, _temp1922 + 21u},{ _temp1925, _temp1925,
_temp1925 + 13u},{ _temp1928, _temp1928, _temp1928 + 15u},{ _temp1931, _temp1931,
_temp1931 + 17u},{ _temp1934, _temp1934, _temp1934 + 19u},{ _temp1937, _temp1937,
_temp1937 + 12u},{ _temp1940, _temp1940, _temp1940 + 17u},{ _temp1943, _temp1943,
_temp1943 + 23u},{ _temp1946, _temp1946, _temp1946 + 24u},{ _temp1949, _temp1949,
_temp1949 + 15u},{ _temp1952, _temp1952, _temp1952 + 11u},{ _temp1955, _temp1955,
_temp1955 + 15u},{ _temp1958, _temp1958, _temp1958 + 10u},{ _temp1961, _temp1961,
_temp1961 + 15u},{ _temp1964, _temp1964, _temp1964 + 5u},{ _temp1967, _temp1967,
_temp1967 + 15u},{ _temp1970, _temp1970, _temp1970 + 15u},{ _temp1973, _temp1973,
_temp1973 + 11u},{ _temp1976, _temp1976, _temp1976 + 22u},{ _temp1979, _temp1979,
_temp1979 + 26u},{ _temp1982, _temp1982, _temp1982 + 16u},{ _temp1985, _temp1985,
_temp1985 + 16u},{ _temp1988, _temp1988, _temp1988 + 24u},{ _temp1991, _temp1991,
_temp1991 + 25u},{ _temp1994, _temp1994, _temp1994 + 21u},{ _temp1997, _temp1997,
_temp1997 + 22u},{ _temp2000, _temp2000, _temp2000 + 16u},{ _temp2003, _temp2003,
_temp2003 + 19u},{ _temp2006, _temp2006, _temp2006 + 25u},{ _temp2009, _temp2009,
_temp2009 + 23u},{ _temp2012, _temp2012, _temp2012 + 24u},{ _temp2015, _temp2015,
_temp2015 + 18u},{ _temp2018, _temp2018, _temp2018 + 17u},{ _temp2021, _temp2021,
_temp2021 + 18u},{ _temp2024, _temp2024, _temp2024 + 17u},{ _temp2027, _temp2027,
_temp2027 + 18u},{ _temp2030, _temp2030, _temp2030 + 12u},{ _temp2033, _temp2033,
_temp2033 + 11u},{ _temp2036, _temp2036, _temp2036 + 18u},{ _temp2039, _temp2039,
_temp2039 + 8u},{ _temp2042, _temp2042, _temp2042 + 13u},{ _temp2045, _temp2045,
_temp2045 + 8u},{ _temp2048, _temp2048, _temp2048 + 4u},{ _temp2051, _temp2051,
_temp2051 + 20u},{ _temp2054, _temp2054, _temp2054 + 20u},{ _temp2057, _temp2057,
_temp2057 + 11u},{ _temp2060, _temp2060, _temp2060 + 14u},{ _temp2063, _temp2063,
_temp2063 + 11u},{ _temp2066, _temp2066, _temp2066 + 15u},{ _temp2069, _temp2069,
_temp2069 + 22u},{ _temp2072, _temp2072, _temp2072 + 16u},{ _temp2075, _temp2075,
_temp2075 + 17u},{ _temp2078, _temp2078, _temp2078 + 12u},{ _temp2081, _temp2081,
_temp2081 + 18u},{ _temp2084, _temp2084, _temp2084 + 17u},{ _temp2087, _temp2087,
_temp2087 + 12u},{ _temp2090, _temp2090, _temp2090 + 16u},{ _temp2093, _temp2093,
_temp2093 + 11u},{ _temp2096, _temp2096, _temp2096 + 10u},{ _temp2099, _temp2099,
_temp2099 + 14u},{ _temp2102, _temp2102, _temp2102 + 15u},{ _temp2105, _temp2105,
_temp2105 + 20u},{ _temp2108, _temp2108, _temp2108 + 27u},{ _temp2111, _temp2111,
_temp2111 + 10u},{ _temp2114, _temp2114, _temp2114 + 18u},{ _temp2117, _temp2117,
_temp2117 + 21u},{ _temp2120, _temp2120, _temp2120 + 19u},{ _temp2123, _temp2123,
_temp2123 + 16u},{ _temp2126, _temp2126, _temp2126 + 11u},{ _temp2129, _temp2129,
_temp2129 + 20u},{ _temp2132, _temp2132, _temp2132 + 15u},{ _temp2135, _temp2135,
_temp2135 + 20u},{ _temp2138, _temp2138, _temp2138 + 15u},{ _temp2141, _temp2141,
_temp2141 + 8u},{ _temp2144, _temp2144, _temp2144 + 19u},{ _temp2147, _temp2147,
_temp2147 + 20u},{ _temp2150, _temp2150, _temp2150 + 14u},{ _temp2153, _temp2153,
_temp2153 + 19u},{ _temp2156, _temp2156, _temp2156 + 20u},{ _temp2159, _temp2159,
_temp2159 + 11u},{ _temp2162, _temp2162, _temp2162 + 22u},{ _temp2165, _temp2165,
_temp2165 + 20u},{ _temp2168, _temp2168, _temp2168 + 23u},{ _temp2171, _temp2171,
_temp2171 + 20u},{ _temp2174, _temp2174, _temp2174 + 22u},{ _temp2177, _temp2177,
_temp2177 + 23u},{ _temp2180, _temp2180, _temp2180 + 24u},{ _temp2183, _temp2183,
_temp2183 + 24u},{ _temp2186, _temp2186, _temp2186 + 15u},{ _temp2189, _temp2189,
_temp2189 + 20u},{ _temp2192, _temp2192, _temp2192 + 22u},{ _temp2195, _temp2195,
_temp2195 + 17u},{ _temp2198, _temp2198, _temp2198 + 20u},{ _temp2201, _temp2201,
_temp2201 + 26u},{ _temp2204, _temp2204, _temp2204 + 16u},{ _temp2207, _temp2207,
_temp2207 + 17u},{ _temp2210, _temp2210, _temp2210 + 14u},{ _temp2213, _temp2213,
_temp2213 + 15u},{ _temp2216, _temp2216, _temp2216 + 19u},{ _temp2219, _temp2219,
_temp2219 + 19u},{ _temp2222, _temp2222, _temp2222 + 25u},{ _temp2225, _temp2225,
_temp2225 + 26u},{ _temp2228, _temp2228, _temp2228 + 9u},{ _temp2231, _temp2231,
_temp2231 + 20u}}; static short Cyc_yyr1[ 395u]={ (short)0, (short)124, (short)125,
(short)125, (short)125, (short)125, (short)125, (short)125, (short)125, (short)126,
(short)126, (short)127, (short)127, (short)127, (short)127, (short)128, (short)128,
(short)129, (short)130, (short)131, (short)132, (short)133, (short)133, (short)133,
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
static short Cyc_yyr2[ 395u]={ (short)0, (short)1, (short)2, (short)3, (short)5,
(short)3, (short)5, (short)6, (short)0, (short)1, (short)1, (short)2, (short)3,
(short)3, (short)4, (short)3, (short)4, (short)2, (short)1, (short)2, (short)1,
(short)2, (short)3, (short)5, (short)3, (short)1, (short)2, (short)2, (short)3,
(short)2, (short)3, (short)2, (short)3, (short)2, (short)3, (short)1, (short)1,
(short)1, (short)1, (short)2, (short)1, (short)1, (short)0, (short)1, (short)6,
(short)1, (short)3, (short)1, (short)1, (short)4, (short)4, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)3, (short)2, (short)4, (short)4,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)5, (short)2, (short)1,
(short)3, (short)1, (short)3, (short)4, (short)6, (short)6, (short)3, (short)3,
(short)0, (short)3, (short)3, (short)1, (short)1, (short)1, (short)1, (short)2,
(short)1, (short)1, (short)3, (short)1, (short)3, (short)3, (short)2, (short)3,
(short)2, (short)3, (short)1, (short)1, (short)3, (short)1, (short)2, (short)3,
(short)6, (short)3, (short)4, (short)5, (short)1, (short)1, (short)1, (short)2,
(short)3, (short)3, (short)0, (short)1, (short)1, (short)2, (short)6, (short)1,
(short)2, (short)1, (short)3, (short)3, (short)4, (short)4, (short)3, (short)5,
(short)4, (short)4, (short)4, (short)2, (short)3, (short)4, (short)4, (short)5,
(short)1, (short)1, (short)4, (short)4, (short)1, (short)0, (short)1, (short)1,
(short)3, (short)1, (short)1, (short)2, (short)1, (short)3, (short)3, (short)1,
(short)3, (short)2, (short)3, (short)1, (short)3, (short)1, (short)3, (short)3,
(short)5, (short)1, (short)3, (short)2, (short)1, (short)2, (short)1, (short)1,
(short)3, (short)1, (short)1, (short)2, (short)3, (short)4, (short)8, (short)1,
(short)2, (short)3, (short)4, (short)2, (short)1, (short)2, (short)3, (short)2,
(short)1, (short)2, (short)1, (short)2, (short)3, (short)3, (short)1, (short)3,
(short)1, (short)1, (short)2, (short)3, (short)2, (short)3, (short)3, (short)4,
(short)2, (short)4, (short)3, (short)3, (short)5, (short)4, (short)4, (short)4,
(short)2, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)6,
(short)3, (short)2, (short)2, (short)3, (short)1, (short)2, (short)2, (short)3,
(short)1, (short)2, (short)1, (short)1, (short)1, (short)5, (short)7, (short)7,
(short)6, (short)0, (short)3, (short)4, (short)5, (short)6, (short)7, (short)5,
(short)7, (short)6, (short)7, (short)7, (short)8, (short)7, (short)8, (short)8,
(short)9, (short)6, (short)7, (short)7, (short)8, (short)3, (short)2, (short)2,
(short)2, (short)3, (short)2, (short)4, (short)5, (short)1, (short)3, (short)1,
(short)2, (short)1, (short)1, (short)1, (short)1, (short)5, (short)4, (short)4,
(short)5, (short)2, (short)2, (short)0, (short)1, (short)1, (short)3, (short)1,
(short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)5, (short)2, (short)2, (short)2,
(short)5, (short)5, (short)1, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)3,
(short)1, (short)3, (short)3, (short)3, (short)3, (short)1, (short)3, (short)3,
(short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)3, (short)1,
(short)4, (short)1, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)4, (short)2, (short)4, (short)7, (short)9, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)4,
(short)3, (short)4, (short)3, (short)3, (short)3, (short)3, (short)2, (short)2,
(short)6, (short)7, (short)4, (short)4, (short)1, (short)1, (short)1, (short)3,
(short)2, (short)5, (short)4, (short)4, (short)5, (short)1, (short)1, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1}; static short Cyc_yydefact[
754u]={ (short)8, (short)35, (short)36, (short)37, (short)38, (short)40, (short)51,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59,
(short)60, (short)71, (short)72, (short)73, (short)88, (short)89, (short)42,
(short)0, (short)0, (short)41, (short)0, (short)0, (short)113, (short)114,
(short)0, (short)393, (short)64, (short)394, (short)85, (short)0, (short)52,
(short)0, (short)141, (short)142, (short)145, (short)1, (short)8, (short)9,
(short)0, (short)0, (short)10, (short)0, (short)42, (short)42, (short)42,
(short)61, (short)62, (short)0, (short)63, (short)0, (short)0, (short)124,
(short)0, (short)146, (short)126, (short)39, (short)0, (short)33, (short)43,
(short)75, (short)272, (short)393, (short)268, (short)271, (short)270, (short)0,
(short)266, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)273, (short)17, (short)19, (short)0, (short)0, (short)0,
(short)66, (short)0, (short)0, (short)0, (short)0, (short)2, (short)8, (short)8,
(short)8, (short)8, (short)21, (short)0, (short)93, (short)94, (short)96,
(short)27, (short)29, (short)31, (short)85, (short)0, (short)85, (short)148,
(short)150, (short)0, (short)85, (short)38, (short)0, (short)25, (short)0,
(short)0, (short)11, (short)0, (short)0, (short)0, (short)136, (short)125,
(short)42, (short)147, (short)8, (short)0, (short)34, (short)0, (short)0,
(short)280, (short)279, (short)269, (short)278, (short)24, (short)0, (short)0,
(short)0, (short)0, (short)42, (short)42, (short)189, (short)191, (short)0,
(short)69, (short)70, (short)65, (short)195, (short)0, (short)127, (short)169,
(short)0, (short)166, (short)0, (short)392, (short)0, (short)0, (short)0,
(short)0, (short)354, (short)355, (short)356, (short)357, (short)358, (short)359,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)389, (short)379,
(short)390, (short)391, (short)0, (short)0, (short)0, (short)0, (short)362,
(short)0, (short)360, (short)361, (short)0, (short)291, (short)304, (short)312,
(short)314, (short)316, (short)318, (short)320, (short)322, (short)325, (short)330,
(short)333, (short)336, (short)340, (short)0, (short)0, (short)342, (short)363,
(short)378, (short)377, (short)0, (short)3, (short)0, (short)5, (short)0,
(short)22, (short)0, (short)0, (short)0, (short)12, (short)28, (short)30,
(short)32, (short)84, (short)0, (short)90, (short)91, (short)0, (short)83,
(short)0, (short)85, (short)0, (short)110, (short)39, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)393, (short)225, (short)227,
(short)0, (short)233, (short)231, (short)0, (short)232, (short)214, (short)215,
(short)216, (short)0, (short)229, (short)217, (short)218, (short)219, (short)0,
(short)289, (short)26, (short)13, (short)96, (short)172, (short)0, (short)131,
(short)0, (short)153, (short)0, (short)0, (short)128, (short)0, (short)137,
(short)0, (short)0, (short)78, (short)0, (short)76, (short)267, (short)282,
(short)0, (short)281, (short)173, (short)0, (short)0, (short)280, (short)162,
(short)192, (short)0, (short)99, (short)101, (short)0, (short)0, (short)197,
(short)190, (short)198, (short)68, (short)0, (short)87, (short)0, (short)86,
(short)0, (short)168, (short)197, (short)170, (short)67, (short)0, (short)0,
(short)350, (short)306, (short)340, (short)0, (short)307, (short)308, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)343, (short)344,
(short)0, (short)0, (short)0, (short)0, (short)346, (short)347, (short)345,
(short)143, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)294, (short)295, (short)296,
(short)297, (short)298, (short)299, (short)300, (short)301, (short)302, (short)303,
(short)293, (short)0, (short)0, (short)348, (short)0, (short)371, (short)372,
(short)0, (short)0, (short)0, (short)381, (short)0, (short)0, (short)144,
(short)18, (short)8, (short)20, (short)8, (short)95, (short)97, (short)175,
(short)174, (short)14, (short)0, (short)80, (short)92, (short)0, (short)0,
(short)103, (short)104, (short)106, (short)0, (short)149, (short)111, (short)85,
(short)119, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)259,
(short)260, (short)261, (short)0, (short)0, (short)263, (short)0, (short)222,
(short)223, (short)0, (short)0, (short)0, (short)0, (short)96, (short)228,
(short)230, (short)226, (short)0, (short)160, (short)0, (short)0, (short)156,
(short)130, (short)0, (short)0, (short)133, (short)135, (short)134, (short)129,
(short)151, (short)139, (short)138, (short)8, (short)48, (short)47, (short)0,
(short)45, (short)0, (short)74, (short)0, (short)275, (short)0, (short)23,
(short)276, (short)0, (short)0, (short)0, (short)0, (short)185, (short)284,
(short)287, (short)0, (short)286, (short)0, (short)0, (short)0, (short)193,
(short)100, (short)102, (short)0, (short)205, (short)0, (short)0, (short)201,
(short)0, (short)199, (short)0, (short)0, (short)0, (short)213, (short)194,
(short)196, (short)167, (short)0, (short)0, (short)176, (short)180, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)380, (short)387, (short)0, (short)386, (short)313,
(short)0, (short)315, (short)317, (short)319, (short)321, (short)323, (short)324,
(short)328, (short)329, (short)326, (short)327, (short)331, (short)332, (short)334,
(short)335, (short)337, (short)338, (short)339, (short)292, (short)0, (short)369,
(short)370, (short)365, (short)0, (short)367, (short)368, (short)0, (short)0,
(short)0, (short)4, (short)6, (short)0, (short)311, (short)107, (short)98,
(short)0, (short)0, (short)0, (short)112, (short)121, (short)120, (short)0,
(short)0, (short)115, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)258, (short)262, (short)0, (short)0, (short)0, (short)221,
(short)0, (short)224, (short)0, (short)15, (short)290, (short)0, (short)158,
(short)0, (short)132, (short)0, (short)155, (short)154, (short)152, (short)140,
(short)7, (short)0, (short)0, (short)0, (short)79, (short)77, (short)283,
(short)188, (short)0, (short)285, (short)184, (short)186, (short)277, (short)0,
(short)274, (short)164, (short)163, (short)0, (short)207, (short)200, (short)203,
(short)0, (short)208, (short)0, (short)0, (short)202, (short)0, (short)349,
(short)0, (short)177, (short)0, (short)181, (short)375, (short)376, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)341, (short)383,
(short)0, (short)0, (short)351, (short)366, (short)364, (short)384, (short)0,
(short)0, (short)82, (short)105, (short)108, (short)81, (short)109, (short)122,
(short)119, (short)119, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)238, (short)264, (short)0,
(short)0, (short)16, (short)161, (short)159, (short)157, (short)0, (short)0,
(short)44, (short)46, (short)187, (short)288, (short)0, (short)206, (short)0,
(short)210, (short)212, (short)211, (short)204, (short)0, (short)178, (short)182,
(short)0, (short)0, (short)309, (short)310, (short)0, (short)385, (short)0,
(short)388, (short)305, (short)382, (short)0, (short)118, (short)117, (short)234,
(short)238, (short)244, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)265,
(short)0, (short)49, (short)50, (short)165, (short)209, (short)0, (short)183,
(short)0, (short)0, (short)373, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)246, (short)0, (short)0, (short)0, (short)254, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)237, (short)220,
(short)0, (short)352, (short)0, (short)374, (short)0, (short)235, (short)236,
(short)245, (short)247, (short)248, (short)0, (short)256, (short)255, (short)0,
(short)250, (short)0, (short)0, (short)0, (short)0, (short)238, (short)239,
(short)0, (short)0, (short)123, (short)249, (short)257, (short)251, (short)252,
(short)0, (short)0, (short)238, (short)240, (short)179, (short)353, (short)253,
(short)238, (short)241, (short)238, (short)242, (short)243, (short)0, (short)0,
(short)0}; static short Cyc_yydefgoto[ 108u]={ (short)751, (short)39, (short)40,
(short)41, (short)241, (short)42, (short)369, (short)43, (short)371, (short)44,
(short)207, (short)45, (short)46, (short)61, (short)62, (short)431, (short)432,
(short)47, (short)143, (short)48, (short)49, (short)270, (short)271, (short)50,
(short)84, (short)51, (short)213, (short)214, (short)95, (short)96, (short)97,
(short)215, (short)138, (short)381, (short)382, (short)383, (short)52, (short)53,
(short)530, (short)531, (short)532, (short)54, (short)55, (short)56, (short)57,
(short)120, (short)107, (short)427, (short)457, (short)416, (short)417, (short)283,
(short)262, (short)149, (short)75, (short)76, (short)472, (short)374, (short)473,
(short)474, (short)443, (short)444, (short)139, (short)144, (short)145, (short)458,
(short)290, (short)244, (short)245, (short)246, (short)247, (short)248, (short)249,
(short)250, (short)679, (short)251, (short)252, (short)274, (short)275, (short)276,
(short)446, (short)447, (short)448, (short)253, (short)254, (short)355, (short)180,
(short)522, (short)181, (short)182, (short)183, (short)184, (short)185, (short)186,
(short)187, (short)188, (short)189, (short)190, (short)191, (short)192, (short)193,
(short)194, (short)195, (short)196, (short)486, (short)487, (short)197, (short)198};
static short Cyc_yypact[ 754u]={ (short)1606, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)5, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)43, (short)24, (short)663, - (short)-32768,
(short)24, (short)61, - (short)-32768, - (short)-32768, (short)58, - (short)-32768,
(short)118, - (short)-32768, (short)120, (short)395, - (short)-32768, (short)156,
(short)179, (short)186, - (short)-32768, - (short)-32768, (short)1606, - (short)-32768,
(short)148, (short)228, - (short)-32768, (short)147, (short)43, (short)43,
(short)43, - (short)-32768, - (short)-32768, (short)283, - (short)-32768,
(short)178, (short)1904, (short)182, (short)139, (short)29, - (short)-32768,
(short)193, (short)201, (short)1964, - (short)-32768, (short)196, - (short)-32768,
(short)165, - (short)-32768, - (short)-32768, - (short)-32768, (short)682, -
(short)-32768, (short)204, (short)230, (short)257, (short)682, (short)290,
(short)251, (short)303, - (short)24, - (short)-32768, - (short)-32768, (short)2216,
(short)57, (short)2216, - (short)-32768, (short)288, (short)2283, (short)3794,
(short)3794, - (short)-32768, (short)1606, (short)1606, (short)1606, (short)1606,
- (short)-32768, (short)344, (short)321, - (short)-32768, (short)1841, (short)1964,
(short)1964, (short)1964, (short)120, (short)2283, (short)120, (short)351, -
(short)-32768, (short)24, (short)108, (short)353, (short)1127, - (short)-32768,
(short)1904, (short)147, - (short)-32768, (short)2104, (short)2216, (short)2658,
- (short)-32768, (short)182, (short)43, - (short)-32768, (short)1606, (short)346,
- (short)-32768, (short)24, (short)348, (short)682, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)367, (short)3794, (short)157, (short)187,
(short)43, (short)43, (short)298, - (short)-32768, - (short)2, - (short)-32768,
- (short)-32768, - (short)-32768, (short)345, (short)70, - (short)-32768,
(short)281, (short)236, - (short)-32768, (short)4007, - (short)-32768, (short)3794,
(short)3865, (short)380, (short)387, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)391, (short)396,
(short)415, (short)4078, (short)4078, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)822, (short)437, (short)4149, (short)4149,
- (short)-32768, (short)4149, - (short)-32768, - (short)-32768, (short)446, -
(short)-32768, - (short)31, (short)413, (short)429, (short)431, (short)443,
(short)374, - (short)19, (short)385, (short)342, (short)92, - (short)-32768,
(short)545, (short)453, (short)4149, (short)110, - (short)-32768, - (short)-32768,
(short)83, (short)457, - (short)-32768, (short)463, - (short)-32768, (short)474,
- (short)-32768, (short)395, (short)2729, (short)1904, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)476, (short)480, (short)2283,
- (short)-32768, (short)220, (short)482, (short)57, (short)120, (short)24,
(short)484, - (short)-32768, (short)485, (short)488, (short)490, (short)1988,
(short)491, (short)493, (short)486, (short)496, (short)2800, (short)1988, -
(short)66, (short)1988, (short)1988, - (short)27, (short)477, - (short)-32768, -
(short)-32768, (short)495, - (short)-32768, - (short)-32768, (short)147, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)498,
(short)1248, - (short)-32768, - (short)-32768, - (short)-32768, (short)190, -
(short)-32768, - (short)-32768, - (short)-32768, (short)494, - (short)-32768,
(short)149, - (short)-32768, (short)497, (short)206, (short)500, (short)76, -
(short)-32768, (short)479, (short)80, (short)504, (short)32, (short)501, (short)506,
(short)509, - (short)-32768, - (short)-32768, (short)508, (short)527, - (short)-32768,
(short)213, (short)629, (short)682, - (short)35, - (short)-32768, (short)513,
(short)2283, (short)2283, (short)1781, (short)2871, (short)60, - (short)-32768,
(short)205, - (short)-32768, (short)149, - (short)-32768, (short)2216, - (short)-32768,
(short)1721, - (short)-32768, (short)127, - (short)-32768, - (short)-32768,
(short)2283, (short)822, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2302, - (short)-32768, (short)557, (short)3794, (short)1666, (short)597,
(short)3794, (short)3794, (short)822, - (short)-32768, - (short)-32768, (short)1248,
(short)532, (short)370, (short)3794, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)4149, (short)3794, (short)4149, (short)4149, (short)4149,
(short)4149, (short)4149, (short)4149, (short)4149, (short)4149, (short)4149,
(short)4149, (short)4149, (short)4149, (short)4149, (short)4149, (short)4149,
(short)4149, (short)4149, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3794, (short)3794, -
(short)-32768, (short)114, - (short)-32768, - (short)-32768, (short)2942,
(short)137, (short)3794, - (short)-32768, (short)2569, (short)531, - (short)-32768,
- (short)-32768, (short)1606, - (short)-32768, (short)1606, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2283, -
(short)-32768, - (short)-32768, (short)3794, (short)541, (short)536, - (short)-32768,
(short)533, (short)2283, - (short)-32768, - (short)-32768, (short)120, (short)471,
(short)3794, (short)3794, (short)3794, (short)612, (short)1369, (short)546, -
(short)-32768, - (short)-32768, - (short)-32768, (short)259, (short)601, -
(short)-32768, (short)3013, - (short)-32768, - (short)-32768, (short)1988,
(short)551, (short)1988, (short)1485, (short)1841, - (short)-32768, - (short)-32768,
- (short)-32768, (short)3794, (short)560, (short)264, (short)548, (short)537, -
(short)-32768, (short)149, (short)2230, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)80,
(short)1606, - (short)-32768, (short)550, (short)553, (short)558, (short)24, -
(short)-32768, (short)3794, - (short)-32768, (short)682, - (short)-32768, -
(short)-32768, (short)565, (short)3794, (short)682, (short)78, - (short)-32768,
- (short)-32768, - (short)-32768, (short)561, (short)559, (short)562, (short)57,
(short)570, - (short)-32768, - (short)-32768, - (short)-32768, (short)149, -
(short)-32768, (short)563, (short)566, - (short)-32768, (short)568, (short)205,
(short)2118, (short)2216, (short)3084, - (short)-32768, - (short)-32768, (short)345,
- (short)-32768, (short)569, (short)586, - (short)-32768, - (short)-32768,
(short)273, (short)2729, (short)377, (short)582, (short)395, (short)587, (short)382,
(short)580, (short)585, (short)590, (short)3936, - (short)-32768, - (short)-32768,
(short)592, (short)596, (short)413, (short)214, (short)429, (short)431, (short)443,
(short)374, - (short)19, - (short)19, (short)385, (short)385, (short)385,
(short)385, (short)342, (short)342, (short)92, (short)92, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)593, - (short)-32768, -
(short)-32768, - (short)-32768, (short)599, - (short)-32768, - (short)-32768,
(short)39, (short)326, (short)2216, - (short)-32768, - (short)-32768, (short)603,
- (short)-32768, - (short)-32768, - (short)-32768, (short)220, (short)3794,
(short)605, - (short)-32768, - (short)-32768, - (short)-32768, (short)606,
(short)24, (short)276, (short)388, (short)397, (short)399, (short)607, (short)3155,
(short)3226, (short)286, - (short)-32768, - (short)-32768, (short)608, (short)610,
(short)609, - (short)-32768, (short)602, - (short)-32768, (short)1904, - (short)-32768,
- (short)-32768, (short)57, - (short)-32768, (short)613, - (short)-32768,
(short)149, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)416, (short)622, (short)32, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)604, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)581, - (short)-32768, (short)611, -
(short)-32768, (short)623, - (short)-32768, - (short)-32768, - (short)-32768,
(short)149, - (short)-32768, (short)624, (short)79, - (short)-32768, (short)616,
(short)631, (short)625, - (short)-32768, (short)2391, - (short)-32768, - (short)-32768,
- (short)-32768, (short)1904, (short)2283, (short)3865, (short)697, (short)631,
(short)633, (short)2569, - (short)-32768, - (short)-32768, (short)3794, (short)3794,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2569,
(short)327, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)300, (short)36, (short)471, (short)1988, (short)638,
(short)1988, (short)3794, (short)3297, (short)295, (short)3368, (short)317,
(short)3439, (short)489, - (short)-32768, (short)640, (short)654, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)642, (short)645, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)570, -
(short)-32768, (short)646, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3794, - (short)-32768, - (short)-32768, (short)2729,
(short)647, - (short)-32768, (short)557, (short)649, - (short)-32768, (short)332,
- (short)-32768, - (short)-32768, - (short)-32768, (short)652, - (short)-32768,
- (short)-32768, (short)730, (short)489, - (short)-32768, (short)420, (short)1988,
(short)422, (short)3510, (short)1988, (short)426, (short)3581, (short)3652,
(short)320, (short)682, (short)650, (short)660, - (short)-32768, (short)1988, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)310, -
(short)-32768, (short)658, (short)2283, - (short)-32768, (short)2480, (short)2283,
(short)1988, (short)662, (short)669, - (short)-32768, (short)1988, (short)1988,
(short)428, - (short)-32768, (short)1988, (short)1988, (short)430, (short)1988,
(short)433, (short)3723, (short)3, (short)1248, - (short)-32768, - (short)-32768,
(short)3794, - (short)-32768, (short)667, - (short)-32768, (short)439, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1988,
- (short)-32768, - (short)-32768, (short)1988, - (short)-32768, (short)1988,
(short)1988, (short)450, (short)3794, (short)1006, - (short)-32768, (short)341,
(short)670, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)1988, (short)313, (short)489, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1006, - (short)-32768, (short)489, -
(short)-32768, - (short)-32768, (short)777, (short)778, - (short)-32768}; static
short Cyc_yypgoto[ 108u]={ - (short)-32768, - (short)21, - (short)-32768,
(short)472, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)50, - (short)41, - (short)54, - (short)-32768, (short)48,
- (short)39, (short)218, - (short)-32768, (short)51, - (short)200, - (short)78,
- (short)-32768, - (short)-32768, (short)352, - (short)-32768, - (short)68, -
(short)-32768, - (short)187, - (short)-32768, - (short)-32768, - (short)-32768,
(short)579, (short)574, - (short)11, - (short)-32768, - (short)-32768, (short)265,
- (short)-32768, - (short)-32768, - (short)99, - (short)-32768, - (short)-32768,
(short)84, - (short)34, - (short)57, - (short)-32768, - (short)-32768, (short)733,
- (short)-32768, - (short)95, - (short)390, (short)499, - (short)388, - (short)85,
- (short)277, (short)677, - (short)-32768, - (short)204, - (short)147, - (short)348,
- (short)249, - (short)-32768, (short)350, - (short)143, - (short)60, - (short)102,
(short)25, - (short)157, - (short)126, - (short)-32768, - (short)-32768, -
(short)45, - (short)226, - (short)-32768, - (short)-32768, - (short)627, -
(short)-32768, - (short)-32768, - (short)10, (short)515, - (short)-32768,
(short)223, - (short)-32768, - (short)-32768, (short)225, (short)113, - (short)-32768,
- (short)137, - (short)409, - (short)142, (short)475, (short)470, (short)473,
(short)478, (short)514, (short)227, (short)129, (short)232, (short)226, -
(short)46, (short)483, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)246, - (short)-32768, - (short)-32768, (short)33};
static short Cyc_yytable[ 4273u]={ (short)113, (short)148, (short)373, (short)137,
(short)111, (short)137, (short)307, (short)124, (short)137, (short)114, (short)134,
(short)308, (short)77, (short)112, (short)264, (short)304, (short)118, (short)516,
(short)386, (short)89, (short)261, (short)140, (short)119, (short)411, (short)468,
(short)137, (short)565, (short)553, (short)318, (short)556, (short)442, (short)401,
(short)568, (short)58, (short)212, (short)402, (short)217, (short)137, (short)137,
(short)694, (short)221, (short)528, (short)529, (short)325, (short)113, (short)209,
(short)210, (short)211, (short)111, (short)429, (short)333, (short)334, (short)450,
(short)208, (short)63, (short)78, (short)243, (short)79, (short)113, (short)126,
(short)242, (short)405, (short)255, (short)576, (short)130, (short)577, (short)58,
(short)256, (short)451, (short)200, (short)201, (short)202, (short)203, (short)58,
- (short)85, (short)147, (short)729, - (short)85, (short)58, (short)406, (short)118,
(short)288, (short)83, (short)326, (short)104, (short)59, (short)108, (short)335,
(short)336, (short)58, (short)298, (short)482, (short)216, (short)137, (short)99,
(short)100, (short)101, (short)15, (short)16, (short)17, (short)393, (short)268,
(short)78, (short)742, (short)147, (short)291, (short)400, (short)78, (short)403,
(short)404, (short)508, (short)730, (short)29, (short)292, (short)747, (short)512,
(short)613, (short)85, (short)31, (short)749, (short)430, (short)750, (short)105,
(short)58, (short)58, (short)58, (short)58, (short)321, (short)322, (short)98,
(short)323, (short)461, (short)136, (short)106, (short)136, - (short)116,
(short)137, (short)136, (short)293, (short)60, (short)219, (short)461, (short)413,
(short)468, (short)422, (short)141, (short)58, (short)646, (short)357, (short)80,
(short)607, (short)387, (short)142, (short)113, (short)136, (short)58, (short)544,
(short)255, (short)272, (short)469, (short)78, (short)286, (short)376, (short)289,
(short)81, (short)635, (short)136, (short)136, (short)267, (short)364, (short)287,
(short)481, (short)299, (short)294, (short)358, (short)359, (short)360, (short)295,
(short)570, (short)294, (short)58, (short)365, (short)294, (short)423, (short)284,
(short)285, (short)647, (short)440, (short)441, (short)425, (short)520, (short)644,
(short)36, (short)37, (short)38, (short)243, (short)366, (short)257, (short)526,
(short)242, (short)179, (short)199, (short)509, (short)216, (short)341, (short)82,
(short)137, (short)137, (short)137, (short)510, (short)426, (short)361, (short)342,
(short)343, (short)83, (short)29, (short)137, (short)220, (short)137, (short)362,
(short)363, (short)31, (short)136, (short)137, (short)137, (short)513, (short)83,
(short)29, (short)296, (short)288, (short)266, (short)297, (short)514, (short)31,
(short)467, (short)29, (short)137, (short)287, (short)58, (short)298, (short)33,
(short)31, (short)414, (short)521, (short)94, (short)90, (short)91, (short)415,
(short)33, (short)58, (short)575, (short)465, (short)658, (short)388, (short)684,
(short)279, (short)477, (short)86, (short)280, (short)36, (short)37, (short)38,
- (short)172, (short)243, (short)119, (short)136, (short)29, (short)242, -
(short)172, (short)445, (short)591, (short)105, (short)31, (short)453, (short)454,
(short)147, (short)58, (short)87, (short)60, (short)545, (short)281, (short)547,
(short)106, (short)115, (short)88, (short)147, (short)282, (short)412, (short)116,
(short)257, (short)147, (short)122, (short)117, (short)413, (short)125, (short)504,
(short)505, (short)506, (short)521, (short)137, (short)384, (short)60, (short)123,
(short)419, (short)521, (short)127, (short)462, (short)137, (short)29, (short)420,
(short)438, (short)463, (short)78, (short)78, (short)31, (short)464, (short)413,
(short)413, (short)128, (short)375, (short)527, (short)33, (short)604, (short)148,
(short)442, (short)92, (short)93, (short)409, (short)380, (short)58, (short)137,
(short)58, (short)36, (short)37, (short)38, (short)136, (short)136, (short)136,
(short)300, (short)301, (short)113, (short)652, (short)137, (short)58, (short)538,
(short)136, (short)129, (short)136, (short)518, (short)558, (short)519, (short)633,
(short)136, (short)136, (short)132, (short)113, (short)541, (short)281, (short)278,
(short)111, (short)652, (short)584, (short)413, (short)552, (short)549, (short)136,
(short)216, (short)583, (short)548, (short)29, (short)559, (short)29, (short)589,
(short)617, (short)216, (short)31, (short)590, (short)31, (short)102, (short)618,
(short)85, (short)103, (short)296, (short)627, (short)137, (short)137, (short)651,
(short)131, (short)521, (short)413, (short)146, (short)287, (short)671, (short)36,
(short)37, (short)38, (short)319, (short)147, (short)413, (short)286, (short)460,
- (short)85, (short)58, (short)133, (short)58, (short)651, (short)83, (short)560,
(short)287, (short)147, (short)36, (short)37, (short)38, (short)413, (short)674,
(short)610, (short)413, (short)706, (short)711, (short)375, (short)413, (short)746,
(short)465, (short)413, (short)205, (short)608, (short)480, (short)566, (short)136,
(short)609, (short)294, (short)690, (short)569, (short)485, (short)661, (short)691,
(short)136, (short)601, (short)218, (short)137, (short)743, (short)204, (short)652,
(short)222, (short)413, (short)331, (short)332, (short)269, (short)687, (short)654,
(short)273, (short)147, (short)337, (short)338, (short)655, (short)277, (short)399,
(short)339, (short)340, (short)136, (short)292, (short)58, (short)496, (short)497,
(short)498, (short)499, (short)272, (short)660, (short)507, (short)485, (short)78,
(short)136, (short)484, (short)413, (short)485, (short)78, (short)528, (short)529,
(short)375, (short)592, (short)413, (short)309, (short)731, (short)29, (short)596,
(short)413, (short)327, (short)651, (short)310, (short)31, (short)619, (short)413,
(short)311, (short)665, (short)113, (short)667, (short)33, (short)312, (short)255,
(short)620, (short)413, (short)621, (short)413, (short)632, (short)741, (short)636,
(short)637, (short)36, (short)37, (short)38, (short)58, (short)677, (short)678,
(short)136, (short)136, (short)485, (short)313, (short)137, (short)663, (short)664,
(short)748, (short)485, (short)695, (short)413, (short)697, (short)413, (short)550,
(short)319, (short)701, (short)413, (short)721, (short)413, (short)724, (short)413,
(short)475, (short)726, (short)413, (short)479, (short)320, (short)319, (short)113,
(short)734, (short)301, (short)696, (short)111, (short)324, (short)700, (short)328,
(short)662, (short)208, (short)329, (short)489, (short)739, (short)413, (short)356,
(short)710, (short)367, (short)58, (short)494, (short)495, (short)330, (short)594,
(short)368, (short)445, (short)616, (short)502, (short)503, (short)716, (short)136,
(short)500, (short)501, (short)719, (short)720, (short)370, (short)377, (short)722,
(short)723, (short)586, (short)725, (short)378, (short)385, (short)395, (short)389,
(short)396, (short)653, (short)407, (short)390, (short)375, (short)515, (short)391,
(short)424, (short)392, (short)394, (short)397, (short)206, (short)735, (short)408,
(short)410, (short)736, (short)418, (short)737, (short)738, (short)421, (short)428,
(short)433, (short)434, (short)78, (short)715, (short)384, (short)435, (short)436,
(short)137, (short)452, (short)745, (short)137, (short)533, (short)534, (short)535,
(short)64, (short)539, (short)344, (short)345, (short)346, (short)347, (short)348,
(short)349, (short)350, (short)351, (short)352, (short)353, (short)437, (short)325,
(short)478, (short)303, (short)483, (short)305, (short)305, (short)517, (short)523,
(short)524, (short)536, (short)525, (short)542, (short)540, (short)546, (short)354,
(short)136, (short)551, (short)315, (short)316, (short)554, (short)561, (short)555,
(short)567, (short)243, (short)562, (short)305, (short)305, (short)242, (short)305,
(short)572, (short)563, (short)573, (short)281, (short)574, (short)578, (short)64,
(short)707, (short)579, (short)29, (short)66, (short)587, (short)67, (short)68,
(short)588, (short)31, (short)243, (short)305, (short)713, (short)580, (short)242,
(short)147, (short)69, (short)597, (short)593, (short)70, (short)71, (short)598,
(short)595, (short)599, (short)440, (short)441, (short)243, (short)72, (short)602,
(short)605, (short)242, (short)73, (short)74, (short)603, (short)64, (short)606,
(short)611, (short)375, (short)614, (short)615, (short)628, (short)629, (short)622,
(short)631, (short)78, (short)630, (short)634, (short)375, (short)642, (short)640,
(short)659, (short)29, (short)66, (short)64, (short)67, (short)68, (short)375,
(short)31, (short)638, (short)643, (short)645, (short)648, (short)439, (short)600,
(short)69, (short)649, (short)656, (short)70, (short)71, (short)657, (short)666,
(short)680, (short)440, (short)441, (short)136, (short)72, (short)681, (short)136,
(short)682, (short)73, (short)74, (short)683, (short)685, (short)688, (short)689,
(short)65, (short)66, (short)692, (short)67, (short)68, (short)693, (short)31,
(short)708, (short)709, (short)712, (short)717, (short)624, (short)626, (short)69,
(short)375, (short)718, (short)70, (short)71, (short)733, (short)29, (short)66,
(short)744, (short)67, (short)68, (short)72, (short)31, (short)752, (short)753,
(short)73, (short)74, (short)639, (short)476, (short)69, (short)372, (short)564,
(short)70, (short)71, (short)379, (short)612, (short)121, (short)466, (short)263,
(short)571, (short)72, (short)449, (short)641, (short)490, (short)73, (short)74,
(short)488, (short)491, (short)0, (short)0, (short)375, (short)0, (short)0,
(short)492, (short)305, (short)0, (short)305, (short)305, (short)305, (short)305,
(short)305, (short)305, (short)305, (short)305, (short)305, (short)305, (short)305,
(short)305, (short)305, (short)305, (short)305, (short)305, (short)305, (short)0,
(short)0, (short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)493, (short)0, (short)0, (short)668, (short)670, (short)0,
(short)673, (short)0, (short)676, (short)0, (short)0, (short)0, (short)0,
(short)150, (short)21, (short)151, (short)0, (short)152, (short)0, (short)305,
(short)153, (short)0, (short)0, (short)0, (short)0, (short)26, (short)27,
(short)154, (short)155, (short)0, (short)686, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)28, (short)0, (short)163,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)699, (short)0, (short)0, (short)703,
(short)705, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)167, (short)168, (short)169, (short)170,
(short)30, (short)31, (short)32, (short)305, (short)0, (short)317, (short)0,
(short)0, (short)171, (short)305, (short)0, (short)34, (short)240, (short)0,
(short)0, (short)0, (short)728, (short)0, (short)0, (short)173, (short)0,
(short)732, (short)174, (short)175, (short)176, (short)0, (short)0, (short)0,
(short)0, (short)177, (short)178, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)740, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)305, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)305, (short)1, (short)2, (short)3, (short)109,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)677, (short)678, (short)20, (short)223, (short)0, (short)224, (short)225,
(short)226, (short)227, (short)228, (short)229, (short)230, (short)231, (short)150,
(short)21, (short)151, (short)22, (short)152, (short)232, (short)0, (short)153,
(short)23, (short)233, (short)0, (short)0, (short)26, (short)27, (short)154,
(short)155, (short)234, (short)235, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)28, (short)236, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)305, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)305, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)237, (short)167, (short)168, (short)169, (short)170, (short)30,
(short)31, (short)32, (short)0, (short)238, (short)110, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)34, (short)240, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)173, (short)0, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)0, (short)0, (short)177,
(short)178, (short)1, (short)2, (short)3, (short)109, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)223, (short)0, (short)224, (short)225, (short)226,
(short)227, (short)228, (short)229, (short)230, (short)231, (short)150, (short)21,
(short)151, (short)22, (short)152, (short)232, (short)0, (short)153, (short)23,
(short)233, (short)0, (short)0, (short)26, (short)27, (short)154, (short)155,
(short)234, (short)235, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)28, (short)236, (short)163, (short)164, (short)0,
(short)165, (short)166, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)237, (short)167, (short)168, (short)169, (short)170, (short)30, (short)31,
(short)32, (short)0, (short)238, (short)110, (short)239, (short)0, (short)171,
(short)0, (short)0, (short)34, (short)240, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)173, (short)0, (short)0, (short)174, (short)175,
(short)176, (short)0, (short)0, (short)0, (short)0, (short)177, (short)178,
(short)1, (short)2, (short)3, (short)109, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)223, (short)0, (short)224, (short)225, (short)226, (short)227, (short)228,
(short)229, (short)230, (short)231, (short)150, (short)21, (short)151, (short)22,
(short)152, (short)232, (short)0, (short)153, (short)23, (short)233, (short)0,
(short)0, (short)26, (short)27, (short)154, (short)155, (short)234, (short)235,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)28, (short)236, (short)163, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)237, (short)167,
(short)168, (short)169, (short)170, (short)30, (short)31, (short)32, (short)0,
(short)238, (short)110, (short)0, (short)0, (short)171, (short)0, (short)0,
(short)34, (short)240, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)173, (short)0, (short)0, (short)174, (short)175, (short)176, (short)0,
(short)0, (short)0, (short)0, (short)177, (short)178, (short)1, (short)2,
(short)3, (short)109, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)150,
(short)21, (short)151, (short)22, (short)152, (short)0, (short)0, (short)153,
(short)23, (short)0, (short)0, (short)0, (short)26, (short)27, (short)154,
(short)155, (short)0, (short)0, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)28, (short)0, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)167, (short)168, (short)169, (short)170, (short)30,
(short)31, (short)32, (short)0, (short)537, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)34, (short)240, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)173, (short)0, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)0, (short)0, (short)177,
(short)178, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)150, (short)21,
(short)151, (short)0, (short)152, (short)0, (short)0, (short)153, (short)0,
(short)0, (short)0, (short)0, (short)26, (short)27, (short)154, (short)155,
(short)0, (short)0, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)28, (short)0, (short)163, (short)164, (short)0,
(short)165, (short)166, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)167, (short)168, (short)169, (short)170, (short)30, (short)31,
(short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171, (short)0,
(short)0, (short)34, (short)240, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)173, (short)0, (short)0, (short)174, (short)175, (short)176,
(short)0, (short)0, (short)0, (short)0, (short)177, (short)178, (short)1,
(short)2, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)21, (short)0, (short)22, (short)0, (short)0, (short)0,
(short)0, (short)23, (short)0, (short)24, (short)25, (short)26, (short)27,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)109, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)30, (short)31, (short)32,
(short)21, (short)0, (short)0, (short)0, (short)0, (short)33, (short)0, (short)23,
(short)34, (short)35, (short)0, (short)26, (short)27, (short)0, (short)0,
(short)0, (short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)0, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0,
(short)0, (short)21, (short)0, (short)30, (short)31, (short)32, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)33, (short)26, (short)27, (short)34,
(short)35, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)36,
(short)37, (short)38, (short)28, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)0, (short)30, (short)31, (short)32, (short)21,
(short)455, (short)0, (short)0, (short)0, (short)296, (short)456, (short)0,
(short)34, (short)35, (short)0, (short)26, (short)27, (short)0, (short)287,
(short)0, (short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)0, (short)1, (short)2, (short)3, (short)109,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)21, (short)455,
(short)22, (short)0, (short)0, (short)286, (short)456, (short)23, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)0, (short)287, (short)0,
(short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)109, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)110,
(short)21, (short)206, (short)22, (short)0, (short)0, (short)34, (short)35,
(short)23, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)0, (short)1, (short)2, (short)3, (short)109,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)21, (short)0,
(short)110, (short)0, (short)0, (short)0, (short)0, (short)23, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)223, (short)0, (short)224,
(short)225, (short)226, (short)227, (short)228, (short)229, (short)230, (short)231,
(short)150, (short)28, (short)151, (short)0, (short)152, (short)232, (short)0,
(short)153, (short)0, (short)233, (short)0, (short)0, (short)0, (short)0,
(short)154, (short)155, (short)234, (short)235, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)236, (short)163,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)30,
(short)0, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)34, (short)35, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)237, (short)167, (short)168, (short)169, (short)170,
(short)0, (short)31, (short)0, (short)0, (short)238, (short)110, (short)0,
(short)0, (short)171, (short)0, (short)0, (short)0, (short)172, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)173, (short)0, (short)0,
(short)174, (short)175, (short)176, (short)0, (short)0, (short)0, (short)0,
(short)177, (short)178, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)258, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)259,
(short)0, (short)0, (short)0, (short)0, (short)260, (short)0, (short)34, (short)35,
(short)0, (short)30, (short)0, (short)32, (short)0, (short)581, (short)0,
(short)0, (short)0, (short)0, (short)582, (short)0, (short)34, (short)35,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)26, (short)27, (short)0, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)135,
(short)557, (short)0, (short)0, (short)0, (short)21, (short)34, (short)35,
(short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)0, (short)0,
(short)26, (short)27, (short)470, (short)0, (short)34, (short)35, (short)0,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)28,
(short)153, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)0, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)34, (short)35,
(short)0, (short)29, (short)167, (short)168, (short)169, (short)170, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)306, (short)471, (short)0,
(short)171, (short)0, (short)0, (short)0, (short)172, (short)0, (short)0,
(short)0, (short)440, (short)441, (short)0, (short)173, (short)0, (short)0,
(short)174, (short)175, (short)176, (short)0, (short)0, (short)0, (short)0,
(short)177, (short)178, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)0, (short)153, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)0,
(short)163, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)167, (short)168, (short)169,
(short)170, (short)0, (short)31, (short)0, (short)0, (short)0, (short)306,
(short)650, (short)0, (short)171, (short)0, (short)0, (short)0, (short)172,
(short)0, (short)0, (short)0, (short)440, (short)441, (short)0, (short)173,
(short)0, (short)0, (short)174, (short)175, (short)176, (short)0, (short)0,
(short)0, (short)0, (short)177, (short)178, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)0, (short)153, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)154, (short)155, (short)0,
(short)0, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)0, (short)163, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167,
(short)168, (short)169, (short)170, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)306, (short)714, (short)0, (short)171, (short)0, (short)0,
(short)0, (short)172, (short)0, (short)0, (short)0, (short)440, (short)441,
(short)0, (short)173, (short)0, (short)0, (short)174, (short)175, (short)176,
(short)0, (short)0, (short)0, (short)0, (short)177, (short)178, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)0, (short)153,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154, (short)155,
(short)0, (short)0, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)0, (short)163, (short)164, (short)0,
(short)165, (short)166, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)167, (short)168, (short)169, (short)170, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)306, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)0, (short)172, (short)0, (short)0, (short)0, (short)440,
(short)441, (short)0, (short)173, (short)0, (short)0, (short)174, (short)175,
(short)176, (short)0, (short)0, (short)0, (short)0, (short)177, (short)178,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)0,
(short)153, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)0, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)167, (short)168, (short)169, (short)170, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)0, (short)172, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)265, (short)173, (short)153, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)154, (short)155, (short)177,
(short)178, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)0, (short)163, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167,
(short)168, (short)169, (short)170, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)306, (short)0, (short)0, (short)171, (short)0, (short)0,
(short)0, (short)172, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)173, (short)153, (short)0, (short)174, (short)175, (short)176,
(short)0, (short)0, (short)154, (short)155, (short)177, (short)178, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)0, (short)163, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)167, (short)168,
(short)169, (short)170, (short)0, (short)31, (short)0, (short)0, (short)398,
(short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)0, (short)172,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)173,
(short)153, (short)0, (short)174, (short)175, (short)176, (short)0, (short)0,
(short)154, (short)155, (short)177, (short)178, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)0, (short)163,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)167, (short)168, (short)169, (short)170,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)0, (short)172, (short)150, (short)0,
(short)151, (short)0, (short)152, (short)459, (short)173, (short)153, (short)0,
(short)174, (short)175, (short)176, (short)0, (short)0, (short)154, (short)155,
(short)177, (short)178, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)0, (short)163, (short)164, (short)0,
(short)165, (short)166, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)167, (short)168, (short)169, (short)170, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171, (short)511,
(short)0, (short)0, (short)172, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)0, (short)173, (short)153, (short)0, (short)174, (short)175,
(short)176, (short)0, (short)0, (short)154, (short)155, (short)177, (short)178,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)0, (short)163, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167, (short)168,
(short)169, (short)170, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)171, (short)543, (short)0, (short)0,
(short)172, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)173, (short)153, (short)0, (short)174, (short)175, (short)176, (short)0,
(short)0, (short)154, (short)155, (short)177, (short)178, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)0,
(short)163, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)167, (short)168, (short)169,
(short)170, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)171, (short)0, (short)0, (short)0, (short)172, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)585, (short)173, (short)153,
(short)0, (short)174, (short)175, (short)176, (short)0, (short)0, (short)154,
(short)155, (short)177, (short)178, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)0, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)167, (short)168, (short)169, (short)170, (short)0,
(short)31, (short)0, (short)0, (short)623, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)0, (short)172, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)173, (short)153, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)154, (short)155, (short)177,
(short)178, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)0, (short)163, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167,
(short)168, (short)169, (short)170, (short)0, (short)31, (short)0, (short)0,
(short)625, (short)0, (short)0, (short)0, (short)171, (short)0, (short)0,
(short)0, (short)172, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)173, (short)153, (short)0, (short)174, (short)175, (short)176,
(short)0, (short)0, (short)154, (short)155, (short)177, (short)178, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)0, (short)163, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)167, (short)168,
(short)169, (short)170, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)171, (short)669, (short)0, (short)0,
(short)172, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)173, (short)153, (short)0, (short)174, (short)175, (short)176, (short)0,
(short)0, (short)154, (short)155, (short)177, (short)178, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)0,
(short)163, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)167, (short)168, (short)169,
(short)170, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)171, (short)672, (short)0, (short)0, (short)172, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)173, (short)153,
(short)0, (short)174, (short)175, (short)176, (short)0, (short)0, (short)154,
(short)155, (short)177, (short)178, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)0, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)167, (short)168, (short)169, (short)170, (short)0,
(short)31, (short)0, (short)0, (short)675, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)0, (short)172, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)173, (short)153, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)154, (short)155, (short)177,
(short)178, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)0, (short)163, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167,
(short)168, (short)169, (short)170, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)171, (short)698, (short)0,
(short)0, (short)172, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)173, (short)153, (short)0, (short)174, (short)175, (short)176,
(short)0, (short)0, (short)154, (short)155, (short)177, (short)178, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)0, (short)163, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)167, (short)168,
(short)169, (short)170, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)171, (short)702, (short)0, (short)0,
(short)172, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)173, (short)153, (short)0, (short)174, (short)175, (short)176, (short)0,
(short)0, (short)154, (short)155, (short)177, (short)178, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)0,
(short)163, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)167, (short)168, (short)169,
(short)170, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)171, (short)704, (short)0, (short)0, (short)172, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)173, (short)153,
(short)0, (short)174, (short)175, (short)176, (short)0, (short)0, (short)154,
(short)155, (short)177, (short)178, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)0, (short)163, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)167, (short)168, (short)169, (short)170, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171,
(short)727, (short)0, (short)0, (short)172, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)173, (short)153, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)154, (short)155, (short)177,
(short)178, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)0, (short)163, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167,
(short)168, (short)169, (short)170, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)0,
(short)172, (short)150, (short)0, (short)151, (short)0, (short)0, (short)0,
(short)173, (short)0, (short)0, (short)174, (short)175, (short)176, (short)0,
(short)0, (short)154, (short)155, (short)177, (short)178, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)0,
(short)0, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)167, (short)168, (short)169,
(short)170, (short)0, (short)31, (short)0, (short)0, (short)0, (short)306,
(short)0, (short)0, (short)171, (short)0, (short)0, (short)0, (short)172,
(short)150, (short)0, (short)151, (short)0, (short)0, (short)0, (short)173,
(short)0, (short)0, (short)174, (short)175, (short)176, (short)0, (short)0,
(short)154, (short)155, (short)177, (short)178, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)167, (short)168, (short)169, (short)170,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)600, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)0, (short)172, (short)150, (short)0,
(short)151, (short)0, (short)0, (short)0, (short)173, (short)0, (short)0,
(short)174, (short)175, (short)176, (short)0, (short)0, (short)154, (short)155,
(short)177, (short)178, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)0, (short)0, (short)164, (short)0,
(short)165, (short)166, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)167, (short)168, (short)169, (short)170, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)302, (short)0,
(short)0, (short)0, (short)172, (short)150, (short)0, (short)151, (short)0,
(short)0, (short)0, (short)173, (short)0, (short)0, (short)174, (short)175,
(short)176, (short)0, (short)0, (short)154, (short)155, (short)177, (short)178,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)0, (short)0, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)167, (short)168,
(short)169, (short)170, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)314, (short)0, (short)0, (short)0, (short)172,
(short)150, (short)0, (short)151, (short)0, (short)0, (short)0, (short)173,
(short)0, (short)0, (short)174, (short)175, (short)176, (short)0, (short)0,
(short)154, (short)155, (short)177, (short)178, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)167, (short)168, (short)169, (short)170,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)0, (short)172, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)173, (short)0, (short)0, (short)174,
(short)175, (short)176, (short)0, (short)0, (short)0, (short)0, (short)177,
(short)178}; static short Cyc_yycheck[ 4273u]={ (short)54, (short)86, (short)206,
(short)81, (short)54, (short)83, (short)153, (short)61, (short)86, (short)54,
(short)78, (short)153, (short)22, (short)54, (short)116, (short)152, (short)55,
(short)365, (short)218, (short)40, (short)115, (short)81, (short)56, (short)249,
(short)301, (short)103, (short)435, (short)415, (short)171, (short)419, (short)279,
(short)97, (short)441, (short)0, (short)102, (short)101, (short)104, (short)115,
(short)116, (short)666, (short)108, (short)5, (short)6, (short)74, (short)98,
(short)99, (short)100, (short)101, (short)98, (short)17, (short)69, (short)70,
(short)87, (short)98, (short)21, (short)22, (short)110, (short)24, (short)112,
(short)69, (short)110, (short)88, (short)112, (short)451, (short)74, (short)455,
(short)33, (short)112, (short)103, (short)90, (short)91, (short)92, (short)93,
(short)40, (short)98, (short)86, (short)73, (short)101, (short)45, (short)106,
(short)119, (short)138, (short)106, (short)114, (short)51, (short)90, (short)53,
(short)106, (short)107, (short)56, (short)147, (short)317, (short)103, (short)171,
(short)46, (short)47, (short)48, (short)17, (short)18, (short)19, (short)226,
(short)122, (short)69, (short)730, (short)115, (short)107, (short)232, (short)74,
(short)234, (short)235, (short)356, (short)108, (short)88, (short)115, (short)741,
(short)361, (short)525, (short)33, (short)94, (short)746, (short)88, (short)748,
(short)93, (short)90, (short)91, (short)92, (short)93, (short)173, (short)174,
(short)45, (short)176, (short)288, (short)81, (short)104, (short)83, (short)99,
(short)214, (short)86, (short)68, (short)96, (short)107, (short)298, (short)103,
(short)420, (short)68, (short)88, (short)113, (short)68, (short)194, (short)88,
(short)111, (short)219, (short)95, (short)207, (short)103, (short)122, (short)402,
(short)207, (short)125, (short)302, (short)127, (short)101, (short)207, (short)138,
(short)106, (short)555, (short)115, (short)116, (short)120, (short)86, (short)110,
(short)314, (short)147, (short)103, (short)64, (short)65, (short)66, (short)107,
(short)100, (short)103, (short)147, (short)98, (short)103, (short)107, (short)136,
(short)137, (short)107, (short)109, (short)110, (short)267, (short)377, (short)581,
(short)112, (short)113, (short)114, (short)249, (short)113, (short)113, (short)385,
(short)249, (short)87, (short)88, (short)88, (short)214, (short)112, (short)87,
(short)284, (short)285, (short)286, (short)95, (short)267, (short)101, (short)120,
(short)121, (short)106, (short)88, (short)294, (short)109, (short)296, (short)109,
(short)110, (short)94, (short)171, (short)301, (short)302, (short)88, (short)106,
(short)88, (short)101, (short)286, (short)117, (short)147, (short)95, (short)94,
(short)294, (short)88, (short)314, (short)110, (short)205, (short)296, (short)101,
(short)94, (short)93, (short)380, (short)97, (short)97, (short)98, (short)98,
(short)101, (short)216, (short)450, (short)290, (short)600, (short)220, (short)642,
(short)98, (short)310, (short)101, (short)101, (short)112, (short)113, (short)114,
(short)97, (short)317, (short)298, (short)214, (short)88, (short)317, (short)103,
(short)279, (short)474, (short)93, (short)94, (short)284, (short)285, (short)286,
(short)243, (short)98, (short)96, (short)405, (short)93, (short)407, (short)104,
(short)101, (short)98, (short)296, (short)99, (short)97, (short)106, (short)205,
(short)301, (short)98, (short)110, (short)103, (short)98, (short)341, (short)342,
(short)343, (short)435, (short)377, (short)216, (short)96, (short)101, (short)97,
(short)441, (short)101, (short)101, (short)385, (short)88, (short)103, (short)97,
(short)106, (short)279, (short)280, (short)94, (short)110, (short)103, (short)103,
(short)88, (short)206, (short)388, (short)101, (short)108, (short)408, (short)573,
(short)97, (short)98, (short)243, (short)108, (short)296, (short)408, (short)298,
(short)112, (short)113, (short)114, (short)284, (short)285, (short)286, (short)102,
(short)103, (short)394, (short)590, (short)420, (short)310, (short)394, (short)294,
(short)89, (short)296, (short)369, (short)427, (short)371, (short)551, (short)301,
(short)302, (short)103, (short)409, (short)97, (short)93, (short)133, (short)409,
(short)609, (short)463, (short)103, (short)99, (short)409, (short)314, (short)377,
(short)462, (short)409, (short)88, (short)427, (short)88, (short)99, (short)97,
(short)385, (short)94, (short)103, (short)94, (short)95, (short)103, (short)296,
(short)98, (short)101, (short)97, (short)462, (short)463, (short)590, (short)97,
(short)525, (short)103, (short)102, (short)110, (short)97, (short)112, (short)113,
(short)114, (short)171, (short)408, (short)103, (short)101, (short)287, (short)101,
(short)369, (short)100, (short)371, (short)609, (short)106, (short)428, (short)110,
(short)420, (short)112, (short)113, (short)114, (short)103, (short)97, (short)517,
(short)103, (short)97, (short)108, (short)306, (short)103, (short)108, (short)461,
(short)103, (short)103, (short)99, (short)313, (short)437, (short)377, (short)103,
(short)103, (short)99, (short)442, (short)320, (short)107, (short)103, (short)385,
(short)483, (short)87, (short)517, (short)99, (short)97, (short)691, (short)90,
(short)103, (short)71, (short)72, (short)101, (short)652, (short)596, (short)102,
(short)462, (short)67, (short)68, (short)596, (short)88, (short)231, (short)115,
(short)116, (short)408, (short)115, (short)428, (short)333, (short)334, (short)335,
(short)336, (short)433, (short)604, (short)355, (short)356, (short)437, (short)420,
(short)102, (short)103, (short)361, (short)442, (short)5, (short)6, (short)365,
(short)102, (short)103, (short)101, (short)708, (short)88, (short)102, (short)103,
(short)73, (short)691, (short)101, (short)94, (short)102, (short)103, (short)101,
(short)619, (short)548, (short)621, (short)101, (short)101, (short)548, (short)102,
(short)103, (short)102, (short)103, (short)548, (short)730, (short)89, (short)90,
(short)112, (short)113, (short)114, (short)477, (short)22, (short)23, (short)462,
(short)463, (short)402, (short)101, (short)595, (short)617, (short)618, (short)746,
(short)408, (short)102, (short)103, (short)102, (short)103, (short)413, (short)302,
(short)102, (short)103, (short)102, (short)103, (short)102, (short)103, (short)309,
(short)102, (short)103, (short)312, (short)101, (short)314, (short)594, (short)102,
(short)103, (short)669, (short)594, (short)99, (short)672, (short)118, (short)616,
(short)594, (short)119, (short)326, (short)102, (short)103, (short)101, (short)681,
(short)99, (short)524, (short)331, (short)332, (short)117, (short)477, (short)99,
(short)573, (short)531, (short)339, (short)340, (short)693, (short)517, (short)337,
(short)338, (short)697, (short)698, (short)99, (short)98, (short)701, (short)702,
(short)464, (short)704, (short)99, (short)98, (short)88, (short)98, (short)97,
(short)595, (short)108, (short)101, (short)474, (short)363, (short)101, (short)111,
(short)101, (short)101, (short)97, (short)100, (short)721, (short)101, (short)99,
(short)724, (short)102, (short)726, (short)727, (short)102, (short)99, (short)103,
(short)99, (short)573, (short)692, (short)524, (short)100, (short)102, (short)689,
(short)99, (short)739, (short)692, (short)390, (short)391, (short)392, (short)37,
(short)394, (short)75, (short)76, (short)77, (short)78, (short)79, (short)80,
(short)81, (short)82, (short)83, (short)84, (short)103, (short)74, (short)35,
(short)150, (short)102, (short)152, (short)153, (short)106, (short)97, (short)103,
(short)28, (short)108, (short)41, (short)97, (short)93, (short)100, (short)595,
(short)87, (short)165, (short)166, (short)102, (short)101, (short)115, (short)88,
(short)708, (short)102, (short)173, (short)174, (short)708, (short)176, (short)99,
(short)103, (short)103, (short)93, (short)102, (short)102, (short)37, (short)677,
(short)102, (short)88, (short)89, (short)102, (short)91, (short)92, (short)88,
(short)94, (short)730, (short)194, (short)689, (short)111, (short)730, (short)692,
(short)101, (short)103, (short)102, (short)104, (short)105, (short)102, (short)101,
(short)99, (short)109, (short)110, (short)746, (short)112, (short)102, (short)102,
(short)746, (short)116, (short)117, (short)103, (short)37, (short)102, (short)99,
(short)590, (short)99, (short)99, (short)98, (short)97, (short)101, (short)107,
(short)677, (short)102, (short)99, (short)600, (short)103, (short)111, (short)603,
(short)88, (short)89, (short)37, (short)91, (short)92, (short)609, (short)94,
(short)102, (short)102, (short)102, (short)111, (short)99, (short)98, (short)101,
(short)106, (short)35, (short)104, (short)105, (short)102, (short)98, (short)97,
(short)109, (short)110, (short)689, (short)112, (short)88, (short)692, (short)102,
(short)116, (short)117, (short)102, (short)102, (short)102, (short)101, (short)88,
(short)89, (short)101, (short)91, (short)92, (short)26, (short)94, (short)108,
(short)99, (short)102, (short)99, (short)537, (short)538, (short)101, (short)652,
(short)97, (short)104, (short)105, (short)102, (short)88, (short)89, (short)102,
(short)91, (short)92, (short)112, (short)94, (short)0, (short)0, (short)116,
(short)117, (short)563, (short)310, (short)101, (short)205, (short)433, (short)104,
(short)105, (short)214, (short)524, (short)57, (short)292, (short)115, (short)443,
(short)112, (short)280, (short)573, (short)327, (short)116, (short)117, (short)325,
(short)328, - (short)1, - (short)1, (short)691, - (short)1, - (short)1, (short)329,
(short)325, - (short)1, (short)327, (short)328, (short)329, (short)330, (short)331,
(short)332, (short)333, (short)334, (short)335, (short)336, (short)337, (short)338,
(short)339, (short)340, (short)341, (short)342, (short)343, - (short)1, -
(short)1, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, (short)330, - (short)1, - (short)1, (short)622, (short)623,
- (short)1, (short)625, - (short)1, (short)627, - (short)1, - (short)1, -
(short)1, - (short)1, (short)35, (short)36, (short)37, - (short)1, (short)39, -
(short)1, (short)380, (short)42, - (short)1, - (short)1, - (short)1, - (short)1,
(short)47, (short)48, (short)49, (short)50, - (short)1, (short)649, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, (short)60, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)671,
- (short)1, - (short)1, (short)674, (short)675, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, (short)93, (short)94,
(short)95, (short)435, - (short)1, (short)98, - (short)1, - (short)1, (short)101,
(short)441, - (short)1, (short)104, (short)105, - (short)1, - (short)1, -
(short)1, (short)706, - (short)1, - (short)1, (short)112, - (short)1, (short)711,
(short)115, (short)116, (short)117, - (short)1, - (short)1, - (short)1, -
(short)1, (short)122, (short)123, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)729, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)483, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)525,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)22, (short)23, (short)24,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, -
(short)1, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59,
(short)60, (short)61, (short)62, (short)63, - (short)1, (short)65, (short)66, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)596,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)604, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, (short)93,
(short)94, (short)95, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, - (short)1, - (short)1, (short)104, (short)105, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, -
(short)1, - (short)1, (short)122, (short)123, (short)3, (short)4, (short)5,
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
(short)101, - (short)1, - (short)1, - (short)1, (short)105, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, -
(short)1, - (short)1, (short)122, (short)123, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, (short)36, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)60, (short)47, (short)48, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)88, - (short)1, - (short)1, - (short)1, - (short)1,
(short)93, - (short)1, (short)95, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, - (short)1, (short)102, - (short)1, (short)104, (short)105, -
(short)1, (short)93, - (short)1, (short)95, - (short)1, (short)97, - (short)1, -
(short)1, - (short)1, - (short)1, (short)102, - (short)1, (short)104, (short)105,
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
base; struct Cyc_Yyltype* last_plus_one; } ; struct _tuple19{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate; int yyn= 0;
int yyerrstatus; int yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({
short* _temp4198=({ unsigned int _temp4195= 200u; short* _temp4196=( short*)
GC_malloc_atomic( sizeof( short) * _temp4195);{ unsigned int _temp4197=
_temp4195; unsigned int i; for( i= 0; i < _temp4197; i ++){ _temp4196[ i]=
(short)0;}}; _temp4196;}); struct _tagged_ptr1 _temp4199; _temp4199.curr=
_temp4198; _temp4199.base= _temp4198; _temp4199.last_plus_one= _temp4198 + 200;
_temp4199;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=({ void** _temp4193=({
unsigned int _temp4190= 200u; void** _temp4191=( void**) GC_malloc( sizeof( void*)
* _temp4190);{ unsigned int _temp4192= _temp4190; unsigned int i; for( i= 0; i <
_temp4192; i ++){ _temp4191[ i]= Cyc_yylval;}}; _temp4191;}); struct
_tagged_ptr2 _temp4194; _temp4194.curr= _temp4193; _temp4194.base= _temp4193;
_temp4194.last_plus_one= _temp4193 + 200; _temp4194;}); int yylsp_offset; struct
_tagged_ptr3 yyls=({ struct Cyc_Yyltype* _temp4188=({ unsigned int _temp4185=
200u; struct Cyc_Yyltype* _temp4186=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp4185);{ unsigned int _temp4187= _temp4185;
unsigned int i; for( i= 0; i < _temp4187; i ++){ _temp4186[ i]= Cyc_yynewloc();}};
_temp4186;}); struct _tagged_ptr3 _temp4189; _temp4189.curr= _temp4188;
_temp4189.base= _temp4188; _temp4189.last_plus_one= _temp4188 + 200; _temp4189;});
int yystacksize= 200; void* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), ++ yyssp_offset)=(
short) yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){
Cyc_yyerror(({ char* _temp2232="parser stack overflow"; struct _tagged_string
_temp2233; _temp2233.curr= _temp2232; _temp2233.base= _temp2232; _temp2233.last_plus_one=
_temp2232 + 22; _temp2233;}));( void) _throw(( void*) Cyc_Yystack_overflow);}
yystacksize *= 2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct
_tagged_ptr1 yyss1=({ unsigned int _temp2242=( unsigned int) yystacksize; short*
_temp2243=( short*) GC_malloc_atomic( sizeof( short) * _temp2242); struct
_tagged_ptr1 _temp2245={ _temp2243, _temp2243, _temp2243 + _temp2242};{
unsigned int _temp2244= _temp2242; unsigned int i; for( i= 0; i < _temp2244; i
++){ _temp2243[ i]= (short)0;}}; _temp2245;}); struct _tagged_ptr2 yyvs1=({
unsigned int _temp2238=( unsigned int) yystacksize; void** _temp2239=( void**)
GC_malloc( sizeof( void*) * _temp2238); struct _tagged_ptr2 _temp2241={
_temp2239, _temp2239, _temp2239 + _temp2238};{ unsigned int _temp2240= _temp2238;
unsigned int i; for( i= 0; i < _temp2240; i ++){ _temp2239[ i]= Cyc_yylval;}};
_temp2241;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2234=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2235=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2234); struct _tagged_ptr3 _temp2237={
_temp2235, _temp2235, _temp2235 + _temp2234};{ unsigned int _temp2236= _temp2234;
unsigned int i; for( i= 0; i < _temp2236; i ++){ _temp2235[ i]= Cyc_yynewloc();}};
_temp2237;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*(( short*(*)( struct
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
754u, yystate)]; if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar == - 2){
Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar= 0;}
else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int) Cyc_yytranslate[
_check_known_subscript_notnull( 352u, Cyc_yychar)]: 232;} yyn += yychar1; if((
yyn < 0? 1: yyn > 4272)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4273u,
yyn)] != yychar1){ goto yydefault;} yyn=( int) Cyc_yytable[
_check_known_subscript_notnull( 4273u, yyn)]; if( yyn < 0){ if( yyn == - 32768){
goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}}
if( yyn == 753){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; if( yyerrstatus
!= 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int) Cyc_yydefact[
_check_known_subscript_notnull( 754u, yystate)]; if( yyn == 0){ goto yyerrlab;}
yyreduce: yylen=( int) Cyc_yyr2[ _check_known_subscript_notnull( 395u, yyn)];
if( yylen > 0){ yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),( yyvsp_offset +
1) - yylen);} switch( yyn){ case 1: _LL2246: yyval=*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); break; case 2:
_LL2247: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2249=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2249[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2250; _temp2250.tag= Cyc_DeclList_tok; _temp2250.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2250;});
_temp2249;}); break; case 3: _LL2248: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2252=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2252[ 0]=({ struct Cyc_DeclList_tok_struct _temp2253; _temp2253.tag= Cyc_DeclList_tok;
_temp2253.f1=({ struct Cyc_List_List* _temp2254=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2254->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2255=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2255->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2256=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2256[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2257; _temp2257.tag= Cyc_Absyn_Using_d;
_temp2257.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2257.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2257;}); _temp2256;})); _temp2255->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2255;}); _temp2254->tl= 0; _temp2254;}); _temp2253;}); _temp2252;}); Cyc_Lex_leave_using();
break; case 4: _LL2251: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2259=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2259[ 0]=({ struct Cyc_DeclList_tok_struct _temp2260; _temp2260.tag= Cyc_DeclList_tok;
_temp2260.f1=({ struct Cyc_List_List* _temp2261=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2261->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2262=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2262->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2263=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2263[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2264; _temp2264.tag= Cyc_Absyn_Using_d;
_temp2264.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2264.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2264;}); _temp2263;})); _temp2262->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2262;}); _temp2261->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2261;});
_temp2260;}); _temp2259;}); break; case 5: _LL2258: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2266=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2266[ 0]=({ struct Cyc_DeclList_tok_struct _temp2267; _temp2267.tag= Cyc_DeclList_tok;
_temp2267.f1=({ struct Cyc_List_List* _temp2268=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2268->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2269=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2269->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2270=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2270[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2271; _temp2271.tag=
Cyc_Absyn_Namespace_d; _temp2271.f1=({ struct _tagged_string* _temp2272=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2272[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2272;}); _temp2271.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2271;}); _temp2270;})); _temp2269->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2269;}); _temp2268->tl= 0; _temp2268;}); _temp2267;}); _temp2266;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2265: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2274=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2274[ 0]=({ struct Cyc_DeclList_tok_struct _temp2275; _temp2275.tag= Cyc_DeclList_tok;
_temp2275.f1=({ struct Cyc_List_List* _temp2276=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2276->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2277=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2277->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2278=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2278[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2279; _temp2279.tag=
Cyc_Absyn_Namespace_d; _temp2279.f1=({ struct _tagged_string* _temp2280=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2280[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2280;}); _temp2279.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2279;}); _temp2278;})); _temp2277->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2277;}); _temp2276->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2276;});
_temp2275;}); _temp2274;}); break; case 7: _LL2273: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),({ char*
_temp2282="C"; struct _tagged_string _temp2283; _temp2283.curr= _temp2282;
_temp2283.base= _temp2282; _temp2283.last_plus_one= _temp2282 + 2; _temp2283;}))
!= 0){ Cyc_Parse_err(({ char* _temp2284="only extern \"C\" { ... } is allowed";
struct _tagged_string _temp2285; _temp2285.curr= _temp2284; _temp2285.base=
_temp2284; _temp2285.last_plus_one= _temp2284 + 35; _temp2285;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2286=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2286[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2287; _temp2287.tag= Cyc_DeclList_tok; _temp2287.f1=({ struct Cyc_List_List*
_temp2288=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2288->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2289=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2289->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2290=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2290[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2291; _temp2291.tag= Cyc_Absyn_ExternC_d;
_temp2291.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2291;}); _temp2290;})); _temp2289->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2289;}); _temp2288->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2288;}); _temp2287;}); _temp2286;}); break;
case 8: _LL2281: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2293=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2293[ 0]=({ struct Cyc_DeclList_tok_struct _temp2294; _temp2294.tag= Cyc_DeclList_tok;
_temp2294.f1= 0; _temp2294;}); _temp2293;}); break; case 9: _LL2292: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2296=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2296[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2297; _temp2297.tag= Cyc_DeclList_tok; _temp2297.f1=({ struct Cyc_List_List*
_temp2298=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2298->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2299=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2299[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2300; _temp2300.tag= Cyc_Absyn_Fn_d;
_temp2300.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2300;}); _temp2299;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2298->tl= 0; _temp2298;}); _temp2297;}); _temp2296;}); break; case 10:
_LL2295: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2301: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2303=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2303[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2304; _temp2304.tag= Cyc_FnDecl_tok; _temp2304.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2304;}); _temp2303;}); break; case 12: _LL2302:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2306=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2306[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2307; _temp2307.tag= Cyc_FnDecl_tok; _temp2307.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2308=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2308->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2308;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2307;}); _temp2306;}); break; case 13: _LL2305: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2310=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2310[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2311; _temp2311.tag= Cyc_FnDecl_tok;
_temp2311.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
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
_temp2311;}); _temp2310;}); break; case 14: _LL2309: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2313=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2313[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2314; _temp2314.tag= Cyc_FnDecl_tok;
_temp2314.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2315=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2315->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2315;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2314;});
_temp2313;}); break; case 15: _LL2312: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2317=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2317[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2318; _temp2318.tag= Cyc_FnDecl_tok;
_temp2318.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2319=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2319->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2319;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2318;});
_temp2317;}); break; case 16: _LL2316: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2321=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2321[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2322; _temp2322.tag= Cyc_FnDecl_tok;
_temp2322.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2323=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2323->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2323;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2322;});
_temp2321;}); break; case 17: _LL2320: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2324: Cyc_Lex_leave_using();
break; case 19: _LL2325: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2327=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2327[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2327;})); yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2326: Cyc_Lex_leave_namespace(); break; case
21: _LL2328: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2330=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2330[ 0]=({ struct Cyc_DeclList_tok_struct _temp2331; _temp2331.tag= Cyc_DeclList_tok;
_temp2331.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2331;}); _temp2330;}); break; case 22: _LL2329: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2333=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2333[ 0]=({ struct Cyc_DeclList_tok_struct _temp2334; _temp2334.tag= Cyc_DeclList_tok;
_temp2334.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2334;}); _temp2333;}); break; case 23: _LL2332: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2336=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2336[ 0]=({ struct Cyc_DeclList_tok_struct _temp2337; _temp2337.tag= Cyc_DeclList_tok;
_temp2337.f1=({ struct Cyc_List_List* _temp2338=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2338->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2338->tl= 0; _temp2338;}); _temp2337;});
_temp2336;}); break; case 24: _LL2335: { struct Cyc_List_List* _temp2340= 0;
goto _LL2341; _LL2341:{ struct Cyc_List_List* _temp2342= Cyc_yyget_IdList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); goto
_LL2343; _LL2343: for( 0; _temp2342 != 0; _temp2342=(( struct Cyc_List_List*)
_check_null( _temp2342))->tl){ struct _tagged_string* _temp2344=( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2342))->hd; goto
_LL2345; _LL2345: { struct _tuple1* _temp2349=({ struct _tuple1* _temp2346=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2346->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp2347=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2347[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp2348; _temp2348.tag= Cyc_Absyn_Rel_n; _temp2348.f1= 0; _temp2348;});
_temp2347;}); _temp2346->f2= _temp2344; _temp2346;}); goto _LL2350; _LL2350: {
struct Cyc_Absyn_Vardecl* _temp2351= Cyc_Absyn_new_vardecl( _temp2349, Cyc_Absyn_wildtyp(),
0); goto _LL2352; _LL2352: _temp2340=({ struct Cyc_List_List* _temp2353=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2353->hd=( void*)
_temp2351; _temp2353->tl= _temp2340; _temp2353;});}}}} _temp2340=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2340); yyval=( void*)({ struct
Cyc_DeclList_tok_struct* _temp2354=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp2354[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2355; _temp2355.tag= Cyc_DeclList_tok; _temp2355.f1=({ struct Cyc_List_List*
_temp2356=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2356->hd=( void*) Cyc_Absyn_letv_decl( _temp2340, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2356->tl= 0; _temp2356;}); _temp2355;}); _temp2354;}); break;} case 25:
_LL2339: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 26:
_LL2357: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2359=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2359[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2360; _temp2360.tag= Cyc_DeclList_tok; _temp2360.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2360;});
_temp2359;}); break; case 27: _LL2358: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2362=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2362[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2363; _temp2363.tag= Cyc_DeclSpec_tok;
_temp2363.f1=({ struct Cyc_Parse_Declaration_spec* _temp2364=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2364->sc=({ struct
Cyc_Core_Opt* _temp2365=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2365->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2365;}); _temp2364->tq= Cyc_Absyn_empty_tqual();
_temp2364->type_specs= 0; _temp2364->is_inline= 0; _temp2364->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2364;});
_temp2363;}); _temp2362;}); break; case 28: _LL2361: if(( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(({
char* _temp2367="Only one storage class is allowed in a declaration"; struct
_tagged_string _temp2368; _temp2368.curr= _temp2367; _temp2368.base= _temp2367;
_temp2368.last_plus_one= _temp2367 + 51; _temp2368;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2369=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2369[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2370; _temp2370.tag= Cyc_DeclSpec_tok; _temp2370.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2371=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2371->sc=({ struct Cyc_Core_Opt* _temp2372=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2372->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2372;});
_temp2371->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2371->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2371->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2371->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2371;}); _temp2370;}); _temp2369;}); break; case 29: _LL2366: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2374=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2374[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2375; _temp2375.tag= Cyc_DeclSpec_tok; _temp2375.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2376=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2376->sc= 0; _temp2376->tq= Cyc_Absyn_empty_tqual(); _temp2376->type_specs=({
struct Cyc_List_List* _temp2377=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2377->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2377->tl=
0; _temp2377;}); _temp2376->is_inline= 0; _temp2376->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2376;});
_temp2375;}); _temp2374;}); break; case 30: _LL2373: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2379=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2379[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2380; _temp2380.tag= Cyc_DeclSpec_tok;
_temp2380.f1=({ struct Cyc_Parse_Declaration_spec* _temp2381=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2381->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2381->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2381->type_specs=({ struct Cyc_List_List* _temp2382=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2382->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2382->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2382;}); _temp2381->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2381->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2381;}); _temp2380;}); _temp2379;}); break; case 31: _LL2378: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2384=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2384[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2385; _temp2385.tag= Cyc_DeclSpec_tok; _temp2385.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2386=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2386->sc= 0; _temp2386->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2386->type_specs= 0; _temp2386->is_inline=
0; _temp2386->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2386;}); _temp2385;}); _temp2384;}); break;
case 32: _LL2383: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2388=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2388[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2389; _temp2389.tag= Cyc_DeclSpec_tok;
_temp2389.f1=({ struct Cyc_Parse_Declaration_spec* _temp2390=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2390->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2390->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2390->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2390->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2390->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2390;}); _temp2389;}); _temp2388;}); break; case 33: _LL2387: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2392=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2392[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2393; _temp2393.tag= Cyc_DeclSpec_tok; _temp2393.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2394=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2394->sc= 0; _temp2394->tq= Cyc_Absyn_empty_tqual(); _temp2394->type_specs=
0; _temp2394->is_inline= 1; _temp2394->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2394;});
_temp2393;}); _temp2392;}); break; case 34: _LL2391: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2396=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2396[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2397; _temp2397.tag= Cyc_DeclSpec_tok;
_temp2397.f1=({ struct Cyc_Parse_Declaration_spec* _temp2398=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2398->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2398->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2398->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2398->is_inline= 1; _temp2398->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2398;}); _temp2397;}); _temp2396;}); break; case 35: _LL2395: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2400=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2400[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2401; _temp2401.tag= Cyc_StorageClass_tok;
_temp2401.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2401;}); _temp2400;});
break; case 36: _LL2399: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2403=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2403[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2404; _temp2404.tag=
Cyc_StorageClass_tok; _temp2404.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2404;}); _temp2403;}); break; case 37: _LL2402: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2406=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2406[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2407; _temp2407.tag=
Cyc_StorageClass_tok; _temp2407.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2407;}); _temp2406;}); break; case 38: _LL2405: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2409=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2409[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2410; _temp2410.tag=
Cyc_StorageClass_tok; _temp2410.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2410;}); _temp2409;}); break; case 39: _LL2408: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),({ char*
_temp2412="C"; struct _tagged_string _temp2413; _temp2413.curr= _temp2412;
_temp2413.base= _temp2412; _temp2413.last_plus_one= _temp2412 + 2; _temp2413;}))
!= 0){ Cyc_Parse_err(({ char* _temp2414="only extern or extern \"C\" is allowed";
struct _tagged_string _temp2415; _temp2415.curr= _temp2414; _temp2415.base=
_temp2414; _temp2415.last_plus_one= _temp2414 + 37; _temp2415;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2416=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2416[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2417; _temp2417.tag= Cyc_StorageClass_tok;
_temp2417.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2417;}); _temp2416;});
break; case 40: _LL2411: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2419=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2419[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2420; _temp2420.tag=
Cyc_StorageClass_tok; _temp2420.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2420;}); _temp2419;}); break; case 41: _LL2418: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2422=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2422[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2423; _temp2423.tag=
Cyc_StorageClass_tok; _temp2423.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2423;}); _temp2422;}); break; case 42: _LL2421: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2425=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2425[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2426; _temp2426.tag=
Cyc_AttributeList_tok; _temp2426.f1= 0; _temp2426;}); _temp2425;}); break; case
43: _LL2424: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 44: _LL2427: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2429=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2429[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2430; _temp2430.tag=
Cyc_AttributeList_tok; _temp2430.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2430;}); _temp2429;}); break;
case 45: _LL2428: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2432=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2432[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2433; _temp2433.tag=
Cyc_AttributeList_tok; _temp2433.f1=({ struct Cyc_List_List* _temp2434=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2434->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2434->tl= 0; _temp2434;}); _temp2433;}); _temp2432;}); break; case 46:
_LL2431: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2436=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2436[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2437; _temp2437.tag=
Cyc_AttributeList_tok; _temp2437.f1=({ struct Cyc_List_List* _temp2438=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2438->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2438->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2438;}); _temp2437;}); _temp2436;}); break; case 47:
_LL2435: { struct _tagged_string _temp2440= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2441; _LL2441: if((((({ struct
_tagged_string _temp2442= _temp2440;( unsigned int)( _temp2442.last_plus_one -
_temp2442.curr);}) > 4?*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp2440, sizeof( char), 0) =='_': 0)?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2440, sizeof( char), 1) =='_': 0)?*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2440, sizeof( char),( int)(({ struct _tagged_string _temp2443= _temp2440;(
unsigned int)( _temp2443.last_plus_one - _temp2443.curr);}) - 2)) =='_': 0)?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2440, sizeof( char),( int)(({ struct
_tagged_string _temp2444= _temp2440;( unsigned int)( _temp2444.last_plus_one -
_temp2444.curr);}) - 3)) =='_': 0){ _temp2440= Cyc_String_substring( _temp2440,
2,({ struct _tagged_string _temp2445= _temp2440;( unsigned int)( _temp2445.last_plus_one
- _temp2445.curr);}) - 5);}{ int i= 0; for( 0; i < 15u; ++ i){ if( Cyc_String_strcmp(
_temp2440,( Cyc_Parse_att_map[ _check_known_subscript_notnull( 15u, i)]).f1) ==
0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2446=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2446[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2447; _temp2447.tag= Cyc_Attribute_tok; _temp2447.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 15u, i)]).f2;
_temp2447;}); _temp2446;}); break;}} if( i == 15u){ Cyc_Parse_err(({ char*
_temp2448="unrecognized attribute"; struct _tagged_string _temp2449; _temp2449.curr=
_temp2448; _temp2449.base= _temp2448; _temp2449.last_plus_one= _temp2448 + 23;
_temp2449;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp2450=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2450[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2451; _temp2451.tag= Cyc_Attribute_tok; _temp2451.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp2451;}); _temp2450;});} break;}} case
48: _LL2439: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2453=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2453[ 0]=({ struct Cyc_Attribute_tok_struct _temp2454; _temp2454.tag= Cyc_Attribute_tok;
_temp2454.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2454;}); _temp2453;});
break; case 49: _LL2452: { struct _tagged_string _temp2456= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); goto
_LL2457; _LL2457: { struct _tuple16 _temp2460; int _temp2461; void* _temp2463;
struct _tuple16* _temp2458= Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2460=* _temp2458; _LL2464: _temp2463= _temp2460.f1;
goto _LL2462; _LL2462: _temp2461= _temp2460.f2; goto _LL2459; _LL2459: { void* a;
if( Cyc_String_zstrcmp( _temp2456,({ char* _temp2465="regparm"; struct
_tagged_string _temp2466; _temp2466.curr= _temp2465; _temp2466.base= _temp2465;
_temp2466.last_plus_one= _temp2465 + 8; _temp2466;})) == 0? 1: Cyc_String_zstrcmp(
_temp2456,({ char* _temp2467="__regparm__"; struct _tagged_string _temp2468;
_temp2468.curr= _temp2467; _temp2468.base= _temp2467; _temp2468.last_plus_one=
_temp2467 + 12; _temp2468;})) == 0){ if( _temp2461 <= 0? 1: _temp2461 > 3){ Cyc_Parse_err(({
char* _temp2469="regparm requires value between 1 and 3"; struct _tagged_string
_temp2470; _temp2470.curr= _temp2469; _temp2470.base= _temp2469; _temp2470.last_plus_one=
_temp2469 + 39; _temp2470;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));} a=( void*)({
struct Cyc_Absyn_Regparm_att_struct* _temp2471=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2471[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2472; _temp2472.tag= Cyc_Absyn_Regparm_att;
_temp2472.f1= _temp2461; _temp2472;}); _temp2471;});} else{ if( Cyc_String_zstrcmp(
_temp2456,({ char* _temp2473="aligned"; struct _tagged_string _temp2474;
_temp2474.curr= _temp2473; _temp2474.base= _temp2473; _temp2474.last_plus_one=
_temp2473 + 8; _temp2474;})) == 0? 1: Cyc_String_zstrcmp( _temp2456,({ char*
_temp2475="__aligned__"; struct _tagged_string _temp2476; _temp2476.curr=
_temp2475; _temp2476.base= _temp2475; _temp2476.last_plus_one= _temp2475 + 12;
_temp2476;})) == 0){ if( _temp2461 < 0){ Cyc_Parse_err(({ char* _temp2477="aligned requires positive power of two";
struct _tagged_string _temp2478; _temp2478.curr= _temp2477; _temp2478.base=
_temp2477; _temp2478.last_plus_one= _temp2477 + 39; _temp2478;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2461; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(({ char* _temp2479="aligned requires positive power of two";
struct _tagged_string _temp2480; _temp2480.curr= _temp2479; _temp2480.base=
_temp2479; _temp2480.last_plus_one= _temp2479 + 39; _temp2480;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2481=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2481[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2482; _temp2482.tag= Cyc_Absyn_Aligned_att;
_temp2482.f1= _temp2461; _temp2482;}); _temp2481;});}} else{ Cyc_Parse_err(({
char* _temp2483="unrecognized attribute"; struct _tagged_string _temp2484;
_temp2484.curr= _temp2483; _temp2484.base= _temp2483; _temp2484.last_plus_one=
_temp2483 + 23; _temp2484;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line)); a=( void*)
Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2485=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2485[ 0]=({ struct Cyc_Attribute_tok_struct _temp2486; _temp2486.tag= Cyc_Attribute_tok;
_temp2486.f1=( void*) a; _temp2486;}); _temp2485;}); break;}}} case 50: _LL2455: {
struct _tagged_string _temp2488= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2489; _LL2489: { struct
_tagged_string _temp2490= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2491; _LL2491: { void* a; if( Cyc_String_zstrcmp(
_temp2488,({ char* _temp2492="section"; struct _tagged_string _temp2493;
_temp2493.curr= _temp2492; _temp2493.base= _temp2492; _temp2493.last_plus_one=
_temp2492 + 8; _temp2493;})) == 0? 1: Cyc_String_zstrcmp( _temp2488,({ char*
_temp2494="__section__"; struct _tagged_string _temp2495; _temp2495.curr=
_temp2494; _temp2495.base= _temp2494; _temp2495.last_plus_one= _temp2494 + 12;
_temp2495;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct* _temp2496=(
struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2496[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2497; _temp2497.tag=
Cyc_Absyn_Section_att; _temp2497.f1= _temp2490; _temp2497;}); _temp2496;});}
else{ Cyc_Parse_err(({ char* _temp2498="unrecognized attribute"; struct
_tagged_string _temp2499; _temp2499.curr= _temp2498; _temp2499.base= _temp2498;
_temp2499.last_plus_one= _temp2498 + 23; _temp2499;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2500=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2500[ 0]=({ struct Cyc_Attribute_tok_struct _temp2501; _temp2501.tag= Cyc_Attribute_tok;
_temp2501.f1=( void*) a; _temp2501;}); _temp2500;}); break;}}} case 51: _LL2487:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2503=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2503[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2504; _temp2504.tag= Cyc_TypeSpecifier_tok;
_temp2504.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2504;}); _temp2503;}); break; case 52: _LL2502: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2506=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2506[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2507; _temp2507.tag=
Cyc_TypeSpecifier_tok; _temp2507.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2507;});
_temp2506;}); break; case 53: _LL2505: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2509=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2509[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2510; _temp2510.tag=
Cyc_TypeSpecifier_tok; _temp2510.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2510;}); _temp2509;}); break; case 54: _LL2508:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2512=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2512[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2513; _temp2513.tag= Cyc_TypeSpecifier_tok;
_temp2513.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2514=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2514[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2515; _temp2515.tag=
Cyc_Parse_Short_spec; _temp2515.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2515;});
_temp2514;})); _temp2513;}); _temp2512;}); break; case 55: _LL2511: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2517=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2517[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2518; _temp2518.tag= Cyc_TypeSpecifier_tok;
_temp2518.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2518;}); _temp2517;}); break; case 56: _LL2516: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2520=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2520[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2521; _temp2521.tag=
Cyc_TypeSpecifier_tok; _temp2521.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2522=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2522[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2523; _temp2523.tag= Cyc_Parse_Long_spec;
_temp2523.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2523;});
_temp2522;})); _temp2521;}); _temp2520;}); break; case 57: _LL2519: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2525=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2525[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2526; _temp2526.tag= Cyc_TypeSpecifier_tok;
_temp2526.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2526;}); _temp2525;}); break; case 58: _LL2524: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2528=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2528[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2529; _temp2529.tag=
Cyc_TypeSpecifier_tok; _temp2529.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2529;}); _temp2528;}); break; case 59: _LL2527:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2531=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2531[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2532; _temp2532.tag= Cyc_TypeSpecifier_tok;
_temp2532.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2533=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2533[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2534; _temp2534.tag=
Cyc_Parse_Signed_spec; _temp2534.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2534;});
_temp2533;})); _temp2532;}); _temp2531;}); break; case 60: _LL2530: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2536=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2536[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2537; _temp2537.tag= Cyc_TypeSpecifier_tok;
_temp2537.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2538=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2538[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2539; _temp2539.tag= Cyc_Parse_Unsigned_spec; _temp2539.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2539;}); _temp2538;})); _temp2537;}); _temp2536;}); break; case 61: _LL2535:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2540: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2541: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 64:
_LL2542: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2544=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2544[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2545; _temp2545.tag=
Cyc_TypeSpecifier_tok; _temp2545.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2545;}); _temp2544;}); break; case 65: _LL2543: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2547=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2547[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2548; _temp2548.tag=
Cyc_TypeSpecifier_tok; _temp2548.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2548;}); _temp2547;}); break; case 66: _LL2546: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2550=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2550[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2551; _temp2551.tag=
Cyc_TypeSpecifier_tok; _temp2551.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2552=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2552[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2553; _temp2553.tag= Cyc_Absyn_TypedefType;
_temp2553.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2553.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2553.f3= 0; _temp2553;}); _temp2552;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2551;}); _temp2550;}); break; case 67: _LL2549: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2555=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2555[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2556; _temp2556.tag=
Cyc_TypeSpecifier_tok; _temp2556.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2557=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2557[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2558; _temp2558.tag= Cyc_Absyn_TupleType;
_temp2558.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2558;});
_temp2557;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2556;});
_temp2555;}); break; case 68: _LL2554: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2560=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2560[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2561; _temp2561.tag=
Cyc_TypeSpecifier_tok; _temp2561.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2562=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2562[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2563; _temp2563.tag= Cyc_Absyn_RgnHandleType;
_temp2563.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2563;}); _temp2562;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2561;}); _temp2560;}); break; case 69: _LL2559: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2565=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2565[ 0]=({ struct Cyc_Kind_tok_struct _temp2566; _temp2566.tag= Cyc_Kind_tok;
_temp2566.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2566;}); _temp2565;}); break; case 70: _LL2564: { struct _tagged_string*
_temp2570; void* _temp2572; struct _tuple1 _temp2568=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2573:
_temp2572= _temp2568.f1; goto _LL2571; _LL2571: _temp2570= _temp2568.f2; goto
_LL2569; _LL2569: if( _temp2572 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(({
char* _temp2574="bad kind in type specifier"; struct _tagged_string _temp2575;
_temp2575.curr= _temp2574; _temp2575.base= _temp2574; _temp2575.last_plus_one=
_temp2574 + 27; _temp2575;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Kind_tok_struct* _temp2576=( struct Cyc_Kind_tok_struct*) GC_malloc(
sizeof( struct Cyc_Kind_tok_struct)); _temp2576[ 0]=({ struct Cyc_Kind_tok_struct
_temp2577; _temp2577.tag= Cyc_Kind_tok; _temp2577.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2570, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2577;});
_temp2576;}); break;} case 71: _LL2567: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2579=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2579[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2580; _temp2580.tag= Cyc_TypeQual_tok;
_temp2580.f1=({ struct Cyc_Absyn_Tqual _temp2581; _temp2581.q_const= 1;
_temp2581.q_volatile= 0; _temp2581.q_restrict= 0; _temp2581;}); _temp2580;});
_temp2579;}); break; case 72: _LL2578: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2583=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2583[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2584; _temp2584.tag= Cyc_TypeQual_tok;
_temp2584.f1=({ struct Cyc_Absyn_Tqual _temp2585; _temp2585.q_const= 0;
_temp2585.q_volatile= 1; _temp2585.q_restrict= 0; _temp2585;}); _temp2584;});
_temp2583;}); break; case 73: _LL2582: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2587=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2587[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2588; _temp2588.tag= Cyc_TypeQual_tok;
_temp2588.f1=({ struct Cyc_Absyn_Tqual _temp2589; _temp2589.q_const= 0;
_temp2589.q_volatile= 0; _temp2589.q_restrict= 1; _temp2589;}); _temp2588;});
_temp2587;}); break; case 74: _LL2586: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2591=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2591[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2592; _temp2592.tag=
Cyc_TypeSpecifier_tok; _temp2592.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2593=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2593[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2594; _temp2594.tag= Cyc_Parse_Decl_spec;
_temp2594.f1=({ struct Cyc_Absyn_Decl* _temp2595=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2595->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2596=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2596[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2597; _temp2597.tag= Cyc_Absyn_Enum_d; _temp2597.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2598=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2598->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2598->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2598->fields=({
struct Cyc_Core_Opt* _temp2599=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2599->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2599;}); _temp2598;}); _temp2597;});
_temp2596;})); _temp2595->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2595;});
_temp2594;}); _temp2593;})); _temp2592;}); _temp2591;}); break; case 75: _LL2590:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2601=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2601[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2602; _temp2602.tag= Cyc_TypeSpecifier_tok;
_temp2602.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2603=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2603[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2604; _temp2604.tag= Cyc_Absyn_EnumType;
_temp2604.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2604.f2= 0; _temp2604;}); _temp2603;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2602;}); _temp2601;}); break; case 76: _LL2600: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2606=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2606[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2607; _temp2607.tag= Cyc_Enumfield_tok;
_temp2607.f1=({ struct Cyc_Absyn_Enumfield* _temp2608=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2608->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2608->tag=
0; _temp2608->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2608;});
_temp2607;}); _temp2606;}); break; case 77: _LL2605: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2610=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2610[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2611; _temp2611.tag= Cyc_Enumfield_tok;
_temp2611.f1=({ struct Cyc_Absyn_Enumfield* _temp2612=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2612->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2612->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2612->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2612;});
_temp2611;}); _temp2610;}); break; case 78: _LL2609: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2614=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2614[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2615; _temp2615.tag=
Cyc_EnumfieldList_tok; _temp2615.f1=({ struct Cyc_List_List* _temp2616=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2616->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2616->tl= 0; _temp2616;}); _temp2615;}); _temp2614;}); break; case 79:
_LL2613: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2618=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2618[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2619; _temp2619.tag=
Cyc_EnumfieldList_tok; _temp2619.f1=({ struct Cyc_List_List* _temp2620=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2620->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2620->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2620;}); _temp2619;}); _temp2618;}); break; case 80:
_LL2617: { void* t;{ void* _temp2622= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2624: if( _temp2622 ==( void*) Cyc_Parse_Struct_su){
goto _LL2625;} else{ goto _LL2626;} _LL2626: if( _temp2622 ==( void*) Cyc_Parse_Union_su){
goto _LL2627;} else{ goto _LL2623;} _LL2625: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2628=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2628[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2629; _temp2629.tag= Cyc_Absyn_AnonStructType; _temp2629.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2629;});
_temp2628;}); goto _LL2623; _LL2627: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2630=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2630[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2631; _temp2631.tag= Cyc_Absyn_AnonUnionType; _temp2631.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2631;});
_temp2630;}); goto _LL2623; _LL2623:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2632=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2632[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2633; _temp2633.tag=
Cyc_TypeSpecifier_tok; _temp2633.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2634=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2634[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2635; _temp2635.tag= Cyc_Parse_Type_spec;
_temp2635.f1=( void*) t; _temp2635.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2635;}); _temp2634;})); _temp2633;}); _temp2632;}); break;} case 81:
_LL2621: { struct Cyc_List_List* _temp2637=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2638; _LL2638: { struct Cyc_Absyn_Decl* d;{ void* _temp2639= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2641:
if( _temp2639 ==( void*) Cyc_Parse_Struct_su){ goto _LL2642;} else{ goto _LL2643;}
_LL2643: if( _temp2639 ==( void*) Cyc_Parse_Union_su){ goto _LL2644;} else{ goto
_LL2640;} _LL2642: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2645=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2645->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2645;}), _temp2637,({ struct Cyc_Core_Opt* _temp2646=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2646->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2646;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2640;
_LL2644: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2647=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2647->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2647;}), _temp2637,({ struct Cyc_Core_Opt* _temp2648=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2648->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2648;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2640;
_LL2640:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2649=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2649[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2650; _temp2650.tag=
Cyc_TypeSpecifier_tok; _temp2650.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2651=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2651[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2652; _temp2652.tag= Cyc_Parse_Decl_spec;
_temp2652.f1= d; _temp2652;}); _temp2651;})); _temp2650;}); _temp2649;}); break;}}
case 82: _LL2636: { struct Cyc_List_List* _temp2654=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2655; _LL2655: { struct Cyc_Absyn_Decl* d;{ void* _temp2656= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2658:
if( _temp2656 ==( void*) Cyc_Parse_Struct_su){ goto _LL2659;} else{ goto _LL2660;}
_LL2660: if( _temp2656 ==( void*) Cyc_Parse_Union_su){ goto _LL2661;} else{ goto
_LL2657;} _LL2659: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2662=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2662->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2662;}), _temp2654,({ struct Cyc_Core_Opt* _temp2663=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2663->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2663;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2657;
_LL2661: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2664=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2664->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2664;}), _temp2654,({ struct Cyc_Core_Opt* _temp2665=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2665->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2665;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2657;
_LL2657:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2666=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2666[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2667; _temp2667.tag=
Cyc_TypeSpecifier_tok; _temp2667.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2668=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2668[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2669; _temp2669.tag= Cyc_Parse_Decl_spec;
_temp2669.f1= d; _temp2669;}); _temp2668;})); _temp2667;}); _temp2666;}); break;}}
case 83: _LL2653:{ void* _temp2671= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2673: if( _temp2671 ==( void*) Cyc_Parse_Struct_su){
goto _LL2674;} else{ goto _LL2675;} _LL2675: if( _temp2671 ==( void*) Cyc_Parse_Union_su){
goto _LL2676;} else{ goto _LL2672;} _LL2674: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2677=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2677[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2678; _temp2678.tag=
Cyc_TypeSpecifier_tok; _temp2678.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2679=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2679[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2680; _temp2680.tag= Cyc_Absyn_StructType;
_temp2680.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2680.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2680.f3= 0;
_temp2680;}); _temp2679;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2678;});
_temp2677;}); goto _LL2672; _LL2676: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2681=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2681[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2682; _temp2682.tag=
Cyc_TypeSpecifier_tok; _temp2682.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2683=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2683[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2684; _temp2684.tag= Cyc_Absyn_UnionType;
_temp2684.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2684.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2684.f3= 0;
_temp2684;}); _temp2683;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2682;});
_temp2681;}); goto _LL2672; _LL2672:;} break; case 84: _LL2670:{ void* _temp2686=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2688: if( _temp2686 ==( void*) Cyc_Parse_Struct_su){ goto _LL2689;} else{
goto _LL2690;} _LL2690: if( _temp2686 ==( void*) Cyc_Parse_Union_su){ goto
_LL2691;} else{ goto _LL2687;} _LL2689: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2692=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2692[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2693; _temp2693.tag=
Cyc_TypeSpecifier_tok; _temp2693.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2694=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2694[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2695; _temp2695.tag= Cyc_Absyn_StructType;
_temp2695.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2695.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2695.f3= 0;
_temp2695;}); _temp2694;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2693;});
_temp2692;}); goto _LL2687; _LL2691: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2696=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2696[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2697; _temp2697.tag=
Cyc_TypeSpecifier_tok; _temp2697.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2698=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2698[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2699; _temp2699.tag= Cyc_Absyn_UnionType;
_temp2699.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2699.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2699.f3= 0;
_temp2699;}); _temp2698;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2697;});
_temp2696;}); goto _LL2687; _LL2687:;} break; case 85: _LL2685: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2701=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2701[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2702; _temp2702.tag= Cyc_TypeList_tok; _temp2702.f1= 0; _temp2702;});
_temp2701;}); break; case 86: _LL2700: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2704=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2704[ 0]=({ struct Cyc_TypeList_tok_struct _temp2705; _temp2705.tag= Cyc_TypeList_tok;
_temp2705.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2705;}); _temp2704;}); break; case 87: _LL2703:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2707=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2707[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2708; _temp2708.tag= Cyc_TypeList_tok; _temp2708.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2708;}); _temp2707;}); break;
case 88: _LL2706: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2710=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2710[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2711; _temp2711.tag=
Cyc_StructOrUnion_tok; _temp2711.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2711;}); _temp2710;}); break; case 89: _LL2709: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2713=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2713[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2714; _temp2714.tag=
Cyc_StructOrUnion_tok; _temp2714.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2714;}); _temp2713;}); break; case 90: _LL2712: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2716=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2716[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2717; _temp2717.tag= Cyc_StructFieldDeclList_tok; _temp2717.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2717;});
_temp2716;}); break; case 91: _LL2715: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2719=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2719[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2720; _temp2720.tag= Cyc_StructFieldDeclListList_tok; _temp2720.f1=({
struct Cyc_List_List* _temp2721=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2721->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2721->tl=
0; _temp2721;}); _temp2720;}); _temp2719;}); break; case 92: _LL2718: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2723=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2723[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2724; _temp2724.tag= Cyc_StructFieldDeclListList_tok;
_temp2724.f1=({ struct Cyc_List_List* _temp2725=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2725->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2725->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2725;}); _temp2724;}); _temp2723;}); break; case 93:
_LL2722: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2727=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2727[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2728; _temp2728.tag=
Cyc_InitDeclList_tok; _temp2728.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2728;}); _temp2727;}); break; case 94:
_LL2726: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2730=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2730[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2731; _temp2731.tag=
Cyc_InitDeclList_tok; _temp2731.f1=({ struct Cyc_List_List* _temp2732=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2732->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2732->tl= 0; _temp2732;}); _temp2731;}); _temp2730;}); break; case 95:
_LL2729: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2734=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2734[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2735; _temp2735.tag=
Cyc_InitDeclList_tok; _temp2735.f1=({ struct Cyc_List_List* _temp2736=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2736->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2736->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2736;}); _temp2735;}); _temp2734;}); break; case 96:
_LL2733: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2738=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2738[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2739; _temp2739.tag= Cyc_InitDecl_tok; _temp2739.f1=({ struct _tuple15*
_temp2740=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2740->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2740->f2= 0; _temp2740;}); _temp2739;}); _temp2738;}); break; case 97:
_LL2737: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2742=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2742[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2743; _temp2743.tag= Cyc_InitDecl_tok; _temp2743.f1=({ struct _tuple15*
_temp2744=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2744->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2744->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2744;}); _temp2743;}); _temp2742;}); break;
case 98: _LL2741: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2746=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2747; _LL2747: { void* _temp2748=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2749; _LL2749: { struct Cyc_List_List* _temp2752; struct Cyc_List_List*
_temp2754; struct _tuple0 _temp2750=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2755: _temp2754= _temp2750.f1; goto
_LL2753; _LL2753: _temp2752= _temp2750.f2; goto _LL2751; _LL2751: { struct Cyc_List_List*
_temp2756=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2748, _temp2754, _temp2746),
_temp2752); goto _LL2757; _LL2757: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2758=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2758[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2759; _temp2759.tag= Cyc_StructFieldDeclList_tok; _temp2759.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2756); _temp2759;}); _temp2758;}); break;}}}} case 99: _LL2745: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2761=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2761[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2762; _temp2762.tag= Cyc_QualSpecList_tok;
_temp2762.f1=({ struct _tuple18* _temp2763=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2763->f1= Cyc_Absyn_empty_tqual(); _temp2763->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2764=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2764->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2764->tl=
0; _temp2764;}); _temp2763->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2763;}); _temp2762;}); _temp2761;});
break; case 100: _LL2760: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2766=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2766[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2767; _temp2767.tag=
Cyc_QualSpecList_tok; _temp2767.f1=({ struct _tuple18* _temp2768=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2768->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2768->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2769=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2769->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2769->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2769;}); _temp2768->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2768;}); _temp2767;}); _temp2766;});
break; case 101: _LL2765: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2771=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2771[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2772; _temp2772.tag=
Cyc_QualSpecList_tok; _temp2772.f1=({ struct _tuple18* _temp2773=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2773->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2773->f2=
0; _temp2773->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2773;}); _temp2772;}); _temp2771;}); break; case 102:
_LL2770: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2775=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2775[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2776; _temp2776.tag=
Cyc_QualSpecList_tok; _temp2776.f1=({ struct _tuple18* _temp2777=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2777->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2777->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2777->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2777;});
_temp2776;}); _temp2775;}); break; case 103: _LL2774: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2779=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2779[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2780; _temp2780.tag= Cyc_DeclaratorExpoptList_tok;
_temp2780.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2780;}); _temp2779;}); break; case 104: _LL2778: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2782=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2782[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2783; _temp2783.tag= Cyc_DeclaratorExpoptList_tok;
_temp2783.f1=({ struct Cyc_List_List* _temp2784=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2784->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2784->tl=
0; _temp2784;}); _temp2783;}); _temp2782;}); break; case 105: _LL2781: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2786=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2786[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2787; _temp2787.tag= Cyc_DeclaratorExpoptList_tok;
_temp2787.f1=({ struct Cyc_List_List* _temp2788=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2788->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2788->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2788;}); _temp2787;}); _temp2786;}); break; case 106:
_LL2785: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2790=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2790[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2791; _temp2791.tag=
Cyc_DeclaratorExpopt_tok; _temp2791.f1=({ struct _tuple13* _temp2792=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2792->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2792->f2=
0; _temp2792;}); _temp2791;}); _temp2790;}); break; case 107: _LL2789: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2794=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2794[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2795; _temp2795.tag= Cyc_DeclaratorExpopt_tok;
_temp2795.f1=({ struct _tuple13* _temp2796=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2796->f1=({ struct Cyc_Parse_Declarator*
_temp2798=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2798->id=({ struct _tuple1* _temp2799=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2799->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2803=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2803[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2804; _temp2804.tag= Cyc_Absyn_Rel_n;
_temp2804.f1= 0; _temp2804;}); _temp2803;}); _temp2799->f2=({ struct
_tagged_string* _temp2800=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2800[ 0]=({ char* _temp2801=""; struct _tagged_string
_temp2802; _temp2802.curr= _temp2801; _temp2802.base= _temp2801; _temp2802.last_plus_one=
_temp2801 + 1; _temp2802;}); _temp2800;}); _temp2799;}); _temp2798->tms= 0;
_temp2798;}); _temp2796->f2=({ struct Cyc_Core_Opt* _temp2797=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2797->v=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2797;});
_temp2796;}); _temp2795;}); _temp2794;}); break; case 108: _LL2793: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2806=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2806[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2807; _temp2807.tag= Cyc_DeclaratorExpopt_tok;
_temp2807.f1=({ struct _tuple13* _temp2808=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2808->f1= Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2808->f2=({ struct Cyc_Core_Opt*
_temp2809=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2809->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2809;}); _temp2808;}); _temp2807;}); _temp2806;}); break;
case 109: _LL2805: { struct Cyc_List_List* _temp2811=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2812; _LL2812: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2813=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2813[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2814; _temp2814.tag=
Cyc_TypeSpecifier_tok; _temp2814.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2815=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2815[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2816; _temp2816.tag= Cyc_Parse_Decl_spec;
_temp2816.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2811,({
struct Cyc_Core_Opt* _temp2817=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2817->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2817;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2816;}); _temp2815;})); _temp2814;}); _temp2813;}); break;} case 110:
_LL2810: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2819=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
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
yyvsp_offset - 2)); _temp2826;}); _temp2825;}); _temp2824;})); _temp2823.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2823.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2823;}); _temp2822;});
_temp2821;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2820;});
_temp2819;}); break; case 111: _LL2818: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2828=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2828[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2829; _temp2829.tag=
Cyc_TypeSpecifier_tok; _temp2829.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2830=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2830[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2831; _temp2831.tag= Cyc_Absyn_TunionType;
_temp2831.f1=({ struct Cyc_Absyn_TunionInfo _temp2832; _temp2832.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2833=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2833[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2834; _temp2834.tag= Cyc_Absyn_UnknownTunion;
_temp2834.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2835; _temp2835.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2835.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2835;}); _temp2834;}); _temp2833;})); _temp2832.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2832.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2832;}); _temp2831;}); _temp2830;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2829;}); _temp2828;}); break; case 112: _LL2827: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2837=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2837[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2838; _temp2838.tag= Cyc_TypeSpecifier_tok;
_temp2838.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2839=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2839[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2840; _temp2840.tag= Cyc_Absyn_TunionFieldType; _temp2840.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2841; _temp2841.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2842=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2842[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2843; _temp2843.tag= Cyc_Absyn_UnknownTunionfield; _temp2843.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2844; _temp2844.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2844.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2844.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2844;}); _temp2843;}); _temp2842;})); _temp2841.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2841;}); _temp2840;}); _temp2839;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2838;}); _temp2837;}); break; case 113: _LL2836: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2846=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2846[ 0]=({ struct Cyc_Bool_tok_struct
_temp2847; _temp2847.tag= Cyc_Bool_tok; _temp2847.f1= 0; _temp2847;}); _temp2846;});
break; case 114: _LL2845: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2849=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2849[ 0]=({ struct Cyc_Bool_tok_struct _temp2850; _temp2850.tag= Cyc_Bool_tok;
_temp2850.f1= 1; _temp2850;}); _temp2849;}); break; case 115: _LL2848: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2852=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2852[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2853; _temp2853.tag= Cyc_TunionFieldList_tok;
_temp2853.f1=({ struct Cyc_List_List* _temp2854=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2854->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2854->tl=
0; _temp2854;}); _temp2853;}); _temp2852;}); break; case 116: _LL2851: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2856=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2856[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2857; _temp2857.tag= Cyc_TunionFieldList_tok;
_temp2857.f1=({ struct Cyc_List_List* _temp2858=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2858->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2858->tl=
0; _temp2858;}); _temp2857;}); _temp2856;}); break; case 117: _LL2855: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2860=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2860[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2861; _temp2861.tag= Cyc_TunionFieldList_tok;
_temp2861.f1=({ struct Cyc_List_List* _temp2862=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2862->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2862->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2862;}); _temp2861;}); _temp2860;}); break; case 118: _LL2859: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2864=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2864[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2865; _temp2865.tag= Cyc_TunionFieldList_tok;
_temp2865.f1=({ struct Cyc_List_List* _temp2866=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2866->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2866->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2866;}); _temp2865;}); _temp2864;}); break; case 119: _LL2863: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2868=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2868[ 0]=({ struct Cyc_Scope_tok_struct
_temp2869; _temp2869.tag= Cyc_Scope_tok; _temp2869.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2869;}); _temp2868;}); break; case 120: _LL2867: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2871=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2871[ 0]=({ struct Cyc_Scope_tok_struct
_temp2872; _temp2872.tag= Cyc_Scope_tok; _temp2872.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2872;}); _temp2871;}); break; case 121: _LL2870: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2874=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2874[ 0]=({ struct Cyc_Scope_tok_struct
_temp2875; _temp2875.tag= Cyc_Scope_tok; _temp2875.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2875;}); _temp2874;}); break; case 122: _LL2873: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2877=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2877[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2878; _temp2878.tag= Cyc_TunionField_tok;
_temp2878.f1=({ struct Cyc_Absyn_Tunionfield* _temp2879=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2879->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2879->tvs=
0; _temp2879->typs= 0; _temp2879->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2879->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2879;}); _temp2878;}); _temp2877;}); break; case 123:
_LL2876: { struct Cyc_List_List* _temp2881=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2882; _LL2882: { struct Cyc_List_List* _temp2883=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2884; _LL2884: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2885=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2885[ 0]=({ struct Cyc_TunionField_tok_struct _temp2886; _temp2886.tag= Cyc_TunionField_tok;
_temp2886.f1=({ struct Cyc_Absyn_Tunionfield* _temp2887=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2887->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2887->tvs=
_temp2883; _temp2887->typs= _temp2881; _temp2887->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2887->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2887;}); _temp2886;}); _temp2885;}); break;}} case 124:
_LL2880: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 125:
_LL2888: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2890=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2890[ 0]=({ struct Cyc_Declarator_tok_struct _temp2891; _temp2891.tag= Cyc_Declarator_tok;
_temp2891.f1=({ struct Cyc_Parse_Declarator* _temp2892=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2892->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2892->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2892;}); _temp2891;}); _temp2890;}); break; case 126: _LL2889: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2894=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2894[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2895; _temp2895.tag= Cyc_Declarator_tok;
_temp2895.f1=({ struct Cyc_Parse_Declarator* _temp2896=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2896->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2896->tms=
0; _temp2896;}); _temp2895;}); _temp2894;}); break; case 127: _LL2893: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
128: _LL2897: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2899=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2899[ 0]=({ struct Cyc_Declarator_tok_struct _temp2900; _temp2900.tag= Cyc_Declarator_tok;
_temp2900.f1=({ struct Cyc_Parse_Declarator* _temp2901=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2901->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2901->tms=({ struct Cyc_List_List* _temp2902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2902->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2902->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2902;}); _temp2901;}); _temp2900;}); _temp2899;});
break; case 129: _LL2898: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2904=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2904[ 0]=({ struct Cyc_Declarator_tok_struct _temp2905; _temp2905.tag= Cyc_Declarator_tok;
_temp2905.f1=({ struct Cyc_Parse_Declarator* _temp2906=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2906->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2906->tms=({ struct Cyc_List_List* _temp2907=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2907->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2908=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2908[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2909; _temp2909.tag= Cyc_Absyn_ConstArray_mod;
_temp2909.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2909;}); _temp2908;})); _temp2907->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2907;}); _temp2906;});
_temp2905;}); _temp2904;}); break; case 130: _LL2903: { struct _tuple17
_temp2913; struct Cyc_Core_Opt* _temp2914; int _temp2916; struct Cyc_List_List*
_temp2918; struct _tuple17* _temp2911= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2913=* _temp2911; _LL2919:
_temp2918= _temp2913.f1; goto _LL2917; _LL2917: _temp2916= _temp2913.f2; goto
_LL2915; _LL2915: _temp2914= _temp2913.f3; goto _LL2912; _LL2912: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2920=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2920[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2921; _temp2921.tag= Cyc_Declarator_tok;
_temp2921.f1=({ struct Cyc_Parse_Declarator* _temp2922=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2922->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2922->tms=({ struct Cyc_List_List* _temp2923=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2923->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2924=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2924[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2925; _temp2925.tag= Cyc_Absyn_Function_mod;
_temp2925.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2926=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2926[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2927; _temp2927.tag= Cyc_Absyn_WithTypes;
_temp2927.f1= _temp2918; _temp2927.f2= _temp2916; _temp2927.f3= _temp2914;
_temp2927;}); _temp2926;})); _temp2925;}); _temp2924;})); _temp2923->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2923;}); _temp2922;}); _temp2921;}); _temp2920;}); break;} case 131:
_LL2910: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2929=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2929[ 0]=({ struct Cyc_Declarator_tok_struct _temp2930; _temp2930.tag= Cyc_Declarator_tok;
_temp2930.f1=({ struct Cyc_Parse_Declarator* _temp2931=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2931->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2931->tms=({ struct Cyc_List_List* _temp2932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2932->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2933=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2933[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2934; _temp2934.tag= Cyc_Absyn_Function_mod;
_temp2934.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2935=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2935[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2936; _temp2936.tag= Cyc_Absyn_WithTypes;
_temp2936.f1= 0; _temp2936.f2= 0; _temp2936.f3= 0; _temp2936;}); _temp2935;}));
_temp2934;}); _temp2933;})); _temp2932->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)))->tms; _temp2932;}); _temp2931;});
_temp2930;}); _temp2929;}); break; case 132: _LL2928: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2938=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2938[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2939; _temp2939.tag= Cyc_Declarator_tok;
_temp2939.f1=({ struct Cyc_Parse_Declarator* _temp2940=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2940->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2940->tms=({ struct Cyc_List_List* _temp2941=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2941->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2942=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2942[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2943; _temp2943.tag= Cyc_Absyn_Function_mod;
_temp2943.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2944=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2944[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2945; _temp2945.tag= Cyc_Absyn_WithTypes;
_temp2945.f1= 0; _temp2945.f2= 0; _temp2945.f3=({ struct Cyc_Core_Opt* _temp2946=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2946->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2947=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2947[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2948; _temp2948.tag= Cyc_Absyn_JoinEff; _temp2948.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2948;}); _temp2947;})); _temp2946;}); _temp2945;}); _temp2944;}));
_temp2943;}); _temp2942;})); _temp2941->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)))->tms; _temp2941;}); _temp2940;});
_temp2939;}); _temp2938;}); break; case 133: _LL2937: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2950=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2950[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2951; _temp2951.tag= Cyc_Declarator_tok;
_temp2951.f1=({ struct Cyc_Parse_Declarator* _temp2952=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2952->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2952->tms=({ struct Cyc_List_List* _temp2953=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2953->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2954=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2954[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2955; _temp2955.tag= Cyc_Absyn_Function_mod;
_temp2955.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2956=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2956[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2957; _temp2957.tag= Cyc_Absyn_NoTypes;
_temp2957.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2957.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2957;});
_temp2956;})); _temp2955;}); _temp2954;})); _temp2953->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2953;}); _temp2952;}); _temp2951;}); _temp2950;}); break; case 134: _LL2949: {
struct Cyc_List_List* _temp2959=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
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
_LL2958:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2968=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2969; _LL2969: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2970=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2970[ 0]=({ struct Cyc_Declarator_tok_struct _temp2971; _temp2971.tag= Cyc_Declarator_tok;
_temp2971.f1=({ struct Cyc_Parse_Declarator* _temp2972=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2972->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2972->tms=({ struct Cyc_List_List* _temp2973=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2973->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2974=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2974[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2975; _temp2975.tag= Cyc_Absyn_TypeParams_mod;
_temp2975.f1= _temp2968; _temp2975.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2975.f3= 0; _temp2975;}); _temp2974;})); _temp2973->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2973;}); _temp2972;}); _temp2971;}); _temp2970;}); break;} case 136:
_LL2967: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2977=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2977[ 0]=({ struct Cyc_Declarator_tok_struct _temp2978; _temp2978.tag= Cyc_Declarator_tok;
_temp2978.f1=({ struct Cyc_Parse_Declarator* _temp2979=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2979->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp2979->tms=({ struct Cyc_List_List* _temp2980=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2980->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2981=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2981[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2982; _temp2982.tag= Cyc_Absyn_Attributes_mod;
_temp2982.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2982.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2982;});
_temp2981;})); _temp2980->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp2980;}); _temp2979;}); _temp2978;});
_temp2977;}); break; case 137: _LL2976: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2984=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2984[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2985; _temp2985.tag= Cyc_TypeModifierList_tok; _temp2985.f1=({ struct Cyc_List_List*
_temp2986=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2986->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2987=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2987[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2988; _temp2988.tag=
Cyc_Absyn_Pointer_mod; _temp2988.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2988.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2988.f3=
Cyc_Absyn_empty_tqual(); _temp2988;}); _temp2987;})); _temp2986->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp2986;}); _temp2985;}); _temp2984;});
break; case 138: _LL2983: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2990=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2990[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2991; _temp2991.tag= Cyc_TypeModifierList_tok; _temp2991.f1=({ struct Cyc_List_List*
_temp2992=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2992->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2993=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2993[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2994; _temp2994.tag=
Cyc_Absyn_Pointer_mod; _temp2994.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2994.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2994.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2994;}); _temp2993;})); _temp2992->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp2992;}); _temp2991;}); _temp2990;}); break; case 139: _LL2989: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2996=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2996[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2997; _temp2997.tag= Cyc_TypeModifierList_tok;
_temp2997.f1=({ struct Cyc_List_List* _temp2998=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2998->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2999=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2999[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3000; _temp3000.tag= Cyc_Absyn_Pointer_mod;
_temp3000.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3000.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3000.f3=
Cyc_Absyn_empty_tqual(); _temp3000;}); _temp2999;})); _temp2998->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp2998;}); _temp2997;}); _temp2996;});
break; case 140: _LL2995: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3002=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3002[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3003; _temp3003.tag= Cyc_TypeModifierList_tok; _temp3003.f1=({ struct Cyc_List_List*
_temp3004=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3004->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3005=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3005[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3006; _temp3006.tag=
Cyc_Absyn_Pointer_mod; _temp3006.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3006.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3006.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3006;}); _temp3005;})); _temp3004->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3004;}); _temp3003;}); _temp3002;}); break; case 141: _LL3001: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp3008=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3008[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3009; _temp3009.tag= Cyc_Pointer_Sort_tok;
_temp3009.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3010=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3010[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3011; _temp3011.tag=
Cyc_Absyn_Nullable_ps; _temp3011.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3011;}); _temp3010;})); _temp3009;}); _temp3008;}); break; case 142:
_LL3007: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3013=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3013[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3014; _temp3014.tag=
Cyc_Pointer_Sort_tok; _temp3014.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3015=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3015[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3016; _temp3016.tag= Cyc_Absyn_NonNullable_ps; _temp3016.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3016;}); _temp3015;})); _temp3014;}); _temp3013;});
break; case 143: _LL3012: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3018=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3018[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3019; _temp3019.tag=
Cyc_Pointer_Sort_tok; _temp3019.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3020=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3020[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3021; _temp3021.tag=
Cyc_Absyn_Nullable_ps; _temp3021.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3021;}); _temp3020;})); _temp3019;});
_temp3018;}); break; case 144: _LL3017: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3023=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3023[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3024; _temp3024.tag=
Cyc_Pointer_Sort_tok; _temp3024.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3025=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3025[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3026; _temp3026.tag= Cyc_Absyn_NonNullable_ps; _temp3026.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3026;});
_temp3025;})); _temp3024;}); _temp3023;}); break; case 145: _LL3022: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3028=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3028[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3029; _temp3029.tag= Cyc_Pointer_Sort_tok;
_temp3029.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3029;});
_temp3028;}); break; case 146: _LL3027: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3031=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3031[ 0]=({ struct Cyc_Rgn_tok_struct _temp3032; _temp3032.tag= Cyc_Rgn_tok;
_temp3032.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3032;}); _temp3031;});
break; case 147: _LL3030: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 148: _LL3033: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3035=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3035[ 0]=({ struct Cyc_Rgn_tok_struct _temp3036; _temp3036.tag= Cyc_Rgn_tok;
_temp3036.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3036;}); _temp3035;});
break; case 149: _LL3034: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({
char* _temp3038="expecting region kind\n"; struct _tagged_string _temp3039;
_temp3039.curr= _temp3038; _temp3039.base= _temp3038; _temp3039.last_plus_one=
_temp3038 + 23; _temp3039;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Rgn_tok_struct* _temp3040=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp3040[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3041; _temp3041.tag= Cyc_Rgn_tok; _temp3041.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3041;}); _temp3040;}); break; case 150: _LL3037: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3043=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3043[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3044; _temp3044.tag= Cyc_Rgn_tok; _temp3044.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp3044;}); _temp3043;}); break; case 151: _LL3042:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 152:
_LL3045: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp3047=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3047[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3048; _temp3048.tag= Cyc_TypeQual_tok; _temp3048.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3048;}); _temp3047;}); break; case 153: _LL3046: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3050=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3050[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3051; _temp3051.tag= Cyc_ParamDeclListBool_tok;
_temp3051.f1=({ struct _tuple17* _temp3052=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3052->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3052->f2= 0; _temp3052->f3= 0; _temp3052;});
_temp3051;}); _temp3050;}); break; case 154: _LL3049: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3054=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3054[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3055; _temp3055.tag= Cyc_ParamDeclListBool_tok;
_temp3055.f1=({ struct _tuple17* _temp3056=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3056->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3056->f2= 1; _temp3056->f3= 0;
_temp3056;}); _temp3055;}); _temp3054;}); break; case 155: _LL3053: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp3058=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3058[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3059; _temp3059.tag= Cyc_ParamDeclListBool_tok;
_temp3059.f1=({ struct _tuple17* _temp3060=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3060->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3060->f2= 0; _temp3060->f3=( struct
Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp3061=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3061->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3062=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3062[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3063; _temp3063.tag= Cyc_Absyn_JoinEff; _temp3063.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3063;}); _temp3062;})); _temp3061;}); _temp3060;}); _temp3059;}); _temp3058;});
break; case 156: _LL3057: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 157: _LL3064: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3066=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3066[ 0]=({ struct Cyc_TypeList_tok_struct _temp3067; _temp3067.tag= Cyc_TypeList_tok;
_temp3067.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3067;}); _temp3066;}); break; case 158:
_LL3065: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3069=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3069[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3070; _temp3070.tag= Cyc_TypeList_tok; _temp3070.f1= 0; _temp3070;});
_temp3069;}); break; case 159: _LL3068: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 160: _LL3071: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3073=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3073[ 0]=({ struct Cyc_TypeList_tok_struct _temp3074; _temp3074.tag= Cyc_TypeList_tok;
_temp3074.f1=({ struct Cyc_List_List* _temp3075=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3075->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3075->tl= 0; _temp3075;}); _temp3074;}); _temp3073;}); break; case 161:
_LL3072: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(({ char* _temp3077="expecing effect kind (E)";
struct _tagged_string _temp3078; _temp3078.curr= _temp3077; _temp3078.base=
_temp3077; _temp3078.last_plus_one= _temp3077 + 25; _temp3078;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3079=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3079[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3080; _temp3080.tag= Cyc_TypeList_tok; _temp3080.f1=({ struct Cyc_List_List*
_temp3081=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3081->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3081->tl= 0; _temp3081;});
_temp3080;}); _temp3079;}); break; case 162: _LL3076: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3083=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3083[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3084; _temp3084.tag= Cyc_TypeList_tok; _temp3084.f1=({ struct Cyc_List_List*
_temp3085=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3085->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3086=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3086[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3087; _temp3087.tag= Cyc_Absyn_AccessEff;
_temp3087.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3087;}); _temp3086;}));
_temp3085->tl= 0; _temp3085;}); _temp3084;}); _temp3083;}); break; case 163:
_LL3082: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3089=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3089[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3090; _temp3090.tag= Cyc_TypeList_tok; _temp3090.f1=({ struct Cyc_List_List*
_temp3091=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3091->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3092=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3092[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3093; _temp3093.tag= Cyc_Absyn_AccessEff;
_temp3093.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3093;}); _temp3092;}));
_temp3091->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3091;}); _temp3090;}); _temp3089;}); break; case 164:
_LL3088: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ char* _temp3095="expecting region kind (R)";
struct _tagged_string _temp3096; _temp3096.curr= _temp3095; _temp3096.base=
_temp3095; _temp3096.last_plus_one= _temp3095 + 26; _temp3096;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3097=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3097[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3098; _temp3098.tag= Cyc_TypeList_tok; _temp3098.f1=({ struct Cyc_List_List*
_temp3099=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3099->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3100=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3100[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3101; _temp3101.tag= Cyc_Absyn_AccessEff;
_temp3101.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3101;}); _temp3100;}));
_temp3099->tl= 0; _temp3099;}); _temp3098;}); _temp3097;}); break; case 165:
_LL3094: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ char* _temp3103="expecting region kind (R)";
struct _tagged_string _temp3104; _temp3104.curr= _temp3103; _temp3104.base=
_temp3103; _temp3104.last_plus_one= _temp3103 + 26; _temp3104;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3105=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3105[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3106; _temp3106.tag= Cyc_TypeList_tok; _temp3106.f1=({ struct Cyc_List_List*
_temp3107=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3107->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3108=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3108[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3109; _temp3109.tag= Cyc_Absyn_AccessEff;
_temp3109.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3109;}); _temp3108;}));
_temp3107->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3107;}); _temp3106;}); _temp3105;}); break; case 166:
_LL3102: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3111=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3111[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3112; _temp3112.tag=
Cyc_ParamDeclList_tok; _temp3112.f1=({ struct Cyc_List_List* _temp3113=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3113->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3113->tl= 0; _temp3113;}); _temp3112;}); _temp3111;}); break; case 167:
_LL3110: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3115=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3115[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3116; _temp3116.tag=
Cyc_ParamDeclList_tok; _temp3116.f1=({ struct Cyc_List_List* _temp3117=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3117->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3117->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3117;}); _temp3116;}); _temp3115;}); break; case 168:
_LL3114: { void* _temp3119= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3120; _LL3120: { struct Cyc_List_List* _temp3121=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3122; _LL3122: { struct Cyc_Absyn_Tqual _temp3123=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3124; _LL3124: { struct Cyc_List_List* _temp3125=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3126; _LL3126: { struct _tuple6 _temp3127= Cyc_Parse_apply_tms( _temp3123,
_temp3119, _temp3121, _temp3125); goto _LL3128; _LL3128: if( _temp3127.f3 != 0){
Cyc_Parse_err(({ char* _temp3129="parameter with bad type params"; struct
_tagged_string _temp3130; _temp3130.curr= _temp3129; _temp3130.base= _temp3129;
_temp3130.last_plus_one= _temp3129 + 31; _temp3130;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3131=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3132; _LL3132:{ void* _temp3133=(*
_temp3131).f1; struct Cyc_List_List* _temp3143; struct Cyc_List_List* _temp3145;
_LL3135: if( _temp3133 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3136;} else{ goto
_LL3137;} _LL3137: if(( unsigned int) _temp3133 > 1u?*(( int*) _temp3133) == Cyc_Absyn_Rel_n:
0){ _LL3144: _temp3143=(( struct Cyc_Absyn_Rel_n_struct*) _temp3133)->f1; if(
_temp3143 == 0){ goto _LL3138;} else{ goto _LL3139;}} else{ goto _LL3139;}
_LL3139: if(( unsigned int) _temp3133 > 1u?*(( int*) _temp3133) == Cyc_Absyn_Abs_n:
0){ _LL3146: _temp3145=(( struct Cyc_Absyn_Abs_n_struct*) _temp3133)->f1; if(
_temp3145 == 0){ goto _LL3140;} else{ goto _LL3141;}} else{ goto _LL3141;}
_LL3141: goto _LL3142; _LL3136: goto _LL3134; _LL3138: goto _LL3134; _LL3140:
goto _LL3134; _LL3142: Cyc_Parse_err(({ char* _temp3147="parameter cannot be qualified with a module name";
struct _tagged_string _temp3148; _temp3148.curr= _temp3147; _temp3148.base=
_temp3147; _temp3148.last_plus_one= _temp3147 + 49; _temp3148;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3134; _LL3134:;}{ struct Cyc_Core_Opt* _temp3150=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3149=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3149->v=( void*)(* _temp3131).f2; _temp3149;}); goto
_LL3151; _LL3151: if( _temp3127.f4 != 0){ Cyc_Parse_warn(({ char* _temp3152="extra attributes on parameter, ignoring";
struct _tagged_string _temp3153; _temp3153.curr= _temp3152; _temp3153.base=
_temp3152; _temp3153.last_plus_one= _temp3152 + 40; _temp3153;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3127.f3 != 0){ Cyc_Parse_warn(({ char* _temp3154="extra type variables on parameter, ignoring";
struct _tagged_string _temp3155; _temp3155.curr= _temp3154; _temp3155.base=
_temp3154; _temp3155.last_plus_one= _temp3154 + 44; _temp3155;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3156=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3156[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3157; _temp3157.tag= Cyc_ParamDecl_tok; _temp3157.f1=({
struct _tuple2* _temp3158=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3158->f1= _temp3150; _temp3158->f2= _temp3127.f1; _temp3158->f3= _temp3127.f2;
_temp3158;}); _temp3157;}); _temp3156;}); break;}}}}}}} case 169: _LL3118: {
void* _temp3160= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3161; _LL3161: { struct Cyc_List_List* _temp3162=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3163; _LL3163: { struct Cyc_Absyn_Tqual _temp3164=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3165; _LL3165: if( _temp3162 != 0){ Cyc_Parse_warn(({ char* _temp3166="bad attributes on parameter, ignoring";
struct _tagged_string _temp3167; _temp3167.curr= _temp3166; _temp3167.base=
_temp3166; _temp3167.last_plus_one= _temp3166 + 38; _temp3167;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3168=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3168[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3169; _temp3169.tag= Cyc_ParamDecl_tok; _temp3169.f1=({
struct _tuple2* _temp3170=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3170->f1= 0; _temp3170->f2= _temp3164; _temp3170->f3= _temp3160; _temp3170;});
_temp3169;}); _temp3168;}); break;}}} case 170: _LL3159: { void* _temp3172= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3173; _LL3173: { struct Cyc_List_List*
_temp3174=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3175; _LL3175: { struct Cyc_Absyn_Tqual
_temp3176=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3177; _LL3177: { struct Cyc_List_List* _temp3178=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3179; _LL3179: { struct _tuple6 _temp3180= Cyc_Parse_apply_tms(
_temp3176, _temp3172, _temp3174, _temp3178); goto _LL3181; _LL3181: if(
_temp3180.f3 != 0){ Cyc_Parse_warn(({ char* _temp3182="bad type parameters on formal argument, ignoring";
struct _tagged_string _temp3183; _temp3183.curr= _temp3182; _temp3183.base=
_temp3182; _temp3183.last_plus_one= _temp3182 + 49; _temp3183;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3180.f4 != 0){ Cyc_Parse_warn(({ char* _temp3184="bad attributes on parameter, ignoring";
struct _tagged_string _temp3185; _temp3185.curr= _temp3184; _temp3185.base=
_temp3184; _temp3185.last_plus_one= _temp3184 + 38; _temp3185;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3186=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3186[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3187; _temp3187.tag= Cyc_ParamDecl_tok; _temp3187.f1=({
struct _tuple2* _temp3188=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3188->f1= 0; _temp3188->f2= _temp3180.f1; _temp3188->f3= _temp3180.f2;
_temp3188;}); _temp3187;}); _temp3186;}); break;}}}}} case 171: _LL3171: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3190=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3190[ 0]=({ struct Cyc_IdList_tok_struct
_temp3191; _temp3191.tag= Cyc_IdList_tok; _temp3191.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3191;}); _temp3190;}); break; case
172: _LL3189: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3193=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3193[ 0]=({ struct Cyc_IdList_tok_struct _temp3194; _temp3194.tag= Cyc_IdList_tok;
_temp3194.f1=({ struct Cyc_List_List* _temp3195=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3195->hd=( void*)({ struct
_tagged_string* _temp3196=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3196[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3196;}); _temp3195->tl= 0; _temp3195;});
_temp3194;}); _temp3193;}); break; case 173: _LL3192: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3198=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3198[ 0]=({ struct Cyc_IdList_tok_struct
_temp3199; _temp3199.tag= Cyc_IdList_tok; _temp3199.f1=({ struct Cyc_List_List*
_temp3200=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3200->hd=( void*)({ struct _tagged_string* _temp3201=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3201[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3201;}); _temp3200->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3200;}); _temp3199;}); _temp3198;});
break; case 174: _LL3197: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 175: _LL3202: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 176: _LL3203: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3205=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3205[ 0]=({ struct Cyc_Exp_tok_struct _temp3206; _temp3206.tag= Cyc_Exp_tok;
_temp3206.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3207=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3207[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3208; _temp3208.tag= Cyc_Absyn_UnresolvedMem_e; _temp3208.f1= 0; _temp3208.f2=
0; _temp3208;}); _temp3207;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3206;});
_temp3205;}); break; case 177: _LL3204: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3210=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3210[ 0]=({ struct Cyc_Exp_tok_struct _temp3211; _temp3211.tag= Cyc_Exp_tok;
_temp3211.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3212=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3212[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3213; _temp3213.tag= Cyc_Absyn_UnresolvedMem_e; _temp3213.f1= 0; _temp3213.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3213;});
_temp3212;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3211;});
_temp3210;}); break; case 178: _LL3209: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3215=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3215[ 0]=({ struct Cyc_Exp_tok_struct _temp3216; _temp3216.tag= Cyc_Exp_tok;
_temp3216.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3217=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3217[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3218; _temp3218.tag= Cyc_Absyn_UnresolvedMem_e; _temp3218.f1= 0; _temp3218.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3218;});
_temp3217;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3216;});
_temp3215;}); break; case 179: _LL3214: { struct Cyc_Absyn_Vardecl* _temp3222=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3220=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3220->f1=( void*) Cyc_Absyn_Loc_n; _temp3220->f2=({
struct _tagged_string* _temp3221=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3221[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3221;}); _temp3220;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3223; _LL3223: _temp3222->tq=({ struct Cyc_Absyn_Tqual _temp3224;
_temp3224.q_const= 1; _temp3224.q_volatile= 0; _temp3224.q_restrict= 1;
_temp3224;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3225=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3225[ 0]=({ struct Cyc_Exp_tok_struct
_temp3226; _temp3226.tag= Cyc_Exp_tok; _temp3226.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3227=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3227[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3228; _temp3228.tag= Cyc_Absyn_Comprehension_e;
_temp3228.f1= _temp3222; _temp3228.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3228.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3228;}); _temp3227;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3226;}); _temp3225;}); break;} case 180: _LL3219: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3230=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3230[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3231; _temp3231.tag= Cyc_InitializerList_tok;
_temp3231.f1=({ struct Cyc_List_List* _temp3232=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3232->hd=( void*)({ struct
_tuple19* _temp3233=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3233->f1= 0; _temp3233->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3233;}); _temp3232->tl= 0; _temp3232;});
_temp3231;}); _temp3230;}); break; case 181: _LL3229: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3235=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3235[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3236; _temp3236.tag= Cyc_InitializerList_tok;
_temp3236.f1=({ struct Cyc_List_List* _temp3237=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3237->hd=( void*)({ struct
_tuple19* _temp3238=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3238->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3238->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3238;}); _temp3237->tl= 0; _temp3237;});
_temp3236;}); _temp3235;}); break; case 182: _LL3234: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3240=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3240[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3241; _temp3241.tag= Cyc_InitializerList_tok;
_temp3241.f1=({ struct Cyc_List_List* _temp3242=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3242->hd=( void*)({ struct
_tuple19* _temp3243=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3243->f1= 0; _temp3243->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3243;}); _temp3242->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3242;});
_temp3241;}); _temp3240;}); break; case 183: _LL3239: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3245=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3245[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3246; _temp3246.tag= Cyc_InitializerList_tok;
_temp3246.f1=({ struct Cyc_List_List* _temp3247=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3247->hd=( void*)({ struct
_tuple19* _temp3248=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3248->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3248->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3248;}); _temp3247->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3247;});
_temp3246;}); _temp3245;}); break; case 184: _LL3244: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3250=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3250[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3251; _temp3251.tag= Cyc_DesignatorList_tok;
_temp3251.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3251;}); _temp3250;}); break; case 185: _LL3249: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3253=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3253[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3254; _temp3254.tag= Cyc_DesignatorList_tok;
_temp3254.f1=({ struct Cyc_List_List* _temp3255=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3255->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3255->tl=
0; _temp3255;}); _temp3254;}); _temp3253;}); break; case 186: _LL3252: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3257=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3257[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3258; _temp3258.tag= Cyc_DesignatorList_tok;
_temp3258.f1=({ struct Cyc_List_List* _temp3259=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3259->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3259->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3259;}); _temp3258;}); _temp3257;}); break; case 187: _LL3256: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3261=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3261[ 0]=({ struct
Cyc_Designator_tok_struct _temp3262; _temp3262.tag= Cyc_Designator_tok;
_temp3262.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3263=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3263[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3264; _temp3264.tag=
Cyc_Absyn_ArrayElement; _temp3264.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3264;}); _temp3263;})); _temp3262;});
_temp3261;}); break; case 188: _LL3260: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3266=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3266[ 0]=({ struct Cyc_Designator_tok_struct _temp3267; _temp3267.tag= Cyc_Designator_tok;
_temp3267.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3268=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3268[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3269; _temp3269.tag= Cyc_Absyn_FieldName;
_temp3269.f1=({ struct _tagged_string* _temp3270=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3270[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3270;});
_temp3269;}); _temp3268;})); _temp3267;}); _temp3266;}); break; case 189:
_LL3265: { void* _temp3272= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3273; _LL3273: { struct Cyc_List_List* _temp3274=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3275; _LL3275: if( _temp3274 != 0){ Cyc_Parse_warn(({ char* _temp3276="ignoring attributes in type";
struct _tagged_string _temp3277; _temp3277.curr= _temp3276; _temp3277.base=
_temp3276; _temp3277.last_plus_one= _temp3276 + 28; _temp3277;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tqual _temp3278=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f1; goto _LL3279; _LL3279: yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp3280=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3280[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3281; _temp3281.tag= Cyc_ParamDecl_tok; _temp3281.f1=({
struct _tuple2* _temp3282=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp3282[ 0]=({ struct _tuple2 _temp3283; _temp3283.f1= 0; _temp3283.f2=
_temp3278; _temp3283.f3= _temp3272; _temp3283;}); _temp3282;}); _temp3281;});
_temp3280;}); break;}}} case 190: _LL3271: { void* _temp3285= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3286; _LL3286: { struct Cyc_List_List*
_temp3287=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3288; _LL3288: { struct Cyc_Absyn_Tqual
_temp3289=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3290; _LL3290: { struct Cyc_List_List* _temp3291=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3292; _LL3292: { struct _tuple6 _temp3293= Cyc_Parse_apply_tms(
_temp3289, _temp3285, _temp3287, _temp3291); goto _LL3294; _LL3294: if(
_temp3293.f3 != 0){ Cyc_Parse_warn(({ char* _temp3295="bad type params, ignoring";
struct _tagged_string _temp3296; _temp3296.curr= _temp3295; _temp3296.base=
_temp3295; _temp3296.last_plus_one= _temp3295 + 26; _temp3296;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3293.f4 != 0){ Cyc_Parse_warn(({ char* _temp3297="bad specifiers, ignoring";
struct _tagged_string _temp3298; _temp3298.curr= _temp3297; _temp3298.base=
_temp3297; _temp3298.last_plus_one= _temp3297 + 25; _temp3298;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3299=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3299[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3300; _temp3300.tag= Cyc_ParamDecl_tok; _temp3300.f1=({
struct _tuple2* _temp3301=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3301->f1= 0; _temp3301->f2= _temp3293.f1; _temp3301->f3= _temp3293.f2;
_temp3301;}); _temp3300;}); _temp3299;}); break;}}}}} case 191: _LL3284: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3303=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3303[ 0]=({ struct Cyc_Type_tok_struct
_temp3304; _temp3304.tag= Cyc_Type_tok; _temp3304.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3304;});
_temp3303;}); break; case 192: _LL3302: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3306=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3306[ 0]=({ struct Cyc_Type_tok_struct _temp3307; _temp3307.tag= Cyc_Type_tok;
_temp3307.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3308=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3308[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3309; _temp3309.tag= Cyc_Absyn_JoinEff;
_temp3309.f1= 0; _temp3309;}); _temp3308;})); _temp3307;}); _temp3306;}); break;
case 193: _LL3305: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3311=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3311[ 0]=({ struct Cyc_Type_tok_struct _temp3312; _temp3312.tag= Cyc_Type_tok;
_temp3312.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3313=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3313[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3314; _temp3314.tag= Cyc_Absyn_JoinEff;
_temp3314.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3314;}); _temp3313;})); _temp3312;}); _temp3311;});
break; case 194: _LL3310: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3316=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3316[ 0]=({ struct Cyc_Type_tok_struct _temp3317; _temp3317.tag= Cyc_Type_tok;
_temp3317.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3318=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3318[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3319; _temp3319.tag= Cyc_Absyn_JoinEff;
_temp3319.f1=({ struct Cyc_List_List* _temp3320=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3320->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3320->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3320;}); _temp3319;}); _temp3318;})); _temp3317;}); _temp3316;}); break;
case 195: _LL3315: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3322=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3322[ 0]=({ struct Cyc_TypeList_tok_struct _temp3323; _temp3323.tag= Cyc_TypeList_tok;
_temp3323.f1=({ struct Cyc_List_List* _temp3324=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3324->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3324->tl=
0; _temp3324;}); _temp3323;}); _temp3322;}); break; case 196: _LL3321: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3326=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3326[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3327; _temp3327.tag= Cyc_TypeList_tok; _temp3327.f1=({ struct Cyc_List_List*
_temp3328=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3328->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3328->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3328;}); _temp3327;}); _temp3326;});
break; case 197: _LL3325: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3330=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3330[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3331; _temp3331.tag= Cyc_AbstractDeclarator_tok; _temp3331.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3332=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3332->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3332;}); _temp3331;}); _temp3330;}); break;
case 198: _LL3329: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 199: _LL3333: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3335=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3335[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3336; _temp3336.tag= Cyc_AbstractDeclarator_tok; _temp3336.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3337=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3337->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3337;}); _temp3336;});
_temp3335;}); break; case 200: _LL3334: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 201: _LL3338: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3340=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3340[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3341; _temp3341.tag= Cyc_AbstractDeclarator_tok; _temp3341.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3342=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3342->tms=({ struct Cyc_List_List* _temp3343=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3343->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3343->tl= 0; _temp3343;}); _temp3342;}); _temp3341;}); _temp3340;}); break;
case 202: _LL3339: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3345=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3345[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3346; _temp3346.tag= Cyc_AbstractDeclarator_tok; _temp3346.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3347=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3347->tms=({ struct Cyc_List_List* _temp3348=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3348->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3348->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3348;}); _temp3347;}); _temp3346;});
_temp3345;}); break; case 203: _LL3344: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3350=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3350[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3351; _temp3351.tag= Cyc_AbstractDeclarator_tok; _temp3351.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3352=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3352->tms=({ struct Cyc_List_List* _temp3353=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3353->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3354=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3354[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3355; _temp3355.tag= Cyc_Absyn_ConstArray_mod;
_temp3355.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3355;}); _temp3354;})); _temp3353->tl= 0; _temp3353;}); _temp3352;});
_temp3351;}); _temp3350;}); break; case 204: _LL3349: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3357=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3357[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3358; _temp3358.tag= Cyc_AbstractDeclarator_tok;
_temp3358.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3359=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3359->tms=({
struct Cyc_List_List* _temp3360=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3360->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3361=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3361[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3362; _temp3362.tag= Cyc_Absyn_ConstArray_mod; _temp3362.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3362;});
_temp3361;})); _temp3360->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3360;}); _temp3359;});
_temp3358;}); _temp3357;}); break; case 205: _LL3356: yyval=( void*)({ struct
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
_temp3371.f1= 0; _temp3371.f2= 0; _temp3371.f3= 0; _temp3371;}); _temp3370;}));
_temp3369;}); _temp3368;})); _temp3367->tl= 0; _temp3367;}); _temp3366;});
_temp3365;}); _temp3364;}); break; case 206: _LL3363: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3373=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3373[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3374; _temp3374.tag= Cyc_AbstractDeclarator_tok;
_temp3374.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3375=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3375->tms=({
struct Cyc_List_List* _temp3376=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3376->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3377=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3377[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3378; _temp3378.tag=
Cyc_Absyn_Function_mod; _temp3378.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3379=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3379[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3380; _temp3380.tag= Cyc_Absyn_WithTypes;
_temp3380.f1= 0; _temp3380.f2= 0; _temp3380.f3=({ struct Cyc_Core_Opt* _temp3381=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3381->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3382=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3382[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3383; _temp3383.tag= Cyc_Absyn_JoinEff; _temp3383.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3383;}); _temp3382;})); _temp3381;}); _temp3380;}); _temp3379;}));
_temp3378;}); _temp3377;})); _temp3376->tl= 0; _temp3376;}); _temp3375;});
_temp3374;}); _temp3373;}); break; case 207: _LL3372: { struct _tuple17
_temp3387; struct Cyc_Core_Opt* _temp3388; int _temp3390; struct Cyc_List_List*
_temp3392; struct _tuple17* _temp3385= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3387=* _temp3385; _LL3393:
_temp3392= _temp3387.f1; goto _LL3391; _LL3391: _temp3390= _temp3387.f2; goto
_LL3389; _LL3389: _temp3388= _temp3387.f3; goto _LL3386; _LL3386: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3394=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3394[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3395; _temp3395.tag= Cyc_AbstractDeclarator_tok;
_temp3395.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3396=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3396->tms=({
struct Cyc_List_List* _temp3397=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3397->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3398=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3398[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3399; _temp3399.tag=
Cyc_Absyn_Function_mod; _temp3399.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3400=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3400[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3401; _temp3401.tag= Cyc_Absyn_WithTypes;
_temp3401.f1= _temp3392; _temp3401.f2= _temp3390; _temp3401.f3= _temp3388;
_temp3401;}); _temp3400;})); _temp3399;}); _temp3398;})); _temp3397->tl= 0;
_temp3397;}); _temp3396;}); _temp3395;}); _temp3394;}); break;} case 208:
_LL3384: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3403=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3403[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3404; _temp3404.tag=
Cyc_AbstractDeclarator_tok; _temp3404.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3405=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3405->tms=({ struct Cyc_List_List* _temp3406=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3406->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3407=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3407[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3408; _temp3408.tag= Cyc_Absyn_Function_mod;
_temp3408.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3409=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3409[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3410; _temp3410.tag= Cyc_Absyn_WithTypes;
_temp3410.f1= 0; _temp3410.f2= 0; _temp3410.f3= 0; _temp3410;}); _temp3409;}));
_temp3408;}); _temp3407;})); _temp3406->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp3406;}); _temp3405;}); _temp3404;}); _temp3403;}); break; case 209: _LL3402: {
struct Cyc_Core_Opt* _temp3415=({ struct Cyc_Core_Opt* _temp3412=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3412->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3413=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3413[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3414; _temp3414.tag= Cyc_Absyn_JoinEff; _temp3414.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3414;}); _temp3413;})); _temp3412;}); goto _LL3416; _LL3416: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3417=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3417[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3418; _temp3418.tag= Cyc_AbstractDeclarator_tok;
_temp3418.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3419=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3419->tms=({
struct Cyc_List_List* _temp3420=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3420->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3421=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3421[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3422; _temp3422.tag=
Cyc_Absyn_Function_mod; _temp3422.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3423=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3423[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3424; _temp3424.tag= Cyc_Absyn_WithTypes;
_temp3424.f1= 0; _temp3424.f2= 0; _temp3424.f3= _temp3415; _temp3424;});
_temp3423;})); _temp3422;}); _temp3421;})); _temp3420->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3420;}); _temp3419;}); _temp3418;}); _temp3417;}); break;} case 210:
_LL3411: { struct _tuple17 _temp3428; struct Cyc_Core_Opt* _temp3429; int
_temp3431; struct Cyc_List_List* _temp3433; struct _tuple17* _temp3426= Cyc_yyget_ParamDeclListBool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3428=*
_temp3426; _LL3434: _temp3433= _temp3428.f1; goto _LL3432; _LL3432: _temp3431=
_temp3428.f2; goto _LL3430; _LL3430: _temp3429= _temp3428.f3; goto _LL3427;
_LL3427: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3435=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3435[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3436; _temp3436.tag=
Cyc_AbstractDeclarator_tok; _temp3436.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3437=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3437->tms=({ struct Cyc_List_List* _temp3438=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3438->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3439=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3439[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3440; _temp3440.tag= Cyc_Absyn_Function_mod;
_temp3440.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3441=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3441[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3442; _temp3442.tag= Cyc_Absyn_WithTypes;
_temp3442.f1= _temp3433; _temp3442.f2= _temp3431; _temp3442.f3= _temp3429;
_temp3442;}); _temp3441;})); _temp3440;}); _temp3439;})); _temp3438->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3438;}); _temp3437;}); _temp3436;}); _temp3435;}); break;} case 211:
_LL3425: { struct Cyc_List_List* _temp3444=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
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
_LL3443:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3453=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3454; _LL3454: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3455=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3455[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3456; _temp3456.tag= Cyc_AbstractDeclarator_tok; _temp3456.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3457=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3457->tms=({ struct Cyc_List_List* _temp3458=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3458->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3459=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3459[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3460; _temp3460.tag= Cyc_Absyn_TypeParams_mod;
_temp3460.f1= _temp3453; _temp3460.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3460.f3= 0; _temp3460;}); _temp3459;})); _temp3458->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3458;}); _temp3457;}); _temp3456;}); _temp3455;}); break;} case 213:
_LL3452: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3462=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3462[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3463; _temp3463.tag=
Cyc_AbstractDeclarator_tok; _temp3463.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3464=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3464->tms=({ struct Cyc_List_List* _temp3465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3465->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3466=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3466[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3467; _temp3467.tag= Cyc_Absyn_Attributes_mod;
_temp3467.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3467.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3467;});
_temp3466;})); _temp3465->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3465;}); _temp3464;});
_temp3463;}); _temp3462;}); break; case 214: _LL3461: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 215: _LL3468: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 216: _LL3469: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 217: _LL3470: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 218: _LL3471: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 219: _LL3472: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 220: _LL3473: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),({
char* _temp3475="`H"; struct _tagged_string _temp3476; _temp3476.curr= _temp3475;
_temp3476.base= _temp3475; _temp3476.last_plus_one= _temp3475 + 3; _temp3476;}))
== 0){ Cyc_Parse_err(({ char* _temp3477="bad occurrence of heap region `H";
struct _tagged_string _temp3478; _temp3478.curr= _temp3477; _temp3478.base=
_temp3477; _temp3478.last_plus_one= _temp3477 + 33; _temp3478;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3489=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3489->name=({ struct
_tagged_string* _temp3490=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3490[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3490;}); _temp3489->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3489;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3487=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3487[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3488; _temp3488.tag= Cyc_Absyn_VarType; _temp3488.f1=
tv; _temp3488;}); _temp3487;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3479=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3479[ 0]=({ struct Cyc_Stmt_tok_struct _temp3480; _temp3480.tag= Cyc_Stmt_tok;
_temp3480.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3481=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3481[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3482; _temp3482.tag= Cyc_Absyn_Region_s;
_temp3482.f1= tv; _temp3482.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3483=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3483->f1=(
void*) Cyc_Absyn_Loc_n; _temp3483->f2=({ struct _tagged_string* _temp3484=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3484[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3484;}); _temp3483;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3485=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3485[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3486; _temp3486.tag= Cyc_Absyn_RgnHandleType; _temp3486.f1=( void*) t;
_temp3486;}); _temp3485;}), 0); _temp3482.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3482;}); _temp3481;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3480;}); _temp3479;}); break;} case 221: _LL3474: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),({
char* _temp3492="H"; struct _tagged_string _temp3493; _temp3493.curr= _temp3492;
_temp3493.base= _temp3492; _temp3493.last_plus_one= _temp3492 + 2; _temp3493;}))
== 0){ Cyc_Parse_err(({ char* _temp3494="bad occurrence of heap region `H";
struct _tagged_string _temp3495; _temp3495.curr= _temp3494; _temp3495.base=
_temp3494; _temp3495.last_plus_one= _temp3494 + 33; _temp3495;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3506=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3506->name=({ struct
_tagged_string* _temp3507=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3507[ 0]=({ struct _tagged_string _temp3508= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); xprintf("`%.*s",
_temp3508.last_plus_one - _temp3508.curr, _temp3508.curr);}); _temp3507;});
_temp3506->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp3506;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3504=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3504[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3505; _temp3505.tag= Cyc_Absyn_VarType;
_temp3505.f1= tv; _temp3505;}); _temp3504;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3496=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3496[ 0]=({ struct Cyc_Stmt_tok_struct _temp3497; _temp3497.tag= Cyc_Stmt_tok;
_temp3497.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3498=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3498[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3499; _temp3499.tag= Cyc_Absyn_Region_s;
_temp3499.f1= tv; _temp3499.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3500=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3500->f1=(
void*) Cyc_Absyn_Loc_n; _temp3500->f2=({ struct _tagged_string* _temp3501=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3501[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3501;}); _temp3500;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3502=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3502[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3503; _temp3503.tag= Cyc_Absyn_RgnHandleType; _temp3503.f1=( void*) t;
_temp3503;}); _temp3502;}), 0); _temp3499.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3499;}); _temp3498;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3497;}); _temp3496;}); break;} case 222: _LL3491: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3510=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3510[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3511; _temp3511.tag= Cyc_Stmt_tok; _temp3511.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3512=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3512[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3513; _temp3513.tag= Cyc_Absyn_Cut_s; _temp3513.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3513;});
_temp3512;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3511;});
_temp3510;}); break; case 223: _LL3509: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3515=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3515[ 0]=({ struct Cyc_Stmt_tok_struct _temp3516; _temp3516.tag= Cyc_Stmt_tok;
_temp3516.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3517=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3517[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3518; _temp3518.tag= Cyc_Absyn_Splice_s;
_temp3518.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3518;}); _temp3517;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3516;}); _temp3515;}); break; case 224: _LL3514: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3520=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3520[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3521; _temp3521.tag= Cyc_Stmt_tok; _temp3521.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3522=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3522[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3523; _temp3523.tag= Cyc_Absyn_Label_s; _temp3523.f1=({
struct _tagged_string* _temp3524=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3524[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3524;}); _temp3523.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3523;});
_temp3522;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3521;});
_temp3520;}); break; case 225: _LL3519: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3526=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3526[ 0]=({ struct Cyc_Stmt_tok_struct _temp3527; _temp3527.tag= Cyc_Stmt_tok;
_temp3527.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3527;});
_temp3526;}); break; case 226: _LL3525: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3529=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3529[ 0]=({ struct Cyc_Stmt_tok_struct _temp3530; _temp3530.tag= Cyc_Stmt_tok;
_temp3530.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3530;});
_temp3529;}); break; case 227: _LL3528: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3532=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3532[ 0]=({ struct Cyc_Stmt_tok_struct _temp3533; _temp3533.tag= Cyc_Stmt_tok;
_temp3533.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3533;});
_temp3532;}); break; case 228: _LL3531: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 229: _LL3534:{ void* _temp3536= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3544; struct Cyc_Absyn_Fndecl* _temp3546; struct Cyc_Absyn_Stmt* _temp3548;
_LL3538: if(*(( int*) _temp3536) == Cyc_Parse_TopDecls_bl){ _LL3545: _temp3544=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp3536)->f1; goto _LL3539;} else{ goto
_LL3540;} _LL3540: if(*(( int*) _temp3536) == Cyc_Parse_FnDecl_bl){ _LL3547:
_temp3546=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp3536)->f1; goto _LL3541;}
else{ goto _LL3542;} _LL3542: if(*(( int*) _temp3536) == Cyc_Parse_Stmt_bl){
_LL3549: _temp3548=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3536)->f1; goto
_LL3543;} else{ goto _LL3537;} _LL3539: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3550=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3550[ 0]=({ struct Cyc_Stmt_tok_struct _temp3551; _temp3551.tag= Cyc_Stmt_tok;
_temp3551.f1= Cyc_Parse_flatten_declarations( _temp3544, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3551;}); _temp3550;}); goto _LL3537; _LL3541:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3552=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3552[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3553; _temp3553.tag= Cyc_Stmt_tok; _temp3553.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3554=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3554[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3555; _temp3555.tag= Cyc_Absyn_Fn_d;
_temp3555.f1= _temp3546; _temp3555;}); _temp3554;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3553;});
_temp3552;}); goto _LL3537; _LL3543: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3556=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3556[ 0]=({ struct Cyc_Stmt_tok_struct _temp3557; _temp3557.tag= Cyc_Stmt_tok;
_temp3557.f1= _temp3548; _temp3557;}); _temp3556;}); goto _LL3537; _LL3537:;}
break; case 230: _LL3535:{ void* _temp3559= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3567;
struct Cyc_Absyn_Fndecl* _temp3569; struct Cyc_Absyn_Stmt* _temp3571; _LL3561:
if(*(( int*) _temp3559) == Cyc_Parse_TopDecls_bl){ _LL3568: _temp3567=(( struct
Cyc_Parse_TopDecls_bl_struct*) _temp3559)->f1; goto _LL3562;} else{ goto _LL3563;}
_LL3563: if(*(( int*) _temp3559) == Cyc_Parse_FnDecl_bl){ _LL3570: _temp3569=((
struct Cyc_Parse_FnDecl_bl_struct*) _temp3559)->f1; goto _LL3564;} else{ goto
_LL3565;} _LL3565: if(*(( int*) _temp3559) == Cyc_Parse_Stmt_bl){ _LL3572:
_temp3571=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3559)->f1; goto _LL3566;}
else{ goto _LL3560;} _LL3562: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3573=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3573[ 0]=({ struct Cyc_Stmt_tok_struct _temp3574; _temp3574.tag= Cyc_Stmt_tok;
_temp3574.f1= Cyc_Parse_flatten_declarations( _temp3567, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3574;});
_temp3573;}); goto _LL3560; _LL3564: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3575=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3575[ 0]=({ struct Cyc_Stmt_tok_struct _temp3576; _temp3576.tag= Cyc_Stmt_tok;
_temp3576.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3577=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3577[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3578; _temp3578.tag= Cyc_Absyn_Fn_d;
_temp3578.f1= _temp3569; _temp3578;}); _temp3577;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3576;}); _temp3575;}); goto _LL3560; _LL3566: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3579=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3579[ 0]=({ struct Cyc_Stmt_tok_struct _temp3580; _temp3580.tag= Cyc_Stmt_tok;
_temp3580.f1= Cyc_Absyn_seq_stmt( _temp3571, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3580;}); _temp3579;}); goto _LL3560; _LL3560:;} break; case 231: _LL3558:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3582=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3582[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3583; _temp3583.tag= Cyc_BlockItem_tok; _temp3583.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3584=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3584[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3585; _temp3585.tag= Cyc_Parse_TopDecls_bl;
_temp3585.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3585;}); _temp3584;})); _temp3583;}); _temp3582;}); break;
case 232: _LL3581: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3587=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3587[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3588; _temp3588.tag= Cyc_BlockItem_tok;
_temp3588.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3589=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3589[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3590; _temp3590.tag= Cyc_Parse_Stmt_bl;
_temp3590.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3590;}); _temp3589;})); _temp3588;}); _temp3587;}); break;
case 233: _LL3586: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3592=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3592[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3593; _temp3593.tag= Cyc_BlockItem_tok;
_temp3593.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3594=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3594[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3595; _temp3595.tag= Cyc_Parse_FnDecl_bl;
_temp3595.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3595;}); _temp3594;})); _temp3593;}); _temp3592;}); break;
case 234: _LL3591: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3597=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3597[ 0]=({ struct Cyc_Stmt_tok_struct _temp3598; _temp3598.tag= Cyc_Stmt_tok;
_temp3598.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3598;}); _temp3597;}); break; case 235: _LL3596: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3600=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3600[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3601; _temp3601.tag= Cyc_Stmt_tok; _temp3601.f1= Cyc_Absyn_ifthenelse_stmt(
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
_temp3601;}); _temp3600;}); break; case 236: _LL3599: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3603=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3603[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3604; _temp3604.tag= Cyc_Stmt_tok; _temp3604.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3604;}); _temp3603;}); break; case 237: _LL3602: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3606=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3606[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3607; _temp3607.tag= Cyc_Stmt_tok; _temp3607.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3607;}); _temp3606;}); break; case 238: _LL3605:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3609=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3609[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3610; _temp3610.tag= Cyc_SwitchClauseList_tok;
_temp3610.f1= 0; _temp3610;}); _temp3609;}); break; case 239: _LL3608: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3612=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3612[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3613; _temp3613.tag= Cyc_SwitchClauseList_tok;
_temp3613.f1=({ struct Cyc_List_List* _temp3614=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3614->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3615=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3615->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3615->pat_vars= 0; _temp3615->where_clause= 0; _temp3615->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3615->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3615;}); _temp3614->tl= 0; _temp3614;});
_temp3613;}); _temp3612;}); break; case 240: _LL3611: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3617=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3617[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3618; _temp3618.tag= Cyc_SwitchClauseList_tok;
_temp3618.f1=({ struct Cyc_List_List* _temp3619=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3619->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3620=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3620->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3620->pat_vars= 0; _temp3620->where_clause= 0;
_temp3620->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3620->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3620;});
_temp3619->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3619;}); _temp3618;}); _temp3617;}); break; case 241:
_LL3616: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3622=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3622[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3623; _temp3623.tag=
Cyc_SwitchClauseList_tok; _temp3623.f1=({ struct Cyc_List_List* _temp3624=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3624->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3625=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3625->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3625->pat_vars=
0; _temp3625->where_clause= 0; _temp3625->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3625->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3625;}); _temp3624->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3624;}); _temp3623;}); _temp3622;});
break; case 242: _LL3621: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3627=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3627[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3628; _temp3628.tag= Cyc_SwitchClauseList_tok; _temp3628.f1=({ struct Cyc_List_List*
_temp3629=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3629->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3630=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3630->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3630->pat_vars=
0; _temp3630->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3630->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3630->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3630;}); _temp3629->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3629;}); _temp3628;}); _temp3627;});
break; case 243: _LL3626: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3632=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3632[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3633; _temp3633.tag= Cyc_SwitchClauseList_tok; _temp3633.f1=({ struct Cyc_List_List*
_temp3634=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3634->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3635=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3635->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3635->pat_vars=
0; _temp3635->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3635->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3635->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3635;}); _temp3634->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3634;});
_temp3633;}); _temp3632;}); break; case 244: _LL3631: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3637=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3638; _temp3638.tag= Cyc_Stmt_tok; _temp3638.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3638;}); _temp3637;}); break; case 245: _LL3636: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3640[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3641; _temp3641.tag= Cyc_Stmt_tok; _temp3641.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3641;}); _temp3640;}); break; case 246: _LL3639: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3643=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3643[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3644; _temp3644.tag= Cyc_Stmt_tok; _temp3644.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3644;}); _temp3643;}); break; case 247: _LL3642: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3646=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3646[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3647; _temp3647.tag= Cyc_Stmt_tok; _temp3647.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3647;});
_temp3646;}); break; case 248: _LL3645: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3649=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3649[ 0]=({ struct Cyc_Stmt_tok_struct _temp3650; _temp3650.tag= Cyc_Stmt_tok;
_temp3650.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3650;}); _temp3649;}); break; case 249: _LL3648:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3652=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3652[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3653; _temp3653.tag= Cyc_Stmt_tok; _temp3653.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
_temp3653;}); _temp3652;}); break; case 250: _LL3651: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3655=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3655[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3656; _temp3656.tag= Cyc_Stmt_tok; _temp3656.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3656;});
_temp3655;}); break; case 251: _LL3654: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3658=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3658[ 0]=({ struct Cyc_Stmt_tok_struct _temp3659; _temp3659.tag= Cyc_Stmt_tok;
_temp3659.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
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
_temp3659;}); _temp3658;}); break; case 252: _LL3657: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3661=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3661[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3662; _temp3662.tag= Cyc_Stmt_tok; _temp3662.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
yylsp_offset)).last_line)); _temp3662;}); _temp3661;}); break; case 253: _LL3660:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3664=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3664[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3665; _temp3665.tag= Cyc_Stmt_tok; _temp3665.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
_temp3665;}); _temp3664;}); break; case 254: _LL3663: { struct Cyc_List_List*
_temp3667= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3668; _LL3668: { struct Cyc_Absyn_Stmt* _temp3669=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3670; _LL3670: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3671=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3671[ 0]=({ struct Cyc_Stmt_tok_struct _temp3672; _temp3672.tag= Cyc_Stmt_tok;
_temp3672.f1= Cyc_Parse_flatten_declarations( _temp3667, _temp3669); _temp3672;});
_temp3671;}); break;}} case 255: _LL3666: { struct Cyc_List_List* _temp3674= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3675; _LL3675: { struct Cyc_Absyn_Stmt* _temp3676= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3677;
_LL3677: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3678=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3678[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3679; _temp3679.tag= Cyc_Stmt_tok; _temp3679.f1= Cyc_Parse_flatten_declarations(
_temp3674, _temp3676); _temp3679;}); _temp3678;}); break;}} case 256: _LL3673: {
struct Cyc_List_List* _temp3681= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3682; _LL3682: { struct Cyc_Absyn_Stmt*
_temp3683= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3684; _LL3684: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3685=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3685[ 0]=({ struct Cyc_Stmt_tok_struct _temp3686; _temp3686.tag= Cyc_Stmt_tok;
_temp3686.f1= Cyc_Parse_flatten_declarations( _temp3681, _temp3683); _temp3686;});
_temp3685;}); break;}} case 257: _LL3680: { struct Cyc_List_List* _temp3688= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3689; _LL3689: { struct Cyc_Absyn_Stmt* _temp3690= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
goto _LL3691; _LL3691: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3692=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3692[ 0]=({ struct Cyc_Stmt_tok_struct _temp3693; _temp3693.tag= Cyc_Stmt_tok;
_temp3693.f1= Cyc_Parse_flatten_declarations( _temp3688, _temp3690); _temp3693;});
_temp3692;}); break;}} case 258: _LL3687: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3695=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3695[ 0]=({ struct Cyc_Stmt_tok_struct _temp3696; _temp3696.tag= Cyc_Stmt_tok;
_temp3696.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3697=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3697[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3697;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3696;});
_temp3695;}); break; case 259: _LL3694: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3699=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3699[ 0]=({ struct Cyc_Stmt_tok_struct _temp3700; _temp3700.tag= Cyc_Stmt_tok;
_temp3700.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3700;}); _temp3699;}); break; case 260: _LL3698: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3702=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3702[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3703; _temp3703.tag= Cyc_Stmt_tok; _temp3703.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3703;}); _temp3702;}); break; case 261: _LL3701: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3705=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3705[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3706; _temp3706.tag= Cyc_Stmt_tok; _temp3706.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3706;}); _temp3705;}); break; case 262:
_LL3704: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3708=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3708[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3709; _temp3709.tag= Cyc_Stmt_tok; _temp3709.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3709;});
_temp3708;}); break; case 263: _LL3707: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3711=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3711[ 0]=({ struct Cyc_Stmt_tok_struct _temp3712; _temp3712.tag= Cyc_Stmt_tok;
_temp3712.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3712;}); _temp3711;}); break; case 264: _LL3710: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3714=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3714[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3715; _temp3715.tag= Cyc_Stmt_tok; _temp3715.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3715;}); _temp3714;}); break; case 265:
_LL3713: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3717=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3717[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3718; _temp3718.tag= Cyc_Stmt_tok; _temp3718.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3718;}); _temp3717;}); break; case 266:
_LL3716: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3720=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3720[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3721; _temp3721.tag= Cyc_Pattern_tok; _temp3721.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3721;});
_temp3720;}); break; case 267: _LL3719: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 268: _LL3722: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3724=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3724[ 0]=({ struct Cyc_Pattern_tok_struct _temp3725; _temp3725.tag= Cyc_Pattern_tok;
_temp3725.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3726=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3726[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3727; _temp3727.tag= Cyc_Absyn_Int_p;
_temp3727.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3727.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3727;}); _temp3726;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3725;}); _temp3724;}); break; case 269: _LL3723: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3729=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3729[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3730; _temp3730.tag= Cyc_Pattern_tok; _temp3730.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3731=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3731[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3732; _temp3732.tag= Cyc_Absyn_Int_p; _temp3732.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3732.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3732;}); _temp3731;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3730;}); _temp3729;}); break; case 270: _LL3728: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3734=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3734[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3735; _temp3735.tag= Cyc_Pattern_tok; _temp3735.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3736=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3736[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3737; _temp3737.tag= Cyc_Absyn_Float_p; _temp3737.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3737;}); _temp3736;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3735;});
_temp3734;}); break; case 271: _LL3733: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3739=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3739[ 0]=({ struct Cyc_Pattern_tok_struct _temp3740; _temp3740.tag= Cyc_Pattern_tok;
_temp3740.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3741=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3741[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3742; _temp3742.tag= Cyc_Absyn_Char_p;
_temp3742.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3742;}); _temp3741;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3740;}); _temp3739;}); break; case 272: _LL3738: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3744=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3744[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3745; _temp3745.tag= Cyc_Pattern_tok; _temp3745.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3745;});
_temp3744;}); break; case 273: _LL3743: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3747=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3747[ 0]=({ struct Cyc_Pattern_tok_struct _temp3748; _temp3748.tag= Cyc_Pattern_tok;
_temp3748.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3749=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3749[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3750; _temp3750.tag=
Cyc_Absyn_UnknownId_p; _temp3750.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3750;}); _temp3749;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3748;}); _temp3747;}); break; case 274: _LL3746: { struct Cyc_List_List*
_temp3752=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3753; _LL3753: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3754=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3754[ 0]=({ struct Cyc_Pattern_tok_struct _temp3755; _temp3755.tag= Cyc_Pattern_tok;
_temp3755.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3756=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3756[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3757; _temp3757.tag= Cyc_Absyn_UnknownCall_p; _temp3757.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3757.f2=
_temp3752; _temp3757.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3757;}); _temp3756;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3755;}); _temp3754;}); break;} case 275: _LL3751: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3759=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3759[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3760; _temp3760.tag= Cyc_Pattern_tok; _temp3760.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3761=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3761[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3762; _temp3762.tag= Cyc_Absyn_Tuple_p; _temp3762.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3762;}); _temp3761;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3760;});
_temp3759;}); break; case 276: _LL3758: { struct Cyc_List_List* _temp3764=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3765; _LL3765: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3766=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3766[ 0]=({ struct Cyc_Pattern_tok_struct _temp3767; _temp3767.tag= Cyc_Pattern_tok;
_temp3767.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3768=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3768[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3769; _temp3769.tag= Cyc_Absyn_UnknownFields_p; _temp3769.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3769.f2=
_temp3764; _temp3769.f3= 0; _temp3769;}); _temp3768;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3767;}); _temp3766;}); break;} case 277: _LL3763: { struct Cyc_List_List*
_temp3771=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3772; _LL3772: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3773=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3773[ 0]=({ struct Cyc_Pattern_tok_struct _temp3774; _temp3774.tag= Cyc_Pattern_tok;
_temp3774.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3775=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3775[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3776; _temp3776.tag= Cyc_Absyn_UnknownFields_p; _temp3776.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3776.f2=
_temp3771; _temp3776.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3776;}); _temp3775;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3774;}); _temp3773;}); break;} case 278: _LL3770: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3778=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3778[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3779; _temp3779.tag= Cyc_Pattern_tok; _temp3779.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3780=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3780[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3781; _temp3781.tag= Cyc_Absyn_Pointer_p;
_temp3781.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3781;}); _temp3780;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3779;}); _temp3778;}); break; case 279: _LL3777: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3783=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3783[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3784; _temp3784.tag= Cyc_Pattern_tok; _temp3784.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3785=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3785[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3786; _temp3786.tag= Cyc_Absyn_Reference_p;
_temp3786.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3787=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3787->f1=( void*) Cyc_Absyn_Loc_n;
_temp3787->f2=({ struct _tagged_string* _temp3788=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3788[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3788;});
_temp3787;}),( void*) Cyc_Absyn_VoidType, 0); _temp3786;}); _temp3785;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3784;}); _temp3783;}); break; case 280: _LL3782: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3790=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3790[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3791; _temp3791.tag= Cyc_PatternList_tok;
_temp3791.f1= 0; _temp3791;}); _temp3790;}); break; case 281: _LL3789: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3793=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3793[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3794; _temp3794.tag= Cyc_PatternList_tok;
_temp3794.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3794;}); _temp3793;}); break; case 282: _LL3792: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3796=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3796[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3797; _temp3797.tag= Cyc_PatternList_tok;
_temp3797.f1=({ struct Cyc_List_List* _temp3798=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3798->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3798->tl=
0; _temp3798;}); _temp3797;}); _temp3796;}); break; case 283: _LL3795: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3800=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3800[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3801; _temp3801.tag= Cyc_PatternList_tok;
_temp3801.f1=({ struct Cyc_List_List* _temp3802=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3802->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3802->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3802;}); _temp3801;}); _temp3800;}); break; case 284: _LL3799: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3804=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3804[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3805; _temp3805.tag= Cyc_FieldPattern_tok;
_temp3805.f1=({ struct _tuple14* _temp3806=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3806->f1= 0; _temp3806->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3806;});
_temp3805;}); _temp3804;}); break; case 285: _LL3803: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3808=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3808[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3809; _temp3809.tag= Cyc_FieldPattern_tok;
_temp3809.f1=({ struct _tuple14* _temp3810=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3810->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3810->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3810;});
_temp3809;}); _temp3808;}); break; case 286: _LL3807: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3812=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3812[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3813; _temp3813.tag= Cyc_FieldPatternList_tok;
_temp3813.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3813;}); _temp3812;}); break; case 287: _LL3811: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3815=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3815[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3816; _temp3816.tag= Cyc_FieldPatternList_tok;
_temp3816.f1=({ struct Cyc_List_List* _temp3817=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3817->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3817->tl=
0; _temp3817;}); _temp3816;}); _temp3815;}); break; case 288: _LL3814: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3819=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3819[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3820; _temp3820.tag= Cyc_FieldPatternList_tok;
_temp3820.f1=({ struct Cyc_List_List* _temp3821=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3821->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3821->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3821;}); _temp3820;}); _temp3819;}); break; case 289: _LL3818: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 290:
_LL3822: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3824=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3824[ 0]=({ struct Cyc_Exp_tok_struct
_temp3825; _temp3825.tag= Cyc_Exp_tok; _temp3825.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3825;}); _temp3824;}); break; case 291: _LL3823: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 292: _LL3826: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3828=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3828[ 0]=({ struct Cyc_Exp_tok_struct
_temp3829; _temp3829.tag= Cyc_Exp_tok; _temp3829.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
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
_temp3829;}); _temp3828;}); break; case 293: _LL3827: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3831=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3831[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3832; _temp3832.tag= Cyc_Primopopt_tok; _temp3832.f1=
0; _temp3832;}); _temp3831;}); break; case 294: _LL3830: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3834=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3834[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3835; _temp3835.tag= Cyc_Primopopt_tok; _temp3835.f1=({
struct Cyc_Core_Opt* _temp3836=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3836->v=( void*)(( void*) Cyc_Absyn_Times); _temp3836;});
_temp3835;}); _temp3834;}); break; case 295: _LL3833: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3838=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3838[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3839; _temp3839.tag= Cyc_Primopopt_tok; _temp3839.f1=({
struct Cyc_Core_Opt* _temp3840=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3840->v=( void*)(( void*) Cyc_Absyn_Div); _temp3840;});
_temp3839;}); _temp3838;}); break; case 296: _LL3837: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3842=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3842[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3843; _temp3843.tag= Cyc_Primopopt_tok; _temp3843.f1=({
struct Cyc_Core_Opt* _temp3844=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3844->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3844;});
_temp3843;}); _temp3842;}); break; case 297: _LL3841: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3846=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3846[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3847; _temp3847.tag= Cyc_Primopopt_tok; _temp3847.f1=({
struct Cyc_Core_Opt* _temp3848=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3848->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3848;});
_temp3847;}); _temp3846;}); break; case 298: _LL3845: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3850=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3850[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3851; _temp3851.tag= Cyc_Primopopt_tok; _temp3851.f1=({
struct Cyc_Core_Opt* _temp3852=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3852->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3852;});
_temp3851;}); _temp3850;}); break; case 299: _LL3849: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3854=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3854[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3855; _temp3855.tag= Cyc_Primopopt_tok; _temp3855.f1=({
struct Cyc_Core_Opt* _temp3856=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3856->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3856;});
_temp3855;}); _temp3854;}); break; case 300: _LL3853: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3858=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3858[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3859; _temp3859.tag= Cyc_Primopopt_tok; _temp3859.f1=({
struct Cyc_Core_Opt* _temp3860=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3860->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3860;});
_temp3859;}); _temp3858;}); break; case 301: _LL3857: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3862=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3862[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3863; _temp3863.tag= Cyc_Primopopt_tok; _temp3863.f1=({
struct Cyc_Core_Opt* _temp3864=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3864->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3864;});
_temp3863;}); _temp3862;}); break; case 302: _LL3861: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3866=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3866[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3867; _temp3867.tag= Cyc_Primopopt_tok; _temp3867.f1=({
struct Cyc_Core_Opt* _temp3868=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3868->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3868;});
_temp3867;}); _temp3866;}); break; case 303: _LL3865: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3870=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3870[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3871; _temp3871.tag= Cyc_Primopopt_tok; _temp3871.f1=({
struct Cyc_Core_Opt* _temp3872=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3872->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3872;});
_temp3871;}); _temp3870;}); break; case 304: _LL3869: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 305: _LL3873: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3875=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3875[ 0]=({ struct Cyc_Exp_tok_struct
_temp3876; _temp3876.tag= Cyc_Exp_tok; _temp3876.f1= Cyc_Absyn_conditional_exp(
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
_temp3876;}); _temp3875;}); break; case 306: _LL3874: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3878=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3878[ 0]=({ struct Cyc_Exp_tok_struct
_temp3879; _temp3879.tag= Cyc_Exp_tok; _temp3879.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3879;}); _temp3878;}); break; case 307: _LL3877: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3881=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3881[ 0]=({ struct Cyc_Exp_tok_struct
_temp3882; _temp3882.tag= Cyc_Exp_tok; _temp3882.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3882;}); _temp3881;}); break; case 308: _LL3880: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3884=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3884[ 0]=({ struct Cyc_Exp_tok_struct
_temp3885; _temp3885.tag= Cyc_Exp_tok; _temp3885.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3885;}); _temp3884;}); break; case 309: _LL3883: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3887=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3887[ 0]=({ struct Cyc_Exp_tok_struct
_temp3888; _temp3888.tag= Cyc_Exp_tok; _temp3888.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3888;});
_temp3887;}); break; case 310: _LL3886: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3890=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3890[ 0]=({ struct Cyc_Exp_tok_struct _temp3891; _temp3891.tag= Cyc_Exp_tok;
_temp3891.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3891;}); _temp3890;}); break; case 311: _LL3889: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 312: _LL3892: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 313: _LL3893: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3895=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3895[ 0]=({ struct Cyc_Exp_tok_struct
_temp3896; _temp3896.tag= Cyc_Exp_tok; _temp3896.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3896;}); _temp3895;}); break; case 314: _LL3894: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 315: _LL3897: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3899=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3899[ 0]=({ struct Cyc_Exp_tok_struct
_temp3900; _temp3900.tag= Cyc_Exp_tok; _temp3900.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3900;}); _temp3899;}); break; case 316: _LL3898: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 317: _LL3901: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3903=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3903[ 0]=({ struct Cyc_Exp_tok_struct
_temp3904; _temp3904.tag= Cyc_Exp_tok; _temp3904.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3904;});
_temp3903;}); break; case 318: _LL3902: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 319: _LL3905: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3907=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3907[ 0]=({ struct Cyc_Exp_tok_struct _temp3908; _temp3908.tag= Cyc_Exp_tok;
_temp3908.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3908;}); _temp3907;}); break; case 320: _LL3906: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 321: _LL3909: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3911=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3911[ 0]=({ struct Cyc_Exp_tok_struct
_temp3912; _temp3912.tag= Cyc_Exp_tok; _temp3912.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3912;});
_temp3911;}); break; case 322: _LL3910: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 323: _LL3913: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3915=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3915[ 0]=({ struct Cyc_Exp_tok_struct _temp3916; _temp3916.tag= Cyc_Exp_tok;
_temp3916.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3916;});
_temp3915;}); break; case 324: _LL3914: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3918=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3918[ 0]=({ struct Cyc_Exp_tok_struct _temp3919; _temp3919.tag= Cyc_Exp_tok;
_temp3919.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3919;});
_temp3918;}); break; case 325: _LL3917: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 326: _LL3920: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3922=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3922[ 0]=({ struct Cyc_Exp_tok_struct _temp3923; _temp3923.tag= Cyc_Exp_tok;
_temp3923.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3923;});
_temp3922;}); break; case 327: _LL3921: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3925=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3925[ 0]=({ struct Cyc_Exp_tok_struct _temp3926; _temp3926.tag= Cyc_Exp_tok;
_temp3926.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3926;});
_temp3925;}); break; case 328: _LL3924: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3928=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3928[ 0]=({ struct Cyc_Exp_tok_struct _temp3929; _temp3929.tag= Cyc_Exp_tok;
_temp3929.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3929;});
_temp3928;}); break; case 329: _LL3927: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3931=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3931[ 0]=({ struct Cyc_Exp_tok_struct _temp3932; _temp3932.tag= Cyc_Exp_tok;
_temp3932.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3932;});
_temp3931;}); break; case 330: _LL3930: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 331: _LL3933: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3935=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3935[ 0]=({ struct Cyc_Exp_tok_struct _temp3936; _temp3936.tag= Cyc_Exp_tok;
_temp3936.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3936;}); _temp3935;}); break; case 332: _LL3934: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3938=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3938[ 0]=({ struct Cyc_Exp_tok_struct
_temp3939; _temp3939.tag= Cyc_Exp_tok; _temp3939.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3939;});
_temp3938;}); break; case 333: _LL3937: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 334: _LL3940: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3942=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3942[ 0]=({ struct Cyc_Exp_tok_struct _temp3943; _temp3943.tag= Cyc_Exp_tok;
_temp3943.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3943;}); _temp3942;}); break; case 335: _LL3941: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3945=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3945[ 0]=({ struct Cyc_Exp_tok_struct
_temp3946; _temp3946.tag= Cyc_Exp_tok; _temp3946.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3946;});
_temp3945;}); break; case 336: _LL3944: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 337: _LL3947: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3949=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3949[ 0]=({ struct Cyc_Exp_tok_struct _temp3950; _temp3950.tag= Cyc_Exp_tok;
_temp3950.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3950;}); _temp3949;}); break; case 338: _LL3948: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3952=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3952[ 0]=({ struct Cyc_Exp_tok_struct
_temp3953; _temp3953.tag= Cyc_Exp_tok; _temp3953.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3953;});
_temp3952;}); break; case 339: _LL3951: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3955=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3955[ 0]=({ struct Cyc_Exp_tok_struct _temp3956; _temp3956.tag= Cyc_Exp_tok;
_temp3956.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3956;}); _temp3955;}); break; case 340: _LL3954: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 341: _LL3957: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3959=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3959[ 0]=({ struct Cyc_Exp_tok_struct
_temp3960; _temp3960.tag= Cyc_Exp_tok; _temp3960.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3960;}); _temp3959;}); break; case 342: _LL3958: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 343: _LL3961: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3963=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3963[ 0]=({ struct Cyc_Exp_tok_struct
_temp3964; _temp3964.tag= Cyc_Exp_tok; _temp3964.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3964;}); _temp3963;}); break; case 344: _LL3962: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3966=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3966[ 0]=({ struct Cyc_Exp_tok_struct
_temp3967; _temp3967.tag= Cyc_Exp_tok; _temp3967.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3967;}); _temp3966;}); break; case 345: _LL3965: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3969=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3969[ 0]=({ struct Cyc_Exp_tok_struct
_temp3970; _temp3970.tag= Cyc_Exp_tok; _temp3970.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3970;}); _temp3969;}); break; case 346: _LL3968: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3972=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3972[ 0]=({ struct Cyc_Exp_tok_struct
_temp3973; _temp3973.tag= Cyc_Exp_tok; _temp3973.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3973;}); _temp3972;}); break; case 347: _LL3971: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 348: _LL3974: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3976=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3976[ 0]=({ struct Cyc_Exp_tok_struct
_temp3977; _temp3977.tag= Cyc_Exp_tok; _temp3977.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3977;}); _temp3976;}); break; case 349: _LL3975: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3979=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3979[ 0]=({ struct Cyc_Exp_tok_struct
_temp3980; _temp3980.tag= Cyc_Exp_tok; _temp3980.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3980;}); _temp3979;}); break; case 350: _LL3978:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3982=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3982[ 0]=({ struct Cyc_Exp_tok_struct
_temp3983; _temp3983.tag= Cyc_Exp_tok; _temp3983.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3983;}); _temp3982;}); break; case 351: _LL3981: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3985=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3985[ 0]=({ struct Cyc_Exp_tok_struct
_temp3986; _temp3986.tag= Cyc_Exp_tok; _temp3986.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3986;}); _temp3985;}); break; case 352: _LL3984: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3988=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3988[ 0]=({ struct Cyc_Exp_tok_struct
_temp3989; _temp3989.tag= Cyc_Exp_tok; _temp3989.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3990=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3990[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3991; _temp3991.tag= Cyc_Absyn_Malloc_e;
_temp3991.f1= 0; _temp3991.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3991;}); _temp3990;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3989;});
_temp3988;}); break; case 353: _LL3987: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3993=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3993[ 0]=({ struct Cyc_Exp_tok_struct _temp3994; _temp3994.tag= Cyc_Exp_tok;
_temp3994.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3995=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3995[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3996; _temp3996.tag= Cyc_Absyn_Malloc_e;
_temp3996.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp3996.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp3996;}); _temp3995;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3994;}); _temp3993;}); break; case 354: _LL3992: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3998=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3998[ 0]=({ struct Cyc_Primop_tok_struct
_temp3999; _temp3999.tag= Cyc_Primop_tok; _temp3999.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp3999;}); _temp3998;}); break; case 355: _LL3997: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4001=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4001[ 0]=({ struct Cyc_Primop_tok_struct
_temp4002; _temp4002.tag= Cyc_Primop_tok; _temp4002.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp4002;}); _temp4001;}); break; case 356: _LL4000: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4004=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4004[ 0]=({ struct Cyc_Primop_tok_struct
_temp4005; _temp4005.tag= Cyc_Primop_tok; _temp4005.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp4005;}); _temp4004;}); break; case 357: _LL4003: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4007=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4007[ 0]=({ struct Cyc_Primop_tok_struct
_temp4008; _temp4008.tag= Cyc_Primop_tok; _temp4008.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp4008;}); _temp4007;}); break; case 358: _LL4006: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4010=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4010[ 0]=({ struct Cyc_Primop_tok_struct
_temp4011; _temp4011.tag= Cyc_Primop_tok; _temp4011.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp4011;}); _temp4010;}); break; case 359: _LL4009: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4013=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4013[ 0]=({ struct Cyc_Primop_tok_struct
_temp4014; _temp4014.tag= Cyc_Primop_tok; _temp4014.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp4014;}); _temp4013;}); break; case 360: _LL4012: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4016=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4016[ 0]=({ struct Cyc_Primop_tok_struct
_temp4017; _temp4017.tag= Cyc_Primop_tok; _temp4017.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp4017;}); _temp4016;}); break; case 361: _LL4015: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4019=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4019[ 0]=({ struct Cyc_Primop_tok_struct
_temp4020; _temp4020.tag= Cyc_Primop_tok; _temp4020.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp4020;}); _temp4019;}); break; case 362: _LL4018: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4022=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4022[ 0]=({ struct Cyc_Primop_tok_struct
_temp4023; _temp4023.tag= Cyc_Primop_tok; _temp4023.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp4023;}); _temp4022;}); break; case 363: _LL4021: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 364: _LL4024: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4026=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4026[ 0]=({ struct Cyc_Exp_tok_struct
_temp4027; _temp4027.tag= Cyc_Exp_tok; _temp4027.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4027;}); _temp4026;}); break; case 365: _LL4025: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4029=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4029[ 0]=({ struct Cyc_Exp_tok_struct
_temp4030; _temp4030.tag= Cyc_Exp_tok; _temp4030.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4030;}); _temp4029;}); break; case 366: _LL4028: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4032=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4032[ 0]=({ struct Cyc_Exp_tok_struct
_temp4033; _temp4033.tag= Cyc_Exp_tok; _temp4033.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4033;}); _temp4032;}); break; case 367: _LL4031: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4035=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4035[ 0]=({ struct Cyc_Exp_tok_struct
_temp4036; _temp4036.tag= Cyc_Exp_tok; _temp4036.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4037=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4037[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4037;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4036;}); _temp4035;}); break; case 368: _LL4034: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4039=(* q).f1; struct Cyc_List_List* _temp4049; struct Cyc_List_List*
_temp4051; _LL4041: if( _temp4039 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4042;}
else{ goto _LL4043;} _LL4043: if(( unsigned int) _temp4039 > 1u?*(( int*)
_temp4039) == Cyc_Absyn_Rel_n: 0){ _LL4050: _temp4049=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4039)->f1; if( _temp4049 == 0){ goto _LL4044;} else{ goto _LL4045;}} else{
goto _LL4045;} _LL4045: if(( unsigned int) _temp4039 > 1u?*(( int*) _temp4039)
== Cyc_Absyn_Abs_n: 0){ _LL4052: _temp4051=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4039)->f1; if( _temp4051 == 0){ goto _LL4046;} else{ goto _LL4047;}} else{
goto _LL4047;} _LL4047: goto _LL4048; _LL4042: goto _LL4040; _LL4044: goto
_LL4040; _LL4046: goto _LL4040; _LL4048: Cyc_Parse_err(({ char* _temp4053="struct field name is qualified";
struct _tagged_string _temp4054; _temp4054.curr= _temp4053; _temp4054.base=
_temp4053; _temp4054.last_plus_one= _temp4053 + 31; _temp4054;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL4040; _LL4040:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4055=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4055[ 0]=({ struct Cyc_Exp_tok_struct _temp4056; _temp4056.tag= Cyc_Exp_tok;
_temp4056.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4056;}); _temp4055;}); break;} case 369: _LL4038: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4058=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4058[ 0]=({ struct Cyc_Exp_tok_struct
_temp4059; _temp4059.tag= Cyc_Exp_tok; _temp4059.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4060=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4060[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4060;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4059;}); _temp4058;}); break; case 370: _LL4057: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4062=(* q).f1; struct Cyc_List_List* _temp4072; struct Cyc_List_List*
_temp4074; _LL4064: if( _temp4062 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4065;}
else{ goto _LL4066;} _LL4066: if(( unsigned int) _temp4062 > 1u?*(( int*)
_temp4062) == Cyc_Absyn_Rel_n: 0){ _LL4073: _temp4072=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4062)->f1; if( _temp4072 == 0){ goto _LL4067;} else{ goto _LL4068;}} else{
goto _LL4068;} _LL4068: if(( unsigned int) _temp4062 > 1u?*(( int*) _temp4062)
== Cyc_Absyn_Abs_n: 0){ _LL4075: _temp4074=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4062)->f1; if( _temp4074 == 0){ goto _LL4069;} else{ goto _LL4070;}} else{
goto _LL4070;} _LL4070: goto _LL4071; _LL4065: goto _LL4063; _LL4067: goto
_LL4063; _LL4069: goto _LL4063; _LL4071: Cyc_Parse_err(({ char* _temp4076="struct field is qualified with module name";
struct _tagged_string _temp4077; _temp4077.curr= _temp4076; _temp4077.base=
_temp4076; _temp4077.last_plus_one= _temp4076 + 43; _temp4077;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL4063; _LL4063:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4078=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4078[ 0]=({ struct Cyc_Exp_tok_struct _temp4079; _temp4079.tag= Cyc_Exp_tok;
_temp4079.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),(* q).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4079;}); _temp4078;}); break;} case 371: _LL4061: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4081=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4081[ 0]=({ struct Cyc_Exp_tok_struct
_temp4082; _temp4082.tag= Cyc_Exp_tok; _temp4082.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4082;}); _temp4081;}); break; case 372: _LL4080: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4084=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4084[ 0]=({ struct Cyc_Exp_tok_struct
_temp4085; _temp4085.tag= Cyc_Exp_tok; _temp4085.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4085;}); _temp4084;}); break; case 373: _LL4083: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4087=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4087[ 0]=({ struct Cyc_Exp_tok_struct
_temp4088; _temp4088.tag= Cyc_Exp_tok; _temp4088.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4089=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4089[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4090; _temp4090.tag= Cyc_Absyn_CompoundLit_e;
_temp4090.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4090.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4090;}); _temp4089;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4088;}); _temp4087;}); break; case 374: _LL4086: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4092=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4092[ 0]=({ struct Cyc_Exp_tok_struct
_temp4093; _temp4093.tag= Cyc_Exp_tok; _temp4093.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4094=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4094[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4095; _temp4095.tag= Cyc_Absyn_CompoundLit_e;
_temp4095.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4095.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4095;}); _temp4094;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4093;}); _temp4092;}); break; case 375: _LL4091: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4097=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4097[ 0]=({ struct Cyc_Exp_tok_struct
_temp4098; _temp4098.tag= Cyc_Exp_tok; _temp4098.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4099=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4099[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4100; _temp4100.tag= Cyc_Absyn_Fill_e; _temp4100.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4100;});
_temp4099;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4098;});
_temp4097;}); break; case 376: _LL4096: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4102=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4102[ 0]=({ struct Cyc_Exp_tok_struct _temp4103; _temp4103.tag= Cyc_Exp_tok;
_temp4103.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4104=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4104[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4105; _temp4105.tag= Cyc_Absyn_Codegen_e;
_temp4105.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4105;}); _temp4104;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4103;}); _temp4102;}); break; case 377: _LL4101: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4107=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4107[ 0]=({ struct Cyc_Exp_tok_struct
_temp4108; _temp4108.tag= Cyc_Exp_tok; _temp4108.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4109=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4109[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4110; _temp4110.tag= Cyc_Absyn_UnknownId_e;
_temp4110.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4110;}); _temp4109;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4108;}); _temp4107;}); break; case 378: _LL4106: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 379: _LL4111: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4113=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4113[ 0]=({ struct Cyc_Exp_tok_struct
_temp4114; _temp4114.tag= Cyc_Exp_tok; _temp4114.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4114;}); _temp4113;}); break; case 380: _LL4112: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 381: _LL4115: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4117=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4117[ 0]=({ struct Cyc_Exp_tok_struct
_temp4118; _temp4118.tag= Cyc_Exp_tok; _temp4118.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4119=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4119[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4120; _temp4120.tag=
Cyc_Absyn_UnknownId_e; _temp4120.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4120;}); _temp4119;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4118;}); _temp4117;}); break; case 382: _LL4116:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4122=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4122[ 0]=({ struct Cyc_Exp_tok_struct
_temp4123; _temp4123.tag= Cyc_Exp_tok; _temp4123.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4124=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4124[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4125; _temp4125.tag=
Cyc_Absyn_UnknownId_e; _temp4125.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4125;}); _temp4124;}), Cyc_Position_segment_of_abs((*((
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
_temp4123;}); _temp4122;}); break; case 383: _LL4121: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4127=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4127[ 0]=({ struct Cyc_Exp_tok_struct
_temp4128; _temp4128.tag= Cyc_Exp_tok; _temp4128.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4128;}); _temp4127;}); break; case 384: _LL4126: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4130=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4130[ 0]=({ struct Cyc_Exp_tok_struct
_temp4131; _temp4131.tag= Cyc_Exp_tok; _temp4131.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4132=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4132[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4133; _temp4133.tag= Cyc_Absyn_Struct_e;
_temp4133.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4133.f2= 0; _temp4133.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4133.f4=
0; _temp4133;}); _temp4132;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4131;});
_temp4130;}); break; case 385: _LL4129: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4135=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4135[ 0]=({ struct Cyc_Exp_tok_struct _temp4136; _temp4136.tag= Cyc_Exp_tok;
_temp4136.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4136;});
_temp4135;}); break; case 386: _LL4134: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4138=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4138[ 0]=({ struct Cyc_ExpList_tok_struct _temp4139; _temp4139.tag= Cyc_ExpList_tok;
_temp4139.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4139;}); _temp4138;}); break; case 387: _LL4137: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4141=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4141[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4142; _temp4142.tag= Cyc_ExpList_tok; _temp4142.f1=({ struct Cyc_List_List*
_temp4143=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4143->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4143->tl= 0; _temp4143;}); _temp4142;}); _temp4141;});
break; case 388: _LL4140: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4145=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4145[ 0]=({ struct Cyc_ExpList_tok_struct _temp4146; _temp4146.tag= Cyc_ExpList_tok;
_temp4146.f1=({ struct Cyc_List_List* _temp4147=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4147->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4147->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4147;}); _temp4146;}); _temp4145;}); break; case 389: _LL4144: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4149=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4149[ 0]=({ struct Cyc_Exp_tok_struct
_temp4150; _temp4150.tag= Cyc_Exp_tok; _temp4150.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4150;}); _temp4149;}); break; case 390: _LL4148: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4152=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4152[ 0]=({ struct Cyc_Exp_tok_struct
_temp4153; _temp4153.tag= Cyc_Exp_tok; _temp4153.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4153;}); _temp4152;}); break; case 391: _LL4151: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4155=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4155[ 0]=({ struct Cyc_Exp_tok_struct
_temp4156; _temp4156.tag= Cyc_Exp_tok; _temp4156.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4156;}); _temp4155;}); break; case 392: _LL4154: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4158=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4158[ 0]=({ struct Cyc_Exp_tok_struct
_temp4159; _temp4159.tag= Cyc_Exp_tok; _temp4159.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4159;}); _temp4158;}); break; case 393: _LL4157: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4161=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4161[ 0]=({ struct Cyc_QualId_tok_struct
_temp4162; _temp4162.tag= Cyc_QualId_tok; _temp4162.f1=({ struct _tuple1*
_temp4163=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4163->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4165=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4165[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4166; _temp4166.tag= Cyc_Absyn_Rel_n; _temp4166.f1= 0; _temp4166;});
_temp4165;}); _temp4163->f2=({ struct _tagged_string* _temp4164=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4164[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp4164;}); _temp4163;}); _temp4162;}); _temp4161;}); break; case 394: _LL4160:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4167:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
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
yylsp_offset)).text=({ char* _temp4169=""; struct _tagged_string _temp4170;
_temp4170.curr= _temp4169; _temp4170.base= _temp4169; _temp4170.last_plus_one=
_temp4169 + 1; _temp4170;});} else{(*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset + yylen) - 1)).last_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_column=(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset +
yylen) - 1)).last_column;} yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull(
395u, yyn)]; yystate= Cyc_yypgoto[ _check_known_subscript_notnull( 108u, yyn -
124)] +*(( short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset); if(( yystate >=
0? yystate <= 4272: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4273u,
yystate)] ==*(( short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset): 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4273u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 108u, yyn - 124)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 754u, yystate)]; if( yyn > - 32768? yyn < 4272:
0){ int sze= 0; struct _tagged_string msg; int x; int count; count= 0; for( x=
yyn < 0? - yyn: 0; x < 232u / sizeof( char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4273u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]) + 15, count ++);}} msg=({
unsigned int _temp4171=( unsigned int)( sze + 15); char* _temp4172=( char*)
GC_malloc_atomic( sizeof( char) * _temp4171); struct _tagged_string _temp4174={
_temp4172, _temp4172, _temp4172 + _temp4171};{ unsigned int _temp4173= _temp4171;
unsigned int i; for( i= 0; i < _temp4173; i ++){ _temp4172[ i]='\000';}};
_temp4174;}); Cyc_String_strcpy( msg,({ char* _temp4175="parse error"; struct
_tagged_string _temp4176; _temp4176.curr= _temp4175; _temp4176.base= _temp4175;
_temp4176.last_plus_one= _temp4175 + 12; _temp4176;})); if( count < 5){ count= 0;
for( x= yyn < 0? - yyn: 0; x < 232u / sizeof( char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4273u, x + yyn)] == x){ Cyc_String_strcat( msg,
count == 0?({ char* _temp4177=", expecting `"; struct _tagged_string _temp4178;
_temp4178.curr= _temp4177; _temp4178.base= _temp4177; _temp4178.last_plus_one=
_temp4177 + 14; _temp4178;}):({ char* _temp4179=" or `"; struct _tagged_string
_temp4180; _temp4180.curr= _temp4179; _temp4180.base= _temp4179; _temp4180.last_plus_one=
_temp4179 + 6; _temp4180;})); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 232u, x)]); Cyc_String_strcat( msg,({ char*
_temp4181="'"; struct _tagged_string _temp4182; _temp4182.curr= _temp4181;
_temp4182.base= _temp4181; _temp4182.last_plus_one= _temp4181 + 2; _temp4182;}));
count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(({ char* _temp4183="parse error";
struct _tagged_string _temp4184; _temp4184.curr= _temp4183; _temp4184.base=
_temp4183; _temp4184.last_plus_one= _temp4183 + 12; _temp4184;}));}} goto
yyerrlab1; yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;}
Cyc_yychar= - 2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if(
yyssp_offset == 0){ return 1;} yyvsp_offset --; yystate=( int)*(( short*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)(
yyss, sizeof( short), -- yyssp_offset); yylsp_offset --; yyerrhandle: yyn=( int)
Cyc_yypact[ _check_known_subscript_notnull( 754u, yystate)]; if( yyn == - 32768){
goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4272)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4273u, yyn)] != 1){ goto yyerrdefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4273u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrpop;}} if( yyn == 753){ return 0;}*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; goto yynewstate;}
void Cyc_yyprint( int i, void* v){ void* _temp4200= v; struct _tuple16*
_temp4220; struct _tuple16 _temp4222; int _temp4223; void* _temp4225; char
_temp4227; short _temp4229; struct _tagged_string _temp4231; struct Cyc_Core_Opt*
_temp4233; struct Cyc_Core_Opt* _temp4235; struct Cyc_Core_Opt _temp4237; struct
_tagged_string* _temp4238; struct _tuple1* _temp4240; struct _tuple1 _temp4242;
struct _tagged_string* _temp4243; void* _temp4245; _LL4202: if( _temp4200 == Cyc_Okay_tok){
goto _LL4203;} else{ goto _LL4204;} _LL4204: if(*(( void**) _temp4200) == Cyc_Int_tok){
_LL4221: _temp4220=(( struct Cyc_Int_tok_struct*) _temp4200)->f1; _temp4222=*
_temp4220; _LL4226: _temp4225= _temp4222.f1; goto _LL4224; _LL4224: _temp4223=
_temp4222.f2; goto _LL4205;} else{ goto _LL4206;} _LL4206: if(*(( void**)
_temp4200) == Cyc_Char_tok){ _LL4228: _temp4227=(( struct Cyc_Char_tok_struct*)
_temp4200)->f1; goto _LL4207;} else{ goto _LL4208;} _LL4208: if(*(( void**)
_temp4200) == Cyc_Short_tok){ _LL4230: _temp4229=(( struct Cyc_Short_tok_struct*)
_temp4200)->f1; goto _LL4209;} else{ goto _LL4210;} _LL4210: if(*(( void**)
_temp4200) == Cyc_String_tok){ _LL4232: _temp4231=(( struct Cyc_String_tok_struct*)
_temp4200)->f1; goto _LL4211;} else{ goto _LL4212;} _LL4212: if(*(( void**)
_temp4200) == Cyc_Stringopt_tok){ _LL4234: _temp4233=(( struct Cyc_Stringopt_tok_struct*)
_temp4200)->f1; if( _temp4233 == 0){ goto _LL4213;} else{ goto _LL4214;}} else{
goto _LL4214;} _LL4214: if(*(( void**) _temp4200) == Cyc_Stringopt_tok){ _LL4236:
_temp4235=(( struct Cyc_Stringopt_tok_struct*) _temp4200)->f1; if( _temp4235 ==
0){ goto _LL4216;} else{ _temp4237=* _temp4235; _LL4239: _temp4238=( struct
_tagged_string*) _temp4237.v; goto _LL4215;}} else{ goto _LL4216;} _LL4216: if(*((
void**) _temp4200) == Cyc_QualId_tok){ _LL4241: _temp4240=(( struct Cyc_QualId_tok_struct*)
_temp4200)->f1; _temp4242=* _temp4240; _LL4246: _temp4245= _temp4242.f1; goto
_LL4244; _LL4244: _temp4243= _temp4242.f2; goto _LL4217;} else{ goto _LL4218;}
_LL4218: goto _LL4219; _LL4203: fprintf( Cyc_Stdio_stderr,"ok"); goto _LL4201;
_LL4205: fprintf( Cyc_Stdio_stderr,"%d", _temp4223); goto _LL4201; _LL4207:
fprintf( Cyc_Stdio_stderr,"%c",( int) _temp4227); goto _LL4201; _LL4209: fprintf(
Cyc_Stdio_stderr,"%ds",( int) _temp4229); goto _LL4201; _LL4211:({ struct
_tagged_string _temp4247= _temp4231; fprintf( Cyc_Stdio_stderr,"\"%.*s\"",
_temp4247.last_plus_one - _temp4247.curr, _temp4247.curr);}); goto _LL4201;
_LL4213: fprintf( Cyc_Stdio_stderr,"null"); goto _LL4201; _LL4215:({ struct
_tagged_string _temp4248=* _temp4238; fprintf( Cyc_Stdio_stderr,"\"%.*s\"",
_temp4248.last_plus_one - _temp4248.curr, _temp4248.curr);}); goto _LL4201;
_LL4217: { struct Cyc_List_List* _temp4249= 0; goto _LL4250; _LL4250:{ void*
_temp4251= _temp4245; struct Cyc_List_List* _temp4259; struct Cyc_List_List*
_temp4261; _LL4253: if(( unsigned int) _temp4251 > 1u?*(( int*) _temp4251) ==
Cyc_Absyn_Rel_n: 0){ _LL4260: _temp4259=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4251)->f1; goto _LL4254;} else{ goto _LL4255;} _LL4255: if(( unsigned int)
_temp4251 > 1u?*(( int*) _temp4251) == Cyc_Absyn_Abs_n: 0){ _LL4262: _temp4261=((
struct Cyc_Absyn_Abs_n_struct*) _temp4251)->f1; goto _LL4256;} else{ goto
_LL4257;} _LL4257: if( _temp4251 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4258;}
else{ goto _LL4252;} _LL4254: _temp4249= _temp4259; goto _LL4252; _LL4256:
_temp4249= _temp4261; goto _LL4252; _LL4258: goto _LL4252; _LL4252:;} for( 0;
_temp4249 != 0; _temp4249=(( struct Cyc_List_List*) _check_null( _temp4249))->tl){({
struct _tagged_string _temp4263=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp4249))->hd); fprintf( Cyc_Stdio_stderr,"%.*s::", _temp4263.last_plus_one
- _temp4263.curr, _temp4263.curr);});}({ struct _tagged_string _temp4264=*
_temp4243; fprintf( Cyc_Stdio_stderr,"%.*s::", _temp4264.last_plus_one -
_temp4264.curr, _temp4264.curr);}); goto _LL4201;} _LL4219: fprintf( Cyc_Stdio_stderr,"?");
goto _LL4201; _LL4201:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4265=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4265->v=(
void*) Cyc_Lexing_from_file( f); _temp4265;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}