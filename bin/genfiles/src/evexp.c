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
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
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
_temp99;});} if(({ struct Cyc_List_List* _temp101= _temp79; if( _temp101 == 0){
_throw( Null_Exception);} _temp101->tl;}) == 0){ cn= Cyc_Evexp_eval_const_unprimop(
_temp81,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp102= _temp79; if(
_temp102 == 0){ _throw( Null_Exception);} _temp102->hd;}));} else{ if(({ struct
Cyc_List_List* _temp104=({ struct Cyc_List_List* _temp103= _temp79; if( _temp103
== 0){ _throw( Null_Exception);} _temp103->tl;}); if( _temp104 == 0){ _throw(
Null_Exception);} _temp104->tl;}) != 0){ Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp105=( char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 48; _temp106;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp107=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp107[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp108; _temp108.tag= Cyc_Absyn_Int_c;
_temp108.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp108.f2= 0; _temp108;});
_temp107;});} else{ cn= Cyc_Evexp_eval_const_binprimop( _temp81,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp109= _temp79; if( _temp109 == 0){ _throw(
Null_Exception);} _temp109->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp111=({ struct Cyc_List_List* _temp110= _temp79; if( _temp110 == 0){ _throw(
Null_Exception);} _temp110->tl;}); if( _temp111 == 0){ _throw( Null_Exception);}
_temp111->hd;}));}} goto _LL54; _LL62: cn= Cyc_Evexp_eval_sizeof( _temp83, e->loc);
goto _LL54; _LL64: cn= Cyc_Evexp_eval_sizeof(( void*)({ struct Cyc_Core_Opt*
_temp112= _temp85->topt; if( _temp112 == 0){ _throw( Null_Exception);} _temp112->v;}),
_temp85->loc); goto _LL54; _LL66: cn= Cyc_Evexp_eval_const_exp( _temp87);{
struct _tuple3 _temp114=({ struct _tuple3 _temp113; _temp113.f1= _temp89;
_temp113.f2= cn; _temp113;}); void* _temp120; int _temp122; void* _temp124; void*
_temp126; void* _temp128; void* _temp130; _LL116: _LL127: _temp126= _temp114.f1;
if(( unsigned int) _temp126 > 4u?*(( int*) _temp126) == Cyc_Absyn_IntType: 0){
_LL131: _temp130=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp126)->f1;
goto _LL129; _LL129: _temp128=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp126)->f2; goto _LL121;} else{ goto _LL118;} _LL121: _temp120= _temp114.f2;
if(( unsigned int) _temp120 > 1u?*(( int*) _temp120) == Cyc_Absyn_Int_c: 0){
_LL125: _temp124=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp120)->f1; goto
_LL123; _LL123: _temp122=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp120)->f2;
goto _LL117;} else{ goto _LL118;} _LL118: goto _LL119; _LL117: if( _temp130 !=
_temp124){ cn=( void*)({ struct Cyc_Absyn_Int_c_struct* _temp132=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp132[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp133; _temp133.tag= Cyc_Absyn_Int_c; _temp133.f1=( void*) _temp130; _temp133.f2=
_temp122; _temp133;}); _temp132;});} goto _LL115; _LL119: Cyc_Evexp_exp_err( e->loc,({
struct _tagged_string _temp134= Cyc_Absynpp_typ2string( _temp89); xprintf("eval_const: cannot cast to %.*s",
_temp134.last_plus_one - _temp134.curr, _temp134.curr);})); return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp135=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp135[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp136; _temp136.tag= Cyc_Absyn_Int_c; _temp136.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp136.f2= 0; _temp136;}); _temp135;}); _LL115:;} goto _LL54; _LL68: return
Cyc_Evexp_eval_const_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp138=({ struct Cyc_Absyn_Enumfield* _temp137= _temp91; if( _temp137 == 0){
_throw( Null_Exception);} _temp137->tag;}); if( _temp138 == 0){ _throw(
Null_Exception);} _temp138;})); _LL70: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp139=( char*)"bad static expression"; struct
_tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 22; _temp140;})); return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp141=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp141[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp142; _temp142.tag= Cyc_Absyn_Int_c;
_temp142.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp142.f2= 0; _temp142;});
_temp141;}); _LL54:;}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp143=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp143[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp144; _temp144.tag= Cyc_Absyn_Const_e;
_temp144.f1=( void*) cn; _temp144;}); _temp143;}))); return cn;} struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; } ; unsigned int Cyc_Evexp_szof( void* t,
struct Cyc_Position_Segment* loc){ void* _temp145= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_Tvar* _temp197; int _temp199; struct Cyc_Core_Opt* _temp201;
void* _temp203; int _temp205; struct Cyc_Core_Opt* _temp207; void* _temp209;
struct Cyc_Absyn_TunionInfo _temp211; struct Cyc_Absyn_TunionFieldInfo _temp213;
struct Cyc_List_List* _temp215; void* _temp217; struct Cyc_Absyn_Tunionfield*
_temp219; struct Cyc_Absyn_Tuniondecl* _temp221; struct Cyc_Absyn_TunionFieldInfo
_temp223; struct Cyc_Absyn_PtrInfo _temp225; struct Cyc_Absyn_Conref* _temp227;
struct Cyc_Absyn_Tqual _temp229; struct Cyc_Absyn_Conref* _temp231; void*
_temp233; void* _temp235; void* _temp237; void* _temp239; void* _temp241; void*
_temp243; void* _temp245; void* _temp247; void* _temp249; void* _temp251; struct
Cyc_Absyn_Exp* _temp253; struct Cyc_Absyn_Tqual _temp255; void* _temp257; struct
Cyc_Absyn_FnInfo _temp259; struct Cyc_List_List* _temp261; void* _temp263;
struct Cyc_Absyn_Structdecl** _temp265; struct Cyc_List_List* _temp267; struct
_tuple0* _temp269; struct Cyc_Absyn_Uniondecl** _temp271; struct Cyc_List_List*
_temp273; struct _tuple0* _temp275; struct Cyc_Absyn_Enumdecl* _temp277; struct
_tuple0* _temp279; struct Cyc_Core_Opt* _temp281; struct Cyc_List_List* _temp283;
struct _tuple0* _temp285; struct Cyc_List_List* _temp287; void* _temp289; _LL147:
if( _temp145 ==( void*) Cyc_Absyn_VoidType){ goto _LL148;} else{ goto _LL149;}
_LL149: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_VarType:
0){ _LL198: _temp197=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp145)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_Evar: 0){ _LL204: _temp203=( void*)((
struct Cyc_Absyn_Evar_struct*) _temp145)->f1; if( _temp203 ==( void*) Cyc_Absyn_BoxKind){
goto _LL202;} else{ goto _LL153;} _LL202: _temp201=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp145)->f2; goto _LL200; _LL200: _temp199=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp145)->f3; goto _LL152;} else{ goto
_LL153;} _LL153: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_Evar:
0){ _LL210: _temp209=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp145)->f1;
goto _LL208; _LL208: _temp207=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f2; goto _LL206; _LL206: _temp205=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp145)->f3; goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int)
_temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_TunionType: 0){ _LL212: _temp211=(
struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*) _temp145)->f1;
goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp145 > 4u?*((
int*) _temp145) == Cyc_Absyn_TunionFieldType: 0){ _LL214: _temp213=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp145)->f1; _LL218: _temp217=( void*)
_temp213.field_info; if(*(( int*) _temp217) == Cyc_Absyn_KnownTunionfield){
_LL222: _temp221=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp217)->f1; goto _LL220; _LL220: _temp219=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp217)->f2; goto _LL216;} else{
goto _LL159;} _LL216: _temp215=( struct Cyc_List_List*) _temp213.targs; goto
_LL158;} else{ goto _LL159;} _LL159: if(( unsigned int) _temp145 > 4u?*(( int*)
_temp145) == Cyc_Absyn_TunionFieldType: 0){ _LL224: _temp223=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp145)->f1; goto _LL160;} else{
goto _LL161;} _LL161: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) ==
Cyc_Absyn_PointerType: 0){ _LL226: _temp225=( struct Cyc_Absyn_PtrInfo)(( struct
Cyc_Absyn_PointerType_struct*) _temp145)->f1; _LL236: _temp235=( void*) _temp225.elt_typ;
goto _LL234; _LL234: _temp233=( void*) _temp225.rgn_typ; goto _LL232; _LL232:
_temp231=( struct Cyc_Absyn_Conref*) _temp225.nullable; goto _LL230; _LL230:
_temp229=( struct Cyc_Absyn_Tqual) _temp225.tq; goto _LL228; _LL228: _temp227=(
struct Cyc_Absyn_Conref*) _temp225.bounds; goto _LL162;} else{ goto _LL163;}
_LL163: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_IntType:
0){ _LL240: _temp239=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp145)->f1;
goto _LL238; _LL238: _temp237=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f2; if( _temp237 ==( void*) Cyc_Absyn_B1){ goto _LL164;} else{ goto
_LL165;}} else{ goto _LL165;} _LL165: if(( unsigned int) _temp145 > 4u?*(( int*)
_temp145) == Cyc_Absyn_IntType: 0){ _LL244: _temp243=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f1; goto _LL242; _LL242: _temp241=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f2; if( _temp241 ==( void*) Cyc_Absyn_B2){ goto _LL166;} else{ goto
_LL167;}} else{ goto _LL167;} _LL167: if(( unsigned int) _temp145 > 4u?*(( int*)
_temp145) == Cyc_Absyn_IntType: 0){ _LL248: _temp247=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f1; goto _LL246; _LL246: _temp245=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f2; if( _temp245 ==( void*) Cyc_Absyn_B4){ goto _LL168;} else{ goto
_LL169;}} else{ goto _LL169;} _LL169: if(( unsigned int) _temp145 > 4u?*(( int*)
_temp145) == Cyc_Absyn_IntType: 0){ _LL252: _temp251=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f1; goto _LL250; _LL250: _temp249=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp145)->f2; if( _temp249 ==( void*) Cyc_Absyn_B8){ goto _LL170;} else{ goto
_LL171;}} else{ goto _LL171;} _LL171: if( _temp145 ==( void*) Cyc_Absyn_FloatType){
goto _LL172;} else{ goto _LL173;} _LL173: if( _temp145 ==( void*) Cyc_Absyn_DoubleType){
goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp145 > 4u?*((
int*) _temp145) == Cyc_Absyn_ArrayType: 0){ _LL258: _temp257=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp145)->f1; goto _LL256; _LL256: _temp255=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp145)->f2;
goto _LL254; _LL254: _temp253=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp145)->f3; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_FnType: 0){ _LL260: _temp259=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp145)->f1; goto
_LL178;} else{ goto _LL179;} _LL179: if(( unsigned int) _temp145 > 4u?*(( int*)
_temp145) == Cyc_Absyn_TupleType: 0){ _LL262: _temp261=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp145)->f1; goto _LL180;} else{ goto
_LL181;} _LL181: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_RgnHandleType:
0){ _LL264: _temp263=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp145)->f1;
goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int) _temp145 > 4u?*((
int*) _temp145) == Cyc_Absyn_StructType: 0){ _LL270: _temp269=( struct _tuple0*)((
struct Cyc_Absyn_StructType_struct*) _temp145)->f1; goto _LL268; _LL268:
_temp267=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp145)->f2; goto _LL266; _LL266: _temp265=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp145)->f3; goto _LL184;} else{ goto
_LL185;} _LL185: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_UnionType:
0){ _LL276: _temp275=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp145)->f1; goto _LL274; _LL274: _temp273=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp145)->f2; goto _LL272; _LL272: _temp271=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp145)->f3;
goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp145 > 4u?*((
int*) _temp145) == Cyc_Absyn_EnumType: 0){ _LL280: _temp279=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp145)->f1; goto _LL278; _LL278: _temp277=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp145)->f2;
goto _LL188;} else{ goto _LL189;} _LL189: if( _temp145 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int) _temp145 > 4u?*((
int*) _temp145) == Cyc_Absyn_TypedefType: 0){ _LL286: _temp285=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp145)->f1; goto _LL284; _LL284:
_temp283=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp145)->f2; goto _LL282; _LL282: _temp281=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp145)->f3; goto _LL192;} else{ goto _LL193;}
_LL193: if(( unsigned int) _temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_JoinEff:
0){ _LL288: _temp287=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp145)->f1; goto _LL194;} else{ goto _LL195;} _LL195: if(( unsigned int)
_temp145 > 4u?*(( int*) _temp145) == Cyc_Absyn_AccessEff: 0){ _LL290: _temp289=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp145)->f1; goto _LL196;} else{
goto _LL146;} _LL148: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp291=( char*)"cannot apply sizeof to void type"; struct _tagged_string
_temp292; _temp292.curr= _temp291; _temp292.base= _temp291; _temp292.last_plus_one=
_temp291 + 33; _temp292;})); return 0u; _LL150: { void* _temp293=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp197->kind); _LL295: if(
_temp293 ==( void*) Cyc_Absyn_BoxKind){ goto _LL296;} else{ goto _LL297;} _LL297:
if( _temp293 ==( void*) Cyc_Absyn_AnyKind){ goto _LL298;} else{ goto _LL299;}
_LL299: if( _temp293 ==( void*) Cyc_Absyn_MemKind){ goto _LL300;} else{ goto
_LL301;} _LL301: if( _temp293 ==( void*) Cyc_Absyn_RgnKind){ goto _LL302;} else{
goto _LL303;} _LL303: if( _temp293 ==( void*) Cyc_Absyn_EffKind){ goto _LL304;}
else{ goto _LL294;} _LL296: return 4u; _LL298: goto _LL300; _LL300: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp305=( char*)"cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 49; _temp306;})); return 0u; _LL302: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp307=( char*)"cannot apply sizeof to a region";
struct _tagged_string _temp308; _temp308.curr= _temp307; _temp308.base= _temp307;
_temp308.last_plus_one= _temp307 + 32; _temp308;})); return 0u; _LL304: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp309=( char*)"cannot apply sizeof to an effect";
struct _tagged_string _temp310; _temp310.curr= _temp309; _temp310.base= _temp309;
_temp310.last_plus_one= _temp309 + 33; _temp310;})); return 0u; _LL294:;} _LL152:
return 4u; _LL154: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp311=( char*)"cannot apply sizeof to unknown type"; struct _tagged_string
_temp312; _temp312.curr= _temp311; _temp312.base= _temp311; _temp312.last_plus_one=
_temp311 + 36; _temp312;})); return 0u; _LL156: return 4u; _LL158: return 4u +
Cyc_Evexp_szof(( void*)({ struct Cyc_Absyn_TupleType_struct* _temp313=( struct
Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp313[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp314; _temp314.tag= Cyc_Absyn_TupleType;
_temp314.f1= _temp219->typs; _temp314;}); _temp313;}), loc); _LL160:( void)
_throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp315=( char*)"unroslved tunionfield type in szof";
struct _tagged_string _temp316; _temp316.curr= _temp315; _temp316.base= _temp315;
_temp316.last_plus_one= _temp315 + 35; _temp316;}))); _LL162: { void* _temp317=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp227))->v; void* _temp325; void* _temp327; _LL319: if(( unsigned int)
_temp317 > 1u?*(( int*) _temp317) == Cyc_Absyn_Eq_constr: 0){ _LL326: _temp325=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp317)->f1; if( _temp325 ==(
void*) Cyc_Absyn_Unknown_b){ goto _LL320;} else{ goto _LL321;}} else{ goto
_LL321;} _LL321: if(( unsigned int) _temp317 > 1u?*(( int*) _temp317) == Cyc_Absyn_Eq_constr:
0){ _LL328: _temp327=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp317)->f1;
goto _LL322;} else{ goto _LL323;} _LL323: goto _LL324; _LL320: return 12u;
_LL322: return 4u; _LL324: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp329=( char*)"bounds information not known for pointer type"; struct
_tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 46; _temp330;})); return 0u; _LL318:;} _LL164:
return 1u; _LL166: return 1u; _LL168: return 1u; _LL170: return 1u; _LL172:
return 4u; _LL174: return 8u; _LL176: if( _temp253 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp331=( char*)"cannot apply sizeof to an array of unknown size";
struct _tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 48; _temp332;}));} return Cyc_Evexp_szof(
_temp257, loc) * Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp333= _temp253; if( _temp333 == 0){ _throw( Null_Exception);}
_temp333;})); _LL178: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp334=( char*)"cannot apply sizeof to function type"; struct _tagged_string
_temp335; _temp335.curr= _temp334; _temp335.base= _temp334; _temp335.last_plus_one=
_temp334 + 37; _temp335;})); return 0u; _LL180: { unsigned int s= 0; for( 0;
_temp261 != 0; _temp261=({ struct Cyc_List_List* _temp336= _temp261; if(
_temp336 == 0){ _throw( Null_Exception);} _temp336->tl;})){ s += Cyc_Evexp_szof((*((
struct _tuple4*)({ struct Cyc_List_List* _temp337= _temp261; if( _temp337 == 0){
_throw( Null_Exception);} _temp337->hd;}))).f2, loc);} return s;} _LL182: return
4u; _LL184: if( _temp265 == 0){ return(( unsigned int(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp338=( char*)"szof on unchecked StructType";
struct _tagged_string _temp339; _temp339.curr= _temp338; _temp339.base= _temp338;
_temp339.last_plus_one= _temp338 + 29; _temp339;}));}{ struct Cyc_Absyn_Structdecl*
sd=({ struct Cyc_Absyn_Structdecl** _temp345= _temp265; if( _temp345 == 0){
_throw( Null_Exception);}* _temp345;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp340=( char*)"cannot apply sizeof to abstract struct type";
struct _tagged_string _temp341; _temp341.curr= _temp340; _temp341.base= _temp340;
_temp341.last_plus_one= _temp340 + 44; _temp341;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp344= sd->fields; if( _temp344 == 0){ _throw( Null_Exception);} _temp344->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp342= fs; if( _temp342 == 0){
_throw( Null_Exception);} _temp342->tl;})){ s += Cyc_Evexp_szof(( void*)((
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp343= fs; if(
_temp343 == 0){ _throw( Null_Exception);} _temp343->hd;}))->type, loc);}} return
s;}} _LL186: if( _temp271 == 0){ return(( unsigned int(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp346=( char*)"szof on unchecked UnionType";
struct _tagged_string _temp347; _temp347.curr= _temp346; _temp347.base= _temp346;
_temp347.last_plus_one= _temp346 + 28; _temp347;}));}{ struct Cyc_Absyn_Uniondecl*
ud=({ struct Cyc_Absyn_Uniondecl** _temp353= _temp271; if( _temp353 == 0){
_throw( Null_Exception);}* _temp353;}); if( ud->fields == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp348=( char*)"cannot apply sizeof to abstract union type";
struct _tagged_string _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 43; _temp349;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp352= ud->fields; if( _temp352 == 0){ _throw( Null_Exception);} _temp352->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp350= fs; if( _temp350 == 0){
_throw( Null_Exception);} _temp350->tl;})){ unsigned int i= Cyc_Evexp_szof((
void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp351= fs;
if( _temp351 == 0){ _throw( Null_Exception);} _temp351->hd;}))->type, loc); s= s
> i? s: i;}} return s;}} _LL188: return 4u; _LL190: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp354=( char*)"cannot apply sizoef to a region";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 32; _temp355;})); return 0u; _LL192: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp356= Cyc_Absynpp_typ2string( t); xprintf("szof typedeftype %.*s",
_temp356.last_plus_one - _temp356.curr, _temp356.curr);})); _LL194: goto _LL196;
_LL196: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp357=( char*)"cannot apply sizoef to an effect";
struct _tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 33; _temp358;})); return 0u; _LL146:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp359=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp359[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp360; _temp360.tag= Cyc_Absyn_Int_c; _temp360.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp360.f2=( int) Cyc_Evexp_szof( t, loc); _temp360;}); _temp359;});} int Cyc_Evexp_is_arith_const(
void* cn){ void* _temp361= cn; char _temp371; void* _temp373; int _temp375; void*
_temp377; struct _tagged_string _temp379; _LL363: if(( unsigned int) _temp361 >
1u?*(( int*) _temp361) == Cyc_Absyn_Char_c: 0){ _LL374: _temp373=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp361)->f1; goto _LL372; _LL372: _temp371=(
char)(( struct Cyc_Absyn_Char_c_struct*) _temp361)->f2; goto _LL364;} else{ goto
_LL365;} _LL365: if(( unsigned int) _temp361 > 1u?*(( int*) _temp361) == Cyc_Absyn_Int_c:
0){ _LL378: _temp377=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp361)->f1;
goto _LL376; _LL376: _temp375=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp361)->f2;
goto _LL366;} else{ goto _LL367;} _LL367: if(( unsigned int) _temp361 > 1u?*((
int*) _temp361) == Cyc_Absyn_Float_c: 0){ _LL380: _temp379=( struct
_tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp361)->f1; goto _LL368;}
else{ goto _LL369;} _LL369: goto _LL370; _LL364: return 1; _LL366: return 1;
_LL368: return 1; _LL370: return 0; _LL362:;} void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp382=({ struct _tuple3 _temp381; _temp381.f1= p;
_temp381.f2= cn; _temp381;}); void* _temp400; void* _temp402; void* _temp404;
int _temp406; void* _temp408; void* _temp410; void* _temp412; short _temp414;
void* _temp416; void* _temp418; void* _temp420; char _temp422; void* _temp424;
void* _temp426; void* _temp428; int _temp430; void* _temp432; void* _temp434;
void* _temp436; void* _temp438; void* _temp440; int _temp442; void* _temp444;
void* _temp446; _LL384: _LL403: _temp402= _temp382.f1; if( _temp402 ==( void*)
Cyc_Absyn_Plus){ goto _LL401;} else{ goto _LL386;} _LL401: _temp400= _temp382.f2;
goto _LL385; _LL386: _LL411: _temp410= _temp382.f1; if( _temp410 ==( void*) Cyc_Absyn_Minus){
goto _LL405;} else{ goto _LL388;} _LL405: _temp404= _temp382.f2; if((
unsigned int) _temp404 > 1u?*(( int*) _temp404) == Cyc_Absyn_Int_c: 0){ _LL409:
_temp408=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp404)->f1; goto _LL407;
_LL407: _temp406=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp404)->f2; goto
_LL387;} else{ goto _LL388;} _LL388: _LL419: _temp418= _temp382.f1; if( _temp418
==( void*) Cyc_Absyn_Minus){ goto _LL413;} else{ goto _LL390;} _LL413: _temp412=
_temp382.f2; if(( unsigned int) _temp412 > 1u?*(( int*) _temp412) == Cyc_Absyn_Short_c:
0){ _LL417: _temp416=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp412)->f1;
goto _LL415; _LL415: _temp414=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp412)->f2; goto _LL389;} else{ goto _LL390;} _LL390: _LL427: _temp426=
_temp382.f1; if( _temp426 ==( void*) Cyc_Absyn_Minus){ goto _LL421;} else{ goto
_LL392;} _LL421: _temp420= _temp382.f2; if(( unsigned int) _temp420 > 1u?*(( int*)
_temp420) == Cyc_Absyn_Char_c: 0){ _LL425: _temp424=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp420)->f1; goto _LL423; _LL423: _temp422=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp420)->f2; goto _LL391;} else{ goto _LL392;} _LL392: _LL435: _temp434=
_temp382.f1; if( _temp434 ==( void*) Cyc_Absyn_Not){ goto _LL429;} else{ goto
_LL394;} _LL429: _temp428= _temp382.f2; if(( unsigned int) _temp428 > 1u?*(( int*)
_temp428) == Cyc_Absyn_Int_c: 0){ _LL433: _temp432=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp428)->f1; goto _LL431; _LL431: _temp430=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp428)->f2; goto _LL393;} else{ goto _LL394;} _LL394: _LL439: _temp438=
_temp382.f1; if( _temp438 ==( void*) Cyc_Absyn_Not){ goto _LL437;} else{ goto
_LL396;} _LL437: _temp436= _temp382.f2; if( _temp436 ==( void*) Cyc_Absyn_Null_c){
goto _LL395;} else{ goto _LL396;} _LL396: _LL447: _temp446= _temp382.f1; if(
_temp446 ==( void*) Cyc_Absyn_Bitnot){ goto _LL441;} else{ goto _LL398;} _LL441:
_temp440= _temp382.f2; if(( unsigned int) _temp440 > 1u?*(( int*) _temp440) ==
Cyc_Absyn_Int_c: 0){ _LL445: _temp444=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp440)->f1; goto _LL443; _LL443: _temp442=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp440)->f2; goto _LL397;} else{ goto _LL398;} _LL398: goto _LL399; _LL385:
if( ! Cyc_Evexp_is_arith_const( cn)){ Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp448=( char*)"expecting arithmetic constant"; struct
_tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 30; _temp449;}));} return cn; _LL387: return(
void*)({ struct Cyc_Absyn_Int_c_struct* _temp450=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp450[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp451; _temp451.tag= Cyc_Absyn_Int_c; _temp451.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp451.f2= - _temp406; _temp451;}); _temp450;}); _LL389: return( void*)({
struct Cyc_Absyn_Short_c_struct* _temp452=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp452[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp453; _temp453.tag= Cyc_Absyn_Short_c; _temp453.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp453.f2= - _temp414; _temp453;}); _temp452;}); _LL391: return( void*)({
struct Cyc_Absyn_Char_c_struct* _temp454=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp454[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp455; _temp455.tag= Cyc_Absyn_Char_c; _temp455.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp455.f2= - _temp422; _temp455;}); _temp454;}); _LL393: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp456=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp456[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp457; _temp457.tag= Cyc_Absyn_Int_c; _temp457.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp457.f2= _temp430 == 0? 1: 0; _temp457;}); _temp456;}); _LL395: return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp458=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp458[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp459; _temp459.tag= Cyc_Absyn_Int_c; _temp459.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp459.f2= 1; _temp459;}); _temp458;}); _LL397: return( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp460=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp460[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp461; _temp461.tag= Cyc_Absyn_Int_c;
_temp461.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp461.f2= ~ _temp442;
_temp461;}); _temp460;}); _LL399: Cyc_Evexp_exp_err( e->loc,( struct
_tagged_string)({ char* _temp462=( char*)"bad constant expression"; struct
_tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 24; _temp463;})); return cn; _LL383:;} struct
_tuple5{ void* f1; int f2; } ; void* Cyc_Evexp_eval_const_binprimop( void* p,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* cn1= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e1)); void* cn2= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e2)); void* s1; void* s2; int i1; int i2;{ void* _temp464= cn1; int _temp470;
void* _temp472; _LL466: if(( unsigned int) _temp464 > 1u?*(( int*) _temp464) ==
Cyc_Absyn_Int_c: 0){ _LL473: _temp472=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp464)->f1; goto _LL471; _LL471: _temp470=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp464)->f2; goto _LL467;} else{ goto _LL468;} _LL468: goto _LL469; _LL467: s1=
_temp472; i1= _temp470; goto _LL465; _LL469: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp474=( char*)"bad constant expression"; struct
_tagged_string _temp475; _temp475.curr= _temp474; _temp475.base= _temp474;
_temp475.last_plus_one= _temp474 + 24; _temp475;})); return cn1; _LL465:;}{ void*
_temp476= cn2; int _temp482; void* _temp484; _LL478: if(( unsigned int) _temp476
> 1u?*(( int*) _temp476) == Cyc_Absyn_Int_c: 0){ _LL485: _temp484=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp476)->f1; goto _LL483; _LL483: _temp482=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp476)->f2; goto _LL479;} else{ goto
_LL480;} _LL480: goto _LL481; _LL479: s2= _temp484; i2= _temp482; goto _LL477;
_LL481: Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char* _temp486=(
char*)"bad constant expression"; struct _tagged_string _temp487; _temp487.curr=
_temp486; _temp487.base= _temp486; _temp487.last_plus_one= _temp486 + 24;
_temp487;})); return cn1; _LL477:;}{ void* _temp488= p; _LL490: if( _temp488 ==(
void*) Cyc_Absyn_Div){ goto _LL491;} else{ goto _LL492;} _LL492: if( _temp488 ==(
void*) Cyc_Absyn_Mod){ goto _LL493;} else{ goto _LL494;} _LL494: goto _LL495;
_LL491: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp496=( char*)"division by zero in constant expression"; struct
_tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 40; _temp497;})); return cn1;} goto _LL489;
_LL493: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp498=( char*)"division by zero in constant expression"; struct
_tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 40; _temp499;})); return cn1;} goto _LL489;
_LL495: goto _LL489; _LL489:;}{ int has_u_arg= s1 ==( void*) Cyc_Absyn_Unsigned?
1: s2 ==( void*) Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1;
unsigned int u2=( unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1;
int use_i3= 0; int use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp501=({ struct
_tuple5 _temp500; _temp500.f1= p; _temp500.f2= has_u_arg; _temp500;}); int
_temp557; void* _temp559; int _temp561; void* _temp563; int _temp565; void*
_temp567; int _temp569; void* _temp571; int _temp573; void* _temp575; int
_temp577; void* _temp579; int _temp581; void* _temp583; int _temp585; void*
_temp587; int _temp589; void* _temp591; int _temp593; void* _temp595; int
_temp597; void* _temp599; int _temp601; void* _temp603; int _temp605; void*
_temp607; int _temp609; void* _temp611; int _temp613; void* _temp615; int
_temp617; void* _temp619; int _temp621; void* _temp623; int _temp625; void*
_temp627; int _temp629; void* _temp631; int _temp633; void* _temp635; int
_temp637; void* _temp639; int _temp641; void* _temp643; int _temp645; void*
_temp647; int _temp649; void* _temp651; int _temp653; void* _temp655; int
_temp657; void* _temp659; _LL503: _LL560: _temp559= _temp501.f1; if( _temp559 ==(
void*) Cyc_Absyn_Plus){ goto _LL558;} else{ goto _LL505;} _LL558: _temp557=
_temp501.f2; if( _temp557 == 0){ goto _LL504;} else{ goto _LL505;} _LL505:
_LL564: _temp563= _temp501.f1; if( _temp563 ==( void*) Cyc_Absyn_Times){ goto
_LL562;} else{ goto _LL507;} _LL562: _temp561= _temp501.f2; if( _temp561 == 0){
goto _LL506;} else{ goto _LL507;} _LL507: _LL568: _temp567= _temp501.f1; if(
_temp567 ==( void*) Cyc_Absyn_Minus){ goto _LL566;} else{ goto _LL509;} _LL566:
_temp565= _temp501.f2; if( _temp565 == 0){ goto _LL508;} else{ goto _LL509;}
_LL509: _LL572: _temp571= _temp501.f1; if( _temp571 ==( void*) Cyc_Absyn_Div){
goto _LL570;} else{ goto _LL511;} _LL570: _temp569= _temp501.f2; if( _temp569 ==
0){ goto _LL510;} else{ goto _LL511;} _LL511: _LL576: _temp575= _temp501.f1; if(
_temp575 ==( void*) Cyc_Absyn_Mod){ goto _LL574;} else{ goto _LL513;} _LL574:
_temp573= _temp501.f2; if( _temp573 == 0){ goto _LL512;} else{ goto _LL513;}
_LL513: _LL580: _temp579= _temp501.f1; if( _temp579 ==( void*) Cyc_Absyn_Plus){
goto _LL578;} else{ goto _LL515;} _LL578: _temp577= _temp501.f2; if( _temp577 ==
1){ goto _LL514;} else{ goto _LL515;} _LL515: _LL584: _temp583= _temp501.f1; if(
_temp583 ==( void*) Cyc_Absyn_Times){ goto _LL582;} else{ goto _LL517;} _LL582:
_temp581= _temp501.f2; if( _temp581 == 1){ goto _LL516;} else{ goto _LL517;}
_LL517: _LL588: _temp587= _temp501.f1; if( _temp587 ==( void*) Cyc_Absyn_Minus){
goto _LL586;} else{ goto _LL519;} _LL586: _temp585= _temp501.f2; if( _temp585 ==
1){ goto _LL518;} else{ goto _LL519;} _LL519: _LL592: _temp591= _temp501.f1; if(
_temp591 ==( void*) Cyc_Absyn_Div){ goto _LL590;} else{ goto _LL521;} _LL590:
_temp589= _temp501.f2; if( _temp589 == 1){ goto _LL520;} else{ goto _LL521;}
_LL521: _LL596: _temp595= _temp501.f1; if( _temp595 ==( void*) Cyc_Absyn_Mod){
goto _LL594;} else{ goto _LL523;} _LL594: _temp593= _temp501.f2; if( _temp593 ==
1){ goto _LL522;} else{ goto _LL523;} _LL523: _LL600: _temp599= _temp501.f1; if(
_temp599 ==( void*) Cyc_Absyn_Eq){ goto _LL598;} else{ goto _LL525;} _LL598:
_temp597= _temp501.f2; goto _LL524; _LL525: _LL604: _temp603= _temp501.f1; if(
_temp603 ==( void*) Cyc_Absyn_Neq){ goto _LL602;} else{ goto _LL527;} _LL602:
_temp601= _temp501.f2; goto _LL526; _LL527: _LL608: _temp607= _temp501.f1; if(
_temp607 ==( void*) Cyc_Absyn_Gt){ goto _LL606;} else{ goto _LL529;} _LL606:
_temp605= _temp501.f2; if( _temp605 == 0){ goto _LL528;} else{ goto _LL529;}
_LL529: _LL612: _temp611= _temp501.f1; if( _temp611 ==( void*) Cyc_Absyn_Lt){
goto _LL610;} else{ goto _LL531;} _LL610: _temp609= _temp501.f2; if( _temp609 ==
0){ goto _LL530;} else{ goto _LL531;} _LL531: _LL616: _temp615= _temp501.f1; if(
_temp615 ==( void*) Cyc_Absyn_Gte){ goto _LL614;} else{ goto _LL533;} _LL614:
_temp613= _temp501.f2; if( _temp613 == 0){ goto _LL532;} else{ goto _LL533;}
_LL533: _LL620: _temp619= _temp501.f1; if( _temp619 ==( void*) Cyc_Absyn_Lte){
goto _LL618;} else{ goto _LL535;} _LL618: _temp617= _temp501.f2; if( _temp617 ==
0){ goto _LL534;} else{ goto _LL535;} _LL535: _LL624: _temp623= _temp501.f1; if(
_temp623 ==( void*) Cyc_Absyn_Gt){ goto _LL622;} else{ goto _LL537;} _LL622:
_temp621= _temp501.f2; if( _temp621 == 1){ goto _LL536;} else{ goto _LL537;}
_LL537: _LL628: _temp627= _temp501.f1; if( _temp627 ==( void*) Cyc_Absyn_Lt){
goto _LL626;} else{ goto _LL539;} _LL626: _temp625= _temp501.f2; if( _temp625 ==
1){ goto _LL538;} else{ goto _LL539;} _LL539: _LL632: _temp631= _temp501.f1; if(
_temp631 ==( void*) Cyc_Absyn_Gte){ goto _LL630;} else{ goto _LL541;} _LL630:
_temp629= _temp501.f2; if( _temp629 == 1){ goto _LL540;} else{ goto _LL541;}
_LL541: _LL636: _temp635= _temp501.f1; if( _temp635 ==( void*) Cyc_Absyn_Lte){
goto _LL634;} else{ goto _LL543;} _LL634: _temp633= _temp501.f2; if( _temp633 ==
1){ goto _LL542;} else{ goto _LL543;} _LL543: _LL640: _temp639= _temp501.f1; if(
_temp639 ==( void*) Cyc_Absyn_Bitand){ goto _LL638;} else{ goto _LL545;} _LL638:
_temp637= _temp501.f2; goto _LL544; _LL545: _LL644: _temp643= _temp501.f1; if(
_temp643 ==( void*) Cyc_Absyn_Bitor){ goto _LL642;} else{ goto _LL547;} _LL642:
_temp641= _temp501.f2; goto _LL546; _LL547: _LL648: _temp647= _temp501.f1; if(
_temp647 ==( void*) Cyc_Absyn_Bitxor){ goto _LL646;} else{ goto _LL549;} _LL646:
_temp645= _temp501.f2; goto _LL548; _LL549: _LL652: _temp651= _temp501.f1; if(
_temp651 ==( void*) Cyc_Absyn_Bitlshift){ goto _LL650;} else{ goto _LL551;}
_LL650: _temp649= _temp501.f2; goto _LL550; _LL551: _LL656: _temp655= _temp501.f1;
if( _temp655 ==( void*) Cyc_Absyn_Bitlrshift){ goto _LL654;} else{ goto _LL553;}
_LL654: _temp653= _temp501.f2; goto _LL552; _LL553: _LL660: _temp659= _temp501.f1;
if( _temp659 ==( void*) Cyc_Absyn_Bitarshift){ goto _LL658;} else{ goto _LL555;}
_LL658: _temp657= _temp501.f2; goto _LL554; _LL555: goto _LL556; _LL504: i3= i1
+ i2; use_i3= 1; goto _LL502; _LL506: i3= i1 * i2; use_i3= 1; goto _LL502;
_LL508: i3= i1 - i2; use_i3= 1; goto _LL502; _LL510: i3= i1 / i2; use_i3= 1;
goto _LL502; _LL512: i3= i1 % i2; use_i3= 1; goto _LL502; _LL514: u3= u1 + u2;
use_u3= 1; goto _LL502; _LL516: u3= u1 * u2; use_u3= 1; goto _LL502; _LL518: u3=
u1 - u2; use_u3= 1; goto _LL502; _LL520: u3= u1 / u2; use_u3= 1; goto _LL502;
_LL522: u3= u1 % u2; use_u3= 1; goto _LL502; _LL524: b3= i1 == i2; use_b3= 1;
goto _LL502; _LL526: b3= i1 != i2; use_b3= 1; goto _LL502; _LL528: b3= i1 > i2;
use_b3= 1; goto _LL502; _LL530: b3= i1 < i2; use_b3= 1; goto _LL502; _LL532: b3=
i1 >= i2; use_b3= 1; goto _LL502; _LL534: b3= i1 <= i2; use_b3= 1; goto _LL502;
_LL536: b3= u1 > u2; use_b3= 1; goto _LL502; _LL538: b3= u1 < u2; use_b3= 1;
goto _LL502; _LL540: b3= u1 >= u2; use_b3= 1; goto _LL502; _LL542: b3= u1 <= u2;
use_b3= 1; goto _LL502; _LL544: u3= u1 & u2; use_u3= 1; goto _LL502; _LL546: u3=
u1 | u2; use_u3= 1; goto _LL502; _LL548: u3= u1 ^ u2; use_u3= 1; goto _LL502;
_LL550: u3= u1 << u2; use_u3= 1; goto _LL502; _LL552: u3= u1 >> u2; use_u3= 1;
goto _LL502; _LL554: Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char*
_temp661=( char*)">>> NOT IMPLEMENTED"; struct _tagged_string _temp662; _temp662.curr=
_temp661; _temp662.base= _temp661; _temp662.last_plus_one= _temp661 + 20;
_temp662;})); return cn1; _LL556: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp663=( char*)"bad constant expression"; struct
_tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 24; _temp664;})); return cn1; _LL502:;} if(
use_i3){ return( void*)({ struct Cyc_Absyn_Int_c_struct* _temp665=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp665[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp666; _temp666.tag= Cyc_Absyn_Int_c; _temp666.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp666.f2= i3; _temp666;}); _temp665;});} if( use_u3){ return( void*)({ struct
Cyc_Absyn_Int_c_struct* _temp667=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp667[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp668; _temp668.tag= Cyc_Absyn_Int_c; _temp668.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp668.f2=( int) u3; _temp668;}); _temp667;});} if( use_b3){ return( void*)({
struct Cyc_Absyn_Int_c_struct* _temp669=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp669[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp670; _temp670.tag= Cyc_Absyn_Int_c; _temp670.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp670.f2= b3? 1: 0; _temp670;}); _temp669;});}( void) _throw(( void*)({
struct Cyc_Core_Unreachable_struct* _temp671=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp671[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp672; _temp672.tag= Cyc_Core_Unreachable;
_temp672.f1=( struct _tagged_string)({ char* _temp673=( char*)"Evexp::eval_const_binop";
struct _tagged_string _temp674; _temp674.curr= _temp673; _temp674.base= _temp673;
_temp674.last_plus_one= _temp673 + 24; _temp674;}); _temp672;}); _temp671;}));}}