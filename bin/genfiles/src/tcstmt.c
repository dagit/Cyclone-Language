#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef unsigned char Cyc_u_char; typedef
unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int
Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
unsigned char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
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
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern unsigned char Cyc_Position_Nocontext[ 14u]; typedef struct _tagged_string*
Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t; typedef
struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple0*
Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern void* Cyc_Absyn_exn_typ; extern struct
Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
new_tvs, struct Cyc_List_List* vds, struct Cyc_Absyn_Switch_clause* clause);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv*, void*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te);
extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te); extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern
void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct Cyc_Absyn_Stmt**); struct
_tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct Cyc_List_List* f2; struct
Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru( struct
Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar*
name); extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void*
f2; } ; extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part); struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; }
; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct Cyc_CfFlowInfo_Place;
typedef void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t;
typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, int new_block); static void Cyc_Tcstmt_decorate_stmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser(
te)); s->try_depth= Cyc_Tcenv_get_try_depth( te);} static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
tvs, struct Cyc_List_List* vs, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp*
where_opt, int new_block){ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
loc, te, tvs); if( new_block){ te2= Cyc_Tcenv_new_block( loc, te2);}{ struct Cyc_Core_Opt*
_temp1=({ struct Cyc_Core_Opt* _temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp0->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp0;});
goto _LL2; _LL2:{ struct Cyc_List_List* _temp3= vs; goto _LL4; _LL4: for( 0;
_temp3 != 0; _temp3=(( struct Cyc_List_List*) _check_null( _temp3))->tl){ te2=
Cyc_Tcenv_add_pat_var( loc, te2,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp3))->hd);(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp3))->hd)->rgn= _temp1;}} if( where_opt != 0){ Cyc_Tcexp_tcTest(
te2,( struct Cyc_Absyn_Exp*) _check_null( where_opt),({ unsigned char* _temp5="switch clause guard";
struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base= _temp5; _temp6.last_plus_one=
_temp5 + 20; _temp6;}));} if( vs != 0){ te2= Cyc_Tcenv_set_encloser( te2, s);}
Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs != 0){ Cyc_NewControlFlow_set_encloser( s,
Cyc_Tcenv_get_encloser( te));}}} void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s0, int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{
void* _temp7=( void*) s0->r; struct Cyc_Absyn_Exp* _temp47; struct Cyc_Absyn_Stmt*
_temp49; struct Cyc_Absyn_Stmt* _temp51; struct Cyc_Absyn_Exp* _temp53; struct
Cyc_Absyn_Stmt* _temp55; struct Cyc_Absyn_Stmt* _temp57; struct Cyc_Absyn_Exp*
_temp59; struct Cyc_Absyn_Stmt* _temp61; struct _tuple2 _temp63; struct Cyc_Absyn_Stmt*
_temp65; struct Cyc_Absyn_Exp* _temp67; struct Cyc_Absyn_Stmt* _temp69; struct
_tuple2 _temp71; struct Cyc_Absyn_Stmt* _temp73; struct Cyc_Absyn_Exp* _temp75;
struct _tuple2 _temp77; struct Cyc_Absyn_Stmt* _temp79; struct Cyc_Absyn_Exp*
_temp81; struct Cyc_Absyn_Exp* _temp83; struct _tuple2 _temp85; struct Cyc_Absyn_Stmt*
_temp87; struct Cyc_Absyn_Exp* _temp89; struct Cyc_Absyn_Stmt* _temp91; struct
Cyc_Absyn_Stmt* _temp93; struct Cyc_Absyn_Stmt** _temp95; struct Cyc_Absyn_Stmt*
_temp96; struct Cyc_Absyn_Stmt** _temp98; struct Cyc_Absyn_Stmt* _temp99; struct
Cyc_Absyn_Stmt** _temp101; struct _tagged_string* _temp102; struct Cyc_Absyn_Switch_clause**
_temp104; struct Cyc_Absyn_Switch_clause*** _temp106; struct Cyc_List_List*
_temp107; struct Cyc_Absyn_Stmt* _temp109; struct _tagged_string* _temp111;
struct Cyc_List_List* _temp113; struct Cyc_Absyn_Exp* _temp115; struct Cyc_List_List*
_temp117; struct Cyc_Absyn_Stmt* _temp119; struct Cyc_Absyn_Stmt* _temp121;
struct Cyc_Absyn_Decl* _temp123; struct Cyc_Absyn_Stmt* _temp125; struct Cyc_Absyn_Vardecl*
_temp127; struct Cyc_Absyn_Tvar* _temp129; struct Cyc_Absyn_Stmt* _temp131;
struct Cyc_Absyn_Stmt* _temp133; _LL9: if( _temp7 ==( void*) Cyc_Absyn_Skip_s){
goto _LL10;} else{ goto _LL11;} _LL11: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_Exp_s: 0){ _LL48: _temp47=(( struct Cyc_Absyn_Exp_s_struct*)
_temp7)->f1; goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Seq_s: 0){ _LL52: _temp51=(( struct Cyc_Absyn_Seq_s_struct*)
_temp7)->f1; goto _LL50; _LL50: _temp49=(( struct Cyc_Absyn_Seq_s_struct*)
_temp7)->f2; goto _LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Return_s: 0){ _LL54: _temp53=(( struct Cyc_Absyn_Return_s_struct*)
_temp7)->f1; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_IfThenElse_s: 0){ _LL60: _temp59=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f1; goto _LL58; _LL58: _temp57=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f2; goto _LL56; _LL56: _temp55=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f3; goto _LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_While_s: 0){ _LL64: _temp63=(( struct Cyc_Absyn_While_s_struct*)
_temp7)->f1; _LL68: _temp67= _temp63.f1; goto _LL66; _LL66: _temp65= _temp63.f2;
goto _LL62; _LL62: _temp61=(( struct Cyc_Absyn_While_s_struct*) _temp7)->f2;
goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_For_s: 0){ _LL84: _temp83=(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f1; goto _LL78; _LL78: _temp77=(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f2; _LL82: _temp81= _temp77.f1; goto _LL80; _LL80: _temp79= _temp77.f2;
goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_For_s_struct*) _temp7)->f3; _LL76:
_temp75= _temp71.f1; goto _LL74; _LL74: _temp73= _temp71.f2; goto _LL70; _LL70:
_temp69=(( struct Cyc_Absyn_For_s_struct*) _temp7)->f4; goto _LL22;} else{ goto
_LL23;} _LL23: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Do_s:
0){ _LL92: _temp91=(( struct Cyc_Absyn_Do_s_struct*) _temp7)->f1; goto _LL86;
_LL86: _temp85=(( struct Cyc_Absyn_Do_s_struct*) _temp7)->f2; _LL90: _temp89=
_temp85.f1; goto _LL88; _LL88: _temp87= _temp85.f2; goto _LL24;} else{ goto
_LL25;} _LL25: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Break_s:
0){ _LL94: _temp93=(( struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; _temp95=&((
struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; goto _LL26;} else{ goto _LL27;}
_LL27: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Continue_s:
0){ _LL97: _temp96=(( struct Cyc_Absyn_Continue_s_struct*) _temp7)->f1; _temp98=&((
struct Cyc_Absyn_Continue_s_struct*) _temp7)->f1; goto _LL28;} else{ goto _LL29;}
_LL29: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Goto_s: 0){
_LL103: _temp102=(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f1; goto _LL100;
_LL100: _temp99=(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; _temp101=&((
struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; goto _LL30;} else{ goto _LL31;}
_LL31: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Fallthru_s:
0){ _LL108: _temp107=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f1; goto
_LL105; _LL105: _temp104=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f2;
_temp106=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f2; goto _LL32;}
else{ goto _LL33;} _LL33: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) ==
Cyc_Absyn_Label_s: 0){ _LL112: _temp111=(( struct Cyc_Absyn_Label_s_struct*)
_temp7)->f1; goto _LL110; _LL110: _temp109=(( struct Cyc_Absyn_Label_s_struct*)
_temp7)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Switch_s: 0){ _LL116: _temp115=(( struct Cyc_Absyn_Switch_s_struct*)
_temp7)->f1; goto _LL114; _LL114: _temp113=(( struct Cyc_Absyn_Switch_s_struct*)
_temp7)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_TryCatch_s: 0){ _LL120: _temp119=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp7)->f1; goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp7)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Decl_s: 0){ _LL124: _temp123=(( struct Cyc_Absyn_Decl_s_struct*)
_temp7)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_Decl_s_struct*)
_temp7)->f2; goto _LL40;} else{ goto _LL41;} _LL41: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Region_s: 0){ _LL130: _temp129=(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f1; goto _LL128; _LL128: _temp127=(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f2; goto _LL126; _LL126: _temp125=(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f3; goto _LL42;} else{ goto _LL43;} _LL43: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Cut_s: 0){ _LL132: _temp131=(( struct Cyc_Absyn_Cut_s_struct*)
_temp7)->f1; goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Splice_s: 0){ _LL134: _temp133=(( struct Cyc_Absyn_Splice_s_struct*)
_temp7)->f1; goto _LL46;} else{ goto _LL8;} _LL10: return; _LL12: Cyc_Tcexp_tcExp(
te, 0, _temp47); return; _LL14: { struct Cyc_Tcenv_Tenv* _temp137= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp135=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp135[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp136; _temp136.tag= Cyc_Tcenv_Stmt_j; _temp136.f1= _temp49; _temp136;});
_temp135;})); goto _LL138; _LL138: Cyc_Tcstmt_tcStmt( _temp137, _temp51, 1);
while( 1) { void* _temp139=( void*) _temp49->r; struct Cyc_Absyn_Stmt* _temp145;
struct Cyc_Absyn_Stmt* _temp147; _LL141: if(( unsigned int) _temp139 > 1u?*((
int*) _temp139) == Cyc_Absyn_Seq_s: 0){ _LL148: _temp147=(( struct Cyc_Absyn_Seq_s_struct*)
_temp139)->f1; goto _LL146; _LL146: _temp145=(( struct Cyc_Absyn_Seq_s_struct*)
_temp139)->f2; goto _LL142;} else{ goto _LL143;} _LL143: goto _LL144; _LL142:
Cyc_Tcstmt_decorate_stmt( te, _temp49); _temp137= Cyc_Tcenv_set_next( te,( void*)({
struct Cyc_Tcenv_Stmt_j_struct* _temp149=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp149[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp150; _temp150.tag= Cyc_Tcenv_Stmt_j; _temp150.f1= _temp145; _temp150;});
_temp149;})); Cyc_Tcstmt_tcStmt( _temp137, _temp147, 1); _temp49= _temp145;
continue; _LL144: goto seq_end; _LL140:;} seq_end: Cyc_Tcstmt_tcStmt( te,
_temp49, 1); return;} _LL16: { void* t= Cyc_Tcenv_return_typ( te); if( _temp53
== 0){ void* _temp151= Cyc_Tcutil_compress( t); _LL153: if( _temp151 ==( void*)
Cyc_Absyn_VoidType){ goto _LL154;} else{ goto _LL155;} _LL155: goto _LL156;
_LL154: goto _LL152; _LL156: Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string
_temp157= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp157.last_plus_one - _temp157.curr, _temp157.curr);})); goto _LL152; _LL152:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) _check_null( _temp53);
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp158= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct _tagged_string
_temp159= Cyc_Absynpp_typ2string( t); xprintf("returns value of type %.*s but requires %.*s",
_temp158.last_plus_one - _temp158.curr, _temp158.curr, _temp159.last_plus_one -
_temp159.curr, _temp159.curr);}));}{ void* _temp160= Cyc_Tcutil_compress( t);
_LL162: if( _temp160 ==( void*) Cyc_Absyn_VoidType){ goto _LL163;} else{ goto
_LL164;} _LL164: goto _LL165; _LL163: Cyc_Tcutil_terr( s0->loc,({ unsigned char*
_temp166="function returns void expression"; struct _tagged_string _temp167;
_temp167.curr= _temp166; _temp167.base= _temp166; _temp167.last_plus_one=
_temp166 + 33; _temp167;})); goto _LL161; _LL165: goto _LL161; _LL161:;}}
return;} _LL18: Cyc_Tcexp_tcTest( te, _temp59,({ unsigned char* _temp168="if statement";
struct _tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 13; _temp169;})); Cyc_Tcstmt_tcStmt( te,
_temp57, 1); Cyc_Tcstmt_tcStmt( te, _temp55, 1); return; _LL20: Cyc_Tcstmt_decorate_stmt(
te, _temp65); Cyc_Tcexp_tcTest( te, _temp67,({ unsigned char* _temp170="while loop";
struct _tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 11; _temp171;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp65), _temp61, 1); return; _LL22: Cyc_Tcstmt_decorate_stmt( te, _temp79);
Cyc_Tcstmt_decorate_stmt( te, _temp73); Cyc_Tcexp_tcExp( te, 0, _temp83); Cyc_Tcexp_tcTest(
te, _temp81,({ unsigned char* _temp172="for loop"; struct _tagged_string
_temp173; _temp173.curr= _temp172; _temp173.base= _temp172; _temp173.last_plus_one=
_temp172 + 9; _temp173;})); te= Cyc_Tcenv_set_in_loop( te, _temp73); Cyc_Tcstmt_tcStmt(
te, _temp69, 1); Cyc_Tcexp_tcExp( te, 0, _temp75); return; _LL24: Cyc_Tcstmt_decorate_stmt(
te, _temp87); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop( te, _temp87), _temp91, 1);
Cyc_Tcexp_tcTest( te, _temp89,({ unsigned char* _temp174="do loop"; struct
_tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 8; _temp175;})); return; _LL26: Cyc_Tcenv_process_break(
te, s0, _temp95); return; _LL28: Cyc_Tcenv_process_continue( te, s0, _temp98);
return; _LL30: Cyc_Tcenv_process_goto( te, s0, _temp102, _temp101); return;
_LL32: { struct _tuple3* _temp176= Cyc_Tcenv_process_fallthru( te, s0, _temp106);
goto _LL177; _LL177: if( _temp176 == 0){ Cyc_Tcutil_terr( s0->loc,({
unsigned char* _temp178="fallthru not in a non-last case"; struct _tagged_string
_temp179; _temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 32; _temp179;})); return;}{ struct _tuple3 _temp182; struct Cyc_List_List*
_temp183; struct Cyc_List_List* _temp185; struct Cyc_Absyn_Switch_clause*
_temp187; struct _tuple3* _temp180=( struct _tuple3*) _check_null( _temp176);
_temp182=* _temp180; _LL188: _temp187= _temp182.f1; goto _LL186; _LL186:
_temp185= _temp182.f2; goto _LL184; _LL184: _temp183= _temp182.f3; goto _LL181;
_LL181: { struct Cyc_List_List* instantiation=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp185); _temp183=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_List_List*, void*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_substitute, instantiation, _temp183); for( 0;
_temp183 != 0? _temp107 != 0: 0;( _temp183=(( struct Cyc_List_List*) _check_null(
_temp183))->tl, _temp107=(( struct Cyc_List_List*) _check_null( _temp107))->tl)){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp107))->hd); if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp107))->hd,( void*)(( struct Cyc_List_List*)
_check_null( _temp183))->hd)){ Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string
_temp189= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp107))->hd)->topt))->v);
struct _tagged_string _temp190= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( _temp183))->hd); xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr, _temp190.last_plus_one -
_temp190.curr, _temp190.curr);}));}} if( _temp107 != 0){ Cyc_Tcutil_terr( s0->loc,({
unsigned char* _temp191="too many arguments to explicit fallthru"; struct
_tagged_string _temp192; _temp192.curr= _temp191; _temp192.base= _temp191;
_temp192.last_plus_one= _temp191 + 40; _temp192;}));} if( _temp183 != 0){ Cyc_Tcutil_terr(
s0->loc,({ unsigned char* _temp193="too few arguments to explicit fallthru";
struct _tagged_string _temp194; _temp194.curr= _temp193; _temp194.base= _temp193;
_temp194.last_plus_one= _temp193 + 39; _temp194;}));} return;}}} _LL34: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label( te, _temp111, _temp109),({
struct Cyc_Absyn_Tvar* _temp195=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp195->name=({ struct _tagged_string* _temp196=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp196[ 0]=({
struct _tagged_string _temp197=* _temp111; xprintf("`%.*s", _temp197.last_plus_one
- _temp197.curr, _temp197.curr);}); _temp196;}); _temp195->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp195;})),
_temp109, 0); return; _LL36: Cyc_Tcexp_tcExp( te, 0, _temp115);{ void* _temp198=(
void*)(( struct Cyc_Core_Opt*) _check_null( _temp115->topt))->v; goto _LL199;
_LL199: te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru( te);{
struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_rev)( _temp113); for( 0; scs != 0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Pat* _temp200=(( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->pattern; goto _LL201; _LL201: {
struct Cyc_List_List* _temp204; struct Cyc_List_List* _temp206; struct _tuple5
_temp202= Cyc_Tcpat_tcPat( te, _temp200,( void**)& _temp198, 0); _LL207:
_temp206= _temp202.f1; goto _LL205; _LL205: _temp204= _temp202.f2; goto _LL203;
_LL203: if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp200->topt))->v, _temp198)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc,({ struct _tagged_string
_temp208= Cyc_Absynpp_typ2string( _temp198); struct _tagged_string _temp209= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp200->topt))->v); xprintf("switch on type %.*s, but case expects type %.*s",
_temp208.last_plus_one - _temp208.curr, _temp208.curr, _temp209.last_plus_one -
_temp209.curr, _temp209.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp200);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp210=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp210->v=( void*) _temp204;
_temp210;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd)->loc, te, _temp206, _temp204,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp206, _temp204,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( scs))->hd);}}} Cyc_Tcpat_check_switch_exhaustive(
s0->loc, _temp113); return;} _LL38:( void*)( _temp119->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Absyn_new_stmt(( void*) _temp119->r, _temp119->loc), Cyc_Absyn_skip_stmt(
_temp119->loc), _temp119->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_enter_try( te), s0), _temp119, 1); te= Cyc_Tcenv_set_in_switch( te);
te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* _temp211=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp117); goto _LL212; _LL212: for( 0;
_temp211 != 0; _temp211=(( struct Cyc_List_List*) _check_null( _temp211))->tl){
struct Cyc_Absyn_Pat* _temp213=(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp211))->hd)->pattern; goto _LL214; _LL214: { struct Cyc_List_List*
_temp217; struct Cyc_List_List* _temp219; struct _tuple5 _temp215= Cyc_Tcpat_tcPat(
te, _temp213,( void**)& Cyc_Absyn_exn_typ, 0); _LL220: _temp219= _temp215.f1;
goto _LL218; _LL218: _temp217= _temp215.f2; goto _LL216; _LL216: if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp213->topt))->v, Cyc_Absyn_exn_typ)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp211))->hd)->loc,({ struct _tagged_string _temp221= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp213->topt))->v); xprintf("expected xtunion exn but found %.*s",
_temp221.last_plus_one - _temp221.curr, _temp221.curr);}));} else{ Cyc_Tcpat_check_pat_regions(
te, _temp213);}(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( _temp211))->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp222=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp222->v=( void*)
_temp217; _temp222;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp211))->hd)->loc, te, _temp219, _temp217,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp211))->hd)->body,((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp211))->hd)->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp219, _temp217,( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp211))->hd);}}} Cyc_Tcpat_check_catch_overlap(
s0->loc, _temp117); return; _LL40: { struct _tagged_string unimp_msg_part; if(
new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp223=( void*)
_temp123->r; struct Cyc_Absyn_Vardecl* _temp249; int _temp251; int* _temp253;
struct Cyc_Absyn_Exp* _temp254; struct Cyc_Core_Opt* _temp256; struct Cyc_Core_Opt**
_temp258; struct Cyc_Core_Opt* _temp259; struct Cyc_Core_Opt** _temp261; struct
Cyc_Absyn_Pat* _temp262; struct Cyc_List_List* _temp264; struct Cyc_List_List*
_temp266; struct _tagged_string* _temp268; struct Cyc_List_List* _temp270;
struct _tuple0* _temp272; struct Cyc_Absyn_Fndecl* _temp274; struct Cyc_Absyn_Structdecl*
_temp276; struct Cyc_Absyn_Uniondecl* _temp278; struct Cyc_Absyn_Tuniondecl*
_temp280; struct Cyc_Absyn_Enumdecl* _temp282; struct Cyc_Absyn_Typedefdecl*
_temp284; struct Cyc_List_List* _temp286; _LL225: if(*(( int*) _temp223) == Cyc_Absyn_Var_d){
_LL250: _temp249=(( struct Cyc_Absyn_Var_d_struct*) _temp223)->f1; goto _LL226;}
else{ goto _LL227;} _LL227: if(*(( int*) _temp223) == Cyc_Absyn_Let_d){ _LL263:
_temp262=(( struct Cyc_Absyn_Let_d_struct*) _temp223)->f1; goto _LL260; _LL260:
_temp259=(( struct Cyc_Absyn_Let_d_struct*) _temp223)->f2; _temp261=&(( struct
Cyc_Absyn_Let_d_struct*) _temp223)->f2; goto _LL257; _LL257: _temp256=(( struct
Cyc_Absyn_Let_d_struct*) _temp223)->f3; _temp258=&(( struct Cyc_Absyn_Let_d_struct*)
_temp223)->f3; goto _LL255; _LL255: _temp254=(( struct Cyc_Absyn_Let_d_struct*)
_temp223)->f4; goto _LL252; _LL252: _temp251=(( struct Cyc_Absyn_Let_d_struct*)
_temp223)->f5; _temp253=&(( struct Cyc_Absyn_Let_d_struct*) _temp223)->f5; goto
_LL228;} else{ goto _LL229;} _LL229: if(*(( int*) _temp223) == Cyc_Absyn_Letv_d){
_LL265: _temp264=(( struct Cyc_Absyn_Letv_d_struct*) _temp223)->f1; goto _LL230;}
else{ goto _LL231;} _LL231: if(*(( int*) _temp223) == Cyc_Absyn_Namespace_d){
_LL269: _temp268=(( struct Cyc_Absyn_Namespace_d_struct*) _temp223)->f1; goto
_LL267; _LL267: _temp266=(( struct Cyc_Absyn_Namespace_d_struct*) _temp223)->f2;
goto _LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp223) == Cyc_Absyn_Using_d){
_LL273: _temp272=(( struct Cyc_Absyn_Using_d_struct*) _temp223)->f1; goto _LL271;
_LL271: _temp270=(( struct Cyc_Absyn_Using_d_struct*) _temp223)->f2; goto _LL234;}
else{ goto _LL235;} _LL235: if(*(( int*) _temp223) == Cyc_Absyn_Fn_d){ _LL275:
_temp274=(( struct Cyc_Absyn_Fn_d_struct*) _temp223)->f1; goto _LL236;} else{
goto _LL237;} _LL237: if(*(( int*) _temp223) == Cyc_Absyn_Struct_d){ _LL277:
_temp276=(( struct Cyc_Absyn_Struct_d_struct*) _temp223)->f1; goto _LL238;}
else{ goto _LL239;} _LL239: if(*(( int*) _temp223) == Cyc_Absyn_Union_d){ _LL279:
_temp278=(( struct Cyc_Absyn_Union_d_struct*) _temp223)->f1; goto _LL240;} else{
goto _LL241;} _LL241: if(*(( int*) _temp223) == Cyc_Absyn_Tunion_d){ _LL281:
_temp280=(( struct Cyc_Absyn_Tunion_d_struct*) _temp223)->f1; goto _LL242;}
else{ goto _LL243;} _LL243: if(*(( int*) _temp223) == Cyc_Absyn_Enum_d){ _LL283:
_temp282=(( struct Cyc_Absyn_Enum_d_struct*) _temp223)->f1; goto _LL244;} else{
goto _LL245;} _LL245: if(*(( int*) _temp223) == Cyc_Absyn_Typedef_d){ _LL285:
_temp284=(( struct Cyc_Absyn_Typedef_d_struct*) _temp223)->f1; goto _LL246;}
else{ goto _LL247;} _LL247: if(*(( int*) _temp223) == Cyc_Absyn_ExternC_d){
_LL287: _temp286=(( struct Cyc_Absyn_ExternC_d_struct*) _temp223)->f1; goto
_LL248;} else{ goto _LL224;} _LL226: { struct Cyc_Absyn_Vardecl _temp290; struct
Cyc_List_List* _temp291; struct Cyc_Core_Opt* _temp293; struct Cyc_Core_Opt**
_temp295; struct Cyc_Absyn_Exp* _temp296; void* _temp298; struct Cyc_Absyn_Tqual
_temp300; struct _tuple0* _temp302; struct _tuple0 _temp304; struct
_tagged_string* _temp305; void* _temp307; void* _temp309; struct Cyc_Absyn_Vardecl*
_temp288= _temp249; _temp290=* _temp288; _LL310: _temp309=( void*) _temp290.sc;
goto _LL303; _LL303: _temp302= _temp290.name; _temp304=* _temp302; _LL308:
_temp307= _temp304.f1; goto _LL306; _LL306: _temp305= _temp304.f2; goto _LL301;
_LL301: _temp300= _temp290.tq; goto _LL299; _LL299: _temp298=( void*) _temp290.type;
goto _LL297; _LL297: _temp296= _temp290.initializer; goto _LL294; _LL294:
_temp293= _temp290.rgn; _temp295=&(* _temp288).rgn; goto _LL292; _LL292:
_temp291= _temp290.attributes; goto _LL289; _LL289: { void* _temp311= Cyc_Tcenv_curr_rgn(
te); goto _LL312; _LL312:* _temp295=({ struct Cyc_Core_Opt* _temp313=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp313->v=( void*)
_temp311; _temp313;});{ void* _temp314= _temp307; struct Cyc_List_List* _temp324;
struct Cyc_List_List* _temp326; _LL316: if( _temp314 ==( void*) Cyc_Absyn_Loc_n){
goto _LL317;} else{ goto _LL318;} _LL318: if(( unsigned int) _temp314 > 1u?*((
int*) _temp314) == Cyc_Absyn_Rel_n: 0){ _LL325: _temp324=(( struct Cyc_Absyn_Rel_n_struct*)
_temp314)->f1; if( _temp324 == 0){ goto _LL319;} else{ goto _LL320;}} else{ goto
_LL320;} _LL320: if(( unsigned int) _temp314 > 1u?*(( int*) _temp314) == Cyc_Absyn_Abs_n:
0){ _LL327: _temp326=(( struct Cyc_Absyn_Abs_n_struct*) _temp314)->f1; goto
_LL321;} else{ goto _LL322;} _LL322: goto _LL323; _LL317: goto _LL315; _LL319:(*
_temp249->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL315; _LL321:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp328="tcstmt: Abs_n declaration";
struct _tagged_string _temp329; _temp329.curr= _temp328; _temp329.base= _temp328;
_temp329.last_plus_one= _temp328 + 26; _temp329;}))); _LL323: Cyc_Tcutil_terr(
_temp123->loc,({ unsigned char* _temp330="cannot declare a qualified local variable";
struct _tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 42; _temp331;})); goto _LL315; _LL315:;}{
void* _temp332= Cyc_Tcutil_compress( _temp298); struct Cyc_Absyn_Exp* _temp339;
struct Cyc_Absyn_Tqual _temp341; void* _temp343; _LL334: if(( unsigned int)
_temp332 > 4u?*(( int*) _temp332) == Cyc_Absyn_ArrayType: 0){ _LL344: _temp343=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp332)->f1; goto _LL342; _LL342:
_temp341=(( struct Cyc_Absyn_ArrayType_struct*) _temp332)->f2; goto _LL340;
_LL340: _temp339=(( struct Cyc_Absyn_ArrayType_struct*) _temp332)->f3; if(
_temp339 == 0){ goto _LL338;} else{ goto _LL336;}} else{ goto _LL336;} _LL338:
if( _temp249->initializer != 0){ goto _LL335;} else{ goto _LL336;} _LL336: goto
_LL337; _LL335:{ void* _temp345=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp249->initializer))->r; void* _temp355; struct _tagged_string _temp357;
struct Cyc_List_List* _temp359; struct Cyc_Core_Opt* _temp361; struct Cyc_List_List*
_temp363; _LL347: if(*(( int*) _temp345) == Cyc_Absyn_Const_e){ _LL356: _temp355=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp345)->f1; if(( unsigned int)
_temp355 > 1u?*(( int*) _temp355) == Cyc_Absyn_String_c: 0){ _LL358: _temp357=((
struct Cyc_Absyn_String_c_struct*) _temp355)->f1; goto _LL348;} else{ goto
_LL349;}} else{ goto _LL349;} _LL349: if(*(( int*) _temp345) == Cyc_Absyn_UnresolvedMem_e){
_LL362: _temp361=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp345)->f1;
goto _LL360; _LL360: _temp359=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp345)->f2; goto _LL350;} else{ goto _LL351;} _LL351: if(*(( int*) _temp345)
== Cyc_Absyn_Array_e){ _LL364: _temp363=(( struct Cyc_Absyn_Array_e_struct*)
_temp345)->f1; goto _LL352;} else{ goto _LL353;} _LL353: goto _LL354; _LL348:
_temp298=( void*)( _temp249->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp365=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp365[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp366; _temp366.tag= Cyc_Absyn_ArrayType;
_temp366.f1=( void*) _temp343; _temp366.f2= _temp341; _temp366.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp367= _temp357;( unsigned int)(
_temp367.last_plus_one - _temp367.curr);}), 0); _temp366;}); _temp365;}))); goto
_LL346; _LL350: _temp363= _temp359; goto _LL352; _LL352: _temp298=( void*)(
_temp249->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp368=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp368[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp369; _temp369.tag= Cyc_Absyn_ArrayType;
_temp369.f1=( void*) _temp343; _temp369.f2= _temp341; _temp369.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp363), 0); _temp369;}); _temp368;}))); goto _LL346; _LL354: goto _LL346;
_LL346:;} goto _LL333; _LL337: goto _LL333; _LL333:;} Cyc_Tcutil_check_type( s0->loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, _temp298);{
struct Cyc_Tcenv_Tenv* _temp370= Cyc_Tcenv_add_local_var( _temp121->loc, te,
_temp249); goto _LL371; _LL371: if( _temp296 != 0){ Cyc_Tcexp_tcExpInitializer(
_temp370,( void**)& _temp298,( struct Cyc_Absyn_Exp*) _check_null( _temp296));
if( ! Cyc_Tcutil_coerce_assign( _temp370,( struct Cyc_Absyn_Exp*) _check_null(
_temp296), _temp298)){ Cyc_Tcutil_terr( _temp123->loc,({ struct _tagged_string
_temp372=* _temp305; struct _tagged_string _temp373= Cyc_Absynpp_typ2string(
_temp298); struct _tagged_string _temp374= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) _check_null(
_temp296))->topt))->v); xprintf("%.*s declared with type %.*s, initialized with type %.*s",
_temp372.last_plus_one - _temp372.curr, _temp372.curr, _temp373.last_plus_one -
_temp373.curr, _temp373.curr, _temp374.last_plus_one - _temp374.curr, _temp374.curr);}));}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp370, s0), _temp121, 0); return;}}}
_LL228: { struct Cyc_Core_Opt* _temp375=* _temp258; goto _LL376; _LL376: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp254);* _temp258= _temp254->topt;{ struct Cyc_List_List* _temp379;
struct Cyc_List_List* _temp381; struct _tuple5 _temp377= Cyc_Tcpat_tcPat( te,
_temp262,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null(
_temp254->topt))->v)), 0); _LL382: _temp381= _temp377.f1; goto _LL380; _LL380:
_temp379= _temp377.f2; goto _LL378; _LL378:* _temp261=({ struct Cyc_Core_Opt*
_temp383=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp383->v=( void*) _temp379; _temp383;}); if( _temp375 != 0? ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp262->topt))->v,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp375))->v): 0){ Cyc_Tcutil_terr( _temp123->loc,({
unsigned char* _temp384="type of pattern does not match declared type"; struct
_tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 45; _temp385;}));} if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp262->topt))->v,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp254->topt))->v)){ Cyc_Tcutil_terr(
_temp123->loc,({ struct _tagged_string _temp386= Cyc_Absynpp_typ2string(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp262->topt))->v); struct _tagged_string
_temp387= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp254->topt))->v); xprintf("pattern type %.*s does not match definition type %.*s",
_temp386.last_plus_one - _temp386.curr, _temp386.curr, _temp387.last_plus_one -
_temp387.curr, _temp387.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp262);}* _temp253= Cyc_Tcpat_check_let_pat_exhaustive( _temp262->loc,
_temp262); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp381, _temp379, _temp121,
0, 0); return;}} _LL230: { void* _temp388= Cyc_Tcenv_curr_rgn( te); goto _LL389;
_LL389: { struct Cyc_Tcenv_Tenv* _temp390= te; goto _LL391; _LL391: for( 0;
_temp264 != 0; _temp264=(( struct Cyc_List_List*) _check_null( _temp264))->tl){
struct Cyc_Absyn_Vardecl* _temp392=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp264))->hd; goto _LL393; _LL393: { struct Cyc_Absyn_Vardecl
_temp396; struct Cyc_List_List* _temp397; struct Cyc_Core_Opt* _temp399; struct
Cyc_Core_Opt** _temp401; struct Cyc_Absyn_Exp* _temp402; void* _temp404; struct
Cyc_Absyn_Tqual _temp406; struct _tuple0* _temp408; struct _tuple0 _temp410;
struct _tagged_string* _temp411; void* _temp413; void* _temp415; struct Cyc_Absyn_Vardecl*
_temp394= _temp392; _temp396=* _temp394; _LL416: _temp415=( void*) _temp396.sc;
goto _LL409; _LL409: _temp408= _temp396.name; _temp410=* _temp408; _LL414:
_temp413= _temp410.f1; goto _LL412; _LL412: _temp411= _temp410.f2; goto _LL407;
_LL407: _temp406= _temp396.tq; goto _LL405; _LL405: _temp404=( void*) _temp396.type;
goto _LL403; _LL403: _temp402= _temp396.initializer; goto _LL400; _LL400:
_temp399= _temp396.rgn; _temp401=&(* _temp394).rgn; goto _LL398; _LL398:
_temp397= _temp396.attributes; goto _LL395; _LL395:* _temp401=({ struct Cyc_Core_Opt*
_temp417=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp417->v=( void*) _temp388; _temp417;});{ void* _temp418= _temp413; struct
Cyc_List_List* _temp428; struct Cyc_List_List* _temp430; _LL420: if( _temp418 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL421;} else{ goto _LL422;} _LL422: if((
unsigned int) _temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_Rel_n: 0){ _LL429:
_temp428=(( struct Cyc_Absyn_Rel_n_struct*) _temp418)->f1; if( _temp428 == 0){
goto _LL423;} else{ goto _LL424;}} else{ goto _LL424;} _LL424: if(( unsigned int)
_temp418 > 1u?*(( int*) _temp418) == Cyc_Absyn_Abs_n: 0){ _LL431: _temp430=((
struct Cyc_Absyn_Abs_n_struct*) _temp418)->f1; goto _LL425;} else{ goto _LL426;}
_LL426: goto _LL427; _LL421: goto _LL419; _LL423:(* _temp392->name).f1=( void*)
Cyc_Absyn_Loc_n; goto _LL419; _LL425:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp432="tcstmt: Abs_n declaration";
struct _tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 26; _temp433;}))); _LL427: Cyc_Tcutil_terr(
_temp123->loc,({ unsigned char* _temp434="cannot declare a qualified local variable";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 42; _temp435;})); goto _LL419; _LL419:;} Cyc_Tcutil_check_type(
s0->loc, _temp390, Cyc_Tcenv_lookup_type_vars( _temp390),( void*) Cyc_Absyn_MemKind,
_temp404); _temp390= Cyc_Tcenv_add_local_var( _temp121->loc, _temp390, _temp392);}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp390, s0), _temp121, 0); return;}}
_LL232: unimp_msg_part=({ unsigned char* _temp436="namespace"; struct
_tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 10; _temp437;}); goto _LL224; _LL234:
unimp_msg_part=({ unsigned char* _temp438="using"; struct _tagged_string
_temp439; _temp439.curr= _temp438; _temp439.base= _temp438; _temp439.last_plus_one=
_temp438 + 6; _temp439;}); goto _LL224; _LL236: unimp_msg_part=({ unsigned char*
_temp440="function"; struct _tagged_string _temp441; _temp441.curr= _temp440;
_temp441.base= _temp440; _temp441.last_plus_one= _temp440 + 9; _temp441;}); goto
_LL224; _LL238: unimp_msg_part=({ unsigned char* _temp442="struct"; struct
_tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 7; _temp443;}); goto _LL224; _LL240:
unimp_msg_part=({ unsigned char* _temp444="union"; struct _tagged_string
_temp445; _temp445.curr= _temp444; _temp445.base= _temp444; _temp445.last_plus_one=
_temp444 + 6; _temp445;}); goto _LL224; _LL242: unimp_msg_part=({ unsigned char*
_temp446="[x]tunion"; struct _tagged_string _temp447; _temp447.curr= _temp446;
_temp447.base= _temp446; _temp447.last_plus_one= _temp446 + 10; _temp447;});
goto _LL224; _LL244: unimp_msg_part=({ unsigned char* _temp448="enum"; struct
_tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 5; _temp449;}); goto _LL224; _LL246:
unimp_msg_part=({ unsigned char* _temp450="typedef"; struct _tagged_string
_temp451; _temp451.curr= _temp450; _temp451.base= _temp450; _temp451.last_plus_one=
_temp450 + 8; _temp451;}); goto _LL224; _LL248: unimp_msg_part=({ unsigned char*
_temp452="extern \"C\""; struct _tagged_string _temp453; _temp453.curr= _temp452;
_temp453.base= _temp452; _temp453.last_plus_one= _temp452 + 11; _temp453;});
goto _LL224; _LL224:;}( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp454= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp454.last_plus_one - _temp454.curr, _temp454.curr);})));} _LL42:( void*)(
_temp125->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt(( void*)
_temp125->r, _temp125->loc), Cyc_Absyn_skip_stmt( _temp125->loc), _temp125->loc))->r));
if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ struct Cyc_Absyn_Vardecl
_temp457; struct Cyc_List_List* _temp458; struct Cyc_Core_Opt* _temp460; struct
Cyc_Core_Opt** _temp462; struct Cyc_Absyn_Exp* _temp463; void* _temp465; struct
Cyc_Absyn_Tqual _temp467; struct _tuple0* _temp469; void* _temp471; struct Cyc_Absyn_Vardecl*
_temp455= _temp127; _temp457=* _temp455; _LL472: _temp471=( void*) _temp457.sc;
goto _LL470; _LL470: _temp469= _temp457.name; goto _LL468; _LL468: _temp467=
_temp457.tq; goto _LL466; _LL466: _temp465=( void*) _temp457.type; goto _LL464;
_LL464: _temp463= _temp457.initializer; goto _LL461; _LL461: _temp460= _temp457.rgn;
_temp462=&(* _temp455).rgn; goto _LL459; _LL459: _temp458= _temp457.attributes;
goto _LL456; _LL456: { void* _temp473= Cyc_Tcenv_curr_rgn( te); goto _LL474;
_LL474: { void* rgn_typ=( void*)({ struct Cyc_Absyn_VarType_struct* _temp481=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp481[ 0]=({ struct Cyc_Absyn_VarType_struct _temp482; _temp482.tag= Cyc_Absyn_VarType;
_temp482.f1= _temp129; _temp482;}); _temp481;});* _temp462=({ struct Cyc_Core_Opt*
_temp475=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp475->v=( void*) _temp473; _temp475;}); te= Cyc_Tcenv_add_type_vars( s0->loc,
te,({ struct Cyc_List_List* _temp476=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp476->hd=( void*) _temp129; _temp476->tl= 0;
_temp476;})); Cyc_Tcutil_check_type( s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_BoxKind, _temp465); te= Cyc_Tcenv_add_region( te, rgn_typ); if(
! Cyc_Tcutil_unify( _temp465,( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp477=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp477[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp478; _temp478.tag=
Cyc_Absyn_RgnHandleType; _temp478.f1=( void*) rgn_typ; _temp478;}); _temp477;}))){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp479="region stmt: type of region handle is wrong!";
struct _tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 45; _temp480;}));} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_add_local_var( _temp125->loc, Cyc_Tcenv_enter_try( te), _temp127), s0),
_temp125, 0); return;}}} _LL44:( void) _throw((( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp483="tcStmt: cut is not implemented";
struct _tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 31; _temp484;}))); _LL46:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp485="tcStmt: splice is not implemented";
struct _tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 34; _temp486;}))); _LL8:;}}