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
struct _tagged_string _temp0=*(( struct _tagged_string*) _check_null( msg1));
struct _tagged_string _temp1= msg2; xprintf("%.*s %.*s", _temp0.last_plus_one -
_temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr, _temp1.curr);}));}}
static void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct
_tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment* loc,
struct _tagged_string* msg){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp2= t; struct _tagged_string _temp3= v; struct _tagged_string _temp4= Cyc_Absynpp_scope2string(
s1); struct _tagged_string _temp5= Cyc_Absynpp_scope2string( s0); xprintf("%.*s %.*s is %.*s whereas expected scope is %.*s",
_temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one - _temp3.curr,
_temp3.curr, _temp4.last_plus_one - _temp4.curr, _temp4.curr, _temp5.last_plus_one
- _temp5.curr, _temp5.curr);}));} struct _tuple4{ void* f1; void* f2; } ; struct
_tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1, struct _tagged_string t,
struct _tagged_string v, struct Cyc_Position_Segment* loc, struct _tagged_string*
msg){{ struct _tuple4 _temp7=({ struct _tuple4 _temp6; _temp6.f1= s0; _temp6.f2=
s1; _temp6;}); void* _temp27; void* _temp29; void* _temp31; void* _temp33; void*
_temp35; void* _temp37; void* _temp39; void* _temp41; void* _temp43; void*
_temp45; void* _temp47; void* _temp49; void* _temp51; void* _temp53; void*
_temp55; void* _temp57; _LL9: _LL30: _temp29= _temp7.f1; if( _temp29 ==( void*)
Cyc_Absyn_ExternC){ goto _LL28;} else{ goto _LL11;} _LL28: _temp27= _temp7.f2;
if( _temp27 ==( void*) Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11:
_LL34: _temp33= _temp7.f1; if( _temp33 ==( void*) Cyc_Absyn_ExternC){ goto _LL32;}
else{ goto _LL13;} _LL32: _temp31= _temp7.f2; goto _LL12; _LL13: _LL38: _temp37=
_temp7.f1; goto _LL36; _LL36: _temp35= _temp7.f2; if( _temp35 ==( void*) Cyc_Absyn_ExternC){
goto _LL14;} else{ goto _LL15;} _LL15: _LL42: _temp41= _temp7.f1; goto _LL40;
_LL40: _temp39= _temp7.f2; if( _temp39 ==( void*) Cyc_Absyn_Extern){ goto _LL16;}
else{ goto _LL17;} _LL17: _LL46: _temp45= _temp7.f1; if( _temp45 ==( void*) Cyc_Absyn_Extern){
goto _LL44;} else{ goto _LL19;} _LL44: _temp43= _temp7.f2; goto _LL18; _LL19:
_LL50: _temp49= _temp7.f1; if( _temp49 ==( void*) Cyc_Absyn_Static){ goto _LL48;}
else{ goto _LL21;} _LL48: _temp47= _temp7.f2; if( _temp47 ==( void*) Cyc_Absyn_Static){
goto _LL20;} else{ goto _LL21;} _LL21: _LL54: _temp53= _temp7.f1; if( _temp53 ==(
void*) Cyc_Absyn_Public){ goto _LL52;} else{ goto _LL23;} _LL52: _temp51= _temp7.f2;
if( _temp51 ==( void*) Cyc_Absyn_Public){ goto _LL22;} else{ goto _LL23;} _LL23:
_LL58: _temp57= _temp7.f1; if( _temp57 ==( void*) Cyc_Absyn_Abstract){ goto
_LL56;} else{ goto _LL25;} _LL56: _temp55= _temp7.f2; if( _temp55 ==( void*) Cyc_Absyn_Abstract){
goto _LL24;} else{ goto _LL25;} _LL25: goto _LL26; _LL10: goto _LL8; _LL12: goto
_LL14; _LL14: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp59; _temp59.f1= s1; _temp59.f2= 0; _temp59;}); _LL16: s1= s0;
goto _LL8; _LL18: goto _LL8; _LL20: goto _LL8; _LL22: goto _LL8; _LL24: goto
_LL8; _LL26: Cyc_Tcdecl_merge_scope_err( s0, s1, t, v, loc, msg); return({
struct _tuple3 _temp60; _temp60.f1= s1; _temp60.f2= 0; _temp60;}); _LL8:;}
return({ struct _tuple3 _temp61; _temp61.f1= s1; _temp61.f2= 1; _temp61;});}
static int Cyc_Tcdecl_check_type( void* t0, void* t1){ return Cyc_Tcutil_unify(
t0, t1);} static unsigned int Cyc_Tcdecl_get_uint_const_value( struct Cyc_Absyn_Exp*
e){ void* _temp62=( void*) e->r; void* _temp68; int _temp70; void* _temp72;
_LL64: if(*(( int*) _temp62) == Cyc_Absyn_Const_e){ _LL69: _temp68=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp62)->f1; if(( unsigned int) _temp68 > 1u?*((
int*) _temp68) == Cyc_Absyn_Int_c: 0){ _LL73: _temp72=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp68)->f1; goto _LL71; _LL71: _temp70=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp68)->f2; goto _LL65;} else{ goto _LL66;}} else{ goto _LL66;} _LL66: goto
_LL67; _LL65: return( unsigned int) _temp70; _LL67:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp74=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp74[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp75; _temp75.tag= Cyc_Core_InvalidArg; _temp75.f1=(
struct _tagged_string)({ char* _temp76=( char*)"Tcdecl::get_uint_const_value";
struct _tagged_string _temp77; _temp77.curr= _temp76; _temp77.base= _temp76;
_temp77.last_plus_one= _temp76 + 29; _temp77;}); _temp75;}); _temp74;})); _LL63:;}
inline static int Cyc_Tcdecl_check_tvs( struct Cyc_List_List* tvs0, struct Cyc_List_List*
tvs1, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs0) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp78= t; struct _tagged_string _temp79= v;
xprintf("%.*s %.*s has a different number of type parameters", _temp78.last_plus_one
- _temp78.curr, _temp78.curr, _temp79.last_plus_one - _temp79.curr, _temp79.curr);}));
return 0;}{ struct Cyc_List_List* _temp80= tvs0; goto _LL81; _LL81: { struct Cyc_List_List*
_temp82= tvs1; goto _LL83; _LL83: for( 0; _temp80 != 0; _temp80=(( struct Cyc_List_List*)
_check_null( _temp80))->tl, _temp82=(( struct Cyc_List_List*) _check_null(
_temp82))->tl){ struct Cyc_Absyn_Conref* _temp84=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp80))->hd)->kind); goto _LL85; _LL85: {
struct Cyc_Absyn_Conref* _temp86=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp82))->hd)->kind); goto _LL87; _LL87: if((( void*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp84) !=(( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp86)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp88= t; struct _tagged_string _temp89= v; struct
_tagged_string _temp90= Cyc_Absynpp_ckind2string( _temp84); struct
_tagged_string _temp91=*(( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp80))->hd)->name; struct _tagged_string _temp92= Cyc_Absynpp_ckind2string(
_temp86); xprintf("%.*s %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp88.last_plus_one - _temp88.curr, _temp88.curr, _temp89.last_plus_one -
_temp89.curr, _temp89.curr, _temp90.last_plus_one - _temp90.curr, _temp90.curr,
_temp91.last_plus_one - _temp91.curr, _temp91.curr, _temp92.last_plus_one -
_temp92.curr, _temp92.curr);})); return 0;}}} return 1;}}} inline static int Cyc_Tcdecl_check_atts(
struct Cyc_List_List* atts0, struct Cyc_List_List* atts1, struct _tagged_string
t, struct _tagged_string v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp93= t; struct _tagged_string _temp94= v;
xprintf("%.*s %.*s has different attributes", _temp93.last_plus_one - _temp93.curr,
_temp93.curr, _temp94.last_plus_one - _temp94.curr, _temp94.curr);})); return 0;}
else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* _temp95= 0; goto _LL96;
_LL96: for( 0; tvs0 != 0; tvs0=(( struct Cyc_List_List*) _check_null( tvs0))->tl,
tvs1=(( struct Cyc_List_List*) _check_null( tvs1))->tl){ _temp95=({ struct Cyc_List_List*
_temp97=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp97->hd=( void*)({ struct _tuple5* _temp98=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp98->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs1))->hd; _temp98->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp99=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp99[ 0]=({ struct Cyc_Absyn_VarType_struct _temp100; _temp100.tag= Cyc_Absyn_VarType;
_temp100.f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs0))->hd;
_temp100;}); _temp99;}); _temp98;}); _temp97->tl= _temp95; _temp97;});} return
_temp95;} struct _tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_string t, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){
struct _tagged_string _temp101= Cyc_Absynpp_qvar2string(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d0->name))->v); goto _LL102; _LL102: { int
_temp103= 1; goto _LL104; _LL104: if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs,
t, _temp101, loc, msg)){ return 0;}{ int _temp107; void* _temp109; struct
_tuple3 _temp105= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc, t,
_temp101, loc, msg); _LL110: _temp109= _temp105.f1; goto _LL108; _LL108:
_temp107= _temp105.f2; goto _LL106; _LL106: if( ! _temp107){ _temp103= 0;} if( !
Cyc_Tcdecl_check_atts( d0->attributes, d1->attributes, t, _temp101, loc, msg)){
_temp103= 0;}{ struct Cyc_Absyn_Structdecl* d2;{ struct _tuple6 _temp112=({
struct _tuple6 _temp111; _temp111.f1= d0->fields; _temp111.f2= d1->fields;
_temp111;}); struct Cyc_Core_Opt* _temp120; struct Cyc_Core_Opt* _temp122;
struct Cyc_Core_Opt* _temp124; struct Cyc_Core_Opt* _temp126; struct Cyc_Core_Opt*
_temp128; struct Cyc_Core_Opt _temp130; struct Cyc_List_List* _temp131; struct
Cyc_Core_Opt* _temp133; struct Cyc_Core_Opt _temp135; struct Cyc_List_List*
_temp136; _LL114: _LL123: _temp122= _temp112.f1; goto _LL121; _LL121: _temp120=
_temp112.f2; if( _temp120 == 0){ goto _LL115;} else{ goto _LL116;} _LL116:
_LL127: _temp126= _temp112.f1; if( _temp126 == 0){ goto _LL125;} else{ goto
_LL118;} _LL125: _temp124= _temp112.f2; goto _LL117; _LL118: _LL134: _temp133=
_temp112.f1; if( _temp133 == 0){ goto _LL113;} else{ _temp135=* _temp133; _LL137:
_temp136=( struct Cyc_List_List*) _temp135.v; goto _LL129;} _LL129: _temp128=
_temp112.f2; if( _temp128 == 0){ goto _LL113;} else{ _temp130=* _temp128; _LL132:
_temp131=( struct Cyc_List_List*) _temp130.v; goto _LL119;} _LL115: d2= d0; goto
_LL113; _LL117: d2= d1; goto _LL113; _LL119: { struct Cyc_List_List* _temp138=
Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); goto _LL139; _LL139: for( 0;
_temp136 != 0? _temp131 != 0: 0; _temp136=(( struct Cyc_List_List*) _check_null(
_temp136))->tl, _temp131=(( struct Cyc_List_List*) _check_null( _temp131))->tl){
struct Cyc_Absyn_Structfield _temp142; struct Cyc_List_List* _temp143; struct
Cyc_Core_Opt* _temp145; void* _temp147; struct Cyc_Absyn_Tqual _temp149; struct
_tagged_string* _temp151; struct Cyc_Absyn_Structfield* _temp140=( struct Cyc_Absyn_Structfield*)((
struct Cyc_List_List*) _check_null( _temp136))->hd; _temp142=* _temp140; _LL152:
_temp151=( struct _tagged_string*) _temp142.name; goto _LL150; _LL150: _temp149=(
struct Cyc_Absyn_Tqual) _temp142.tq; goto _LL148; _LL148: _temp147=( void*)
_temp142.type; goto _LL146; _LL146: _temp145=( struct Cyc_Core_Opt*) _temp142.width;
goto _LL144; _LL144: _temp143=( struct Cyc_List_List*) _temp142.attributes; goto
_LL141; _LL141: { struct Cyc_Absyn_Structfield _temp155; struct Cyc_List_List*
_temp156; struct Cyc_Core_Opt* _temp158; void* _temp160; struct Cyc_Absyn_Tqual
_temp162; struct _tagged_string* _temp164; struct Cyc_Absyn_Structfield*
_temp153=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp131))->hd; _temp155=* _temp153; _LL165: _temp164=( struct _tagged_string*)
_temp155.name; goto _LL163; _LL163: _temp162=( struct Cyc_Absyn_Tqual) _temp155.tq;
goto _LL161; _LL161: _temp160=( void*) _temp155.type; goto _LL159; _LL159:
_temp158=( struct Cyc_Core_Opt*) _temp155.width; goto _LL157; _LL157: _temp156=(
struct Cyc_List_List*) _temp155.attributes; goto _LL154; _LL154: if( Cyc_String_zstrptrcmp(
_temp151, _temp164) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp166= t; struct _tagged_string _temp167= _temp101; struct _tagged_string
_temp168=* _temp151; struct _tagged_string _temp169=* _temp164; xprintf("%.*s %.*s : field name mismatch %.*s != %.*s",
_temp166.last_plus_one - _temp166.curr, _temp166.curr, _temp167.last_plus_one -
_temp167.curr, _temp167.curr, _temp168.last_plus_one - _temp168.curr, _temp168.curr,
_temp169.last_plus_one - _temp169.curr, _temp169.curr);})); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp143, _temp156)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp170= t; struct _tagged_string _temp171= _temp101; struct _tagged_string
_temp172=* _temp151; xprintf("%.*s %.*s : attribute mismatch on field %.*s",
_temp170.last_plus_one - _temp170.curr, _temp170.curr, _temp171.last_plus_one -
_temp171.curr, _temp171.curr, _temp172.last_plus_one - _temp172.curr, _temp172.curr);}));
_temp103= 0;} if( ! Cyc_Tcutil_equal_tqual( _temp149, _temp162)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp173= t; struct _tagged_string _temp174=
_temp101; struct _tagged_string _temp175=* _temp151; xprintf("%.*s %.*s : qualifier mismatch on field %.*s",
_temp173.last_plus_one - _temp173.curr, _temp173.curr, _temp174.last_plus_one -
_temp174.curr, _temp174.curr, _temp175.last_plus_one - _temp175.curr, _temp175.curr);}));
_temp103= 0;} if(((( _temp145 != 0? _temp158 != 0: 0)? Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp145))->v) !=
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp158))->v): 0)? 1:( _temp145 == 0? _temp158 != 0: 0))? 1:(
_temp145 != 0? _temp158 == 0: 0)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp176= t; struct _tagged_string _temp177= _temp101; struct
_tagged_string _temp178=* _temp151; xprintf("%.*s %.*s : bitfield mismatch on field %.*s",
_temp176.last_plus_one - _temp176.curr, _temp176.curr, _temp177.last_plus_one -
_temp177.curr, _temp177.curr, _temp178.last_plus_one - _temp178.curr, _temp178.curr);}));
_temp103= 0;}{ void* subst_t1= Cyc_Tcutil_substitute( _temp138, _temp160); if( !
Cyc_Tcdecl_check_type( _temp147, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp179= t; struct _tagged_string _temp180= _temp101; struct
_tagged_string _temp181=* _temp151; struct _tagged_string _temp182= Cyc_Absynpp_typ2string(
_temp147); struct _tagged_string _temp183= Cyc_Absynpp_typ2string( subst_t1);
xprintf("%.*s %.*s : type mismatch on field %.*s: %.*s != %.*s", _temp179.last_plus_one
- _temp179.curr, _temp179.curr, _temp180.last_plus_one - _temp180.curr, _temp180.curr,
_temp181.last_plus_one - _temp181.curr, _temp181.curr, _temp182.last_plus_one -
_temp182.curr, _temp182.curr, _temp183.last_plus_one - _temp183.curr, _temp183.curr);}));
_temp103= 0;}}}} if( _temp136 != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp184= t; struct _tagged_string _temp185= _temp101; struct
_tagged_string _temp186=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp136))->hd)->name; xprintf("%.*s %.*s is missing field %.*s",
_temp184.last_plus_one - _temp184.curr, _temp184.curr, _temp185.last_plus_one -
_temp185.curr, _temp185.curr, _temp186.last_plus_one - _temp186.curr, _temp186.curr);}));
_temp103= 0;} if( _temp131 != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp187= t; struct _tagged_string _temp188= _temp101; struct
_tagged_string _temp189=*(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp131))->hd)->name; xprintf("%.*s %.*s has extra field %.*s",
_temp187.last_plus_one - _temp187.curr, _temp187.curr, _temp188.last_plus_one -
_temp188.curr, _temp188.curr, _temp189.last_plus_one - _temp189.curr, _temp189.curr);}));
_temp103= 0;} d2= d0; goto _LL113;} _LL113:;} if( ! _temp103){ return 0;} if(
_temp109 ==( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*) d2;} else{ d2=({
struct Cyc_Absyn_Structdecl* _temp190=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp190[ 0]=* d2; _temp190;});( void*)(
d2->sc=( void*) _temp109); return( struct Cyc_Absyn_Structdecl*) d2;}}}}} struct
Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0, d1,(
struct _tagged_string)({ char* _temp191=( char*)"struct"; struct _tagged_string
_temp192; _temp192.curr= _temp191; _temp192.base= _temp191; _temp192.last_plus_one=
_temp191 + 7; _temp192;}), loc, msg);} struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ return( struct Cyc_Absyn_Uniondecl*) Cyc_Tcdecl_merge_struct_or_union_decl((
struct Cyc_Absyn_Structdecl*) d0,( struct Cyc_Absyn_Structdecl*) d1,( struct
_tagged_string)({ char* _temp193=( char*)"union"; struct _tagged_string _temp194;
_temp194.curr= _temp193; _temp194.base= _temp193; _temp194.last_plus_one=
_temp193 + 6; _temp194;}), loc, msg);} inline static struct _tagged_string Cyc_Tcdecl_is_x2string(
int is_x){ if( is_x){ return( struct _tagged_string)({ char* _temp195=( char*)"xtunion";
struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
_temp196.last_plus_one= _temp195 + 8; _temp196;});} else{ return( struct
_tagged_string)({ char* _temp197=( char*)"tunion"; struct _tagged_string
_temp198; _temp198.curr= _temp197; _temp198.base= _temp197; _temp198.last_plus_one=
_temp197 + 7; _temp198;});}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_merge_tunionfield( struct
Cyc_Absyn_Tunionfield* f0, struct Cyc_Absyn_Tunionfield* f1, struct Cyc_List_List*
inst, struct _tagged_string t, struct _tagged_string v, struct _tagged_string*
msg){ struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_String_zstrptrcmp((* f0->name).f2,(*
f1->name).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp199=
t; struct _tagged_string _temp200= v; struct _tagged_string _temp201=*(* f1->name).f2;
struct _tagged_string _temp202=*(* f0->name).f2; xprintf("%.*s %.*s: field name mismatch %.*s != %.*s",
_temp199.last_plus_one - _temp199.curr, _temp199.curr, _temp200.last_plus_one -
_temp200.curr, _temp200.curr, _temp201.last_plus_one - _temp201.curr, _temp201.curr,
_temp202.last_plus_one - _temp202.curr, _temp202.curr);})); return 0;}{ struct
_tagged_string _temp203=*(* f0->name).f2; goto _LL204; _LL204: { int _temp209;
void* _temp211; struct _tuple3 _temp207= Cyc_Tcdecl_merge_scope(( void*) f0->sc,(
void*) f1->sc,({ struct _tagged_string _temp205= t; struct _tagged_string
_temp206= v; xprintf("in %.*s %.*s, field", _temp205.last_plus_one - _temp205.curr,
_temp205.curr, _temp206.last_plus_one - _temp206.curr, _temp206.curr);}),
_temp203, loc, msg); _LL212: _temp211= _temp207.f1; goto _LL210; _LL210:
_temp209= _temp207.f2; goto _LL208; _LL208: { struct Cyc_List_List* _temp213= f0->tvs;
goto _LL214; _LL214: { struct Cyc_List_List* _temp215= f1->tvs; goto _LL216;
_LL216: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp213) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp215)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp217= t; struct _tagged_string _temp218= v;
struct _tagged_string _temp219= _temp203; xprintf("%.*s %.*s, field %.*s: type parameter number mismatch",
_temp217.last_plus_one - _temp217.curr, _temp217.curr, _temp218.last_plus_one -
_temp218.curr, _temp218.curr, _temp219.last_plus_one - _temp219.curr, _temp219.curr);}));
return 0;}{ struct Cyc_List_List* _temp220= Cyc_Tcdecl_build_tvs_map( _temp213,
_temp215); goto _LL221; _LL221: { struct Cyc_List_List* _temp222= f0->typs; goto
_LL223; _LL223: { struct Cyc_List_List* _temp224= f1->typs; goto _LL225; _LL225:
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp222) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp224)){ Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp226= t; struct _tagged_string _temp227= v;
struct _tagged_string _temp228= _temp203; xprintf("%.*s %.*s, field %.*s: parameter number mismatch",
_temp226.last_plus_one - _temp226.curr, _temp226.curr, _temp227.last_plus_one -
_temp227.curr, _temp227.curr, _temp228.last_plus_one - _temp228.curr, _temp228.curr);}));
_temp209= 0;} for( 0; _temp222 != 0; _temp222=(( struct Cyc_List_List*)
_check_null( _temp222))->tl, _temp224=(( struct Cyc_List_List*) _check_null(
_temp224))->tl){ if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp222))->hd)).f1,(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp224))->hd)).f1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp229= t; struct _tagged_string _temp230= v; struct
_tagged_string _temp231= _temp203; xprintf("%.*s %.*s, field %.*s: parameter qualifier",
_temp229.last_plus_one - _temp229.curr, _temp229.curr, _temp230.last_plus_one -
_temp230.curr, _temp230.curr, _temp231.last_plus_one - _temp231.curr, _temp231.curr);}));
_temp209= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp220,
inst),(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp224))->hd)).f2);
if( ! Cyc_Tcdecl_check_type((*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( _temp222))->hd)).f2, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp232= t; struct _tagged_string _temp233= v; struct
_tagged_string _temp234= _temp203; struct _tagged_string _temp235= Cyc_Absynpp_typ2string((*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp222))->hd)).f2);
struct _tagged_string _temp236= Cyc_Absynpp_typ2string( subst_t1); xprintf("%.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp232.last_plus_one - _temp232.curr, _temp232.curr, _temp233.last_plus_one -
_temp233.curr, _temp233.curr, _temp234.last_plus_one - _temp234.curr, _temp234.curr,
_temp235.last_plus_one - _temp235.curr, _temp235.curr, _temp236.last_plus_one -
_temp236.curr, _temp236.curr);})); _temp209= 0;}}} if( ! _temp209){ return 0;}
if(( void*) f0->sc == _temp211){ struct Cyc_Absyn_Tunionfield* _temp238=({
struct Cyc_Absyn_Tunionfield* _temp237=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp237[ 0]=* f0; _temp237;});
goto _LL239; _LL239:( void*)((( struct Cyc_Absyn_Tunionfield*) _check_null(
_temp238))->sc=( void*) _temp211); return _temp238;} else{ return( struct Cyc_Absyn_Tunionfield*)
f0;}}}}}}}}} static int Cyc_Tcdecl_substitute_tunionfield_f1( struct
_tagged_string* name, struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp( x->name,
name) == 0;} static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2( struct
Cyc_List_List* inst, struct _tuple7* x){ struct _tuple7 _temp242; void* _temp243;
struct Cyc_Absyn_Tqual _temp245; struct _tuple7* _temp240= x; _temp242=*
_temp240; _LL246: _temp245= _temp242.f1; goto _LL244; _LL244: _temp243= _temp242.f2;
goto _LL241; _LL241: return({ struct _tuple7* _temp247=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7)); _temp247->f1= _temp245; _temp247->f2= Cyc_Tcutil_substitute(
inst, _temp243); _temp247;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* _temp249=({
struct Cyc_Absyn_Tunionfield* _temp248=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp248[ 0]=* f1; _temp248;});
goto _LL250; _LL250: { struct Cyc_List_List* _temp251= 0; goto _LL252; _LL252:{
struct Cyc_List_List* _temp253= f1->tvs; goto _LL254; _LL254: for( 0; _temp253
!= 0; _temp253=(( struct Cyc_List_List*) _check_null( _temp253))->tl){ struct
Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp253))->hd; if((( int(*)( int(* pred)( struct _tagged_string*,
struct Cyc_Absyn_Tvar*), struct _tagged_string* env, struct Cyc_List_List* x))
Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1, tv->name, d0tvs)? 1:((
int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*), struct
_tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)){ int i= 0; struct _tagged_string base=* tv->name; struct
_tagged_string* new_name; do { new_name=({ struct _tagged_string* _temp255=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp255[ 0]=
Cyc_String_strconcat( base, Cyc_Core_string_of_int( i)); _temp255;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*), struct
_tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*),
struct _tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp256=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp256[ 0]=* tv; _temp256;}); tv->name=
new_name;} _temp251=({ struct Cyc_List_List* _temp257=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp257->hd=( void*) tv; _temp257->tl=
_temp251; _temp257;});}} _temp251=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp251); _temp249->tvs= _temp251;{ struct Cyc_List_List*
_temp258= Cyc_Tcdecl_build_tvs_map( _temp251, f1->tvs); goto _LL259; _LL259: {
struct Cyc_List_List* _temp260=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp258, inst1); goto _LL261;
_LL261: _temp249->typs=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct
Cyc_List_List*, struct _tuple7*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcdecl_substitute_tunionfield_f2, _temp260, f1->typs);
return _temp249;}}}} static struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
struct Cyc_List_List* tvs0, struct Cyc_List_List* tvs1, int* res, int* incl,
struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp275=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp275[ 0]= 0; _temp275;}); struct Cyc_List_List** _temp262= f2sp; goto _LL263;
_LL263: { int cmp= - 1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*)
_check_null( f1s))->tl){ while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { struct Cyc_List_List*
_temp265=({ struct Cyc_List_List* _temp264=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp264->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd); _temp264->tl= 0; _temp264;});
goto _LL266; _LL266:* _temp262= _temp265; _temp262=&(( struct Cyc_List_List*)
_check_null( _temp265))->tl; f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;}
if( f0s == 0? 1: cmp > 0){* incl= 0;{ struct Cyc_List_List* _temp268=({ struct
Cyc_List_List* _temp267=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp267->hd=( void*) Cyc_Tcdecl_substitute_tunionfield( tvs0, tvs1, inst,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd);
_temp267->tl= 0; _temp267;}); goto _LL269; _LL269:* _temp262= _temp268; _temp262=&((
struct Cyc_List_List*) _check_null( _temp268))->tl;}} else{ struct Cyc_Absyn_Tunionfield*
_temp270= Cyc_Tcdecl_merge_tunionfield(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( f0s))->hd,( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( f1s))->hd, inst, t, v, msg); goto _LL271; _LL271:
if( _temp270 != 0){ if( _temp270 !=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)){* incl= 0;}{ struct Cyc_List_List*
_temp273=({ struct Cyc_List_List* _temp272=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp272->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp270))); _temp272->tl= 0;
_temp272;}); goto _LL274; _LL274:* _temp262= _temp273; _temp262=&(( struct Cyc_List_List*)
_check_null( _temp273))->tl;}} else{* res= 0;} f0s=(( struct Cyc_List_List*)
_check_null( f0s))->tl;}} if( f1s != 0){* incl= 0;* _temp262= f1s;} else{*
_temp262= f0s;} return* f2sp;}} static struct Cyc_List_List* Cyc_Tcdecl_sub_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
int* res, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp286=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp286[ 0]= 0; _temp286;}); struct Cyc_List_List** _temp276= f2sp; goto _LL277;
_LL277: { int cmp= - 1; for( 0; f0s != 0? f1s != 0: 0; f1s=(( struct Cyc_List_List*)
_check_null( f1s))->tl){ while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd)->name)) < 0: 0) { f0s=(( struct
Cyc_List_List*) _check_null( f0s))->tl;} if( f0s == 0? 1: cmp > 0){ struct Cyc_List_List*
_temp279=({ struct Cyc_List_List* _temp278=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp278->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f1s))->hd); _temp278->tl= 0; _temp278;});
goto _LL280; _LL280:* _temp276= _temp279; _temp276=&(( struct Cyc_List_List*)
_check_null( _temp279))->tl;} else{ struct Cyc_Absyn_Tunionfield* _temp281= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f0s))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( f1s))->hd,
inst, t, v, msg); goto _LL282; _LL282: if( _temp281 == 0){* res= 0;} else{ if(
_temp281 !=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( f0s))->hd)){ struct Cyc_List_List* _temp284=({
struct Cyc_List_List* _temp283=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp283->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp281))); _temp283->tl= 0;
_temp283;}); goto _LL285; _LL285:* _temp276= _temp284; _temp276=&(( struct Cyc_List_List*)
_check_null( _temp284))->tl;}} f0s=(( struct Cyc_List_List*) _check_null( f0s))->tl;}}
if( f1s != 0){* _temp276= f1s;} return* f2sp;}} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct
Cyc_List_List* f){ if( f == 0){ return({ struct _tuple8 _temp287; _temp287.f1= 0;
_temp287.f2= 0; _temp287;});} if((( struct Cyc_List_List*) _check_null( f))->tl
== 0){ return({ struct _tuple8 _temp288; _temp288.f1= f; _temp288.f2= 0;
_temp288;});}{ struct Cyc_List_List* _temp291; struct Cyc_List_List* _temp293;
struct _tuple8 _temp289=(( struct _tuple8(*)( struct Cyc_List_List* f)) Cyc_Tcdecl_split)(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( f))->tl))->tl);
_LL294: _temp293= _temp289.f1; goto _LL292; _LL292: _temp291= _temp289.f2; goto
_LL290; _LL290: return({ struct _tuple8 _temp295; _temp295.f1=({ struct Cyc_List_List*
_temp297=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp297->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( f))->hd);
_temp297->tl= _temp293; _temp297;}); _temp295.f2=({ struct Cyc_List_List*
_temp296=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp296->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( f))->tl))->hd); _temp296->tl= _temp291; _temp296;});
_temp295;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct Cyc_List_List* _temp300; struct Cyc_List_List*
_temp302; struct _tuple8 _temp298=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL303: _temp302= _temp298.f1; goto _LL301; _LL301:
_temp300= _temp298.f2; goto _LL299; _LL299: if( _temp302 != 0?(( struct Cyc_List_List*)
_check_null( _temp302))->tl != 0: 0){ _temp302= Cyc_Tcdecl_sort_xtunion_fields(
_temp302, res, v, loc, msg);} if( _temp300 != 0?(( struct Cyc_List_List*)
_check_null( _temp300))->tl != 0: 0){ _temp300= Cyc_Tcdecl_sort_xtunion_fields(
_temp300, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp302,
_temp300, 0, 0, 0, res,({ int* _temp304=( int*) GC_malloc_atomic( sizeof( int));
_temp304[ 0]= 1; _temp304;}),( struct _tagged_string)({ char* _temp305=( char*)"xtunion";
struct _tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 8; _temp306;}),* v, loc, msg);} struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tagged_string _temp307= Cyc_Absynpp_qvar2string( d0->name); goto _LL308; _LL308: {
struct _tagged_string t=( struct _tagged_string)({ char* _temp362=( char*)"[x]tunion";
struct _tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 10; _temp363;}); int _temp309= 1; goto _LL310;
_LL310: if( d0->is_xtunion != d1->is_xtunion){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp311= _temp307; struct _tagged_string _temp312= Cyc_Tcdecl_is_x2string(
d0->is_xtunion); struct _tagged_string _temp313= Cyc_Tcdecl_is_x2string( d1->is_xtunion);
xprintf("expected %.*s to be a %.*s instead of a %.*s", _temp311.last_plus_one -
_temp311.curr, _temp311.curr, _temp312.last_plus_one - _temp312.curr, _temp312.curr,
_temp313.last_plus_one - _temp313.curr, _temp313.curr);})); _temp309= 0;} else{
t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs( d0->tvs,
d1->tvs, t, _temp307, loc, msg)){ return 0;}{ int _temp316; void* _temp318;
struct _tuple3 _temp314= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, _temp307, loc, msg); _LL319: _temp318= _temp314.f1; goto _LL317; _LL317:
_temp316= _temp314.f2; goto _LL315; _LL315: if( ! _temp316){ _temp309= 0;}{
struct Cyc_Absyn_Tuniondecl* d2;{ struct _tuple6 _temp321=({ struct _tuple6
_temp320; _temp320.f1= d0->fields; _temp320.f2= d1->fields; _temp320;}); struct
Cyc_Core_Opt* _temp329; struct Cyc_Core_Opt* _temp331; struct Cyc_Core_Opt*
_temp333; struct Cyc_Core_Opt* _temp335; struct Cyc_Core_Opt* _temp337; struct
Cyc_Core_Opt _temp339; struct Cyc_List_List* _temp340; struct Cyc_Core_Opt*
_temp342; struct Cyc_Core_Opt _temp344; struct Cyc_List_List* _temp345; _LL323:
_LL332: _temp331= _temp321.f1; goto _LL330; _LL330: _temp329= _temp321.f2; if(
_temp329 == 0){ goto _LL324;} else{ goto _LL325;} _LL325: _LL336: _temp335=
_temp321.f1; if( _temp335 == 0){ goto _LL334;} else{ goto _LL327;} _LL334:
_temp333= _temp321.f2; goto _LL326; _LL327: _LL343: _temp342= _temp321.f1; if(
_temp342 == 0){ goto _LL322;} else{ _temp344=* _temp342; _LL346: _temp345=(
struct Cyc_List_List*) _temp344.v; goto _LL338;} _LL338: _temp337= _temp321.f2;
if( _temp337 == 0){ goto _LL322;} else{ _temp339=* _temp337; _LL341: _temp340=(
struct Cyc_List_List*) _temp339.v; goto _LL328;} _LL324: d2= d0; goto _LL322;
_LL326: d2= d1; goto _LL322; _LL328: { struct Cyc_List_List* _temp347= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); goto _LL348; _LL348: if( d0->is_xtunion){ int _temp349= 1;
goto _LL350; _LL350: { struct Cyc_List_List* _temp351= Cyc_Tcdecl_merge_xtunion_fields(
_temp345, _temp340, _temp347, d0->tvs, d1->tvs,& _temp309,& _temp349, t,
_temp307, loc, msg); goto _LL352; _LL352: if( _temp349){ d2= d0;} else{ d2=({
struct Cyc_Absyn_Tuniondecl* _temp353=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp353[ 0]=* d0; _temp353;});( void*)(
d2->sc=( void*) _temp318); d2->fields=({ struct Cyc_Core_Opt* _temp354=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp354->v=( void*)
_temp351; _temp354;});}}} else{ for( 0; _temp345 != 0? _temp340 != 0: 0;
_temp345=(( struct Cyc_List_List*) _check_null( _temp345))->tl, _temp340=((
struct Cyc_List_List*) _check_null( _temp340))->tl){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp345))->hd,(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( _temp340))->hd,
_temp347, t, _temp307, msg);} if( _temp345 != 0){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp355= t; struct _tagged_string _temp356= _temp307;
struct _tagged_string _temp357=*(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp340))->hd)->name).f2; xprintf("%.*s %.*s has extra field %.*s",
_temp355.last_plus_one - _temp355.curr, _temp355.curr, _temp356.last_plus_one -
_temp356.curr, _temp356.curr, _temp357.last_plus_one - _temp357.curr, _temp357.curr);}));
_temp309= 0;} if( _temp340 != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp358= t; struct _tagged_string _temp359= _temp307; struct
_tagged_string _temp360=*(*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp340))->hd)->name).f2; xprintf("%.*s %.*s is missing field %.*s",
_temp358.last_plus_one - _temp358.curr, _temp358.curr, _temp359.last_plus_one -
_temp359.curr, _temp359.curr, _temp360.last_plus_one - _temp360.curr, _temp360.curr);}));
_temp309= 0;} d2= d0;} goto _LL322;} _LL322:;} if( ! _temp309){ return 0;} if(
_temp318 ==( void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*) d2;} else{ d2=({
struct Cyc_Absyn_Tuniondecl* _temp361=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp361[ 0]=* d2; _temp361;});( void*)(
d2->sc=( void*) _temp318); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}}} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl* d0,
struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string _temp364= Cyc_Absynpp_qvar2string(
d0->name); goto _LL365; _LL365: { int _temp366= 1; goto _LL367; _LL367: { int
_temp372; void* _temp374; struct _tuple3 _temp370= Cyc_Tcdecl_merge_scope(( void*)
d0->sc,( void*) d1->sc,( struct _tagged_string)({ char* _temp368=( char*)"enum";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 5; _temp369;}), _temp364, loc, msg); _LL375:
_temp374= _temp370.f1; goto _LL373; _LL373: _temp372= _temp370.f2; goto _LL371;
_LL371: if( ! _temp372){ _temp366= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct
_tuple6 _temp377=({ struct _tuple6 _temp376; _temp376.f1= d0->fields; _temp376.f2=
d1->fields; _temp376;}); struct Cyc_Core_Opt* _temp385; struct Cyc_Core_Opt*
_temp387; struct Cyc_Core_Opt* _temp389; struct Cyc_Core_Opt* _temp391; struct
Cyc_Core_Opt* _temp393; struct Cyc_Core_Opt _temp395; struct Cyc_List_List*
_temp396; struct Cyc_Core_Opt* _temp398; struct Cyc_Core_Opt _temp400; struct
Cyc_List_List* _temp401; _LL379: _LL388: _temp387= _temp377.f1; goto _LL386;
_LL386: _temp385= _temp377.f2; if( _temp385 == 0){ goto _LL380;} else{ goto
_LL381;} _LL381: _LL392: _temp391= _temp377.f1; if( _temp391 == 0){ goto _LL390;}
else{ goto _LL383;} _LL390: _temp389= _temp377.f2; goto _LL382; _LL383: _LL399:
_temp398= _temp377.f1; if( _temp398 == 0){ goto _LL378;} else{ _temp400=*
_temp398; _LL402: _temp401=( struct Cyc_List_List*) _temp400.v; goto _LL394;}
_LL394: _temp393= _temp377.f2; if( _temp393 == 0){ goto _LL378;} else{ _temp395=*
_temp393; _LL397: _temp396=( struct Cyc_List_List*) _temp395.v; goto _LL384;}
_LL380: d2= d0; goto _LL378; _LL382: d2= d1; goto _LL378; _LL384: for( 0;
_temp401 != 0? _temp396 != 0: 0; _temp401=(( struct Cyc_List_List*) _check_null(
_temp401))->tl, _temp396=(( struct Cyc_List_List*) _check_null( _temp396))->tl){
struct Cyc_Absyn_Enumfield _temp405; struct Cyc_Position_Segment* _temp406;
struct Cyc_Absyn_Exp* _temp408; struct _tuple0* _temp410; struct Cyc_Absyn_Enumfield*
_temp403=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp401))->hd; _temp405=* _temp403; _LL411: _temp410=( struct _tuple0*)
_temp405.name; goto _LL409; _LL409: _temp408=( struct Cyc_Absyn_Exp*) _temp405.tag;
goto _LL407; _LL407: _temp406=( struct Cyc_Position_Segment*) _temp405.loc; goto
_LL404; _LL404: { struct Cyc_Absyn_Enumfield _temp414; struct Cyc_Position_Segment*
_temp415; struct Cyc_Absyn_Exp* _temp417; struct _tuple0* _temp419; struct Cyc_Absyn_Enumfield*
_temp412=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null(
_temp396))->hd; _temp414=* _temp412; _LL420: _temp419=( struct _tuple0*)
_temp414.name; goto _LL418; _LL418: _temp417=( struct Cyc_Absyn_Exp*) _temp414.tag;
goto _LL416; _LL416: _temp415=( struct Cyc_Position_Segment*) _temp414.loc; goto
_LL413; _LL413: if( Cyc_String_zstrptrcmp((* _temp419).f2,(* _temp410).f2) != 0){
Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp421= _temp364; struct
_tagged_string _temp422=*(* _temp419).f2; struct _tagged_string _temp423=*(*
_temp410).f2; xprintf("enum %.*s: field name mismatch %.*s != %.*s", _temp421.last_plus_one
- _temp421.curr, _temp421.curr, _temp422.last_plus_one - _temp422.curr, _temp422.curr,
_temp423.last_plus_one - _temp423.curr, _temp423.curr);})); _temp366= 0;} if(
Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp408))) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp417)))){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp424= _temp364; struct _tagged_string _temp425=*(*
_temp419).f2; xprintf("enum %.*s, field %.*s, value mismatch", _temp424.last_plus_one
- _temp424.curr, _temp424.curr, _temp425.last_plus_one - _temp425.curr, _temp425.curr);}));
_temp366= 0;}}} d2= d0; goto _LL378; _LL378:;} if( ! _temp366){ return 0;} if((
void*) d2->sc == _temp374){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({
struct Cyc_Absyn_Enumdecl* _temp426=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp426[ 0]=* d2; _temp426;});( void*)( d2->sc=(
void*) _temp374); return( struct Cyc_Absyn_Enumdecl*) d2;}}}}} inline static
struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl( void* sc0, void* t0, struct Cyc_Absyn_Tqual
tq0, struct Cyc_List_List* atts0, void* sc1, void* t1, struct Cyc_Absyn_Tqual
tq1, struct Cyc_List_List* atts1, struct _tagged_string t, struct _tagged_string
v, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ int _temp427=
1; goto _LL428; _LL428: { int _temp431; void* _temp433; struct _tuple3 _temp429=
Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg); _LL434: _temp433= _temp429.f1;
goto _LL432; _LL432: _temp431= _temp429.f2; goto _LL430; _LL430: if( ! _temp431){
_temp427= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp435= t; struct _tagged_string _temp436= v; struct
_tagged_string _temp437= Cyc_Absynpp_typ2string( t1); struct _tagged_string
_temp438= Cyc_Absynpp_typ2string( t0); xprintf("%.*s %.*s has type %.*s instead of %.*s",
_temp435.last_plus_one - _temp435.curr, _temp435.curr, _temp436.last_plus_one -
_temp436.curr, _temp436.curr, _temp437.last_plus_one - _temp437.curr, _temp437.curr,
_temp438.last_plus_one - _temp438.curr, _temp438.curr);})); _temp427= 0;} if( !
Cyc_Tcutil_equal_tqual( tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp439= t; struct _tagged_string _temp440= v; xprintf("%.*s %.*s has different type qualifiers",
_temp439.last_plus_one - _temp439.curr, _temp439.curr, _temp440.last_plus_one -
_temp440.curr, _temp440.curr);})); _temp427= 0;} if( ! Cyc_Tcutil_same_atts(
atts0, atts1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp441= t;
struct _tagged_string _temp442= v; xprintf("%.*s %.*s has different attributes",
_temp441.last_plus_one - _temp441.curr, _temp441.curr, _temp442.last_plus_one -
_temp442.curr, _temp442.curr);})); _temp427= 0;} return({ struct _tuple3
_temp443; _temp443.f1= _temp433; _temp443.f2= _temp427; _temp443;});}} struct
Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl( struct Cyc_Absyn_Vardecl* d0,
struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string _temp444= Cyc_Absynpp_qvar2string(
d0->name); goto _LL445; _LL445: { int _temp450; void* _temp452; struct _tuple3
_temp448= Cyc_Tcdecl_check_var_or_fn_decl(( void*) d0->sc,( void*) d0->type, d0->tq,
d0->attributes,( void*) d1->sc,( void*) d1->type, d1->tq, d1->attributes,(
struct _tagged_string)({ char* _temp446=( char*)"variable"; struct
_tagged_string _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 9; _temp447;}), _temp444, loc, msg); _LL453:
_temp452= _temp448.f1; goto _LL451; _LL451: _temp450= _temp448.f2; goto _LL449;
_LL449: if( ! _temp450){ return 0;} if(( void*) d0->sc == _temp452){ return(
struct Cyc_Absyn_Vardecl*) d0;} else{ struct Cyc_Absyn_Vardecl* _temp455=({
struct Cyc_Absyn_Vardecl* _temp454=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp454[ 0]=* d0; _temp454;}); goto _LL456;
_LL456:( void*)((( struct Cyc_Absyn_Vardecl*) _check_null( _temp455))->sc=( void*)
_temp452); return _temp455;}}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct _tagged_string _temp457= Cyc_Absynpp_qvar2string(
d0->name); goto _LL458; _LL458: if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs,(
struct _tagged_string)({ char* _temp459=( char*)"typedef"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 8; _temp460;}), _temp457, loc, msg)){ return 0;}{ struct Cyc_List_List*
_temp461= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); goto _LL462; _LL462: {
void* subst_defn1= Cyc_Tcutil_substitute( _temp461,( void*) d1->defn); if( ! Cyc_Tcdecl_check_type((
void*) d0->defn, subst_defn1)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp463= _temp457; struct _tagged_string _temp464= Cyc_Absynpp_typ2string(
subst_defn1); struct _tagged_string _temp465= Cyc_Absynpp_typ2string(( void*) d0->defn);
xprintf("typedef %.*s does refer to the same type: %.*s != %.*s", _temp463.last_plus_one
- _temp463.curr, _temp463.curr, _temp464.last_plus_one - _temp464.curr, _temp464.curr,
_temp465.last_plus_one - _temp465.curr, _temp465.curr);})); return 0;} return(
struct Cyc_Absyn_Typedefdecl*) d0;}}} void* Cyc_Tcdecl_merge_binding( void* b0,
void* b1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tuple4 _temp467=({ struct _tuple4 _temp466; _temp466.f1= b0; _temp466.f2= b1;
_temp466;}); void* _temp481; void* _temp483; void* _temp485; struct Cyc_Absyn_Vardecl*
_temp487; void* _temp489; struct Cyc_Absyn_Vardecl* _temp491; void* _temp493;
struct Cyc_Absyn_Fndecl* _temp495; void* _temp497; struct Cyc_Absyn_Vardecl*
_temp499; void* _temp501; struct Cyc_Absyn_Fndecl* _temp503; void* _temp505;
struct Cyc_Absyn_Fndecl* _temp507; void* _temp509; struct Cyc_Absyn_Vardecl*
_temp511; void* _temp513; struct Cyc_Absyn_Fndecl* _temp515; _LL469: _LL484:
_temp483= _temp467.f1; if( _temp483 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL482;} else{ goto _LL471;} _LL482: _temp481= _temp467.f2; if( _temp481 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL470;} else{ goto _LL471;} _LL471: _LL490:
_temp489= _temp467.f1; if(( unsigned int) _temp489 > 1u?*(( int*) _temp489) ==
Cyc_Absyn_Global_b: 0){ _LL492: _temp491=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Global_b_struct*) _temp489)->f1; goto _LL486;} else{ goto _LL473;}
_LL486: _temp485= _temp467.f2; if(( unsigned int) _temp485 > 1u?*(( int*)
_temp485) == Cyc_Absyn_Global_b: 0){ _LL488: _temp487=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp485)->f1; goto _LL472;} else{ goto
_LL473;} _LL473: _LL498: _temp497= _temp467.f1; if(( unsigned int) _temp497 > 1u?*((
int*) _temp497) == Cyc_Absyn_Global_b: 0){ _LL500: _temp499=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp497)->f1; goto _LL494;} else{ goto
_LL475;} _LL494: _temp493= _temp467.f2; if(( unsigned int) _temp493 > 1u?*(( int*)
_temp493) == Cyc_Absyn_Funname_b: 0){ _LL496: _temp495=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp493)->f1; goto _LL474;} else{ goto
_LL475;} _LL475: _LL506: _temp505= _temp467.f1; if(( unsigned int) _temp505 > 1u?*((
int*) _temp505) == Cyc_Absyn_Funname_b: 0){ _LL508: _temp507=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp505)->f1; goto _LL502;} else{ goto
_LL477;} _LL502: _temp501= _temp467.f2; if(( unsigned int) _temp501 > 1u?*(( int*)
_temp501) == Cyc_Absyn_Funname_b: 0){ _LL504: _temp503=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp501)->f1; goto _LL476;} else{ goto
_LL477;} _LL477: _LL514: _temp513= _temp467.f1; if(( unsigned int) _temp513 > 1u?*((
int*) _temp513) == Cyc_Absyn_Funname_b: 0){ _LL516: _temp515=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp513)->f1; goto _LL510;} else{ goto
_LL479;} _LL510: _temp509= _temp467.f2; if(( unsigned int) _temp509 > 1u?*(( int*)
_temp509) == Cyc_Absyn_Global_b: 0){ _LL512: _temp511=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp509)->f1; goto _LL478;} else{ goto
_LL479;} _LL479: goto _LL480; _LL470: return( void*) Cyc_Absyn_Unresolved_b;
_LL472: { struct Cyc_Absyn_Vardecl* _temp517= Cyc_Tcdecl_merge_vardecl( _temp491,
_temp487, loc, msg); goto _LL518; _LL518: if( _temp517 == 0){ return( void*) Cyc_Absyn_Unresolved_b;}
if(( struct Cyc_Absyn_Vardecl*) _temp491 == _temp517){ return b0;} else{ if((
struct Cyc_Absyn_Vardecl*) _temp487 == _temp517){ return b1;} else{ return( void*)({
struct Cyc_Absyn_Global_b_struct* _temp519=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp519[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp520; _temp520.tag= Cyc_Absyn_Global_b; _temp520.f1=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Vardecl*) _check_null( _temp517));
_temp520;}); _temp519;});}}} _LL474: { int _temp525; void* _temp527; struct
_tuple3 _temp523= Cyc_Tcdecl_check_var_or_fn_decl(( void*) _temp499->sc,( void*)
_temp499->type, _temp499->tq, _temp499->attributes,( void*) _temp495->sc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp495->cached_typ))->v, Cyc_Absyn_empty_tqual(),
_temp495->attributes,( struct _tagged_string)({ char* _temp521=( char*)"function";
struct _tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 9; _temp522;}), Cyc_Absynpp_qvar2string(
_temp499->name), loc, msg); _LL528: _temp527= _temp523.f1; goto _LL526; _LL526:
_temp525= _temp523.f2; goto _LL524; _LL524: if( ! _temp525){ return( void*) Cyc_Absyn_Unresolved_b;}
return b1;} _LL476: Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp529=
Cyc_Absynpp_qvar2string( _temp503->name); xprintf("redefinition of function %.*s",
_temp529.last_plus_one - _temp529.curr, _temp529.curr);})); return( void*) Cyc_Absyn_Unresolved_b;
_LL478: { int _temp534; void* _temp536; struct _tuple3 _temp532= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp515->sc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp515->cached_typ))->v,
Cyc_Absyn_empty_tqual(), _temp515->attributes,( void*) _temp511->sc,( void*)
_temp511->type, _temp511->tq, _temp511->attributes,( struct _tagged_string)({
char* _temp530=( char*)"variable"; struct _tagged_string _temp531; _temp531.curr=
_temp530; _temp531.base= _temp530; _temp531.last_plus_one= _temp530 + 9;
_temp531;}), Cyc_Absynpp_qvar2string( _temp515->name), loc, msg); _LL537:
_temp536= _temp532.f1; goto _LL535; _LL535: _temp534= _temp532.f2; goto _LL533;
_LL533: if( ! _temp534){ return( void*) Cyc_Absyn_Unresolved_b;} return b0;}
_LL480:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp538=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp538[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp539; _temp539.tag= Cyc_Core_InvalidArg;
_temp539.f1=( struct _tagged_string)({ char* _temp540=( char*)"Tcdecl::merge_binding";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 22; _temp541;}); _temp539;}); _temp538;}));
_LL468:;} struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_sub_structdecl( struct Cyc_Absyn_Structdecl*
ye, struct Cyc_Absyn_Structdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Structdecl* _temp542= Cyc_Tcdecl_merge_structdecl(
ye, y, loc, msg); goto _LL543; _LL543: if( _temp542 !=( struct Cyc_Absyn_Structdecl*)
ye){ if( _temp542 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp544= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( y->name))->v); xprintf("abstract struct %.*s is being used as non-abstract",
_temp544.last_plus_one - _temp544.curr, _temp544.curr);}));} return 0;} struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_sub_uniondecl( struct Cyc_Absyn_Uniondecl* ye,
struct Cyc_Absyn_Uniondecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Uniondecl* _temp545= Cyc_Tcdecl_merge_uniondecl(
ye, y, loc, msg); goto _LL546; _LL546: if( _temp545 !=( struct Cyc_Absyn_Uniondecl*)
ye){ if( _temp545 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp547= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( y->name))->v); xprintf("abstract union %.*s is being used as non-abstract",
_temp547.last_plus_one - _temp547.curr, _temp547.curr);}));} return 0;} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_sub_enumdecl( struct Cyc_Absyn_Enumdecl* ye,
struct Cyc_Absyn_Enumdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Enumdecl* _temp548= Cyc_Tcdecl_merge_enumdecl(
ye, y, loc, msg); goto _LL549; _LL549: if( _temp548 !=( struct Cyc_Absyn_Enumdecl*)
ye){ if( _temp548 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp550= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract enum %.*s is being used as non-abstract",
_temp550.last_plus_one - _temp550.curr, _temp550.curr);}));} return 0;} struct
Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_sub_typedefdecl( struct Cyc_Absyn_Typedefdecl*
ye, struct Cyc_Absyn_Typedefdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Typedefdecl* _temp551= Cyc_Tcdecl_merge_typedefdecl(
ye, y, loc, msg); goto _LL552; _LL552: if( _temp551 !=( struct Cyc_Absyn_Typedefdecl*)
ye){ if( _temp551 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp553= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract typedef %.*s is being used as non-abstract",
_temp553.last_plus_one - _temp553.curr, _temp553.curr);}));} return 0;} struct
Cyc_Absyn_Vardecl* Cyc_Tcdecl_sub_vardecl( struct Cyc_Absyn_Vardecl* ye, struct
Cyc_Absyn_Vardecl* y, struct Cyc_Position_Segment* loc, struct _tagged_string*
msg, int* res){ struct Cyc_Absyn_Vardecl* _temp554= Cyc_Tcdecl_merge_vardecl( ye,
y, loc, msg); goto _LL555; _LL555: if( _temp554 !=( struct Cyc_Absyn_Vardecl*)
ye){ if( _temp554 == 0){ return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp556= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract var %.*s is being used as non-abstract",
_temp556.last_plus_one - _temp556.curr, _temp556.curr);}));} return 0;} void Cyc_Tcdecl_f(){;}
struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_sub_tuniondecl( struct Cyc_Absyn_Tuniondecl*
ye, struct Cyc_Absyn_Tuniondecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Tuniondecl* _temp557= Cyc_Tcdecl_merge_tuniondecl(
ye, y, loc, msg); goto _LL558; _LL558: if( _temp557 !=( struct Cyc_Absyn_Tuniondecl*)
ye){ if( _temp557 == 0){ return 0;} if( y->is_xtunion? ye->fields != 0: 0){
_temp557=({ struct Cyc_Absyn_Tuniondecl* _temp559=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp559[ 0]=* y; _temp559;});((
struct Cyc_Absyn_Tuniondecl*) _check_null( _temp557))->fields=({ struct Cyc_Core_Opt*
_temp560=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp560->v=( void*) Cyc_Tcdecl_sub_xtunion_fields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ye->fields))->v,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( y->fields))->v, Cyc_Tcdecl_build_tvs_map( ye->tvs,
y->tvs), res,( struct _tagged_string)({ char* _temp561=( char*)"xtunion"; struct
_tagged_string _temp562; _temp562.curr= _temp561; _temp562.base= _temp561;
_temp562.last_plus_one= _temp561 + 8; _temp562;}), Cyc_Absynpp_qvar2string( y->name),
loc, msg); _temp560;}); return _temp557;} else{* res= 0; Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp563= Cyc_Absynpp_qvar2string( y->name);
xprintf("abstract [x]tunion %.*s is being used as non-abstract", _temp563.last_plus_one
- _temp563.curr, _temp563.curr);}));}} return 0;}