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
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext[
14u]; extern void Cyc_Position_post_error( struct Cyc_Position_Error*); typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); struct Cyc_Set_Set; typedef
struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
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
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern unsigned int Cyc_Evexp_szof( void* t, struct Cyc_Position_Segment*
loc); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment* loc, struct
_tagged_string msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,({
struct _tagged_string _temp0= msg; xprintf("Exp_err: %.*s", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);})));} extern void* Cyc_Evexp_promote_const( void*
cn); extern void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_Evexp_eval_const_unprimop( void* p, struct Cyc_Absyn_Exp* e); extern
void* Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2); extern void* Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment*
loc); unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e){ void*
_temp1= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int _temp7; void*
_temp9; _LL3: if(( unsigned int) _temp1 > 1u?*(( int*) _temp1) == Cyc_Absyn_Int_c:
0){ _LL10: _temp9=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1)->f1; goto
_LL8; _LL8: _temp7=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1)->f2; goto
_LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: return( unsigned int) _temp7;
_LL6: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp11=( char*)"expecting unsigned int";
struct _tagged_string _temp12; _temp12.curr= _temp11; _temp12.base= _temp11;
_temp12.last_plus_one= _temp11 + 23; _temp12;})); return 0u; _LL2:;} void* Cyc_Evexp_promote_const(
void* cn){ void* _temp13= cn; char _temp21; void* _temp23; short _temp25; void*
_temp27; _LL15: if(( unsigned int) _temp13 > 1u?*(( int*) _temp13) == Cyc_Absyn_Char_c:
0){ _LL24: _temp23=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp13)->f1;
goto _LL22; _LL22: _temp21=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp13)->f2;
goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp13 > 1u?*(( int*)
_temp13) == Cyc_Absyn_Short_c: 0){ _LL28: _temp27=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp13)->f1; goto _LL26; _LL26: _temp25=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp13)->f2; goto _LL18;} else{ goto _LL19;} _LL19: goto _LL20; _LL16: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp29=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp29[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp30; _temp30.tag= Cyc_Absyn_Int_c; _temp30.f1=( void*) _temp23; _temp30.f2=(
int) _temp21; _temp30;}); _temp29;}); _LL18: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp31=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp31[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp32; _temp32.tag= Cyc_Absyn_Int_c;
_temp32.f1=( void*) _temp27; _temp32.f2=( int) _temp25; _temp32;}); _temp31;});
_LL20: return cn; _LL14:;} int Cyc_Evexp_eval_const_bool_exp( struct Cyc_Absyn_Exp*
e){ void* _temp33= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int
_temp43; void* _temp45; long long _temp47; void* _temp49; _LL35: if((
unsigned int) _temp33 > 1u?*(( int*) _temp33) == Cyc_Absyn_Int_c: 0){ _LL46:
_temp45=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp33)->f1; goto _LL44;
_LL44: _temp43=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp33)->f2; goto _LL36;}
else{ goto _LL37;} _LL37: if(( unsigned int) _temp33 > 1u?*(( int*) _temp33) ==
Cyc_Absyn_LongLong_c: 0){ _LL50: _temp49=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp33)->f1; goto _LL48; _LL48: _temp47=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp33)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if( _temp33 ==( void*) Cyc_Absyn_Null_c){
goto _LL40;} else{ goto _LL41;} _LL41: goto _LL42; _LL36: return _temp43 != 0;
_LL38: return _temp47 != 0; _LL40: return 0; _LL42: Cyc_Evexp_exp_err( e->loc,(
struct _tagged_string)({ char* _temp51=( char*)"expecting bool"; struct
_tagged_string _temp52; _temp52.curr= _temp51; _temp52.base= _temp51; _temp52.last_plus_one=
_temp51 + 15; _temp52;})); return 0; _LL34:;} struct _tuple3{ void* f1; void* f2;
} ; void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp* e){ void* cn;{ void*
_temp53=( void*) e->r; void* _temp71; struct Cyc_Absyn_Exp* _temp73; struct Cyc_Absyn_Exp*
_temp75; struct Cyc_Absyn_Exp* _temp77; struct Cyc_List_List* _temp79; void*
_temp81; void* _temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp*
_temp87; void* _temp89; struct Cyc_Absyn_Enumfield* _temp91; struct Cyc_Absyn_Enumdecl*
_temp93; struct _tuple0* _temp95; _LL55: if(*(( int*) _temp53) == Cyc_Absyn_Const_e){
_LL72: _temp71=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp53)->f1; goto
_LL56;} else{ goto _LL57;} _LL57: if(*(( int*) _temp53) == Cyc_Absyn_Conditional_e){
_LL78: _temp77=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f1; goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f2; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp53)->f3; goto _LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp53) ==
Cyc_Absyn_Primop_e){ _LL82: _temp81=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp53)->f1; goto _LL80; _LL80: _temp79=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp53)->f2; goto _LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp53) ==
Cyc_Absyn_Sizeoftyp_e){ _LL84: _temp83=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp53)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp53) ==
Cyc_Absyn_Sizeofexp_e){ _LL86: _temp85=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp53)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if(*(( int*) _temp53) ==
Cyc_Absyn_Cast_e){ _LL90: _temp89=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp53)->f1; goto _LL88; _LL88: _temp87=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp53)->f2; goto _LL66;} else{ goto _LL67;} _LL67: if(*(( int*) _temp53) ==
Cyc_Absyn_Enum_e){ _LL96: _temp95=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp53)->f1; goto _LL94; _LL94: _temp93=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp53)->f2; goto _LL92; _LL92: _temp91=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp53)->f3; goto _LL68;} else{ goto _LL69;}
_LL69: goto _LL70; _LL56: return _temp71; _LL58: cn= Cyc_Evexp_eval_const_bool_exp(
_temp77)? Cyc_Evexp_eval_const_exp( _temp75): Cyc_Evexp_eval_const_exp( _temp73);
goto _LL54; _LL60: if( _temp79 == 0){ Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp97=( char*)"bad static expression (no args to primop)";
struct _tagged_string _temp98; _temp98.curr= _temp97; _temp98.base= _temp97;
_temp98.last_plus_one= _temp97 + 42; _temp98;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp99=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp99[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp100; _temp100.tag= Cyc_Absyn_Int_c;
_temp100.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp100.f2= 0; _temp100;});
_temp99;});} if((( struct Cyc_List_List*) _check_null( _temp79))->tl == 0){ cn=
Cyc_Evexp_eval_const_unprimop( _temp81,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp79))->hd);} else{ if((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp79))->tl))->tl != 0){ Cyc_Evexp_exp_err(
e->loc,( struct _tagged_string)({ char* _temp101=( char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 48; _temp102;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp103=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp103[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp104; _temp104.tag= Cyc_Absyn_Int_c;
_temp104.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp104.f2= 0; _temp104;});
_temp103;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp81,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp79))->hd,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp79))->tl))->hd);}} goto _LL54; _LL62: cn= Cyc_Evexp_eval_sizeof( _temp83, e->loc);
goto _LL54; _LL64: cn= Cyc_Evexp_eval_sizeof(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp85->topt))->v, _temp85->loc); goto _LL54; _LL66: cn= Cyc_Evexp_eval_const_exp(
_temp87);{ struct _tuple3 _temp106=({ struct _tuple3 _temp105; _temp105.f1=
_temp89; _temp105.f2= cn; _temp105;}); void* _temp112; int _temp114; void*
_temp116; void* _temp118; void* _temp120; void* _temp122; _LL108: _LL119:
_temp118= _temp106.f1; if(( unsigned int) _temp118 > 4u?*(( int*) _temp118) ==
Cyc_Absyn_IntType: 0){ _LL123: _temp122=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f1; goto _LL121; _LL121: _temp120=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f2; goto _LL113;} else{ goto _LL110;} _LL113: _temp112= _temp106.f2;
if(( unsigned int) _temp112 > 1u?*(( int*) _temp112) == Cyc_Absyn_Int_c: 0){
_LL117: _temp116=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp112)->f1; goto
_LL115; _LL115: _temp114=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp112)->f2;
goto _LL109;} else{ goto _LL110;} _LL110: goto _LL111; _LL109: if( _temp122 !=
_temp116){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct* _temp124=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp124[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp125; _temp125.tag= Cyc_Absyn_Int_c; _temp125.f1=( void*) _temp122; _temp125.f2=
_temp114; _temp125;}); _temp124;});} goto _LL107; _LL111: Cyc_Evexp_exp_err( e->loc,({
struct _tagged_string _temp126= Cyc_Absynpp_typ2string( _temp89); xprintf("eval_const: cannot cast to %.*s",
_temp126.last_plus_one - _temp126.curr, _temp126.curr);})); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp127=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp127[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp128; _temp128.tag= Cyc_Absyn_Int_c; _temp128.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp128.f2= 0; _temp128;}); _temp127;}); _LL107:;} goto _LL54; _LL68: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null((( struct Cyc_Absyn_Enumfield*) _check_null( _temp91))->tag)));
_LL70: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp129=(
char*)"bad static expression"; struct _tagged_string _temp130; _temp130.curr=
_temp129; _temp130.base= _temp129; _temp130.last_plus_one= _temp129 + 22;
_temp130;})); return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp131=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp131[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp132; _temp132.tag= Cyc_Absyn_Int_c;
_temp132.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp132.f2= 0; _temp132;});
_temp131;}); _LL54:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp133=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp133[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp134; _temp134.tag= Cyc_Absyn_Const_e;
_temp134.f1=( void*) cn; _temp134;}); _temp133;}))); return cn;} struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; } ; unsigned int Cyc_Evexp_szof( void* t,
struct Cyc_Position_Segment* loc){ void* _temp135= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp191; int _temp193; struct Cyc_Core_Opt* _temp195;
void* _temp197; int _temp199; struct Cyc_Core_Opt* _temp201; void* _temp203;
struct Cyc_Absyn_TunionInfo _temp205; struct Cyc_Absyn_TunionFieldInfo _temp207;
struct Cyc_List_List* _temp209; void* _temp211; struct Cyc_Absyn_Tunionfield*
_temp213; struct Cyc_Absyn_Tuniondecl* _temp215; struct Cyc_Absyn_TunionFieldInfo
_temp217; struct Cyc_Absyn_PtrInfo _temp219; struct Cyc_Absyn_Conref* _temp221;
struct Cyc_Absyn_Tqual _temp223; struct Cyc_Absyn_Conref* _temp225; void*
_temp227; void* _temp229; void* _temp231; void* _temp233; void* _temp235; void*
_temp237; void* _temp239; void* _temp241; void* _temp243; void* _temp245; struct
Cyc_Absyn_Exp* _temp247; struct Cyc_Absyn_Tqual _temp249; void* _temp251; struct
Cyc_Absyn_FnInfo _temp253; struct Cyc_List_List* _temp255; void* _temp257;
struct Cyc_Absyn_Structdecl** _temp259; struct Cyc_List_List* _temp261; struct
_tuple0* _temp263; struct Cyc_List_List* _temp265; struct Cyc_Absyn_Uniondecl**
_temp267; struct Cyc_List_List* _temp269; struct _tuple0* _temp271; struct Cyc_List_List*
_temp273; struct Cyc_Absyn_Enumdecl* _temp275; struct _tuple0* _temp277; struct
Cyc_Core_Opt* _temp279; struct Cyc_List_List* _temp281; struct _tuple0* _temp283;
struct Cyc_List_List* _temp285; void* _temp287; _LL137: if( _temp135 ==( void*)
Cyc_Absyn_VoidType){ goto _LL138;} else{ goto _LL139;} _LL139: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_VarType: 0){ _LL192: _temp191=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp135)->f1; goto
_LL140;} else{ goto _LL141;} _LL141: if(( unsigned int) _temp135 > 4u?*(( int*)
_temp135) == Cyc_Absyn_Evar: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp135)->f1; if( _temp197 ==( void*) Cyc_Absyn_BoxKind){ goto _LL196;} else{
goto _LL143;} _LL196: _temp195=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp135)->f2; goto _LL194; _LL194: _temp193=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp135)->f3; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_Evar: 0){ _LL204: _temp203=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp135)->f1; goto _LL202; _LL202: _temp201=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp135)->f2; goto
_LL200; _LL200: _temp199=( int)(( struct Cyc_Absyn_Evar_struct*) _temp135)->f3;
goto _LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp135 > 4u?*((
int*) _temp135) == Cyc_Absyn_TunionType: 0){ _LL206: _temp205=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp135)->f1; goto _LL146;} else{ goto
_LL147;} _LL147: if(( unsigned int) _temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_TunionFieldType:
0){ _LL208: _temp207=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp135)->f1; _LL212: _temp211=( void*) _temp207.field_info; if(*(( int*)
_temp211) == Cyc_Absyn_KnownTunionfield){ _LL216: _temp215=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp211)->f1; goto _LL214; _LL214:
_temp213=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp211)->f2; goto _LL210;} else{ goto _LL149;} _LL210: _temp209=( struct Cyc_List_List*)
_temp207.targs; goto _LL148;} else{ goto _LL149;} _LL149: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_TunionFieldType: 0){ _LL218:
_temp217=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp135)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_PointerType: 0){ _LL220: _temp219=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp135)->f1;
_LL230: _temp229=( void*) _temp219.elt_typ; goto _LL228; _LL228: _temp227=( void*)
_temp219.rgn_typ; goto _LL226; _LL226: _temp225=( struct Cyc_Absyn_Conref*)
_temp219.nullable; goto _LL224; _LL224: _temp223=( struct Cyc_Absyn_Tqual)
_temp219.tq; goto _LL222; _LL222: _temp221=( struct Cyc_Absyn_Conref*) _temp219.bounds;
goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int) _temp135 > 4u?*((
int*) _temp135) == Cyc_Absyn_IntType: 0){ _LL234: _temp233=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f1; goto _LL232; _LL232: _temp231=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f2; if( _temp231 ==( void*) Cyc_Absyn_B1){ goto _LL154;} else{ goto
_LL155;}} else{ goto _LL155;} _LL155: if(( unsigned int) _temp135 > 4u?*(( int*)
_temp135) == Cyc_Absyn_IntType: 0){ _LL238: _temp237=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f1; goto _LL236; _LL236: _temp235=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f2; if( _temp235 ==( void*) Cyc_Absyn_B2){ goto _LL156;} else{ goto
_LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp135 > 4u?*(( int*)
_temp135) == Cyc_Absyn_IntType: 0){ _LL242: _temp241=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f1; goto _LL240; _LL240: _temp239=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f2; if( _temp239 ==( void*) Cyc_Absyn_B4){ goto _LL158;} else{ goto
_LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp135 > 4u?*(( int*)
_temp135) == Cyc_Absyn_IntType: 0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f1; goto _LL244; _LL244: _temp243=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp135)->f2; if( _temp243 ==( void*) Cyc_Absyn_B8){ goto _LL160;} else{ goto
_LL161;}} else{ goto _LL161;} _LL161: if( _temp135 ==( void*) Cyc_Absyn_FloatType){
goto _LL162;} else{ goto _LL163;} _LL163: if( _temp135 ==( void*) Cyc_Absyn_DoubleType){
goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int) _temp135 > 4u?*((
int*) _temp135) == Cyc_Absyn_ArrayType: 0){ _LL252: _temp251=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp135)->f1; goto _LL250; _LL250: _temp249=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp135)->f2;
goto _LL248; _LL248: _temp247=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp135)->f3; goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_FnType: 0){ _LL254: _temp253=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp135)->f1; goto
_LL168;} else{ goto _LL169;} _LL169: if(( unsigned int) _temp135 > 4u?*(( int*)
_temp135) == Cyc_Absyn_TupleType: 0){ _LL256: _temp255=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp135)->f1; goto _LL170;} else{ goto
_LL171;} _LL171: if(( unsigned int) _temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_RgnHandleType:
0){ _LL258: _temp257=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp135)->f1;
goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp135 > 4u?*((
int*) _temp135) == Cyc_Absyn_StructType: 0){ _LL264: _temp263=( struct _tuple0*)((
struct Cyc_Absyn_StructType_struct*) _temp135)->f1; goto _LL262; _LL262:
_temp261=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp135)->f2; goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp135)->f3; goto _LL174;} else{ goto
_LL175;} _LL175: if(( unsigned int) _temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_AnonStructType:
0){ _LL266: _temp265=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp135)->f1; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_UnionType: 0){ _LL272: _temp271=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp135)->f1; goto
_LL270; _LL270: _temp269=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp135)->f2; goto _LL268; _LL268: _temp267=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp135)->f3; goto _LL178;} else{ goto
_LL179;} _LL179: if(( unsigned int) _temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_AnonUnionType:
0){ _LL274: _temp273=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp135)->f1; goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_EnumType: 0){ _LL278: _temp277=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp135)->f1; goto _LL276;
_LL276: _temp275=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp135)->f2; goto _LL182;} else{ goto _LL183;} _LL183: if( _temp135 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL184;} else{ goto _LL185;} _LL185: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_TypedefType: 0){ _LL284: _temp283=(
struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp135)->f1; goto
_LL282; _LL282: _temp281=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp135)->f2; goto _LL280; _LL280: _temp279=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp135)->f3; goto _LL186;} else{ goto _LL187;}
_LL187: if(( unsigned int) _temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_JoinEff:
0){ _LL286: _temp285=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp135)->f1; goto _LL188;} else{ goto _LL189;} _LL189: if(( unsigned int)
_temp135 > 4u?*(( int*) _temp135) == Cyc_Absyn_AccessEff: 0){ _LL288: _temp287=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp135)->f1; goto _LL190;} else{
goto _LL136;} _LL138: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp289=( char*)"cannot apply sizeof to void type"; struct _tagged_string
_temp290; _temp290.curr= _temp289; _temp290.base= _temp289; _temp290.last_plus_one=
_temp289 + 33; _temp290;})); return 0u; _LL140: { void* _temp291=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp191->kind); _LL293: if(
_temp291 ==( void*) Cyc_Absyn_BoxKind){ goto _LL294;} else{ goto _LL295;} _LL295:
if( _temp291 ==( void*) Cyc_Absyn_AnyKind){ goto _LL296;} else{ goto _LL297;}
_LL297: if( _temp291 ==( void*) Cyc_Absyn_MemKind){ goto _LL298;} else{ goto
_LL299;} _LL299: if( _temp291 ==( void*) Cyc_Absyn_RgnKind){ goto _LL300;} else{
goto _LL301;} _LL301: if( _temp291 ==( void*) Cyc_Absyn_EffKind){ goto _LL302;}
else{ goto _LL292;} _LL294: return 4u; _LL296: goto _LL298; _LL298: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp303=( char*)"cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp304; _temp304.curr= _temp303; _temp304.base= _temp303;
_temp304.last_plus_one= _temp303 + 49; _temp304;})); return 0u; _LL300: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp305=( char*)"cannot apply sizeof to a region";
struct _tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 32; _temp306;})); return 0u; _LL302: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp307=( char*)"cannot apply sizeof to an effect";
struct _tagged_string _temp308; _temp308.curr= _temp307; _temp308.base= _temp307;
_temp308.last_plus_one= _temp307 + 33; _temp308;})); return 0u; _LL292:;} _LL142:
return 4u; _LL144: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp309=( char*)"cannot apply sizeof to unknown type"; struct _tagged_string
_temp310; _temp310.curr= _temp309; _temp310.base= _temp309; _temp310.last_plus_one=
_temp309 + 36; _temp310;})); return 0u; _LL146: return 4u; _LL148: return 4u +
Cyc_Evexp_szof(( void*)({ struct Cyc_Absyn_TupleType_struct* _temp311=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp311[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp312; _temp312.tag= Cyc_Absyn_TupleType;
_temp312.f1= _temp213->typs; _temp312;}); _temp311;}), loc); _LL150:( void)
_throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp313=( char*)"unroslved tunionfield type in szof";
struct _tagged_string _temp314; _temp314.curr= _temp313; _temp314.base= _temp313;
_temp314.last_plus_one= _temp313 + 35; _temp314;}))); _LL152: { void* _temp315=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp221))->v; void* _temp323; void* _temp325; _LL317: if(( unsigned int)
_temp315 > 1u?*(( int*) _temp315) == Cyc_Absyn_Eq_constr: 0){ _LL324: _temp323=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp315)->f1; if( _temp323 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL318;} else{ goto _LL319;}} else{ goto
_LL319;} _LL319: if(( unsigned int) _temp315 > 1u?*(( int*) _temp315) == Cyc_Absyn_Eq_constr:
0){ _LL326: _temp325=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp315)->f1;
goto _LL320;} else{ goto _LL321;} _LL321: goto _LL322; _LL318: return 12u;
_LL320: return 4u; _LL322: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp327=( char*)"bounds information not known for pointer type"; struct
_tagged_string _temp328; _temp328.curr= _temp327; _temp328.base= _temp327;
_temp328.last_plus_one= _temp327 + 46; _temp328;})); return 0u; _LL316:;} _LL154:
return 1u; _LL156: return 1u; _LL158: return 1u; _LL160: return 1u; _LL162:
return 4u; _LL164: return 8u; _LL166: if( _temp247 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp329=( char*)"cannot apply sizeof to an array of unknown size";
struct _tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 48; _temp330;}));} return Cyc_Evexp_szof(
_temp251, loc) * Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp*) _check_null( _temp247))); _LL168: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp331=( char*)"cannot apply sizeof to function type";
struct _tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 37; _temp332;})); return 0u; _LL170: {
unsigned int s= 0; for( 0; _temp255 != 0; _temp255=(( struct Cyc_List_List*)
_check_null( _temp255))->tl){ s += Cyc_Evexp_szof((*(( struct _tuple4*)(( struct
Cyc_List_List*) _check_null( _temp255))->hd)).f2, loc);} return s;} _LL172:
return 4u; _LL174: if( _temp259 == 0){ return(( unsigned int(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp333=(
char*)"szof on unchecked StructType"; struct _tagged_string _temp334; _temp334.curr=
_temp333; _temp334.base= _temp333; _temp334.last_plus_one= _temp333 + 29;
_temp334;}));}{ struct Cyc_Absyn_Structdecl* _temp335=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp259)); goto _LL336; _LL336: if( _temp335->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp337=( char*)"cannot apply sizeof to abstract struct type";
struct _tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 44; _temp338;}));} _temp265=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp335->fields))->v; goto _LL176;} _LL176: {
unsigned int s= 0; for( 0; _temp265 != 0; _temp265=(( struct Cyc_List_List*)
_check_null( _temp265))->tl){ s += Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp265))->hd)->type, loc);} return s;}
_LL178: if( _temp267 == 0){ return(( unsigned int(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp339=( char*)"szof on unchecked UnionType";
struct _tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 28; _temp340;}));}{ struct Cyc_Absyn_Uniondecl*
_temp341=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp267)); goto _LL342;
_LL342: if( _temp341->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp343=( char*)"cannot apply sizeof to abstract union type"; struct
_tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 43; _temp344;}));} _temp273=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp341->fields))->v; goto _LL180;} _LL180: {
unsigned int s= 0; for( 0; _temp273 != 0; _temp273=(( struct Cyc_List_List*)
_check_null( _temp273))->tl){ unsigned int i= Cyc_Evexp_szof(( void*)(( struct
Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp273))->hd)->type,
loc); s= s > i? s: i;} return s;} _LL182: return 4u; _LL184: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp345=( char*)"cannot apply sizoef to a region";
struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 32; _temp346;})); return 0u; _LL186: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp347= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_temp347.last_plus_one - _temp347.curr, _temp347.curr);})); _LL188: goto _LL190;
_LL190: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp348=( char*)"cannot apply sizoef to an effect";
struct _tagged_string _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 33; _temp349;})); return 0u; _LL136:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp350=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp350[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp351; _temp351.tag= Cyc_Absyn_Int_c; _temp351.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp351.f2=( int) Cyc_Evexp_szof( t, loc); _temp351;}); _temp350;});} int Cyc_Evexp_is_arith_const(
void* cn){ void* _temp352= cn; char _temp362; void* _temp364; int _temp366; void*
_temp368; struct _tagged_string _temp370; _LL354: if(( unsigned int) _temp352 >
1u?*(( int*) _temp352) == Cyc_Absyn_Char_c: 0){ _LL365: _temp364=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp352)->f1; goto _LL363; _LL363: _temp362=(
char)(( struct Cyc_Absyn_Char_c_struct*) _temp352)->f2; goto _LL355;} else{ goto
_LL356;} _LL356: if(( unsigned int) _temp352 > 1u?*(( int*) _temp352) == Cyc_Absyn_Int_c:
0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp352)->f1;
goto _LL367; _LL367: _temp366=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp352)->f2;
goto _LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp352 > 1u?*((
int*) _temp352) == Cyc_Absyn_Float_c: 0){ _LL371: _temp370=( struct
_tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp352)->f1; goto _LL359;}
else{ goto _LL360;} _LL360: goto _LL361; _LL355: return 1; _LL357: return 1;
_LL359: return 1; _LL361: return 0; _LL353:;} void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp373=({ struct _tuple3 _temp372; _temp372.f1= p;
_temp372.f2= cn; _temp372;}); void* _temp391; void* _temp393; void* _temp395;
int _temp397; void* _temp399; void* _temp401; void* _temp403; short _temp405;
void* _temp407; void* _temp409; void* _temp411; char _temp413; void* _temp415;
void* _temp417; void* _temp419; int _temp421; void* _temp423; void* _temp425;
void* _temp427; void* _temp429; void* _temp431; int _temp433; void* _temp435;
void* _temp437; _LL375: _LL394: _temp393= _temp373.f1; if( _temp393 ==( void*)
Cyc_Absyn_Plus){ goto _LL392;} else{ goto _LL377;} _LL392: _temp391= _temp373.f2;
goto _LL376; _LL377: _LL402: _temp401= _temp373.f1; if( _temp401 ==( void*) Cyc_Absyn_Minus){
goto _LL396;} else{ goto _LL379;} _LL396: _temp395= _temp373.f2; if((
unsigned int) _temp395 > 1u?*(( int*) _temp395) == Cyc_Absyn_Int_c: 0){ _LL400:
_temp399=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp395)->f1; goto _LL398;
_LL398: _temp397=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp395)->f2; goto
_LL378;} else{ goto _LL379;} _LL379: _LL410: _temp409= _temp373.f1; if( _temp409
==( void*) Cyc_Absyn_Minus){ goto _LL404;} else{ goto _LL381;} _LL404: _temp403=
_temp373.f2; if(( unsigned int) _temp403 > 1u?*(( int*) _temp403) == Cyc_Absyn_Short_c:
0){ _LL408: _temp407=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp403)->f1;
goto _LL406; _LL406: _temp405=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp403)->f2; goto _LL380;} else{ goto _LL381;} _LL381: _LL418: _temp417=
_temp373.f1; if( _temp417 ==( void*) Cyc_Absyn_Minus){ goto _LL412;} else{ goto
_LL383;} _LL412: _temp411= _temp373.f2; if(( unsigned int) _temp411 > 1u?*(( int*)
_temp411) == Cyc_Absyn_Char_c: 0){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp411)->f1; goto _LL414; _LL414: _temp413=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp411)->f2; goto _LL382;} else{ goto _LL383;} _LL383: _LL426: _temp425=
_temp373.f1; if( _temp425 ==( void*) Cyc_Absyn_Not){ goto _LL420;} else{ goto
_LL385;} _LL420: _temp419= _temp373.f2; if(( unsigned int) _temp419 > 1u?*(( int*)
_temp419) == Cyc_Absyn_Int_c: 0){ _LL424: _temp423=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp419)->f1; goto _LL422; _LL422: _temp421=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp419)->f2; goto _LL384;} else{ goto _LL385;} _LL385: _LL430: _temp429=
_temp373.f1; if( _temp429 ==( void*) Cyc_Absyn_Not){ goto _LL428;} else{ goto
_LL387;} _LL428: _temp427= _temp373.f2; if( _temp427 ==( void*) Cyc_Absyn_Null_c){
goto _LL386;} else{ goto _LL387;} _LL387: _LL438: _temp437= _temp373.f1; if(
_temp437 ==( void*) Cyc_Absyn_Bitnot){ goto _LL432;} else{ goto _LL389;} _LL432:
_temp431= _temp373.f2; if(( unsigned int) _temp431 > 1u?*(( int*) _temp431) ==
Cyc_Absyn_Int_c: 0){ _LL436: _temp435=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp431)->f1; goto _LL434; _LL434: _temp433=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp431)->f2; goto _LL388;} else{ goto _LL389;} _LL389: goto _LL390; _LL376:
if( ! Cyc_Evexp_is_arith_const( cn)){ Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp439=( char*)"expecting arithmetic constant"; struct
_tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 30; _temp440;}));} return cn; _LL378: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp441=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp441[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp442; _temp442.tag= Cyc_Absyn_Int_c; _temp442.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp442.f2= - _temp397; _temp442;}); _temp441;}); _LL380: return( void*)({
struct Cyc_Absyn_Short_c_struct* _temp443=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp443[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp444; _temp444.tag= Cyc_Absyn_Short_c; _temp444.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp444.f2= - _temp405; _temp444;}); _temp443;}); _LL382: return( void*)({
struct Cyc_Absyn_Char_c_struct* _temp445=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp445[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp446; _temp446.tag= Cyc_Absyn_Char_c; _temp446.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp446.f2= - _temp413; _temp446;}); _temp445;}); _LL384: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp447=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp447[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp448; _temp448.tag= Cyc_Absyn_Int_c; _temp448.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp448.f2= _temp421 == 0? 1: 0; _temp448;}); _temp447;}); _LL386: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp449=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp449[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp450; _temp450.tag= Cyc_Absyn_Int_c; _temp450.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp450.f2= 1; _temp450;}); _temp449;}); _LL388: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp451=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp451[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp452; _temp452.tag= Cyc_Absyn_Int_c;
_temp452.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp452.f2= ~ _temp433;
_temp452;}); _temp451;}); _LL390: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp453=( char*)"bad constant expression"; struct
_tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 24; _temp454;})); return cn; _LL374:;} struct
_tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop( void* p,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* _temp455= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e1)); goto _LL456; _LL456: { void* _temp457= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e2)); goto _LL458; _LL458: { void* s1; void* s2; int
i1; int i2;{ void* _temp459= _temp455; int _temp465; void* _temp467; _LL461: if((
unsigned int) _temp459 > 1u?*(( int*) _temp459) == Cyc_Absyn_Int_c: 0){ _LL468:
_temp467=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp459)->f1; goto _LL466;
_LL466: _temp465=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp459)->f2; goto
_LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462: s1= _temp467; i1=
_temp465; goto _LL460; _LL464: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp469=( char*)"bad constant expression"; struct
_tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 24; _temp470;})); return _temp455; _LL460:;}{
void* _temp471= _temp457; int _temp477; void* _temp479; _LL473: if((
unsigned int) _temp471 > 1u?*(( int*) _temp471) == Cyc_Absyn_Int_c: 0){ _LL480:
_temp479=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp471)->f1; goto _LL478;
_LL478: _temp477=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp471)->f2; goto
_LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL474: s2= _temp479; i2=
_temp477; goto _LL472; _LL476: Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp481=( char*)"bad constant expression"; struct
_tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 24; _temp482;})); return _temp455; _LL472:;}{
void* _temp483= p; _LL485: if( _temp483 ==( void*) Cyc_Absyn_Div){ goto _LL486;}
else{ goto _LL487;} _LL487: if( _temp483 ==( void*) Cyc_Absyn_Mod){ goto _LL488;}
else{ goto _LL489;} _LL489: goto _LL490; _LL486: if( i2 == 0){ Cyc_Evexp_exp_err(
e2->loc,( struct _tagged_string)({ char* _temp491=( char*)"division by zero in constant expression";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 40; _temp492;})); return _temp455;} goto
_LL484; _LL488: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({
char* _temp493=( char*)"division by zero in constant expression"; struct
_tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 40; _temp494;})); return _temp455;} goto
_LL484; _LL490: goto _LL484; _LL484:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned?
1: s2 ==( void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1;
unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1;
int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp496=({ struct
_tuple5 _temp495; _temp495.f1= p; _temp495.f2= has_u_arg; _temp495;}); int
_temp552; void* _temp554; int _temp556; void* _temp558; int _temp560; void*
_temp562; int _temp564; void* _temp566; int _temp568; void* _temp570; int
_temp572; void* _temp574; int _temp576; void* _temp578; int _temp580; void*
_temp582; int _temp584; void* _temp586; int _temp588; void* _temp590; int
_temp592; void* _temp594; int _temp596; void* _temp598; int _temp600; void*
_temp602; int _temp604; void* _temp606; int _temp608; void* _temp610; int
_temp612; void* _temp614; int _temp616; void* _temp618; int _temp620; void*
_temp622; int _temp624; void* _temp626; int _temp628; void* _temp630; int
_temp632; void* _temp634; int _temp636; void* _temp638; int _temp640; void*
_temp642; int _temp644; void* _temp646; int _temp648; void* _temp650; int
_temp652; void* _temp654; _LL498: _LL555: _temp554= _temp496.f1; if( _temp554 ==(
void*) Cyc_Absyn_Plus){ goto _LL553;} else{ goto _LL500;} _LL553: _temp552=
_temp496.f2; if( _temp552 == 0){ goto _LL499;} else{ goto _LL500;} _LL500:
_LL559: _temp558= _temp496.f1; if( _temp558 ==( void*) Cyc_Absyn_Times){ goto
_LL557;} else{ goto _LL502;} _LL557: _temp556= _temp496.f2; if( _temp556 == 0){
goto _LL501;} else{ goto _LL502;} _LL502: _LL563: _temp562= _temp496.f1; if(
_temp562 ==( void*) Cyc_Absyn_Minus){ goto _LL561;} else{ goto _LL504;} _LL561:
_temp560= _temp496.f2; if( _temp560 == 0){ goto _LL503;} else{ goto _LL504;}
_LL504: _LL567: _temp566= _temp496.f1; if( _temp566 ==( void*) Cyc_Absyn_Div){
goto _LL565;} else{ goto _LL506;} _LL565: _temp564= _temp496.f2; if( _temp564 ==
0){ goto _LL505;} else{ goto _LL506;} _LL506: _LL571: _temp570= _temp496.f1; if(
_temp570 ==( void*) Cyc_Absyn_Mod){ goto _LL569;} else{ goto _LL508;} _LL569:
_temp568= _temp496.f2; if( _temp568 == 0){ goto _LL507;} else{ goto _LL508;}
_LL508: _LL575: _temp574= _temp496.f1; if( _temp574 ==( void*) Cyc_Absyn_Plus){
goto _LL573;} else{ goto _LL510;} _LL573: _temp572= _temp496.f2; if( _temp572 ==
1){ goto _LL509;} else{ goto _LL510;} _LL510: _LL579: _temp578= _temp496.f1; if(
_temp578 ==( void*) Cyc_Absyn_Times){ goto _LL577;} else{ goto _LL512;} _LL577:
_temp576= _temp496.f2; if( _temp576 == 1){ goto _LL511;} else{ goto _LL512;}
_LL512: _LL583: _temp582= _temp496.f1; if( _temp582 ==( void*) Cyc_Absyn_Minus){
goto _LL581;} else{ goto _LL514;} _LL581: _temp580= _temp496.f2; if( _temp580 ==
1){ goto _LL513;} else{ goto _LL514;} _LL514: _LL587: _temp586= _temp496.f1; if(
_temp586 ==( void*) Cyc_Absyn_Div){ goto _LL585;} else{ goto _LL516;} _LL585:
_temp584= _temp496.f2; if( _temp584 == 1){ goto _LL515;} else{ goto _LL516;}
_LL516: _LL591: _temp590= _temp496.f1; if( _temp590 ==( void*) Cyc_Absyn_Mod){
goto _LL589;} else{ goto _LL518;} _LL589: _temp588= _temp496.f2; if( _temp588 ==
1){ goto _LL517;} else{ goto _LL518;} _LL518: _LL595: _temp594= _temp496.f1; if(
_temp594 ==( void*) Cyc_Absyn_Eq){ goto _LL593;} else{ goto _LL520;} _LL593:
_temp592= _temp496.f2; goto _LL519; _LL520: _LL599: _temp598= _temp496.f1; if(
_temp598 ==( void*) Cyc_Absyn_Neq){ goto _LL597;} else{ goto _LL522;} _LL597:
_temp596= _temp496.f2; goto _LL521; _LL522: _LL603: _temp602= _temp496.f1; if(
_temp602 ==( void*) Cyc_Absyn_Gt){ goto _LL601;} else{ goto _LL524;} _LL601:
_temp600= _temp496.f2; if( _temp600 == 0){ goto _LL523;} else{ goto _LL524;}
_LL524: _LL607: _temp606= _temp496.f1; if( _temp606 ==( void*) Cyc_Absyn_Lt){
goto _LL605;} else{ goto _LL526;} _LL605: _temp604= _temp496.f2; if( _temp604 ==
0){ goto _LL525;} else{ goto _LL526;} _LL526: _LL611: _temp610= _temp496.f1; if(
_temp610 ==( void*) Cyc_Absyn_Gte){ goto _LL609;} else{ goto _LL528;} _LL609:
_temp608= _temp496.f2; if( _temp608 == 0){ goto _LL527;} else{ goto _LL528;}
_LL528: _LL615: _temp614= _temp496.f1; if( _temp614 ==( void*) Cyc_Absyn_Lte){
goto _LL613;} else{ goto _LL530;} _LL613: _temp612= _temp496.f2; if( _temp612 ==
0){ goto _LL529;} else{ goto _LL530;} _LL530: _LL619: _temp618= _temp496.f1; if(
_temp618 ==( void*) Cyc_Absyn_Gt){ goto _LL617;} else{ goto _LL532;} _LL617:
_temp616= _temp496.f2; if( _temp616 == 1){ goto _LL531;} else{ goto _LL532;}
_LL532: _LL623: _temp622= _temp496.f1; if( _temp622 ==( void*) Cyc_Absyn_Lt){
goto _LL621;} else{ goto _LL534;} _LL621: _temp620= _temp496.f2; if( _temp620 ==
1){ goto _LL533;} else{ goto _LL534;} _LL534: _LL627: _temp626= _temp496.f1; if(
_temp626 ==( void*) Cyc_Absyn_Gte){ goto _LL625;} else{ goto _LL536;} _LL625:
_temp624= _temp496.f2; if( _temp624 == 1){ goto _LL535;} else{ goto _LL536;}
_LL536: _LL631: _temp630= _temp496.f1; if( _temp630 ==( void*) Cyc_Absyn_Lte){
goto _LL629;} else{ goto _LL538;} _LL629: _temp628= _temp496.f2; if( _temp628 ==
1){ goto _LL537;} else{ goto _LL538;} _LL538: _LL635: _temp634= _temp496.f1; if(
_temp634 ==( void*) Cyc_Absyn_Bitand){ goto _LL633;} else{ goto _LL540;} _LL633:
_temp632= _temp496.f2; goto _LL539; _LL540: _LL639: _temp638= _temp496.f1; if(
_temp638 ==( void*) Cyc_Absyn_Bitor){ goto _LL637;} else{ goto _LL542;} _LL637:
_temp636= _temp496.f2; goto _LL541; _LL542: _LL643: _temp642= _temp496.f1; if(
_temp642 ==( void*) Cyc_Absyn_Bitxor){ goto _LL641;} else{ goto _LL544;} _LL641:
_temp640= _temp496.f2; goto _LL543; _LL544: _LL647: _temp646= _temp496.f1; if(
_temp646 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL645;} else{ goto _LL546;}
_LL645: _temp644= _temp496.f2; goto _LL545; _LL546: _LL651: _temp650= _temp496.f1;
if( _temp650 ==( void*) Cyc_Absyn_Bitlrshift){ goto _LL649;} else{ goto _LL548;}
_LL649: _temp648= _temp496.f2; goto _LL547; _LL548: _LL655: _temp654= _temp496.f1;
if( _temp654 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL653;} else{ goto _LL550;}
_LL653: _temp652= _temp496.f2; goto _LL549; _LL550: goto _LL551; _LL499: i3= i1
+ i2; use_i3= 1; goto _LL497; _LL501: i3= i1 * i2; use_i3= 1; goto _LL497;
_LL503: i3= i1 - i2; use_i3= 1; goto _LL497; _LL505: i3= i1 / i2; use_i3= 1;
goto _LL497; _LL507: i3= i1 % i2; use_i3= 1; goto _LL497; _LL509: u3= u1 + u2;
use_u3= 1; goto _LL497; _LL511: u3= u1 * u2; use_u3= 1; goto _LL497; _LL513: u3=
u1 - u2; use_u3= 1; goto _LL497; _LL515: u3= u1 / u2; use_u3= 1; goto _LL497;
_LL517: u3= u1 % u2; use_u3= 1; goto _LL497; _LL519: b3= i1 == i2; use_b3= 1;
goto _LL497; _LL521: b3= i1 != i2; use_b3= 1; goto _LL497; _LL523: b3= i1 > i2;
use_b3= 1; goto _LL497; _LL525: b3= i1 < i2; use_b3= 1; goto _LL497; _LL527: b3=
i1 >= i2; use_b3= 1; goto _LL497; _LL529: b3= i1 <= i2; use_b3= 1; goto _LL497;
_LL531: b3= u1 > u2; use_b3= 1; goto _LL497; _LL533: b3= u1 < u2; use_b3= 1;
goto _LL497; _LL535: b3= u1 >= u2; use_b3= 1; goto _LL497; _LL537: b3= u1 <= u2;
use_b3= 1; goto _LL497; _LL539: u3= u1 & u2; use_u3= 1; goto _LL497; _LL541: u3=
u1 | u2; use_u3= 1; goto _LL497; _LL543: u3= u1 ^ u2; use_u3= 1; goto _LL497;
_LL545: u3= u1 << u2; use_u3= 1; goto _LL497; _LL547: u3= u1 >> u2; use_u3= 1;
goto _LL497; _LL549: Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char*
_temp656=( char*)">>> NOT IMPLEMENTED"; struct _tagged_string _temp657; _temp657.curr=
_temp656; _temp657.base= _temp656; _temp657.last_plus_one= _temp656 + 20;
_temp657;})); return _temp455; _LL551: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp658=( char*)"bad constant expression"; struct
_tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 24; _temp659;})); return _temp455; _LL497:;}
if( use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp660=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp660[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp661; _temp661.tag= Cyc_Absyn_Int_c;
_temp661.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp661.f2= i3; _temp661;});
_temp660;});} if( use_u3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp662=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp662[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp663; _temp663.tag= Cyc_Absyn_Int_c;
_temp663.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp663.f2=( int) u3;
_temp663;}); _temp662;});} if( use_b3){ return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp664=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp664[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp665; _temp665.tag= Cyc_Absyn_Int_c;
_temp665.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp665.f2= b3? 1: 0; _temp665;});
_temp664;});}( void) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp666=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp666[ 0]=({ struct Cyc_Core_Unreachable_struct _temp667; _temp667.tag= Cyc_Core_Unreachable;
_temp667.f1=( struct _tagged_string)({ char* _temp668=( char*)"Evexp::eval_const_binop";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 24; _temp669;}); _temp667;}); _temp666;}));}}}}