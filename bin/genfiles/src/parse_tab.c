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
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att=
2; static const int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att=
4; static const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int
Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att= 6; static const
int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att= 8; static
const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att= 10;
static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
12; static const int Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att=
14; static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int
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
_tagged_string Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern void Cyc_Lex_register_typedef(
struct _tuple1* s); extern void Cyc_Lex_enter_namespace( struct _tagged_string*);
extern void Cyc_Lex_leave_namespace(); extern void Cyc_Lex_enter_using( struct
_tuple1*); extern void Cyc_Lex_leave_using(); static const int Cyc_Parse_Struct_su=
0; static const int Cyc_Parse_Union_su= 1; typedef void* Cyc_Parse_struct_or_union_t;
static const int Cyc_Parse_TopDecls_bl= 0; struct Cyc_Parse_TopDecls_bl_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Parse_Stmt_bl= 1;
struct Cyc_Parse_Stmt_bl_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Parse_FnDecl_bl= 2; struct Cyc_Parse_FnDecl_bl_struct{ int tag;
struct Cyc_Absyn_Fndecl* f1; } ; typedef void* Cyc_Parse_blockitem_t; static
const int Cyc_Parse_Signed_spec= 0; struct Cyc_Parse_Signed_spec_struct{ int tag;
struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Unsigned_spec= 1;
struct Cyc_Parse_Unsigned_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Short_spec= 2; struct Cyc_Parse_Short_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Long_spec=
3; struct Cyc_Parse_Long_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Type_spec= 4; struct Cyc_Parse_Type_spec_struct{
int tag; void* f1; struct Cyc_Position_Segment* f2; } ; static const int Cyc_Parse_Decl_spec=
5; struct Cyc_Parse_Decl_spec_struct{ int tag; struct Cyc_Absyn_Decl* f1; } ;
typedef void* Cyc_Parse_type_specifier_t; static const int Cyc_Parse_Typedef_sc=
0; static const int Cyc_Parse_Extern_sc= 1; static const int Cyc_Parse_ExternC_sc=
2; static const int Cyc_Parse_Static_sc= 3; static const int Cyc_Parse_Auto_sc=
4; static const int Cyc_Parse_Register_sc= 5; static const int Cyc_Parse_Abstract_sc=
6; typedef void* Cyc_Parse_storage_class_t; struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual tq; struct Cyc_List_List*
type_specs; int is_inline; struct Cyc_List_List* attributes; } ; typedef struct
Cyc_Parse_Declaration_spec* Cyc_Parse_decl_spec_t; struct Cyc_Parse_Declarator{
struct _tuple1* id; struct Cyc_List_List* tms; } ; typedef struct Cyc_Parse_Declarator*
Cyc_Parse_declarator_t; struct Cyc_Parse_Abstractdeclarator{ struct Cyc_List_List*
tms; } ; typedef struct Cyc_Parse_Abstractdeclarator* Cyc_Parse_abstractdeclarator_t;
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
static char _temp9[ 9u]="fastcall"; static char _temp12[ 9u]="noreturn"; static
char _temp15[ 6u]="const"; static char _temp18[ 8u]="aligned"; static char
_temp21[ 7u]="packed"; static char _temp24[ 7u]="shared"; static char _temp27[ 7u]="unused";
static char _temp30[ 5u]="weak"; static char _temp33[ 10u]="dllimport"; static
char _temp36[ 10u]="dllexport"; static char _temp39[ 23u]="no_instrument_function";
static char _temp42[ 12u]="constructor"; static char _temp45[ 11u]="destructor";
static char _temp48[ 22u]="no_check_memory_usage"; struct _tuple8 Cyc_Parse_att_map[
16u]={{{ _temp3, _temp3, _temp3 + 8u},( void*) 0u},{{ _temp6, _temp6, _temp6 + 6u},(
void*) 1u},{{ _temp9, _temp9, _temp9 + 9u},( void*) 2u},{{ _temp12, _temp12,
_temp12 + 9u},( void*) 3u},{{ _temp15, _temp15, _temp15 + 6u},( void*) 4u},{{
_temp18, _temp18, _temp18 + 8u},( void*)& Cyc_Parse_att_aligned},{{ _temp21,
_temp21, _temp21 + 7u},( void*) 5u},{{ _temp24, _temp24, _temp24 + 7u},( void*)
7u},{{ _temp27, _temp27, _temp27 + 7u},( void*) 8u},{{ _temp30, _temp30, _temp30
+ 5u},( void*) 9u},{{ _temp33, _temp33, _temp33 + 10u},( void*) 10u},{{ _temp36,
_temp36, _temp36 + 10u},( void*) 11u},{{ _temp39, _temp39, _temp39 + 23u},( void*)
12u},{{ _temp42, _temp42, _temp42 + 12u},( void*) 13u},{{ _temp45, _temp45,
_temp45 + 11u},( void*) 14u},{{ _temp48, _temp48, _temp48 + 22u},( void*) 15u}};
static void Cyc_Parse_err( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_string msg, struct Cyc_Position_Segment* sg){
Cyc_Parse_err( msg, sg);( void) _throw(( void*) Cyc_Position_Exit);} static void
Cyc_Parse_warn( struct _tagged_string msg, struct Cyc_Position_Segment* sg){({
struct _tagged_string _temp49= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp50= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n",
_temp49.last_plus_one - _temp49.curr, _temp49.curr, _temp50.last_plus_one -
_temp50.curr, _temp50.curr);}); return;} static void* Cyc_Parse_unimp( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp51= msg; xprintf("%.*s unimplemented", _temp51.last_plus_one
- _temp51.curr, _temp51.curr);}), sg);} static void Cyc_Parse_unimp2( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp52= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp53= msg;
fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp52.last_plus_one - _temp52.curr, _temp52.curr, _temp53.last_plus_one -
_temp53.curr, _temp53.curr);}); return;} struct _tuple9{ struct _tuple7* f1;
struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct Cyc_Core_Opt*
_temp56; struct _tuple7* _temp58; struct _tuple9 _temp54=* field_info; _LL59:
_temp58= _temp54.f1; goto _LL57; _LL57: _temp56= _temp54.f2; goto _LL55; _LL55:
if((* _temp58).f4 != 0){ Cyc_Parse_err(({ char* _temp60="bad type params in struct field";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 32; _temp61;}), loc);}{ struct _tuple1* _temp62=(*
_temp58).f1; goto _LL63; _LL63:{ void* _temp64=(* _temp62).f1; struct Cyc_List_List*
_temp74; struct Cyc_List_List* _temp76; _LL66: if(( unsigned int) _temp64 > 1u?*((
int*) _temp64) == Cyc_Absyn_Rel_n: 0){ _LL75: _temp74=(( struct Cyc_Absyn_Rel_n_struct*)
_temp64)->f1; if( _temp74 == 0){ goto _LL67;} else{ goto _LL68;}} else{ goto
_LL68;} _LL68: if(( unsigned int) _temp64 > 1u?*(( int*) _temp64) == Cyc_Absyn_Abs_n:
0){ _LL77: _temp76=(( struct Cyc_Absyn_Abs_n_struct*) _temp64)->f1; if( _temp76
== 0){ goto _LL69;} else{ goto _LL70;}} else{ goto _LL70;} _LL70: if( _temp64 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL71;} else{ goto _LL72;} _LL72: goto _LL73;
_LL67: goto _LL65; _LL69: goto _LL65; _LL71: goto _LL65; _LL73: Cyc_Parse_err(({
char* _temp78="struct field cannot be qualified with a module name"; struct
_tagged_string _temp79; _temp79.curr= _temp78; _temp79.base= _temp78; _temp79.last_plus_one=
_temp78 + 52; _temp79;}), loc); goto _LL65; _LL65:;}{ struct Cyc_List_List*
_temp80=(* _temp58).f5; goto _LL81; _LL81: return({ struct Cyc_Absyn_Structfield*
_temp82=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp82->name=(* _temp62).f2; _temp82->tq=(* _temp58).f2; _temp82->type=( void*)(*
_temp58).f3; _temp82->width= _temp56; _temp82->attributes= _temp80; _temp82;});}}}
struct _tuple10{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple10* field){ struct _tuple10
_temp85; struct Cyc_List_List* _temp86; void* _temp88; struct Cyc_Absyn_Tqual
_temp90; struct Cyc_Core_Opt* _temp92; struct _tuple10* _temp83= field; _temp85=*
_temp83; _LL93: _temp92= _temp85.f1; goto _LL91; _LL91: _temp90= _temp85.f2;
goto _LL89; _LL89: _temp88= _temp85.f3; goto _LL87; _LL87: _temp86= _temp85.f4;
goto _LL84; _LL84: { struct Cyc_Core_Opt* _temp94= 0; goto _LL95; _LL95: if(
_temp92 != 0){ _temp94=({ struct Cyc_Core_Opt* _temp96=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp96->v=( void*)(*(( struct _tuple1*)((
struct Cyc_Core_Opt*) _check_null( _temp92))->v)).f2; _temp96;});{ void* _temp97=(*((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp92))->v)).f1; struct
Cyc_List_List* _temp107; struct Cyc_List_List* _temp109; _LL99: if((
unsigned int) _temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Rel_n: 0){ _LL108:
_temp107=(( struct Cyc_Absyn_Rel_n_struct*) _temp97)->f1; if( _temp107 == 0){
goto _LL100;} else{ goto _LL101;}} else{ goto _LL101;} _LL101: if(( unsigned int)
_temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Abs_n: 0){ _LL110: _temp109=((
struct Cyc_Absyn_Abs_n_struct*) _temp97)->f1; if( _temp109 == 0){ goto _LL102;}
else{ goto _LL103;}} else{ goto _LL103;} _LL103: if( _temp97 ==( void*) Cyc_Absyn_Loc_n){
goto _LL104;} else{ goto _LL105;} _LL105: goto _LL106; _LL100: goto _LL98;
_LL102: goto _LL98; _LL104: goto _LL98; _LL106: Cyc_Parse_err(({ char* _temp111="parameter cannot be qualified with a module name";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 49; _temp112;}), loc); goto _LL98; _LL98:;}}
if( _temp86 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ char* _temp113="parameter should have no type parameters";
struct _tagged_string _temp114; _temp114.curr= _temp113; _temp114.base= _temp113;
_temp114.last_plus_one= _temp113 + 41; _temp114;}), loc);} return({ struct
_tuple2* _temp115=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp115->f1= _temp94; _temp115->f2= _temp90; _temp115->f3= _temp88; _temp115;});}}
static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){
return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp116=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp116[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp117; _temp117.tag= Cyc_Parse_Type_spec; _temp117.f1=(
void*) t; _temp117.f2= loc; _temp117;}); _temp116;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp118= t; struct Cyc_Absyn_Exp* _temp124; struct Cyc_Absyn_Tqual
_temp126; void* _temp128; _LL120: if(( unsigned int) _temp118 > 4u?*(( int*)
_temp118) == Cyc_Absyn_ArrayType: 0){ _LL129: _temp128=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f1; goto _LL127; _LL127: _temp126=(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f2; goto _LL125; _LL125: _temp124=(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f3; goto _LL121;} else{ goto _LL122;} _LL122: goto _LL123; _LL121:
return Cyc_Absyn_starb_typ( _temp128,( void*) Cyc_Absyn_HeapRgn, _temp126,
_temp124 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp130=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp130[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp131; _temp131.tag= Cyc_Absyn_Upper_b;
_temp131.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp124); _temp131;});
_temp130;})); _LL123: return t; _LL119:;} static void Cyc_Parse_arg_array2ptr(
struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple11*
_temp132=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp132->f1=(*
t).f2; _temp132->f2=(* t).f3; _temp132;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp133=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp159;
int _temp161; struct Cyc_Absyn_Exp* _temp163; struct Cyc_Core_Opt* _temp165;
struct Cyc_Core_Opt* _temp167; struct Cyc_Absyn_Pat* _temp169; struct Cyc_List_List*
_temp171; struct Cyc_Absyn_Fndecl* _temp173; struct Cyc_Absyn_Structdecl*
_temp175; struct Cyc_Absyn_Uniondecl* _temp177; struct Cyc_Absyn_Tuniondecl*
_temp179; struct Cyc_Absyn_Typedefdecl* _temp181; struct Cyc_Absyn_Enumdecl*
_temp183; struct Cyc_List_List* _temp185; struct _tagged_string* _temp187;
struct Cyc_List_List* _temp189; struct _tuple1* _temp191; struct Cyc_List_List*
_temp193; _LL135: if(*(( int*) _temp133) == Cyc_Absyn_Var_d){ _LL160: _temp159=((
struct Cyc_Absyn_Var_d_struct*) _temp133)->f1; goto _LL136;} else{ goto _LL137;}
_LL137: if(*(( int*) _temp133) == Cyc_Absyn_Let_d){ _LL170: _temp169=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f1; goto _LL168; _LL168: _temp167=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f2; goto _LL166; _LL166: _temp165=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f3; goto _LL164; _LL164: _temp163=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f4; goto _LL162; _LL162: _temp161=(( struct
Cyc_Absyn_Let_d_struct*) _temp133)->f5; goto _LL138;} else{ goto _LL139;} _LL139:
if(*(( int*) _temp133) == Cyc_Absyn_Letv_d){ _LL172: _temp171=(( struct Cyc_Absyn_Letv_d_struct*)
_temp133)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if(*(( int*) _temp133)
== Cyc_Absyn_Fn_d){ _LL174: _temp173=(( struct Cyc_Absyn_Fn_d_struct*) _temp133)->f1;
goto _LL142;} else{ goto _LL143;} _LL143: if(*(( int*) _temp133) == Cyc_Absyn_Struct_d){
_LL176: _temp175=(( struct Cyc_Absyn_Struct_d_struct*) _temp133)->f1; goto
_LL144;} else{ goto _LL145;} _LL145: if(*(( int*) _temp133) == Cyc_Absyn_Union_d){
_LL178: _temp177=(( struct Cyc_Absyn_Union_d_struct*) _temp133)->f1; goto _LL146;}
else{ goto _LL147;} _LL147: if(*(( int*) _temp133) == Cyc_Absyn_Tunion_d){
_LL180: _temp179=(( struct Cyc_Absyn_Tunion_d_struct*) _temp133)->f1; goto
_LL148;} else{ goto _LL149;} _LL149: if(*(( int*) _temp133) == Cyc_Absyn_Typedef_d){
_LL182: _temp181=(( struct Cyc_Absyn_Typedef_d_struct*) _temp133)->f1; goto
_LL150;} else{ goto _LL151;} _LL151: if(*(( int*) _temp133) == Cyc_Absyn_Enum_d){
_LL184: _temp183=(( struct Cyc_Absyn_Enum_d_struct*) _temp133)->f1; goto _LL152;}
else{ goto _LL153;} _LL153: if(*(( int*) _temp133) == Cyc_Absyn_Namespace_d){
_LL188: _temp187=(( struct Cyc_Absyn_Namespace_d_struct*) _temp133)->f1; goto
_LL186; _LL186: _temp185=(( struct Cyc_Absyn_Namespace_d_struct*) _temp133)->f2;
goto _LL154;} else{ goto _LL155;} _LL155: if(*(( int*) _temp133) == Cyc_Absyn_Using_d){
_LL192: _temp191=(( struct Cyc_Absyn_Using_d_struct*) _temp133)->f1; goto _LL190;
_LL190: _temp189=(( struct Cyc_Absyn_Using_d_struct*) _temp133)->f2; goto _LL156;}
else{ goto _LL157;} _LL157: if(*(( int*) _temp133) == Cyc_Absyn_ExternC_d){
_LL194: _temp193=(( struct Cyc_Absyn_ExternC_d_struct*) _temp133)->f1; goto
_LL158;} else{ goto _LL134;} _LL136: if( _temp159->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp195="initializers are not allowed in parameter declarations"; struct
_tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
_temp196.last_plus_one= _temp195 + 55; _temp196;}), x->loc);}{ void* _temp197=(*
_temp159->name).f1; struct Cyc_List_List* _temp207; struct Cyc_List_List*
_temp209; _LL199: if( _temp197 ==( void*) Cyc_Absyn_Loc_n){ goto _LL200;} else{
goto _LL201;} _LL201: if(( unsigned int) _temp197 > 1u?*(( int*) _temp197) ==
Cyc_Absyn_Rel_n: 0){ _LL208: _temp207=(( struct Cyc_Absyn_Rel_n_struct*)
_temp197)->f1; if( _temp207 == 0){ goto _LL202;} else{ goto _LL203;}} else{ goto
_LL203;} _LL203: if(( unsigned int) _temp197 > 1u?*(( int*) _temp197) == Cyc_Absyn_Abs_n:
0){ _LL210: _temp209=(( struct Cyc_Absyn_Abs_n_struct*) _temp197)->f1; if(
_temp209 == 0){ goto _LL204;} else{ goto _LL205;}} else{ goto _LL205;} _LL205:
goto _LL206; _LL200: goto _LL198; _LL202: goto _LL198; _LL204: goto _LL198;
_LL206: Cyc_Parse_err(({ char* _temp211="module names not allowed on parameter declarations";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 51; _temp212;}), x->loc); goto _LL198; _LL198:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp159->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp213=*(*
_temp159->name).f2; xprintf("%.*s is not listed as a parameter", _temp213.last_plus_one
- _temp213.curr, _temp213.curr);}), x->loc);} return;} _LL138: decl_kind=({ char*
_temp214="let declaration"; struct _tagged_string _temp215; _temp215.curr=
_temp214; _temp215.base= _temp214; _temp215.last_plus_one= _temp214 + 16;
_temp215;}); goto _LL134; _LL140: decl_kind=({ char* _temp216="let declaration";
struct _tagged_string _temp217; _temp217.curr= _temp216; _temp217.base= _temp216;
_temp217.last_plus_one= _temp216 + 16; _temp217;}); goto _LL134; _LL142:
decl_kind=({ char* _temp218="function declaration"; struct _tagged_string
_temp219; _temp219.curr= _temp218; _temp219.base= _temp218; _temp219.last_plus_one=
_temp218 + 21; _temp219;}); goto _LL134; _LL144: decl_kind=({ char* _temp220="struct declaration";
struct _tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 19; _temp221;}); goto _LL134; _LL146:
decl_kind=({ char* _temp222="union declaration"; struct _tagged_string _temp223;
_temp223.curr= _temp222; _temp223.base= _temp222; _temp223.last_plus_one=
_temp222 + 18; _temp223;}); goto _LL134; _LL148: decl_kind=({ char* _temp224="tunion declaration";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 19; _temp225;}); goto _LL134; _LL150:
decl_kind=({ char* _temp226="typedef"; struct _tagged_string _temp227; _temp227.curr=
_temp226; _temp227.base= _temp226; _temp227.last_plus_one= _temp226 + 8;
_temp227;}); goto _LL134; _LL152: decl_kind=({ char* _temp228="enum declaration";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 17; _temp229;}); goto _LL134; _LL154:
decl_kind=({ char* _temp230="namespace declaration"; struct _tagged_string
_temp231; _temp231.curr= _temp230; _temp231.base= _temp230; _temp231.last_plus_one=
_temp230 + 22; _temp231;}); goto _LL134; _LL156: decl_kind=({ char* _temp232="using declaration";
struct _tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 18; _temp233;}); goto _LL134; _LL158:
decl_kind=({ char* _temp234="extern C declaration"; struct _tagged_string
_temp235; _temp235.curr= _temp234; _temp235.base= _temp234; _temp235.last_plus_one=
_temp234 + 21; _temp235;}); goto _LL134; _LL134:;}(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp236= decl_kind; xprintf("%.*s appears in parameter type",
_temp236.last_plus_one - _temp236.curr, _temp236.curr);}), x->loc); return;}
struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_string* x){ struct _tuple12 _temp239; struct Cyc_Position_Segment*
_temp240; struct Cyc_List_List* _temp242; struct _tuple12* _temp237= env;
_temp239=* _temp237; _LL243: _temp242= _temp239.f1; goto _LL241; _LL241:
_temp240= _temp239.f2; goto _LL238; _LL238: if( _temp242 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp244=* x; xprintf("missing type for parameter %.*s",
_temp244.last_plus_one - _temp244.curr, _temp244.curr);}), _temp240);}{ void*
_temp245=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp242))->hd)->r; struct Cyc_Absyn_Vardecl* _temp251; _LL247: if(*(( int*)
_temp245) == Cyc_Absyn_Var_d){ _LL252: _temp251=(( struct Cyc_Absyn_Var_d_struct*)
_temp245)->f1; goto _LL248;} else{ goto _LL249;} _LL249: goto _LL250; _LL248:{
void* _temp253=(* _temp251->name).f1; struct Cyc_List_List* _temp263; struct Cyc_List_List*
_temp265; _LL255: if( _temp253 ==( void*) Cyc_Absyn_Loc_n){ goto _LL256;} else{
goto _LL257;} _LL257: if(( unsigned int) _temp253 > 1u?*(( int*) _temp253) ==
Cyc_Absyn_Rel_n: 0){ _LL264: _temp263=(( struct Cyc_Absyn_Rel_n_struct*)
_temp253)->f1; if( _temp263 == 0){ goto _LL258;} else{ goto _LL259;}} else{ goto
_LL259;} _LL259: if(( unsigned int) _temp253 > 1u?*(( int*) _temp253) == Cyc_Absyn_Abs_n:
0){ _LL266: _temp265=(( struct Cyc_Absyn_Abs_n_struct*) _temp253)->f1; if(
_temp265 == 0){ goto _LL260;} else{ goto _LL261;}} else{ goto _LL261;} _LL261:
goto _LL262; _LL256: goto _LL254; _LL258: goto _LL254; _LL260: goto _LL254;
_LL262: Cyc_Parse_err(({ char* _temp267="module name not allowed on parameter";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 37; _temp268;}), _temp240); goto _LL254;
_LL254:;} if( Cyc_String_zstrptrcmp((* _temp251->name).f2, x) == 0){ return({
struct _tuple2* _temp269=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp269[ 0]=({ struct _tuple2 _temp270; _temp270.f1=({ struct Cyc_Core_Opt*
_temp271=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp271[ 0]=({ struct Cyc_Core_Opt _temp272; _temp272.v=( void*)(* _temp251->name).f2;
_temp272;}); _temp271;}); _temp270.f2= _temp251->tq; _temp270.f3=( void*)
_temp251->type; _temp270;}); _temp269;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp273=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp273->f1=(( struct Cyc_List_List*) _check_null( _temp242))->tl; _temp273->f2=
_temp240; _temp273;}), x);} _LL250: return(( struct _tuple2*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ char*
_temp274="non-variable declaration"; struct _tagged_string _temp275; _temp275.curr=
_temp274; _temp275.base= _temp274; _temp275.last_plus_one= _temp274 + 25;
_temp275;}),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp242))->hd)->loc); _LL246:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp276= tm; int _temp282; struct Cyc_Position_Segment* _temp284; struct
Cyc_List_List* _temp286; _LL278: if(( unsigned int) _temp276 > 1u?*(( int*)
_temp276) == Cyc_Absyn_TypeParams_mod: 0){ _LL287: _temp286=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp276)->f1; goto _LL285; _LL285: _temp284=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp276)->f2; goto _LL283; _LL283: _temp282=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp276)->f3; goto _LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279:
return 1; _LL281: return 0; _LL277:;} static void* Cyc_Parse_id2type( struct
_tagged_string s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,({ char*
_temp288="`H"; struct _tagged_string _temp289; _temp289.curr= _temp288; _temp289.base=
_temp288; _temp289.last_plus_one= _temp288 + 3; _temp289;})) == 0){ return( void*)
Cyc_Absyn_HeapRgn;} else{ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp290=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp290[ 0]=({ struct Cyc_Absyn_VarType_struct _temp291; _temp291.tag= Cyc_Absyn_VarType;
_temp291.f1=({ struct Cyc_Absyn_Tvar* _temp292=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp292->name=({ struct
_tagged_string* _temp293=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp293[ 0]= s; _temp293;}); _temp292->kind= k; _temp292;});
_temp291;}); _temp290;});}} static struct Cyc_Absyn_Tvar* Cyc_Parse_typ2tvar(
struct Cyc_Position_Segment* loc, void* t){ void* _temp294= t; struct Cyc_Absyn_Tvar*
_temp300; _LL296: if(( unsigned int) _temp294 > 4u?*(( int*) _temp294) == Cyc_Absyn_VarType:
0){ _LL301: _temp300=(( struct Cyc_Absyn_VarType_struct*) _temp294)->f1; goto
_LL297;} else{ goto _LL298;} _LL298: goto _LL299; _LL297: return _temp300;
_LL299: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ char* _temp302="expecting a list of type variables, not types";
struct _tagged_string _temp303; _temp303.curr= _temp302; _temp303.base= _temp302;
_temp303.last_plus_one= _temp302 + 46; _temp303;}), loc); _LL295:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp304=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp304[ 0]=({ struct Cyc_Absyn_VarType_struct _temp305; _temp305.tag= Cyc_Absyn_VarType;
_temp305.f1= pr; _temp305;}); _temp304;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp306=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp312; _LL308:
if(( unsigned int) _temp306 > 1u?*(( int*) _temp306) == Cyc_Absyn_Function_mod:
0){ _LL313: _temp312=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp306)->f1;
goto _LL309;} else{ goto _LL310;} _LL310: goto _LL311; _LL309: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp314= _temp312; struct Cyc_Core_Opt* _temp320; int _temp322;
struct Cyc_List_List* _temp324; struct Cyc_Position_Segment* _temp326; struct
Cyc_List_List* _temp328; _LL316: if(*(( int*) _temp314) == Cyc_Absyn_WithTypes){
_LL325: _temp324=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f1; goto
_LL323; _LL323: _temp322=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f2;
goto _LL321; _LL321: _temp320=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f3;
goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp314) == Cyc_Absyn_NoTypes){
_LL329: _temp328=(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f1; goto _LL327;
_LL327: _temp326=(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f2; goto _LL319;}
else{ goto _LL315;} _LL317: Cyc_Parse_warn(({ char* _temp330="function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 92; _temp331;}), loc); return tms; _LL319:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp328, tds);{ struct _tuple12* _temp333=({ struct _tuple12* _temp332=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp332->f1= tds; _temp332->f2=
loc; _temp332;}); goto _LL334; _LL334: return({ struct Cyc_List_List* _temp335=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp335->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp336=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp336[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp337; _temp337.tag= Cyc_Absyn_Function_mod;
_temp337.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp338=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp338[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp339; _temp339.tag= Cyc_Absyn_WithTypes;
_temp339.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_string*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp333, _temp328); _temp339.f2= 0; _temp339.f3= 0;
_temp339;}); _temp338;})); _temp337;}); _temp336;})); _temp335->tl= 0; _temp335;});}
_LL315:;} else{ return({ struct Cyc_List_List* _temp340=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp340->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( tms))->hd); _temp340->tl= Cyc_Parse_oldstyle2newstyle(((
struct Cyc_List_List*) _check_null( tms))->tl, tds, loc); _temp340;});} _LL311:
return({ struct Cyc_List_List* _temp341=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp341->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd); _temp341->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*)
_check_null( tms))->tl, tds, loc); _temp341;}); _LL307:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp342=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp342->id= d->id; _temp342->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp342;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp343=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL345: if( _temp343 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL346;} else{ goto _LL347;} _LL347: if(
_temp343 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL348;} else{ goto _LL349;}
_LL349: if( _temp343 ==( void*) Cyc_Parse_Static_sc){ goto _LL350;} else{ goto
_LL351;} _LL351: goto _LL352; _LL346: sc=( void*) Cyc_Absyn_Extern; goto _LL344;
_LL348: sc=( void*) Cyc_Absyn_ExternC; goto _LL344; _LL350: sc=( void*) Cyc_Absyn_Static;
goto _LL344; _LL352: Cyc_Parse_err(({ char* _temp353="bad storage class on function";
struct _tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 30; _temp354;}), loc); goto _LL344; _LL344:;}}{
struct Cyc_Core_Opt* _temp357; void* _temp359; struct _tuple5 _temp355= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL360: _temp359= _temp355.f1; goto _LL358; _LL358: _temp357=
_temp355.f2; goto _LL356; _LL356: { struct Cyc_List_List* _temp363; struct Cyc_List_List*
_temp365; void* _temp367; struct Cyc_Absyn_Tqual _temp369; struct _tuple6
_temp361= Cyc_Parse_apply_tms( tq, _temp359, atts, d->tms); _LL370: _temp369=
_temp361.f1; goto _LL368; _LL368: _temp367= _temp361.f2; goto _LL366; _LL366:
_temp365= _temp361.f3; goto _LL364; _LL364: _temp363= _temp361.f4; goto _LL362;
_LL362: if( _temp357 != 0){ Cyc_Parse_warn(({ char* _temp371="nested type declaration within function prototype";
struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 50; _temp372;}), loc);} if( _temp365 != 0){
Cyc_Parse_warn(({ char* _temp373="bad type params, ignoring"; struct
_tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 26; _temp374;}), loc);}{ void* _temp375=
_temp367; struct Cyc_Absyn_FnInfo _temp381; struct Cyc_List_List* _temp383; int
_temp385; struct Cyc_List_List* _temp387; void* _temp389; struct Cyc_Core_Opt*
_temp391; struct Cyc_List_List* _temp393; _LL377: if(( unsigned int) _temp375 >
4u?*(( int*) _temp375) == Cyc_Absyn_FnType: 0){ _LL382: _temp381=(( struct Cyc_Absyn_FnType_struct*)
_temp375)->f1; _LL394: _temp393= _temp381.tvars; goto _LL392; _LL392: _temp391=
_temp381.effect; goto _LL390; _LL390: _temp389=( void*) _temp381.ret_typ; goto
_LL388; _LL388: _temp387= _temp381.args; goto _LL386; _LL386: _temp385= _temp381.varargs;
goto _LL384; _LL384: _temp383= _temp381.attributes; goto _LL378;} else{ goto
_LL379;} _LL379: goto _LL380; _LL378: { struct Cyc_List_List* _temp395=(( struct
Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_fnargspec_to_arg, loc, _temp387); goto _LL396; _LL396: return({ struct
Cyc_Absyn_Fndecl* _temp397=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct
Cyc_Absyn_Fndecl)); _temp397->sc=( void*) sc; _temp397->name= d->id; _temp397->tvs=
_temp393; _temp397->is_inline= is_inline; _temp397->effect= _temp391; _temp397->ret_type=(
void*) _temp389; _temp397->args= _temp395; _temp397->varargs= _temp385; _temp397->body=
body; _temp397->cached_typ= 0; _temp397->param_vardecls= 0; _temp397->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp383, _temp363); _temp397;});} _LL380: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp398="declarator is not a function prototype"; struct _tagged_string
_temp399; _temp399.curr= _temp398; _temp399.base= _temp398; _temp399.last_plus_one=
_temp398 + 39; _temp399;}), loc); _LL376:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(({
char* _temp400="missing argument variable in function prototype"; struct
_tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 48; _temp401;}), loc); return({ struct
_tuple4* _temp402=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp402->f1=({ struct _tagged_string* _temp403=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp403[ 0]=({ char* _temp404="?";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 2; _temp405;}); _temp403;}); _temp402->f2=(*
t).f2; _temp402->f3=(* t).f3; _temp402;});} else{ return({ struct _tuple4*
_temp406=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp406->f1=(
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp406->f2=(* t).f2; _temp406->f3=(* t).f3; _temp406;});}} static char
_temp409[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1={ _temp409, _temp409, _temp409 + 52u};
static char _temp412[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2={ _temp412, _temp412, _temp412 + 63u};
static char _temp415[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3={ _temp415, _temp415, _temp415 + 50u};
static char _temp418[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4={ _temp418, _temp418, _temp418 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp419=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp433; void* _temp435; struct Cyc_Position_Segment*
_temp437; struct Cyc_Position_Segment* _temp439; struct Cyc_Position_Segment*
_temp441; struct Cyc_Position_Segment* _temp443; struct Cyc_Absyn_Decl* _temp445;
_LL421: if(*(( int*) _temp419) == Cyc_Parse_Type_spec){ _LL436: _temp435=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp419)->f1; goto _LL434; _LL434: _temp433=((
struct Cyc_Parse_Type_spec_struct*) _temp419)->f2; goto _LL422;} else{ goto
_LL423;} _LL423: if(*(( int*) _temp419) == Cyc_Parse_Signed_spec){ _LL438:
_temp437=(( struct Cyc_Parse_Signed_spec_struct*) _temp419)->f1; goto _LL424;}
else{ goto _LL425;} _LL425: if(*(( int*) _temp419) == Cyc_Parse_Unsigned_spec){
_LL440: _temp439=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp419)->f1; goto
_LL426;} else{ goto _LL427;} _LL427: if(*(( int*) _temp419) == Cyc_Parse_Short_spec){
_LL442: _temp441=(( struct Cyc_Parse_Short_spec_struct*) _temp419)->f1; goto
_LL428;} else{ goto _LL429;} _LL429: if(*(( int*) _temp419) == Cyc_Parse_Long_spec){
_LL444: _temp443=(( struct Cyc_Parse_Long_spec_struct*) _temp419)->f1; goto
_LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp419) == Cyc_Parse_Decl_spec){
_LL446: _temp445=(( struct Cyc_Parse_Decl_spec_struct*) _temp419)->f1; goto
_LL432;} else{ goto _LL420;} _LL422: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp433);} last_loc= _temp433; seen_type= 1; t= _temp435; goto _LL420; _LL424:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp437);} if( seen_type){ Cyc_Parse_err(({
char* _temp447="signed qualifier must come before type"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 39; _temp448;}), _temp437);} last_loc= _temp437; seen_sign= 1; sgn=(
void*) Cyc_Absyn_Signed; goto _LL420; _LL426: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4,
_temp439);} if( seen_type){ Cyc_Parse_err(({ char* _temp449="signed qualifier must come before type";
struct _tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 39; _temp450;}), _temp439);} last_loc=
_temp439; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL420; _LL428:
if( seen_size){ Cyc_Parse_err(({ char* _temp451="integral size may appear only once within a type specifier";
struct _tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 59; _temp452;}), _temp441);} if( seen_type){
Cyc_Parse_err(({ char* _temp453="short modifier must come before base type";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 42; _temp454;}), _temp441);} last_loc=
_temp441; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL420; _LL430: if(
seen_type){ Cyc_Parse_err(({ char* _temp455="long modifier must come before base type";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 41; _temp456;}), _temp443);} if( seen_size){
void* _temp457= sz; _LL459: if( _temp457 ==( void*) Cyc_Absyn_B4){ goto _LL460;}
else{ goto _LL461;} _LL461: goto _LL462; _LL460: sz=( void*) Cyc_Absyn_B8; goto
_LL458; _LL462: Cyc_Parse_err(({ char* _temp463="extra long in type specifier";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 29; _temp464;}), _temp443); goto _LL458;
_LL458:;} last_loc= _temp443; seen_size= 1; goto _LL420; _LL432: last_loc=
_temp445->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp465=(
void*) _temp445->r; struct Cyc_Absyn_Structdecl* _temp477; struct Cyc_Absyn_Tuniondecl*
_temp479; struct Cyc_Absyn_Uniondecl* _temp481; struct Cyc_Absyn_Enumdecl*
_temp483; _LL467: if(*(( int*) _temp465) == Cyc_Absyn_Struct_d){ _LL478:
_temp477=(( struct Cyc_Absyn_Struct_d_struct*) _temp465)->f1; goto _LL468;}
else{ goto _LL469;} _LL469: if(*(( int*) _temp465) == Cyc_Absyn_Tunion_d){
_LL480: _temp479=(( struct Cyc_Absyn_Tunion_d_struct*) _temp465)->f1; goto
_LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp465) == Cyc_Absyn_Union_d){
_LL482: _temp481=(( struct Cyc_Absyn_Union_d_struct*) _temp465)->f1; goto _LL472;}
else{ goto _LL473;} _LL473: if(*(( int*) _temp465) == Cyc_Absyn_Enum_d){ _LL484:
_temp483=(( struct Cyc_Absyn_Enum_d_struct*) _temp465)->f1; goto _LL474;} else{
goto _LL475;} _LL475: goto _LL476; _LL468: { struct Cyc_List_List* _temp485=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp477->tvs); goto _LL486; _LL486: t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp487=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp487[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp488; _temp488.tag= Cyc_Absyn_StructType;
_temp488.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp477->name))->v); _temp488.f2= _temp485; _temp488.f3= 0;
_temp488;}); _temp487;}); if( _temp477->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp489=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp489->v=( void*) _temp445; _temp489;});} goto _LL466;} _LL470: { struct Cyc_List_List*
_temp490=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp479->tvs);
goto _LL491; _LL491: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp492=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp492[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp493; _temp493.tag= Cyc_Absyn_TunionType;
_temp493.f1=({ struct Cyc_Absyn_TunionInfo _temp494; _temp494.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp495=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp495[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp496; _temp496.tag= Cyc_Absyn_KnownTunion;
_temp496.f1= _temp479; _temp496;}); _temp495;})); _temp494.targs= _temp490;
_temp494.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp494;}); _temp493;});
_temp492;}); if( _temp479->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp497=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp497->v=( void*) _temp445; _temp497;});} goto _LL466;} _LL472: { struct Cyc_List_List*
_temp498=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp481->tvs);
goto _LL499; _LL499: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp500=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp500[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp501; _temp501.tag= Cyc_Absyn_UnionType;
_temp501.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp481->name))->v); _temp501.f2= _temp498; _temp501.f3= 0;
_temp501;}); _temp500;}); if( _temp481->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp502=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp502->v=( void*) _temp445; _temp502;});} goto _LL466;} _LL474: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp503=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp503[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp504; _temp504.tag= Cyc_Absyn_EnumType; _temp504.f1=
_temp483->name; _temp504.f2= 0; _temp504;}); _temp503;}); declopt=({ struct Cyc_Core_Opt*
_temp505=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp505->v=( void*) _temp445; _temp505;}); goto _LL466; _LL476:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp506="bad declaration within type specifier"; struct _tagged_string
_temp507; _temp507.curr= _temp506; _temp507.base= _temp506; _temp507.last_plus_one=
_temp506 + 38; _temp507;}), _temp445->loc); goto _LL466; _LL466:;}} else{ Cyc_Parse_err(
Cyc_Parse_msg3, _temp445->loc);} goto _LL420; _LL420:;} if( ! seen_type){ if( !
seen_sign? ! seen_size: 0){ Cyc_Parse_err(({ char* _temp508="missing type withing specifier";
struct _tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 31; _temp509;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp510=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp510[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp511; _temp511.tag= Cyc_Absyn_IntType; _temp511.f1=( void*) sgn; _temp511.f2=(
void*) sz; _temp511;}); _temp510;});} else{ if( seen_sign){ void* _temp512= t;
void* _temp518; void* _temp520; _LL514: if(( unsigned int) _temp512 > 4u?*(( int*)
_temp512) == Cyc_Absyn_IntType: 0){ _LL521: _temp520=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp512)->f1; goto _LL519; _LL519: _temp518=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp512)->f2; goto _LL515;} else{ goto _LL516;} _LL516: goto _LL517; _LL515: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp522=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp522[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp523; _temp523.tag= Cyc_Absyn_IntType; _temp523.f1=( void*) sgn; _temp523.f2=(
void*) _temp518; _temp523;}); _temp522;}); goto _LL513; _LL517: Cyc_Parse_err(({
char* _temp524="sign specification on non-integral type"; struct _tagged_string
_temp525; _temp525.curr= _temp524; _temp525.base= _temp524; _temp525.last_plus_one=
_temp524 + 40; _temp525;}), last_loc); goto _LL513; _LL513:;} if( seen_size){
void* _temp526= t; void* _temp532; void* _temp534; _LL528: if(( unsigned int)
_temp526 > 4u?*(( int*) _temp526) == Cyc_Absyn_IntType: 0){ _LL535: _temp534=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp526)->f1; goto _LL533; _LL533:
_temp532=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp526)->f2; goto _LL529;}
else{ goto _LL530;} _LL530: goto _LL531; _LL529: t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp536=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp536[ 0]=({ struct Cyc_Absyn_IntType_struct _temp537; _temp537.tag= Cyc_Absyn_IntType;
_temp537.f1=( void*) _temp534; _temp537.f2=( void*) sz; _temp537;}); _temp536;});
goto _LL527; _LL531: Cyc_Parse_err(({ char* _temp538="size qualifier on non-integral type";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 36; _temp539;}), last_loc); goto _LL527;
_LL527:;}} return({ struct _tuple5 _temp540; _temp540.f1= t; _temp540.f2=
declopt; _temp540;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp541=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL542; _LL542: { struct _tuple1* _temp543= _temp541->id; goto _LL544;
_LL544: { struct Cyc_List_List* _temp547; struct Cyc_List_List* _temp549; void*
_temp551; struct Cyc_Absyn_Tqual _temp553; struct _tuple6 _temp545= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp541->tms); _LL554: _temp553= _temp545.f1; goto _LL552;
_LL552: _temp551= _temp545.f2; goto _LL550; _LL550: _temp549= _temp545.f3; goto
_LL548; _LL548: _temp547= _temp545.f4; goto _LL546; _LL546: return({ struct Cyc_List_List*
_temp555=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp555->hd=( void*)({ struct _tuple7* _temp556=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp556->f1= _temp543; _temp556->f2= _temp553;
_temp556->f3= _temp551; _temp556->f4= _temp549; _temp556->f5= _temp547; _temp556;});
_temp555->tl= Cyc_Parse_apply_tmss( _temp553, t,(( struct Cyc_List_List*)
_check_null( ds))->tl, shared_atts); _temp555;});}}}} static struct _tuple6 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp557; _temp557.f1= tq; _temp557.f2=
t; _temp557.f3= 0; _temp557.f4= atts; _temp557;});}{ void* _temp558=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp572;
void* _temp574; int _temp576; struct Cyc_Position_Segment* _temp578; struct Cyc_List_List*
_temp580; struct Cyc_Absyn_Tqual _temp582; void* _temp584; void* _temp586;
struct Cyc_List_List* _temp588; struct Cyc_Position_Segment* _temp590; _LL560:
if( _temp558 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL561;} else{ goto _LL562;}
_LL562: if(( unsigned int) _temp558 > 1u?*(( int*) _temp558) == Cyc_Absyn_ConstArray_mod:
0){ _LL573: _temp572=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp558)->f1;
goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int) _temp558 > 1u?*((
int*) _temp558) == Cyc_Absyn_Function_mod: 0){ _LL575: _temp574=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp558)->f1; goto _LL565;} else{ goto
_LL566;} _LL566: if(( unsigned int) _temp558 > 1u?*(( int*) _temp558) == Cyc_Absyn_TypeParams_mod:
0){ _LL581: _temp580=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp558)->f1;
goto _LL579; _LL579: _temp578=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp558)->f2; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp558)->f3; goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int)
_temp558 > 1u?*(( int*) _temp558) == Cyc_Absyn_Pointer_mod: 0){ _LL587: _temp586=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp558)->f1; goto _LL585;
_LL585: _temp584=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp558)->f2;
goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp558)->f3;
goto _LL569;} else{ goto _LL570;} _LL570: if(( unsigned int) _temp558 > 1u?*((
int*) _temp558) == Cyc_Absyn_Attributes_mod: 0){ _LL591: _temp590=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp558)->f1; goto _LL589; _LL589: _temp588=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp558)->f2; goto _LL571;} else{ goto _LL559;} _LL561: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp592=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp592[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp593; _temp593.tag= Cyc_Absyn_ArrayType;
_temp593.f1=( void*) t; _temp593.f2= tq; _temp593.f3= 0; _temp593;}); _temp592;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL563: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp594=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp594[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp595; _temp595.tag= Cyc_Absyn_ArrayType;
_temp595.f1=( void*) t; _temp595.f2= tq; _temp595.f3=( struct Cyc_Absyn_Exp*)
_temp572; _temp595;}); _temp594;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL565: { void* _temp596= _temp574; struct Cyc_Core_Opt* _temp602;
int _temp604; struct Cyc_List_List* _temp606; struct Cyc_Position_Segment*
_temp608; struct Cyc_List_List* _temp610; _LL598: if(*(( int*) _temp596) == Cyc_Absyn_WithTypes){
_LL607: _temp606=(( struct Cyc_Absyn_WithTypes_struct*) _temp596)->f1; goto
_LL605; _LL605: _temp604=(( struct Cyc_Absyn_WithTypes_struct*) _temp596)->f2;
goto _LL603; _LL603: _temp602=(( struct Cyc_Absyn_WithTypes_struct*) _temp596)->f3;
goto _LL599;} else{ goto _LL600;} _LL600: if(*(( int*) _temp596) == Cyc_Absyn_NoTypes){
_LL611: _temp610=(( struct Cyc_Absyn_NoTypes_struct*) _temp596)->f1; goto _LL609;
_LL609: _temp608=(( struct Cyc_Absyn_NoTypes_struct*) _temp596)->f2; goto _LL601;}
else{ goto _LL597;} _LL599: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List*
fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts;
for( 0; as != 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp612=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp612->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp612->tl= fn_atts; _temp612;});} else{ new_atts=({ struct Cyc_List_List*
_temp613=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp613->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp613->tl= new_atts; _temp613;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp614=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp620; struct Cyc_Position_Segment*
_temp622; struct Cyc_List_List* _temp624; _LL616: if(( unsigned int) _temp614 >
1u?*(( int*) _temp614) == Cyc_Absyn_TypeParams_mod: 0){ _LL625: _temp624=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp614)->f1; goto _LL623; _LL623:
_temp622=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp614)->f2; goto _LL621;
_LL621: _temp620=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp614)->f3; goto
_LL617;} else{ goto _LL618;} _LL618: goto _LL619; _LL617: typvars= _temp624; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL615; _LL619: goto _LL615;
_LL615:;} if(((( ! _temp604? _temp606 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp606))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp606))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp606))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp606= 0;
_temp604= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp606);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp602, t, _temp606, _temp604, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL601:( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ char*
_temp626="function declaration without parameter types"; struct _tagged_string
_temp627; _temp627.curr= _temp626; _temp627.base= _temp626; _temp627.last_plus_one=
_temp626 + 45; _temp627;}), _temp608)); _LL597:;} _LL567: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0){ return({ struct _tuple6 _temp628; _temp628.f1= tq;
_temp628.f2= t; _temp628.f3= _temp580; _temp628.f4= atts; _temp628;});}( void)
_throw((( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(({ char* _temp629="type parameters must appear before function arguments in declarator";
struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base= _temp629;
_temp630.last_plus_one= _temp629 + 68; _temp630;}), _temp578)); _LL569: { void*
_temp631= _temp586; struct Cyc_Absyn_Exp* _temp639; struct Cyc_Absyn_Exp*
_temp641; _LL633: if(( unsigned int) _temp631 > 1u?*(( int*) _temp631) == Cyc_Absyn_NonNullable_ps:
0){ _LL640: _temp639=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp631)->f1;
goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int) _temp631 > 1u?*((
int*) _temp631) == Cyc_Absyn_Nullable_ps: 0){ _LL642: _temp641=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp631)->f1; goto _LL636;} else{ goto _LL637;} _LL637: if( _temp631 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL638;} else{ goto _LL632;} _LL634: return Cyc_Parse_apply_tms(
_temp582, Cyc_Absyn_atb_typ( t, _temp584, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp643=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp643[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp644; _temp644.tag= Cyc_Absyn_Upper_b;
_temp644.f1= _temp639; _temp644;}); _temp643;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL636: return Cyc_Parse_apply_tms( _temp582, Cyc_Absyn_starb_typ(
t, _temp584, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp645=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp645[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp646; _temp646.tag= Cyc_Absyn_Upper_b;
_temp646.f1= _temp641; _temp646;}); _temp645;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL638: return Cyc_Parse_apply_tms( _temp582, Cyc_Absyn_tagged_typ(
t, _temp584, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL632:;}
_LL571: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp588),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL559:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp649; void* _temp651; struct _tuple5 _temp647= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL652: _temp651= _temp647.f1; goto _LL650; _LL650: _temp649=
_temp647.f2; goto _LL648; _LL648: if( _temp649 != 0){ Cyc_Parse_warn(({ char*
_temp653="ignoring nested type declaration(s) in specifier list"; struct
_tagged_string _temp654; _temp654.curr= _temp653; _temp654.base= _temp653;
_temp654.last_plus_one= _temp653 + 54; _temp654;}), loc);} return _temp651;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp655=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp655[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp656; _temp656.tag= Cyc_Absyn_Decl_s;
_temp656.f1= d; _temp656.f2= s; _temp656;}); _temp655;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(({ char* _temp657="inline is only allowed on function definitions";
struct _tagged_string _temp658; _temp658.curr= _temp657; _temp658.base= _temp657;
_temp658.last_plus_one= _temp657 + 47; _temp658;}), loc);} if( tss == 0){ Cyc_Parse_err(({
char* _temp659="missing type specifiers in declaration"; struct _tagged_string
_temp660; _temp660.curr= _temp659; _temp660.base= _temp659; _temp660.last_plus_one=
_temp659 + 39; _temp660;}), loc); return 0;} if( ds->sc != 0){ void* _temp661=(
void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v; _LL663: if( _temp661 ==(
void*) Cyc_Parse_Typedef_sc){ goto _LL664;} else{ goto _LL665;} _LL665: if(
_temp661 ==( void*) Cyc_Parse_Extern_sc){ goto _LL666;} else{ goto _LL667;}
_LL667: if( _temp661 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL668;} else{ goto
_LL669;} _LL669: if( _temp661 ==( void*) Cyc_Parse_Static_sc){ goto _LL670;}
else{ goto _LL671;} _LL671: if( _temp661 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL672;} else{ goto _LL673;} _LL673: if( _temp661 ==( void*) Cyc_Parse_Register_sc){
goto _LL674;} else{ goto _LL675;} _LL675: if( _temp661 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL676;} else{ goto _LL662;} _LL664: istypedef= 1; goto _LL662; _LL666: s=(
void*) Cyc_Absyn_Extern; goto _LL662; _LL668: s=( void*) Cyc_Absyn_ExternC; goto
_LL662; _LL670: s=( void*) Cyc_Absyn_Static; goto _LL662; _LL672: s=( void*) Cyc_Absyn_Public;
goto _LL662; _LL674: s=( void*) Cyc_Absyn_Public; goto _LL662; _LL676: s=( void*)
Cyc_Absyn_Abstract; goto _LL662; _LL662:;}{ struct Cyc_List_List* _temp679;
struct Cyc_List_List* _temp681; struct _tuple0 _temp677=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL682: _temp681= _temp677.f1;
goto _LL680; _LL680: _temp679= _temp677.f2; goto _LL678; _LL678: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp679; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp683= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL684; _LL684: if( _temp681 == 0){ struct Cyc_Core_Opt* _temp687; void*
_temp689; struct _tuple5 _temp685= _temp683; _LL690: _temp689= _temp685.f1; goto
_LL688; _LL688: _temp687= _temp685.f2; goto _LL686; _LL686: if( _temp687 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp687))->v;{ void* _temp691=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp703; struct Cyc_Absyn_Structdecl* _temp705; struct Cyc_Absyn_Uniondecl*
_temp707; struct Cyc_Absyn_Tuniondecl* _temp709; _LL693: if(*(( int*) _temp691)
== Cyc_Absyn_Enum_d){ _LL704: _temp703=(( struct Cyc_Absyn_Enum_d_struct*)
_temp691)->f1; goto _LL694;} else{ goto _LL695;} _LL695: if(*(( int*) _temp691)
== Cyc_Absyn_Struct_d){ _LL706: _temp705=(( struct Cyc_Absyn_Struct_d_struct*)
_temp691)->f1; goto _LL696;} else{ goto _LL697;} _LL697: if(*(( int*) _temp691)
== Cyc_Absyn_Union_d){ _LL708: _temp707=(( struct Cyc_Absyn_Union_d_struct*)
_temp691)->f1; goto _LL698;} else{ goto _LL699;} _LL699: if(*(( int*) _temp691)
== Cyc_Absyn_Tunion_d){ _LL710: _temp709=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp691)->f1; goto _LL700;} else{ goto _LL701;} _LL701: goto _LL702; _LL694:(
void*)( _temp703->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(({ char*
_temp711="bad attributes on enum"; struct _tagged_string _temp712; _temp712.curr=
_temp711; _temp712.base= _temp711; _temp712.last_plus_one= _temp711 + 23;
_temp712;}), loc);} goto _LL692; _LL696:( void*)( _temp705->sc=( void*) s);
_temp705->attributes= atts; goto _LL692; _LL698:( void*)( _temp707->sc=( void*)
s); _temp707->attributes= atts; goto _LL692; _LL700:( void*)( _temp709->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(({ char* _temp713="bad attributes on tunion";
struct _tagged_string _temp714; _temp714.curr= _temp713; _temp714.base= _temp713;
_temp714.last_plus_one= _temp713 + 25; _temp714;}), loc);} goto _LL692; _LL702:
Cyc_Parse_err(({ char* _temp715="bad declaration"; struct _tagged_string
_temp716; _temp716.curr= _temp715; _temp716.base= _temp715; _temp716.last_plus_one=
_temp715 + 16; _temp716;}), loc); return 0; _LL692:;} return({ struct Cyc_List_List*
_temp717=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp717->hd=( void*) d; _temp717->tl= 0; _temp717;});} else{ void* _temp718=
_temp689; struct Cyc_Absyn_Structdecl** _temp732; struct Cyc_List_List* _temp734;
struct _tuple1* _temp736; struct Cyc_Absyn_TunionInfo _temp738; void* _temp740;
struct Cyc_List_List* _temp742; void* _temp744; struct Cyc_Absyn_Tuniondecl*
_temp746; struct Cyc_Absyn_TunionInfo _temp748; void* _temp750; struct Cyc_List_List*
_temp752; void* _temp754; struct Cyc_Absyn_UnknownTunionInfo _temp756; int
_temp758; struct _tuple1* _temp760; struct Cyc_Absyn_Uniondecl** _temp762;
struct Cyc_List_List* _temp764; struct _tuple1* _temp766; struct Cyc_Absyn_Enumdecl*
_temp768; struct _tuple1* _temp770; _LL720: if(( unsigned int) _temp718 > 4u?*((
int*) _temp718) == Cyc_Absyn_StructType: 0){ _LL737: _temp736=(( struct Cyc_Absyn_StructType_struct*)
_temp718)->f1; goto _LL735; _LL735: _temp734=(( struct Cyc_Absyn_StructType_struct*)
_temp718)->f2; goto _LL733; _LL733: _temp732=(( struct Cyc_Absyn_StructType_struct*)
_temp718)->f3; goto _LL721;} else{ goto _LL722;} _LL722: if(( unsigned int)
_temp718 > 4u?*(( int*) _temp718) == Cyc_Absyn_TunionType: 0){ _LL739: _temp738=((
struct Cyc_Absyn_TunionType_struct*) _temp718)->f1; _LL745: _temp744=( void*)
_temp738.tunion_info; if(*(( int*) _temp744) == Cyc_Absyn_KnownTunion){ _LL747:
_temp746=(( struct Cyc_Absyn_KnownTunion_struct*) _temp744)->f1; goto _LL743;}
else{ goto _LL724;} _LL743: _temp742= _temp738.targs; goto _LL741; _LL741:
_temp740=( void*) _temp738.rgn; goto _LL723;} else{ goto _LL724;} _LL724: if((
unsigned int) _temp718 > 4u?*(( int*) _temp718) == Cyc_Absyn_TunionType: 0){
_LL749: _temp748=(( struct Cyc_Absyn_TunionType_struct*) _temp718)->f1; _LL755:
_temp754=( void*) _temp748.tunion_info; if(*(( int*) _temp754) == Cyc_Absyn_UnknownTunion){
_LL757: _temp756=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp754)->f1;
_LL761: _temp760= _temp756.name; goto _LL759; _LL759: _temp758= _temp756.is_xtunion;
goto _LL753;} else{ goto _LL726;} _LL753: _temp752= _temp748.targs; goto _LL751;
_LL751: _temp750=( void*) _temp748.rgn; goto _LL725;} else{ goto _LL726;} _LL726:
if(( unsigned int) _temp718 > 4u?*(( int*) _temp718) == Cyc_Absyn_UnionType: 0){
_LL767: _temp766=(( struct Cyc_Absyn_UnionType_struct*) _temp718)->f1; goto
_LL765; _LL765: _temp764=(( struct Cyc_Absyn_UnionType_struct*) _temp718)->f2;
goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_UnionType_struct*) _temp718)->f3;
goto _LL727;} else{ goto _LL728;} _LL728: if(( unsigned int) _temp718 > 4u?*((
int*) _temp718) == Cyc_Absyn_EnumType: 0){ _LL771: _temp770=(( struct Cyc_Absyn_EnumType_struct*)
_temp718)->f1; goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_EnumType_struct*)
_temp718)->f2; goto _LL729;} else{ goto _LL730;} _LL730: goto _LL731; _LL721: {
struct Cyc_List_List* _temp772=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp734);
goto _LL773; _LL773: { struct Cyc_Absyn_Structdecl* _temp776=({ struct Cyc_Absyn_Structdecl*
_temp774=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp774->sc=( void*) s; _temp774->name=({ struct Cyc_Core_Opt* _temp775=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp775->v=(
void*)(( struct _tuple1*) _check_null( _temp736)); _temp775;}); _temp774->tvs=
_temp772; _temp774->fields= 0; _temp774->attributes= 0; _temp774;}); goto _LL777;
_LL777: if( atts != 0){ Cyc_Parse_err(({ char* _temp778="bad attributes on struct";
struct _tagged_string _temp779; _temp779.curr= _temp778; _temp779.base= _temp778;
_temp779.last_plus_one= _temp778 + 25; _temp779;}), loc);} return({ struct Cyc_List_List*
_temp780=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp780->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp781=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp781[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp782; _temp782.tag= Cyc_Absyn_Struct_d;
_temp782.f1= _temp776; _temp782;}); _temp781;}), loc); _temp780->tl= 0; _temp780;});}}
_LL723: if( atts != 0){ Cyc_Parse_err(({ char* _temp783="bad attributes on tunion";
struct _tagged_string _temp784; _temp784.curr= _temp783; _temp784.base= _temp783;
_temp784.last_plus_one= _temp783 + 25; _temp784;}), loc);} return({ struct Cyc_List_List*
_temp785=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp785->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp786=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp786[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp787; _temp787.tag= Cyc_Absyn_Tunion_d;
_temp787.f1= _temp746; _temp787;}); _temp786;}), loc); _temp785->tl= 0; _temp785;});
_LL725: { struct Cyc_List_List* _temp788=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp752);
goto _LL789; _LL789: { struct Cyc_Absyn_Decl* _temp790= Cyc_Absyn_tunion_decl( s,
_temp760, _temp788, 0, _temp758, loc); goto _LL791; _LL791: if( atts != 0){ Cyc_Parse_err(({
char* _temp792="bad attributes on tunion"; struct _tagged_string _temp793;
_temp793.curr= _temp792; _temp793.base= _temp792; _temp793.last_plus_one=
_temp792 + 25; _temp793;}), loc);} return({ struct Cyc_List_List* _temp794=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp794->hd=(
void*) _temp790; _temp794->tl= 0; _temp794;});}} _LL727: { struct Cyc_List_List*
_temp795=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp764); goto _LL796; _LL796: { struct Cyc_Absyn_Uniondecl*
_temp799=({ struct Cyc_Absyn_Uniondecl* _temp797=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp797->sc=( void*) s;
_temp797->name=({ struct Cyc_Core_Opt* _temp798=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp798->v=( void*)(( struct _tuple1*)
_check_null( _temp766)); _temp798;}); _temp797->tvs= _temp795; _temp797->fields=
0; _temp797->attributes= 0; _temp797;}); goto _LL800; _LL800: if( atts != 0){
Cyc_Parse_err(({ char* _temp801="bad attributes on union"; struct _tagged_string
_temp802; _temp802.curr= _temp801; _temp802.base= _temp801; _temp802.last_plus_one=
_temp801 + 24; _temp802;}), loc);} return({ struct Cyc_List_List* _temp803=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp803->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp804=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp804->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp805=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp805[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp806; _temp806.tag= Cyc_Absyn_Union_d;
_temp806.f1= _temp799; _temp806;}); _temp805;})); _temp804->loc= loc; _temp804;});
_temp803->tl= 0; _temp803;});}} _LL729: { struct Cyc_Absyn_Enumdecl* _temp808=({
struct Cyc_Absyn_Enumdecl* _temp807=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp807->sc=( void*) s; _temp807->name=
_temp770; _temp807->fields= 0; _temp807;}); goto _LL809; _LL809: if( atts != 0){
Cyc_Parse_err(({ char* _temp810="bad attributes on enum"; struct _tagged_string
_temp811; _temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 23; _temp811;}), loc);} return({ struct Cyc_List_List* _temp812=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp812->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp813=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp813->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp814=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp814[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp815; _temp815.tag= Cyc_Absyn_Enum_d;
_temp815.f1= _temp808; _temp815;}); _temp814;})); _temp813->loc= loc; _temp813;});
_temp812->tl= 0; _temp812;});} _LL731: Cyc_Parse_err(({ char* _temp816="missing declarator";
struct _tagged_string _temp817; _temp817.curr= _temp816; _temp817.base= _temp816;
_temp817.last_plus_one= _temp816 + 19; _temp817;}), loc); return 0; _LL719:;}}
else{ void* t= _temp683.f1; struct Cyc_List_List* _temp818= Cyc_Parse_apply_tmss(
tq, t, _temp681, atts); goto _LL819; _LL819: if( istypedef){ if( ! exps_empty){
Cyc_Parse_err(({ char* _temp820="initializer in typedef declaration"; struct
_tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 35; _temp821;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp818); if( _temp683.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp683.f2))->v;{ void* _temp822=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp834; struct Cyc_Absyn_Tuniondecl* _temp836; struct Cyc_Absyn_Uniondecl*
_temp838; struct Cyc_Absyn_Enumdecl* _temp840; _LL824: if(*(( int*) _temp822) ==
Cyc_Absyn_Struct_d){ _LL835: _temp834=(( struct Cyc_Absyn_Struct_d_struct*)
_temp822)->f1; goto _LL825;} else{ goto _LL826;} _LL826: if(*(( int*) _temp822)
== Cyc_Absyn_Tunion_d){ _LL837: _temp836=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp822)->f1; goto _LL827;} else{ goto _LL828;} _LL828: if(*(( int*) _temp822)
== Cyc_Absyn_Union_d){ _LL839: _temp838=(( struct Cyc_Absyn_Union_d_struct*)
_temp822)->f1; goto _LL829;} else{ goto _LL830;} _LL830: if(*(( int*) _temp822)
== Cyc_Absyn_Enum_d){ _LL841: _temp840=(( struct Cyc_Absyn_Enum_d_struct*)
_temp822)->f1; goto _LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL825:(
void*)( _temp834->sc=( void*) s); _temp834->attributes= atts; atts= 0; goto
_LL823; _LL827:( void*)( _temp836->sc=( void*) s); goto _LL823; _LL829:( void*)(
_temp838->sc=( void*) s); goto _LL823; _LL831:( void*)( _temp840->sc=( void*) s);
goto _LL823; _LL833: Cyc_Parse_err(({ char* _temp842="declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp843; _temp843.curr= _temp842; _temp843.base= _temp842;
_temp843.last_plus_one= _temp842 + 70; _temp843;}), loc); goto _LL823; _LL823:;}
decls=({ struct Cyc_List_List* _temp844=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp844->hd=( void*) d; _temp844->tl= decls;
_temp844;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp845=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s in typedef", _temp845.last_plus_one - _temp845.curr,
_temp845.curr);}), loc);} return decls;}} else{ if( _temp683.f2 != 0){ Cyc_Parse_unimp2(({
char* _temp846="nested type declaration within declarator"; struct
_tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 42; _temp847;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp848= _temp818; goto _LL849; _LL849: for( 0;
_temp848 != 0;( _temp848=(( struct Cyc_List_List*) _check_null( _temp848))->tl,
_temp679=(( struct Cyc_List_List*) _check_null( _temp679))->tl)){ struct _tuple7
_temp852; struct Cyc_List_List* _temp853; struct Cyc_List_List* _temp855; void*
_temp857; struct Cyc_Absyn_Tqual _temp859; struct _tuple1* _temp861; struct
_tuple7* _temp850=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp848))->hd; _temp852=* _temp850; _LL862: _temp861= _temp852.f1; goto _LL860;
_LL860: _temp859= _temp852.f2; goto _LL858; _LL858: _temp857= _temp852.f3; goto
_LL856; _LL856: _temp855= _temp852.f4; goto _LL854; _LL854: _temp853= _temp852.f5;
goto _LL851; _LL851: if( _temp855 != 0){ Cyc_Parse_warn(({ char* _temp863="bad type params, ignoring";
struct _tagged_string _temp864; _temp864.curr= _temp863; _temp864.base= _temp863;
_temp864.last_plus_one= _temp863 + 26; _temp864;}), loc);} if( _temp679 == 0){((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
char* _temp865="unexpected null in parse!"; struct _tagged_string _temp866;
_temp866.curr= _temp865; _temp866.base= _temp865; _temp866.last_plus_one=
_temp865 + 26; _temp866;}), loc);}{ struct Cyc_Absyn_Exp* _temp867=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp679))->hd; goto _LL868; _LL868: {
struct Cyc_Absyn_Vardecl* _temp869= Cyc_Absyn_new_vardecl( _temp861, _temp857,
_temp867); goto _LL870; _LL870: _temp869->tq= _temp859;( void*)( _temp869->sc=(
void*) s); _temp869->attributes= _temp853;{ struct Cyc_Absyn_Decl* _temp874=({
struct Cyc_Absyn_Decl* _temp871=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp871->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp872=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp872[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp873; _temp873.tag= Cyc_Absyn_Var_d;
_temp873.f1= _temp869; _temp873;}); _temp872;})); _temp871->loc= loc; _temp871;});
goto _LL875; _LL875: decls=({ struct Cyc_List_List* _temp876=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp876->hd=( void*) _temp874;
_temp876->tl= decls; _temp876;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp877= s; xprintf("bad kind: %.*s",
_temp877.last_plus_one - _temp877.curr, _temp877.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), 0)){
case 'A': _LL878: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL879: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL880: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL881: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL882: return(
void*) Cyc_Absyn_EffKind; default: _LL883: Cyc_Parse_err(({ struct
_tagged_string _temp885= s; xprintf("bad kind: %.*s", _temp885.last_plus_one -
_temp885.curr, _temp885.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp886=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp886->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp887=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp887[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp888; _temp888.tag= Cyc_Absyn_Attributes_mod;
_temp888.f1= loc; _temp888.f2= atts; _temp888;}); _temp887;})); _temp886->tl=
tms; _temp886;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp889= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp889.last_plus_one - _temp889.curr, _temp889.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp890=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp890[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp891; _temp891.tag= Cyc_Absyn_Typedef_d; _temp891.f1=({
struct Cyc_Absyn_Typedefdecl* _temp892=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp892->name= x; _temp892->tvs=(*
t).f4; _temp892->defn=( void*)(* t).f3; _temp892;}); _temp891;}); _temp890;}),
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
} ; static char _temp896[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,{ _temp896, _temp896, _temp896 + 8u}};
struct _tuple16* Cyc_yyget_Int_tok( void* yy1){ void* _temp897= yy1; struct
_tuple16* _temp903; _LL899: if(*(( void**) _temp897) == Cyc_Int_tok){ _LL904:
_temp903=(( struct Cyc_Int_tok_struct*) _temp897)->f1; goto _LL900;} else{ goto
_LL901;} _LL901: goto _LL902; _LL900: return _temp903; _LL902:( void) _throw((
void*)& Cyc_yyfail_Int_tok); _LL898:;} static char _temp908[ 11u]="String_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={ Cyc_Core_Failure,{
_temp908, _temp908, _temp908 + 11u}}; struct _tagged_string Cyc_yyget_String_tok(
void* yy1){ void* _temp909= yy1; struct _tagged_string _temp915; _LL911: if(*((
void**) _temp909) == Cyc_String_tok){ _LL916: _temp915=(( struct Cyc_String_tok_struct*)
_temp909)->f1; goto _LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL912:
return _temp915; _LL914:( void) _throw(( void*)& Cyc_yyfail_String_tok); _LL910:;}
static char _temp920[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{ _temp920, _temp920, _temp920 + 9u}}; char Cyc_yyget_Char_tok(
void* yy1){ void* _temp921= yy1; char _temp927; _LL923: if(*(( void**) _temp921)
== Cyc_Char_tok){ _LL928: _temp927=(( struct Cyc_Char_tok_struct*) _temp921)->f1;
goto _LL924;} else{ goto _LL925;} _LL925: goto _LL926; _LL924: return _temp927;
_LL926:( void) _throw(( void*)& Cyc_yyfail_Char_tok); _LL922:;} static char
_temp932[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={
Cyc_Core_Failure,{ _temp932, _temp932, _temp932 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
void* yy1){ void* _temp933= yy1; void* _temp939; _LL935: if(*(( void**) _temp933)
== Cyc_Pointer_Sort_tok){ _LL940: _temp939=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp933)->f1; goto _LL936;} else{ goto _LL937;} _LL937: goto _LL938; _LL936:
return _temp939; _LL938:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL934:;} static char _temp944[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,{ _temp944, _temp944, _temp944 + 8u}};
struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){ void* _temp945= yy1; struct
Cyc_Absyn_Exp* _temp951; _LL947: if(*(( void**) _temp945) == Cyc_Exp_tok){
_LL952: _temp951=(( struct Cyc_Exp_tok_struct*) _temp945)->f1; goto _LL948;}
else{ goto _LL949;} _LL949: goto _LL950; _LL948: return _temp951; _LL950:( void)
_throw(( void*)& Cyc_yyfail_Exp_tok); _LL946:;} static char _temp956[ 12u]="ExpList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{
_temp956, _temp956, _temp956 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok(
void* yy1){ void* _temp957= yy1; struct Cyc_List_List* _temp963; _LL959: if(*((
void**) _temp957) == Cyc_ExpList_tok){ _LL964: _temp963=(( struct Cyc_ExpList_tok_struct*)
_temp957)->f1; goto _LL960;} else{ goto _LL961;} _LL961: goto _LL962; _LL960:
return _temp963; _LL962:( void) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL958:;}
static char _temp968[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{ _temp968, _temp968, _temp968
+ 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok( void* yy1){ void*
_temp969= yy1; struct Cyc_List_List* _temp975; _LL971: if(*(( void**) _temp969)
== Cyc_InitializerList_tok){ _LL976: _temp975=(( struct Cyc_InitializerList_tok_struct*)
_temp969)->f1; goto _LL972;} else{ goto _LL973;} _LL973: goto _LL974; _LL972:
return _temp975; _LL974:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL970:;} static char _temp980[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp980, _temp980, _temp980 + 11u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* _temp981= yy1; void* _temp987;
_LL983: if(*(( void**) _temp981) == Cyc_Primop_tok){ _LL988: _temp987=( void*)((
struct Cyc_Primop_tok_struct*) _temp981)->f1; goto _LL984;} else{ goto _LL985;}
_LL985: goto _LL986; _LL984: return _temp987; _LL986:( void) _throw(( void*)&
Cyc_yyfail_Primop_tok); _LL982:;} static char _temp992[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{
_temp992, _temp992, _temp992 + 14u}}; struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok(
void* yy1){ void* _temp993= yy1; struct Cyc_Core_Opt* _temp999; _LL995: if(*((
void**) _temp993) == Cyc_Primopopt_tok){ _LL1000: _temp999=(( struct Cyc_Primopopt_tok_struct*)
_temp993)->f1; goto _LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL996:
return _temp999; _LL998:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL994:;} static char _temp1004[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{ _temp1004, _temp1004, _temp1004 + 11u}};
struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){ void* _temp1005= yy1; struct
_tuple1* _temp1011; _LL1007: if(*(( void**) _temp1005) == Cyc_QualId_tok){
_LL1012: _temp1011=(( struct Cyc_QualId_tok_struct*) _temp1005)->f1; goto
_LL1008;} else{ goto _LL1009;} _LL1009: goto _LL1010; _LL1008: return _temp1011;
_LL1010:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1006:;} static char
_temp1016[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,{ _temp1016, _temp1016, _temp1016 + 9u}}; struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1017= yy1; struct Cyc_Absyn_Stmt*
_temp1023; _LL1019: if(*(( void**) _temp1017) == Cyc_Stmt_tok){ _LL1024:
_temp1023=(( struct Cyc_Stmt_tok_struct*) _temp1017)->f1; goto _LL1020;} else{
goto _LL1021;} _LL1021: goto _LL1022; _LL1020: return _temp1023; _LL1022:( void)
_throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1018:;} static char _temp1028[ 14u]="BlockItem_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure,{
_temp1028, _temp1028, _temp1028 + 14u}}; void* Cyc_yyget_BlockItem_tok( void*
yy1){ void* _temp1029= yy1; void* _temp1035; _LL1031: if(*(( void**) _temp1029)
== Cyc_BlockItem_tok){ _LL1036: _temp1035=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp1029)->f1; goto _LL1032;} else{ goto _LL1033;} _LL1033: goto _LL1034;
_LL1032: return _temp1035; _LL1034:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL1030:;} static char _temp1040[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{ _temp1040, _temp1040,
_temp1040 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( void*
yy1){ void* _temp1041= yy1; struct Cyc_List_List* _temp1047; _LL1043: if(*((
void**) _temp1041) == Cyc_SwitchClauseList_tok){ _LL1048: _temp1047=(( struct
Cyc_SwitchClauseList_tok_struct*) _temp1041)->f1; goto _LL1044;} else{ goto
_LL1045;} _LL1045: goto _LL1046; _LL1044: return _temp1047; _LL1046:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1042:;} static char
_temp1052[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={
Cyc_Core_Failure,{ _temp1052, _temp1052, _temp1052 + 12u}}; struct Cyc_Absyn_Pat*
Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1053= yy1; struct Cyc_Absyn_Pat*
_temp1059; _LL1055: if(*(( void**) _temp1053) == Cyc_Pattern_tok){ _LL1060:
_temp1059=(( struct Cyc_Pattern_tok_struct*) _temp1053)->f1; goto _LL1056;}
else{ goto _LL1057;} _LL1057: goto _LL1058; _LL1056: return _temp1059; _LL1058:(
void) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL1054:;} static char _temp1064[
16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure,{ _temp1064, _temp1064, _temp1064 + 16u}}; struct Cyc_List_List*
Cyc_yyget_PatternList_tok( void* yy1){ void* _temp1065= yy1; struct Cyc_List_List*
_temp1071; _LL1067: if(*(( void**) _temp1065) == Cyc_PatternList_tok){ _LL1072:
_temp1071=(( struct Cyc_PatternList_tok_struct*) _temp1065)->f1; goto _LL1068;}
else{ goto _LL1069;} _LL1069: goto _LL1070; _LL1068: return _temp1071; _LL1070:(
void) _throw(( void*)& Cyc_yyfail_PatternList_tok); _LL1066:;} static char
_temp1076[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,{ _temp1076, _temp1076, _temp1076 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp1077= yy1; struct _tuple14* _temp1083; _LL1079: if(*((
void**) _temp1077) == Cyc_FieldPattern_tok){ _LL1084: _temp1083=(( struct Cyc_FieldPattern_tok_struct*)
_temp1077)->f1; goto _LL1080;} else{ goto _LL1081;} _LL1081: goto _LL1082;
_LL1080: return _temp1083; _LL1082:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1078:;} static char _temp1088[ 21u]="FieldPatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{ _temp1088, _temp1088,
_temp1088 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok( void*
yy1){ void* _temp1089= yy1; struct Cyc_List_List* _temp1095; _LL1091: if(*((
void**) _temp1089) == Cyc_FieldPatternList_tok){ _LL1096: _temp1095=(( struct
Cyc_FieldPatternList_tok_struct*) _temp1089)->f1; goto _LL1092;} else{ goto
_LL1093;} _LL1093: goto _LL1094; _LL1092: return _temp1095; _LL1094:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1090:;} static char
_temp1100[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,{ _temp1100, _temp1100, _temp1100 + 11u}}; struct Cyc_Absyn_Fndecl*
Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1101= yy1; struct Cyc_Absyn_Fndecl*
_temp1107; _LL1103: if(*(( void**) _temp1101) == Cyc_FnDecl_tok){ _LL1108:
_temp1107=(( struct Cyc_FnDecl_tok_struct*) _temp1101)->f1; goto _LL1104;} else{
goto _LL1105;} _LL1105: goto _LL1106; _LL1104: return _temp1107; _LL1106:( void)
_throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL1102:;} static char _temp1112[ 13u]="DeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{
_temp1112, _temp1112, _temp1112 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
void* yy1){ void* _temp1113= yy1; struct Cyc_List_List* _temp1119; _LL1115: if(*((
void**) _temp1113) == Cyc_DeclList_tok){ _LL1120: _temp1119=(( struct Cyc_DeclList_tok_struct*)
_temp1113)->f1; goto _LL1116;} else{ goto _LL1117;} _LL1117: goto _LL1118;
_LL1116: return _temp1119; _LL1118:( void) _throw(( void*)& Cyc_yyfail_DeclList_tok);
_LL1114:;} static char _temp1124[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp1124, _temp1124, _temp1124 +
13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){
void* _temp1125= yy1; struct Cyc_Parse_Declaration_spec* _temp1131; _LL1127: if(*((
void**) _temp1125) == Cyc_DeclSpec_tok){ _LL1132: _temp1131=(( struct Cyc_DeclSpec_tok_struct*)
_temp1125)->f1; goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130;
_LL1128: return _temp1131; _LL1130:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1126:;} static char _temp1136[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp1136, _temp1136, _temp1136 +
13u}}; struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){ void* _temp1137= yy1;
struct _tuple15* _temp1143; _LL1139: if(*(( void**) _temp1137) == Cyc_InitDecl_tok){
_LL1144: _temp1143=(( struct Cyc_InitDecl_tok_struct*) _temp1137)->f1; goto
_LL1140;} else{ goto _LL1141;} _LL1141: goto _LL1142; _LL1140: return _temp1143;
_LL1142:( void) _throw(( void*)& Cyc_yyfail_InitDecl_tok); _LL1138:;} static
char _temp1148[ 17u]="InitDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{ _temp1148, _temp1148, _temp1148
+ 17u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( void* yy1){ void*
_temp1149= yy1; struct Cyc_List_List* _temp1155; _LL1151: if(*(( void**)
_temp1149) == Cyc_InitDeclList_tok){ _LL1156: _temp1155=(( struct Cyc_InitDeclList_tok_struct*)
_temp1149)->f1; goto _LL1152;} else{ goto _LL1153;} _LL1153: goto _LL1154;
_LL1152: return _temp1155; _LL1154:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1150:;} static char _temp1160[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{ _temp1160, _temp1160, _temp1160
+ 17u}}; void* Cyc_yyget_StorageClass_tok( void* yy1){ void* _temp1161= yy1;
void* _temp1167; _LL1163: if(*(( void**) _temp1161) == Cyc_StorageClass_tok){
_LL1168: _temp1167=( void*)(( struct Cyc_StorageClass_tok_struct*) _temp1161)->f1;
goto _LL1164;} else{ goto _LL1165;} _LL1165: goto _LL1166; _LL1164: return
_temp1167; _LL1166:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1162:;} static char _temp1172[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{ _temp1172, _temp1172,
_temp1172 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void* yy1){ void*
_temp1173= yy1; void* _temp1179; _LL1175: if(*(( void**) _temp1173) == Cyc_TypeSpecifier_tok){
_LL1180: _temp1179=( void*)(( struct Cyc_TypeSpecifier_tok_struct*) _temp1173)->f1;
goto _LL1176;} else{ goto _LL1177;} _LL1177: goto _LL1178; _LL1176: return
_temp1179; _LL1178:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1174:;} static char _temp1184[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{ _temp1184, _temp1184,
_temp1184 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void* yy1){ void*
_temp1185= yy1; void* _temp1191; _LL1187: if(*(( void**) _temp1185) == Cyc_StructOrUnion_tok){
_LL1192: _temp1191=( void*)(( struct Cyc_StructOrUnion_tok_struct*) _temp1185)->f1;
goto _LL1188;} else{ goto _LL1189;} _LL1189: goto _LL1190; _LL1188: return
_temp1191; _LL1190:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1186:;} static char _temp1196[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp1196, _temp1196, _temp1196 +
13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok( void* yy1){ void*
_temp1197= yy1; struct Cyc_Absyn_Tqual _temp1203; _LL1199: if(*(( void**)
_temp1197) == Cyc_TypeQual_tok){ _LL1204: _temp1203=(( struct Cyc_TypeQual_tok_struct*)
_temp1197)->f1; goto _LL1200;} else{ goto _LL1201;} _LL1201: goto _LL1202;
_LL1200: return _temp1203; _LL1202:( void) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1198:;} static char _temp1208[ 24u]="StructFieldDeclList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp1208, _temp1208, _temp1208 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp1209= yy1; struct Cyc_List_List* _temp1215; _LL1211: if(*((
void**) _temp1209) == Cyc_StructFieldDeclList_tok){ _LL1216: _temp1215=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1209)->f1; goto _LL1212;} else{ goto
_LL1213;} _LL1213: goto _LL1214; _LL1212: return _temp1215; _LL1214:( void)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1210:;} static char
_temp1220[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1220, _temp1220,
_temp1220 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1221= yy1; struct Cyc_List_List* _temp1227; _LL1223: if(*((
void**) _temp1221) == Cyc_StructFieldDeclListList_tok){ _LL1228: _temp1227=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1221)->f1; goto _LL1224;}
else{ goto _LL1225;} _LL1225: goto _LL1226; _LL1224: return _temp1227; _LL1226:(
void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1222:;}
static char _temp1232[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1232, _temp1232,
_temp1232 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1233= yy1; struct Cyc_List_List* _temp1239; _LL1235: if(*((
void**) _temp1233) == Cyc_TypeModifierList_tok){ _LL1240: _temp1239=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1233)->f1; goto _LL1236;} else{ goto
_LL1237;} _LL1237: goto _LL1238; _LL1236: return _temp1239; _LL1238:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1234:;} static char
_temp1244[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgn_tok={
Cyc_Core_Failure,{ _temp1244, _temp1244, _temp1244 + 8u}}; void* Cyc_yyget_Rgn_tok(
void* yy1){ void* _temp1245= yy1; void* _temp1251; _LL1247: if(*(( void**)
_temp1245) == Cyc_Rgn_tok){ _LL1252: _temp1251=( void*)(( struct Cyc_Rgn_tok_struct*)
_temp1245)->f1; goto _LL1248;} else{ goto _LL1249;} _LL1249: goto _LL1250;
_LL1248: return _temp1251; _LL1250:( void) _throw(( void*)& Cyc_yyfail_Rgn_tok);
_LL1246:;} static char _temp1256[ 15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{ _temp1256, _temp1256, _temp1256 +
15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( void* yy1){ void*
_temp1257= yy1; struct Cyc_Parse_Declarator* _temp1263; _LL1259: if(*(( void**)
_temp1257) == Cyc_Declarator_tok){ _LL1264: _temp1263=(( struct Cyc_Declarator_tok_struct*)
_temp1257)->f1; goto _LL1260;} else{ goto _LL1261;} _LL1261: goto _LL1262;
_LL1260: return _temp1263; _LL1262:( void) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1258:;} static char _temp1268[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1268, _temp1268,
_temp1268 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1269= yy1; struct _tuple13* _temp1275; _LL1271: if(*(( void**)
_temp1269) == Cyc_DeclaratorExpopt_tok){ _LL1276: _temp1275=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1269)->f1; goto _LL1272;} else{ goto _LL1273;} _LL1273: goto _LL1274;
_LL1272: return _temp1275; _LL1274:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1270:;} static char _temp1280[ 25u]="DeclaratorExpoptList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1280, _temp1280, _temp1280 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1281= yy1; struct Cyc_List_List* _temp1287; _LL1283: if(*((
void**) _temp1281) == Cyc_DeclaratorExpoptList_tok){ _LL1288: _temp1287=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1281)->f1; goto _LL1284;}
else{ goto _LL1285;} _LL1285: goto _LL1286; _LL1284: return _temp1287; _LL1286:(
void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1282:;} static
char _temp1292[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1292, _temp1292,
_temp1292 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1293= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1299;
_LL1295: if(*(( void**) _temp1293) == Cyc_AbstractDeclarator_tok){ _LL1300:
_temp1299=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1293)->f1; goto
_LL1296;} else{ goto _LL1297;} _LL1297: goto _LL1298; _LL1296: return _temp1299;
_LL1298:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1294:;}
static char _temp1304[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Bool_tok={
Cyc_Core_Failure,{ _temp1304, _temp1304, _temp1304 + 9u}}; int Cyc_yyget_Bool_tok(
void* yy1){ void* _temp1305= yy1; int _temp1311; _LL1307: if(*(( void**)
_temp1305) == Cyc_Bool_tok){ _LL1312: _temp1311=(( struct Cyc_Bool_tok_struct*)
_temp1305)->f1; goto _LL1308;} else{ goto _LL1309;} _LL1309: goto _LL1310;
_LL1308: return _temp1311; _LL1310:( void) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1306:;} static char _temp1316[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1316, _temp1316, _temp1316 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1317= yy1; void* _temp1323;
_LL1319: if(*(( void**) _temp1317) == Cyc_Scope_tok){ _LL1324: _temp1323=( void*)((
struct Cyc_Scope_tok_struct*) _temp1317)->f1; goto _LL1320;} else{ goto _LL1321;}
_LL1321: goto _LL1322; _LL1320: return _temp1323; _LL1322:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1318:;} static char _temp1328[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1328, _temp1328, _temp1328 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1329= yy1; struct Cyc_Absyn_Tunionfield* _temp1335;
_LL1331: if(*(( void**) _temp1329) == Cyc_TunionField_tok){ _LL1336: _temp1335=((
struct Cyc_TunionField_tok_struct*) _temp1329)->f1; goto _LL1332;} else{ goto
_LL1333;} _LL1333: goto _LL1334; _LL1332: return _temp1335; _LL1334:( void)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1330:;} static char _temp1340[
20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1340, _temp1340, _temp1340 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1341= yy1; struct Cyc_List_List*
_temp1347; _LL1343: if(*(( void**) _temp1341) == Cyc_TunionFieldList_tok){
_LL1348: _temp1347=(( struct Cyc_TunionFieldList_tok_struct*) _temp1341)->f1;
goto _LL1344;} else{ goto _LL1345;} _LL1345: goto _LL1346; _LL1344: return
_temp1347; _LL1346:( void) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1342:;} static char _temp1352[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{ _temp1352, _temp1352, _temp1352
+ 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok( void* yy1){ void*
_temp1353= yy1; struct _tuple18* _temp1359; _LL1355: if(*(( void**) _temp1353)
== Cyc_QualSpecList_tok){ _LL1360: _temp1359=(( struct Cyc_QualSpecList_tok_struct*)
_temp1353)->f1; goto _LL1356;} else{ goto _LL1357;} _LL1357: goto _LL1358;
_LL1356: return _temp1359; _LL1358:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1354:;} static char _temp1364[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1364, _temp1364, _temp1364 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1365= yy1;
struct Cyc_List_List* _temp1371; _LL1367: if(*(( void**) _temp1365) == Cyc_IdList_tok){
_LL1372: _temp1371=(( struct Cyc_IdList_tok_struct*) _temp1365)->f1; goto
_LL1368;} else{ goto _LL1369;} _LL1369: goto _LL1370; _LL1368: return _temp1371;
_LL1370:( void) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1366:;} static char
_temp1376[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure,{ _temp1376, _temp1376, _temp1376 + 14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok(
void* yy1){ void* _temp1377= yy1; struct _tuple2* _temp1383; _LL1379: if(*((
void**) _temp1377) == Cyc_ParamDecl_tok){ _LL1384: _temp1383=(( struct Cyc_ParamDecl_tok_struct*)
_temp1377)->f1; goto _LL1380;} else{ goto _LL1381;} _LL1381: goto _LL1382;
_LL1380: return _temp1383; _LL1382:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok);
_LL1378:;} static char _temp1388[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1388, _temp1388,
_temp1388 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1389= yy1; struct Cyc_List_List* _temp1395; _LL1391: if(*(( void**)
_temp1389) == Cyc_ParamDeclList_tok){ _LL1396: _temp1395=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1389)->f1; goto _LL1392;} else{ goto _LL1393;} _LL1393: goto _LL1394;
_LL1392: return _temp1395; _LL1394:( void) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1390:;} static char _temp1400[ 22u]="ParamDeclListBool_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1400, _temp1400, _temp1400 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1401= yy1; struct _tuple17* _temp1407; _LL1403: if(*((
void**) _temp1401) == Cyc_ParamDeclListBool_tok){ _LL1408: _temp1407=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1401)->f1; goto _LL1404;} else{ goto
_LL1405;} _LL1405: goto _LL1406; _LL1404: return _temp1407; _LL1406:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1402:;} static char
_temp1412[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={
Cyc_Core_Failure,{ _temp1412, _temp1412, _temp1412 + 13u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1413= yy1; struct Cyc_List_List*
_temp1419; _LL1415: if(*(( void**) _temp1413) == Cyc_TypeList_tok){ _LL1420:
_temp1419=(( struct Cyc_TypeList_tok_struct*) _temp1413)->f1; goto _LL1416;}
else{ goto _LL1417;} _LL1417: goto _LL1418; _LL1416: return _temp1419; _LL1418:(
void) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1414:;} static char
_temp1424[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={
Cyc_Core_Failure,{ _temp1424, _temp1424, _temp1424 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1425= yy1; struct Cyc_List_List*
_temp1431; _LL1427: if(*(( void**) _temp1425) == Cyc_DesignatorList_tok){
_LL1432: _temp1431=(( struct Cyc_DesignatorList_tok_struct*) _temp1425)->f1;
goto _LL1428;} else{ goto _LL1429;} _LL1429: goto _LL1430; _LL1428: return
_temp1431; _LL1430:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1426:;} static char _temp1436[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1436, _temp1436, _temp1436 +
15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void* _temp1437= yy1; void*
_temp1443; _LL1439: if(*(( void**) _temp1437) == Cyc_Designator_tok){ _LL1444:
_temp1443=( void*)(( struct Cyc_Designator_tok_struct*) _temp1437)->f1; goto
_LL1440;} else{ goto _LL1441;} _LL1441: goto _LL1442; _LL1440: return _temp1443;
_LL1442:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1438:;} static
char _temp1448[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,{ _temp1448, _temp1448, _temp1448 + 9u}}; void* Cyc_yyget_Kind_tok(
void* yy1){ void* _temp1449= yy1; void* _temp1455; _LL1451: if(*(( void**)
_temp1449) == Cyc_Kind_tok){ _LL1456: _temp1455=( void*)(( struct Cyc_Kind_tok_struct*)
_temp1449)->f1; goto _LL1452;} else{ goto _LL1453;} _LL1453: goto _LL1454;
_LL1452: return _temp1455; _LL1454:( void) _throw(( void*)& Cyc_yyfail_Kind_tok);
_LL1450:;} static char _temp1460[ 9u]="Type_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{ _temp1460, _temp1460, _temp1460 + 9u}};
void* Cyc_yyget_Type_tok( void* yy1){ void* _temp1461= yy1; void* _temp1467;
_LL1463: if(*(( void**) _temp1461) == Cyc_Type_tok){ _LL1468: _temp1467=( void*)((
struct Cyc_Type_tok_struct*) _temp1461)->f1; goto _LL1464;} else{ goto _LL1465;}
_LL1465: goto _LL1466; _LL1464: return _temp1467; _LL1466:( void) _throw(( void*)&
Cyc_yyfail_Type_tok); _LL1462:;} static char _temp1472[ 18u]="AttributeList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{
_temp1472, _temp1472, _temp1472 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1473= yy1; struct Cyc_List_List* _temp1479; _LL1475: if(*((
void**) _temp1473) == Cyc_AttributeList_tok){ _LL1480: _temp1479=(( struct Cyc_AttributeList_tok_struct*)
_temp1473)->f1; goto _LL1476;} else{ goto _LL1477;} _LL1477: goto _LL1478;
_LL1476: return _temp1479; _LL1478:( void) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1474:;} static char _temp1484[ 14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1484, _temp1484, _temp1484 +
14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1485= yy1; void*
_temp1491; _LL1487: if(*(( void**) _temp1485) == Cyc_Attribute_tok){ _LL1492:
_temp1491=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1485)->f1; goto
_LL1488;} else{ goto _LL1489;} _LL1489: goto _LL1490; _LL1488: return _temp1491;
_LL1490:( void) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1486:;} static
char _temp1496[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={
Cyc_Core_Failure,{ _temp1496, _temp1496, _temp1496 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1497= yy1; struct Cyc_Absyn_Enumfield*
_temp1503; _LL1499: if(*(( void**) _temp1497) == Cyc_Enumfield_tok){ _LL1504:
_temp1503=(( struct Cyc_Enumfield_tok_struct*) _temp1497)->f1; goto _LL1500;}
else{ goto _LL1501;} _LL1501: goto _LL1502; _LL1500: return _temp1503; _LL1502:(
void) _throw(( void*)& Cyc_yyfail_Enumfield_tok); _LL1498:;} static char
_temp1508[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={
Cyc_Core_Failure,{ _temp1508, _temp1508, _temp1508 + 18u}}; struct Cyc_List_List*
Cyc_yyget_EnumfieldList_tok( void* yy1){ void* _temp1509= yy1; struct Cyc_List_List*
_temp1515; _LL1511: if(*(( void**) _temp1509) == Cyc_EnumfieldList_tok){ _LL1516:
_temp1515=(( struct Cyc_EnumfieldList_tok_struct*) _temp1509)->f1; goto _LL1512;}
else{ goto _LL1513;} _LL1513: goto _LL1514; _LL1512: return _temp1515; _LL1514:(
void) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1510:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1517; _temp1517.timestamp=
0; _temp1517.first_line= 0; _temp1517.first_column= 0; _temp1517.last_line= 0;
_temp1517.last_column= 0; _temp1517.text=({ char* _temp1518=""; struct
_tagged_string _temp1519; _temp1519.curr= _temp1518; _temp1519.base= _temp1518;
_temp1519.last_plus_one= _temp1518 + 1; _temp1519;}); _temp1517;});} static char
_temp1522[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{ _temp1522,
_temp1522, _temp1522 + 1u}}; static short Cyc_yytranslate[ 352u]={ (short)0,
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
(short)96}; static char _temp1525[ 2u]="$"; static char _temp1528[ 6u]="error";
static char _temp1531[ 12u]="$undefined."; static char _temp1534[ 5u]="AUTO";
static char _temp1537[ 9u]="REGISTER"; static char _temp1540[ 7u]="STATIC";
static char _temp1543[ 7u]="EXTERN"; static char _temp1546[ 8u]="TYPEDEF";
static char _temp1549[ 5u]="VOID"; static char _temp1552[ 5u]="CHAR"; static
char _temp1555[ 6u]="SHORT"; static char _temp1558[ 4u]="INT"; static char
_temp1561[ 5u]="LONG"; static char _temp1564[ 6u]="FLOAT"; static char _temp1567[
7u]="DOUBLE"; static char _temp1570[ 7u]="SIGNED"; static char _temp1573[ 9u]="UNSIGNED";
static char _temp1576[ 6u]="CONST"; static char _temp1579[ 9u]="VOLATILE";
static char _temp1582[ 9u]="RESTRICT"; static char _temp1585[ 7u]="STRUCT";
static char _temp1588[ 6u]="UNION"; static char _temp1591[ 5u]="CASE"; static
char _temp1594[ 8u]="DEFAULT"; static char _temp1597[ 7u]="INLINE"; static char
_temp1600[ 3u]="IF"; static char _temp1603[ 5u]="ELSE"; static char _temp1606[ 7u]="SWITCH";
static char _temp1609[ 6u]="WHILE"; static char _temp1612[ 3u]="DO"; static char
_temp1615[ 4u]="FOR"; static char _temp1618[ 5u]="GOTO"; static char _temp1621[
9u]="CONTINUE"; static char _temp1624[ 6u]="BREAK"; static char _temp1627[ 7u]="RETURN";
static char _temp1630[ 7u]="SIZEOF"; static char _temp1633[ 5u]="ENUM"; static
char _temp1636[ 8u]="NULL_kw"; static char _temp1639[ 4u]="LET"; static char
_temp1642[ 6u]="THROW"; static char _temp1645[ 4u]="TRY"; static char _temp1648[
6u]="CATCH"; static char _temp1651[ 4u]="NEW"; static char _temp1654[ 9u]="ABSTRACT";
static char _temp1657[ 9u]="FALLTHRU"; static char _temp1660[ 6u]="USING";
static char _temp1663[ 10u]="NAMESPACE"; static char _temp1666[ 7u]="TUNION";
static char _temp1669[ 8u]="XTUNION"; static char _temp1672[ 5u]="FILL"; static
char _temp1675[ 8u]="CODEGEN"; static char _temp1678[ 4u]="CUT"; static char
_temp1681[ 7u]="SPLICE"; static char _temp1684[ 7u]="PRINTF"; static char
_temp1687[ 8u]="FPRINTF"; static char _temp1690[ 8u]="XPRINTF"; static char
_temp1693[ 6u]="SCANF"; static char _temp1696[ 7u]="FSCANF"; static char
_temp1699[ 7u]="SSCANF"; static char _temp1702[ 7u]="MALLOC"; static char
_temp1705[ 9u]="REGION_T"; static char _temp1708[ 7u]="REGION"; static char
_temp1711[ 5u]="RNEW"; static char _temp1714[ 8u]="RMALLOC"; static char
_temp1717[ 7u]="PTR_OP"; static char _temp1720[ 7u]="INC_OP"; static char
_temp1723[ 7u]="DEC_OP"; static char _temp1726[ 8u]="LEFT_OP"; static char
_temp1729[ 9u]="RIGHT_OP"; static char _temp1732[ 6u]="LE_OP"; static char
_temp1735[ 6u]="GE_OP"; static char _temp1738[ 6u]="EQ_OP"; static char
_temp1741[ 6u]="NE_OP"; static char _temp1744[ 7u]="AND_OP"; static char
_temp1747[ 6u]="OR_OP"; static char _temp1750[ 11u]="MUL_ASSIGN"; static char
_temp1753[ 11u]="DIV_ASSIGN"; static char _temp1756[ 11u]="MOD_ASSIGN"; static
char _temp1759[ 11u]="ADD_ASSIGN"; static char _temp1762[ 11u]="SUB_ASSIGN";
static char _temp1765[ 12u]="LEFT_ASSIGN"; static char _temp1768[ 13u]="RIGHT_ASSIGN";
static char _temp1771[ 11u]="AND_ASSIGN"; static char _temp1774[ 11u]="XOR_ASSIGN";
static char _temp1777[ 10u]="OR_ASSIGN"; static char _temp1780[ 9u]="ELLIPSIS";
static char _temp1783[ 11u]="LEFT_RIGHT"; static char _temp1786[ 12u]="COLON_COLON";
static char _temp1789[ 11u]="IDENTIFIER"; static char _temp1792[ 17u]="INTEGER_CONSTANT";
static char _temp1795[ 7u]="STRING"; static char _temp1798[ 19u]="CHARACTER_CONSTANT";
static char _temp1801[ 18u]="FLOATING_CONSTANT"; static char _temp1804[ 9u]="TYPE_VAR";
static char _temp1807[ 16u]="QUAL_IDENTIFIER"; static char _temp1810[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1813[ 10u]="ATTRIBUTE"; static char _temp1816[ 4u]="';'";
static char _temp1819[ 4u]="'{'"; static char _temp1822[ 4u]="'}'"; static char
_temp1825[ 4u]="'='"; static char _temp1828[ 4u]="'('"; static char _temp1831[ 4u]="')'";
static char _temp1834[ 4u]="','"; static char _temp1837[ 4u]="'_'"; static char
_temp1840[ 4u]="'$'"; static char _temp1843[ 4u]="'<'"; static char _temp1846[ 4u]="'>'";
static char _temp1849[ 4u]="':'"; static char _temp1852[ 4u]="'.'"; static char
_temp1855[ 4u]="'['"; static char _temp1858[ 4u]="']'"; static char _temp1861[ 4u]="'*'";
static char _temp1864[ 4u]="'@'"; static char _temp1867[ 4u]="'?'"; static char
_temp1870[ 4u]="'+'"; static char _temp1873[ 4u]="'-'"; static char _temp1876[ 4u]="'&'";
static char _temp1879[ 4u]="'|'"; static char _temp1882[ 4u]="'^'"; static char
_temp1885[ 4u]="'/'"; static char _temp1888[ 4u]="'%'"; static char _temp1891[ 4u]="'~'";
static char _temp1894[ 4u]="'!'"; static char _temp1897[ 5u]="prog"; static char
_temp1900[ 17u]="translation_unit"; static char _temp1903[ 21u]="external_declaration";
static char _temp1906[ 20u]="function_definition"; static char _temp1909[ 21u]="function_definition2";
static char _temp1912[ 13u]="using_action"; static char _temp1915[ 15u]="unusing_action";
static char _temp1918[ 17u]="namespace_action"; static char _temp1921[ 19u]="unnamespace_action";
static char _temp1924[ 12u]="declaration"; static char _temp1927[ 17u]="declaration_list";
static char _temp1930[ 23u]="declaration_specifiers"; static char _temp1933[ 24u]="storage_class_specifier";
static char _temp1936[ 15u]="attributes_opt"; static char _temp1939[ 11u]="attributes";
static char _temp1942[ 15u]="attribute_list"; static char _temp1945[ 10u]="attribute";
static char _temp1948[ 15u]="type_specifier"; static char _temp1951[ 5u]="kind";
static char _temp1954[ 15u]="type_qualifier"; static char _temp1957[ 15u]="enum_specifier";
static char _temp1960[ 11u]="enum_field"; static char _temp1963[ 22u]="enum_declaration_list";
static char _temp1966[ 26u]="struct_or_union_specifier"; static char _temp1969[
16u]="type_params_opt"; static char _temp1972[ 16u]="struct_or_union"; static
char _temp1975[ 24u]="struct_declaration_list"; static char _temp1978[ 25u]="struct_declaration_list0";
static char _temp1981[ 21u]="init_declarator_list"; static char _temp1984[ 22u]="init_declarator_list0";
static char _temp1987[ 16u]="init_declarator"; static char _temp1990[ 19u]="struct_declaration";
static char _temp1993[ 25u]="specifier_qualifier_list"; static char _temp1996[
23u]="struct_declarator_list"; static char _temp1999[ 24u]="struct_declarator_list0";
static char _temp2002[ 18u]="struct_declarator"; static char _temp2005[ 17u]="tunion_specifier";
static char _temp2008[ 18u]="tunion_or_xtunion"; static char _temp2011[ 17u]="tunionfield_list";
static char _temp2014[ 18u]="tunionfield_scope"; static char _temp2017[ 12u]="tunionfield";
static char _temp2020[ 11u]="declarator"; static char _temp2023[ 18u]="direct_declarator";
static char _temp2026[ 8u]="pointer"; static char _temp2029[ 13u]="pointer_char";
static char _temp2032[ 8u]="rgn_opt"; static char _temp2035[ 4u]="rgn"; static
char _temp2038[ 20u]="type_qualifier_list"; static char _temp2041[ 20u]="parameter_type_list";
static char _temp2044[ 11u]="effect_set"; static char _temp2047[ 14u]="atomic_effect";
static char _temp2050[ 11u]="region_set"; static char _temp2053[ 15u]="parameter_list";
static char _temp2056[ 22u]="parameter_declaration"; static char _temp2059[ 16u]="identifier_list";
static char _temp2062[ 17u]="identifier_list0"; static char _temp2065[ 12u]="initializer";
static char _temp2068[ 18u]="array_initializer"; static char _temp2071[ 17u]="initializer_list";
static char _temp2074[ 12u]="designation"; static char _temp2077[ 16u]="designator_list";
static char _temp2080[ 11u]="designator"; static char _temp2083[ 10u]="type_name";
static char _temp2086[ 14u]="any_type_name"; static char _temp2089[ 15u]="type_name_list";
static char _temp2092[ 20u]="abstract_declarator"; static char _temp2095[ 27u]="direct_abstract_declarator";
static char _temp2098[ 10u]="statement"; static char _temp2101[ 18u]="labeled_statement";
static char _temp2104[ 21u]="expression_statement"; static char _temp2107[ 19u]="compound_statement";
static char _temp2110[ 16u]="block_item_list"; static char _temp2113[ 11u]="block_item";
static char _temp2116[ 20u]="selection_statement"; static char _temp2119[ 15u]="switch_clauses";
static char _temp2122[ 20u]="iteration_statement"; static char _temp2125[ 15u]="jump_statement";
static char _temp2128[ 8u]="pattern"; static char _temp2131[ 19u]="tuple_pattern_list";
static char _temp2134[ 20u]="tuple_pattern_list0"; static char _temp2137[ 14u]="field_pattern";
static char _temp2140[ 19u]="field_pattern_list"; static char _temp2143[ 20u]="field_pattern_list0";
static char _temp2146[ 11u]="expression"; static char _temp2149[ 22u]="assignment_expression";
static char _temp2152[ 20u]="assignment_operator"; static char _temp2155[ 23u]="conditional_expression";
static char _temp2158[ 20u]="constant_expression"; static char _temp2161[ 22u]="logical_or_expression";
static char _temp2164[ 23u]="logical_and_expression"; static char _temp2167[ 24u]="inclusive_or_expression";
static char _temp2170[ 24u]="exclusive_or_expression"; static char _temp2173[ 15u]="and_expression";
static char _temp2176[ 20u]="equality_expression"; static char _temp2179[ 22u]="relational_expression";
static char _temp2182[ 17u]="shift_expression"; static char _temp2185[ 20u]="additive_expression";
static char _temp2188[ 26u]="multiplicative_expression"; static char _temp2191[
16u]="cast_expression"; static char _temp2194[ 17u]="unary_expression"; static
char _temp2197[ 14u]="format_primop"; static char _temp2200[ 15u]="unary_operator";
static char _temp2203[ 19u]="postfix_expression"; static char _temp2206[ 19u]="primary_expression";
static char _temp2209[ 25u]="argument_expression_list"; static char _temp2212[
26u]="argument_expression_list0"; static char _temp2215[ 9u]="constant"; static
char _temp2218[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
232u]={{ _temp1525, _temp1525, _temp1525 + 2u},{ _temp1528, _temp1528, _temp1528
+ 6u},{ _temp1531, _temp1531, _temp1531 + 12u},{ _temp1534, _temp1534, _temp1534
+ 5u},{ _temp1537, _temp1537, _temp1537 + 9u},{ _temp1540, _temp1540, _temp1540
+ 7u},{ _temp1543, _temp1543, _temp1543 + 7u},{ _temp1546, _temp1546, _temp1546
+ 8u},{ _temp1549, _temp1549, _temp1549 + 5u},{ _temp1552, _temp1552, _temp1552
+ 5u},{ _temp1555, _temp1555, _temp1555 + 6u},{ _temp1558, _temp1558, _temp1558
+ 4u},{ _temp1561, _temp1561, _temp1561 + 5u},{ _temp1564, _temp1564, _temp1564
+ 6u},{ _temp1567, _temp1567, _temp1567 + 7u},{ _temp1570, _temp1570, _temp1570
+ 7u},{ _temp1573, _temp1573, _temp1573 + 9u},{ _temp1576, _temp1576, _temp1576
+ 6u},{ _temp1579, _temp1579, _temp1579 + 9u},{ _temp1582, _temp1582, _temp1582
+ 9u},{ _temp1585, _temp1585, _temp1585 + 7u},{ _temp1588, _temp1588, _temp1588
+ 6u},{ _temp1591, _temp1591, _temp1591 + 5u},{ _temp1594, _temp1594, _temp1594
+ 8u},{ _temp1597, _temp1597, _temp1597 + 7u},{ _temp1600, _temp1600, _temp1600
+ 3u},{ _temp1603, _temp1603, _temp1603 + 5u},{ _temp1606, _temp1606, _temp1606
+ 7u},{ _temp1609, _temp1609, _temp1609 + 6u},{ _temp1612, _temp1612, _temp1612
+ 3u},{ _temp1615, _temp1615, _temp1615 + 4u},{ _temp1618, _temp1618, _temp1618
+ 5u},{ _temp1621, _temp1621, _temp1621 + 9u},{ _temp1624, _temp1624, _temp1624
+ 6u},{ _temp1627, _temp1627, _temp1627 + 7u},{ _temp1630, _temp1630, _temp1630
+ 7u},{ _temp1633, _temp1633, _temp1633 + 5u},{ _temp1636, _temp1636, _temp1636
+ 8u},{ _temp1639, _temp1639, _temp1639 + 4u},{ _temp1642, _temp1642, _temp1642
+ 6u},{ _temp1645, _temp1645, _temp1645 + 4u},{ _temp1648, _temp1648, _temp1648
+ 6u},{ _temp1651, _temp1651, _temp1651 + 4u},{ _temp1654, _temp1654, _temp1654
+ 9u},{ _temp1657, _temp1657, _temp1657 + 9u},{ _temp1660, _temp1660, _temp1660
+ 6u},{ _temp1663, _temp1663, _temp1663 + 10u},{ _temp1666, _temp1666, _temp1666
+ 7u},{ _temp1669, _temp1669, _temp1669 + 8u},{ _temp1672, _temp1672, _temp1672
+ 5u},{ _temp1675, _temp1675, _temp1675 + 8u},{ _temp1678, _temp1678, _temp1678
+ 4u},{ _temp1681, _temp1681, _temp1681 + 7u},{ _temp1684, _temp1684, _temp1684
+ 7u},{ _temp1687, _temp1687, _temp1687 + 8u},{ _temp1690, _temp1690, _temp1690
+ 8u},{ _temp1693, _temp1693, _temp1693 + 6u},{ _temp1696, _temp1696, _temp1696
+ 7u},{ _temp1699, _temp1699, _temp1699 + 7u},{ _temp1702, _temp1702, _temp1702
+ 7u},{ _temp1705, _temp1705, _temp1705 + 9u},{ _temp1708, _temp1708, _temp1708
+ 7u},{ _temp1711, _temp1711, _temp1711 + 5u},{ _temp1714, _temp1714, _temp1714
+ 8u},{ _temp1717, _temp1717, _temp1717 + 7u},{ _temp1720, _temp1720, _temp1720
+ 7u},{ _temp1723, _temp1723, _temp1723 + 7u},{ _temp1726, _temp1726, _temp1726
+ 8u},{ _temp1729, _temp1729, _temp1729 + 9u},{ _temp1732, _temp1732, _temp1732
+ 6u},{ _temp1735, _temp1735, _temp1735 + 6u},{ _temp1738, _temp1738, _temp1738
+ 6u},{ _temp1741, _temp1741, _temp1741 + 6u},{ _temp1744, _temp1744, _temp1744
+ 7u},{ _temp1747, _temp1747, _temp1747 + 6u},{ _temp1750, _temp1750, _temp1750
+ 11u},{ _temp1753, _temp1753, _temp1753 + 11u},{ _temp1756, _temp1756,
_temp1756 + 11u},{ _temp1759, _temp1759, _temp1759 + 11u},{ _temp1762, _temp1762,
_temp1762 + 11u},{ _temp1765, _temp1765, _temp1765 + 12u},{ _temp1768, _temp1768,
_temp1768 + 13u},{ _temp1771, _temp1771, _temp1771 + 11u},{ _temp1774, _temp1774,
_temp1774 + 11u},{ _temp1777, _temp1777, _temp1777 + 10u},{ _temp1780, _temp1780,
_temp1780 + 9u},{ _temp1783, _temp1783, _temp1783 + 11u},{ _temp1786, _temp1786,
_temp1786 + 12u},{ _temp1789, _temp1789, _temp1789 + 11u},{ _temp1792, _temp1792,
_temp1792 + 17u},{ _temp1795, _temp1795, _temp1795 + 7u},{ _temp1798, _temp1798,
_temp1798 + 19u},{ _temp1801, _temp1801, _temp1801 + 18u},{ _temp1804, _temp1804,
_temp1804 + 9u},{ _temp1807, _temp1807, _temp1807 + 16u},{ _temp1810, _temp1810,
_temp1810 + 18u},{ _temp1813, _temp1813, _temp1813 + 10u},{ _temp1816, _temp1816,
_temp1816 + 4u},{ _temp1819, _temp1819, _temp1819 + 4u},{ _temp1822, _temp1822,
_temp1822 + 4u},{ _temp1825, _temp1825, _temp1825 + 4u},{ _temp1828, _temp1828,
_temp1828 + 4u},{ _temp1831, _temp1831, _temp1831 + 4u},{ _temp1834, _temp1834,
_temp1834 + 4u},{ _temp1837, _temp1837, _temp1837 + 4u},{ _temp1840, _temp1840,
_temp1840 + 4u},{ _temp1843, _temp1843, _temp1843 + 4u},{ _temp1846, _temp1846,
_temp1846 + 4u},{ _temp1849, _temp1849, _temp1849 + 4u},{ _temp1852, _temp1852,
_temp1852 + 4u},{ _temp1855, _temp1855, _temp1855 + 4u},{ _temp1858, _temp1858,
_temp1858 + 4u},{ _temp1861, _temp1861, _temp1861 + 4u},{ _temp1864, _temp1864,
_temp1864 + 4u},{ _temp1867, _temp1867, _temp1867 + 4u},{ _temp1870, _temp1870,
_temp1870 + 4u},{ _temp1873, _temp1873, _temp1873 + 4u},{ _temp1876, _temp1876,
_temp1876 + 4u},{ _temp1879, _temp1879, _temp1879 + 4u},{ _temp1882, _temp1882,
_temp1882 + 4u},{ _temp1885, _temp1885, _temp1885 + 4u},{ _temp1888, _temp1888,
_temp1888 + 4u},{ _temp1891, _temp1891, _temp1891 + 4u},{ _temp1894, _temp1894,
_temp1894 + 4u},{ _temp1897, _temp1897, _temp1897 + 5u},{ _temp1900, _temp1900,
_temp1900 + 17u},{ _temp1903, _temp1903, _temp1903 + 21u},{ _temp1906, _temp1906,
_temp1906 + 20u},{ _temp1909, _temp1909, _temp1909 + 21u},{ _temp1912, _temp1912,
_temp1912 + 13u},{ _temp1915, _temp1915, _temp1915 + 15u},{ _temp1918, _temp1918,
_temp1918 + 17u},{ _temp1921, _temp1921, _temp1921 + 19u},{ _temp1924, _temp1924,
_temp1924 + 12u},{ _temp1927, _temp1927, _temp1927 + 17u},{ _temp1930, _temp1930,
_temp1930 + 23u},{ _temp1933, _temp1933, _temp1933 + 24u},{ _temp1936, _temp1936,
_temp1936 + 15u},{ _temp1939, _temp1939, _temp1939 + 11u},{ _temp1942, _temp1942,
_temp1942 + 15u},{ _temp1945, _temp1945, _temp1945 + 10u},{ _temp1948, _temp1948,
_temp1948 + 15u},{ _temp1951, _temp1951, _temp1951 + 5u},{ _temp1954, _temp1954,
_temp1954 + 15u},{ _temp1957, _temp1957, _temp1957 + 15u},{ _temp1960, _temp1960,
_temp1960 + 11u},{ _temp1963, _temp1963, _temp1963 + 22u},{ _temp1966, _temp1966,
_temp1966 + 26u},{ _temp1969, _temp1969, _temp1969 + 16u},{ _temp1972, _temp1972,
_temp1972 + 16u},{ _temp1975, _temp1975, _temp1975 + 24u},{ _temp1978, _temp1978,
_temp1978 + 25u},{ _temp1981, _temp1981, _temp1981 + 21u},{ _temp1984, _temp1984,
_temp1984 + 22u},{ _temp1987, _temp1987, _temp1987 + 16u},{ _temp1990, _temp1990,
_temp1990 + 19u},{ _temp1993, _temp1993, _temp1993 + 25u},{ _temp1996, _temp1996,
_temp1996 + 23u},{ _temp1999, _temp1999, _temp1999 + 24u},{ _temp2002, _temp2002,
_temp2002 + 18u},{ _temp2005, _temp2005, _temp2005 + 17u},{ _temp2008, _temp2008,
_temp2008 + 18u},{ _temp2011, _temp2011, _temp2011 + 17u},{ _temp2014, _temp2014,
_temp2014 + 18u},{ _temp2017, _temp2017, _temp2017 + 12u},{ _temp2020, _temp2020,
_temp2020 + 11u},{ _temp2023, _temp2023, _temp2023 + 18u},{ _temp2026, _temp2026,
_temp2026 + 8u},{ _temp2029, _temp2029, _temp2029 + 13u},{ _temp2032, _temp2032,
_temp2032 + 8u},{ _temp2035, _temp2035, _temp2035 + 4u},{ _temp2038, _temp2038,
_temp2038 + 20u},{ _temp2041, _temp2041, _temp2041 + 20u},{ _temp2044, _temp2044,
_temp2044 + 11u},{ _temp2047, _temp2047, _temp2047 + 14u},{ _temp2050, _temp2050,
_temp2050 + 11u},{ _temp2053, _temp2053, _temp2053 + 15u},{ _temp2056, _temp2056,
_temp2056 + 22u},{ _temp2059, _temp2059, _temp2059 + 16u},{ _temp2062, _temp2062,
_temp2062 + 17u},{ _temp2065, _temp2065, _temp2065 + 12u},{ _temp2068, _temp2068,
_temp2068 + 18u},{ _temp2071, _temp2071, _temp2071 + 17u},{ _temp2074, _temp2074,
_temp2074 + 12u},{ _temp2077, _temp2077, _temp2077 + 16u},{ _temp2080, _temp2080,
_temp2080 + 11u},{ _temp2083, _temp2083, _temp2083 + 10u},{ _temp2086, _temp2086,
_temp2086 + 14u},{ _temp2089, _temp2089, _temp2089 + 15u},{ _temp2092, _temp2092,
_temp2092 + 20u},{ _temp2095, _temp2095, _temp2095 + 27u},{ _temp2098, _temp2098,
_temp2098 + 10u},{ _temp2101, _temp2101, _temp2101 + 18u},{ _temp2104, _temp2104,
_temp2104 + 21u},{ _temp2107, _temp2107, _temp2107 + 19u},{ _temp2110, _temp2110,
_temp2110 + 16u},{ _temp2113, _temp2113, _temp2113 + 11u},{ _temp2116, _temp2116,
_temp2116 + 20u},{ _temp2119, _temp2119, _temp2119 + 15u},{ _temp2122, _temp2122,
_temp2122 + 20u},{ _temp2125, _temp2125, _temp2125 + 15u},{ _temp2128, _temp2128,
_temp2128 + 8u},{ _temp2131, _temp2131, _temp2131 + 19u},{ _temp2134, _temp2134,
_temp2134 + 20u},{ _temp2137, _temp2137, _temp2137 + 14u},{ _temp2140, _temp2140,
_temp2140 + 19u},{ _temp2143, _temp2143, _temp2143 + 20u},{ _temp2146, _temp2146,
_temp2146 + 11u},{ _temp2149, _temp2149, _temp2149 + 22u},{ _temp2152, _temp2152,
_temp2152 + 20u},{ _temp2155, _temp2155, _temp2155 + 23u},{ _temp2158, _temp2158,
_temp2158 + 20u},{ _temp2161, _temp2161, _temp2161 + 22u},{ _temp2164, _temp2164,
_temp2164 + 23u},{ _temp2167, _temp2167, _temp2167 + 24u},{ _temp2170, _temp2170,
_temp2170 + 24u},{ _temp2173, _temp2173, _temp2173 + 15u},{ _temp2176, _temp2176,
_temp2176 + 20u},{ _temp2179, _temp2179, _temp2179 + 22u},{ _temp2182, _temp2182,
_temp2182 + 17u},{ _temp2185, _temp2185, _temp2185 + 20u},{ _temp2188, _temp2188,
_temp2188 + 26u},{ _temp2191, _temp2191, _temp2191 + 16u},{ _temp2194, _temp2194,
_temp2194 + 17u},{ _temp2197, _temp2197, _temp2197 + 14u},{ _temp2200, _temp2200,
_temp2200 + 15u},{ _temp2203, _temp2203, _temp2203 + 19u},{ _temp2206, _temp2206,
_temp2206 + 19u},{ _temp2209, _temp2209, _temp2209 + 25u},{ _temp2212, _temp2212,
_temp2212 + 26u},{ _temp2215, _temp2215, _temp2215 + 9u},{ _temp2218, _temp2218,
_temp2218 + 20u}}; static short Cyc_yyr1[ 395u]={ (short)0, (short)124, (short)125,
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
short* _temp4185=({ unsigned int _temp4182= 200u; short* _temp4183=( short*)
GC_malloc_atomic( sizeof( short) * _temp4182);{ unsigned int _temp4184=
_temp4182; unsigned int i; for( i= 0; i < _temp4184; i ++){ _temp4183[ i]=
(short)0;}}; _temp4183;}); struct _tagged_ptr1 _temp4186; _temp4186.curr=
_temp4185; _temp4186.base= _temp4185; _temp4186.last_plus_one= _temp4185 + 200;
_temp4186;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=({ void** _temp4180=({
unsigned int _temp4177= 200u; void** _temp4178=( void**) GC_malloc( sizeof( void*)
* _temp4177);{ unsigned int _temp4179= _temp4177; unsigned int i; for( i= 0; i <
_temp4179; i ++){ _temp4178[ i]= Cyc_yylval;}}; _temp4178;}); struct
_tagged_ptr2 _temp4181; _temp4181.curr= _temp4180; _temp4181.base= _temp4180;
_temp4181.last_plus_one= _temp4180 + 200; _temp4181;}); int yylsp_offset; struct
_tagged_ptr3 yyls=({ struct Cyc_Yyltype* _temp4175=({ unsigned int _temp4172=
200u; struct Cyc_Yyltype* _temp4173=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp4172);{ unsigned int _temp4174= _temp4172;
unsigned int i; for( i= 0; i < _temp4174; i ++){ _temp4173[ i]= Cyc_yynewloc();}};
_temp4173;}); struct _tagged_ptr3 _temp4176; _temp4176.curr= _temp4175;
_temp4176.base= _temp4175; _temp4176.last_plus_one= _temp4175 + 200; _temp4176;});
int yystacksize= 200; void* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), ++ yyssp_offset)=(
short) yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){
Cyc_yyerror(({ char* _temp2219="parser stack overflow"; struct _tagged_string
_temp2220; _temp2220.curr= _temp2219; _temp2220.base= _temp2219; _temp2220.last_plus_one=
_temp2219 + 22; _temp2220;}));( void) _throw(( void*) Cyc_Yystack_overflow);}
yystacksize *= 2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct
_tagged_ptr1 yyss1=({ unsigned int _temp2229=( unsigned int) yystacksize; short*
_temp2230=( short*) GC_malloc_atomic( sizeof( short) * _temp2229); struct
_tagged_ptr1 _temp2232={ _temp2230, _temp2230, _temp2230 + _temp2229};{
unsigned int _temp2231= _temp2229; unsigned int i; for( i= 0; i < _temp2231; i
++){ _temp2230[ i]= (short)0;}}; _temp2232;}); struct _tagged_ptr2 yyvs1=({
unsigned int _temp2225=( unsigned int) yystacksize; void** _temp2226=( void**)
GC_malloc( sizeof( void*) * _temp2225); struct _tagged_ptr2 _temp2228={
_temp2226, _temp2226, _temp2226 + _temp2225};{ unsigned int _temp2227= _temp2225;
unsigned int i; for( i= 0; i < _temp2227; i ++){ _temp2226[ i]= Cyc_yylval;}};
_temp2228;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2221=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2222=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2221); struct _tagged_ptr3 _temp2224={
_temp2222, _temp2222, _temp2222 + _temp2221};{ unsigned int _temp2223= _temp2221;
unsigned int i; for( i= 0; i < _temp2223; i ++){ _temp2222[ i]= Cyc_yynewloc();}};
_temp2224;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*(( short*(*)( struct
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
1) - yylen);} switch( yyn){ case 1: _LL2233: yyval=*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); break; case 2:
_LL2234: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2236=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2236[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2237; _temp2237.tag= Cyc_DeclList_tok; _temp2237.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2237;});
_temp2236;}); break; case 3: _LL2235: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2239=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2239[ 0]=({ struct Cyc_DeclList_tok_struct _temp2240; _temp2240.tag= Cyc_DeclList_tok;
_temp2240.f1=({ struct Cyc_List_List* _temp2241=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2241->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2242=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2242->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2243=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2243[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2244; _temp2244.tag= Cyc_Absyn_Using_d;
_temp2244.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2244.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2244;}); _temp2243;})); _temp2242->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2242;}); _temp2241->tl= 0; _temp2241;}); _temp2240;}); _temp2239;}); Cyc_Lex_leave_using();
break; case 4: _LL2238: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
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
yyvsp_offset - 4)); _temp2251.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2251;}); _temp2250;})); _temp2249->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2249;}); _temp2248->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2248;});
_temp2247;}); _temp2246;}); break; case 5: _LL2245: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2253=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2253[ 0]=({ struct Cyc_DeclList_tok_struct _temp2254; _temp2254.tag= Cyc_DeclList_tok;
_temp2254.f1=({ struct Cyc_List_List* _temp2255=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2255->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2256=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2256->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2257=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2257[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2258; _temp2258.tag=
Cyc_Absyn_Namespace_d; _temp2258.f1=({ struct _tagged_string* _temp2259=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2259[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2259;}); _temp2258.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2258;}); _temp2257;})); _temp2256->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2256;}); _temp2255->tl= 0; _temp2255;}); _temp2254;}); _temp2253;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2252: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2261=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2261[ 0]=({ struct Cyc_DeclList_tok_struct _temp2262; _temp2262.tag= Cyc_DeclList_tok;
_temp2262.f1=({ struct Cyc_List_List* _temp2263=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2263->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2264=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2264->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2265=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2265[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2266; _temp2266.tag=
Cyc_Absyn_Namespace_d; _temp2266.f1=({ struct _tagged_string* _temp2267=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2267[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2267;}); _temp2266.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2266;}); _temp2265;})); _temp2264->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2264;}); _temp2263->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2263;});
_temp2262;}); _temp2261;}); break; case 7: _LL2260: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),({ char*
_temp2269="C"; struct _tagged_string _temp2270; _temp2270.curr= _temp2269;
_temp2270.base= _temp2269; _temp2270.last_plus_one= _temp2269 + 2; _temp2270;}))
!= 0){ Cyc_Parse_err(({ char* _temp2271="only extern \"C\" { ... } is allowed";
struct _tagged_string _temp2272; _temp2272.curr= _temp2271; _temp2272.base=
_temp2271; _temp2272.last_plus_one= _temp2271 + 35; _temp2272;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2273=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2273[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2274; _temp2274.tag= Cyc_DeclList_tok; _temp2274.f1=({ struct Cyc_List_List*
_temp2275=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2275->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2276=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2276->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2277=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2277[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2278; _temp2278.tag= Cyc_Absyn_ExternC_d;
_temp2278.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2278;}); _temp2277;})); _temp2276->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2276;}); _temp2275->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2275;}); _temp2274;}); _temp2273;}); break;
case 8: _LL2268: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2280=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2280[ 0]=({ struct Cyc_DeclList_tok_struct _temp2281; _temp2281.tag= Cyc_DeclList_tok;
_temp2281.f1= 0; _temp2281;}); _temp2280;}); break; case 9: _LL2279: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2283=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2283[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2284; _temp2284.tag= Cyc_DeclList_tok; _temp2284.f1=({ struct Cyc_List_List*
_temp2285=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2285->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2286=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2286[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2287; _temp2287.tag= Cyc_Absyn_Fn_d;
_temp2287.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2287;}); _temp2286;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2285->tl= 0; _temp2285;}); _temp2284;}); _temp2283;}); break; case 10:
_LL2282: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2288: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2290=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2290[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2291; _temp2291.tag= Cyc_FnDecl_tok; _temp2291.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2291;}); _temp2290;}); break; case 12: _LL2289:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2293=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2293[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2294; _temp2294.tag= Cyc_FnDecl_tok; _temp2294.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2295=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2295->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2295;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2294;}); _temp2293;}); break; case 13: _LL2292: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2297=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2297[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2298; _temp2298.tag= Cyc_FnDecl_tok;
_temp2298.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
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
_temp2298;}); _temp2297;}); break; case 14: _LL2296: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2300=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2300[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2301; _temp2301.tag= Cyc_FnDecl_tok;
_temp2301.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2302=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2302->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2302;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2301;});
_temp2300;}); break; case 15: _LL2299: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2304=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2304[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2305; _temp2305.tag= Cyc_FnDecl_tok;
_temp2305.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2306=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2306->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2306;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2305;});
_temp2304;}); break; case 16: _LL2303: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2308=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2308[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2309; _temp2309.tag= Cyc_FnDecl_tok;
_temp2309.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2310=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2310->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2310;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2309;});
_temp2308;}); break; case 17: _LL2307: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2311: Cyc_Lex_leave_using();
break; case 19: _LL2312: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2314=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2314[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2314;})); yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2313: Cyc_Lex_leave_namespace(); break; case
21: _LL2315: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2317=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2317[ 0]=({ struct Cyc_DeclList_tok_struct _temp2318; _temp2318.tag= Cyc_DeclList_tok;
_temp2318.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2318;}); _temp2317;}); break; case 22: _LL2316: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2320=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2320[ 0]=({ struct Cyc_DeclList_tok_struct _temp2321; _temp2321.tag= Cyc_DeclList_tok;
_temp2321.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2321;}); _temp2320;}); break; case 23: _LL2319: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2323=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2323[ 0]=({ struct Cyc_DeclList_tok_struct _temp2324; _temp2324.tag= Cyc_DeclList_tok;
_temp2324.f1=({ struct Cyc_List_List* _temp2325=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2325->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2325->tl= 0; _temp2325;}); _temp2324;});
_temp2323;}); break; case 24: _LL2322: { struct Cyc_List_List* _temp2327= 0;
goto _LL2328; _LL2328:{ struct Cyc_List_List* _temp2329= Cyc_yyget_IdList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); goto
_LL2330; _LL2330: for( 0; _temp2329 != 0; _temp2329=(( struct Cyc_List_List*)
_check_null( _temp2329))->tl){ struct _tagged_string* _temp2331=( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2329))->hd; goto
_LL2332; _LL2332: { struct _tuple1* _temp2336=({ struct _tuple1* _temp2333=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2333->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp2334=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2334[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp2335; _temp2335.tag= Cyc_Absyn_Rel_n; _temp2335.f1= 0; _temp2335;});
_temp2334;}); _temp2333->f2= _temp2331; _temp2333;}); goto _LL2337; _LL2337: {
struct Cyc_Absyn_Vardecl* _temp2338= Cyc_Absyn_new_vardecl( _temp2336, Cyc_Absyn_wildtyp(),
0); goto _LL2339; _LL2339: _temp2327=({ struct Cyc_List_List* _temp2340=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2340->hd=( void*)
_temp2338; _temp2340->tl= _temp2327; _temp2340;});}}}} _temp2327=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2327); yyval=( void*)({ struct
Cyc_DeclList_tok_struct* _temp2341=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp2341[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2342; _temp2342.tag= Cyc_DeclList_tok; _temp2342.f1=({ struct Cyc_List_List*
_temp2343=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2343->hd=( void*) Cyc_Absyn_letv_decl( _temp2327, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2343->tl= 0; _temp2343;}); _temp2342;}); _temp2341;}); break;} case 25:
_LL2326: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 26:
_LL2344: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2346=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2346[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2347; _temp2347.tag= Cyc_DeclList_tok; _temp2347.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2347;});
_temp2346;}); break; case 27: _LL2345: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2349=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2349[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2350; _temp2350.tag= Cyc_DeclSpec_tok;
_temp2350.f1=({ struct Cyc_Parse_Declaration_spec* _temp2351=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2351->sc=({ struct
Cyc_Core_Opt* _temp2352=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2352->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2352;}); _temp2351->tq= Cyc_Absyn_empty_tqual();
_temp2351->type_specs= 0; _temp2351->is_inline= 0; _temp2351->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2351;});
_temp2350;}); _temp2349;}); break; case 28: _LL2348: if(( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(({
char* _temp2354="Only one storage class is allowed in a declaration"; struct
_tagged_string _temp2355; _temp2355.curr= _temp2354; _temp2355.base= _temp2354;
_temp2355.last_plus_one= _temp2354 + 51; _temp2355;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2356=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2356[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2357; _temp2357.tag= Cyc_DeclSpec_tok; _temp2357.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2358=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2358->sc=({ struct Cyc_Core_Opt* _temp2359=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2359->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2359;});
_temp2358->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2358->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2358->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2358->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2358;}); _temp2357;}); _temp2356;}); break; case 29: _LL2353: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2361=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2361[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2362; _temp2362.tag= Cyc_DeclSpec_tok; _temp2362.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2363=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2363->sc= 0; _temp2363->tq= Cyc_Absyn_empty_tqual(); _temp2363->type_specs=({
struct Cyc_List_List* _temp2364=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2364->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2364->tl=
0; _temp2364;}); _temp2363->is_inline= 0; _temp2363->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2363;});
_temp2362;}); _temp2361;}); break; case 30: _LL2360: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2366=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2366[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2367; _temp2367.tag= Cyc_DeclSpec_tok;
_temp2367.f1=({ struct Cyc_Parse_Declaration_spec* _temp2368=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2368->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2368->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2368->type_specs=({ struct Cyc_List_List* _temp2369=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2369->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2369->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2369;}); _temp2368->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2368->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2368;}); _temp2367;}); _temp2366;}); break; case 31: _LL2365: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2371=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2371[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2372; _temp2372.tag= Cyc_DeclSpec_tok; _temp2372.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2373=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2373->sc= 0; _temp2373->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2373->type_specs= 0; _temp2373->is_inline=
0; _temp2373->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2373;}); _temp2372;}); _temp2371;}); break;
case 32: _LL2370: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2375=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2375[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2376; _temp2376.tag= Cyc_DeclSpec_tok;
_temp2376.f1=({ struct Cyc_Parse_Declaration_spec* _temp2377=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2377->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2377->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2377->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2377->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2377->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2377;}); _temp2376;}); _temp2375;}); break; case 33: _LL2374: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2379=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2379[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2380; _temp2380.tag= Cyc_DeclSpec_tok; _temp2380.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2381=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2381->sc= 0; _temp2381->tq= Cyc_Absyn_empty_tqual(); _temp2381->type_specs=
0; _temp2381->is_inline= 1; _temp2381->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2381;});
_temp2380;}); _temp2379;}); break; case 34: _LL2378: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2383=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2383[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2384; _temp2384.tag= Cyc_DeclSpec_tok;
_temp2384.f1=({ struct Cyc_Parse_Declaration_spec* _temp2385=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2385->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2385->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2385->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2385->is_inline= 1; _temp2385->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2385;}); _temp2384;}); _temp2383;}); break; case 35: _LL2382: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2387=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2387[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2388; _temp2388.tag= Cyc_StorageClass_tok;
_temp2388.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2388;}); _temp2387;});
break; case 36: _LL2386: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2390=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2390[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2391; _temp2391.tag=
Cyc_StorageClass_tok; _temp2391.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2391;}); _temp2390;}); break; case 37: _LL2389: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2393=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2393[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2394; _temp2394.tag=
Cyc_StorageClass_tok; _temp2394.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2394;}); _temp2393;}); break; case 38: _LL2392: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2396=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2396[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2397; _temp2397.tag=
Cyc_StorageClass_tok; _temp2397.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2397;}); _temp2396;}); break; case 39: _LL2395: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),({ char*
_temp2399="C"; struct _tagged_string _temp2400; _temp2400.curr= _temp2399;
_temp2400.base= _temp2399; _temp2400.last_plus_one= _temp2399 + 2; _temp2400;}))
!= 0){ Cyc_Parse_err(({ char* _temp2401="only extern or extern \"C\" is allowed";
struct _tagged_string _temp2402; _temp2402.curr= _temp2401; _temp2402.base=
_temp2401; _temp2402.last_plus_one= _temp2401 + 37; _temp2402;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2403=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2403[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2404; _temp2404.tag= Cyc_StorageClass_tok;
_temp2404.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2404;}); _temp2403;});
break; case 40: _LL2398: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2406=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2406[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2407; _temp2407.tag=
Cyc_StorageClass_tok; _temp2407.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2407;}); _temp2406;}); break; case 41: _LL2405: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2409=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2409[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2410; _temp2410.tag=
Cyc_StorageClass_tok; _temp2410.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2410;}); _temp2409;}); break; case 42: _LL2408: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2412=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2412[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2413; _temp2413.tag=
Cyc_AttributeList_tok; _temp2413.f1= 0; _temp2413;}); _temp2412;}); break; case
43: _LL2411: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 44: _LL2414: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2416=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2416[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2417; _temp2417.tag=
Cyc_AttributeList_tok; _temp2417.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2417;}); _temp2416;}); break;
case 45: _LL2415: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2419=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2419[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2420; _temp2420.tag=
Cyc_AttributeList_tok; _temp2420.f1=({ struct Cyc_List_List* _temp2421=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2421->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2421->tl= 0; _temp2421;}); _temp2420;}); _temp2419;}); break; case 46:
_LL2418: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2423=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2423[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2424; _temp2424.tag=
Cyc_AttributeList_tok; _temp2424.f1=({ struct Cyc_List_List* _temp2425=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2425->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2425->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2425;}); _temp2424;}); _temp2423;}); break; case 47:
_LL2422: { struct _tagged_string _temp2427= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2428; _LL2428: if((((({ struct
_tagged_string _temp2429= _temp2427;( unsigned int)( _temp2429.last_plus_one -
_temp2429.curr);}) > 4?*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp2427, sizeof( char), 0) =='_': 0)?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2427, sizeof( char), 1) =='_': 0)?*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2427, sizeof( char),( int)(({ struct _tagged_string _temp2430= _temp2427;(
unsigned int)( _temp2430.last_plus_one - _temp2430.curr);}) - 2)) =='_': 0)?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2427, sizeof( char),( int)(({ struct
_tagged_string _temp2431= _temp2427;( unsigned int)( _temp2431.last_plus_one -
_temp2431.curr);}) - 3)) =='_': 0){ _temp2427= Cyc_String_substring( _temp2427,
2,({ struct _tagged_string _temp2432= _temp2427;( unsigned int)( _temp2432.last_plus_one
- _temp2432.curr);}) - 5);}{ int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp(
_temp2427,( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f1) ==
0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2433=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2433[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2434; _temp2434.tag= Cyc_Attribute_tok; _temp2434.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp2434;}); _temp2433;}); break;}} if( i == 16u){ Cyc_Parse_err(({ char*
_temp2435="unrecognized attribute"; struct _tagged_string _temp2436; _temp2436.curr=
_temp2435; _temp2436.base= _temp2435; _temp2436.last_plus_one= _temp2435 + 23;
_temp2436;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp2437=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2437[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2438; _temp2438.tag= Cyc_Attribute_tok; _temp2438.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp2438;}); _temp2437;});} break;}} case
48: _LL2426: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2440=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2440[ 0]=({ struct Cyc_Attribute_tok_struct _temp2441; _temp2441.tag= Cyc_Attribute_tok;
_temp2441.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2441;}); _temp2440;});
break; case 49: _LL2439: { struct _tagged_string _temp2443= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); goto
_LL2444; _LL2444: { struct _tuple16 _temp2447; int _temp2448; void* _temp2450;
struct _tuple16* _temp2445= Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2447=* _temp2445; _LL2451: _temp2450= _temp2447.f1;
goto _LL2449; _LL2449: _temp2448= _temp2447.f2; goto _LL2446; _LL2446: { void* a;
if( Cyc_String_zstrcmp( _temp2443,({ char* _temp2452="regparm"; struct
_tagged_string _temp2453; _temp2453.curr= _temp2452; _temp2453.base= _temp2452;
_temp2453.last_plus_one= _temp2452 + 8; _temp2453;})) == 0? 1: Cyc_String_zstrcmp(
_temp2443,({ char* _temp2454="__regparm__"; struct _tagged_string _temp2455;
_temp2455.curr= _temp2454; _temp2455.base= _temp2454; _temp2455.last_plus_one=
_temp2454 + 12; _temp2455;})) == 0){ if( _temp2448 <= 0? 1: _temp2448 > 3){ Cyc_Parse_err(({
char* _temp2456="regparm requires value between 1 and 3"; struct _tagged_string
_temp2457; _temp2457.curr= _temp2456; _temp2457.base= _temp2456; _temp2457.last_plus_one=
_temp2456 + 39; _temp2457;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));} a=( void*)({
struct Cyc_Absyn_Regparm_att_struct* _temp2458=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2458[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2459; _temp2459.tag= Cyc_Absyn_Regparm_att;
_temp2459.f1= _temp2448; _temp2459;}); _temp2458;});} else{ if( Cyc_String_zstrcmp(
_temp2443,({ char* _temp2460="aligned"; struct _tagged_string _temp2461;
_temp2461.curr= _temp2460; _temp2461.base= _temp2460; _temp2461.last_plus_one=
_temp2460 + 8; _temp2461;})) == 0? 1: Cyc_String_zstrcmp( _temp2443,({ char*
_temp2462="__aligned__"; struct _tagged_string _temp2463; _temp2463.curr=
_temp2462; _temp2463.base= _temp2462; _temp2463.last_plus_one= _temp2462 + 12;
_temp2463;})) == 0){ if( _temp2448 < 0){ Cyc_Parse_err(({ char* _temp2464="aligned requires positive power of two";
struct _tagged_string _temp2465; _temp2465.curr= _temp2464; _temp2465.base=
_temp2464; _temp2465.last_plus_one= _temp2464 + 39; _temp2465;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2448; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(({ char* _temp2466="aligned requires positive power of two";
struct _tagged_string _temp2467; _temp2467.curr= _temp2466; _temp2467.base=
_temp2466; _temp2467.last_plus_one= _temp2466 + 39; _temp2467;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2468=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2468[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2469; _temp2469.tag= Cyc_Absyn_Aligned_att;
_temp2469.f1= _temp2448; _temp2469;}); _temp2468;});}} else{ Cyc_Parse_err(({
char* _temp2470="unrecognized attribute"; struct _tagged_string _temp2471;
_temp2471.curr= _temp2470; _temp2471.base= _temp2470; _temp2471.last_plus_one=
_temp2470 + 23; _temp2471;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line)); a=( void*)
Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2472=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2472[ 0]=({ struct Cyc_Attribute_tok_struct _temp2473; _temp2473.tag= Cyc_Attribute_tok;
_temp2473.f1=( void*) a; _temp2473;}); _temp2472;}); break;}}} case 50: _LL2442: {
struct _tagged_string _temp2475= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2476; _LL2476: { struct
_tagged_string _temp2477= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2478; _LL2478: { void* a; if( Cyc_String_zstrcmp(
_temp2475,({ char* _temp2479="section"; struct _tagged_string _temp2480;
_temp2480.curr= _temp2479; _temp2480.base= _temp2479; _temp2480.last_plus_one=
_temp2479 + 8; _temp2480;})) == 0? 1: Cyc_String_zstrcmp( _temp2475,({ char*
_temp2481="__section__"; struct _tagged_string _temp2482; _temp2482.curr=
_temp2481; _temp2482.base= _temp2481; _temp2482.last_plus_one= _temp2481 + 12;
_temp2482;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct* _temp2483=(
struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2483[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2484; _temp2484.tag=
Cyc_Absyn_Section_att; _temp2484.f1= _temp2477; _temp2484;}); _temp2483;});}
else{ Cyc_Parse_err(({ char* _temp2485="unrecognized attribute"; struct
_tagged_string _temp2486; _temp2486.curr= _temp2485; _temp2486.base= _temp2485;
_temp2486.last_plus_one= _temp2485 + 23; _temp2486;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2487=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2487[ 0]=({ struct Cyc_Attribute_tok_struct _temp2488; _temp2488.tag= Cyc_Attribute_tok;
_temp2488.f1=( void*) a; _temp2488;}); _temp2487;}); break;}}} case 51: _LL2474:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2490=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2490[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2491; _temp2491.tag= Cyc_TypeSpecifier_tok;
_temp2491.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2491;}); _temp2490;}); break; case 52: _LL2489: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2493=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2493[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2494; _temp2494.tag=
Cyc_TypeSpecifier_tok; _temp2494.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2494;});
_temp2493;}); break; case 53: _LL2492: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2496=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2496[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2497; _temp2497.tag=
Cyc_TypeSpecifier_tok; _temp2497.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2497;}); _temp2496;}); break; case 54: _LL2495:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2499=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2499[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2500; _temp2500.tag= Cyc_TypeSpecifier_tok;
_temp2500.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2501=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2501[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2502; _temp2502.tag=
Cyc_Parse_Short_spec; _temp2502.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2502;});
_temp2501;})); _temp2500;}); _temp2499;}); break; case 55: _LL2498: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2504=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2504[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2505; _temp2505.tag= Cyc_TypeSpecifier_tok;
_temp2505.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2505;}); _temp2504;}); break; case 56: _LL2503: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2507=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2507[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2508; _temp2508.tag=
Cyc_TypeSpecifier_tok; _temp2508.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2509=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2509[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2510; _temp2510.tag= Cyc_Parse_Long_spec;
_temp2510.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2510;});
_temp2509;})); _temp2508;}); _temp2507;}); break; case 57: _LL2506: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2512=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2512[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2513; _temp2513.tag= Cyc_TypeSpecifier_tok;
_temp2513.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2513;}); _temp2512;}); break; case 58: _LL2511: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2515=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2515[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2516; _temp2516.tag=
Cyc_TypeSpecifier_tok; _temp2516.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2516;}); _temp2515;}); break; case 59: _LL2514:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2518=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2518[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2519; _temp2519.tag= Cyc_TypeSpecifier_tok;
_temp2519.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2520=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2520[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2521; _temp2521.tag=
Cyc_Parse_Signed_spec; _temp2521.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2521;});
_temp2520;})); _temp2519;}); _temp2518;}); break; case 60: _LL2517: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2523=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2523[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2524; _temp2524.tag= Cyc_TypeSpecifier_tok;
_temp2524.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2525=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2525[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2526; _temp2526.tag= Cyc_Parse_Unsigned_spec; _temp2526.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2526;}); _temp2525;})); _temp2524;}); _temp2523;}); break; case 61: _LL2522:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2527: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2528: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 64:
_LL2529: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2531=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2531[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2532; _temp2532.tag=
Cyc_TypeSpecifier_tok; _temp2532.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2532;}); _temp2531;}); break; case 65: _LL2530: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2534=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2534[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2535; _temp2535.tag=
Cyc_TypeSpecifier_tok; _temp2535.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2535;}); _temp2534;}); break; case 66: _LL2533: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2537=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2537[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2538; _temp2538.tag=
Cyc_TypeSpecifier_tok; _temp2538.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2539=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2539[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2540; _temp2540.tag= Cyc_Absyn_TypedefType;
_temp2540.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2540.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2540.f3= 0; _temp2540;}); _temp2539;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2538;}); _temp2537;}); break; case 67: _LL2536: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2542=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2542[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2543; _temp2543.tag=
Cyc_TypeSpecifier_tok; _temp2543.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2544=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2544[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2545; _temp2545.tag= Cyc_Absyn_TupleType;
_temp2545.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2545;});
_temp2544;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2543;});
_temp2542;}); break; case 68: _LL2541: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2547=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2547[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2548; _temp2548.tag=
Cyc_TypeSpecifier_tok; _temp2548.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2549=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2549[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2550; _temp2550.tag= Cyc_Absyn_RgnHandleType;
_temp2550.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2550;}); _temp2549;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2548;}); _temp2547;}); break; case 69: _LL2546: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2552=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2552[ 0]=({ struct Cyc_Kind_tok_struct _temp2553; _temp2553.tag= Cyc_Kind_tok;
_temp2553.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2553;}); _temp2552;}); break; case 70: _LL2551: { struct _tagged_string*
_temp2557; void* _temp2559; struct _tuple1 _temp2555=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2560:
_temp2559= _temp2555.f1; goto _LL2558; _LL2558: _temp2557= _temp2555.f2; goto
_LL2556; _LL2556: if( _temp2559 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(({
char* _temp2561="bad kind in type specifier"; struct _tagged_string _temp2562;
_temp2562.curr= _temp2561; _temp2562.base= _temp2561; _temp2562.last_plus_one=
_temp2561 + 27; _temp2562;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Kind_tok_struct* _temp2563=( struct Cyc_Kind_tok_struct*) GC_malloc(
sizeof( struct Cyc_Kind_tok_struct)); _temp2563[ 0]=({ struct Cyc_Kind_tok_struct
_temp2564; _temp2564.tag= Cyc_Kind_tok; _temp2564.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2557, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2564;});
_temp2563;}); break;} case 71: _LL2554: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2566=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2566[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2567; _temp2567.tag= Cyc_TypeQual_tok;
_temp2567.f1=({ struct Cyc_Absyn_Tqual _temp2568; _temp2568.q_const= 1;
_temp2568.q_volatile= 0; _temp2568.q_restrict= 0; _temp2568;}); _temp2567;});
_temp2566;}); break; case 72: _LL2565: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2570=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2570[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2571; _temp2571.tag= Cyc_TypeQual_tok;
_temp2571.f1=({ struct Cyc_Absyn_Tqual _temp2572; _temp2572.q_const= 0;
_temp2572.q_volatile= 1; _temp2572.q_restrict= 0; _temp2572;}); _temp2571;});
_temp2570;}); break; case 73: _LL2569: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2574=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2574[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2575; _temp2575.tag= Cyc_TypeQual_tok;
_temp2575.f1=({ struct Cyc_Absyn_Tqual _temp2576; _temp2576.q_const= 0;
_temp2576.q_volatile= 0; _temp2576.q_restrict= 1; _temp2576;}); _temp2575;});
_temp2574;}); break; case 74: _LL2573: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2578=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2578[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2579; _temp2579.tag=
Cyc_TypeSpecifier_tok; _temp2579.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2580=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2580[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2581; _temp2581.tag= Cyc_Parse_Decl_spec;
_temp2581.f1=({ struct Cyc_Absyn_Decl* _temp2582=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2582->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2583=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2583[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2584; _temp2584.tag= Cyc_Absyn_Enum_d; _temp2584.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2585=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2585->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2585->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2585->fields=({
struct Cyc_Core_Opt* _temp2586=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2586->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2586;}); _temp2585;}); _temp2584;});
_temp2583;})); _temp2582->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2582;});
_temp2581;}); _temp2580;})); _temp2579;}); _temp2578;}); break; case 75: _LL2577:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2588=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2588[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2589; _temp2589.tag= Cyc_TypeSpecifier_tok;
_temp2589.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2590=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2590[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2591; _temp2591.tag= Cyc_Absyn_EnumType;
_temp2591.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2591.f2= 0; _temp2591;}); _temp2590;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2589;}); _temp2588;}); break; case 76: _LL2587: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2593=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2593[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2594; _temp2594.tag= Cyc_Enumfield_tok;
_temp2594.f1=({ struct Cyc_Absyn_Enumfield* _temp2595=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2595->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2595->tag=
0; _temp2595->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2595;});
_temp2594;}); _temp2593;}); break; case 77: _LL2592: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2597=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2597[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2598; _temp2598.tag= Cyc_Enumfield_tok;
_temp2598.f1=({ struct Cyc_Absyn_Enumfield* _temp2599=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2599->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2599->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2599->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2599;});
_temp2598;}); _temp2597;}); break; case 78: _LL2596: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2601=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2601[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2602; _temp2602.tag=
Cyc_EnumfieldList_tok; _temp2602.f1=({ struct Cyc_List_List* _temp2603=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2603->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2603->tl= 0; _temp2603;}); _temp2602;}); _temp2601;}); break; case 79:
_LL2600: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2605=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2605[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2606; _temp2606.tag=
Cyc_EnumfieldList_tok; _temp2606.f1=({ struct Cyc_List_List* _temp2607=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2607->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2607->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2607;}); _temp2606;}); _temp2605;}); break; case 80:
_LL2604: { void* t;{ void* _temp2609= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2611: if( _temp2609 ==( void*) Cyc_Parse_Struct_su){
goto _LL2612;} else{ goto _LL2613;} _LL2613: if( _temp2609 ==( void*) Cyc_Parse_Union_su){
goto _LL2614;} else{ goto _LL2610;} _LL2612: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2615=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2615[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2616; _temp2616.tag= Cyc_Absyn_AnonStructType; _temp2616.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2616;});
_temp2615;}); goto _LL2610; _LL2614: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2617=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2617[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2618; _temp2618.tag= Cyc_Absyn_AnonUnionType; _temp2618.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2618;});
_temp2617;}); goto _LL2610; _LL2610:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2619=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2619[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2620; _temp2620.tag=
Cyc_TypeSpecifier_tok; _temp2620.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2621=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2621[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2622; _temp2622.tag= Cyc_Parse_Type_spec;
_temp2622.f1=( void*) t; _temp2622.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2622;}); _temp2621;})); _temp2620;}); _temp2619;}); break;} case 81:
_LL2608: { struct Cyc_List_List* _temp2624=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2625; _LL2625: { struct Cyc_Absyn_Decl* d;{ void* _temp2626= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2628:
if( _temp2626 ==( void*) Cyc_Parse_Struct_su){ goto _LL2629;} else{ goto _LL2630;}
_LL2630: if( _temp2626 ==( void*) Cyc_Parse_Union_su){ goto _LL2631;} else{ goto
_LL2627;} _LL2629: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2632=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2632->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2632;}), _temp2624,({ struct Cyc_Core_Opt* _temp2633=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2633->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2633;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2627;
_LL2631: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2634=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2634->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2634;}), _temp2624,({ struct Cyc_Core_Opt* _temp2635=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2635->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2635;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2627;
_LL2627:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2636=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2636[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2637; _temp2637.tag=
Cyc_TypeSpecifier_tok; _temp2637.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2638=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2638[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2639; _temp2639.tag= Cyc_Parse_Decl_spec;
_temp2639.f1= d; _temp2639;}); _temp2638;})); _temp2637;}); _temp2636;}); break;}}
case 82: _LL2623: { struct Cyc_List_List* _temp2641=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2642; _LL2642: { struct Cyc_Absyn_Decl* d;{ void* _temp2643= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2645:
if( _temp2643 ==( void*) Cyc_Parse_Struct_su){ goto _LL2646;} else{ goto _LL2647;}
_LL2647: if( _temp2643 ==( void*) Cyc_Parse_Union_su){ goto _LL2648;} else{ goto
_LL2644;} _LL2646: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2649=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2649->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2649;}), _temp2641,({ struct Cyc_Core_Opt* _temp2650=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2650->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2650;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2644;
_LL2648: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2651=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2651->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2651;}), _temp2641,({ struct Cyc_Core_Opt* _temp2652=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2652->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2652;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2644;
_LL2644:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2653=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2653[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2654; _temp2654.tag=
Cyc_TypeSpecifier_tok; _temp2654.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2655=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2655[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2656; _temp2656.tag= Cyc_Parse_Decl_spec;
_temp2656.f1= d; _temp2656;}); _temp2655;})); _temp2654;}); _temp2653;}); break;}}
case 83: _LL2640:{ void* _temp2658= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2660: if( _temp2658 ==( void*) Cyc_Parse_Struct_su){
goto _LL2661;} else{ goto _LL2662;} _LL2662: if( _temp2658 ==( void*) Cyc_Parse_Union_su){
goto _LL2663;} else{ goto _LL2659;} _LL2661: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2664=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2664[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2665; _temp2665.tag=
Cyc_TypeSpecifier_tok; _temp2665.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2666=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2666[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2667; _temp2667.tag= Cyc_Absyn_StructType;
_temp2667.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2667.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2667.f3= 0;
_temp2667;}); _temp2666;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2665;});
_temp2664;}); goto _LL2659; _LL2663: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2668=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2668[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2669; _temp2669.tag=
Cyc_TypeSpecifier_tok; _temp2669.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2670=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2670[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2671; _temp2671.tag= Cyc_Absyn_UnionType;
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
_temp2668;}); goto _LL2659; _LL2659:;} break; case 84: _LL2657:{ void* _temp2673=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2675: if( _temp2673 ==( void*) Cyc_Parse_Struct_su){ goto _LL2676;} else{
goto _LL2677;} _LL2677: if( _temp2673 ==( void*) Cyc_Parse_Union_su){ goto
_LL2678;} else{ goto _LL2674;} _LL2676: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2679=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2679[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2680; _temp2680.tag=
Cyc_TypeSpecifier_tok; _temp2680.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2681=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2681[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2682; _temp2682.tag= Cyc_Absyn_StructType;
_temp2682.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2682.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2682.f3= 0;
_temp2682;}); _temp2681;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2680;});
_temp2679;}); goto _LL2674; _LL2678: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2683=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2683[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2684; _temp2684.tag=
Cyc_TypeSpecifier_tok; _temp2684.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2685=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2685[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2686; _temp2686.tag= Cyc_Absyn_UnionType;
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
_temp2683;}); goto _LL2674; _LL2674:;} break; case 85: _LL2672: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2688=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2688[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2689; _temp2689.tag= Cyc_TypeList_tok; _temp2689.f1= 0; _temp2689;});
_temp2688;}); break; case 86: _LL2687: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2691=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2691[ 0]=({ struct Cyc_TypeList_tok_struct _temp2692; _temp2692.tag= Cyc_TypeList_tok;
_temp2692.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2692;}); _temp2691;}); break; case 87: _LL2690:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2694=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2694[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2695; _temp2695.tag= Cyc_TypeList_tok; _temp2695.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2695;}); _temp2694;}); break;
case 88: _LL2693: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2697=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2697[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2698; _temp2698.tag=
Cyc_StructOrUnion_tok; _temp2698.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2698;}); _temp2697;}); break; case 89: _LL2696: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2700=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2700[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2701; _temp2701.tag=
Cyc_StructOrUnion_tok; _temp2701.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2701;}); _temp2700;}); break; case 90: _LL2699: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2703=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2703[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2704; _temp2704.tag= Cyc_StructFieldDeclList_tok; _temp2704.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2704;});
_temp2703;}); break; case 91: _LL2702: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2706=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2706[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2707; _temp2707.tag= Cyc_StructFieldDeclListList_tok; _temp2707.f1=({
struct Cyc_List_List* _temp2708=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2708->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2708->tl=
0; _temp2708;}); _temp2707;}); _temp2706;}); break; case 92: _LL2705: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2710=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2710[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2711; _temp2711.tag= Cyc_StructFieldDeclListList_tok;
_temp2711.f1=({ struct Cyc_List_List* _temp2712=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2712->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2712->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2712;}); _temp2711;}); _temp2710;}); break; case 93:
_LL2709: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2714=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2714[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2715; _temp2715.tag=
Cyc_InitDeclList_tok; _temp2715.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2715;}); _temp2714;}); break; case 94:
_LL2713: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2717=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2717[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2718; _temp2718.tag=
Cyc_InitDeclList_tok; _temp2718.f1=({ struct Cyc_List_List* _temp2719=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2719->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2719->tl= 0; _temp2719;}); _temp2718;}); _temp2717;}); break; case 95:
_LL2716: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2721=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2721[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2722; _temp2722.tag=
Cyc_InitDeclList_tok; _temp2722.f1=({ struct Cyc_List_List* _temp2723=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2723->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2723->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2723;}); _temp2722;}); _temp2721;}); break; case 96:
_LL2720: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2725=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2725[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2726; _temp2726.tag= Cyc_InitDecl_tok; _temp2726.f1=({ struct _tuple15*
_temp2727=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2727->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2727->f2= 0; _temp2727;}); _temp2726;}); _temp2725;}); break; case 97:
_LL2724: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2729=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2729[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2730; _temp2730.tag= Cyc_InitDecl_tok; _temp2730.f1=({ struct _tuple15*
_temp2731=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2731->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2731->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2731;}); _temp2730;}); _temp2729;}); break;
case 98: _LL2728: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2733=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2734; _LL2734: { void* _temp2735=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2736; _LL2736: { struct Cyc_List_List* _temp2739; struct Cyc_List_List*
_temp2741; struct _tuple0 _temp2737=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2742: _temp2741= _temp2737.f1; goto
_LL2740; _LL2740: _temp2739= _temp2737.f2; goto _LL2738; _LL2738: { struct Cyc_List_List*
_temp2743=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2735, _temp2741, _temp2733),
_temp2739); goto _LL2744; _LL2744: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2745=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2745[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2746; _temp2746.tag= Cyc_StructFieldDeclList_tok; _temp2746.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2743); _temp2746;}); _temp2745;}); break;}}}} case 99: _LL2732: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2748=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2748[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2749; _temp2749.tag= Cyc_QualSpecList_tok;
_temp2749.f1=({ struct _tuple18* _temp2750=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2750->f1= Cyc_Absyn_empty_tqual(); _temp2750->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2751=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2751->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2751->tl=
0; _temp2751;}); _temp2750->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2750;}); _temp2749;}); _temp2748;});
break; case 100: _LL2747: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2753=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2753[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2754; _temp2754.tag=
Cyc_QualSpecList_tok; _temp2754.f1=({ struct _tuple18* _temp2755=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2755->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2755->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2756=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2756->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2756->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2756;}); _temp2755->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2755;}); _temp2754;}); _temp2753;});
break; case 101: _LL2752: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2758=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2758[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2759; _temp2759.tag=
Cyc_QualSpecList_tok; _temp2759.f1=({ struct _tuple18* _temp2760=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2760->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2760->f2=
0; _temp2760->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2760;}); _temp2759;}); _temp2758;}); break; case 102:
_LL2757: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2762=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2762[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2763; _temp2763.tag=
Cyc_QualSpecList_tok; _temp2763.f1=({ struct _tuple18* _temp2764=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2764->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2764->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2764->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2764;});
_temp2763;}); _temp2762;}); break; case 103: _LL2761: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2766=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2766[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2767; _temp2767.tag= Cyc_DeclaratorExpoptList_tok;
_temp2767.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2767;}); _temp2766;}); break; case 104: _LL2765: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2769=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2769[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2770; _temp2770.tag= Cyc_DeclaratorExpoptList_tok;
_temp2770.f1=({ struct Cyc_List_List* _temp2771=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2771->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2771->tl=
0; _temp2771;}); _temp2770;}); _temp2769;}); break; case 105: _LL2768: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2773=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2773[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2774; _temp2774.tag= Cyc_DeclaratorExpoptList_tok;
_temp2774.f1=({ struct Cyc_List_List* _temp2775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2775->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2775->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2775;}); _temp2774;}); _temp2773;}); break; case 106:
_LL2772: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2777=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2777[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2778; _temp2778.tag=
Cyc_DeclaratorExpopt_tok; _temp2778.f1=({ struct _tuple13* _temp2779=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2779->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2779->f2=
0; _temp2779;}); _temp2778;}); _temp2777;}); break; case 107: _LL2776: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2781=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2781[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2782; _temp2782.tag= Cyc_DeclaratorExpopt_tok;
_temp2782.f1=({ struct _tuple13* _temp2783=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2783->f1=({ struct Cyc_Parse_Declarator*
_temp2785=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2785->id=({ struct _tuple1* _temp2786=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2786->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2790=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2790[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2791; _temp2791.tag= Cyc_Absyn_Rel_n;
_temp2791.f1= 0; _temp2791;}); _temp2790;}); _temp2786->f2=({ struct
_tagged_string* _temp2787=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2787[ 0]=({ char* _temp2788=""; struct _tagged_string
_temp2789; _temp2789.curr= _temp2788; _temp2789.base= _temp2788; _temp2789.last_plus_one=
_temp2788 + 1; _temp2789;}); _temp2787;}); _temp2786;}); _temp2785->tms= 0;
_temp2785;}); _temp2783->f2=({ struct Cyc_Core_Opt* _temp2784=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2784->v=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2784;});
_temp2783;}); _temp2782;}); _temp2781;}); break; case 108: _LL2780: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp2793=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2793[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2794; _temp2794.tag= Cyc_DeclaratorExpopt_tok;
_temp2794.f1=({ struct _tuple13* _temp2795=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2795->f1= Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2795->f2=({ struct Cyc_Core_Opt*
_temp2796=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2796->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2796;}); _temp2795;}); _temp2794;}); _temp2793;}); break;
case 109: _LL2792: { struct Cyc_List_List* _temp2798=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2799; _LL2799: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2800=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2800[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2801; _temp2801.tag=
Cyc_TypeSpecifier_tok; _temp2801.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2802=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2802[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2803; _temp2803.tag= Cyc_Parse_Decl_spec;
_temp2803.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2798,({
struct Cyc_Core_Opt* _temp2804=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2804->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2804;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2803;}); _temp2802;})); _temp2801;}); _temp2800;}); break;} case 110:
_LL2797: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2806=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2806[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2807; _temp2807.tag=
Cyc_TypeSpecifier_tok; _temp2807.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2808=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2808[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2809; _temp2809.tag= Cyc_Absyn_TunionType;
_temp2809.f1=({ struct Cyc_Absyn_TunionInfo _temp2810; _temp2810.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2811=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2811[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2812; _temp2812.tag= Cyc_Absyn_UnknownTunion;
_temp2812.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2813; _temp2813.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2813.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2813;}); _temp2812;}); _temp2811;})); _temp2810.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2810.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2810;}); _temp2809;});
_temp2808;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2807;});
_temp2806;}); break; case 111: _LL2805: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2815=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2815[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2816; _temp2816.tag=
Cyc_TypeSpecifier_tok; _temp2816.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2817=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2817[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2818; _temp2818.tag= Cyc_Absyn_TunionType;
_temp2818.f1=({ struct Cyc_Absyn_TunionInfo _temp2819; _temp2819.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2820=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2820[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2821; _temp2821.tag= Cyc_Absyn_UnknownTunion;
_temp2821.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2822; _temp2822.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2822.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2822;}); _temp2821;}); _temp2820;})); _temp2819.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2819.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2819;}); _temp2818;}); _temp2817;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2816;}); _temp2815;}); break; case 112: _LL2814: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2824=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2824[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2825; _temp2825.tag= Cyc_TypeSpecifier_tok;
_temp2825.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2826=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2826[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2827; _temp2827.tag= Cyc_Absyn_TunionFieldType; _temp2827.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2828; _temp2828.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2829=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2829[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2830; _temp2830.tag= Cyc_Absyn_UnknownTunionfield; _temp2830.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2831; _temp2831.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2831.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2831.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2831;}); _temp2830;}); _temp2829;})); _temp2828.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2828;}); _temp2827;}); _temp2826;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2825;}); _temp2824;}); break; case 113: _LL2823: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2833=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2833[ 0]=({ struct Cyc_Bool_tok_struct
_temp2834; _temp2834.tag= Cyc_Bool_tok; _temp2834.f1= 0; _temp2834;}); _temp2833;});
break; case 114: _LL2832: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2836=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2836[ 0]=({ struct Cyc_Bool_tok_struct _temp2837; _temp2837.tag= Cyc_Bool_tok;
_temp2837.f1= 1; _temp2837;}); _temp2836;}); break; case 115: _LL2835: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2839=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2839[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2840; _temp2840.tag= Cyc_TunionFieldList_tok;
_temp2840.f1=({ struct Cyc_List_List* _temp2841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2841->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2841->tl=
0; _temp2841;}); _temp2840;}); _temp2839;}); break; case 116: _LL2838: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2843=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2843[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2844; _temp2844.tag= Cyc_TunionFieldList_tok;
_temp2844.f1=({ struct Cyc_List_List* _temp2845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2845->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2845->tl=
0; _temp2845;}); _temp2844;}); _temp2843;}); break; case 117: _LL2842: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2847=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2847[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2848; _temp2848.tag= Cyc_TunionFieldList_tok;
_temp2848.f1=({ struct Cyc_List_List* _temp2849=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2849->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2849->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2849;}); _temp2848;}); _temp2847;}); break; case 118: _LL2846: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2851=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2851[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2852; _temp2852.tag= Cyc_TunionFieldList_tok;
_temp2852.f1=({ struct Cyc_List_List* _temp2853=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2853->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2853->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2853;}); _temp2852;}); _temp2851;}); break; case 119: _LL2850: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2855=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2855[ 0]=({ struct Cyc_Scope_tok_struct
_temp2856; _temp2856.tag= Cyc_Scope_tok; _temp2856.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2856;}); _temp2855;}); break; case 120: _LL2854: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2858=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2858[ 0]=({ struct Cyc_Scope_tok_struct
_temp2859; _temp2859.tag= Cyc_Scope_tok; _temp2859.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2859;}); _temp2858;}); break; case 121: _LL2857: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2861=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2861[ 0]=({ struct Cyc_Scope_tok_struct
_temp2862; _temp2862.tag= Cyc_Scope_tok; _temp2862.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2862;}); _temp2861;}); break; case 122: _LL2860: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2864=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2864[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2865; _temp2865.tag= Cyc_TunionField_tok;
_temp2865.f1=({ struct Cyc_Absyn_Tunionfield* _temp2866=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2866->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2866->tvs=
0; _temp2866->typs= 0; _temp2866->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2866->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2866;}); _temp2865;}); _temp2864;}); break; case 123:
_LL2863: { struct Cyc_List_List* _temp2868=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2869; _LL2869: { struct Cyc_List_List* _temp2870=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2871; _LL2871: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2872=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2872[ 0]=({ struct Cyc_TunionField_tok_struct _temp2873; _temp2873.tag= Cyc_TunionField_tok;
_temp2873.f1=({ struct Cyc_Absyn_Tunionfield* _temp2874=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2874->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2874->tvs=
_temp2870; _temp2874->typs= _temp2868; _temp2874->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2874->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2874;}); _temp2873;}); _temp2872;}); break;}} case 124:
_LL2867: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 125:
_LL2875: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2877=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2877[ 0]=({ struct Cyc_Declarator_tok_struct _temp2878; _temp2878.tag= Cyc_Declarator_tok;
_temp2878.f1=({ struct Cyc_Parse_Declarator* _temp2879=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2879->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2879->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2879;}); _temp2878;}); _temp2877;}); break; case 126: _LL2876: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2881=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2881[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2882; _temp2882.tag= Cyc_Declarator_tok;
_temp2882.f1=({ struct Cyc_Parse_Declarator* _temp2883=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2883->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2883->tms=
0; _temp2883;}); _temp2882;}); _temp2881;}); break; case 127: _LL2880: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
128: _LL2884: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2886=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2886[ 0]=({ struct Cyc_Declarator_tok_struct _temp2887; _temp2887.tag= Cyc_Declarator_tok;
_temp2887.f1=({ struct Cyc_Parse_Declarator* _temp2888=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2888->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2888->tms=({ struct Cyc_List_List* _temp2889=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2889->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2889->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2889;}); _temp2888;}); _temp2887;}); _temp2886;});
break; case 129: _LL2885: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2891=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2891[ 0]=({ struct Cyc_Declarator_tok_struct _temp2892; _temp2892.tag= Cyc_Declarator_tok;
_temp2892.f1=({ struct Cyc_Parse_Declarator* _temp2893=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2893->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2893->tms=({ struct Cyc_List_List* _temp2894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2894->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2895=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2895[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2896; _temp2896.tag= Cyc_Absyn_ConstArray_mod;
_temp2896.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2896;}); _temp2895;})); _temp2894->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2894;}); _temp2893;});
_temp2892;}); _temp2891;}); break; case 130: _LL2890: { struct _tuple17
_temp2900; struct Cyc_Core_Opt* _temp2901; int _temp2903; struct Cyc_List_List*
_temp2905; struct _tuple17* _temp2898= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2900=* _temp2898; _LL2906:
_temp2905= _temp2900.f1; goto _LL2904; _LL2904: _temp2903= _temp2900.f2; goto
_LL2902; _LL2902: _temp2901= _temp2900.f3; goto _LL2899; _LL2899: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2907=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2907[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2908; _temp2908.tag= Cyc_Declarator_tok;
_temp2908.f1=({ struct Cyc_Parse_Declarator* _temp2909=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2909->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2909->tms=({ struct Cyc_List_List* _temp2910=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2910->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2911=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2911[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2912; _temp2912.tag= Cyc_Absyn_Function_mod;
_temp2912.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2913=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2913[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2914; _temp2914.tag= Cyc_Absyn_WithTypes;
_temp2914.f1= _temp2905; _temp2914.f2= _temp2903; _temp2914.f3= _temp2901;
_temp2914;}); _temp2913;})); _temp2912;}); _temp2911;})); _temp2910->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2910;}); _temp2909;}); _temp2908;}); _temp2907;}); break;} case 131:
_LL2897: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2916=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2916[ 0]=({ struct Cyc_Declarator_tok_struct _temp2917; _temp2917.tag= Cyc_Declarator_tok;
_temp2917.f1=({ struct Cyc_Parse_Declarator* _temp2918=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2918->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2918->tms=({ struct Cyc_List_List* _temp2919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2919->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2920=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2920[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2921; _temp2921.tag= Cyc_Absyn_Function_mod;
_temp2921.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2922=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2922[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2923; _temp2923.tag= Cyc_Absyn_WithTypes;
_temp2923.f1= 0; _temp2923.f2= 0; _temp2923.f3= 0; _temp2923;}); _temp2922;}));
_temp2921;}); _temp2920;})); _temp2919->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)))->tms; _temp2919;}); _temp2918;});
_temp2917;}); _temp2916;}); break; case 132: _LL2915: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2925=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2925[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2926; _temp2926.tag= Cyc_Declarator_tok;
_temp2926.f1=({ struct Cyc_Parse_Declarator* _temp2927=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2927->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2927->tms=({ struct Cyc_List_List* _temp2928=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2928->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2929=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2929[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2930; _temp2930.tag= Cyc_Absyn_Function_mod;
_temp2930.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2931=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2931[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2932; _temp2932.tag= Cyc_Absyn_WithTypes;
_temp2932.f1= 0; _temp2932.f2= 0; _temp2932.f3=({ struct Cyc_Core_Opt* _temp2933=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2933->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2934=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2934[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2935; _temp2935.tag= Cyc_Absyn_JoinEff; _temp2935.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2935;}); _temp2934;})); _temp2933;}); _temp2932;}); _temp2931;}));
_temp2930;}); _temp2929;})); _temp2928->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)))->tms; _temp2928;}); _temp2927;});
_temp2926;}); _temp2925;}); break; case 133: _LL2924: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2937=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2937[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2938; _temp2938.tag= Cyc_Declarator_tok;
_temp2938.f1=({ struct Cyc_Parse_Declarator* _temp2939=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2939->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2939->tms=({ struct Cyc_List_List* _temp2940=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2940->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2941=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2941[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2942; _temp2942.tag= Cyc_Absyn_Function_mod;
_temp2942.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2943=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2943[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2944; _temp2944.tag= Cyc_Absyn_NoTypes;
_temp2944.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2944.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2944;});
_temp2943;})); _temp2942;}); _temp2941;})); _temp2940->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2940;}); _temp2939;}); _temp2938;}); _temp2937;}); break; case 134: _LL2936: {
struct Cyc_List_List* _temp2946=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2947; _LL2947: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2948=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2948[ 0]=({ struct Cyc_Declarator_tok_struct _temp2949; _temp2949.tag= Cyc_Declarator_tok;
_temp2949.f1=({ struct Cyc_Parse_Declarator* _temp2950=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2950->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2950->tms=({ struct Cyc_List_List* _temp2951=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2951->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2952=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2952[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2953; _temp2953.tag= Cyc_Absyn_TypeParams_mod;
_temp2953.f1= _temp2946; _temp2953.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2953.f3= 0; _temp2953;}); _temp2952;})); _temp2951->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2951;}); _temp2950;}); _temp2949;}); _temp2948;}); break;} case 135:
_LL2945:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2955=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2956; _LL2956: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2957=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2957[ 0]=({ struct Cyc_Declarator_tok_struct _temp2958; _temp2958.tag= Cyc_Declarator_tok;
_temp2958.f1=({ struct Cyc_Parse_Declarator* _temp2959=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2959->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2959->tms=({ struct Cyc_List_List* _temp2960=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2960->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2961=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2961[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2962; _temp2962.tag= Cyc_Absyn_TypeParams_mod;
_temp2962.f1= _temp2955; _temp2962.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2962.f3= 0; _temp2962;}); _temp2961;})); _temp2960->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2960;}); _temp2959;}); _temp2958;}); _temp2957;}); break;} case 136:
_LL2954: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2964=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2964[ 0]=({ struct Cyc_Declarator_tok_struct _temp2965; _temp2965.tag= Cyc_Declarator_tok;
_temp2965.f1=({ struct Cyc_Parse_Declarator* _temp2966=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2966->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp2966->tms=({ struct Cyc_List_List* _temp2967=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2967->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2968=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2968[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2969; _temp2969.tag= Cyc_Absyn_Attributes_mod;
_temp2969.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2969.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2969;});
_temp2968;})); _temp2967->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp2967;}); _temp2966;}); _temp2965;});
_temp2964;}); break; case 137: _LL2963: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2971=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2971[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2972; _temp2972.tag= Cyc_TypeModifierList_tok; _temp2972.f1=({ struct Cyc_List_List*
_temp2973=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2973->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2974=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2974[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2975; _temp2975.tag=
Cyc_Absyn_Pointer_mod; _temp2975.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2975.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2975.f3=
Cyc_Absyn_empty_tqual(); _temp2975;}); _temp2974;})); _temp2973->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp2973;}); _temp2972;}); _temp2971;});
break; case 138: _LL2970: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2977=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2977[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2978; _temp2978.tag= Cyc_TypeModifierList_tok; _temp2978.f1=({ struct Cyc_List_List*
_temp2979=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2979->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2980=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2980[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2981; _temp2981.tag=
Cyc_Absyn_Pointer_mod; _temp2981.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2981.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2981.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2981;}); _temp2980;})); _temp2979->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp2979;}); _temp2978;}); _temp2977;}); break; case 139: _LL2976: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2983=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2983[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2984; _temp2984.tag= Cyc_TypeModifierList_tok;
_temp2984.f1=({ struct Cyc_List_List* _temp2985=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2985->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2986=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2986[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2987; _temp2987.tag= Cyc_Absyn_Pointer_mod;
_temp2987.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp2987.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2987.f3=
Cyc_Absyn_empty_tqual(); _temp2987;}); _temp2986;})); _temp2985->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp2985;}); _temp2984;}); _temp2983;});
break; case 140: _LL2982: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2989=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2989[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2990; _temp2990.tag= Cyc_TypeModifierList_tok; _temp2990.f1=({ struct Cyc_List_List*
_temp2991=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2991->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2992=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2992[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2993; _temp2993.tag=
Cyc_Absyn_Pointer_mod; _temp2993.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2993.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2993.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2993;}); _temp2992;})); _temp2991->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp2991;}); _temp2990;}); _temp2989;}); break; case 141: _LL2988: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp2995=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2995[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2996; _temp2996.tag= Cyc_Pointer_Sort_tok;
_temp2996.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2997=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2997[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2998; _temp2998.tag=
Cyc_Absyn_Nullable_ps; _temp2998.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2998;}); _temp2997;})); _temp2996;}); _temp2995;}); break; case 142:
_LL2994: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3000=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3000[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3001; _temp3001.tag=
Cyc_Pointer_Sort_tok; _temp3001.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3002=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3002[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3003; _temp3003.tag= Cyc_Absyn_NonNullable_ps; _temp3003.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3003;}); _temp3002;})); _temp3001;}); _temp3000;});
break; case 143: _LL2999: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3005=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3005[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3006; _temp3006.tag=
Cyc_Pointer_Sort_tok; _temp3006.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3007=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3007[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3008; _temp3008.tag=
Cyc_Absyn_Nullable_ps; _temp3008.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3008;}); _temp3007;})); _temp3006;});
_temp3005;}); break; case 144: _LL3004: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3010=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3010[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3011; _temp3011.tag=
Cyc_Pointer_Sort_tok; _temp3011.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3012=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3012[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3013; _temp3013.tag= Cyc_Absyn_NonNullable_ps; _temp3013.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3013;});
_temp3012;})); _temp3011;}); _temp3010;}); break; case 145: _LL3009: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3015=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3015[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3016; _temp3016.tag= Cyc_Pointer_Sort_tok;
_temp3016.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3016;});
_temp3015;}); break; case 146: _LL3014: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3018=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3018[ 0]=({ struct Cyc_Rgn_tok_struct _temp3019; _temp3019.tag= Cyc_Rgn_tok;
_temp3019.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3019;}); _temp3018;});
break; case 147: _LL3017: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 148: _LL3020: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3022=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3022[ 0]=({ struct Cyc_Rgn_tok_struct _temp3023; _temp3023.tag= Cyc_Rgn_tok;
_temp3023.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3023;}); _temp3022;});
break; case 149: _LL3021: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({
char* _temp3025="expecting region kind\n"; struct _tagged_string _temp3026;
_temp3026.curr= _temp3025; _temp3026.base= _temp3025; _temp3026.last_plus_one=
_temp3025 + 23; _temp3026;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Rgn_tok_struct* _temp3027=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp3027[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3028; _temp3028.tag= Cyc_Rgn_tok; _temp3028.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3028;}); _temp3027;}); break; case 150: _LL3024: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3030=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3030[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3031; _temp3031.tag= Cyc_Rgn_tok; _temp3031.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp3031;}); _temp3030;}); break; case 151: _LL3029:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 152:
_LL3032: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp3034=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3034[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3035; _temp3035.tag= Cyc_TypeQual_tok; _temp3035.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3035;}); _temp3034;}); break; case 153: _LL3033: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3037=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3037[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3038; _temp3038.tag= Cyc_ParamDeclListBool_tok;
_temp3038.f1=({ struct _tuple17* _temp3039=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3039->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3039->f2= 0; _temp3039->f3= 0; _temp3039;});
_temp3038;}); _temp3037;}); break; case 154: _LL3036: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3041=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3041[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3042; _temp3042.tag= Cyc_ParamDeclListBool_tok;
_temp3042.f1=({ struct _tuple17* _temp3043=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3043->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3043->f2= 1; _temp3043->f3= 0;
_temp3043;}); _temp3042;}); _temp3041;}); break; case 155: _LL3040: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp3045=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3045[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3046; _temp3046.tag= Cyc_ParamDeclListBool_tok;
_temp3046.f1=({ struct _tuple17* _temp3047=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3047->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3047->f2= 0; _temp3047->f3=( struct
Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp3048=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3048->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3049=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3049[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3050; _temp3050.tag= Cyc_Absyn_JoinEff; _temp3050.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3050;}); _temp3049;})); _temp3048;}); _temp3047;}); _temp3046;}); _temp3045;});
break; case 156: _LL3044: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 157: _LL3051: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3053=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3053[ 0]=({ struct Cyc_TypeList_tok_struct _temp3054; _temp3054.tag= Cyc_TypeList_tok;
_temp3054.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3054;}); _temp3053;}); break; case 158:
_LL3052: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3056=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3056[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3057; _temp3057.tag= Cyc_TypeList_tok; _temp3057.f1= 0; _temp3057;});
_temp3056;}); break; case 159: _LL3055: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 160: _LL3058: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3060=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3060[ 0]=({ struct Cyc_TypeList_tok_struct _temp3061; _temp3061.tag= Cyc_TypeList_tok;
_temp3061.f1=({ struct Cyc_List_List* _temp3062=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3062->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3062->tl= 0; _temp3062;}); _temp3061;}); _temp3060;}); break; case 161:
_LL3059: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(({ char* _temp3064="expecing effect kind (E)";
struct _tagged_string _temp3065; _temp3065.curr= _temp3064; _temp3065.base=
_temp3064; _temp3065.last_plus_one= _temp3064 + 25; _temp3065;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3066=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3066[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3067; _temp3067.tag= Cyc_TypeList_tok; _temp3067.f1=({ struct Cyc_List_List*
_temp3068=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3068->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3068->tl= 0; _temp3068;});
_temp3067;}); _temp3066;}); break; case 162: _LL3063: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3070=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3070[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3071; _temp3071.tag= Cyc_TypeList_tok; _temp3071.f1=({ struct Cyc_List_List*
_temp3072=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3072->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3073=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3073[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3074; _temp3074.tag= Cyc_Absyn_AccessEff;
_temp3074.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3074;}); _temp3073;}));
_temp3072->tl= 0; _temp3072;}); _temp3071;}); _temp3070;}); break; case 163:
_LL3069: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3076=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3076[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3077; _temp3077.tag= Cyc_TypeList_tok; _temp3077.f1=({ struct Cyc_List_List*
_temp3078=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3078->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3079=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3079[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3080; _temp3080.tag= Cyc_Absyn_AccessEff;
_temp3080.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3080;}); _temp3079;}));
_temp3078->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3078;}); _temp3077;}); _temp3076;}); break; case 164:
_LL3075: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ char* _temp3082="expecting region kind (R)";
struct _tagged_string _temp3083; _temp3083.curr= _temp3082; _temp3083.base=
_temp3082; _temp3083.last_plus_one= _temp3082 + 26; _temp3083;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3084=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3084[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3085; _temp3085.tag= Cyc_TypeList_tok; _temp3085.f1=({ struct Cyc_List_List*
_temp3086=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3086->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3087=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3087[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3088; _temp3088.tag= Cyc_Absyn_AccessEff;
_temp3088.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3088;}); _temp3087;}));
_temp3086->tl= 0; _temp3086;}); _temp3085;}); _temp3084;}); break; case 165:
_LL3081: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ char* _temp3090="expecting region kind (R)";
struct _tagged_string _temp3091; _temp3091.curr= _temp3090; _temp3091.base=
_temp3090; _temp3091.last_plus_one= _temp3090 + 26; _temp3091;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3092=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3092[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3093; _temp3093.tag= Cyc_TypeList_tok; _temp3093.f1=({ struct Cyc_List_List*
_temp3094=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3094->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3095=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3095[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3096; _temp3096.tag= Cyc_Absyn_AccessEff;
_temp3096.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3096;}); _temp3095;}));
_temp3094->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3094;}); _temp3093;}); _temp3092;}); break; case 166:
_LL3089: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3098=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3098[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3099; _temp3099.tag=
Cyc_ParamDeclList_tok; _temp3099.f1=({ struct Cyc_List_List* _temp3100=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3100->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3100->tl= 0; _temp3100;}); _temp3099;}); _temp3098;}); break; case 167:
_LL3097: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3102=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3102[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3103; _temp3103.tag=
Cyc_ParamDeclList_tok; _temp3103.f1=({ struct Cyc_List_List* _temp3104=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3104->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3104->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3104;}); _temp3103;}); _temp3102;}); break; case 168:
_LL3101: { void* _temp3106= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3107; _LL3107: { struct Cyc_List_List* _temp3108=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3109; _LL3109: { struct Cyc_Absyn_Tqual _temp3110=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3111; _LL3111: { struct Cyc_List_List* _temp3112=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3113; _LL3113: { struct _tuple6 _temp3114= Cyc_Parse_apply_tms( _temp3110,
_temp3106, _temp3108, _temp3112); goto _LL3115; _LL3115: if( _temp3114.f3 != 0){
Cyc_Parse_err(({ char* _temp3116="parameter with bad type params"; struct
_tagged_string _temp3117; _temp3117.curr= _temp3116; _temp3117.base= _temp3116;
_temp3117.last_plus_one= _temp3116 + 31; _temp3117;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3118=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3119; _LL3119:{ void* _temp3120=(*
_temp3118).f1; struct Cyc_List_List* _temp3130; struct Cyc_List_List* _temp3132;
_LL3122: if( _temp3120 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3123;} else{ goto
_LL3124;} _LL3124: if(( unsigned int) _temp3120 > 1u?*(( int*) _temp3120) == Cyc_Absyn_Rel_n:
0){ _LL3131: _temp3130=(( struct Cyc_Absyn_Rel_n_struct*) _temp3120)->f1; if(
_temp3130 == 0){ goto _LL3125;} else{ goto _LL3126;}} else{ goto _LL3126;}
_LL3126: if(( unsigned int) _temp3120 > 1u?*(( int*) _temp3120) == Cyc_Absyn_Abs_n:
0){ _LL3133: _temp3132=(( struct Cyc_Absyn_Abs_n_struct*) _temp3120)->f1; if(
_temp3132 == 0){ goto _LL3127;} else{ goto _LL3128;}} else{ goto _LL3128;}
_LL3128: goto _LL3129; _LL3123: goto _LL3121; _LL3125: goto _LL3121; _LL3127:
goto _LL3121; _LL3129: Cyc_Parse_err(({ char* _temp3134="parameter cannot be qualified with a module name";
struct _tagged_string _temp3135; _temp3135.curr= _temp3134; _temp3135.base=
_temp3134; _temp3135.last_plus_one= _temp3134 + 49; _temp3135;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3121; _LL3121:;}{ struct Cyc_Core_Opt* _temp3137=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3136=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3136->v=( void*)(* _temp3118).f2; _temp3136;}); goto
_LL3138; _LL3138: if( _temp3114.f4 != 0){ Cyc_Parse_warn(({ char* _temp3139="extra attributes on parameter, ignoring";
struct _tagged_string _temp3140; _temp3140.curr= _temp3139; _temp3140.base=
_temp3139; _temp3140.last_plus_one= _temp3139 + 40; _temp3140;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3114.f3 != 0){ Cyc_Parse_warn(({ char* _temp3141="extra type variables on parameter, ignoring";
struct _tagged_string _temp3142; _temp3142.curr= _temp3141; _temp3142.base=
_temp3141; _temp3142.last_plus_one= _temp3141 + 44; _temp3142;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3143=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3143[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3144; _temp3144.tag= Cyc_ParamDecl_tok; _temp3144.f1=({
struct _tuple2* _temp3145=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3145->f1= _temp3137; _temp3145->f2= _temp3114.f1; _temp3145->f3= _temp3114.f2;
_temp3145;}); _temp3144;}); _temp3143;}); break;}}}}}}} case 169: _LL3105: {
void* _temp3147= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3148; _LL3148: { struct Cyc_List_List* _temp3149=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3150; _LL3150: { struct Cyc_Absyn_Tqual _temp3151=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3152; _LL3152: if( _temp3149 != 0){ Cyc_Parse_warn(({ char* _temp3153="bad attributes on parameter, ignoring";
struct _tagged_string _temp3154; _temp3154.curr= _temp3153; _temp3154.base=
_temp3153; _temp3154.last_plus_one= _temp3153 + 38; _temp3154;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3155=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3155[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3156; _temp3156.tag= Cyc_ParamDecl_tok; _temp3156.f1=({
struct _tuple2* _temp3157=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3157->f1= 0; _temp3157->f2= _temp3151; _temp3157->f3= _temp3147; _temp3157;});
_temp3156;}); _temp3155;}); break;}}} case 170: _LL3146: { void* _temp3159= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3160; _LL3160: { struct Cyc_List_List*
_temp3161=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3162; _LL3162: { struct Cyc_Absyn_Tqual
_temp3163=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3164; _LL3164: { struct Cyc_List_List* _temp3165=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3166; _LL3166: { struct _tuple6 _temp3167= Cyc_Parse_apply_tms(
_temp3163, _temp3159, _temp3161, _temp3165); goto _LL3168; _LL3168: if(
_temp3167.f3 != 0){ Cyc_Parse_warn(({ char* _temp3169="bad type parameters on formal argument, ignoring";
struct _tagged_string _temp3170; _temp3170.curr= _temp3169; _temp3170.base=
_temp3169; _temp3170.last_plus_one= _temp3169 + 49; _temp3170;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3167.f4 != 0){ Cyc_Parse_warn(({ char* _temp3171="bad attributes on parameter, ignoring";
struct _tagged_string _temp3172; _temp3172.curr= _temp3171; _temp3172.base=
_temp3171; _temp3172.last_plus_one= _temp3171 + 38; _temp3172;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3173=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3173[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3174; _temp3174.tag= Cyc_ParamDecl_tok; _temp3174.f1=({
struct _tuple2* _temp3175=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3175->f1= 0; _temp3175->f2= _temp3167.f1; _temp3175->f3= _temp3167.f2;
_temp3175;}); _temp3174;}); _temp3173;}); break;}}}}} case 171: _LL3158: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3177=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3177[ 0]=({ struct Cyc_IdList_tok_struct
_temp3178; _temp3178.tag= Cyc_IdList_tok; _temp3178.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3178;}); _temp3177;}); break; case
172: _LL3176: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3180=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3180[ 0]=({ struct Cyc_IdList_tok_struct _temp3181; _temp3181.tag= Cyc_IdList_tok;
_temp3181.f1=({ struct Cyc_List_List* _temp3182=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3182->hd=( void*)({ struct
_tagged_string* _temp3183=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3183[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3183;}); _temp3182->tl= 0; _temp3182;});
_temp3181;}); _temp3180;}); break; case 173: _LL3179: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3185=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3185[ 0]=({ struct Cyc_IdList_tok_struct
_temp3186; _temp3186.tag= Cyc_IdList_tok; _temp3186.f1=({ struct Cyc_List_List*
_temp3187=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3187->hd=( void*)({ struct _tagged_string* _temp3188=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3188[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3188;}); _temp3187->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3187;}); _temp3186;}); _temp3185;});
break; case 174: _LL3184: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 175: _LL3189: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 176: _LL3190: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3192=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3192[ 0]=({ struct Cyc_Exp_tok_struct _temp3193; _temp3193.tag= Cyc_Exp_tok;
_temp3193.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3194=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3194[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3195; _temp3195.tag= Cyc_Absyn_UnresolvedMem_e; _temp3195.f1= 0; _temp3195.f2=
0; _temp3195;}); _temp3194;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3193;});
_temp3192;}); break; case 177: _LL3191: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3197=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3197[ 0]=({ struct Cyc_Exp_tok_struct _temp3198; _temp3198.tag= Cyc_Exp_tok;
_temp3198.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3199=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3199[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3200; _temp3200.tag= Cyc_Absyn_UnresolvedMem_e; _temp3200.f1= 0; _temp3200.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3200;});
_temp3199;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3198;});
_temp3197;}); break; case 178: _LL3196: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3202=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3202[ 0]=({ struct Cyc_Exp_tok_struct _temp3203; _temp3203.tag= Cyc_Exp_tok;
_temp3203.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3204=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3204[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3205; _temp3205.tag= Cyc_Absyn_UnresolvedMem_e; _temp3205.f1= 0; _temp3205.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3205;});
_temp3204;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3203;});
_temp3202;}); break; case 179: _LL3201: { struct Cyc_Absyn_Vardecl* _temp3209=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3207=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3207->f1=( void*) Cyc_Absyn_Loc_n; _temp3207->f2=({
struct _tagged_string* _temp3208=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3208[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3208;}); _temp3207;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3210; _LL3210: _temp3209->tq=({ struct Cyc_Absyn_Tqual _temp3211;
_temp3211.q_const= 1; _temp3211.q_volatile= 0; _temp3211.q_restrict= 1;
_temp3211;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3212=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3212[ 0]=({ struct Cyc_Exp_tok_struct
_temp3213; _temp3213.tag= Cyc_Exp_tok; _temp3213.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3214=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3214[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3215; _temp3215.tag= Cyc_Absyn_Comprehension_e;
_temp3215.f1= _temp3209; _temp3215.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3215.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3215;}); _temp3214;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3213;}); _temp3212;}); break;} case 180: _LL3206: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3217=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3217[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3218; _temp3218.tag= Cyc_InitializerList_tok;
_temp3218.f1=({ struct Cyc_List_List* _temp3219=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3219->hd=( void*)({ struct
_tuple19* _temp3220=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3220->f1= 0; _temp3220->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3220;}); _temp3219->tl= 0; _temp3219;});
_temp3218;}); _temp3217;}); break; case 181: _LL3216: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3222=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3222[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3223; _temp3223.tag= Cyc_InitializerList_tok;
_temp3223.f1=({ struct Cyc_List_List* _temp3224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3224->hd=( void*)({ struct
_tuple19* _temp3225=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3225->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3225->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3225;}); _temp3224->tl= 0; _temp3224;});
_temp3223;}); _temp3222;}); break; case 182: _LL3221: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3227=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3227[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3228; _temp3228.tag= Cyc_InitializerList_tok;
_temp3228.f1=({ struct Cyc_List_List* _temp3229=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3229->hd=( void*)({ struct
_tuple19* _temp3230=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3230->f1= 0; _temp3230->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3230;}); _temp3229->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3229;});
_temp3228;}); _temp3227;}); break; case 183: _LL3226: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3232=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3232[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3233; _temp3233.tag= Cyc_InitializerList_tok;
_temp3233.f1=({ struct Cyc_List_List* _temp3234=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3234->hd=( void*)({ struct
_tuple19* _temp3235=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3235->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3235->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3235;}); _temp3234->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3234;});
_temp3233;}); _temp3232;}); break; case 184: _LL3231: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3237=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3237[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3238; _temp3238.tag= Cyc_DesignatorList_tok;
_temp3238.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3238;}); _temp3237;}); break; case 185: _LL3236: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3240=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3240[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3241; _temp3241.tag= Cyc_DesignatorList_tok;
_temp3241.f1=({ struct Cyc_List_List* _temp3242=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3242->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3242->tl=
0; _temp3242;}); _temp3241;}); _temp3240;}); break; case 186: _LL3239: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3244=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3244[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3245; _temp3245.tag= Cyc_DesignatorList_tok;
_temp3245.f1=({ struct Cyc_List_List* _temp3246=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3246->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3246->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3246;}); _temp3245;}); _temp3244;}); break; case 187: _LL3243: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3248=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3248[ 0]=({ struct
Cyc_Designator_tok_struct _temp3249; _temp3249.tag= Cyc_Designator_tok;
_temp3249.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3250=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3250[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3251; _temp3251.tag=
Cyc_Absyn_ArrayElement; _temp3251.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3251;}); _temp3250;})); _temp3249;});
_temp3248;}); break; case 188: _LL3247: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3253=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3253[ 0]=({ struct Cyc_Designator_tok_struct _temp3254; _temp3254.tag= Cyc_Designator_tok;
_temp3254.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3255=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3255[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3256; _temp3256.tag= Cyc_Absyn_FieldName;
_temp3256.f1=({ struct _tagged_string* _temp3257=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3257[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3257;});
_temp3256;}); _temp3255;})); _temp3254;}); _temp3253;}); break; case 189:
_LL3252: { void* _temp3259= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3260; _LL3260: { struct Cyc_List_List* _temp3261=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3262; _LL3262: if( _temp3261 != 0){ Cyc_Parse_warn(({ char* _temp3263="ignoring attributes in type";
struct _tagged_string _temp3264; _temp3264.curr= _temp3263; _temp3264.base=
_temp3263; _temp3264.last_plus_one= _temp3263 + 28; _temp3264;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tqual _temp3265=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f1; goto _LL3266; _LL3266: yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp3267=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3267[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3268; _temp3268.tag= Cyc_ParamDecl_tok; _temp3268.f1=({
struct _tuple2* _temp3269=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp3269[ 0]=({ struct _tuple2 _temp3270; _temp3270.f1= 0; _temp3270.f2=
_temp3265; _temp3270.f3= _temp3259; _temp3270;}); _temp3269;}); _temp3268;});
_temp3267;}); break;}}} case 190: _LL3258: { void* _temp3272= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3273; _LL3273: { struct Cyc_List_List*
_temp3274=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3275; _LL3275: { struct Cyc_Absyn_Tqual
_temp3276=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3277; _LL3277: { struct Cyc_List_List* _temp3278=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3279; _LL3279: { struct _tuple6 _temp3280= Cyc_Parse_apply_tms(
_temp3276, _temp3272, _temp3274, _temp3278); goto _LL3281; _LL3281: if(
_temp3280.f3 != 0){ Cyc_Parse_warn(({ char* _temp3282="bad type params, ignoring";
struct _tagged_string _temp3283; _temp3283.curr= _temp3282; _temp3283.base=
_temp3282; _temp3283.last_plus_one= _temp3282 + 26; _temp3283;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3280.f4 != 0){ Cyc_Parse_warn(({ char* _temp3284="bad specifiers, ignoring";
struct _tagged_string _temp3285; _temp3285.curr= _temp3284; _temp3285.base=
_temp3284; _temp3285.last_plus_one= _temp3284 + 25; _temp3285;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3286=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3286[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3287; _temp3287.tag= Cyc_ParamDecl_tok; _temp3287.f1=({
struct _tuple2* _temp3288=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3288->f1= 0; _temp3288->f2= _temp3280.f1; _temp3288->f3= _temp3280.f2;
_temp3288;}); _temp3287;}); _temp3286;}); break;}}}}} case 191: _LL3271: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3290=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3290[ 0]=({ struct Cyc_Type_tok_struct
_temp3291; _temp3291.tag= Cyc_Type_tok; _temp3291.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3291;});
_temp3290;}); break; case 192: _LL3289: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3293=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3293[ 0]=({ struct Cyc_Type_tok_struct _temp3294; _temp3294.tag= Cyc_Type_tok;
_temp3294.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3295=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3295[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3296; _temp3296.tag= Cyc_Absyn_JoinEff;
_temp3296.f1= 0; _temp3296;}); _temp3295;})); _temp3294;}); _temp3293;}); break;
case 193: _LL3292: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3298=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3298[ 0]=({ struct Cyc_Type_tok_struct _temp3299; _temp3299.tag= Cyc_Type_tok;
_temp3299.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3300=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3300[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3301; _temp3301.tag= Cyc_Absyn_JoinEff;
_temp3301.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3301;}); _temp3300;})); _temp3299;}); _temp3298;});
break; case 194: _LL3297: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3303=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3303[ 0]=({ struct Cyc_Type_tok_struct _temp3304; _temp3304.tag= Cyc_Type_tok;
_temp3304.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3305=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3305[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3306; _temp3306.tag= Cyc_Absyn_JoinEff;
_temp3306.f1=({ struct Cyc_List_List* _temp3307=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3307->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3307->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3307;}); _temp3306;}); _temp3305;})); _temp3304;}); _temp3303;}); break;
case 195: _LL3302: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3309=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3309[ 0]=({ struct Cyc_TypeList_tok_struct _temp3310; _temp3310.tag= Cyc_TypeList_tok;
_temp3310.f1=({ struct Cyc_List_List* _temp3311=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3311->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3311->tl=
0; _temp3311;}); _temp3310;}); _temp3309;}); break; case 196: _LL3308: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3313=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3313[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3314; _temp3314.tag= Cyc_TypeList_tok; _temp3314.f1=({ struct Cyc_List_List*
_temp3315=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3315->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3315->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3315;}); _temp3314;}); _temp3313;});
break; case 197: _LL3312: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3317=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3317[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3318; _temp3318.tag= Cyc_AbstractDeclarator_tok; _temp3318.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3319=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3319->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3319;}); _temp3318;}); _temp3317;}); break;
case 198: _LL3316: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 199: _LL3320: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3322=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3322[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3323; _temp3323.tag= Cyc_AbstractDeclarator_tok; _temp3323.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3324=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3324->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3324;}); _temp3323;});
_temp3322;}); break; case 200: _LL3321: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 201: _LL3325: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3327=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3327[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3328; _temp3328.tag= Cyc_AbstractDeclarator_tok; _temp3328.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3329=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3329->tms=({ struct Cyc_List_List* _temp3330=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3330->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3330->tl= 0; _temp3330;}); _temp3329;}); _temp3328;}); _temp3327;}); break;
case 202: _LL3326: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3332=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3332[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3333; _temp3333.tag= Cyc_AbstractDeclarator_tok; _temp3333.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3334=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3334->tms=({ struct Cyc_List_List* _temp3335=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3335->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3335->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3335;}); _temp3334;}); _temp3333;});
_temp3332;}); break; case 203: _LL3331: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3337=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3337[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3338; _temp3338.tag= Cyc_AbstractDeclarator_tok; _temp3338.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3339=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3339->tms=({ struct Cyc_List_List* _temp3340=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3340->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3341=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3341[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3342; _temp3342.tag= Cyc_Absyn_ConstArray_mod;
_temp3342.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3342;}); _temp3341;})); _temp3340->tl= 0; _temp3340;}); _temp3339;});
_temp3338;}); _temp3337;}); break; case 204: _LL3336: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3344=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3344[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3345; _temp3345.tag= Cyc_AbstractDeclarator_tok;
_temp3345.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3346=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3346->tms=({
struct Cyc_List_List* _temp3347=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3347->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3348=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3348[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3349; _temp3349.tag= Cyc_Absyn_ConstArray_mod; _temp3349.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3349;});
_temp3348;})); _temp3347->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3347;}); _temp3346;});
_temp3345;}); _temp3344;}); break; case 205: _LL3343: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3351=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3351[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3352; _temp3352.tag= Cyc_AbstractDeclarator_tok;
_temp3352.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3353=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3353->tms=({
struct Cyc_List_List* _temp3354=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3354->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3355=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3355[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3356; _temp3356.tag=
Cyc_Absyn_Function_mod; _temp3356.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3357=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3357[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3358; _temp3358.tag= Cyc_Absyn_WithTypes;
_temp3358.f1= 0; _temp3358.f2= 0; _temp3358.f3= 0; _temp3358;}); _temp3357;}));
_temp3356;}); _temp3355;})); _temp3354->tl= 0; _temp3354;}); _temp3353;});
_temp3352;}); _temp3351;}); break; case 206: _LL3350: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3360=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3360[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3361; _temp3361.tag= Cyc_AbstractDeclarator_tok;
_temp3361.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3362=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3362->tms=({
struct Cyc_List_List* _temp3363=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3363->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3364=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3364[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3365; _temp3365.tag=
Cyc_Absyn_Function_mod; _temp3365.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3366=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3366[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3367; _temp3367.tag= Cyc_Absyn_WithTypes;
_temp3367.f1= 0; _temp3367.f2= 0; _temp3367.f3=({ struct Cyc_Core_Opt* _temp3368=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3368->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3369=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3369[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3370; _temp3370.tag= Cyc_Absyn_JoinEff; _temp3370.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3370;}); _temp3369;})); _temp3368;}); _temp3367;}); _temp3366;}));
_temp3365;}); _temp3364;})); _temp3363->tl= 0; _temp3363;}); _temp3362;});
_temp3361;}); _temp3360;}); break; case 207: _LL3359: { struct _tuple17
_temp3374; struct Cyc_Core_Opt* _temp3375; int _temp3377; struct Cyc_List_List*
_temp3379; struct _tuple17* _temp3372= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3374=* _temp3372; _LL3380:
_temp3379= _temp3374.f1; goto _LL3378; _LL3378: _temp3377= _temp3374.f2; goto
_LL3376; _LL3376: _temp3375= _temp3374.f3; goto _LL3373; _LL3373: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3381=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3381[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3382; _temp3382.tag= Cyc_AbstractDeclarator_tok;
_temp3382.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3383=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3383->tms=({
struct Cyc_List_List* _temp3384=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3384->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3385=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3385[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3386; _temp3386.tag=
Cyc_Absyn_Function_mod; _temp3386.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3387=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3387[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3388; _temp3388.tag= Cyc_Absyn_WithTypes;
_temp3388.f1= _temp3379; _temp3388.f2= _temp3377; _temp3388.f3= _temp3375;
_temp3388;}); _temp3387;})); _temp3386;}); _temp3385;})); _temp3384->tl= 0;
_temp3384;}); _temp3383;}); _temp3382;}); _temp3381;}); break;} case 208:
_LL3371: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3390=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3390[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3391; _temp3391.tag=
Cyc_AbstractDeclarator_tok; _temp3391.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3392=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3392->tms=({ struct Cyc_List_List* _temp3393=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3393->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3394=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3394[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3395; _temp3395.tag= Cyc_Absyn_Function_mod;
_temp3395.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3396=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3396[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3397; _temp3397.tag= Cyc_Absyn_WithTypes;
_temp3397.f1= 0; _temp3397.f2= 0; _temp3397.f3= 0; _temp3397;}); _temp3396;}));
_temp3395;}); _temp3394;})); _temp3393->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp3393;}); _temp3392;}); _temp3391;}); _temp3390;}); break; case 209: _LL3389: {
struct Cyc_Core_Opt* _temp3402=({ struct Cyc_Core_Opt* _temp3399=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3399->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3400=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3400[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3401; _temp3401.tag= Cyc_Absyn_JoinEff; _temp3401.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3401;}); _temp3400;})); _temp3399;}); goto _LL3403; _LL3403: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3404=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3404[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3405; _temp3405.tag= Cyc_AbstractDeclarator_tok;
_temp3405.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3406=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3406->tms=({
struct Cyc_List_List* _temp3407=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3407->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3408=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3408[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3409; _temp3409.tag=
Cyc_Absyn_Function_mod; _temp3409.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3410=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3410[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3411; _temp3411.tag= Cyc_Absyn_WithTypes;
_temp3411.f1= 0; _temp3411.f2= 0; _temp3411.f3= _temp3402; _temp3411;});
_temp3410;})); _temp3409;}); _temp3408;})); _temp3407->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3407;}); _temp3406;}); _temp3405;}); _temp3404;}); break;} case 210:
_LL3398: { struct _tuple17 _temp3415; struct Cyc_Core_Opt* _temp3416; int
_temp3418; struct Cyc_List_List* _temp3420; struct _tuple17* _temp3413= Cyc_yyget_ParamDeclListBool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3415=*
_temp3413; _LL3421: _temp3420= _temp3415.f1; goto _LL3419; _LL3419: _temp3418=
_temp3415.f2; goto _LL3417; _LL3417: _temp3416= _temp3415.f3; goto _LL3414;
_LL3414: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3422=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3422[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3423; _temp3423.tag=
Cyc_AbstractDeclarator_tok; _temp3423.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3424=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3424->tms=({ struct Cyc_List_List* _temp3425=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3425->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3426=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3426[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3427; _temp3427.tag= Cyc_Absyn_Function_mod;
_temp3427.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3428=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3428[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3429; _temp3429.tag= Cyc_Absyn_WithTypes;
_temp3429.f1= _temp3420; _temp3429.f2= _temp3418; _temp3429.f3= _temp3416;
_temp3429;}); _temp3428;})); _temp3427;}); _temp3426;})); _temp3425->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3425;}); _temp3424;}); _temp3423;}); _temp3422;}); break;} case 211:
_LL3412: { struct Cyc_List_List* _temp3431=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3432; _LL3432: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3433=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3433[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3434; _temp3434.tag= Cyc_AbstractDeclarator_tok; _temp3434.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3435=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3435->tms=({ struct Cyc_List_List* _temp3436=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3436->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3437=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3437[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3438; _temp3438.tag= Cyc_Absyn_TypeParams_mod;
_temp3438.f1= _temp3431; _temp3438.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3438.f3= 0; _temp3438;}); _temp3437;})); _temp3436->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3436;}); _temp3435;}); _temp3434;}); _temp3433;}); break;} case 212:
_LL3430:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3440=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3441; _LL3441: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3442=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3442[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3443; _temp3443.tag= Cyc_AbstractDeclarator_tok; _temp3443.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3444=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3444->tms=({ struct Cyc_List_List* _temp3445=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3445->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3446=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3446[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3447; _temp3447.tag= Cyc_Absyn_TypeParams_mod;
_temp3447.f1= _temp3440; _temp3447.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3447.f3= 0; _temp3447;}); _temp3446;})); _temp3445->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3445;}); _temp3444;}); _temp3443;}); _temp3442;}); break;} case 213:
_LL3439: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3449=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3449[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3450; _temp3450.tag=
Cyc_AbstractDeclarator_tok; _temp3450.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3451=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3451->tms=({ struct Cyc_List_List* _temp3452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3452->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3453=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3453[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3454; _temp3454.tag= Cyc_Absyn_Attributes_mod;
_temp3454.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3454.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3454;});
_temp3453;})); _temp3452->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3452;}); _temp3451;});
_temp3450;}); _temp3449;}); break; case 214: _LL3448: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 215: _LL3455: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 216: _LL3456: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 217: _LL3457: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 218: _LL3458: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 219: _LL3459: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 220: _LL3460: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),({
char* _temp3462="`H"; struct _tagged_string _temp3463; _temp3463.curr= _temp3462;
_temp3463.base= _temp3462; _temp3463.last_plus_one= _temp3462 + 3; _temp3463;}))
== 0){ Cyc_Parse_err(({ char* _temp3464="bad occurrence of heap region `H";
struct _tagged_string _temp3465; _temp3465.curr= _temp3464; _temp3465.base=
_temp3464; _temp3465.last_plus_one= _temp3464 + 33; _temp3465;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3476=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3476->name=({ struct
_tagged_string* _temp3477=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3477[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3477;}); _temp3476->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3476;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3474=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3474[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3475; _temp3475.tag= Cyc_Absyn_VarType; _temp3475.f1=
tv; _temp3475;}); _temp3474;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3466=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3466[ 0]=({ struct Cyc_Stmt_tok_struct _temp3467; _temp3467.tag= Cyc_Stmt_tok;
_temp3467.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3468=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3468[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3469; _temp3469.tag= Cyc_Absyn_Region_s;
_temp3469.f1= tv; _temp3469.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3470=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3470->f1=(
void*) Cyc_Absyn_Loc_n; _temp3470->f2=({ struct _tagged_string* _temp3471=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3471[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3471;}); _temp3470;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3472=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3472[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3473; _temp3473.tag= Cyc_Absyn_RgnHandleType; _temp3473.f1=( void*) t;
_temp3473;}); _temp3472;}), 0); _temp3469.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3469;}); _temp3468;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3467;}); _temp3466;}); break;} case 221: _LL3461: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),({
char* _temp3479="H"; struct _tagged_string _temp3480; _temp3480.curr= _temp3479;
_temp3480.base= _temp3479; _temp3480.last_plus_one= _temp3479 + 2; _temp3480;}))
== 0){ Cyc_Parse_err(({ char* _temp3481="bad occurrence of heap region `H";
struct _tagged_string _temp3482; _temp3482.curr= _temp3481; _temp3482.base=
_temp3481; _temp3482.last_plus_one= _temp3481 + 33; _temp3482;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3493=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3493->name=({ struct
_tagged_string* _temp3494=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3494[ 0]=({ struct _tagged_string _temp3495= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); xprintf("`%.*s",
_temp3495.last_plus_one - _temp3495.curr, _temp3495.curr);}); _temp3494;});
_temp3493->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp3493;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3491=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3491[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3492; _temp3492.tag= Cyc_Absyn_VarType;
_temp3492.f1= tv; _temp3492;}); _temp3491;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3483=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3483[ 0]=({ struct Cyc_Stmt_tok_struct _temp3484; _temp3484.tag= Cyc_Stmt_tok;
_temp3484.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3485=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3485[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3486; _temp3486.tag= Cyc_Absyn_Region_s;
_temp3486.f1= tv; _temp3486.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3487=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3487->f1=(
void*) Cyc_Absyn_Loc_n; _temp3487->f2=({ struct _tagged_string* _temp3488=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3488[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3488;}); _temp3487;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3489=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3489[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3490; _temp3490.tag= Cyc_Absyn_RgnHandleType; _temp3490.f1=( void*) t;
_temp3490;}); _temp3489;}), 0); _temp3486.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3486;}); _temp3485;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3484;}); _temp3483;}); break;} case 222: _LL3478: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3497=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3497[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3498; _temp3498.tag= Cyc_Stmt_tok; _temp3498.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3499=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3499[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3500; _temp3500.tag= Cyc_Absyn_Cut_s; _temp3500.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3500;});
_temp3499;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3498;});
_temp3497;}); break; case 223: _LL3496: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3502=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3502[ 0]=({ struct Cyc_Stmt_tok_struct _temp3503; _temp3503.tag= Cyc_Stmt_tok;
_temp3503.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3504=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3504[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3505; _temp3505.tag= Cyc_Absyn_Splice_s;
_temp3505.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3505;}); _temp3504;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3503;}); _temp3502;}); break; case 224: _LL3501: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3507=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3507[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3508; _temp3508.tag= Cyc_Stmt_tok; _temp3508.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3509=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3509[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3510; _temp3510.tag= Cyc_Absyn_Label_s; _temp3510.f1=({
struct _tagged_string* _temp3511=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3511[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3511;}); _temp3510.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3510;});
_temp3509;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3508;});
_temp3507;}); break; case 225: _LL3506: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3513=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3513[ 0]=({ struct Cyc_Stmt_tok_struct _temp3514; _temp3514.tag= Cyc_Stmt_tok;
_temp3514.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3514;});
_temp3513;}); break; case 226: _LL3512: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3516=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3516[ 0]=({ struct Cyc_Stmt_tok_struct _temp3517; _temp3517.tag= Cyc_Stmt_tok;
_temp3517.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3517;});
_temp3516;}); break; case 227: _LL3515: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3519=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3519[ 0]=({ struct Cyc_Stmt_tok_struct _temp3520; _temp3520.tag= Cyc_Stmt_tok;
_temp3520.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3520;});
_temp3519;}); break; case 228: _LL3518: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 229: _LL3521:{ void* _temp3523= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3531; struct Cyc_Absyn_Fndecl* _temp3533; struct Cyc_Absyn_Stmt* _temp3535;
_LL3525: if(*(( int*) _temp3523) == Cyc_Parse_TopDecls_bl){ _LL3532: _temp3531=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp3523)->f1; goto _LL3526;} else{ goto
_LL3527;} _LL3527: if(*(( int*) _temp3523) == Cyc_Parse_FnDecl_bl){ _LL3534:
_temp3533=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp3523)->f1; goto _LL3528;}
else{ goto _LL3529;} _LL3529: if(*(( int*) _temp3523) == Cyc_Parse_Stmt_bl){
_LL3536: _temp3535=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3523)->f1; goto
_LL3530;} else{ goto _LL3524;} _LL3526: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3537=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3537[ 0]=({ struct Cyc_Stmt_tok_struct _temp3538; _temp3538.tag= Cyc_Stmt_tok;
_temp3538.f1= Cyc_Parse_flatten_declarations( _temp3531, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3538;}); _temp3537;}); goto _LL3524; _LL3528:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3539=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3539[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3540; _temp3540.tag= Cyc_Stmt_tok; _temp3540.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3541=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3541[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3542; _temp3542.tag= Cyc_Absyn_Fn_d;
_temp3542.f1= _temp3533; _temp3542;}); _temp3541;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3540;});
_temp3539;}); goto _LL3524; _LL3530: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3543=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3543[ 0]=({ struct Cyc_Stmt_tok_struct _temp3544; _temp3544.tag= Cyc_Stmt_tok;
_temp3544.f1= _temp3535; _temp3544;}); _temp3543;}); goto _LL3524; _LL3524:;}
break; case 230: _LL3522:{ void* _temp3546= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3554;
struct Cyc_Absyn_Fndecl* _temp3556; struct Cyc_Absyn_Stmt* _temp3558; _LL3548:
if(*(( int*) _temp3546) == Cyc_Parse_TopDecls_bl){ _LL3555: _temp3554=(( struct
Cyc_Parse_TopDecls_bl_struct*) _temp3546)->f1; goto _LL3549;} else{ goto _LL3550;}
_LL3550: if(*(( int*) _temp3546) == Cyc_Parse_FnDecl_bl){ _LL3557: _temp3556=((
struct Cyc_Parse_FnDecl_bl_struct*) _temp3546)->f1; goto _LL3551;} else{ goto
_LL3552;} _LL3552: if(*(( int*) _temp3546) == Cyc_Parse_Stmt_bl){ _LL3559:
_temp3558=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3546)->f1; goto _LL3553;}
else{ goto _LL3547;} _LL3549: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3560=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3560[ 0]=({ struct Cyc_Stmt_tok_struct _temp3561; _temp3561.tag= Cyc_Stmt_tok;
_temp3561.f1= Cyc_Parse_flatten_declarations( _temp3554, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3561;});
_temp3560;}); goto _LL3547; _LL3551: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3562=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3562[ 0]=({ struct Cyc_Stmt_tok_struct _temp3563; _temp3563.tag= Cyc_Stmt_tok;
_temp3563.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3564=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3564[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3565; _temp3565.tag= Cyc_Absyn_Fn_d;
_temp3565.f1= _temp3556; _temp3565;}); _temp3564;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3563;}); _temp3562;}); goto _LL3547; _LL3553: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3566=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3566[ 0]=({ struct Cyc_Stmt_tok_struct _temp3567; _temp3567.tag= Cyc_Stmt_tok;
_temp3567.f1= Cyc_Absyn_seq_stmt( _temp3558, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3567;}); _temp3566;}); goto _LL3547; _LL3547:;} break; case 231: _LL3545:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3569=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3569[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3570; _temp3570.tag= Cyc_BlockItem_tok; _temp3570.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3571=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3571[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3572; _temp3572.tag= Cyc_Parse_TopDecls_bl;
_temp3572.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3572;}); _temp3571;})); _temp3570;}); _temp3569;}); break;
case 232: _LL3568: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3574=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3574[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3575; _temp3575.tag= Cyc_BlockItem_tok;
_temp3575.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3576=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3576[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3577; _temp3577.tag= Cyc_Parse_Stmt_bl;
_temp3577.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3577;}); _temp3576;})); _temp3575;}); _temp3574;}); break;
case 233: _LL3573: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3579=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3579[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3580; _temp3580.tag= Cyc_BlockItem_tok;
_temp3580.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3581=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3581[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3582; _temp3582.tag= Cyc_Parse_FnDecl_bl;
_temp3582.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3582;}); _temp3581;})); _temp3580;}); _temp3579;}); break;
case 234: _LL3578: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3584=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3584[ 0]=({ struct Cyc_Stmt_tok_struct _temp3585; _temp3585.tag= Cyc_Stmt_tok;
_temp3585.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3585;}); _temp3584;}); break; case 235: _LL3583: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3587=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3587[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3588; _temp3588.tag= Cyc_Stmt_tok; _temp3588.f1= Cyc_Absyn_ifthenelse_stmt(
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
_temp3588;}); _temp3587;}); break; case 236: _LL3586: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3590=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3590[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3591; _temp3591.tag= Cyc_Stmt_tok; _temp3591.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3591;}); _temp3590;}); break; case 237: _LL3589: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3593=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3593[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3594; _temp3594.tag= Cyc_Stmt_tok; _temp3594.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3594;}); _temp3593;}); break; case 238: _LL3592:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3596=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3596[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3597; _temp3597.tag= Cyc_SwitchClauseList_tok;
_temp3597.f1= 0; _temp3597;}); _temp3596;}); break; case 239: _LL3595: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3599=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3599[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3600; _temp3600.tag= Cyc_SwitchClauseList_tok;
_temp3600.f1=({ struct Cyc_List_List* _temp3601=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3601->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3602=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3602->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3602->pat_vars= 0; _temp3602->where_clause= 0; _temp3602->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3602->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3602;}); _temp3601->tl= 0; _temp3601;});
_temp3600;}); _temp3599;}); break; case 240: _LL3598: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3604=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3604[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3605; _temp3605.tag= Cyc_SwitchClauseList_tok;
_temp3605.f1=({ struct Cyc_List_List* _temp3606=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3606->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3607=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3607->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3607->pat_vars= 0; _temp3607->where_clause= 0;
_temp3607->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3607->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3607;});
_temp3606->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3606;}); _temp3605;}); _temp3604;}); break; case 241:
_LL3603: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3609=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3609[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3610; _temp3610.tag=
Cyc_SwitchClauseList_tok; _temp3610.f1=({ struct Cyc_List_List* _temp3611=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3611->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3612=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3612->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3612->pat_vars=
0; _temp3612->where_clause= 0; _temp3612->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3612->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3612;}); _temp3611->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3611;}); _temp3610;}); _temp3609;});
break; case 242: _LL3608: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3614=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3614[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3615; _temp3615.tag= Cyc_SwitchClauseList_tok; _temp3615.f1=({ struct Cyc_List_List*
_temp3616=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3616->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3617=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3617->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3617->pat_vars=
0; _temp3617->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3617->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3617->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3617;}); _temp3616->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3616;}); _temp3615;}); _temp3614;});
break; case 243: _LL3613: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3619=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3619[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3620; _temp3620.tag= Cyc_SwitchClauseList_tok; _temp3620.f1=({ struct Cyc_List_List*
_temp3621=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3621->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3622=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3622->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3622->pat_vars=
0; _temp3622->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3622->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3622->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3622;}); _temp3621->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3621;});
_temp3620;}); _temp3619;}); break; case 244: _LL3618: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3624=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3624[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3625; _temp3625.tag= Cyc_Stmt_tok; _temp3625.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3625;}); _temp3624;}); break; case 245: _LL3623: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3627=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3627[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3628; _temp3628.tag= Cyc_Stmt_tok; _temp3628.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3628;}); _temp3627;}); break; case 246: _LL3626: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3630=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3630[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3631; _temp3631.tag= Cyc_Stmt_tok; _temp3631.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3631;}); _temp3630;}); break; case 247: _LL3629: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3633=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3633[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3634; _temp3634.tag= Cyc_Stmt_tok; _temp3634.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3634;});
_temp3633;}); break; case 248: _LL3632: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3636=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3636[ 0]=({ struct Cyc_Stmt_tok_struct _temp3637; _temp3637.tag= Cyc_Stmt_tok;
_temp3637.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3637;}); _temp3636;}); break; case 249: _LL3635:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3639=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3639[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3640; _temp3640.tag= Cyc_Stmt_tok; _temp3640.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
_temp3640;}); _temp3639;}); break; case 250: _LL3638: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3642=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3642[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3643; _temp3643.tag= Cyc_Stmt_tok; _temp3643.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3643;});
_temp3642;}); break; case 251: _LL3641: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3645=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3645[ 0]=({ struct Cyc_Stmt_tok_struct _temp3646; _temp3646.tag= Cyc_Stmt_tok;
_temp3646.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
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
_temp3646;}); _temp3645;}); break; case 252: _LL3644: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3648=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3648[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3649; _temp3649.tag= Cyc_Stmt_tok; _temp3649.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
yylsp_offset)).last_line)); _temp3649;}); _temp3648;}); break; case 253: _LL3647:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3651=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3651[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3652; _temp3652.tag= Cyc_Stmt_tok; _temp3652.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
_temp3652;}); _temp3651;}); break; case 254: _LL3650: { struct Cyc_List_List*
_temp3654= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3655; _LL3655: { struct Cyc_Absyn_Stmt* _temp3656=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3657; _LL3657: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3658=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3658[ 0]=({ struct Cyc_Stmt_tok_struct _temp3659; _temp3659.tag= Cyc_Stmt_tok;
_temp3659.f1= Cyc_Parse_flatten_declarations( _temp3654, _temp3656); _temp3659;});
_temp3658;}); break;}} case 255: _LL3653: { struct Cyc_List_List* _temp3661= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3662; _LL3662: { struct Cyc_Absyn_Stmt* _temp3663= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3664;
_LL3664: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3665=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3665[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3666; _temp3666.tag= Cyc_Stmt_tok; _temp3666.f1= Cyc_Parse_flatten_declarations(
_temp3661, _temp3663); _temp3666;}); _temp3665;}); break;}} case 256: _LL3660: {
struct Cyc_List_List* _temp3668= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3669; _LL3669: { struct Cyc_Absyn_Stmt*
_temp3670= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3671; _LL3671: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3672=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3672[ 0]=({ struct Cyc_Stmt_tok_struct _temp3673; _temp3673.tag= Cyc_Stmt_tok;
_temp3673.f1= Cyc_Parse_flatten_declarations( _temp3668, _temp3670); _temp3673;});
_temp3672;}); break;}} case 257: _LL3667: { struct Cyc_List_List* _temp3675= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3676; _LL3676: { struct Cyc_Absyn_Stmt* _temp3677= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
goto _LL3678; _LL3678: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3679=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3679[ 0]=({ struct Cyc_Stmt_tok_struct _temp3680; _temp3680.tag= Cyc_Stmt_tok;
_temp3680.f1= Cyc_Parse_flatten_declarations( _temp3675, _temp3677); _temp3680;});
_temp3679;}); break;}} case 258: _LL3674: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3682=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3682[ 0]=({ struct Cyc_Stmt_tok_struct _temp3683; _temp3683.tag= Cyc_Stmt_tok;
_temp3683.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3684=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3684[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3684;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3683;});
_temp3682;}); break; case 259: _LL3681: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3686=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3686[ 0]=({ struct Cyc_Stmt_tok_struct _temp3687; _temp3687.tag= Cyc_Stmt_tok;
_temp3687.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3687;}); _temp3686;}); break; case 260: _LL3685: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3689=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3689[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3690; _temp3690.tag= Cyc_Stmt_tok; _temp3690.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3690;}); _temp3689;}); break; case 261: _LL3688: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3692=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3692[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3693; _temp3693.tag= Cyc_Stmt_tok; _temp3693.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3693;}); _temp3692;}); break; case 262:
_LL3691: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3695=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3695[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3696; _temp3696.tag= Cyc_Stmt_tok; _temp3696.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3696;});
_temp3695;}); break; case 263: _LL3694: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3698=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3698[ 0]=({ struct Cyc_Stmt_tok_struct _temp3699; _temp3699.tag= Cyc_Stmt_tok;
_temp3699.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3699;}); _temp3698;}); break; case 264: _LL3697: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3701=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3702; _temp3702.tag= Cyc_Stmt_tok; _temp3702.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3702;}); _temp3701;}); break; case 265:
_LL3700: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3704=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3704[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3705; _temp3705.tag= Cyc_Stmt_tok; _temp3705.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3705;}); _temp3704;}); break; case 266:
_LL3703: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3707=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3707[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3708; _temp3708.tag= Cyc_Pattern_tok; _temp3708.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3708;});
_temp3707;}); break; case 267: _LL3706: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 268: _LL3709: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3711=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3711[ 0]=({ struct Cyc_Pattern_tok_struct _temp3712; _temp3712.tag= Cyc_Pattern_tok;
_temp3712.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3713=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3713[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3714; _temp3714.tag= Cyc_Absyn_Int_p;
_temp3714.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3714.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3714;}); _temp3713;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3712;}); _temp3711;}); break; case 269: _LL3710: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3716=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3716[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3717; _temp3717.tag= Cyc_Pattern_tok; _temp3717.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3718=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3718[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3719; _temp3719.tag= Cyc_Absyn_Int_p; _temp3719.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3719.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3719;}); _temp3718;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3717;}); _temp3716;}); break; case 270: _LL3715: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3721=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3721[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3722; _temp3722.tag= Cyc_Pattern_tok; _temp3722.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3723=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3723[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3724; _temp3724.tag= Cyc_Absyn_Float_p; _temp3724.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3724;}); _temp3723;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3722;});
_temp3721;}); break; case 271: _LL3720: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3726=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3726[ 0]=({ struct Cyc_Pattern_tok_struct _temp3727; _temp3727.tag= Cyc_Pattern_tok;
_temp3727.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3728=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3728[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3729; _temp3729.tag= Cyc_Absyn_Char_p;
_temp3729.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3729;}); _temp3728;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3727;}); _temp3726;}); break; case 272: _LL3725: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3731=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3731[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3732; _temp3732.tag= Cyc_Pattern_tok; _temp3732.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3732;});
_temp3731;}); break; case 273: _LL3730: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3734=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3734[ 0]=({ struct Cyc_Pattern_tok_struct _temp3735; _temp3735.tag= Cyc_Pattern_tok;
_temp3735.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3736=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3736[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3737; _temp3737.tag=
Cyc_Absyn_UnknownId_p; _temp3737.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3737;}); _temp3736;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3735;}); _temp3734;}); break; case 274: _LL3733: { struct Cyc_List_List*
_temp3739=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3740; _LL3740: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3741=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3741[ 0]=({ struct Cyc_Pattern_tok_struct _temp3742; _temp3742.tag= Cyc_Pattern_tok;
_temp3742.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3743=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3743[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3744; _temp3744.tag= Cyc_Absyn_UnknownCall_p; _temp3744.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3744.f2=
_temp3739; _temp3744.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3744;}); _temp3743;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3742;}); _temp3741;}); break;} case 275: _LL3738: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3746=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3746[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3747; _temp3747.tag= Cyc_Pattern_tok; _temp3747.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3748=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3748[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3749; _temp3749.tag= Cyc_Absyn_Tuple_p; _temp3749.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3749;}); _temp3748;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3747;});
_temp3746;}); break; case 276: _LL3745: { struct Cyc_List_List* _temp3751=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3752; _LL3752: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3753=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3753[ 0]=({ struct Cyc_Pattern_tok_struct _temp3754; _temp3754.tag= Cyc_Pattern_tok;
_temp3754.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3755=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3755[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3756; _temp3756.tag= Cyc_Absyn_UnknownFields_p; _temp3756.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3756.f2=
_temp3751; _temp3756.f3= 0; _temp3756;}); _temp3755;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3754;}); _temp3753;}); break;} case 277: _LL3750: { struct Cyc_List_List*
_temp3758=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3759; _LL3759: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3760=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3760[ 0]=({ struct Cyc_Pattern_tok_struct _temp3761; _temp3761.tag= Cyc_Pattern_tok;
_temp3761.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3762=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3762[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3763; _temp3763.tag= Cyc_Absyn_UnknownFields_p; _temp3763.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3763.f2=
_temp3758; _temp3763.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3763;}); _temp3762;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3761;}); _temp3760;}); break;} case 278: _LL3757: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3765=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3765[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3766; _temp3766.tag= Cyc_Pattern_tok; _temp3766.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3767=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3767[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3768; _temp3768.tag= Cyc_Absyn_Pointer_p;
_temp3768.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3768;}); _temp3767;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3766;}); _temp3765;}); break; case 279: _LL3764: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3770=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3770[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3771; _temp3771.tag= Cyc_Pattern_tok; _temp3771.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3772=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3772[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3773; _temp3773.tag= Cyc_Absyn_Reference_p;
_temp3773.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3774=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3774->f1=( void*) Cyc_Absyn_Loc_n;
_temp3774->f2=({ struct _tagged_string* _temp3775=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3775[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3775;});
_temp3774;}),( void*) Cyc_Absyn_VoidType, 0); _temp3773;}); _temp3772;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3771;}); _temp3770;}); break; case 280: _LL3769: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3777=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3777[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3778; _temp3778.tag= Cyc_PatternList_tok;
_temp3778.f1= 0; _temp3778;}); _temp3777;}); break; case 281: _LL3776: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3780=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3780[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3781; _temp3781.tag= Cyc_PatternList_tok;
_temp3781.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3781;}); _temp3780;}); break; case 282: _LL3779: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3783=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3783[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3784; _temp3784.tag= Cyc_PatternList_tok;
_temp3784.f1=({ struct Cyc_List_List* _temp3785=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3785->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3785->tl=
0; _temp3785;}); _temp3784;}); _temp3783;}); break; case 283: _LL3782: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3787=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3787[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3788; _temp3788.tag= Cyc_PatternList_tok;
_temp3788.f1=({ struct Cyc_List_List* _temp3789=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3789->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3789->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3789;}); _temp3788;}); _temp3787;}); break; case 284: _LL3786: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3791=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3791[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3792; _temp3792.tag= Cyc_FieldPattern_tok;
_temp3792.f1=({ struct _tuple14* _temp3793=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3793->f1= 0; _temp3793->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3793;});
_temp3792;}); _temp3791;}); break; case 285: _LL3790: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3795=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3795[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3796; _temp3796.tag= Cyc_FieldPattern_tok;
_temp3796.f1=({ struct _tuple14* _temp3797=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3797->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3797->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3797;});
_temp3796;}); _temp3795;}); break; case 286: _LL3794: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3799=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3799[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3800; _temp3800.tag= Cyc_FieldPatternList_tok;
_temp3800.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3800;}); _temp3799;}); break; case 287: _LL3798: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3802=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3802[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3803; _temp3803.tag= Cyc_FieldPatternList_tok;
_temp3803.f1=({ struct Cyc_List_List* _temp3804=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3804->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3804->tl=
0; _temp3804;}); _temp3803;}); _temp3802;}); break; case 288: _LL3801: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3806=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3806[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3807; _temp3807.tag= Cyc_FieldPatternList_tok;
_temp3807.f1=({ struct Cyc_List_List* _temp3808=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3808->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3808->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3808;}); _temp3807;}); _temp3806;}); break; case 289: _LL3805: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 290:
_LL3809: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3811=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3811[ 0]=({ struct Cyc_Exp_tok_struct
_temp3812; _temp3812.tag= Cyc_Exp_tok; _temp3812.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3812;}); _temp3811;}); break; case 291: _LL3810: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 292: _LL3813: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3815=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3815[ 0]=({ struct Cyc_Exp_tok_struct
_temp3816; _temp3816.tag= Cyc_Exp_tok; _temp3816.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
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
_temp3816;}); _temp3815;}); break; case 293: _LL3814: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3818=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3818[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3819; _temp3819.tag= Cyc_Primopopt_tok; _temp3819.f1=
0; _temp3819;}); _temp3818;}); break; case 294: _LL3817: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3821=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3821[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3822; _temp3822.tag= Cyc_Primopopt_tok; _temp3822.f1=({
struct Cyc_Core_Opt* _temp3823=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3823->v=( void*)(( void*) Cyc_Absyn_Times); _temp3823;});
_temp3822;}); _temp3821;}); break; case 295: _LL3820: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3825=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3825[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3826; _temp3826.tag= Cyc_Primopopt_tok; _temp3826.f1=({
struct Cyc_Core_Opt* _temp3827=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3827->v=( void*)(( void*) Cyc_Absyn_Div); _temp3827;});
_temp3826;}); _temp3825;}); break; case 296: _LL3824: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3829=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3829[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3830; _temp3830.tag= Cyc_Primopopt_tok; _temp3830.f1=({
struct Cyc_Core_Opt* _temp3831=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3831->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3831;});
_temp3830;}); _temp3829;}); break; case 297: _LL3828: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3833=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3833[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3834; _temp3834.tag= Cyc_Primopopt_tok; _temp3834.f1=({
struct Cyc_Core_Opt* _temp3835=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3835->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3835;});
_temp3834;}); _temp3833;}); break; case 298: _LL3832: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3837=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3837[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3838; _temp3838.tag= Cyc_Primopopt_tok; _temp3838.f1=({
struct Cyc_Core_Opt* _temp3839=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3839->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3839;});
_temp3838;}); _temp3837;}); break; case 299: _LL3836: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3841=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3841[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3842; _temp3842.tag= Cyc_Primopopt_tok; _temp3842.f1=({
struct Cyc_Core_Opt* _temp3843=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3843->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3843;});
_temp3842;}); _temp3841;}); break; case 300: _LL3840: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3845=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3845[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3846; _temp3846.tag= Cyc_Primopopt_tok; _temp3846.f1=({
struct Cyc_Core_Opt* _temp3847=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3847->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3847;});
_temp3846;}); _temp3845;}); break; case 301: _LL3844: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3849=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3849[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3850; _temp3850.tag= Cyc_Primopopt_tok; _temp3850.f1=({
struct Cyc_Core_Opt* _temp3851=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3851->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3851;});
_temp3850;}); _temp3849;}); break; case 302: _LL3848: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3853=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3853[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3854; _temp3854.tag= Cyc_Primopopt_tok; _temp3854.f1=({
struct Cyc_Core_Opt* _temp3855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3855->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3855;});
_temp3854;}); _temp3853;}); break; case 303: _LL3852: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3857=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3857[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3858; _temp3858.tag= Cyc_Primopopt_tok; _temp3858.f1=({
struct Cyc_Core_Opt* _temp3859=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3859->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3859;});
_temp3858;}); _temp3857;}); break; case 304: _LL3856: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 305: _LL3860: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3862=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3862[ 0]=({ struct Cyc_Exp_tok_struct
_temp3863; _temp3863.tag= Cyc_Exp_tok; _temp3863.f1= Cyc_Absyn_conditional_exp(
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
_temp3863;}); _temp3862;}); break; case 306: _LL3861: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3865=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3865[ 0]=({ struct Cyc_Exp_tok_struct
_temp3866; _temp3866.tag= Cyc_Exp_tok; _temp3866.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3866;}); _temp3865;}); break; case 307: _LL3864: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3868=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3868[ 0]=({ struct Cyc_Exp_tok_struct
_temp3869; _temp3869.tag= Cyc_Exp_tok; _temp3869.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3869;}); _temp3868;}); break; case 308: _LL3867: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3871=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3871[ 0]=({ struct Cyc_Exp_tok_struct
_temp3872; _temp3872.tag= Cyc_Exp_tok; _temp3872.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3872;}); _temp3871;}); break; case 309: _LL3870: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3874=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3874[ 0]=({ struct Cyc_Exp_tok_struct
_temp3875; _temp3875.tag= Cyc_Exp_tok; _temp3875.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3875;});
_temp3874;}); break; case 310: _LL3873: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3877=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3877[ 0]=({ struct Cyc_Exp_tok_struct _temp3878; _temp3878.tag= Cyc_Exp_tok;
_temp3878.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3878;}); _temp3877;}); break; case 311: _LL3876: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 312: _LL3879: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 313: _LL3880: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3882=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3882[ 0]=({ struct Cyc_Exp_tok_struct
_temp3883; _temp3883.tag= Cyc_Exp_tok; _temp3883.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3883;}); _temp3882;}); break; case 314: _LL3881: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 315: _LL3884: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3886=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3886[ 0]=({ struct Cyc_Exp_tok_struct
_temp3887; _temp3887.tag= Cyc_Exp_tok; _temp3887.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3887;}); _temp3886;}); break; case 316: _LL3885: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 317: _LL3888: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3890=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3890[ 0]=({ struct Cyc_Exp_tok_struct
_temp3891; _temp3891.tag= Cyc_Exp_tok; _temp3891.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3891;});
_temp3890;}); break; case 318: _LL3889: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 319: _LL3892: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3894=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3894[ 0]=({ struct Cyc_Exp_tok_struct _temp3895; _temp3895.tag= Cyc_Exp_tok;
_temp3895.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3895;}); _temp3894;}); break; case 320: _LL3893: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 321: _LL3896: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3898=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3898[ 0]=({ struct Cyc_Exp_tok_struct
_temp3899; _temp3899.tag= Cyc_Exp_tok; _temp3899.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3899;});
_temp3898;}); break; case 322: _LL3897: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 323: _LL3900: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3902=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3902[ 0]=({ struct Cyc_Exp_tok_struct _temp3903; _temp3903.tag= Cyc_Exp_tok;
_temp3903.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3903;});
_temp3902;}); break; case 324: _LL3901: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3905=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3905[ 0]=({ struct Cyc_Exp_tok_struct _temp3906; _temp3906.tag= Cyc_Exp_tok;
_temp3906.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3906;});
_temp3905;}); break; case 325: _LL3904: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 326: _LL3907: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3909=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3909[ 0]=({ struct Cyc_Exp_tok_struct _temp3910; _temp3910.tag= Cyc_Exp_tok;
_temp3910.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3910;});
_temp3909;}); break; case 327: _LL3908: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3912=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3912[ 0]=({ struct Cyc_Exp_tok_struct _temp3913; _temp3913.tag= Cyc_Exp_tok;
_temp3913.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3913;});
_temp3912;}); break; case 328: _LL3911: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3915=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3915[ 0]=({ struct Cyc_Exp_tok_struct _temp3916; _temp3916.tag= Cyc_Exp_tok;
_temp3916.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3916;});
_temp3915;}); break; case 329: _LL3914: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3918=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3918[ 0]=({ struct Cyc_Exp_tok_struct _temp3919; _temp3919.tag= Cyc_Exp_tok;
_temp3919.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3919;});
_temp3918;}); break; case 330: _LL3917: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 331: _LL3920: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3922=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3922[ 0]=({ struct Cyc_Exp_tok_struct _temp3923; _temp3923.tag= Cyc_Exp_tok;
_temp3923.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3923;}); _temp3922;}); break; case 332: _LL3921: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3925=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3925[ 0]=({ struct Cyc_Exp_tok_struct
_temp3926; _temp3926.tag= Cyc_Exp_tok; _temp3926.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3926;});
_temp3925;}); break; case 333: _LL3924: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 334: _LL3927: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3929=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3929[ 0]=({ struct Cyc_Exp_tok_struct _temp3930; _temp3930.tag= Cyc_Exp_tok;
_temp3930.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3930;}); _temp3929;}); break; case 335: _LL3928: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3932=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3932[ 0]=({ struct Cyc_Exp_tok_struct
_temp3933; _temp3933.tag= Cyc_Exp_tok; _temp3933.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3933;});
_temp3932;}); break; case 336: _LL3931: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 337: _LL3934: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3936=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3936[ 0]=({ struct Cyc_Exp_tok_struct _temp3937; _temp3937.tag= Cyc_Exp_tok;
_temp3937.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3937;}); _temp3936;}); break; case 338: _LL3935: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3939=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3939[ 0]=({ struct Cyc_Exp_tok_struct
_temp3940; _temp3940.tag= Cyc_Exp_tok; _temp3940.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3940;});
_temp3939;}); break; case 339: _LL3938: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3942=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3942[ 0]=({ struct Cyc_Exp_tok_struct _temp3943; _temp3943.tag= Cyc_Exp_tok;
_temp3943.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3943;}); _temp3942;}); break; case 340: _LL3941: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 341: _LL3944: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3946=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3946[ 0]=({ struct Cyc_Exp_tok_struct
_temp3947; _temp3947.tag= Cyc_Exp_tok; _temp3947.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3947;}); _temp3946;}); break; case 342: _LL3945: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 343: _LL3948: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3950=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3950[ 0]=({ struct Cyc_Exp_tok_struct
_temp3951; _temp3951.tag= Cyc_Exp_tok; _temp3951.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3951;}); _temp3950;}); break; case 344: _LL3949: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3953=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3953[ 0]=({ struct Cyc_Exp_tok_struct
_temp3954; _temp3954.tag= Cyc_Exp_tok; _temp3954.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3954;}); _temp3953;}); break; case 345: _LL3952: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3956=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3956[ 0]=({ struct Cyc_Exp_tok_struct
_temp3957; _temp3957.tag= Cyc_Exp_tok; _temp3957.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3957;}); _temp3956;}); break; case 346: _LL3955: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3959=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3959[ 0]=({ struct Cyc_Exp_tok_struct
_temp3960; _temp3960.tag= Cyc_Exp_tok; _temp3960.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3960;}); _temp3959;}); break; case 347: _LL3958: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 348: _LL3961: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3963=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3963[ 0]=({ struct Cyc_Exp_tok_struct
_temp3964; _temp3964.tag= Cyc_Exp_tok; _temp3964.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3964;}); _temp3963;}); break; case 349: _LL3962: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3966=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3966[ 0]=({ struct Cyc_Exp_tok_struct
_temp3967; _temp3967.tag= Cyc_Exp_tok; _temp3967.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3967;}); _temp3966;}); break; case 350: _LL3965:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3969=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3969[ 0]=({ struct Cyc_Exp_tok_struct
_temp3970; _temp3970.tag= Cyc_Exp_tok; _temp3970.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3970;}); _temp3969;}); break; case 351: _LL3968: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3972=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3972[ 0]=({ struct Cyc_Exp_tok_struct
_temp3973; _temp3973.tag= Cyc_Exp_tok; _temp3973.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3973;}); _temp3972;}); break; case 352: _LL3971: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3975=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3975[ 0]=({ struct Cyc_Exp_tok_struct
_temp3976; _temp3976.tag= Cyc_Exp_tok; _temp3976.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3977=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3977[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3978; _temp3978.tag= Cyc_Absyn_Malloc_e;
_temp3978.f1= 0; _temp3978.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3978;}); _temp3977;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3976;});
_temp3975;}); break; case 353: _LL3974: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3980=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3980[ 0]=({ struct Cyc_Exp_tok_struct _temp3981; _temp3981.tag= Cyc_Exp_tok;
_temp3981.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3982=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3982[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3983; _temp3983.tag= Cyc_Absyn_Malloc_e;
_temp3983.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp3983.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp3983;}); _temp3982;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3981;}); _temp3980;}); break; case 354: _LL3979: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3985=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3985[ 0]=({ struct Cyc_Primop_tok_struct
_temp3986; _temp3986.tag= Cyc_Primop_tok; _temp3986.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp3986;}); _temp3985;}); break; case 355: _LL3984: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3988=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3988[ 0]=({ struct Cyc_Primop_tok_struct
_temp3989; _temp3989.tag= Cyc_Primop_tok; _temp3989.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp3989;}); _temp3988;}); break; case 356: _LL3987: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3991=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3991[ 0]=({ struct Cyc_Primop_tok_struct
_temp3992; _temp3992.tag= Cyc_Primop_tok; _temp3992.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp3992;}); _temp3991;}); break; case 357: _LL3990: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3994=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3994[ 0]=({ struct Cyc_Primop_tok_struct
_temp3995; _temp3995.tag= Cyc_Primop_tok; _temp3995.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp3995;}); _temp3994;}); break; case 358: _LL3993: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3997=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3997[ 0]=({ struct Cyc_Primop_tok_struct
_temp3998; _temp3998.tag= Cyc_Primop_tok; _temp3998.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp3998;}); _temp3997;}); break; case 359: _LL3996: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4000=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4000[ 0]=({ struct Cyc_Primop_tok_struct
_temp4001; _temp4001.tag= Cyc_Primop_tok; _temp4001.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp4001;}); _temp4000;}); break; case 360: _LL3999: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4003=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4003[ 0]=({ struct Cyc_Primop_tok_struct
_temp4004; _temp4004.tag= Cyc_Primop_tok; _temp4004.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp4004;}); _temp4003;}); break; case 361: _LL4002: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4006=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4006[ 0]=({ struct Cyc_Primop_tok_struct
_temp4007; _temp4007.tag= Cyc_Primop_tok; _temp4007.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp4007;}); _temp4006;}); break; case 362: _LL4005: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4009=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4009[ 0]=({ struct Cyc_Primop_tok_struct
_temp4010; _temp4010.tag= Cyc_Primop_tok; _temp4010.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp4010;}); _temp4009;}); break; case 363: _LL4008: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 364: _LL4011: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4013=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4013[ 0]=({ struct Cyc_Exp_tok_struct
_temp4014; _temp4014.tag= Cyc_Exp_tok; _temp4014.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4014;}); _temp4013;}); break; case 365: _LL4012: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4016=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4016[ 0]=({ struct Cyc_Exp_tok_struct
_temp4017; _temp4017.tag= Cyc_Exp_tok; _temp4017.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4017;}); _temp4016;}); break; case 366: _LL4015: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4019=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4019[ 0]=({ struct Cyc_Exp_tok_struct
_temp4020; _temp4020.tag= Cyc_Exp_tok; _temp4020.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4020;}); _temp4019;}); break; case 367: _LL4018: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4022=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4022[ 0]=({ struct Cyc_Exp_tok_struct
_temp4023; _temp4023.tag= Cyc_Exp_tok; _temp4023.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4024=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4024[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4024;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4023;}); _temp4022;}); break; case 368: _LL4021: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4026=(* q).f1; struct Cyc_List_List* _temp4036; struct Cyc_List_List*
_temp4038; _LL4028: if( _temp4026 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4029;}
else{ goto _LL4030;} _LL4030: if(( unsigned int) _temp4026 > 1u?*(( int*)
_temp4026) == Cyc_Absyn_Rel_n: 0){ _LL4037: _temp4036=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4026)->f1; if( _temp4036 == 0){ goto _LL4031;} else{ goto _LL4032;}} else{
goto _LL4032;} _LL4032: if(( unsigned int) _temp4026 > 1u?*(( int*) _temp4026)
== Cyc_Absyn_Abs_n: 0){ _LL4039: _temp4038=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4026)->f1; if( _temp4038 == 0){ goto _LL4033;} else{ goto _LL4034;}} else{
goto _LL4034;} _LL4034: goto _LL4035; _LL4029: goto _LL4027; _LL4031: goto
_LL4027; _LL4033: goto _LL4027; _LL4035: Cyc_Parse_err(({ char* _temp4040="struct field name is qualified";
struct _tagged_string _temp4041; _temp4041.curr= _temp4040; _temp4041.base=
_temp4040; _temp4041.last_plus_one= _temp4040 + 31; _temp4041;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL4027; _LL4027:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4042=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4042[ 0]=({ struct Cyc_Exp_tok_struct _temp4043; _temp4043.tag= Cyc_Exp_tok;
_temp4043.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4043;}); _temp4042;}); break;} case 369: _LL4025: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4045=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4045[ 0]=({ struct Cyc_Exp_tok_struct
_temp4046; _temp4046.tag= Cyc_Exp_tok; _temp4046.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4047=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4047[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4047;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4046;}); _temp4045;}); break; case 370: _LL4044: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4049=(* q).f1; struct Cyc_List_List* _temp4059; struct Cyc_List_List*
_temp4061; _LL4051: if( _temp4049 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4052;}
else{ goto _LL4053;} _LL4053: if(( unsigned int) _temp4049 > 1u?*(( int*)
_temp4049) == Cyc_Absyn_Rel_n: 0){ _LL4060: _temp4059=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4049)->f1; if( _temp4059 == 0){ goto _LL4054;} else{ goto _LL4055;}} else{
goto _LL4055;} _LL4055: if(( unsigned int) _temp4049 > 1u?*(( int*) _temp4049)
== Cyc_Absyn_Abs_n: 0){ _LL4062: _temp4061=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4049)->f1; if( _temp4061 == 0){ goto _LL4056;} else{ goto _LL4057;}} else{
goto _LL4057;} _LL4057: goto _LL4058; _LL4052: goto _LL4050; _LL4054: goto
_LL4050; _LL4056: goto _LL4050; _LL4058: Cyc_Parse_err(({ char* _temp4063="struct field is qualified with module name";
struct _tagged_string _temp4064; _temp4064.curr= _temp4063; _temp4064.base=
_temp4063; _temp4064.last_plus_one= _temp4063 + 43; _temp4064;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL4050; _LL4050:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4065=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4065[ 0]=({ struct Cyc_Exp_tok_struct _temp4066; _temp4066.tag= Cyc_Exp_tok;
_temp4066.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),(* q).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4066;}); _temp4065;}); break;} case 371: _LL4048: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4068=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4068[ 0]=({ struct Cyc_Exp_tok_struct
_temp4069; _temp4069.tag= Cyc_Exp_tok; _temp4069.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4069;}); _temp4068;}); break; case 372: _LL4067: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4071=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4071[ 0]=({ struct Cyc_Exp_tok_struct
_temp4072; _temp4072.tag= Cyc_Exp_tok; _temp4072.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4072;}); _temp4071;}); break; case 373: _LL4070: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4074=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4074[ 0]=({ struct Cyc_Exp_tok_struct
_temp4075; _temp4075.tag= Cyc_Exp_tok; _temp4075.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4076=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4076[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4077; _temp4077.tag= Cyc_Absyn_CompoundLit_e;
_temp4077.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4077.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4077;}); _temp4076;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4075;}); _temp4074;}); break; case 374: _LL4073: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4079=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4079[ 0]=({ struct Cyc_Exp_tok_struct
_temp4080; _temp4080.tag= Cyc_Exp_tok; _temp4080.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4081=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4081[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4082; _temp4082.tag= Cyc_Absyn_CompoundLit_e;
_temp4082.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4082.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4082;}); _temp4081;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4080;}); _temp4079;}); break; case 375: _LL4078: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4084=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4084[ 0]=({ struct Cyc_Exp_tok_struct
_temp4085; _temp4085.tag= Cyc_Exp_tok; _temp4085.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4086=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4086[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4087; _temp4087.tag= Cyc_Absyn_Fill_e; _temp4087.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4087;});
_temp4086;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4085;});
_temp4084;}); break; case 376: _LL4083: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4089=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4089[ 0]=({ struct Cyc_Exp_tok_struct _temp4090; _temp4090.tag= Cyc_Exp_tok;
_temp4090.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4091=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4091[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4092; _temp4092.tag= Cyc_Absyn_Codegen_e;
_temp4092.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4092;}); _temp4091;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4090;}); _temp4089;}); break; case 377: _LL4088: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4094=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4094[ 0]=({ struct Cyc_Exp_tok_struct
_temp4095; _temp4095.tag= Cyc_Exp_tok; _temp4095.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4096=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4096[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4097; _temp4097.tag= Cyc_Absyn_UnknownId_e;
_temp4097.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4097;}); _temp4096;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4095;}); _temp4094;}); break; case 378: _LL4093: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 379: _LL4098: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4100=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4100[ 0]=({ struct Cyc_Exp_tok_struct
_temp4101; _temp4101.tag= Cyc_Exp_tok; _temp4101.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4101;}); _temp4100;}); break; case 380: _LL4099: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 381: _LL4102: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4104=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4104[ 0]=({ struct Cyc_Exp_tok_struct
_temp4105; _temp4105.tag= Cyc_Exp_tok; _temp4105.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4106=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4106[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4107; _temp4107.tag=
Cyc_Absyn_UnknownId_e; _temp4107.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4107;}); _temp4106;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4105;}); _temp4104;}); break; case 382: _LL4103:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4109=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4109[ 0]=({ struct Cyc_Exp_tok_struct
_temp4110; _temp4110.tag= Cyc_Exp_tok; _temp4110.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4111=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4111[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4112; _temp4112.tag=
Cyc_Absyn_UnknownId_e; _temp4112.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4112;}); _temp4111;}), Cyc_Position_segment_of_abs((*((
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
_temp4110;}); _temp4109;}); break; case 383: _LL4108: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4114=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4114[ 0]=({ struct Cyc_Exp_tok_struct
_temp4115; _temp4115.tag= Cyc_Exp_tok; _temp4115.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4115;}); _temp4114;}); break; case 384: _LL4113: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4117=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4117[ 0]=({ struct Cyc_Exp_tok_struct
_temp4118; _temp4118.tag= Cyc_Exp_tok; _temp4118.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4119=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4119[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4120; _temp4120.tag= Cyc_Absyn_Struct_e;
_temp4120.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4120.f2= 0; _temp4120.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4120.f4=
0; _temp4120;}); _temp4119;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4118;});
_temp4117;}); break; case 385: _LL4116: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4122=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4122[ 0]=({ struct Cyc_Exp_tok_struct _temp4123; _temp4123.tag= Cyc_Exp_tok;
_temp4123.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4123;});
_temp4122;}); break; case 386: _LL4121: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4125=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4125[ 0]=({ struct Cyc_ExpList_tok_struct _temp4126; _temp4126.tag= Cyc_ExpList_tok;
_temp4126.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4126;}); _temp4125;}); break; case 387: _LL4124: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4128=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4128[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4129; _temp4129.tag= Cyc_ExpList_tok; _temp4129.f1=({ struct Cyc_List_List*
_temp4130=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4130->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4130->tl= 0; _temp4130;}); _temp4129;}); _temp4128;});
break; case 388: _LL4127: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4132=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4132[ 0]=({ struct Cyc_ExpList_tok_struct _temp4133; _temp4133.tag= Cyc_ExpList_tok;
_temp4133.f1=({ struct Cyc_List_List* _temp4134=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4134->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4134->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4134;}); _temp4133;}); _temp4132;}); break; case 389: _LL4131: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4136=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4136[ 0]=({ struct Cyc_Exp_tok_struct
_temp4137; _temp4137.tag= Cyc_Exp_tok; _temp4137.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4137;}); _temp4136;}); break; case 390: _LL4135: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4139=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4139[ 0]=({ struct Cyc_Exp_tok_struct
_temp4140; _temp4140.tag= Cyc_Exp_tok; _temp4140.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4140;}); _temp4139;}); break; case 391: _LL4138: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4142=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4142[ 0]=({ struct Cyc_Exp_tok_struct
_temp4143; _temp4143.tag= Cyc_Exp_tok; _temp4143.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4143;}); _temp4142;}); break; case 392: _LL4141: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4145=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4145[ 0]=({ struct Cyc_Exp_tok_struct
_temp4146; _temp4146.tag= Cyc_Exp_tok; _temp4146.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4146;}); _temp4145;}); break; case 393: _LL4144: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4148=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4148[ 0]=({ struct Cyc_QualId_tok_struct
_temp4149; _temp4149.tag= Cyc_QualId_tok; _temp4149.f1=({ struct _tuple1*
_temp4150=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4150->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4152=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4152[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4153; _temp4153.tag= Cyc_Absyn_Rel_n; _temp4153.f1= 0; _temp4153;});
_temp4152;}); _temp4150->f2=({ struct _tagged_string* _temp4151=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4151[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp4151;}); _temp4150;}); _temp4149;}); _temp4148;}); break; case 394: _LL4147:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4154:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
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
yylsp_offset)).text=({ char* _temp4156=""; struct _tagged_string _temp4157;
_temp4157.curr= _temp4156; _temp4157.base= _temp4156; _temp4157.last_plus_one=
_temp4156 + 1; _temp4157;});} else{(*(( struct Cyc_Yyltype*(*)( struct
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
unsigned int _temp4158=( unsigned int)( sze + 15); char* _temp4159=( char*)
GC_malloc_atomic( sizeof( char) * _temp4158); struct _tagged_string _temp4161={
_temp4159, _temp4159, _temp4159 + _temp4158};{ unsigned int _temp4160= _temp4158;
unsigned int i; for( i= 0; i < _temp4160; i ++){ _temp4159[ i]='\000';}};
_temp4161;}); Cyc_String_strcpy( msg,({ char* _temp4162="parse error"; struct
_tagged_string _temp4163; _temp4163.curr= _temp4162; _temp4163.base= _temp4162;
_temp4163.last_plus_one= _temp4162 + 12; _temp4163;})); if( count < 5){ count= 0;
for( x= yyn < 0? - yyn: 0; x < 232u / sizeof( char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4273u, x + yyn)] == x){ Cyc_String_strcat( msg,
count == 0?({ char* _temp4164=", expecting `"; struct _tagged_string _temp4165;
_temp4165.curr= _temp4164; _temp4165.base= _temp4164; _temp4165.last_plus_one=
_temp4164 + 14; _temp4165;}):({ char* _temp4166=" or `"; struct _tagged_string
_temp4167; _temp4167.curr= _temp4166; _temp4167.base= _temp4166; _temp4167.last_plus_one=
_temp4166 + 6; _temp4167;})); Cyc_String_strcat( msg, Cyc_yytname[
_check_known_subscript_notnull( 232u, x)]); Cyc_String_strcat( msg,({ char*
_temp4168="'"; struct _tagged_string _temp4169; _temp4169.curr= _temp4168;
_temp4169.base= _temp4168; _temp4169.last_plus_one= _temp4168 + 2; _temp4169;}));
count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(({ char* _temp4170="parse error";
struct _tagged_string _temp4171; _temp4171.curr= _temp4170; _temp4171.base=
_temp4170; _temp4171.last_plus_one= _temp4170 + 12; _temp4171;}));}} goto
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
void Cyc_yyprint( int i, void* v){ void* _temp4187= v; struct _tuple16*
_temp4207; struct _tuple16 _temp4209; int _temp4210; void* _temp4212; char
_temp4214; short _temp4216; struct _tagged_string _temp4218; struct Cyc_Core_Opt*
_temp4220; struct Cyc_Core_Opt* _temp4222; struct Cyc_Core_Opt _temp4224; struct
_tagged_string* _temp4225; struct _tuple1* _temp4227; struct _tuple1 _temp4229;
struct _tagged_string* _temp4230; void* _temp4232; _LL4189: if( _temp4187 == Cyc_Okay_tok){
goto _LL4190;} else{ goto _LL4191;} _LL4191: if(*(( void**) _temp4187) == Cyc_Int_tok){
_LL4208: _temp4207=(( struct Cyc_Int_tok_struct*) _temp4187)->f1; _temp4209=*
_temp4207; _LL4213: _temp4212= _temp4209.f1; goto _LL4211; _LL4211: _temp4210=
_temp4209.f2; goto _LL4192;} else{ goto _LL4193;} _LL4193: if(*(( void**)
_temp4187) == Cyc_Char_tok){ _LL4215: _temp4214=(( struct Cyc_Char_tok_struct*)
_temp4187)->f1; goto _LL4194;} else{ goto _LL4195;} _LL4195: if(*(( void**)
_temp4187) == Cyc_Short_tok){ _LL4217: _temp4216=(( struct Cyc_Short_tok_struct*)
_temp4187)->f1; goto _LL4196;} else{ goto _LL4197;} _LL4197: if(*(( void**)
_temp4187) == Cyc_String_tok){ _LL4219: _temp4218=(( struct Cyc_String_tok_struct*)
_temp4187)->f1; goto _LL4198;} else{ goto _LL4199;} _LL4199: if(*(( void**)
_temp4187) == Cyc_Stringopt_tok){ _LL4221: _temp4220=(( struct Cyc_Stringopt_tok_struct*)
_temp4187)->f1; if( _temp4220 == 0){ goto _LL4200;} else{ goto _LL4201;}} else{
goto _LL4201;} _LL4201: if(*(( void**) _temp4187) == Cyc_Stringopt_tok){ _LL4223:
_temp4222=(( struct Cyc_Stringopt_tok_struct*) _temp4187)->f1; if( _temp4222 ==
0){ goto _LL4203;} else{ _temp4224=* _temp4222; _LL4226: _temp4225=( struct
_tagged_string*) _temp4224.v; goto _LL4202;}} else{ goto _LL4203;} _LL4203: if(*((
void**) _temp4187) == Cyc_QualId_tok){ _LL4228: _temp4227=(( struct Cyc_QualId_tok_struct*)
_temp4187)->f1; _temp4229=* _temp4227; _LL4233: _temp4232= _temp4229.f1; goto
_LL4231; _LL4231: _temp4230= _temp4229.f2; goto _LL4204;} else{ goto _LL4205;}
_LL4205: goto _LL4206; _LL4190: fprintf( Cyc_Stdio_stderr,"ok"); goto _LL4188;
_LL4192: fprintf( Cyc_Stdio_stderr,"%d", _temp4210); goto _LL4188; _LL4194:
fprintf( Cyc_Stdio_stderr,"%c",( int) _temp4214); goto _LL4188; _LL4196: fprintf(
Cyc_Stdio_stderr,"%ds",( int) _temp4216); goto _LL4188; _LL4198:({ struct
_tagged_string _temp4234= _temp4218; fprintf( Cyc_Stdio_stderr,"\"%.*s\"",
_temp4234.last_plus_one - _temp4234.curr, _temp4234.curr);}); goto _LL4188;
_LL4200: fprintf( Cyc_Stdio_stderr,"null"); goto _LL4188; _LL4202:({ struct
_tagged_string _temp4235=* _temp4225; fprintf( Cyc_Stdio_stderr,"\"%.*s\"",
_temp4235.last_plus_one - _temp4235.curr, _temp4235.curr);}); goto _LL4188;
_LL4204: { struct Cyc_List_List* _temp4236= 0; goto _LL4237; _LL4237:{ void*
_temp4238= _temp4232; struct Cyc_List_List* _temp4246; struct Cyc_List_List*
_temp4248; _LL4240: if(( unsigned int) _temp4238 > 1u?*(( int*) _temp4238) ==
Cyc_Absyn_Rel_n: 0){ _LL4247: _temp4246=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4238)->f1; goto _LL4241;} else{ goto _LL4242;} _LL4242: if(( unsigned int)
_temp4238 > 1u?*(( int*) _temp4238) == Cyc_Absyn_Abs_n: 0){ _LL4249: _temp4248=((
struct Cyc_Absyn_Abs_n_struct*) _temp4238)->f1; goto _LL4243;} else{ goto
_LL4244;} _LL4244: if( _temp4238 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4245;}
else{ goto _LL4239;} _LL4241: _temp4236= _temp4246; goto _LL4239; _LL4243:
_temp4236= _temp4248; goto _LL4239; _LL4245: goto _LL4239; _LL4239:;} for( 0;
_temp4236 != 0; _temp4236=(( struct Cyc_List_List*) _check_null( _temp4236))->tl){({
struct _tagged_string _temp4250=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp4236))->hd); fprintf( Cyc_Stdio_stderr,"%.*s::", _temp4250.last_plus_one
- _temp4250.curr, _temp4250.curr);});}({ struct _tagged_string _temp4251=*
_temp4230; fprintf( Cyc_Stdio_stderr,"%.*s::", _temp4251.last_plus_one -
_temp4251.curr, _temp4251.curr);}); goto _LL4188;} _LL4206: fprintf( Cyc_Stdio_stderr,"?");
goto _LL4188; _LL4188:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4252=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4252->v=(
void*) Cyc_Lexing_from_file( f); _temp4252;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}