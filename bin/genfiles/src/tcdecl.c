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
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_exists_c( int(* pred)(
void*, void*), void* env, struct Cyc_List_List* x); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext[ 14u]; typedef struct
_tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple0*
Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern int Cyc_String_strptrcmp( struct _tagged_string* s1, struct
_tagged_string* s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strconcat(
struct _tagged_string, struct _tagged_string); extern char Cyc_Tcdecl_Incompatible[
17u]; extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment* loc, struct
_tagged_string* msg0, struct _tagged_string msg1); struct _tuple3{ void* f1; int
f2; } ; extern struct _tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1, struct
_tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment* loc,
struct _tagged_string* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern void* Cyc_Tcdecl_merge_binding( void*
d0, void* d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg);
extern struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_sub_structdecl(
struct Cyc_Absyn_Structdecl* ye, struct Cyc_Absyn_Structdecl* y, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg, int* res); extern struct Cyc_Absyn_Uniondecl*
Cyc_Tcdecl_sub_uniondecl( struct Cyc_Absyn_Uniondecl* ye, struct Cyc_Absyn_Uniondecl*
y, struct Cyc_Position_Segment* loc, struct _tagged_string* msg, int* res);
extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_sub_tuniondecl( struct Cyc_Absyn_Tuniondecl*
ye, struct Cyc_Absyn_Tuniondecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_sub_enumdecl(
struct Cyc_Absyn_Enumdecl* ye, struct Cyc_Absyn_Enumdecl* y, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg, int* res); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_sub_vardecl(
struct Cyc_Absyn_Vardecl* ye, struct Cyc_Absyn_Vardecl* y, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg, int* res); extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_sub_typedefdecl( struct Cyc_Absyn_Typedefdecl* ye, struct Cyc_Absyn_Typedefdecl*
y, struct Cyc_Position_Segment* loc, struct _tagged_string* msg, int* res);
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern int Cyc_Tcutil_equal_tqual( struct Cyc_Absyn_Tqual
tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts( struct Cyc_List_List*,
struct Cyc_List_List*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_typ2string( void*); extern struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_scope2string( void*
sc); char Cyc_Tcdecl_Incompatible[ 17u]; void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_string* msg1, struct _tagged_string msg2){ if( msg1 == 0){(
void) _throw(( void*) Cyc_Tcdecl_Incompatible);} else{ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1=({ struct _tagged_string* _temp0= msg1; if( _temp0
== 0){ _throw( Null_Exception);}* _temp0;}); struct _tagged_string _temp2= msg2;
xprintf("%.*s %.*s", _temp1.last_plus_one - _temp1.curr, _temp1.curr, _temp2.last_plus_one
- _temp2.curr, _temp2.curr);}));}} static void Cyc_Tcdecl_merge_scope_err( void*
s0, void* s1, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp3= t; struct _tagged_string _temp4= v; struct _tagged_string
_temp5= Cyc_Absynpp_scope2string( s1); struct _tagged_string _temp6= Cyc_Absynpp_scope2string(
s0); xprintf("%.*s %.*s is %.*s whereas expected scope is %.*s", _temp3.last_plus_one
- _temp3.curr, _temp3.curr, _temp4.last_plus_one - _temp4.curr, _temp4.curr,
_temp5.last_plus_one - _temp5.curr, _temp5.curr, _temp6.last_plus_one - _temp6.curr,
_temp6.curr);}));} struct _tuple4{ void* f1; void* f2; } ; struct _tuple3 Cyc_Tcdecl_merge_scope(
void* s0, void* s1, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){{ struct _tuple4 _temp8=({ struct _tuple4
_temp7; _temp7.f1= s0; _temp7.f2= s1; _temp7;}); void* _temp28; void* _temp30;
void* _temp32; void* _temp34; void* _temp36; void* _temp38; void* _temp40; void*
_temp42; void* _temp44; void* _temp46; void* _temp48; void* _temp50; void*
_temp52; void* _temp54; void* _temp56; void* _temp58; _LL10: _LL31: _temp30=
_temp8.f1; if( _temp30 ==( void*) Cyc_Absyn_ExternC){ goto _LL29;} else{ goto
_LL12;} _LL29: _temp28= _temp8.f2; if( _temp28 ==( void*) Cyc_Absyn_ExternC){
goto _LL11;} else{ goto _LL12;} _LL12: _LL35: _temp34= _temp8.f1; if( _temp34 ==(
void*) Cyc_Absyn_ExternC){ goto _LL33;} else{ goto _LL14;} _LL33: _temp32=
_temp8.f2; goto _LL13; _LL14: _LL39: _temp38= _temp8.f1; goto _LL37; _LL37:
_temp36= _temp8.f2; if( _temp36 ==( void*) Cyc_Absyn_ExternC){ goto _LL15;}
else{ goto _LL16;} _LL16: _LL43: _temp42= _temp8.f1; goto _LL41; _LL41: _temp40=
_temp8.f2; if( _temp40 ==( void*) Cyc_Absyn_Extern){ goto _LL17;} else{ goto
_LL18;} _LL18: _LL47: _temp46= _temp8.f1; if( _temp46 ==( void*) Cyc_Absyn_Extern){
goto _LL45;} else{ goto _LL20;} _LL45: _temp44= _temp8.f2; goto _LL19; _LL20:
_LL51: _temp50= _temp8.f1; if( _temp50 ==( void*) Cyc_Absyn_Static){ goto _LL49;}
else{ goto _LL22;} _LL49: _temp48= _temp8.f2; if( _temp48 ==( void*) Cyc_Absyn_Static){
goto _LL21;} else{ goto _LL22;} _LL22: _LL55: _temp54= _temp8.f1; if( _temp54 ==(
void*) Cyc_Absyn_Public){ goto _LL53;} else{ goto _LL24;} _LL53: _temp52= _temp8.f2;
if( _temp52 ==( void*) Cyc_Absyn_Public){ goto _LL23;} else{ goto _LL24;} _LL24:
_LL59: _temp58= _temp8.f1; if( _temp58 ==( void*) Cyc_Absyn_Abstract){ goto
_LL57;} else{ goto _LL26;} _LL57: _temp56= _temp8.f2; if( _temp56 ==( void*) Cyc_Absyn_Abstract){
goto _LL25;} else{ goto _LL26;} _LL26: goto _LL27; _LL11: goto _LL9; _LL13: goto
_LL15; _LL15: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp60; _temp60.f1= s1; _temp60.f2= 0; _temp60;}); _LL17: s1= s0;
goto _LL9; _LL19: goto _LL9; _LL21: goto _LL9; _LL23: goto _LL9; _LL25: goto
_LL9; _LL27: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp61; _temp61.f1= s1; _temp61.f2= 0; _temp61;}); _LL9:;}
return({ struct _tuple3 _temp62; _temp62.f1= s1; _temp62.f2= 1; _temp62;});}
static int Cyc_Tcdecl_check_type( void* t0, void* t1){ return Cyc_Tcutil_unify(
t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value( struct Cyc_Absyn_Exp*
e){ void* _temp63=( void*) e->r; void* _temp69; int _temp71; void* _temp73;
_LL65: if(*(( int*) _temp63) == Cyc_Absyn_Const_e){ _LL70: _temp69=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp63)->f1; if(( unsigned int) _temp69 > 1u?*((
int*) _temp69) == Cyc_Absyn_Int_c: 0){ _LL74: _temp73=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp69)->f1; goto _LL72; _LL72: _temp71=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp69)->f2; goto _LL66;} else{ goto _LL67;}} else{ goto _LL67;} _LL67: goto
_LL68; _LL66: return( unsigned int) _temp71; _LL68:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp75=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp75[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp76; _temp76.tag= Cyc_Core_InvalidArg; _temp76.f1=(
struct _tagged_string)({ char* _temp77=( char*)"Tcdecl::get_uint_const_value";
struct _tagged_string _temp78; _temp78.curr= _temp77; _temp78.base= _temp77;
_temp78.last_plus_one= _temp77 + 29; _temp78;}); _temp76;}); _temp75;})); _LL64:;}
inline static int Cyc_Tcdecl_check_tvs( struct Cyc_List_List* tvs0, struct Cyc_List_List*
tvs1, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs0) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp79= t; struct _tagged_string _temp80= v;
xprintf("%.*s %.*s has a different number of type parameters", _temp79.last_plus_one
- _temp79.curr, _temp79.curr, _temp80.last_plus_one - _temp80.curr, _temp80.curr);}));
return 0;}{ struct Cyc_List_List* _temp81= tvs0; goto _LL82; _LL82: { struct Cyc_List_List*
_temp83= tvs1; goto _LL84; _LL84: for( 0; _temp81 != 0; _temp81=({ struct Cyc_List_List*
_temp85= _temp81; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->tl;}),
_temp83=({ struct Cyc_List_List* _temp86= _temp83; if( _temp86 == 0){ _throw(
Null_Exception);} _temp86->tl;})){ struct Cyc_Absyn_Conref* _temp88=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp87= _temp81; if( _temp87 ==
0){ _throw( Null_Exception);} _temp87->hd;}))->kind); goto _LL89; _LL89: {
struct Cyc_Absyn_Conref* _temp91=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp90= _temp83; if( _temp90 == 0){ _throw( Null_Exception);} _temp90->hd;}))->kind);
goto _LL92; _LL92: if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp88) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp91)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp94= t; struct
_tagged_string _temp95= v; struct _tagged_string _temp96= Cyc_Absynpp_ckind2string(
_temp88); struct _tagged_string _temp97=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp93= _temp81; if( _temp93 == 0){ _throw( Null_Exception);} _temp93->hd;}))->name;
struct _tagged_string _temp98= Cyc_Absynpp_ckind2string( _temp91); xprintf("%.*s %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp94.last_plus_one - _temp94.curr, _temp94.curr, _temp95.last_plus_one -
_temp95.curr, _temp95.curr, _temp96.last_plus_one - _temp96.curr, _temp96.curr,
_temp97.last_plus_one - _temp97.curr, _temp97.curr, _temp98.last_plus_one -
_temp98.curr, _temp98.curr);})); return 0;}}} return 1;}}} inline static int Cyc_Tcdecl_check_atts(
struct Cyc_List_List* atts0, struct Cyc_List_List* atts1, struct _tagged_string
t, struct _tagged_string v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp99= t; struct _tagged_string _temp100= v;
xprintf("%.*s %.*s has different attributes", _temp99.last_plus_one - _temp99.curr,
_temp99.curr, _temp100.last_plus_one - _temp100.curr, _temp100.curr);})); return
0;} else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* _temp101= 0; goto
_LL102; _LL102: for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List* _temp103= tvs0;
if( _temp103 == 0){ _throw( Null_Exception);} _temp103->tl;}), tvs1=({ struct
Cyc_List_List* _temp104= tvs1; if( _temp104 == 0){ _throw( Null_Exception);}
_temp104->tl;})){ _temp101=({ struct Cyc_List_List* _temp105=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp105->hd=( void*)({ struct
_tuple5* _temp106=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp106->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp110= tvs1;
if( _temp110 == 0){ _throw( Null_Exception);} _temp110->hd;}); _temp106->f2=(
void*)({ struct Cyc_Absyn_VarType_struct* _temp107=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp107[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp108; _temp108.tag= Cyc_Absyn_VarType; _temp108.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp109= tvs0; if( _temp109 == 0){ _throw( Null_Exception);}
_temp109->hd;}); _temp108;}); _temp107;}); _temp106;}); _temp105->tl= _temp101;
_temp105;});} return _temp101;} struct _tuple6{ struct Cyc_Core_Opt* f1; struct
Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_string t, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){
struct _tagged_string _temp112= Cyc_Absynpp_qvar2string(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp111= d0->name; if( _temp111 == 0){ _throw(
Null_Exception);} _temp111->v;})); goto _LL113; _LL113: { int _temp114= 1; goto
_LL115; _LL115: if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, t, _temp112, loc,
msg)){ return 0;}{ int _temp118; void* _temp120; struct _tuple3 _temp116= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc, t, _temp112, loc, msg); _LL121: _temp120=
_temp116.f1; goto _LL119; _LL119: _temp118= _temp116.f2; goto _LL117; _LL117:
if( ! _temp118){ _temp114= 0;} if( ! Cyc_Tcdecl_check_atts( d0->attributes, d1->attributes,
t, _temp112, loc, msg)){ _temp114= 0;}{ struct Cyc_Absyn_Structdecl* d2;{ struct
_tuple6 _temp123=({ struct _tuple6 _temp122; _temp122.f1= d0->fields; _temp122.f2=
d1->fields; _temp122;}); struct Cyc_Core_Opt* _temp131; struct Cyc_Core_Opt*
_temp133; struct Cyc_Core_Opt* _temp135; struct Cyc_Core_Opt* _temp137; struct
Cyc_Core_Opt* _temp139; struct Cyc_Core_Opt _temp141; struct Cyc_List_List*
_temp142; struct Cyc_Core_Opt* _temp144; struct Cyc_Core_Opt _temp146; struct
Cyc_List_List* _temp147; _LL125: _LL134: _temp133= _temp123.f1; goto _LL132;
_LL132: _temp131= _temp123.f2; if( _temp131 == 0){ goto _LL126;} else{ goto
_LL127;} _LL127: _LL138: _temp137= _temp123.f1; if( _temp137 == 0){ goto _LL136;}
else{ goto _LL129;} _LL136: _temp135= _temp123.f2; goto _LL128; _LL129: _LL145:
_temp144= _temp123.f1; if( _temp144 == 0){ goto _LL124;} else{ _temp146=*
_temp144; _LL148: _temp147=( struct Cyc_List_List*) _temp146.v; goto _LL140;}
_LL140: _temp139= _temp123.f2; if( _temp139 == 0){ goto _LL124;} else{ _temp141=*
_temp139; _LL143: _temp142=( struct Cyc_List_List*) _temp141.v; goto _LL130;}
_LL126: d2= d0; goto _LL124; _LL128: d2= d1; goto _LL124; _LL130: { struct Cyc_List_List*
_temp149= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); goto _LL150; _LL150: for(
0; _temp147 != 0? _temp142 != 0: 0; _temp147=({ struct Cyc_List_List* _temp151=
_temp147; if( _temp151 == 0){ _throw( Null_Exception);} _temp151->tl;}),
_temp142=({ struct Cyc_List_List* _temp152= _temp142; if( _temp152 == 0){ _throw(
Null_Exception);} _temp152->tl;})){ struct Cyc_Absyn_Structfield _temp156;
struct Cyc_List_List* _temp157; struct Cyc_Core_Opt* _temp159; void* _temp161;
struct Cyc_Absyn_Tqual _temp163; struct _tagged_string* _temp165; struct Cyc_Absyn_Structfield*
_temp154=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp153=
_temp147; if( _temp153 == 0){ _throw( Null_Exception);} _temp153->hd;});
_temp156=* _temp154; _LL166: _temp165=( struct _tagged_string*) _temp156.name;
goto _LL164; _LL164: _temp163=( struct Cyc_Absyn_Tqual) _temp156.tq; goto _LL162;
_LL162: _temp161=( void*) _temp156.type; goto _LL160; _LL160: _temp159=( struct
Cyc_Core_Opt*) _temp156.width; goto _LL158; _LL158: _temp157=( struct Cyc_List_List*)
_temp156.attributes; goto _LL155; _LL155: { struct Cyc_Absyn_Structfield
_temp170; struct Cyc_List_List* _temp171; struct Cyc_Core_Opt* _temp173; void*
_temp175; struct Cyc_Absyn_Tqual _temp177; struct _tagged_string* _temp179;
struct Cyc_Absyn_Structfield* _temp168=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp167= _temp142; if( _temp167 == 0){ _throw( Null_Exception);}
_temp167->hd;}); _temp170=* _temp168; _LL180: _temp179=( struct _tagged_string*)
_temp170.name; goto _LL178; _LL178: _temp177=( struct Cyc_Absyn_Tqual) _temp170.tq;
goto _LL176; _LL176: _temp175=( void*) _temp170.type; goto _LL174; _LL174:
_temp173=( struct Cyc_Core_Opt*) _temp170.width; goto _LL172; _LL172: _temp171=(
struct Cyc_List_List*) _temp170.attributes; goto _LL169; _LL169: if( Cyc_String_zstrptrcmp(
_temp165, _temp179) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp181= t; struct _tagged_string _temp182= _temp112; struct _tagged_string
_temp183=* _temp165; struct _tagged_string _temp184=* _temp179; xprintf("%.*s %.*s : field name mismatch %.*s != %.*s",
_temp181.last_plus_one - _temp181.curr, _temp181.curr, _temp182.last_plus_one -
_temp182.curr, _temp182.curr, _temp183.last_plus_one - _temp183.curr, _temp183.curr,
_temp184.last_plus_one - _temp184.curr, _temp184.curr);})); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp157, _temp171)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp185= t; struct _tagged_string _temp186= _temp112; struct _tagged_string
_temp187=* _temp165; xprintf("%.*s %.*s : attribute mismatch on field %.*s",
_temp185.last_plus_one - _temp185.curr, _temp185.curr, _temp186.last_plus_one -
_temp186.curr, _temp186.curr, _temp187.last_plus_one - _temp187.curr, _temp187.curr);}));
_temp114= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp163, _temp177)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp188= t; struct _tagged_string _temp189=
_temp112; struct _tagged_string _temp190=* _temp165; xprintf("%.*s %.*s : qualifier mismatch on field %.*s",
_temp188.last_plus_one - _temp188.curr, _temp188.curr, _temp189.last_plus_one -
_temp189.curr, _temp189.curr, _temp190.last_plus_one - _temp190.curr, _temp190.curr);}));
_temp114= 0;} if(((( _temp159 != 0? _temp173 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp191= _temp159; if( _temp191
== 0){ _throw( Null_Exception);} _temp191->v;})) != Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp192= _temp173; if( _temp192
== 0){ _throw( Null_Exception);} _temp192->v;})): 0)? 1:( _temp159 == 0?
_temp173 != 0: 0))? 1:( _temp159 != 0? _temp173 == 0: 0)){ Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp193= t; struct _tagged_string _temp194=
_temp112; struct _tagged_string _temp195=* _temp165; xprintf("%.*s %.*s : bitfield mismatch on field %.*s",
_temp193.last_plus_one - _temp193.curr, _temp193.curr, _temp194.last_plus_one -
_temp194.curr, _temp194.curr, _temp195.last_plus_one - _temp195.curr, _temp195.curr);}));
_temp114= 0;}{ void* subst_t1= Cyc_Tcutil_substitute( _temp149, _temp175); if( !
Cyc_Tcdecl_check_type( _temp161, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp196= t; struct _tagged_string _temp197= _temp112; struct
_tagged_string _temp198=* _temp165; struct _tagged_string _temp199= Cyc_Absynpp_typ2string(
_temp161); struct _tagged_string _temp200= Cyc_Absynpp_typ2string( subst_t1);
xprintf("%.*s %.*s : type mismatch on field %.*s: %.*s != %.*s", _temp196.last_plus_one
- _temp196.curr, _temp196.curr, _temp197.last_plus_one - _temp197.curr, _temp197.curr,
_temp198.last_plus_one - _temp198.curr, _temp198.curr, _temp199.last_plus_one -
_temp199.curr, _temp199.curr, _temp200.last_plus_one - _temp200.curr, _temp200.curr);}));
_temp114= 0;}}}} if( _temp147 != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp202= t; struct _tagged_string _temp203= _temp112; struct
_tagged_string _temp204=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp201= _temp147; if( _temp201 == 0){ _throw( Null_Exception);} _temp201->hd;}))->name;
xprintf("%.*s %.*s is missing field %.*s", _temp202.last_plus_one - _temp202.curr,
_temp202.curr, _temp203.last_plus_one - _temp203.curr, _temp203.curr, _temp204.last_plus_one
- _temp204.curr, _temp204.curr);})); _temp114= 0;} if( _temp142 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp206= t; struct _tagged_string _temp207=
_temp112; struct _tagged_string _temp208=*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp205= _temp142; if( _temp205 == 0){ _throw(
Null_Exception);} _temp205->hd;}))->name; xprintf("%.*s %.*s has extra field %.*s",
_temp206.last_plus_one - _temp206.curr, _temp206.curr, _temp207.last_plus_one -
_temp207.curr, _temp207.curr, _temp208.last_plus_one - _temp208.curr, _temp208.curr);}));
_temp114= 0;} d2= d0; goto _LL124;} _LL124:;} if( ! _temp114){ return 0;} if(
_temp120 ==( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*) d2;} else{ d2=({
struct Cyc_Absyn_Structdecl* _temp209=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp209[ 0]=* d2; _temp209;});( void*)(
d2->sc=( void*) _temp120); return( struct Cyc_Absyn_Structdecl*) d2;}}}}} struct
Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0, d1,(
struct _tagged_string)({ char* _temp210=( char*)"struct"; struct _tagged_string
_temp211; _temp211.curr= _temp210; _temp211.base= _temp210; _temp211.last_plus_one=
_temp210 + 7; _temp211;}), loc, msg);} struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ return( struct Cyc_Absyn_Uniondecl*) Cyc_Tcdecl_merge_struct_or_union_decl((
struct Cyc_Absyn_Structdecl*) d0,( struct Cyc_Absyn_Structdecl*) d1,( struct
_tagged_string)({ char* _temp212=( char*)"union"; struct _tagged_string _temp213;
_temp213.curr= _temp212; _temp213.base= _temp212; _temp213.last_plus_one=
_temp212 + 6; _temp213;}), loc, msg);} inline static struct _tagged_string Cyc_Tcdecl_is_x2string(
int is_x){ if( is_x){ return( struct _tagged_string)({ char* _temp214=( char*)"xtunion";
struct _tagged_string _temp215; _temp215.curr= _temp214; _temp215.base= _temp214;
_temp215.last_plus_one= _temp214 + 8; _temp215;});} else{ return( struct
_tagged_string)({ char* _temp216=( char*)"tunion"; struct _tagged_string
_temp217; _temp217.curr= _temp216; _temp217.base= _temp216; _temp217.last_plus_one=
_temp216 + 7; _temp217;});}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_merge_tunionfield( struct
Cyc_Absyn_Tunionfield* f0, struct Cyc_Absyn_Tunionfield* f1, struct Cyc_List_List*
inst, struct _tagged_string t, struct _tagged_string v, struct _tagged_string*
msg){ struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_String_zstrptrcmp((* f0->name).f2,(*
f1->name).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp218=
t; struct _tagged_string _temp219= v; struct _tagged_string _temp220=*(* f1->name).f2;
struct _tagged_string _temp221=*(* f0->name).f2; xprintf("%.*s %.*s: field name mismatch %.*s != %.*s",
_temp218.last_plus_one - _temp218.curr, _temp218.curr, _temp219.last_plus_one -
_temp219.curr, _temp219.curr, _temp220.last_plus_one - _temp220.curr, _temp220.curr,
_temp221.last_plus_one - _temp221.curr, _temp221.curr);})); return 0;}{ struct
_tagged_string _temp222=*(* f0->name).f2; goto _LL223; _LL223: { int _temp228;
void* _temp230; struct _tuple3 _temp226= Cyc_Tcdecl_merge_scope(( void*) f0->sc,(
void*) f1->sc,({ struct _tagged_string _temp224= t; struct _tagged_string
_temp225= v; xprintf("in %.*s %.*s, field", _temp224.last_plus_one - _temp224.curr,
_temp224.curr, _temp225.last_plus_one - _temp225.curr, _temp225.curr);}),
_temp222, loc, msg); _LL231: _temp230= _temp226.f1; goto _LL229; _LL229:
_temp228= _temp226.f2; goto _LL227; _LL227: { struct Cyc_List_List* _temp232= f0->tvs;
goto _LL233; _LL233: { struct Cyc_List_List* _temp234= f1->tvs; goto _LL235;
_LL235: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp232) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp234)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp236= t; struct _tagged_string _temp237= v;
struct _tagged_string _temp238= _temp222; xprintf("%.*s %.*s, field %.*s: type parameter number mismatch",
_temp236.last_plus_one - _temp236.curr, _temp236.curr, _temp237.last_plus_one -
_temp237.curr, _temp237.curr, _temp238.last_plus_one - _temp238.curr, _temp238.curr);}));
return 0;}{ struct Cyc_List_List* _temp239= Cyc_Tcdecl_build_tvs_map( _temp232,
_temp234); goto _LL240; _LL240: { struct Cyc_List_List* _temp241= f0->typs; goto
_LL242; _LL242: { struct Cyc_List_List* _temp243= f1->typs; goto _LL244; _LL244:
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp241) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp243)){ Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp245= t; struct _tagged_string _temp246= v;
struct _tagged_string _temp247= _temp222; xprintf("%.*s %.*s, field %.*s: parameter number mismatch",
_temp245.last_plus_one - _temp245.curr, _temp245.curr, _temp246.last_plus_one -
_temp246.curr, _temp246.curr, _temp247.last_plus_one - _temp247.curr, _temp247.curr);}));
_temp228= 0;} for( 0; _temp241 != 0; _temp241=({ struct Cyc_List_List* _temp248=
_temp241; if( _temp248 == 0){ _throw( Null_Exception);} _temp248->tl;}),
_temp243=({ struct Cyc_List_List* _temp249= _temp243; if( _temp249 == 0){ _throw(
Null_Exception);} _temp249->tl;})){ if( ! Cyc_Tcutil_equal_tqual((*(( struct
_tuple7*)({ struct Cyc_List_List* _temp250= _temp241; if( _temp250 == 0){ _throw(
Null_Exception);} _temp250->hd;}))).f1,(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp251= _temp243; if( _temp251 == 0){ _throw( Null_Exception);} _temp251->hd;}))).f1)){
Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp252= t; struct
_tagged_string _temp253= v; struct _tagged_string _temp254= _temp222; xprintf("%.*s %.*s, field %.*s: parameter qualifier",
_temp252.last_plus_one - _temp252.curr, _temp252.curr, _temp253.last_plus_one -
_temp253.curr, _temp253.curr, _temp254.last_plus_one - _temp254.curr, _temp254.curr);}));
_temp228= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp239,
inst),(*(( struct _tuple7*)({ struct Cyc_List_List* _temp262= _temp243; if(
_temp262 == 0){ _throw( Null_Exception);} _temp262->hd;}))).f2); if( ! Cyc_Tcdecl_check_type((*((
struct _tuple7*)({ struct Cyc_List_List* _temp255= _temp241; if( _temp255 == 0){
_throw( Null_Exception);} _temp255->hd;}))).f2, subst_t1)){ Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp257= t; struct _tagged_string _temp258= v;
struct _tagged_string _temp259= _temp222; struct _tagged_string _temp260= Cyc_Absynpp_typ2string((*((
struct _tuple7*)({ struct Cyc_List_List* _temp256= _temp241; if( _temp256 == 0){
_throw( Null_Exception);} _temp256->hd;}))).f2); struct _tagged_string _temp261=
Cyc_Absynpp_typ2string( subst_t1); xprintf("%.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp257.last_plus_one - _temp257.curr, _temp257.curr, _temp258.last_plus_one -
_temp258.curr, _temp258.curr, _temp259.last_plus_one - _temp259.curr, _temp259.curr,
_temp260.last_plus_one - _temp260.curr, _temp260.curr, _temp261.last_plus_one -
_temp261.curr, _temp261.curr);})); _temp228= 0;}}} if( ! _temp228){ return 0;}
if(( void*) f0->sc == _temp230){ struct Cyc_Absyn_Tunionfield* _temp264=({
struct Cyc_Absyn_Tunionfield* _temp263=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp263[ 0]=* f0; _temp263;});
goto _LL265; _LL265:( void*)({ struct Cyc_Absyn_Tunionfield* _temp266= _temp264;
if( _temp266 == 0){ _throw( Null_Exception);} _temp266->sc=( void*) _temp230;});
return _temp264;} else{ return( struct Cyc_Absyn_Tunionfield*) f0;}}}}}}}}}
static int Cyc_Tcdecl_substitute_tunionfield_f1( struct _tagged_string* name,
struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp( x->name, name) == 0;}
static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2( struct Cyc_List_List*
inst, struct _tuple7* x){ struct _tuple7 _temp269; void* _temp270; struct Cyc_Absyn_Tqual
_temp272; struct _tuple7* _temp267= x; _temp269=* _temp267; _LL273: _temp272=
_temp269.f1; goto _LL271; _LL271: _temp270= _temp269.f2; goto _LL268; _LL268:
return({ struct _tuple7* _temp274=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp274->f1= _temp272; _temp274->f2= Cyc_Tcutil_substitute( inst,
_temp270); _temp274;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp276=({
struct Cyc_Absyn_Tunionfield* _temp275=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp275[ 0]=* f1; _temp275;});
goto _LL277; _LL277: { struct Cyc_List_List* _temp278= 0; goto _LL279; _LL279:{
struct Cyc_List_List* _temp280= f1->tvs; goto _LL281; _LL281: for( 0; _temp280
!= 0; _temp280=({ struct Cyc_List_List* _temp282= _temp280; if( _temp282 == 0){
_throw( Null_Exception);} _temp282->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp286= _temp280; if( _temp286 == 0){
_throw( Null_Exception);} _temp286->hd;}); if((( int(*)( int(* pred)( struct
_tagged_string*, struct Cyc_Absyn_Tvar*), struct _tagged_string* env, struct Cyc_List_List*
x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)?
1:(( int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*),
struct _tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)){ int i= 0; struct _tagged_string base=* tv->name; struct
_tagged_string* new_name; do { new_name=({ struct _tagged_string* _temp283=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp283[ 0]=
Cyc_String_strconcat( base, Cyc_Core_string_of_int( i)); _temp283;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*), struct
_tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*),
struct _tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp284=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp284[ 0]=* tv; _temp284;}); tv->name=
new_name;} _temp278=({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp285->hd=( void*) tv; _temp285->tl=
_temp278; _temp285;});}} _temp278=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp278); _temp276->tvs= _temp278;{ struct Cyc_List_List*
_temp287= Cyc_Tcdecl_build_tvs_map( _temp278, f1->tvs); goto _LL288; _LL288: {
struct Cyc_List_List* _temp289=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp287, inst1); goto _LL290;
_LL290: _temp276->typs=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct
Cyc_List_List*, struct _tuple7*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2, _temp289, f1->typs);
return _temp276;}}}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp318=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp318[ 0]= 0; _temp318;}); struct Cyc_List_List** _temp291= f2sp; goto _LL292;
_LL292: { int cmp= - 1; for( 0; f0s != 0? f1s != 0: 0; f1s=({ struct Cyc_List_List*
_temp293= f1s; if( _temp293 == 0){ _throw( Null_Exception);} _temp293->tl;})){
while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp294= f0s; if( _temp294 == 0){ _throw( Null_Exception);}
_temp294->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp295= f1s; if( _temp295 == 0){ _throw( Null_Exception);} _temp295->hd;}))->name))
< 0: 0) { struct Cyc_List_List* _temp298=({ struct Cyc_List_List* _temp296=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp296->hd=(
void*)(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp297= f0s;
if( _temp297 == 0){ _throw( Null_Exception);} _temp297->hd;})); _temp296->tl= 0;
_temp296;}); goto _LL299; _LL299:* _temp291= _temp298; _temp291=( struct Cyc_List_List**)({
struct Cyc_List_List* _temp300= _temp298; if( _temp300 == 0){ _throw(
Null_Exception);}& _temp300->tl;}); f0s=({ struct Cyc_List_List* _temp301= f0s;
if( _temp301 == 0){ _throw( Null_Exception);} _temp301->tl;});} if( f0s == 0? 1:
cmp > 0){* incl= 0;{ struct Cyc_List_List* _temp304=({ struct Cyc_List_List*
_temp302=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp302->hd=( void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp303= f1s; if(
_temp303 == 0){ _throw( Null_Exception);} _temp303->hd;})); _temp302->tl= 0;
_temp302;}); goto _LL305; _LL305:* _temp291= _temp304; _temp291=( struct Cyc_List_List**)({
struct Cyc_List_List* _temp306= _temp304; if( _temp306 == 0){ _throw(
Null_Exception);}& _temp306->tl;});}} else{ struct Cyc_Absyn_Tunionfield*
_temp309= Cyc_Tcdecl_merge_tunionfield(( struct Cyc_Absyn_Tunionfield*)({ struct
Cyc_List_List* _temp307= f0s; if( _temp307 == 0){ _throw( Null_Exception);}
_temp307->hd;}),( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp308= f1s; if( _temp308 == 0){ _throw( Null_Exception);} _temp308->hd;}),
inst, t, v, msg); goto _LL310; _LL310: if( _temp309 != 0){ if( _temp309 !=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp311= f0s; if( _temp311 == 0){ _throw( Null_Exception);} _temp311->hd;}))){*
incl= 0;}{ struct Cyc_List_List* _temp314=({ struct Cyc_List_List* _temp312=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp312->hd=(
void*)(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_Absyn_Tunionfield* _temp313=
_temp309; if( _temp313 == 0){ _throw( Null_Exception);} _temp313;})); _temp312->tl=
0; _temp312;}); goto _LL315; _LL315:* _temp291= _temp314; _temp291=( struct Cyc_List_List**)({
struct Cyc_List_List* _temp316= _temp314; if( _temp316 == 0){ _throw(
Null_Exception);}& _temp316->tl;});}} else{* res= 0;} f0s=({ struct Cyc_List_List*
_temp317= f0s; if( _temp317 == 0){ _throw( Null_Exception);} _temp317->tl;});}}
if( f1s != 0){* incl= 0;* _temp291= f1s;} else{* _temp291= f0s;} return* f2sp;}}
static struct Cyc_List_List* Cyc_Tcdecl_sub_xtunion_fields( struct Cyc_List_List*
f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst, int* res, struct
_tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment* loc,
struct _tagged_string* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp341=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp341[ 0]= 0; _temp341;}); struct Cyc_List_List** _temp319= f2sp; goto _LL320;
_LL320: { int cmp= - 1; for( 0; f0s != 0? f1s != 0: 0; f1s=({ struct Cyc_List_List*
_temp321= f1s; if( _temp321 == 0){ _throw( Null_Exception);} _temp321->tl;})){
while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp322= f0s; if( _temp322 == 0){ _throw( Null_Exception);}
_temp322->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp323= f1s; if( _temp323 == 0){ _throw( Null_Exception);} _temp323->hd;}))->name))
< 0: 0) { f0s=({ struct Cyc_List_List* _temp324= f0s; if( _temp324 == 0){ _throw(
Null_Exception);} _temp324->tl;});} if( f0s == 0? 1: cmp > 0){ struct Cyc_List_List*
_temp327=({ struct Cyc_List_List* _temp325=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp325->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp326= f1s; if( _temp326 == 0){ _throw( Null_Exception);}
_temp326->hd;})); _temp325->tl= 0; _temp325;}); goto _LL328; _LL328:* _temp319=
_temp327; _temp319=( struct Cyc_List_List**)({ struct Cyc_List_List* _temp329=
_temp327; if( _temp329 == 0){ _throw( Null_Exception);}& _temp329->tl;});} else{
struct Cyc_Absyn_Tunionfield* _temp332= Cyc_Tcdecl_merge_tunionfield(( struct
Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp330= f0s; if( _temp330 == 0){
_throw( Null_Exception);} _temp330->hd;}),( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp331= f1s; if( _temp331 == 0){ _throw( Null_Exception);}
_temp331->hd;}), inst, t, v, msg); goto _LL333; _LL333: if( _temp332 == 0){* res=
0;} else{ if( _temp332 !=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp334= f0s; if( _temp334 == 0){ _throw( Null_Exception);}
_temp334->hd;}))){ struct Cyc_List_List* _temp337=({ struct Cyc_List_List*
_temp335=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp335->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_Absyn_Tunionfield*
_temp336= _temp332; if( _temp336 == 0){ _throw( Null_Exception);} _temp336;}));
_temp335->tl= 0; _temp335;}); goto _LL338; _LL338:* _temp319= _temp337; _temp319=(
struct Cyc_List_List**)({ struct Cyc_List_List* _temp339= _temp337; if( _temp339
== 0){ _throw( Null_Exception);}& _temp339->tl;});}} f0s=({ struct Cyc_List_List*
_temp340= f0s; if( _temp340 == 0){ _throw( Null_Exception);} _temp340->tl;});}}
if( f1s != 0){* _temp319= f1s;} return* f2sp;}} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct
Cyc_List_List* f){ if( f == 0){ return({ struct _tuple8 _temp342; _temp342.f1= 0;
_temp342.f2= 0; _temp342;});} if(({ struct Cyc_List_List* _temp343= f; if(
_temp343 == 0){ _throw( Null_Exception);} _temp343->tl;}) == 0){ return({ struct
_tuple8 _temp344; _temp344.f1= f; _temp344.f2= 0; _temp344;});}{ struct Cyc_List_List*
_temp349; struct Cyc_List_List* _temp351; struct _tuple8 _temp347=(( struct
_tuple8(*)( struct Cyc_List_List* f)) Cyc_Tcdecl_split)(({ struct Cyc_List_List*
_temp346=({ struct Cyc_List_List* _temp345= f; if( _temp345 == 0){ _throw(
Null_Exception);} _temp345->tl;}); if( _temp346 == 0){ _throw( Null_Exception);}
_temp346->tl;})); _LL352: _temp351= _temp347.f1; goto _LL350; _LL350: _temp349=
_temp347.f2; goto _LL348; _LL348: return({ struct _tuple8 _temp353; _temp353.f1=({
struct Cyc_List_List* _temp357=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp357->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp358= f; if( _temp358 == 0){ _throw( Null_Exception);} _temp358->hd;}));
_temp357->tl= _temp351; _temp357;}); _temp353.f2=({ struct Cyc_List_List*
_temp354=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp354->hd=( void*)(( void*)({ struct Cyc_List_List* _temp356=({ struct Cyc_List_List*
_temp355= f; if( _temp355 == 0){ _throw( Null_Exception);} _temp355->tl;}); if(
_temp356 == 0){ _throw( Null_Exception);} _temp356->hd;})); _temp354->tl=
_temp349; _temp354;}); _temp353;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List* _temp361; struct Cyc_List_List*
_temp363; struct _tuple8 _temp359=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL364: _temp363= _temp359.f1; goto _LL362; _LL362:
_temp361= _temp359.f2; goto _LL360; _LL360: if( _temp363 != 0?({ struct Cyc_List_List*
_temp365= _temp363; if( _temp365 == 0){ _throw( Null_Exception);} _temp365->tl;})
!= 0: 0){ _temp363= Cyc_Tcdecl_sort_xtunion_fields( _temp363, res, v, loc, msg);}
if( _temp361 != 0?({ struct Cyc_List_List* _temp366= _temp361; if( _temp366 == 0){
_throw( Null_Exception);} _temp366->tl;}) != 0: 0){ _temp361= Cyc_Tcdecl_sort_xtunion_fields(
_temp361, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp363,
_temp361, 0, 0, 0, res,({ int* _temp367=( int*) GC_malloc_atomic( sizeof( int));
_temp367[ 0]= 1; _temp367;}),( struct _tagged_string)({ char* _temp368=( char*)"xtunion";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 8; _temp369;}),* v, loc, msg);} struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tagged_string _temp370= Cyc_Absynpp_qvar2string( d0->name); goto _LL371; _LL371: {
struct _tagged_string t=( struct _tagged_string)({ char* _temp431=( char*)"[x]tunion";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 10; _temp432;}); int _temp372= 1; goto _LL373;
_LL373: if( d0->is_xtunion != d1->is_xtunion){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp374= _temp370; struct _tagged_string _temp375= Cyc_Tcdecl_is_x2string(
d0->is_xtunion); struct _tagged_string _temp376= Cyc_Tcdecl_is_x2string( d1->is_xtunion);
xprintf("expected %.*s to be a %.*s instead of a %.*s", _temp374.last_plus_one -
_temp374.curr, _temp374.curr, _temp375.last_plus_one - _temp375.curr, _temp375.curr,
_temp376.last_plus_one - _temp376.curr, _temp376.curr);})); _temp372= 0;} else{
t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs( d0->tvs,
d1->tvs, t, _temp370, loc, msg)){ return 0;}{ int _temp379; void* _temp381;
struct _tuple3 _temp377= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, _temp370, loc, msg); _LL382: _temp381= _temp377.f1; goto _LL380; _LL380:
_temp379= _temp377.f2; goto _LL378; _LL378: if( ! _temp379){ _temp372= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp384=({ struct _tuple6
_temp383; _temp383.f1= d0->fields; _temp383.f2= d1->fields; _temp383;}); struct
Cyc_Core_Opt* _temp392; struct Cyc_Core_Opt* _temp394; struct Cyc_Core_Opt*
_temp396; struct Cyc_Core_Opt* _temp398; struct Cyc_Core_Opt* _temp400; struct
Cyc_Core_Opt _temp402; struct Cyc_List_List* _temp403; struct Cyc_Core_Opt*
_temp405; struct Cyc_Core_Opt _temp407; struct Cyc_List_List* _temp408; _LL386:
_LL395: _temp394= _temp384.f1; goto _LL393; _LL393: _temp392= _temp384.f2; if(
_temp392 == 0){ goto _LL387;} else{ goto _LL388;} _LL388: _LL399: _temp398=
_temp384.f1; if( _temp398 == 0){ goto _LL397;} else{ goto _LL390;} _LL397:
_temp396= _temp384.f2; goto _LL389; _LL390: _LL406: _temp405= _temp384.f1; if(
_temp405 == 0){ goto _LL385;} else{ _temp407=* _temp405; _LL409: _temp408=(
struct Cyc_List_List*) _temp407.v; goto _LL401;} _LL401: _temp400= _temp384.f2;
if( _temp400 == 0){ goto _LL385;} else{ _temp402=* _temp400; _LL404: _temp403=(
struct Cyc_List_List*) _temp402.v; goto _LL391;} _LL387: d2= d0; goto _LL385;
_LL389: d2= d1; goto _LL385; _LL391: { struct Cyc_List_List* _temp410= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); goto _LL411; _LL411: if( d0->is_xtunion){ int _temp412= 1;
goto _LL413; _LL413: { struct Cyc_List_List* _temp414= Cyc_Tcdecl_merge_xtunion_fields(
_temp408, _temp403, _temp410, d0->tvs, d1->tvs,& _temp372,& _temp412, t,
_temp370, loc, msg); goto _LL415; _LL415: if( _temp412){ d2= d0;} else{ d2=({
struct Cyc_Absyn_Tuniondecl* _temp416=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp416[ 0]=* d0; _temp416;});( void*)(
d2->sc=( void*) _temp381); d2->fields=({ struct Cyc_Core_Opt* _temp417=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp417->v=( void*)
_temp414; _temp417;});}}} else{ for( 0; _temp408 != 0? _temp403 != 0: 0;
_temp408=({ struct Cyc_List_List* _temp418= _temp408; if( _temp418 == 0){ _throw(
Null_Exception);} _temp418->tl;}), _temp403=({ struct Cyc_List_List* _temp419=
_temp403; if( _temp419 == 0){ _throw( Null_Exception);} _temp419->tl;})){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp420= _temp408; if(
_temp420 == 0){ _throw( Null_Exception);} _temp420->hd;}),( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp421= _temp403; if( _temp421 == 0){ _throw(
Null_Exception);} _temp421->hd;}), _temp410, t, _temp370, msg);} if( _temp408 !=
0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp423= t; struct
_tagged_string _temp424= _temp370; struct _tagged_string _temp425=*(*(( struct
Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp422= _temp403; if( _temp422
== 0){ _throw( Null_Exception);} _temp422->hd;}))->name).f2; xprintf("%.*s %.*s has extra field %.*s",
_temp423.last_plus_one - _temp423.curr, _temp423.curr, _temp424.last_plus_one -
_temp424.curr, _temp424.curr, _temp425.last_plus_one - _temp425.curr, _temp425.curr);}));
_temp372= 0;} if( _temp403 != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp427= t; struct _tagged_string _temp428= _temp370; struct
_tagged_string _temp429=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp426= _temp403; if( _temp426 == 0){ _throw( Null_Exception);} _temp426->hd;}))->name).f2;
xprintf("%.*s %.*s is missing field %.*s", _temp427.last_plus_one - _temp427.curr,
_temp427.curr, _temp428.last_plus_one - _temp428.curr, _temp428.curr, _temp429.last_plus_one
- _temp429.curr, _temp429.curr);})); _temp372= 0;} d2= d0;} goto _LL385;} _LL385:;}
if( ! _temp372){ return 0;} if( _temp381 ==( void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp430=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp430[ 0]=* d2; _temp430;});(
void*)( d2->sc=( void*) _temp381); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}}}
struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string _temp433= Cyc_Absynpp_qvar2string(
d0->name); goto _LL434; _LL434: { int _temp435= 1; goto _LL436; _LL436: { int
_temp441; void* _temp443; struct _tuple3 _temp439= Cyc_Tcdecl_merge_scope(( void*)
d0->sc,( void*) d1->sc,( struct _tagged_string)({ char* _temp437=( char*)"enum";
struct _tagged_string _temp438; _temp438.curr= _temp437; _temp438.base= _temp437;
_temp438.last_plus_one= _temp437 + 5; _temp438;}), _temp433, loc, msg); _LL444:
_temp443= _temp439.f1; goto _LL442; _LL442: _temp441= _temp439.f2; goto _LL440;
_LL440: if( ! _temp441){ _temp435= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct
_tuple6 _temp446=({ struct _tuple6 _temp445; _temp445.f1= d0->fields; _temp445.f2=
d1->fields; _temp445;}); struct Cyc_Core_Opt* _temp454; struct Cyc_Core_Opt*
_temp456; struct Cyc_Core_Opt* _temp458; struct Cyc_Core_Opt* _temp460; struct
Cyc_Core_Opt* _temp462; struct Cyc_Core_Opt _temp464; struct Cyc_List_List*
_temp465; struct Cyc_Core_Opt* _temp467; struct Cyc_Core_Opt _temp469; struct
Cyc_List_List* _temp470; _LL448: _LL457: _temp456= _temp446.f1; goto _LL455;
_LL455: _temp454= _temp446.f2; if( _temp454 == 0){ goto _LL449;} else{ goto
_LL450;} _LL450: _LL461: _temp460= _temp446.f1; if( _temp460 == 0){ goto _LL459;}
else{ goto _LL452;} _LL459: _temp458= _temp446.f2; goto _LL451; _LL452: _LL468:
_temp467= _temp446.f1; if( _temp467 == 0){ goto _LL447;} else{ _temp469=*
_temp467; _LL471: _temp470=( struct Cyc_List_List*) _temp469.v; goto _LL463;}
_LL463: _temp462= _temp446.f2; if( _temp462 == 0){ goto _LL447;} else{ _temp464=*
_temp462; _LL466: _temp465=( struct Cyc_List_List*) _temp464.v; goto _LL453;}
_LL449: d2= d0; goto _LL447; _LL451: d2= d1; goto _LL447; _LL453: for( 0;
_temp470 != 0? _temp465 != 0: 0; _temp470=({ struct Cyc_List_List* _temp472=
_temp470; if( _temp472 == 0){ _throw( Null_Exception);} _temp472->tl;}),
_temp465=({ struct Cyc_List_List* _temp473= _temp465; if( _temp473 == 0){ _throw(
Null_Exception);} _temp473->tl;})){ struct Cyc_Absyn_Enumfield _temp477; struct
Cyc_Position_Segment* _temp478; struct Cyc_Absyn_Exp* _temp480; struct _tuple0*
_temp482; struct Cyc_Absyn_Enumfield* _temp475=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp474= _temp470; if( _temp474 == 0){ _throw(
Null_Exception);} _temp474->hd;}); _temp477=* _temp475; _LL483: _temp482=(
struct _tuple0*) _temp477.name; goto _LL481; _LL481: _temp480=( struct Cyc_Absyn_Exp*)
_temp477.tag; goto _LL479; _LL479: _temp478=( struct Cyc_Position_Segment*)
_temp477.loc; goto _LL476; _LL476: { struct Cyc_Absyn_Enumfield _temp487; struct
Cyc_Position_Segment* _temp488; struct Cyc_Absyn_Exp* _temp490; struct _tuple0*
_temp492; struct Cyc_Absyn_Enumfield* _temp485=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp484= _temp465; if( _temp484 == 0){ _throw(
Null_Exception);} _temp484->hd;}); _temp487=* _temp485; _LL493: _temp492=(
struct _tuple0*) _temp487.name; goto _LL491; _LL491: _temp490=( struct Cyc_Absyn_Exp*)
_temp487.tag; goto _LL489; _LL489: _temp488=( struct Cyc_Position_Segment*)
_temp487.loc; goto _LL486; _LL486: if( Cyc_String_zstrptrcmp((* _temp492).f2,(*
_temp482).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp494=
_temp433; struct _tagged_string _temp495=*(* _temp492).f2; struct _tagged_string
_temp496=*(* _temp482).f2; xprintf("enum %.*s: field name mismatch %.*s != %.*s",
_temp494.last_plus_one - _temp494.curr, _temp494.curr, _temp495.last_plus_one -
_temp495.curr, _temp495.curr, _temp496.last_plus_one - _temp496.curr, _temp496.curr);}));
_temp435= 0;} if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp497= _temp480; if( _temp497 == 0){ _throw(
Null_Exception);} _temp497;})) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp498= _temp490; if( _temp498 == 0){ _throw(
Null_Exception);} _temp498;}))){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp499= _temp433; struct _tagged_string _temp500=*(* _temp492).f2;
xprintf("enum %.*s, field %.*s, value mismatch", _temp499.last_plus_one -
_temp499.curr, _temp499.curr, _temp500.last_plus_one - _temp500.curr, _temp500.curr);}));
_temp435= 0;}}} d2= d0; goto _LL447; _LL447:;} if( ! _temp435){ return 0;} if((
void*) d2->sc == _temp443){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({
struct Cyc_Absyn_Enumdecl* _temp501=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp501[ 0]=* d2; _temp501;});( void*)( d2->sc=(
void*) _temp443); return( struct Cyc_Absyn_Enumdecl*) d2;}}}}} inline static
struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl( void* sc0, void* t0, struct Cyc_Absyn_Tqual
tq0, struct Cyc_List_List* atts0, void* sc1, void* t1, struct Cyc_Absyn_Tqual
tq1, struct Cyc_List_List* atts1, struct _tagged_string t, struct _tagged_string
v, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ int _temp502=
1; goto _LL503; _LL503: { int _temp506; void* _temp508; struct _tuple3 _temp504=
Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL509: _temp508= _temp504.f1;
goto _LL507; _LL507: _temp506= _temp504.f2; goto _LL505; _LL505: if( ! _temp506){
_temp502= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp510= t; struct _tagged_string _temp511= v; struct
_tagged_string _temp512= Cyc_Absynpp_typ2string( t1); struct _tagged_string
_temp513= Cyc_Absynpp_typ2string( t0); xprintf("%.*s %.*s has type %.*s instead of %.*s",
_temp510.last_plus_one - _temp510.curr, _temp510.curr, _temp511.last_plus_one -
_temp511.curr, _temp511.curr, _temp512.last_plus_one - _temp512.curr, _temp512.curr,
_temp513.last_plus_one - _temp513.curr, _temp513.curr);})); _temp502= 0;} if( !
Cyc_Tcutil_equal_tqual( tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp514= t; struct _tagged_string _temp515= v; xprintf("%.*s %.*s has different type qualifiers",
_temp514.last_plus_one - _temp514.curr, _temp514.curr, _temp515.last_plus_one -
_temp515.curr, _temp515.curr);})); _temp502= 0;} if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp516= t;
struct _tagged_string _temp517= v; xprintf("%.*s %.*s has different attributes",
_temp516.last_plus_one - _temp516.curr, _temp516.curr, _temp517.last_plus_one -
_temp517.curr, _temp517.curr);})); _temp502= 0;} return({ struct _tuple3
_temp518; _temp518.f1= _temp508; _temp518.f2= _temp502; _temp518;});}} struct
Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl( struct Cyc_Absyn_Vardecl* d0,
struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string _temp519= Cyc_Absynpp_qvar2string(
d0->name); goto _LL520; _LL520: { int _temp525; void* _temp527; struct _tuple3
_temp523= Cyc_Tcdecl_check_var_or_fn_decl(( void*) d0->sc,( void*) d0->type, d0->tq,
d0->attributes,( void*) d1->sc,( void*) d1->type, d1->tq, d1->attributes,(
struct _tagged_string)({ char* _temp521=( char*)"variable"; struct
_tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 9; _temp522;}), _temp519, loc, msg); _LL528:
_temp527= _temp523.f1; goto _LL526; _LL526: _temp525= _temp523.f2; goto _LL524;
_LL524: if( ! _temp525){ return 0;} if(( void*) d0->sc == _temp527){ return(
struct Cyc_Absyn_Vardecl*) d0;} else{ struct Cyc_Absyn_Vardecl* _temp530=({
struct Cyc_Absyn_Vardecl* _temp529=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp529[ 0]=* d0; _temp529;}); goto _LL531;
_LL531:( void*)({ struct Cyc_Absyn_Vardecl* _temp532= _temp530; if( _temp532 ==
0){ _throw( Null_Exception);} _temp532->sc=( void*) _temp527;}); return _temp530;}}}
struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl( struct Cyc_Absyn_Typedefdecl*
d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string _temp533= Cyc_Absynpp_qvar2string(
d0->name); goto _LL534; _LL534: if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs,(
struct _tagged_string)({ char* _temp535=( char*)"typedef"; struct _tagged_string
_temp536; _temp536.curr= _temp535; _temp536.base= _temp535; _temp536.last_plus_one=
_temp535 + 8; _temp536;}), _temp533, loc, msg)){ return 0;}{ struct Cyc_List_List*
_temp537= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); goto _LL538; _LL538: {
void* subst_defn1= Cyc_Tcutil_substitute( _temp537,( void*) d1->defn); if( ! Cyc_Tcdecl_check_type((
void*) d0->defn, subst_defn1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp539= _temp533; struct _tagged_string _temp540= Cyc_Absynpp_typ2string(
subst_defn1); struct _tagged_string _temp541= Cyc_Absynpp_typ2string(( void*) d0->defn);
xprintf("typedef %.*s does refer to the same type: %.*s != %.*s", _temp539.last_plus_one
- _temp539.curr, _temp539.curr, _temp540.last_plus_one - _temp540.curr, _temp540.curr,
_temp541.last_plus_one - _temp541.curr, _temp541.curr);})); return 0;} return(
struct Cyc_Absyn_Typedefdecl*) d0;}}} void* Cyc_Tcdecl_merge_binding( void* b0,
void* b1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tuple4 _temp543=({ struct _tuple4 _temp542; _temp542.f1= b0; _temp542.f2= b1;
_temp542;}); void* _temp557; void* _temp559; void* _temp561; struct Cyc_Absyn_Vardecl*
_temp563; void* _temp565; struct Cyc_Absyn_Vardecl* _temp567; void* _temp569;
struct Cyc_Absyn_Fndecl* _temp571; void* _temp573; struct Cyc_Absyn_Vardecl*
_temp575; void* _temp577; struct Cyc_Absyn_Fndecl* _temp579; void* _temp581;
struct Cyc_Absyn_Fndecl* _temp583; void* _temp585; struct Cyc_Absyn_Vardecl*
_temp587; void* _temp589; struct Cyc_Absyn_Fndecl* _temp591; _LL545: _LL560:
_temp559= _temp543.f1; if( _temp559 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL558;} else{ goto _LL547;} _LL558: _temp557= _temp543.f2; if( _temp557 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL546;} else{ goto _LL547;} _LL547: _LL566:
_temp565= _temp543.f1; if(( unsigned int) _temp565 > 1u?*(( int*) _temp565) ==
Cyc_Absyn_Global_b: 0){ _LL568: _temp567=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Global_b_struct*) _temp565)->f1; goto _LL562;} else{ goto _LL549;}
_LL562: _temp561= _temp543.f2; if(( unsigned int) _temp561 > 1u?*(( int*)
_temp561) == Cyc_Absyn_Global_b: 0){ _LL564: _temp563=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp561)->f1; goto _LL548;} else{ goto
_LL549;} _LL549: _LL574: _temp573= _temp543.f1; if(( unsigned int) _temp573 > 1u?*((
int*) _temp573) == Cyc_Absyn_Global_b: 0){ _LL576: _temp575=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp573)->f1; goto _LL570;} else{ goto
_LL551;} _LL570: _temp569= _temp543.f2; if(( unsigned int) _temp569 > 1u?*(( int*)
_temp569) == Cyc_Absyn_Funname_b: 0){ _LL572: _temp571=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp569)->f1; goto _LL550;} else{ goto
_LL551;} _LL551: _LL582: _temp581= _temp543.f1; if(( unsigned int) _temp581 > 1u?*((
int*) _temp581) == Cyc_Absyn_Funname_b: 0){ _LL584: _temp583=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp581)->f1; goto _LL578;} else{ goto
_LL553;} _LL578: _temp577= _temp543.f2; if(( unsigned int) _temp577 > 1u?*(( int*)
_temp577) == Cyc_Absyn_Funname_b: 0){ _LL580: _temp579=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp577)->f1; goto _LL552;} else{ goto
_LL553;} _LL553: _LL590: _temp589= _temp543.f1; if(( unsigned int) _temp589 > 1u?*((
int*) _temp589) == Cyc_Absyn_Funname_b: 0){ _LL592: _temp591=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp589)->f1; goto _LL586;} else{ goto
_LL555;} _LL586: _temp585= _temp543.f2; if(( unsigned int) _temp585 > 1u?*(( int*)
_temp585) == Cyc_Absyn_Global_b: 0){ _LL588: _temp587=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp585)->f1; goto _LL554;} else{ goto
_LL555;} _LL555: goto _LL556; _LL546: return( void*) Cyc_Absyn_Unresolved_b;
_LL548: { struct Cyc_Absyn_Vardecl* _temp593= Cyc_Tcdecl_merge_vardecl( _temp567,
_temp563, loc, msg); goto _LL594; _LL594: if( _temp593 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp567 == _temp593){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp563 == _temp593){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp595=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp595[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp596; _temp596.tag= Cyc_Absyn_Global_b; _temp596.f1=(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp597= _temp593; if(
_temp597 == 0){ _throw( Null_Exception);} _temp597;}); _temp596;}); _temp595;});}}}
_LL550: { int _temp603; void* _temp605; struct _tuple3 _temp601= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp575->sc,( void*) _temp575->type, _temp575->tq, _temp575->attributes,(
void*) _temp571->sc,( void*)({ struct Cyc_Core_Opt* _temp598= _temp571->cached_typ;
if( _temp598 == 0){ _throw( Null_Exception);} _temp598->v;}), Cyc_Absyn_empty_tqual(),
_temp571->attributes,( struct _tagged_string)({ char* _temp599=( char*)"function";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 9; _temp600;}), Cyc_Absynpp_qvar2string(
_temp575->name), loc, msg); _LL606: _temp605= _temp601.f1; goto _LL604; _LL604:
_temp603= _temp601.f2; goto _LL602; _LL602: if( ! _temp603){ return( void*) Cyc_Absyn_Unresolved_b;}
return b1;} _LL552: Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp607=
Cyc_Absynpp_qvar2string( _temp579->name); xprintf("redefinition of function %.*s",
_temp607.last_plus_one - _temp607.curr, _temp607.curr);})); return( void*) Cyc_Absyn_Unresolved_b;
_LL554: { int _temp613; void* _temp615; struct _tuple3 _temp611= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp591->sc,( void*)({ struct Cyc_Core_Opt* _temp608= _temp591->cached_typ;
if( _temp608 == 0){ _throw( Null_Exception);} _temp608->v;}), Cyc_Absyn_empty_tqual(),
_temp591->attributes,( void*) _temp587->sc,( void*) _temp587->type, _temp587->tq,
_temp587->attributes,( struct _tagged_string)({ char* _temp609=( char*)"variable";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 9; _temp610;}), Cyc_Absynpp_qvar2string(
_temp591->name), loc, msg); _LL616: _temp615= _temp611.f1; goto _LL614; _LL614:
_temp613= _temp611.f2; goto _LL612; _LL612: if( ! _temp613){ return( void*) Cyc_Absyn_Unresolved_b;}
return b0;} _LL556:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp617=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp617[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp618; _temp618.tag= Cyc_Core_InvalidArg;
_temp618.f1=( struct _tagged_string)({ char* _temp619=( char*)"Tcdecl::merge_binding";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 22; _temp620;}); _temp618;}); _temp617;}));
_LL544:;} struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_sub_structdecl( struct Cyc_Absyn_Structdecl*
ye, struct Cyc_Absyn_Structdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Structdecl* _temp621= Cyc_Tcdecl_merge_structdecl(
ye, y, loc, msg); goto _LL622; _LL622: if( _temp621 !=( struct Cyc_Absyn_Structdecl*)
ye){ if( _temp621 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp624= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp623= y->name; if( _temp623 == 0){ _throw( Null_Exception);} _temp623->v;}));
xprintf("abstract struct %.*s is being used as non-abstract", _temp624.last_plus_one
- _temp624.curr, _temp624.curr);}));} return 0;} struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_sub_uniondecl(
struct Cyc_Absyn_Uniondecl* ye, struct Cyc_Absyn_Uniondecl* y, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg, int* res){ struct Cyc_Absyn_Uniondecl* _temp625=
Cyc_Tcdecl_merge_uniondecl( ye, y, loc, msg); goto _LL626; _LL626: if( _temp625
!=( struct Cyc_Absyn_Uniondecl*) ye){ if( _temp625 == 0){ return 0;}* res= 0;
Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp628= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp627= y->name; if( _temp627 == 0){
_throw( Null_Exception);} _temp627->v;})); xprintf("abstract union %.*s is being used as non-abstract",
_temp628.last_plus_one - _temp628.curr, _temp628.curr);}));} return 0;} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_sub_enumdecl( struct Cyc_Absyn_Enumdecl* ye,
struct Cyc_Absyn_Enumdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Enumdecl* _temp629= Cyc_Tcdecl_merge_enumdecl(
ye, y, loc, msg); goto _LL630; _LL630: if( _temp629 !=( struct Cyc_Absyn_Enumdecl*)
ye){ if( _temp629 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp631= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract enum %.*s is being used as non-abstract",
_temp631.last_plus_one - _temp631.curr, _temp631.curr);}));} return 0;} struct
Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_sub_typedefdecl( struct Cyc_Absyn_Typedefdecl*
ye, struct Cyc_Absyn_Typedefdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Typedefdecl* _temp632= Cyc_Tcdecl_merge_typedefdecl(
ye, y, loc, msg); goto _LL633; _LL633: if( _temp632 !=( struct Cyc_Absyn_Typedefdecl*)
ye){ if( _temp632 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp634= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract typedef %.*s is being used as non-abstract",
_temp634.last_plus_one - _temp634.curr, _temp634.curr);}));} return 0;} struct
Cyc_Absyn_Vardecl* Cyc_Tcdecl_sub_vardecl( struct Cyc_Absyn_Vardecl* ye, struct
Cyc_Absyn_Vardecl* y, struct Cyc_Position_Segment* loc, struct _tagged_string*
msg, int* res){ struct Cyc_Absyn_Vardecl* _temp635= Cyc_Tcdecl_merge_vardecl( ye,
y, loc, msg); goto _LL636; _LL636: if( _temp635 !=( struct Cyc_Absyn_Vardecl*)
ye){ if( _temp635 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp637= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract var %.*s is being used as non-abstract",
_temp637.last_plus_one - _temp637.curr, _temp637.curr);}));} return 0;} void Cyc_Tcdecl_f(){;}
struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_sub_tuniondecl( struct Cyc_Absyn_Tuniondecl*
ye, struct Cyc_Absyn_Tuniondecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Tuniondecl* _temp638= Cyc_Tcdecl_merge_tuniondecl(
ye, y, loc, msg); goto _LL639; _LL639: if( _temp638 !=( struct Cyc_Absyn_Tuniondecl*)
ye){ if( _temp638 == 0){ return 0;} if( y->is_xtunion? ye->fields != 0: 0){
_temp638=({ struct Cyc_Absyn_Tuniondecl* _temp640=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp640[ 0]=* y; _temp640;});({
struct Cyc_Absyn_Tuniondecl* _temp641= _temp638; if( _temp641 == 0){ _throw(
Null_Exception);} _temp641->fields=({ struct Cyc_Core_Opt* _temp642=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp642->v=( void*) Cyc_Tcdecl_sub_xtunion_fields((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp643= ye->fields; if( _temp643
== 0){ _throw( Null_Exception);} _temp643->v;}),( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp644= y->fields; if( _temp644 == 0){ _throw(
Null_Exception);} _temp644->v;}), Cyc_Tcdecl_build_tvs_map( ye->tvs, y->tvs),
res,( struct _tagged_string)({ char* _temp645=( char*)"xtunion"; struct
_tagged_string _temp646; _temp646.curr= _temp645; _temp646.base= _temp645;
_temp646.last_plus_one= _temp645 + 8; _temp646;}), Cyc_Absynpp_qvar2string( y->name),
loc, msg); _temp642;});}); return _temp638;} else{* res= 0; Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp647= Cyc_Absynpp_qvar2string( y->name);
xprintf("abstract [x]tunion %.*s is being used as non-abstract", _temp647.last_plus_one
- _temp647.curr, _temp647.curr);}));}} return 0;}