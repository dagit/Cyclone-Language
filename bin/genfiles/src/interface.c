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
Cyc_Core_opt_t; extern void* Cyc_Core_identity( void*); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct
_tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern
unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int
Cyc_Stdio_fpos_t; extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern
unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern unsigned char Cyc_Position_Nocontext[ 14u]; typedef
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl*
Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo* cyc_varargs,
struct Cyc_List_List*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct
Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern void* Cyc_Dict_fold( void*(* f)(
void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum); extern void Cyc_Dict_iter_c(
void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d);
static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j=
0; static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(); struct Cyc_Interface_I; typedef struct Cyc_Interface_I* Cyc_Interface_i_t;
extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern struct Cyc_Interface_I*
Cyc_Interface_final(); extern struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_string f1; struct
_tagged_string f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); typedef struct Cyc_Interface_I*
Cyc_interface_t; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
var_default_init, struct Cyc_List_List* ds); extern unsigned char Cyc_Lexing_Error[
10u]; struct Cyc_Lexing_Error_struct{ unsigned char* tag; struct _tagged_string
f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*);
void* refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_List_List* Cyc_Parse_parse_file( struct
Cyc_Stdio___sFILE* f); typedef void* Cyc_struct_or_union_t; typedef void* Cyc_blockitem_t;
typedef void* Cyc_type_specifier_t; typedef void* Cyc_storage_class_t; struct
Cyc_Declaration_spec; typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t;
struct Cyc_Declarator; typedef struct Cyc_Declarator* Cyc_declarator_t; struct
Cyc_Abstractdeclarator; typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Bool_tok[ 13u];
struct Cyc_Bool_tok_struct{ unsigned char* tag; int f1; } ; extern unsigned char
Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{ unsigned char* tag;
unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclSpec_tok[
17u]; struct Cyc_DeclSpec_tok_struct{ unsigned char* tag; struct Cyc_Declaration_spec*
f1; } ; extern unsigned char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple4* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple5{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple5* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct _tuple6{ struct Cyc_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ unsigned char*
tag; struct _tuple6* f1; } ; extern unsigned char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple7{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple7* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple8{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; } ; struct
Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct _tuple8* f1; } ;
extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple9{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple9* f1; } ; extern unsigned char Cyc_Rgn_tok[ 12u];
struct Cyc_Rgn_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{ unsigned char*
tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{
unsigned char* tag; short f1; } ; extern unsigned char Cyc_Stmt_tok[ 13u];
struct Cyc_Stmt_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Stmt* f1; } ;
extern unsigned char Cyc_StorageClass_tok[ 21u]; struct Cyc_StorageClass_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_String_tok[ 15u];
struct Cyc_String_tok_struct{ unsigned char* tag; struct _tagged_string f1; } ;
extern unsigned char Cyc_Stringopt_tok[ 18u]; struct Cyc_Stringopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_StructFieldDeclListList_tok[
32u]; struct Cyc_StructFieldDeclListList_tok_struct{ unsigned char* tag; struct
Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[ 28u];
struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[
25u]; struct Cyc_SwitchClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionFieldList_tok[ 24u]; struct Cyc_TunionFieldList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionField_tok[
20u]; struct Cyc_TunionField_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; extern unsigned char Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[
25u]; struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; extern
int Cyc_String_strptrcmp( struct _tagged_string* s1, struct _tagged_string* s2);
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_is_function_type( void* t); extern unsigned char
Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl*
base; struct Cyc_Absyn_Tunionfield* field; } ; typedef struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_xtunionfielddecl_t; extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_string* msg0, struct _tagged_string msg1); extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern void Cyc_Lex_lex_init();
typedef struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Interface_xtunionfielddecl_t;
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; typedef struct Cyc_Interface_Ienv* Cyc_Interface_ienv_t;
struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports; struct Cyc_Dict_Dict*
exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){ return({
struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*) GC_malloc(
sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});} static
struct Cyc_Interface_Ienv* Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv*
ie= 0; if( ie == 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();}
return( struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)((
struct Cyc_Interface_Ienv*(*)( void* ignored)) Cyc_Interface_skel2i_f, skel);
_temp1->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(* f)(
void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct Cyc_Interface_Ienv*(*)(
void* ignored)) Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I*
Cyc_Interface_empty(){ return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
_temp3= Cyc_Interface_empty(); goto _LL4; _LL4: { struct Cyc_Interface_Ienv*
_temp5= Cyc_Interface_new_ienv(); goto _LL6; _LL6: { struct Cyc_Absyn_Tuniondecl*
_temp8=({ struct Cyc_Absyn_Tuniondecl* _temp7=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp7[ 0]=* Cyc_Absyn_exn_xed;
_temp7;}); goto _LL9; _LL9:( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));
_temp5->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp5->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp8); _temp3->exports=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp5);{
struct Cyc_Interface_Ienv* _temp10= Cyc_Interface_new_ienv(); goto _LL11; _LL11: {
struct _tuple0* qmain=({ struct _tuple0* _temp18=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp18->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp22=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp22[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp23; _temp23.tag= Cyc_Absyn_Abs_n;
_temp23.f1= 0; _temp23;}); _temp22;}); _temp18->f2=({ struct _tagged_string*
_temp19=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp19[ 0]=({ unsigned char* _temp20="main"; struct _tagged_string _temp21;
_temp21.curr= _temp20; _temp21.base= _temp20; _temp21.last_plus_one= _temp20 + 5;
_temp21;}); _temp19;}); _temp18;}); struct Cyc_Absyn_Vardecl* _temp16= Cyc_Absyn_new_vardecl(
qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({ struct Cyc_List_List*
_temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp12->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_uint_t; _temp15;}); _temp12->tl=({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp14=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp14->f1= 0; _temp14->f2= Cyc_Absyn_empty_tqual();
_temp14->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp14;}); _temp13->tl= 0; _temp13;}); _temp12;}), 0,
0, 0), 0); goto _LL17; _LL17:( void*)( _temp16->sc=( void*)(( void*) Cyc_Absyn_Extern));
_temp10->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp10->vardecls,(*
qmain).f2, _temp16); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp10); return _temp3;}}}}} static void Cyc_Interface_err(
struct _tagged_string msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_string sc, struct _tagged_string t, struct _tuple0* n){ Cyc_Interface_err(({
struct _tagged_string _temp24= sc; struct _tagged_string _temp25= t; struct
_tagged_string _temp26= Cyc_Absynpp_qvar2string( n); xprintf("fields of %.*s %.*s %.*s have never been defined",
_temp24.last_plus_one - _temp24.curr, _temp24.curr, _temp25.last_plus_one -
_temp25.curr, _temp25.curr, _temp26.last_plus_one - _temp26.curr, _temp26.curr);}));}
static void Cyc_Interface_body_err( struct _tagged_string sc, struct _tuple0* n){
Cyc_Interface_err(({ struct _tagged_string _temp27= sc; struct _tagged_string
_temp28= Cyc_Absynpp_qvar2string( n); xprintf("the body of %.*s function %.*s has never been defined",
_temp27.last_plus_one - _temp27.curr, _temp27.curr, _temp28.last_plus_one -
_temp28.curr, _temp28.curr);}));} static void Cyc_Interface_static_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp29= obj1;
struct _tagged_string _temp30= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp31= obj2; struct _tagged_string _temp32= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s", _temp29.last_plus_one
- _temp29.curr, _temp29.curr, _temp30.last_plus_one - _temp30.curr, _temp30.curr,
_temp31.last_plus_one - _temp31.curr, _temp31.curr, _temp32.last_plus_one -
_temp32.curr, _temp32.curr);}));} static void Cyc_Interface_abstract_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp33= obj1;
struct _tagged_string _temp34= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp35= obj2; struct _tagged_string _temp36= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr, _temp35.last_plus_one - _temp35.curr, _temp35.curr,
_temp36.last_plus_one - _temp36.curr, _temp36.curr);}));} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; typedef struct Cyc_Interface_Seen* Cyc_Interface_seen_t; static
struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({ struct Cyc_Interface_Seen*
_temp37=( struct Cyc_Interface_Seen*) GC_malloc( sizeof( struct Cyc_Interface_Seen));
_temp37->structs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp37->unions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp37->tunions=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp37;});} typedef struct Cyc_Dict_Dict* Cyc_Interface_aenv_t; static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp38= 1; goto _LL39; _LL39: for( 0; l != 0; l=(( struct Cyc_List_List*)
_check_null( l))->tl){ if( ! Cyc_Interface_check_public_type( ae, seen, obj,
name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){ _temp38= 0;}}
return _temp38;} static int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp40; _push_handler(& _temp40);{ int _temp42= 0; if( setjmp(
_temp40.handler)){ _temp42= 1;} if( ! _temp42){{ int _temp43=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp43;}; _pop_handler();} else{ void* _temp41=( void*) _exn_thrown;
void* _temp45= _temp41; _LL47: if( _temp45 == Cyc_Dict_Absent){ goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { int _temp51= 1; goto _LL52; _LL52:
seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp51); if( d->fields != 0){
struct Cyc_List_List* _temp53=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL54; _LL54: for( 0; _temp53 != 0; _temp53=((
struct Cyc_List_List*) _check_null( _temp53))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp55="struct"; struct _tagged_string _temp56;
_temp56.curr= _temp55; _temp56.base= _temp55; _temp56.last_plus_one= _temp55 + 7;
_temp56;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp53))->hd)->type)){ _temp51= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp51);
return _temp51;} _LL50:( void) _throw( _temp45); _LL46:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp57; _push_handler(& _temp57);{ int _temp59= 0; if(
setjmp( _temp57.handler)){ _temp59= 1;} if( ! _temp59){{ int _temp60=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp60;}; _pop_handler();} else{ void* _temp58=(
void*) _exn_thrown; void* _temp62= _temp58; _LL64: if( _temp62 == Cyc_Dict_Absent){
goto _LL65;} else{ goto _LL66;} _LL66: goto _LL67; _LL65: { int _temp68= 1; goto
_LL69; _LL69: seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp68); if( d->fields != 0){
struct Cyc_List_List* _temp70=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL71; _LL71: for( 0; _temp70 != 0; _temp70=((
struct Cyc_List_List*) _check_null( _temp70))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp72="union"; struct _tagged_string _temp73;
_temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one= _temp72 + 6;
_temp73;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,(
void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp70))->hd)->type)){ _temp68= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp68);
return _temp68;} _LL67:( void) _throw( _temp62); _LL63:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl*
d){ struct _handler_cons _temp74; _push_handler(& _temp74);{ int _temp76= 0; if(
setjmp( _temp74.handler)){ _temp76= 1;} if( ! _temp76){{ int _temp77=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions,
d->name); _npop_handler( 0u); return _temp77;}; _pop_handler();} else{ void*
_temp75=( void*) _exn_thrown; void* _temp79= _temp75; _LL81: if( _temp79 == Cyc_Dict_Absent){
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL82: { int _temp85= 1; goto
_LL86; _LL86: seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp85); if( d->fields != 0){ struct Cyc_List_List* _temp87=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; goto _LL88; _LL88: for( 0;
_temp87 != 0; _temp87=(( struct Cyc_List_List*) _check_null( _temp87))->tl){ if(
!(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct
_tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen,({
unsigned char* _temp89="[x]tunion"; struct _tagged_string _temp90; _temp90.curr=
_temp89; _temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 10; _temp90;}),
d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp87))->hd)->typs)){ _temp85= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions,
d->name, _temp85); return _temp85;} _LL84:( void) _throw( _temp79); _LL80:;}}}
static int Cyc_Interface_check_public_enumdecl( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl* d){ return 1;} static int
Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Typedefdecl* d){ return Cyc_Interface_check_public_type(
ae, seen,({ unsigned char* _temp91="typedef"; struct _tagged_string _temp92;
_temp92.curr= _temp91; _temp92.base= _temp91; _temp92.last_plus_one= _temp91 + 8;
_temp92;}), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen,({ unsigned char* _temp93="variable";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 9; _temp94;}), d->name,( void*) d->type);}
static void* Cyc_Interface_get_type2( struct _tuple1* x){ return(* x).f3;}
static struct Cyc_List_List* Cyc_Interface_get_abs_ns( void* ns){ void* _temp95=
ns; struct Cyc_List_List* _temp101; _LL97: if(( unsigned int) _temp95 > 1u?*((
int*) _temp95) == Cyc_Absyn_Abs_n: 0){ _LL102: _temp101=(( struct Cyc_Absyn_Abs_n_struct*)
_temp95)->f1; goto _LL98;} else{ goto _LL99;} _LL99: goto _LL100; _LL98: return
_temp101; _LL100:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp103=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp103[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp104; _temp104.tag= Cyc_Core_InvalidArg;
_temp104.f1=({ unsigned char* _temp105="get_abs_ns"; struct _tagged_string
_temp106; _temp106.curr= _temp105; _temp106.base= _temp105; _temp106.last_plus_one=
_temp105 + 11; _temp106;}); _temp104;}); _temp103;})); _LL96:;} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t){ void* _temp107= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp129; struct Cyc_Absyn_Conref* _temp131; struct Cyc_Absyn_Tqual
_temp133; struct Cyc_Absyn_Conref* _temp135; void* _temp137; void* _temp139;
struct Cyc_Absyn_Exp* _temp141; struct Cyc_Absyn_Tqual _temp143; void* _temp145;
struct Cyc_Core_Opt* _temp147; struct Cyc_Core_Opt _temp149; void* _temp150;
struct Cyc_List_List* _temp152; struct _tuple0* _temp154; struct Cyc_Absyn_FnInfo
_temp156; struct Cyc_List_List* _temp158; struct Cyc_Absyn_VarargInfo* _temp160;
int _temp162; struct Cyc_List_List* _temp164; void* _temp166; struct Cyc_Core_Opt*
_temp168; struct Cyc_List_List* _temp170; struct Cyc_List_List* _temp172; struct
Cyc_Absyn_Structdecl** _temp174; struct Cyc_Absyn_Structdecl* _temp176; struct
Cyc_List_List* _temp177; struct _tuple0* _temp179; struct Cyc_Absyn_Uniondecl**
_temp181; struct Cyc_Absyn_Uniondecl* _temp183; struct Cyc_List_List* _temp184;
struct _tuple0* _temp186; struct Cyc_Absyn_TunionInfo _temp188; void* _temp190;
struct Cyc_List_List* _temp192; void* _temp194; struct Cyc_Absyn_Tuniondecl*
_temp196; struct Cyc_Absyn_TunionFieldInfo _temp198; struct Cyc_List_List*
_temp200; void* _temp202; struct Cyc_Absyn_Tunionfield* _temp204; struct Cyc_Absyn_Tuniondecl*
_temp206; _LL109: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_PointerType:
0){ _LL130: _temp129=(( struct Cyc_Absyn_PointerType_struct*) _temp107)->f1;
_LL140: _temp139=( void*) _temp129.elt_typ; goto _LL138; _LL138: _temp137=( void*)
_temp129.rgn_typ; goto _LL136; _LL136: _temp135= _temp129.nullable; goto _LL134;
_LL134: _temp133= _temp129.tq; goto _LL132; _LL132: _temp131= _temp129.bounds;
goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp107 > 4u?*((
int*) _temp107) == Cyc_Absyn_ArrayType: 0){ _LL146: _temp145=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp107)->f1; goto _LL144; _LL144: _temp143=((
struct Cyc_Absyn_ArrayType_struct*) _temp107)->f2; goto _LL142; _LL142: _temp141=((
struct Cyc_Absyn_ArrayType_struct*) _temp107)->f3; goto _LL112;} else{ goto
_LL113;} _LL113: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TypedefType:
0){ _LL155: _temp154=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f1;
goto _LL153; _LL153: _temp152=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f2;
goto _LL148; _LL148: _temp147=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f3;
if( _temp147 == 0){ goto _LL115;} else{ _temp149=* _temp147; _LL151: _temp150=(
void*) _temp149.v; goto _LL114;}} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_FnType: 0){ _LL157: _temp156=((
struct Cyc_Absyn_FnType_struct*) _temp107)->f1; _LL171: _temp170= _temp156.tvars;
goto _LL169; _LL169: _temp168= _temp156.effect; goto _LL167; _LL167: _temp166=(
void*) _temp156.ret_typ; goto _LL165; _LL165: _temp164= _temp156.args; goto
_LL163; _LL163: _temp162= _temp156.c_varargs; goto _LL161; _LL161: _temp160=
_temp156.cyc_varargs; goto _LL159; _LL159: _temp158= _temp156.attributes; goto
_LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp107 > 4u?*(( int*)
_temp107) == Cyc_Absyn_TupleType: 0){ _LL173: _temp172=(( struct Cyc_Absyn_TupleType_struct*)
_temp107)->f1; goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_StructType: 0){ _LL180: _temp179=((
struct Cyc_Absyn_StructType_struct*) _temp107)->f1; goto _LL178; _LL178:
_temp177=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f2; goto _LL175;
_LL175: _temp174=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f3; if(
_temp174 == 0){ goto _LL121;} else{ _temp176=* _temp174; goto _LL120;}} else{
goto _LL121;} _LL121: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) ==
Cyc_Absyn_UnionType: 0){ _LL187: _temp186=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f1; goto _LL185; _LL185: _temp184=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f2; goto _LL182; _LL182: _temp181=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f3; if( _temp181 == 0){ goto _LL123;} else{ _temp183=* _temp181; goto
_LL122;}} else{ goto _LL123;} _LL123: if(( unsigned int) _temp107 > 4u?*(( int*)
_temp107) == Cyc_Absyn_TunionType: 0){ _LL189: _temp188=(( struct Cyc_Absyn_TunionType_struct*)
_temp107)->f1; _LL195: _temp194=( void*) _temp188.tunion_info; if(*(( int*)
_temp194) == Cyc_Absyn_KnownTunion){ _LL197: _temp196=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp194)->f1; goto _LL193;} else{ goto _LL125;} _LL193: _temp192= _temp188.targs;
goto _LL191; _LL191: _temp190=( void*) _temp188.rgn; goto _LL124;} else{ goto
_LL125;} _LL125: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TunionFieldType:
0){ _LL199: _temp198=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp107)->f1;
_LL203: _temp202=( void*) _temp198.field_info; if(*(( int*) _temp202) == Cyc_Absyn_KnownTunionfield){
_LL207: _temp206=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp202)->f1;
goto _LL205; _LL205: _temp204=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp202)->f2; goto _LL201;} else{ goto _LL127;} _LL201: _temp200= _temp198.targs;
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL110: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp139); _LL112: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp145); _LL114: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp150); _LL116: { int b=(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp164)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp166): 0; if( _temp160 != 0){ int _temp210; void*
_temp212; void* _temp214; struct Cyc_Absyn_Tqual _temp216; struct Cyc_Core_Opt*
_temp218; struct Cyc_Absyn_VarargInfo _temp208=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp160)); _LL219: _temp218= _temp208.name; goto _LL217; _LL217:
_temp216= _temp208.tq; goto _LL215; _LL215: _temp214=( void*) _temp208.type;
goto _LL213; _LL213: _temp212=( void*) _temp208.rgn; goto _LL211; _LL211:
_temp210= _temp208.inject; goto _LL209; _LL209: b= Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp214);} return b;} _LL118: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp172); _LL120: if(( void*)
_temp176->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({
unsigned char* _temp220="struct"; struct _tagged_string _temp221; _temp221.curr=
_temp220; _temp221.base= _temp220; _temp221.last_plus_one= _temp220 + 7;
_temp221;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp176->name))->v);
return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj,
name,( void*(*)( void*)) Cyc_Core_identity, _temp177)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp176): 0; _LL122: if(( void*) _temp183->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ unsigned char* _temp222="union"; struct
_tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 6; _temp223;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp183->name))->v); return 0;} return(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp184)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp183): 0; _LL124: { struct _tuple0 _temp226; struct _tagged_string*
_temp227; void* _temp229; struct _tuple0* _temp224= _temp196->name; _temp226=*
_temp224; _LL230: _temp229= _temp226.f1; goto _LL228; _LL228: _temp227= _temp226.f2;
goto _LL225; _LL225: { struct Cyc_List_List* _temp231= Cyc_Interface_get_abs_ns(
_temp229); goto _LL232; _LL232: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp233; _push_handler(& _temp233);{ int _temp235= 0; if( setjmp(
_temp233.handler)){ _temp235= 1;} if( ! _temp235){{ struct Cyc_Tcenv_Genv*
_temp236=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp231); goto _LL237; _LL237: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp236->tuniondecls,
_temp227);}; _pop_handler();} else{ void* _temp234=( void*) _exn_thrown; void*
_temp239= _temp234; _LL241: if( _temp239 == Cyc_Dict_Absent){ goto _LL242;}
else{ goto _LL243;} _LL243: goto _LL244; _LL242:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp245=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp245[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp246; _temp246.tag= Cyc_Core_InvalidArg; _temp246.f1=({
unsigned char* _temp247="check_public_type"; struct _tagged_string _temp248;
_temp248.curr= _temp247; _temp248.base= _temp247; _temp248.last_plus_one=
_temp247 + 18; _temp248;}); _temp246;}); _temp245;})); _LL244:( void) _throw(
_temp239); _LL240:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,({ unsigned char* _temp249="[x]tunion"; struct _tagged_string _temp250;
_temp250.curr= _temp249; _temp250.base= _temp249; _temp250.last_plus_one=
_temp249 + 10; _temp250;}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp192)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}} _LL126: { struct _tuple0 _temp253; struct _tagged_string*
_temp254; void* _temp256; struct _tuple0* _temp251= _temp206->name; _temp253=*
_temp251; _LL257: _temp256= _temp253.f1; goto _LL255; _LL255: _temp254= _temp253.f2;
goto _LL252; _LL252: { struct Cyc_List_List* _temp258= Cyc_Interface_get_abs_ns(
_temp256); goto _LL259; _LL259: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp260; _push_handler(& _temp260);{ int _temp262= 0; if( setjmp(
_temp260.handler)){ _temp262= 1;} if( ! _temp262){{ struct Cyc_Tcenv_Genv*
_temp263=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp258); goto _LL264; _LL264: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp263->tuniondecls,
_temp254);}; _pop_handler();} else{ void* _temp261=( void*) _exn_thrown; void*
_temp266= _temp261; _LL268: if( _temp266 == Cyc_Dict_Absent){ goto _LL269;}
else{ goto _LL270;} _LL270: goto _LL271; _LL269:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp272=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp272[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp273; _temp273.tag= Cyc_Core_InvalidArg; _temp273.f1=({
unsigned char* _temp274="check_public_type"; struct _tagged_string _temp275;
_temp275.curr= _temp274; _temp275.base= _temp274; _temp275.last_plus_one=
_temp274 + 18; _temp275;}); _temp273;}); _temp272;})); _LL271:( void) _throw(
_temp266); _LL267:;}}} if( tud->fields == 0){( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp276=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp276[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp277; _temp277.tag= Cyc_Core_InvalidArg; _temp277.f1=({
unsigned char* _temp278="check_public_type"; struct _tagged_string _temp279;
_temp279.curr= _temp278; _temp279.base= _temp278; _temp279.last_plus_one=
_temp278 + 18; _temp279;}); _temp277;}); _temp276;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp280=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; goto _LL281; _LL281: for( 0; _temp280 != 0;
_temp280=(( struct Cyc_List_List*) _check_null( _temp280))->tl){ int cmp; if((
cmp= Cyc_Absyn_qvar_cmp( _temp204->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp280))->hd)->name)) >= 0){ if( cmp == 0){
tuf1=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp280))->hd);} break;}}} if( tuf1 == 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp282=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp282[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp283; _temp283.tag= Cyc_Core_InvalidArg; _temp283.f1=({
unsigned char* _temp284="check_public_type"; struct _tagged_string _temp285;
_temp285.curr= _temp284; _temp285.base= _temp284; _temp285.last_plus_one=
_temp284 + 18; _temp285;}); _temp283;}); _temp282;}));}{ struct Cyc_Absyn_Tunionfield*
tuf=( struct Cyc_Absyn_Tunionfield*) _check_null( tuf1); if(( void*) tud->sc ==(
void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({ unsigned char*
_temp286="[x]tunion"; struct _tagged_string _temp287; _temp287.curr= _temp286;
_temp287.base= _temp286; _temp287.last_plus_one= _temp286 + 10; _temp287;}), tud->name);
return 0;} if(( void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name,({ unsigned char* _temp288="[x]tunion"; struct _tagged_string _temp289;
_temp289.curr= _temp288; _temp289.base= _temp288; _temp289.last_plus_one=
_temp288 + 10; _temp289;}), tud->name); return 0;} if(( void*) tuf->sc ==( void*)
Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({ struct _tagged_string
_temp290= Cyc_Absynpp_qvar2string( tuf->name); xprintf("field %.*s of", _temp290.last_plus_one
- _temp290.curr, _temp290.curr);}), tud->name); return 0;} return(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp200)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}}} _LL128: return 1; _LL108:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp293; struct Cyc_Interface_I* _temp294; struct Cyc_Interface_Seen*
_temp296; struct Cyc_Dict_Dict* _temp298; int _temp300; struct Cyc_Interface_Ienv*
_temp302; struct Cyc_Interface_Ienv* _temp304; struct _tuple11* _temp291= env;
_temp293=* _temp291; _LL305: _temp304= _temp293.f1; goto _LL303; _LL303:
_temp302= _temp293.f2; goto _LL301; _LL301: _temp300= _temp293.f3; goto _LL299;
_LL299: _temp298= _temp293.f4; goto _LL297; _LL297: _temp296= _temp293.f5; goto
_LL295; _LL295: _temp294= _temp293.f6; goto _LL292; _LL292: { struct Cyc_Absyn_Structdecl*
_temp306=* dp; goto _LL307; _LL307: { void* _temp308=( void*) _temp306->sc;
_LL310: if( _temp308 ==( void*) Cyc_Absyn_Static){ goto _LL311;} else{ goto
_LL312;} _LL312: if( _temp308 ==( void*) Cyc_Absyn_Abstract){ goto _LL313;}
else{ goto _LL314;} _LL314: if( _temp308 ==( void*) Cyc_Absyn_Public){ goto
_LL315;} else{ goto _LL316;} _LL316: if( _temp308 ==( void*) Cyc_Absyn_ExternC){
goto _LL317;} else{ goto _LL318;} _LL318: if( _temp308 ==( void*) Cyc_Absyn_Extern){
goto _LL319;} else{ goto _LL309;} _LL311: if( _temp300? _temp306->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp320="static"; struct
_tagged_string _temp321; _temp321.curr= _temp320; _temp321.base= _temp320;
_temp321.last_plus_one= _temp320 + 7; _temp321;}),({ unsigned char* _temp322="struct";
struct _tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 7; _temp323;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp306->name))->v);} goto _LL309; _LL313: if(
_temp306->fields == 0){ if( _temp300){ Cyc_Interface_fields_err(({ unsigned char*
_temp324="abstract"; struct _tagged_string _temp325; _temp325.curr= _temp324;
_temp325.base= _temp324; _temp325.last_plus_one= _temp324 + 9; _temp325;}),({
unsigned char* _temp326="struct"; struct _tagged_string _temp327; _temp327.curr=
_temp326; _temp327.base= _temp326; _temp327.last_plus_one= _temp326 + 7;
_temp327;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp306->name))->v);}}
else{ _temp306=({ struct Cyc_Absyn_Structdecl* _temp328=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp328[ 0]=* _temp306;
_temp328;}); _temp306->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp298, _temp296, _temp306)){ _temp302->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp302->structdecls, x, _temp306);} goto _LL309;
_LL315: if( _temp306->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp329="public"; struct _tagged_string _temp330; _temp330.curr= _temp329;
_temp330.base= _temp329; _temp330.last_plus_one= _temp329 + 7; _temp330;}),({
unsigned char* _temp331="struct"; struct _tagged_string _temp332; _temp332.curr=
_temp331; _temp332.base= _temp331; _temp332.last_plus_one= _temp331 + 7;
_temp332;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp306->name))->v);
_temp306=({ struct Cyc_Absyn_Structdecl* _temp333=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp333[ 0]=* _temp306;
_temp333;});( void*)( _temp306->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_structdecl( _temp298, _temp296, _temp306)){ _temp302->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp302->structdecls, x,
_temp306);} goto _LL309; _LL317:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp334=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp334[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp335; _temp335.tag= Cyc_Core_InvalidArg;
_temp335.f1=({ unsigned char* _temp336="extract_structdecl"; struct
_tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 19; _temp337;}); _temp335;}); _temp334;}));
_LL319: if( Cyc_Interface_check_public_structdecl( _temp298, _temp296, _temp306)){
_temp304->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp304->structdecls, x, _temp306);} goto _LL309; _LL309:;}}} static void Cyc_Interface_extract_uniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple11 _temp340; struct Cyc_Interface_I* _temp341; struct Cyc_Interface_Seen*
_temp343; struct Cyc_Dict_Dict* _temp345; int _temp347; struct Cyc_Interface_Ienv*
_temp349; struct Cyc_Interface_Ienv* _temp351; struct _tuple11* _temp338= env;
_temp340=* _temp338; _LL352: _temp351= _temp340.f1; goto _LL350; _LL350:
_temp349= _temp340.f2; goto _LL348; _LL348: _temp347= _temp340.f3; goto _LL346;
_LL346: _temp345= _temp340.f4; goto _LL344; _LL344: _temp343= _temp340.f5; goto
_LL342; _LL342: _temp341= _temp340.f6; goto _LL339; _LL339: { struct Cyc_Absyn_Uniondecl*
_temp353=* dp; goto _LL354; _LL354: { void* _temp355=( void*) _temp353->sc;
_LL357: if( _temp355 ==( void*) Cyc_Absyn_Static){ goto _LL358;} else{ goto
_LL359;} _LL359: if( _temp355 ==( void*) Cyc_Absyn_Abstract){ goto _LL360;}
else{ goto _LL361;} _LL361: if( _temp355 ==( void*) Cyc_Absyn_Public){ goto
_LL362;} else{ goto _LL363;} _LL363: if( _temp355 ==( void*) Cyc_Absyn_ExternC){
goto _LL364;} else{ goto _LL365;} _LL365: if( _temp355 ==( void*) Cyc_Absyn_Extern){
goto _LL366;} else{ goto _LL356;} _LL358: if( _temp347? _temp353->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp367="static"; struct
_tagged_string _temp368; _temp368.curr= _temp367; _temp368.base= _temp367;
_temp368.last_plus_one= _temp367 + 7; _temp368;}),({ unsigned char* _temp369="union";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 6; _temp370;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp353->name))->v);} goto _LL356; _LL360: if(
_temp353->fields == 0){ if( _temp347){ Cyc_Interface_fields_err(({ unsigned char*
_temp371="abstract"; struct _tagged_string _temp372; _temp372.curr= _temp371;
_temp372.base= _temp371; _temp372.last_plus_one= _temp371 + 9; _temp372;}),({
unsigned char* _temp373="union"; struct _tagged_string _temp374; _temp374.curr=
_temp373; _temp374.base= _temp373; _temp374.last_plus_one= _temp373 + 6;
_temp374;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp353->name))->v);}}
else{ _temp353=({ struct Cyc_Absyn_Uniondecl* _temp375=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp375[ 0]=* _temp353;
_temp375;}); _temp353->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp345, _temp343, _temp353)){ _temp349->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp349->uniondecls, x, _temp353);} goto _LL356;
_LL362: if( _temp353->fields == 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp376="public"; struct _tagged_string _temp377; _temp377.curr= _temp376;
_temp377.base= _temp376; _temp377.last_plus_one= _temp376 + 7; _temp377;}),({
unsigned char* _temp378="union"; struct _tagged_string _temp379; _temp379.curr=
_temp378; _temp379.base= _temp378; _temp379.last_plus_one= _temp378 + 6;
_temp379;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp353->name))->v);
_temp353=({ struct Cyc_Absyn_Uniondecl* _temp380=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp380[ 0]=* _temp353;
_temp380;});( void*)( _temp353->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if(
Cyc_Interface_check_public_uniondecl( _temp345, _temp343, _temp353)){ _temp349->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp349->uniondecls, x,
_temp353);} goto _LL356; _LL364:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp381=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp381[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp382; _temp382.tag= Cyc_Core_InvalidArg;
_temp382.f1=({ unsigned char* _temp383="extract_uniondecl"; struct
_tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 18; _temp384;}); _temp382;}); _temp381;}));
_LL366: if( Cyc_Interface_check_public_uniondecl( _temp345, _temp343, _temp353)){
_temp351->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp351->uniondecls, x, _temp353);} goto _LL356; _LL356:;}}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp387; struct Cyc_Interface_I* _temp388; struct Cyc_Interface_Seen*
_temp390; struct Cyc_Dict_Dict* _temp392; int _temp394; struct Cyc_Interface_Ienv*
_temp396; struct Cyc_Interface_Ienv* _temp398; struct _tuple11* _temp385= env;
_temp387=* _temp385; _LL399: _temp398= _temp387.f1; goto _LL397; _LL397:
_temp396= _temp387.f2; goto _LL395; _LL395: _temp394= _temp387.f3; goto _LL393;
_LL393: _temp392= _temp387.f4; goto _LL391; _LL391: _temp390= _temp387.f5; goto
_LL389; _LL389: _temp388= _temp387.f6; goto _LL386; _LL386: { struct Cyc_Absyn_Enumdecl*
_temp400=* dp; goto _LL401; _LL401: { void* _temp402=( void*) _temp400->sc;
_LL404: if( _temp402 ==( void*) Cyc_Absyn_Static){ goto _LL405;} else{ goto
_LL406;} _LL406: if( _temp402 ==( void*) Cyc_Absyn_Abstract){ goto _LL407;}
else{ goto _LL408;} _LL408: if( _temp402 ==( void*) Cyc_Absyn_Public){ goto
_LL409;} else{ goto _LL410;} _LL410: if( _temp402 ==( void*) Cyc_Absyn_ExternC){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp402 ==( void*) Cyc_Absyn_Extern){
goto _LL413;} else{ goto _LL403;} _LL405: if( _temp394? _temp400->fields == 0: 0){
Cyc_Interface_fields_err(({ unsigned char* _temp414="static"; struct
_tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 7; _temp415;}),({ unsigned char* _temp416="enum";
struct _tagged_string _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 5; _temp417;}), _temp400->name);} goto _LL403;
_LL407: if( _temp400->fields == 0){ if( _temp394){ Cyc_Interface_fields_err(({
unsigned char* _temp418="abstract"; struct _tagged_string _temp419; _temp419.curr=
_temp418; _temp419.base= _temp418; _temp419.last_plus_one= _temp418 + 9;
_temp419;}),({ unsigned char* _temp420="enum"; struct _tagged_string _temp421;
_temp421.curr= _temp420; _temp421.base= _temp420; _temp421.last_plus_one=
_temp420 + 5; _temp421;}), _temp400->name);}} else{ _temp400=({ struct Cyc_Absyn_Enumdecl*
_temp422=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp422[ 0]=* _temp400; _temp422;}); _temp400->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp392, _temp390, _temp400)){ _temp396->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp396->enumdecls, x, _temp400);} goto _LL403; _LL409:
if( _temp400->fields == 0){ Cyc_Interface_fields_err(({ unsigned char* _temp423="public";
struct _tagged_string _temp424; _temp424.curr= _temp423; _temp424.base= _temp423;
_temp424.last_plus_one= _temp423 + 7; _temp424;}),({ unsigned char* _temp425="enum";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 5; _temp426;}), _temp400->name); _temp400=({
struct Cyc_Absyn_Enumdecl* _temp427=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp427[ 0]=* _temp400; _temp427;});( void*)(
_temp400->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl(
_temp392, _temp390, _temp400)){ _temp396->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp396->enumdecls, x, _temp400);} goto _LL403; _LL411:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp428=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp428[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp429; _temp429.tag= Cyc_Core_InvalidArg; _temp429.f1=({
unsigned char* _temp430="extract_enumdecl"; struct _tagged_string _temp431;
_temp431.curr= _temp430; _temp431.base= _temp430; _temp431.last_plus_one=
_temp430 + 17; _temp431;}); _temp429;}); _temp428;})); _LL413: if( Cyc_Interface_check_public_enumdecl(
_temp392, _temp390, _temp400)){ _temp398->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp398->enumdecls, x, _temp400);} goto _LL403; _LL403:;}}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp432= 0; goto _LL433; _LL433:{ void* _temp434=(* f->name).f1; struct Cyc_List_List*
_temp442; struct Cyc_List_List* _temp444; _LL436: if(( unsigned int) _temp434 >
1u?*(( int*) _temp434) == Cyc_Absyn_Rel_n: 0){ _LL443: _temp442=(( struct Cyc_Absyn_Rel_n_struct*)
_temp434)->f1; goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int)
_temp434 > 1u?*(( int*) _temp434) == Cyc_Absyn_Abs_n: 0){ _LL445: _temp444=((
struct Cyc_Absyn_Abs_n_struct*) _temp434)->f1; goto _LL439;} else{ goto _LL440;}
_LL440: goto _LL441; _LL437: _temp444= _temp442; goto _LL439; _LL439: _temp432=
_temp444; goto _LL435; _LL441: goto _LL435; _LL435:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp446=( void*) f->sc; _LL448: if( _temp446 ==( void*) Cyc_Absyn_Static){
goto _LL449;} else{ goto _LL450;} _LL450: if( _temp446 ==( void*) Cyc_Absyn_Extern){
goto _LL451;} else{ goto _LL452;} _LL452: if( _temp446 ==( void*) Cyc_Absyn_Public){
goto _LL453;} else{ goto _LL454;} _LL454: goto _LL455; _LL449: return; _LL451:
dict=& i->imports; goto _LL447; _LL453: dict=& i->exports; goto _LL447; _LL455:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp456=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp456[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp457; _temp457.tag= Cyc_Core_InvalidArg; _temp457.f1=({
unsigned char* _temp458="add_xtunionfielddecl"; struct _tagged_string _temp459;
_temp459.curr= _temp458; _temp459.base= _temp458; _temp459.last_plus_one=
_temp458 + 21; _temp459;}); _temp457;}); _temp456;})); _LL447:;}{ struct Cyc_Interface_Ienv*
env;{ struct _handler_cons _temp460; _push_handler(& _temp460);{ int _temp462= 0;
if( setjmp( _temp460.handler)){ _temp462= 1;} if( ! _temp462){ env=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict,
_temp432);; _pop_handler();} else{ void* _temp461=( void*) _exn_thrown; void*
_temp464= _temp461; _LL466: if( _temp464 == Cyc_Dict_Absent){ goto _LL467;}
else{ goto _LL468;} _LL468: goto _LL469; _LL467: env= Cyc_Interface_new_ienv();*
dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* dict, _temp432, env);
goto _LL465; _LL469:( void) _throw( _temp464); _LL465:;}}} env->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( env->xtunionfielddecls,(*
f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl* _temp470=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp470->base= d;
_temp470->field= f; _temp470;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp473; struct Cyc_Interface_I* _temp474; struct Cyc_Interface_Seen*
_temp476; struct Cyc_Dict_Dict* _temp478; int _temp480; struct Cyc_Interface_Ienv*
_temp482; struct Cyc_Interface_Ienv* _temp484; struct _tuple11* _temp471= env;
_temp473=* _temp471; _LL485: _temp484= _temp473.f1; goto _LL483; _LL483:
_temp482= _temp473.f2; goto _LL481; _LL481: _temp480= _temp473.f3; goto _LL479;
_LL479: _temp478= _temp473.f4; goto _LL477; _LL477: _temp476= _temp473.f5; goto
_LL475; _LL475: _temp474= _temp473.f6; goto _LL472; _LL472: { struct Cyc_Absyn_Tuniondecl*
_temp486=* dp; goto _LL487; _LL487: { void* _temp488=( void*) _temp486->sc;
_LL490: if( _temp488 ==( void*) Cyc_Absyn_Static){ goto _LL491;} else{ goto
_LL492;} _LL492: if( _temp488 ==( void*) Cyc_Absyn_Abstract){ goto _LL493;}
else{ goto _LL494;} _LL494: if( _temp488 ==( void*) Cyc_Absyn_Public){ goto
_LL495;} else{ goto _LL496;} _LL496: if( _temp488 ==( void*) Cyc_Absyn_ExternC){
goto _LL497;} else{ goto _LL498;} _LL498: if( _temp488 ==( void*) Cyc_Absyn_Extern){
goto _LL499;} else{ goto _LL489;} _LL491: if(( ! _temp486->is_xtunion? _temp480:
0)? _temp486->fields == 0: 0){ Cyc_Interface_fields_err(({ unsigned char*
_temp500="static"; struct _tagged_string _temp501; _temp501.curr= _temp500;
_temp501.base= _temp500; _temp501.last_plus_one= _temp500 + 7; _temp501;}),({
unsigned char* _temp502="tunion"; struct _tagged_string _temp503; _temp503.curr=
_temp502; _temp503.base= _temp502; _temp503.last_plus_one= _temp502 + 7;
_temp503;}), _temp486->name);} goto _LL489; _LL493: if( _temp486->fields == 0){
if( ! _temp486->is_xtunion? _temp480: 0){ Cyc_Interface_fields_err(({
unsigned char* _temp504="abstract"; struct _tagged_string _temp505; _temp505.curr=
_temp504; _temp505.base= _temp504; _temp505.last_plus_one= _temp504 + 9;
_temp505;}),({ unsigned char* _temp506="tunion"; struct _tagged_string _temp507;
_temp507.curr= _temp506; _temp507.base= _temp506; _temp507.last_plus_one=
_temp506 + 7; _temp507;}), _temp486->name);}} else{ _temp486=({ struct Cyc_Absyn_Tuniondecl*
_temp508=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp508[ 0]=* _temp486; _temp508;}); _temp486->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp478, _temp476, _temp486)){ _temp482->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp482->tuniondecls, x, _temp486);} goto _LL489;
_LL495: _temp486=({ struct Cyc_Absyn_Tuniondecl* _temp509=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp509[ 0]=* _temp486;
_temp509;}); if( ! _temp486->is_xtunion? _temp486->fields == 0: 0){ Cyc_Interface_fields_err(({
unsigned char* _temp510="public"; struct _tagged_string _temp511; _temp511.curr=
_temp510; _temp511.base= _temp510; _temp511.last_plus_one= _temp510 + 7;
_temp511;}),({ unsigned char* _temp512="tunion"; struct _tagged_string _temp513;
_temp513.curr= _temp512; _temp513.base= _temp512; _temp513.last_plus_one=
_temp512 + 7; _temp513;}), _temp486->name);( void*)( _temp486->sc=( void*)((
void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp478, _temp476, _temp486)){ if( _temp486->is_xtunion? _temp486->fields != 0:
0){ struct Cyc_List_List* _temp514=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp486->fields))->v; goto _LL515; _LL515: _temp486->fields= 0;{
struct Cyc_List_List* _temp516= _temp514; goto _LL517; _LL517: for( 0; _temp516
!= 0; _temp516=(( struct Cyc_List_List*) _check_null( _temp516))->tl){ Cyc_Interface_extract_xtunionfielddecl(
_temp474, _temp486,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp516))->hd);}}} _temp482->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp482->tuniondecls, x, _temp486);} goto _LL489;
_LL497:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp518=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp518[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp519; _temp519.tag= Cyc_Core_InvalidArg;
_temp519.f1=({ unsigned char* _temp520="extract_tuniondecl"; struct
_tagged_string _temp521; _temp521.curr= _temp520; _temp521.base= _temp520;
_temp521.last_plus_one= _temp520 + 19; _temp521;}); _temp519;}); _temp518;}));
_LL499: if( Cyc_Interface_check_public_tuniondecl( _temp478, _temp476, _temp486)){
if( _temp486->is_xtunion? _temp486->fields != 0: 0){ _temp486=({ struct Cyc_Absyn_Tuniondecl*
_temp522=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp522[ 0]=* _temp486; _temp522;});{ struct Cyc_List_List* _temp523=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp486->fields))->v; goto
_LL524; _LL524: _temp486->fields= 0;{ struct Cyc_List_List* _temp525= _temp523;
goto _LL526; _LL526: for( 0; _temp525 != 0; _temp525=(( struct Cyc_List_List*)
_check_null( _temp525))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp474,
_temp486,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp525))->hd);}}}} _temp484->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp484->tuniondecls, x, _temp486);} goto _LL489; _LL489:;}}}
static void Cyc_Interface_extract_typedef( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){ struct _tuple11 _temp529;
struct Cyc_Interface_I* _temp530; struct Cyc_Interface_Seen* _temp532; struct
Cyc_Dict_Dict* _temp534; int _temp536; struct Cyc_Interface_Ienv* _temp538;
struct Cyc_Interface_Ienv* _temp540; struct _tuple11* _temp527= env; _temp529=*
_temp527; _LL541: _temp540= _temp529.f1; goto _LL539; _LL539: _temp538= _temp529.f2;
goto _LL537; _LL537: _temp536= _temp529.f3; goto _LL535; _LL535: _temp534=
_temp529.f4; goto _LL533; _LL533: _temp532= _temp529.f5; goto _LL531; _LL531:
_temp530= _temp529.f6; goto _LL528; _LL528: if( Cyc_Interface_check_public_typedefdecl(
_temp534, _temp532, d)){ _temp538->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp538->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_string* x, struct _tuple7* v){ struct
_tuple11 _temp544; struct Cyc_Interface_I* _temp545; struct Cyc_Interface_Seen*
_temp547; struct Cyc_Dict_Dict* _temp549; int _temp551; struct Cyc_Interface_Ienv*
_temp553; struct Cyc_Interface_Ienv* _temp555; struct _tuple11* _temp542= env;
_temp544=* _temp542; _LL556: _temp555= _temp544.f1; goto _LL554; _LL554:
_temp553= _temp544.f2; goto _LL552; _LL552: _temp551= _temp544.f3; goto _LL550;
_LL550: _temp549= _temp544.f4; goto _LL548; _LL548: _temp547= _temp544.f5; goto
_LL546; _LL546: _temp545= _temp544.f6; goto _LL543; _LL543: { void* _temp557=(*
v).f1; goto _LL558; _LL558: { void* _temp559= _temp557; void* _temp565; _LL561:
if(*(( int*) _temp559) == Cyc_Tcenv_VarRes){ _LL566: _temp565=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp559)->f1; goto _LL562;} else{ goto _LL563;}
_LL563: goto _LL564; _LL562:{ void* _temp567= _temp565; struct Cyc_Absyn_Fndecl*
_temp575; struct Cyc_Absyn_Vardecl* _temp577; _LL569: if(( unsigned int)
_temp567 > 1u?*(( int*) _temp567) == Cyc_Absyn_Funname_b: 0){ _LL576: _temp575=((
struct Cyc_Absyn_Funname_b_struct*) _temp567)->f1; goto _LL570;} else{ goto
_LL571;} _LL571: if(( unsigned int) _temp567 > 1u?*(( int*) _temp567) == Cyc_Absyn_Global_b:
0){ _LL578: _temp577=(( struct Cyc_Absyn_Global_b_struct*) _temp567)->f1; goto
_LL572;} else{ goto _LL573;} _LL573: goto _LL574; _LL570: { struct Cyc_Absyn_Vardecl*
_temp581=({ struct Cyc_Absyn_Vardecl* _temp579=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp579->sc=( void*)(( void*)
_temp575->sc); _temp579->name= _temp575->name; _temp579->tq=({ struct Cyc_Absyn_Tqual
_temp580; _temp580.q_const= 0; _temp580.q_volatile= 0; _temp580.q_restrict= 0;
_temp580;}); _temp579->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp575->cached_typ))->v); _temp579->initializer= 0; _temp579->rgn=
0; _temp579->attributes= 0; _temp579;}); goto _LL582; _LL582: _temp551= 0;
_temp577= _temp581; goto _LL572;} _LL572: if( _temp577->initializer != 0){
_temp577=({ struct Cyc_Absyn_Vardecl* _temp583=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp583[ 0]=* _temp577; _temp583;});
_temp577->initializer= 0;}{ void* _temp584=( void*) _temp577->sc; _LL586: if(
_temp584 ==( void*) Cyc_Absyn_Static){ goto _LL587;} else{ goto _LL588;} _LL588:
if( _temp584 ==( void*) Cyc_Absyn_Abstract){ goto _LL589;} else{ goto _LL590;}
_LL590: if( _temp584 ==( void*) Cyc_Absyn_Public){ goto _LL591;} else{ goto
_LL592;} _LL592: if( _temp584 ==( void*) Cyc_Absyn_ExternC){ goto _LL593;} else{
goto _LL594;} _LL594: if( _temp584 ==( void*) Cyc_Absyn_Extern){ goto _LL595;}
else{ goto _LL585;} _LL587: if( _temp551? Cyc_Tcutil_is_function_type(( void*)
_temp577->type): 0){ Cyc_Interface_body_err(({ unsigned char* _temp596="static";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 7; _temp597;}), _temp577->name);} goto _LL585;
_LL589:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp598=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp598[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp599; _temp599.tag= Cyc_Core_InvalidArg;
_temp599.f1=({ unsigned char* _temp600="extract_ordinarie"; struct
_tagged_string _temp601; _temp601.curr= _temp600; _temp601.base= _temp600;
_temp601.last_plus_one= _temp600 + 18; _temp601;}); _temp599;}); _temp598;}));
_LL591: if( _temp551? Cyc_Tcutil_is_function_type(( void*) _temp577->type): 0){
Cyc_Interface_body_err(({ unsigned char* _temp602="public"; struct
_tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 7; _temp603;}), _temp577->name);} if( Cyc_Interface_check_public_vardecl(
_temp549, _temp547, _temp577)){ _temp553->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( _temp553->vardecls, x, _temp577);} goto _LL585; _LL593:
goto _LL595; _LL595: if( Cyc_Interface_check_public_vardecl( _temp549, _temp547,
_temp577)){ _temp555->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp555->vardecls, x, _temp577);} goto _LL585; _LL585:;} goto _LL568; _LL574:
goto _LL568; _LL568:;} goto _LL560; _LL564: goto _LL560; _LL560:;}}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp606; struct Cyc_Interface_Seen* _temp607; struct Cyc_Dict_Dict* _temp609;
int _temp611; struct Cyc_Interface_I* _temp613; struct _tuple12* _temp604= env_f;
_temp606=* _temp604; _LL614: _temp613= _temp606.f1; goto _LL612; _LL612:
_temp611= _temp606.f2; goto _LL610; _LL610: _temp609= _temp606.f3; goto _LL608;
_LL608: _temp607= _temp606.f4; goto _LL605; _LL605: { struct _tuple11 _temp616=({
struct _tuple11 _temp615; _temp615.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp613->imports,
ns); _temp615.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp613->exports, ns); _temp615.f3=
_temp611; _temp615.f4= _temp609; _temp615.f5= _temp607; _temp615.f6= _temp613;
_temp615;}); goto _LL617; _LL617:(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& _temp616,
ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct _tagged_string*,
struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp616, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp616, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp616, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp616, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp616, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp619=({ struct _tuple12
_temp618; _temp618.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp618.f2= check_complete_defs; _temp618.f3= ae;
_temp618.f4= Cyc_Interface_new_seen(); _temp618;}); goto _LL620; _LL620:(( void(*)(
void(* f)( struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*),
struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp619, ae); return _temp619.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_string* msg1, struct
_tagged_string msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_string*);
struct _tagged_string f4; struct _tagged_string* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_string* x, void* y1){ struct _tuple13
_temp623; struct _tagged_string* _temp624; struct _tagged_string _temp626; int(*
_temp628)( void*, void*, struct _tagged_string*); struct Cyc_Dict_Dict* _temp630;
int _temp632; int* _temp634; struct _tuple13* _temp621= env; _temp623=* _temp621;
_LL633: _temp632= _temp623.f1; _temp634=&(* _temp621).f1; goto _LL631; _LL631:
_temp630= _temp623.f2; goto _LL629; _LL629: _temp628= _temp623.f3; goto _LL627;
_LL627: _temp626= _temp623.f4; goto _LL625; _LL625: _temp624= _temp623.f5; goto
_LL622; _LL622: { struct _handler_cons _temp635; _push_handler(& _temp635);{ int
_temp637= 0; if( setjmp( _temp635.handler)){ _temp637= 1;} if( ! _temp637){{
void* _temp638=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp630, x); goto _LL639; _LL639: if( ! _temp628( y1,
_temp638, _temp624)){* _temp634= 0;}}; _pop_handler();} else{ void* _temp636=(
void*) _exn_thrown; void* _temp641= _temp636; _LL643: if( _temp641 == Cyc_Dict_Absent){
goto _LL644;} else{ goto _LL645;} _LL645: goto _LL646; _LL644: Cyc_Interface_check_err(
_temp624,({ struct _tagged_string _temp647= _temp626; struct _tagged_string
_temp648=* x; xprintf("%.*s %.*s is missing", _temp647.last_plus_one - _temp647.curr,
_temp647.curr, _temp648.last_plus_one - _temp648.curr, _temp648.curr);}));*
_temp634= 0; goto _LL642; _LL646:( void) _throw( _temp641); _LL642:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple13 _temp650=({
struct _tuple13 _temp649; _temp649.f1= 1; _temp649.f2= dic2; _temp649.f3= incl_f;
_temp649.f4= t; _temp649.f5= msg; _temp649;}); goto _LL651; _LL651:(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_string*, void*), struct _tuple13*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& _temp650, dic1);
return _temp650.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Structdecl*
_temp652= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); goto _LL653; _LL653: if(
_temp652 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp652){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp654= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp654.last_plus_one - _temp654.curr, _temp654.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct
Cyc_Absyn_Uniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Uniondecl*
_temp655= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); goto _LL656; _LL656: if(
_temp655 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 != _temp655){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp657= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp657.last_plus_one - _temp657.curr, _temp657.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Tuniondecl*
_temp658= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); goto _LL659; _LL659: if(
_temp658 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 != _temp658){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp660= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp660.last_plus_one - _temp660.curr, _temp660.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Enumdecl*
_temp661= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); goto _LL662; _LL662: if(
_temp661 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp661){ Cyc_Interface_check_err(
msg,({ struct _tagged_string _temp663= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of enum %.*s discloses too much information", _temp663.last_plus_one
- _temp663.curr, _temp663.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct
_tagged_string* msg){ struct Cyc_Absyn_Vardecl* _temp664= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL665; _LL665: if( _temp664 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp664){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp666= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of variable %.*s discloses too much information",
_temp666.last_plus_one - _temp666.curr, _temp666.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0,
struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Typedefdecl*
_temp667= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); goto _LL668; _LL668:
if( _temp667 == 0){ return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 !=
_temp667){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp669= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_temp669.last_plus_one - _temp669.curr, _temp669.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_string* msg){ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp670= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1,
0, msg); goto _LL671; _LL671: if( _temp670 == 0){ return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*)
d0 != _temp670){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp672=
Cyc_Absynpp_qvar2string(( d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp672.last_plus_one - _temp672.curr, _temp672.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ int _temp675=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl,({
unsigned char* _temp673="struct"; struct _tagged_string _temp674; _temp674.curr=
_temp673; _temp674.base= _temp673; _temp674.last_plus_one= _temp673 + 7;
_temp674;}), msg); goto _LL676; _LL676: { int _temp679=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl,({ unsigned char* _temp677="union"; struct
_tagged_string _temp678; _temp678.curr= _temp677; _temp678.base= _temp677;
_temp678.last_plus_one= _temp677 + 6; _temp678;}), msg); goto _LL680; _LL680: {
int _temp683=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl,({
unsigned char* _temp681="tunion"; struct _tagged_string _temp682; _temp682.curr=
_temp681; _temp682.base= _temp681; _temp682.last_plus_one= _temp681 + 7;
_temp682;}), msg); goto _LL684; _LL684: { int _temp687=(( int(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls,
Cyc_Interface_incl_enumdecl,({ unsigned char* _temp685="enum"; struct
_tagged_string _temp686; _temp686.curr= _temp685; _temp686.base= _temp685;
_temp686.last_plus_one= _temp685 + 5; _temp686;}), msg); goto _LL688; _LL688: {
int _temp691=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_strptrcmp), Cyc_Tcdecl_merge_typedefdecl,({
unsigned char* _temp689="typedef"; struct _tagged_string _temp690; _temp690.curr=
_temp689; _temp690.base= _temp689; _temp690.last_plus_one= _temp689 + 8;
_temp690;}), msg) != 0; goto _LL692; _LL692: { int _temp695=(( int(*)( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->vardecls, ie2->vardecls,
Cyc_Interface_incl_vardecl,({ unsigned char* _temp693="variable"; struct
_tagged_string _temp694; _temp694.curr= _temp693; _temp694.base= _temp693;
_temp694.last_plus_one= _temp693 + 9; _temp694;}), msg); goto _LL696; _LL696: {
int _temp699=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,({
unsigned char* _temp697="xtunionfield"; struct _tagged_string _temp698; _temp698.curr=
_temp697; _temp698.base= _temp697; _temp698.last_plus_one= _temp697 + 13;
_temp698;}), msg); goto _LL700; _LL700: return((((( _temp675? _temp679: 0)?
_temp683: 0)? _temp687: 0)? _temp691: 0)? _temp695: 0)? _temp699: 0;}}}}}}}
struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct _tagged_string* f3; }
; static void Cyc_Interface_incl_ns_f( struct _tuple14* env, struct Cyc_List_List*
ns, struct Cyc_Interface_Ienv* ie1){ struct _tuple14 _temp703; struct
_tagged_string* _temp704; struct Cyc_Dict_Dict* _temp706; int _temp708; int*
_temp710; struct _tuple14* _temp701= env; _temp703=* _temp701; _LL709: _temp708=
_temp703.f1; _temp710=&(* _temp701).f1; goto _LL707; _LL707: _temp706= _temp703.f2;
goto _LL705; _LL705: _temp704= _temp703.f3; goto _LL702; _LL702: { struct Cyc_Interface_Ienv*
ie2;{ struct _handler_cons _temp711; _push_handler(& _temp711);{ int _temp713= 0;
if( setjmp( _temp711.handler)){ _temp713= 1;} if( ! _temp713){ ie2=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp706,
ns);; _pop_handler();} else{ void* _temp712=( void*) _exn_thrown; void* _temp715=
_temp712; _LL717: if( _temp715 == Cyc_Dict_Absent){ goto _LL718;} else{ goto
_LL719;} _LL719: goto _LL720; _LL718: ie2= Cyc_Interface_lazy_new_ienv(); goto
_LL716; _LL720:( void) _throw( _temp715); _LL716:;}}} if( ! Cyc_Interface_incl_ienv(
ie1, ie2, _temp704)){* _temp710= 0;}}} static int Cyc_Interface_incl_ns( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string* msg){
struct _tuple14 _temp722=({ struct _tuple14 _temp721; _temp721.f1= 1; _temp721.f2=
dic2; _temp721.f3= msg; _temp721;}); goto _LL723; _LL723:(( void(*)( void(* f)(
struct _tuple14*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_ns_f,&
_temp722, dic1); return _temp722.f1;} int Cyc_Interface_is_subinterface( struct
Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct
_handler_cons _temp724; _push_handler(& _temp724);{ int _temp726= 0; if( setjmp(
_temp724.handler)){ _temp726= 1;} if( ! _temp726){{ int _temp727= 1; goto _LL728;
_LL728: { struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct
_tagged_string* _temp729=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp729[ 0]=({ struct _tagged_string _temp730=(*(( struct
_tuple3*) _check_null( info))).f1; struct _tagged_string _temp731=(*(( struct
_tuple3*) _check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp730.last_plus_one - _temp730.curr, _temp730.curr, _temp731.last_plus_one -
_temp731.curr, _temp731.curr);}); _temp729;});} if( ! Cyc_Interface_incl_ns( i1->exports,
i2->exports, msg)){ _temp727= 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp732=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp732[ 0]=({ struct _tagged_string _temp733=(*(( struct _tuple3*) _check_null(
info))).f2; struct _tagged_string _temp734=(*(( struct _tuple3*) _check_null(
info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp733.last_plus_one - _temp733.curr, _temp733.curr, _temp734.last_plus_one -
_temp734.curr, _temp734.curr);}); _temp732;});} if( ! Cyc_Interface_incl_ns( i2->imports,
i1->imports, msg)){ _temp727= 0;}{ int _temp735= _temp727; _npop_handler( 0u);
return _temp735;}}}; _pop_handler();} else{ void* _temp725=( void*) _exn_thrown;
void* _temp737= _temp725; _LL739: if( _temp737 == Cyc_Tcdecl_Incompatible){ goto
_LL740;} else{ goto _LL741;} _LL741: goto _LL742; _LL740: return 0; _LL742:(
void) _throw( _temp737); _LL738:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); struct
_tagged_string f6; struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f(
struct _tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15
_temp745; struct _tagged_string* _temp746; struct _tagged_string _temp748; void**(*
_temp750)( void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*);
struct Cyc_Dict_Dict* _temp752; struct Cyc_Dict_Dict* _temp754; struct Cyc_Dict_Dict*
_temp756; struct Cyc_Dict_Dict** _temp758; int _temp759; int* _temp761; struct
_tuple15* _temp743= env; _temp745=* _temp743; _LL760: _temp759= _temp745.f1;
_temp761=&(* _temp743).f1; goto _LL757; _LL757: _temp756= _temp745.f2; _temp758=&(*
_temp743).f2; goto _LL755; _LL755: _temp754= _temp745.f3; goto _LL753; _LL753:
_temp752= _temp745.f4; goto _LL751; _LL751: _temp750= _temp745.f5; goto _LL749;
_LL749: _temp748= _temp745.f6; goto _LL747; _LL747: _temp746= _temp745.f7; goto
_LL744; _LL744: { void** y;{ struct _handler_cons _temp762; _push_handler(&
_temp762);{ int _temp764= 0; if( setjmp( _temp762.handler)){ _temp764= 1;} if( !
_temp764){{ void** _temp765=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp754, x); goto _LL766; _LL766: {
void** _temp767= _temp750( _temp765, y2, 0, _temp746); goto _LL768; _LL768: if(
!(( unsigned int) _temp767)){* _temp761= 0; _npop_handler( 0u); return;} y=(
void**) _check_null( _temp767);}}; _pop_handler();} else{ void* _temp763=( void*)
_exn_thrown; void* _temp770= _temp763; _LL772: if( _temp770 == Cyc_Dict_Absent){
goto _LL773;} else{ goto _LL774;} _LL774: goto _LL775; _LL773: y= y2; goto
_LL771; _LL775:( void) _throw( _temp770); _LL771:;}}}{ struct _handler_cons
_temp776; _push_handler(& _temp776);{ int _temp778= 0; if( setjmp( _temp776.handler)){
_temp778= 1;} if( ! _temp778){{ void** _temp779=(( void**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp752, x); goto _LL780;
_LL780: { void** _temp781= _temp750( _temp779, y, 0, _temp746); goto _LL782;
_LL782: if( _temp781 !=( void**) _temp779){ if(( unsigned int) _temp781){ Cyc_Interface_check_err(
_temp746,({ struct _tagged_string _temp783= _temp748; struct _tagged_string
_temp784=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp783.last_plus_one - _temp783.curr, _temp783.curr, _temp784.last_plus_one -
_temp784.curr, _temp784.curr);}));}* _temp761= 0;}}}; _pop_handler();} else{
void* _temp777=( void*) _exn_thrown; void* _temp786= _temp777; _LL788: if(
_temp786 == Cyc_Dict_Absent){ goto _LL789;} else{ goto _LL790;} _LL790: goto
_LL791; _LL789: if(* _temp761){* _temp758=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(*
_temp758, x, y);} goto _LL787; _LL791:( void) _throw( _temp786); _LL787:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple15 _temp793=({
struct _tuple15 _temp792; _temp792.f1= 1; _temp792.f2= dic1; _temp792.f3= dic1;
_temp792.f4= excl; _temp792.f5= merge_f; _temp792.f6= t; _temp792.f7= msg;
_temp792;}); goto _LL794; _LL794:(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp793, dic2); if( _temp793.f1){ return({ struct Cyc_Core_Opt* _temp795=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp795->v=(
void*) _temp793.f2; _temp795;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp798=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->structdecls, ie2->structdecls, iexcl->structdecls,
Cyc_Tcdecl_merge_structdecl,({ unsigned char* _temp796="struct"; struct
_tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 7; _temp797;}), msg); goto _LL799; _LL799: {
struct Cyc_Core_Opt* _temp802=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(*
merge_f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls,
Cyc_Tcdecl_merge_uniondecl,({ unsigned char* _temp800="union"; struct
_tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 6; _temp801;}), msg); goto _LL803; _LL803: {
struct Cyc_Core_Opt* _temp806=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(*
merge_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls,
Cyc_Tcdecl_merge_tuniondecl,({ unsigned char* _temp804="tunion"; struct
_tagged_string _temp805; _temp805.curr= _temp804; _temp805.base= _temp804;
_temp805.last_plus_one= _temp804 + 7; _temp805;}), msg); goto _LL807; _LL807: {
struct Cyc_Core_Opt* _temp810=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(*
merge_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls,
Cyc_Tcdecl_merge_enumdecl,({ unsigned char* _temp808="enum"; struct
_tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 5; _temp809;}), msg); goto _LL811; _LL811: {
struct Cyc_Core_Opt* _temp814=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,
iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp812="typedef";
struct _tagged_string _temp813; _temp813.curr= _temp812; _temp813.base= _temp812;
_temp813.last_plus_one= _temp812 + 8; _temp813;}), msg); goto _LL815; _LL815: {
struct Cyc_Core_Opt* _temp818=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(*
merge_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->vardecls, ie2->vardecls, iexcl->vardecls,
Cyc_Tcdecl_merge_vardecl,({ unsigned char* _temp816="variable"; struct
_tagged_string _temp817; _temp817.curr= _temp816; _temp817.base= _temp816;
_temp817.last_plus_one= _temp816 + 9; _temp817;}), msg); goto _LL819; _LL819: {
struct Cyc_Core_Opt* _temp822=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(*
merge_f)( struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl,({
unsigned char* _temp820="xtunionfield"; struct _tagged_string _temp821; _temp821.curr=
_temp820; _temp821.base= _temp820; _temp821.last_plus_one= _temp820 + 13;
_temp821;}), msg); goto _LL823; _LL823: if(((((( !(( unsigned int) _temp798)? 1:
!(( unsigned int) _temp802))? 1: !(( unsigned int) _temp806))? 1: !((
unsigned int) _temp810))? 1: !(( unsigned int) _temp814))? 1: !(( unsigned int)
_temp818))? 1: !(( unsigned int) _temp822)){ return 0;} else{ return({ struct
Cyc_Interface_Ienv* _temp824=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof(
struct Cyc_Interface_Ienv)); _temp824->structdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp798))->v; _temp824->uniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp802))->v; _temp824->tuniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp806))->v;
_temp824->enumdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp810))->v; _temp824->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp814))->v; _temp824->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp818))->v; _temp824->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp822))->v; _temp824;});}}}}}}}}
struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple16 _temp827; struct _tagged_string* _temp828; struct Cyc_Dict_Dict*
_temp830; struct Cyc_Dict_Dict* _temp832; struct Cyc_Dict_Dict* _temp834; struct
Cyc_Dict_Dict** _temp836; int _temp837; int* _temp839; struct _tuple16* _temp825=
env; _temp827=* _temp825; _LL838: _temp837= _temp827.f1; _temp839=&(* _temp825).f1;
goto _LL835; _LL835: _temp834= _temp827.f2; _temp836=&(* _temp825).f2; goto
_LL833; _LL833: _temp832= _temp827.f3; goto _LL831; _LL831: _temp830= _temp827.f4;
goto _LL829; _LL829: _temp828= _temp827.f5; goto _LL826; _LL826: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp840;
_push_handler(& _temp840);{ int _temp842= 0; if( setjmp( _temp840.handler)){
_temp842= 1;} if( ! _temp842){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp830, ns);;
_pop_handler();} else{ void* _temp841=( void*) _exn_thrown; void* _temp844=
_temp841; _LL846: if( _temp844 == Cyc_Dict_Absent){ goto _LL847;} else{ goto
_LL848;} _LL848: goto _LL849; _LL847: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL845; _LL849:( void) _throw( _temp844); _LL845:;}}}{ struct _handler_cons
_temp850; _push_handler(& _temp850);{ int _temp852= 0; if( setjmp( _temp850.handler)){
_temp852= 1;} if( ! _temp852){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp832, ns);{ struct Cyc_Interface_Ienv*
_temp853= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp828); goto _LL854; _LL854: if( _temp853 == 0){* _temp839= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*) _check_null(
_temp853);}; _pop_handler();} else{ void* _temp851=( void*) _exn_thrown; void*
_temp856= _temp851; _LL858: if( _temp856 == Cyc_Dict_Absent){ goto _LL859;}
else{ goto _LL860;} _LL860: goto _LL861; _LL859: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL857; _LL861:( void) _throw( _temp856); _LL857:;}}}{ struct Cyc_Interface_Ienv*
_temp862= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp828); goto
_LL863; _LL863: if( _temp862 == 0){* _temp839= 0;} else{ if(* _temp839){*
_temp836=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp836, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp862));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 _temp865=({ struct _tuple16
_temp864; _temp864.f1= 1; _temp864.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp864.f3= dic1; _temp864.f4= excl; _temp864.f5= msg; _temp864;}); goto _LL866;
_LL866:(( void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct
Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_ns_f,& _temp865, dic2); if( _temp865.f1){ return({
struct Cyc_Core_Opt* _temp867=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp867->v=( void*) _temp865.f2; _temp867;});} else{ return 0;}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp870; struct _tagged_string* _temp871; struct _tagged_string _temp873;
struct Cyc_Dict_Dict* _temp875; struct Cyc_Dict_Dict* _temp877; struct Cyc_Dict_Dict**
_temp879; int _temp880; int* _temp882; struct _tuple17* _temp868= env; _temp870=*
_temp868; _LL881: _temp880= _temp870.f1; _temp882=&(* _temp868).f1; goto _LL878;
_LL878: _temp877= _temp870.f2; _temp879=&(* _temp868).f2; goto _LL876; _LL876:
_temp875= _temp870.f3; goto _LL874; _LL874: _temp873= _temp870.f4; goto _LL872;
_LL872: _temp871= _temp870.f5; goto _LL869; _LL869: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp875, x)){ Cyc_Interface_check_err(
_temp871,({ struct _tagged_string _temp883= _temp873; struct _tagged_string
_temp884=* x; xprintf("%.*s %.*s is exported more than once", _temp883.last_plus_one
- _temp883.curr, _temp883.curr, _temp884.last_plus_one - _temp884.curr, _temp884.curr);}));*
_temp882= 0;} else{ if(* _temp882){* _temp879=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(*
_temp879, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg){ struct _tuple17 _temp886=({ struct _tuple17 _temp885;
_temp885.f1= 1; _temp885.f2= dic1; _temp885.f3= dic1; _temp885.f4= t; _temp885.f5=
msg; _temp885;}); goto _LL887; _LL887:(( void(*)( void(* f)( struct _tuple17*,
struct _tagged_string*, void*), struct _tuple17* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct _tuple17* env, struct _tagged_string* x, void*
y)) Cyc_Interface_disj_merge_dict_f,& _temp886, dic2); if( _temp886.f1){ return({
struct Cyc_Core_Opt* _temp888=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp888->v=( void*) _temp886.f2; _temp888;});} else{ return 0;}}
struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp891=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->structdecls, ie2->structdecls,({ unsigned char* _temp889="struct"; struct
_tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 7; _temp890;}), msg); goto _LL892; _LL892: {
struct Cyc_Core_Opt* _temp895=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls,({
unsigned char* _temp893="union"; struct _tagged_string _temp894; _temp894.curr=
_temp893; _temp894.base= _temp893; _temp894.last_plus_one= _temp893 + 6;
_temp894;}), msg); goto _LL896; _LL896: { struct Cyc_Core_Opt* _temp899=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls,({ unsigned char* _temp897="[x]tunion";
struct _tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 10; _temp898;}), msg); goto _LL900; _LL900: {
struct Cyc_Core_Opt* _temp903=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,({
unsigned char* _temp901="enum"; struct _tagged_string _temp902; _temp902.curr=
_temp901; _temp902.base= _temp901; _temp902.last_plus_one= _temp901 + 5;
_temp902;}), msg); goto _LL904; _LL904: { struct Cyc_Core_Opt* _temp907=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl,({ unsigned char* _temp905="typedef"; struct
_tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 8; _temp906;}), msg); goto _LL908; _LL908: {
struct Cyc_Core_Opt* _temp911=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls,({
unsigned char* _temp909="variable"; struct _tagged_string _temp910; _temp910.curr=
_temp909; _temp910.base= _temp909; _temp910.last_plus_one= _temp909 + 9;
_temp910;}), msg); goto _LL912; _LL912: { struct Cyc_Core_Opt* _temp915=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls,({ unsigned char* _temp913="xtunionfield";
struct _tagged_string _temp914; _temp914.curr= _temp913; _temp914.base= _temp913;
_temp914.last_plus_one= _temp913 + 13; _temp914;}), msg); goto _LL916; _LL916:
if(((((( !(( unsigned int) _temp891)? 1: !(( unsigned int) _temp895))? 1: !((
unsigned int) _temp899))? 1: !(( unsigned int) _temp903))? 1: !(( unsigned int)
_temp907))? 1: !(( unsigned int) _temp911))? 1: !(( unsigned int) _temp915)){
return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp917=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp917->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp891))->v; _temp917->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp895))->v;
_temp917->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp899))->v; _temp917->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp903))->v; _temp917->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp907))->v;
_temp917->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp911))->v; _temp917->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp915))->v; _temp917;});}}}}}}}} struct _tuple18{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; } ; void
Cyc_Interface_disj_merge_ns_f( struct _tuple18* env, struct Cyc_List_List* ns,
struct Cyc_Interface_Ienv* ie2){ struct _tuple18 _temp920; struct _tagged_string*
_temp921; struct Cyc_Dict_Dict* _temp923; struct Cyc_Dict_Dict* _temp925; struct
Cyc_Dict_Dict** _temp927; int _temp928; int* _temp930; struct _tuple18* _temp918=
env; _temp920=* _temp918; _LL929: _temp928= _temp920.f1; _temp930=&(* _temp918).f1;
goto _LL926; _LL926: _temp925= _temp920.f2; _temp927=&(* _temp918).f2; goto
_LL924; _LL924: _temp923= _temp920.f3; goto _LL922; _LL922: _temp921= _temp920.f4;
goto _LL919; _LL919: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons
_temp931; _push_handler(& _temp931);{ int _temp933= 0; if( setjmp( _temp931.handler)){
_temp933= 1;} if( ! _temp933){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp923, ns);; _pop_handler();}
else{ void* _temp932=( void*) _exn_thrown; void* _temp935= _temp932; _LL937: if(
_temp935 == Cyc_Dict_Absent){ goto _LL938;} else{ goto _LL939;} _LL939: goto
_LL940; _LL938: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL936; _LL940:( void)
_throw( _temp935); _LL936:;}}}{ struct Cyc_Interface_Ienv* _temp941= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp921); goto _LL942; _LL942: if( _temp941 == 0){* _temp930= 0;}
else{ if(* _temp930){* _temp927=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp927, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp941));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple18 _temp944=({
struct _tuple18 _temp943; _temp943.f1= 1; _temp943.f2= dic1; _temp943.f3= dic1;
_temp943.f4= msg; _temp943;}); goto _LL945; _LL945:(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp944, dic2); if( _temp944.f1){ return({ struct Cyc_Core_Opt* _temp946=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp946->v=(
void*) _temp944.f2; _temp946;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp947; _push_handler(& _temp947);{ int _temp949= 0; if(
setjmp( _temp947.handler)){ _temp949= 1;} if( ! _temp949){{ struct
_tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string* _temp950=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp950[ 0]=({
struct _tagged_string _temp951=(*(( struct _tuple3*) _check_null( info))).f1;
struct _tagged_string _temp952=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging exports of %.*s and %.*s,", _temp951.last_plus_one - _temp951.curr,
_temp951.curr, _temp952.last_plus_one - _temp952.curr, _temp952.curr);});
_temp950;});}{ struct Cyc_Core_Opt* _temp953= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL954; _LL954: if( _temp953 == 0){ struct Cyc_Interface_I*
_temp955= 0; _npop_handler( 0u); return _temp955;} if( info != 0){ msg=({ struct
_tagged_string* _temp956=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp956[ 0]=({ struct _tagged_string _temp957=(*(( struct
_tuple3*) _check_null( info))).f1; struct _tagged_string _temp958=(*(( struct
_tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp957.last_plus_one - _temp957.curr, _temp957.curr, _temp958.last_plus_one -
_temp958.curr, _temp958.curr);}); _temp956;});}{ struct Cyc_Core_Opt* _temp959=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp953))->v, msg); goto _LL960; _LL960: if(
_temp959 == 0){ struct Cyc_Interface_I* _temp961= 0; _npop_handler( 0u); return
_temp961;}{ struct Cyc_Interface_I* _temp963=({ struct Cyc_Interface_I* _temp962=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp962->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp959))->v;
_temp962->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp953))->v; _temp962;}); _npop_handler( 0u); return _temp963;}}}};
_pop_handler();} else{ void* _temp948=( void*) _exn_thrown; void* _temp965=
_temp948; _LL967: if( _temp965 == Cyc_Tcdecl_Incompatible){ goto _LL968;} else{
goto _LL969;} _LL969: goto _LL970; _LL968: return 0; _LL970:( void) _throw(
_temp965); _LL966:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_string* curr_info= linfo != 0?( struct _tagged_string*)(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=((
struct Cyc_List_List*) _check_null( li))->tl; if( linfo != 0){ linfo=(( struct
Cyc_List_List*) _check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp972= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp971=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp971->f1=*(( struct _tagged_string*)
_check_null( curr_info)); _temp971->f2=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp971;}): 0); goto _LL973; _LL973: if( _temp972 ==
0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null( _temp972); if(
curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_string* _temp974=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp974[ 0]=({
struct _tagged_string _temp975=*(( struct _tagged_string*) _check_null(
curr_info)); struct _tagged_string _temp976=*(( struct _tagged_string*)(( struct
Cyc_List_List*) _check_null( linfo))->hd); xprintf("%.*s+%.*s", _temp975.last_plus_one
- _temp975.curr, _temp975.curr, _temp976.last_plus_one - _temp976.curr, _temp976.curr);});
_temp974;}): 0;} if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null(
linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List*
Cyc_Interface_add_namespace( struct Cyc_List_List* tds, struct Cyc_List_List* ns){
if( ns == 0){ return tds;} return({ struct Cyc_List_List* _temp977=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp977->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp978=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp978[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp979; _temp979.tag= Cyc_Absyn_Namespace_d;
_temp979.f1=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp979.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp979;}); _temp978;}), 0); _temp977->tl= 0; _temp977;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_string*
x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp980=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp980->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp981=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp981[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp982; _temp982.tag= Cyc_Absyn_Struct_d;
_temp982.f1= d; _temp982;}); _temp981;}), 0); _temp980->tl= tds; _temp980;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp983=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp983[ 0]=* d; _temp983;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp984=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp984->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp985=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp985[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp986; _temp986.tag= Cyc_Absyn_Struct_d;
_temp986.f1= d; _temp986;}); _temp985;}), 0); _temp984->tl= tds; _temp984;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp987=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp987->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp988=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp988[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp989; _temp989.tag= Cyc_Absyn_Union_d;
_temp989.f1= d; _temp989;}); _temp988;}), 0); _temp987->tl= tds; _temp987;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp990=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp990[ 0]=* d; _temp990;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp991=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp991->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp992=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp992[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp993; _temp993.tag= Cyc_Absyn_Union_d;
_temp993.f1= d; _temp993;}); _temp992;}), 0); _temp991->tl= tds; _temp991;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp994=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp994->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp995=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp995[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp996; _temp996.tag= Cyc_Absyn_Tunion_d;
_temp996.f1= d; _temp996;}); _temp995;}), 0); _temp994->tl= tds; _temp994;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp997=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp997[ 0]=* d; _temp997;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp998=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp998->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp999=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp999[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1000; _temp1000.tag= Cyc_Absyn_Tunion_d;
_temp1000.f1= d; _temp1000;}); _temp999;}), 0); _temp998->tl= tds; _temp998;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1001=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1001->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp1002=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1002[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1003; _temp1003.tag= Cyc_Absyn_Enum_d;
_temp1003.f1= d; _temp1003;}); _temp1002;}), 0); _temp1001->tl= tds; _temp1001;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp1004=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp1004[ 0]=* d; _temp1004;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1005=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1005->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp1006=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp1006[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp1007; _temp1007.tag= Cyc_Absyn_Enum_d;
_temp1007.f1= d; _temp1007;}); _temp1006;}), 0); _temp1005->tl= tds; _temp1005;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1008=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1008->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp1009=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp1009[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp1010; _temp1010.tag= Cyc_Absyn_Typedef_d;
_temp1010.f1= d; _temp1010;}); _temp1009;}), 0); _temp1008->tl= tds; _temp1008;});}
static struct Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp1011=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1011->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp1012=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp1012[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp1013; _temp1013.tag= Cyc_Absyn_Var_d;
_temp1013.f1= d; _temp1013;}); _temp1012;}), 0); _temp1011->tl= tds; _temp1011;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1016; struct Cyc_Absyn_Tunionfield*
_temp1017; struct Cyc_Absyn_Tuniondecl* _temp1019; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1014= d; _temp1016=* _temp1014; _LL1020: _temp1019= _temp1016.base; goto
_LL1018; _LL1018: _temp1017= _temp1016.field; goto _LL1015; _LL1015: _temp1019=({
struct Cyc_Absyn_Tuniondecl* _temp1021=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1021[ 0]=* _temp1019; _temp1021;});
_temp1019->fields=({ struct Cyc_Core_Opt* _temp1022=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1022->v=( void*)({ struct Cyc_List_List*
_temp1023=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1023->hd=( void*) _temp1017; _temp1023->tl= 0; _temp1023;}); _temp1022;});(
void*)( _temp1019->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1024=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1024->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1025=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1025[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1026; _temp1026.tag= Cyc_Absyn_Tunion_d;
_temp1026.f1= _temp1019; _temp1026;}); _temp1025;}), 0); _temp1024->tl= tds;
_temp1024;});} static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp1027= 0; goto _LL1028; _LL1028: _temp1027=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp1027);
_temp1027=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp1027); _temp1027=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp1027); _temp1027=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp1027); if( _temp1027 != 0){ _temp1027=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1027); _temp1027= Cyc_Interface_add_namespace(
_temp1027, ns); Cyc_Absynpp_decllist2file( _temp1027, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1029= 0; goto _LL1030; _LL1030: _temp1029=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp1029); if( _temp1029 != 0){ _temp1029=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1029); _temp1029= Cyc_Interface_add_namespace(
_temp1029, ns); Cyc_Absynpp_decllist2file( _temp1029, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1031= 0; goto _LL1032; _LL1032: _temp1031=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls,
_temp1031); _temp1031=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, _temp1031); _temp1031=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp1031); _temp1031=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp1031); _temp1031=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp1031); _temp1031=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp1031); if( _temp1031 != 0){ _temp1031=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1031); _temp1031= Cyc_Interface_add_namespace(
_temp1031, ns); Cyc_Absynpp_decllist2file( _temp1031, f);}} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ fprintf( f,"/****** needed (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports); fprintf( f,"\n/****** provided (headers) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports); fprintf( f,"\n/****** provided (typedefs) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports); fprintf( f,"\n/****** needed (declarations) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->imports); fprintf( f,"\n/****** provided (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1033= Cyc_Parse_parse_file(
f); goto _LL1034; _LL1034: Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv*
_temp1035= Cyc_Tcenv_tc_init(); goto _LL1036; _LL1036: Cyc_Tc_tc( _temp1035, 0,
0, _temp1033); return Cyc_Interface_gen_extract( _temp1035->ae, 0);}}}