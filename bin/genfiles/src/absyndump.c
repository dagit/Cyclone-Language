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
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_Stdio_file_string_write(
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
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
void*), void*); extern void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields); typedef struct _tagged_string Cyc_Absyndump_dump_string_t; struct Cyc_Stdio___sFILE**
Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout; void Cyc_Absyndump_ignore( void* x){
return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp0= s;(
unsigned int)( _temp0.last_plus_one - _temp0.curr);}); if( !(( int)*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), sz - 1))){ -- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos
> 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);}
else{ Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp1= s;(
unsigned int)( _temp1.last_plus_one - _temp1.curr);}); if( !(( int)*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), sz - 1))){ -- sz;} Cyc_Absyndump_pos += sz; Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char( int c){ ++
Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str(
struct _tagged_string* s){ Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep( void(* f)(
void*), struct Cyc_List_List* l, struct _tagged_string sep){ if( l == 0){
return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0; l=(( struct
Cyc_List_List*) _check_null( l))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0;
l=(( struct Cyc_List_List*) _check_null( l))->tl){ f( env,( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_group( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_group_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end, struct
_tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace( end);}
void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ if(
l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* _temp2= 0; goto _LL3; _LL3:{ void* _temp4=(* v).f1; struct
Cyc_List_List* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List*
_temp20; struct Cyc_List_List* _temp22; _LL6: if( _temp4 ==( void*) Cyc_Absyn_Loc_n){
goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp4 > 1u?*(( int*)
_temp4) == Cyc_Absyn_Rel_n: 0){ _LL17: _temp16=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp4)->f1; if( _temp16 == 0){ goto _LL9;} else{ goto
_LL10;}} else{ goto _LL10;} _LL10: if(( unsigned int) _temp4 > 1u?*(( int*)
_temp4) == Cyc_Absyn_Abs_n: 0){ _LL19: _temp18=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp4)->f1; if( _temp18 == 0){ goto _LL11;} else{ goto
_LL12;}} else{ goto _LL12;} _LL12: if(( unsigned int) _temp4 > 1u?*(( int*)
_temp4) == Cyc_Absyn_Rel_n: 0){ _LL21: _temp20=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp4)->f1; goto _LL13;} else{ goto _LL14;} _LL14: if((
unsigned int) _temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Abs_n: 0){ _LL23:
_temp22=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4)->f1;
goto _LL15;} else{ goto _LL5;} _LL7: goto _LL9; _LL9: goto _LL11; _LL11: Cyc_Absyndump_dump_str((*
v).f2); return; _LL13: _temp22= _temp20; goto _LL15; _LL15: _temp2= _temp22;
goto _LL5; _LL5:;} Cyc_Absyndump_dump_str(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp2))->hd); for( _temp2=(( struct Cyc_List_List*) _check_null(
_temp2))->tl; _temp2 != 0; _temp2=(( struct Cyc_List_List*) _check_null( _temp2))->tl){
if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp24=( char*)"::"; struct _tagged_string
_temp25; _temp25.curr= _temp24; _temp25.base= _temp24; _temp25.last_plus_one=
_temp24 + 3; _temp25;}));} Cyc_Absyndump_dump_nospace(*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( _temp2))->hd));} if( Cyc_Absynpp_qvar_to_Cids){
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp26=( char*)"_";
struct _tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26;
_temp27.last_plus_one= _temp26 + 2; _temp27;}));} else{ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp28=( char*)"::"; struct _tagged_string
_temp29; _temp29.curr= _temp28; _temp29.base= _temp28; _temp29.last_plus_one=
_temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp30=( char*)"restrict"; struct _tagged_string
_temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 9; _temp31;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp32=( char*)"volatile"; struct _tagged_string
_temp33; _temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one=
_temp32 + 9; _temp33;}));} if( tq.q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp34=( char*)"const"; struct _tagged_string _temp35;
_temp35.curr= _temp34; _temp35.base= _temp34; _temp35.last_plus_one= _temp34 + 6;
_temp35;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp36= sc; _LL38:
if( _temp36 ==( void*) Cyc_Absyn_Static){ goto _LL39;} else{ goto _LL40;} _LL40:
if( _temp36 ==( void*) Cyc_Absyn_Public){ goto _LL41;} else{ goto _LL42;} _LL42:
if( _temp36 ==( void*) Cyc_Absyn_Extern){ goto _LL43;} else{ goto _LL44;} _LL44:
if( _temp36 ==( void*) Cyc_Absyn_ExternC){ goto _LL45;} else{ goto _LL46;} _LL46:
if( _temp36 ==( void*) Cyc_Absyn_Abstract){ goto _LL47;} else{ goto _LL37;}
_LL39: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp48=( char*)"static";
struct _tagged_string _temp49; _temp49.curr= _temp48; _temp49.base= _temp48;
_temp49.last_plus_one= _temp48 + 7; _temp49;})); return; _LL41: return; _LL43:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp50=( char*)"extern";
struct _tagged_string _temp51; _temp51.curr= _temp50; _temp51.base= _temp50;
_temp51.last_plus_one= _temp50 + 7; _temp51;})); return; _LL45: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp52=( char*)"extern \"C\""; struct
_tagged_string _temp53; _temp53.curr= _temp52; _temp53.base= _temp52; _temp53.last_plus_one=
_temp52 + 11; _temp53;})); return; _LL47: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp54=( char*)"abstract"; struct _tagged_string
_temp55; _temp55.curr= _temp54; _temp55.base= _temp54; _temp55.last_plus_one=
_temp54 + 9; _temp55;})); return; _LL37:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp56= k; _LL58: if( _temp56 ==( void*) Cyc_Absyn_AnyKind){ goto _LL59;}
else{ goto _LL60;} _LL60: if( _temp56 ==( void*) Cyc_Absyn_MemKind){ goto _LL61;}
else{ goto _LL62;} _LL62: if( _temp56 ==( void*) Cyc_Absyn_BoxKind){ goto _LL63;}
else{ goto _LL64;} _LL64: if( _temp56 ==( void*) Cyc_Absyn_RgnKind){ goto _LL65;}
else{ goto _LL66;} _LL66: if( _temp56 ==( void*) Cyc_Absyn_EffKind){ goto _LL67;}
else{ goto _LL57;} _LL59: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp68=( char*)"A"; struct _tagged_string _temp69; _temp69.curr= _temp68;
_temp69.base= _temp68; _temp69.last_plus_one= _temp68 + 2; _temp69;})); return;
_LL61: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp70=( char*)"M";
struct _tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70;
_temp71.last_plus_one= _temp70 + 2; _temp71;})); return; _LL63: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp72=( char*)"B"; struct _tagged_string
_temp73; _temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one=
_temp72 + 2; _temp73;})); return; _LL65: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp74=( char*)"R"; struct _tagged_string _temp75;
_temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one= _temp74 + 2;
_temp75;})); return; _LL67: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp76=( char*)"E"; struct _tagged_string _temp77; _temp77.curr= _temp76;
_temp77.base= _temp76; _temp77.last_plus_one= _temp76 + 2; _temp77;})); return;
_LL57:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp,
ts,( struct _tagged_string)({ char* _temp78=( char*)"<"; struct _tagged_string
_temp79; _temp79.curr= _temp78; _temp79.base= _temp78; _temp79.last_plus_one=
_temp78 + 2; _temp79;}),( struct _tagged_string)({ char* _temp80=( char*)">";
struct _tagged_string _temp81; _temp81.curr= _temp80; _temp81.base= _temp80;
_temp81.last_plus_one= _temp80 + 2; _temp81;}),( struct _tagged_string)({ char*
_temp82=( char*)","; struct _tagged_string _temp83; _temp83.curr= _temp82;
_temp83.base= _temp82; _temp83.last_plus_one= _temp82 + 2; _temp83;}));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp84=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp92; void* _temp94;
_LL86: if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) == Cyc_Absyn_Eq_constr:
0){ _LL93: _temp92=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp84)->f1;
if( _temp92 ==( void*) Cyc_Absyn_BoxKind){ goto _LL87;} else{ goto _LL88;}}
else{ goto _LL88;} _LL88: if(( unsigned int) _temp84 > 1u?*(( int*) _temp84) ==
Cyc_Absyn_Eq_constr: 0){ _LL95: _temp94=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp84)->f1; goto _LL89;} else{ goto _LL90;} _LL90: goto _LL91; _LL87: goto
_LL85; _LL89: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp96=( char*)"::";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 3; _temp97;})); Cyc_Absyndump_dumpkind( _temp94);
goto _LL85; _LL91: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp98=(
char*)"::?"; struct _tagged_string _temp99; _temp99.curr= _temp98; _temp99.base=
_temp98; _temp99.last_plus_one= _temp98 + 4; _temp99;})); goto _LL85; _LL85:;}}
void Cyc_Absyndump_dumptvars( struct Cyc_List_List* tvs){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({ char* _temp100=( char*)"<";
struct _tagged_string _temp101; _temp101.curr= _temp100; _temp101.base= _temp100;
_temp101.last_plus_one= _temp100 + 2; _temp101;}),( struct _tagged_string)({
char* _temp102=( char*)">"; struct _tagged_string _temp103; _temp103.curr=
_temp102; _temp103.base= _temp102; _temp103.last_plus_one= _temp102 + 2;
_temp103;}),( struct _tagged_string)({ char* _temp104=( char*)","; struct
_tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 2; _temp105;}));} void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
tvs,( struct _tagged_string)({ char* _temp106=( char*)"<"; struct _tagged_string
_temp107; _temp107.curr= _temp106; _temp107.base= _temp106; _temp107.last_plus_one=
_temp106 + 2; _temp107;}),( struct _tagged_string)({ char* _temp108=( char*)">";
struct _tagged_string _temp109; _temp109.curr= _temp108; _temp109.base= _temp108;
_temp109.last_plus_one= _temp108 + 2; _temp109;}),( struct _tagged_string)({
char* _temp110=( char*)","; struct _tagged_string _temp111; _temp111.curr=
_temp110; _temp111.base= _temp110; _temp111.last_plus_one= _temp110 + 2;
_temp111;}));} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void Cyc_Absyndump_dumparg(
struct _tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( int),
int)) Cyc_Absyndump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts,( struct _tagged_string)({ char* _temp112=( char*)"("; struct _tagged_string
_temp113; _temp113.curr= _temp112; _temp113.base= _temp112; _temp113.last_plus_one=
_temp112 + 2; _temp113;}),( struct _tagged_string)({ char* _temp114=( char*)")";
struct _tagged_string _temp115; _temp115.curr= _temp114; _temp115.base= _temp114;
_temp115.last_plus_one= _temp114 + 2; _temp115;}),( struct _tagged_string)({
char* _temp116=( char*)","; struct _tagged_string _temp117; _temp117.curr=
_temp116; _temp117.base= _temp116; _temp117.last_plus_one= _temp116 + 2;
_temp117;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp118=( char*)" __attribute__(("; struct _tagged_string _temp119; _temp119.curr=
_temp118; _temp119.base= _temp118; _temp119.last_plus_one= _temp118 + 17;
_temp119;})); for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));}} Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp122=(
char*)")) "; struct _tagged_string _temp123; _temp123.curr= _temp122; _temp123.base=
_temp122; _temp123.last_plus_one= _temp122 + 4; _temp123;}));} int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List* tms){ if( tms == 0){ return 0;}{ void* _temp124=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp130;
void* _temp132; void* _temp134; _LL126: if(( unsigned int) _temp124 > 1u?*(( int*)
_temp124) == Cyc_Absyn_Pointer_mod: 0){ _LL135: _temp134=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp124)->f1; goto _LL133; _LL133: _temp132=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp124)->f2; goto _LL131; _LL131: _temp130=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp124)->f3; goto _LL127;} else{ goto _LL128;}
_LL128: goto _LL129; _LL127: return 1; _LL129: return 0; _LL125:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp136= Cyc_Tcutil_compress( t); _LL138:
if( _temp136 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL139;} else{ goto _LL140;}
_LL140: goto _LL141; _LL139: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp142=( char*)"`H"; struct _tagged_string _temp143; _temp143.curr= _temp142;
_temp143.base= _temp142; _temp143.last_plus_one= _temp142 + 3; _temp143;}));
goto _LL137; _LL141: Cyc_Absyndump_dumpntyp( t); goto _LL137; _LL137:;} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List* regions= 0;
struct Cyc_List_List* effects= 0;{ void* _temp144= Cyc_Tcutil_compress( t); void*
_temp152; struct Cyc_List_List* _temp154; _LL146: if(( unsigned int) _temp144 >
4u?*(( int*) _temp144) == Cyc_Absyn_AccessEff: 0){ _LL153: _temp152=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp144)->f1; goto _LL147;} else{ goto
_LL148;} _LL148: if(( unsigned int) _temp144 > 4u?*(( int*) _temp144) == Cyc_Absyn_JoinEff:
0){ _LL155: _temp154=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp144)->f1; goto _LL149;} else{ goto _LL150;} _LL150: goto _LL151; _LL147:
regions=({ struct Cyc_List_List* _temp156=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp156->hd=( void*) _temp152; _temp156->tl=
regions; _temp156;}); goto _LL145; _LL149: for( 0; _temp154 != 0; _temp154=((
struct Cyc_List_List*) _check_null( _temp154))->tl){ struct Cyc_List_List*
_temp159; struct Cyc_List_List* _temp161; struct _tuple6 _temp157= Cyc_Absyndump_effects_split((
void*)(( struct Cyc_List_List*) _check_null( _temp154))->hd); _LL162: _temp161=
_temp157.f1; goto _LL160; _LL160: _temp159= _temp157.f2; goto _LL158; _LL158:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp161, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp159, effects);} goto _LL145; _LL151: effects=({ struct Cyc_List_List*
_temp163=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp163->hd=( void*) t; _temp163->tl= effects; _temp163;}); goto _LL145; _LL145:;}
return({ struct _tuple6 _temp164; _temp164.f1= regions; _temp164.f2= effects;
_temp164;});} static void Cyc_Absyndump_dumpeff( void* t){ struct Cyc_List_List*
_temp167; struct Cyc_List_List* _temp169; struct _tuple6 _temp165= Cyc_Absyndump_effects_split(
t); _LL170: _temp169= _temp165.f1; goto _LL168; _LL168: _temp167= _temp165.f2;
goto _LL166; _LL166: _temp169=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp169); _temp167=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp167); for( 0; _temp167 != 0; _temp167=((
struct Cyc_List_List*) _check_null( _temp167))->tl){ Cyc_Absyndump_dumpntyp((
void*)(( struct Cyc_List_List*) _check_null( _temp167))->hd); Cyc_Absyndump_dump_char((
int)'+');} Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp169 != 0; _temp169=((
struct Cyc_List_List*) _check_null( _temp169))->tl){ Cyc_Absyndump_dumprgn((
void*)(( struct Cyc_List_List*) _check_null( _temp169))->hd); if((( struct Cyc_List_List*)
_check_null( _temp169))->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char((
int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void* _temp171= t; struct Cyc_Absyn_Exp*
_temp237; struct Cyc_Absyn_Tqual _temp239; void* _temp241; struct Cyc_Absyn_FnInfo
_temp243; struct Cyc_Absyn_PtrInfo _temp245; struct Cyc_Absyn_Tvar* _temp247;
int _temp249; struct Cyc_Core_Opt* _temp251; void* _temp253; int _temp255;
struct Cyc_Core_Opt* _temp257; struct Cyc_Core_Opt _temp259; void* _temp260;
void* _temp262; struct Cyc_Absyn_TunionInfo _temp264; void* _temp266; struct Cyc_List_List*
_temp268; void* _temp270; struct Cyc_Absyn_TunionFieldInfo _temp272; struct Cyc_List_List*
_temp274; void* _temp276; struct Cyc_Absyn_Enumdecl* _temp278; struct _tuple0*
_temp280; void* _temp282; void* _temp284; void* _temp286; void* _temp288; void*
_temp290; void* _temp292; void* _temp294; void* _temp296; void* _temp298; void*
_temp300; void* _temp302; void* _temp304; void* _temp306; void* _temp308; void*
_temp310; void* _temp312; struct Cyc_List_List* _temp314; struct Cyc_Absyn_Structdecl**
_temp316; struct Cyc_List_List* _temp318; struct _tuple0* _temp320; struct Cyc_Absyn_Structdecl**
_temp322; struct Cyc_List_List* _temp324; struct _tuple0* _temp326; struct Cyc_Absyn_Uniondecl**
_temp328; struct Cyc_List_List* _temp330; struct _tuple0* _temp332; struct Cyc_Absyn_Uniondecl**
_temp334; struct Cyc_List_List* _temp336; struct _tuple0* _temp338; struct Cyc_List_List*
_temp340; struct Cyc_List_List* _temp342; struct Cyc_Core_Opt* _temp344; struct
Cyc_List_List* _temp346; struct _tuple0* _temp348; void* _temp350; void*
_temp352; struct Cyc_List_List* _temp354; _LL173: if(( unsigned int) _temp171 >
4u?*(( int*) _temp171) == Cyc_Absyn_ArrayType: 0){ _LL242: _temp241=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp171)->f1; goto _LL240; _LL240: _temp239=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp171)->f2;
goto _LL238; _LL238: _temp237=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp171)->f3; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_FnType: 0){ _LL244: _temp243=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp171)->f1; goto
_LL176;} else{ goto _LL177;} _LL177: if(( unsigned int) _temp171 > 4u?*(( int*)
_temp171) == Cyc_Absyn_PointerType: 0){ _LL246: _temp245=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp171)->f1; goto _LL178;} else{ goto
_LL179;} _LL179: if( _temp171 ==( void*) Cyc_Absyn_VoidType){ goto _LL180;}
else{ goto _LL181;} _LL181: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171)
== Cyc_Absyn_VarType: 0){ _LL248: _temp247=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp171)->f1; goto _LL182;} else{ goto _LL183;}
_LL183: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_Evar:
0){ _LL254: _temp253=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp171)->f1;
goto _LL252; _LL252: _temp251=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp171)->f2; if( _temp251 == 0){ goto _LL250;} else{ goto _LL185;} _LL250:
_temp249=( int)(( struct Cyc_Absyn_Evar_struct*) _temp171)->f3; goto _LL184;}
else{ goto _LL185;} _LL185: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171)
== Cyc_Absyn_Evar: 0){ _LL263: _temp262=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp171)->f1; goto _LL258; _LL258: _temp257=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp171)->f2; if( _temp257 == 0){ goto _LL187;} else{
_temp259=* _temp257; _LL261: _temp260=( void*) _temp259.v; goto _LL256;} _LL256:
_temp255=( int)(( struct Cyc_Absyn_Evar_struct*) _temp171)->f3; goto _LL186;}
else{ goto _LL187;} _LL187: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171)
== Cyc_Absyn_TunionType: 0){ _LL265: _temp264=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp171)->f1; _LL271: _temp270=( void*)
_temp264.tunion_info; goto _LL269; _LL269: _temp268=( struct Cyc_List_List*)
_temp264.targs; goto _LL267; _LL267: _temp266=( void*) _temp264.rgn; goto _LL188;}
else{ goto _LL189;} _LL189: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171)
== Cyc_Absyn_TunionFieldType: 0){ _LL273: _temp272=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp171)->f1; _LL277: _temp276=( void*)
_temp272.field_info; goto _LL275; _LL275: _temp274=( struct Cyc_List_List*)
_temp272.targs; goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_EnumType: 0){ _LL281: _temp280=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp171)->f1; goto _LL279;
_LL279: _temp278=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp171)->f2; goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL285: _temp284=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp284 ==( void*)
Cyc_Absyn_Signed){ goto _LL283;} else{ goto _LL195;} _LL283: _temp282=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp282 ==( void*) Cyc_Absyn_B1){
goto _LL194;} else{ goto _LL195;}} else{ goto _LL195;} _LL195: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL289: _temp288=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp288 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL287;} else{ goto _LL197;} _LL287: _temp286=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp286 ==( void*) Cyc_Absyn_B1){
goto _LL196;} else{ goto _LL197;}} else{ goto _LL197;} _LL197: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL293: _temp292=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp292 ==( void*)
Cyc_Absyn_Signed){ goto _LL291;} else{ goto _LL199;} _LL291: _temp290=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp290 ==( void*) Cyc_Absyn_B2){
goto _LL198;} else{ goto _LL199;}} else{ goto _LL199;} _LL199: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL297: _temp296=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp296 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL295;} else{ goto _LL201;} _LL295: _temp294=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp294 ==( void*) Cyc_Absyn_B2){
goto _LL200;} else{ goto _LL201;}} else{ goto _LL201;} _LL201: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL301: _temp300=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp300 ==( void*)
Cyc_Absyn_Signed){ goto _LL299;} else{ goto _LL203;} _LL299: _temp298=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp298 ==( void*) Cyc_Absyn_B4){
goto _LL202;} else{ goto _LL203;}} else{ goto _LL203;} _LL203: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL305: _temp304=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp304 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL303;} else{ goto _LL205;} _LL303: _temp302=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp302 ==( void*) Cyc_Absyn_B4){
goto _LL204;} else{ goto _LL205;}} else{ goto _LL205;} _LL205: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL309: _temp308=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp308 ==( void*)
Cyc_Absyn_Signed){ goto _LL307;} else{ goto _LL207;} _LL307: _temp306=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp306 ==( void*) Cyc_Absyn_B8){
goto _LL206;} else{ goto _LL207;}} else{ goto _LL207;} _LL207: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_IntType: 0){ _LL313: _temp312=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1; if( _temp312 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL311;} else{ goto _LL209;} _LL311: _temp310=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp171)->f2; if( _temp310 ==( void*) Cyc_Absyn_B8){
goto _LL208;} else{ goto _LL209;}} else{ goto _LL209;} _LL209: if( _temp171 ==(
void*) Cyc_Absyn_FloatType){ goto _LL210;} else{ goto _LL211;} _LL211: if(
_temp171 ==( void*) Cyc_Absyn_DoubleType){ goto _LL212;} else{ goto _LL213;}
_LL213: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_TupleType:
0){ _LL315: _temp314=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp171)->f1; goto _LL214;} else{ goto _LL215;} _LL215: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_StructType: 0){ _LL321: _temp320=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp171)->f1; if(
_temp320 == 0){ goto _LL319;} else{ goto _LL217;} _LL319: _temp318=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp171)->f2; goto _LL317; _LL317:
_temp316=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp171)->f3; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_StructType: 0){ _LL327: _temp326=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp171)->f1; goto
_LL325; _LL325: _temp324=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp171)->f2; goto _LL323; _LL323: _temp322=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp171)->f3; goto _LL218;} else{ goto
_LL219;} _LL219: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_UnionType:
0){ _LL333: _temp332=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp171)->f1; if( _temp332 == 0){ goto _LL331;} else{ goto _LL221;} _LL331:
_temp330=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp171)->f2;
goto _LL329; _LL329: _temp328=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp171)->f3; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_UnionType: 0){ _LL339: _temp338=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp171)->f1; goto
_LL337; _LL337: _temp336=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp171)->f2; goto _LL335; _LL335: _temp334=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp171)->f3; goto _LL222;} else{ goto
_LL223;} _LL223: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_AnonStructType:
0){ _LL341: _temp340=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp171)->f1; goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_AnonUnionType: 0){ _LL343:
_temp342=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp171)->f1; goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int)
_temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_TypedefType: 0){ _LL349: _temp348=(
struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp171)->f1; goto
_LL347; _LL347: _temp346=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp171)->f2; goto _LL345; _LL345: _temp344=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp171)->f3; goto _LL228;} else{ goto _LL229;}
_LL229: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_RgnHandleType:
0){ _LL351: _temp350=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp171)->f1;
goto _LL230;} else{ goto _LL231;} _LL231: if( _temp171 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int) _temp171 > 4u?*((
int*) _temp171) == Cyc_Absyn_AccessEff: 0){ _LL353: _temp352=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp171)->f1; goto _LL234;} else{ goto _LL235;}
_LL235: if(( unsigned int) _temp171 > 4u?*(( int*) _temp171) == Cyc_Absyn_JoinEff:
0){ _LL355: _temp354=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp171)->f1; goto _LL236;} else{ goto _LL172;} _LL174: return; _LL176: return;
_LL178: return; _LL180: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp356=( char*)"void"; struct _tagged_string _temp357; _temp357.curr= _temp356;
_temp357.base= _temp356; _temp357.last_plus_one= _temp356 + 5; _temp357;}));
return; _LL182: Cyc_Absyndump_dump_str( _temp247->name); return; _LL184: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp358=( char*)"%"; struct _tagged_string
_temp359; _temp359.curr= _temp358; _temp359.base= _temp358; _temp359.last_plus_one=
_temp358 + 2; _temp359;})); Cyc_Absyndump_dumpkind( _temp253); Cyc_Absyndump_dump(
xprintf("(%d)", _temp249)); return; _LL186: Cyc_Absyndump_dumpntyp( _temp260);
return; _LL188:{ void* _temp360= _temp270; struct Cyc_Absyn_UnknownTunionInfo
_temp366; int _temp368; struct _tuple0* _temp370; struct Cyc_Absyn_Tuniondecl*
_temp372; struct Cyc_Absyn_Tuniondecl _temp374; int _temp375; struct Cyc_Core_Opt*
_temp377; struct Cyc_List_List* _temp379; struct _tuple0* _temp381; void*
_temp383; _LL362: if(*(( int*) _temp360) == Cyc_Absyn_UnknownTunion){ _LL367:
_temp366=( struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp360)->f1; _LL371: _temp370=( struct _tuple0*) _temp366.name; goto _LL369;
_LL369: _temp368=( int) _temp366.is_xtunion; goto _LL363;} else{ goto _LL364;}
_LL364: if(*(( int*) _temp360) == Cyc_Absyn_KnownTunion){ _LL373: _temp372=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*) _temp360)->f1;
_temp374=* _temp372; _LL384: _temp383=( void*) _temp374.sc; goto _LL382; _LL382:
_temp381=( struct _tuple0*) _temp374.name; goto _LL380; _LL380: _temp379=(
struct Cyc_List_List*) _temp374.tvs; goto _LL378; _LL378: _temp377=( struct Cyc_Core_Opt*)
_temp374.fields; goto _LL376; _LL376: _temp375=( int) _temp374.is_xtunion; goto
_LL365;} else{ goto _LL361;} _LL363: _temp381= _temp370; _temp375= _temp368;
goto _LL365; _LL365: if( _temp375){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp385=( char*)"xtunion "; struct _tagged_string _temp386; _temp386.curr=
_temp385; _temp386.base= _temp385; _temp386.last_plus_one= _temp385 + 9;
_temp386;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp387=( char*)"tunion "; struct _tagged_string _temp388; _temp388.curr=
_temp387; _temp388.base= _temp387; _temp388.last_plus_one= _temp387 + 8;
_temp388;}));}{ void* _temp389= Cyc_Tcutil_compress( _temp266); _LL391: if(
_temp389 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL392;} else{ goto _LL393;} _LL393:
goto _LL394; _LL392: goto _LL390; _LL394: Cyc_Absyndump_dumptyp( _temp266); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp395=( char*)" "; struct _tagged_string
_temp396; _temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 2; _temp396;})); goto _LL390; _LL390:;} Cyc_Absyndump_dumpqvar(
_temp381); Cyc_Absyndump_dumptps( _temp268); goto _LL361; _LL361:;} goto _LL172;
_LL190:{ void* _temp397= _temp276; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp403; int _temp405; struct _tuple0* _temp407; struct _tuple0* _temp409;
struct Cyc_Absyn_Tunionfield* _temp411; struct Cyc_Absyn_Tunionfield _temp413;
void* _temp414; struct Cyc_Position_Segment* _temp416; struct Cyc_List_List*
_temp418; struct Cyc_List_List* _temp420; struct _tuple0* _temp422; struct Cyc_Absyn_Tuniondecl*
_temp424; struct Cyc_Absyn_Tuniondecl _temp426; int _temp427; struct Cyc_Core_Opt*
_temp429; struct Cyc_List_List* _temp431; struct _tuple0* _temp433; void*
_temp435; _LL399: if(*(( int*) _temp397) == Cyc_Absyn_UnknownTunionfield){
_LL404: _temp403=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp397)->f1; _LL410: _temp409=( struct _tuple0*) _temp403.tunion_name; goto
_LL408; _LL408: _temp407=( struct _tuple0*) _temp403.field_name; goto _LL406;
_LL406: _temp405=( int) _temp403.is_xtunion; goto _LL400;} else{ goto _LL401;}
_LL401: if(*(( int*) _temp397) == Cyc_Absyn_KnownTunionfield){ _LL425: _temp424=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp397)->f1; _temp426=* _temp424; _LL436: _temp435=( void*) _temp426.sc; goto
_LL434; _LL434: _temp433=( struct _tuple0*) _temp426.name; goto _LL432; _LL432:
_temp431=( struct Cyc_List_List*) _temp426.tvs; goto _LL430; _LL430: _temp429=(
struct Cyc_Core_Opt*) _temp426.fields; goto _LL428; _LL428: _temp427=( int)
_temp426.is_xtunion; goto _LL412; _LL412: _temp411=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp397)->f2; _temp413=* _temp411;
_LL423: _temp422=( struct _tuple0*) _temp413.name; goto _LL421; _LL421: _temp420=(
struct Cyc_List_List*) _temp413.tvs; goto _LL419; _LL419: _temp418=( struct Cyc_List_List*)
_temp413.typs; goto _LL417; _LL417: _temp416=( struct Cyc_Position_Segment*)
_temp413.loc; goto _LL415; _LL415: _temp414=( void*) _temp413.sc; goto _LL402;}
else{ goto _LL398;} _LL400: _temp433= _temp409; _temp427= _temp405; _temp422=
_temp407; goto _LL402; _LL402: if( _temp427){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp437=( char*)"xtunion "; struct _tagged_string
_temp438; _temp438.curr= _temp437; _temp438.base= _temp437; _temp438.last_plus_one=
_temp437 + 9; _temp438;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp439=( char*)"tunion "; struct _tagged_string _temp440; _temp440.curr=
_temp439; _temp440.base= _temp439; _temp440.last_plus_one= _temp439 + 8;
_temp440;}));} Cyc_Absyndump_dumpqvar( _temp433); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp441=( char*)"."; struct _tagged_string _temp442;
_temp442.curr= _temp441; _temp442.base= _temp441; _temp442.last_plus_one=
_temp441 + 2; _temp442;})); Cyc_Absyndump_dumpqvar( _temp422); Cyc_Absyndump_dumptps(
_temp274); goto _LL398; _LL398:;} goto _LL172; _LL192: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp443=( char*)"enum "; struct _tagged_string
_temp444; _temp444.curr= _temp443; _temp444.base= _temp443; _temp444.last_plus_one=
_temp443 + 6; _temp444;})); Cyc_Absyndump_dumpqvar( _temp280); return; _LL194:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp445=( char*)"signed char";
struct _tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 12; _temp446;})); return; _LL196: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp447=( char*)"char"; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 5; _temp448;})); return; _LL198: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp449=( char*)"short"; struct _tagged_string _temp450;
_temp450.curr= _temp449; _temp450.base= _temp449; _temp450.last_plus_one=
_temp449 + 6; _temp450;})); return; _LL200: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp451=( char*)"unsigned short"; struct _tagged_string
_temp452; _temp452.curr= _temp451; _temp452.base= _temp451; _temp452.last_plus_one=
_temp451 + 15; _temp452;})); return; _LL202: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp453=( char*)"int"; struct _tagged_string _temp454;
_temp454.curr= _temp453; _temp454.base= _temp453; _temp454.last_plus_one=
_temp453 + 4; _temp454;})); return; _LL204: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp455=( char*)"unsigned int"; struct _tagged_string
_temp456; _temp456.curr= _temp455; _temp456.base= _temp455; _temp456.last_plus_one=
_temp455 + 13; _temp456;})); return; _LL206: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp457=( char*)"long long"; struct _tagged_string
_temp458; _temp458.curr= _temp457; _temp458.base= _temp457; _temp458.last_plus_one=
_temp457 + 10; _temp458;})); return; _LL208: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp459=( char*)"unsigned long long"; struct
_tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 19; _temp460;})); return; _LL210: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp461=( char*)"float"; struct _tagged_string
_temp462; _temp462.curr= _temp461; _temp462.base= _temp461; _temp462.last_plus_one=
_temp461 + 6; _temp462;})); return; _LL212: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp463=( char*)"double"; struct _tagged_string
_temp464; _temp464.curr= _temp463; _temp464.base= _temp463; _temp464.last_plus_one=
_temp463 + 7; _temp464;})); return; _LL214: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp314); return; _LL216: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp465=( char*)"struct"; struct _tagged_string
_temp466; _temp466.curr= _temp465; _temp466.base= _temp465; _temp466.last_plus_one=
_temp465 + 7; _temp466;})); Cyc_Absyndump_dumptps( _temp318); return; _LL218:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp467=( char*)"struct";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 7; _temp468;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct _tuple0*) _check_null( _temp326))); Cyc_Absyndump_dumptps(
_temp324); return; _LL220: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp469=( char*)"union"; struct _tagged_string _temp470; _temp470.curr=
_temp469; _temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 6;
_temp470;})); Cyc_Absyndump_dumptps( _temp330); return; _LL222: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp471=( char*)"union"; struct _tagged_string
_temp472; _temp472.curr= _temp471; _temp472.base= _temp471; _temp472.last_plus_one=
_temp471 + 6; _temp472;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)(( struct
_tuple0*) _check_null( _temp338))); Cyc_Absyndump_dumptps( _temp336); return;
_LL224: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp473=( char*)"struct {";
struct _tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 9; _temp474;})); Cyc_Absyndump_dumpstructfields(
_temp340); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp475=( char*)"}";
struct _tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 2; _temp476;})); return; _LL226: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp477=( char*)"union {"; struct _tagged_string
_temp478; _temp478.curr= _temp477; _temp478.base= _temp477; _temp478.last_plus_one=
_temp477 + 8; _temp478;})); Cyc_Absyndump_dumpstructfields( _temp342); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp479=( char*)"}"; struct _tagged_string
_temp480; _temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 2; _temp480;})); return; _LL228:( Cyc_Absyndump_dumpqvar( _temp348),
Cyc_Absyndump_dumptps( _temp346)); return; _LL230: Cyc_Absyndump_dumprgn(
_temp350); return; _LL232: return; _LL234: return; _LL236: return; _LL172:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void
Cyc_Absyndump_dumpfunarg( struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((*
t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args=(( struct Cyc_List_List*) _check_null( args))->tl){
Cyc_Absyndump_dumpfunarg(( struct _tuple1*)(( struct Cyc_List_List*) _check_null(
args))->hd); if((( struct Cyc_List_List*) _check_null( args))->tl != 0? 1:
varargs){ Cyc_Absyndump_dump_char(( int)',');}} if( varargs){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp481=( char*)"..."; struct _tagged_string
_temp482; _temp482.curr= _temp481; _temp482.base= _temp481; _temp482.last_plus_one=
_temp481 + 4; _temp482;}));} if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*)(( struct Cyc_Core_Opt*) _check_null( effopt))->v);} Cyc_Absyndump_dump_char((
int)')');} void Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp483; _temp483.q_const= 0; _temp483.q_volatile= 0; _temp483.q_restrict= 0;
_temp483;}), t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp484= d; struct Cyc_Absyn_Exp* _temp490; struct
_tagged_string* _temp492; _LL486: if(*(( int*) _temp484) == Cyc_Absyn_ArrayElement){
_LL491: _temp490=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp484)->f1; goto _LL487;} else{ goto _LL488;} _LL488: if(*(( int*) _temp484)
== Cyc_Absyn_FieldName){ _LL493: _temp492=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp484)->f1; goto _LL489;} else{ goto _LL485;} _LL487: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp494=( char*)".["; struct _tagged_string
_temp495; _temp495.curr= _temp494; _temp495.base= _temp494; _temp495.last_plus_one=
_temp494 + 3; _temp495;})); Cyc_Absyndump_dumpexp( _temp490); Cyc_Absyndump_dump_char((
int)']'); goto _LL485; _LL489: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp492); goto _LL485; _LL485:;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp496=(
char*)""; struct _tagged_string _temp497; _temp497.curr= _temp496; _temp497.base=
_temp496; _temp497.last_plus_one= _temp496 + 1; _temp497;}),( struct
_tagged_string)({ char* _temp498=( char*)"="; struct _tagged_string _temp499;
_temp499.curr= _temp498; _temp499.base= _temp498; _temp499.last_plus_one=
_temp498 + 2; _temp499;}),( struct _tagged_string)({ char* _temp500=( char*)"=";
struct _tagged_string _temp501; _temp501.curr= _temp500; _temp501.base= _temp500;
_temp501.last_plus_one= _temp500 + 2; _temp501;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp502=(
char*)""; struct _tagged_string _temp503; _temp503.curr= _temp502; _temp503.base=
_temp502; _temp503.last_plus_one= _temp502 + 1; _temp503;}),( struct
_tagged_string)({ char* _temp504=( char*)""; struct _tagged_string _temp505;
_temp505.curr= _temp504; _temp505.base= _temp504; _temp505.last_plus_one=
_temp504 + 1; _temp505;}),( struct _tagged_string)({ char* _temp506=( char*)",";
struct _tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 2; _temp507;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp508=( char*)"("; struct _tagged_string _temp509; _temp509.curr= _temp508;
_temp509.base= _temp508; _temp509.last_plus_one= _temp508 + 2; _temp509;}));}{
void* _temp510=( void*) e->r; void* _temp602; char _temp604; void* _temp606;
void* _temp608; short _temp610; void* _temp612; void* _temp614; int _temp616;
void* _temp618; void* _temp620; int _temp622; void* _temp624; void* _temp626;
long long _temp628; void* _temp630; void* _temp632; struct _tagged_string
_temp634; void* _temp636; void* _temp638; struct _tagged_string _temp640; struct
_tuple0* _temp642; void* _temp644; struct _tuple0* _temp646; struct Cyc_List_List*
_temp648; void* _temp650; struct Cyc_Absyn_Exp* _temp652; struct Cyc_Core_Opt*
_temp654; struct Cyc_Absyn_Exp* _temp656; void* _temp658; struct Cyc_Absyn_Exp*
_temp660; void* _temp662; struct Cyc_Absyn_Exp* _temp664; void* _temp666; struct
Cyc_Absyn_Exp* _temp668; void* _temp670; struct Cyc_Absyn_Exp* _temp672; struct
Cyc_Absyn_Exp* _temp674; struct Cyc_Absyn_Exp* _temp676; struct Cyc_Absyn_Exp*
_temp678; struct Cyc_Absyn_Exp* _temp680; struct Cyc_Absyn_Exp* _temp682; struct
Cyc_List_List* _temp684; struct Cyc_Absyn_Exp* _temp686; struct Cyc_List_List*
_temp688; struct Cyc_Absyn_Exp* _temp690; struct Cyc_Absyn_Exp* _temp692; struct
Cyc_Absyn_Exp* _temp694; struct Cyc_List_List* _temp696; struct Cyc_Absyn_Exp*
_temp698; struct Cyc_Absyn_Exp* _temp700; void* _temp702; struct Cyc_Absyn_Exp*
_temp704; struct Cyc_Absyn_Exp* _temp706; struct Cyc_Absyn_Exp* _temp708; void*
_temp710; struct Cyc_Absyn_Exp* _temp712; struct Cyc_Absyn_Exp* _temp714; struct
_tagged_string* _temp716; struct Cyc_Absyn_Exp* _temp718; struct _tagged_string*
_temp720; struct Cyc_Absyn_Exp* _temp722; struct Cyc_Absyn_Exp* _temp724; struct
Cyc_Absyn_Exp* _temp726; struct Cyc_List_List* _temp728; struct Cyc_List_List*
_temp730; struct _tuple1* _temp732; struct Cyc_List_List* _temp734; struct Cyc_Absyn_Exp*
_temp736; struct Cyc_Absyn_Exp* _temp738; struct Cyc_Absyn_Vardecl* _temp740;
struct Cyc_Absyn_Structdecl* _temp742; struct Cyc_List_List* _temp744; struct
Cyc_Core_Opt* _temp746; struct _tuple0* _temp748; struct Cyc_List_List* _temp750;
void* _temp752; struct Cyc_Absyn_Tunionfield* _temp754; struct Cyc_Absyn_Tuniondecl*
_temp756; struct Cyc_List_List* _temp758; struct Cyc_Core_Opt* _temp760; struct
Cyc_Core_Opt* _temp762; struct Cyc_Absyn_Enumfield* _temp764; struct Cyc_Absyn_Enumdecl*
_temp766; struct _tuple0* _temp768; void* _temp770; struct Cyc_Absyn_Exp*
_temp772; struct Cyc_List_List* _temp774; struct Cyc_Core_Opt* _temp776; struct
Cyc_Absyn_Stmt* _temp778; struct Cyc_Absyn_Fndecl* _temp780; struct Cyc_Absyn_Exp*
_temp782; _LL512: if(*(( int*) _temp510) == Cyc_Absyn_Const_e){ _LL603: _temp602=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp510)->f1; if(( unsigned int)
_temp602 > 1u?*(( int*) _temp602) == Cyc_Absyn_Char_c: 0){ _LL607: _temp606=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp602)->f1; goto _LL605; _LL605:
_temp604=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp602)->f2; goto _LL513;}
else{ goto _LL514;}} else{ goto _LL514;} _LL514: if(*(( int*) _temp510) == Cyc_Absyn_Const_e){
_LL609: _temp608=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp510)->f1; if((
unsigned int) _temp608 > 1u?*(( int*) _temp608) == Cyc_Absyn_Short_c: 0){ _LL613:
_temp612=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp608)->f1; goto _LL611;
_LL611: _temp610=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp608)->f2;
goto _LL515;} else{ goto _LL516;}} else{ goto _LL516;} _LL516: if(*(( int*)
_temp510) == Cyc_Absyn_Const_e){ _LL615: _temp614=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp510)->f1; if(( unsigned int) _temp614 > 1u?*(( int*) _temp614) == Cyc_Absyn_Int_c:
0){ _LL619: _temp618=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp614)->f1;
if( _temp618 ==( void*) Cyc_Absyn_Signed){ goto _LL617;} else{ goto _LL518;}
_LL617: _temp616=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp614)->f2; goto
_LL517;} else{ goto _LL518;}} else{ goto _LL518;} _LL518: if(*(( int*) _temp510)
== Cyc_Absyn_Const_e){ _LL621: _temp620=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp510)->f1; if(( unsigned int) _temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Int_c:
0){ _LL625: _temp624=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp620)->f1;
if( _temp624 ==( void*) Cyc_Absyn_Unsigned){ goto _LL623;} else{ goto _LL520;}
_LL623: _temp622=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp620)->f2; goto
_LL519;} else{ goto _LL520;}} else{ goto _LL520;} _LL520: if(*(( int*) _temp510)
== Cyc_Absyn_Const_e){ _LL627: _temp626=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp510)->f1; if(( unsigned int) _temp626 > 1u?*(( int*) _temp626) == Cyc_Absyn_LongLong_c:
0){ _LL631: _temp630=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp626)->f1;
goto _LL629; _LL629: _temp628=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp626)->f2; goto _LL521;} else{ goto _LL522;}} else{ goto _LL522;} _LL522:
if(*(( int*) _temp510) == Cyc_Absyn_Const_e){ _LL633: _temp632=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp510)->f1; if(( unsigned int) _temp632 > 1u?*((
int*) _temp632) == Cyc_Absyn_Float_c: 0){ _LL635: _temp634=( struct
_tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp632)->f1; goto _LL523;}
else{ goto _LL524;}} else{ goto _LL524;} _LL524: if(*(( int*) _temp510) == Cyc_Absyn_Const_e){
_LL637: _temp636=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp510)->f1; if(
_temp636 ==( void*) Cyc_Absyn_Null_c){ goto _LL525;} else{ goto _LL526;}} else{
goto _LL526;} _LL526: if(*(( int*) _temp510) == Cyc_Absyn_Const_e){ _LL639:
_temp638=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp510)->f1; if((
unsigned int) _temp638 > 1u?*(( int*) _temp638) == Cyc_Absyn_String_c: 0){
_LL641: _temp640=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp638)->f1; goto _LL527;} else{ goto _LL528;}} else{ goto _LL528;} _LL528:
if(*(( int*) _temp510) == Cyc_Absyn_UnknownId_e){ _LL643: _temp642=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp510)->f1; goto _LL529;}
else{ goto _LL530;} _LL530: if(*(( int*) _temp510) == Cyc_Absyn_Var_e){ _LL647:
_temp646=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp510)->f1;
goto _LL645; _LL645: _temp644=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp510)->f2; goto _LL531;} else{ goto _LL532;} _LL532: if(*(( int*) _temp510)
== Cyc_Absyn_Primop_e){ _LL651: _temp650=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp510)->f1; goto _LL649; _LL649: _temp648=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp510)->f2; goto _LL533;} else{ goto _LL534;}
_LL534: if(*(( int*) _temp510) == Cyc_Absyn_AssignOp_e){ _LL657: _temp656=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp510)->f1;
goto _LL655; _LL655: _temp654=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp510)->f2; goto _LL653; _LL653: _temp652=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp510)->f3; goto _LL535;} else{ goto _LL536;}
_LL536: if(*(( int*) _temp510) == Cyc_Absyn_Increment_e){ _LL661: _temp660=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp510)->f1;
goto _LL659; _LL659: _temp658=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f2; if( _temp658 ==( void*) Cyc_Absyn_PreInc){ goto _LL537;} else{
goto _LL538;}} else{ goto _LL538;} _LL538: if(*(( int*) _temp510) == Cyc_Absyn_Increment_e){
_LL665: _temp664=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f1; goto _LL663; _LL663: _temp662=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f2; if( _temp662 ==( void*) Cyc_Absyn_PreDec){ goto _LL539;} else{
goto _LL540;}} else{ goto _LL540;} _LL540: if(*(( int*) _temp510) == Cyc_Absyn_Increment_e){
_LL669: _temp668=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f1; goto _LL667; _LL667: _temp666=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f2; if( _temp666 ==( void*) Cyc_Absyn_PostInc){ goto _LL541;} else{
goto _LL542;}} else{ goto _LL542;} _LL542: if(*(( int*) _temp510) == Cyc_Absyn_Increment_e){
_LL673: _temp672=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f1; goto _LL671; _LL671: _temp670=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp510)->f2; if( _temp670 ==( void*) Cyc_Absyn_PostDec){ goto _LL543;} else{
goto _LL544;}} else{ goto _LL544;} _LL544: if(*(( int*) _temp510) == Cyc_Absyn_Conditional_e){
_LL679: _temp678=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp510)->f1; goto _LL677; _LL677: _temp676=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp510)->f2; goto _LL675; _LL675: _temp674=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp510)->f3;
goto _LL545;} else{ goto _LL546;} _LL546: if(*(( int*) _temp510) == Cyc_Absyn_SeqExp_e){
_LL683: _temp682=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp510)->f1; goto _LL681; _LL681: _temp680=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp510)->f2; goto _LL547;} else{ goto _LL548;}
_LL548: if(*(( int*) _temp510) == Cyc_Absyn_UnknownCall_e){ _LL687: _temp686=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp510)->f1;
goto _LL685; _LL685: _temp684=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp510)->f2; goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp510)
== Cyc_Absyn_FnCall_e){ _LL691: _temp690=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp510)->f1; goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp510)->f2; goto _LL551;} else{ goto _LL552;}
_LL552: if(*(( int*) _temp510) == Cyc_Absyn_Throw_e){ _LL693: _temp692=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp510)->f1; goto _LL553;}
else{ goto _LL554;} _LL554: if(*(( int*) _temp510) == Cyc_Absyn_NoInstantiate_e){
_LL695: _temp694=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp510)->f1; goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp510)
== Cyc_Absyn_Instantiate_e){ _LL699: _temp698=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp510)->f1; goto _LL697; _LL697: _temp696=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp510)->f2;
goto _LL557;} else{ goto _LL558;} _LL558: if(*(( int*) _temp510) == Cyc_Absyn_Cast_e){
_LL703: _temp702=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp510)->f1; goto
_LL701; _LL701: _temp700=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp510)->f2; goto _LL559;} else{ goto _LL560;} _LL560: if(*(( int*) _temp510)
== Cyc_Absyn_Address_e){ _LL705: _temp704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp510)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp510)
== Cyc_Absyn_New_e){ _LL709: _temp708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp510)->f1; goto _LL707; _LL707: _temp706=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp510)->f2; goto _LL563;} else{ goto _LL564;} _LL564:
if(*(( int*) _temp510) == Cyc_Absyn_Sizeoftyp_e){ _LL711: _temp710=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp510)->f1; goto _LL565;} else{ goto
_LL566;} _LL566: if(*(( int*) _temp510) == Cyc_Absyn_Sizeofexp_e){ _LL713:
_temp712=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp510)->f1; goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp510)
== Cyc_Absyn_Deref_e){ _LL715: _temp714=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp510)->f1; goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp510)
== Cyc_Absyn_StructMember_e){ _LL719: _temp718=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp510)->f1; goto _LL717; _LL717: _temp716=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp510)->f2;
goto _LL571;} else{ goto _LL572;} _LL572: if(*(( int*) _temp510) == Cyc_Absyn_StructArrow_e){
_LL723: _temp722=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp510)->f1; goto _LL721; _LL721: _temp720=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp510)->f2; goto _LL573;} else{ goto _LL574;}
_LL574: if(*(( int*) _temp510) == Cyc_Absyn_Subscript_e){ _LL727: _temp726=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp510)->f1;
goto _LL725; _LL725: _temp724=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp510)->f2; goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp510)
== Cyc_Absyn_Tuple_e){ _LL729: _temp728=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp510)->f1; goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp510)
== Cyc_Absyn_CompoundLit_e){ _LL733: _temp732=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp510)->f1; goto _LL731; _LL731: _temp730=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp510)->f2; goto _LL579;} else{ goto _LL580;}
_LL580: if(*(( int*) _temp510) == Cyc_Absyn_Array_e){ _LL735: _temp734=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp510)->f1; goto _LL581;}
else{ goto _LL582;} _LL582: if(*(( int*) _temp510) == Cyc_Absyn_Comprehension_e){
_LL741: _temp740=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp510)->f1; goto _LL739; _LL739: _temp738=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp510)->f2; goto _LL737; _LL737: _temp736=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp510)->f3;
goto _LL583;} else{ goto _LL584;} _LL584: if(*(( int*) _temp510) == Cyc_Absyn_Struct_e){
_LL749: _temp748=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp510)->f1; goto _LL747; _LL747: _temp746=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp510)->f2; goto _LL745; _LL745: _temp744=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp510)->f3; goto
_LL743; _LL743: _temp742=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp510)->f4; goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp510)
== Cyc_Absyn_AnonStruct_e){ _LL753: _temp752=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp510)->f1; goto _LL751; _LL751: _temp750=( struct Cyc_List_List*)(( struct
Cyc_Absyn_AnonStruct_e_struct*) _temp510)->f2; goto _LL587;} else{ goto _LL588;}
_LL588: if(*(( int*) _temp510) == Cyc_Absyn_Tunion_e){ _LL763: _temp762=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp510)->f1; goto _LL761;
_LL761: _temp760=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp510)->f2; goto _LL759; _LL759: _temp758=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp510)->f3; goto _LL757; _LL757: _temp756=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp510)->f4;
goto _LL755; _LL755: _temp754=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp510)->f5; goto _LL589;} else{ goto _LL590;} _LL590: if(*(( int*) _temp510)
== Cyc_Absyn_Enum_e){ _LL769: _temp768=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp510)->f1; goto _LL767; _LL767: _temp766=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp510)->f2; goto _LL765; _LL765: _temp764=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp510)->f3;
goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp510) == Cyc_Absyn_Malloc_e){
_LL773: _temp772=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp510)->f1; goto _LL771; _LL771: _temp770=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp510)->f2; goto _LL593;} else{ goto _LL594;} _LL594: if(*(( int*) _temp510)
== Cyc_Absyn_UnresolvedMem_e){ _LL777: _temp776=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp510)->f1; goto _LL775; _LL775: _temp774=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp510)->f2;
goto _LL595;} else{ goto _LL596;} _LL596: if(*(( int*) _temp510) == Cyc_Absyn_StmtExp_e){
_LL779: _temp778=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp510)->f1; goto _LL597;} else{ goto _LL598;} _LL598: if(*(( int*) _temp510)
== Cyc_Absyn_Codegen_e){ _LL781: _temp780=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp510)->f1; goto _LL599;} else{ goto _LL600;}
_LL600: if(*(( int*) _temp510) == Cyc_Absyn_Fill_e){ _LL783: _temp782=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp510)->f1; goto _LL601;}
else{ goto _LL511;} _LL513: Cyc_Absyndump_dump_char(( int)'\''); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp604)); Cyc_Absyndump_dump_char(( int)'\''); goto
_LL511; _LL515: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp784=(
char*)"(short)"; struct _tagged_string _temp785; _temp785.curr= _temp784;
_temp785.base= _temp784; _temp785.last_plus_one= _temp784 + 8; _temp785;})); Cyc_Absyndump_dump_nospace(
Cyc_Core_string_of_int(( int) _temp610)); goto _LL511; _LL517: Cyc_Absyndump_dump(
Cyc_Core_string_of_int( _temp616)); goto _LL511; _LL519: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp622)); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp786=(
char*)"u"; struct _tagged_string _temp787; _temp787.curr= _temp786; _temp787.base=
_temp786; _temp787.last_plus_one= _temp786 + 2; _temp787;})); goto _LL511;
_LL521: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp788=( char*)"<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp789; _temp789.curr= _temp788; _temp789.base= _temp788;
_temp789.last_plus_one= _temp788 + 27; _temp789;})); goto _LL511; _LL523: Cyc_Absyndump_dump(
_temp634); goto _LL511; _LL525: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp790=( char*)"null"; struct _tagged_string _temp791; _temp791.curr=
_temp790; _temp791.base= _temp790; _temp791.last_plus_one= _temp790 + 5;
_temp791;})); goto _LL511; _LL527: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp640)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL511; _LL529: _temp646= _temp642; goto _LL531; _LL531: Cyc_Absyndump_dumpqvar(
_temp646); goto _LL511; _LL533: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp650); if( ! Cyc_Absyn_is_format_prim( _temp650)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp648)){ case 1: _LL792: if( _temp650 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp648))->hd); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp794=( char*)".size"; struct _tagged_string _temp795;
_temp795.curr= _temp794; _temp795.base= _temp794; _temp795.last_plus_one=
_temp794 + 6; _temp795;}));} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp648))->hd);}
break; case 2: _LL793: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp648))->hd); Cyc_Absyndump_dump( pstr);
Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp648))->tl))->hd); break;
default: _LL796:( void) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp798=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp798[ 0]=({ struct Cyc_Core_Failure_struct _temp799; _temp799.tag= Cyc_Core_Failure;
_temp799.f1=( struct _tagged_string)({ char* _temp800=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 47; _temp801;}); _temp799;}); _temp798;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp802=( char*)"("; struct _tagged_string _temp803;
_temp803.curr= _temp802; _temp803.base= _temp802; _temp803.last_plus_one=
_temp802 + 2; _temp803;})); Cyc_Absyndump_dumpexps_prec( 20, _temp648); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp804=( char*)")"; struct _tagged_string
_temp805; _temp805.curr= _temp804; _temp805.base= _temp804; _temp805.last_plus_one=
_temp804 + 2; _temp805;}));} goto _LL511;} _LL535: Cyc_Absyndump_dumpexp_prec(
myprec, _temp656); if( _temp654 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp654))->v));} Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp806=( char*)"="; struct _tagged_string
_temp807; _temp807.curr= _temp806; _temp807.base= _temp806; _temp807.last_plus_one=
_temp806 + 2; _temp807;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp652); goto
_LL511; _LL537: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp808=(
char*)"++"; struct _tagged_string _temp809; _temp809.curr= _temp808; _temp809.base=
_temp808; _temp809.last_plus_one= _temp808 + 3; _temp809;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp660); goto _LL511; _LL539: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp810=( char*)"--"; struct _tagged_string _temp811;
_temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 3; _temp811;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp664); goto
_LL511; _LL541: Cyc_Absyndump_dumpexp_prec( myprec, _temp668); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp812=( char*)"++"; struct _tagged_string
_temp813; _temp813.curr= _temp812; _temp813.base= _temp812; _temp813.last_plus_one=
_temp812 + 3; _temp813;})); goto _LL511; _LL543: Cyc_Absyndump_dumpexp_prec(
myprec, _temp672); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp814=(
char*)"--"; struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base=
_temp814; _temp815.last_plus_one= _temp814 + 3; _temp815;})); goto _LL511;
_LL545: Cyc_Absyndump_dumpexp_prec( myprec, _temp678); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp676); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp674); goto _LL511; _LL547:
Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec( myprec, _temp682);
Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec( myprec, _temp680);
Cyc_Absyndump_dump_char(( int)')'); goto _LL511; _LL549: _temp690= _temp686;
_temp688= _temp684; goto _LL551; _LL551: Cyc_Absyndump_dumpexp_prec( myprec,
_temp690); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp816=(
char*)"("; struct _tagged_string _temp817; _temp817.curr= _temp816; _temp817.base=
_temp816; _temp817.last_plus_one= _temp816 + 2; _temp817;})); Cyc_Absyndump_dumpexps_prec(
20, _temp688); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp818=( char*)")"; struct _tagged_string _temp819; _temp819.curr= _temp818;
_temp819.base= _temp818; _temp819.last_plus_one= _temp818 + 2; _temp819;}));
goto _LL511; _LL553: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp820=( char*)"throw"; struct _tagged_string _temp821; _temp821.curr=
_temp820; _temp821.base= _temp820; _temp821.last_plus_one= _temp820 + 6;
_temp821;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp692); goto _LL511; _LL555:
_temp698= _temp694; goto _LL557; _LL557: Cyc_Absyndump_dumpexp_prec( inprec,
_temp698); goto _LL511; _LL559: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp702); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp700); goto _LL511; _LL561: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp704); goto _LL511; _LL563: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp822=( char*)"new "; struct _tagged_string _temp823;
_temp823.curr= _temp822; _temp823.base= _temp822; _temp823.last_plus_one=
_temp822 + 5; _temp823;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp706); goto
_LL511; _LL565: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp824=(
char*)"sizeof("; struct _tagged_string _temp825; _temp825.curr= _temp824;
_temp825.base= _temp824; _temp825.last_plus_one= _temp824 + 8; _temp825;})); Cyc_Absyndump_dumptyp(
_temp710); Cyc_Absyndump_dump_char(( int)')'); goto _LL511; _LL567: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp826=( char*)"sizeof("; struct _tagged_string
_temp827; _temp827.curr= _temp826; _temp827.base= _temp826; _temp827.last_plus_one=
_temp826 + 8; _temp827;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp712); Cyc_Absyndump_dump_char((
int)')'); goto _LL511; _LL569: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp714); goto _LL511; _LL571: Cyc_Absyndump_dumpexp_prec( myprec,
_temp718); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp716); goto _LL511; _LL573: Cyc_Absyndump_dumpexp_prec( myprec, _temp722);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp828=( char*)"->";
struct _tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 3; _temp829;})); Cyc_Absyndump_dump_nospace(*
_temp720); goto _LL511; _LL575: Cyc_Absyndump_dumpexp_prec( myprec, _temp726);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp724); Cyc_Absyndump_dump_char((
int)']'); goto _LL511; _LL577: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp830=( char*)"$("; struct _tagged_string _temp831; _temp831.curr=
_temp830; _temp831.base= _temp830; _temp831.last_plus_one= _temp830 + 3;
_temp831;})); Cyc_Absyndump_dumpexps_prec( 20, _temp728); Cyc_Absyndump_dump_char((
int)')'); goto _LL511; _LL579: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp732).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp730,( struct _tagged_string)({ char* _temp832=( char*)"{"; struct
_tagged_string _temp833; _temp833.curr= _temp832; _temp833.base= _temp832;
_temp833.last_plus_one= _temp832 + 2; _temp833;}),( struct _tagged_string)({
char* _temp834=( char*)"}"; struct _tagged_string _temp835; _temp835.curr=
_temp834; _temp835.base= _temp834; _temp835.last_plus_one= _temp834 + 2;
_temp835;}),( struct _tagged_string)({ char* _temp836=( char*)","; struct
_tagged_string _temp837; _temp837.curr= _temp836; _temp837.base= _temp836;
_temp837.last_plus_one= _temp836 + 2; _temp837;})); goto _LL511; _LL581:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp734,( struct _tagged_string)({ char* _temp838=( char*)"{";
struct _tagged_string _temp839; _temp839.curr= _temp838; _temp839.base= _temp838;
_temp839.last_plus_one= _temp838 + 2; _temp839;}),( struct _tagged_string)({
char* _temp840=( char*)"}"; struct _tagged_string _temp841; _temp841.curr=
_temp840; _temp841.base= _temp840; _temp841.last_plus_one= _temp840 + 2;
_temp841;}),( struct _tagged_string)({ char* _temp842=( char*)","; struct
_tagged_string _temp843; _temp843.curr= _temp842; _temp843.base= _temp842;
_temp843.last_plus_one= _temp842 + 2; _temp843;})); goto _LL511; _LL583: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp844=( char*)"new {for"; struct
_tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 9; _temp845;})); Cyc_Absyndump_dump_str((*
_temp740->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp738); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp736);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL511; _LL585: Cyc_Absyndump_dumpqvar(
_temp748);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp744,( struct
_tagged_string)({ char* _temp846=( char*)"{"; struct _tagged_string _temp847;
_temp847.curr= _temp846; _temp847.base= _temp846; _temp847.last_plus_one=
_temp846 + 2; _temp847;}),( struct _tagged_string)({ char* _temp848=( char*)"}";
struct _tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 2; _temp849;}),( struct _tagged_string)({
char* _temp850=( char*)","; struct _tagged_string _temp851; _temp851.curr=
_temp850; _temp851.base= _temp850; _temp851.last_plus_one= _temp850 + 2;
_temp851;})); goto _LL511; _LL587:(( void(*)( void(* f)( struct _tuple7*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp750,(
struct _tagged_string)({ char* _temp852=( char*)"{"; struct _tagged_string
_temp853; _temp853.curr= _temp852; _temp853.base= _temp852; _temp853.last_plus_one=
_temp852 + 2; _temp853;}),( struct _tagged_string)({ char* _temp854=( char*)"}";
struct _tagged_string _temp855; _temp855.curr= _temp854; _temp855.base= _temp854;
_temp855.last_plus_one= _temp854 + 2; _temp855;}),( struct _tagged_string)({
char* _temp856=( char*)","; struct _tagged_string _temp857; _temp857.curr=
_temp856; _temp857.base= _temp856; _temp857.last_plus_one= _temp856 + 2;
_temp857;})); goto _LL511; _LL589: Cyc_Absyndump_dumpqvar( _temp754->name); if(
_temp758 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpexp, _temp758,( struct
_tagged_string)({ char* _temp858=( char*)"("; struct _tagged_string _temp859;
_temp859.curr= _temp858; _temp859.base= _temp858; _temp859.last_plus_one=
_temp858 + 2; _temp859;}),( struct _tagged_string)({ char* _temp860=( char*)")";
struct _tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 2; _temp861;}),( struct _tagged_string)({
char* _temp862=( char*)","; struct _tagged_string _temp863; _temp863.curr=
_temp862; _temp863.base= _temp862; _temp863.last_plus_one= _temp862 + 2;
_temp863;}));} goto _LL511; _LL591: Cyc_Absyndump_dumpqvar( _temp768); goto
_LL511; _LL593: if( _temp772 != 0){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp864=( char*)"rmalloc("; struct _tagged_string _temp865; _temp865.curr=
_temp864; _temp865.base= _temp864; _temp865.last_plus_one= _temp864 + 9;
_temp865;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp772))); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp866=( char*)","; struct _tagged_string _temp867; _temp867.curr= _temp866;
_temp867.base= _temp866; _temp867.last_plus_one= _temp866 + 2; _temp867;}));}
else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp868=( char*)"malloc(";
struct _tagged_string _temp869; _temp869.curr= _temp868; _temp869.base= _temp868;
_temp869.last_plus_one= _temp868 + 8; _temp869;}));} Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp870=( char*)"sizeof("; struct _tagged_string
_temp871; _temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 8; _temp871;})); Cyc_Absyndump_dumptyp( _temp770); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp872=( char*)"))"; struct _tagged_string
_temp873; _temp873.curr= _temp872; _temp873.base= _temp872; _temp873.last_plus_one=
_temp872 + 3; _temp873;})); goto _LL511; _LL595:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp774,( struct _tagged_string)({ char* _temp874=( char*)"{"; struct
_tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 2; _temp875;}),( struct _tagged_string)({
char* _temp876=( char*)"}"; struct _tagged_string _temp877; _temp877.curr=
_temp876; _temp877.base= _temp876; _temp877.last_plus_one= _temp876 + 2;
_temp877;}),( struct _tagged_string)({ char* _temp878=( char*)","; struct
_tagged_string _temp879; _temp879.curr= _temp878; _temp879.base= _temp878;
_temp879.last_plus_one= _temp878 + 2; _temp879;})); goto _LL511; _LL597: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp880=( char*)"({"; struct _tagged_string
_temp881; _temp881.curr= _temp880; _temp881.base= _temp880; _temp881.last_plus_one=
_temp880 + 3; _temp881;})); Cyc_Absyndump_dumpstmt( _temp778); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp882=( char*)"})"; struct _tagged_string
_temp883; _temp883.curr= _temp882; _temp883.base= _temp882; _temp883.last_plus_one=
_temp882 + 3; _temp883;})); goto _LL511; _LL599: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp884=( char*)"codegen("; struct _tagged_string
_temp885; _temp885.curr= _temp884; _temp885.base= _temp884; _temp885.last_plus_one=
_temp884 + 9; _temp885;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp886=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp886->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp887=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp887[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp888; _temp888.tag= Cyc_Absyn_Fn_d;
_temp888.f1= _temp780; _temp888;}); _temp887;})); _temp886->loc= e->loc;
_temp886;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp889=(
char*)")"; struct _tagged_string _temp890; _temp890.curr= _temp889; _temp890.base=
_temp889; _temp890.last_plus_one= _temp889 + 2; _temp890;})); goto _LL511;
_LL601: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp891=( char*)"fill(";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 6; _temp892;})); Cyc_Absyndump_dumpexp(
_temp782); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp893=( char*)")";
struct _tagged_string _temp894; _temp894.curr= _temp893; _temp894.base= _temp893;
_temp894.last_plus_one= _temp893 + 2; _temp894;})); goto _LL511; _LL511:;} if(
inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}} void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec( 0, e);} void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List* scs){ for( 0; scs != 0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause* _temp895=( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd; goto _LL896; _LL896: if( _temp895->where_clause
== 0?( void*)( _temp895->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp897=( char*)"default:"; struct
_tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 9; _temp898;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp899=( char*)"case"; struct _tagged_string
_temp900; _temp900.curr= _temp899; _temp900.base= _temp899; _temp900.last_plus_one=
_temp899 + 5; _temp900;})); Cyc_Absyndump_dumppat( _temp895->pattern); if(
_temp895->where_clause != 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp901=( char*)"&&"; struct _tagged_string _temp902; _temp902.curr= _temp901;
_temp902.base= _temp901; _temp902.last_plus_one= _temp901 + 3; _temp902;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp895->where_clause)));}
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp903=( char*)":";
struct _tagged_string _temp904; _temp904.curr= _temp903; _temp904.base= _temp903;
_temp904.last_plus_one= _temp903 + 2; _temp904;}));} Cyc_Absyndump_dumpstmt(
_temp895->body);}} void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt* s){ void*
_temp905=( void*) s->r; struct Cyc_Absyn_Exp* _temp949; struct Cyc_Absyn_Stmt*
_temp951; struct Cyc_Absyn_Stmt* _temp953; struct Cyc_Absyn_Exp* _temp955;
struct Cyc_Absyn_Exp* _temp957; struct Cyc_Absyn_Stmt* _temp959; struct Cyc_Absyn_Stmt*
_temp961; struct Cyc_Absyn_Exp* _temp963; struct Cyc_Absyn_Stmt* _temp965;
struct _tuple2 _temp967; struct Cyc_Absyn_Stmt* _temp969; struct Cyc_Absyn_Exp*
_temp971; struct Cyc_Absyn_Stmt* _temp973; struct Cyc_Absyn_Stmt* _temp975;
struct Cyc_Absyn_Stmt* _temp977; struct _tagged_string* _temp979; struct Cyc_Absyn_Switch_clause**
_temp981; struct Cyc_List_List* _temp983; struct Cyc_Absyn_Switch_clause**
_temp985; struct Cyc_List_List* _temp987; struct Cyc_Absyn_Stmt* _temp989;
struct _tuple2 _temp991; struct Cyc_Absyn_Stmt* _temp993; struct Cyc_Absyn_Exp*
_temp995; struct _tuple2 _temp997; struct Cyc_Absyn_Stmt* _temp999; struct Cyc_Absyn_Exp*
_temp1001; struct Cyc_Absyn_Exp* _temp1003; struct Cyc_List_List* _temp1005;
struct Cyc_Absyn_Exp* _temp1007; struct Cyc_Absyn_Stmt* _temp1009; struct Cyc_Absyn_Decl*
_temp1011; struct Cyc_Absyn_Stmt* _temp1013; struct Cyc_Absyn_Stmt* _temp1015;
struct Cyc_Absyn_Stmt* _temp1017; struct _tagged_string* _temp1019; struct
_tuple2 _temp1021; struct Cyc_Absyn_Stmt* _temp1023; struct Cyc_Absyn_Exp*
_temp1025; struct Cyc_Absyn_Stmt* _temp1027; struct Cyc_List_List* _temp1029;
struct Cyc_Absyn_Stmt* _temp1031; struct Cyc_Absyn_Stmt* _temp1033; struct Cyc_Absyn_Vardecl*
_temp1035; struct Cyc_Absyn_Tvar* _temp1037; _LL907: if( _temp905 ==( void*) Cyc_Absyn_Skip_s){
goto _LL908;} else{ goto _LL909;} _LL909: if(( unsigned int) _temp905 > 1u?*((
int*) _temp905) == Cyc_Absyn_Exp_s: 0){ _LL950: _temp949=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp905)->f1; goto _LL910;} else{ goto _LL911;}
_LL911: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Seq_s:
0){ _LL954: _temp953=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp905)->f1; goto _LL952; _LL952: _temp951=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp905)->f2; goto _LL912;} else{ goto _LL913;} _LL913:
if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Return_s: 0){
_LL956: _temp955=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp905)->f1; if( _temp955 == 0){ goto _LL914;} else{ goto _LL915;}} else{ goto
_LL915;} _LL915: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Return_s:
0){ _LL958: _temp957=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp905)->f1; goto _LL916;} else{ goto _LL917;} _LL917: if(( unsigned int)
_temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_IfThenElse_s: 0){ _LL964:
_temp963=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp905)->f1; goto _LL962; _LL962: _temp961=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp905)->f2; goto _LL960; _LL960: _temp959=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp905)->f3;
goto _LL918;} else{ goto _LL919;} _LL919: if(( unsigned int) _temp905 > 1u?*((
int*) _temp905) == Cyc_Absyn_While_s: 0){ _LL968: _temp967=( struct _tuple2)((
struct Cyc_Absyn_While_s_struct*) _temp905)->f1; _LL972: _temp971= _temp967.f1;
goto _LL970; _LL970: _temp969= _temp967.f2; goto _LL966; _LL966: _temp965=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp905)->f2; goto
_LL920;} else{ goto _LL921;} _LL921: if(( unsigned int) _temp905 > 1u?*(( int*)
_temp905) == Cyc_Absyn_Break_s: 0){ _LL974: _temp973=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp905)->f1; goto _LL922;} else{ goto _LL923;}
_LL923: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Continue_s:
0){ _LL976: _temp975=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp905)->f1; goto _LL924;} else{ goto _LL925;} _LL925: if(( unsigned int)
_temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Goto_s: 0){ _LL980: _temp979=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp905)->f1; goto
_LL978; _LL978: _temp977=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp905)->f2; goto _LL926;} else{ goto _LL927;} _LL927: if(( unsigned int)
_temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Fallthru_s: 0){ _LL984: _temp983=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp905)->f1; if(
_temp983 == 0){ goto _LL982;} else{ goto _LL929;} _LL982: _temp981=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp905)->f2; goto _LL928;} else{ goto
_LL929;} _LL929: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Fallthru_s:
0){ _LL988: _temp987=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp905)->f1; goto _LL986; _LL986: _temp985=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp905)->f2; goto _LL930;} else{ goto
_LL931;} _LL931: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_For_s:
0){ _LL1004: _temp1003=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp905)->f1; goto _LL998; _LL998: _temp997=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp905)->f2; _LL1002: _temp1001= _temp997.f1; goto _LL1000; _LL1000: _temp999=
_temp997.f2; goto _LL992; _LL992: _temp991=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp905)->f3; _LL996: _temp995= _temp991.f1; goto _LL994; _LL994: _temp993=
_temp991.f2; goto _LL990; _LL990: _temp989=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp905)->f4; goto _LL932;} else{ goto _LL933;} _LL933:
if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Switch_s: 0){
_LL1008: _temp1007=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp905)->f1; goto _LL1006; _LL1006: _temp1005=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp905)->f2; goto _LL934;} else{ goto
_LL935;} _LL935: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Decl_s:
0){ _LL1012: _temp1011=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp905)->f1; goto _LL1010; _LL1010: _temp1009=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp905)->f2; goto _LL936;} else{ goto _LL937;}
_LL937: if(( unsigned int) _temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Cut_s:
0){ _LL1014: _temp1013=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp905)->f1; goto _LL938;} else{ goto _LL939;} _LL939: if(( unsigned int)
_temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Splice_s: 0){ _LL1016: _temp1015=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp905)->f1; goto
_LL940;} else{ goto _LL941;} _LL941: if(( unsigned int) _temp905 > 1u?*(( int*)
_temp905) == Cyc_Absyn_Label_s: 0){ _LL1020: _temp1019=( struct _tagged_string*)((
struct Cyc_Absyn_Label_s_struct*) _temp905)->f1; goto _LL1018; _LL1018:
_temp1017=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*) _temp905)->f2;
goto _LL942;} else{ goto _LL943;} _LL943: if(( unsigned int) _temp905 > 1u?*((
int*) _temp905) == Cyc_Absyn_Do_s: 0){ _LL1028: _temp1027=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp905)->f1; goto _LL1022; _LL1022: _temp1021=(
struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp905)->f2; _LL1026:
_temp1025= _temp1021.f1; goto _LL1024; _LL1024: _temp1023= _temp1021.f2; goto
_LL944;} else{ goto _LL945;} _LL945: if(( unsigned int) _temp905 > 1u?*(( int*)
_temp905) == Cyc_Absyn_TryCatch_s: 0){ _LL1032: _temp1031=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp905)->f1; goto _LL1030; _LL1030:
_temp1029=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp905)->f2; goto _LL946;} else{ goto _LL947;} _LL947: if(( unsigned int)
_temp905 > 1u?*(( int*) _temp905) == Cyc_Absyn_Region_s: 0){ _LL1038: _temp1037=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*) _temp905)->f1; goto
_LL1036; _LL1036: _temp1035=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp905)->f2; goto _LL1034; _LL1034: _temp1033=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp905)->f3; goto _LL948;} else{ goto
_LL906;} _LL908: Cyc_Absyndump_dump_semi(); goto _LL906; _LL910: Cyc_Absyndump_dumpexp(
_temp949); Cyc_Absyndump_dump_semi(); goto _LL906; _LL912: if( Cyc_Absynpp_is_declaration(
_temp953)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp953);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp953);}
if( Cyc_Absynpp_is_declaration( _temp951)){ Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp951); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dumpstmt( _temp951);} goto _LL906; _LL914: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1039=( char*)"return;"; struct
_tagged_string _temp1040; _temp1040.curr= _temp1039; _temp1040.base= _temp1039;
_temp1040.last_plus_one= _temp1039 + 8; _temp1040;})); goto _LL906; _LL916: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1041=( char*)"return"; struct _tagged_string
_temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041; _temp1042.last_plus_one=
_temp1041 + 7; _temp1042;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp957))); Cyc_Absyndump_dump_semi(); goto
_LL906; _LL918: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1043=(
char*)"if("; struct _tagged_string _temp1044; _temp1044.curr= _temp1043;
_temp1044.base= _temp1043; _temp1044.last_plus_one= _temp1043 + 4; _temp1044;}));
Cyc_Absyndump_dumpexp( _temp963); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1045=( char*)"){"; struct _tagged_string _temp1046;
_temp1046.curr= _temp1045; _temp1046.base= _temp1045; _temp1046.last_plus_one=
_temp1045 + 3; _temp1046;})); Cyc_Absyndump_dumpstmt( _temp961); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp1047=( void*) _temp959->r; _LL1049: if( _temp1047 ==( void*)
Cyc_Absyn_Skip_s){ goto _LL1050;} else{ goto _LL1051;} _LL1051: goto _LL1052;
_LL1050: goto _LL1048; _LL1052: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1053=( char*)"else{"; struct _tagged_string _temp1054; _temp1054.curr=
_temp1053; _temp1054.base= _temp1053; _temp1054.last_plus_one= _temp1053 + 6;
_temp1054;})); Cyc_Absyndump_dumpstmt( _temp959); Cyc_Absyndump_dump_char(( int)'}');
goto _LL1048; _LL1048:;} goto _LL906; _LL920: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1055=( char*)"while("; struct _tagged_string
_temp1056; _temp1056.curr= _temp1055; _temp1056.base= _temp1055; _temp1056.last_plus_one=
_temp1055 + 7; _temp1056;})); Cyc_Absyndump_dumpexp( _temp971); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1057=( char*)") {"; struct _tagged_string
_temp1058; _temp1058.curr= _temp1057; _temp1058.base= _temp1057; _temp1058.last_plus_one=
_temp1057 + 4; _temp1058;})); Cyc_Absyndump_dumpstmt( _temp965); Cyc_Absyndump_dump_char((
int)'}'); goto _LL906; _LL922: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1059=( char*)"break;"; struct _tagged_string _temp1060; _temp1060.curr=
_temp1059; _temp1060.base= _temp1059; _temp1060.last_plus_one= _temp1059 + 7;
_temp1060;})); goto _LL906; _LL924: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1061=( char*)"continue;"; struct _tagged_string _temp1062; _temp1062.curr=
_temp1061; _temp1062.base= _temp1061; _temp1062.last_plus_one= _temp1061 + 10;
_temp1062;})); goto _LL906; _LL926: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1063=( char*)"goto"; struct _tagged_string _temp1064; _temp1064.curr=
_temp1063; _temp1064.base= _temp1063; _temp1064.last_plus_one= _temp1063 + 5;
_temp1064;})); Cyc_Absyndump_dump_str( _temp979); Cyc_Absyndump_dump_semi();
goto _LL906; _LL928: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1065=( char*)"fallthru;"; struct _tagged_string _temp1066; _temp1066.curr=
_temp1065; _temp1066.base= _temp1065; _temp1066.last_plus_one= _temp1065 + 10;
_temp1066;})); goto _LL906; _LL930: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1067=( char*)"fallthru("; struct _tagged_string _temp1068; _temp1068.curr=
_temp1067; _temp1068.base= _temp1067; _temp1068.last_plus_one= _temp1067 + 10;
_temp1068;})); Cyc_Absyndump_dumpexps_prec( 20, _temp987); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1069=( char*)");"; struct _tagged_string
_temp1070; _temp1070.curr= _temp1069; _temp1070.base= _temp1069; _temp1070.last_plus_one=
_temp1069 + 3; _temp1070;})); goto _LL906; _LL932: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1071=( char*)"for("; struct _tagged_string
_temp1072; _temp1072.curr= _temp1071; _temp1072.base= _temp1071; _temp1072.last_plus_one=
_temp1071 + 5; _temp1072;})); Cyc_Absyndump_dumpexp( _temp1003); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp1001); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp995); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1073=(
char*)"){"; struct _tagged_string _temp1074; _temp1074.curr= _temp1073;
_temp1074.base= _temp1073; _temp1074.last_plus_one= _temp1073 + 3; _temp1074;}));
Cyc_Absyndump_dumpstmt( _temp989); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL906; _LL934: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1075=(
char*)"switch("; struct _tagged_string _temp1076; _temp1076.curr= _temp1075;
_temp1076.base= _temp1075; _temp1076.last_plus_one= _temp1075 + 8; _temp1076;}));
Cyc_Absyndump_dumpexp( _temp1007); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1077=( char*)"){"; struct _tagged_string _temp1078;
_temp1078.curr= _temp1077; _temp1078.base= _temp1077; _temp1078.last_plus_one=
_temp1077 + 3; _temp1078;})); Cyc_Absyndump_dumpswitchclauses( _temp1005); Cyc_Absyndump_dump_char((
int)'}'); goto _LL906; _LL936: Cyc_Absyndump_dumpdecl( _temp1011); Cyc_Absyndump_dumpstmt(
_temp1009); goto _LL906; _LL938: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1079=( char*)"cut"; struct _tagged_string _temp1080; _temp1080.curr=
_temp1079; _temp1080.base= _temp1079; _temp1080.last_plus_one= _temp1079 + 4;
_temp1080;})); Cyc_Absyndump_dumpstmt( _temp1013); goto _LL906; _LL940: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1081=( char*)"splice"; struct _tagged_string
_temp1082; _temp1082.curr= _temp1081; _temp1082.base= _temp1081; _temp1082.last_plus_one=
_temp1081 + 7; _temp1082;})); Cyc_Absyndump_dumpstmt( _temp1015); goto _LL906;
_LL942: if( Cyc_Absynpp_is_declaration( _temp1017)){ Cyc_Absyndump_dump_str(
_temp1019); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1083=( char*)": {"; struct _tagged_string _temp1084; _temp1084.curr=
_temp1083; _temp1084.base= _temp1083; _temp1084.last_plus_one= _temp1083 + 4;
_temp1084;})); Cyc_Absyndump_dumpstmt( _temp1017); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1019); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1017);} goto _LL906; _LL944: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1085=( char*)"do {"; struct _tagged_string
_temp1086; _temp1086.curr= _temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one=
_temp1085 + 5; _temp1086;})); Cyc_Absyndump_dumpstmt( _temp1027); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1087=( char*)"} while ("; struct
_tagged_string _temp1088; _temp1088.curr= _temp1087; _temp1088.base= _temp1087;
_temp1088.last_plus_one= _temp1087 + 10; _temp1088;})); Cyc_Absyndump_dumpexp(
_temp1025); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1089=( char*)");"; struct _tagged_string _temp1090; _temp1090.curr=
_temp1089; _temp1090.base= _temp1089; _temp1090.last_plus_one= _temp1089 + 3;
_temp1090;})); goto _LL906; _LL946: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1091=( char*)"try"; struct _tagged_string _temp1092; _temp1092.curr=
_temp1091; _temp1092.base= _temp1091; _temp1092.last_plus_one= _temp1091 + 4;
_temp1092;})); Cyc_Absyndump_dumpstmt( _temp1031); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1093=( char*)"catch {"; struct _tagged_string
_temp1094; _temp1094.curr= _temp1093; _temp1094.base= _temp1093; _temp1094.last_plus_one=
_temp1093 + 8; _temp1094;})); Cyc_Absyndump_dumpswitchclauses( _temp1029); Cyc_Absyndump_dump_char((
int)'}'); goto _LL906; _LL948: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1095=( char*)"region<"; struct _tagged_string _temp1096; _temp1096.curr=
_temp1095; _temp1096.base= _temp1095; _temp1096.last_plus_one= _temp1095 + 8;
_temp1096;})); Cyc_Absyndump_dumptvar( _temp1037); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1097=( char*)"> "; struct _tagged_string _temp1098;
_temp1098.curr= _temp1097; _temp1098.base= _temp1097; _temp1098.last_plus_one=
_temp1097 + 3; _temp1098;})); Cyc_Absyndump_dumpqvar( _temp1035->name); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1099=( char*)"{"; struct _tagged_string
_temp1100; _temp1100.curr= _temp1099; _temp1100.base= _temp1099; _temp1100.last_plus_one=
_temp1099 + 2; _temp1100;})); Cyc_Absyndump_dumpstmt( _temp1033); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1101=( char*)"}"; struct _tagged_string
_temp1102; _temp1102.curr= _temp1101; _temp1102.base= _temp1101; _temp1102.last_plus_one=
_temp1101 + 2; _temp1102;})); goto _LL906; _LL906:;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char*
_temp1103=( char*)""; struct _tagged_string _temp1104; _temp1104.curr= _temp1103;
_temp1104.base= _temp1103; _temp1104.last_plus_one= _temp1103 + 1; _temp1104;}),(
struct _tagged_string)({ char* _temp1105=( char*)"="; struct _tagged_string
_temp1106; _temp1106.curr= _temp1105; _temp1106.base= _temp1105; _temp1106.last_plus_one=
_temp1105 + 2; _temp1106;}),( struct _tagged_string)({ char* _temp1107=( char*)"=";
struct _tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base=
_temp1107; _temp1108.last_plus_one= _temp1107 + 2; _temp1108;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1109=(
void*) p->r; int _temp1143; void* _temp1145; int _temp1147; void* _temp1149;
char _temp1151; struct _tagged_string _temp1153; struct Cyc_Absyn_Vardecl*
_temp1155; struct Cyc_List_List* _temp1157; struct Cyc_Absyn_Pat* _temp1159;
struct Cyc_Absyn_Vardecl* _temp1161; struct _tuple0* _temp1163; struct Cyc_List_List*
_temp1165; struct Cyc_List_List* _temp1167; struct _tuple0* _temp1169; struct
Cyc_List_List* _temp1171; struct Cyc_List_List* _temp1173; struct _tuple0*
_temp1175; struct Cyc_List_List* _temp1177; struct Cyc_List_List* _temp1179;
struct Cyc_Core_Opt* _temp1181; struct Cyc_Absyn_Structdecl* _temp1183; struct
Cyc_List_List* _temp1185; struct Cyc_List_List* _temp1187; struct Cyc_Absyn_Tunionfield*
_temp1189; struct Cyc_Absyn_Tuniondecl* _temp1191; struct Cyc_Absyn_Enumfield*
_temp1193; struct Cyc_Absyn_Enumdecl* _temp1195; _LL1111: if( _temp1109 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1112;} else{ goto _LL1113;} _LL1113: if( _temp1109 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1114;} else{ goto _LL1115;} _LL1115: if((
unsigned int) _temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Int_p: 0){
_LL1146: _temp1145=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1109)->f1;
if( _temp1145 ==( void*) Cyc_Absyn_Signed){ goto _LL1144;} else{ goto _LL1117;}
_LL1144: _temp1143=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1109)->f2; goto
_LL1116;} else{ goto _LL1117;} _LL1117: if(( unsigned int) _temp1109 > 2u?*((
int*) _temp1109) == Cyc_Absyn_Int_p: 0){ _LL1150: _temp1149=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1109)->f1; if( _temp1149 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1148;} else{ goto _LL1119;} _LL1148: _temp1147=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1109)->f2; goto _LL1118;} else{ goto _LL1119;} _LL1119: if(( unsigned int)
_temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Char_p: 0){ _LL1152: _temp1151=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp1109)->f1; goto _LL1120;} else{
goto _LL1121;} _LL1121: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109)
== Cyc_Absyn_Float_p: 0){ _LL1154: _temp1153=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp1109)->f1; goto _LL1122;} else{ goto _LL1123;}
_LL1123: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Var_p:
0){ _LL1156: _temp1155=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1109)->f1; goto _LL1124;} else{ goto _LL1125;} _LL1125: if(( unsigned int)
_temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Tuple_p: 0){ _LL1158: _temp1157=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1109)->f1; goto
_LL1126;} else{ goto _LL1127;} _LL1127: if(( unsigned int) _temp1109 > 2u?*((
int*) _temp1109) == Cyc_Absyn_Pointer_p: 0){ _LL1160: _temp1159=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1109)->f1; goto _LL1128;} else{ goto
_LL1129;} _LL1129: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Reference_p:
0){ _LL1162: _temp1161=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1109)->f1; goto _LL1130;} else{ goto _LL1131;} _LL1131: if(( unsigned int)
_temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_UnknownId_p: 0){ _LL1164:
_temp1163=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1109)->f1;
goto _LL1132;} else{ goto _LL1133;} _LL1133: if(( unsigned int) _temp1109 > 2u?*((
int*) _temp1109) == Cyc_Absyn_UnknownCall_p: 0){ _LL1170: _temp1169=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1109)->f1; goto _LL1168;
_LL1168: _temp1167=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1109)->f2; goto _LL1166; _LL1166: _temp1165=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1109)->f3; goto _LL1134;} else{
goto _LL1135;} _LL1135: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1176: _temp1175=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1109)->f1; goto _LL1174; _LL1174:
_temp1173=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1109)->f2; goto _LL1172; _LL1172: _temp1171=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1109)->f3; goto _LL1136;} else{
goto _LL1137;} _LL1137: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109)
== Cyc_Absyn_Struct_p: 0){ _LL1184: _temp1183=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1109)->f1; goto _LL1182; _LL1182:
_temp1181=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1109)->f2;
goto _LL1180; _LL1180: _temp1179=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1109)->f3; goto _LL1178; _LL1178: _temp1177=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1109)->f4; goto _LL1138;} else{ goto
_LL1139;} _LL1139: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Tunion_p:
0){ _LL1192: _temp1191=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1109)->f1; goto _LL1190; _LL1190: _temp1189=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1109)->f2; goto _LL1188; _LL1188:
_temp1187=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1109)->f3; goto _LL1186; _LL1186: _temp1185=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1109)->f4; goto _LL1140;} else{ goto
_LL1141;} _LL1141: if(( unsigned int) _temp1109 > 2u?*(( int*) _temp1109) == Cyc_Absyn_Enum_p:
0){ _LL1196: _temp1195=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1109)->f1; goto _LL1194; _LL1194: _temp1193=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1109)->f2; goto _LL1142;} else{ goto
_LL1110;} _LL1112: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1110; _LL1114:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1197=( char*)"null";
struct _tagged_string _temp1198; _temp1198.curr= _temp1197; _temp1198.base=
_temp1197; _temp1198.last_plus_one= _temp1197 + 5; _temp1198;})); goto _LL1110;
_LL1116: Cyc_Absyndump_dump( xprintf("%d", _temp1143)); goto _LL1110; _LL1118:
Cyc_Absyndump_dump( xprintf("%u",( unsigned int) _temp1147)); goto _LL1110;
_LL1120: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1199=( char*)"'";
struct _tagged_string _temp1200; _temp1200.curr= _temp1199; _temp1200.base=
_temp1199; _temp1200.last_plus_one= _temp1199 + 2; _temp1200;})); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp1151)); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1201=( char*)"'"; struct _tagged_string _temp1202;
_temp1202.curr= _temp1201; _temp1202.base= _temp1201; _temp1202.last_plus_one=
_temp1201 + 2; _temp1202;})); goto _LL1110; _LL1122: Cyc_Absyndump_dump(
_temp1153); goto _LL1110; _LL1124: Cyc_Absyndump_dumpqvar( _temp1155->name);
goto _LL1110; _LL1126:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1157,( struct
_tagged_string)({ char* _temp1203=( char*)"$("; struct _tagged_string _temp1204;
_temp1204.curr= _temp1203; _temp1204.base= _temp1203; _temp1204.last_plus_one=
_temp1203 + 3; _temp1204;}),( struct _tagged_string)({ char* _temp1205=( char*)")";
struct _tagged_string _temp1206; _temp1206.curr= _temp1205; _temp1206.base=
_temp1205; _temp1206.last_plus_one= _temp1205 + 2; _temp1206;}),( struct
_tagged_string)({ char* _temp1207=( char*)","; struct _tagged_string _temp1208;
_temp1208.curr= _temp1207; _temp1208.base= _temp1207; _temp1208.last_plus_one=
_temp1207 + 2; _temp1208;})); goto _LL1110; _LL1128: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1209=( char*)"&"; struct _tagged_string _temp1210;
_temp1210.curr= _temp1209; _temp1210.base= _temp1209; _temp1210.last_plus_one=
_temp1209 + 2; _temp1210;})); Cyc_Absyndump_dumppat( _temp1159); goto _LL1110;
_LL1130: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1211=( char*)"*";
struct _tagged_string _temp1212; _temp1212.curr= _temp1211; _temp1212.base=
_temp1211; _temp1212.last_plus_one= _temp1211 + 2; _temp1212;})); Cyc_Absyndump_dumpqvar(
_temp1161->name); goto _LL1110; _LL1132: Cyc_Absyndump_dumpqvar( _temp1163);
goto _LL1110; _LL1134: Cyc_Absyndump_dumpqvar( _temp1169); Cyc_Absyndump_dumptvars(
_temp1167);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1165,( struct
_tagged_string)({ char* _temp1213=( char*)"("; struct _tagged_string _temp1214;
_temp1214.curr= _temp1213; _temp1214.base= _temp1213; _temp1214.last_plus_one=
_temp1213 + 2; _temp1214;}),( struct _tagged_string)({ char* _temp1215=( char*)")";
struct _tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base=
_temp1215; _temp1216.last_plus_one= _temp1215 + 2; _temp1216;}),( struct
_tagged_string)({ char* _temp1217=( char*)","; struct _tagged_string _temp1218;
_temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 2; _temp1218;})); goto _LL1110; _LL1136: Cyc_Absyndump_dumpqvar(
_temp1175); Cyc_Absyndump_dumptvars( _temp1173);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1171,( struct _tagged_string)({ char* _temp1219=( char*)"{"; struct
_tagged_string _temp1220; _temp1220.curr= _temp1219; _temp1220.base= _temp1219;
_temp1220.last_plus_one= _temp1219 + 2; _temp1220;}),( struct _tagged_string)({
char* _temp1221=( char*)"}"; struct _tagged_string _temp1222; _temp1222.curr=
_temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one= _temp1221 + 2;
_temp1222;}),( struct _tagged_string)({ char* _temp1223=( char*)","; struct
_tagged_string _temp1224; _temp1224.curr= _temp1223; _temp1224.base= _temp1223;
_temp1224.last_plus_one= _temp1223 + 2; _temp1224;})); goto _LL1110; _LL1138:
if( _temp1183->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1183->name))->v);} Cyc_Absyndump_dumptvars(
_temp1179);(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1177,( struct
_tagged_string)({ char* _temp1225=( char*)"{"; struct _tagged_string _temp1226;
_temp1226.curr= _temp1225; _temp1226.base= _temp1225; _temp1226.last_plus_one=
_temp1225 + 2; _temp1226;}),( struct _tagged_string)({ char* _temp1227=( char*)"}";
struct _tagged_string _temp1228; _temp1228.curr= _temp1227; _temp1228.base=
_temp1227; _temp1228.last_plus_one= _temp1227 + 2; _temp1228;}),( struct
_tagged_string)({ char* _temp1229=( char*)","; struct _tagged_string _temp1230;
_temp1230.curr= _temp1229; _temp1230.base= _temp1229; _temp1230.last_plus_one=
_temp1229 + 2; _temp1230;})); goto _LL1110; _LL1140: Cyc_Absyndump_dumpqvar(
_temp1189->name); Cyc_Absyndump_dumptvars( _temp1187); if( _temp1185 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat, _temp1185,( struct _tagged_string)({ char* _temp1231=(
char*)"("; struct _tagged_string _temp1232; _temp1232.curr= _temp1231; _temp1232.base=
_temp1231; _temp1232.last_plus_one= _temp1231 + 2; _temp1232;}),( struct
_tagged_string)({ char* _temp1233=( char*)")"; struct _tagged_string _temp1234;
_temp1234.curr= _temp1233; _temp1234.base= _temp1233; _temp1234.last_plus_one=
_temp1233 + 2; _temp1234;}),( struct _tagged_string)({ char* _temp1235=( char*)",";
struct _tagged_string _temp1236; _temp1236.curr= _temp1235; _temp1236.base=
_temp1235; _temp1236.last_plus_one= _temp1235 + 2; _temp1236;}));} goto _LL1110;
_LL1142: Cyc_Absyndump_dumpqvar( _temp1193->name); goto _LL1110; _LL1110:;} void
Cyc_Absyndump_dumptunionfield( struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar(
ef->name); if( ef->typs != 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,( struct _tagged_string)({ char* _temp1237=(
char*)","; struct _tagged_string _temp1238; _temp1238.curr= _temp1237; _temp1238.base=
_temp1237; _temp1238.last_plus_one= _temp1237 + 2; _temp1238;}));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1239=( char*)" = ";
struct _tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base=
_temp1239; _temp1240.last_plus_one= _temp1239 + 4; _temp1240;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( ef->tag)));}} void
Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)( void(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield, fields,( struct
_tagged_string)({ char* _temp1241=( char*)","; struct _tagged_string _temp1242;
_temp1242.curr= _temp1241; _temp1242.base= _temp1241; _temp1242.last_plus_one=
_temp1241 + 2; _temp1242;}));} void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ struct Cyc_Absyn_Structfield _temp1245; struct Cyc_List_List*
_temp1246; struct Cyc_Core_Opt* _temp1248; void* _temp1250; struct Cyc_Absyn_Tqual
_temp1252; struct _tagged_string* _temp1254; struct Cyc_Absyn_Structfield*
_temp1243=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1245=* _temp1243; _LL1255: _temp1254=( struct _tagged_string*)
_temp1245.name; goto _LL1253; _LL1253: _temp1252=( struct Cyc_Absyn_Tqual)
_temp1245.tq; goto _LL1251; _LL1251: _temp1250=( void*) _temp1245.type; goto
_LL1249; _LL1249: _temp1248=( struct Cyc_Core_Opt*) _temp1245.width; goto
_LL1247; _LL1247: _temp1246=( struct Cyc_List_List*) _temp1245.attributes; goto
_LL1244; _LL1244:(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
_tagged_string*), struct _tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1252,
_temp1250, Cyc_Absyndump_dump_str, _temp1254); Cyc_Absyndump_dumpatts( _temp1246);
if( _temp1248 != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp1248))->v);}
Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl*
td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars( td->tvs);}
static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1256=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1280; struct Cyc_Absyn_Structdecl* _temp1282; struct Cyc_Absyn_Uniondecl*
_temp1284; struct Cyc_Absyn_Vardecl* _temp1286; struct Cyc_Absyn_Vardecl
_temp1288; struct Cyc_List_List* _temp1289; struct Cyc_Core_Opt* _temp1291;
struct Cyc_Absyn_Exp* _temp1293; void* _temp1295; struct Cyc_Absyn_Tqual
_temp1297; struct _tuple0* _temp1299; void* _temp1301; struct Cyc_Absyn_Tuniondecl*
_temp1303; struct Cyc_Absyn_Tuniondecl _temp1305; int _temp1306; struct Cyc_Core_Opt*
_temp1308; struct Cyc_List_List* _temp1310; struct _tuple0* _temp1312; void*
_temp1314; struct Cyc_Absyn_Enumdecl* _temp1316; struct Cyc_Absyn_Enumdecl
_temp1318; struct Cyc_Core_Opt* _temp1319; struct _tuple0* _temp1321; void*
_temp1323; int _temp1325; struct Cyc_Absyn_Exp* _temp1327; struct Cyc_Core_Opt*
_temp1329; struct Cyc_Core_Opt* _temp1331; struct Cyc_Absyn_Pat* _temp1333;
struct Cyc_Absyn_Typedefdecl* _temp1335; struct Cyc_List_List* _temp1337; struct
_tagged_string* _temp1339; struct Cyc_List_List* _temp1341; struct _tuple0*
_temp1343; struct Cyc_List_List* _temp1345; _LL1258: if(*(( int*) _temp1256) ==
Cyc_Absyn_Fn_d){ _LL1281: _temp1280=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1256)->f1; goto _LL1259;} else{ goto _LL1260;} _LL1260: if(*(( int*)
_temp1256) == Cyc_Absyn_Struct_d){ _LL1283: _temp1282=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1256)->f1; goto _LL1261;} else{ goto
_LL1262;} _LL1262: if(*(( int*) _temp1256) == Cyc_Absyn_Union_d){ _LL1285:
_temp1284=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1256)->f1; goto _LL1263;} else{ goto _LL1264;} _LL1264: if(*(( int*)
_temp1256) == Cyc_Absyn_Var_d){ _LL1287: _temp1286=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1256)->f1; _temp1288=* _temp1286; _LL1302:
_temp1301=( void*) _temp1288.sc; goto _LL1300; _LL1300: _temp1299=( struct
_tuple0*) _temp1288.name; goto _LL1298; _LL1298: _temp1297=( struct Cyc_Absyn_Tqual)
_temp1288.tq; goto _LL1296; _LL1296: _temp1295=( void*) _temp1288.type; goto
_LL1294; _LL1294: _temp1293=( struct Cyc_Absyn_Exp*) _temp1288.initializer; goto
_LL1292; _LL1292: _temp1291=( struct Cyc_Core_Opt*) _temp1288.rgn; goto _LL1290;
_LL1290: _temp1289=( struct Cyc_List_List*) _temp1288.attributes; goto _LL1265;}
else{ goto _LL1266;} _LL1266: if(*(( int*) _temp1256) == Cyc_Absyn_Tunion_d){
_LL1304: _temp1303=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1256)->f1; _temp1305=* _temp1303; _LL1315: _temp1314=( void*) _temp1305.sc;
goto _LL1313; _LL1313: _temp1312=( struct _tuple0*) _temp1305.name; goto _LL1311;
_LL1311: _temp1310=( struct Cyc_List_List*) _temp1305.tvs; goto _LL1309; _LL1309:
_temp1308=( struct Cyc_Core_Opt*) _temp1305.fields; goto _LL1307; _LL1307:
_temp1306=( int) _temp1305.is_xtunion; goto _LL1267;} else{ goto _LL1268;}
_LL1268: if(*(( int*) _temp1256) == Cyc_Absyn_Enum_d){ _LL1317: _temp1316=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1256)->f1;
_temp1318=* _temp1316; _LL1324: _temp1323=( void*) _temp1318.sc; goto _LL1322;
_LL1322: _temp1321=( struct _tuple0*) _temp1318.name; goto _LL1320; _LL1320:
_temp1319=( struct Cyc_Core_Opt*) _temp1318.fields; goto _LL1269;} else{ goto
_LL1270;} _LL1270: if(*(( int*) _temp1256) == Cyc_Absyn_Let_d){ _LL1334:
_temp1333=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1256)->f1;
goto _LL1332; _LL1332: _temp1331=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1256)->f2; goto _LL1330; _LL1330: _temp1329=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1256)->f3; goto _LL1328; _LL1328: _temp1327=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1256)->f4; goto
_LL1326; _LL1326: _temp1325=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1256)->f5;
goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*) _temp1256) == Cyc_Absyn_Typedef_d){
_LL1336: _temp1335=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1256)->f1; goto _LL1273;} else{ goto _LL1274;} _LL1274: if(*(( int*)
_temp1256) == Cyc_Absyn_Namespace_d){ _LL1340: _temp1339=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1256)->f1; goto _LL1338; _LL1338:
_temp1337=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1256)->f2; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*)
_temp1256) == Cyc_Absyn_Using_d){ _LL1344: _temp1343=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp1256)->f1; goto _LL1342; _LL1342: _temp1341=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1256)->f2; goto
_LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*) _temp1256) == Cyc_Absyn_ExternC_d){
_LL1346: _temp1345=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1256)->f1; goto _LL1279;} else{ goto _LL1257;} _LL1259: if( _temp1280->is_inline){
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1347=( char*)"inline";
struct _tagged_string _temp1348; _temp1348.curr= _temp1347; _temp1348.base=
_temp1347; _temp1348.last_plus_one= _temp1347 + 7; _temp1348;}));} Cyc_Absyndump_dumpscope((
void*) _temp1280->sc);{ void* t=( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1350=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1350[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1351; _temp1351.tag= Cyc_Absyn_FnType;
_temp1351.f1=({ struct Cyc_Absyn_FnInfo _temp1352; _temp1352.tvars= _temp1280->tvs;
_temp1352.effect= _temp1280->effect; _temp1352.ret_typ=( void*)(( void*)
_temp1280->ret_type); _temp1352.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1280->args); _temp1352.varargs= _temp1280->varargs; _temp1352.attributes= 0;
_temp1352;}); _temp1351;}); _temp1350;});(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1349; _temp1349.q_const= 0; _temp1349.q_volatile= 0;
_temp1349.q_restrict= 0; _temp1349;}), t, Cyc_Absyndump_dump_atts_qvar,
_temp1280); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1280->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1257;} _LL1261:
Cyc_Absyndump_dumpscope(( void*) _temp1282->sc); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1353=( char*)"struct"; struct _tagged_string
_temp1354; _temp1354.curr= _temp1353; _temp1354.base= _temp1353; _temp1354.last_plus_one=
_temp1353 + 7; _temp1354;})); if( _temp1282->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1282->name))->v);}
Cyc_Absyndump_dumptvars( _temp1282->tvs); if( _temp1282->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1282->fields))->v);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1355=( char*)"}";
struct _tagged_string _temp1356; _temp1356.curr= _temp1355; _temp1356.base=
_temp1355; _temp1356.last_plus_one= _temp1355 + 2; _temp1356;})); Cyc_Absyndump_dumpatts(
_temp1282->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1357=( char*)";"; struct _tagged_string _temp1358; _temp1358.curr=
_temp1357; _temp1358.base= _temp1357; _temp1358.last_plus_one= _temp1357 + 2;
_temp1358;}));} goto _LL1257; _LL1263: Cyc_Absyndump_dumpscope(( void*)
_temp1284->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1359=(
char*)"union"; struct _tagged_string _temp1360; _temp1360.curr= _temp1359;
_temp1360.base= _temp1359; _temp1360.last_plus_one= _temp1359 + 6; _temp1360;}));
if( _temp1284->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1284->name))->v);} Cyc_Absyndump_dumptvars(
_temp1284->tvs); if( _temp1284->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1284->fields))->v); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1361=( char*)"}"; struct _tagged_string
_temp1362; _temp1362.curr= _temp1361; _temp1362.base= _temp1361; _temp1362.last_plus_one=
_temp1361 + 2; _temp1362;})); Cyc_Absyndump_dumpatts( _temp1284->attributes);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1363=( char*)";";
struct _tagged_string _temp1364; _temp1364.curr= _temp1363; _temp1364.base=
_temp1363; _temp1364.last_plus_one= _temp1363 + 2; _temp1364;}));} goto _LL1257;
_LL1265: Cyc_Absyndump_dumpscope( _temp1301);(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct _tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)(
_temp1297, _temp1295, Cyc_Absyndump_dumpqvar, _temp1299); Cyc_Absyndump_dumpatts(
_temp1289); if( _temp1293 != 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp1293)));} Cyc_Absyndump_dump_semi();
goto _LL1257; _LL1267: Cyc_Absyndump_dumpscope( _temp1314); if( _temp1306){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1365=( char*)"xtunion "; struct
_tagged_string _temp1366; _temp1366.curr= _temp1365; _temp1366.base= _temp1365;
_temp1366.last_plus_one= _temp1365 + 9; _temp1366;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1367=( char*)"tunion "; struct
_tagged_string _temp1368; _temp1368.curr= _temp1367; _temp1368.base= _temp1367;
_temp1368.last_plus_one= _temp1367 + 8; _temp1368;}));} Cyc_Absyndump_dumpqvar(
_temp1312); Cyc_Absyndump_dumptvars( _temp1310); if( _temp1308 == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1308))->v); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1369=( char*)"};"; struct _tagged_string
_temp1370; _temp1370.curr= _temp1369; _temp1370.base= _temp1369; _temp1370.last_plus_one=
_temp1369 + 3; _temp1370;}));} goto _LL1257; _LL1269: Cyc_Absyndump_dumpscope(
_temp1323); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1371=( char*)"enum ";
struct _tagged_string _temp1372; _temp1372.curr= _temp1371; _temp1372.base=
_temp1371; _temp1372.last_plus_one= _temp1371 + 6; _temp1372;})); Cyc_Absyndump_dumpqvar(
_temp1321); if( _temp1319 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1319))->v); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({
char* _temp1373=( char*)"};"; struct _tagged_string _temp1374; _temp1374.curr=
_temp1373; _temp1374.base= _temp1373; _temp1374.last_plus_one= _temp1373 + 3;
_temp1374;}));} return; _LL1271: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1375=( char*)"let"; struct _tagged_string _temp1376; _temp1376.curr=
_temp1375; _temp1376.base= _temp1375; _temp1376.last_plus_one= _temp1375 + 4;
_temp1376;})); Cyc_Absyndump_dumppat( _temp1333); Cyc_Absyndump_dump_char(( int)'=');
Cyc_Absyndump_dumpexp( _temp1327); Cyc_Absyndump_dump_semi(); goto _LL1257;
_LL1273: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1377=( char*)"typedef";
struct _tagged_string _temp1378; _temp1378.curr= _temp1377; _temp1378.base=
_temp1377; _temp1378.last_plus_one= _temp1377 + 8; _temp1378;}));(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*), struct
Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1379; _temp1379.q_const= 0; _temp1379.q_volatile= 0; _temp1379.q_restrict=
0; _temp1379;}),( void*) _temp1335->defn, Cyc_Absyndump_dumptypedefname,
_temp1335); Cyc_Absyndump_dump_semi(); goto _LL1257; _LL1275: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1380=( char*)"namespace"; struct
_tagged_string _temp1381; _temp1381.curr= _temp1380; _temp1381.base= _temp1380;
_temp1381.last_plus_one= _temp1380 + 10; _temp1381;})); Cyc_Absyndump_dump_str(
_temp1339); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1337 != 0;
_temp1337=(( struct Cyc_List_List*) _check_null( _temp1337))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( _temp1337))->hd);}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL1257; _LL1277: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1382=( char*)"using"; struct _tagged_string
_temp1383; _temp1383.curr= _temp1382; _temp1383.base= _temp1382; _temp1383.last_plus_one=
_temp1382 + 6; _temp1383;})); Cyc_Absyndump_dumpqvar( _temp1343); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1341 != 0; _temp1341=(( struct Cyc_List_List*)
_check_null( _temp1341))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1341))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1257; _LL1279: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1384=( char*)"extern \"C\" {"; struct _tagged_string _temp1385;
_temp1385.curr= _temp1384; _temp1385.base= _temp1384; _temp1385.last_plus_one=
_temp1384 + 13; _temp1385;})); for( 0; _temp1345 != 0; _temp1345=(( struct Cyc_List_List*)
_check_null( _temp1345))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1345))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1257; _LL1257:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1386=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1404;
void* _temp1406; void* _temp1408; struct Cyc_Absyn_Exp* _temp1410; struct Cyc_Absyn_Tqual
_temp1412; void* _temp1414; void* _temp1416; struct Cyc_Absyn_Exp* _temp1418;
struct Cyc_Absyn_Tqual _temp1420; void* _temp1422; void* _temp1424; struct Cyc_Absyn_Tqual
_temp1426; void* _temp1428; struct Cyc_Absyn_Tvar* _temp1430; void* _temp1432;
struct Cyc_Absyn_Exp* _temp1434; struct Cyc_Absyn_Tqual _temp1436; void*
_temp1438; struct Cyc_Absyn_Tvar* _temp1440; void* _temp1442; struct Cyc_Absyn_Exp*
_temp1444; struct Cyc_Absyn_Tqual _temp1446; void* _temp1448; struct Cyc_Absyn_Tvar*
_temp1450; void* _temp1452; struct Cyc_Absyn_Tqual _temp1454; void* _temp1456;
void* _temp1458; _LL1388: if(( unsigned int) _temp1386 > 1u?*(( int*) _temp1386)
== Cyc_Absyn_Pointer_mod: 0){ _LL1409: _temp1408=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1386)->f1; if(( unsigned int) _temp1408 > 1u?*(( int*) _temp1408) == Cyc_Absyn_Nullable_ps:
0){ _LL1411: _temp1410=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1408)->f1; goto _LL1407;} else{ goto _LL1390;} _LL1407: _temp1406=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f2; if( _temp1406 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL1405;} else{ goto _LL1390;} _LL1405: _temp1404=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f3;
goto _LL1389;} else{ goto _LL1390;} _LL1390: if(( unsigned int) _temp1386 > 1u?*((
int*) _temp1386) == Cyc_Absyn_Pointer_mod: 0){ _LL1417: _temp1416=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f1; if(( unsigned int)
_temp1416 > 1u?*(( int*) _temp1416) == Cyc_Absyn_NonNullable_ps: 0){ _LL1419:
_temp1418=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1416)->f1; goto _LL1415;} else{ goto _LL1392;} _LL1415: _temp1414=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f2; if( _temp1414 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL1413;} else{ goto _LL1392;} _LL1413: _temp1412=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f3;
goto _LL1391;} else{ goto _LL1392;} _LL1392: if(( unsigned int) _temp1386 > 1u?*((
int*) _temp1386) == Cyc_Absyn_Pointer_mod: 0){ _LL1425: _temp1424=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f1; if( _temp1424 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL1423;} else{ goto _LL1394;} _LL1423:
_temp1422=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f2; if(
_temp1422 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1421;} else{ goto _LL1394;}
_LL1421: _temp1420=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1386)->f3; goto _LL1393;} else{ goto _LL1394;} _LL1394: if(( unsigned int)
_temp1386 > 1u?*(( int*) _temp1386) == Cyc_Absyn_Pointer_mod: 0){ _LL1433:
_temp1432=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f1; if((
unsigned int) _temp1432 > 1u?*(( int*) _temp1432) == Cyc_Absyn_Nullable_ps: 0){
_LL1435: _temp1434=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1432)->f1; goto _LL1429;} else{ goto _LL1396;} _LL1429: _temp1428=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f2; if(( unsigned int)
_temp1428 > 4u?*(( int*) _temp1428) == Cyc_Absyn_VarType: 0){ _LL1431: _temp1430=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp1428)->f1; goto
_LL1427;} else{ goto _LL1396;} _LL1427: _temp1426=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f3; goto _LL1395;} else{ goto
_LL1396;} _LL1396: if(( unsigned int) _temp1386 > 1u?*(( int*) _temp1386) == Cyc_Absyn_Pointer_mod:
0){ _LL1443: _temp1442=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1386)->f1; if(( unsigned int) _temp1442 > 1u?*(( int*) _temp1442) == Cyc_Absyn_NonNullable_ps:
0){ _LL1445: _temp1444=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1442)->f1; goto _LL1439;} else{ goto _LL1398;} _LL1439: _temp1438=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f2; if(( unsigned int)
_temp1438 > 4u?*(( int*) _temp1438) == Cyc_Absyn_VarType: 0){ _LL1441: _temp1440=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp1438)->f1; goto
_LL1437;} else{ goto _LL1398;} _LL1437: _temp1436=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f3; goto _LL1397;} else{ goto
_LL1398;} _LL1398: if(( unsigned int) _temp1386 > 1u?*(( int*) _temp1386) == Cyc_Absyn_Pointer_mod:
0){ _LL1453: _temp1452=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1386)->f1; if( _temp1452 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1449;}
else{ goto _LL1400;} _LL1449: _temp1448=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1386)->f2; if(( unsigned int) _temp1448 > 4u?*(( int*) _temp1448) == Cyc_Absyn_VarType:
0){ _LL1451: _temp1450=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1448)->f1; goto _LL1447;} else{ goto _LL1400;} _LL1447: _temp1446=( struct
Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f3; goto
_LL1399;} else{ goto _LL1400;} _LL1400: if(( unsigned int) _temp1386 > 1u?*((
int*) _temp1386) == Cyc_Absyn_Pointer_mod: 0){ _LL1459: _temp1458=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f1; goto _LL1457; _LL1457:
_temp1456=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1386)->f2; goto
_LL1455; _LL1455: _temp1454=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1386)->f3; goto _LL1401;} else{ goto _LL1402;} _LL1402: goto _LL1403;
_LL1389: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1410);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1391: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1418);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1393: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1395: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1434); Cyc_Absyndump_dump_str( _temp1430->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1397: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1444); Cyc_Absyndump_dump_str(
_temp1440->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1399: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1450->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1401:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1460=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1460[ 0]=({ struct Cyc_Core_Impossible_struct _temp1461; _temp1461.tag= Cyc_Core_Impossible;
_temp1461.f1=( struct _tagged_string)({ char* _temp1462=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1463; _temp1463.curr= _temp1462; _temp1463.base=
_temp1462; _temp1463.last_plus_one= _temp1462 + 25; _temp1463;}); _temp1461;});
_temp1460;})); _LL1403: { int next_is_pointer= 0; if((( struct Cyc_List_List*)
_check_null( tms))->tl != 0){ void* _temp1464=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual
_temp1470; void* _temp1472; void* _temp1474; _LL1466: if(( unsigned int)
_temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_Pointer_mod: 0){ _LL1475:
_temp1474=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1464)->f1; goto
_LL1473; _LL1473: _temp1472=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1464)->f2; goto _LL1471; _LL1471: _temp1470=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1464)->f3; goto _LL1467;} else{ goto
_LL1468;} _LL1468: goto _LL1469; _LL1467: next_is_pointer= 1; goto _LL1465;
_LL1469: goto _LL1465; _LL1465:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
if( next_is_pointer){ Cyc_Absyndump_dump_char(( int)')');}{ void* _temp1476=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp*
_temp1492; void* _temp1494; struct Cyc_Core_Opt* _temp1496; int _temp1498;
struct Cyc_List_List* _temp1500; void* _temp1502; struct Cyc_Position_Segment*
_temp1504; struct Cyc_List_List* _temp1506; int _temp1508; struct Cyc_Position_Segment*
_temp1510; struct Cyc_List_List* _temp1512; struct Cyc_List_List* _temp1514;
struct Cyc_Position_Segment* _temp1516; struct Cyc_Absyn_Tqual _temp1518; void*
_temp1520; void* _temp1522; _LL1478: if( _temp1476 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL1479;} else{ goto _LL1480;} _LL1480: if(( unsigned int) _temp1476 > 1u?*((
int*) _temp1476) == Cyc_Absyn_ConstArray_mod: 0){ _LL1493: _temp1492=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1476)->f1; goto
_LL1481;} else{ goto _LL1482;} _LL1482: if(( unsigned int) _temp1476 > 1u?*((
int*) _temp1476) == Cyc_Absyn_Function_mod: 0){ _LL1495: _temp1494=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp1476)->f1; if(*(( int*) _temp1494)
== Cyc_Absyn_WithTypes){ _LL1501: _temp1500=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp1494)->f1; goto _LL1499; _LL1499: _temp1498=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp1494)->f2; goto _LL1497; _LL1497:
_temp1496=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1494)->f3; goto _LL1483;} else{ goto _LL1484;}} else{ goto _LL1484;}
_LL1484: if(( unsigned int) _temp1476 > 1u?*(( int*) _temp1476) == Cyc_Absyn_Function_mod:
0){ _LL1503: _temp1502=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1476)->f1; if(*(( int*) _temp1502) == Cyc_Absyn_NoTypes){ _LL1507:
_temp1506=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1502)->f1;
goto _LL1505; _LL1505: _temp1504=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1502)->f2; goto _LL1485;} else{ goto _LL1486;}} else{ goto _LL1486;}
_LL1486: if(( unsigned int) _temp1476 > 1u?*(( int*) _temp1476) == Cyc_Absyn_TypeParams_mod:
0){ _LL1513: _temp1512=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1476)->f1; goto _LL1511; _LL1511: _temp1510=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1476)->f2; goto _LL1509; _LL1509:
_temp1508=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1476)->f3; goto
_LL1487;} else{ goto _LL1488;} _LL1488: if(( unsigned int) _temp1476 > 1u?*((
int*) _temp1476) == Cyc_Absyn_Attributes_mod: 0){ _LL1517: _temp1516=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1476)->f1;
goto _LL1515; _LL1515: _temp1514=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1476)->f2; goto _LL1489;} else{ goto _LL1490;} _LL1490: if(( unsigned int)
_temp1476 > 1u?*(( int*) _temp1476) == Cyc_Absyn_Pointer_mod: 0){ _LL1523:
_temp1522=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1476)->f1; goto
_LL1521; _LL1521: _temp1520=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1476)->f2; goto _LL1519; _LL1519: _temp1518=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1476)->f3; goto _LL1491;} else{ goto
_LL1477;} _LL1479: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1524=(
char*)"[]"; struct _tagged_string _temp1525; _temp1525.curr= _temp1524;
_temp1525.base= _temp1524; _temp1525.last_plus_one= _temp1524 + 3; _temp1525;}));
goto _LL1477; _LL1481: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1492); Cyc_Absyndump_dump_char(( int)']'); goto _LL1477; _LL1483: Cyc_Absyndump_dumpfunargs(
_temp1500, _temp1498, _temp1496); goto _LL1477; _LL1485:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1506,( struct _tagged_string)({ char* _temp1526=( char*)"("; struct
_tagged_string _temp1527; _temp1527.curr= _temp1526; _temp1527.base= _temp1526;
_temp1527.last_plus_one= _temp1526 + 2; _temp1527;}),( struct _tagged_string)({
char* _temp1528=( char*)")"; struct _tagged_string _temp1529; _temp1529.curr=
_temp1528; _temp1529.base= _temp1528; _temp1529.last_plus_one= _temp1528 + 2;
_temp1529;}),( struct _tagged_string)({ char* _temp1530=( char*)","; struct
_tagged_string _temp1531; _temp1531.curr= _temp1530; _temp1531.base= _temp1530;
_temp1531.last_plus_one= _temp1530 + 2; _temp1531;})); goto _LL1477; _LL1487:
if( _temp1508){ Cyc_Absyndump_dumpkindedtvars( _temp1512);} else{ Cyc_Absyndump_dumptvars(
_temp1512);} goto _LL1477; _LL1489: Cyc_Absyndump_dumpatts( _temp1514); goto
_LL1477; _LL1491:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1532=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1532[ 0]=({ struct Cyc_Core_Impossible_struct _temp1533; _temp1533.tag= Cyc_Core_Impossible;
_temp1533.f1=( struct _tagged_string)({ char* _temp1534=( char*)"dumptms";
struct _tagged_string _temp1535; _temp1535.curr= _temp1534; _temp1535.base=
_temp1534; _temp1535.last_plus_one= _temp1534 + 8; _temp1535;}); _temp1533;});
_temp1532;})); _LL1477:;} return;} _LL1387:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1538; void* _temp1540; struct Cyc_Absyn_Tqual _temp1542; struct _tuple4
_temp1536= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual) tq, t); _LL1543:
_temp1542= _temp1536.f1; goto _LL1541; _LL1541: _temp1540= _temp1536.f2; goto
_LL1539; _LL1539: _temp1538= _temp1536.f3; goto _LL1537; _LL1537: Cyc_Absyndump_dumptq(
_temp1542); Cyc_Absyndump_dumpntyp( _temp1540);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1538), f, a);} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=(( struct Cyc_List_List*)
_check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( tdl))->hd);}}