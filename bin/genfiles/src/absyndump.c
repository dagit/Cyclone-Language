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
tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fputc( int,
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern int Cyc_Stdio_file_string_write(
struct Cyc_Stdio___sFILE* fd, struct _tagged_string src, int src_offset, int
max_count); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty[ 15u]; extern struct
Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; typedef struct
Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0;
static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[ 14u]; typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t;
typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef void*
Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces;
struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict*
tuniondecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs;
struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List* availables; } ; typedef
struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct
Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Absyndump_dumptyp(
void*); extern void Cyc_Absyndump_dumpntyp( void* t); extern void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
extern void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*); extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*);
extern void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual, void*, void(* f)(
void*), void*); typedef struct _tagged_string Cyc_Absyndump_dump_string_t;
struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout; void Cyc_Absyndump_ignore(
void* x){ return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp3= s;(
unsigned int)( _temp3.last_plus_one - _temp3.curr);}); if( !(( int)({ struct
_tagged_string _temp0= s; char* _temp2= _temp0.curr +( sz - 1); if( _temp0.base
== 0? 1:( _temp2 < _temp0.base? 1: _temp2 >= _temp0.last_plus_one)){ _throw(
Null_Exception);}* _temp2;}))){ -- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos
> 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);}
else{ Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp7= s;(
unsigned int)( _temp7.last_plus_one - _temp7.curr);}); if( !(( int)({ struct
_tagged_string _temp4= s; char* _temp6= _temp4.curr +( sz - 1); if( _temp4.base
== 0? 1:( _temp6 < _temp4.base? 1: _temp6 >= _temp4.last_plus_one)){ _throw(
Null_Exception);}* _temp6;}))){ -- sz;} Cyc_Absyndump_pos += sz; Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char( int c){ ++
Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str(
struct _tagged_string* s){ Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep( void(* f)(
void*), struct Cyc_List_List* l, struct _tagged_string sep){ if( l == 0){
return;} for( 0;({ struct Cyc_List_List* _temp8= l; if( _temp8 == 0){ _throw(
Null_Exception);} _temp8->tl;}) != 0; l=({ struct Cyc_List_List* _temp9= l; if(
_temp9 == 0){ _throw( Null_Exception);} _temp9->tl;})){ f(( void*)({ struct Cyc_List_List*
_temp10= l; if( _temp10 == 0){ _throw( Null_Exception);} _temp10->hd;})); Cyc_Absyndump_dump_nospace(
sep);} f(( void*)({ struct Cyc_List_List* _temp11= l; if( _temp11 == 0){ _throw(
Null_Exception);} _temp11->hd;}));} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;({ struct Cyc_List_List* _temp12= l; if( _temp12 ==
0){ _throw( Null_Exception);} _temp12->tl;}) != 0; l=({ struct Cyc_List_List*
_temp13= l; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->tl;})){ f( env,(
void*)({ struct Cyc_List_List* _temp14= l; if( _temp14 == 0){ _throw(
Null_Exception);} _temp14->hd;})); Cyc_Absyndump_dump_nospace( sep);} f( env,(
void*)({ struct Cyc_List_List* _temp15= l; if( _temp15 == 0){ _throw(
Null_Exception);} _temp15->hd;}));} void Cyc_Absyndump_group( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*, void*), void* env, struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep){ if( l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* nsl= 0;{ void* _temp16=(* v).f1; struct Cyc_List_List*
_temp28; struct Cyc_List_List* _temp30; struct Cyc_List_List* _temp32; struct
Cyc_List_List* _temp34; _LL18: if( _temp16 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL19;} else{ goto _LL20;} _LL20: if(( unsigned int) _temp16 > 1u?*(( int*)
_temp16) == Cyc_Absyn_Rel_n: 0){ _LL29: _temp28=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if( _temp28 == 0){ goto _LL21;}
else{ goto _LL22;}} else{ goto _LL22;} _LL22: if(( unsigned int) _temp16 > 1u?*((
int*) _temp16) == Cyc_Absyn_Abs_n: 0){ _LL31: _temp30=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp16)->f1; if( _temp30 == 0){ goto _LL23;}
else{ goto _LL24;}} else{ goto _LL24;} _LL24: if(( unsigned int) _temp16 > 1u?*((
int*) _temp16) == Cyc_Absyn_Rel_n: 0){ _LL33: _temp32=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; goto _LL25;} else{ goto _LL26;}
_LL26: if(( unsigned int) _temp16 > 1u?*(( int*) _temp16) == Cyc_Absyn_Abs_n: 0){
_LL35: _temp34=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp16)->f1; goto _LL27;} else{ goto _LL17;} _LL19: goto _LL21; _LL21: goto
_LL23; _LL23: Cyc_Absyndump_dump_str((* v).f2); return; _LL25: _temp34= _temp32;
goto _LL27; _LL27: nsl= _temp34; goto _LL17; _LL17:;} Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_List_List* _temp36= nsl; if( _temp36 == 0){
_throw( Null_Exception);} _temp36->hd;})); for( nsl=({ struct Cyc_List_List*
_temp37= nsl; if( _temp37 == 0){ _throw( Null_Exception);} _temp37->tl;}); nsl
!= 0; nsl=({ struct Cyc_List_List* _temp38= nsl; if( _temp38 == 0){ _throw(
Null_Exception);} _temp38->tl;})){ if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp39=( char*)"::"; struct _tagged_string _temp40; _temp40.curr= _temp39;
_temp40.base= _temp39; _temp40.last_plus_one= _temp39 + 3; _temp40;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*)({ struct Cyc_List_List* _temp41= nsl; if( _temp41 == 0){
_throw( Null_Exception);} _temp41->hd;})));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp42=( char*)"_"; struct _tagged_string
_temp43; _temp43.curr= _temp42; _temp43.base= _temp42; _temp43.last_plus_one=
_temp42 + 2; _temp43;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp44=( char*)"::"; struct _tagged_string _temp45;
_temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one= _temp44 + 3;
_temp45;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp46=( char*)"restrict"; struct _tagged_string
_temp47; _temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one=
_temp46 + 9; _temp47;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp48=( char*)"volatile"; struct _tagged_string
_temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 9; _temp49;}));} if( tq.q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp50=( char*)"const"; struct _tagged_string _temp51;
_temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one= _temp50 + 6;
_temp51;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp52= sc; _LL54:
if( _temp52 ==( void*) Cyc_Absyn_Static){ goto _LL55;} else{ goto _LL56;} _LL56:
if( _temp52 ==( void*) Cyc_Absyn_Public){ goto _LL57;} else{ goto _LL58;} _LL58:
if( _temp52 ==( void*) Cyc_Absyn_Extern){ goto _LL59;} else{ goto _LL60;} _LL60:
if( _temp52 ==( void*) Cyc_Absyn_ExternC){ goto _LL61;} else{ goto _LL62;} _LL62:
if( _temp52 ==( void*) Cyc_Absyn_Abstract){ goto _LL63;} else{ goto _LL53;}
_LL55: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp64=( char*)"static";
struct _tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64;
_temp65.last_plus_one= _temp64 + 7; _temp65;})); return; _LL57: return; _LL59:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp66=( char*)"extern";
struct _tagged_string _temp67; _temp67.curr= _temp66; _temp67.base= _temp66;
_temp67.last_plus_one= _temp66 + 7; _temp67;})); return; _LL61: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp68=( char*)"extern \"C\""; struct
_tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 11; _temp69;})); return; _LL63: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp70=( char*)"abstract"; struct _tagged_string
_temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 9; _temp71;})); return; _LL53:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp72= k; _LL74: if( _temp72 ==( void*) Cyc_Absyn_AnyKind){ goto _LL75;}
else{ goto _LL76;} _LL76: if( _temp72 ==( void*) Cyc_Absyn_MemKind){ goto _LL77;}
else{ goto _LL78;} _LL78: if( _temp72 ==( void*) Cyc_Absyn_BoxKind){ goto _LL79;}
else{ goto _LL80;} _LL80: if( _temp72 ==( void*) Cyc_Absyn_RgnKind){ goto _LL81;}
else{ goto _LL82;} _LL82: if( _temp72 ==( void*) Cyc_Absyn_EffKind){ goto _LL83;}
else{ goto _LL73;} _LL75: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp84=( char*)"A"; struct _tagged_string _temp85; _temp85.curr= _temp84;
_temp85.base= _temp84; _temp85.last_plus_one= _temp84 + 2; _temp85;})); return;
_LL77: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp86=( char*)"M";
struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86;
_temp87.last_plus_one= _temp86 + 2; _temp87;})); return; _LL79: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp88=( char*)"B"; struct _tagged_string
_temp89; _temp89.curr= _temp88; _temp89.base= _temp88; _temp89.last_plus_one=
_temp88 + 2; _temp89;})); return; _LL81: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp90=( char*)"R"; struct _tagged_string _temp91;
_temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one= _temp90 + 2;
_temp91;})); return; _LL83: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp92=( char*)"E"; struct _tagged_string _temp93; _temp93.curr= _temp92;
_temp93.base= _temp92; _temp93.last_plus_one= _temp92 + 2; _temp93;})); return;
_LL73:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp,
ts,( struct _tagged_string)({ char* _temp94=( char*)"<"; struct _tagged_string
_temp95; _temp95.curr= _temp94; _temp95.base= _temp94; _temp95.last_plus_one=
_temp94 + 2; _temp95;}),( struct _tagged_string)({ char* _temp96=( char*)">";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 2; _temp97;}),( struct _tagged_string)({ char*
_temp98=( char*)","; struct _tagged_string _temp99; _temp99.curr= _temp98;
_temp99.base= _temp98; _temp99.last_plus_one= _temp98 + 2; _temp99;}));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp100=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp108; void* _temp110;
_LL102: if(( unsigned int) _temp100 > 1u?*(( int*) _temp100) == Cyc_Absyn_Eq_constr:
0){ _LL109: _temp108=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp100)->f1;
if( _temp108 ==( void*) Cyc_Absyn_BoxKind){ goto _LL103;} else{ goto _LL104;}}
else{ goto _LL104;} _LL104: if(( unsigned int) _temp100 > 1u?*(( int*) _temp100)
== Cyc_Absyn_Eq_constr: 0){ _LL111: _temp110=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp100)->f1; goto _LL105;} else{ goto _LL106;} _LL106: goto _LL107; _LL103:
goto _LL101; _LL105: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp112=( char*)"::"; struct _tagged_string _temp113; _temp113.curr= _temp112;
_temp113.base= _temp112; _temp113.last_plus_one= _temp112 + 3; _temp113;})); Cyc_Absyndump_dumpkind(
_temp110); goto _LL101; _LL107: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp114=( char*)"::?"; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 4;
_temp115;})); goto _LL101; _LL101:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({
char* _temp116=( char*)"<"; struct _tagged_string _temp117; _temp117.curr=
_temp116; _temp117.base= _temp116; _temp117.last_plus_one= _temp116 + 2;
_temp117;}),( struct _tagged_string)({ char* _temp118=( char*)">"; struct
_tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}),( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));} void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar, tvs,( struct _tagged_string)({ char* _temp122=(
char*)"<"; struct _tagged_string _temp123; _temp123.curr= _temp122; _temp123.base=
_temp122; _temp123.last_plus_one= _temp122 + 2; _temp123;}),( struct
_tagged_string)({ char* _temp124=( char*)">"; struct _tagged_string _temp125;
_temp125.curr= _temp124; _temp125.base= _temp124; _temp125.last_plus_one=
_temp124 + 2; _temp125;}),( struct _tagged_string)({ char* _temp126=( char*)",";
struct _tagged_string _temp127; _temp127.curr= _temp126; _temp127.base= _temp126;
_temp127.last_plus_one= _temp126 + 2; _temp127;}));} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((*
pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs(
struct Cyc_List_List* ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg, ts,( struct _tagged_string)({
char* _temp128=( char*)"("; struct _tagged_string _temp129; _temp129.curr=
_temp128; _temp129.base= _temp128; _temp129.last_plus_one= _temp128 + 2;
_temp129;}),( struct _tagged_string)({ char* _temp130=( char*)")"; struct
_tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 2; _temp131;}),( struct _tagged_string)({
char* _temp132=( char*)","; struct _tagged_string _temp133; _temp133.curr=
_temp132; _temp133.base= _temp132; _temp133.last_plus_one= _temp132 + 2;
_temp133;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp134=( char*)" __attribute__(("; struct _tagged_string _temp135; _temp135.curr=
_temp134; _temp135.base= _temp134; _temp135.last_plus_one= _temp134 + 17;
_temp135;})); for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp136= atts;
if( _temp136 == 0){ _throw( Null_Exception);} _temp136->tl;})){ Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp137= atts; if(
_temp137 == 0){ _throw( Null_Exception);} _temp137->hd;}))); if(({ struct Cyc_List_List*
_temp138= atts; if( _temp138 == 0){ _throw( Null_Exception);} _temp138->tl;}) !=
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp139=( char*)",";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 2; _temp140;}));}} Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp141=( char*)")) "; struct _tagged_string
_temp142; _temp142.curr= _temp141; _temp142.base= _temp141; _temp142.last_plus_one=
_temp141 + 4; _temp142;}));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp144=( void*)({ struct Cyc_List_List*
_temp143= tms; if( _temp143 == 0){ _throw( Null_Exception);} _temp143->hd;});
struct Cyc_Absyn_Tqual _temp150; void* _temp152; void* _temp154; _LL146: if((
unsigned int) _temp144 > 1u?*(( int*) _temp144) == Cyc_Absyn_Pointer_mod: 0){
_LL155: _temp154=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp144)->f1;
goto _LL153; _LL153: _temp152=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp144)->f2; goto _LL151; _LL151: _temp150=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp144)->f3; goto _LL147;} else{ goto _LL148;}
_LL148: goto _LL149; _LL147: return 1; _LL149: return 0; _LL145:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp156= Cyc_Tcutil_compress( t); _LL158:
if( _temp156 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL159;} else{ goto _LL160;}
_LL160: goto _LL161; _LL159: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp162=( char*)"`H"; struct _tagged_string _temp163; _temp163.curr= _temp162;
_temp163.base= _temp162; _temp163.last_plus_one= _temp162 + 3; _temp163;}));
goto _LL157; _LL161: Cyc_Absyndump_dumpntyp( t); goto _LL157; _LL157:;} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List* regions= 0;
struct Cyc_List_List* effects= 0;{ void* _temp164= Cyc_Tcutil_compress( t); void*
_temp172; struct Cyc_List_List* _temp174; _LL166: if(( unsigned int) _temp164 >
4u?*(( int*) _temp164) == Cyc_Absyn_AccessEff: 0){ _LL173: _temp172=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp164)->f1; goto _LL167;} else{ goto
_LL168;} _LL168: if(( unsigned int) _temp164 > 4u?*(( int*) _temp164) == Cyc_Absyn_JoinEff:
0){ _LL175: _temp174=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp164)->f1; goto _LL169;} else{ goto _LL170;} _LL170: goto _LL171; _LL167:
regions=({ struct Cyc_List_List* _temp176=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp176->hd=( void*) _temp172; _temp176->tl=
regions; _temp176;}); goto _LL165; _LL169: for( 0; _temp174 != 0; _temp174=({
struct Cyc_List_List* _temp177= _temp174; if( _temp177 == 0){ _throw(
Null_Exception);} _temp177->tl;})){ struct Cyc_List_List* _temp181; struct Cyc_List_List*
_temp183; struct _tuple6 _temp179= Cyc_Absyndump_effects_split(( void*)({ struct
Cyc_List_List* _temp178= _temp174; if( _temp178 == 0){ _throw( Null_Exception);}
_temp178->hd;})); _LL184: _temp183= _temp179.f1; goto _LL182; _LL182: _temp181=
_temp179.f2; goto _LL180; _LL180: regions=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp183,
regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp181, effects);} goto _LL165;
_LL171: effects=({ struct Cyc_List_List* _temp185=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp185->hd=( void*) t; _temp185->tl=
effects; _temp185;}); goto _LL165; _LL165:;} return({ struct _tuple6 _temp186;
_temp186.f1= regions; _temp186.f2= effects; _temp186;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp189; struct Cyc_List_List* _temp191; struct
_tuple6 _temp187= Cyc_Absyndump_effects_split( t); _LL192: _temp191= _temp187.f1;
goto _LL190; _LL190: _temp189= _temp187.f2; goto _LL188; _LL188: _temp191=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp191);
_temp189=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp189); for( 0; _temp189 != 0; _temp189=({ struct Cyc_List_List* _temp193=
_temp189; if( _temp193 == 0){ _throw( Null_Exception);} _temp193->tl;})){ Cyc_Absyndump_dumpntyp((
void*)({ struct Cyc_List_List* _temp194= _temp189; if( _temp194 == 0){ _throw(
Null_Exception);} _temp194->hd;})); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp191 != 0; _temp191=({ struct Cyc_List_List* _temp195=
_temp191; if( _temp195 == 0){ _throw( Null_Exception);} _temp195->tl;})){ Cyc_Absyndump_dumprgn((
void*)({ struct Cyc_List_List* _temp196= _temp191; if( _temp196 == 0){ _throw(
Null_Exception);} _temp196->hd;})); if(({ struct Cyc_List_List* _temp197=
_temp191; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->tl;}) != 0){
Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');} void
Cyc_Absyndump_dumpntyp( void* t){ void* _temp198= t; struct Cyc_Absyn_Exp*
_temp260; struct Cyc_Absyn_Tqual _temp262; void* _temp264; struct Cyc_Absyn_FnInfo
_temp266; struct Cyc_Absyn_PtrInfo _temp268; struct Cyc_Absyn_Tvar* _temp270;
int _temp272; struct Cyc_Core_Opt* _temp274; void* _temp276; int _temp278;
struct Cyc_Core_Opt* _temp280; struct Cyc_Core_Opt _temp282; void* _temp283;
void* _temp285; struct Cyc_Absyn_TunionInfo _temp287; void* _temp289; struct Cyc_List_List*
_temp291; void* _temp293; struct Cyc_Absyn_TunionFieldInfo _temp295; struct Cyc_List_List*
_temp297; void* _temp299; struct Cyc_Absyn_Enumdecl* _temp301; struct _tuple0*
_temp303; void* _temp305; void* _temp307; void* _temp309; void* _temp311; void*
_temp313; void* _temp315; void* _temp317; void* _temp319; void* _temp321; void*
_temp323; void* _temp325; void* _temp327; void* _temp329; void* _temp331; void*
_temp333; void* _temp335; struct Cyc_List_List* _temp337; struct Cyc_Absyn_Structdecl**
_temp339; struct Cyc_List_List* _temp341; struct _tuple0* _temp343; struct Cyc_Absyn_Structdecl**
_temp345; struct Cyc_List_List* _temp347; struct _tuple0* _temp349; struct Cyc_Absyn_Uniondecl**
_temp351; struct Cyc_List_List* _temp353; struct _tuple0* _temp355; struct Cyc_Absyn_Uniondecl**
_temp357; struct Cyc_List_List* _temp359; struct _tuple0* _temp361; struct Cyc_Core_Opt*
_temp363; struct Cyc_List_List* _temp365; struct _tuple0* _temp367; void*
_temp369; void* _temp371; struct Cyc_List_List* _temp373; _LL200: if((
unsigned int) _temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_ArrayType: 0){
_LL265: _temp264=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f1;
goto _LL263; _LL263: _temp262=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp198)->f2; goto _LL261; _LL261: _temp260=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp198)->f3; goto _LL201;} else{ goto _LL202;}
_LL202: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_FnType:
0){ _LL267: _temp266=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp198)->f1; goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_PointerType: 0){ _LL269: _temp268=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp198)->f1;
goto _LL205;} else{ goto _LL206;} _LL206: if( _temp198 ==( void*) Cyc_Absyn_VoidType){
goto _LL207;} else{ goto _LL208;} _LL208: if(( unsigned int) _temp198 > 4u?*((
int*) _temp198) == Cyc_Absyn_VarType: 0){ _LL271: _temp270=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp198)->f1; goto _LL209;} else{ goto _LL210;}
_LL210: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_Evar:
0){ _LL277: _temp276=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f1;
goto _LL275; _LL275: _temp274=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp198)->f2; if( _temp274 == 0){ goto _LL273;} else{ goto _LL212;} _LL273:
_temp272=( int)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f3; goto _LL211;}
else{ goto _LL212;} _LL212: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198)
== Cyc_Absyn_Evar: 0){ _LL286: _temp285=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp198)->f1; goto _LL281; _LL281: _temp280=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp198)->f2; if( _temp280 == 0){ goto _LL214;} else{
_temp282=* _temp280; _LL284: _temp283=( void*) _temp282.v; goto _LL279;} _LL279:
_temp278=( int)(( struct Cyc_Absyn_Evar_struct*) _temp198)->f3; goto _LL213;}
else{ goto _LL214;} _LL214: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198)
== Cyc_Absyn_TunionType: 0){ _LL288: _temp287=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp198)->f1; _LL294: _temp293=( void*)
_temp287.tunion_info; goto _LL292; _LL292: _temp291=( struct Cyc_List_List*)
_temp287.targs; goto _LL290; _LL290: _temp289=( void*) _temp287.rgn; goto _LL215;}
else{ goto _LL216;} _LL216: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198)
== Cyc_Absyn_TunionFieldType: 0){ _LL296: _temp295=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp198)->f1; _LL300: _temp299=( void*)
_temp295.field_info; goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)
_temp295.targs; goto _LL217;} else{ goto _LL218;} _LL218: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_EnumType: 0){ _LL304: _temp303=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp198)->f1; goto _LL302;
_LL302: _temp301=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp198)->f2; goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL308: _temp307=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp307 ==( void*)
Cyc_Absyn_Signed){ goto _LL306;} else{ goto _LL222;} _LL306: _temp305=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp305 ==( void*) Cyc_Absyn_B1){
goto _LL221;} else{ goto _LL222;}} else{ goto _LL222;} _LL222: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL312: _temp311=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp311 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL310;} else{ goto _LL224;} _LL310: _temp309=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp309 ==( void*) Cyc_Absyn_B1){
goto _LL223;} else{ goto _LL224;}} else{ goto _LL224;} _LL224: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL316: _temp315=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp315 ==( void*)
Cyc_Absyn_Signed){ goto _LL314;} else{ goto _LL226;} _LL314: _temp313=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp313 ==( void*) Cyc_Absyn_B2){
goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;} _LL226: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL320: _temp319=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp319 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL318;} else{ goto _LL228;} _LL318: _temp317=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp317 ==( void*) Cyc_Absyn_B2){
goto _LL227;} else{ goto _LL228;}} else{ goto _LL228;} _LL228: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL324: _temp323=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp323 ==( void*)
Cyc_Absyn_Signed){ goto _LL322;} else{ goto _LL230;} _LL322: _temp321=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp321 ==( void*) Cyc_Absyn_B4){
goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL328: _temp327=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp327 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL326;} else{ goto _LL232;} _LL326: _temp325=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp325 ==( void*) Cyc_Absyn_B4){
goto _LL231;} else{ goto _LL232;}} else{ goto _LL232;} _LL232: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL332: _temp331=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp331 ==( void*)
Cyc_Absyn_Signed){ goto _LL330;} else{ goto _LL234;} _LL330: _temp329=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp329 ==( void*) Cyc_Absyn_B8){
goto _LL233;} else{ goto _LL234;}} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_IntType: 0){ _LL336: _temp335=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp198)->f1; if( _temp335 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL334;} else{ goto _LL236;} _LL334: _temp333=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp198)->f2; if( _temp333 ==( void*) Cyc_Absyn_B8){
goto _LL235;} else{ goto _LL236;}} else{ goto _LL236;} _LL236: if( _temp198 ==(
void*) Cyc_Absyn_FloatType){ goto _LL237;} else{ goto _LL238;} _LL238: if(
_temp198 ==( void*) Cyc_Absyn_DoubleType){ goto _LL239;} else{ goto _LL240;}
_LL240: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_TupleType:
0){ _LL338: _temp337=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp198)->f1; goto _LL241;} else{ goto _LL242;} _LL242: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_StructType: 0){ _LL344: _temp343=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1; if(
_temp343 == 0){ goto _LL342;} else{ goto _LL244;} _LL342: _temp341=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp198)->f2; goto _LL340; _LL340:
_temp339=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp198)->f3; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_StructType: 0){ _LL350: _temp349=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp198)->f1; goto
_LL348; _LL348: _temp347=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp198)->f2; goto _LL346; _LL346: _temp345=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp198)->f3; goto _LL245;} else{ goto
_LL246;} _LL246: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_UnionType:
0){ _LL356: _temp355=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f1; if( _temp355 == 0){ goto _LL354;} else{ goto _LL248;} _LL354:
_temp353=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f2;
goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f3; goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_UnionType: 0){ _LL362: _temp361=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp198)->f1; goto
_LL360; _LL360: _temp359=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp198)->f2; goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp198)->f3; goto _LL249;} else{ goto
_LL250;} _LL250: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_TypedefType:
0){ _LL368: _temp367=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp198)->f1; goto _LL366; _LL366: _temp365=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp198)->f2; goto _LL364; _LL364: _temp363=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*) _temp198)->f3;
goto _LL251;} else{ goto _LL252;} _LL252: if(( unsigned int) _temp198 > 4u?*((
int*) _temp198) == Cyc_Absyn_RgnHandleType: 0){ _LL370: _temp369=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp198)->f1; goto _LL253;} else{ goto
_LL254;} _LL254: if( _temp198 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL255;} else{
goto _LL256;} _LL256: if(( unsigned int) _temp198 > 4u?*(( int*) _temp198) ==
Cyc_Absyn_AccessEff: 0){ _LL372: _temp371=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp198)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int)
_temp198 > 4u?*(( int*) _temp198) == Cyc_Absyn_JoinEff: 0){ _LL374: _temp373=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp198)->f1; goto
_LL259;} else{ goto _LL199;} _LL201: return; _LL203: return; _LL205: return;
_LL207: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp375=( char*)"void";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 5; _temp376;})); return; _LL209: Cyc_Absyndump_dump_str(
_temp270->name); return; _LL211: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp377=( char*)"%"; struct _tagged_string _temp378; _temp378.curr=
_temp377; _temp378.base= _temp377; _temp378.last_plus_one= _temp377 + 2;
_temp378;})); Cyc_Absyndump_dumpkind( _temp276); Cyc_Absyndump_dump( xprintf("(%d)",
_temp272)); return; _LL213: Cyc_Absyndump_dumpntyp( _temp283); return; _LL215:{
void* _temp379= _temp293; struct Cyc_Absyn_UnknownTunionInfo _temp385; int
_temp387; struct _tuple0* _temp389; struct Cyc_Absyn_Tuniondecl* _temp391;
struct Cyc_Absyn_Tuniondecl _temp393; int _temp394; struct Cyc_Core_Opt*
_temp396; struct Cyc_List_List* _temp398; struct _tuple0* _temp400; void*
_temp402; _LL381: if(*(( int*) _temp379) == Cyc_Absyn_UnknownTunion){ _LL386:
_temp385=( struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp379)->f1; _LL390: _temp389=( struct _tuple0*) _temp385.name; goto _LL388;
_LL388: _temp387=( int) _temp385.is_xtunion; goto _LL382;} else{ goto _LL383;}
_LL383: if(*(( int*) _temp379) == Cyc_Absyn_KnownTunion){ _LL392: _temp391=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*) _temp379)->f1;
_temp393=* _temp391; _LL403: _temp402=( void*) _temp393.sc; goto _LL401; _LL401:
_temp400=( struct _tuple0*) _temp393.name; goto _LL399; _LL399: _temp398=(
struct Cyc_List_List*) _temp393.tvs; goto _LL397; _LL397: _temp396=( struct Cyc_Core_Opt*)
_temp393.fields; goto _LL395; _LL395: _temp394=( int) _temp393.is_xtunion; goto
_LL384;} else{ goto _LL380;} _LL382: _temp400= _temp389; _temp394= _temp387;
goto _LL384; _LL384: if( _temp394){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp404=( char*)"xtunion "; struct _tagged_string _temp405; _temp405.curr=
_temp404; _temp405.base= _temp404; _temp405.last_plus_one= _temp404 + 9;
_temp405;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp406=( char*)"tunion "; struct _tagged_string _temp407; _temp407.curr=
_temp406; _temp407.base= _temp406; _temp407.last_plus_one= _temp406 + 8;
_temp407;}));}{ void* _temp408= Cyc_Tcutil_compress( _temp289); _LL410: if(
_temp408 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL411;} else{ goto _LL412;} _LL412:
goto _LL413; _LL411: goto _LL409; _LL413: Cyc_Absyndump_dumptyp( _temp289); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp414=( char*)" "; struct _tagged_string
_temp415; _temp415.curr= _temp414; _temp415.base= _temp414; _temp415.last_plus_one=
_temp414 + 2; _temp415;})); goto _LL409; _LL409:;} Cyc_Absyndump_dumpqvar(
_temp400); Cyc_Absyndump_dumptps( _temp291); goto _LL380; _LL380:;} goto _LL199;
_LL217:{ void* _temp416= _temp299; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp422; int _temp424; struct _tuple0* _temp426; struct _tuple0* _temp428;
struct Cyc_Absyn_Tunionfield* _temp430; struct Cyc_Absyn_Tunionfield _temp432;
struct Cyc_Position_Segment* _temp433; struct Cyc_List_List* _temp435; struct
Cyc_List_List* _temp437; struct _tuple0* _temp439; struct Cyc_Absyn_Tuniondecl*
_temp441; struct Cyc_Absyn_Tuniondecl _temp443; int _temp444; struct Cyc_Core_Opt*
_temp446; struct Cyc_List_List* _temp448; struct _tuple0* _temp450; void*
_temp452; _LL418: if(*(( int*) _temp416) == Cyc_Absyn_UnknownTunionfield){
_LL423: _temp422=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp416)->f1; _LL429: _temp428=( struct _tuple0*) _temp422.tunion_name; goto
_LL427; _LL427: _temp426=( struct _tuple0*) _temp422.field_name; goto _LL425;
_LL425: _temp424=( int) _temp422.is_xtunion; goto _LL419;} else{ goto _LL420;}
_LL420: if(*(( int*) _temp416) == Cyc_Absyn_KnownTunionfield){ _LL442: _temp441=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp416)->f1; _temp443=* _temp441; _LL453: _temp452=( void*) _temp443.sc; goto
_LL451; _LL451: _temp450=( struct _tuple0*) _temp443.name; goto _LL449; _LL449:
_temp448=( struct Cyc_List_List*) _temp443.tvs; goto _LL447; _LL447: _temp446=(
struct Cyc_Core_Opt*) _temp443.fields; goto _LL445; _LL445: _temp444=( int)
_temp443.is_xtunion; goto _LL431; _LL431: _temp430=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp416)->f2; _temp432=* _temp430;
_LL440: _temp439=( struct _tuple0*) _temp432.name; goto _LL438; _LL438: _temp437=(
struct Cyc_List_List*) _temp432.tvs; goto _LL436; _LL436: _temp435=( struct Cyc_List_List*)
_temp432.typs; goto _LL434; _LL434: _temp433=( struct Cyc_Position_Segment*)
_temp432.loc; goto _LL421;} else{ goto _LL417;} _LL419: _temp450= _temp428;
_temp444= _temp424; _temp439= _temp426; goto _LL421; _LL421: if( _temp444){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp454=( char*)"xtunion "; struct
_tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 9; _temp455;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp456=( char*)"tunion "; struct _tagged_string
_temp457; _temp457.curr= _temp456; _temp457.base= _temp456; _temp457.last_plus_one=
_temp456 + 8; _temp457;}));} Cyc_Absyndump_dumpqvar( _temp450); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp458=( char*)"."; struct _tagged_string
_temp459; _temp459.curr= _temp458; _temp459.base= _temp458; _temp459.last_plus_one=
_temp458 + 2; _temp459;})); Cyc_Absyndump_dumpqvar( _temp439); Cyc_Absyndump_dumptps(
_temp297); goto _LL417; _LL417:;} goto _LL199; _LL219: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp460=( char*)"enum "; struct _tagged_string
_temp461; _temp461.curr= _temp460; _temp461.base= _temp460; _temp461.last_plus_one=
_temp460 + 6; _temp461;})); Cyc_Absyndump_dumpqvar( _temp303); return; _LL221:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp462=( char*)"signed char";
struct _tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 12; _temp463;})); return; _LL223: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp464=( char*)"char"; struct _tagged_string
_temp465; _temp465.curr= _temp464; _temp465.base= _temp464; _temp465.last_plus_one=
_temp464 + 5; _temp465;})); return; _LL225: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp466=( char*)"short"; struct _tagged_string _temp467;
_temp467.curr= _temp466; _temp467.base= _temp466; _temp467.last_plus_one=
_temp466 + 6; _temp467;})); return; _LL227: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp468=( char*)"unsigned short"; struct _tagged_string
_temp469; _temp469.curr= _temp468; _temp469.base= _temp468; _temp469.last_plus_one=
_temp468 + 15; _temp469;})); return; _LL229: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp470=( char*)"int"; struct _tagged_string _temp471;
_temp471.curr= _temp470; _temp471.base= _temp470; _temp471.last_plus_one=
_temp470 + 4; _temp471;})); return; _LL231: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp472=( char*)"unsigned int"; struct _tagged_string
_temp473; _temp473.curr= _temp472; _temp473.base= _temp472; _temp473.last_plus_one=
_temp472 + 13; _temp473;})); return; _LL233: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp474=( char*)"long long"; struct _tagged_string
_temp475; _temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 10; _temp475;})); return; _LL235: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp476=( char*)"unsigned long long"; struct
_tagged_string _temp477; _temp477.curr= _temp476; _temp477.base= _temp476;
_temp477.last_plus_one= _temp476 + 19; _temp477;})); return; _LL237: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp478=( char*)"float"; struct _tagged_string
_temp479; _temp479.curr= _temp478; _temp479.base= _temp478; _temp479.last_plus_one=
_temp478 + 6; _temp479;})); return; _LL239: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp480=( char*)"double"; struct _tagged_string
_temp481; _temp481.curr= _temp480; _temp481.base= _temp480; _temp481.last_plus_one=
_temp480 + 7; _temp481;})); return; _LL241: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp337); return; _LL243: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp482=( char*)"struct"; struct _tagged_string
_temp483; _temp483.curr= _temp482; _temp483.base= _temp482; _temp483.last_plus_one=
_temp482 + 7; _temp483;})); Cyc_Absyndump_dumptps( _temp341); return; _LL245:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp484=( char*)"struct";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 7; _temp485;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp486= _temp349; if( _temp486 == 0){
_throw( Null_Exception);} _temp486;})); Cyc_Absyndump_dumptps( _temp347);
return; _LL247: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp487=(
char*)"union"; struct _tagged_string _temp488; _temp488.curr= _temp487; _temp488.base=
_temp487; _temp488.last_plus_one= _temp487 + 6; _temp488;})); Cyc_Absyndump_dumptps(
_temp353); return; _LL249: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp489=( char*)"union"; struct _tagged_string _temp490; _temp490.curr=
_temp489; _temp490.base= _temp489; _temp490.last_plus_one= _temp489 + 6;
_temp490;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct _tuple0*
_temp491= _temp361; if( _temp491 == 0){ _throw( Null_Exception);} _temp491;}));
Cyc_Absyndump_dumptps( _temp359); return; _LL251: Cyc_Absyndump_dumpqvar(
_temp367), Cyc_Absyndump_dumptps( _temp365); return; _LL253: Cyc_Absyndump_dumprgn(
_temp369); return; _LL255: return; _LL257: return; _LL259: return; _LL199:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp492= vo; if( _temp492 == 0){
_throw( Null_Exception);} _temp492->v;}));}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3,
Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List*
args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char(( int)'(');
for( 0; args != 0; args=({ struct Cyc_List_List* _temp493= args; if( _temp493 ==
0){ _throw( Null_Exception);} _temp493->tl;})){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)({ struct Cyc_List_List* _temp494= args; if( _temp494 == 0){
_throw( Null_Exception);} _temp494->hd;})); if(({ struct Cyc_List_List* _temp495=
args; if( _temp495 == 0){ _throw( Null_Exception);} _temp495->tl;}) != 0? 1:
varargs){ Cyc_Absyndump_dump_char(( int)',');}} if( varargs){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp496=( char*)"..."; struct _tagged_string
_temp497; _temp497.curr= _temp496; _temp497.base= _temp496; _temp497.last_plus_one=
_temp496 + 4; _temp497;}));} if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*)({ struct Cyc_Core_Opt* _temp498= effopt; if( _temp498 == 0){ _throw(
Null_Exception);} _temp498->v;}));} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp499;
_temp499.q_const= 0; _temp499.q_volatile= 0; _temp499.q_restrict= 0; _temp499;}),
t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp500= d; struct Cyc_Absyn_Exp* _temp506; struct
_tagged_string* _temp508; _LL502: if(*(( int*) _temp500) == Cyc_Absyn_ArrayElement){
_LL507: _temp506=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp500)->f1; goto _LL503;} else{ goto _LL504;} _LL504: if(*(( int*) _temp500)
== Cyc_Absyn_FieldName){ _LL509: _temp508=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp500)->f1; goto _LL505;} else{ goto _LL501;} _LL503: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp510=( char*)".["; struct _tagged_string
_temp511; _temp511.curr= _temp510; _temp511.base= _temp510; _temp511.last_plus_one=
_temp510 + 3; _temp511;})); Cyc_Absyndump_dumpexp( _temp506); Cyc_Absyndump_dump_char((
int)']'); goto _LL501; _LL505: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp508); goto _LL501; _LL501:;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp512=(
char*)""; struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base=
_temp512; _temp513.last_plus_one= _temp512 + 1; _temp513;}),( struct
_tagged_string)({ char* _temp514=( char*)"="; struct _tagged_string _temp515;
_temp515.curr= _temp514; _temp515.base= _temp514; _temp515.last_plus_one=
_temp514 + 2; _temp515;}),( struct _tagged_string)({ char* _temp516=( char*)"=";
struct _tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 2; _temp517;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp518=(
char*)""; struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base=
_temp518; _temp519.last_plus_one= _temp518 + 1; _temp519;}),( struct
_tagged_string)({ char* _temp520=( char*)""; struct _tagged_string _temp521;
_temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 1; _temp521;}),( struct _tagged_string)({ char* _temp522=( char*)",";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 2; _temp523;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp524=( char*)"("; struct _tagged_string _temp525; _temp525.curr= _temp524;
_temp525.base= _temp524; _temp525.last_plus_one= _temp524 + 2; _temp525;}));}{
void* _temp526=( void*) e->r; void* _temp616; char _temp618; void* _temp620;
void* _temp622; short _temp624; void* _temp626; void* _temp628; int _temp630;
void* _temp632; void* _temp634; int _temp636; void* _temp638; void* _temp640;
long long _temp642; void* _temp644; void* _temp646; struct _tagged_string
_temp648; void* _temp650; void* _temp652; struct _tagged_string _temp654; struct
_tuple0* _temp656; void* _temp658; struct _tuple0* _temp660; struct Cyc_List_List*
_temp662; void* _temp664; struct Cyc_Absyn_Exp* _temp666; struct Cyc_Core_Opt*
_temp668; struct Cyc_Absyn_Exp* _temp670; void* _temp672; struct Cyc_Absyn_Exp*
_temp674; void* _temp676; struct Cyc_Absyn_Exp* _temp678; void* _temp680; struct
Cyc_Absyn_Exp* _temp682; void* _temp684; struct Cyc_Absyn_Exp* _temp686; struct
Cyc_Absyn_Exp* _temp688; struct Cyc_Absyn_Exp* _temp690; struct Cyc_Absyn_Exp*
_temp692; struct Cyc_Absyn_Exp* _temp694; struct Cyc_Absyn_Exp* _temp696; struct
Cyc_List_List* _temp698; struct Cyc_Absyn_Exp* _temp700; struct Cyc_List_List*
_temp702; struct Cyc_Absyn_Exp* _temp704; struct Cyc_Absyn_Exp* _temp706; struct
Cyc_Absyn_Exp* _temp708; struct Cyc_List_List* _temp710; struct Cyc_Absyn_Exp*
_temp712; struct Cyc_Absyn_Exp* _temp714; void* _temp716; struct Cyc_Absyn_Exp*
_temp718; struct Cyc_Absyn_Exp* _temp720; struct Cyc_Absyn_Exp* _temp722; void*
_temp724; struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Exp* _temp728; struct
_tagged_string* _temp730; struct Cyc_Absyn_Exp* _temp732; struct _tagged_string*
_temp734; struct Cyc_Absyn_Exp* _temp736; struct Cyc_Absyn_Exp* _temp738; struct
Cyc_Absyn_Exp* _temp740; struct Cyc_List_List* _temp742; struct Cyc_List_List*
_temp744; struct _tuple1* _temp746; struct Cyc_List_List* _temp748; struct Cyc_Absyn_Exp*
_temp750; struct Cyc_Absyn_Exp* _temp752; struct Cyc_Absyn_Vardecl* _temp754;
struct Cyc_Absyn_Structdecl* _temp756; struct Cyc_List_List* _temp758; struct
Cyc_Core_Opt* _temp760; struct _tuple0* _temp762; struct Cyc_Absyn_Tunionfield*
_temp764; struct Cyc_Absyn_Tuniondecl* _temp766; struct Cyc_List_List* _temp768;
struct Cyc_Core_Opt* _temp770; struct Cyc_Core_Opt* _temp772; struct Cyc_Absyn_Enumfield*
_temp774; struct Cyc_Absyn_Enumdecl* _temp776; struct _tuple0* _temp778; void*
_temp780; struct Cyc_Absyn_Exp* _temp782; struct Cyc_List_List* _temp784; struct
Cyc_Core_Opt* _temp786; struct Cyc_Absyn_Stmt* _temp788; struct Cyc_Absyn_Fndecl*
_temp790; struct Cyc_Absyn_Exp* _temp792; _LL528: if(*(( int*) _temp526) == Cyc_Absyn_Const_e){
_LL617: _temp616=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp526)->f1; if((
unsigned int) _temp616 > 1u?*(( int*) _temp616) == Cyc_Absyn_Char_c: 0){ _LL621:
_temp620=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp616)->f1; goto _LL619;
_LL619: _temp618=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp616)->f2; goto
_LL529;} else{ goto _LL530;}} else{ goto _LL530;} _LL530: if(*(( int*) _temp526)
== Cyc_Absyn_Const_e){ _LL623: _temp622=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp526)->f1; if(( unsigned int) _temp622 > 1u?*(( int*) _temp622) == Cyc_Absyn_Short_c:
0){ _LL627: _temp626=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp622)->f1;
goto _LL625; _LL625: _temp624=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp622)->f2; goto _LL531;} else{ goto _LL532;}} else{ goto _LL532;} _LL532:
if(*(( int*) _temp526) == Cyc_Absyn_Const_e){ _LL629: _temp628=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp526)->f1; if(( unsigned int) _temp628 > 1u?*((
int*) _temp628) == Cyc_Absyn_Int_c: 0){ _LL633: _temp632=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp628)->f1; if( _temp632 ==( void*) Cyc_Absyn_Signed){ goto _LL631;} else{
goto _LL534;} _LL631: _temp630=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp628)->f2;
goto _LL533;} else{ goto _LL534;}} else{ goto _LL534;} _LL534: if(*(( int*)
_temp526) == Cyc_Absyn_Const_e){ _LL635: _temp634=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp526)->f1; if(( unsigned int) _temp634 > 1u?*(( int*) _temp634) == Cyc_Absyn_Int_c:
0){ _LL639: _temp638=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp634)->f1;
if( _temp638 ==( void*) Cyc_Absyn_Unsigned){ goto _LL637;} else{ goto _LL536;}
_LL637: _temp636=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp634)->f2; goto
_LL535;} else{ goto _LL536;}} else{ goto _LL536;} _LL536: if(*(( int*) _temp526)
== Cyc_Absyn_Const_e){ _LL641: _temp640=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp526)->f1; if(( unsigned int) _temp640 > 1u?*(( int*) _temp640) == Cyc_Absyn_LongLong_c:
0){ _LL645: _temp644=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp640)->f1;
goto _LL643; _LL643: _temp642=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp640)->f2; goto _LL537;} else{ goto _LL538;}} else{ goto _LL538;} _LL538:
if(*(( int*) _temp526) == Cyc_Absyn_Const_e){ _LL647: _temp646=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp526)->f1; if(( unsigned int) _temp646 > 1u?*((
int*) _temp646) == Cyc_Absyn_Float_c: 0){ _LL649: _temp648=( struct
_tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp646)->f1; goto _LL539;}
else{ goto _LL540;}} else{ goto _LL540;} _LL540: if(*(( int*) _temp526) == Cyc_Absyn_Const_e){
_LL651: _temp650=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp526)->f1; if(
_temp650 ==( void*) Cyc_Absyn_Null_c){ goto _LL541;} else{ goto _LL542;}} else{
goto _LL542;} _LL542: if(*(( int*) _temp526) == Cyc_Absyn_Const_e){ _LL653:
_temp652=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp526)->f1; if((
unsigned int) _temp652 > 1u?*(( int*) _temp652) == Cyc_Absyn_String_c: 0){
_LL655: _temp654=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp652)->f1; goto _LL543;} else{ goto _LL544;}} else{ goto _LL544;} _LL544:
if(*(( int*) _temp526) == Cyc_Absyn_UnknownId_e){ _LL657: _temp656=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp526)->f1; goto _LL545;}
else{ goto _LL546;} _LL546: if(*(( int*) _temp526) == Cyc_Absyn_Var_e){ _LL661:
_temp660=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp526)->f1;
goto _LL659; _LL659: _temp658=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp526)->f2; goto _LL547;} else{ goto _LL548;} _LL548: if(*(( int*) _temp526)
== Cyc_Absyn_Primop_e){ _LL665: _temp664=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp526)->f1; goto _LL663; _LL663: _temp662=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp526)->f2; goto _LL549;} else{ goto _LL550;}
_LL550: if(*(( int*) _temp526) == Cyc_Absyn_AssignOp_e){ _LL671: _temp670=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp526)->f1;
goto _LL669; _LL669: _temp668=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp526)->f2; goto _LL667; _LL667: _temp666=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp526)->f3; goto _LL551;} else{ goto _LL552;}
_LL552: if(*(( int*) _temp526) == Cyc_Absyn_Increment_e){ _LL675: _temp674=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp526)->f1;
goto _LL673; _LL673: _temp672=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f2; if( _temp672 ==( void*) Cyc_Absyn_PreInc){ goto _LL553;} else{
goto _LL554;}} else{ goto _LL554;} _LL554: if(*(( int*) _temp526) == Cyc_Absyn_Increment_e){
_LL679: _temp678=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f1; goto _LL677; _LL677: _temp676=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f2; if( _temp676 ==( void*) Cyc_Absyn_PreDec){ goto _LL555;} else{
goto _LL556;}} else{ goto _LL556;} _LL556: if(*(( int*) _temp526) == Cyc_Absyn_Increment_e){
_LL683: _temp682=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f1; goto _LL681; _LL681: _temp680=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f2; if( _temp680 ==( void*) Cyc_Absyn_PostInc){ goto _LL557;} else{
goto _LL558;}} else{ goto _LL558;} _LL558: if(*(( int*) _temp526) == Cyc_Absyn_Increment_e){
_LL687: _temp686=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f1; goto _LL685; _LL685: _temp684=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp526)->f2; if( _temp684 ==( void*) Cyc_Absyn_PostDec){ goto _LL559;} else{
goto _LL560;}} else{ goto _LL560;} _LL560: if(*(( int*) _temp526) == Cyc_Absyn_Conditional_e){
_LL693: _temp692=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp526)->f1; goto _LL691; _LL691: _temp690=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp526)->f2; goto _LL689; _LL689: _temp688=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp526)->f3;
goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp526) == Cyc_Absyn_SeqExp_e){
_LL697: _temp696=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp526)->f1; goto _LL695; _LL695: _temp694=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp526)->f2; goto _LL563;} else{ goto _LL564;}
_LL564: if(*(( int*) _temp526) == Cyc_Absyn_UnknownCall_e){ _LL701: _temp700=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp526)->f1;
goto _LL699; _LL699: _temp698=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp526)->f2; goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp526)
== Cyc_Absyn_FnCall_e){ _LL705: _temp704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp526)->f1; goto _LL703; _LL703: _temp702=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp526)->f2; goto _LL567;} else{ goto _LL568;}
_LL568: if(*(( int*) _temp526) == Cyc_Absyn_Throw_e){ _LL707: _temp706=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp526)->f1; goto _LL569;}
else{ goto _LL570;} _LL570: if(*(( int*) _temp526) == Cyc_Absyn_NoInstantiate_e){
_LL709: _temp708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp526)->f1; goto _LL571;} else{ goto _LL572;} _LL572: if(*(( int*) _temp526)
== Cyc_Absyn_Instantiate_e){ _LL713: _temp712=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp526)->f1; goto _LL711; _LL711: _temp710=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp526)->f2;
goto _LL573;} else{ goto _LL574;} _LL574: if(*(( int*) _temp526) == Cyc_Absyn_Cast_e){
_LL717: _temp716=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp526)->f1; goto
_LL715; _LL715: _temp714=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp526)->f2; goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp526)
== Cyc_Absyn_Address_e){ _LL719: _temp718=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp526)->f1; goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp526)
== Cyc_Absyn_New_e){ _LL723: _temp722=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp526)->f1; goto _LL721; _LL721: _temp720=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp526)->f2; goto _LL579;} else{ goto _LL580;} _LL580:
if(*(( int*) _temp526) == Cyc_Absyn_Sizeoftyp_e){ _LL725: _temp724=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp526)->f1; goto _LL581;} else{ goto
_LL582;} _LL582: if(*(( int*) _temp526) == Cyc_Absyn_Sizeofexp_e){ _LL727:
_temp726=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp526)->f1; goto _LL583;} else{ goto _LL584;} _LL584: if(*(( int*) _temp526)
== Cyc_Absyn_Deref_e){ _LL729: _temp728=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp526)->f1; goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp526)
== Cyc_Absyn_StructMember_e){ _LL733: _temp732=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp526)->f1; goto _LL731; _LL731: _temp730=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp526)->f2;
goto _LL587;} else{ goto _LL588;} _LL588: if(*(( int*) _temp526) == Cyc_Absyn_StructArrow_e){
_LL737: _temp736=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp526)->f1; goto _LL735; _LL735: _temp734=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp526)->f2; goto _LL589;} else{ goto _LL590;}
_LL590: if(*(( int*) _temp526) == Cyc_Absyn_Subscript_e){ _LL741: _temp740=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp526)->f1;
goto _LL739; _LL739: _temp738=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp526)->f2; goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp526)
== Cyc_Absyn_Tuple_e){ _LL743: _temp742=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp526)->f1; goto _LL593;} else{ goto _LL594;} _LL594: if(*(( int*) _temp526)
== Cyc_Absyn_CompoundLit_e){ _LL747: _temp746=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp526)->f1; goto _LL745; _LL745: _temp744=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp526)->f2; goto _LL595;} else{ goto _LL596;}
_LL596: if(*(( int*) _temp526) == Cyc_Absyn_Array_e){ _LL749: _temp748=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp526)->f1; goto _LL597;}
else{ goto _LL598;} _LL598: if(*(( int*) _temp526) == Cyc_Absyn_Comprehension_e){
_LL755: _temp754=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp526)->f1; goto _LL753; _LL753: _temp752=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp526)->f2; goto _LL751; _LL751: _temp750=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp526)->f3;
goto _LL599;} else{ goto _LL600;} _LL600: if(*(( int*) _temp526) == Cyc_Absyn_Struct_e){
_LL763: _temp762=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp526)->f1; goto _LL761; _LL761: _temp760=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp526)->f2; goto _LL759; _LL759: _temp758=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp526)->f3; goto
_LL757; _LL757: _temp756=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp526)->f4; goto _LL601;} else{ goto _LL602;} _LL602: if(*(( int*) _temp526)
== Cyc_Absyn_Tunion_e){ _LL773: _temp772=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp526)->f1; goto _LL771; _LL771: _temp770=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp526)->f2; goto _LL769; _LL769: _temp768=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp526)->f3; goto
_LL767; _LL767: _temp766=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp526)->f4; goto _LL765; _LL765: _temp764=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp526)->f5; goto _LL603;} else{ goto
_LL604;} _LL604: if(*(( int*) _temp526) == Cyc_Absyn_Enum_e){ _LL779: _temp778=(
struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*) _temp526)->f1; goto _LL777;
_LL777: _temp776=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp526)->f2; goto _LL775; _LL775: _temp774=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp526)->f3; goto _LL605;} else{ goto _LL606;}
_LL606: if(*(( int*) _temp526) == Cyc_Absyn_Malloc_e){ _LL783: _temp782=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp526)->f1; goto _LL781;
_LL781: _temp780=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp526)->f2;
goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp526) == Cyc_Absyn_UnresolvedMem_e){
_LL787: _temp786=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp526)->f1; goto _LL785; _LL785: _temp784=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp526)->f2; goto _LL609;} else{ goto
_LL610;} _LL610: if(*(( int*) _temp526) == Cyc_Absyn_StmtExp_e){ _LL789:
_temp788=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp526)->f1; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp526)
== Cyc_Absyn_Codegen_e){ _LL791: _temp790=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp526)->f1; goto _LL613;} else{ goto _LL614;}
_LL614: if(*(( int*) _temp526) == Cyc_Absyn_Fill_e){ _LL793: _temp792=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp526)->f1; goto _LL615;}
else{ goto _LL527;} _LL529: Cyc_Absyndump_dump_char(( int)'\''); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp618)); Cyc_Absyndump_dump_char(( int)'\''); goto
_LL527; _LL531: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp794=(
char*)"(short)"; struct _tagged_string _temp795; _temp795.curr= _temp794;
_temp795.base= _temp794; _temp795.last_plus_one= _temp794 + 8; _temp795;})); Cyc_Absyndump_dump_nospace(
Cyc_Core_string_of_int(( int) _temp624)); goto _LL527; _LL533: Cyc_Absyndump_dump(
Cyc_Core_string_of_int( _temp630)); goto _LL527; _LL535: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp636)); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp796=(
char*)"u"; struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base=
_temp796; _temp797.last_plus_one= _temp796 + 2; _temp797;})); goto _LL527;
_LL537: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp798=( char*)"<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp799; _temp799.curr= _temp798; _temp799.base= _temp798;
_temp799.last_plus_one= _temp798 + 27; _temp799;})); goto _LL527; _LL539: Cyc_Absyndump_dump(
_temp648); goto _LL527; _LL541: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp800=( char*)"null"; struct _tagged_string _temp801; _temp801.curr=
_temp800; _temp801.base= _temp800; _temp801.last_plus_one= _temp800 + 5;
_temp801;})); goto _LL527; _LL543: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp654)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL527; _LL545: _temp660= _temp656; goto _LL547; _LL547: Cyc_Absyndump_dumpqvar(
_temp660); goto _LL527; _LL549: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp664); if( ! Cyc_Absyn_is_format_prim( _temp664)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp662)){ case 1: _LL802: if( _temp664 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp804= _temp662; if( _temp804 == 0){ _throw(
Null_Exception);} _temp804->hd;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp805=( char*)".size"; struct _tagged_string _temp806; _temp806.curr=
_temp805; _temp806.base= _temp805; _temp806.last_plus_one= _temp805 + 6;
_temp806;}));} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp807= _temp662; if(
_temp807 == 0){ _throw( Null_Exception);} _temp807->hd;}));} break; case 2:
_LL803: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp809= _temp662; if( _temp809 == 0){ _throw( Null_Exception);} _temp809->hd;}));
Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp811=({ struct Cyc_List_List* _temp810= _temp662; if(
_temp810 == 0){ _throw( Null_Exception);} _temp810->tl;}); if( _temp811 == 0){
_throw( Null_Exception);} _temp811->hd;})); break; default: _LL808:( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp813=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp813[ 0]=({ struct Cyc_Core_Failure_struct
_temp814; _temp814.tag= Cyc_Core_Failure; _temp814.f1=( struct _tagged_string)({
char* _temp815=( char*)"Absyndump -- Bad number of arguments to primop"; struct
_tagged_string _temp816; _temp816.curr= _temp815; _temp816.base= _temp815;
_temp816.last_plus_one= _temp815 + 47; _temp816;}); _temp814;}); _temp813;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp817=( char*)"("; struct _tagged_string _temp818;
_temp818.curr= _temp817; _temp818.base= _temp817; _temp818.last_plus_one=
_temp817 + 2; _temp818;})); Cyc_Absyndump_dumpexps_prec( 20, _temp662); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp819=( char*)")"; struct _tagged_string
_temp820; _temp820.curr= _temp819; _temp820.base= _temp819; _temp820.last_plus_one=
_temp819 + 2; _temp820;}));} goto _LL527;} _LL551: Cyc_Absyndump_dumpexp_prec(
myprec, _temp670); if( _temp668 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)({ struct Cyc_Core_Opt* _temp821= _temp668; if( _temp821 == 0){ _throw(
Null_Exception);} _temp821->v;})));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp822=( char*)"="; struct _tagged_string _temp823;
_temp823.curr= _temp822; _temp823.base= _temp822; _temp823.last_plus_one=
_temp822 + 2; _temp823;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp666); goto
_LL527; _LL553: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp824=(
char*)"++"; struct _tagged_string _temp825; _temp825.curr= _temp824; _temp825.base=
_temp824; _temp825.last_plus_one= _temp824 + 3; _temp825;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp674); goto _LL527; _LL555: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp826=( char*)"--"; struct _tagged_string _temp827;
_temp827.curr= _temp826; _temp827.base= _temp826; _temp827.last_plus_one=
_temp826 + 3; _temp827;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp678); goto
_LL527; _LL557: Cyc_Absyndump_dumpexp_prec( myprec, _temp682); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp828=( char*)"++"; struct _tagged_string
_temp829; _temp829.curr= _temp828; _temp829.base= _temp828; _temp829.last_plus_one=
_temp828 + 3; _temp829;})); goto _LL527; _LL559: Cyc_Absyndump_dumpexp_prec(
myprec, _temp686); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp830=(
char*)"--"; struct _tagged_string _temp831; _temp831.curr= _temp830; _temp831.base=
_temp830; _temp831.last_plus_one= _temp830 + 3; _temp831;})); goto _LL527;
_LL561: Cyc_Absyndump_dumpexp_prec( myprec, _temp692); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp690); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp688); goto _LL527; _LL563:
Cyc_Absyndump_dumpexp_prec( myprec, _temp696); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp694); goto _LL527; _LL565: _temp704=
_temp700; _temp702= _temp698; goto _LL567; _LL567: Cyc_Absyndump_dumpexp_prec(
myprec, _temp704); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp832=( char*)"("; struct _tagged_string _temp833; _temp833.curr= _temp832;
_temp833.base= _temp832; _temp833.last_plus_one= _temp832 + 2; _temp833;})); Cyc_Absyndump_dumpexps_prec(
20, _temp702); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp834=( char*)")"; struct _tagged_string _temp835; _temp835.curr= _temp834;
_temp835.base= _temp834; _temp835.last_plus_one= _temp834 + 2; _temp835;}));
goto _LL527; _LL569: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp836=( char*)"throw"; struct _tagged_string _temp837; _temp837.curr=
_temp836; _temp837.base= _temp836; _temp837.last_plus_one= _temp836 + 6;
_temp837;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp706); goto _LL527; _LL571:
_temp712= _temp708; goto _LL573; _LL573: Cyc_Absyndump_dumpexp_prec( inprec,
_temp712); goto _LL527; _LL575: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp716); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp714); goto _LL527; _LL577: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp718); goto _LL527; _LL579: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp838=( char*)"new "; struct _tagged_string _temp839;
_temp839.curr= _temp838; _temp839.base= _temp838; _temp839.last_plus_one=
_temp838 + 5; _temp839;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp720); goto
_LL527; _LL581: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp840=(
char*)"sizeof("; struct _tagged_string _temp841; _temp841.curr= _temp840;
_temp841.base= _temp840; _temp841.last_plus_one= _temp840 + 8; _temp841;})); Cyc_Absyndump_dumptyp(
_temp724); Cyc_Absyndump_dump_char(( int)')'); goto _LL527; _LL583: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp842=( char*)"sizeof("; struct _tagged_string
_temp843; _temp843.curr= _temp842; _temp843.base= _temp842; _temp843.last_plus_one=
_temp842 + 8; _temp843;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp726); Cyc_Absyndump_dump_char((
int)')'); goto _LL527; _LL585: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp728); goto _LL527; _LL587: Cyc_Absyndump_dumpexp_prec( myprec,
_temp732); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp730); goto _LL527; _LL589: Cyc_Absyndump_dumpexp_prec( myprec, _temp736);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp844=( char*)"->";
struct _tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 3; _temp845;})); Cyc_Absyndump_dump_nospace(*
_temp734); goto _LL527; _LL591: Cyc_Absyndump_dumpexp_prec( myprec, _temp740);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp738); Cyc_Absyndump_dump_char((
int)']'); goto _LL527; _LL593: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp846=( char*)"$("; struct _tagged_string _temp847; _temp847.curr=
_temp846; _temp847.base= _temp846; _temp847.last_plus_one= _temp846 + 3;
_temp847;})); Cyc_Absyndump_dumpexps_prec( 20, _temp742); Cyc_Absyndump_dump_char((
int)')'); goto _LL527; _LL595: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp746).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp744,( struct _tagged_string)({ char* _temp848=( char*)"{"; struct
_tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 2; _temp849;}),( struct _tagged_string)({
char* _temp850=( char*)"}"; struct _tagged_string _temp851; _temp851.curr=
_temp850; _temp851.base= _temp850; _temp851.last_plus_one= _temp850 + 2;
_temp851;}),( struct _tagged_string)({ char* _temp852=( char*)","; struct
_tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 2; _temp853;})); goto _LL527; _LL597:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp748,( struct _tagged_string)({ char* _temp854=( char*)"{";
struct _tagged_string _temp855; _temp855.curr= _temp854; _temp855.base= _temp854;
_temp855.last_plus_one= _temp854 + 2; _temp855;}),( struct _tagged_string)({
char* _temp856=( char*)"}"; struct _tagged_string _temp857; _temp857.curr=
_temp856; _temp857.base= _temp856; _temp857.last_plus_one= _temp856 + 2;
_temp857;}),( struct _tagged_string)({ char* _temp858=( char*)","; struct
_tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 2; _temp859;})); goto _LL527; _LL599: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp860=( char*)"new {for"; struct
_tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 9; _temp861;})); Cyc_Absyndump_dump_str((*
_temp754->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp752); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp750);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL527; _LL601: Cyc_Absyndump_dumpqvar(
_temp762);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp758,( struct
_tagged_string)({ char* _temp862=( char*)"{"; struct _tagged_string _temp863;
_temp863.curr= _temp862; _temp863.base= _temp862; _temp863.last_plus_one=
_temp862 + 2; _temp863;}),( struct _tagged_string)({ char* _temp864=( char*)"}";
struct _tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 2; _temp865;}),( struct _tagged_string)({
char* _temp866=( char*)","; struct _tagged_string _temp867; _temp867.curr=
_temp866; _temp867.base= _temp866; _temp867.last_plus_one= _temp866 + 2;
_temp867;})); goto _LL527; _LL603: Cyc_Absyndump_dumpqvar( _temp764->name); if(
_temp768 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpexp, _temp768,( struct
_tagged_string)({ char* _temp868=( char*)"("; struct _tagged_string _temp869;
_temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 2; _temp869;}),( struct _tagged_string)({ char* _temp870=( char*)")";
struct _tagged_string _temp871; _temp871.curr= _temp870; _temp871.base= _temp870;
_temp871.last_plus_one= _temp870 + 2; _temp871;}),( struct _tagged_string)({
char* _temp872=( char*)","; struct _tagged_string _temp873; _temp873.curr=
_temp872; _temp873.base= _temp872; _temp873.last_plus_one= _temp872 + 2;
_temp873;}));} goto _LL527; _LL605: Cyc_Absyndump_dumpqvar( _temp778); goto
_LL527; _LL607: if( _temp782 != 0){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp874=( char*)"rmalloc("; struct _tagged_string _temp875; _temp875.curr=
_temp874; _temp875.base= _temp874; _temp875.last_plus_one= _temp874 + 9;
_temp875;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp876= _temp782; if( _temp876 == 0){ _throw( Null_Exception);} _temp876;}));
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp877=( char*)","; struct
_tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 2; _temp878;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp879=( char*)"malloc("; struct _tagged_string
_temp880; _temp880.curr= _temp879; _temp880.base= _temp879; _temp880.last_plus_one=
_temp879 + 8; _temp880;}));} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp881=( char*)"sizeof("; struct _tagged_string _temp882; _temp882.curr=
_temp881; _temp882.base= _temp881; _temp882.last_plus_one= _temp881 + 8;
_temp882;})); Cyc_Absyndump_dumptyp( _temp780); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp883=( char*)"))"; struct _tagged_string _temp884;
_temp884.curr= _temp883; _temp884.base= _temp883; _temp884.last_plus_one=
_temp883 + 3; _temp884;})); goto _LL527; _LL609:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp784,( struct _tagged_string)({ char* _temp885=( char*)"{"; struct
_tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 2; _temp886;}),( struct _tagged_string)({
char* _temp887=( char*)"}"; struct _tagged_string _temp888; _temp888.curr=
_temp887; _temp888.base= _temp887; _temp888.last_plus_one= _temp887 + 2;
_temp888;}),( struct _tagged_string)({ char* _temp889=( char*)","; struct
_tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 2; _temp890;})); goto _LL527; _LL611: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp891=( char*)"({"; struct _tagged_string
_temp892; _temp892.curr= _temp891; _temp892.base= _temp891; _temp892.last_plus_one=
_temp891 + 3; _temp892;})); Cyc_Absyndump_dumpstmt( _temp788); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp893=( char*)"})"; struct _tagged_string
_temp894; _temp894.curr= _temp893; _temp894.base= _temp893; _temp894.last_plus_one=
_temp893 + 3; _temp894;})); goto _LL527; _LL613: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp895=( char*)"codegen("; struct _tagged_string
_temp896; _temp896.curr= _temp895; _temp896.base= _temp895; _temp896.last_plus_one=
_temp895 + 9; _temp896;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp897=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp897->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp898=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp898[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp899; _temp899.tag= Cyc_Absyn_Fn_d;
_temp899.f1= _temp790; _temp899;}); _temp898;})); _temp897->loc= e->loc;
_temp897;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp900=(
char*)")"; struct _tagged_string _temp901; _temp901.curr= _temp900; _temp901.base=
_temp900; _temp901.last_plus_one= _temp900 + 2; _temp901;})); goto _LL527;
_LL615: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp902=( char*)"fill(";
struct _tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 6; _temp903;})); Cyc_Absyndump_dumpexp(
_temp792); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp904=( char*)")";
struct _tagged_string _temp905; _temp905.curr= _temp904; _temp905.base= _temp904;
_temp905.last_plus_one= _temp904 + 2; _temp905;})); goto _LL527; _LL527:;} if(
inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}} void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec( 0, e);} void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List* scs){ for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp906= scs; if( _temp906 == 0){ _throw( Null_Exception);} _temp906->tl;})){
struct Cyc_Absyn_Switch_clause* c=( struct Cyc_Absyn_Switch_clause*)({ struct
Cyc_List_List* _temp916= scs; if( _temp916 == 0){ _throw( Null_Exception);}
_temp916->hd;}); if( c->where_clause == 0?( void*)( c->pattern)->r ==( void*)
Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp907=( char*)"default:"; struct _tagged_string _temp908; _temp908.curr=
_temp907; _temp908.base= _temp907; _temp908.last_plus_one= _temp907 + 9;
_temp908;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp909=( char*)"case"; struct _tagged_string _temp910; _temp910.curr= _temp909;
_temp910.base= _temp909; _temp910.last_plus_one= _temp909 + 5; _temp910;})); Cyc_Absyndump_dumppat(
c->pattern); if( c->where_clause != 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp911=( char*)"&&"; struct _tagged_string _temp912;
_temp912.curr= _temp911; _temp912.base= _temp911; _temp912.last_plus_one=
_temp911 + 3; _temp912;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp913= c->where_clause; if( _temp913 == 0){ _throw(
Null_Exception);} _temp913;}));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp914=( char*)":"; struct _tagged_string _temp915;
_temp915.curr= _temp914; _temp915.base= _temp914; _temp915.last_plus_one=
_temp914 + 2; _temp915;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp917=( void*) s->r; struct Cyc_Absyn_Exp*
_temp961; struct Cyc_Absyn_Stmt* _temp963; struct Cyc_Absyn_Stmt* _temp965;
struct Cyc_Absyn_Exp* _temp967; struct Cyc_Absyn_Exp* _temp969; struct Cyc_Absyn_Stmt*
_temp971; struct Cyc_Absyn_Stmt* _temp973; struct Cyc_Absyn_Exp* _temp975;
struct Cyc_Absyn_Stmt* _temp977; struct _tuple2 _temp979; struct Cyc_Absyn_Stmt*
_temp981; struct Cyc_Absyn_Exp* _temp983; struct Cyc_Absyn_Stmt* _temp985;
struct Cyc_Absyn_Stmt* _temp987; struct Cyc_Absyn_Stmt* _temp989; struct
_tagged_string* _temp991; struct Cyc_Absyn_Switch_clause** _temp993; struct Cyc_List_List*
_temp995; struct Cyc_Absyn_Switch_clause** _temp997; struct Cyc_List_List*
_temp999; struct Cyc_Absyn_Stmt* _temp1001; struct _tuple2 _temp1003; struct Cyc_Absyn_Stmt*
_temp1005; struct Cyc_Absyn_Exp* _temp1007; struct _tuple2 _temp1009; struct Cyc_Absyn_Stmt*
_temp1011; struct Cyc_Absyn_Exp* _temp1013; struct Cyc_Absyn_Exp* _temp1015;
struct Cyc_List_List* _temp1017; struct Cyc_Absyn_Exp* _temp1019; struct Cyc_Absyn_Stmt*
_temp1021; struct Cyc_Absyn_Decl* _temp1023; struct Cyc_Absyn_Stmt* _temp1025;
struct Cyc_Absyn_Stmt* _temp1027; struct Cyc_Absyn_Stmt* _temp1029; struct
_tagged_string* _temp1031; struct _tuple2 _temp1033; struct Cyc_Absyn_Stmt*
_temp1035; struct Cyc_Absyn_Exp* _temp1037; struct Cyc_Absyn_Stmt* _temp1039;
struct Cyc_List_List* _temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct Cyc_Absyn_Stmt*
_temp1045; struct Cyc_Absyn_Vardecl* _temp1047; struct Cyc_Absyn_Tvar* _temp1049;
_LL919: if( _temp917 ==( void*) Cyc_Absyn_Skip_s){ goto _LL920;} else{ goto
_LL921;} _LL921: if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Exp_s:
0){ _LL962: _temp961=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp917)->f1; goto _LL922;} else{ goto _LL923;} _LL923: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Seq_s: 0){ _LL966: _temp965=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp917)->f1; goto
_LL964; _LL964: _temp963=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp917)->f2; goto _LL924;} else{ goto _LL925;} _LL925: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Return_s: 0){ _LL968: _temp967=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp917)->f1; if(
_temp967 == 0){ goto _LL926;} else{ goto _LL927;}} else{ goto _LL927;} _LL927:
if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Return_s: 0){
_LL970: _temp969=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp917)->f1; goto _LL928;} else{ goto _LL929;} _LL929: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_IfThenElse_s: 0){ _LL976:
_temp975=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp917)->f1; goto _LL974; _LL974: _temp973=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp917)->f2; goto _LL972; _LL972: _temp971=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp917)->f3;
goto _LL930;} else{ goto _LL931;} _LL931: if(( unsigned int) _temp917 > 1u?*((
int*) _temp917) == Cyc_Absyn_While_s: 0){ _LL980: _temp979=( struct _tuple2)((
struct Cyc_Absyn_While_s_struct*) _temp917)->f1; _LL984: _temp983= _temp979.f1;
goto _LL982; _LL982: _temp981= _temp979.f2; goto _LL978; _LL978: _temp977=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp917)->f2; goto
_LL932;} else{ goto _LL933;} _LL933: if(( unsigned int) _temp917 > 1u?*(( int*)
_temp917) == Cyc_Absyn_Break_s: 0){ _LL986: _temp985=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp917)->f1; goto _LL934;} else{ goto _LL935;}
_LL935: if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Continue_s:
0){ _LL988: _temp987=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp917)->f1; goto _LL936;} else{ goto _LL937;} _LL937: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Goto_s: 0){ _LL992: _temp991=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp917)->f1; goto
_LL990; _LL990: _temp989=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp917)->f2; goto _LL938;} else{ goto _LL939;} _LL939: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Fallthru_s: 0){ _LL996: _temp995=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp917)->f1; if(
_temp995 == 0){ goto _LL994;} else{ goto _LL941;} _LL994: _temp993=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp917)->f2; goto _LL940;} else{ goto
_LL941;} _LL941: if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Fallthru_s:
0){ _LL1000: _temp999=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp917)->f1; goto _LL998; _LL998: _temp997=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp917)->f2; goto _LL942;} else{ goto
_LL943;} _LL943: if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_For_s:
0){ _LL1016: _temp1015=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp917)->f1; goto _LL1010; _LL1010: _temp1009=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp917)->f2; _LL1014: _temp1013= _temp1009.f1; goto _LL1012; _LL1012:
_temp1011= _temp1009.f2; goto _LL1004; _LL1004: _temp1003=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp917)->f3; _LL1008: _temp1007= _temp1003.f1;
goto _LL1006; _LL1006: _temp1005= _temp1003.f2; goto _LL1002; _LL1002: _temp1001=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp917)->f4; goto
_LL944;} else{ goto _LL945;} _LL945: if(( unsigned int) _temp917 > 1u?*(( int*)
_temp917) == Cyc_Absyn_Switch_s: 0){ _LL1020: _temp1019=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp917)->f1; goto _LL1018; _LL1018:
_temp1017=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp917)->f2;
goto _LL946;} else{ goto _LL947;} _LL947: if(( unsigned int) _temp917 > 1u?*((
int*) _temp917) == Cyc_Absyn_Decl_s: 0){ _LL1024: _temp1023=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp917)->f1; goto _LL1022; _LL1022: _temp1021=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp917)->f2; goto
_LL948;} else{ goto _LL949;} _LL949: if(( unsigned int) _temp917 > 1u?*(( int*)
_temp917) == Cyc_Absyn_Cut_s: 0){ _LL1026: _temp1025=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp917)->f1; goto _LL950;} else{ goto _LL951;}
_LL951: if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Splice_s:
0){ _LL1028: _temp1027=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp917)->f1; goto _LL952;} else{ goto _LL953;} _LL953: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Label_s: 0){ _LL1032: _temp1031=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp917)->f1; goto
_LL1030; _LL1030: _temp1029=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp917)->f2; goto _LL954;} else{ goto _LL955;} _LL955: if(( unsigned int)
_temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Do_s: 0){ _LL1040: _temp1039=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp917)->f1; goto
_LL1034; _LL1034: _temp1033=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp917)->f2; _LL1038: _temp1037= _temp1033.f1; goto _LL1036; _LL1036:
_temp1035= _temp1033.f2; goto _LL956;} else{ goto _LL957;} _LL957: if((
unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_TryCatch_s: 0){
_LL1044: _temp1043=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp917)->f1; goto _LL1042; _LL1042: _temp1041=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp917)->f2; goto _LL958;} else{ goto
_LL959;} _LL959: if(( unsigned int) _temp917 > 1u?*(( int*) _temp917) == Cyc_Absyn_Region_s:
0){ _LL1050: _temp1049=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp917)->f1; goto _LL1048; _LL1048: _temp1047=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp917)->f2; goto _LL1046; _LL1046:
_temp1045=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp917)->f3; goto _LL960;} else{ goto _LL918;} _LL920: Cyc_Absyndump_dump_semi();
goto _LL918; _LL922: Cyc_Absyndump_dumpexp( _temp961); Cyc_Absyndump_dump_semi();
goto _LL918; _LL924: if( Cyc_Absynpp_is_declaration( _temp965)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp965); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp965);} if( Cyc_Absynpp_is_declaration(
_temp963)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp963);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp963);}
goto _LL918; _LL926: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1051=( char*)"return;"; struct _tagged_string _temp1052; _temp1052.curr=
_temp1051; _temp1052.base= _temp1051; _temp1052.last_plus_one= _temp1051 + 8;
_temp1052;})); goto _LL918; _LL928: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1053=( char*)"return"; struct _tagged_string _temp1054; _temp1054.curr=
_temp1053; _temp1054.base= _temp1053; _temp1054.last_plus_one= _temp1053 + 7;
_temp1054;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1055= _temp969; if( _temp1055 == 0){ _throw( Null_Exception);} _temp1055;}));
Cyc_Absyndump_dump_semi(); goto _LL918; _LL930: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1056=( char*)"if("; struct _tagged_string _temp1057;
_temp1057.curr= _temp1056; _temp1057.base= _temp1056; _temp1057.last_plus_one=
_temp1056 + 4; _temp1057;})); Cyc_Absyndump_dumpexp( _temp975); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1058=( char*)"){"; struct _tagged_string
_temp1059; _temp1059.curr= _temp1058; _temp1059.base= _temp1058; _temp1059.last_plus_one=
_temp1058 + 3; _temp1059;})); Cyc_Absyndump_dumpstmt( _temp973); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp1060=( void*) _temp971->r; _LL1062: if( _temp1060 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1063;} else{ goto _LL1064;} _LL1064: goto _LL1065;
_LL1063: goto _LL1061; _LL1065: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1066=( char*)"else{"; struct _tagged_string _temp1067; _temp1067.curr=
_temp1066; _temp1067.base= _temp1066; _temp1067.last_plus_one= _temp1066 + 6;
_temp1067;})); Cyc_Absyndump_dumpstmt( _temp971); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1061; _LL1061:;} goto _LL918; _LL932: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1068=( char*)"while("; struct _tagged_string
_temp1069; _temp1069.curr= _temp1068; _temp1069.base= _temp1068; _temp1069.last_plus_one=
_temp1068 + 7; _temp1069;})); Cyc_Absyndump_dumpexp( _temp983); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1070=( char*)") {"; struct _tagged_string
_temp1071; _temp1071.curr= _temp1070; _temp1071.base= _temp1070; _temp1071.last_plus_one=
_temp1070 + 4; _temp1071;})); Cyc_Absyndump_dumpstmt( _temp977); Cyc_Absyndump_dump_char((
int)'}'); goto _LL918; _LL934: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1072=( char*)"break;"; struct _tagged_string _temp1073; _temp1073.curr=
_temp1072; _temp1073.base= _temp1072; _temp1073.last_plus_one= _temp1072 + 7;
_temp1073;})); goto _LL918; _LL936: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1074=( char*)"continue;"; struct _tagged_string _temp1075; _temp1075.curr=
_temp1074; _temp1075.base= _temp1074; _temp1075.last_plus_one= _temp1074 + 10;
_temp1075;})); goto _LL918; _LL938: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1076=( char*)"goto"; struct _tagged_string _temp1077; _temp1077.curr=
_temp1076; _temp1077.base= _temp1076; _temp1077.last_plus_one= _temp1076 + 5;
_temp1077;})); Cyc_Absyndump_dump_str( _temp991); Cyc_Absyndump_dump_semi();
goto _LL918; _LL940: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1078=( char*)"fallthru;"; struct _tagged_string _temp1079; _temp1079.curr=
_temp1078; _temp1079.base= _temp1078; _temp1079.last_plus_one= _temp1078 + 10;
_temp1079;})); goto _LL918; _LL942: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1080=( char*)"fallthru("; struct _tagged_string _temp1081; _temp1081.curr=
_temp1080; _temp1081.base= _temp1080; _temp1081.last_plus_one= _temp1080 + 10;
_temp1081;})); Cyc_Absyndump_dumpexps_prec( 20, _temp999); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1082=( char*)");"; struct _tagged_string
_temp1083; _temp1083.curr= _temp1082; _temp1083.base= _temp1082; _temp1083.last_plus_one=
_temp1082 + 3; _temp1083;})); goto _LL918; _LL944: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1084=( char*)"for("; struct _tagged_string
_temp1085; _temp1085.curr= _temp1084; _temp1085.base= _temp1084; _temp1085.last_plus_one=
_temp1084 + 5; _temp1085;})); Cyc_Absyndump_dumpexp( _temp1015); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp1013); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1007); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1086=( char*)"){"; struct _tagged_string _temp1087; _temp1087.curr=
_temp1086; _temp1087.base= _temp1086; _temp1087.last_plus_one= _temp1086 + 3;
_temp1087;})); Cyc_Absyndump_dumpstmt( _temp1001); Cyc_Absyndump_dump_char(( int)'}');
goto _LL918; _LL946: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1088=( char*)"switch("; struct _tagged_string _temp1089; _temp1089.curr=
_temp1088; _temp1089.base= _temp1088; _temp1089.last_plus_one= _temp1088 + 8;
_temp1089;})); Cyc_Absyndump_dumpexp( _temp1019); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1090=( char*)"){"; struct _tagged_string
_temp1091; _temp1091.curr= _temp1090; _temp1091.base= _temp1090; _temp1091.last_plus_one=
_temp1090 + 3; _temp1091;})); Cyc_Absyndump_dumpswitchclauses( _temp1017); Cyc_Absyndump_dump_char((
int)'}'); goto _LL918; _LL948: Cyc_Absyndump_dumpdecl( _temp1023); Cyc_Absyndump_dumpstmt(
_temp1021); goto _LL918; _LL950: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1092=( char*)"cut"; struct _tagged_string _temp1093; _temp1093.curr=
_temp1092; _temp1093.base= _temp1092; _temp1093.last_plus_one= _temp1092 + 4;
_temp1093;})); Cyc_Absyndump_dumpstmt( _temp1025); goto _LL918; _LL952: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1094=( char*)"splice"; struct _tagged_string
_temp1095; _temp1095.curr= _temp1094; _temp1095.base= _temp1094; _temp1095.last_plus_one=
_temp1094 + 7; _temp1095;})); Cyc_Absyndump_dumpstmt( _temp1027); goto _LL918;
_LL954: if( Cyc_Absynpp_is_declaration( _temp1029)){ Cyc_Absyndump_dump_str(
_temp1031); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1096=( char*)": {"; struct _tagged_string _temp1097; _temp1097.curr=
_temp1096; _temp1097.base= _temp1096; _temp1097.last_plus_one= _temp1096 + 4;
_temp1097;})); Cyc_Absyndump_dumpstmt( _temp1029); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1031); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1029);} goto _LL918; _LL956: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1098=( char*)"do {"; struct _tagged_string
_temp1099; _temp1099.curr= _temp1098; _temp1099.base= _temp1098; _temp1099.last_plus_one=
_temp1098 + 5; _temp1099;})); Cyc_Absyndump_dumpstmt( _temp1039); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1100=( char*)"} while ("; struct
_tagged_string _temp1101; _temp1101.curr= _temp1100; _temp1101.base= _temp1100;
_temp1101.last_plus_one= _temp1100 + 10; _temp1101;})); Cyc_Absyndump_dumpexp(
_temp1037); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1102=( char*)");"; struct _tagged_string _temp1103; _temp1103.curr=
_temp1102; _temp1103.base= _temp1102; _temp1103.last_plus_one= _temp1102 + 3;
_temp1103;})); goto _LL918; _LL958: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1104=( char*)"try"; struct _tagged_string _temp1105; _temp1105.curr=
_temp1104; _temp1105.base= _temp1104; _temp1105.last_plus_one= _temp1104 + 4;
_temp1105;})); Cyc_Absyndump_dumpstmt( _temp1043); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1106=( char*)"catch {"; struct _tagged_string
_temp1107; _temp1107.curr= _temp1106; _temp1107.base= _temp1106; _temp1107.last_plus_one=
_temp1106 + 8; _temp1107;})); Cyc_Absyndump_dumpswitchclauses( _temp1041); Cyc_Absyndump_dump_char((
int)'}'); goto _LL918; _LL960: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1108=( char*)"region<"; struct _tagged_string _temp1109; _temp1109.curr=
_temp1108; _temp1109.base= _temp1108; _temp1109.last_plus_one= _temp1108 + 8;
_temp1109;})); Cyc_Absyndump_dumptvar( _temp1049); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1110=( char*)"> "; struct _tagged_string _temp1111;
_temp1111.curr= _temp1110; _temp1111.base= _temp1110; _temp1111.last_plus_one=
_temp1110 + 3; _temp1111;})); Cyc_Absyndump_dumpqvar( _temp1047->name); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1112=( char*)"{"; struct _tagged_string
_temp1113; _temp1113.curr= _temp1112; _temp1113.base= _temp1112; _temp1113.last_plus_one=
_temp1112 + 2; _temp1113;})); Cyc_Absyndump_dumpstmt( _temp1045); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1114=( char*)"}"; struct _tagged_string
_temp1115; _temp1115.curr= _temp1114; _temp1115.base= _temp1114; _temp1115.last_plus_one=
_temp1114 + 2; _temp1115;})); goto _LL918; _LL918:;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char*
_temp1116=( char*)""; struct _tagged_string _temp1117; _temp1117.curr= _temp1116;
_temp1117.base= _temp1116; _temp1117.last_plus_one= _temp1116 + 1; _temp1117;}),(
struct _tagged_string)({ char* _temp1118=( char*)"="; struct _tagged_string
_temp1119; _temp1119.curr= _temp1118; _temp1119.base= _temp1118; _temp1119.last_plus_one=
_temp1118 + 2; _temp1119;}),( struct _tagged_string)({ char* _temp1120=( char*)"=";
struct _tagged_string _temp1121; _temp1121.curr= _temp1120; _temp1121.base=
_temp1120; _temp1121.last_plus_one= _temp1120 + 2; _temp1121;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1122=(
void*) p->r; int _temp1156; void* _temp1158; int _temp1160; void* _temp1162;
char _temp1164; struct _tagged_string _temp1166; struct Cyc_Absyn_Vardecl*
_temp1168; struct Cyc_List_List* _temp1170; struct Cyc_Absyn_Pat* _temp1172;
struct Cyc_Absyn_Vardecl* _temp1174; struct _tuple0* _temp1176; struct Cyc_List_List*
_temp1178; struct Cyc_List_List* _temp1180; struct _tuple0* _temp1182; struct
Cyc_List_List* _temp1184; struct Cyc_List_List* _temp1186; struct _tuple0*
_temp1188; struct Cyc_List_List* _temp1190; struct Cyc_List_List* _temp1192;
struct Cyc_Core_Opt* _temp1194; struct Cyc_Absyn_Structdecl* _temp1196; struct
Cyc_List_List* _temp1198; struct Cyc_List_List* _temp1200; struct Cyc_Absyn_Tunionfield*
_temp1202; struct Cyc_Absyn_Tuniondecl* _temp1204; struct Cyc_Absyn_Enumfield*
_temp1206; struct Cyc_Absyn_Enumdecl* _temp1208; _LL1124: if( _temp1122 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1125;} else{ goto _LL1126;} _LL1126: if( _temp1122 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1127;} else{ goto _LL1128;} _LL1128: if((
unsigned int) _temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Int_p: 0){
_LL1159: _temp1158=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1122)->f1;
if( _temp1158 ==( void*) Cyc_Absyn_Signed){ goto _LL1157;} else{ goto _LL1130;}
_LL1157: _temp1156=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1122)->f2; goto
_LL1129;} else{ goto _LL1130;} _LL1130: if(( unsigned int) _temp1122 > 2u?*((
int*) _temp1122) == Cyc_Absyn_Int_p: 0){ _LL1163: _temp1162=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1122)->f1; if( _temp1162 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1161;} else{ goto _LL1132;} _LL1161: _temp1160=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1122)->f2; goto _LL1131;} else{ goto _LL1132;} _LL1132: if(( unsigned int)
_temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Char_p: 0){ _LL1165: _temp1164=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp1122)->f1; goto _LL1133;} else{
goto _LL1134;} _LL1134: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122)
== Cyc_Absyn_Float_p: 0){ _LL1167: _temp1166=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp1122)->f1; goto _LL1135;} else{ goto _LL1136;}
_LL1136: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Var_p:
0){ _LL1169: _temp1168=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1122)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: if(( unsigned int)
_temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Tuple_p: 0){ _LL1171: _temp1170=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1122)->f1; goto
_LL1139;} else{ goto _LL1140;} _LL1140: if(( unsigned int) _temp1122 > 2u?*((
int*) _temp1122) == Cyc_Absyn_Pointer_p: 0){ _LL1173: _temp1172=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1122)->f1; goto _LL1141;} else{ goto
_LL1142;} _LL1142: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Reference_p:
0){ _LL1175: _temp1174=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1122)->f1; goto _LL1143;} else{ goto _LL1144;} _LL1144: if(( unsigned int)
_temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_UnknownId_p: 0){ _LL1177:
_temp1176=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1122)->f1;
goto _LL1145;} else{ goto _LL1146;} _LL1146: if(( unsigned int) _temp1122 > 2u?*((
int*) _temp1122) == Cyc_Absyn_UnknownCall_p: 0){ _LL1183: _temp1182=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1122)->f1; goto _LL1181;
_LL1181: _temp1180=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1122)->f2; goto _LL1179; _LL1179: _temp1178=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1122)->f3; goto _LL1147;} else{
goto _LL1148;} _LL1148: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1189: _temp1188=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1122)->f1; goto _LL1187; _LL1187:
_temp1186=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1122)->f2; goto _LL1185; _LL1185: _temp1184=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1122)->f3; goto _LL1149;} else{
goto _LL1150;} _LL1150: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122)
== Cyc_Absyn_Struct_p: 0){ _LL1197: _temp1196=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1122)->f1; goto _LL1195; _LL1195:
_temp1194=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1122)->f2;
goto _LL1193; _LL1193: _temp1192=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1122)->f3; goto _LL1191; _LL1191: _temp1190=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1122)->f4; goto _LL1151;} else{ goto
_LL1152;} _LL1152: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Tunion_p:
0){ _LL1205: _temp1204=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1122)->f1; goto _LL1203; _LL1203: _temp1202=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1122)->f2; goto _LL1201; _LL1201:
_temp1200=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1122)->f3; goto _LL1199; _LL1199: _temp1198=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1122)->f4; goto _LL1153;} else{ goto
_LL1154;} _LL1154: if(( unsigned int) _temp1122 > 2u?*(( int*) _temp1122) == Cyc_Absyn_Enum_p:
0){ _LL1209: _temp1208=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1122)->f1; goto _LL1207; _LL1207: _temp1206=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1122)->f2; goto _LL1155;} else{ goto
_LL1123;} _LL1125: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1123; _LL1127:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1210=( char*)"null";
struct _tagged_string _temp1211; _temp1211.curr= _temp1210; _temp1211.base=
_temp1210; _temp1211.last_plus_one= _temp1210 + 5; _temp1211;})); goto _LL1123;
_LL1129: Cyc_Absyndump_dump( xprintf("%d", _temp1156)); goto _LL1123; _LL1131:
Cyc_Absyndump_dump( xprintf("%u",( unsigned int) _temp1160)); goto _LL1123;
_LL1133: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1212=( char*)"'";
struct _tagged_string _temp1213; _temp1213.curr= _temp1212; _temp1213.base=
_temp1212; _temp1213.last_plus_one= _temp1212 + 2; _temp1213;})); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp1164)); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1214=( char*)"'"; struct _tagged_string _temp1215;
_temp1215.curr= _temp1214; _temp1215.base= _temp1214; _temp1215.last_plus_one=
_temp1214 + 2; _temp1215;})); goto _LL1123; _LL1135: Cyc_Absyndump_dump(
_temp1166); goto _LL1123; _LL1137: Cyc_Absyndump_dumpqvar( _temp1168->name);
goto _LL1123; _LL1139:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1170,( struct
_tagged_string)({ char* _temp1216=( char*)"$("; struct _tagged_string _temp1217;
_temp1217.curr= _temp1216; _temp1217.base= _temp1216; _temp1217.last_plus_one=
_temp1216 + 3; _temp1217;}),( struct _tagged_string)({ char* _temp1218=( char*)")";
struct _tagged_string _temp1219; _temp1219.curr= _temp1218; _temp1219.base=
_temp1218; _temp1219.last_plus_one= _temp1218 + 2; _temp1219;}),( struct
_tagged_string)({ char* _temp1220=( char*)","; struct _tagged_string _temp1221;
_temp1221.curr= _temp1220; _temp1221.base= _temp1220; _temp1221.last_plus_one=
_temp1220 + 2; _temp1221;})); goto _LL1123; _LL1141: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1222=( char*)"&"; struct _tagged_string _temp1223;
_temp1223.curr= _temp1222; _temp1223.base= _temp1222; _temp1223.last_plus_one=
_temp1222 + 2; _temp1223;})); Cyc_Absyndump_dumppat( _temp1172); goto _LL1123;
_LL1143: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1224=( char*)"*";
struct _tagged_string _temp1225; _temp1225.curr= _temp1224; _temp1225.base=
_temp1224; _temp1225.last_plus_one= _temp1224 + 2; _temp1225;})); Cyc_Absyndump_dumpqvar(
_temp1174->name); goto _LL1123; _LL1145: Cyc_Absyndump_dumpqvar( _temp1176);
goto _LL1123; _LL1147: Cyc_Absyndump_dumpqvar( _temp1182); Cyc_Absyndump_dumptvars(
_temp1180);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1178,( struct
_tagged_string)({ char* _temp1226=( char*)"("; struct _tagged_string _temp1227;
_temp1227.curr= _temp1226; _temp1227.base= _temp1226; _temp1227.last_plus_one=
_temp1226 + 2; _temp1227;}),( struct _tagged_string)({ char* _temp1228=( char*)")";
struct _tagged_string _temp1229; _temp1229.curr= _temp1228; _temp1229.base=
_temp1228; _temp1229.last_plus_one= _temp1228 + 2; _temp1229;}),( struct
_tagged_string)({ char* _temp1230=( char*)","; struct _tagged_string _temp1231;
_temp1231.curr= _temp1230; _temp1231.base= _temp1230; _temp1231.last_plus_one=
_temp1230 + 2; _temp1231;})); goto _LL1123; _LL1149: Cyc_Absyndump_dumpqvar(
_temp1188); Cyc_Absyndump_dumptvars( _temp1186);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1184,( struct _tagged_string)({ char* _temp1232=( char*)"{"; struct
_tagged_string _temp1233; _temp1233.curr= _temp1232; _temp1233.base= _temp1232;
_temp1233.last_plus_one= _temp1232 + 2; _temp1233;}),( struct _tagged_string)({
char* _temp1234=( char*)"}"; struct _tagged_string _temp1235; _temp1235.curr=
_temp1234; _temp1235.base= _temp1234; _temp1235.last_plus_one= _temp1234 + 2;
_temp1235;}),( struct _tagged_string)({ char* _temp1236=( char*)","; struct
_tagged_string _temp1237; _temp1237.curr= _temp1236; _temp1237.base= _temp1236;
_temp1237.last_plus_one= _temp1236 + 2; _temp1237;})); goto _LL1123; _LL1151:
if( _temp1196->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp1238= _temp1196->name; if( _temp1238 == 0){ _throw(
Null_Exception);} _temp1238->v;}));} Cyc_Absyndump_dumptvars( _temp1192);(( void(*)(
void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp1190,( struct _tagged_string)({ char* _temp1239=(
char*)"{"; struct _tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base=
_temp1239; _temp1240.last_plus_one= _temp1239 + 2; _temp1240;}),( struct
_tagged_string)({ char* _temp1241=( char*)"}"; struct _tagged_string _temp1242;
_temp1242.curr= _temp1241; _temp1242.base= _temp1241; _temp1242.last_plus_one=
_temp1241 + 2; _temp1242;}),( struct _tagged_string)({ char* _temp1243=( char*)",";
struct _tagged_string _temp1244; _temp1244.curr= _temp1243; _temp1244.base=
_temp1243; _temp1244.last_plus_one= _temp1243 + 2; _temp1244;})); goto _LL1123;
_LL1153: Cyc_Absyndump_dumpqvar( _temp1202->name); Cyc_Absyndump_dumptvars(
_temp1200); if( _temp1198 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1198,( struct _tagged_string)({ char* _temp1245=( char*)"("; struct
_tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base= _temp1245;
_temp1246.last_plus_one= _temp1245 + 2; _temp1246;}),( struct _tagged_string)({
char* _temp1247=( char*)")"; struct _tagged_string _temp1248; _temp1248.curr=
_temp1247; _temp1248.base= _temp1247; _temp1248.last_plus_one= _temp1247 + 2;
_temp1248;}),( struct _tagged_string)({ char* _temp1249=( char*)","; struct
_tagged_string _temp1250; _temp1250.curr= _temp1249; _temp1250.base= _temp1249;
_temp1250.last_plus_one= _temp1249 + 2; _temp1250;}));} goto _LL1123; _LL1155:
Cyc_Absyndump_dumpqvar( _temp1206->name); goto _LL1123; _LL1123:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,( struct _tagged_string)({ char* _temp1251=(
char*)","; struct _tagged_string _temp1252; _temp1252.curr= _temp1251; _temp1252.base=
_temp1251; _temp1252.last_plus_one= _temp1251 + 2; _temp1252;}));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1253=( char*)" = ";
struct _tagged_string _temp1254; _temp1254.curr= _temp1253; _temp1254.base=
_temp1253; _temp1254.last_plus_one= _temp1253 + 4; _temp1254;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1255= ef->tag; if( _temp1255
== 0){ _throw( Null_Exception);} _temp1255;}));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield, fields,( struct _tagged_string)({ char* _temp1256=(
char*)","; struct _tagged_string _temp1257; _temp1257.curr= _temp1256; _temp1257.base=
_temp1256; _temp1257.last_plus_one= _temp1256 + 2; _temp1257;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1258= fields; if( _temp1258 == 0){ _throw( Null_Exception);} _temp1258->tl;})){
struct Cyc_Absyn_Structfield _temp1262; struct Cyc_List_List* _temp1263; struct
Cyc_Core_Opt* _temp1265; void* _temp1267; struct Cyc_Absyn_Tqual _temp1269;
struct _tagged_string* _temp1271; struct Cyc_Absyn_Structfield* _temp1260=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1259= fields; if(
_temp1259 == 0){ _throw( Null_Exception);} _temp1259->hd;}); _temp1262=*
_temp1260; _LL1272: _temp1271=( struct _tagged_string*) _temp1262.name; goto
_LL1270; _LL1270: _temp1269=( struct Cyc_Absyn_Tqual) _temp1262.tq; goto _LL1268;
_LL1268: _temp1267=( void*) _temp1262.type; goto _LL1266; _LL1266: _temp1265=(
struct Cyc_Core_Opt*) _temp1262.width; goto _LL1264; _LL1264: _temp1263=( struct
Cyc_List_List*) _temp1262.attributes; goto _LL1261; _LL1261:(( void(*)( struct
Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1269, _temp1267, Cyc_Absyndump_dump_str,
_temp1271); Cyc_Absyndump_dumpatts( _temp1263); if( _temp1265 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1273= _temp1265; if( _temp1273 == 0){ _throw( Null_Exception);} _temp1273->v;}));}
Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl*
td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars( td->tvs);}
static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1274=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1298; struct Cyc_Absyn_Structdecl* _temp1300; struct Cyc_Absyn_Uniondecl*
_temp1302; struct Cyc_Absyn_Vardecl* _temp1304; struct Cyc_Absyn_Vardecl
_temp1306; struct Cyc_List_List* _temp1307; struct Cyc_Core_Opt* _temp1309;
struct Cyc_Absyn_Exp* _temp1311; void* _temp1313; struct Cyc_Absyn_Tqual
_temp1315; struct _tuple0* _temp1317; void* _temp1319; struct Cyc_Absyn_Tuniondecl*
_temp1321; struct Cyc_Absyn_Tuniondecl _temp1323; int _temp1324; struct Cyc_Core_Opt*
_temp1326; struct Cyc_List_List* _temp1328; struct _tuple0* _temp1330; void*
_temp1332; struct Cyc_Absyn_Enumdecl* _temp1334; struct Cyc_Absyn_Enumdecl
_temp1336; struct Cyc_List_List* _temp1337; struct _tuple0* _temp1339; void*
_temp1341; int _temp1343; struct Cyc_Absyn_Exp* _temp1345; struct Cyc_Core_Opt*
_temp1347; struct Cyc_Core_Opt* _temp1349; struct Cyc_Absyn_Pat* _temp1351;
struct Cyc_Absyn_Typedefdecl* _temp1353; struct Cyc_List_List* _temp1355; struct
_tagged_string* _temp1357; struct Cyc_List_List* _temp1359; struct _tuple0*
_temp1361; struct Cyc_List_List* _temp1363; _LL1276: if(*(( int*) _temp1274) ==
Cyc_Absyn_Fn_d){ _LL1299: _temp1298=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1274)->f1; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*)
_temp1274) == Cyc_Absyn_Struct_d){ _LL1301: _temp1300=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1274)->f1; goto _LL1279;} else{ goto
_LL1280;} _LL1280: if(*(( int*) _temp1274) == Cyc_Absyn_Union_d){ _LL1303:
_temp1302=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1274)->f1; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1274) == Cyc_Absyn_Var_d){ _LL1305: _temp1304=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1274)->f1; _temp1306=* _temp1304; _LL1320:
_temp1319=( void*) _temp1306.sc; goto _LL1318; _LL1318: _temp1317=( struct
_tuple0*) _temp1306.name; goto _LL1316; _LL1316: _temp1315=( struct Cyc_Absyn_Tqual)
_temp1306.tq; goto _LL1314; _LL1314: _temp1313=( void*) _temp1306.type; goto
_LL1312; _LL1312: _temp1311=( struct Cyc_Absyn_Exp*) _temp1306.initializer; goto
_LL1310; _LL1310: _temp1309=( struct Cyc_Core_Opt*) _temp1306.rgn; goto _LL1308;
_LL1308: _temp1307=( struct Cyc_List_List*) _temp1306.attributes; goto _LL1283;}
else{ goto _LL1284;} _LL1284: if(*(( int*) _temp1274) == Cyc_Absyn_Tunion_d){
_LL1322: _temp1321=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1274)->f1; _temp1323=* _temp1321; _LL1333: _temp1332=( void*) _temp1323.sc;
goto _LL1331; _LL1331: _temp1330=( struct _tuple0*) _temp1323.name; goto _LL1329;
_LL1329: _temp1328=( struct Cyc_List_List*) _temp1323.tvs; goto _LL1327; _LL1327:
_temp1326=( struct Cyc_Core_Opt*) _temp1323.fields; goto _LL1325; _LL1325:
_temp1324=( int) _temp1323.is_xtunion; goto _LL1285;} else{ goto _LL1286;}
_LL1286: if(*(( int*) _temp1274) == Cyc_Absyn_Enum_d){ _LL1335: _temp1334=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1274)->f1;
_temp1336=* _temp1334; _LL1342: _temp1341=( void*) _temp1336.sc; goto _LL1340;
_LL1340: _temp1339=( struct _tuple0*) _temp1336.name; goto _LL1338; _LL1338:
_temp1337=( struct Cyc_List_List*) _temp1336.fields; goto _LL1287;} else{ goto
_LL1288;} _LL1288: if(*(( int*) _temp1274) == Cyc_Absyn_Let_d){ _LL1352:
_temp1351=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1274)->f1;
goto _LL1350; _LL1350: _temp1349=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1274)->f2; goto _LL1348; _LL1348: _temp1347=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1274)->f3; goto _LL1346; _LL1346: _temp1345=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1274)->f4; goto
_LL1344; _LL1344: _temp1343=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1274)->f5;
goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*(( int*) _temp1274) == Cyc_Absyn_Typedef_d){
_LL1354: _temp1353=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1274)->f1; goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*)
_temp1274) == Cyc_Absyn_Namespace_d){ _LL1358: _temp1357=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1274)->f1; goto _LL1356; _LL1356:
_temp1355=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1274)->f2; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*)
_temp1274) == Cyc_Absyn_Using_d){ _LL1362: _temp1361=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp1274)->f1; goto _LL1360; _LL1360: _temp1359=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1274)->f2; goto
_LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*) _temp1274) == Cyc_Absyn_ExternC_d){
_LL1364: _temp1363=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1274)->f1; goto _LL1297;} else{ goto _LL1275;} _LL1277: if( _temp1298->is_inline){
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1365=( char*)"inline";
struct _tagged_string _temp1366; _temp1366.curr= _temp1365; _temp1366.base=
_temp1365; _temp1366.last_plus_one= _temp1365 + 7; _temp1366;}));} Cyc_Absyndump_dumpscope((
void*) _temp1298->sc);{ void* t=( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1368=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1368[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1369; _temp1369.tag= Cyc_Absyn_FnType;
_temp1369.f1=({ struct Cyc_Absyn_FnInfo _temp1370; _temp1370.tvars= _temp1298->tvs;
_temp1370.effect= _temp1298->effect; _temp1370.ret_typ=( void*)(( void*)
_temp1298->ret_type); _temp1370.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1298->args); _temp1370.varargs= _temp1298->varargs; _temp1370.attributes= 0;
_temp1370;}); _temp1369;}); _temp1368;});(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1367; _temp1367.q_const= 0; _temp1367.q_volatile= 0;
_temp1367.q_restrict= 0; _temp1367;}), t, Cyc_Absyndump_dump_atts_qvar,
_temp1298); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1298->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1275;} _LL1279:
Cyc_Absyndump_dumpscope(( void*) _temp1300->sc); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1371=( char*)"struct"; struct _tagged_string
_temp1372; _temp1372.curr= _temp1371; _temp1372.base= _temp1371; _temp1372.last_plus_one=
_temp1371 + 7; _temp1372;})); if( _temp1300->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp1373= _temp1300->name; if(
_temp1373 == 0){ _throw( Null_Exception);} _temp1373->v;}));} Cyc_Absyndump_dumptvars(
_temp1300->tvs); if( _temp1300->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1374= _temp1300->fields; if( _temp1374 == 0){ _throw(
Null_Exception);} _temp1374->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1375=( char*)"}"; struct _tagged_string _temp1376; _temp1376.curr=
_temp1375; _temp1376.base= _temp1375; _temp1376.last_plus_one= _temp1375 + 2;
_temp1376;})); Cyc_Absyndump_dumpatts( _temp1300->attributes); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1377=( char*)";"; struct _tagged_string
_temp1378; _temp1378.curr= _temp1377; _temp1378.base= _temp1377; _temp1378.last_plus_one=
_temp1377 + 2; _temp1378;}));} goto _LL1275; _LL1281: Cyc_Absyndump_dumpscope((
void*) _temp1302->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1379=( char*)"union"; struct _tagged_string _temp1380; _temp1380.curr=
_temp1379; _temp1380.base= _temp1379; _temp1380.last_plus_one= _temp1379 + 6;
_temp1380;})); if( _temp1302->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)({ struct Cyc_Core_Opt* _temp1381= _temp1302->name; if( _temp1381 == 0){
_throw( Null_Exception);} _temp1381->v;}));} Cyc_Absyndump_dumptvars( _temp1302->tvs);
if( _temp1302->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1382= _temp1302->fields; if( _temp1382 == 0){ _throw( Null_Exception);}
_temp1382->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1383=(
char*)"}"; struct _tagged_string _temp1384; _temp1384.curr= _temp1383; _temp1384.base=
_temp1383; _temp1384.last_plus_one= _temp1383 + 2; _temp1384;})); Cyc_Absyndump_dumpatts(
_temp1302->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1385=( char*)";"; struct _tagged_string _temp1386; _temp1386.curr=
_temp1385; _temp1386.base= _temp1385; _temp1386.last_plus_one= _temp1385 + 2;
_temp1386;}));} goto _LL1275; _LL1283: Cyc_Absyndump_dumpscope( _temp1319);((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*), struct
_tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1315, _temp1313, Cyc_Absyndump_dumpqvar,
_temp1317); Cyc_Absyndump_dumpatts( _temp1307); if( _temp1311 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1387= _temp1311; if( _temp1387 == 0){ _throw( Null_Exception);} _temp1387;}));}
Cyc_Absyndump_dump_semi(); goto _LL1275; _LL1285: Cyc_Absyndump_dumpscope(
_temp1332); if( _temp1324){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1388=( char*)"xtunion "; struct _tagged_string _temp1389; _temp1389.curr=
_temp1388; _temp1389.base= _temp1388; _temp1389.last_plus_one= _temp1388 + 9;
_temp1389;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1390=( char*)"tunion "; struct _tagged_string _temp1391; _temp1391.curr=
_temp1390; _temp1391.base= _temp1390; _temp1391.last_plus_one= _temp1390 + 8;
_temp1391;}));} Cyc_Absyndump_dumpqvar( _temp1330); Cyc_Absyndump_dumptvars(
_temp1328); if( _temp1326 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1392= _temp1326; if( _temp1392 == 0){ _throw( Null_Exception);} _temp1392->v;}));
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1393=( char*)"};";
struct _tagged_string _temp1394; _temp1394.curr= _temp1393; _temp1394.base=
_temp1393; _temp1394.last_plus_one= _temp1393 + 3; _temp1394;}));} goto _LL1275;
_LL1287: Cyc_Absyndump_dumpscope( _temp1341); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1395=( char*)"enum "; struct _tagged_string
_temp1396; _temp1396.curr= _temp1395; _temp1396.base= _temp1395; _temp1396.last_plus_one=
_temp1395 + 6; _temp1396;})); Cyc_Absyndump_dumpqvar( _temp1339); Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1337); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1397=( char*)"};"; struct _tagged_string
_temp1398; _temp1398.curr= _temp1397; _temp1398.base= _temp1397; _temp1398.last_plus_one=
_temp1397 + 3; _temp1398;})); return; _LL1289: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1399=( char*)"let"; struct _tagged_string _temp1400;
_temp1400.curr= _temp1399; _temp1400.base= _temp1399; _temp1400.last_plus_one=
_temp1399 + 4; _temp1400;})); Cyc_Absyndump_dumppat( _temp1351); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1345); Cyc_Absyndump_dump_semi(); goto
_LL1275; _LL1291: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1401=(
char*)"typedef"; struct _tagged_string _temp1402; _temp1402.curr= _temp1401;
_temp1402.base= _temp1401; _temp1402.last_plus_one= _temp1401 + 8; _temp1402;}));((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1403; _temp1403.q_const= 0; _temp1403.q_volatile= 0; _temp1403.q_restrict=
0; _temp1403;}),( void*) _temp1353->defn, Cyc_Absyndump_dumptypedefname,
_temp1353); Cyc_Absyndump_dump_semi(); goto _LL1275; _LL1293: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1404=( char*)"namespace"; struct
_tagged_string _temp1405; _temp1405.curr= _temp1404; _temp1405.base= _temp1404;
_temp1405.last_plus_one= _temp1404 + 10; _temp1405;})); Cyc_Absyndump_dump_str(
_temp1357); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1355 != 0;
_temp1355=({ struct Cyc_List_List* _temp1406= _temp1355; if( _temp1406 == 0){
_throw( Null_Exception);} _temp1406->tl;})){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp1407= _temp1355; if( _temp1407 == 0){ _throw(
Null_Exception);} _temp1407->hd;}));} Cyc_Absyndump_dump_char(( int)'}'); goto
_LL1275; _LL1295: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1408=(
char*)"using"; struct _tagged_string _temp1409; _temp1409.curr= _temp1408;
_temp1409.base= _temp1408; _temp1409.last_plus_one= _temp1408 + 6; _temp1409;}));
Cyc_Absyndump_dumpqvar( _temp1361); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1359 != 0; _temp1359=({ struct Cyc_List_List* _temp1410= _temp1359; if(
_temp1410 == 0){ _throw( Null_Exception);} _temp1410->tl;})){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp1411= _temp1359; if(
_temp1411 == 0){ _throw( Null_Exception);} _temp1411->hd;}));} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1275; _LL1297: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1412=( char*)"extern \"C\" {"; struct _tagged_string _temp1413;
_temp1413.curr= _temp1412; _temp1413.base= _temp1412; _temp1413.last_plus_one=
_temp1412 + 13; _temp1413;})); for( 0; _temp1363 != 0; _temp1363=({ struct Cyc_List_List*
_temp1414= _temp1363; if( _temp1414 == 0){ _throw( Null_Exception);} _temp1414->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1415= _temp1363; if( _temp1415 == 0){ _throw( Null_Exception);} _temp1415->hd;}));}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL1275; _LL1275:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1417=( void*)({
struct Cyc_List_List* _temp1416= tms; if( _temp1416 == 0){ _throw(
Null_Exception);} _temp1416->hd;}); struct Cyc_Absyn_Tqual _temp1435; void*
_temp1437; void* _temp1439; struct Cyc_Absyn_Exp* _temp1441; struct Cyc_Absyn_Tqual
_temp1443; void* _temp1445; void* _temp1447; struct Cyc_Absyn_Exp* _temp1449;
struct Cyc_Absyn_Tqual _temp1451; void* _temp1453; void* _temp1455; struct Cyc_Absyn_Tqual
_temp1457; void* _temp1459; struct Cyc_Absyn_Tvar* _temp1461; void* _temp1463;
struct Cyc_Absyn_Exp* _temp1465; struct Cyc_Absyn_Tqual _temp1467; void*
_temp1469; struct Cyc_Absyn_Tvar* _temp1471; void* _temp1473; struct Cyc_Absyn_Exp*
_temp1475; struct Cyc_Absyn_Tqual _temp1477; void* _temp1479; struct Cyc_Absyn_Tvar*
_temp1481; void* _temp1483; struct Cyc_Absyn_Tqual _temp1485; void* _temp1487;
void* _temp1489; _LL1419: if(( unsigned int) _temp1417 > 1u?*(( int*) _temp1417)
== Cyc_Absyn_Pointer_mod: 0){ _LL1440: _temp1439=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1417)->f1; if(( unsigned int) _temp1439 > 1u?*(( int*) _temp1439) == Cyc_Absyn_Nullable_ps:
0){ _LL1442: _temp1441=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1439)->f1; goto _LL1438;} else{ goto _LL1421;} _LL1438: _temp1437=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f2; if( _temp1437 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL1436;} else{ goto _LL1421;} _LL1436: _temp1435=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f3;
goto _LL1420;} else{ goto _LL1421;} _LL1421: if(( unsigned int) _temp1417 > 1u?*((
int*) _temp1417) == Cyc_Absyn_Pointer_mod: 0){ _LL1448: _temp1447=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f1; if(( unsigned int)
_temp1447 > 1u?*(( int*) _temp1447) == Cyc_Absyn_NonNullable_ps: 0){ _LL1450:
_temp1449=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1447)->f1; goto _LL1446;} else{ goto _LL1423;} _LL1446: _temp1445=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f2; if( _temp1445 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL1444;} else{ goto _LL1423;} _LL1444: _temp1443=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f3;
goto _LL1422;} else{ goto _LL1423;} _LL1423: if(( unsigned int) _temp1417 > 1u?*((
int*) _temp1417) == Cyc_Absyn_Pointer_mod: 0){ _LL1456: _temp1455=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f1; if( _temp1455 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL1454;} else{ goto _LL1425;} _LL1454:
_temp1453=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f2; if(
_temp1453 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1452;} else{ goto _LL1425;}
_LL1452: _temp1451=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1417)->f3; goto _LL1424;} else{ goto _LL1425;} _LL1425: if(( unsigned int)
_temp1417 > 1u?*(( int*) _temp1417) == Cyc_Absyn_Pointer_mod: 0){ _LL1464:
_temp1463=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f1; if((
unsigned int) _temp1463 > 1u?*(( int*) _temp1463) == Cyc_Absyn_Nullable_ps: 0){
_LL1466: _temp1465=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1463)->f1; goto _LL1460;} else{ goto _LL1427;} _LL1460: _temp1459=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f2; if(( unsigned int)
_temp1459 > 4u?*(( int*) _temp1459) == Cyc_Absyn_VarType: 0){ _LL1462: _temp1461=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp1459)->f1; goto
_LL1458;} else{ goto _LL1427;} _LL1458: _temp1457=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f3; goto _LL1426;} else{ goto
_LL1427;} _LL1427: if(( unsigned int) _temp1417 > 1u?*(( int*) _temp1417) == Cyc_Absyn_Pointer_mod:
0){ _LL1474: _temp1473=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1417)->f1; if(( unsigned int) _temp1473 > 1u?*(( int*) _temp1473) == Cyc_Absyn_NonNullable_ps:
0){ _LL1476: _temp1475=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1473)->f1; goto _LL1470;} else{ goto _LL1429;} _LL1470: _temp1469=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f2; if(( unsigned int)
_temp1469 > 4u?*(( int*) _temp1469) == Cyc_Absyn_VarType: 0){ _LL1472: _temp1471=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp1469)->f1; goto
_LL1468;} else{ goto _LL1429;} _LL1468: _temp1467=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f3; goto _LL1428;} else{ goto
_LL1429;} _LL1429: if(( unsigned int) _temp1417 > 1u?*(( int*) _temp1417) == Cyc_Absyn_Pointer_mod:
0){ _LL1484: _temp1483=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1417)->f1; if( _temp1483 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1480;}
else{ goto _LL1431;} _LL1480: _temp1479=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1417)->f2; if(( unsigned int) _temp1479 > 4u?*(( int*) _temp1479) == Cyc_Absyn_VarType:
0){ _LL1482: _temp1481=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1479)->f1; goto _LL1478;} else{ goto _LL1431;} _LL1478: _temp1477=( struct
Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f3; goto
_LL1430;} else{ goto _LL1431;} _LL1431: if(( unsigned int) _temp1417 > 1u?*((
int*) _temp1417) == Cyc_Absyn_Pointer_mod: 0){ _LL1490: _temp1489=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f1; goto _LL1488; _LL1488:
_temp1487=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1417)->f2; goto
_LL1486; _LL1486: _temp1485=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1417)->f3; goto _LL1432;} else{ goto _LL1433;} _LL1433: goto _LL1434;
_LL1420: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1441);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1491= tms; if( _temp1491 ==
0){ _throw( Null_Exception);} _temp1491->tl;}), f, a); return; _LL1422: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1449);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1492= tms; if( _temp1492 == 0){ _throw( Null_Exception);} _temp1492->tl;}),
f, a); return; _LL1424: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct
Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({
struct Cyc_List_List* _temp1493= tms; if( _temp1493 == 0){ _throw(
Null_Exception);} _temp1493->tl;}), f, a); return; _LL1426: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1465); Cyc_Absyndump_dump_str(
_temp1461->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1494= tms; if(
_temp1494 == 0){ _throw( Null_Exception);} _temp1494->tl;}), f, a); return;
_LL1428: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1475); Cyc_Absyndump_dump_str( _temp1471->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1495= tms; if( _temp1495 == 0){ _throw( Null_Exception);} _temp1495->tl;}),
f, a); return; _LL1430: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1481->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1496= tms; if(
_temp1496 == 0){ _throw( Null_Exception);} _temp1496->tl;}), f, a); return;
_LL1432:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1497=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1497[ 0]=({ struct Cyc_Core_Impossible_struct _temp1498; _temp1498.tag= Cyc_Core_Impossible;
_temp1498.f1=( struct _tagged_string)({ char* _temp1499=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1500; _temp1500.curr= _temp1499; _temp1500.base=
_temp1499; _temp1500.last_plus_one= _temp1499 + 25; _temp1500;}); _temp1498;});
_temp1497;})); _LL1434: { int next_is_pointer= 0; if(({ struct Cyc_List_List*
_temp1501= tms; if( _temp1501 == 0){ _throw( Null_Exception);} _temp1501->tl;})
!= 0){ void* _temp1504=( void*)({ struct Cyc_List_List* _temp1503=({ struct Cyc_List_List*
_temp1502= tms; if( _temp1502 == 0){ _throw( Null_Exception);} _temp1502->tl;});
if( _temp1503 == 0){ _throw( Null_Exception);} _temp1503->hd;}); struct Cyc_Absyn_Tqual
_temp1510; void* _temp1512; void* _temp1514; _LL1506: if(( unsigned int)
_temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_Pointer_mod: 0){ _LL1515:
_temp1514=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1504)->f1; goto
_LL1513; _LL1513: _temp1512=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1504)->f2; goto _LL1511; _LL1511: _temp1510=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1504)->f3; goto _LL1507;} else{ goto
_LL1508;} _LL1508: goto _LL1509; _LL1507: next_is_pointer= 1; goto _LL1505;
_LL1509: goto _LL1505; _LL1505:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1516= tms; if( _temp1516 ==
0){ _throw( Null_Exception);} _temp1516->tl;}), f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1518=( void*)({ struct Cyc_List_List* _temp1517= tms; if(
_temp1517 == 0){ _throw( Null_Exception);} _temp1517->hd;}); struct Cyc_Absyn_Exp*
_temp1534; void* _temp1536; struct Cyc_Core_Opt* _temp1538; int _temp1540;
struct Cyc_List_List* _temp1542; void* _temp1544; struct Cyc_Position_Segment*
_temp1546; struct Cyc_List_List* _temp1548; int _temp1550; struct Cyc_Position_Segment*
_temp1552; struct Cyc_List_List* _temp1554; struct Cyc_List_List* _temp1556;
struct Cyc_Position_Segment* _temp1558; struct Cyc_Absyn_Tqual _temp1560; void*
_temp1562; void* _temp1564; _LL1520: if( _temp1518 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( unsigned int) _temp1518 > 1u?*((
int*) _temp1518) == Cyc_Absyn_ConstArray_mod: 0){ _LL1535: _temp1534=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1518)->f1; goto
_LL1523;} else{ goto _LL1524;} _LL1524: if(( unsigned int) _temp1518 > 1u?*((
int*) _temp1518) == Cyc_Absyn_Function_mod: 0){ _LL1537: _temp1536=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp1518)->f1; if(*(( int*) _temp1536)
== Cyc_Absyn_WithTypes){ _LL1543: _temp1542=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp1536)->f1; goto _LL1541; _LL1541: _temp1540=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp1536)->f2; goto _LL1539; _LL1539:
_temp1538=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1536)->f3; goto _LL1525;} else{ goto _LL1526;}} else{ goto _LL1526;}
_LL1526: if(( unsigned int) _temp1518 > 1u?*(( int*) _temp1518) == Cyc_Absyn_Function_mod:
0){ _LL1545: _temp1544=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1518)->f1; if(*(( int*) _temp1544) == Cyc_Absyn_NoTypes){ _LL1549:
_temp1548=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1544)->f1;
goto _LL1547; _LL1547: _temp1546=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1544)->f2; goto _LL1527;} else{ goto _LL1528;}} else{ goto _LL1528;}
_LL1528: if(( unsigned int) _temp1518 > 1u?*(( int*) _temp1518) == Cyc_Absyn_TypeParams_mod:
0){ _LL1555: _temp1554=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1518)->f1; goto _LL1553; _LL1553: _temp1552=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1518)->f2; goto _LL1551; _LL1551:
_temp1550=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1518)->f3; goto
_LL1529;} else{ goto _LL1530;} _LL1530: if(( unsigned int) _temp1518 > 1u?*((
int*) _temp1518) == Cyc_Absyn_Attributes_mod: 0){ _LL1559: _temp1558=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1518)->f1;
goto _LL1557; _LL1557: _temp1556=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1518)->f2; goto _LL1531;} else{ goto _LL1532;} _LL1532: if(( unsigned int)
_temp1518 > 1u?*(( int*) _temp1518) == Cyc_Absyn_Pointer_mod: 0){ _LL1565:
_temp1564=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1518)->f1; goto
_LL1563; _LL1563: _temp1562=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1518)->f2; goto _LL1561; _LL1561: _temp1560=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1518)->f3; goto _LL1533;} else{ goto
_LL1519;} _LL1521: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1566=(
char*)"[]"; struct _tagged_string _temp1567; _temp1567.curr= _temp1566;
_temp1567.base= _temp1566; _temp1567.last_plus_one= _temp1566 + 3; _temp1567;}));
goto _LL1519; _LL1523: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1534); Cyc_Absyndump_dump_char(( int)']'); goto _LL1519; _LL1525: Cyc_Absyndump_dumpfunargs(
_temp1542, _temp1540, _temp1538); goto _LL1519; _LL1527:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1548,( struct _tagged_string)({ char* _temp1568=( char*)"("; struct
_tagged_string _temp1569; _temp1569.curr= _temp1568; _temp1569.base= _temp1568;
_temp1569.last_plus_one= _temp1568 + 2; _temp1569;}),( struct _tagged_string)({
char* _temp1570=( char*)")"; struct _tagged_string _temp1571; _temp1571.curr=
_temp1570; _temp1571.base= _temp1570; _temp1571.last_plus_one= _temp1570 + 2;
_temp1571;}),( struct _tagged_string)({ char* _temp1572=( char*)","; struct
_tagged_string _temp1573; _temp1573.curr= _temp1572; _temp1573.base= _temp1572;
_temp1573.last_plus_one= _temp1572 + 2; _temp1573;})); goto _LL1519; _LL1529:
if( _temp1550){ Cyc_Absyndump_dumpkindedtvars( _temp1554);} else{ Cyc_Absyndump_dumptvars(
_temp1554);} goto _LL1519; _LL1531: Cyc_Absyndump_dumpatts( _temp1556); goto
_LL1519; _LL1533:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1574=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1574[ 0]=({ struct Cyc_Core_Impossible_struct _temp1575; _temp1575.tag= Cyc_Core_Impossible;
_temp1575.f1=( struct _tagged_string)({ char* _temp1576=( char*)"dumptms";
struct _tagged_string _temp1577; _temp1577.curr= _temp1576; _temp1577.base=
_temp1576; _temp1577.last_plus_one= _temp1576 + 8; _temp1577;}); _temp1575;});
_temp1574;})); _LL1519:;} return;} _LL1418:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1580; void* _temp1582; struct Cyc_Absyn_Tqual _temp1584; struct _tuple4
_temp1578= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual) tq, t); _LL1585:
_temp1584= _temp1578.f1; goto _LL1583; _LL1583: _temp1582= _temp1578.f2; goto
_LL1581; _LL1581: _temp1580= _temp1578.f3; goto _LL1579; _LL1579: Cyc_Absyndump_dumptq(
_temp1584); Cyc_Absyndump_dumpntyp( _temp1582);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1580), f, a);} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp1586= tdl; if( _temp1586 == 0){ _throw( Null_Exception);} _temp1586->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1587= tdl; if( _temp1587 == 0){ _throw( Null_Exception);} _temp1587->hd;}));}}