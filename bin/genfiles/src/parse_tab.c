#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
unsigned char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef unsigned char* Cyc_caddr_t; typedef
unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t; typedef
unsigned int Cyc_vm_size_t; typedef unsigned char Cyc_int8_t; typedef
unsigned char Cyc_u_int8_t; typedef short Cyc_int16_t; typedef unsigned short
Cyc_u_int16_t; typedef int Cyc_int32_t; typedef unsigned int Cyc_u_int32_t;
typedef long long Cyc_int64_t; typedef unsigned long long Cyc_u_int64_t; typedef
int Cyc_register_t; typedef short Cyc_dev_t; typedef int Cyc_off_t; typedef
unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int Cyc_pid_t;
typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned char* Cyc_addr_t;
typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask;
struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern
struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Stdio___sFILE*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern
unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter( void(* f)(
void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(* f)( void*,
void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_zip(
struct Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
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
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_string Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; typedef
void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_string); extern unsigned char Cyc_Position_Nocontext[ 14u];
extern void Cyc_Position_post_error( struct Cyc_Position_Error*); typedef struct
_tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1*
Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t; typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t; typedef struct Cyc_Absyn_TunionFieldInfo
Cyc_Absyn_tunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void*
Cyc_Absyn_incrementor_t; typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
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
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
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
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
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
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y);
extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t; extern void*
Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b);
extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq,
void* b); extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp* rgn_handle,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
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
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List*); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); extern int Cyc_Absyn_fntype_att( void* a); struct Cyc_PP_Ppstate;
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
f2; } ; static unsigned char _temp3[ 8u]="stdcall"; static unsigned char _temp6[
6u]="cdecl"; static unsigned char _temp9[ 9u]="fastcall"; static unsigned char
_temp12[ 9u]="noreturn"; static unsigned char _temp15[ 6u]="const"; static
unsigned char _temp18[ 8u]="aligned"; static unsigned char _temp21[ 7u]="packed";
static unsigned char _temp24[ 7u]="shared"; static unsigned char _temp27[ 7u]="unused";
static unsigned char _temp30[ 5u]="weak"; static unsigned char _temp33[ 10u]="dllimport";
static unsigned char _temp36[ 10u]="dllexport"; static unsigned char _temp39[ 23u]="no_instrument_function";
static unsigned char _temp42[ 12u]="constructor"; static unsigned char _temp45[
11u]="destructor"; static unsigned char _temp48[ 22u]="no_check_memory_usage";
struct _tuple8 Cyc_Parse_att_map[ 16u]={{{ _temp3, _temp3, _temp3 + 8u},( void*)
0u},{{ _temp6, _temp6, _temp6 + 6u},( void*) 1u},{{ _temp9, _temp9, _temp9 + 9u},(
void*) 2u},{{ _temp12, _temp12, _temp12 + 9u},( void*) 3u},{{ _temp15, _temp15,
_temp15 + 6u},( void*) 4u},{{ _temp18, _temp18, _temp18 + 8u},( void*)& Cyc_Parse_att_aligned},{{
_temp21, _temp21, _temp21 + 7u},( void*) 5u},{{ _temp24, _temp24, _temp24 + 7u},(
void*) 7u},{{ _temp27, _temp27, _temp27 + 7u},( void*) 8u},{{ _temp30, _temp30,
_temp30 + 5u},( void*) 9u},{{ _temp33, _temp33, _temp33 + 10u},( void*) 10u},{{
_temp36, _temp36, _temp36 + 10u},( void*) 11u},{{ _temp39, _temp39, _temp39 + 23u},(
void*) 12u},{{ _temp42, _temp42, _temp42 + 12u},( void*) 13u},{{ _temp45,
_temp45, _temp45 + 11u},( void*) 14u},{{ _temp48, _temp48, _temp48 + 22u},( void*)
15u}}; static void Cyc_Parse_err( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_string msg, struct Cyc_Position_Segment* sg){
Cyc_Parse_err( msg, sg);( void) _throw(( void*) Cyc_Position_Exit);} static void
Cyc_Parse_warn( struct _tagged_string msg, struct Cyc_Position_Segment* sg){({
struct _tagged_string _temp49= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp50= msg; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: %.*s\n",
_temp49.last_plus_one - _temp49.curr, _temp49.curr, _temp50.last_plus_one -
_temp50.curr, _temp50.curr);}); return;} static void* Cyc_Parse_unimp( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp51= msg; xprintf("%.*s unimplemented", _temp51.last_plus_one
- _temp51.curr, _temp51.curr);}), sg);} static void Cyc_Parse_unimp2( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp52= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp53= msg;
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp52.last_plus_one - _temp52.curr, _temp52.curr, _temp53.last_plus_one -
_temp53.curr, _temp53.curr);}); return;} struct _tuple9{ struct _tuple7* f1;
struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct Cyc_Core_Opt*
_temp56; struct _tuple7* _temp58; struct _tuple9 _temp54=* field_info; _LL59:
_temp58= _temp54.f1; goto _LL57; _LL57: _temp56= _temp54.f2; goto _LL55; _LL55:
if((* _temp58).f4 != 0){ Cyc_Parse_err(({ unsigned char* _temp60="bad type params in struct field";
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
unsigned char* _temp78="struct field cannot be qualified with a module name";
struct _tagged_string _temp79; _temp79.curr= _temp78; _temp79.base= _temp78;
_temp79.last_plus_one= _temp78 + 52; _temp79;}), loc); goto _LL65; _LL65:;}{
struct Cyc_List_List* _temp80=(* _temp58).f5; goto _LL81; _LL81: return({ struct
Cyc_Absyn_Structfield* _temp82=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp82->name=(* _temp62).f2; _temp82->tq=(*
_temp58).f2; _temp82->type=( void*)(* _temp58).f3; _temp82->width= _temp56;
_temp82->attributes= _temp80; _temp82;});}}} struct _tuple10{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; struct Cyc_List_List* f4; } ; static
struct _tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc, struct
_tuple10* field){ struct _tuple10 _temp85; struct Cyc_List_List* _temp86; void*
_temp88; struct Cyc_Absyn_Tqual _temp90; struct Cyc_Core_Opt* _temp92; struct
_tuple10* _temp83= field; _temp85=* _temp83; _LL93: _temp92= _temp85.f1; goto
_LL91; _LL91: _temp90= _temp85.f2; goto _LL89; _LL89: _temp88= _temp85.f3; goto
_LL87; _LL87: _temp86= _temp85.f4; goto _LL84; _LL84: { struct Cyc_Core_Opt*
_temp94= 0; goto _LL95; _LL95: if( _temp92 != 0){ _temp94=({ struct Cyc_Core_Opt*
_temp96=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp96->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp92))->v)).f2; _temp96;});{ void* _temp97=(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp92))->v)).f1; struct Cyc_List_List* _temp107; struct Cyc_List_List*
_temp109; _LL99: if(( unsigned int) _temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Rel_n:
0){ _LL108: _temp107=(( struct Cyc_Absyn_Rel_n_struct*) _temp97)->f1; if(
_temp107 == 0){ goto _LL100;} else{ goto _LL101;}} else{ goto _LL101;} _LL101:
if(( unsigned int) _temp97 > 1u?*(( int*) _temp97) == Cyc_Absyn_Abs_n: 0){
_LL110: _temp109=(( struct Cyc_Absyn_Abs_n_struct*) _temp97)->f1; if( _temp109
== 0){ goto _LL102;} else{ goto _LL103;}} else{ goto _LL103;} _LL103: if(
_temp97 ==( void*) Cyc_Absyn_Loc_n){ goto _LL104;} else{ goto _LL105;} _LL105:
goto _LL106; _LL100: goto _LL98; _LL102: goto _LL98; _LL104: goto _LL98; _LL106:
Cyc_Parse_err(({ unsigned char* _temp111="parameter cannot be qualified with a module name";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 49; _temp112;}), loc); goto _LL98; _LL98:;}}
if( _temp86 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ unsigned char* _temp113="parameter should have no type parameters";
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
unsigned char* _temp195="initializers are not allowed in parameter declarations";
struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
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
_LL206: Cyc_Parse_err(({ unsigned char* _temp211="module names not allowed on parameter declarations";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 51; _temp212;}), x->loc); goto _LL198; _LL198:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp159->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp213=*(*
_temp159->name).f2; xprintf("%.*s is not listed as a parameter", _temp213.last_plus_one
- _temp213.curr, _temp213.curr);}), x->loc);} return;} _LL138: decl_kind=({
unsigned char* _temp214="let declaration"; struct _tagged_string _temp215;
_temp215.curr= _temp214; _temp215.base= _temp214; _temp215.last_plus_one=
_temp214 + 16; _temp215;}); goto _LL134; _LL140: decl_kind=({ unsigned char*
_temp216="let declaration"; struct _tagged_string _temp217; _temp217.curr=
_temp216; _temp217.base= _temp216; _temp217.last_plus_one= _temp216 + 16;
_temp217;}); goto _LL134; _LL142: decl_kind=({ unsigned char* _temp218="function declaration";
struct _tagged_string _temp219; _temp219.curr= _temp218; _temp219.base= _temp218;
_temp219.last_plus_one= _temp218 + 21; _temp219;}); goto _LL134; _LL144:
decl_kind=({ unsigned char* _temp220="struct declaration"; struct _tagged_string
_temp221; _temp221.curr= _temp220; _temp221.base= _temp220; _temp221.last_plus_one=
_temp220 + 19; _temp221;}); goto _LL134; _LL146: decl_kind=({ unsigned char*
_temp222="union declaration"; struct _tagged_string _temp223; _temp223.curr=
_temp222; _temp223.base= _temp222; _temp223.last_plus_one= _temp222 + 18;
_temp223;}); goto _LL134; _LL148: decl_kind=({ unsigned char* _temp224="tunion declaration";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 19; _temp225;}); goto _LL134; _LL150:
decl_kind=({ unsigned char* _temp226="typedef"; struct _tagged_string _temp227;
_temp227.curr= _temp226; _temp227.base= _temp226; _temp227.last_plus_one=
_temp226 + 8; _temp227;}); goto _LL134; _LL152: decl_kind=({ unsigned char*
_temp228="enum declaration"; struct _tagged_string _temp229; _temp229.curr=
_temp228; _temp229.base= _temp228; _temp229.last_plus_one= _temp228 + 17;
_temp229;}); goto _LL134; _LL154: decl_kind=({ unsigned char* _temp230="namespace declaration";
struct _tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 22; _temp231;}); goto _LL134; _LL156:
decl_kind=({ unsigned char* _temp232="using declaration"; struct _tagged_string
_temp233; _temp233.curr= _temp232; _temp233.base= _temp232; _temp233.last_plus_one=
_temp232 + 18; _temp233;}); goto _LL134; _LL158: decl_kind=({ unsigned char*
_temp234="extern C declaration"; struct _tagged_string _temp235; _temp235.curr=
_temp234; _temp235.base= _temp234; _temp235.last_plus_one= _temp234 + 21;
_temp235;}); goto _LL134; _LL134:;}(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp236=
decl_kind; xprintf("%.*s appears in parameter type", _temp236.last_plus_one -
_temp236.curr, _temp236.curr);}), x->loc); return;} struct _tuple12{ struct Cyc_List_List*
f1; struct Cyc_Position_Segment* f2; } ; static struct _tuple2* Cyc_Parse_get_param_type(
struct _tuple12* env, struct _tagged_string* x){ struct _tuple12 _temp239;
struct Cyc_Position_Segment* _temp240; struct Cyc_List_List* _temp242; struct
_tuple12* _temp237= env; _temp239=* _temp237; _LL243: _temp242= _temp239.f1;
goto _LL241; _LL241: _temp240= _temp239.f2; goto _LL238; _LL238: if( _temp242 ==
0){ return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp244=* x; xprintf("missing type for parameter %.*s",
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
_LL262: Cyc_Parse_err(({ unsigned char* _temp267="module name not allowed on parameter";
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
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp274="non-variable declaration"; struct _tagged_string
_temp275; _temp275.curr= _temp274; _temp275.base= _temp274; _temp275.last_plus_one=
_temp274 + 25; _temp275;}),(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp242))->hd)->loc); _LL246:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp276= tm; int _temp282; struct Cyc_Position_Segment*
_temp284; struct Cyc_List_List* _temp286; _LL278: if(( unsigned int) _temp276 >
1u?*(( int*) _temp276) == Cyc_Absyn_TypeParams_mod: 0){ _LL287: _temp286=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f1; goto _LL285; _LL285:
_temp284=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f2; goto _LL283;
_LL283: _temp282=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp276)->f3; goto
_LL279;} else{ goto _LL280;} _LL280: goto _LL281; _LL279: return 1; _LL281:
return 0; _LL277:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,({ unsigned char*
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
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp302="expecting a list of type variables, not types";
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
== 0: 0)){ void* _temp314= _temp312; struct Cyc_Core_Opt* _temp320; struct Cyc_Absyn_VarargInfo*
_temp322; int _temp324; struct Cyc_List_List* _temp326; struct Cyc_Position_Segment*
_temp328; struct Cyc_List_List* _temp330; _LL316: if(*(( int*) _temp314) == Cyc_Absyn_WithTypes){
_LL327: _temp326=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f1; goto
_LL325; _LL325: _temp324=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f2;
goto _LL323; _LL323: _temp322=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f3;
goto _LL321; _LL321: _temp320=(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f4;
goto _LL317;} else{ goto _LL318;} _LL318: if(*(( int*) _temp314) == Cyc_Absyn_NoTypes){
_LL331: _temp330=(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f1; goto _LL329;
_LL329: _temp328=(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f2; goto _LL319;}
else{ goto _LL315;} _LL317: Cyc_Parse_warn(({ unsigned char* _temp332="function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 92; _temp333;}), loc); return tms; _LL319:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp330, tds);{ struct _tuple12* _temp335=({ struct _tuple12* _temp334=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp334->f1= tds; _temp334->f2=
loc; _temp334;}); goto _LL336; _LL336: return({ struct Cyc_List_List* _temp337=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp337->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp338=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp338[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp339; _temp339.tag= Cyc_Absyn_Function_mod;
_temp339.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp340=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp340[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp341; _temp341.tag= Cyc_Absyn_WithTypes;
_temp341.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_string*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp335, _temp330); _temp341.f2= 0; _temp341.f3= 0;
_temp341.f4= 0; _temp341;}); _temp340;})); _temp339;}); _temp338;})); _temp337->tl=
0; _temp337;});} _LL315:;} else{ return({ struct Cyc_List_List* _temp342=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp342->hd=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd); _temp342->tl=
Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null( tms))->tl,
tds, loc); _temp342;});} _LL311: return({ struct Cyc_List_List* _temp343=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp343->hd=(
void*)(( void*)(( struct Cyc_List_List*) _check_null( tms))->hd); _temp343->tl=
Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*) _check_null( tms))->tl,
tds, loc); _temp343;}); _LL307:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds !=
0){ d=({ struct Cyc_Parse_Declarator* _temp344=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp344->id= d->id; _temp344->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp344;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp345=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL347: if( _temp345 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL348;} else{ goto _LL349;} _LL349: if(
_temp345 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL350;} else{ goto _LL351;}
_LL351: if( _temp345 ==( void*) Cyc_Parse_Static_sc){ goto _LL352;} else{ goto
_LL353;} _LL353: goto _LL354; _LL348: sc=( void*) Cyc_Absyn_Extern; goto _LL346;
_LL350: sc=( void*) Cyc_Absyn_ExternC; goto _LL346; _LL352: sc=( void*) Cyc_Absyn_Static;
goto _LL346; _LL354: Cyc_Parse_err(({ unsigned char* _temp355="bad storage class on function";
struct _tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 30; _temp356;}), loc); goto _LL346; _LL346:;}}{
struct Cyc_Core_Opt* _temp359; void* _temp361; struct _tuple5 _temp357= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL362: _temp361= _temp357.f1; goto _LL360; _LL360: _temp359=
_temp357.f2; goto _LL358; _LL358: { struct Cyc_List_List* _temp365; struct Cyc_List_List*
_temp367; void* _temp369; struct Cyc_Absyn_Tqual _temp371; struct _tuple6
_temp363= Cyc_Parse_apply_tms( tq, _temp361, atts, d->tms); _LL372: _temp371=
_temp363.f1; goto _LL370; _LL370: _temp369= _temp363.f2; goto _LL368; _LL368:
_temp367= _temp363.f3; goto _LL366; _LL366: _temp365= _temp363.f4; goto _LL364;
_LL364: if( _temp359 != 0){ Cyc_Parse_warn(({ unsigned char* _temp373="nested type declaration within function prototype";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 50; _temp374;}), loc);} if( _temp367 != 0){
Cyc_Parse_warn(({ unsigned char* _temp375="bad type params, ignoring"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 26; _temp376;}), loc);}{ void* _temp377=
_temp369; struct Cyc_Absyn_FnInfo _temp383; struct Cyc_List_List* _temp385;
struct Cyc_Absyn_VarargInfo* _temp387; int _temp389; struct Cyc_List_List*
_temp391; void* _temp393; struct Cyc_Core_Opt* _temp395; struct Cyc_List_List*
_temp397; _LL379: if(( unsigned int) _temp377 > 4u?*(( int*) _temp377) == Cyc_Absyn_FnType:
0){ _LL384: _temp383=(( struct Cyc_Absyn_FnType_struct*) _temp377)->f1; _LL398:
_temp397= _temp383.tvars; goto _LL396; _LL396: _temp395= _temp383.effect; goto
_LL394; _LL394: _temp393=( void*) _temp383.ret_typ; goto _LL392; _LL392:
_temp391= _temp383.args; goto _LL390; _LL390: _temp389= _temp383.c_varargs; goto
_LL388; _LL388: _temp387= _temp383.cyc_varargs; goto _LL386; _LL386: _temp385=
_temp383.attributes; goto _LL380;} else{ goto _LL381;} _LL381: goto _LL382;
_LL380: { struct Cyc_List_List* _temp399=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp391); goto _LL400; _LL400: return({ struct Cyc_Absyn_Fndecl* _temp401=(
struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp401->sc=(
void*) sc; _temp401->name= d->id; _temp401->tvs= _temp397; _temp401->is_inline=
is_inline; _temp401->effect= _temp395; _temp401->ret_type=( void*) _temp393;
_temp401->args= _temp399; _temp401->c_varargs= _temp389; _temp401->cyc_varargs=
_temp387; _temp401->body= body; _temp401->cached_typ= 0; _temp401->param_vardecls=
0; _temp401->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp385, _temp365); _temp401;});}
_LL382: return(( struct Cyc_Absyn_Fndecl*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp402="declarator is not a function prototype";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 39; _temp403;}), loc); _LL378:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(({ unsigned char* _temp404="missing argument variable in function prototype";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 48; _temp405;}), loc); return({ struct
_tuple4* _temp406=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp406->f1=({ struct _tagged_string* _temp407=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp407[ 0]=({ unsigned char*
_temp408="?"; struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base=
_temp408; _temp409.last_plus_one= _temp408 + 2; _temp409;}); _temp407;});
_temp406->f2=(* t).f2; _temp406->f3=(* t).f3; _temp406;});} else{ return({
struct _tuple4* _temp410=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp410->f1=( struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null((* t).f1))->v;
_temp410->f2=(* t).f2; _temp410->f3=(* t).f3; _temp410;});}} static
unsigned char _temp413[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1={ _temp413, _temp413, _temp413 + 52u};
static unsigned char _temp416[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2={ _temp416, _temp416, _temp416 + 63u};
static unsigned char _temp419[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3={ _temp419, _temp419, _temp419 + 50u};
static unsigned char _temp422[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4={ _temp422, _temp422, _temp422 + 60u};
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* declopt= 0; int
seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*) Cyc_Absyn_VoidType;
void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*) _check_null( ts))->tl){
void* _temp423=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd; struct
Cyc_Position_Segment* _temp437; void* _temp439; struct Cyc_Position_Segment*
_temp441; struct Cyc_Position_Segment* _temp443; struct Cyc_Position_Segment*
_temp445; struct Cyc_Position_Segment* _temp447; struct Cyc_Absyn_Decl* _temp449;
_LL425: if(*(( int*) _temp423) == Cyc_Parse_Type_spec){ _LL440: _temp439=( void*)((
struct Cyc_Parse_Type_spec_struct*) _temp423)->f1; goto _LL438; _LL438: _temp437=((
struct Cyc_Parse_Type_spec_struct*) _temp423)->f2; goto _LL426;} else{ goto
_LL427;} _LL427: if(*(( int*) _temp423) == Cyc_Parse_Signed_spec){ _LL442:
_temp441=(( struct Cyc_Parse_Signed_spec_struct*) _temp423)->f1; goto _LL428;}
else{ goto _LL429;} _LL429: if(*(( int*) _temp423) == Cyc_Parse_Unsigned_spec){
_LL444: _temp443=(( struct Cyc_Parse_Unsigned_spec_struct*) _temp423)->f1; goto
_LL430;} else{ goto _LL431;} _LL431: if(*(( int*) _temp423) == Cyc_Parse_Short_spec){
_LL446: _temp445=(( struct Cyc_Parse_Short_spec_struct*) _temp423)->f1; goto
_LL432;} else{ goto _LL433;} _LL433: if(*(( int*) _temp423) == Cyc_Parse_Long_spec){
_LL448: _temp447=(( struct Cyc_Parse_Long_spec_struct*) _temp423)->f1; goto
_LL434;} else{ goto _LL435;} _LL435: if(*(( int*) _temp423) == Cyc_Parse_Decl_spec){
_LL450: _temp449=(( struct Cyc_Parse_Decl_spec_struct*) _temp423)->f1; goto
_LL436;} else{ goto _LL424;} _LL426: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1,
_temp437);} last_loc= _temp437; seen_type= 1; t= _temp439; goto _LL424; _LL428:
if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp441);} if( seen_type){ Cyc_Parse_err(({
unsigned char* _temp451="signed qualifier must come before type"; struct
_tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 39; _temp452;}), _temp441);} last_loc=
_temp441; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL424; _LL430: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp443);} if( seen_type){ Cyc_Parse_err(({
unsigned char* _temp453="signed qualifier must come before type"; struct
_tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 39; _temp454;}), _temp443);} last_loc=
_temp443; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL424; _LL432:
if( seen_size){ Cyc_Parse_err(({ unsigned char* _temp455="integral size may appear only once within a type specifier";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 59; _temp456;}), _temp445);} if( seen_type){
Cyc_Parse_err(({ unsigned char* _temp457="short modifier must come before base type";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 42; _temp458;}), _temp445);} last_loc=
_temp445; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL424; _LL434: if(
seen_type){ Cyc_Parse_err(({ unsigned char* _temp459="long modifier must come before base type";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 41; _temp460;}), _temp447);} if( seen_size){
void* _temp461= sz; _LL463: if( _temp461 ==( void*) Cyc_Absyn_B4){ goto _LL464;}
else{ goto _LL465;} _LL465: goto _LL466; _LL464: sz=( void*) Cyc_Absyn_B8; goto
_LL462; _LL466: Cyc_Parse_err(({ unsigned char* _temp467="extra long in type specifier";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 29; _temp468;}), _temp447); goto _LL462;
_LL462:;} last_loc= _temp447; seen_size= 1; goto _LL424; _LL436: last_loc=
_temp449->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp469=(
void*) _temp449->r; struct Cyc_Absyn_Structdecl* _temp481; struct Cyc_Absyn_Tuniondecl*
_temp483; struct Cyc_Absyn_Uniondecl* _temp485; struct Cyc_Absyn_Enumdecl*
_temp487; _LL471: if(*(( int*) _temp469) == Cyc_Absyn_Struct_d){ _LL482:
_temp481=(( struct Cyc_Absyn_Struct_d_struct*) _temp469)->f1; goto _LL472;}
else{ goto _LL473;} _LL473: if(*(( int*) _temp469) == Cyc_Absyn_Tunion_d){
_LL484: _temp483=(( struct Cyc_Absyn_Tunion_d_struct*) _temp469)->f1; goto
_LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp469) == Cyc_Absyn_Union_d){
_LL486: _temp485=(( struct Cyc_Absyn_Union_d_struct*) _temp469)->f1; goto _LL476;}
else{ goto _LL477;} _LL477: if(*(( int*) _temp469) == Cyc_Absyn_Enum_d){ _LL488:
_temp487=(( struct Cyc_Absyn_Enum_d_struct*) _temp469)->f1; goto _LL478;} else{
goto _LL479;} _LL479: goto _LL480; _LL472: { struct Cyc_List_List* _temp489=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp481->tvs); goto _LL490; _LL490: t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp491=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp491[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp492; _temp492.tag= Cyc_Absyn_StructType;
_temp492.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp481->name))->v); _temp492.f2= _temp489; _temp492.f3= 0;
_temp492;}); _temp491;}); if( _temp481->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp493=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp493->v=( void*) _temp449; _temp493;});} goto _LL470;} _LL474: { struct Cyc_List_List*
_temp494=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp483->tvs);
goto _LL495; _LL495: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp496=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp496[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp497; _temp497.tag= Cyc_Absyn_TunionType;
_temp497.f1=({ struct Cyc_Absyn_TunionInfo _temp498; _temp498.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp499=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp499[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp500; _temp500.tag= Cyc_Absyn_KnownTunion;
_temp500.f1= _temp483; _temp500;}); _temp499;})); _temp498.targs= _temp494;
_temp498.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp498;}); _temp497;});
_temp496;}); if( _temp483->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp501=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp501->v=( void*) _temp449; _temp501;});} goto _LL470;} _LL476: { struct Cyc_List_List*
_temp502=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp485->tvs);
goto _LL503; _LL503: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp504=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp504[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp505; _temp505.tag= Cyc_Absyn_UnionType;
_temp505.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp485->name))->v); _temp505.f2= _temp502; _temp505.f3= 0;
_temp505;}); _temp504;}); if( _temp485->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp506=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp506->v=( void*) _temp449; _temp506;});} goto _LL470;} _LL478: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp507=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp507[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp508; _temp508.tag= Cyc_Absyn_EnumType; _temp508.f1=
_temp487->name; _temp508.f2= 0; _temp508;}); _temp507;}); declopt=({ struct Cyc_Core_Opt*
_temp509=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp509->v=( void*) _temp449; _temp509;}); goto _LL470; _LL480:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp510="bad declaration within type specifier"; struct
_tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 38; _temp511;}), _temp449->loc); goto _LL470;
_LL470:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp449->loc);} goto _LL424;
_LL424:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err(({
unsigned char* _temp512="missing type withing specifier"; struct _tagged_string
_temp513; _temp513.curr= _temp512; _temp513.base= _temp512; _temp513.last_plus_one=
_temp512 + 31; _temp513;}), last_loc);} t=( void*)({ struct Cyc_Absyn_IntType_struct*
_temp514=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp514[ 0]=({ struct Cyc_Absyn_IntType_struct _temp515; _temp515.tag= Cyc_Absyn_IntType;
_temp515.f1=( void*) sgn; _temp515.f2=( void*) sz; _temp515;}); _temp514;});}
else{ if( seen_sign){ void* _temp516= t; void* _temp522; void* _temp524; _LL518:
if(( unsigned int) _temp516 > 4u?*(( int*) _temp516) == Cyc_Absyn_IntType: 0){
_LL525: _temp524=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp516)->f1;
goto _LL523; _LL523: _temp522=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp516)->f2; goto _LL519;} else{ goto _LL520;} _LL520: goto _LL521; _LL519: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp526=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp526[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp527; _temp527.tag= Cyc_Absyn_IntType; _temp527.f1=( void*) sgn; _temp527.f2=(
void*) _temp522; _temp527;}); _temp526;}); goto _LL517; _LL521: Cyc_Parse_err(({
unsigned char* _temp528="sign specification on non-integral type"; struct
_tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 40; _temp529;}), last_loc); goto _LL517;
_LL517:;} if( seen_size){ void* _temp530= t; void* _temp536; void* _temp538;
_LL532: if(( unsigned int) _temp530 > 4u?*(( int*) _temp530) == Cyc_Absyn_IntType:
0){ _LL539: _temp538=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp530)->f1;
goto _LL537; _LL537: _temp536=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp530)->f2; goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL533: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp540=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp540[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp541; _temp541.tag= Cyc_Absyn_IntType; _temp541.f1=( void*) _temp538;
_temp541.f2=( void*) sz; _temp541;}); _temp540;}); goto _LL531; _LL535: Cyc_Parse_err(({
unsigned char* _temp542="size qualifier on non-integral type"; struct
_tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 36; _temp543;}), last_loc); goto _LL531;
_LL531:;}} return({ struct _tuple5 _temp544; _temp544.f1= t; _temp544.f2=
declopt; _temp544;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp545=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL546; _LL546: { struct _tuple1* _temp547= _temp545->id; goto _LL548;
_LL548: { struct Cyc_List_List* _temp551; struct Cyc_List_List* _temp553; void*
_temp555; struct Cyc_Absyn_Tqual _temp557; struct _tuple6 _temp549= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp545->tms); _LL558: _temp557= _temp549.f1; goto _LL556;
_LL556: _temp555= _temp549.f2; goto _LL554; _LL554: _temp553= _temp549.f3; goto
_LL552; _LL552: _temp551= _temp549.f4; goto _LL550; _LL550: return({ struct Cyc_List_List*
_temp559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp559->hd=( void*)({ struct _tuple7* _temp560=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp560->f1= _temp547; _temp560->f2= _temp557;
_temp560->f3= _temp555; _temp560->f4= _temp553; _temp560->f5= _temp551; _temp560;});
_temp559->tl= Cyc_Parse_apply_tmss( _temp557, t,(( struct Cyc_List_List*)
_check_null( ds))->tl, shared_atts); _temp559;});}}}} static struct _tuple6 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp561; _temp561.f1= tq; _temp561.f2=
t; _temp561.f3= 0; _temp561.f4= atts; _temp561;});}{ void* _temp562=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp576;
void* _temp578; int _temp580; struct Cyc_Position_Segment* _temp582; struct Cyc_List_List*
_temp584; struct Cyc_Absyn_Tqual _temp586; void* _temp588; void* _temp590;
struct Cyc_List_List* _temp592; struct Cyc_Position_Segment* _temp594; _LL564:
if( _temp562 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL565;} else{ goto _LL566;}
_LL566: if(( unsigned int) _temp562 > 1u?*(( int*) _temp562) == Cyc_Absyn_ConstArray_mod:
0){ _LL577: _temp576=(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp562)->f1;
goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int) _temp562 > 1u?*((
int*) _temp562) == Cyc_Absyn_Function_mod: 0){ _LL579: _temp578=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp562)->f1; goto _LL569;} else{ goto
_LL570;} _LL570: if(( unsigned int) _temp562 > 1u?*(( int*) _temp562) == Cyc_Absyn_TypeParams_mod:
0){ _LL585: _temp584=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp562)->f1;
goto _LL583; _LL583: _temp582=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp562)->f2; goto _LL581; _LL581: _temp580=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp562)->f3; goto _LL571;} else{ goto _LL572;} _LL572: if(( unsigned int)
_temp562 > 1u?*(( int*) _temp562) == Cyc_Absyn_Pointer_mod: 0){ _LL591: _temp590=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp562)->f1; goto _LL589;
_LL589: _temp588=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp562)->f2;
goto _LL587; _LL587: _temp586=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp562)->f3;
goto _LL573;} else{ goto _LL574;} _LL574: if(( unsigned int) _temp562 > 1u?*((
int*) _temp562) == Cyc_Absyn_Attributes_mod: 0){ _LL595: _temp594=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp562)->f1; goto _LL593; _LL593: _temp592=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp562)->f2; goto _LL575;} else{ goto _LL563;} _LL565: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp596=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp596[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp597; _temp597.tag= Cyc_Absyn_ArrayType;
_temp597.f1=( void*) t; _temp597.f2= tq; _temp597.f3= 0; _temp597;}); _temp596;}),
atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL567: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp598=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp598[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp599; _temp599.tag= Cyc_Absyn_ArrayType;
_temp599.f1=( void*) t; _temp599.f2= tq; _temp599.f3=( struct Cyc_Absyn_Exp*)
_temp576; _temp599;}); _temp598;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL569: { void* _temp600= _temp578; struct Cyc_Core_Opt* _temp606;
struct Cyc_Absyn_VarargInfo* _temp608; int _temp610; struct Cyc_List_List*
_temp612; struct Cyc_Position_Segment* _temp614; struct Cyc_List_List* _temp616;
_LL602: if(*(( int*) _temp600) == Cyc_Absyn_WithTypes){ _LL613: _temp612=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f1; goto _LL611; _LL611: _temp610=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f2; goto _LL609; _LL609: _temp608=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f3; goto _LL607; _LL607: _temp606=((
struct Cyc_Absyn_WithTypes_struct*) _temp600)->f4; goto _LL603;} else{ goto
_LL604;} _LL604: if(*(( int*) _temp600) == Cyc_Absyn_NoTypes){ _LL617: _temp616=((
struct Cyc_Absyn_NoTypes_struct*) _temp600)->f1; goto _LL615; _LL615: _temp614=((
struct Cyc_Absyn_NoTypes_struct*) _temp600)->f2; goto _LL605;} else{ goto _LL601;}
_LL603: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=(( struct Cyc_List_List*) _check_null( as))->tl){ if( Cyc_Absyn_fntype_att((
void*)(( struct Cyc_List_List*) _check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List*
_temp618=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp618->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp618->tl= fn_atts; _temp618;});} else{ new_atts=({ struct Cyc_List_List*
_temp619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp619->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( as))->hd);
_temp619->tl= new_atts; _temp619;});}}} if((( struct Cyc_List_List*) _check_null(
tms))->tl != 0){ void* _temp620=( void*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( tms))->tl))->hd; int _temp626; struct Cyc_Position_Segment*
_temp628; struct Cyc_List_List* _temp630; _LL622: if(( unsigned int) _temp620 >
1u?*(( int*) _temp620) == Cyc_Absyn_TypeParams_mod: 0){ _LL631: _temp630=((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp620)->f1; goto _LL629; _LL629:
_temp628=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp620)->f2; goto _LL627;
_LL627: _temp626=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp620)->f3; goto
_LL623;} else{ goto _LL624;} _LL624: goto _LL625; _LL623: typvars= _temp630; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL621; _LL625: goto _LL621;
_LL621:;} if((((( ! _temp610? _temp608 == 0: 0)? _temp612 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp612))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp612))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp612))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp612= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp612); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp606, t, _temp612,
_temp610, _temp608, fn_atts), new_atts,(( struct Cyc_List_List*) _check_null(
tms))->tl);} _LL605:( void) _throw((( void*(*)( struct _tagged_string msg,
struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp632="function declaration without parameter types";
struct _tagged_string _temp633; _temp633.curr= _temp632; _temp633.base= _temp632;
_temp633.last_plus_one= _temp632 + 45; _temp633;}), _temp614)); _LL601:;} _LL571:
if((( struct Cyc_List_List*) _check_null( tms))->tl == 0){ return({ struct
_tuple6 _temp634; _temp634.f1= tq; _temp634.f2= t; _temp634.f3= _temp584;
_temp634.f4= atts; _temp634;});}( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
unsigned char* _temp635="type parameters must appear before function arguments in declarator";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 68; _temp636;}), _temp582)); _LL573: { void*
_temp637= _temp590; struct Cyc_Absyn_Exp* _temp645; struct Cyc_Absyn_Exp*
_temp647; _LL639: if(( unsigned int) _temp637 > 1u?*(( int*) _temp637) == Cyc_Absyn_NonNullable_ps:
0){ _LL646: _temp645=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp637)->f1;
goto _LL640;} else{ goto _LL641;} _LL641: if(( unsigned int) _temp637 > 1u?*((
int*) _temp637) == Cyc_Absyn_Nullable_ps: 0){ _LL648: _temp647=(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp637)->f1; goto _LL642;} else{ goto _LL643;} _LL643: if( _temp637 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL644;} else{ goto _LL638;} _LL640: return Cyc_Parse_apply_tms(
_temp586, Cyc_Absyn_atb_typ( t, _temp588, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp649=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp649[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp650; _temp650.tag= Cyc_Absyn_Upper_b;
_temp650.f1= _temp645; _temp650;}); _temp649;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL642: return Cyc_Parse_apply_tms( _temp586, Cyc_Absyn_starb_typ(
t, _temp588, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp651=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp651[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp652; _temp652.tag= Cyc_Absyn_Upper_b;
_temp652.f1= _temp647; _temp652;}); _temp651;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL644: return Cyc_Parse_apply_tms( _temp586, Cyc_Absyn_tagged_typ(
t, _temp588, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL638:;}
_LL575: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp592),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL563:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp655; void* _temp657; struct _tuple5 _temp653= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL658: _temp657= _temp653.f1; goto _LL656; _LL656: _temp655=
_temp653.f2; goto _LL654; _LL654: if( _temp655 != 0){ Cyc_Parse_warn(({
unsigned char* _temp659="ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp660; _temp660.curr= _temp659; _temp660.base= _temp659;
_temp660.last_plus_one= _temp659 + 54; _temp660;}), loc);} return _temp657;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp661=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp661[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp662; _temp662.tag= Cyc_Absyn_Decl_s;
_temp662.f1= d; _temp662.f2= s; _temp662;}); _temp661;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(({ unsigned char* _temp663="inline is only allowed on function definitions";
struct _tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 47; _temp664;}), loc);} if( tss == 0){ Cyc_Parse_err(({
unsigned char* _temp665="missing type specifiers in declaration"; struct
_tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 39; _temp666;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp667=( void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v;
_LL669: if( _temp667 ==( void*) Cyc_Parse_Typedef_sc){ goto _LL670;} else{ goto
_LL671;} _LL671: if( _temp667 ==( void*) Cyc_Parse_Extern_sc){ goto _LL672;}
else{ goto _LL673;} _LL673: if( _temp667 ==( void*) Cyc_Parse_ExternC_sc){ goto
_LL674;} else{ goto _LL675;} _LL675: if( _temp667 ==( void*) Cyc_Parse_Static_sc){
goto _LL676;} else{ goto _LL677;} _LL677: if( _temp667 ==( void*) Cyc_Parse_Auto_sc){
goto _LL678;} else{ goto _LL679;} _LL679: if( _temp667 ==( void*) Cyc_Parse_Register_sc){
goto _LL680;} else{ goto _LL681;} _LL681: if( _temp667 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL682;} else{ goto _LL668;} _LL670: istypedef= 1; goto _LL668; _LL672: s=(
void*) Cyc_Absyn_Extern; goto _LL668; _LL674: s=( void*) Cyc_Absyn_ExternC; goto
_LL668; _LL676: s=( void*) Cyc_Absyn_Static; goto _LL668; _LL678: s=( void*) Cyc_Absyn_Public;
goto _LL668; _LL680: s=( void*) Cyc_Absyn_Public; goto _LL668; _LL682: s=( void*)
Cyc_Absyn_Abstract; goto _LL668; _LL668:;}{ struct Cyc_List_List* _temp685;
struct Cyc_List_List* _temp687; struct _tuple0 _temp683=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL688: _temp687= _temp683.f1;
goto _LL686; _LL686: _temp685= _temp683.f2; goto _LL684; _LL684: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp685; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp689= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL690; _LL690: if( _temp687 == 0){ struct Cyc_Core_Opt* _temp693; void*
_temp695; struct _tuple5 _temp691= _temp689; _LL696: _temp695= _temp691.f1; goto
_LL694; _LL694: _temp693= _temp691.f2; goto _LL692; _LL692: if( _temp693 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp693))->v;{ void* _temp697=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp709; struct Cyc_Absyn_Structdecl* _temp711; struct Cyc_Absyn_Uniondecl*
_temp713; struct Cyc_Absyn_Tuniondecl* _temp715; _LL699: if(*(( int*) _temp697)
== Cyc_Absyn_Enum_d){ _LL710: _temp709=(( struct Cyc_Absyn_Enum_d_struct*)
_temp697)->f1; goto _LL700;} else{ goto _LL701;} _LL701: if(*(( int*) _temp697)
== Cyc_Absyn_Struct_d){ _LL712: _temp711=(( struct Cyc_Absyn_Struct_d_struct*)
_temp697)->f1; goto _LL702;} else{ goto _LL703;} _LL703: if(*(( int*) _temp697)
== Cyc_Absyn_Union_d){ _LL714: _temp713=(( struct Cyc_Absyn_Union_d_struct*)
_temp697)->f1; goto _LL704;} else{ goto _LL705;} _LL705: if(*(( int*) _temp697)
== Cyc_Absyn_Tunion_d){ _LL716: _temp715=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp697)->f1; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL700:(
void*)( _temp709->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(({ unsigned char*
_temp717="bad attributes on enum"; struct _tagged_string _temp718; _temp718.curr=
_temp717; _temp718.base= _temp717; _temp718.last_plus_one= _temp717 + 23;
_temp718;}), loc);} goto _LL698; _LL702:( void*)( _temp711->sc=( void*) s);
_temp711->attributes= atts; goto _LL698; _LL704:( void*)( _temp713->sc=( void*)
s); _temp713->attributes= atts; goto _LL698; _LL706:( void*)( _temp715->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp719="bad attributes on tunion";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 25; _temp720;}), loc);} goto _LL698; _LL708:
Cyc_Parse_err(({ unsigned char* _temp721="bad declaration"; struct
_tagged_string _temp722; _temp722.curr= _temp721; _temp722.base= _temp721;
_temp722.last_plus_one= _temp721 + 16; _temp722;}), loc); return 0; _LL698:;}
return({ struct Cyc_List_List* _temp723=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp723->hd=( void*) d; _temp723->tl= 0;
_temp723;});} else{ void* _temp724= _temp695; struct Cyc_Absyn_Structdecl**
_temp738; struct Cyc_List_List* _temp740; struct _tuple1* _temp742; struct Cyc_Absyn_TunionInfo
_temp744; void* _temp746; struct Cyc_List_List* _temp748; void* _temp750; struct
Cyc_Absyn_Tuniondecl* _temp752; struct Cyc_Absyn_TunionInfo _temp754; void*
_temp756; struct Cyc_List_List* _temp758; void* _temp760; struct Cyc_Absyn_UnknownTunionInfo
_temp762; int _temp764; struct _tuple1* _temp766; struct Cyc_Absyn_Uniondecl**
_temp768; struct Cyc_List_List* _temp770; struct _tuple1* _temp772; struct Cyc_Absyn_Enumdecl*
_temp774; struct _tuple1* _temp776; _LL726: if(( unsigned int) _temp724 > 4u?*((
int*) _temp724) == Cyc_Absyn_StructType: 0){ _LL743: _temp742=(( struct Cyc_Absyn_StructType_struct*)
_temp724)->f1; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_StructType_struct*)
_temp724)->f2; goto _LL739; _LL739: _temp738=(( struct Cyc_Absyn_StructType_struct*)
_temp724)->f3; goto _LL727;} else{ goto _LL728;} _LL728: if(( unsigned int)
_temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_TunionType: 0){ _LL745: _temp744=((
struct Cyc_Absyn_TunionType_struct*) _temp724)->f1; _LL751: _temp750=( void*)
_temp744.tunion_info; if(*(( int*) _temp750) == Cyc_Absyn_KnownTunion){ _LL753:
_temp752=(( struct Cyc_Absyn_KnownTunion_struct*) _temp750)->f1; goto _LL749;}
else{ goto _LL730;} _LL749: _temp748= _temp744.targs; goto _LL747; _LL747:
_temp746=( void*) _temp744.rgn; goto _LL729;} else{ goto _LL730;} _LL730: if((
unsigned int) _temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_TunionType: 0){
_LL755: _temp754=(( struct Cyc_Absyn_TunionType_struct*) _temp724)->f1; _LL761:
_temp760=( void*) _temp754.tunion_info; if(*(( int*) _temp760) == Cyc_Absyn_UnknownTunion){
_LL763: _temp762=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp760)->f1;
_LL767: _temp766= _temp762.name; goto _LL765; _LL765: _temp764= _temp762.is_xtunion;
goto _LL759;} else{ goto _LL732;} _LL759: _temp758= _temp754.targs; goto _LL757;
_LL757: _temp756=( void*) _temp754.rgn; goto _LL731;} else{ goto _LL732;} _LL732:
if(( unsigned int) _temp724 > 4u?*(( int*) _temp724) == Cyc_Absyn_UnionType: 0){
_LL773: _temp772=(( struct Cyc_Absyn_UnionType_struct*) _temp724)->f1; goto
_LL771; _LL771: _temp770=(( struct Cyc_Absyn_UnionType_struct*) _temp724)->f2;
goto _LL769; _LL769: _temp768=(( struct Cyc_Absyn_UnionType_struct*) _temp724)->f3;
goto _LL733;} else{ goto _LL734;} _LL734: if(( unsigned int) _temp724 > 4u?*((
int*) _temp724) == Cyc_Absyn_EnumType: 0){ _LL777: _temp776=(( struct Cyc_Absyn_EnumType_struct*)
_temp724)->f1; goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_EnumType_struct*)
_temp724)->f2; goto _LL735;} else{ goto _LL736;} _LL736: goto _LL737; _LL727: {
struct Cyc_List_List* _temp778=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp740);
goto _LL779; _LL779: { struct Cyc_Absyn_Structdecl* _temp782=({ struct Cyc_Absyn_Structdecl*
_temp780=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp780->sc=( void*) s; _temp780->name=({ struct Cyc_Core_Opt* _temp781=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp781->v=(
void*)(( struct _tuple1*) _check_null( _temp742)); _temp781;}); _temp780->tvs=
_temp778; _temp780->fields= 0; _temp780->attributes= 0; _temp780;}); goto _LL783;
_LL783: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp784="bad attributes on struct";
struct _tagged_string _temp785; _temp785.curr= _temp784; _temp785.base= _temp784;
_temp785.last_plus_one= _temp784 + 25; _temp785;}), loc);} return({ struct Cyc_List_List*
_temp786=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp786->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp787=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp787[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp788; _temp788.tag= Cyc_Absyn_Struct_d;
_temp788.f1= _temp782; _temp788;}); _temp787;}), loc); _temp786->tl= 0; _temp786;});}}
_LL729: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp789="bad attributes on tunion";
struct _tagged_string _temp790; _temp790.curr= _temp789; _temp790.base= _temp789;
_temp790.last_plus_one= _temp789 + 25; _temp790;}), loc);} return({ struct Cyc_List_List*
_temp791=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp791->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp792=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp792[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp793; _temp793.tag= Cyc_Absyn_Tunion_d;
_temp793.f1= _temp752; _temp793;}); _temp792;}), loc); _temp791->tl= 0; _temp791;});
_LL731: { struct Cyc_List_List* _temp794=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp758);
goto _LL795; _LL795: { struct Cyc_Absyn_Decl* _temp796= Cyc_Absyn_tunion_decl( s,
_temp766, _temp794, 0, _temp764, loc); goto _LL797; _LL797: if( atts != 0){ Cyc_Parse_err(({
unsigned char* _temp798="bad attributes on tunion"; struct _tagged_string
_temp799; _temp799.curr= _temp798; _temp799.base= _temp798; _temp799.last_plus_one=
_temp798 + 25; _temp799;}), loc);} return({ struct Cyc_List_List* _temp800=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp800->hd=(
void*) _temp796; _temp800->tl= 0; _temp800;});}} _LL733: { struct Cyc_List_List*
_temp801=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp770); goto _LL802; _LL802: { struct Cyc_Absyn_Uniondecl*
_temp805=({ struct Cyc_Absyn_Uniondecl* _temp803=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp803->sc=( void*) s;
_temp803->name=({ struct Cyc_Core_Opt* _temp804=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp804->v=( void*)(( struct _tuple1*)
_check_null( _temp772)); _temp804;}); _temp803->tvs= _temp801; _temp803->fields=
0; _temp803->attributes= 0; _temp803;}); goto _LL806; _LL806: if( atts != 0){
Cyc_Parse_err(({ unsigned char* _temp807="bad attributes on union"; struct
_tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 24; _temp808;}), loc);} return({ struct Cyc_List_List*
_temp809=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp809->hd=( void*)({ struct Cyc_Absyn_Decl* _temp810=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp810->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp811=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp811[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp812; _temp812.tag= Cyc_Absyn_Union_d; _temp812.f1= _temp805; _temp812;});
_temp811;})); _temp810->loc= loc; _temp810;}); _temp809->tl= 0; _temp809;});}}
_LL735: { struct Cyc_Absyn_Enumdecl* _temp814=({ struct Cyc_Absyn_Enumdecl*
_temp813=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp813->sc=( void*) s; _temp813->name= _temp776; _temp813->fields= 0; _temp813;});
goto _LL815; _LL815: if( atts != 0){ Cyc_Parse_err(({ unsigned char* _temp816="bad attributes on enum";
struct _tagged_string _temp817; _temp817.curr= _temp816; _temp817.base= _temp816;
_temp817.last_plus_one= _temp816 + 23; _temp817;}), loc);} return({ struct Cyc_List_List*
_temp818=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp818->hd=( void*)({ struct Cyc_Absyn_Decl* _temp819=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp819->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp820=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp820[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp821; _temp821.tag= Cyc_Absyn_Enum_d; _temp821.f1= _temp814; _temp821;});
_temp820;})); _temp819->loc= loc; _temp819;}); _temp818->tl= 0; _temp818;});}
_LL737: Cyc_Parse_err(({ unsigned char* _temp822="missing declarator"; struct
_tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 19; _temp823;}), loc); return 0; _LL725:;}}
else{ void* t= _temp689.f1; struct Cyc_List_List* _temp824= Cyc_Parse_apply_tmss(
tq, t, _temp687, atts); goto _LL825; _LL825: if( istypedef){ if( ! exps_empty){
Cyc_Parse_err(({ unsigned char* _temp826="initializer in typedef declaration";
struct _tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 35; _temp827;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp824); if( _temp689.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp689.f2))->v;{ void* _temp828=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp840; struct Cyc_Absyn_Tuniondecl* _temp842; struct Cyc_Absyn_Uniondecl*
_temp844; struct Cyc_Absyn_Enumdecl* _temp846; _LL830: if(*(( int*) _temp828) ==
Cyc_Absyn_Struct_d){ _LL841: _temp840=(( struct Cyc_Absyn_Struct_d_struct*)
_temp828)->f1; goto _LL831;} else{ goto _LL832;} _LL832: if(*(( int*) _temp828)
== Cyc_Absyn_Tunion_d){ _LL843: _temp842=(( struct Cyc_Absyn_Tunion_d_struct*)
_temp828)->f1; goto _LL833;} else{ goto _LL834;} _LL834: if(*(( int*) _temp828)
== Cyc_Absyn_Union_d){ _LL845: _temp844=(( struct Cyc_Absyn_Union_d_struct*)
_temp828)->f1; goto _LL835;} else{ goto _LL836;} _LL836: if(*(( int*) _temp828)
== Cyc_Absyn_Enum_d){ _LL847: _temp846=(( struct Cyc_Absyn_Enum_d_struct*)
_temp828)->f1; goto _LL837;} else{ goto _LL838;} _LL838: goto _LL839; _LL831:(
void*)( _temp840->sc=( void*) s); _temp840->attributes= atts; atts= 0; goto
_LL829; _LL833:( void*)( _temp842->sc=( void*) s); goto _LL829; _LL835:( void*)(
_temp844->sc=( void*) s); goto _LL829; _LL837:( void*)( _temp846->sc=( void*) s);
goto _LL829; _LL839: Cyc_Parse_err(({ unsigned char* _temp848="declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 70; _temp849;}), loc); goto _LL829; _LL829:;}
decls=({ struct Cyc_List_List* _temp850=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp850->hd=( void*) d; _temp850->tl= decls;
_temp850;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp851=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s in typedef", _temp851.last_plus_one - _temp851.curr,
_temp851.curr);}), loc);} return decls;}} else{ if( _temp689.f2 != 0){ Cyc_Parse_unimp2(({
unsigned char* _temp852="nested type declaration within declarator"; struct
_tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 42; _temp853;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp854= _temp824; goto _LL855; _LL855: for( 0;
_temp854 != 0;( _temp854=(( struct Cyc_List_List*) _check_null( _temp854))->tl,
_temp685=(( struct Cyc_List_List*) _check_null( _temp685))->tl)){ struct _tuple7
_temp858; struct Cyc_List_List* _temp859; struct Cyc_List_List* _temp861; void*
_temp863; struct Cyc_Absyn_Tqual _temp865; struct _tuple1* _temp867; struct
_tuple7* _temp856=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp854))->hd; _temp858=* _temp856; _LL868: _temp867= _temp858.f1; goto _LL866;
_LL866: _temp865= _temp858.f2; goto _LL864; _LL864: _temp863= _temp858.f3; goto
_LL862; _LL862: _temp861= _temp858.f4; goto _LL860; _LL860: _temp859= _temp858.f5;
goto _LL857; _LL857: if( _temp861 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp869="bad type params, ignoring"; struct _tagged_string _temp870; _temp870.curr=
_temp869; _temp870.base= _temp869; _temp870.last_plus_one= _temp869 + 26;
_temp870;}), loc);} if( _temp685 == 0){(( void(*)( struct _tagged_string msg,
struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ unsigned char* _temp871="unexpected null in parse!";
struct _tagged_string _temp872; _temp872.curr= _temp871; _temp872.base= _temp871;
_temp872.last_plus_one= _temp871 + 26; _temp872;}), loc);}{ struct Cyc_Absyn_Exp*
_temp873=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp685))->hd;
goto _LL874; _LL874: { struct Cyc_Absyn_Vardecl* _temp875= Cyc_Absyn_new_vardecl(
_temp867, _temp863, _temp873); goto _LL876; _LL876: _temp875->tq= _temp865;(
void*)( _temp875->sc=( void*) s); _temp875->attributes= _temp859;{ struct Cyc_Absyn_Decl*
_temp880=({ struct Cyc_Absyn_Decl* _temp877=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp877->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp878=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp878[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp879; _temp879.tag= Cyc_Absyn_Var_d;
_temp879.f1= _temp875; _temp879;}); _temp878;})); _temp877->loc= loc; _temp877;});
goto _LL881; _LL881: decls=({ struct Cyc_List_List* _temp882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp882->hd=( void*) _temp880;
_temp882->tl= decls; _temp882;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp883= s; xprintf("bad kind: %.*s",
_temp883.last_plus_one - _temp883.curr, _temp883.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( unsigned char),
0)){ case 'A': _LL884: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL885:
return( void*) Cyc_Absyn_MemKind; case 'B': _LL886: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL887: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL888: return(
void*) Cyc_Absyn_EffKind; default: _LL889: Cyc_Parse_err(({ struct
_tagged_string _temp891= s; xprintf("bad kind: %.*s", _temp891.last_plus_one -
_temp891.curr, _temp891.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp892=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp892->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp893=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp893[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp894; _temp894.tag= Cyc_Absyn_Attributes_mod;
_temp894.f1= loc; _temp894.f2= atts; _temp894;}); _temp893;})); _temp892->tl=
tms; _temp892;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp895= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp895.last_plus_one - _temp895.curr, _temp895.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp896=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp896[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp897; _temp897.tag= Cyc_Absyn_Typedef_d; _temp897.f1=({
struct Cyc_Absyn_Typedefdecl* _temp898=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp898->name= x; _temp898->tvs=(*
t).f4; _temp898->defn=( void*)(* t).f3; _temp898;}); _temp897;}); _temp896;}),
loc);} unsigned char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ unsigned char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; unsigned char Cyc_AttributeList_tok[ 22u]="\000\000\000\000AttributeList_tok";
struct Cyc_AttributeList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Attribute_tok[ 18u]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char
Cyc_BlockItem_tok[ 18u]="\000\000\000\000BlockItem_tok"; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Bool_tok[ 13u]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; unsigned char Cyc_Char_tok[
13u]="\000\000\000\000Char_tok"; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; unsigned char Cyc_DeclList_tok[ 17u]="\000\000\000\000DeclList_tok";
struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_DeclSpec_tok[ 17u]="\000\000\000\000DeclSpec_tok"; struct
Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Parse_Declaration_spec*
f1; } ; unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]="\000\000\000\000DeclaratorExpoptList_tok";
struct Cyc_DeclaratorExpoptList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_DeclaratorExpopt_tok[ 25u]="\000\000\000\000DeclaratorExpopt_tok";
struct _tuple13{ struct Cyc_Parse_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple13* f1;
} ; unsigned char Cyc_Declarator_tok[ 19u]="\000\000\000\000Declarator_tok";
struct Cyc_Declarator_tok_struct{ unsigned char* tag; struct Cyc_Parse_Declarator*
f1; } ; unsigned char Cyc_DesignatorList_tok[ 23u]="\000\000\000\000DesignatorList_tok";
struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Designator_tok[ 19u]="\000\000\000\000Designator_tok";
struct Cyc_Designator_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_EnumfieldList_tok[ 22u]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Enumfield_tok[ 18u]="\000\000\000\000Enumfield_tok";
struct Cyc_Enumfield_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Enumfield*
f1; } ; unsigned char Cyc_ExpList_tok[ 16u]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; } ;
unsigned char Cyc_Exp_tok[ 12u]="\000\000\000\000Exp_tok"; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; unsigned char Cyc_FieldPatternList_tok[
25u]="\000\000\000\000FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_FieldPattern_tok[
21u]="\000\000\000\000FieldPattern_tok"; struct _tuple14{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple14* f1; } ; unsigned char Cyc_FnDecl_tok[ 15u]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl* f1; }
; unsigned char Cyc_IdList_tok[ 15u]="\000\000\000\000IdList_tok"; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_InitDeclList_tok[
21u]="\000\000\000\000InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_InitDecl_tok[
17u]="\000\000\000\000InitDecl_tok"; struct _tuple15{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple15* f1; } ; unsigned char Cyc_InitializerList_tok[ 24u]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok"; struct
_tuple16{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{ unsigned char* tag;
struct _tuple16* f1; } ; unsigned char Cyc_Kind_tok[ 13u]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_Okay_tok[
13u]="\000\000\000\000Okay_tok"; unsigned char Cyc_ParamDeclListBool_tok[ 26u]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple17{ struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo*
f3; struct Cyc_Core_Opt* f4; } ; struct Cyc_ParamDeclListBool_tok_struct{
unsigned char* tag; struct _tuple17* f1; } ; unsigned char Cyc_ParamDeclList_tok[
22u]="\000\000\000\000ParamDeclList_tok"; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_ParamDecl_tok[
18u]="\000\000\000\000ParamDecl_tok"; struct Cyc_ParamDecl_tok_struct{
unsigned char* tag; struct _tuple2* f1; } ; unsigned char Cyc_PatternList_tok[
20u]="\000\000\000\000PatternList_tok"; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_Pattern_tok[
16u]="\000\000\000\000Pattern_tok"; struct Cyc_Pattern_tok_struct{ unsigned char*
tag; struct Cyc_Absyn_Pat* f1; } ; unsigned char Cyc_Pointer_Sort_tok[ 21u]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_Primop_tok[ 15u]="\000\000\000\000Primop_tok"; struct Cyc_Primop_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Primopopt_tok[ 18u]="\000\000\000\000Primopopt_tok";
struct Cyc_Primopopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; unsigned char Cyc_QualId_tok[ 15u]="\000\000\000\000QualId_tok"; struct Cyc_QualId_tok_struct{
unsigned char* tag; struct _tuple1* f1; } ; unsigned char Cyc_QualSpecList_tok[
21u]="\000\000\000\000QualSpecList_tok"; struct _tuple18{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple18* f1; } ; unsigned char Cyc_Rgn_tok[ 12u]="\000\000\000\000Rgn_tok";
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; unsigned char Cyc_Scope_tok[
14u]="\000\000\000\000Scope_tok"; struct Cyc_Scope_tok_struct{ unsigned char*
tag; void* f1; } ; unsigned char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; unsigned char Cyc_Stmt_tok[
13u]="\000\000\000\000Stmt_tok"; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; unsigned char Cyc_StorageClass_tok[ 21u]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ;
unsigned char Cyc_String_tok[ 15u]="\000\000\000\000String_tok"; struct Cyc_String_tok_struct{
unsigned char* tag; struct _tagged_string f1; } ; unsigned char Cyc_Stringopt_tok[
18u]="\000\000\000\000Stringopt_tok"; struct Cyc_Stringopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; unsigned char Cyc_StructFieldDeclListList_tok[
32u]="\000\000\000\000StructFieldDeclListList_tok"; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_StructFieldDeclList_tok[
28u]="\000\000\000\000StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_StructOrUnion_tok[
22u]="\000\000\000\000StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_SwitchClauseList_tok[ 25u]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TunionFieldList_tok[ 24u]="\000\000\000\000TunionFieldList_tok";
struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TunionField_tok[ 20u]="\000\000\000\000TunionField_tok";
struct Cyc_TunionField_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; unsigned char Cyc_TypeList_tok[ 17u]="\000\000\000\000TypeList_tok";
struct Cyc_TypeList_tok_struct{ unsigned char* tag; struct Cyc_List_List* f1; }
; unsigned char Cyc_TypeModifierList_tok[ 25u]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; unsigned char Cyc_TypeOpt_tok[ 16u]="\000\000\000\000TypeOpt_tok";
struct Cyc_TypeOpt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; } ;
unsigned char Cyc_TypeQual_tok[ 17u]="\000\000\000\000TypeQual_tok"; struct Cyc_TypeQual_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tqual f1; } ; unsigned char Cyc_TypeSpecifier_tok[
22u]="\000\000\000\000TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; unsigned char Cyc_Type_tok[ 13u]="\000\000\000\000Type_tok";
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; static
unsigned char _temp902[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={
Cyc_Core_Failure,{ _temp902, _temp902, _temp902 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok(
void* yy1){ void* _temp903= yy1; struct _tuple16* _temp909; _LL905: if(*(( void**)
_temp903) == Cyc_Int_tok){ _LL910: _temp909=(( struct Cyc_Int_tok_struct*)
_temp903)->f1; goto _LL906;} else{ goto _LL907;} _LL907: goto _LL908; _LL906:
return _temp909; _LL908:( void) _throw(( void*)& Cyc_yyfail_Int_tok); _LL904:;}
static unsigned char _temp914[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,{ _temp914, _temp914, _temp914 + 11u}};
struct _tagged_string Cyc_yyget_String_tok( void* yy1){ void* _temp915= yy1;
struct _tagged_string _temp921; _LL917: if(*(( void**) _temp915) == Cyc_String_tok){
_LL922: _temp921=(( struct Cyc_String_tok_struct*) _temp915)->f1; goto _LL918;}
else{ goto _LL919;} _LL919: goto _LL920; _LL918: return _temp921; _LL920:( void)
_throw(( void*)& Cyc_yyfail_String_tok); _LL916:;} static unsigned char _temp926[
9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={ Cyc_Core_Failure,{
_temp926, _temp926, _temp926 + 9u}}; unsigned char Cyc_yyget_Char_tok( void* yy1){
void* _temp927= yy1; unsigned char _temp933; _LL929: if(*(( void**) _temp927) ==
Cyc_Char_tok){ _LL934: _temp933=(( struct Cyc_Char_tok_struct*) _temp927)->f1;
goto _LL930;} else{ goto _LL931;} _LL931: goto _LL932; _LL930: return _temp933;
_LL932:( void) _throw(( void*)& Cyc_yyfail_Char_tok); _LL928:;} static
unsigned char _temp938[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,{ _temp938, _temp938, _temp938 +
17u}}; void* Cyc_yyget_Pointer_Sort_tok( void* yy1){ void* _temp939= yy1; void*
_temp945; _LL941: if(*(( void**) _temp939) == Cyc_Pointer_Sort_tok){ _LL946:
_temp945=( void*)(( struct Cyc_Pointer_Sort_tok_struct*) _temp939)->f1; goto
_LL942;} else{ goto _LL943;} _LL943: goto _LL944; _LL942: return _temp945;
_LL944:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok); _LL940:;} static
unsigned char _temp950[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Exp_tok={
Cyc_Core_Failure,{ _temp950, _temp950, _temp950 + 8u}}; struct Cyc_Absyn_Exp*
Cyc_yyget_Exp_tok( void* yy1){ void* _temp951= yy1; struct Cyc_Absyn_Exp*
_temp957; _LL953: if(*(( void**) _temp951) == Cyc_Exp_tok){ _LL958: _temp957=((
struct Cyc_Exp_tok_struct*) _temp951)->f1; goto _LL954;} else{ goto _LL955;}
_LL955: goto _LL956; _LL954: return _temp957; _LL956:( void) _throw(( void*)&
Cyc_yyfail_Exp_tok); _LL952:;} static unsigned char _temp962[ 12u]="ExpList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,{
_temp962, _temp962, _temp962 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok(
void* yy1){ void* _temp963= yy1; struct Cyc_List_List* _temp969; _LL965: if(*((
void**) _temp963) == Cyc_ExpList_tok){ _LL970: _temp969=(( struct Cyc_ExpList_tok_struct*)
_temp963)->f1; goto _LL966;} else{ goto _LL967;} _LL967: goto _LL968; _LL966:
return _temp969; _LL968:( void) _throw(( void*)& Cyc_yyfail_ExpList_tok); _LL964:;}
static unsigned char _temp974[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,{ _temp974, _temp974, _temp974
+ 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok( void* yy1){ void*
_temp975= yy1; struct Cyc_List_List* _temp981; _LL977: if(*(( void**) _temp975)
== Cyc_InitializerList_tok){ _LL982: _temp981=(( struct Cyc_InitializerList_tok_struct*)
_temp975)->f1; goto _LL978;} else{ goto _LL979;} _LL979: goto _LL980; _LL978:
return _temp981; _LL980:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL976:;} static unsigned char _temp986[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,{ _temp986, _temp986, _temp986 + 11u}};
void* Cyc_yyget_Primop_tok( void* yy1){ void* _temp987= yy1; void* _temp993;
_LL989: if(*(( void**) _temp987) == Cyc_Primop_tok){ _LL994: _temp993=( void*)((
struct Cyc_Primop_tok_struct*) _temp987)->f1; goto _LL990;} else{ goto _LL991;}
_LL991: goto _LL992; _LL990: return _temp993; _LL992:( void) _throw(( void*)&
Cyc_yyfail_Primop_tok); _LL988:;} static unsigned char _temp998[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,{
_temp998, _temp998, _temp998 + 14u}}; struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok(
void* yy1){ void* _temp999= yy1; struct Cyc_Core_Opt* _temp1005; _LL1001: if(*((
void**) _temp999) == Cyc_Primopopt_tok){ _LL1006: _temp1005=(( struct Cyc_Primopopt_tok_struct*)
_temp999)->f1; goto _LL1002;} else{ goto _LL1003;} _LL1003: goto _LL1004;
_LL1002: return _temp1005; _LL1004:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL1000:;} static unsigned char _temp1010[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,{ _temp1010, _temp1010, _temp1010 + 11u}};
struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){ void* _temp1011= yy1; struct
_tuple1* _temp1017; _LL1013: if(*(( void**) _temp1011) == Cyc_QualId_tok){
_LL1018: _temp1017=(( struct Cyc_QualId_tok_struct*) _temp1011)->f1; goto
_LL1014;} else{ goto _LL1015;} _LL1015: goto _LL1016; _LL1014: return _temp1017;
_LL1016:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1012:;} static
unsigned char _temp1022[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,{ _temp1022, _temp1022, _temp1022 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1023= yy1;
struct Cyc_Absyn_Stmt* _temp1029; _LL1025: if(*(( void**) _temp1023) == Cyc_Stmt_tok){
_LL1030: _temp1029=(( struct Cyc_Stmt_tok_struct*) _temp1023)->f1; goto _LL1026;}
else{ goto _LL1027;} _LL1027: goto _LL1028; _LL1026: return _temp1029; _LL1028:(
void) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1024:;} static unsigned char
_temp1034[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure,{ _temp1034, _temp1034, _temp1034 + 14u}}; void* Cyc_yyget_BlockItem_tok(
void* yy1){ void* _temp1035= yy1; void* _temp1041; _LL1037: if(*(( void**)
_temp1035) == Cyc_BlockItem_tok){ _LL1042: _temp1041=( void*)(( struct Cyc_BlockItem_tok_struct*)
_temp1035)->f1; goto _LL1038;} else{ goto _LL1039;} _LL1039: goto _LL1040;
_LL1038: return _temp1041; _LL1040:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok);
_LL1036:;} static unsigned char _temp1046[ 21u]="SwitchClauseList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,{
_temp1046, _temp1046, _temp1046 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1047= yy1; struct Cyc_List_List* _temp1053; _LL1049: if(*((
void**) _temp1047) == Cyc_SwitchClauseList_tok){ _LL1054: _temp1053=(( struct
Cyc_SwitchClauseList_tok_struct*) _temp1047)->f1; goto _LL1050;} else{ goto
_LL1051;} _LL1051: goto _LL1052; _LL1050: return _temp1053; _LL1052:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1048:;} static
unsigned char _temp1058[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure,{ _temp1058, _temp1058, _temp1058 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1059= yy1;
struct Cyc_Absyn_Pat* _temp1065; _LL1061: if(*(( void**) _temp1059) == Cyc_Pattern_tok){
_LL1066: _temp1065=(( struct Cyc_Pattern_tok_struct*) _temp1059)->f1; goto
_LL1062;} else{ goto _LL1063;} _LL1063: goto _LL1064; _LL1062: return _temp1065;
_LL1064:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok); _LL1060:;} static
unsigned char _temp1070[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,{ _temp1070, _temp1070, _temp1070
+ 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok( void* yy1){ void*
_temp1071= yy1; struct Cyc_List_List* _temp1077; _LL1073: if(*(( void**)
_temp1071) == Cyc_PatternList_tok){ _LL1078: _temp1077=(( struct Cyc_PatternList_tok_struct*)
_temp1071)->f1; goto _LL1074;} else{ goto _LL1075;} _LL1075: goto _LL1076;
_LL1074: return _temp1077; _LL1076:( void) _throw(( void*)& Cyc_yyfail_PatternList_tok);
_LL1072:;} static unsigned char _temp1082[ 17u]="FieldPattern_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure,{
_temp1082, _temp1082, _temp1082 + 17u}}; struct _tuple14* Cyc_yyget_FieldPattern_tok(
void* yy1){ void* _temp1083= yy1; struct _tuple14* _temp1089; _LL1085: if(*((
void**) _temp1083) == Cyc_FieldPattern_tok){ _LL1090: _temp1089=(( struct Cyc_FieldPattern_tok_struct*)
_temp1083)->f1; goto _LL1086;} else{ goto _LL1087;} _LL1087: goto _LL1088;
_LL1086: return _temp1089; _LL1088:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1084:;} static unsigned char _temp1094[ 21u]="FieldPatternList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,{
_temp1094, _temp1094, _temp1094 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1095= yy1; struct Cyc_List_List* _temp1101; _LL1097: if(*((
void**) _temp1095) == Cyc_FieldPatternList_tok){ _LL1102: _temp1101=(( struct
Cyc_FieldPatternList_tok_struct*) _temp1095)->f1; goto _LL1098;} else{ goto
_LL1099;} _LL1099: goto _LL1100; _LL1098: return _temp1101; _LL1100:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1096:;} static
unsigned char _temp1106[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure,{ _temp1106, _temp1106, _temp1106 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1107= yy1;
struct Cyc_Absyn_Fndecl* _temp1113; _LL1109: if(*(( void**) _temp1107) == Cyc_FnDecl_tok){
_LL1114: _temp1113=(( struct Cyc_FnDecl_tok_struct*) _temp1107)->f1; goto
_LL1110;} else{ goto _LL1111;} _LL1111: goto _LL1112; _LL1110: return _temp1113;
_LL1112:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok); _LL1108:;} static
unsigned char _temp1118[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,{ _temp1118, _temp1118, _temp1118 +
13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void* yy1){ void* _temp1119=
yy1; struct Cyc_List_List* _temp1125; _LL1121: if(*(( void**) _temp1119) == Cyc_DeclList_tok){
_LL1126: _temp1125=(( struct Cyc_DeclList_tok_struct*) _temp1119)->f1; goto
_LL1122;} else{ goto _LL1123;} _LL1123: goto _LL1124; _LL1122: return _temp1125;
_LL1124:( void) _throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1120:;} static
unsigned char _temp1130[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,{ _temp1130, _temp1130, _temp1130 +
13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok( void* yy1){
void* _temp1131= yy1; struct Cyc_Parse_Declaration_spec* _temp1137; _LL1133: if(*((
void**) _temp1131) == Cyc_DeclSpec_tok){ _LL1138: _temp1137=(( struct Cyc_DeclSpec_tok_struct*)
_temp1131)->f1; goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136;
_LL1134: return _temp1137; _LL1136:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1132:;} static unsigned char _temp1142[ 13u]="InitDecl_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,{ _temp1142,
_temp1142, _temp1142 + 13u}}; struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1143= yy1; struct _tuple15* _temp1149; _LL1145: if(*(( void**)
_temp1143) == Cyc_InitDecl_tok){ _LL1150: _temp1149=(( struct Cyc_InitDecl_tok_struct*)
_temp1143)->f1; goto _LL1146;} else{ goto _LL1147;} _LL1147: goto _LL1148;
_LL1146: return _temp1149; _LL1148:( void) _throw(( void*)& Cyc_yyfail_InitDecl_tok);
_LL1144:;} static unsigned char _temp1154[ 17u]="InitDeclList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,{
_temp1154, _temp1154, _temp1154 + 17u}}; struct Cyc_List_List* Cyc_yyget_InitDeclList_tok(
void* yy1){ void* _temp1155= yy1; struct Cyc_List_List* _temp1161; _LL1157: if(*((
void**) _temp1155) == Cyc_InitDeclList_tok){ _LL1162: _temp1161=(( struct Cyc_InitDeclList_tok_struct*)
_temp1155)->f1; goto _LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160;
_LL1158: return _temp1161; _LL1160:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1156:;} static unsigned char _temp1166[ 17u]="StorageClass_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,{
_temp1166, _temp1166, _temp1166 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1167= yy1; void* _temp1173; _LL1169: if(*(( void**) _temp1167)
== Cyc_StorageClass_tok){ _LL1174: _temp1173=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1167)->f1; goto _LL1170;} else{ goto _LL1171;} _LL1171: goto _LL1172;
_LL1170: return _temp1173; _LL1172:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1168:;} static unsigned char _temp1178[ 18u]="TypeSpecifier_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,{
_temp1178, _temp1178, _temp1178 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1179= yy1; void* _temp1185; _LL1181: if(*(( void**) _temp1179)
== Cyc_TypeSpecifier_tok){ _LL1186: _temp1185=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1179)->f1; goto _LL1182;} else{ goto _LL1183;} _LL1183: goto _LL1184;
_LL1182: return _temp1185; _LL1184:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1180:;} static unsigned char _temp1190[ 18u]="StructOrUnion_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,{
_temp1190, _temp1190, _temp1190 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1191= yy1; void* _temp1197; _LL1193: if(*(( void**) _temp1191)
== Cyc_StructOrUnion_tok){ _LL1198: _temp1197=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1191)->f1; goto _LL1194;} else{ goto _LL1195;} _LL1195: goto _LL1196;
_LL1194: return _temp1197; _LL1196:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1192:;} static unsigned char _temp1202[ 13u]="TypeQual_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,{ _temp1202,
_temp1202, _temp1202 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1203= yy1; struct Cyc_Absyn_Tqual _temp1209; _LL1205: if(*((
void**) _temp1203) == Cyc_TypeQual_tok){ _LL1210: _temp1209=(( struct Cyc_TypeQual_tok_struct*)
_temp1203)->f1; goto _LL1206;} else{ goto _LL1207;} _LL1207: goto _LL1208;
_LL1206: return _temp1209; _LL1208:( void) _throw(( void*)& Cyc_yyfail_TypeQual_tok);
_LL1204:;} static unsigned char _temp1214[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,{
_temp1214, _temp1214, _temp1214 + 24u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
void* yy1){ void* _temp1215= yy1; struct Cyc_List_List* _temp1221; _LL1217: if(*((
void**) _temp1215) == Cyc_StructFieldDeclList_tok){ _LL1222: _temp1221=(( struct
Cyc_StructFieldDeclList_tok_struct*) _temp1215)->f1; goto _LL1218;} else{ goto
_LL1219;} _LL1219: goto _LL1220; _LL1218: return _temp1221; _LL1220:( void)
_throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok); _LL1216:;} static
unsigned char _temp1226[ 28u]="StructFieldDeclListList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,{ _temp1226, _temp1226,
_temp1226 + 28u}}; struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
void* yy1){ void* _temp1227= yy1; struct Cyc_List_List* _temp1233; _LL1229: if(*((
void**) _temp1227) == Cyc_StructFieldDeclListList_tok){ _LL1234: _temp1233=((
struct Cyc_StructFieldDeclListList_tok_struct*) _temp1227)->f1; goto _LL1230;}
else{ goto _LL1231;} _LL1231: goto _LL1232; _LL1230: return _temp1233; _LL1232:(
void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok); _LL1228:;}
static unsigned char _temp1238[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,{ _temp1238, _temp1238,
_temp1238 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( void*
yy1){ void* _temp1239= yy1; struct Cyc_List_List* _temp1245; _LL1241: if(*((
void**) _temp1239) == Cyc_TypeModifierList_tok){ _LL1246: _temp1245=(( struct
Cyc_TypeModifierList_tok_struct*) _temp1239)->f1; goto _LL1242;} else{ goto
_LL1243;} _LL1243: goto _LL1244; _LL1242: return _temp1245; _LL1244:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1240:;} static
unsigned char _temp1250[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure,{ _temp1250, _temp1250, _temp1250 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1251= yy1; void* _temp1257;
_LL1253: if(*(( void**) _temp1251) == Cyc_Rgn_tok){ _LL1258: _temp1257=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1251)->f1; goto _LL1254;} else{ goto _LL1255;}
_LL1255: goto _LL1256; _LL1254: return _temp1257; _LL1256:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1252:;} static unsigned char _temp1262[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,{
_temp1262, _temp1262, _temp1262 + 15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
void* yy1){ void* _temp1263= yy1; struct Cyc_Parse_Declarator* _temp1269;
_LL1265: if(*(( void**) _temp1263) == Cyc_Declarator_tok){ _LL1270: _temp1269=((
struct Cyc_Declarator_tok_struct*) _temp1263)->f1; goto _LL1266;} else{ goto
_LL1267;} _LL1267: goto _LL1268; _LL1266: return _temp1269; _LL1268:( void)
_throw(( void*)& Cyc_yyfail_Declarator_tok); _LL1264:;} static unsigned char
_temp1274[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,{ _temp1274, _temp1274,
_temp1274 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok( void* yy1){
void* _temp1275= yy1; struct _tuple13* _temp1281; _LL1277: if(*(( void**)
_temp1275) == Cyc_DeclaratorExpopt_tok){ _LL1282: _temp1281=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1275)->f1; goto _LL1278;} else{ goto _LL1279;} _LL1279: goto _LL1280;
_LL1278: return _temp1281; _LL1280:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok);
_LL1276:;} static unsigned char _temp1286[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,{
_temp1286, _temp1286, _temp1286 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1287= yy1; struct Cyc_List_List* _temp1293; _LL1289: if(*((
void**) _temp1287) == Cyc_DeclaratorExpoptList_tok){ _LL1294: _temp1293=((
struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1287)->f1; goto _LL1290;}
else{ goto _LL1291;} _LL1291: goto _LL1292; _LL1290: return _temp1293; _LL1292:(
void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok); _LL1288:;} static
unsigned char _temp1298[ 23u]="AbstractDeclarator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,{ _temp1298, _temp1298,
_temp1298 + 23u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
void* yy1){ void* _temp1299= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1305;
_LL1301: if(*(( void**) _temp1299) == Cyc_AbstractDeclarator_tok){ _LL1306:
_temp1305=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1299)->f1; goto
_LL1302;} else{ goto _LL1303;} _LL1303: goto _LL1304; _LL1302: return _temp1305;
_LL1304:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok); _LL1300:;}
static unsigned char _temp1310[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,{ _temp1310, _temp1310, _temp1310 + 9u}};
int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1311= yy1; int _temp1317; _LL1313:
if(*(( void**) _temp1311) == Cyc_Bool_tok){ _LL1318: _temp1317=(( struct Cyc_Bool_tok_struct*)
_temp1311)->f1; goto _LL1314;} else{ goto _LL1315;} _LL1315: goto _LL1316;
_LL1314: return _temp1317; _LL1316:( void) _throw(( void*)& Cyc_yyfail_Bool_tok);
_LL1312:;} static unsigned char _temp1322[ 10u]="Scope_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Scope_tok={ Cyc_Core_Failure,{ _temp1322, _temp1322, _temp1322 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1323= yy1; void* _temp1329;
_LL1325: if(*(( void**) _temp1323) == Cyc_Scope_tok){ _LL1330: _temp1329=( void*)((
struct Cyc_Scope_tok_struct*) _temp1323)->f1; goto _LL1326;} else{ goto _LL1327;}
_LL1327: goto _LL1328; _LL1326: return _temp1329; _LL1328:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1324:;} static unsigned char _temp1334[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,{
_temp1334, _temp1334, _temp1334 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
void* yy1){ void* _temp1335= yy1; struct Cyc_Absyn_Tunionfield* _temp1341;
_LL1337: if(*(( void**) _temp1335) == Cyc_TunionField_tok){ _LL1342: _temp1341=((
struct Cyc_TunionField_tok_struct*) _temp1335)->f1; goto _LL1338;} else{ goto
_LL1339;} _LL1339: goto _LL1340; _LL1338: return _temp1341; _LL1340:( void)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1336:;} static unsigned char
_temp1346[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,{ _temp1346, _temp1346, _temp1346 + 20u}}; struct Cyc_List_List*
Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1347= yy1; struct Cyc_List_List*
_temp1353; _LL1349: if(*(( void**) _temp1347) == Cyc_TunionFieldList_tok){
_LL1354: _temp1353=(( struct Cyc_TunionFieldList_tok_struct*) _temp1347)->f1;
goto _LL1350;} else{ goto _LL1351;} _LL1351: goto _LL1352; _LL1350: return
_temp1353; _LL1352:( void) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1348:;} static unsigned char _temp1358[ 17u]="QualSpecList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,{
_temp1358, _temp1358, _temp1358 + 17u}}; struct _tuple18* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1359= yy1; struct _tuple18* _temp1365; _LL1361: if(*((
void**) _temp1359) == Cyc_QualSpecList_tok){ _LL1366: _temp1365=(( struct Cyc_QualSpecList_tok_struct*)
_temp1359)->f1; goto _LL1362;} else{ goto _LL1363;} _LL1363: goto _LL1364;
_LL1362: return _temp1365; _LL1364:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1360:;} static unsigned char _temp1370[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,{ _temp1370, _temp1370, _temp1370 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1371= yy1;
struct Cyc_List_List* _temp1377; _LL1373: if(*(( void**) _temp1371) == Cyc_IdList_tok){
_LL1378: _temp1377=(( struct Cyc_IdList_tok_struct*) _temp1371)->f1; goto
_LL1374;} else{ goto _LL1375;} _LL1375: goto _LL1376; _LL1374: return _temp1377;
_LL1376:( void) _throw(( void*)& Cyc_yyfail_IdList_tok); _LL1372:;} static
unsigned char _temp1382[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,{ _temp1382, _temp1382, _temp1382 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1383= yy1;
struct _tuple2* _temp1389; _LL1385: if(*(( void**) _temp1383) == Cyc_ParamDecl_tok){
_LL1390: _temp1389=(( struct Cyc_ParamDecl_tok_struct*) _temp1383)->f1; goto
_LL1386;} else{ goto _LL1387;} _LL1387: goto _LL1388; _LL1386: return _temp1389;
_LL1388:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1384:;} static
unsigned char _temp1394[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,{ _temp1394, _temp1394,
_temp1394 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( void* yy1){
void* _temp1395= yy1; struct Cyc_List_List* _temp1401; _LL1397: if(*(( void**)
_temp1395) == Cyc_ParamDeclList_tok){ _LL1402: _temp1401=(( struct Cyc_ParamDeclList_tok_struct*)
_temp1395)->f1; goto _LL1398;} else{ goto _LL1399;} _LL1399: goto _LL1400;
_LL1398: return _temp1401; _LL1400:( void) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1396:;} static unsigned char _temp1406[ 22u]="ParamDeclListBool_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,{
_temp1406, _temp1406, _temp1406 + 22u}}; struct _tuple17* Cyc_yyget_ParamDeclListBool_tok(
void* yy1){ void* _temp1407= yy1; struct _tuple17* _temp1413; _LL1409: if(*((
void**) _temp1407) == Cyc_ParamDeclListBool_tok){ _LL1414: _temp1413=(( struct
Cyc_ParamDeclListBool_tok_struct*) _temp1407)->f1; goto _LL1410;} else{ goto
_LL1411;} _LL1411: goto _LL1412; _LL1410: return _temp1413; _LL1412:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok); _LL1408:;} static
unsigned char _temp1418[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,{ _temp1418, _temp1418, _temp1418 +
13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void* yy1){ void* _temp1419=
yy1; struct Cyc_List_List* _temp1425; _LL1421: if(*(( void**) _temp1419) == Cyc_TypeList_tok){
_LL1426: _temp1425=(( struct Cyc_TypeList_tok_struct*) _temp1419)->f1; goto
_LL1422;} else{ goto _LL1423;} _LL1423: goto _LL1424; _LL1422: return _temp1425;
_LL1424:( void) _throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1420:;} static
unsigned char _temp1430[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,{ _temp1430, _temp1430,
_temp1430 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( void* yy1){
void* _temp1431= yy1; struct Cyc_List_List* _temp1437; _LL1433: if(*(( void**)
_temp1431) == Cyc_DesignatorList_tok){ _LL1438: _temp1437=(( struct Cyc_DesignatorList_tok_struct*)
_temp1431)->f1; goto _LL1434;} else{ goto _LL1435;} _LL1435: goto _LL1436;
_LL1434: return _temp1437; _LL1436:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1432:;} static unsigned char _temp1442[ 15u]="Designator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,{ _temp1442,
_temp1442, _temp1442 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1443= yy1; void* _temp1449; _LL1445: if(*(( void**) _temp1443) == Cyc_Designator_tok){
_LL1450: _temp1449=( void*)(( struct Cyc_Designator_tok_struct*) _temp1443)->f1;
goto _LL1446;} else{ goto _LL1447;} _LL1447: goto _LL1448; _LL1446: return
_temp1449; _LL1448:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1444:;}
static unsigned char _temp1454[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure,{ _temp1454, _temp1454, _temp1454 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1455= yy1; void* _temp1461;
_LL1457: if(*(( void**) _temp1455) == Cyc_Kind_tok){ _LL1462: _temp1461=( void*)((
struct Cyc_Kind_tok_struct*) _temp1455)->f1; goto _LL1458;} else{ goto _LL1459;}
_LL1459: goto _LL1460; _LL1458: return _temp1461; _LL1460:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1456:;} static unsigned char _temp1466[ 9u]="Type_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,{
_temp1466, _temp1466, _temp1466 + 9u}}; void* Cyc_yyget_Type_tok( void* yy1){
void* _temp1467= yy1; void* _temp1473; _LL1469: if(*(( void**) _temp1467) == Cyc_Type_tok){
_LL1474: _temp1473=( void*)(( struct Cyc_Type_tok_struct*) _temp1467)->f1; goto
_LL1470;} else{ goto _LL1471;} _LL1471: goto _LL1472; _LL1470: return _temp1473;
_LL1472:( void) _throw(( void*)& Cyc_yyfail_Type_tok); _LL1468:;} static
unsigned char _temp1478[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,{ _temp1478, _temp1478,
_temp1478 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( void* yy1){
void* _temp1479= yy1; struct Cyc_List_List* _temp1485; _LL1481: if(*(( void**)
_temp1479) == Cyc_AttributeList_tok){ _LL1486: _temp1485=(( struct Cyc_AttributeList_tok_struct*)
_temp1479)->f1; goto _LL1482;} else{ goto _LL1483;} _LL1483: goto _LL1484;
_LL1482: return _temp1485; _LL1484:( void) _throw(( void*)& Cyc_yyfail_AttributeList_tok);
_LL1480:;} static unsigned char _temp1490[ 14u]="Attribute_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure,{ _temp1490,
_temp1490, _temp1490 + 14u}}; void* Cyc_yyget_Attribute_tok( void* yy1){ void*
_temp1491= yy1; void* _temp1497; _LL1493: if(*(( void**) _temp1491) == Cyc_Attribute_tok){
_LL1498: _temp1497=( void*)(( struct Cyc_Attribute_tok_struct*) _temp1491)->f1;
goto _LL1494;} else{ goto _LL1495;} _LL1495: goto _LL1496; _LL1494: return
_temp1497; _LL1496:( void) _throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1492:;}
static unsigned char _temp1502[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,{ _temp1502, _temp1502, _temp1502 +
14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok( void* yy1){ void*
_temp1503= yy1; struct Cyc_Absyn_Enumfield* _temp1509; _LL1505: if(*(( void**)
_temp1503) == Cyc_Enumfield_tok){ _LL1510: _temp1509=(( struct Cyc_Enumfield_tok_struct*)
_temp1503)->f1; goto _LL1506;} else{ goto _LL1507;} _LL1507: goto _LL1508;
_LL1506: return _temp1509; _LL1508:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1504:;} static unsigned char _temp1514[ 18u]="EnumfieldList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,{
_temp1514, _temp1514, _temp1514 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1515= yy1; struct Cyc_List_List* _temp1521; _LL1517: if(*((
void**) _temp1515) == Cyc_EnumfieldList_tok){ _LL1522: _temp1521=(( struct Cyc_EnumfieldList_tok_struct*)
_temp1515)->f1; goto _LL1518;} else{ goto _LL1519;} _LL1519: goto _LL1520;
_LL1518: return _temp1521; _LL1520:( void) _throw(( void*)& Cyc_yyfail_EnumfieldList_tok);
_LL1516:;} static unsigned char _temp1526[ 12u]="TypeOpt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeOpt_tok={ Cyc_Core_Failure,{ _temp1526, _temp1526, _temp1526 + 12u}};
struct Cyc_Core_Opt* Cyc_yyget_TypeOpt_tok( void* yy1){ void* _temp1527= yy1;
struct Cyc_Core_Opt* _temp1533; _LL1529: if(*(( void**) _temp1527) == Cyc_TypeOpt_tok){
_LL1534: _temp1533=(( struct Cyc_TypeOpt_tok_struct*) _temp1527)->f1; goto
_LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532; _LL1530: return _temp1533;
_LL1532:( void) _throw(( void*)& Cyc_yyfail_TypeOpt_tok); _LL1528:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1535; _temp1535.timestamp=
0; _temp1535.first_line= 0; _temp1535.first_column= 0; _temp1535.last_line= 0;
_temp1535.last_column= 0; _temp1535.text=({ unsigned char* _temp1536=""; struct
_tagged_string _temp1537; _temp1537.curr= _temp1536; _temp1537.base= _temp1536;
_temp1537.last_plus_one= _temp1536 + 1; _temp1537;}); _temp1535;});} static
unsigned char _temp1540[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc={ 0, 0, 0, 0, 0,{
_temp1540, _temp1540, _temp1540 + 1u}}; static short Cyc_yytranslate[ 352u]={ 0,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 123, 2, 2, 105, 121, 117, 2, 101, 102, 112, 115, 103, 116, 109,
120, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 108, 97, 106, 100, 107, 114, 113, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 110, 2, 111,
119, 104, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 98, 118, 99, 122, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96}; static
unsigned char _temp1543[ 2u]="$"; static unsigned char _temp1546[ 6u]="error";
static unsigned char _temp1549[ 12u]="$undefined."; static unsigned char
_temp1552[ 5u]="AUTO"; static unsigned char _temp1555[ 9u]="REGISTER"; static
unsigned char _temp1558[ 7u]="STATIC"; static unsigned char _temp1561[ 7u]="EXTERN";
static unsigned char _temp1564[ 8u]="TYPEDEF"; static unsigned char _temp1567[ 5u]="VOID";
static unsigned char _temp1570[ 5u]="CHAR"; static unsigned char _temp1573[ 6u]="SHORT";
static unsigned char _temp1576[ 4u]="INT"; static unsigned char _temp1579[ 5u]="LONG";
static unsigned char _temp1582[ 6u]="FLOAT"; static unsigned char _temp1585[ 7u]="DOUBLE";
static unsigned char _temp1588[ 7u]="SIGNED"; static unsigned char _temp1591[ 9u]="UNSIGNED";
static unsigned char _temp1594[ 6u]="CONST"; static unsigned char _temp1597[ 9u]="VOLATILE";
static unsigned char _temp1600[ 9u]="RESTRICT"; static unsigned char _temp1603[
7u]="STRUCT"; static unsigned char _temp1606[ 6u]="UNION"; static unsigned char
_temp1609[ 5u]="CASE"; static unsigned char _temp1612[ 8u]="DEFAULT"; static
unsigned char _temp1615[ 7u]="INLINE"; static unsigned char _temp1618[ 3u]="IF";
static unsigned char _temp1621[ 5u]="ELSE"; static unsigned char _temp1624[ 7u]="SWITCH";
static unsigned char _temp1627[ 6u]="WHILE"; static unsigned char _temp1630[ 3u]="DO";
static unsigned char _temp1633[ 4u]="FOR"; static unsigned char _temp1636[ 5u]="GOTO";
static unsigned char _temp1639[ 9u]="CONTINUE"; static unsigned char _temp1642[
6u]="BREAK"; static unsigned char _temp1645[ 7u]="RETURN"; static unsigned char
_temp1648[ 7u]="SIZEOF"; static unsigned char _temp1651[ 5u]="ENUM"; static
unsigned char _temp1654[ 8u]="NULL_kw"; static unsigned char _temp1657[ 4u]="LET";
static unsigned char _temp1660[ 6u]="THROW"; static unsigned char _temp1663[ 4u]="TRY";
static unsigned char _temp1666[ 6u]="CATCH"; static unsigned char _temp1669[ 4u]="NEW";
static unsigned char _temp1672[ 9u]="ABSTRACT"; static unsigned char _temp1675[
9u]="FALLTHRU"; static unsigned char _temp1678[ 6u]="USING"; static
unsigned char _temp1681[ 10u]="NAMESPACE"; static unsigned char _temp1684[ 7u]="TUNION";
static unsigned char _temp1687[ 8u]="XTUNION"; static unsigned char _temp1690[ 5u]="FILL";
static unsigned char _temp1693[ 8u]="CODEGEN"; static unsigned char _temp1696[ 4u]="CUT";
static unsigned char _temp1699[ 7u]="SPLICE"; static unsigned char _temp1702[ 7u]="PRINTF";
static unsigned char _temp1705[ 8u]="FPRINTF"; static unsigned char _temp1708[ 8u]="XPRINTF";
static unsigned char _temp1711[ 6u]="SCANF"; static unsigned char _temp1714[ 7u]="FSCANF";
static unsigned char _temp1717[ 7u]="SSCANF"; static unsigned char _temp1720[ 7u]="MALLOC";
static unsigned char _temp1723[ 9u]="REGION_T"; static unsigned char _temp1726[
7u]="REGION"; static unsigned char _temp1729[ 5u]="RNEW"; static unsigned char
_temp1732[ 8u]="RMALLOC"; static unsigned char _temp1735[ 7u]="PTR_OP"; static
unsigned char _temp1738[ 7u]="INC_OP"; static unsigned char _temp1741[ 7u]="DEC_OP";
static unsigned char _temp1744[ 8u]="LEFT_OP"; static unsigned char _temp1747[ 9u]="RIGHT_OP";
static unsigned char _temp1750[ 6u]="LE_OP"; static unsigned char _temp1753[ 6u]="GE_OP";
static unsigned char _temp1756[ 6u]="EQ_OP"; static unsigned char _temp1759[ 6u]="NE_OP";
static unsigned char _temp1762[ 7u]="AND_OP"; static unsigned char _temp1765[ 6u]="OR_OP";
static unsigned char _temp1768[ 11u]="MUL_ASSIGN"; static unsigned char
_temp1771[ 11u]="DIV_ASSIGN"; static unsigned char _temp1774[ 11u]="MOD_ASSIGN";
static unsigned char _temp1777[ 11u]="ADD_ASSIGN"; static unsigned char
_temp1780[ 11u]="SUB_ASSIGN"; static unsigned char _temp1783[ 12u]="LEFT_ASSIGN";
static unsigned char _temp1786[ 13u]="RIGHT_ASSIGN"; static unsigned char
_temp1789[ 11u]="AND_ASSIGN"; static unsigned char _temp1792[ 11u]="XOR_ASSIGN";
static unsigned char _temp1795[ 10u]="OR_ASSIGN"; static unsigned char _temp1798[
9u]="ELLIPSIS"; static unsigned char _temp1801[ 11u]="LEFT_RIGHT"; static
unsigned char _temp1804[ 12u]="COLON_COLON"; static unsigned char _temp1807[ 11u]="IDENTIFIER";
static unsigned char _temp1810[ 17u]="INTEGER_CONSTANT"; static unsigned char
_temp1813[ 7u]="STRING"; static unsigned char _temp1816[ 19u]="CHARACTER_CONSTANT";
static unsigned char _temp1819[ 18u]="FLOATING_CONSTANT"; static unsigned char
_temp1822[ 9u]="TYPE_VAR"; static unsigned char _temp1825[ 16u]="QUAL_IDENTIFIER";
static unsigned char _temp1828[ 18u]="QUAL_TYPEDEF_NAME"; static unsigned char
_temp1831[ 10u]="ATTRIBUTE"; static unsigned char _temp1834[ 4u]="';'"; static
unsigned char _temp1837[ 4u]="'{'"; static unsigned char _temp1840[ 4u]="'}'";
static unsigned char _temp1843[ 4u]="'='"; static unsigned char _temp1846[ 4u]="'('";
static unsigned char _temp1849[ 4u]="')'"; static unsigned char _temp1852[ 4u]="','";
static unsigned char _temp1855[ 4u]="'_'"; static unsigned char _temp1858[ 4u]="'$'";
static unsigned char _temp1861[ 4u]="'<'"; static unsigned char _temp1864[ 4u]="'>'";
static unsigned char _temp1867[ 4u]="':'"; static unsigned char _temp1870[ 4u]="'.'";
static unsigned char _temp1873[ 4u]="'['"; static unsigned char _temp1876[ 4u]="']'";
static unsigned char _temp1879[ 4u]="'*'"; static unsigned char _temp1882[ 4u]="'@'";
static unsigned char _temp1885[ 4u]="'?'"; static unsigned char _temp1888[ 4u]="'+'";
static unsigned char _temp1891[ 4u]="'-'"; static unsigned char _temp1894[ 4u]="'&'";
static unsigned char _temp1897[ 4u]="'|'"; static unsigned char _temp1900[ 4u]="'^'";
static unsigned char _temp1903[ 4u]="'/'"; static unsigned char _temp1906[ 4u]="'%'";
static unsigned char _temp1909[ 4u]="'~'"; static unsigned char _temp1912[ 4u]="'!'";
static unsigned char _temp1915[ 5u]="prog"; static unsigned char _temp1918[ 17u]="translation_unit";
static unsigned char _temp1921[ 21u]="external_declaration"; static
unsigned char _temp1924[ 20u]="function_definition"; static unsigned char
_temp1927[ 21u]="function_definition2"; static unsigned char _temp1930[ 13u]="using_action";
static unsigned char _temp1933[ 15u]="unusing_action"; static unsigned char
_temp1936[ 17u]="namespace_action"; static unsigned char _temp1939[ 19u]="unnamespace_action";
static unsigned char _temp1942[ 12u]="declaration"; static unsigned char
_temp1945[ 17u]="declaration_list"; static unsigned char _temp1948[ 23u]="declaration_specifiers";
static unsigned char _temp1951[ 24u]="storage_class_specifier"; static
unsigned char _temp1954[ 15u]="attributes_opt"; static unsigned char _temp1957[
11u]="attributes"; static unsigned char _temp1960[ 15u]="attribute_list"; static
unsigned char _temp1963[ 10u]="attribute"; static unsigned char _temp1966[ 15u]="type_specifier";
static unsigned char _temp1969[ 5u]="kind"; static unsigned char _temp1972[ 15u]="type_qualifier";
static unsigned char _temp1975[ 15u]="enum_specifier"; static unsigned char
_temp1978[ 11u]="enum_field"; static unsigned char _temp1981[ 22u]="enum_declaration_list";
static unsigned char _temp1984[ 26u]="struct_or_union_specifier"; static
unsigned char _temp1987[ 16u]="type_params_opt"; static unsigned char _temp1990[
16u]="struct_or_union"; static unsigned char _temp1993[ 24u]="struct_declaration_list";
static unsigned char _temp1996[ 25u]="struct_declaration_list0"; static
unsigned char _temp1999[ 21u]="init_declarator_list"; static unsigned char
_temp2002[ 22u]="init_declarator_list0"; static unsigned char _temp2005[ 16u]="init_declarator";
static unsigned char _temp2008[ 19u]="struct_declaration"; static unsigned char
_temp2011[ 25u]="specifier_qualifier_list"; static unsigned char _temp2014[ 23u]="struct_declarator_list";
static unsigned char _temp2017[ 24u]="struct_declarator_list0"; static
unsigned char _temp2020[ 18u]="struct_declarator"; static unsigned char
_temp2023[ 17u]="tunion_specifier"; static unsigned char _temp2026[ 18u]="tunion_or_xtunion";
static unsigned char _temp2029[ 17u]="tunionfield_list"; static unsigned char
_temp2032[ 18u]="tunionfield_scope"; static unsigned char _temp2035[ 12u]="tunionfield";
static unsigned char _temp2038[ 11u]="declarator"; static unsigned char
_temp2041[ 18u]="direct_declarator"; static unsigned char _temp2044[ 8u]="pointer";
static unsigned char _temp2047[ 13u]="pointer_char"; static unsigned char
_temp2050[ 8u]="rgn_opt"; static unsigned char _temp2053[ 4u]="rgn"; static
unsigned char _temp2056[ 20u]="type_qualifier_list"; static unsigned char
_temp2059[ 20u]="parameter_type_list"; static unsigned char _temp2062[ 16u]="optional_effect";
static unsigned char _temp2065[ 16u]="optional_inject"; static unsigned char
_temp2068[ 11u]="effect_set"; static unsigned char _temp2071[ 14u]="atomic_effect";
static unsigned char _temp2074[ 11u]="region_set"; static unsigned char
_temp2077[ 15u]="parameter_list"; static unsigned char _temp2080[ 22u]="parameter_declaration";
static unsigned char _temp2083[ 16u]="identifier_list"; static unsigned char
_temp2086[ 17u]="identifier_list0"; static unsigned char _temp2089[ 12u]="initializer";
static unsigned char _temp2092[ 18u]="array_initializer"; static unsigned char
_temp2095[ 17u]="initializer_list"; static unsigned char _temp2098[ 12u]="designation";
static unsigned char _temp2101[ 16u]="designator_list"; static unsigned char
_temp2104[ 11u]="designator"; static unsigned char _temp2107[ 10u]="type_name";
static unsigned char _temp2110[ 14u]="any_type_name"; static unsigned char
_temp2113[ 15u]="type_name_list"; static unsigned char _temp2116[ 20u]="abstract_declarator";
static unsigned char _temp2119[ 27u]="direct_abstract_declarator"; static
unsigned char _temp2122[ 10u]="statement"; static unsigned char _temp2125[ 18u]="labeled_statement";
static unsigned char _temp2128[ 21u]="expression_statement"; static
unsigned char _temp2131[ 19u]="compound_statement"; static unsigned char
_temp2134[ 16u]="block_item_list"; static unsigned char _temp2137[ 11u]="block_item";
static unsigned char _temp2140[ 20u]="selection_statement"; static unsigned char
_temp2143[ 15u]="switch_clauses"; static unsigned char _temp2146[ 20u]="iteration_statement";
static unsigned char _temp2149[ 15u]="jump_statement"; static unsigned char
_temp2152[ 8u]="pattern"; static unsigned char _temp2155[ 19u]="tuple_pattern_list";
static unsigned char _temp2158[ 20u]="tuple_pattern_list0"; static unsigned char
_temp2161[ 14u]="field_pattern"; static unsigned char _temp2164[ 19u]="field_pattern_list";
static unsigned char _temp2167[ 20u]="field_pattern_list0"; static unsigned char
_temp2170[ 11u]="expression"; static unsigned char _temp2173[ 22u]="assignment_expression";
static unsigned char _temp2176[ 20u]="assignment_operator"; static unsigned char
_temp2179[ 23u]="conditional_expression"; static unsigned char _temp2182[ 20u]="constant_expression";
static unsigned char _temp2185[ 22u]="logical_or_expression"; static
unsigned char _temp2188[ 23u]="logical_and_expression"; static unsigned char
_temp2191[ 24u]="inclusive_or_expression"; static unsigned char _temp2194[ 24u]="exclusive_or_expression";
static unsigned char _temp2197[ 15u]="and_expression"; static unsigned char
_temp2200[ 20u]="equality_expression"; static unsigned char _temp2203[ 22u]="relational_expression";
static unsigned char _temp2206[ 17u]="shift_expression"; static unsigned char
_temp2209[ 20u]="additive_expression"; static unsigned char _temp2212[ 26u]="multiplicative_expression";
static unsigned char _temp2215[ 16u]="cast_expression"; static unsigned char
_temp2218[ 17u]="unary_expression"; static unsigned char _temp2221[ 14u]="format_primop";
static unsigned char _temp2224[ 15u]="unary_operator"; static unsigned char
_temp2227[ 19u]="postfix_expression"; static unsigned char _temp2230[ 19u]="primary_expression";
static unsigned char _temp2233[ 25u]="argument_expression_list"; static
unsigned char _temp2236[ 26u]="argument_expression_list0"; static unsigned char
_temp2239[ 9u]="constant"; static unsigned char _temp2242[ 20u]="qual_opt_identifier";
static struct _tagged_string Cyc_yytname[ 234u]={{ _temp1543, _temp1543,
_temp1543 + 2u},{ _temp1546, _temp1546, _temp1546 + 6u},{ _temp1549, _temp1549,
_temp1549 + 12u},{ _temp1552, _temp1552, _temp1552 + 5u},{ _temp1555, _temp1555,
_temp1555 + 9u},{ _temp1558, _temp1558, _temp1558 + 7u},{ _temp1561, _temp1561,
_temp1561 + 7u},{ _temp1564, _temp1564, _temp1564 + 8u},{ _temp1567, _temp1567,
_temp1567 + 5u},{ _temp1570, _temp1570, _temp1570 + 5u},{ _temp1573, _temp1573,
_temp1573 + 6u},{ _temp1576, _temp1576, _temp1576 + 4u},{ _temp1579, _temp1579,
_temp1579 + 5u},{ _temp1582, _temp1582, _temp1582 + 6u},{ _temp1585, _temp1585,
_temp1585 + 7u},{ _temp1588, _temp1588, _temp1588 + 7u},{ _temp1591, _temp1591,
_temp1591 + 9u},{ _temp1594, _temp1594, _temp1594 + 6u},{ _temp1597, _temp1597,
_temp1597 + 9u},{ _temp1600, _temp1600, _temp1600 + 9u},{ _temp1603, _temp1603,
_temp1603 + 7u},{ _temp1606, _temp1606, _temp1606 + 6u},{ _temp1609, _temp1609,
_temp1609 + 5u},{ _temp1612, _temp1612, _temp1612 + 8u},{ _temp1615, _temp1615,
_temp1615 + 7u},{ _temp1618, _temp1618, _temp1618 + 3u},{ _temp1621, _temp1621,
_temp1621 + 5u},{ _temp1624, _temp1624, _temp1624 + 7u},{ _temp1627, _temp1627,
_temp1627 + 6u},{ _temp1630, _temp1630, _temp1630 + 3u},{ _temp1633, _temp1633,
_temp1633 + 4u},{ _temp1636, _temp1636, _temp1636 + 5u},{ _temp1639, _temp1639,
_temp1639 + 9u},{ _temp1642, _temp1642, _temp1642 + 6u},{ _temp1645, _temp1645,
_temp1645 + 7u},{ _temp1648, _temp1648, _temp1648 + 7u},{ _temp1651, _temp1651,
_temp1651 + 5u},{ _temp1654, _temp1654, _temp1654 + 8u},{ _temp1657, _temp1657,
_temp1657 + 4u},{ _temp1660, _temp1660, _temp1660 + 6u},{ _temp1663, _temp1663,
_temp1663 + 4u},{ _temp1666, _temp1666, _temp1666 + 6u},{ _temp1669, _temp1669,
_temp1669 + 4u},{ _temp1672, _temp1672, _temp1672 + 9u},{ _temp1675, _temp1675,
_temp1675 + 9u},{ _temp1678, _temp1678, _temp1678 + 6u},{ _temp1681, _temp1681,
_temp1681 + 10u},{ _temp1684, _temp1684, _temp1684 + 7u},{ _temp1687, _temp1687,
_temp1687 + 8u},{ _temp1690, _temp1690, _temp1690 + 5u},{ _temp1693, _temp1693,
_temp1693 + 8u},{ _temp1696, _temp1696, _temp1696 + 4u},{ _temp1699, _temp1699,
_temp1699 + 7u},{ _temp1702, _temp1702, _temp1702 + 7u},{ _temp1705, _temp1705,
_temp1705 + 8u},{ _temp1708, _temp1708, _temp1708 + 8u},{ _temp1711, _temp1711,
_temp1711 + 6u},{ _temp1714, _temp1714, _temp1714 + 7u},{ _temp1717, _temp1717,
_temp1717 + 7u},{ _temp1720, _temp1720, _temp1720 + 7u},{ _temp1723, _temp1723,
_temp1723 + 9u},{ _temp1726, _temp1726, _temp1726 + 7u},{ _temp1729, _temp1729,
_temp1729 + 5u},{ _temp1732, _temp1732, _temp1732 + 8u},{ _temp1735, _temp1735,
_temp1735 + 7u},{ _temp1738, _temp1738, _temp1738 + 7u},{ _temp1741, _temp1741,
_temp1741 + 7u},{ _temp1744, _temp1744, _temp1744 + 8u},{ _temp1747, _temp1747,
_temp1747 + 9u},{ _temp1750, _temp1750, _temp1750 + 6u},{ _temp1753, _temp1753,
_temp1753 + 6u},{ _temp1756, _temp1756, _temp1756 + 6u},{ _temp1759, _temp1759,
_temp1759 + 6u},{ _temp1762, _temp1762, _temp1762 + 7u},{ _temp1765, _temp1765,
_temp1765 + 6u},{ _temp1768, _temp1768, _temp1768 + 11u},{ _temp1771, _temp1771,
_temp1771 + 11u},{ _temp1774, _temp1774, _temp1774 + 11u},{ _temp1777, _temp1777,
_temp1777 + 11u},{ _temp1780, _temp1780, _temp1780 + 11u},{ _temp1783, _temp1783,
_temp1783 + 12u},{ _temp1786, _temp1786, _temp1786 + 13u},{ _temp1789, _temp1789,
_temp1789 + 11u},{ _temp1792, _temp1792, _temp1792 + 11u},{ _temp1795, _temp1795,
_temp1795 + 10u},{ _temp1798, _temp1798, _temp1798 + 9u},{ _temp1801, _temp1801,
_temp1801 + 11u},{ _temp1804, _temp1804, _temp1804 + 12u},{ _temp1807, _temp1807,
_temp1807 + 11u},{ _temp1810, _temp1810, _temp1810 + 17u},{ _temp1813, _temp1813,
_temp1813 + 7u},{ _temp1816, _temp1816, _temp1816 + 19u},{ _temp1819, _temp1819,
_temp1819 + 18u},{ _temp1822, _temp1822, _temp1822 + 9u},{ _temp1825, _temp1825,
_temp1825 + 16u},{ _temp1828, _temp1828, _temp1828 + 18u},{ _temp1831, _temp1831,
_temp1831 + 10u},{ _temp1834, _temp1834, _temp1834 + 4u},{ _temp1837, _temp1837,
_temp1837 + 4u},{ _temp1840, _temp1840, _temp1840 + 4u},{ _temp1843, _temp1843,
_temp1843 + 4u},{ _temp1846, _temp1846, _temp1846 + 4u},{ _temp1849, _temp1849,
_temp1849 + 4u},{ _temp1852, _temp1852, _temp1852 + 4u},{ _temp1855, _temp1855,
_temp1855 + 4u},{ _temp1858, _temp1858, _temp1858 + 4u},{ _temp1861, _temp1861,
_temp1861 + 4u},{ _temp1864, _temp1864, _temp1864 + 4u},{ _temp1867, _temp1867,
_temp1867 + 4u},{ _temp1870, _temp1870, _temp1870 + 4u},{ _temp1873, _temp1873,
_temp1873 + 4u},{ _temp1876, _temp1876, _temp1876 + 4u},{ _temp1879, _temp1879,
_temp1879 + 4u},{ _temp1882, _temp1882, _temp1882 + 4u},{ _temp1885, _temp1885,
_temp1885 + 4u},{ _temp1888, _temp1888, _temp1888 + 4u},{ _temp1891, _temp1891,
_temp1891 + 4u},{ _temp1894, _temp1894, _temp1894 + 4u},{ _temp1897, _temp1897,
_temp1897 + 4u},{ _temp1900, _temp1900, _temp1900 + 4u},{ _temp1903, _temp1903,
_temp1903 + 4u},{ _temp1906, _temp1906, _temp1906 + 4u},{ _temp1909, _temp1909,
_temp1909 + 4u},{ _temp1912, _temp1912, _temp1912 + 4u},{ _temp1915, _temp1915,
_temp1915 + 5u},{ _temp1918, _temp1918, _temp1918 + 17u},{ _temp1921, _temp1921,
_temp1921 + 21u},{ _temp1924, _temp1924, _temp1924 + 20u},{ _temp1927, _temp1927,
_temp1927 + 21u},{ _temp1930, _temp1930, _temp1930 + 13u},{ _temp1933, _temp1933,
_temp1933 + 15u},{ _temp1936, _temp1936, _temp1936 + 17u},{ _temp1939, _temp1939,
_temp1939 + 19u},{ _temp1942, _temp1942, _temp1942 + 12u},{ _temp1945, _temp1945,
_temp1945 + 17u},{ _temp1948, _temp1948, _temp1948 + 23u},{ _temp1951, _temp1951,
_temp1951 + 24u},{ _temp1954, _temp1954, _temp1954 + 15u},{ _temp1957, _temp1957,
_temp1957 + 11u},{ _temp1960, _temp1960, _temp1960 + 15u},{ _temp1963, _temp1963,
_temp1963 + 10u},{ _temp1966, _temp1966, _temp1966 + 15u},{ _temp1969, _temp1969,
_temp1969 + 5u},{ _temp1972, _temp1972, _temp1972 + 15u},{ _temp1975, _temp1975,
_temp1975 + 15u},{ _temp1978, _temp1978, _temp1978 + 11u},{ _temp1981, _temp1981,
_temp1981 + 22u},{ _temp1984, _temp1984, _temp1984 + 26u},{ _temp1987, _temp1987,
_temp1987 + 16u},{ _temp1990, _temp1990, _temp1990 + 16u},{ _temp1993, _temp1993,
_temp1993 + 24u},{ _temp1996, _temp1996, _temp1996 + 25u},{ _temp1999, _temp1999,
_temp1999 + 21u},{ _temp2002, _temp2002, _temp2002 + 22u},{ _temp2005, _temp2005,
_temp2005 + 16u},{ _temp2008, _temp2008, _temp2008 + 19u},{ _temp2011, _temp2011,
_temp2011 + 25u},{ _temp2014, _temp2014, _temp2014 + 23u},{ _temp2017, _temp2017,
_temp2017 + 24u},{ _temp2020, _temp2020, _temp2020 + 18u},{ _temp2023, _temp2023,
_temp2023 + 17u},{ _temp2026, _temp2026, _temp2026 + 18u},{ _temp2029, _temp2029,
_temp2029 + 17u},{ _temp2032, _temp2032, _temp2032 + 18u},{ _temp2035, _temp2035,
_temp2035 + 12u},{ _temp2038, _temp2038, _temp2038 + 11u},{ _temp2041, _temp2041,
_temp2041 + 18u},{ _temp2044, _temp2044, _temp2044 + 8u},{ _temp2047, _temp2047,
_temp2047 + 13u},{ _temp2050, _temp2050, _temp2050 + 8u},{ _temp2053, _temp2053,
_temp2053 + 4u},{ _temp2056, _temp2056, _temp2056 + 20u},{ _temp2059, _temp2059,
_temp2059 + 20u},{ _temp2062, _temp2062, _temp2062 + 16u},{ _temp2065, _temp2065,
_temp2065 + 16u},{ _temp2068, _temp2068, _temp2068 + 11u},{ _temp2071, _temp2071,
_temp2071 + 14u},{ _temp2074, _temp2074, _temp2074 + 11u},{ _temp2077, _temp2077,
_temp2077 + 15u},{ _temp2080, _temp2080, _temp2080 + 22u},{ _temp2083, _temp2083,
_temp2083 + 16u},{ _temp2086, _temp2086, _temp2086 + 17u},{ _temp2089, _temp2089,
_temp2089 + 12u},{ _temp2092, _temp2092, _temp2092 + 18u},{ _temp2095, _temp2095,
_temp2095 + 17u},{ _temp2098, _temp2098, _temp2098 + 12u},{ _temp2101, _temp2101,
_temp2101 + 16u},{ _temp2104, _temp2104, _temp2104 + 11u},{ _temp2107, _temp2107,
_temp2107 + 10u},{ _temp2110, _temp2110, _temp2110 + 14u},{ _temp2113, _temp2113,
_temp2113 + 15u},{ _temp2116, _temp2116, _temp2116 + 20u},{ _temp2119, _temp2119,
_temp2119 + 27u},{ _temp2122, _temp2122, _temp2122 + 10u},{ _temp2125, _temp2125,
_temp2125 + 18u},{ _temp2128, _temp2128, _temp2128 + 21u},{ _temp2131, _temp2131,
_temp2131 + 19u},{ _temp2134, _temp2134, _temp2134 + 16u},{ _temp2137, _temp2137,
_temp2137 + 11u},{ _temp2140, _temp2140, _temp2140 + 20u},{ _temp2143, _temp2143,
_temp2143 + 15u},{ _temp2146, _temp2146, _temp2146 + 20u},{ _temp2149, _temp2149,
_temp2149 + 15u},{ _temp2152, _temp2152, _temp2152 + 8u},{ _temp2155, _temp2155,
_temp2155 + 19u},{ _temp2158, _temp2158, _temp2158 + 20u},{ _temp2161, _temp2161,
_temp2161 + 14u},{ _temp2164, _temp2164, _temp2164 + 19u},{ _temp2167, _temp2167,
_temp2167 + 20u},{ _temp2170, _temp2170, _temp2170 + 11u},{ _temp2173, _temp2173,
_temp2173 + 22u},{ _temp2176, _temp2176, _temp2176 + 20u},{ _temp2179, _temp2179,
_temp2179 + 23u},{ _temp2182, _temp2182, _temp2182 + 20u},{ _temp2185, _temp2185,
_temp2185 + 22u},{ _temp2188, _temp2188, _temp2188 + 23u},{ _temp2191, _temp2191,
_temp2191 + 24u},{ _temp2194, _temp2194, _temp2194 + 24u},{ _temp2197, _temp2197,
_temp2197 + 15u},{ _temp2200, _temp2200, _temp2200 + 20u},{ _temp2203, _temp2203,
_temp2203 + 22u},{ _temp2206, _temp2206, _temp2206 + 17u},{ _temp2209, _temp2209,
_temp2209 + 20u},{ _temp2212, _temp2212, _temp2212 + 26u},{ _temp2215, _temp2215,
_temp2215 + 16u},{ _temp2218, _temp2218, _temp2218 + 17u},{ _temp2221, _temp2221,
_temp2221 + 14u},{ _temp2224, _temp2224, _temp2224 + 15u},{ _temp2227, _temp2227,
_temp2227 + 19u},{ _temp2230, _temp2230, _temp2230 + 19u},{ _temp2233, _temp2233,
_temp2233 + 25u},{ _temp2236, _temp2236, _temp2236 + 26u},{ _temp2239, _temp2239,
_temp2239 + 9u},{ _temp2242, _temp2242, _temp2242 + 20u}}; static short Cyc_yyr1[
400u]={ 0, 124, 125, 125, 125, 125, 125, 125, 125, 126, 126, 127, 127, 127, 127,
128, 128, 129, 130, 131, 132, 133, 133, 133, 133, 134, 134, 135, 135, 135, 135,
135, 135, 135, 135, 136, 136, 136, 136, 136, 136, 136, 137, 137, 138, 139, 139,
140, 140, 140, 140, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141,
141, 141, 141, 141, 141, 141, 142, 142, 143, 143, 143, 144, 144, 145, 145, 146,
146, 147, 147, 147, 147, 147, 148, 148, 148, 149, 149, 150, 151, 151, 152, 153,
153, 154, 154, 155, 156, 156, 156, 156, 157, 158, 158, 159, 159, 159, 160, 160,
160, 160, 161, 161, 162, 162, 162, 162, 163, 163, 163, 164, 164, 165, 165, 166,
166, 166, 166, 166, 166, 166, 166, 166, 166, 166, 167, 167, 167, 167, 168, 168,
168, 168, 168, 169, 169, 170, 170, 170, 171, 171, 172, 172, 172, 172, 173, 173,
174, 174, 175, 175, 176, 176, 176, 176, 177, 177, 177, 177, 178, 178, 179, 179,
179, 180, 181, 181, 182, 182, 183, 183, 183, 183, 184, 184, 184, 184, 185, 186,
186, 187, 187, 188, 188, 189, 189, 189, 189, 190, 190, 191, 191, 191, 192, 192,
192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 193, 193, 193, 193,
193, 193, 193, 193, 193, 193, 194, 195, 195, 196, 196, 197, 197, 198, 198, 198,
199, 199, 199, 199, 200, 200, 200, 200, 200, 200, 201, 201, 201, 201, 201, 201,
201, 201, 201, 201, 201, 201, 201, 201, 202, 202, 202, 202, 202, 202, 202, 202,
203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 204, 204,
205, 205, 206, 206, 207, 208, 208, 209, 209, 210, 210, 211, 211, 211, 211, 211,
211, 211, 211, 211, 211, 211, 212, 212, 212, 212, 212, 212, 212, 213, 214, 214,
215, 215, 216, 216, 217, 217, 218, 218, 219, 219, 219, 220, 220, 220, 220, 220,
221, 221, 221, 222, 222, 222, 223, 223, 223, 223, 224, 224, 225, 225, 225, 225,
225, 225, 225, 225, 225, 225, 225, 225, 226, 226, 226, 226, 226, 226, 227, 227,
227, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 229,
229, 229, 229, 229, 229, 229, 229, 229, 230, 231, 231, 232, 232, 232, 232, 233,
233}; static short Cyc_yyr2[ 400u]={ 0, 1, 2, 3, 5, 3, 5, 6, 0, 1, 1, 2, 3, 3, 4,
3, 4, 2, 1, 2, 1, 2, 3, 5, 3, 1, 2, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 2, 1, 1,
0, 1, 6, 1, 3, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 4, 4,
1, 1, 1, 1, 1, 5, 2, 1, 3, 1, 3, 4, 6, 6, 3, 3, 0, 3, 3, 1, 1, 1, 1, 2, 1, 1, 3,
1, 3, 3, 2, 3, 2, 3, 1, 1, 3, 1, 2, 3, 6, 3, 4, 5, 1, 1, 1, 2, 3, 3, 0, 1, 1, 2,
6, 1, 2, 1, 3, 3, 4, 4, 3, 5, 4, 4, 4, 2, 3, 4, 4, 5, 1, 1, 4, 4, 1, 0, 1, 1, 3,
1, 1, 2, 2, 4, 5, 7, 0, 2, 0, 1, 1, 3, 2, 3, 1, 3, 1, 3, 3, 5, 1, 3, 2, 1, 2, 1,
1, 3, 1, 1, 2, 3, 4, 8, 1, 2, 3, 4, 2, 1, 2, 3, 2, 1, 2, 1, 2, 3, 3, 1, 3, 1, 1,
2, 3, 2, 3, 3, 4, 2, 4, 3, 3, 5, 4, 4, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 2, 2, 3, 1,
2, 2, 3, 1, 2, 1, 1, 1, 5, 7, 7, 6, 0, 3, 4, 5, 6, 7, 5, 7, 6, 7, 7, 8, 7, 8, 8,
9, 6, 7, 7, 8, 3, 2, 2, 2, 3, 2, 4, 5, 1, 3, 1, 2, 1, 1, 1, 1, 5, 4, 4, 5, 2, 2,
0, 1, 1, 3, 1, 2, 1, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 2,
2, 2, 5, 5, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 3, 3, 1, 3, 3, 1,
3, 3, 1, 3, 3, 3, 1, 4, 1, 2, 2, 2, 2, 2, 2, 4, 2, 4, 7, 9, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 4, 3, 4, 3, 3, 3, 3, 2, 2, 6, 7, 4, 4, 1, 1, 1, 3, 2, 5, 4, 4, 5, 1, 1,
3, 1, 1, 1, 1, 1, 1}; static short Cyc_yydefact[ 766u]={ 8, 35, 36, 37, 38, 40,
51, 53, 54, 55, 56, 57, 58, 59, 60, 71, 72, 73, 88, 89, 42, 0, 0, 41, 0, 0, 113,
114, 0, 398, 64, 399, 85, 0, 52, 0, 141, 142, 145, 1, 8, 9, 0, 0, 10, 0, 42, 42,
42, 61, 62, 0, 63, 0, 0, 124, 0, 146, 126, 39, 0, 33, 43, 75, 277, 398, 273, 276,
275, 0, 271, 0, 0, 0, 0, 0, 176, 0, 278, 17, 19, 0, 0, 0, 66, 0, 0, 0, 0, 2, 8,
8, 8, 8, 21, 0, 93, 94, 96, 27, 29, 31, 85, 0, 85, 148, 150, 0, 85, 38, 0, 25, 0,
0, 11, 0, 0, 0, 136, 125, 42, 147, 8, 0, 34, 0, 0, 285, 284, 274, 283, 24, 0, 0,
0, 0, 42, 42, 194, 196, 0, 69, 70, 65, 200, 0, 127, 174, 0, 171, 0, 397, 0, 0, 0,
0, 359, 360, 361, 362, 363, 364, 0, 0, 0, 0, 0, 394, 384, 395, 396, 0, 0, 0, 0,
367, 0, 365, 366, 0, 296, 309, 317, 319, 321, 323, 325, 327, 330, 335, 338, 341,
345, 0, 0, 347, 368, 383, 382, 0, 3, 0, 5, 0, 22, 0, 0, 0, 12, 28, 30, 32, 84, 0,
90, 91, 0, 83, 0, 85, 0, 110, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 398,
230, 232, 0, 238, 236, 0, 237, 219, 220, 221, 0, 234, 222, 223, 224, 0, 294, 26,
13, 96, 0, 177, 0, 131, 0, 157, 0, 0, 128, 0, 137, 0, 0, 78, 0, 76, 272, 287, 0,
286, 178, 0, 0, 285, 167, 197, 0, 99, 101, 0, 0, 202, 195, 203, 68, 0, 87, 0, 86,
0, 173, 202, 175, 67, 0, 0, 355, 311, 345, 0, 312, 313, 0, 0, 0, 0, 0, 0, 348,
349, 0, 0, 0, 0, 351, 352, 350, 143, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 298, 0, 0, 353, 0,
376, 377, 0, 0, 0, 386, 0, 0, 144, 18, 8, 20, 8, 95, 97, 180, 179, 14, 0, 80, 92,
0, 0, 103, 104, 106, 0, 149, 111, 85, 119, 0, 0, 0, 0, 0, 0, 264, 265, 266, 0, 0,
268, 0, 227, 228, 0, 0, 0, 0, 96, 233, 235, 231, 0, 159, 165, 0, 0, 161, 130, 0,
0, 153, 133, 135, 134, 129, 151, 139, 138, 8, 48, 47, 0, 45, 0, 74, 0, 280, 0,
23, 281, 0, 0, 0, 0, 190, 289, 292, 0, 291, 0, 0, 0, 198, 100, 102, 0, 210, 0, 0,
206, 0, 204, 0, 0, 0, 218, 199, 201, 172, 0, 0, 181, 185, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 385, 392, 0, 391, 318, 0, 320, 322, 324, 326, 328, 329, 333, 334, 331,
332, 336, 337, 339, 340, 342, 343, 344, 297, 0, 374, 375, 370, 0, 372, 373, 0, 0,
0, 4, 6, 0, 316, 107, 98, 0, 0, 0, 112, 121, 120, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0,
263, 267, 0, 0, 0, 226, 0, 229, 0, 15, 295, 160, 0, 0, 163, 0, 132, 0, 158, 157,
152, 140, 7, 0, 0, 0, 79, 77, 288, 193, 0, 290, 189, 191, 282, 0, 279, 169, 168,
0, 212, 205, 208, 0, 213, 0, 0, 207, 0, 354, 0, 182, 0, 186, 380, 381, 0, 0, 0,
0, 0, 0, 0, 346, 388, 0, 0, 356, 371, 369, 389, 0, 0, 82, 105, 108, 81, 109, 122,
119, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 243, 269, 0, 0, 16, 157, 166, 164, 162, 159,
154, 0, 0, 44, 46, 192, 293, 0, 211, 0, 215, 217, 216, 209, 0, 183, 187, 0, 0,
314, 315, 0, 390, 0, 393, 310, 387, 0, 118, 117, 239, 243, 249, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 270, 0, 155, 0, 49, 50, 170, 214, 0, 188, 0, 0, 378, 0, 0, 0,
0, 0, 251, 0, 0, 0, 259, 0, 0, 0, 0, 0, 0, 0, 0, 242, 225, 157, 0, 357, 0, 379,
0, 240, 241, 250, 252, 253, 0, 261, 260, 0, 255, 0, 0, 0, 0, 243, 244, 156, 0, 0,
123, 254, 262, 256, 257, 0, 0, 243, 245, 184, 358, 258, 243, 246, 243, 247, 248,
0, 0, 0}; static short Cyc_yydefgoto[ 110u]={ 763, 39, 40, 41, 241, 42, 370, 43,
372, 44, 207, 45, 46, 61, 62, 434, 435, 47, 143, 48, 49, 271, 272, 50, 84, 51,
213, 214, 95, 96, 97, 215, 147, 382, 383, 384, 52, 53, 533, 534, 535, 54, 55, 56,
57, 120, 107, 430, 460, 423, 555, 418, 419, 284, 263, 149, 75, 76, 475, 375, 476,
477, 446, 447, 139, 144, 145, 461, 291, 244, 245, 246, 247, 248, 249, 250, 687,
251, 252, 275, 276, 277, 449, 450, 451, 253, 254, 356, 180, 525, 181, 182, 183,
184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 489, 490, 197,
198}; static short Cyc_yypact[ 766u]={ 1682, - -32768, - -32768, - -32768, - 48,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, 18, 68,
589, - -32768, 68, - 36, - -32768, - -32768, - 18, - -32768, 48, - -32768, 27,
288, - -32768, 58, 47, 106, - -32768, - -32768, 1682, - -32768, 154, 176, -
-32768, 373, 18, 18, 18, - -32768, - -32768, 339, - -32768, 109, 1980, 257, 138,
28, - -32768, 108, 122, 2040, - -32768, 136, - -32768, 201, - -32768, - -32768,
- -32768, 675, - -32768, 184, 199, 259, 675, 209, 247, 299, 188, - -32768, -
-32768, 2292, - 19, 2292, - -32768, 267, 2359, 3781, 3781, - -32768, 1682, 1682,
1682, 1682, - -32768, 283, 303, - -32768, 1917, 2040, 2040, 2040, 27, 2359, 27,
321, - -32768, 68, - 8, 332, 1087, - -32768, 1980, 373, - -32768, 2180, 2292,
2645, - -32768, 257, 18, - -32768, 1682, 335, - -32768, 68, 364, 675, - -32768,
- -32768, - -32768, - -32768, 388, 3781, 123, 206, 18, 18, 178, - -32768, 77, -
-32768, - -32768, - -32768, 353, 41, - -32768, 183, 223, - -32768, 3994, -
-32768, 3781, 3852, 381, 392, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, 397, 404, 409, 4065, 4065, - -32768, - -32768, - -32768, - -32768,
1445, 411, 4136, 4136, - -32768, 4136, - -32768, - -32768, 390, - -32768, - 6,
444, 403, 415, 433, 420, 82, 361, 393, 121, - -32768, 502, 436, 4136, 330, -
-32768, - -32768, - 33, 445, - -32768, 448, - -32768, 454, - -32768, 288, 2716,
1980, - -32768, - -32768, - -32768, - -32768, 457, 459, 2359, - -32768, 549, 463,
- 19, 27, 68, 471, - -32768, 492, 495, 500, 2064, 509, 530, 517, 525, 2787, 2064,
93, 2064, 2064, - 7, 523, - -32768, - -32768, 524, - -32768, - -32768, 373, -
-32768, - -32768, - -32768, - -32768, 545, 1208, - -32768, - -32768, - -32768,
225, - -32768, - -32768, - -32768, 542, 28, - -32768, 217, - -32768, 558, 230,
565, 128, - -32768, 560, 73, 570, 17, 573, 574, 579, - -32768, - -32768, 580,
583, - -32768, 243, 447, 675, - 52, - -32768, 590, 2359, 2359, 1857, 2858, 21, -
-32768, 269, - -32768, 217, - -32768, 2292, - -32768, 1797, - -32768, 9, -
-32768, - -32768, 2359, 1445, - -32768, - -32768, - -32768, 800, - -32768, 614,
3781, 1742, 657, 3781, 3781, 1445, - -32768, - -32768, 1208, 593, 413, 3781, -
-32768, - -32768, - -32768, - -32768, 4136, 3781, 4136, 4136, 4136, 4136, 4136,
4136, 4136, 4136, 4136, 4136, 4136, 4136, 4136, 4136, 4136, 4136, 4136, - -32768,
- -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, 3781, 3781, - -32768, 84, - -32768, - -32768, 2929, 86, 3781,
- -32768, 2556, 591, - -32768, - -32768, 1682, - -32768, 1682, - -32768, -
-32768, - -32768, - -32768, - -32768, 2359, - -32768, - -32768, 3781, 601, 596,
- -32768, 592, 2359, - -32768, - -32768, 27, 490, 3781, 3781, 3781, 674, 1329,
606, - -32768, - -32768, - -32768, 271, 666, - -32768, 3000, - -32768, - -32768,
2064, 616, 2064, 1561, 1917, - -32768, - -32768, - -32768, 3781, 622, 624, 279,
611, 599, - -32768, 217, 2306, - -32768, - -32768, - -32768, - -32768, - -32768,
- -32768, - -32768, 73, 1682, - -32768, 615, 613, 617, 68, - -32768, 3781, -
-32768, 675, - -32768, - -32768, 629, 3781, 675, 54, - -32768, - -32768, -
-32768, 620, 618, 625, - 19, 631, - -32768, - -32768, - -32768, 217, - -32768,
626, 627, - -32768, 619, 269, 2194, 2292, 3071, - -32768, - -32768, 353, -
-32768, 630, 634, - -32768, - -32768, 348, 2716, 417, 632, 288, 636, 421, 628,
638, 643, 3923, - -32768, - -32768, 642, 644, 444, 235, 403, 415, 433, 420, 82,
82, 361, 361, 361, 361, 393, 393, 121, 121, - -32768, - -32768, - -32768, -
-32768, 646, - -32768, - -32768, - -32768, 648, - -32768, - -32768, 144, 350,
2292, - -32768, - -32768, 647, - -32768, - -32768, - -32768, 549, 3781, 652, -
-32768, - -32768, - -32768, 653, 68, 280, 424, 427, 430, 654, 3142, 3213, 284, -
-32768, - -32768, 658, 662, 659, - -32768, 655, - -32768, 1980, - -32768, -
-32768, - -32768, 2359, - 19, - -32768, 671, - -32768, 217, - -32768, 78, -
-32768, - -32768, - -32768, 453, 663, 17, - -32768, - -32768, - -32768, - -32768,
660, - -32768, - -32768, - -32768, - -32768, 503, - -32768, 669, - -32768, 672,
- -32768, - -32768, - -32768, 217, - -32768, 673, 162, - -32768, 677, 679, 639,
- -32768, 2378, - -32768, - -32768, - -32768, 1980, 2359, 3852, 738, 679, 680,
2556, - -32768, - -32768, 3781, 3781, - -32768, - -32768, - -32768, - -32768,
2556, 356, - -32768, - -32768, - -32768, - -32768, - -32768, 291, 44, 490, 2064,
687, 2064, 3781, 3284, 313, 3355, 316, 3426, 543, - -32768, 689, 701, - -32768,
698, - -32768, - -32768, - -32768, 622, - -32768, 694, 696, - -32768, - -32768,
- -32768, - -32768, 631, - -32768, 697, - -32768, - -32768, - -32768, - -32768,
3781, - -32768, - -32768, 2716, 699, - -32768, 614, 702, - -32768, 378, - -32768,
- -32768, - -32768, 703, - -32768, - -32768, 774, 543, - -32768, 472, 2064, 485,
3497, 2064, 496, 3568, 3639, 320, 675, 717, 706, - -32768, 2064, - -32768, 2359,
- -32768, - -32768, - -32768, - -32768, 347, - -32768, 700, 2359, - -32768, 2467,
2359, 2064, 708, 729, - -32768, 2064, 2064, 526, - -32768, 2064, 2064, 533, 2064,
536, 3710, - 34, 1208, - -32768, - -32768, 698, 3781, - -32768, 726, - -32768,
538, - -32768, - -32768, - -32768, - -32768, - -32768, 2064, - -32768, - -32768,
2064, - -32768, 2064, 2064, 546, 3781, 925, - -32768, - -32768, 380, 727, -
-32768, - -32768, - -32768, - -32768, - -32768, 2064, 357, 543, - -32768, -
-32768, - -32768, - -32768, 925, - -32768, 543, - -32768, - -32768, 831, 832, -
-32768}; static short Cyc_yypgoto[ 110u]={ - -32768, 76, - -32768, 527, - -32768,
- -32768, - -32768, - -32768, - -32768, - 50, - 39, - 54, - -32768, 130, - 32,
266, - -32768, 34, - 197, - 75, - -32768, - -32768, 400, - -32768, 26, - -32768,
- 124, - -32768, - -32768, - -32768, 635, 633, - 78, - -32768, - -32768, 314, -
-32768, - -32768, - 17, - -32768, - -32768, 96, - 38, - 68, - -32768, - -32768,
- 44, - -32768, - 99, - 538, 202, - 395, 550, - 387, - 85, - 280, 730, - -32768,
- 170, - 144, - 347, - 260, - -32768, 402, - 142, - 49, - 102, 0, - 204, 46, -
-32768, - -32768, - 37, - 247, - -32768, - -32768, - 647, - -32768, - -32768, -
10, 571, - -32768, 268, - -32768, - -32768, 111, - 5, - -32768, - 121, - 401, -
143, 534, 539, 535, 540, 537, 326, 236, 327, 344, 42, 480, - -32768, - -32768, -
-32768, - -32768, - 285, - -32768, - -32768, 33}; static short Cyc_yytable[ 4260u]={
113, 148, 412, 138, 111, 138, 137, 124, 137, 308, 309, 137, 77, 121, 265, 112,
262, 114, 119, 519, 445, 387, 471, 118, 643, 216, 561, 704, 137, 319, 558, 305,
140, 58, 432, 453, 374, 570, 138, 740, 137, 137, 59, 573, 113, 209, 210, 211,
111, 531, 532, 454, 80, 365, 63, 78, 243, 79, 113, 126, 242, 208, 255, 582, 130,
366, 58, 581, 326, 141, 289, 485, 511, 58, 741, 256, 142, 515, 58, 299, 367, 406,
179, 199, 104, 464, 108, 118, 81, 58, 15, 16, 17, 138, 754, 464, 137, 29, 83,
407, 690, 220, 78, 31, 134, 433, 759, 78, 327, 294, 297, 761, 267, 762, 60, 136,
89, 136, 547, 288, 136, 105, 287, 58, 58, 58, 58, 618, 212, 85, 217, 288, 106,
83, 221, 82, 216, 136, 290, 137, 219, 98, 471, - 116, 295, 87, 58, 300, 296, 136,
136, 334, 335, 113, 575, 58, 29, 255, 273, 86, 78, 472, 31, 443, 444, 641, 200,
201, 202, 203, 377, 105, 512, 484, 516, 421, 99, 100, 101, 513, 58, 517, 106,
743, 292, 36, 37, 38, 336, 337, 402, 652, 293, 428, 403, 243, 425, 29, 269, 242,
429, 376, 105, 31, 88, 136, 122, 456, 457, 257, 137, 137, 137, 106, 415, 322,
323, 138, 324, 289, 137, 280, 137, 123, 281, 138, 29, 137, 137, 299, 654, 295,
31, 342, 125, 426, 358, 138, 58, 33, 137, 343, 344, 298, 279, 388, 470, 414, 136,
58, 268, 90, 91, 389, 523, 612, 580, 480, 666, 468, 524, 119, 529, 694, 243, 295,
285, 286, 242, 655, 448, 29, 394, 92, 93, 638, 58, 31, 401, 287, 404, 405, 320,
463, 297, 127, - 85, 128, 288, - 85, 36, 37, 38, 288, 83, 36, 37, 38, - 177, 282,
216, 257, 376, 137, - 177, 283, 131, 596, 216, 483, 416, 137, 385, 78, 78, 417,
488, 524, 445, 136, 136, 136, 413, 524, 148, 301, 302, 421, 414, 136, 58, 136,
58, 422, 137, 660, 136, 136, 414, 410, 441, 113, 400, 609, 58, 541, 414, 137,
129, 136, 132, 510, 488, 60, 660, 563, 113, 488, 115, 639, 111, 376, 564, 116,
589, 60, 588, 117, 544, 146, 465, 551, 282, 552, 414, 466, 29, 622, 557, 467,
204, 632, 31, 623, 507, 508, 509, 414, 138, 33, 137, 137, - 85, 85, 359, 360,
361, 83, 488, 133, 36, 37, 38, 58, 488, 58, 205, 524, 218, 553, 679, 721, 136,
682, 320, 530, 414, 716, 615, 414, 136, 478, 222, 414, 482, 659, 320, 29, 338,
339, 571, 362, 468, 31, 102, 574, 270, 103, 492, 363, 364, 660, 138, 136, 659,
137, 521, 594, 522, 613, 414, 595, 548, 614, 550, 722, 136, 662, 663, 295, 414,
29, 591, 669, 58, 758, 274, 31, 293, 273, 94, 742, 376, 78, 33, 518, 278, 700,
78, 755, 137, 701, 310, 414, 64, 36, 37, 38, 668, 325, 697, 332, 333, 311, 753,
531, 532, 113, 312, 136, 136, 255, 536, 537, 538, 313, 542, 565, 340, 341, 314,
760, 321, 58, 637, 487, 414, 328, 642, 597, 414, 329, 661, 601, 414, 137, 624,
414, 606, 625, 414, 659, 626, 414, 330, 29, 66, 357, 67, 68, 64, 31, 644, 645,
368, 113, 442, 369, 69, 111, 331, 70, 71, 371, 136, 378, 443, 444, 379, 72, 58,
386, 208, 73, 74, 685, 686, 621, 448, 390, 499, 500, 501, 502, 705, 414, 599,
345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 707, 414, 136, 376, 29, 66,
391, 67, 68, 392, 31, 711, 414, 376, 393, 355, 667, 69, 671, 672, 70, 71, 376,
395, 78, 443, 444, 397, 72, 137, 726, 396, 73, 74, 724, 398, 385, 137, 409, 64,
137, 732, 414, 304, 408, 306, 306, 136, 735, 414, 29, 737, 414, 746, 302, 206,
31, 411, 316, 317, 670, 751, 414, 33, 629, 631, 306, 306, 376, 306, 381, 497,
498, 420, 36, 37, 38, 243, 503, 504, 424, 242, 431, 673, 427, 675, 437, 306, 717,
436, 65, 66, 438, 67, 68, 439, 31, 505, 506, 440, 243, 326, 455, 69, 242, 481,
70, 71, 486, 376, 520, 526, 527, 528, 72, 539, 543, 243, 73, 74, 545, 242, 549,
554, 556, 64, 559, 560, 567, 566, 572, 78, 577, 568, 578, 593, 706, 282, 136,
710, 579, 583, 584, 585, 602, 592, 136, 598, 720, 136, 600, 676, 678, 603, 681,
604, 684, 607, 657, 616, 608, 610, 727, 611, 619, 620, 730, 731, 627, 633, 733,
734, 634, 736, 635, 636, 29, 66, 646, 67, 68, 696, 31, 640, 648, 650, 664, 651,
653, 69, 605, 747, 70, 71, 748, 665, 749, 750, 674, 688, 72, 656, 689, 709, 73,
74, 713, 715, 421, 692, 757, 693, 695, 703, 698, 723, 699, 702, 719, 306, 728,
306, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306, 306,
306, 718, 729, 739, 745, 756, 473, 764, 765, 744, 647, 150, 569, 151, 479, 152,
373, 617, 153, 469, 691, 264, 649, 380, 576, 154, 155, 752, 452, 156, 157, 158,
159, 160, 161, 162, 491, 306, 163, 164, 494, 165, 166, 493, 496, 0, 495, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0,
307, 474, 0, 171, 0, 0, 0, 172, 0, 0, 0, 443, 444, 0, 173, 0, 0, 174, 175, 176,
306, 0, 0, 0, 177, 178, 306, 0, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 685, 686, 20, 223, 0, 224, 225, 226, 227, 228, 229,
230, 231, 150, 21, 151, 22, 152, 232, 306, 153, 23, 233, 0, 0, 26, 27, 154, 155,
234, 235, 156, 157, 158, 159, 160, 161, 162, 28, 236, 163, 164, 0, 165, 166, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 0, 0, 0, 0, 237, 167, 168, 169,
170, 30, 31, 32, 0, 238, 110, 0, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0,
0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 306, 0, 0, 0, 0, 0, 0, 0,
306, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0,
20, 223, 0, 224, 225, 226, 227, 228, 229, 230, 231, 150, 21, 151, 22, 152, 232,
0, 153, 23, 233, 0, 0, 26, 27, 154, 155, 234, 235, 156, 157, 158, 159, 160, 161,
162, 28, 236, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 237, 167, 168, 169, 170, 30, 31, 32, 0, 238, 110, 239, 0, 171,
0, 0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178,
1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20,
223, 0, 224, 225, 226, 227, 228, 229, 230, 231, 150, 21, 151, 22, 152, 232, 0,
153, 23, 233, 0, 0, 26, 27, 154, 155, 234, 235, 156, 157, 158, 159, 160, 161,
162, 28, 236, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 237, 167, 168, 169, 170, 30, 31, 32, 0, 238, 110, 0, 0, 171, 0,
0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 1,
2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 150, 21, 151, 22, 152, 0, 0, 153, 23, 0, 0, 0, 26, 27,
154, 155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 28, 0, 163, 164, 0, 165, 166,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169,
170, 30, 31, 32, 0, 540, 0, 0, 0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0,
0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150, 21, 151, 0, 152, 0, 0,
153, 0, 0, 0, 0, 26, 27, 154, 155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 28,
0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 29, 167, 168, 169, 170, 30, 31, 32, 0, 0, 318, 0, 0, 171, 0, 0, 34, 240,
0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 150,
21, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 26, 27, 154, 155, 0, 0, 156, 157, 158,
159, 160, 161, 162, 28, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 30, 31, 32, 0, 0, 0, 0,
0, 171, 0, 0, 34, 240, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0,
177, 178, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0,
0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 22, 0, 0, 0, 0, 23, 0, 24, 25, 26,
27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 0, 29, 0, 0, 0, 0, 30, 31,
32, 21, 0, 0, 0, 0, 33, 0, 23, 34, 35, 0, 26, 27, 0, 0, 0, 36, 37, 38, 0, 0, 0,
0, 0, 28, 0, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 21, 0, 30, 31, 32, 0, 0, 0, 0, 0, 33, 26, 27, 34,
35, 0, 0, 0, 0, 0, 0, 36, 37, 38, 28, 0, 0, 0, 0, 0, 0, 0, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 258, 0, 0, 29, 0, 0, 0, 0, 30, 31, 32,
21, 458, 0, 0, 0, 297, 459, 0, 34, 35, 0, 26, 27, 0, 288, 0, 36, 37, 38, 0, 0, 0,
0, 0, 28, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 0, 0, 20, 258, 0, 0, 0, 0, 0, 0, 0, 30, 0, 32, 21, 458, 22, 0, 0, 287, 459,
23, 34, 35, 0, 26, 27, 0, 288, 0, 36, 37, 38, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0,
1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0,
0, 0, 0, 0, 30, 0, 32, 0, 0, 110, 21, 206, 22, 0, 0, 34, 35, 23, 0, 0, 0, 26, 27,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 1, 2, 3, 109, 5, 6, 7, 8, 9, 10, 11,
12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 32, 21,
0, 110, 0, 0, 0, 0, 23, 34, 35, 0, 26, 27, 223, 0, 224, 225, 226, 227, 228, 229,
230, 231, 150, 28, 151, 0, 152, 232, 0, 153, 0, 233, 0, 0, 0, 0, 154, 155, 234,
235, 156, 157, 158, 159, 160, 161, 162, 0, 236, 163, 164, 0, 165, 166, 0, 0, 30,
0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 34, 35, 0, 0, 0, 0, 0, 0, 237, 167, 168, 169, 170,
0, 31, 0, 0, 238, 110, 0, 0, 171, 0, 0, 0, 172, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174,
175, 176, 0, 0, 0, 0, 177, 178, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 26, 27, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 26, 27, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 258, 0, 0, 259, 0, 0, 0, 0, 30,
0, 32, 0, 260, 0, 258, 0, 0, 261, 0, 34, 35, 0, 30, 0, 32, 0, 586, 0, 0, 0, 0,
587, 0, 34, 35, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 6, 7, 8, 9,
10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 27,
0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 26, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
28, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 30, 0, 32, 0,
0, 135, 562, 0, 0, 0, 21, 34, 35, 0, 30, 0, 32, 0, 0, 0, 0, 26, 27, 0, 0, 34, 35,
0, 150, 0, 151, 0, 152, 0, 28, 153, 0, 0, 0, 0, 0, 0, 154, 155, 0, 0, 156, 157,
158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 30, 0,
32, 0, 0, 0, 0, 0, 0, 0, 0, 34, 35, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0,
307, 658, 0, 171, 0, 0, 0, 172, 0, 0, 0, 443, 444, 0, 173, 0, 0, 174, 175, 176,
0, 0, 0, 0, 177, 178, 150, 0, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 0, 0, 154, 155,
0, 0, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31,
0, 0, 0, 307, 725, 0, 171, 0, 0, 0, 172, 0, 0, 0, 443, 444, 0, 173, 0, 0, 174,
175, 176, 0, 0, 0, 0, 177, 178, 150, 0, 151, 0, 152, 0, 0, 153, 0, 0, 0, 0, 0, 0,
154, 155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169,
170, 0, 31, 0, 0, 0, 307, 0, 0, 171, 0, 0, 0, 172, 0, 0, 0, 443, 444, 0, 173, 0,
0, 174, 175, 176, 0, 0, 0, 0, 177, 178, 150, 0, 151, 0, 152, 0, 0, 153, 0, 0, 0,
0, 0, 0, 154, 155, 0, 0, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0,
165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167,
168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152,
266, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159,
160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 307, 0, 0,
171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154,
155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169,
170, 0, 31, 0, 0, 399, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173,
153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161,
162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0, 0, 0,
172, 150, 0, 151, 0, 152, 462, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177,
178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31,
0, 0, 0, 0, 0, 0, 171, 514, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174,
175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 546, 0, 0, 172, 150, 0,
151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157,
158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0,
0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 590, 173, 153, 0, 174, 175, 176, 0, 0,
154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165,
166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168,
169, 170, 0, 31, 0, 0, 628, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 0,
173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160,
161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 630, 0, 0, 0, 171, 0,
0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155,
177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170,
0, 31, 0, 0, 0, 0, 0, 0, 171, 677, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153,
0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0,
0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 680, 0, 0, 172,
150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178,
156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0,
683, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175,
176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164,
0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29,
167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 708, 0, 0, 172, 150, 0, 151, 0,
152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159,
160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171,
712, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154,
155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169,
170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 714, 0, 0, 172, 150, 0, 151, 0, 152, 0, 173,
153, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161,
162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 738, 0, 0,
172, 150, 0, 151, 0, 152, 0, 173, 153, 0, 174, 175, 176, 0, 0, 154, 155, 177,
178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 163, 164, 0, 165, 166, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31,
0, 0, 0, 0, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174, 175,
176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 0, 164,
0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29,
167, 168, 169, 170, 0, 31, 0, 0, 0, 307, 0, 0, 171, 0, 0, 0, 172, 150, 0, 151, 0,
0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159,
160, 161, 162, 0, 0, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 605, 0, 0, 171,
0, 0, 0, 172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 154, 155,
177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 0, 164, 0, 165, 166, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0,
31, 0, 0, 0, 0, 0, 0, 303, 0, 0, 0, 172, 150, 0, 151, 0, 0, 0, 173, 0, 0, 174,
175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159, 160, 161, 162, 0, 0, 0,
164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 315, 0, 0, 0, 172, 150, 0, 151,
0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 154, 155, 177, 178, 156, 157, 158, 159,
160, 161, 162, 0, 0, 0, 164, 0, 165, 166, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 168, 169, 170, 0, 31, 0, 0, 0, 0, 0, 0, 171, 0,
0, 0, 172, 0, 0, 0, 0, 0, 0, 173, 0, 0, 174, 175, 176, 0, 0, 0, 0, 177, 178};
static short Cyc_yycheck[ 4260u]={ 54, 86, 249, 81, 54, 83, 81, 61, 83, 153, 153,
86, 22, 57, 116, 54, 115, 54, 56, 366, 280, 218, 302, 55, 562, 103, 421, 674,
103, 171, 417, 152, 81, 0, 17, 87, 206, 438, 116, 73, 115, 116, 90, 444, 98, 99,
100, 101, 98, 5, 6, 103, 88, 86, 21, 22, 110, 24, 112, 69, 110, 98, 112, 458, 74,
98, 33, 454, 74, 88, 138, 318, 357, 40, 108, 112, 95, 362, 45, 147, 113, 88, 87,
88, 51, 289, 53, 119, 106, 56, 17, 18, 19, 171, 741, 299, 171, 88, 106, 106, 638,
109, 69, 94, 78, 88, 753, 74, 114, 68, 101, 758, 117, 760, 96, 81, 40, 83, 403,
110, 86, 93, 101, 90, 91, 92, 93, 528, 102, 33, 104, 110, 104, 106, 108, 87, 214,
103, 138, 214, 107, 45, 422, 99, 103, 98, 113, 147, 107, 115, 116, 69, 70, 207,
100, 122, 88, 207, 125, 101, 127, 303, 94, 109, 110, 560, 90, 91, 92, 93, 207,
93, 88, 315, 88, 97, 46, 47, 48, 95, 147, 95, 104, 721, 107, 112, 113, 114, 106,
107, 97, 586, 115, 268, 101, 249, 68, 88, 122, 249, 268, 206, 93, 94, 98, 171,
98, 285, 286, 113, 285, 286, 287, 104, 258, 173, 174, 295, 176, 287, 295, 98,
297, 101, 101, 303, 88, 302, 303, 297, 68, 103, 94, 112, 98, 107, 194, 315, 205,
101, 315, 120, 121, 147, 133, 219, 295, 103, 214, 216, 120, 97, 98, 220, 378,
111, 453, 311, 605, 291, 381, 299, 386, 650, 318, 103, 136, 137, 318, 107, 280,
88, 226, 97, 98, 555, 243, 94, 232, 101, 234, 235, 171, 288, 101, 101, 98, 88,
110, 101, 112, 113, 114, 110, 106, 112, 113, 114, 97, 93, 378, 205, 307, 378,
103, 99, 97, 477, 386, 314, 93, 386, 216, 280, 281, 98, 321, 438, 578, 285, 286,
287, 97, 444, 409, 102, 103, 97, 103, 295, 297, 297, 299, 103, 409, 595, 302,
303, 103, 243, 97, 395, 231, 108, 311, 395, 103, 422, 89, 315, 103, 356, 357, 96,
614, 430, 410, 362, 101, 556, 410, 366, 430, 106, 466, 96, 465, 110, 97, 102,
101, 410, 93, 410, 103, 106, 88, 97, 99, 110, 97, 97, 94, 103, 342, 343, 344,
103, 466, 101, 465, 466, 101, 297, 64, 65, 66, 106, 403, 100, 112, 113, 114, 370,
409, 372, 103, 528, 87, 414, 97, 691, 378, 97, 303, 389, 103, 97, 520, 103, 386,
310, 90, 103, 313, 595, 315, 88, 67, 68, 440, 101, 464, 94, 95, 445, 101, 98,
327, 109, 110, 701, 520, 409, 614, 520, 370, 99, 372, 99, 103, 103, 406, 103,
408, 108, 422, 601, 601, 103, 103, 88, 467, 107, 431, 108, 102, 94, 115, 436, 97,
718, 477, 440, 101, 364, 88, 99, 445, 99, 555, 103, 101, 103, 37, 112, 113, 114,
609, 99, 660, 71, 72, 101, 741, 5, 6, 551, 101, 465, 466, 551, 391, 392, 393,
101, 395, 431, 115, 116, 101, 758, 101, 480, 551, 102, 103, 73, 562, 102, 103,
118, 600, 102, 103, 600, 102, 103, 486, 102, 103, 701, 102, 103, 119, 88, 89,
101, 91, 92, 37, 94, 89, 90, 99, 599, 99, 99, 101, 599, 117, 104, 105, 99, 520,
98, 109, 110, 99, 112, 527, 98, 599, 116, 117, 22, 23, 534, 578, 98, 334, 335,
336, 337, 102, 103, 480, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 102, 103, 555,
595, 88, 89, 101, 91, 92, 101, 94, 102, 103, 605, 101, 100, 608, 101, 622, 623,
104, 105, 614, 101, 578, 109, 110, 97, 112, 691, 702, 88, 116, 117, 699, 97, 527,
699, 101, 37, 702, 102, 103, 150, 108, 152, 153, 600, 102, 103, 88, 102, 103,
102, 103, 100, 94, 99, 165, 166, 621, 102, 103, 101, 540, 541, 173, 174, 660,
176, 108, 332, 333, 102, 112, 113, 114, 718, 338, 339, 102, 718, 99, 624, 111,
626, 99, 194, 685, 103, 88, 89, 100, 91, 92, 102, 94, 340, 341, 103, 741, 74, 99,
101, 741, 35, 104, 105, 102, 701, 106, 97, 103, 108, 112, 28, 97, 758, 116, 117,
41, 758, 93, 88, 87, 37, 102, 115, 102, 101, 88, 685, 99, 103, 103, 88, 677, 93,
691, 680, 102, 102, 102, 111, 103, 102, 699, 102, 689, 702, 101, 627, 628, 102,
630, 99, 632, 102, 106, 99, 103, 102, 703, 102, 99, 99, 707, 708, 101, 98, 711,
712, 97, 714, 102, 107, 88, 89, 102, 91, 92, 657, 94, 99, 111, 103, 35, 102, 102,
101, 98, 732, 104, 105, 735, 102, 737, 738, 98, 97, 112, 111, 88, 679, 116, 117,
682, 683, 97, 102, 751, 102, 102, 26, 102, 102, 101, 101, 99, 326, 99, 328, 329,
330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 108,
97, 716, 102, 102, 30, 0, 0, 722, 568, 35, 436, 37, 311, 39, 205, 527, 42, 293,
642, 115, 578, 214, 446, 49, 50, 740, 281, 53, 54, 55, 56, 57, 58, 59, 326, 381,
62, 63, 329, 65, 66, 328, 331, - 1, 330, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105, - 1, - 1, - 1, 109, 110, - 1, 112,
- 1, - 1, 115, 116, 117, 438, - 1, - 1, - 1, 122, 123, 444, - 1, - 1, - 1, 3, 4,
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -
1, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 486, 42, 43, 44, - 1,
- 1, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, - 1, 65,
66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, 528, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97, 98, - 1, -
1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115,
116, 117, - 1, - 1, - 1, - 1, 122, 123, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 601, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
609, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, -
1, 24, 25, - 1, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, - 1, 42,
43, 44, - 1, - 1, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97,
98, 99, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, -
1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, 25, - 1, 27, 28, 29, 30, 31,
32, 33, 34, 35, 36, 37, 38, 39, 40, - 1, 42, 43, 44, - 1, - 1, 47, 48, 49, 50,
51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, - 1, 65, 66, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, 97, 98, - 1, - 1, 101, - 1, - 1,
104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1,
- 1, - 1, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 35, 36,
37, 38, 39, - 1, - 1, 42, 43, - 1, - 1, - 1, 47, 48, 49, 50, - 1, - 1, 53, 54,
55, 56, 57, 58, 59, 60, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, 93, 94, 95, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, 104, 105, - 1,
- 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122,
123, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 35, 36, 37, - 1, 39, - 1, - 1, 42, -
1, - 1, - 1, - 1, 47, 48, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, 60, - 1,
62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1,
- 1, 98, - 1, - 1, 101, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, -
1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 8, 9, 10, 11, 12, 13, 14,
15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, 35, 36, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1, 47, 48, 49,
50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, 60, - 1, 62, 63, - 1, 65, 66, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 88, 89, 90, 91, 92, 93, 94, 95, - 1, - 1, - 1, - 1, - 1, 101, - 1,
- 1, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1,
- 1, - 1, - 1, 122, 123, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
36, - 1, 38, - 1, - 1, - 1, - 1, 43, - 1, 45, 46, 47, 48, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1, - 1, 88, - 1, - 1, -
1, - 1, 93, 94, 95, 36, - 1, - 1, - 1, - 1, 101, - 1, 43, 104, 105, - 1, 47, 48,
- 1, - 1, - 1, 112, 113, 114, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, - 1, - 1, 36, - 1, 93, 94, 95, - 1, - 1, - 1, - 1, - 1,
101, 47, 48, 104, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, 113, 114, 60, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, - 1, 85, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93, 94, 95, 36, 97, - 1, - 1,
- 1, 101, 102, - 1, 104, 105, - 1, 47, 48, - 1, 110, - 1, 112, 113, 114, - 1, -
1, - 1, - 1, - 1, 60, - 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
17, 18, 19, 20, 21, - 1, - 1, 24, 85, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, - 1,
95, 36, 97, 38, - 1, - 1, 101, 102, 43, 104, 105, - 1, 47, 48, - 1, 110, - 1,
112, 113, 114, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1, - 1, - 1, - 1, 3, 4, 5, 6,
7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, 24, - 1, - 1,
- 1, - 1, - 1, 93, - 1, 95, - 1, - 1, 98, 36, 100, 38, - 1, - 1, 104, 105, 43, -
1, - 1, - 1, 47, 48, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 60,
- 1, - 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, -
1, - 1, 24, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, - 1, 95, 36, - 1, 98, -
1, - 1, - 1, - 1, 43, 104, 105, - 1, 47, 48, 25, - 1, 27, 28, 29, 30, 31, 32, 33,
34, 35, 60, 37, - 1, 39, 40, - 1, 42, - 1, 44, - 1, - 1, - 1, - 1, 49, 50, 51,
52, 53, 54, 55, 56, 57, 58, 59, - 1, 61, 62, 63, - 1, 65, 66, - 1, - 1, 93, - 1,
95, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 104, 105, - 1, - 1, - 1, - 1, - 1, -
1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, 97, 98, - 1, - 1, 101, - 1, - 1, - 1,
105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1,
- 1, 122, 123, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 36, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 47, 48, - 1, 36, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
60, 47, 48, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 60, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 85, - 1, - 1, 88, - 1, - 1, - 1, - 1, 93,
- 1, 95, - 1, 97, - 1, 85, - 1, - 1, 102, - 1, 104, 105, - 1, 93, - 1, 95, - 1,
97, - 1, - 1, - 1, - 1, 102, - 1, 104, 105, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
18, 19, 20, 21, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 36, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 47, 48, - 1, 36, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 60, 47, 48, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, 60, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, - 1, - 1, -
1, - 1, 93, - 1, 95, - 1, - 1, 98, 85, - 1, - 1, - 1, 36, 104, 105, - 1, 93, - 1,
95, - 1, - 1, - 1, - 1, 47, 48, - 1, - 1, 104, 105, - 1, 35, - 1, 37, - 1, 39, -
1, 60, 42, - 1, - 1, - 1, - 1, - 1, - 1, 49, 50, - 1, - 1, 53, 54, 55, 56, 57,
58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 93, -
1, 95, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 104, 105, - 1, 88, 89, 90, 91, 92,
- 1, 94, - 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105, - 1, - 1, - 1, 109,
110, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1, 122, 123, 35, - 1,
37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1, - 1, - 1, 49, 50, - 1, - 1, 53,
54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, 99, - 1, 101, - 1, - 1, - 1, 105,
- 1, - 1, - 1, 109, 110, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, - 1, - 1,
122, 123, 35, - 1, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1, - 1, - 1, - 1, 49,
50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, 98, - 1, - 1, 101,
- 1, - 1, - 1, 105, - 1, - 1, - 1, 109, 110, - 1, 112, - 1, - 1, 115, 116, 117,
- 1, - 1, - 1, - 1, 122, 123, 35, - 1, 37, - 1, 39, - 1, - 1, 42, - 1, - 1, - 1,
- 1, - 1, - 1, 49, 50, - 1, - 1, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, -
1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, 111, 112, 42, - 1,
115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1,
62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112,
42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59,
- 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39,
- 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57,
58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91,
92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37,
- 1, 39, 111, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54,
55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105,
35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122,
123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1,
- 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49,
50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101,
- 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, 111, 112, 42, - 1, 115, 116, 117, - 1,
- 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, 97, - 1, - 1, -
1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116,
117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, 97, -
1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1,
115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1,
62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112,
42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59,
- 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105, 35, - 1, 37, - 1, 39,
- 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57,
58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91,
92, - 1, 94, - 1, - 1, 97, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37,
- 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54,
55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88,
89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1, - 1, 105,
35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122,
123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, 102, - 1,
- 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1, - 1, 49,
50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101,
102, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116, 117, - 1,
- 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, - 1, 65, 66,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, -
1, 101, 102, - 1, - 1, 105, 35, - 1, 37, - 1, 39, - 1, 112, 42, - 1, 115, 116,
117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, 62, 63, -
1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, -
1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, - 1, - 1, 112, - 1, - 1,
115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1,
- 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, -
1, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, - 1, - 1, 112,
- 1, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56, 57, 58, 59,
- 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90, 91, 92, - 1,
94, - 1, - 1, - 1, 98, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1, 37, - 1, - 1,
- 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53, 54, 55, 56,
57, 58, 59, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 88, 89, 90,
91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1, 105, 35, - 1,
37, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 49, 50, 122, 123, 53,
54, 55, 56, 57, 58, 59, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, - 1, - 1, - 1,
105, 35, - 1, 37, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, - 1, - 1, 49, 50,
122, 123, 53, 54, 55, 56, 57, 58, 59, - 1, - 1, - 1, 63, - 1, 65, 66, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 88, 89, 90, 91, 92, - 1, 94, - 1, - 1, - 1, - 1, - 1, - 1, 101, -
1, - 1, - 1, 105, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, 115, 116, 117, -
1, - 1, - 1, - 1, 122, 123}; unsigned char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
unsigned char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow";
extern void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex();
unsigned char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL"; static
int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static
int Cyc_yynerrs= 0; struct _tagged_ptr1{ short* curr; short* base; short*
last_plus_one; } ; struct _tagged_ptr2{ void** curr; void** base; void**
last_plus_one; } ; struct _tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype*
base; struct Cyc_Yyltype* last_plus_one; } ; struct _tuple19{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate; int yyn= 0;
int yyerrstatus; int yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({
short* _temp4259=({ unsigned int _temp4256= 200u; short* _temp4257=( short*)
GC_malloc_atomic( sizeof( short) * _temp4256);{ unsigned int _temp4258=
_temp4256; unsigned int i; for( i= 0; i < _temp4258; i ++){ _temp4257[ i]= 0;}};
_temp4257;}); struct _tagged_ptr1 _temp4260; _temp4260.curr= _temp4259;
_temp4260.base= _temp4259; _temp4260.last_plus_one= _temp4259 + 200; _temp4260;});
int yyvsp_offset; struct _tagged_ptr2 yyvs=({ void** _temp4254=({ unsigned int
_temp4251= 200u; void** _temp4252=( void**) GC_malloc( sizeof( void*) *
_temp4251);{ unsigned int _temp4253= _temp4251; unsigned int i; for( i= 0; i <
_temp4253; i ++){ _temp4252[ i]= Cyc_yylval;}}; _temp4252;}); struct
_tagged_ptr2 _temp4255; _temp4255.curr= _temp4254; _temp4255.base= _temp4254;
_temp4255.last_plus_one= _temp4254 + 200; _temp4255;}); int yylsp_offset; struct
_tagged_ptr3 yyls=({ struct Cyc_Yyltype* _temp4249=({ unsigned int _temp4246=
200u; struct Cyc_Yyltype* _temp4247=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp4246);{ unsigned int _temp4248= _temp4246;
unsigned int i; for( i= 0; i < _temp4248; i ++){ _temp4247[ i]= Cyc_yynewloc();}};
_temp4247;}); struct _tagged_ptr3 _temp4250; _temp4250.curr= _temp4249;
_temp4250.base= _temp4249; _temp4250.last_plus_one= _temp4249 + 200; _temp4250;});
int yystacksize= 200; void* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), ++ yyssp_offset)=(
short) yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){
Cyc_yyerror(({ unsigned char* _temp2243="parser stack overflow"; struct
_tagged_string _temp2244; _temp2244.curr= _temp2243; _temp2244.base= _temp2243;
_temp2244.last_plus_one= _temp2243 + 22; _temp2244;}));( void) _throw(( void*)
Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){ yystacksize=
10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2253=( unsigned int)
yystacksize; short* _temp2254=( short*) GC_malloc_atomic( sizeof( short) *
_temp2253); struct _tagged_ptr1 _temp2256={ _temp2254, _temp2254, _temp2254 +
_temp2253};{ unsigned int _temp2255= _temp2253; unsigned int i; for( i= 0; i <
_temp2255; i ++){ _temp2254[ i]= 0;}}; _temp2256;}); struct _tagged_ptr2 yyvs1=({
unsigned int _temp2249=( unsigned int) yystacksize; void** _temp2250=( void**)
GC_malloc( sizeof( void*) * _temp2249); struct _tagged_ptr2 _temp2252={
_temp2250, _temp2250, _temp2250 + _temp2249};{ unsigned int _temp2251= _temp2249;
unsigned int i; for( i= 0; i < _temp2251; i ++){ _temp2250[ i]= Cyc_yylval;}};
_temp2252;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2245=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2246=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2245); struct _tagged_ptr3 _temp2248={
_temp2246, _temp2246, _temp2246 + _temp2245};{ unsigned int _temp2247= _temp2245;
unsigned int i; for( i= 0; i < _temp2247; i ++){ _temp2246[ i]= Cyc_yynewloc();}};
_temp2248;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*(( short*(*)( struct
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
766u, yystate)]; if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar == - 2){
Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar= 0;}
else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int) Cyc_yytranslate[
_check_known_subscript_notnull( 352u, Cyc_yychar)]: 234;} yyn += yychar1; if((
yyn < 0? 1: yyn > 4259)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4260u,
yyn)] != yychar1){ goto yydefault;} yyn=( int) Cyc_yytable[
_check_known_subscript_notnull( 4260u, yyn)]; if( yyn < 0){ if( yyn == - 32768){
goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}}
if( yyn == 765){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; if( yyerrstatus
!= 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int) Cyc_yydefact[
_check_known_subscript_notnull( 766u, yystate)]; if( yyn == 0){ goto yyerrlab;}
yyreduce: yylen=( int) Cyc_yyr2[ _check_known_subscript_notnull( 400u, yyn)];
if( yylen > 0){ yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),( yyvsp_offset +
1) - yylen);} switch( yyn){ case 1: _LL2257: yyval=*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); break; case 2:
_LL2258: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2260=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2260[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2261; _temp2261.tag= Cyc_DeclList_tok; _temp2261.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2261;});
_temp2260;}); break; case 3: _LL2259: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2263=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2263[ 0]=({ struct Cyc_DeclList_tok_struct _temp2264; _temp2264.tag= Cyc_DeclList_tok;
_temp2264.f1=({ struct Cyc_List_List* _temp2265=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2265->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2266=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2266->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2267=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2267[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2268; _temp2268.tag= Cyc_Absyn_Using_d;
_temp2268.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2268.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2268;}); _temp2267;})); _temp2266->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2266;}); _temp2265->tl= 0; _temp2265;}); _temp2264;}); _temp2263;}); Cyc_Lex_leave_using();
break; case 4: _LL2262: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2270=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2270[ 0]=({ struct Cyc_DeclList_tok_struct _temp2271; _temp2271.tag= Cyc_DeclList_tok;
_temp2271.f1=({ struct Cyc_List_List* _temp2272=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2272->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2273=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2273->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2274=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2274[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2275; _temp2275.tag= Cyc_Absyn_Using_d;
_temp2275.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2275.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2275;}); _temp2274;})); _temp2273->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2273;}); _temp2272->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2272;});
_temp2271;}); _temp2270;}); break; case 5: _LL2269: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2277=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2277[ 0]=({ struct Cyc_DeclList_tok_struct _temp2278; _temp2278.tag= Cyc_DeclList_tok;
_temp2278.f1=({ struct Cyc_List_List* _temp2279=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2279->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2280=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2280->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2281=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2281[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2282; _temp2282.tag=
Cyc_Absyn_Namespace_d; _temp2282.f1=({ struct _tagged_string* _temp2283=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2283[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2283;}); _temp2282.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2282;}); _temp2281;})); _temp2280->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2280;}); _temp2279->tl= 0; _temp2279;}); _temp2278;}); _temp2277;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2276: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2285=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2285[ 0]=({ struct Cyc_DeclList_tok_struct _temp2286; _temp2286.tag= Cyc_DeclList_tok;
_temp2286.f1=({ struct Cyc_List_List* _temp2287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2287->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2288=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2288->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2289=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2289[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2290; _temp2290.tag=
Cyc_Absyn_Namespace_d; _temp2290.f1=({ struct _tagged_string* _temp2291=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2291[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2291;}); _temp2290.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2290;}); _temp2289;})); _temp2288->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2288;}); _temp2287->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2287;});
_temp2286;}); _temp2285;}); break; case 7: _LL2284: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),({
unsigned char* _temp2293="C"; struct _tagged_string _temp2294; _temp2294.curr=
_temp2293; _temp2294.base= _temp2293; _temp2294.last_plus_one= _temp2293 + 2;
_temp2294;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp2295="only extern \"C\" { ... } is allowed";
struct _tagged_string _temp2296; _temp2296.curr= _temp2295; _temp2296.base=
_temp2295; _temp2296.last_plus_one= _temp2295 + 35; _temp2296;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2297=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2297[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2298; _temp2298.tag= Cyc_DeclList_tok; _temp2298.f1=({ struct Cyc_List_List*
_temp2299=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2299->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2300=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2300->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2301=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2301[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2302; _temp2302.tag= Cyc_Absyn_ExternC_d;
_temp2302.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2302;}); _temp2301;})); _temp2300->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2300;}); _temp2299->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2299;}); _temp2298;}); _temp2297;}); break;
case 8: _LL2292: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2304=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2304[ 0]=({ struct Cyc_DeclList_tok_struct _temp2305; _temp2305.tag= Cyc_DeclList_tok;
_temp2305.f1= 0; _temp2305;}); _temp2304;}); break; case 9: _LL2303: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2307=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2307[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2308; _temp2308.tag= Cyc_DeclList_tok; _temp2308.f1=({ struct Cyc_List_List*
_temp2309=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2309->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2310=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2310[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2311; _temp2311.tag= Cyc_Absyn_Fn_d;
_temp2311.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2311;}); _temp2310;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2309->tl= 0; _temp2309;}); _temp2308;}); _temp2307;}); break; case 10:
_LL2306: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2312: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2314=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2314[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2315; _temp2315.tag= Cyc_FnDecl_tok; _temp2315.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2315;}); _temp2314;}); break; case 12: _LL2313:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2317=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2317[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2318; _temp2318.tag= Cyc_FnDecl_tok; _temp2318.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2319=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2319->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2319;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2318;}); _temp2317;}); break; case 13: _LL2316: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2321=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2321[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2322; _temp2322.tag= Cyc_FnDecl_tok;
_temp2322.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
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
_temp2322;}); _temp2321;}); break; case 14: _LL2320: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2324=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2324[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2325; _temp2325.tag= Cyc_FnDecl_tok;
_temp2325.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2326=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2326->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2326;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2325;});
_temp2324;}); break; case 15: _LL2323: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2328=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2328[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2329; _temp2329.tag= Cyc_FnDecl_tok;
_temp2329.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2330=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2330->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2330;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2329;});
_temp2328;}); break; case 16: _LL2327: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2332=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2332[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2333; _temp2333.tag= Cyc_FnDecl_tok;
_temp2333.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2334=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2334->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2334;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2333;});
_temp2332;}); break; case 17: _LL2331: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2335: Cyc_Lex_leave_using();
break; case 19: _LL2336: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2338=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2338[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2338;})); yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2337: Cyc_Lex_leave_namespace(); break; case
21: _LL2339: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2341=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2341[ 0]=({ struct Cyc_DeclList_tok_struct _temp2342; _temp2342.tag= Cyc_DeclList_tok;
_temp2342.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2342;}); _temp2341;}); break; case 22: _LL2340: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2344=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2344[ 0]=({ struct Cyc_DeclList_tok_struct _temp2345; _temp2345.tag= Cyc_DeclList_tok;
_temp2345.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2345;}); _temp2344;}); break; case 23: _LL2343: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2347=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2347[ 0]=({ struct Cyc_DeclList_tok_struct _temp2348; _temp2348.tag= Cyc_DeclList_tok;
_temp2348.f1=({ struct Cyc_List_List* _temp2349=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2349->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2349->tl= 0; _temp2349;}); _temp2348;});
_temp2347;}); break; case 24: _LL2346: { struct Cyc_List_List* _temp2351= 0;
goto _LL2352; _LL2352:{ struct Cyc_List_List* _temp2353= Cyc_yyget_IdList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); goto
_LL2354; _LL2354: for( 0; _temp2353 != 0; _temp2353=(( struct Cyc_List_List*)
_check_null( _temp2353))->tl){ struct _tagged_string* _temp2355=( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2353))->hd; goto
_LL2356; _LL2356: { struct _tuple1* _temp2360=({ struct _tuple1* _temp2357=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2357->f1=( void*)({
struct Cyc_Absyn_Rel_n_struct* _temp2358=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2358[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp2359; _temp2359.tag= Cyc_Absyn_Rel_n; _temp2359.f1= 0; _temp2359;});
_temp2358;}); _temp2357->f2= _temp2355; _temp2357;}); goto _LL2361; _LL2361: {
struct Cyc_Absyn_Vardecl* _temp2362= Cyc_Absyn_new_vardecl( _temp2360, Cyc_Absyn_wildtyp(),
0); goto _LL2363; _LL2363: _temp2351=({ struct Cyc_List_List* _temp2364=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2364->hd=( void*)
_temp2362; _temp2364->tl= _temp2351; _temp2364;});}}}} _temp2351=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2351); yyval=( void*)({ struct
Cyc_DeclList_tok_struct* _temp2365=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp2365[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2366; _temp2366.tag= Cyc_DeclList_tok; _temp2366.f1=({ struct Cyc_List_List*
_temp2367=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2367->hd=( void*) Cyc_Absyn_letv_decl( _temp2351, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2367->tl= 0; _temp2367;}); _temp2366;}); _temp2365;}); break;} case 25:
_LL2350: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 26:
_LL2368: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2370=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2370[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2371; _temp2371.tag= Cyc_DeclList_tok; _temp2371.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2371;});
_temp2370;}); break; case 27: _LL2369: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2373=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2373[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2374; _temp2374.tag= Cyc_DeclSpec_tok;
_temp2374.f1=({ struct Cyc_Parse_Declaration_spec* _temp2375=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2375->sc=({ struct
Cyc_Core_Opt* _temp2376=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2376->v=( void*) Cyc_yyget_StorageClass_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2376;}); _temp2375->tq= Cyc_Absyn_empty_tqual();
_temp2375->type_specs= 0; _temp2375->is_inline= 0; _temp2375->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2375;});
_temp2374;}); _temp2373;}); break; case 28: _LL2372: if(( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(({
unsigned char* _temp2378="Only one storage class is allowed in a declaration";
struct _tagged_string _temp2379; _temp2379.curr= _temp2378; _temp2379.base=
_temp2378; _temp2379.last_plus_one= _temp2378 + 51; _temp2379;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2380=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2380[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2381; _temp2381.tag= Cyc_DeclSpec_tok; _temp2381.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2382=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2382->sc=({ struct Cyc_Core_Opt* _temp2383=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2383->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2383;});
_temp2382->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2382->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2382->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2382->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2382;}); _temp2381;}); _temp2380;}); break; case 29: _LL2377: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2385=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2385[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2386; _temp2386.tag= Cyc_DeclSpec_tok; _temp2386.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2387=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2387->sc= 0; _temp2387->tq= Cyc_Absyn_empty_tqual(); _temp2387->type_specs=({
struct Cyc_List_List* _temp2388=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2388->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2388->tl=
0; _temp2388;}); _temp2387->is_inline= 0; _temp2387->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2387;});
_temp2386;}); _temp2385;}); break; case 30: _LL2384: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2390=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2390[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2391; _temp2391.tag= Cyc_DeclSpec_tok;
_temp2391.f1=({ struct Cyc_Parse_Declaration_spec* _temp2392=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2392->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2392->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2392->type_specs=({ struct Cyc_List_List* _temp2393=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2393->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2393->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2393;}); _temp2392->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2392->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2392;}); _temp2391;}); _temp2390;}); break; case 31: _LL2389: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2395=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2395[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2396; _temp2396.tag= Cyc_DeclSpec_tok; _temp2396.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2397=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2397->sc= 0; _temp2397->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2397->type_specs= 0; _temp2397->is_inline=
0; _temp2397->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2397;}); _temp2396;}); _temp2395;}); break;
case 32: _LL2394: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2399=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2399[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2400; _temp2400.tag= Cyc_DeclSpec_tok;
_temp2400.f1=({ struct Cyc_Parse_Declaration_spec* _temp2401=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2401->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2401->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2401->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2401->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2401->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2401;}); _temp2400;}); _temp2399;}); break; case 33: _LL2398: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2403=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2403[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2404; _temp2404.tag= Cyc_DeclSpec_tok; _temp2404.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2405=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2405->sc= 0; _temp2405->tq= Cyc_Absyn_empty_tqual(); _temp2405->type_specs=
0; _temp2405->is_inline= 1; _temp2405->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2405;});
_temp2404;}); _temp2403;}); break; case 34: _LL2402: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2407=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2407[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2408; _temp2408.tag= Cyc_DeclSpec_tok;
_temp2408.f1=({ struct Cyc_Parse_Declaration_spec* _temp2409=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2409->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2409->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2409->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2409->is_inline= 1; _temp2409->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2409;}); _temp2408;}); _temp2407;}); break; case 35: _LL2406: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2411=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2411[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2412; _temp2412.tag= Cyc_StorageClass_tok;
_temp2412.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2412;}); _temp2411;});
break; case 36: _LL2410: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2414=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2414[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2415; _temp2415.tag=
Cyc_StorageClass_tok; _temp2415.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2415;}); _temp2414;}); break; case 37: _LL2413: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2417=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2417[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2418; _temp2418.tag=
Cyc_StorageClass_tok; _temp2418.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2418;}); _temp2417;}); break; case 38: _LL2416: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2420=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2420[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2421; _temp2421.tag=
Cyc_StorageClass_tok; _temp2421.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2421;}); _temp2420;}); break; case 39: _LL2419: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),({ unsigned char*
_temp2423="C"; struct _tagged_string _temp2424; _temp2424.curr= _temp2423;
_temp2424.base= _temp2423; _temp2424.last_plus_one= _temp2423 + 2; _temp2424;}))
!= 0){ Cyc_Parse_err(({ unsigned char* _temp2425="only extern or extern \"C\" is allowed";
struct _tagged_string _temp2426; _temp2426.curr= _temp2425; _temp2426.base=
_temp2425; _temp2426.last_plus_one= _temp2425 + 37; _temp2426;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2427=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2427[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2428; _temp2428.tag= Cyc_StorageClass_tok;
_temp2428.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2428;}); _temp2427;});
break; case 40: _LL2422: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2430=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2430[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2431; _temp2431.tag=
Cyc_StorageClass_tok; _temp2431.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2431;}); _temp2430;}); break; case 41: _LL2429: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2433=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2433[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2434; _temp2434.tag=
Cyc_StorageClass_tok; _temp2434.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2434;}); _temp2433;}); break; case 42: _LL2432: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2436=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2436[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2437; _temp2437.tag=
Cyc_AttributeList_tok; _temp2437.f1= 0; _temp2437;}); _temp2436;}); break; case
43: _LL2435: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 44: _LL2438: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2440=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2440[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2441; _temp2441.tag=
Cyc_AttributeList_tok; _temp2441.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2441;}); _temp2440;}); break;
case 45: _LL2439: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2443=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2443[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2444; _temp2444.tag=
Cyc_AttributeList_tok; _temp2444.f1=({ struct Cyc_List_List* _temp2445=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2445->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2445->tl= 0; _temp2445;}); _temp2444;}); _temp2443;}); break; case 46:
_LL2442: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2447=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2447[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2448; _temp2448.tag=
Cyc_AttributeList_tok; _temp2448.f1=({ struct Cyc_List_List* _temp2449=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2449->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2449->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2449;}); _temp2448;}); _temp2447;}); break; case 47:
_LL2446: { struct _tagged_string _temp2451= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2452; _LL2452: if((((({ struct
_tagged_string _temp2453= _temp2451;( unsigned int)( _temp2453.last_plus_one -
_temp2453.curr);}) > 4?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2451, sizeof(
unsigned char), 0) =='_': 0)?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2451, sizeof(
unsigned char), 1) =='_': 0)?*(( unsigned char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( _temp2451, sizeof(
unsigned char),( int)(({ struct _tagged_string _temp2454= _temp2451;(
unsigned int)( _temp2454.last_plus_one - _temp2454.curr);}) - 2)) =='_': 0)?*((
unsigned char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2451, sizeof( unsigned char),( int)(({ struct
_tagged_string _temp2455= _temp2451;( unsigned int)( _temp2455.last_plus_one -
_temp2455.curr);}) - 3)) =='_': 0){ _temp2451= Cyc_String_substring( _temp2451,
2,({ struct _tagged_string _temp2456= _temp2451;( unsigned int)( _temp2456.last_plus_one
- _temp2456.curr);}) - 5);}{ int i= 0; for( 0; i < 16u; ++ i){ if( Cyc_String_strcmp(
_temp2451,( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f1) ==
0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2457=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2457[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2458; _temp2458.tag= Cyc_Attribute_tok; _temp2458.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 16u, i)]).f2;
_temp2458;}); _temp2457;}); break;}} if( i == 16u){ Cyc_Parse_err(({
unsigned char* _temp2459="unrecognized attribute"; struct _tagged_string
_temp2460; _temp2460.curr= _temp2459; _temp2460.base= _temp2459; _temp2460.last_plus_one=
_temp2459 + 23; _temp2460;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); yyval=( void*)({
struct Cyc_Attribute_tok_struct* _temp2461=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2461[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2462; _temp2462.tag= Cyc_Attribute_tok; _temp2462.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp2462;}); _temp2461;});} break;}} case
48: _LL2450: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2464=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2464[ 0]=({ struct Cyc_Attribute_tok_struct _temp2465; _temp2465.tag= Cyc_Attribute_tok;
_temp2465.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2465;}); _temp2464;});
break; case 49: _LL2463: { struct _tagged_string _temp2467= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); goto
_LL2468; _LL2468: { struct _tuple16 _temp2471; int _temp2472; void* _temp2474;
struct _tuple16* _temp2469= Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2471=* _temp2469; _LL2475: _temp2474= _temp2471.f1;
goto _LL2473; _LL2473: _temp2472= _temp2471.f2; goto _LL2470; _LL2470: { void* a;
if( Cyc_String_zstrcmp( _temp2467,({ unsigned char* _temp2476="regparm"; struct
_tagged_string _temp2477; _temp2477.curr= _temp2476; _temp2477.base= _temp2476;
_temp2477.last_plus_one= _temp2476 + 8; _temp2477;})) == 0? 1: Cyc_String_zstrcmp(
_temp2467,({ unsigned char* _temp2478="__regparm__"; struct _tagged_string
_temp2479; _temp2479.curr= _temp2478; _temp2479.base= _temp2478; _temp2479.last_plus_one=
_temp2478 + 12; _temp2479;})) == 0){ if( _temp2472 <= 0? 1: _temp2472 > 3){ Cyc_Parse_err(({
unsigned char* _temp2480="regparm requires value between 1 and 3"; struct
_tagged_string _temp2481; _temp2481.curr= _temp2480; _temp2481.base= _temp2480;
_temp2481.last_plus_one= _temp2480 + 39; _temp2481;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2482=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2482[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2483; _temp2483.tag= Cyc_Absyn_Regparm_att;
_temp2483.f1= _temp2472; _temp2483;}); _temp2482;});} else{ if( Cyc_String_zstrcmp(
_temp2467,({ unsigned char* _temp2484="aligned"; struct _tagged_string _temp2485;
_temp2485.curr= _temp2484; _temp2485.base= _temp2484; _temp2485.last_plus_one=
_temp2484 + 8; _temp2485;})) == 0? 1: Cyc_String_zstrcmp( _temp2467,({
unsigned char* _temp2486="__aligned__"; struct _tagged_string _temp2487;
_temp2487.curr= _temp2486; _temp2487.base= _temp2486; _temp2487.last_plus_one=
_temp2486 + 12; _temp2487;})) == 0){ if( _temp2472 < 0){ Cyc_Parse_err(({
unsigned char* _temp2488="aligned requires positive power of two"; struct
_tagged_string _temp2489; _temp2489.curr= _temp2488; _temp2489.base= _temp2488;
_temp2489.last_plus_one= _temp2488 + 39; _temp2489;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2472; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(({ unsigned char* _temp2490="aligned requires positive power of two";
struct _tagged_string _temp2491; _temp2491.curr= _temp2490; _temp2491.base=
_temp2490; _temp2491.last_plus_one= _temp2490 + 39; _temp2491;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2492=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2492[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2493; _temp2493.tag= Cyc_Absyn_Aligned_att;
_temp2493.f1= _temp2472; _temp2493;}); _temp2492;});}} else{ Cyc_Parse_err(({
unsigned char* _temp2494="unrecognized attribute"; struct _tagged_string
_temp2495; _temp2495.curr= _temp2494; _temp2495.base= _temp2494; _temp2495.last_plus_one=
_temp2494 + 23; _temp2495;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line)); a=( void*)
Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2496=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2496[ 0]=({ struct Cyc_Attribute_tok_struct _temp2497; _temp2497.tag= Cyc_Attribute_tok;
_temp2497.f1=( void*) a; _temp2497;}); _temp2496;}); break;}}} case 50: _LL2466: {
struct _tagged_string _temp2499= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2500; _LL2500: { struct
_tagged_string _temp2501= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2502; _LL2502: { void* a; if( Cyc_String_zstrcmp(
_temp2499,({ unsigned char* _temp2503="section"; struct _tagged_string _temp2504;
_temp2504.curr= _temp2503; _temp2504.base= _temp2503; _temp2504.last_plus_one=
_temp2503 + 8; _temp2504;})) == 0? 1: Cyc_String_zstrcmp( _temp2499,({
unsigned char* _temp2505="__section__"; struct _tagged_string _temp2506;
_temp2506.curr= _temp2505; _temp2506.base= _temp2505; _temp2506.last_plus_one=
_temp2505 + 12; _temp2506;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2507=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2507[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2508; _temp2508.tag=
Cyc_Absyn_Section_att; _temp2508.f1= _temp2501; _temp2508;}); _temp2507;});}
else{ Cyc_Parse_err(({ unsigned char* _temp2509="unrecognized attribute"; struct
_tagged_string _temp2510; _temp2510.curr= _temp2509; _temp2510.base= _temp2509;
_temp2510.last_plus_one= _temp2509 + 23; _temp2510;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2511=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2511[ 0]=({ struct Cyc_Attribute_tok_struct _temp2512; _temp2512.tag= Cyc_Attribute_tok;
_temp2512.f1=( void*) a; _temp2512;}); _temp2511;}); break;}}} case 51: _LL2498:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2514=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2514[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2515; _temp2515.tag= Cyc_TypeSpecifier_tok;
_temp2515.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2515;}); _temp2514;}); break; case 52: _LL2513: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2517=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2517[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2518; _temp2518.tag=
Cyc_TypeSpecifier_tok; _temp2518.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2518;});
_temp2517;}); break; case 53: _LL2516: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2520=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2520[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2521; _temp2521.tag=
Cyc_TypeSpecifier_tok; _temp2521.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2521;}); _temp2520;}); break; case 54: _LL2519:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2523=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2523[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2524; _temp2524.tag= Cyc_TypeSpecifier_tok;
_temp2524.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2525=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2525[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2526; _temp2526.tag=
Cyc_Parse_Short_spec; _temp2526.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2526;});
_temp2525;})); _temp2524;}); _temp2523;}); break; case 55: _LL2522: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2528=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2528[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2529; _temp2529.tag= Cyc_TypeSpecifier_tok;
_temp2529.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2529;}); _temp2528;}); break; case 56: _LL2527: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2531=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2531[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2532; _temp2532.tag=
Cyc_TypeSpecifier_tok; _temp2532.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2533=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2533[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2534; _temp2534.tag= Cyc_Parse_Long_spec;
_temp2534.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2534;});
_temp2533;})); _temp2532;}); _temp2531;}); break; case 57: _LL2530: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2536=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2536[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2537; _temp2537.tag= Cyc_TypeSpecifier_tok;
_temp2537.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2537;}); _temp2536;}); break; case 58: _LL2535: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2539=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2539[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2540; _temp2540.tag=
Cyc_TypeSpecifier_tok; _temp2540.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2540;}); _temp2539;}); break; case 59: _LL2538:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2542=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2542[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2543; _temp2543.tag= Cyc_TypeSpecifier_tok;
_temp2543.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2544=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2544[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2545; _temp2545.tag=
Cyc_Parse_Signed_spec; _temp2545.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2545;});
_temp2544;})); _temp2543;}); _temp2542;}); break; case 60: _LL2541: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2547=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2547[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2548; _temp2548.tag= Cyc_TypeSpecifier_tok;
_temp2548.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2549=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2549[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2550; _temp2550.tag= Cyc_Parse_Unsigned_spec; _temp2550.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2550;}); _temp2549;})); _temp2548;}); _temp2547;}); break; case 61: _LL2546:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2551: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2552: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 64:
_LL2553: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2555=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2555[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2556; _temp2556.tag=
Cyc_TypeSpecifier_tok; _temp2556.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2556;}); _temp2555;}); break; case 65: _LL2554: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2558=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2558[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2559; _temp2559.tag=
Cyc_TypeSpecifier_tok; _temp2559.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2559;}); _temp2558;}); break; case 66: _LL2557: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2561=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2561[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2562; _temp2562.tag=
Cyc_TypeSpecifier_tok; _temp2562.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2563=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2563[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2564; _temp2564.tag= Cyc_Absyn_TypedefType;
_temp2564.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2564.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2564.f3= 0; _temp2564;}); _temp2563;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2562;}); _temp2561;}); break; case 67: _LL2560: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2566=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2566[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2567; _temp2567.tag=
Cyc_TypeSpecifier_tok; _temp2567.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2568=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2568[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2569; _temp2569.tag= Cyc_Absyn_TupleType;
_temp2569.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2569;});
_temp2568;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2567;});
_temp2566;}); break; case 68: _LL2565: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2571=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2571[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2572; _temp2572.tag=
Cyc_TypeSpecifier_tok; _temp2572.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2573=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2573[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2574; _temp2574.tag= Cyc_Absyn_RgnHandleType;
_temp2574.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2574;}); _temp2573;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2572;}); _temp2571;}); break; case 69: _LL2570: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2576=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2576[ 0]=({ struct Cyc_Kind_tok_struct _temp2577; _temp2577.tag= Cyc_Kind_tok;
_temp2577.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2577;}); _temp2576;}); break; case 70: _LL2575: { struct _tagged_string*
_temp2581; void* _temp2583; struct _tuple1 _temp2579=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2584:
_temp2583= _temp2579.f1; goto _LL2582; _LL2582: _temp2581= _temp2579.f2; goto
_LL2580; _LL2580: if( _temp2583 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err(({
unsigned char* _temp2585="bad kind in type specifier"; struct _tagged_string
_temp2586; _temp2586.curr= _temp2585; _temp2586.base= _temp2585; _temp2586.last_plus_one=
_temp2585 + 27; _temp2586;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Kind_tok_struct* _temp2587=( struct Cyc_Kind_tok_struct*) GC_malloc(
sizeof( struct Cyc_Kind_tok_struct)); _temp2587[ 0]=({ struct Cyc_Kind_tok_struct
_temp2588; _temp2588.tag= Cyc_Kind_tok; _temp2588.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2581, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2588;});
_temp2587;}); break;} case 71: _LL2578: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2590=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2590[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2591; _temp2591.tag= Cyc_TypeQual_tok;
_temp2591.f1=({ struct Cyc_Absyn_Tqual _temp2592; _temp2592.q_const= 1;
_temp2592.q_volatile= 0; _temp2592.q_restrict= 0; _temp2592;}); _temp2591;});
_temp2590;}); break; case 72: _LL2589: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2594=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2594[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2595; _temp2595.tag= Cyc_TypeQual_tok;
_temp2595.f1=({ struct Cyc_Absyn_Tqual _temp2596; _temp2596.q_const= 0;
_temp2596.q_volatile= 1; _temp2596.q_restrict= 0; _temp2596;}); _temp2595;});
_temp2594;}); break; case 73: _LL2593: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2598=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2598[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2599; _temp2599.tag= Cyc_TypeQual_tok;
_temp2599.f1=({ struct Cyc_Absyn_Tqual _temp2600; _temp2600.q_const= 0;
_temp2600.q_volatile= 0; _temp2600.q_restrict= 1; _temp2600;}); _temp2599;});
_temp2598;}); break; case 74: _LL2597: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2602=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2602[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2603; _temp2603.tag=
Cyc_TypeSpecifier_tok; _temp2603.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2604=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2604[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2605; _temp2605.tag= Cyc_Parse_Decl_spec;
_temp2605.f1=({ struct Cyc_Absyn_Decl* _temp2606=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2606->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2607=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2607[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2608; _temp2608.tag= Cyc_Absyn_Enum_d; _temp2608.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2609=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2609->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2609->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2609->fields=({
struct Cyc_Core_Opt* _temp2610=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2610->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2610;}); _temp2609;}); _temp2608;});
_temp2607;})); _temp2606->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2606;});
_temp2605;}); _temp2604;})); _temp2603;}); _temp2602;}); break; case 75: _LL2601:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2612=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2612[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2613; _temp2613.tag= Cyc_TypeSpecifier_tok;
_temp2613.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2614=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2614[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2615; _temp2615.tag= Cyc_Absyn_EnumType;
_temp2615.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2615.f2= 0; _temp2615;}); _temp2614;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2613;}); _temp2612;}); break; case 76: _LL2611: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2617=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2617[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2618; _temp2618.tag= Cyc_Enumfield_tok;
_temp2618.f1=({ struct Cyc_Absyn_Enumfield* _temp2619=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2619->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2619->tag=
0; _temp2619->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2619;});
_temp2618;}); _temp2617;}); break; case 77: _LL2616: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2621=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2621[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2622; _temp2622.tag= Cyc_Enumfield_tok;
_temp2622.f1=({ struct Cyc_Absyn_Enumfield* _temp2623=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2623->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2623->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2623->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2623;});
_temp2622;}); _temp2621;}); break; case 78: _LL2620: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2625=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2625[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2626; _temp2626.tag=
Cyc_EnumfieldList_tok; _temp2626.f1=({ struct Cyc_List_List* _temp2627=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2627->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2627->tl= 0; _temp2627;}); _temp2626;}); _temp2625;}); break; case 79:
_LL2624: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2629=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2629[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2630; _temp2630.tag=
Cyc_EnumfieldList_tok; _temp2630.f1=({ struct Cyc_List_List* _temp2631=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2631->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2631->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2631;}); _temp2630;}); _temp2629;}); break; case 80:
_LL2628: { void* t;{ void* _temp2633= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2635: if( _temp2633 ==( void*) Cyc_Parse_Struct_su){
goto _LL2636;} else{ goto _LL2637;} _LL2637: if( _temp2633 ==( void*) Cyc_Parse_Union_su){
goto _LL2638;} else{ goto _LL2634;} _LL2636: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2639=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2639[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2640; _temp2640.tag= Cyc_Absyn_AnonStructType; _temp2640.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2640;});
_temp2639;}); goto _LL2634; _LL2638: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2641=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2641[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2642; _temp2642.tag= Cyc_Absyn_AnonUnionType; _temp2642.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2642;});
_temp2641;}); goto _LL2634; _LL2634:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2643=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2643[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2644; _temp2644.tag=
Cyc_TypeSpecifier_tok; _temp2644.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2645=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2645[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2646; _temp2646.tag= Cyc_Parse_Type_spec;
_temp2646.f1=( void*) t; _temp2646.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2646;}); _temp2645;})); _temp2644;}); _temp2643;}); break;} case 81:
_LL2632: { struct Cyc_List_List* _temp2648=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2649; _LL2649: { struct Cyc_Absyn_Decl* d;{ void* _temp2650= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2652:
if( _temp2650 ==( void*) Cyc_Parse_Struct_su){ goto _LL2653;} else{ goto _LL2654;}
_LL2654: if( _temp2650 ==( void*) Cyc_Parse_Union_su){ goto _LL2655;} else{ goto
_LL2651;} _LL2653: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2656=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2656->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2656;}), _temp2648,({ struct Cyc_Core_Opt* _temp2657=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2657->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2657;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2651;
_LL2655: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2658=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2658->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2658;}), _temp2648,({ struct Cyc_Core_Opt* _temp2659=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2659->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2659;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2651;
_LL2651:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2660=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2660[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2661; _temp2661.tag=
Cyc_TypeSpecifier_tok; _temp2661.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2662=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2662[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2663; _temp2663.tag= Cyc_Parse_Decl_spec;
_temp2663.f1= d; _temp2663;}); _temp2662;})); _temp2661;}); _temp2660;}); break;}}
case 82: _LL2647: { struct Cyc_List_List* _temp2665=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2666; _LL2666: { struct Cyc_Absyn_Decl* d;{ void* _temp2667= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2669:
if( _temp2667 ==( void*) Cyc_Parse_Struct_su){ goto _LL2670;} else{ goto _LL2671;}
_LL2671: if( _temp2667 ==( void*) Cyc_Parse_Union_su){ goto _LL2672;} else{ goto
_LL2668;} _LL2670: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2673=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2673->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2673;}), _temp2665,({ struct Cyc_Core_Opt* _temp2674=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2674->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2674;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2668;
_LL2672: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2675=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2675->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2675;}), _temp2665,({ struct Cyc_Core_Opt* _temp2676=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2676->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2676;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2668;
_LL2668:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2677=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2677[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2678; _temp2678.tag=
Cyc_TypeSpecifier_tok; _temp2678.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2679=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2679[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2680; _temp2680.tag= Cyc_Parse_Decl_spec;
_temp2680.f1= d; _temp2680;}); _temp2679;})); _temp2678;}); _temp2677;}); break;}}
case 83: _LL2664:{ void* _temp2682= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2684: if( _temp2682 ==( void*) Cyc_Parse_Struct_su){
goto _LL2685;} else{ goto _LL2686;} _LL2686: if( _temp2682 ==( void*) Cyc_Parse_Union_su){
goto _LL2687;} else{ goto _LL2683;} _LL2685: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2688=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2688[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2689; _temp2689.tag=
Cyc_TypeSpecifier_tok; _temp2689.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2690=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2690[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2691; _temp2691.tag= Cyc_Absyn_StructType;
_temp2691.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2691.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2691.f3= 0;
_temp2691;}); _temp2690;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2689;});
_temp2688;}); goto _LL2683; _LL2687: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2692=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2692[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2693; _temp2693.tag=
Cyc_TypeSpecifier_tok; _temp2693.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2694=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2694[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2695; _temp2695.tag= Cyc_Absyn_UnionType;
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
_temp2692;}); goto _LL2683; _LL2683:;} break; case 84: _LL2681:{ void* _temp2697=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2699: if( _temp2697 ==( void*) Cyc_Parse_Struct_su){ goto _LL2700;} else{
goto _LL2701;} _LL2701: if( _temp2697 ==( void*) Cyc_Parse_Union_su){ goto
_LL2702;} else{ goto _LL2698;} _LL2700: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2703=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2703[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2704; _temp2704.tag=
Cyc_TypeSpecifier_tok; _temp2704.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2705=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2705[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2706; _temp2706.tag= Cyc_Absyn_StructType;
_temp2706.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2706.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2706.f3= 0;
_temp2706;}); _temp2705;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2704;});
_temp2703;}); goto _LL2698; _LL2702: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2707=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2707[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2708; _temp2708.tag=
Cyc_TypeSpecifier_tok; _temp2708.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2709=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2709[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2710; _temp2710.tag= Cyc_Absyn_UnionType;
_temp2710.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2710.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2710.f3= 0;
_temp2710;}); _temp2709;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2708;});
_temp2707;}); goto _LL2698; _LL2698:;} break; case 85: _LL2696: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2712=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2712[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2713; _temp2713.tag= Cyc_TypeList_tok; _temp2713.f1= 0; _temp2713;});
_temp2712;}); break; case 86: _LL2711: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2715=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2715[ 0]=({ struct Cyc_TypeList_tok_struct _temp2716; _temp2716.tag= Cyc_TypeList_tok;
_temp2716.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2716;}); _temp2715;}); break; case 87: _LL2714:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2718=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2718[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2719; _temp2719.tag= Cyc_TypeList_tok; _temp2719.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2719;}); _temp2718;}); break;
case 88: _LL2717: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2721=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2721[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2722; _temp2722.tag=
Cyc_StructOrUnion_tok; _temp2722.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2722;}); _temp2721;}); break; case 89: _LL2720: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2724=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2724[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2725; _temp2725.tag=
Cyc_StructOrUnion_tok; _temp2725.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2725;}); _temp2724;}); break; case 90: _LL2723: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2727=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2727[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2728; _temp2728.tag= Cyc_StructFieldDeclList_tok; _temp2728.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2728;});
_temp2727;}); break; case 91: _LL2726: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2730=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2730[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2731; _temp2731.tag= Cyc_StructFieldDeclListList_tok; _temp2731.f1=({
struct Cyc_List_List* _temp2732=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2732->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2732->tl=
0; _temp2732;}); _temp2731;}); _temp2730;}); break; case 92: _LL2729: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2734=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2734[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2735; _temp2735.tag= Cyc_StructFieldDeclListList_tok;
_temp2735.f1=({ struct Cyc_List_List* _temp2736=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2736->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2736->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2736;}); _temp2735;}); _temp2734;}); break; case 93:
_LL2733: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2738=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2738[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2739; _temp2739.tag=
Cyc_InitDeclList_tok; _temp2739.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2739;}); _temp2738;}); break; case 94:
_LL2737: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2741=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2741[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2742; _temp2742.tag=
Cyc_InitDeclList_tok; _temp2742.f1=({ struct Cyc_List_List* _temp2743=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2743->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2743->tl= 0; _temp2743;}); _temp2742;}); _temp2741;}); break; case 95:
_LL2740: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2745=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2745[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2746; _temp2746.tag=
Cyc_InitDeclList_tok; _temp2746.f1=({ struct Cyc_List_List* _temp2747=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2747->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2747->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2747;}); _temp2746;}); _temp2745;}); break; case 96:
_LL2744: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2749=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2749[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2750; _temp2750.tag= Cyc_InitDecl_tok; _temp2750.f1=({ struct _tuple15*
_temp2751=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2751->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2751->f2= 0; _temp2751;}); _temp2750;}); _temp2749;}); break; case 97:
_LL2748: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2753=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2753[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2754; _temp2754.tag= Cyc_InitDecl_tok; _temp2754.f1=({ struct _tuple15*
_temp2755=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2755->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2755->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2755;}); _temp2754;}); _temp2753;}); break;
case 98: _LL2752: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2757=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2758; _LL2758: { void* _temp2759=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2760; _LL2760: { struct Cyc_List_List* _temp2763; struct Cyc_List_List*
_temp2765; struct _tuple0 _temp2761=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2766: _temp2765= _temp2761.f1; goto
_LL2764; _LL2764: _temp2763= _temp2761.f2; goto _LL2762; _LL2762: { struct Cyc_List_List*
_temp2767=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2759, _temp2765, _temp2757),
_temp2763); goto _LL2768; _LL2768: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2769=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2769[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2770; _temp2770.tag= Cyc_StructFieldDeclList_tok; _temp2770.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2767); _temp2770;}); _temp2769;}); break;}}}} case 99: _LL2756: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2772=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2772[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2773; _temp2773.tag= Cyc_QualSpecList_tok;
_temp2773.f1=({ struct _tuple18* _temp2774=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2774->f1= Cyc_Absyn_empty_tqual(); _temp2774->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2775->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2775->tl=
0; _temp2775;}); _temp2774->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2774;}); _temp2773;}); _temp2772;});
break; case 100: _LL2771: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2777=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2777[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2778; _temp2778.tag=
Cyc_QualSpecList_tok; _temp2778.f1=({ struct _tuple18* _temp2779=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2779->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2779->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2780->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2780->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2780;}); _temp2779->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2779;}); _temp2778;}); _temp2777;});
break; case 101: _LL2776: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2782=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2782[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2783; _temp2783.tag=
Cyc_QualSpecList_tok; _temp2783.f1=({ struct _tuple18* _temp2784=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2784->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2784->f2=
0; _temp2784->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2784;}); _temp2783;}); _temp2782;}); break; case 102:
_LL2781: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2786=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2786[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2787; _temp2787.tag=
Cyc_QualSpecList_tok; _temp2787.f1=({ struct _tuple18* _temp2788=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2788->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2788->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2788->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2788;});
_temp2787;}); _temp2786;}); break; case 103: _LL2785: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2790=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2790[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2791; _temp2791.tag= Cyc_DeclaratorExpoptList_tok;
_temp2791.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2791;}); _temp2790;}); break; case 104: _LL2789: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2793=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2793[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2794; _temp2794.tag= Cyc_DeclaratorExpoptList_tok;
_temp2794.f1=({ struct Cyc_List_List* _temp2795=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2795->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2795->tl=
0; _temp2795;}); _temp2794;}); _temp2793;}); break; case 105: _LL2792: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2797=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2797[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2798; _temp2798.tag= Cyc_DeclaratorExpoptList_tok;
_temp2798.f1=({ struct Cyc_List_List* _temp2799=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2799->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2799->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2799;}); _temp2798;}); _temp2797;}); break; case 106:
_LL2796: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2801=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2801[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2802; _temp2802.tag=
Cyc_DeclaratorExpopt_tok; _temp2802.f1=({ struct _tuple13* _temp2803=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2803->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2803->f2=
0; _temp2803;}); _temp2802;}); _temp2801;}); break; case 107: _LL2800: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2805=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2805[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2806; _temp2806.tag= Cyc_DeclaratorExpopt_tok;
_temp2806.f1=({ struct _tuple13* _temp2807=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2807->f1=({ struct Cyc_Parse_Declarator*
_temp2809=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2809->id=({ struct _tuple1* _temp2810=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2810->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2814=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2814[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2815; _temp2815.tag= Cyc_Absyn_Rel_n;
_temp2815.f1= 0; _temp2815;}); _temp2814;}); _temp2810->f2=({ struct
_tagged_string* _temp2811=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2811[ 0]=({ unsigned char* _temp2812=""; struct
_tagged_string _temp2813; _temp2813.curr= _temp2812; _temp2813.base= _temp2812;
_temp2813.last_plus_one= _temp2812 + 1; _temp2813;}); _temp2811;}); _temp2810;});
_temp2809->tms= 0; _temp2809;}); _temp2807->f2=({ struct Cyc_Core_Opt* _temp2808=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2808->v=(
void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2808;}); _temp2807;}); _temp2806;}); _temp2805;}); break; case 108: _LL2804:
yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2817=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2817[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2818; _temp2818.tag= Cyc_DeclaratorExpopt_tok;
_temp2818.f1=({ struct _tuple13* _temp2819=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2819->f1= Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2819->f2=({ struct Cyc_Core_Opt*
_temp2820=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2820->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2820;}); _temp2819;}); _temp2818;}); _temp2817;}); break;
case 109: _LL2816: { struct Cyc_List_List* _temp2822=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2823; _LL2823: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2824=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2824[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2825; _temp2825.tag=
Cyc_TypeSpecifier_tok; _temp2825.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2826=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2826[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2827; _temp2827.tag= Cyc_Parse_Decl_spec;
_temp2827.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2822,({
struct Cyc_Core_Opt* _temp2828=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2828->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2828;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2827;}); _temp2826;})); _temp2825;}); _temp2824;}); break;} case 110:
_LL2821: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2830=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2830[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2831; _temp2831.tag=
Cyc_TypeSpecifier_tok; _temp2831.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2832=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2832[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2833; _temp2833.tag= Cyc_Absyn_TunionType;
_temp2833.f1=({ struct Cyc_Absyn_TunionInfo _temp2834; _temp2834.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2835=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2835[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2836; _temp2836.tag= Cyc_Absyn_UnknownTunion;
_temp2836.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2837; _temp2837.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2837.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2837;}); _temp2836;}); _temp2835;})); _temp2834.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2834.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2834;}); _temp2833;});
_temp2832;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2831;});
_temp2830;}); break; case 111: _LL2829: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2839=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2839[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2840; _temp2840.tag=
Cyc_TypeSpecifier_tok; _temp2840.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2841=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2841[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2842; _temp2842.tag= Cyc_Absyn_TunionType;
_temp2842.f1=({ struct Cyc_Absyn_TunionInfo _temp2843; _temp2843.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2844=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2844[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2845; _temp2845.tag= Cyc_Absyn_UnknownTunion;
_temp2845.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2846; _temp2846.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2846.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2846;}); _temp2845;}); _temp2844;})); _temp2843.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2843.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2843;}); _temp2842;}); _temp2841;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2840;}); _temp2839;}); break; case 112: _LL2838: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2848=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2848[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2849; _temp2849.tag= Cyc_TypeSpecifier_tok;
_temp2849.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2850=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2850[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2851; _temp2851.tag= Cyc_Absyn_TunionFieldType; _temp2851.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2852; _temp2852.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2853=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2853[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2854; _temp2854.tag= Cyc_Absyn_UnknownTunionfield; _temp2854.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2855; _temp2855.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2855.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2855.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2855;}); _temp2854;}); _temp2853;})); _temp2852.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2852;}); _temp2851;}); _temp2850;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2849;}); _temp2848;}); break; case 113: _LL2847: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2857=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2857[ 0]=({ struct Cyc_Bool_tok_struct
_temp2858; _temp2858.tag= Cyc_Bool_tok; _temp2858.f1= 0; _temp2858;}); _temp2857;});
break; case 114: _LL2856: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2860=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2860[ 0]=({ struct Cyc_Bool_tok_struct _temp2861; _temp2861.tag= Cyc_Bool_tok;
_temp2861.f1= 1; _temp2861;}); _temp2860;}); break; case 115: _LL2859: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2863=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2863[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2864; _temp2864.tag= Cyc_TunionFieldList_tok;
_temp2864.f1=({ struct Cyc_List_List* _temp2865=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2865->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2865->tl=
0; _temp2865;}); _temp2864;}); _temp2863;}); break; case 116: _LL2862: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2867=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2867[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2868; _temp2868.tag= Cyc_TunionFieldList_tok;
_temp2868.f1=({ struct Cyc_List_List* _temp2869=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2869->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2869->tl=
0; _temp2869;}); _temp2868;}); _temp2867;}); break; case 117: _LL2866: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2871=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2871[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2872; _temp2872.tag= Cyc_TunionFieldList_tok;
_temp2872.f1=({ struct Cyc_List_List* _temp2873=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2873->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2873->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2873;}); _temp2872;}); _temp2871;}); break; case 118: _LL2870: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2875=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2875[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2876; _temp2876.tag= Cyc_TunionFieldList_tok;
_temp2876.f1=({ struct Cyc_List_List* _temp2877=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2877->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2877->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2877;}); _temp2876;}); _temp2875;}); break; case 119: _LL2874: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2879=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2879[ 0]=({ struct Cyc_Scope_tok_struct
_temp2880; _temp2880.tag= Cyc_Scope_tok; _temp2880.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2880;}); _temp2879;}); break; case 120: _LL2878: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2882=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2882[ 0]=({ struct Cyc_Scope_tok_struct
_temp2883; _temp2883.tag= Cyc_Scope_tok; _temp2883.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2883;}); _temp2882;}); break; case 121: _LL2881: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2885=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2885[ 0]=({ struct Cyc_Scope_tok_struct
_temp2886; _temp2886.tag= Cyc_Scope_tok; _temp2886.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2886;}); _temp2885;}); break; case 122: _LL2884: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2888=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2888[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2889; _temp2889.tag= Cyc_TunionField_tok;
_temp2889.f1=({ struct Cyc_Absyn_Tunionfield* _temp2890=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2890->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2890->tvs=
0; _temp2890->typs= 0; _temp2890->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2890->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2890;}); _temp2889;}); _temp2888;}); break; case 123:
_LL2887: { struct Cyc_List_List* _temp2892=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2893; _LL2893: { struct Cyc_List_List* _temp2894=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2895; _LL2895: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2896=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2896[ 0]=({ struct Cyc_TunionField_tok_struct _temp2897; _temp2897.tag= Cyc_TunionField_tok;
_temp2897.f1=({ struct Cyc_Absyn_Tunionfield* _temp2898=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2898->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2898->tvs=
_temp2894; _temp2898->typs= _temp2892; _temp2898->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2898->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2898;}); _temp2897;}); _temp2896;}); break;}} case 124:
_LL2891: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 125:
_LL2899: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2901=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2901[ 0]=({ struct Cyc_Declarator_tok_struct _temp2902; _temp2902.tag= Cyc_Declarator_tok;
_temp2902.f1=({ struct Cyc_Parse_Declarator* _temp2903=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2903->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2903->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2903;}); _temp2902;}); _temp2901;}); break; case 126: _LL2900: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2905=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2905[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2906; _temp2906.tag= Cyc_Declarator_tok;
_temp2906.f1=({ struct Cyc_Parse_Declarator* _temp2907=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2907->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2907->tms=
0; _temp2907;}); _temp2906;}); _temp2905;}); break; case 127: _LL2904: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
128: _LL2908: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2910=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2910[ 0]=({ struct Cyc_Declarator_tok_struct _temp2911; _temp2911.tag= Cyc_Declarator_tok;
_temp2911.f1=({ struct Cyc_Parse_Declarator* _temp2912=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2912->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2912->tms=({ struct Cyc_List_List* _temp2913=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2913->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2913->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2913;}); _temp2912;}); _temp2911;}); _temp2910;});
break; case 129: _LL2909: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2915=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2915[ 0]=({ struct Cyc_Declarator_tok_struct _temp2916; _temp2916.tag= Cyc_Declarator_tok;
_temp2916.f1=({ struct Cyc_Parse_Declarator* _temp2917=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2917->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2917->tms=({ struct Cyc_List_List* _temp2918=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2918->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2919=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2919[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2920; _temp2920.tag= Cyc_Absyn_ConstArray_mod;
_temp2920.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2920;}); _temp2919;})); _temp2918->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2918;}); _temp2917;});
_temp2916;}); _temp2915;}); break; case 130: _LL2914: { struct _tuple17
_temp2924; struct Cyc_Core_Opt* _temp2925; struct Cyc_Absyn_VarargInfo*
_temp2927; int _temp2929; struct Cyc_List_List* _temp2931; struct _tuple17*
_temp2922= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2924=* _temp2922; _LL2932: _temp2931= _temp2924.f1;
goto _LL2930; _LL2930: _temp2929= _temp2924.f2; goto _LL2928; _LL2928: _temp2927=
_temp2924.f3; goto _LL2926; _LL2926: _temp2925= _temp2924.f4; goto _LL2923;
_LL2923: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2933=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2933[ 0]=({ struct Cyc_Declarator_tok_struct _temp2934; _temp2934.tag= Cyc_Declarator_tok;
_temp2934.f1=({ struct Cyc_Parse_Declarator* _temp2935=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2935->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2935->tms=({ struct Cyc_List_List* _temp2936=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2936->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2937=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2937[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2938; _temp2938.tag= Cyc_Absyn_Function_mod;
_temp2938.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2939=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2939[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2940; _temp2940.tag= Cyc_Absyn_WithTypes;
_temp2940.f1= _temp2931; _temp2940.f2= _temp2929; _temp2940.f3= _temp2927;
_temp2940.f4= _temp2925; _temp2940;}); _temp2939;})); _temp2938;}); _temp2937;}));
_temp2936->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)))->tms; _temp2936;}); _temp2935;}); _temp2934;}); _temp2933;});
break;} case 131: _LL2921: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2942=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2942[ 0]=({ struct Cyc_Declarator_tok_struct _temp2943; _temp2943.tag= Cyc_Declarator_tok;
_temp2943.f1=({ struct Cyc_Parse_Declarator* _temp2944=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2944->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2944->tms=({ struct Cyc_List_List* _temp2945=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2945->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2946=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2946[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2947; _temp2947.tag= Cyc_Absyn_Function_mod;
_temp2947.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2948=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2948[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2949; _temp2949.tag= Cyc_Absyn_WithTypes;
_temp2949.f1= 0; _temp2949.f2= 0; _temp2949.f3= 0; _temp2949.f4= 0; _temp2949;});
_temp2948;})); _temp2947;}); _temp2946;})); _temp2945->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp2945;}); _temp2944;}); _temp2943;}); _temp2942;}); break; case 132: _LL2941:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2951=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2951[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2952; _temp2952.tag= Cyc_Declarator_tok;
_temp2952.f1=({ struct Cyc_Parse_Declarator* _temp2953=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2953->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2953->tms=({ struct Cyc_List_List* _temp2954=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2954->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2955=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2955[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2956; _temp2956.tag= Cyc_Absyn_Function_mod;
_temp2956.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2957=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2957[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2958; _temp2958.tag= Cyc_Absyn_WithTypes;
_temp2958.f1= 0; _temp2958.f2= 0; _temp2958.f3= 0; _temp2958.f4=({ struct Cyc_Core_Opt*
_temp2959=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2959->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2960=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp2960[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp2961; _temp2961.tag= Cyc_Absyn_JoinEff;
_temp2961.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2961;}); _temp2960;})); _temp2959;}); _temp2958;});
_temp2957;})); _temp2956;}); _temp2955;})); _temp2954->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp2954;}); _temp2953;}); _temp2952;}); _temp2951;}); break; case 133: _LL2950:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2963=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2963[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2964; _temp2964.tag= Cyc_Declarator_tok;
_temp2964.f1=({ struct Cyc_Parse_Declarator* _temp2965=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2965->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2965->tms=({ struct Cyc_List_List* _temp2966=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2966->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2967=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2967[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2968; _temp2968.tag= Cyc_Absyn_Function_mod;
_temp2968.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2969=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2969[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2970; _temp2970.tag= Cyc_Absyn_NoTypes;
_temp2970.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2970.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2970;});
_temp2969;})); _temp2968;}); _temp2967;})); _temp2966->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2966;}); _temp2965;}); _temp2964;}); _temp2963;}); break; case 134: _LL2962: {
struct Cyc_List_List* _temp2972=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2973; _LL2973: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2974=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2974[ 0]=({ struct Cyc_Declarator_tok_struct _temp2975; _temp2975.tag= Cyc_Declarator_tok;
_temp2975.f1=({ struct Cyc_Parse_Declarator* _temp2976=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2976->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2976->tms=({ struct Cyc_List_List* _temp2977=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2977->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2978=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2978[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2979; _temp2979.tag= Cyc_Absyn_TypeParams_mod;
_temp2979.f1= _temp2972; _temp2979.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2979.f3= 0; _temp2979;}); _temp2978;})); _temp2977->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2977;}); _temp2976;}); _temp2975;}); _temp2974;}); break;} case 135:
_LL2971:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2981=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2982; _LL2982: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2983=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2983[ 0]=({ struct Cyc_Declarator_tok_struct _temp2984; _temp2984.tag= Cyc_Declarator_tok;
_temp2984.f1=({ struct Cyc_Parse_Declarator* _temp2985=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2985->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2985->tms=({ struct Cyc_List_List* _temp2986=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2986->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2987=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2987[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2988; _temp2988.tag= Cyc_Absyn_TypeParams_mod;
_temp2988.f1= _temp2981; _temp2988.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2988.f3= 0; _temp2988;}); _temp2987;})); _temp2986->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2986;}); _temp2985;}); _temp2984;}); _temp2983;}); break;} case 136:
_LL2980: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2990=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2990[ 0]=({ struct Cyc_Declarator_tok_struct _temp2991; _temp2991.tag= Cyc_Declarator_tok;
_temp2991.f1=({ struct Cyc_Parse_Declarator* _temp2992=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2992->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp2992->tms=({ struct Cyc_List_List* _temp2993=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2993->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2994=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2994[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2995; _temp2995.tag= Cyc_Absyn_Attributes_mod;
_temp2995.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2995.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2995;});
_temp2994;})); _temp2993->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp2993;}); _temp2992;}); _temp2991;});
_temp2990;}); break; case 137: _LL2989: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2997=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2997[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2998; _temp2998.tag= Cyc_TypeModifierList_tok; _temp2998.f1=({ struct Cyc_List_List*
_temp2999=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2999->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3000=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3000[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3001; _temp3001.tag=
Cyc_Absyn_Pointer_mod; _temp3001.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3001.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3001.f3=
Cyc_Absyn_empty_tqual(); _temp3001;}); _temp3000;})); _temp2999->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp2999;}); _temp2998;}); _temp2997;});
break; case 138: _LL2996: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3003=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3003[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3004; _temp3004.tag= Cyc_TypeModifierList_tok; _temp3004.f1=({ struct Cyc_List_List*
_temp3005=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3005->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3006=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3006[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3007; _temp3007.tag=
Cyc_Absyn_Pointer_mod; _temp3007.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3007.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3007.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3007;}); _temp3006;})); _temp3005->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp3005;}); _temp3004;}); _temp3003;}); break; case 139: _LL3002: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp3009=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp3009[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp3010; _temp3010.tag= Cyc_TypeModifierList_tok;
_temp3010.f1=({ struct Cyc_List_List* _temp3011=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3011->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp3012=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp3012[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp3013; _temp3013.tag= Cyc_Absyn_Pointer_mod;
_temp3013.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3013.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3013.f3=
Cyc_Absyn_empty_tqual(); _temp3013;}); _temp3012;})); _temp3011->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3011;}); _temp3010;}); _temp3009;});
break; case 140: _LL3008: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp3015=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3015[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp3016; _temp3016.tag= Cyc_TypeModifierList_tok; _temp3016.f1=({ struct Cyc_List_List*
_temp3017=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3017->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp3018=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3018[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp3019; _temp3019.tag=
Cyc_Absyn_Pointer_mod; _temp3019.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3019.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3019.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3019;}); _temp3018;})); _temp3017->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3017;}); _temp3016;}); _temp3015;}); break; case 141: _LL3014: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp3021=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3021[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3022; _temp3022.tag= Cyc_Pointer_Sort_tok;
_temp3022.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3023=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3023[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3024; _temp3024.tag=
Cyc_Absyn_Nullable_ps; _temp3024.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3024;}); _temp3023;})); _temp3022;}); _temp3021;}); break; case 142:
_LL3020: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3026=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3026[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3027; _temp3027.tag=
Cyc_Pointer_Sort_tok; _temp3027.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3028=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3028[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3029; _temp3029.tag= Cyc_Absyn_NonNullable_ps; _temp3029.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3029;}); _temp3028;})); _temp3027;}); _temp3026;});
break; case 143: _LL3025: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3031=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3031[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3032; _temp3032.tag=
Cyc_Pointer_Sort_tok; _temp3032.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3033=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3033[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3034; _temp3034.tag=
Cyc_Absyn_Nullable_ps; _temp3034.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3034;}); _temp3033;})); _temp3032;});
_temp3031;}); break; case 144: _LL3030: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3036=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3036[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3037; _temp3037.tag=
Cyc_Pointer_Sort_tok; _temp3037.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3038=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3038[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3039; _temp3039.tag= Cyc_Absyn_NonNullable_ps; _temp3039.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3039;});
_temp3038;})); _temp3037;}); _temp3036;}); break; case 145: _LL3035: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3041=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3041[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3042; _temp3042.tag= Cyc_Pointer_Sort_tok;
_temp3042.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3042;});
_temp3041;}); break; case 146: _LL3040: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3044=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3044[ 0]=({ struct Cyc_Rgn_tok_struct _temp3045; _temp3045.tag= Cyc_Rgn_tok;
_temp3045.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3045;}); _temp3044;});
break; case 147: _LL3043: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 148: _LL3046: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3048=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3048[ 0]=({ struct Cyc_Rgn_tok_struct _temp3049; _temp3049.tag= Cyc_Rgn_tok;
_temp3049.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3049;}); _temp3048;});
break; case 149: _LL3047: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({
unsigned char* _temp3051="expecting region kind\n"; struct _tagged_string
_temp3052; _temp3052.curr= _temp3051; _temp3052.base= _temp3051; _temp3052.last_plus_one=
_temp3051 + 23; _temp3052;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_Rgn_tok_struct* _temp3053=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp3053[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3054; _temp3054.tag= Cyc_Rgn_tok; _temp3054.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3054;}); _temp3053;}); break; case 150: _LL3050: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3056=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3056[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3057; _temp3057.tag= Cyc_Rgn_tok; _temp3057.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp3057;}); _temp3056;}); break; case 151: _LL3055:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 152:
_LL3058: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp3060=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3060[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3061; _temp3061.tag= Cyc_TypeQual_tok; _temp3061.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3061;}); _temp3060;}); break; case 153: _LL3059: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3063=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3063[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3064; _temp3064.tag= Cyc_ParamDeclListBool_tok;
_temp3064.f1=({ struct _tuple17* _temp3065=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3065->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp3065->f2= 0; _temp3065->f3= 0;
_temp3065->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3065;}); _temp3064;}); _temp3063;}); break; case 154:
_LL3062: yyval=( void*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp3067=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp3067[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp3068; _temp3068.tag=
Cyc_ParamDeclListBool_tok; _temp3068.f1=({ struct _tuple17* _temp3069=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp3069->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); _temp3069->f2=
1; _temp3069->f3= 0; _temp3069->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3069;}); _temp3068;}); _temp3067;}); break;
case 155: _LL3066: { void* _temp3073; struct Cyc_Absyn_Tqual _temp3075; struct
Cyc_Core_Opt* _temp3077; struct _tuple2 _temp3071=* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _LL3078:
_temp3077= _temp3071.f1; goto _LL3076; _LL3076: _temp3075= _temp3071.f2; goto
_LL3074; _LL3074: _temp3073= _temp3071.f3; goto _LL3072; _LL3072: { struct Cyc_Absyn_VarargInfo*
_temp3080=({ struct Cyc_Absyn_VarargInfo* _temp3079=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3079->name= _temp3077;
_temp3079->tq= _temp3075; _temp3079->type=( void*) _temp3073; _temp3079->rgn=(
void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp3079->inject= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3079;}); goto _LL3081; _LL3081: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3082=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3082[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3083; _temp3083.tag= Cyc_ParamDeclListBool_tok;
_temp3083.f1=({ struct _tuple17* _temp3084=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3084->f1= 0; _temp3084->f2= 0; _temp3084->f3=
_temp3080; _temp3084->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3084;}); _temp3083;}); _temp3082;}); break;}}
case 156: _LL3070: { void* _temp3088; struct Cyc_Absyn_Tqual _temp3090; struct
Cyc_Core_Opt* _temp3092; struct _tuple2 _temp3086=* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _LL3093:
_temp3092= _temp3086.f1; goto _LL3091; _LL3091: _temp3090= _temp3086.f2; goto
_LL3089; _LL3089: _temp3088= _temp3086.f3; goto _LL3087; _LL3087: { struct Cyc_Absyn_VarargInfo*
_temp3095=({ struct Cyc_Absyn_VarargInfo* _temp3094=( struct Cyc_Absyn_VarargInfo*)
GC_malloc( sizeof( struct Cyc_Absyn_VarargInfo)); _temp3094->name= _temp3092;
_temp3094->tq= _temp3090; _temp3094->type=( void*) _temp3088; _temp3094->rgn=(
void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp3094->inject= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3094;}); goto _LL3096; _LL3096: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3097=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3097[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3098; _temp3098.tag= Cyc_ParamDeclListBool_tok;
_temp3098.f1=({ struct _tuple17* _temp3099=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3099->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6))); _temp3099->f2= 0; _temp3099->f3= _temp3095;
_temp3099->f4= Cyc_yyget_TypeOpt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3099;}); _temp3098;}); _temp3097;}); break;}} case 157:
_LL3085: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct* _temp3101=( struct Cyc_TypeOpt_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct)); _temp3101[ 0]=({ struct Cyc_TypeOpt_tok_struct
_temp3102; _temp3102.tag= Cyc_TypeOpt_tok; _temp3102.f1= 0; _temp3102;});
_temp3101;}); break; case 158: _LL3100: yyval=( void*)({ struct Cyc_TypeOpt_tok_struct*
_temp3104=( struct Cyc_TypeOpt_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeOpt_tok_struct));
_temp3104[ 0]=({ struct Cyc_TypeOpt_tok_struct _temp3105; _temp3105.tag= Cyc_TypeOpt_tok;
_temp3105.f1=({ struct Cyc_Core_Opt* _temp3106=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3106->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3107=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3107[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3108; _temp3108.tag= Cyc_Absyn_JoinEff;
_temp3108.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3108;}); _temp3107;})); _temp3106;}); _temp3105;});
_temp3104;}); break; case 159: _LL3103: yyval=( void*)({ struct Cyc_Bool_tok_struct*
_temp3110=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp3110[ 0]=({ struct Cyc_Bool_tok_struct _temp3111; _temp3111.tag= Cyc_Bool_tok;
_temp3111.f1= 0; _temp3111;}); _temp3110;}); break; case 160: _LL3109: { struct
_tagged_string _temp3113= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); goto _LL3114; _LL3114: if( Cyc_String_zstrcmp(
_temp3113,({ unsigned char* _temp3115="inject"; struct _tagged_string _temp3116;
_temp3116.curr= _temp3115; _temp3116.base= _temp3115; _temp3116.last_plus_one=
_temp3115 + 7; _temp3116;})) != 0){ Cyc_Parse_err(({ unsigned char* _temp3117="missing type in function declaration";
struct _tagged_string _temp3118; _temp3118.curr= _temp3117; _temp3118.base=
_temp3117; _temp3118.last_plus_one= _temp3117 + 37; _temp3118;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3119=( struct Cyc_Bool_tok_struct*)
GC_malloc( sizeof( struct Cyc_Bool_tok_struct)); _temp3119[ 0]=({ struct Cyc_Bool_tok_struct
_temp3120; _temp3120.tag= Cyc_Bool_tok; _temp3120.f1= 1; _temp3120;}); _temp3119;});
break;} case 161: _LL3112: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 162: _LL3121: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3123=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3123[ 0]=({ struct Cyc_TypeList_tok_struct _temp3124; _temp3124.tag= Cyc_TypeList_tok;
_temp3124.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3124;}); _temp3123;}); break; case 163:
_LL3122: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3126=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3126[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3127; _temp3127.tag= Cyc_TypeList_tok; _temp3127.f1= 0; _temp3127;});
_temp3126;}); break; case 164: _LL3125: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 165: _LL3128: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3130=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3130[ 0]=({ struct Cyc_TypeList_tok_struct _temp3131; _temp3131.tag= Cyc_TypeList_tok;
_temp3131.f1=({ struct Cyc_List_List* _temp3132=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3132->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3132->tl= 0; _temp3132;}); _temp3131;}); _temp3130;}); break; case 166:
_LL3129: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(({ unsigned char* _temp3134="expecing effect kind (E)";
struct _tagged_string _temp3135; _temp3135.curr= _temp3134; _temp3135.base=
_temp3134; _temp3135.last_plus_one= _temp3134 + 25; _temp3135;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3136=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3136[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3137; _temp3137.tag= Cyc_TypeList_tok; _temp3137.f1=({ struct Cyc_List_List*
_temp3138=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3138->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3138->tl= 0; _temp3138;});
_temp3137;}); _temp3136;}); break; case 167: _LL3133: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3140=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3140[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3141; _temp3141.tag= Cyc_TypeList_tok; _temp3141.f1=({ struct Cyc_List_List*
_temp3142=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3142->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3143=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3143[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3144; _temp3144.tag= Cyc_Absyn_AccessEff;
_temp3144.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3144;}); _temp3143;}));
_temp3142->tl= 0; _temp3142;}); _temp3141;}); _temp3140;}); break; case 168:
_LL3139: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3146=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3146[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3147; _temp3147.tag= Cyc_TypeList_tok; _temp3147.f1=({ struct Cyc_List_List*
_temp3148=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3148->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3149=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3149[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3150; _temp3150.tag= Cyc_Absyn_AccessEff;
_temp3150.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3150;}); _temp3149;}));
_temp3148->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3148;}); _temp3147;}); _temp3146;}); break; case 169:
_LL3145: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ unsigned char* _temp3152="expecting region kind (R)";
struct _tagged_string _temp3153; _temp3153.curr= _temp3152; _temp3153.base=
_temp3152; _temp3153.last_plus_one= _temp3152 + 26; _temp3153;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3154=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3154[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3155; _temp3155.tag= Cyc_TypeList_tok; _temp3155.f1=({ struct Cyc_List_List*
_temp3156=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3156->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3157=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3157[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3158; _temp3158.tag= Cyc_Absyn_AccessEff;
_temp3158.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3158;}); _temp3157;}));
_temp3156->tl= 0; _temp3156;}); _temp3155;}); _temp3154;}); break; case 170:
_LL3151: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(({ unsigned char* _temp3160="expecting region kind (R)";
struct _tagged_string _temp3161; _temp3161.curr= _temp3160; _temp3161.base=
_temp3160; _temp3161.last_plus_one= _temp3160 + 26; _temp3161;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3162=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3162[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3163; _temp3163.tag= Cyc_TypeList_tok; _temp3163.f1=({ struct Cyc_List_List*
_temp3164=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3164->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3165=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3165[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3166; _temp3166.tag= Cyc_Absyn_AccessEff;
_temp3166.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3166;}); _temp3165;}));
_temp3164->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3164;}); _temp3163;}); _temp3162;}); break; case 171:
_LL3159: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3168=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3168[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3169; _temp3169.tag=
Cyc_ParamDeclList_tok; _temp3169.f1=({ struct Cyc_List_List* _temp3170=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3170->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3170->tl= 0; _temp3170;}); _temp3169;}); _temp3168;}); break; case 172:
_LL3167: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3172=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3172[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3173; _temp3173.tag=
Cyc_ParamDeclList_tok; _temp3173.f1=({ struct Cyc_List_List* _temp3174=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3174->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3174->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3174;}); _temp3173;}); _temp3172;}); break; case 173:
_LL3171: { void* _temp3176= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3177; _LL3177: { struct Cyc_List_List* _temp3178=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3179; _LL3179: { struct Cyc_Absyn_Tqual _temp3180=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3181; _LL3181: { struct Cyc_List_List* _temp3182=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3183; _LL3183: { struct _tuple6 _temp3184= Cyc_Parse_apply_tms( _temp3180,
_temp3176, _temp3178, _temp3182); goto _LL3185; _LL3185: if( _temp3184.f3 != 0){
Cyc_Parse_err(({ unsigned char* _temp3186="parameter with bad type params";
struct _tagged_string _temp3187; _temp3187.curr= _temp3186; _temp3187.base=
_temp3186; _temp3187.last_plus_one= _temp3186 + 31; _temp3187;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3188=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3189; _LL3189:{ void* _temp3190=(*
_temp3188).f1; struct Cyc_List_List* _temp3200; struct Cyc_List_List* _temp3202;
_LL3192: if( _temp3190 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3193;} else{ goto
_LL3194;} _LL3194: if(( unsigned int) _temp3190 > 1u?*(( int*) _temp3190) == Cyc_Absyn_Rel_n:
0){ _LL3201: _temp3200=(( struct Cyc_Absyn_Rel_n_struct*) _temp3190)->f1; if(
_temp3200 == 0){ goto _LL3195;} else{ goto _LL3196;}} else{ goto _LL3196;}
_LL3196: if(( unsigned int) _temp3190 > 1u?*(( int*) _temp3190) == Cyc_Absyn_Abs_n:
0){ _LL3203: _temp3202=(( struct Cyc_Absyn_Abs_n_struct*) _temp3190)->f1; if(
_temp3202 == 0){ goto _LL3197;} else{ goto _LL3198;}} else{ goto _LL3198;}
_LL3198: goto _LL3199; _LL3193: goto _LL3191; _LL3195: goto _LL3191; _LL3197:
goto _LL3191; _LL3199: Cyc_Parse_err(({ unsigned char* _temp3204="parameter cannot be qualified with a module name";
struct _tagged_string _temp3205; _temp3205.curr= _temp3204; _temp3205.base=
_temp3204; _temp3205.last_plus_one= _temp3204 + 49; _temp3205;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3191; _LL3191:;}{ struct Cyc_Core_Opt* _temp3207=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3206=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3206->v=( void*)(* _temp3188).f2; _temp3206;}); goto
_LL3208; _LL3208: if( _temp3184.f4 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3209="extra attributes on parameter, ignoring"; struct _tagged_string
_temp3210; _temp3210.curr= _temp3209; _temp3210.base= _temp3209; _temp3210.last_plus_one=
_temp3209 + 40; _temp3210;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} if( _temp3184.f3
!= 0){ Cyc_Parse_warn(({ unsigned char* _temp3211="extra type variables on parameter, ignoring";
struct _tagged_string _temp3212; _temp3212.curr= _temp3211; _temp3212.base=
_temp3211; _temp3212.last_plus_one= _temp3211 + 44; _temp3212;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3213=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3213[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3214; _temp3214.tag= Cyc_ParamDecl_tok; _temp3214.f1=({
struct _tuple2* _temp3215=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3215->f1= _temp3207; _temp3215->f2= _temp3184.f1; _temp3215->f3= _temp3184.f2;
_temp3215;}); _temp3214;}); _temp3213;}); break;}}}}}}} case 174: _LL3175: {
void* _temp3217= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3218; _LL3218: { struct Cyc_List_List* _temp3219=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3220; _LL3220: { struct Cyc_Absyn_Tqual _temp3221=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3222; _LL3222: if( _temp3219 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3223="bad attributes on parameter, ignoring"; struct _tagged_string
_temp3224; _temp3224.curr= _temp3223; _temp3224.base= _temp3223; _temp3224.last_plus_one=
_temp3223 + 38; _temp3224;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_ParamDecl_tok_struct* _temp3225=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3225[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3226; _temp3226.tag= Cyc_ParamDecl_tok; _temp3226.f1=({
struct _tuple2* _temp3227=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3227->f1= 0; _temp3227->f2= _temp3221; _temp3227->f3= _temp3217; _temp3227;});
_temp3226;}); _temp3225;}); break;}}} case 175: _LL3216: { void* _temp3229= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3230; _LL3230: { struct Cyc_List_List*
_temp3231=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3232; _LL3232: { struct Cyc_Absyn_Tqual
_temp3233=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3234; _LL3234: { struct Cyc_List_List* _temp3235=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3236; _LL3236: { struct _tuple6 _temp3237= Cyc_Parse_apply_tms(
_temp3233, _temp3229, _temp3231, _temp3235); goto _LL3238; _LL3238: if(
_temp3237.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3239="bad type parameters on formal argument, ignoring";
struct _tagged_string _temp3240; _temp3240.curr= _temp3239; _temp3240.base=
_temp3239; _temp3240.last_plus_one= _temp3239 + 49; _temp3240;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3237.f4 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3241="bad attributes on parameter, ignoring";
struct _tagged_string _temp3242; _temp3242.curr= _temp3241; _temp3242.base=
_temp3241; _temp3242.last_plus_one= _temp3241 + 38; _temp3242;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3243=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3243[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3244; _temp3244.tag= Cyc_ParamDecl_tok; _temp3244.f1=({
struct _tuple2* _temp3245=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3245->f1= 0; _temp3245->f2= _temp3237.f1; _temp3245->f3= _temp3237.f2;
_temp3245;}); _temp3244;}); _temp3243;}); break;}}}}} case 176: _LL3228: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3247=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3247[ 0]=({ struct Cyc_IdList_tok_struct
_temp3248; _temp3248.tag= Cyc_IdList_tok; _temp3248.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3248;}); _temp3247;}); break; case
177: _LL3246: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3250=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3250[ 0]=({ struct Cyc_IdList_tok_struct _temp3251; _temp3251.tag= Cyc_IdList_tok;
_temp3251.f1=({ struct Cyc_List_List* _temp3252=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3252->hd=( void*)({ struct
_tagged_string* _temp3253=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3253[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3253;}); _temp3252->tl= 0; _temp3252;});
_temp3251;}); _temp3250;}); break; case 178: _LL3249: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3255=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3255[ 0]=({ struct Cyc_IdList_tok_struct
_temp3256; _temp3256.tag= Cyc_IdList_tok; _temp3256.f1=({ struct Cyc_List_List*
_temp3257=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3257->hd=( void*)({ struct _tagged_string* _temp3258=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3258[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3258;}); _temp3257->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3257;}); _temp3256;}); _temp3255;});
break; case 179: _LL3254: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 180: _LL3259: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 181: _LL3260: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3262=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3262[ 0]=({ struct Cyc_Exp_tok_struct _temp3263; _temp3263.tag= Cyc_Exp_tok;
_temp3263.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3264=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3264[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3265; _temp3265.tag= Cyc_Absyn_UnresolvedMem_e; _temp3265.f1= 0; _temp3265.f2=
0; _temp3265;}); _temp3264;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3263;});
_temp3262;}); break; case 182: _LL3261: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3267=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3267[ 0]=({ struct Cyc_Exp_tok_struct _temp3268; _temp3268.tag= Cyc_Exp_tok;
_temp3268.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3269=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3269[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3270; _temp3270.tag= Cyc_Absyn_UnresolvedMem_e; _temp3270.f1= 0; _temp3270.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3270;});
_temp3269;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3268;});
_temp3267;}); break; case 183: _LL3266: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3272=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3272[ 0]=({ struct Cyc_Exp_tok_struct _temp3273; _temp3273.tag= Cyc_Exp_tok;
_temp3273.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3274=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3274[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3275; _temp3275.tag= Cyc_Absyn_UnresolvedMem_e; _temp3275.f1= 0; _temp3275.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3275;});
_temp3274;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3273;});
_temp3272;}); break; case 184: _LL3271: { struct Cyc_Absyn_Vardecl* _temp3279=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3277=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3277->f1=( void*) Cyc_Absyn_Loc_n; _temp3277->f2=({
struct _tagged_string* _temp3278=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3278[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3278;}); _temp3277;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3280; _LL3280: _temp3279->tq=({ struct Cyc_Absyn_Tqual _temp3281;
_temp3281.q_const= 1; _temp3281.q_volatile= 0; _temp3281.q_restrict= 1;
_temp3281;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3282=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3282[ 0]=({ struct Cyc_Exp_tok_struct
_temp3283; _temp3283.tag= Cyc_Exp_tok; _temp3283.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3284=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3284[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3285; _temp3285.tag= Cyc_Absyn_Comprehension_e;
_temp3285.f1= _temp3279; _temp3285.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3285.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3285;}); _temp3284;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3283;}); _temp3282;}); break;} case 185: _LL3276: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3287=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3287[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3288; _temp3288.tag= Cyc_InitializerList_tok;
_temp3288.f1=({ struct Cyc_List_List* _temp3289=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3289->hd=( void*)({ struct
_tuple19* _temp3290=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3290->f1= 0; _temp3290->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3290;}); _temp3289->tl= 0; _temp3289;});
_temp3288;}); _temp3287;}); break; case 186: _LL3286: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3292=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3292[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3293; _temp3293.tag= Cyc_InitializerList_tok;
_temp3293.f1=({ struct Cyc_List_List* _temp3294=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3294->hd=( void*)({ struct
_tuple19* _temp3295=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3295->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3295->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3295;}); _temp3294->tl= 0; _temp3294;});
_temp3293;}); _temp3292;}); break; case 187: _LL3291: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3297=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3297[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3298; _temp3298.tag= Cyc_InitializerList_tok;
_temp3298.f1=({ struct Cyc_List_List* _temp3299=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3299->hd=( void*)({ struct
_tuple19* _temp3300=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3300->f1= 0; _temp3300->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3300;}); _temp3299->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3299;});
_temp3298;}); _temp3297;}); break; case 188: _LL3296: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3302=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3302[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3303; _temp3303.tag= Cyc_InitializerList_tok;
_temp3303.f1=({ struct Cyc_List_List* _temp3304=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3304->hd=( void*)({ struct
_tuple19* _temp3305=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3305->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3305->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3305;}); _temp3304->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3304;});
_temp3303;}); _temp3302;}); break; case 189: _LL3301: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3307=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3307[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3308; _temp3308.tag= Cyc_DesignatorList_tok;
_temp3308.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3308;}); _temp3307;}); break; case 190: _LL3306: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3310=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3310[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3311; _temp3311.tag= Cyc_DesignatorList_tok;
_temp3311.f1=({ struct Cyc_List_List* _temp3312=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3312->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3312->tl=
0; _temp3312;}); _temp3311;}); _temp3310;}); break; case 191: _LL3309: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3314=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3314[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3315; _temp3315.tag= Cyc_DesignatorList_tok;
_temp3315.f1=({ struct Cyc_List_List* _temp3316=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3316->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3316->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3316;}); _temp3315;}); _temp3314;}); break; case 192: _LL3313: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3318=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3318[ 0]=({ struct
Cyc_Designator_tok_struct _temp3319; _temp3319.tag= Cyc_Designator_tok;
_temp3319.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3320=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3320[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3321; _temp3321.tag=
Cyc_Absyn_ArrayElement; _temp3321.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3321;}); _temp3320;})); _temp3319;});
_temp3318;}); break; case 193: _LL3317: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3323=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3323[ 0]=({ struct Cyc_Designator_tok_struct _temp3324; _temp3324.tag= Cyc_Designator_tok;
_temp3324.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3325=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3325[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3326; _temp3326.tag= Cyc_Absyn_FieldName;
_temp3326.f1=({ struct _tagged_string* _temp3327=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3327[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3327;});
_temp3326;}); _temp3325;})); _temp3324;}); _temp3323;}); break; case 194:
_LL3322: { void* _temp3329= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3330; _LL3330: { struct Cyc_List_List* _temp3331=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3332; _LL3332: if( _temp3331 != 0){ Cyc_Parse_warn(({ unsigned char*
_temp3333="ignoring attributes in type"; struct _tagged_string _temp3334;
_temp3334.curr= _temp3333; _temp3334.base= _temp3333; _temp3334.last_plus_one=
_temp3333 + 28; _temp3334;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{ struct Cyc_Absyn_Tqual
_temp3335=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; goto _LL3336; _LL3336: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp3337=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp3337[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp3338; _temp3338.tag= Cyc_ParamDecl_tok;
_temp3338.f1=({ struct _tuple2* _temp3339=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp3339[ 0]=({ struct _tuple2 _temp3340; _temp3340.f1= 0;
_temp3340.f2= _temp3335; _temp3340.f3= _temp3329; _temp3340;}); _temp3339;});
_temp3338;}); _temp3337;}); break;}}} case 195: _LL3328: { void* _temp3342= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3343; _LL3343: { struct Cyc_List_List*
_temp3344=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3345; _LL3345: { struct Cyc_Absyn_Tqual
_temp3346=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3347; _LL3347: { struct Cyc_List_List* _temp3348=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3349; _LL3349: { struct _tuple6 _temp3350= Cyc_Parse_apply_tms(
_temp3346, _temp3342, _temp3344, _temp3348); goto _LL3351; _LL3351: if(
_temp3350.f3 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3352="bad type params, ignoring";
struct _tagged_string _temp3353; _temp3353.curr= _temp3352; _temp3353.base=
_temp3352; _temp3353.last_plus_one= _temp3352 + 26; _temp3353;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3350.f4 != 0){ Cyc_Parse_warn(({ unsigned char* _temp3354="bad specifiers, ignoring";
struct _tagged_string _temp3355; _temp3355.curr= _temp3354; _temp3355.base=
_temp3354; _temp3355.last_plus_one= _temp3354 + 25; _temp3355;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3356=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3356[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3357; _temp3357.tag= Cyc_ParamDecl_tok; _temp3357.f1=({
struct _tuple2* _temp3358=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3358->f1= 0; _temp3358->f2= _temp3350.f1; _temp3358->f3= _temp3350.f2;
_temp3358;}); _temp3357;}); _temp3356;}); break;}}}}} case 196: _LL3341: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3360=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3360[ 0]=({ struct Cyc_Type_tok_struct
_temp3361; _temp3361.tag= Cyc_Type_tok; _temp3361.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3361;});
_temp3360;}); break; case 197: _LL3359: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3363=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3363[ 0]=({ struct Cyc_Type_tok_struct _temp3364; _temp3364.tag= Cyc_Type_tok;
_temp3364.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3365=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3365[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3366; _temp3366.tag= Cyc_Absyn_JoinEff;
_temp3366.f1= 0; _temp3366;}); _temp3365;})); _temp3364;}); _temp3363;}); break;
case 198: _LL3362: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3368=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3368[ 0]=({ struct Cyc_Type_tok_struct _temp3369; _temp3369.tag= Cyc_Type_tok;
_temp3369.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3370=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3370[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3371; _temp3371.tag= Cyc_Absyn_JoinEff;
_temp3371.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3371;}); _temp3370;})); _temp3369;}); _temp3368;});
break; case 199: _LL3367: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3373=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3373[ 0]=({ struct Cyc_Type_tok_struct _temp3374; _temp3374.tag= Cyc_Type_tok;
_temp3374.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3375=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3375[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3376; _temp3376.tag= Cyc_Absyn_JoinEff;
_temp3376.f1=({ struct Cyc_List_List* _temp3377=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3377->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3377->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3377;}); _temp3376;}); _temp3375;})); _temp3374;}); _temp3373;}); break;
case 200: _LL3372: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3379=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3379[ 0]=({ struct Cyc_TypeList_tok_struct _temp3380; _temp3380.tag= Cyc_TypeList_tok;
_temp3380.f1=({ struct Cyc_List_List* _temp3381=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3381->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3381->tl=
0; _temp3381;}); _temp3380;}); _temp3379;}); break; case 201: _LL3378: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3383=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3383[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3384; _temp3384.tag= Cyc_TypeList_tok; _temp3384.f1=({ struct Cyc_List_List*
_temp3385=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3385->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3385->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3385;}); _temp3384;}); _temp3383;});
break; case 202: _LL3382: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3387=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3387[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3388; _temp3388.tag= Cyc_AbstractDeclarator_tok; _temp3388.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3389=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3389->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3389;}); _temp3388;}); _temp3387;}); break;
case 203: _LL3386: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 204: _LL3390: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3392=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3392[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3393; _temp3393.tag= Cyc_AbstractDeclarator_tok; _temp3393.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3394=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3394->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3394;}); _temp3393;});
_temp3392;}); break; case 205: _LL3391: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 206: _LL3395: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3397=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3397[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3398; _temp3398.tag= Cyc_AbstractDeclarator_tok; _temp3398.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3399=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3399->tms=({ struct Cyc_List_List* _temp3400=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3400->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3400->tl= 0; _temp3400;}); _temp3399;}); _temp3398;}); _temp3397;}); break;
case 207: _LL3396: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3402=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3402[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3403; _temp3403.tag= Cyc_AbstractDeclarator_tok; _temp3403.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3404=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3404->tms=({ struct Cyc_List_List* _temp3405=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3405->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3405->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3405;}); _temp3404;}); _temp3403;});
_temp3402;}); break; case 208: _LL3401: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3407=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3407[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3408; _temp3408.tag= Cyc_AbstractDeclarator_tok; _temp3408.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3409=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3409->tms=({ struct Cyc_List_List* _temp3410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3410->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3411=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3411[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3412; _temp3412.tag= Cyc_Absyn_ConstArray_mod;
_temp3412.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3412;}); _temp3411;})); _temp3410->tl= 0; _temp3410;}); _temp3409;});
_temp3408;}); _temp3407;}); break; case 209: _LL3406: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3414=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3414[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3415; _temp3415.tag= Cyc_AbstractDeclarator_tok;
_temp3415.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3416=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3416->tms=({
struct Cyc_List_List* _temp3417=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3417->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3418=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3418[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3419; _temp3419.tag= Cyc_Absyn_ConstArray_mod; _temp3419.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3419;});
_temp3418;})); _temp3417->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3417;}); _temp3416;});
_temp3415;}); _temp3414;}); break; case 210: _LL3413: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3421=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3421[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3422; _temp3422.tag= Cyc_AbstractDeclarator_tok;
_temp3422.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3423=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3423->tms=({
struct Cyc_List_List* _temp3424=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3424->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3425=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3425[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3426; _temp3426.tag=
Cyc_Absyn_Function_mod; _temp3426.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3427=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3427[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3428; _temp3428.tag= Cyc_Absyn_WithTypes;
_temp3428.f1= 0; _temp3428.f2= 0; _temp3428.f3= 0; _temp3428.f4= 0; _temp3428;});
_temp3427;})); _temp3426;}); _temp3425;})); _temp3424->tl= 0; _temp3424;});
_temp3423;}); _temp3422;}); _temp3421;}); break; case 211: _LL3420: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3430=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3430[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3431; _temp3431.tag= Cyc_AbstractDeclarator_tok;
_temp3431.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3432=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3432->tms=({
struct Cyc_List_List* _temp3433=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3433->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3434=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3434[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3435; _temp3435.tag=
Cyc_Absyn_Function_mod; _temp3435.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3436=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3436[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3437; _temp3437.tag= Cyc_Absyn_WithTypes;
_temp3437.f1= 0; _temp3437.f2= 0; _temp3437.f3= 0; _temp3437.f4=({ struct Cyc_Core_Opt*
_temp3438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3438->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3439=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3439[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3440; _temp3440.tag= Cyc_Absyn_JoinEff;
_temp3440.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3440;}); _temp3439;})); _temp3438;}); _temp3437;});
_temp3436;})); _temp3435;}); _temp3434;})); _temp3433->tl= 0; _temp3433;});
_temp3432;}); _temp3431;}); _temp3430;}); break; case 212: _LL3429: { struct
_tuple17 _temp3444; struct Cyc_Core_Opt* _temp3445; struct Cyc_Absyn_VarargInfo*
_temp3447; int _temp3449; struct Cyc_List_List* _temp3451; struct _tuple17*
_temp3442= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3444=* _temp3442; _LL3452: _temp3451= _temp3444.f1;
goto _LL3450; _LL3450: _temp3449= _temp3444.f2; goto _LL3448; _LL3448: _temp3447=
_temp3444.f3; goto _LL3446; _LL3446: _temp3445= _temp3444.f4; goto _LL3443;
_LL3443: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3453=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3453[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3454; _temp3454.tag=
Cyc_AbstractDeclarator_tok; _temp3454.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3455=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3455->tms=({ struct Cyc_List_List* _temp3456=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3456->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3457=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3457[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3458; _temp3458.tag= Cyc_Absyn_Function_mod;
_temp3458.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3459=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3459[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3460; _temp3460.tag= Cyc_Absyn_WithTypes;
_temp3460.f1= _temp3451; _temp3460.f2= _temp3449; _temp3460.f3= _temp3447;
_temp3460.f4= _temp3445; _temp3460;}); _temp3459;})); _temp3458;}); _temp3457;}));
_temp3456->tl= 0; _temp3456;}); _temp3455;}); _temp3454;}); _temp3453;}); break;}
case 213: _LL3441: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3462=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3462[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3463; _temp3463.tag= Cyc_AbstractDeclarator_tok; _temp3463.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3464=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3464->tms=({ struct Cyc_List_List* _temp3465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3465->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3466=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3466[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3467; _temp3467.tag= Cyc_Absyn_Function_mod;
_temp3467.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3468=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3468[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3469; _temp3469.tag= Cyc_Absyn_WithTypes;
_temp3469.f1= 0; _temp3469.f2= 0; _temp3469.f3= 0; _temp3469.f4= 0; _temp3469;});
_temp3468;})); _temp3467;}); _temp3466;})); _temp3465->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp3465;}); _temp3464;}); _temp3463;}); _temp3462;}); break; case 214: _LL3461: {
struct Cyc_Core_Opt* _temp3474=({ struct Cyc_Core_Opt* _temp3471=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3471->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3472=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3472[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3473; _temp3473.tag= Cyc_Absyn_JoinEff; _temp3473.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3473;}); _temp3472;})); _temp3471;}); goto _LL3475; _LL3475: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3476=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3476[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3477; _temp3477.tag= Cyc_AbstractDeclarator_tok;
_temp3477.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3478=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3478->tms=({
struct Cyc_List_List* _temp3479=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3479->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3480=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3480[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3481; _temp3481.tag=
Cyc_Absyn_Function_mod; _temp3481.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3482=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3482[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3483; _temp3483.tag= Cyc_Absyn_WithTypes;
_temp3483.f1= 0; _temp3483.f2= 0; _temp3483.f3= 0; _temp3483.f4= _temp3474;
_temp3483;}); _temp3482;})); _temp3481;}); _temp3480;})); _temp3479->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3479;}); _temp3478;}); _temp3477;}); _temp3476;}); break;} case 215:
_LL3470: { struct _tuple17 _temp3487; struct Cyc_Core_Opt* _temp3488; struct Cyc_Absyn_VarargInfo*
_temp3490; int _temp3492; struct Cyc_List_List* _temp3494; struct _tuple17*
_temp3485= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3487=* _temp3485; _LL3495: _temp3494= _temp3487.f1;
goto _LL3493; _LL3493: _temp3492= _temp3487.f2; goto _LL3491; _LL3491: _temp3490=
_temp3487.f3; goto _LL3489; _LL3489: _temp3488= _temp3487.f4; goto _LL3486;
_LL3486: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3496=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3496[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3497; _temp3497.tag=
Cyc_AbstractDeclarator_tok; _temp3497.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3498=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3498->tms=({ struct Cyc_List_List* _temp3499=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3499->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3500=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3500[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3501; _temp3501.tag= Cyc_Absyn_Function_mod;
_temp3501.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3502=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3502[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3503; _temp3503.tag= Cyc_Absyn_WithTypes;
_temp3503.f1= _temp3494; _temp3503.f2= _temp3492; _temp3503.f3= _temp3490;
_temp3503.f4= _temp3488; _temp3503;}); _temp3502;})); _temp3501;}); _temp3500;}));
_temp3499->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)))->tms; _temp3499;}); _temp3498;}); _temp3497;});
_temp3496;}); break;} case 216: _LL3484: { struct Cyc_List_List* _temp3505=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))); goto _LL3506; _LL3506: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3507=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3507[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3508; _temp3508.tag= Cyc_AbstractDeclarator_tok;
_temp3508.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3509=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3509->tms=({
struct Cyc_List_List* _temp3510=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3510->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp3511=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp3511[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp3512; _temp3512.tag= Cyc_Absyn_TypeParams_mod; _temp3512.f1= _temp3505;
_temp3512.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3512.f3= 0;
_temp3512;}); _temp3511;})); _temp3510->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3510;}); _temp3509;}); _temp3508;}); _temp3507;}); break;} case 217:
_LL3504:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3514=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3515; _LL3515: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3516=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3516[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3517; _temp3517.tag= Cyc_AbstractDeclarator_tok; _temp3517.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3518=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3518->tms=({ struct Cyc_List_List* _temp3519=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3519->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3520=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3520[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3521; _temp3521.tag= Cyc_Absyn_TypeParams_mod;
_temp3521.f1= _temp3514; _temp3521.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3521.f3= 0; _temp3521;}); _temp3520;})); _temp3519->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3519;}); _temp3518;}); _temp3517;}); _temp3516;}); break;} case 218:
_LL3513: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3523=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3523[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3524; _temp3524.tag=
Cyc_AbstractDeclarator_tok; _temp3524.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3525=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3525->tms=({ struct Cyc_List_List* _temp3526=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3526->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3527=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3527[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3528; _temp3528.tag= Cyc_Absyn_Attributes_mod;
_temp3528.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3528.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3528;});
_temp3527;})); _temp3526->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3526;}); _temp3525;});
_temp3524;}); _temp3523;}); break; case 219: _LL3522: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 220: _LL3529: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 221: _LL3530: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 222: _LL3531: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 223: _LL3532: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 224: _LL3533: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 225: _LL3534: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),({
unsigned char* _temp3536="`H"; struct _tagged_string _temp3537; _temp3537.curr=
_temp3536; _temp3537.base= _temp3536; _temp3537.last_plus_one= _temp3536 + 3;
_temp3537;})) == 0){ Cyc_Parse_err(({ unsigned char* _temp3538="bad occurrence of heap region `H";
struct _tagged_string _temp3539; _temp3539.curr= _temp3538; _temp3539.base=
_temp3538; _temp3539.last_plus_one= _temp3538 + 33; _temp3539;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3550=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3550->name=({ struct
_tagged_string* _temp3551=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3551[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3551;}); _temp3550->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3550;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3548=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3548[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3549; _temp3549.tag= Cyc_Absyn_VarType; _temp3549.f1=
tv; _temp3549;}); _temp3548;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3540=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3540[ 0]=({ struct Cyc_Stmt_tok_struct _temp3541; _temp3541.tag= Cyc_Stmt_tok;
_temp3541.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3542=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3542[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3543; _temp3543.tag= Cyc_Absyn_Region_s;
_temp3543.f1= tv; _temp3543.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3544=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3544->f1=(
void*) Cyc_Absyn_Loc_n; _temp3544->f2=({ struct _tagged_string* _temp3545=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3545[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3545;}); _temp3544;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3546=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3546[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3547; _temp3547.tag= Cyc_Absyn_RgnHandleType; _temp3547.f1=( void*) t;
_temp3547;}); _temp3546;}), 0); _temp3543.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3543;}); _temp3542;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3541;}); _temp3540;}); break;} case 226: _LL3535: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),({
unsigned char* _temp3553="H"; struct _tagged_string _temp3554; _temp3554.curr=
_temp3553; _temp3554.base= _temp3553; _temp3554.last_plus_one= _temp3553 + 2;
_temp3554;})) == 0){ Cyc_Parse_err(({ unsigned char* _temp3555="bad occurrence of heap region `H";
struct _tagged_string _temp3556; _temp3556.curr= _temp3555; _temp3556.base=
_temp3555; _temp3556.last_plus_one= _temp3555 + 33; _temp3556;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3567=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3567->name=({ struct
_tagged_string* _temp3568=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3568[ 0]=({ struct _tagged_string _temp3569= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); xprintf("`%.*s",
_temp3569.last_plus_one - _temp3569.curr, _temp3569.curr);}); _temp3568;});
_temp3567->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp3567;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3565=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3565[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3566; _temp3566.tag= Cyc_Absyn_VarType;
_temp3566.f1= tv; _temp3566;}); _temp3565;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3557=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3557[ 0]=({ struct Cyc_Stmt_tok_struct _temp3558; _temp3558.tag= Cyc_Stmt_tok;
_temp3558.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3559=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3559[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3560; _temp3560.tag= Cyc_Absyn_Region_s;
_temp3560.f1= tv; _temp3560.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3561=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3561->f1=(
void*) Cyc_Absyn_Loc_n; _temp3561->f2=({ struct _tagged_string* _temp3562=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3562[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3562;}); _temp3561;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3563=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3563[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3564; _temp3564.tag= Cyc_Absyn_RgnHandleType; _temp3564.f1=( void*) t;
_temp3564;}); _temp3563;}), 0); _temp3560.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3560;}); _temp3559;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3558;}); _temp3557;}); break;} case 227: _LL3552: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3571=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3571[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3572; _temp3572.tag= Cyc_Stmt_tok; _temp3572.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3573=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3573[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3574; _temp3574.tag= Cyc_Absyn_Cut_s; _temp3574.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3574;});
_temp3573;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3572;});
_temp3571;}); break; case 228: _LL3570: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3576=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3576[ 0]=({ struct Cyc_Stmt_tok_struct _temp3577; _temp3577.tag= Cyc_Stmt_tok;
_temp3577.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3578=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3578[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3579; _temp3579.tag= Cyc_Absyn_Splice_s;
_temp3579.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3579;}); _temp3578;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3577;}); _temp3576;}); break; case 229: _LL3575: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3581=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3581[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3582; _temp3582.tag= Cyc_Stmt_tok; _temp3582.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3583=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3583[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3584; _temp3584.tag= Cyc_Absyn_Label_s; _temp3584.f1=({
struct _tagged_string* _temp3585=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3585[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3585;}); _temp3584.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3584;});
_temp3583;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3582;});
_temp3581;}); break; case 230: _LL3580: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3587=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3587[ 0]=({ struct Cyc_Stmt_tok_struct _temp3588; _temp3588.tag= Cyc_Stmt_tok;
_temp3588.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3588;});
_temp3587;}); break; case 231: _LL3586: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3590=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3590[ 0]=({ struct Cyc_Stmt_tok_struct _temp3591; _temp3591.tag= Cyc_Stmt_tok;
_temp3591.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3591;});
_temp3590;}); break; case 232: _LL3589: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3593=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3593[ 0]=({ struct Cyc_Stmt_tok_struct _temp3594; _temp3594.tag= Cyc_Stmt_tok;
_temp3594.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3594;});
_temp3593;}); break; case 233: _LL3592: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 234: _LL3595:{ void* _temp3597= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3605; struct Cyc_Absyn_Fndecl* _temp3607; struct Cyc_Absyn_Stmt* _temp3609;
_LL3599: if(*(( int*) _temp3597) == Cyc_Parse_TopDecls_bl){ _LL3606: _temp3605=((
struct Cyc_Parse_TopDecls_bl_struct*) _temp3597)->f1; goto _LL3600;} else{ goto
_LL3601;} _LL3601: if(*(( int*) _temp3597) == Cyc_Parse_FnDecl_bl){ _LL3608:
_temp3607=(( struct Cyc_Parse_FnDecl_bl_struct*) _temp3597)->f1; goto _LL3602;}
else{ goto _LL3603;} _LL3603: if(*(( int*) _temp3597) == Cyc_Parse_Stmt_bl){
_LL3610: _temp3609=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3597)->f1; goto
_LL3604;} else{ goto _LL3598;} _LL3600: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3611=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3611[ 0]=({ struct Cyc_Stmt_tok_struct _temp3612; _temp3612.tag= Cyc_Stmt_tok;
_temp3612.f1= Cyc_Parse_flatten_declarations( _temp3605, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3612;}); _temp3611;}); goto _LL3598; _LL3602:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3613=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3613[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3614; _temp3614.tag= Cyc_Stmt_tok; _temp3614.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3615=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3615[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3616; _temp3616.tag= Cyc_Absyn_Fn_d;
_temp3616.f1= _temp3607; _temp3616;}); _temp3615;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3614;});
_temp3613;}); goto _LL3598; _LL3604: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3617=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3617[ 0]=({ struct Cyc_Stmt_tok_struct _temp3618; _temp3618.tag= Cyc_Stmt_tok;
_temp3618.f1= _temp3609; _temp3618;}); _temp3617;}); goto _LL3598; _LL3598:;}
break; case 235: _LL3596:{ void* _temp3620= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3628;
struct Cyc_Absyn_Fndecl* _temp3630; struct Cyc_Absyn_Stmt* _temp3632; _LL3622:
if(*(( int*) _temp3620) == Cyc_Parse_TopDecls_bl){ _LL3629: _temp3628=(( struct
Cyc_Parse_TopDecls_bl_struct*) _temp3620)->f1; goto _LL3623;} else{ goto _LL3624;}
_LL3624: if(*(( int*) _temp3620) == Cyc_Parse_FnDecl_bl){ _LL3631: _temp3630=((
struct Cyc_Parse_FnDecl_bl_struct*) _temp3620)->f1; goto _LL3625;} else{ goto
_LL3626;} _LL3626: if(*(( int*) _temp3620) == Cyc_Parse_Stmt_bl){ _LL3633:
_temp3632=(( struct Cyc_Parse_Stmt_bl_struct*) _temp3620)->f1; goto _LL3627;}
else{ goto _LL3621;} _LL3623: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3634=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3634[ 0]=({ struct Cyc_Stmt_tok_struct _temp3635; _temp3635.tag= Cyc_Stmt_tok;
_temp3635.f1= Cyc_Parse_flatten_declarations( _temp3628, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3635;});
_temp3634;}); goto _LL3621; _LL3625: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3636=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3636[ 0]=({ struct Cyc_Stmt_tok_struct _temp3637; _temp3637.tag= Cyc_Stmt_tok;
_temp3637.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3638=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3638[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3639; _temp3639.tag= Cyc_Absyn_Fn_d;
_temp3639.f1= _temp3630; _temp3639;}); _temp3638;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3637;}); _temp3636;}); goto _LL3621; _LL3627: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3640[ 0]=({ struct Cyc_Stmt_tok_struct _temp3641; _temp3641.tag= Cyc_Stmt_tok;
_temp3641.f1= Cyc_Absyn_seq_stmt( _temp3632, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3641;}); _temp3640;}); goto _LL3621; _LL3621:;} break; case 236: _LL3619:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3643=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3643[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3644; _temp3644.tag= Cyc_BlockItem_tok; _temp3644.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3645=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3645[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3646; _temp3646.tag= Cyc_Parse_TopDecls_bl;
_temp3646.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3646;}); _temp3645;})); _temp3644;}); _temp3643;}); break;
case 237: _LL3642: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3648=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3648[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3649; _temp3649.tag= Cyc_BlockItem_tok;
_temp3649.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3650=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3650[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3651; _temp3651.tag= Cyc_Parse_Stmt_bl;
_temp3651.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3651;}); _temp3650;})); _temp3649;}); _temp3648;}); break;
case 238: _LL3647: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3653=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3653[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3654; _temp3654.tag= Cyc_BlockItem_tok;
_temp3654.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3655=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3655[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3656; _temp3656.tag= Cyc_Parse_FnDecl_bl;
_temp3656.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3656;}); _temp3655;})); _temp3654;}); _temp3653;}); break;
case 239: _LL3652: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3658=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3658[ 0]=({ struct Cyc_Stmt_tok_struct _temp3659; _temp3659.tag= Cyc_Stmt_tok;
_temp3659.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3659;}); _temp3658;}); break; case 240: _LL3657: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3661=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3661[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3662; _temp3662.tag= Cyc_Stmt_tok; _temp3662.f1= Cyc_Absyn_ifthenelse_stmt(
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
_temp3662;}); _temp3661;}); break; case 241: _LL3660: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3664=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3664[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3665; _temp3665.tag= Cyc_Stmt_tok; _temp3665.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3665;}); _temp3664;}); break; case 242: _LL3663: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3667=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3667[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3668; _temp3668.tag= Cyc_Stmt_tok; _temp3668.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3668;}); _temp3667;}); break; case 243: _LL3666:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3670=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3670[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3671; _temp3671.tag= Cyc_SwitchClauseList_tok;
_temp3671.f1= 0; _temp3671;}); _temp3670;}); break; case 244: _LL3669: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3673=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3673[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3674; _temp3674.tag= Cyc_SwitchClauseList_tok;
_temp3674.f1=({ struct Cyc_List_List* _temp3675=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3675->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3676=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3676->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3676->pat_vars= 0; _temp3676->where_clause= 0; _temp3676->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3676->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3676;}); _temp3675->tl= 0; _temp3675;});
_temp3674;}); _temp3673;}); break; case 245: _LL3672: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3678=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3678[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3679; _temp3679.tag= Cyc_SwitchClauseList_tok;
_temp3679.f1=({ struct Cyc_List_List* _temp3680=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3680->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3681=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3681->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3681->pat_vars= 0; _temp3681->where_clause= 0;
_temp3681->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3681->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3681;});
_temp3680->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3680;}); _temp3679;}); _temp3678;}); break; case 246:
_LL3677: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3683=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3683[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3684; _temp3684.tag=
Cyc_SwitchClauseList_tok; _temp3684.f1=({ struct Cyc_List_List* _temp3685=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3685->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3686=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3686->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3686->pat_vars=
0; _temp3686->where_clause= 0; _temp3686->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3686->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3686;}); _temp3685->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3685;}); _temp3684;}); _temp3683;});
break; case 247: _LL3682: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3688=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3688[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3689; _temp3689.tag= Cyc_SwitchClauseList_tok; _temp3689.f1=({ struct Cyc_List_List*
_temp3690=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3690->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3691=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3691->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3691->pat_vars=
0; _temp3691->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3691->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3691->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3691;}); _temp3690->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3690;}); _temp3689;}); _temp3688;});
break; case 248: _LL3687: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3693=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3693[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3694; _temp3694.tag= Cyc_SwitchClauseList_tok; _temp3694.f1=({ struct Cyc_List_List*
_temp3695=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3695->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3696=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3696->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3696->pat_vars=
0; _temp3696->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3696->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3696->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3696;}); _temp3695->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3695;});
_temp3694;}); _temp3693;}); break; case 249: _LL3692: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3698=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3698[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3699; _temp3699.tag= Cyc_Stmt_tok; _temp3699.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3699;}); _temp3698;}); break; case 250: _LL3697: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3701=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3702; _temp3702.tag= Cyc_Stmt_tok; _temp3702.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3702;}); _temp3701;}); break; case 251: _LL3700: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3704=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3704[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3705; _temp3705.tag= Cyc_Stmt_tok; _temp3705.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3705;}); _temp3704;}); break; case 252: _LL3703: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3707=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3707[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3708; _temp3708.tag= Cyc_Stmt_tok; _temp3708.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3708;});
_temp3707;}); break; case 253: _LL3706: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3710=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3710[ 0]=({ struct Cyc_Stmt_tok_struct _temp3711; _temp3711.tag= Cyc_Stmt_tok;
_temp3711.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3711;}); _temp3710;}); break; case 254: _LL3709:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3713=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3713[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3714; _temp3714.tag= Cyc_Stmt_tok; _temp3714.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
_temp3714;}); _temp3713;}); break; case 255: _LL3712: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3716=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3716[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3717; _temp3717.tag= Cyc_Stmt_tok; _temp3717.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3717;});
_temp3716;}); break; case 256: _LL3715: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3719=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3719[ 0]=({ struct Cyc_Stmt_tok_struct _temp3720; _temp3720.tag= Cyc_Stmt_tok;
_temp3720.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
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
_temp3720;}); _temp3719;}); break; case 257: _LL3718: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3722=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3722[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3723; _temp3723.tag= Cyc_Stmt_tok; _temp3723.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
yylsp_offset)).last_line)); _temp3723;}); _temp3722;}); break; case 258: _LL3721:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3725=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3725[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3726; _temp3726.tag= Cyc_Stmt_tok; _temp3726.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
_temp3726;}); _temp3725;}); break; case 259: _LL3724: { struct Cyc_List_List*
_temp3728= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3729; _LL3729: { struct Cyc_Absyn_Stmt* _temp3730=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3731; _LL3731: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3732=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3732[ 0]=({ struct Cyc_Stmt_tok_struct _temp3733; _temp3733.tag= Cyc_Stmt_tok;
_temp3733.f1= Cyc_Parse_flatten_declarations( _temp3728, _temp3730); _temp3733;});
_temp3732;}); break;}} case 260: _LL3727: { struct Cyc_List_List* _temp3735= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3736; _LL3736: { struct Cyc_Absyn_Stmt* _temp3737= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3738;
_LL3738: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3739=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3739[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3740; _temp3740.tag= Cyc_Stmt_tok; _temp3740.f1= Cyc_Parse_flatten_declarations(
_temp3735, _temp3737); _temp3740;}); _temp3739;}); break;}} case 261: _LL3734: {
struct Cyc_List_List* _temp3742= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3743; _LL3743: { struct Cyc_Absyn_Stmt*
_temp3744= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3745; _LL3745: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3746=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3746[ 0]=({ struct Cyc_Stmt_tok_struct _temp3747; _temp3747.tag= Cyc_Stmt_tok;
_temp3747.f1= Cyc_Parse_flatten_declarations( _temp3742, _temp3744); _temp3747;});
_temp3746;}); break;}} case 262: _LL3741: { struct Cyc_List_List* _temp3749= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3750; _LL3750: { struct Cyc_Absyn_Stmt* _temp3751= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
goto _LL3752; _LL3752: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3753=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3753[ 0]=({ struct Cyc_Stmt_tok_struct _temp3754; _temp3754.tag= Cyc_Stmt_tok;
_temp3754.f1= Cyc_Parse_flatten_declarations( _temp3749, _temp3751); _temp3754;});
_temp3753;}); break;}} case 263: _LL3748: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3756=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3756[ 0]=({ struct Cyc_Stmt_tok_struct _temp3757; _temp3757.tag= Cyc_Stmt_tok;
_temp3757.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3758=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3758[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3758;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3757;});
_temp3756;}); break; case 264: _LL3755: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3760=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3760[ 0]=({ struct Cyc_Stmt_tok_struct _temp3761; _temp3761.tag= Cyc_Stmt_tok;
_temp3761.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3761;}); _temp3760;}); break; case 265: _LL3759: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3763=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3763[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3764; _temp3764.tag= Cyc_Stmt_tok; _temp3764.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3764;}); _temp3763;}); break; case 266: _LL3762: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3766=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3766[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3767; _temp3767.tag= Cyc_Stmt_tok; _temp3767.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3767;}); _temp3766;}); break; case 267:
_LL3765: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3769=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3769[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3770; _temp3770.tag= Cyc_Stmt_tok; _temp3770.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3770;});
_temp3769;}); break; case 268: _LL3768: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3772=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3772[ 0]=({ struct Cyc_Stmt_tok_struct _temp3773; _temp3773.tag= Cyc_Stmt_tok;
_temp3773.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3773;}); _temp3772;}); break; case 269: _LL3771: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3775=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3775[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3776; _temp3776.tag= Cyc_Stmt_tok; _temp3776.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3776;}); _temp3775;}); break; case 270:
_LL3774: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3778=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3778[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3779; _temp3779.tag= Cyc_Stmt_tok; _temp3779.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3779;}); _temp3778;}); break; case 271:
_LL3777: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3781=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3781[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3782; _temp3782.tag= Cyc_Pattern_tok; _temp3782.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3782;});
_temp3781;}); break; case 272: _LL3780: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 273: _LL3783: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3785=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3785[ 0]=({ struct Cyc_Pattern_tok_struct _temp3786; _temp3786.tag= Cyc_Pattern_tok;
_temp3786.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3787=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3787[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3788; _temp3788.tag= Cyc_Absyn_Int_p;
_temp3788.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3788.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3788;}); _temp3787;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3786;}); _temp3785;}); break; case 274: _LL3784: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3790=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3790[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3791; _temp3791.tag= Cyc_Pattern_tok; _temp3791.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3792=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3792[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3793; _temp3793.tag= Cyc_Absyn_Int_p; _temp3793.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3793.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3793;}); _temp3792;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3791;}); _temp3790;}); break; case 275: _LL3789: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3795=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3795[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3796; _temp3796.tag= Cyc_Pattern_tok; _temp3796.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3797=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3797[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3798; _temp3798.tag= Cyc_Absyn_Float_p; _temp3798.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3798;}); _temp3797;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3796;});
_temp3795;}); break; case 276: _LL3794: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3800=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3800[ 0]=({ struct Cyc_Pattern_tok_struct _temp3801; _temp3801.tag= Cyc_Pattern_tok;
_temp3801.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3802=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3802[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3803; _temp3803.tag= Cyc_Absyn_Char_p;
_temp3803.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3803;}); _temp3802;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3801;}); _temp3800;}); break; case 277: _LL3799: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3805=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3805[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3806; _temp3806.tag= Cyc_Pattern_tok; _temp3806.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3806;});
_temp3805;}); break; case 278: _LL3804: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3808=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3808[ 0]=({ struct Cyc_Pattern_tok_struct _temp3809; _temp3809.tag= Cyc_Pattern_tok;
_temp3809.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3810=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3810[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3811; _temp3811.tag=
Cyc_Absyn_UnknownId_p; _temp3811.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3811;}); _temp3810;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3809;}); _temp3808;}); break; case 279: _LL3807: { struct Cyc_List_List*
_temp3813=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3814; _LL3814: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3815=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3815[ 0]=({ struct Cyc_Pattern_tok_struct _temp3816; _temp3816.tag= Cyc_Pattern_tok;
_temp3816.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3817=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3817[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3818; _temp3818.tag= Cyc_Absyn_UnknownCall_p; _temp3818.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3818.f2=
_temp3813; _temp3818.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3818;}); _temp3817;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3816;}); _temp3815;}); break;} case 280: _LL3812: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3820=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3820[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3821; _temp3821.tag= Cyc_Pattern_tok; _temp3821.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3822=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3822[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3823; _temp3823.tag= Cyc_Absyn_Tuple_p; _temp3823.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3823;}); _temp3822;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3821;});
_temp3820;}); break; case 281: _LL3819: { struct Cyc_List_List* _temp3825=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3826; _LL3826: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3827=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3827[ 0]=({ struct Cyc_Pattern_tok_struct _temp3828; _temp3828.tag= Cyc_Pattern_tok;
_temp3828.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3829=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3829[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3830; _temp3830.tag= Cyc_Absyn_UnknownFields_p; _temp3830.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3830.f2=
_temp3825; _temp3830.f3= 0; _temp3830;}); _temp3829;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3828;}); _temp3827;}); break;} case 282: _LL3824: { struct Cyc_List_List*
_temp3832=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3833; _LL3833: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3834=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3834[ 0]=({ struct Cyc_Pattern_tok_struct _temp3835; _temp3835.tag= Cyc_Pattern_tok;
_temp3835.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3836=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3836[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3837; _temp3837.tag= Cyc_Absyn_UnknownFields_p; _temp3837.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3837.f2=
_temp3832; _temp3837.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3837;}); _temp3836;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3835;}); _temp3834;}); break;} case 283: _LL3831: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3839=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3839[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3840; _temp3840.tag= Cyc_Pattern_tok; _temp3840.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3841=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3841[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3842; _temp3842.tag= Cyc_Absyn_Pointer_p;
_temp3842.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3842;}); _temp3841;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3840;}); _temp3839;}); break; case 284: _LL3838: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3844=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3844[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3845; _temp3845.tag= Cyc_Pattern_tok; _temp3845.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3846=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3846[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3847; _temp3847.tag= Cyc_Absyn_Reference_p;
_temp3847.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3848=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3848->f1=( void*) Cyc_Absyn_Loc_n;
_temp3848->f2=({ struct _tagged_string* _temp3849=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3849[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3849;});
_temp3848;}),( void*) Cyc_Absyn_VoidType, 0); _temp3847;}); _temp3846;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3845;}); _temp3844;}); break; case 285: _LL3843: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3851=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3851[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3852; _temp3852.tag= Cyc_PatternList_tok;
_temp3852.f1= 0; _temp3852;}); _temp3851;}); break; case 286: _LL3850: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3854=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3854[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3855; _temp3855.tag= Cyc_PatternList_tok;
_temp3855.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3855;}); _temp3854;}); break; case 287: _LL3853: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3857=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3857[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3858; _temp3858.tag= Cyc_PatternList_tok;
_temp3858.f1=({ struct Cyc_List_List* _temp3859=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3859->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3859->tl=
0; _temp3859;}); _temp3858;}); _temp3857;}); break; case 288: _LL3856: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3861=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3861[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3862; _temp3862.tag= Cyc_PatternList_tok;
_temp3862.f1=({ struct Cyc_List_List* _temp3863=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3863->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3863->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3863;}); _temp3862;}); _temp3861;}); break; case 289: _LL3860: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3865=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3865[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3866; _temp3866.tag= Cyc_FieldPattern_tok;
_temp3866.f1=({ struct _tuple14* _temp3867=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3867->f1= 0; _temp3867->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3867;});
_temp3866;}); _temp3865;}); break; case 290: _LL3864: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3869=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3869[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3870; _temp3870.tag= Cyc_FieldPattern_tok;
_temp3870.f1=({ struct _tuple14* _temp3871=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3871->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3871->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3871;});
_temp3870;}); _temp3869;}); break; case 291: _LL3868: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3873=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3873[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3874; _temp3874.tag= Cyc_FieldPatternList_tok;
_temp3874.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3874;}); _temp3873;}); break; case 292: _LL3872: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3876=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3876[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3877; _temp3877.tag= Cyc_FieldPatternList_tok;
_temp3877.f1=({ struct Cyc_List_List* _temp3878=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3878->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3878->tl=
0; _temp3878;}); _temp3877;}); _temp3876;}); break; case 293: _LL3875: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3880=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3880[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3881; _temp3881.tag= Cyc_FieldPatternList_tok;
_temp3881.f1=({ struct Cyc_List_List* _temp3882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3882->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3882->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3882;}); _temp3881;}); _temp3880;}); break; case 294: _LL3879: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 295:
_LL3883: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3885=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3885[ 0]=({ struct Cyc_Exp_tok_struct
_temp3886; _temp3886.tag= Cyc_Exp_tok; _temp3886.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3886;}); _temp3885;}); break; case 296: _LL3884: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 297: _LL3887: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3889=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3889[ 0]=({ struct Cyc_Exp_tok_struct
_temp3890; _temp3890.tag= Cyc_Exp_tok; _temp3890.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
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
_temp3890;}); _temp3889;}); break; case 298: _LL3888: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3892=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3892[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3893; _temp3893.tag= Cyc_Primopopt_tok; _temp3893.f1=
0; _temp3893;}); _temp3892;}); break; case 299: _LL3891: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3895=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3895[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3896; _temp3896.tag= Cyc_Primopopt_tok; _temp3896.f1=({
struct Cyc_Core_Opt* _temp3897=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3897->v=( void*)(( void*) Cyc_Absyn_Times); _temp3897;});
_temp3896;}); _temp3895;}); break; case 300: _LL3894: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3899=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3899[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3900; _temp3900.tag= Cyc_Primopopt_tok; _temp3900.f1=({
struct Cyc_Core_Opt* _temp3901=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3901->v=( void*)(( void*) Cyc_Absyn_Div); _temp3901;});
_temp3900;}); _temp3899;}); break; case 301: _LL3898: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3903=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3903[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3904; _temp3904.tag= Cyc_Primopopt_tok; _temp3904.f1=({
struct Cyc_Core_Opt* _temp3905=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3905->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3905;});
_temp3904;}); _temp3903;}); break; case 302: _LL3902: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3907=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3907[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3908; _temp3908.tag= Cyc_Primopopt_tok; _temp3908.f1=({
struct Cyc_Core_Opt* _temp3909=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3909->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3909;});
_temp3908;}); _temp3907;}); break; case 303: _LL3906: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3911=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3911[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3912; _temp3912.tag= Cyc_Primopopt_tok; _temp3912.f1=({
struct Cyc_Core_Opt* _temp3913=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3913->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3913;});
_temp3912;}); _temp3911;}); break; case 304: _LL3910: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3915=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3915[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3916; _temp3916.tag= Cyc_Primopopt_tok; _temp3916.f1=({
struct Cyc_Core_Opt* _temp3917=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3917->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3917;});
_temp3916;}); _temp3915;}); break; case 305: _LL3914: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3919=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3919[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3920; _temp3920.tag= Cyc_Primopopt_tok; _temp3920.f1=({
struct Cyc_Core_Opt* _temp3921=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3921->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3921;});
_temp3920;}); _temp3919;}); break; case 306: _LL3918: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3923=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3923[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3924; _temp3924.tag= Cyc_Primopopt_tok; _temp3924.f1=({
struct Cyc_Core_Opt* _temp3925=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3925->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3925;});
_temp3924;}); _temp3923;}); break; case 307: _LL3922: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3927=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3927[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3928; _temp3928.tag= Cyc_Primopopt_tok; _temp3928.f1=({
struct Cyc_Core_Opt* _temp3929=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3929->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3929;});
_temp3928;}); _temp3927;}); break; case 308: _LL3926: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3931=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3931[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3932; _temp3932.tag= Cyc_Primopopt_tok; _temp3932.f1=({
struct Cyc_Core_Opt* _temp3933=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3933->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3933;});
_temp3932;}); _temp3931;}); break; case 309: _LL3930: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 310: _LL3934: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3936=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3936[ 0]=({ struct Cyc_Exp_tok_struct
_temp3937; _temp3937.tag= Cyc_Exp_tok; _temp3937.f1= Cyc_Absyn_conditional_exp(
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
_temp3937;}); _temp3936;}); break; case 311: _LL3935: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3939=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3939[ 0]=({ struct Cyc_Exp_tok_struct
_temp3940; _temp3940.tag= Cyc_Exp_tok; _temp3940.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3940;}); _temp3939;}); break; case 312: _LL3938: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3942=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3942[ 0]=({ struct Cyc_Exp_tok_struct
_temp3943; _temp3943.tag= Cyc_Exp_tok; _temp3943.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3943;}); _temp3942;}); break; case 313: _LL3941: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3945=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3945[ 0]=({ struct Cyc_Exp_tok_struct
_temp3946; _temp3946.tag= Cyc_Exp_tok; _temp3946.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3946;}); _temp3945;}); break; case 314: _LL3944: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3948=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3948[ 0]=({ struct Cyc_Exp_tok_struct
_temp3949; _temp3949.tag= Cyc_Exp_tok; _temp3949.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3949;});
_temp3948;}); break; case 315: _LL3947: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3951=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3951[ 0]=({ struct Cyc_Exp_tok_struct _temp3952; _temp3952.tag= Cyc_Exp_tok;
_temp3952.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3952;}); _temp3951;}); break; case 316: _LL3950: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 317: _LL3953: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 318: _LL3954: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3956=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3956[ 0]=({ struct Cyc_Exp_tok_struct
_temp3957; _temp3957.tag= Cyc_Exp_tok; _temp3957.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3957;}); _temp3956;}); break; case 319: _LL3955: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 320: _LL3958: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3960=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3960[ 0]=({ struct Cyc_Exp_tok_struct
_temp3961; _temp3961.tag= Cyc_Exp_tok; _temp3961.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3961;}); _temp3960;}); break; case 321: _LL3959: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 322: _LL3962: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3964=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3964[ 0]=({ struct Cyc_Exp_tok_struct
_temp3965; _temp3965.tag= Cyc_Exp_tok; _temp3965.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3965;});
_temp3964;}); break; case 323: _LL3963: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 324: _LL3966: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3968=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3968[ 0]=({ struct Cyc_Exp_tok_struct _temp3969; _temp3969.tag= Cyc_Exp_tok;
_temp3969.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3969;}); _temp3968;}); break; case 325: _LL3967: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 326: _LL3970: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3972=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3972[ 0]=({ struct Cyc_Exp_tok_struct
_temp3973; _temp3973.tag= Cyc_Exp_tok; _temp3973.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3973;});
_temp3972;}); break; case 327: _LL3971: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 328: _LL3974: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3976=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3976[ 0]=({ struct Cyc_Exp_tok_struct _temp3977; _temp3977.tag= Cyc_Exp_tok;
_temp3977.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3977;});
_temp3976;}); break; case 329: _LL3975: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3979=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3979[ 0]=({ struct Cyc_Exp_tok_struct _temp3980; _temp3980.tag= Cyc_Exp_tok;
_temp3980.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3980;});
_temp3979;}); break; case 330: _LL3978: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 331: _LL3981: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3983=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3983[ 0]=({ struct Cyc_Exp_tok_struct _temp3984; _temp3984.tag= Cyc_Exp_tok;
_temp3984.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3984;});
_temp3983;}); break; case 332: _LL3982: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3986=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3986[ 0]=({ struct Cyc_Exp_tok_struct _temp3987; _temp3987.tag= Cyc_Exp_tok;
_temp3987.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3987;});
_temp3986;}); break; case 333: _LL3985: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3989=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3989[ 0]=({ struct Cyc_Exp_tok_struct _temp3990; _temp3990.tag= Cyc_Exp_tok;
_temp3990.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3990;});
_temp3989;}); break; case 334: _LL3988: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3992=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3992[ 0]=({ struct Cyc_Exp_tok_struct _temp3993; _temp3993.tag= Cyc_Exp_tok;
_temp3993.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3993;});
_temp3992;}); break; case 335: _LL3991: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 336: _LL3994: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3996=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3996[ 0]=({ struct Cyc_Exp_tok_struct _temp3997; _temp3997.tag= Cyc_Exp_tok;
_temp3997.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3997;}); _temp3996;}); break; case 337: _LL3995: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3999=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3999[ 0]=({ struct Cyc_Exp_tok_struct
_temp4000; _temp4000.tag= Cyc_Exp_tok; _temp4000.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4000;});
_temp3999;}); break; case 338: _LL3998: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 339: _LL4001: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4003=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4003[ 0]=({ struct Cyc_Exp_tok_struct _temp4004; _temp4004.tag= Cyc_Exp_tok;
_temp4004.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4004;}); _temp4003;}); break; case 340: _LL4002: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4006=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4006[ 0]=({ struct Cyc_Exp_tok_struct
_temp4007; _temp4007.tag= Cyc_Exp_tok; _temp4007.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4007;});
_temp4006;}); break; case 341: _LL4005: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 342: _LL4008: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4010=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4010[ 0]=({ struct Cyc_Exp_tok_struct _temp4011; _temp4011.tag= Cyc_Exp_tok;
_temp4011.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4011;}); _temp4010;}); break; case 343: _LL4009: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4013=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4013[ 0]=({ struct Cyc_Exp_tok_struct
_temp4014; _temp4014.tag= Cyc_Exp_tok; _temp4014.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4014;});
_temp4013;}); break; case 344: _LL4012: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4016=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4016[ 0]=({ struct Cyc_Exp_tok_struct _temp4017; _temp4017.tag= Cyc_Exp_tok;
_temp4017.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4017;}); _temp4016;}); break; case 345: _LL4015: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 346: _LL4018: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4020=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4020[ 0]=({ struct Cyc_Exp_tok_struct
_temp4021; _temp4021.tag= Cyc_Exp_tok; _temp4021.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4021;}); _temp4020;}); break; case 347: _LL4019: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 348: _LL4022: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4024=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4024[ 0]=({ struct Cyc_Exp_tok_struct
_temp4025; _temp4025.tag= Cyc_Exp_tok; _temp4025.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4025;}); _temp4024;}); break; case 349: _LL4023: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4027=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4027[ 0]=({ struct Cyc_Exp_tok_struct
_temp4028; _temp4028.tag= Cyc_Exp_tok; _temp4028.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4028;}); _temp4027;}); break; case 350: _LL4026: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4030=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4030[ 0]=({ struct Cyc_Exp_tok_struct
_temp4031; _temp4031.tag= Cyc_Exp_tok; _temp4031.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4031;}); _temp4030;}); break; case 351: _LL4029: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4033=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4033[ 0]=({ struct Cyc_Exp_tok_struct
_temp4034; _temp4034.tag= Cyc_Exp_tok; _temp4034.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4034;}); _temp4033;}); break; case 352: _LL4032: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 353: _LL4035: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4037=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4037[ 0]=({ struct Cyc_Exp_tok_struct
_temp4038; _temp4038.tag= Cyc_Exp_tok; _temp4038.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4038;}); _temp4037;}); break; case 354: _LL4036: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4040=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4040[ 0]=({ struct Cyc_Exp_tok_struct
_temp4041; _temp4041.tag= Cyc_Exp_tok; _temp4041.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4041;}); _temp4040;}); break; case 355: _LL4039:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4043=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4043[ 0]=({ struct Cyc_Exp_tok_struct
_temp4044; _temp4044.tag= Cyc_Exp_tok; _temp4044.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4044;}); _temp4043;}); break; case 356: _LL4042: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4046=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4046[ 0]=({ struct Cyc_Exp_tok_struct
_temp4047; _temp4047.tag= Cyc_Exp_tok; _temp4047.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4047;}); _temp4046;}); break; case 357: _LL4045: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4049=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4049[ 0]=({ struct Cyc_Exp_tok_struct
_temp4050; _temp4050.tag= Cyc_Exp_tok; _temp4050.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp4051=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp4051[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp4052; _temp4052.tag= Cyc_Absyn_Malloc_e;
_temp4052.f1= 0; _temp4052.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp4052;}); _temp4051;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4050;});
_temp4049;}); break; case 358: _LL4048: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4054=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4054[ 0]=({ struct Cyc_Exp_tok_struct _temp4055; _temp4055.tag= Cyc_Exp_tok;
_temp4055.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp4056=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp4056[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp4057; _temp4057.tag= Cyc_Absyn_Malloc_e;
_temp4057.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp4057.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp4057;}); _temp4056;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4055;}); _temp4054;}); break; case 359: _LL4053: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4059=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4059[ 0]=({ struct Cyc_Primop_tok_struct
_temp4060; _temp4060.tag= Cyc_Primop_tok; _temp4060.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp4060;}); _temp4059;}); break; case 360: _LL4058: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4062=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4062[ 0]=({ struct Cyc_Primop_tok_struct
_temp4063; _temp4063.tag= Cyc_Primop_tok; _temp4063.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp4063;}); _temp4062;}); break; case 361: _LL4061: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4065=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4065[ 0]=({ struct Cyc_Primop_tok_struct
_temp4066; _temp4066.tag= Cyc_Primop_tok; _temp4066.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp4066;}); _temp4065;}); break; case 362: _LL4064: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4068=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4068[ 0]=({ struct Cyc_Primop_tok_struct
_temp4069; _temp4069.tag= Cyc_Primop_tok; _temp4069.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp4069;}); _temp4068;}); break; case 363: _LL4067: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4071=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4071[ 0]=({ struct Cyc_Primop_tok_struct
_temp4072; _temp4072.tag= Cyc_Primop_tok; _temp4072.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp4072;}); _temp4071;}); break; case 364: _LL4070: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4074=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4074[ 0]=({ struct Cyc_Primop_tok_struct
_temp4075; _temp4075.tag= Cyc_Primop_tok; _temp4075.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp4075;}); _temp4074;}); break; case 365: _LL4073: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4077=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4077[ 0]=({ struct Cyc_Primop_tok_struct
_temp4078; _temp4078.tag= Cyc_Primop_tok; _temp4078.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp4078;}); _temp4077;}); break; case 366: _LL4076: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4080=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4080[ 0]=({ struct Cyc_Primop_tok_struct
_temp4081; _temp4081.tag= Cyc_Primop_tok; _temp4081.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp4081;}); _temp4080;}); break; case 367: _LL4079: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4083=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4083[ 0]=({ struct Cyc_Primop_tok_struct
_temp4084; _temp4084.tag= Cyc_Primop_tok; _temp4084.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp4084;}); _temp4083;}); break; case 368: _LL4082: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 369: _LL4085: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4087=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4087[ 0]=({ struct Cyc_Exp_tok_struct
_temp4088; _temp4088.tag= Cyc_Exp_tok; _temp4088.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4088;}); _temp4087;}); break; case 370: _LL4086: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4090=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4090[ 0]=({ struct Cyc_Exp_tok_struct
_temp4091; _temp4091.tag= Cyc_Exp_tok; _temp4091.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4091;}); _temp4090;}); break; case 371: _LL4089: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4093=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4093[ 0]=({ struct Cyc_Exp_tok_struct
_temp4094; _temp4094.tag= Cyc_Exp_tok; _temp4094.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4094;}); _temp4093;}); break; case 372: _LL4092: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4096=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4096[ 0]=({ struct Cyc_Exp_tok_struct
_temp4097; _temp4097.tag= Cyc_Exp_tok; _temp4097.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4098=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4098[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4098;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4097;}); _temp4096;}); break; case 373: _LL4095: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4100=(* q).f1; struct Cyc_List_List* _temp4110; struct Cyc_List_List*
_temp4112; _LL4102: if( _temp4100 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4103;}
else{ goto _LL4104;} _LL4104: if(( unsigned int) _temp4100 > 1u?*(( int*)
_temp4100) == Cyc_Absyn_Rel_n: 0){ _LL4111: _temp4110=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4100)->f1; if( _temp4110 == 0){ goto _LL4105;} else{ goto _LL4106;}} else{
goto _LL4106;} _LL4106: if(( unsigned int) _temp4100 > 1u?*(( int*) _temp4100)
== Cyc_Absyn_Abs_n: 0){ _LL4113: _temp4112=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4100)->f1; if( _temp4112 == 0){ goto _LL4107;} else{ goto _LL4108;}} else{
goto _LL4108;} _LL4108: goto _LL4109; _LL4103: goto _LL4101; _LL4105: goto
_LL4101; _LL4107: goto _LL4101; _LL4109: Cyc_Parse_err(({ unsigned char*
_temp4114="struct field name is qualified"; struct _tagged_string _temp4115;
_temp4115.curr= _temp4114; _temp4115.base= _temp4114; _temp4115.last_plus_one=
_temp4114 + 31; _temp4115;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4101;
_LL4101:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4116=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4116[ 0]=({ struct Cyc_Exp_tok_struct
_temp4117; _temp4117.tag= Cyc_Exp_tok; _temp4117.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4117;}); _temp4116;}); break;} case 374:
_LL4099: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4119=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4119[ 0]=({ struct Cyc_Exp_tok_struct
_temp4120; _temp4120.tag= Cyc_Exp_tok; _temp4120.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4121=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4121[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4121;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4120;}); _temp4119;}); break; case 375: _LL4118: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4123=(* q).f1; struct Cyc_List_List* _temp4133; struct Cyc_List_List*
_temp4135; _LL4125: if( _temp4123 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4126;}
else{ goto _LL4127;} _LL4127: if(( unsigned int) _temp4123 > 1u?*(( int*)
_temp4123) == Cyc_Absyn_Rel_n: 0){ _LL4134: _temp4133=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4123)->f1; if( _temp4133 == 0){ goto _LL4128;} else{ goto _LL4129;}} else{
goto _LL4129;} _LL4129: if(( unsigned int) _temp4123 > 1u?*(( int*) _temp4123)
== Cyc_Absyn_Abs_n: 0){ _LL4136: _temp4135=(( struct Cyc_Absyn_Abs_n_struct*)
_temp4123)->f1; if( _temp4135 == 0){ goto _LL4130;} else{ goto _LL4131;}} else{
goto _LL4131;} _LL4131: goto _LL4132; _LL4126: goto _LL4124; _LL4128: goto
_LL4124; _LL4130: goto _LL4124; _LL4132: Cyc_Parse_err(({ unsigned char*
_temp4137="struct field is qualified with module name"; struct _tagged_string
_temp4138; _temp4138.curr= _temp4137; _temp4138.base= _temp4137; _temp4138.last_plus_one=
_temp4137 + 43; _temp4138;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4124;
_LL4124:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4139=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4139[ 0]=({ struct Cyc_Exp_tok_struct
_temp4140; _temp4140.tag= Cyc_Exp_tok; _temp4140.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4140;}); _temp4139;}); break;} case 376:
_LL4122: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4142=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4142[ 0]=({ struct Cyc_Exp_tok_struct
_temp4143; _temp4143.tag= Cyc_Exp_tok; _temp4143.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4143;}); _temp4142;}); break; case 377: _LL4141: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4145=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4145[ 0]=({ struct Cyc_Exp_tok_struct
_temp4146; _temp4146.tag= Cyc_Exp_tok; _temp4146.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4146;}); _temp4145;}); break; case 378: _LL4144: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4148=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4148[ 0]=({ struct Cyc_Exp_tok_struct
_temp4149; _temp4149.tag= Cyc_Exp_tok; _temp4149.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4150=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4150[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4151; _temp4151.tag= Cyc_Absyn_CompoundLit_e;
_temp4151.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4151.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4151;}); _temp4150;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4149;}); _temp4148;}); break; case 379: _LL4147: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4153=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4153[ 0]=({ struct Cyc_Exp_tok_struct
_temp4154; _temp4154.tag= Cyc_Exp_tok; _temp4154.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4155=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4155[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4156; _temp4156.tag= Cyc_Absyn_CompoundLit_e;
_temp4156.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4156.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4156;}); _temp4155;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4154;}); _temp4153;}); break; case 380: _LL4152: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4158=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4158[ 0]=({ struct Cyc_Exp_tok_struct
_temp4159; _temp4159.tag= Cyc_Exp_tok; _temp4159.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4160=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4160[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4161; _temp4161.tag= Cyc_Absyn_Fill_e; _temp4161.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4161;});
_temp4160;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4159;});
_temp4158;}); break; case 381: _LL4157: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4163=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4163[ 0]=({ struct Cyc_Exp_tok_struct _temp4164; _temp4164.tag= Cyc_Exp_tok;
_temp4164.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4165=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4165[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4166; _temp4166.tag= Cyc_Absyn_Codegen_e;
_temp4166.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4166;}); _temp4165;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4164;}); _temp4163;}); break; case 382: _LL4162: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4168=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4168[ 0]=({ struct Cyc_Exp_tok_struct
_temp4169; _temp4169.tag= Cyc_Exp_tok; _temp4169.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4170=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4170[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4171; _temp4171.tag= Cyc_Absyn_UnknownId_e;
_temp4171.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4171;}); _temp4170;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4169;}); _temp4168;}); break; case 383: _LL4167: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 384: _LL4172: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4174=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4174[ 0]=({ struct Cyc_Exp_tok_struct
_temp4175; _temp4175.tag= Cyc_Exp_tok; _temp4175.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4175;}); _temp4174;}); break; case 385: _LL4173: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 386: _LL4176: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4178=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4178[ 0]=({ struct Cyc_Exp_tok_struct
_temp4179; _temp4179.tag= Cyc_Exp_tok; _temp4179.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4180=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4180[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4181; _temp4181.tag=
Cyc_Absyn_UnknownId_e; _temp4181.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4181;}); _temp4180;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4179;}); _temp4178;}); break; case 387: _LL4177:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4183=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4183[ 0]=({ struct Cyc_Exp_tok_struct
_temp4184; _temp4184.tag= Cyc_Exp_tok; _temp4184.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4185=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4185[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4186; _temp4186.tag=
Cyc_Absyn_UnknownId_e; _temp4186.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4186;}); _temp4185;}), Cyc_Position_segment_of_abs((*((
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
_temp4184;}); _temp4183;}); break; case 388: _LL4182: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4188=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4188[ 0]=({ struct Cyc_Exp_tok_struct
_temp4189; _temp4189.tag= Cyc_Exp_tok; _temp4189.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4189;}); _temp4188;}); break; case 389: _LL4187: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4191=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4191[ 0]=({ struct Cyc_Exp_tok_struct
_temp4192; _temp4192.tag= Cyc_Exp_tok; _temp4192.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4193=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4193[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4194; _temp4194.tag= Cyc_Absyn_Struct_e;
_temp4194.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4194.f2= 0; _temp4194.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4194.f4=
0; _temp4194;}); _temp4193;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4192;});
_temp4191;}); break; case 390: _LL4190: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4196=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4196[ 0]=({ struct Cyc_Exp_tok_struct _temp4197; _temp4197.tag= Cyc_Exp_tok;
_temp4197.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4197;});
_temp4196;}); break; case 391: _LL4195: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4199=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4199[ 0]=({ struct Cyc_ExpList_tok_struct _temp4200; _temp4200.tag= Cyc_ExpList_tok;
_temp4200.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4200;}); _temp4199;}); break; case 392: _LL4198: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4202=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4202[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4203; _temp4203.tag= Cyc_ExpList_tok; _temp4203.f1=({ struct Cyc_List_List*
_temp4204=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4204->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4204->tl= 0; _temp4204;}); _temp4203;}); _temp4202;});
break; case 393: _LL4201: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4206=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4206[ 0]=({ struct Cyc_ExpList_tok_struct _temp4207; _temp4207.tag= Cyc_ExpList_tok;
_temp4207.f1=({ struct Cyc_List_List* _temp4208=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4208->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4208->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4208;}); _temp4207;}); _temp4206;}); break; case 394: _LL4205: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4210=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4210[ 0]=({ struct Cyc_Exp_tok_struct
_temp4211; _temp4211.tag= Cyc_Exp_tok; _temp4211.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4211;}); _temp4210;}); break; case 395: _LL4209: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4213=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4213[ 0]=({ struct Cyc_Exp_tok_struct
_temp4214; _temp4214.tag= Cyc_Exp_tok; _temp4214.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4214;}); _temp4213;}); break; case 396: _LL4212: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4216=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4216[ 0]=({ struct Cyc_Exp_tok_struct
_temp4217; _temp4217.tag= Cyc_Exp_tok; _temp4217.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4217;}); _temp4216;}); break; case 397: _LL4215: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4219=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4219[ 0]=({ struct Cyc_Exp_tok_struct
_temp4220; _temp4220.tag= Cyc_Exp_tok; _temp4220.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4220;}); _temp4219;}); break; case 398: _LL4218: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4222=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4222[ 0]=({ struct Cyc_QualId_tok_struct
_temp4223; _temp4223.tag= Cyc_QualId_tok; _temp4223.f1=({ struct _tuple1*
_temp4224=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4224->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4226=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4226[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4227; _temp4227.tag= Cyc_Absyn_Rel_n; _temp4227.f1= 0; _temp4227;});
_temp4226;}); _temp4224->f2=({ struct _tagged_string* _temp4225=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp4225[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4225;});
_temp4224;}); _temp4223;}); _temp4222;}); break; case 399: _LL4221: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4228:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
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
yylsp_offset)).text=({ unsigned char* _temp4230=""; struct _tagged_string
_temp4231; _temp4231.curr= _temp4230; _temp4231.base= _temp4230; _temp4231.last_plus_one=
_temp4230 + 1; _temp4231;});} else{(*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset + yylen) - 1)).last_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_column=(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset +
yylen) - 1)).last_column;} yyn=( int) Cyc_yyr1[ _check_known_subscript_notnull(
400u, yyn)]; yystate= Cyc_yypgoto[ _check_known_subscript_notnull( 110u, yyn -
124)] +*(( short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset); if(( yystate >=
0? yystate <= 4259: 0)? Cyc_yycheck[ _check_known_subscript_notnull( 4260u,
yystate)] ==*(( short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), yyssp_offset): 0){ yystate=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4260u, yystate)];} else{
yystate=( int) Cyc_yydefgoto[ _check_known_subscript_notnull( 110u, yyn - 124)];}
goto yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 766u, yystate)]; if( yyn > - 32768? yyn < 4259:
0){ int sze= 0; struct _tagged_string msg; int x; int count; count= 0; for( x=
yyn < 0? - yyn: 0; x < 234u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4260u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 234u, x)]) + 15, count ++);}} msg=({
unsigned int _temp4232=( unsigned int)( sze + 15); unsigned char* _temp4233=(
unsigned char*) GC_malloc_atomic( sizeof( unsigned char) * _temp4232); struct
_tagged_string _temp4235={ _temp4233, _temp4233, _temp4233 + _temp4232};{
unsigned int _temp4234= _temp4232; unsigned int i; for( i= 0; i < _temp4234; i
++){ _temp4233[ i]='\000';}}; _temp4235;}); Cyc_String_strcpy( msg,({
unsigned char* _temp4236="parse error"; struct _tagged_string _temp4237;
_temp4237.curr= _temp4236; _temp4237.base= _temp4236; _temp4237.last_plus_one=
_temp4236 + 12; _temp4237;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 234u / sizeof( unsigned char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4260u, x + yyn)] == x){ Cyc_String_strcat( msg,
count == 0?({ unsigned char* _temp4238=", expecting `"; struct _tagged_string
_temp4239; _temp4239.curr= _temp4238; _temp4239.base= _temp4238; _temp4239.last_plus_one=
_temp4238 + 14; _temp4239;}):({ unsigned char* _temp4240=" or `"; struct
_tagged_string _temp4241; _temp4241.curr= _temp4240; _temp4241.base= _temp4240;
_temp4241.last_plus_one= _temp4240 + 6; _temp4241;})); Cyc_String_strcat( msg,
Cyc_yytname[ _check_known_subscript_notnull( 234u, x)]); Cyc_String_strcat( msg,({
unsigned char* _temp4242="'"; struct _tagged_string _temp4243; _temp4243.curr=
_temp4242; _temp4243.base= _temp4242; _temp4243.last_plus_one= _temp4242 + 2;
_temp4243;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(({
unsigned char* _temp4244="parse error"; struct _tagged_string _temp4245;
_temp4245.curr= _temp4244; _temp4245.base= _temp4244; _temp4245.last_plus_one=
_temp4244 + 12; _temp4245;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), -- yyssp_offset);
yylsp_offset --; yyerrhandle: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 766u, yystate)]; if( yyn == - 32768){ goto
yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4259)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4260u, yyn)] != 1){ goto yyerrdefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4260u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrpop;}} if( yyn == 765){ return 0;}*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; goto yynewstate;}
void Cyc_yyprint( int i, void* v){ void* _temp4261= v; struct _tuple16*
_temp4281; struct _tuple16 _temp4283; int _temp4284; void* _temp4286;
unsigned char _temp4288; short _temp4290; struct _tagged_string _temp4292;
struct Cyc_Core_Opt* _temp4294; struct Cyc_Core_Opt* _temp4296; struct Cyc_Core_Opt
_temp4298; struct _tagged_string* _temp4299; struct _tuple1* _temp4301; struct
_tuple1 _temp4303; struct _tagged_string* _temp4304; void* _temp4306; _LL4263:
if( _temp4261 == Cyc_Okay_tok){ goto _LL4264;} else{ goto _LL4265;} _LL4265: if(*((
void**) _temp4261) == Cyc_Int_tok){ _LL4282: _temp4281=(( struct Cyc_Int_tok_struct*)
_temp4261)->f1; _temp4283=* _temp4281; _LL4287: _temp4286= _temp4283.f1; goto
_LL4285; _LL4285: _temp4284= _temp4283.f2; goto _LL4266;} else{ goto _LL4267;}
_LL4267: if(*(( void**) _temp4261) == Cyc_Char_tok){ _LL4289: _temp4288=((
struct Cyc_Char_tok_struct*) _temp4261)->f1; goto _LL4268;} else{ goto _LL4269;}
_LL4269: if(*(( void**) _temp4261) == Cyc_Short_tok){ _LL4291: _temp4290=((
struct Cyc_Short_tok_struct*) _temp4261)->f1; goto _LL4270;} else{ goto _LL4271;}
_LL4271: if(*(( void**) _temp4261) == Cyc_String_tok){ _LL4293: _temp4292=((
struct Cyc_String_tok_struct*) _temp4261)->f1; goto _LL4272;} else{ goto _LL4273;}
_LL4273: if(*(( void**) _temp4261) == Cyc_Stringopt_tok){ _LL4295: _temp4294=((
struct Cyc_Stringopt_tok_struct*) _temp4261)->f1; if( _temp4294 == 0){ goto
_LL4274;} else{ goto _LL4275;}} else{ goto _LL4275;} _LL4275: if(*(( void**)
_temp4261) == Cyc_Stringopt_tok){ _LL4297: _temp4296=(( struct Cyc_Stringopt_tok_struct*)
_temp4261)->f1; if( _temp4296 == 0){ goto _LL4277;} else{ _temp4298=* _temp4296;
_LL4300: _temp4299=( struct _tagged_string*) _temp4298.v; goto _LL4276;}} else{
goto _LL4277;} _LL4277: if(*(( void**) _temp4261) == Cyc_QualId_tok){ _LL4302:
_temp4301=(( struct Cyc_QualId_tok_struct*) _temp4261)->f1; _temp4303=*
_temp4301; _LL4307: _temp4306= _temp4303.f1; goto _LL4305; _LL4305: _temp4304=
_temp4303.f2; goto _LL4278;} else{ goto _LL4279;} _LL4279: goto _LL4280; _LL4264:
fprintf( sfile_to_file( Cyc_Stdio_stderr),"ok"); goto _LL4262; _LL4266: fprintf(
sfile_to_file( Cyc_Stdio_stderr),"%d", _temp4284); goto _LL4262; _LL4268:
fprintf( sfile_to_file( Cyc_Stdio_stderr),"%c",( int) _temp4288); goto _LL4262;
_LL4270: fprintf( sfile_to_file( Cyc_Stdio_stderr),"%ds",( int) _temp4290); goto
_LL4262; _LL4272:({ struct _tagged_string _temp4308= _temp4292; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _temp4308.last_plus_one - _temp4308.curr,
_temp4308.curr);}); goto _LL4262; _LL4274: fprintf( sfile_to_file( Cyc_Stdio_stderr),"null");
goto _LL4262; _LL4276:({ struct _tagged_string _temp4309=* _temp4299; fprintf(
sfile_to_file( Cyc_Stdio_stderr),"\"%.*s\"", _temp4309.last_plus_one - _temp4309.curr,
_temp4309.curr);}); goto _LL4262; _LL4278: { struct Cyc_List_List* _temp4310= 0;
goto _LL4311; _LL4311:{ void* _temp4312= _temp4306; struct Cyc_List_List*
_temp4320; struct Cyc_List_List* _temp4322; _LL4314: if(( unsigned int)
_temp4312 > 1u?*(( int*) _temp4312) == Cyc_Absyn_Rel_n: 0){ _LL4321: _temp4320=((
struct Cyc_Absyn_Rel_n_struct*) _temp4312)->f1; goto _LL4315;} else{ goto
_LL4316;} _LL4316: if(( unsigned int) _temp4312 > 1u?*(( int*) _temp4312) == Cyc_Absyn_Abs_n:
0){ _LL4323: _temp4322=(( struct Cyc_Absyn_Abs_n_struct*) _temp4312)->f1; goto
_LL4317;} else{ goto _LL4318;} _LL4318: if( _temp4312 ==( void*) Cyc_Absyn_Loc_n){
goto _LL4319;} else{ goto _LL4313;} _LL4315: _temp4310= _temp4320; goto _LL4313;
_LL4317: _temp4310= _temp4322; goto _LL4313; _LL4319: goto _LL4313; _LL4313:;}
for( 0; _temp4310 != 0; _temp4310=(( struct Cyc_List_List*) _check_null(
_temp4310))->tl){({ struct _tagged_string _temp4324=*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( _temp4310))->hd); fprintf( sfile_to_file(
Cyc_Stdio_stderr),"%.*s::", _temp4324.last_plus_one - _temp4324.curr, _temp4324.curr);});}({
struct _tagged_string _temp4325=* _temp4304; fprintf( sfile_to_file( Cyc_Stdio_stderr),"%.*s::",
_temp4325.last_plus_one - _temp4325.curr, _temp4325.curr);}); goto _LL4262;}
_LL4280: fprintf( sfile_to_file( Cyc_Stdio_stderr),"?"); goto _LL4262; _LL4262:;}
struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result=
0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4326=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4326->v=( void*) Cyc_Lexing_from_file(
f); _temp4326;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}