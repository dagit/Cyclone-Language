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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth[ 8u];
extern char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{ char* tag;
struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)(
struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_string lex_buffer;
int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int lex_curr_pos; int
lex_last_pos; int lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf*
Cyc_Lexing_Lexbuf; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_string, int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct _tagged_string Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*);
extern char Cyc_Lexing_lexeme_char( struct Cyc_Lexing_lexbuf*, int); extern int
Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern void
Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set* s); extern char Cyc_Set_Absent[
11u]; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern
int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
extern struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*,
struct _tagged_string); extern struct _tagged_string Cyc_String_zstrncpy( struct
_tagged_string, int, struct _tagged_string, int, unsigned int); extern struct
_tagged_string Cyc_String_substring( struct _tagged_string, int ofs,
unsigned int n); struct _tagged_ptr1{ void** curr; void** base; void**
last_plus_one; } ; struct Cyc_Xarray_Xarray{ struct _tagged_ptr1 elmts; int
num_elmts; } ; typedef struct Cyc_Xarray_Xarray* Cyc_Xarray_xarray_t; extern
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create( int, void*); extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*,
void*); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs( int, int); static const int Cyc_Position_Lex= 0;
static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*,
struct _tagged_string); extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
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
f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_varlist_cmp( struct
Cyc_List_List*, struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Parse_lbuf;
typedef void* Cyc_struct_or_union_t; typedef void* Cyc_blockitem_t; typedef void*
Cyc_type_specifier_t; typedef void* Cyc_storage_class_t; struct Cyc_Declaration_spec;
typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t; struct Cyc_Declarator;
typedef struct Cyc_Declarator* Cyc_declarator_t; struct Cyc_Abstractdeclarator;
typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t; extern char Cyc_AbstractDeclarator_tok[
27u]; struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern char Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ; extern char Cyc_BlockItem_tok[
18u]; struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Bool_tok[
13u]; struct Cyc_Bool_tok_struct{ char* tag; int f1; } ; extern char Cyc_Char_tok[
13u]; struct Cyc_Char_tok_struct{ char* tag; char f1; } ; extern char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{ char* tag;
struct Cyc_Declaration_spec* f1; } ; extern char Cyc_DeclaratorExpoptList_tok[
29u]; struct Cyc_DeclaratorExpoptList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_DeclaratorExpopt_tok[ 25u]; struct _tuple3{ struct Cyc_Declarator*
f1; struct Cyc_Core_Opt* f2; } ; struct Cyc_DeclaratorExpopt_tok_struct{ char*
tag; struct _tuple3* f1; } ; extern char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
char* tag; struct Cyc_Declarator* f1; } ; extern char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_EnumfieldList_tok[ 22u]; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Enumfield_tok[ 18u];
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
extern char Cyc_ExpList_tok[ 16u]; struct Cyc_ExpList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{ char* tag;
struct _tuple4* f1; } ; extern char Cyc_FnDecl_tok[ 15u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_IdList_tok[ 15u];
struct Cyc_IdList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_InitDeclList_tok[ 21u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_InitDecl_tok[ 17u]; struct _tuple5{
struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{
char* tag; struct _tuple5* f1; } ; extern char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Int_tok[ 12u]; struct _tuple6{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Okay_tok[ 13u]; extern char Cyc_ParamDeclListBool_tok[
26u]; struct _tuple7{ struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo*
f3; struct Cyc_Core_Opt* f4; } ; struct Cyc_ParamDeclListBool_tok_struct{ char*
tag; struct _tuple7* f1; } ; extern char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDecl_tok[ 18u];
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple1* f1; } ; extern char
Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok[ 16u]; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; extern char Cyc_Pointer_Sort_tok[ 21u];
struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Primop_tok[
15u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Primopopt_tok[
18u]; struct Cyc_Primopopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ;
extern char Cyc_QualId_tok[ 15u]; struct Cyc_QualId_tok_struct{ char* tag;
struct _tuple0* f1; } ; extern char Cyc_QualSpecList_tok[ 21u]; struct _tuple8{
struct Cyc_Absyn_Tqual f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; struct Cyc_QualSpecList_tok_struct{ char* tag; struct _tuple8* f1; } ; extern
char Cyc_Rgn_tok[ 12u]; struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{ char* tag; void*
f1; } ; extern char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ char* tag;
short f1; } ; extern char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ char*
tag; struct Cyc_Absyn_Stmt* f1; } ; extern char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_String_tok[
15u]; struct Cyc_String_tok_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stringopt_tok[ 18u]; struct Cyc_Stringopt_tok_struct{ char* tag;
struct Cyc_Core_Opt* f1; } ; extern char Cyc_StructFieldDeclListList_tok[ 32u];
struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclList_tok[ 28u]; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_StructOrUnion_tok[ 22u];
struct Cyc_StructOrUnion_tok_struct{ char* tag; void* f1; } ; extern char Cyc_SwitchClauseList_tok[
25u]; struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_TunionFieldList_tok[ 24u]; struct Cyc_TunionFieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_TunionField_tok[ 20u];
struct Cyc_TunionField_tok_struct{ char* tag; struct Cyc_Absyn_Tunionfield* f1;
} ; extern char Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{ char* tag;
struct Cyc_Core_Opt* f1; } ; extern char Cyc_TypeQual_tok[ 17u]; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual f1; } ; extern char Cyc_TypeSpecifier_tok[ 22u];
struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Type_tok[
13u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; extern
struct Cyc_Yyltype Cyc_yylloc; extern void* Cyc_yylval; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[
12u]; extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); void Cyc_yyerror( struct _tagged_string s){
Cyc_Position_post_error( Cyc_Position_mk_err_parse( Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line, Cyc_yylloc.last_line), s));} struct Cyc_Lex_Trie; typedef
struct Cyc_Lex_Trie* Cyc_Lex_trie_t; typedef struct Cyc_Lex_Trie** Cyc_Lex_trie_child_v_t;
typedef struct Cyc_Lex_Trie*** Cyc_Lex_trie_child_t; struct Cyc_Lex_Trie{ struct
Cyc_Lex_Trie*** children; int shared_str; } ; static int Cyc_Lex_num_kws= 0;
static struct _tagged_ptr0 Cyc_Lex_kw_nums={ 0, 0, 0}; static struct Cyc_Xarray_Xarray*
Cyc_Lex_symbols= 0; static struct Cyc_Lex_Trie* Cyc_Lex_ids_trie= 0; static
struct Cyc_Lex_Trie* Cyc_Lex_typedefs_trie= 0; static int Cyc_Lex_comment_depth=
0; static struct _tuple6 Cyc_Lex_token_int_pair={( void*) 0u, 0}; static char
_temp2[ 8u]="*bogus*"; static struct _tagged_string Cyc_Lex_bogus_string={
_temp2, _temp2, _temp2 + 8u}; static struct Cyc_Absyn_Abs_n_struct Cyc_Lex_absn_null={
1u, 0}; static struct _tuple0 Cyc_Lex_token_id_pair={( void*)& Cyc_Lex_absn_null,&
Cyc_Lex_bogus_string}; static char Cyc_Lex_token_char='\000'; static char _temp6[
1u]=""; static struct _tagged_string Cyc_Lex_token_string={ _temp6, _temp6,
_temp6 + 1u}; static struct _tuple6* Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;
static struct _tuple0* Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair; static int
Cyc_Lex_runaway_start= 0; static void Cyc_Lex_err( struct _tagged_string msg,
struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s= Cyc_Position_segment_of_abs(((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lb),(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lb)); Cyc_Position_post_error(
Cyc_Position_mk_err_lex( s, msg));} static void Cyc_Lex_runaway_err( struct
_tagged_string msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment*
s= Cyc_Position_segment_of_abs( Cyc_Lex_runaway_start,(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)( lb)); Cyc_Position_post_error( Cyc_Position_mk_err_lex(
s, msg));} struct _tuple9{ struct _tagged_string f1; short f2; } ; static char
_temp9[ 14u]="__attribute__"; static char _temp12[ 9u]="abstract"; static char
_temp15[ 5u]="auto"; static char _temp18[ 6u]="break"; static char _temp21[ 5u]="case";
static char _temp24[ 6u]="catch"; static char _temp27[ 5u]="char"; static char
_temp30[ 8u]="codegen"; static char _temp33[ 6u]="const"; static char _temp36[ 9u]="continue";
static char _temp39[ 4u]="cut"; static char _temp42[ 8u]="default"; static char
_temp45[ 3u]="do"; static char _temp48[ 7u]="double"; static char _temp51[ 5u]="else";
static char _temp54[ 5u]="enum"; static char _temp57[ 7u]="extern"; static char
_temp60[ 9u]="fallthru"; static char _temp63[ 5u]="fill"; static char _temp66[ 6u]="float";
static char _temp69[ 4u]="for"; static char _temp72[ 8u]="fprintf"; static char
_temp75[ 7u]="fscanf"; static char _temp78[ 5u]="goto"; static char _temp81[ 3u]="if";
static char _temp84[ 7u]="inline"; static char _temp87[ 4u]="int"; static char
_temp90[ 4u]="let"; static char _temp93[ 5u]="long"; static char _temp96[ 7u]="malloc";
static char _temp99[ 10u]="namespace"; static char _temp102[ 4u]="new"; static
char _temp105[ 5u]="null"; static char _temp108[ 7u]="printf"; static char
_temp111[ 9u]="region_t"; static char _temp114[ 7u]="region"; static char
_temp117[ 9u]="register"; static char _temp120[ 9u]="restrict"; static char
_temp123[ 7u]="return"; static char _temp126[ 8u]="rmalloc"; static char
_temp129[ 5u]="rnew"; static char _temp132[ 6u]="scanf"; static char _temp135[ 6u]="short";
static char _temp138[ 7u]="signed"; static char _temp141[ 7u]="sizeof"; static
char _temp144[ 7u]="splice"; static char _temp147[ 7u]="sscanf"; static char
_temp150[ 7u]="static"; static char _temp153[ 7u]="struct"; static char _temp156[
7u]="switch"; static char _temp159[ 6u]="throw"; static char _temp162[ 4u]="try";
static char _temp165[ 7u]="tunion"; static char _temp168[ 8u]="typedef"; static
char _temp171[ 6u]="union"; static char _temp174[ 9u]="unsigned"; static char
_temp177[ 6u]="using"; static char _temp180[ 5u]="void"; static char _temp183[ 9u]="volatile";
static char _temp186[ 6u]="while"; static char _temp189[ 8u]="xtunion"; static
char _temp192[ 8u]="xprintf"; static struct _tuple9 Cyc_Lex_rw_array[ 62u]={{{
_temp9, _temp9, _temp9 + 14u}, (short)351},{{ _temp12, _temp12, _temp12 + 9u},
(short)298},{{ _temp15, _temp15, _temp15 + 5u}, (short)258},{{ _temp18, _temp18,
_temp18 + 6u}, (short)288},{{ _temp21, _temp21, _temp21 + 5u}, (short)277},{{
_temp24, _temp24, _temp24 + 6u}, (short)296},{{ _temp27, _temp27, _temp27 + 5u},
(short)264},{{ _temp30, _temp30, _temp30 + 8u}, (short)305},{{ _temp33, _temp33,
_temp33 + 6u}, (short)272},{{ _temp36, _temp36, _temp36 + 9u}, (short)287},{{
_temp39, _temp39, _temp39 + 4u}, (short)306},{{ _temp42, _temp42, _temp42 + 8u},
(short)278},{{ _temp45, _temp45, _temp45 + 3u}, (short)284},{{ _temp48, _temp48,
_temp48 + 7u}, (short)269},{{ _temp51, _temp51, _temp51 + 5u}, (short)281},{{
_temp54, _temp54, _temp54 + 5u}, (short)291},{{ _temp57, _temp57, _temp57 + 7u},
(short)261},{{ _temp60, _temp60, _temp60 + 9u}, (short)299},{{ _temp63, _temp63,
_temp63 + 5u}, (short)304},{{ _temp66, _temp66, _temp66 + 6u}, (short)268},{{
_temp69, _temp69, _temp69 + 4u}, (short)285},{{ _temp72, _temp72, _temp72 + 8u},
(short)309},{{ _temp75, _temp75, _temp75 + 7u}, (short)312},{{ _temp78, _temp78,
_temp78 + 5u}, (short)286},{{ _temp81, _temp81, _temp81 + 3u}, (short)280},{{
_temp84, _temp84, _temp84 + 7u}, (short)279},{{ _temp87, _temp87, _temp87 + 4u},
(short)266},{{ _temp90, _temp90, _temp90 + 4u}, (short)293},{{ _temp93, _temp93,
_temp93 + 5u}, (short)267},{{ _temp96, _temp96, _temp96 + 7u}, (short)314},{{
_temp99, _temp99, _temp99 + 10u}, (short)301},{{ _temp102, _temp102, _temp102 +
4u}, (short)297},{{ _temp105, _temp105, _temp105 + 5u}, (short)292},{{ _temp108,
_temp108, _temp108 + 7u}, (short)308},{{ _temp111, _temp111, _temp111 + 9u},
(short)315},{{ _temp114, _temp114, _temp114 + 7u}, (short)316},{{ _temp117,
_temp117, _temp117 + 9u}, (short)259},{{ _temp120, _temp120, _temp120 + 9u},
(short)274},{{ _temp123, _temp123, _temp123 + 7u}, (short)289},{{ _temp126,
_temp126, _temp126 + 8u}, (short)318},{{ _temp129, _temp129, _temp129 + 5u},
(short)317},{{ _temp132, _temp132, _temp132 + 6u}, (short)311},{{ _temp135,
_temp135, _temp135 + 6u}, (short)265},{{ _temp138, _temp138, _temp138 + 7u},
(short)270},{{ _temp141, _temp141, _temp141 + 7u}, (short)290},{{ _temp144,
_temp144, _temp144 + 7u}, (short)307},{{ _temp147, _temp147, _temp147 + 7u},
(short)313},{{ _temp150, _temp150, _temp150 + 7u}, (short)260},{{ _temp153,
_temp153, _temp153 + 7u}, (short)275},{{ _temp156, _temp156, _temp156 + 7u},
(short)282},{{ _temp159, _temp159, _temp159 + 6u}, (short)294},{{ _temp162,
_temp162, _temp162 + 4u}, (short)295},{{ _temp165, _temp165, _temp165 + 7u},
(short)302},{{ _temp168, _temp168, _temp168 + 8u}, (short)262},{{ _temp171,
_temp171, _temp171 + 6u}, (short)276},{{ _temp174, _temp174, _temp174 + 9u},
(short)271},{{ _temp177, _temp177, _temp177 + 6u}, (short)300},{{ _temp180,
_temp180, _temp180 + 5u}, (short)263},{{ _temp183, _temp183, _temp183 + 9u},
(short)273},{{ _temp186, _temp186, _temp186 + 6u}, (short)283},{{ _temp189,
_temp189, _temp189 + 8u}, (short)303},{{ _temp192, _temp192, _temp192 + 8u},
(short)310}}; static int Cyc_Lex_str_index( struct _tagged_string buff, int
offset, int len){ int i= offset; int last=( offset + len) - 1; struct Cyc_Lex_Trie*
t= Cyc_Lex_ids_trie; while( i <= last) { int ch=( int)*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( buff,
sizeof( char), i) - 48; if((( struct Cyc_Lex_Trie*) _check_null( t))->children
== 0){ while( i <= last) { struct Cyc_Lex_Trie** _temp196=( struct Cyc_Lex_Trie**)({
unsigned int _temp193= 75u; struct Cyc_Lex_Trie** _temp194=( struct Cyc_Lex_Trie**)
GC_malloc( sizeof( struct Cyc_Lex_Trie*) * _temp193);{ unsigned int _temp195=
_temp193; unsigned int q; for( q= 0; q < _temp195; q ++){ _temp194[ q]= 0;}};
_temp194;}); goto _LL197; _LL197: { struct Cyc_Lex_Trie*** _temp199=({ struct
Cyc_Lex_Trie*** _temp198=( struct Cyc_Lex_Trie***) GC_malloc( sizeof( struct Cyc_Lex_Trie**)
* 1); _temp198[ 0]= _temp196; _temp198;}); goto _LL200; _LL200:(( struct Cyc_Lex_Trie*)
_check_null( t))->children=( struct Cyc_Lex_Trie***) _temp199;(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]=({ struct Cyc_Lex_Trie* _temp201=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp201->children=
0; _temp201->shared_str= - 1; _temp201;}); t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]; ++ i; ch=( int)*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( buff,
sizeof( char), i) - 48;}}{ struct _tagged_string _temp202= Cyc_Core_new_string(
len + 1); goto _LL203; _LL203: Cyc_String_zstrncpy( _temp202, 0, buff, offset,(
unsigned int) len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*, struct
_tagged_string*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*) _check_null(
Cyc_Lex_symbols),({ struct _tagged_string* _temp204=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp204[ 0]= _temp202; _temp204;}));((
struct Cyc_Lex_Trie*) _check_null( t))->shared_str= ans; return ans;}}} if((*((
struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)] == 0){(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]=({ struct Cyc_Lex_Trie* _temp205=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp205->children=
0; _temp205->shared_str= - 1; _temp205;});} t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]; ++ i;} if((( struct Cyc_Lex_Trie*)
_check_null( t))->shared_str == - 1){ struct _tagged_string _temp206= Cyc_Core_new_string(
len + 1); goto _LL207; _LL207: Cyc_String_zstrncpy( _temp206, 0, buff, offset,(
unsigned int) len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*, struct
_tagged_string*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*) _check_null(
Cyc_Lex_symbols),({ struct _tagged_string* _temp208=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp208[ 0]= _temp206; _temp208;}));((
struct Cyc_Lex_Trie*) _check_null( t))->shared_str= ans; return ans;}} return((
struct Cyc_Lex_Trie*) _check_null( t))->shared_str;} static int Cyc_Lex_str_index_lbuf(
struct Cyc_Lexing_lexbuf* lbuf){ return Cyc_Lex_str_index( lbuf->lex_buffer,
lbuf->lex_start_pos, lbuf->lex_curr_pos - lbuf->lex_start_pos);} struct _tuple10{
struct Cyc_Lex_Trie** f1; } ; static void Cyc_Lex_insert_typedef( struct
_tagged_string* sptr){ struct _tagged_string s=* sptr; int len=( int)(({ struct
_tagged_string _temp215= s;( unsigned int)( _temp215.last_plus_one - _temp215.curr);})
- 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len;
++ i){ int ch=( int)*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( char), i) - 48; if(((
struct Cyc_Lex_Trie*) _check_null( t))->children == 0){ while( i < len) {((
struct Cyc_Lex_Trie*) _check_null( t))->children=( struct Cyc_Lex_Trie***)({
struct _tuple10* _temp209=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp209->f1=({ unsigned int _temp210= 75u; struct Cyc_Lex_Trie** _temp211=(
struct Cyc_Lex_Trie**) GC_malloc( sizeof( struct Cyc_Lex_Trie*) * _temp210);{
unsigned int _temp212= _temp210; unsigned int i; for( i= 0; i < _temp212; i ++){
_temp211[ i]= 0;}}; _temp211;}); _temp209;});(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]=({ struct Cyc_Lex_Trie* _temp213=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp213->children=
0; _temp213->shared_str= 0; _temp213;}); t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]; ++ i; ch=( int)*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i) - 48;}(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str= 1;
return;} if((*(( struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*)
_check_null( t))->children)))[ _check_known_subscript_notnull( 75u, ch)] == 0){(*((
struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)]=({ struct Cyc_Lex_Trie* _temp214=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp214->children=
0; _temp214->shared_str= 0; _temp214;});} t=(*(( struct Cyc_Lex_Trie***)
_check_null((( struct Cyc_Lex_Trie*) _check_null( t))->children)))[
_check_known_subscript_notnull( 75u, ch)];}}(( struct Cyc_Lex_Trie*) _check_null(
t))->shared_str= 1; return;} static struct _tagged_string* Cyc_Lex_get_symbol(
int symbol_num){ return(( struct _tagged_string*(*)( struct Cyc_Xarray_Xarray*,
int)) Cyc_Xarray_get)(( struct Cyc_Xarray_Xarray*) _check_null( Cyc_Lex_symbols),
symbol_num);} static int Cyc_Lex_int_of_char( char c){ if('0' <= c? c <='9': 0){
return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';} else{ if('A'
<= c? c <='F': 0){ return( 10 + c) -'A';} else{( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp216=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp216[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp217; _temp217.tag= Cyc_Core_InvalidArg; _temp217.f1=({
char* _temp218="string to integer conversion"; struct _tagged_string _temp219;
_temp219.curr= _temp218; _temp219.base= _temp218; _temp219.last_plus_one=
_temp218 + 29; _temp219;}); _temp217;}); _temp216;}));}}}} struct _tuple6* Cyc_Lex_intconst(
struct Cyc_Lexing_lexbuf* lbuf, int start, int base){ unsigned int n= 0; int end=
lbuf->lex_curr_pos; struct _tagged_string buff= lbuf->lex_buffer; int i= start +
lbuf->lex_start_pos;{ int i= start + lbuf->lex_start_pos; for( 0; i < end; ++ i){
char c=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( buff, sizeof( char), i); switch( c){ case 'u': _LL220:
goto _LL221; case 'U': _LL221: return({ struct _tuple6* _temp223=( struct
_tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp223->f1=( void*) Cyc_Absyn_Unsigned;
_temp223->f2=( int) n; _temp223;}); case 'l': _LL222: break; case 'L': _LL224:
break; default: _LL225: n= n * base +( unsigned int) Cyc_Lex_int_of_char( c);
break;}}} return({ struct _tuple6* _temp227=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp227->f1=( void*) Cyc_Absyn_Signed; _temp227->f2=(
int) n; _temp227;});} static char Cyc_Lex_char_for_octal_code( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany){ int c= 0;{ int i= 0; for( 0; i < howmany; i ++){ c=(
8 * c +(( char(*)( struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)( lb,
start + i)) - 48;}} return( char) c;} static char Cyc_Lex_char_for_hex_code(
struct _tagged_string s, int start){ int c= 0; int len=( int) Cyc_String_strlen(
s);{ int i= 0; for( 0; start + i < len; i ++){ char x=*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), start + i); if('0' <= x? x <='9': 0){ c=( 16 * c +( int) x) -(
int)'0';} else{ if('A' <= x? x <='F': 0){ c=( 16 * c +( int) x) -( int)'A';}
else{ if('a' <= x? x <='f': 0){ c=( 16 * c +( int) x) -( int)'a';} else{ break;}}}}}
return( char) c;} static char _temp230[ 11u]="xxxxxxxxxx"; struct _tagged_string
Cyc_Lex_string_buffer={ _temp230, _temp230, _temp230 + 11u}; int Cyc_Lex_string_pos=
0; void Cyc_Lex_store_string_char( char c){ int sz=( int)({ struct
_tagged_string _temp235= Cyc_Lex_string_buffer;( unsigned int)( _temp235.last_plus_one
- _temp235.curr);}); if( Cyc_Lex_string_pos >= sz){ int newsz= sz; while( Cyc_Lex_string_pos
>= newsz) { newsz= newsz * 2;}{ struct _tagged_string str=({ unsigned int
_temp231=( unsigned int) newsz; char* _temp232=( char*) GC_malloc_atomic(
sizeof( char) * _temp231); struct _tagged_string _temp234={ _temp232, _temp232,
_temp232 + _temp231};{ unsigned int _temp233= _temp231; unsigned int i; for( i=
0; i < _temp233; i ++){ _temp232[ i]= i < sz?*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( Cyc_Lex_string_buffer,
sizeof( char),( int) i):'\000';}}; _temp234;}); Cyc_Lex_string_buffer= str;}}*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Lex_string_buffer, sizeof( char), Cyc_Lex_string_pos)=
c; ++ Cyc_Lex_string_pos;} struct _tagged_string Cyc_Lex_get_stored_string(){
struct _tagged_string str= Cyc_String_substring( Cyc_Lex_string_buffer, 0,(
unsigned int) Cyc_Lex_string_pos); Cyc_Lex_string_pos= 0; return str;} struct
Cyc_Lex_Ldecls{ struct Cyc_Set_Set* typedefs; struct Cyc_Set_Set* namespaces; }
; typedef struct Cyc_Lex_Ldecls* Cyc_Lex_ldecls_t; struct Cyc_Lex_Lvis{ struct
Cyc_List_List* current_namespace; struct Cyc_List_List* imported_namespaces; } ;
typedef struct Cyc_Lex_Lvis* Cyc_Lex_lvis; struct Cyc_Lex_Lstate{ struct Cyc_List_List*
lstack; struct Cyc_Dict_Dict* decls; } ; typedef struct Cyc_Lex_Lstate* Cyc_Lex_lstate_t;
static struct Cyc_Core_Opt* Cyc_Lex_lstate= 0; static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis* _temp237=({ struct Cyc_Lex_Lvis* _temp236=( struct Cyc_Lex_Lvis*)
GC_malloc( sizeof( struct Cyc_Lex_Lvis)); _temp236->current_namespace= 0;
_temp236->imported_namespaces= 0; _temp236;}); goto _LL238; _LL238: { struct Cyc_List_List*
_temp240=({ struct Cyc_List_List* _temp239=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp239->hd=( void*) _temp237; _temp239->tl= 0;
_temp239;}); goto _LL241; _LL241: { struct Cyc_Dict_Dict* _temp243=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), 0,({ struct
Cyc_Lex_Ldecls* _temp242=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp242->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp242->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp242;})); goto
_LL244; _LL244: Cyc_Lex_lstate=({ struct Cyc_Core_Opt* _temp245=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp245->v=( void*)({ struct Cyc_Lex_Lstate*
_temp246=( struct Cyc_Lex_Lstate*) GC_malloc( sizeof( struct Cyc_Lex_Lstate));
_temp246->lstack= _temp240; _temp246->decls= _temp243; _temp246;}); _temp245;});}}}
static struct Cyc_List_List* Cyc_Lex_get_absolute_namespace( struct Cyc_List_List*
ns){ struct _tagged_string* n=( struct _tagged_string*) ns->hd;{ struct Cyc_List_List*
ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 0; ls=(( struct Cyc_List_List*)
_check_null( ls))->tl){ struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*)(( struct
Cyc_List_List*) _check_null( ls))->hd; struct Cyc_List_List* x=({ struct Cyc_List_List*
_temp247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp247->hd=( void*) lv->current_namespace; _temp247->tl= lv->imported_namespaces;
_temp247;}); for( 0; x != 0; x=(( struct Cyc_List_List*) _check_null( x))->tl){
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,( struct Cyc_List_List*)((
struct Cyc_List_List*) _check_null( x))->hd); if((( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( ld->namespaces, n)){ return((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd,( struct
Cyc_List_List*) ns);}}}} Cyc_yyerror(({ struct _tagged_string _temp250= Cyc_String_str_sepstr((
struct Cyc_List_List*) ns,({ char* _temp248="::"; struct _tagged_string _temp249;
_temp249.curr= _temp248; _temp249.base= _temp248; _temp249.last_plus_one=
_temp248 + 3; _temp249;})); xprintf("undeclared namespace %.*s", _temp250.last_plus_one
- _temp250.curr, _temp250.curr);})); return 0;} static void Cyc_Lex_recompute_typedefs(){
Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp251=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp251->children= 0; _temp251->shared_str=
0; _temp251;});{ struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack; for( 0; ls != 0;
ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ struct Cyc_Lex_Lvis* lv=(
struct Cyc_Lex_Lvis*)(( struct Cyc_List_List*) _check_null( ls))->hd; struct Cyc_List_List*
x=({ struct Cyc_List_List* _temp252=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp252->hd=( void*) lv->current_namespace; _temp252->tl=
lv->imported_namespaces; _temp252;}); for( 0; x != 0; x=(( struct Cyc_List_List*)
_check_null( x))->tl){ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct
Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)(( struct Cyc_List_List*) _check_null( x))->hd);(( void(*)(
void(* f)( struct _tagged_string*), struct Cyc_Set_Set* s)) Cyc_Set_iter)( Cyc_Lex_insert_typedef,
ld->typedefs);}}}} static int Cyc_Lex_is_typedef_in_namespace( struct Cyc_List_List*
ns, struct _tagged_string* v){ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace(
ns); struct _handler_cons _temp253; _push_handler(& _temp253);{ int _temp255= 0;
if( setjmp( _temp253.handler)){ _temp255= 1;} if( ! _temp255){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls, ans); int _temp256=(( int(*)( struct
Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ld->typedefs, v);
_npop_handler( 0u); return _temp256;}; _pop_handler();} else{ void* _temp254=(
void*) _exn_thrown; void* _temp258= _temp254; _LL260: if( _temp258 == Cyc_Dict_Absent){
goto _LL261;} else{ goto _LL262;} _LL262: goto _LL263; _LL261: return 0; _LL263:(
void) _throw( _temp258); _LL259:;}}} static int Cyc_Lex_is_typedef( struct Cyc_List_List*
ns, struct _tagged_string* v){ if( ns != 0){ return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*) _check_null( ns), v);}{ struct _tagged_string s=* v; int
len=( int)(({ struct _tagged_string _temp264= s;( unsigned int)( _temp264.last_plus_one
- _temp264.curr);}) - 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i=
0; for( 0; i < len; ++ i){ if( t == 0? 1:(( struct Cyc_Lex_Trie*) _check_null( t))->children
== 0){ return 0;} else{ t=(*(( struct Cyc_Lex_Trie***) _check_null((( struct Cyc_Lex_Trie*)
_check_null( t))->children)))[ _check_known_subscript_notnull( 75u,( int)*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( char), i) - 48)];}}} if( t == 0){ return 0;}
return(( struct Cyc_Lex_Trie*) _check_null( t))->shared_str;}} void Cyc_Lex_enter_namespace(
struct _tagged_string* s){ struct Cyc_List_List* ns=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List* new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp269=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp269->hd=( void*) s; _temp269->tl= 0; _temp269;}));(( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack=({ struct Cyc_List_List*
_temp265=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp265->hd=( void*)({ struct Cyc_Lex_Lvis* _temp266=( struct Cyc_Lex_Lvis*)
GC_malloc( sizeof( struct Cyc_Lex_Lvis)); _temp266->current_namespace= new_ns;
_temp266->imported_namespaces= 0; _temp266;}); _temp265->tl=( struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack;
_temp265;});{ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, ns); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ld->namespaces,
s)){(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls, ns,({ struct Cyc_Lex_Ldecls*
_temp267=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp267->typedefs= ld->typedefs; _temp267->namespaces=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)( ld->namespaces,
s); _temp267;}));(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Lex_lstate))->v)->decls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls,
new_ns,({ struct Cyc_Lex_Ldecls* _temp268=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp268->typedefs=(( struct Cyc_Set_Set*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)(
Cyc_String_zstrptrcmp); _temp268->namespaces=(( struct Cyc_Set_Set*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp268;}));} Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_namespace(){((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack=(
struct Cyc_List_List*) _check_null(((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->tl); Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_enter_using( struct _tuple0* q){ struct Cyc_List_List* ns;{ void*
_temp270=(* q).f1; struct Cyc_List_List* _temp278; struct Cyc_List_List*
_temp280; _LL272: if( _temp270 ==( void*) Cyc_Absyn_Loc_n){ goto _LL273;} else{
goto _LL274;} _LL274: if(( unsigned int) _temp270 > 1u?*(( int*) _temp270) ==
Cyc_Absyn_Rel_n: 0){ _LL279: _temp278=(( struct Cyc_Absyn_Rel_n_struct*)
_temp270)->f1; goto _LL275;} else{ goto _LL276;} _LL276: if(( unsigned int)
_temp270 > 1u?*(( int*) _temp270) == Cyc_Absyn_Abs_n: 0){ _LL281: _temp280=((
struct Cyc_Absyn_Abs_n_struct*) _temp270)->f1; goto _LL277;} else{ goto _LL271;}
_LL273: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp282=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp282->hd=( void*)(*
q).f2; _temp282->tl= 0; _temp282;}); goto _LL271; _LL275: _temp280= _temp278;
goto _LL277; _LL277: ns=( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp280,({
struct Cyc_List_List* _temp283=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp283->hd=( void*)(* q).f2; _temp283->tl= 0; _temp283;})));
goto _LL271; _LL271:;}{ struct Cyc_List_List* _temp284= Cyc_Lex_get_absolute_namespace(
ns); goto _LL285; _LL285: { struct Cyc_List_List* _temp286=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
goto _LL287; _LL287:(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List* _temp288=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp288->hd=( void*) _temp284; _temp288->tl= _temp286;
_temp288;}); Cyc_Lex_recompute_typedefs();}}} void Cyc_Lex_leave_using(){ struct
Cyc_List_List* _temp289=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
goto _LL290; _LL290:(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct
Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((
struct Cyc_List_List*) _check_null( _temp289))->tl; Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_register_typedef( struct _tuple0* q){ struct Cyc_List_List*
_temp291=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace; goto _LL292;
_LL292: { struct Cyc_Dict_Dict* _temp293=(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Lex_lstate))->v)->decls; goto _LL294; _LL294: { struct Cyc_Lex_Ldecls*
_temp295=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( _temp293, _temp291); goto _LL296; _LL296: { struct Cyc_Lex_Ldecls*
_temp298=({ struct Cyc_Lex_Ldecls* _temp297=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp297->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)( _temp295->typedefs,(*
q).f2); _temp297->namespaces= _temp295->namespaces; _temp297;}); goto _LL299;
_LL299:(( struct Cyc_Lex_Lstate*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)( _temp293, _temp291, _temp298);
Cyc_Lex_insert_typedef((* q).f2);}}}} static short Cyc_Lex_process_id( struct
Cyc_Lexing_lexbuf* lbuf){ int symbol_num=(( int(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_str_index_lbuf)( lbuf); if( symbol_num < Cyc_Lex_num_kws){ return(
short)*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Lex_kw_nums, sizeof( int), symbol_num);}{ struct
_tagged_string* s= Cyc_Lex_get_symbol( symbol_num); if( Cyc_Lex_is_typedef( 0, s)){
Cyc_Lex_token_qvar=({ struct _tuple0* _temp300=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp300->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp301=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp301[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp302; _temp302.tag= Cyc_Absyn_Rel_n;
_temp302.f1= 0; _temp302;}); _temp301;}); _temp300->f2= s; _temp300;}); return
(short)350;} Cyc_Lex_token_string=* s; return (short)343;}} static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf* lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos;
struct _tagged_string s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0;
while( i < end) { int start= i; for( 0; i < end?*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i) !=':': 0; i ++){;} if( start == i){( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp303=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp303[ 0]=({ struct
Cyc_Core_Impossible_struct _temp304; _temp304.tag= Cyc_Core_Impossible; _temp304.f1=({
char* _temp305="bad namespace"; struct _tagged_string _temp306; _temp306.curr=
_temp305; _temp306.base= _temp305; _temp306.last_plus_one= _temp305 + 14;
_temp306;}); _temp304;}); _temp303;}));}{ int vlen= i - start; struct
_tagged_string* v= Cyc_Lex_get_symbol( Cyc_Lex_str_index( s, start, vlen));
rev_vs=({ struct Cyc_List_List* _temp307=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp307->hd=( void*) v; _temp307->tl= rev_vs;
_temp307;}); i += 2;}} if( rev_vs == 0){( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp308=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp308[ 0]=({ struct Cyc_Core_Impossible_struct _temp309; _temp309.tag= Cyc_Core_Impossible;
_temp309.f1=({ char* _temp310="bad namespace"; struct _tagged_string _temp311;
_temp311.curr= _temp310; _temp311.base= _temp310; _temp311.last_plus_one=
_temp310 + 14; _temp311;}); _temp309;}); _temp308;}));}{ struct _tagged_string*
v=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( rev_vs))->hd;
struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)((( struct Cyc_List_List*) _check_null( rev_vs))->tl); Cyc_Lex_token_qvar=({
struct _tuple0* _temp312=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp312->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct* _temp313=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp313[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp314; _temp314.tag= Cyc_Absyn_Rel_n; _temp314.f1= vs; _temp314;}); _temp313;});
_temp312->f2= v; _temp312;}); if( Cyc_Lex_is_typedef( vs, v)){ return (short)350;}
else{ return (short)349;}}} extern int Cyc_Lex_token( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null(
Cyc_Parse_lbuf))->v); Cyc_yylloc.first_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Parse_lbuf))->v); switch( ans){ case 343: _LL315: Cyc_yylval=(
void*)({ struct Cyc_String_tok_struct* _temp317=( struct Cyc_String_tok_struct*)
GC_malloc( sizeof( struct Cyc_String_tok_struct)); _temp317[ 0]=({ struct Cyc_String_tok_struct
_temp318; _temp318.tag= Cyc_String_tok; _temp318.f1= Cyc_Lex_token_string;
_temp318;}); _temp317;}); break; case 349: _LL316: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp320=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp320[ 0]=({ struct Cyc_QualId_tok_struct
_temp321; _temp321.tag= Cyc_QualId_tok; _temp321.f1= Cyc_Lex_token_qvar;
_temp321;}); _temp320;}); break; case 350: _LL319: Cyc_yylval=( void*)({ struct
Cyc_QualId_tok_struct* _temp323=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp323[ 0]=({ struct Cyc_QualId_tok_struct
_temp324; _temp324.tag= Cyc_QualId_tok; _temp324.f1= Cyc_Lex_token_qvar;
_temp324;}); _temp323;}); break; case 348: _LL322: Cyc_yylval=( void*)({ struct
Cyc_String_tok_struct* _temp326=( struct Cyc_String_tok_struct*) GC_malloc(
sizeof( struct Cyc_String_tok_struct)); _temp326[ 0]=({ struct Cyc_String_tok_struct
_temp327; _temp327.tag= Cyc_String_tok; _temp327.f1= Cyc_Lex_token_string;
_temp327;}); _temp326;}); break; case 344: _LL325: Cyc_yylval=( void*)({ struct
Cyc_Int_tok_struct* _temp329=( struct Cyc_Int_tok_struct*) GC_malloc( sizeof(
struct Cyc_Int_tok_struct)); _temp329[ 0]=({ struct Cyc_Int_tok_struct _temp330;
_temp330.tag= Cyc_Int_tok; _temp330.f1= Cyc_Lex_token_int; _temp330;}); _temp329;});
break; case 346: _LL328: Cyc_yylval=( void*)({ struct Cyc_Char_tok_struct*
_temp332=( struct Cyc_Char_tok_struct*) GC_malloc( sizeof( struct Cyc_Char_tok_struct));
_temp332[ 0]=({ struct Cyc_Char_tok_struct _temp333; _temp333.tag= Cyc_Char_tok;
_temp333.f1= Cyc_Lex_token_char; _temp333;}); _temp332;}); break; case 347:
_LL331: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp335=( struct
Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp335[ 0]=({ struct Cyc_String_tok_struct _temp336; _temp336.tag= Cyc_String_tok;
_temp336.f1= Cyc_Lex_token_string; _temp336;}); _temp335;}); break; case 345:
_LL334: Cyc_yylval=( void*)({ struct Cyc_String_tok_struct* _temp338=( struct
Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp338[ 0]=({ struct Cyc_String_tok_struct _temp339; _temp339.tag= Cyc_String_tok;
_temp339.f1= Cyc_Lex_token_string; _temp339;}); _temp338;}); break; default:
_LL337: break;} return ans;} struct Cyc_Lexing_lex_tables* Cyc_Lex_lt= 0; int
Cyc_Lex_lbase[ 151u]={ 0, 113, 17, 83, 16, 2, - 3, - 1, - 2, - 19, - 20, 4, 118,
119, - 21, 5, - 13, - 12, 85, - 14, - 11, - 4, - 5, - 6, - 7, - 8, - 9, - 10,
209, 305, 111, - 15, 166, - 20, - 57, 8, 30, - 39, 12, 31, 116, 175, 32, 135,
141, 192, 145, 384, 427, 83, 82, 84, 87, 497, 85, 572, 647, 94, - 56, - 22, - 28,
722, 797, 93, 855, 930, 98, - 23, 1005, 96, - 26, 119, - 31, - 25, - 34, 1080,
1109, 292, 96, 107, 394, 1119, 1149, 465, 1090, 1182, 1213, 1251, 103, 114, 1321,
1359, 106, 117, 109, 119, 114, 128, - 6, - 38, 13, - 37, 6, 138, 1291, - 33, -
16, - 18, - 32, - 17, - 19, 2, 1399, 150, 158, 413, 162, 168, 169, 170, 171, 173,
180, 185, 189, 1472, 1556, - 54, - 46, - 45, - 44, - 43, - 42, - 41, - 40, - 47,
- 50, - 53, 488, - 52, 191, - 51, - 48, - 49, - 55, - 27, - 24, 14, - 35, 18,
477}; int Cyc_Lex_lbacktrk[ 151u]={ - 1, - 1, - 1, 5, 3, 4, - 1, - 1, - 1, - 1,
- 1, 18, 1, 21, - 1, 2, - 1, - 1, 16, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 17, 15, - 1, - 1, - 1, - 1, 35, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56,
56, 5, 7, 56, 56, 56, 56, 0, 56, 56, 56, 56, - 1, - 1, - 1, 3, 1, - 1, - 1, 2, -
1, - 1, 0, 29, - 1, 28, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9,
5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, - 1, 36, - 1, 9, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 34, 35}; int Cyc_Lex_ldefault[
151u]={ 34, 9, 3, 3, - 1, - 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1, - 1, 0, 147, - 1, -
1, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 100, 0, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 0, - 1, 0, 0, 0, 0, 0, 0, 147, 0, - 1, - 1}; int
Cyc_Lex_ltrans[ 1813u]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 35, 35, 35, 33, 6,
101, 150, 150, 150, 150, 150, 148, 101, 148, 149, 102, 149, 148, 0, 0, 0, 35, 36,
37, 38, 0, 39, 40, 41, 150, 144, 42, 43, 7, 44, 45, 46, 47, 48, 48, 48, 48, 48,
48, 48, 48, 48, 49, 4, 50, 51, 52, 8, 5, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 17, 14, 110, 54,
55, 56, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 10, 57, - 1, 11, 32, 32, 6, - 1, 32, 15, 30, 30,
30, 30, 30, 30, 30, 30, 74, 71, 16, 72, 20, 67, 12, 19, 69, 32, 64, 7, 16, 145,
59, 64, 70, 17, 31, 31, 31, 31, 31, 31, 31, 31, 18, 18, 18, 18, 18, 18, 18, 18,
32, 32, 146, 31, 32, 73, 25, 19, 25, 105, - 1, 106, 99, 23, 143, 23, 22, 100, 22,
24, 24, 109, 142, 32, 98, 7, 136, 107, 108, 98, 13, 33, 135, 134, 133, 132, 20,
131, 25, - 1, 25, 21, 22, 60, 130, 23, 23, 23, 22, 129, 22, 24, 24, 128, 24, 141,
98, 0, 25, 0, 26, 98, 27, 103, 28, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 0, 0, 0, 0, 0, 0, 58, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 112, - 1, - 1,
- 1, 0, 0, 6, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 80, 0, 80, 0, - 1, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 0, 0, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 14, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 75, - 1, 86, 86, 86, 86, 86, 86, 86,
86, 87, 87, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 137, 77, 0, 0, 0, 0, 0, 0,
88, 138, 138, 138, 138, 138, 138, 138, 138, 89, 0, 0, 90, 75, 0, 76, 76, 76, 76,
76, 76, 76, 76, 76, 76, 77, 150, 150, 150, 150, 150, 0, 88, 0, 0, 0, 77, 0, 0, 0,
0, 89, 0, 78, 90, 0, 0, 0, 84, 150, 84, 0, 79, 85, 85, 85, 85, 85, 85, 85, 85,
85, 85, 0, 0, 0, 0, 139, 77, 0, 0, 0, 0, 0, 0, 78, 140, 140, 140, 140, 140, 140,
140, 140, 79, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0,
62, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0,
0, 0, 0, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, 0, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 85, 85, 85, 85, 85,
85, 85, 85, 85, 85, 0, 83, 27, 0, 0, 0, 0, 75, 27, 76, 76, 76, 76, 76, 76, 76,
76, 76, 76, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 77, 0, 0, 83, 27, 0, 0,
78, 0, 0, 27, 26, 0, 0, 0, 0, 79, 26, 0, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,
0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 78, 83, 27, 0, 26, 0, 0, 0, 27, 79, 26, 0, 0, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 27, 27, 0,
0, 0, 0, 27, 27, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 77, 0, 27, 0, 0, 0, 0, 96, 27, 0, 0, 0, 0, 0, 0, 75, 97, 87, 87,
87, 87, 87, 87, 87, 87, 87, 87, 0, 0, 0, 0, 0, 77, 0, 0, 0, 0, 0, 77, 96, 0, 0,
0, 0, 0, 94, 0, 0, 97, 0, 0, 0, 0, 0, 95, 0, 0, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 94, 83, 27, 0, 0, 0, 0, 0, 27,
95, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91,
91, 91, 83, 27, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91, 91, 91,
91, 91, 91, 91, 0, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 113,
0, 92, 0, 0, 114, 0, 0, 0, 0, 0, 93, 0, 0, 115, 115, 115, 115, 115, 115, 115,
115, 0, 91, 91, 91, 91, 91, 91, 116, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 93,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 0, 0, 0, 0, 118, 119, 0, 0, 0,
120, 0, 0, 0, 0, 0, 0, 0, 121, 0, 0, 0, 122, 0, 123, 0, 124, 0, 125, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 127, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Cyc_Lex_lcheck[ 1813u]={ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 11, 15, 102, 35, 35, 35, 35, 35, 38, 100, 147, 38, 100, 147, 149, - 1,
- 1, - 1, 0, 0, 0, 0, - 1, 0, 0, 0, 35, 111, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 12, 12, 13, 3, 12,
13, 18, 18, 18, 18, 18, 18, 18, 18, 49, 50, 50, 50, 51, 54, 1, 52, 52, 12, 63,
12, 13, 40, 57, 66, 69, 13, 30, 30, 30, 30, 30, 30, 30, 30, 13, 13, 13, 13, 13,
13, 13, 13, 32, 32, 40, 43, 32, 71, 78, 13, 79, 103, 41, 44, 46, 88, 113, 89, 92,
46, 93, 94, 95, 43, 114, 32, 96, 32, 116, 44, 44, 97, 1, 46, 117, 118, 119, 120,
13, 121, 78, 41, 79, 13, 13, 57, 122, 88, 13, 89, 92, 123, 93, 94, 95, 124, 13,
140, 96, - 1, 13, - 1, 13, 97, 13, 45, 13, 45, 45, 45, 45, 45, 45, 45, 45, 45,
45, - 1, - 1, - 1, - 1, - 1, - 1, 0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 41,
38, 100, 147, - 1, - 1, 2, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, - 1, - 1, - 1, - 1, - 1, - 1,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, - 1, - 1, - 1, 77, - 1, 77, - 1, 3, 77, 77, 77, 77, 77,
77, 77, 77, 77, 77, - 1, - 1, - 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1,
- 1, - 1, - 1, - 1, - 1, 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1, - 1, - 1, - 1, - 1, - 1,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, - 1, - 1, 47, 41, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 115, 47, - 1, - 1, - 1, - 1, - 1, - 1,
47, 115, 115, 115, 115, 115, 115, 115, 115, 47, - 1, - 1, 47, 48, - 1, 48, 48,
48, 48, 48, 48, 48, 48, 48, 48, 47, 150, 150, 150, 150, 150, - 1, 47, - 1, - 1,
- 1, 48, - 1, - 1, - 1, - 1, 47, - 1, 48, 47, - 1, - 1, - 1, 83, 150, 83, - 1,
48, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, - 1, - 1, - 1, - 1, 138, 48, - 1, -
1, - 1, - 1, - 1, - 1, 48, 138, 138, 138, 138, 138, 138, 138, 138, 48, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, - 1, - 1, - 1, - 1, - 1, - 1, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, - 1, - 1, - 1, - 1, 53, - 1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, - 1, - 1, - 1, - 1, - 1, - 1, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, - 1,
- 1, - 1, - 1, 55, - 1, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, - 1, - 1, -
1, - 1, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1, - 1, - 1,
61, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1,
- 1, - 1, - 1, - 1, - 1, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, - 1, 62, - 1, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, - 1, - 1, - 1, - 1, 64, - 1, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1,
- 1, - 1, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, 65, - 1, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, - 1, - 1, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, 68, - 1, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 75, 75,
75, 75, 75, 75, 75, 75, 75, 75, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, - 1, 75,
75, - 1, - 1, - 1, - 1, 76, 75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81,
81, 81, 81, 81, 81, 81, 81, 81, - 1, 76, - 1, - 1, 75, 75, - 1, - 1, 76, - 1, -
1, 75, 81, - 1, - 1, - 1, - 1, 76, 81, - 1, 82, 82, 82, 82, 82, 82, 82, 82, 82,
82, - 1, - 1, - 1, 76, - 1, - 1, - 1, - 1, - 1, - 1, 76, 82, 82, - 1, 81, - 1, -
1, - 1, 82, 76, 81, - 1, - 1, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, 82, 85, - 1, - 1, - 1, - 1, 82, 85,
86, - 1, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 86, - 1, 85, - 1, - 1, - 1, - 1, 86, 85, - 1, - 1, - 1, -
1, - 1, - 1, 87, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, - 1, - 1, - 1, - 1,
- 1, 86, - 1, - 1, - 1, - 1, - 1, 87, 86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1,
86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, - 1, - 1, - 1, 87, - 1, - 1, - 1, - 1, - 1, - 1, 87, 104, 104, -
1, - 1, - 1, - 1, - 1, 104, 87, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 90, 90, 90, 90, 90, 90, 104, 104, - 1, - 1, - 1, - 1, -
1, 104, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, 91, 91, 91, 91, 91, 91, 91, 91,
91, - 1, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, - 1, - 1, - 1, 112, - 1,
91, - 1, - 1, 112, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, 112, 112, 112, 112,
112, 112, 112, 112, - 1, 91, 91, 91, 91, 91, 91, 112, - 1, - 1, - 1, - 1, 91, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, - 1, 112, 112, - 1, - 1, - 1,
112, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, 112, - 1, 112, - 1,
112, - 1, 112, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 126, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int Cyc_Lex_token_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp341=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp341->lex_base=({ int* _temp350=( int*) Cyc_Lex_lbase; struct _tagged_ptr0
_temp351; _temp351.curr= _temp350; _temp351.base= _temp350; _temp351.last_plus_one=
_temp350 + 151; _temp351;}); _temp341->lex_backtrk=({ int* _temp348=( int*) Cyc_Lex_lbacktrk;
struct _tagged_ptr0 _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 151; _temp349;}); _temp341->lex_default=({
int* _temp346=( int*) Cyc_Lex_ldefault; struct _tagged_ptr0 _temp347; _temp347.curr=
_temp346; _temp347.base= _temp346; _temp347.last_plus_one= _temp346 + 151;
_temp347;}); _temp341->lex_trans=({ int* _temp344=( int*) Cyc_Lex_ltrans; struct
_tagged_ptr0 _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 1813; _temp345;}); _temp341->lex_check=({ int*
_temp342=( int*) Cyc_Lex_lcheck; struct _tagged_ptr0 _temp343; _temp343.curr=
_temp342; _temp343.base= _temp342; _temp343.last_plus_one= _temp342 + 1813;
_temp343;}); _temp341;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*)
_check_null( Cyc_Lex_lt), lexstate, lexbuf); switch( lexstate){ case 0: _LL352:
return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_id)(
lexbuf); case 1: _LL353: return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf))
Cyc_Lex_process_id)( lexbuf); case 2: _LL354: return( int)(( short(*)( struct
Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_qual_id)( lexbuf); case 3: _LL355: Cyc_Lex_token_string=*
Cyc_Lex_get_symbol((( int(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_str_index_lbuf)(
lexbuf)); return 348; case 4: _LL356: Cyc_Lex_token_int=(( struct _tuple6*(*)(
struct Cyc_Lexing_lexbuf* lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf,
2, 16); return 344; case 5: _LL357: Cyc_Lex_token_int=(( struct _tuple6*(*)(
struct Cyc_Lexing_lexbuf* lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf,
0, 8); return 344; case 6: _LL358: Cyc_Lex_token_int=(( struct _tuple6*(*)(
struct Cyc_Lexing_lexbuf* lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf,
0, 10); return 344; case 7: _LL359: Cyc_Lex_token_int=(( struct _tuple6*(*)(
struct Cyc_Lexing_lexbuf* lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf,
0, 10); return 344; case 8: _LL360: Cyc_Lex_token_string=(( struct
_tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf);
return 347; case 9: _LL361: Cyc_Lex_token_string=(( struct _tagged_string(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf); return 347; case 10:
_LL362: return 326; case 11: _LL363: return 327; case 12: _LL364: return 324;
case 13: _LL365: return 325; case 14: _LL366: return 320; case 15: _LL367:
return 321; case 16: _LL368: return 333; case 17: _LL369: return 334; case 18:
_LL370: return 330; case 19: _LL371: return 331; case 20: _LL372: return 332;
case 21: _LL373: return 339; case 22: _LL374: return 338; case 23: _LL375:
return 337; case 24: _LL376: return 335; case 25: _LL377: return 336; case 26:
_LL378: return 328; case 27: _LL379: return 329; case 28: _LL380: return 322;
case 29: _LL381: return 323; case 30: _LL382: return 341; case 31: _LL383:
return 319; case 32: _LL384: return 340; case 33: _LL385: return 342; case 34:
_LL386: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)( lexbuf);
case 35: _LL387: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 36: _LL388: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 37: _LL389: Cyc_Lex_comment_depth= 1; Cyc_Lex_runaway_start=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)( struct
Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)( lexbuf); case 38: _LL390: Cyc_Lex_string_pos= 0; Cyc_Lex_runaway_start=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); Cyc_Lex_token_string= Cyc_Lex_get_stored_string();
return 345; case 39: _LL391: Cyc_Lex_token_char='\a'; return 346; case 40:
_LL392: Cyc_Lex_token_char='\b'; return 346; case 41: _LL393: Cyc_Lex_token_char='\f';
return 346; case 42: _LL394: Cyc_Lex_token_char='\n'; return 346; case 43:
_LL395: Cyc_Lex_token_char='\r'; return 346; case 44: _LL396: Cyc_Lex_token_char='\t';
return 346; case 45: _LL397: Cyc_Lex_token_char='\v'; return 346; case 46:
_LL398: Cyc_Lex_token_char='\\'; return 346; case 47: _LL399: Cyc_Lex_token_char='\'';
return 346; case 48: _LL400: Cyc_Lex_token_char='"'; return 346; case 49: _LL401:
Cyc_Lex_token_char='?'; return 346; case 50: _LL402: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 3); return 346; case 51: _LL403: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 2); return 346; case 52: _LL404: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 1); return 346; case 53: _LL405: Cyc_Lex_token_char= Cyc_Lex_char_for_hex_code(((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf),
3); return 346; case 54: _LL406: Cyc_Lex_token_char=(( char(*)( struct Cyc_Lexing_lexbuf*,
int)) Cyc_Lexing_lexeme_char)( lexbuf, 1); return 346; case 55: _LL407: return -
1; case 56: _LL408: return( int)(( char(*)( struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)(
lexbuf, 0); default: _LL409:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf,
lexstate);}( void) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp411=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp411[ 0]=({ struct Cyc_Lexing_Error_struct _temp412; _temp412.tag= Cyc_Lexing_Error;
_temp412.f1=({ char* _temp413="some action didn't return!"; struct
_tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 27; _temp414;}); _temp412;}); _temp411;}));}
int Cyc_Lex_token( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf, 0);} int
Cyc_Lex_strng_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt
== 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables* _temp415=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp415->lex_base=({ int*
_temp424=( int*) Cyc_Lex_lbase; struct _tagged_ptr0 _temp425; _temp425.curr=
_temp424; _temp425.base= _temp424; _temp425.last_plus_one= _temp424 + 151;
_temp425;}); _temp415->lex_backtrk=({ int* _temp422=( int*) Cyc_Lex_lbacktrk;
struct _tagged_ptr0 _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 151; _temp423;}); _temp415->lex_default=({
int* _temp420=( int*) Cyc_Lex_ldefault; struct _tagged_ptr0 _temp421; _temp421.curr=
_temp420; _temp421.base= _temp420; _temp421.last_plus_one= _temp420 + 151;
_temp421;}); _temp415->lex_trans=({ int* _temp418=( int*) Cyc_Lex_ltrans; struct
_tagged_ptr0 _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 1813; _temp419;}); _temp415->lex_check=({ int*
_temp416=( int*) Cyc_Lex_lcheck; struct _tagged_ptr0 _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 1813;
_temp417;}); _temp415;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*)
_check_null( Cyc_Lex_lt), lexstate, lexbuf); switch( lexstate){ case 0: _LL426:
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 1:
_LL427: return 0; case 2: _LL428: return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 3: _LL429: Cyc_Lex_store_string_char('\a'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 4: _LL430: Cyc_Lex_store_string_char('\b');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 5:
_LL431: Cyc_Lex_store_string_char('\f'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 6: _LL432: Cyc_Lex_store_string_char('\n'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 7: _LL433: Cyc_Lex_store_string_char('\r');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 8:
_LL434: Cyc_Lex_store_string_char('\t'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 9: _LL435: Cyc_Lex_store_string_char('\v'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 10: _LL436:
Cyc_Lex_store_string_char('\\'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 11: _LL437: Cyc_Lex_store_string_char('\'');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 12:
_LL438: Cyc_Lex_store_string_char('"'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 13: _LL439: Cyc_Lex_store_string_char('?'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 14: _LL440:
Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf* lb, int start,
int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 3)); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 15: _LL441: Cyc_Lex_store_string_char(((
char(*)( struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 1, 2)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 16: _LL442: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 1));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 17:
_LL443: Cyc_Lex_store_string_char( Cyc_Lex_char_for_hex_code((( struct
_tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf), 2));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 18:
_LL444: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lexbuf, 0)); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 19: _LL445:(( void(*)( struct _tagged_string msg,
struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(({ char* _temp447="string ends in newline";
struct _tagged_string _temp448; _temp448.curr= _temp447; _temp448.base= _temp447;
_temp448.last_plus_one= _temp447 + 23; _temp448;}), lexbuf); return 0; case 20:
_LL446:(( void(*)( struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(({
char* _temp450="unterminated string"; struct _tagged_string _temp451; _temp451.curr=
_temp450; _temp451.base= _temp450; _temp451.last_plus_one= _temp450 + 20;
_temp451;}), lexbuf); return 0; case 21: _LL449:(( void(*)( struct
_tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_err)(({ char*
_temp453="bad character following backslash in string"; struct _tagged_string
_temp454; _temp454.curr= _temp453; _temp454.base= _temp453; _temp454.last_plus_one=
_temp453 + 44; _temp454;}), lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); default: _LL452:( lexbuf->refill_buff)( lexbuf); return((
int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, lexstate);}( void) _throw(( void*)({ struct Cyc_Lexing_Error_struct*
_temp456=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp456[ 0]=({ struct Cyc_Lexing_Error_struct _temp457; _temp457.tag= Cyc_Lexing_Error;
_temp457.f1=({ char* _temp458="some action didn't return!"; struct
_tagged_string _temp459; _temp459.curr= _temp458; _temp459.base= _temp458;
_temp459.last_plus_one= _temp458 + 27; _temp459;}); _temp457;}); _temp456;}));}
int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)( lexbuf, 1);} int
Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt
== 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables* _temp460=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp460->lex_base=({ int*
_temp469=( int*) Cyc_Lex_lbase; struct _tagged_ptr0 _temp470; _temp470.curr=
_temp469; _temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 151;
_temp470;}); _temp460->lex_backtrk=({ int* _temp467=( int*) Cyc_Lex_lbacktrk;
struct _tagged_ptr0 _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 151; _temp468;}); _temp460->lex_default=({
int* _temp465=( int*) Cyc_Lex_ldefault; struct _tagged_ptr0 _temp466; _temp466.curr=
_temp465; _temp466.base= _temp465; _temp466.last_plus_one= _temp465 + 151;
_temp466;}); _temp460->lex_trans=({ int* _temp463=( int*) Cyc_Lex_ltrans; struct
_tagged_ptr0 _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 1813; _temp464;}); _temp460->lex_check=({ int*
_temp461=( int*) Cyc_Lex_lcheck; struct _tagged_ptr0 _temp462; _temp462.curr=
_temp461; _temp462.base= _temp461; _temp462.last_plus_one= _temp461 + 1813;
_temp462;}); _temp460;});} lexstate=(( int(*)( struct Cyc_Lexing_lex_tables*,
int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)(( struct Cyc_Lexing_lex_tables*)
_check_null( Cyc_Lex_lt), lexstate, lexbuf); switch( lexstate){ case 0: _LL471:
++ Cyc_Lex_comment_depth; return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf); case 1: _LL472: -- Cyc_Lex_comment_depth; if( Cyc_Lex_comment_depth > 0){
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf);} return
0; case 2: _LL473:(( void(*)( struct _tagged_string msg, struct Cyc_Lexing_lexbuf*
lb)) Cyc_Lex_runaway_err)(({ char* _temp475="unterminated comment"; struct
_tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 21; _temp476;}), lexbuf); return 0; case 3:
_LL474: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf);
case 4: _LL477: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf); case 5: _LL478: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf); default: _LL479:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf,
lexstate);}( void) _throw(( void*)({ struct Cyc_Lexing_Error_struct* _temp481=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp481[ 0]=({ struct Cyc_Lexing_Error_struct _temp482; _temp482.tag= Cyc_Lexing_Error;
_temp482.f1=({ char* _temp483="some action didn't return!"; struct
_tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 27; _temp484;}); _temp482;}); _temp481;}));}
int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf, 2);}
void Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=({ struct Cyc_Lex_Trie* _temp485=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp485[ 0]=({
struct Cyc_Lex_Trie _temp486; _temp486.children= 0; _temp486.shared_str= - 1;
_temp486;}); _temp485;}); Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp487=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp487[ 0]=({
struct Cyc_Lex_Trie _temp488; _temp488.children= 0; _temp488.shared_str= 0;
_temp488;}); _temp487;}); Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)(( struct
Cyc_Xarray_Xarray*(*)( int, struct _tagged_string*)) Cyc_Xarray_create)( 101,({
struct _tagged_string* _temp489=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp489[ 0]=({ char* _temp490=""; struct
_tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 1; _temp491;}); _temp489;})); Cyc_Lex_num_kws=(
int) 62u; Cyc_Lex_kw_nums=({ unsigned int _temp492=( unsigned int) Cyc_Lex_num_kws;
int* _temp493=( int*) GC_malloc_atomic( sizeof( int) * _temp492); struct
_tagged_ptr0 _temp495={ _temp493, _temp493, _temp493 + _temp492};{ unsigned int
_temp494= _temp492; unsigned int i; for( i= 0; i < _temp494; i ++){ _temp493[ i]=
0;}}; _temp495;});{ int i= 0; for( 0; i < Cyc_Lex_num_kws; ++ i){ struct
_tagged_string _temp496=( Cyc_Lex_rw_array[ _check_known_subscript_notnull( 62u,
i)]).f1; goto _LL497; _LL497: Cyc_Lex_str_index( _temp496, 0,( int) Cyc_String_strlen(
_temp496));*(( int*(*)( struct _tagged_ptr0, unsigned int, unsigned int))
_check_unknown_subscript)( Cyc_Lex_kw_nums, sizeof( int), i)=( int)( Cyc_Lex_rw_array[
_check_known_subscript_notnull( 62u, i)]).f2;}} Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth=
0;}