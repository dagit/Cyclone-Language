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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern void* Cyc_Absyn_exn_typ; extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
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
extern char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
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
_temp3 != 0; _temp3=({ struct Cyc_List_List* _temp5= _temp3; if( _temp5 == 0){
_throw( Null_Exception);} _temp5->tl;})){ te2= Cyc_Tcenv_add_pat_var( loc, te2,(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp6= _temp3; if( _temp6 ==
0){ _throw( Null_Exception);} _temp6->hd;}));(( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp7= _temp3; if( _temp7 == 0){ _throw( Null_Exception);}
_temp7->hd;}))->rgn= _temp1;}} if( where_opt != 0){ Cyc_Tcexp_tcTest( te2,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp8= where_opt; if( _temp8 ==
0){ _throw( Null_Exception);} _temp8;}),( struct _tagged_string)({ char* _temp9=(
char*)"switch clause guard"; struct _tagged_string _temp10; _temp10.curr= _temp9;
_temp10.base= _temp9; _temp10.last_plus_one= _temp9 + 20; _temp10;}));} if( vs
!= 0){ te2= Cyc_Tcenv_set_encloser( te2, s);} Cyc_Tcstmt_tcStmt( te2, s, 0); if(
vs != 0){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));}}}
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s0,
int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{ void* _temp11=( void*) s0->r;
struct Cyc_Absyn_Exp* _temp51; struct Cyc_Absyn_Stmt* _temp53; struct Cyc_Absyn_Stmt*
_temp55; struct Cyc_Absyn_Exp* _temp57; struct Cyc_Absyn_Stmt* _temp59; struct
Cyc_Absyn_Stmt* _temp61; struct Cyc_Absyn_Exp* _temp63; struct Cyc_Absyn_Stmt*
_temp65; struct _tuple2 _temp67; struct Cyc_Absyn_Stmt* _temp69; struct Cyc_Absyn_Exp*
_temp71; struct Cyc_Absyn_Stmt* _temp73; struct _tuple2 _temp75; struct Cyc_Absyn_Stmt*
_temp77; struct Cyc_Absyn_Exp* _temp79; struct _tuple2 _temp81; struct Cyc_Absyn_Stmt*
_temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_Exp* _temp87; struct
_tuple2 _temp89; struct Cyc_Absyn_Stmt* _temp91; struct Cyc_Absyn_Exp* _temp93;
struct Cyc_Absyn_Stmt* _temp95; struct Cyc_Absyn_Stmt* _temp97; struct Cyc_Absyn_Stmt**
_temp99; struct Cyc_Absyn_Stmt* _temp100; struct Cyc_Absyn_Stmt** _temp102;
struct Cyc_Absyn_Stmt* _temp103; struct Cyc_Absyn_Stmt** _temp105; struct
_tagged_string* _temp106; struct Cyc_Absyn_Switch_clause** _temp108; struct Cyc_Absyn_Switch_clause***
_temp110; struct Cyc_List_List* _temp111; struct Cyc_Absyn_Stmt* _temp113;
struct _tagged_string* _temp115; struct Cyc_List_List* _temp117; struct Cyc_Absyn_Exp*
_temp119; struct Cyc_List_List* _temp121; struct Cyc_Absyn_Stmt* _temp123;
struct Cyc_Absyn_Stmt* _temp125; struct Cyc_Absyn_Decl* _temp127; struct Cyc_Absyn_Stmt*
_temp129; struct Cyc_Absyn_Vardecl* _temp131; struct Cyc_Absyn_Tvar* _temp133;
struct Cyc_Absyn_Stmt* _temp135; struct Cyc_Absyn_Stmt* _temp137; _LL13: if(
_temp11 ==( void*) Cyc_Absyn_Skip_s){ goto _LL14;} else{ goto _LL15;} _LL15: if((
unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_Exp_s: 0){ _LL52:
_temp51=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp11)->f1;
goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_Seq_s: 0){ _LL56: _temp55=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp11)->f1; goto _LL54; _LL54: _temp53=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp11)->f2; goto
_LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_Return_s: 0){ _LL58: _temp57=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp11)->f1; goto _LL20;} else{ goto _LL21;}
_LL21: if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_IfThenElse_s:
0){ _LL64: _temp63=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp11)->f1; goto _LL62; _LL62: _temp61=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp11)->f2; goto _LL60; _LL60: _temp59=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp11)->f3; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp11
> 1u?*(( int*) _temp11) == Cyc_Absyn_While_s: 0){ _LL68: _temp67=( struct
_tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp11)->f1; _LL72: _temp71=
_temp67.f1; goto _LL70; _LL70: _temp69= _temp67.f2; goto _LL66; _LL66: _temp65=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp11)->f2; goto
_LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_For_s: 0){ _LL88: _temp87=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp11)->f1; goto _LL82; _LL82: _temp81=(
struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp11)->f2; _LL86: _temp85=
_temp81.f1; goto _LL84; _LL84: _temp83= _temp81.f2; goto _LL76; _LL76: _temp75=(
struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp11)->f3; _LL80: _temp79=
_temp75.f1; goto _LL78; _LL78: _temp77= _temp75.f2; goto _LL74; _LL74: _temp73=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp11)->f4; goto
_LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_Do_s: 0){ _LL96: _temp95=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp11)->f1; goto _LL90; _LL90: _temp89=( struct
_tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp11)->f2; _LL94: _temp93= _temp89.f1;
goto _LL92; _LL92: _temp91= _temp89.f2; goto _LL28;} else{ goto _LL29;} _LL29:
if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_Break_s: 0){
_LL98: _temp97=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp11)->f1; _temp99=&(( struct Cyc_Absyn_Break_s_struct*) _temp11)->f1; goto
_LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_Continue_s: 0){ _LL101: _temp100=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp11)->f1; _temp102=&(( struct Cyc_Absyn_Continue_s_struct*)
_temp11)->f1; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp11
> 1u?*(( int*) _temp11) == Cyc_Absyn_Goto_s: 0){ _LL107: _temp106=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp11)->f1; goto _LL104;
_LL104: _temp103=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp11)->f2; _temp105=&(( struct Cyc_Absyn_Goto_s_struct*) _temp11)->f2; goto
_LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_Fallthru_s: 0){ _LL112: _temp111=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp11)->f1; goto _LL109; _LL109: _temp108=(
struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp11)->f2;
_temp110=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp11)->f2; goto _LL36;}
else{ goto _LL37;} _LL37: if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) ==
Cyc_Absyn_Label_s: 0){ _LL116: _temp115=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp11)->f1; goto _LL114; _LL114: _temp113=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Label_s_struct*) _temp11)->f2; goto _LL38;} else{ goto _LL39;} _LL39:
if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_Switch_s: 0){
_LL120: _temp119=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp11)->f1; goto _LL118; _LL118: _temp117=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp11)->f2; goto _LL40;} else{ goto _LL41;} _LL41:
if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_TryCatch_s: 0){
_LL124: _temp123=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp11)->f1; goto _LL122; _LL122: _temp121=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp11)->f2; goto _LL42;} else{ goto _LL43;}
_LL43: if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_Decl_s: 0){
_LL128: _temp127=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp11)->f1; goto _LL126; _LL126: _temp125=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp11)->f2; goto _LL44;} else{ goto _LL45;} _LL45:
if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_Region_s: 0){
_LL134: _temp133=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp11)->f1; goto _LL132; _LL132: _temp131=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp11)->f2; goto _LL130; _LL130: _temp129=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*) _temp11)->f3; goto
_LL46;} else{ goto _LL47;} _LL47: if(( unsigned int) _temp11 > 1u?*(( int*)
_temp11) == Cyc_Absyn_Cut_s: 0){ _LL136: _temp135=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp11)->f1; goto _LL48;} else{ goto _LL49;}
_LL49: if(( unsigned int) _temp11 > 1u?*(( int*) _temp11) == Cyc_Absyn_Splice_s:
0){ _LL138: _temp137=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp11)->f1; goto _LL50;} else{ goto _LL12;} _LL14: return; _LL16: Cyc_Tcexp_tcExp(
te, 0, _temp51); return; _LL18: { struct Cyc_Tcenv_Tenv* _temp141= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp139=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp139[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp140; _temp140.tag= Cyc_Tcenv_Stmt_j; _temp140.f1= _temp53; _temp140;});
_temp139;})); goto _LL142; _LL142: Cyc_Tcstmt_tcStmt( _temp141, _temp55, 1);
while( 1) { void* _temp143=( void*) _temp53->r; struct Cyc_Absyn_Stmt* _temp149;
struct Cyc_Absyn_Stmt* _temp151; _LL145: if(( unsigned int) _temp143 > 1u?*((
int*) _temp143) == Cyc_Absyn_Seq_s: 0){ _LL152: _temp151=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp143)->f1; goto _LL150; _LL150: _temp149=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp143)->f2; goto
_LL146;} else{ goto _LL147;} _LL147: goto _LL148; _LL146: Cyc_Tcstmt_decorate_stmt(
te, _temp53); _temp141= Cyc_Tcenv_set_next( te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp153=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp153[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp154; _temp154.tag= Cyc_Tcenv_Stmt_j;
_temp154.f1= _temp149; _temp154;}); _temp153;})); Cyc_Tcstmt_tcStmt( _temp141,
_temp151, 1); _temp53= _temp149; continue; _LL148: goto seq_end; _LL144:;}
seq_end: Cyc_Tcstmt_tcStmt( te, _temp53, 1); return;} _LL20: { void* t= Cyc_Tcenv_return_typ(
te); if( _temp57 == 0){ void* _temp155= Cyc_Tcutil_compress( t); _LL157: if(
_temp155 ==( void*) Cyc_Absyn_VoidType){ goto _LL158;} else{ goto _LL159;}
_LL159: goto _LL160; _LL158: goto _LL156; _LL160: Cyc_Tcutil_terr( s0->loc,({
struct _tagged_string _temp161= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp161.last_plus_one - _temp161.curr, _temp161.curr);})); goto _LL156; _LL156:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp173= _temp57; if( _temp173 == 0){ _throw( Null_Exception);} _temp173;});
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp163= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp162= e->topt; if( _temp162 == 0){ _throw(
Null_Exception);} _temp162->v;})); struct _tagged_string _temp164= Cyc_Absynpp_typ2string(
t); xprintf("returns value of type %.*s but requires %.*s", _temp163.last_plus_one
- _temp163.curr, _temp163.curr, _temp164.last_plus_one - _temp164.curr, _temp164.curr);}));}{
void* _temp165= Cyc_Tcutil_compress( t); _LL167: if( _temp165 ==( void*) Cyc_Absyn_VoidType){
goto _LL168;} else{ goto _LL169;} _LL169: goto _LL170; _LL168: Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp171=( char*)"function returns void expression";
struct _tagged_string _temp172; _temp172.curr= _temp171; _temp172.base= _temp171;
_temp172.last_plus_one= _temp171 + 33; _temp172;})); goto _LL166; _LL170: goto
_LL166; _LL166:;}} return;} _LL22: Cyc_Tcexp_tcTest( te, _temp63,( struct
_tagged_string)({ char* _temp174=( char*)"if statement"; struct _tagged_string
_temp175; _temp175.curr= _temp174; _temp175.base= _temp174; _temp175.last_plus_one=
_temp174 + 13; _temp175;})); Cyc_Tcstmt_tcStmt( te, _temp61, 1); Cyc_Tcstmt_tcStmt(
te, _temp59, 1); return; _LL24: Cyc_Tcstmt_decorate_stmt( te, _temp69); Cyc_Tcexp_tcTest(
te, _temp71,( struct _tagged_string)({ char* _temp176=( char*)"while loop";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 11; _temp177;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp69), _temp65, 1); return; _LL26: Cyc_Tcstmt_decorate_stmt( te, _temp83);
Cyc_Tcstmt_decorate_stmt( te, _temp77); Cyc_Tcexp_tcExp( te, 0, _temp87); Cyc_Tcexp_tcTest(
te, _temp85,( struct _tagged_string)({ char* _temp178=( char*)"for loop"; struct
_tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 9; _temp179;})); te= Cyc_Tcenv_set_in_loop(
te, _temp77); Cyc_Tcstmt_tcStmt( te, _temp73, 1); Cyc_Tcexp_tcExp( te, 0,
_temp79); return; _LL28: Cyc_Tcstmt_decorate_stmt( te, _temp91); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp91), _temp95, 1); Cyc_Tcexp_tcTest( te, _temp93,(
struct _tagged_string)({ char* _temp180=( char*)"do loop"; struct _tagged_string
_temp181; _temp181.curr= _temp180; _temp181.base= _temp180; _temp181.last_plus_one=
_temp180 + 8; _temp181;})); return; _LL30: Cyc_Tcenv_process_break( te, s0,
_temp99); return; _LL32: Cyc_Tcenv_process_continue( te, s0, _temp102); return;
_LL34: Cyc_Tcenv_process_goto( te, s0, _temp106, _temp105); return; _LL36: {
struct _tuple3* _temp182= Cyc_Tcenv_process_fallthru( te, s0, _temp110); goto
_LL183; _LL183: if( _temp182 == 0){ Cyc_Tcutil_terr( s0->loc,( struct
_tagged_string)({ char* _temp184=( char*)"fallthru not in a non-last case";
struct _tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 32; _temp185;})); return;}{ struct _tuple3
_temp189; struct Cyc_List_List* _temp190; struct Cyc_List_List* _temp192; struct
Cyc_Absyn_Switch_clause* _temp194; struct _tuple3* _temp187=( struct _tuple3*)({
struct _tuple3* _temp186= _temp182; if( _temp186 == 0){ _throw( Null_Exception);}
_temp186;}); _temp189=* _temp187; _LL195: _temp194= _temp189.f1; goto _LL193;
_LL193: _temp192= _temp189.f2; goto _LL191; _LL191: _temp190= _temp189.f3; goto
_LL188; _LL188: { struct Cyc_List_List* instantiation=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcutil_make_inst_var, _temp192); _temp190=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_List_List*, void*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcutil_substitute, instantiation, _temp190); for( 0;
_temp190 != 0? _temp111 != 0: 0; _temp190=({ struct Cyc_List_List* _temp196=
_temp190; if( _temp196 == 0){ _throw( Null_Exception);} _temp196->tl;}),
_temp111=({ struct Cyc_List_List* _temp197= _temp111; if( _temp197 == 0){ _throw(
Null_Exception);} _temp197->tl;})){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp198= _temp111; if( _temp198 == 0){ _throw(
Null_Exception);} _temp198->hd;})); if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp199= _temp111; if( _temp199 == 0){ _throw(
Null_Exception);} _temp199->hd;}),( void*)({ struct Cyc_List_List* _temp200=
_temp190; if( _temp200 == 0){ _throw( Null_Exception);} _temp200->hd;}))){ Cyc_Tcutil_terr(
s0->loc,({ struct _tagged_string _temp204= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp202=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp201= _temp111; if( _temp201 == 0){ _throw( Null_Exception);} _temp201->hd;}))->topt;
if( _temp202 == 0){ _throw( Null_Exception);} _temp202->v;})); struct
_tagged_string _temp205= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp203= _temp190; if( _temp203 == 0){ _throw( Null_Exception);} _temp203->hd;}));
xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp204.last_plus_one - _temp204.curr, _temp204.curr, _temp205.last_plus_one -
_temp205.curr, _temp205.curr);}));}} if( _temp111 != 0){ Cyc_Tcutil_terr( s0->loc,(
struct _tagged_string)({ char* _temp206=( char*)"too many arguments to explicit fallthru";
struct _tagged_string _temp207; _temp207.curr= _temp206; _temp207.base= _temp206;
_temp207.last_plus_one= _temp206 + 40; _temp207;}));} if( _temp190 != 0){ Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp208=( char*)"too few arguments to explicit fallthru";
struct _tagged_string _temp209; _temp209.curr= _temp208; _temp209.base= _temp208;
_temp209.last_plus_one= _temp208 + 39; _temp209;}));} return;}}} _LL38: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label( te, _temp115, _temp113),({
struct Cyc_Absyn_Tvar* _temp210=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp210->name=({ struct _tagged_string* _temp211=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp211[ 0]=({
struct _tagged_string _temp212=* _temp115; xprintf("`%.*s", _temp212.last_plus_one
- _temp212.curr, _temp212.curr);}); _temp211;}); _temp210->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp210;})),
_temp113, 0); return; _LL40: Cyc_Tcexp_tcExp( te, 0, _temp119);{ void* _temp214=(
void*)({ struct Cyc_Core_Opt* _temp213= _temp119->topt; if( _temp213 == 0){
_throw( Null_Exception);} _temp213->v;}); goto _LL215; _LL215: te= Cyc_Tcenv_set_in_switch(
te); te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp117); for( 0; scs != 0; scs=({
struct Cyc_List_List* _temp216= scs; if( _temp216 == 0){ _throw( Null_Exception);}
_temp216->tl;})){ struct Cyc_Absyn_Pat* _temp218=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp217= scs; if( _temp217 == 0){ _throw( Null_Exception);}
_temp217->hd;}))->pattern; goto _LL219; _LL219: { struct Cyc_List_List* _temp222;
struct Cyc_List_List* _temp224; struct _tuple5 _temp220= Cyc_Tcpat_tcPat( te,
_temp218,( void**)& _temp214, 0); _LL225: _temp224= _temp220.f1; goto _LL223;
_LL223: _temp222= _temp220.f2; goto _LL221; _LL221: if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp226= _temp218->topt; if( _temp226 == 0){
_throw( Null_Exception);} _temp226->v;}), _temp214)){ Cyc_Tcutil_terr((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp227= scs; if( _temp227 ==
0){ _throw( Null_Exception);} _temp227->hd;}))->loc,({ struct _tagged_string
_temp229= Cyc_Absynpp_typ2string( _temp214); struct _tagged_string _temp230= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp228= _temp218->topt; if( _temp228 == 0){
_throw( Null_Exception);} _temp228->v;})); xprintf("switch on type %.*s, but case expects type %.*s",
_temp229.last_plus_one - _temp229.curr, _temp229.curr, _temp230.last_plus_one -
_temp230.curr, _temp230.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp218);}(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp231=
scs; if( _temp231 == 0){ _throw( Null_Exception);} _temp231->hd;}))->pat_vars=({
struct Cyc_Core_Opt* _temp232=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp232->v=( void*) _temp222; _temp232;}); Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp233= scs; if(
_temp233 == 0){ _throw( Null_Exception);} _temp233->hd;}))->loc, te, _temp224,
_temp222,(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp234=
scs; if( _temp234 == 0){ _throw( Null_Exception);} _temp234->hd;}))->body,((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp235= scs; if(
_temp235 == 0){ _throw( Null_Exception);} _temp235->hd;}))->where_clause, 1); te=
Cyc_Tcenv_set_fallthru( te, _temp224, _temp222,( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp236= scs; if( _temp236 == 0){ _throw( Null_Exception);}
_temp236->hd;}));}}} Cyc_Tcpat_check_switch_exhaustive( s0->loc, _temp117);
return;} _LL42:( void*)( _temp123->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt((
void*) _temp123->r, _temp123->loc), Cyc_Absyn_skip_stmt( _temp123->loc),
_temp123->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_enter_try(
te), s0), _temp123, 1); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* _temp237=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp121); goto _LL238; _LL238: for( 0; _temp237 != 0;
_temp237=({ struct Cyc_List_List* _temp239= _temp237; if( _temp239 == 0){ _throw(
Null_Exception);} _temp239->tl;})){ struct Cyc_Absyn_Pat* _temp241=(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp240= _temp237; if( _temp240 == 0){ _throw(
Null_Exception);} _temp240->hd;}))->pattern; goto _LL242; _LL242: { struct Cyc_List_List*
_temp245; struct Cyc_List_List* _temp247; struct _tuple5 _temp243= Cyc_Tcpat_tcPat(
te, _temp241,( void**)& Cyc_Absyn_exn_typ, 0); _LL248: _temp247= _temp243.f1;
goto _LL246; _LL246: _temp245= _temp243.f2; goto _LL244; _LL244: if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp249= _temp241->topt; if( _temp249 == 0){
_throw( Null_Exception);} _temp249->v;}), Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp250= _temp237; if(
_temp250 == 0){ _throw( Null_Exception);} _temp250->hd;}))->loc,({ struct
_tagged_string _temp252= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp251= _temp241->topt; if( _temp251 == 0){ _throw( Null_Exception);} _temp251->v;}));
xprintf("expected xtunion exn but found %.*s", _temp252.last_plus_one - _temp252.curr,
_temp252.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te, _temp241);}(( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp253= _temp237; if(
_temp253 == 0){ _throw( Null_Exception);} _temp253->hd;}))->pat_vars=({ struct
Cyc_Core_Opt* _temp254=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp254->v=( void*) _temp245; _temp254;}); Cyc_Tcstmt_pattern_synth((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp255= _temp237; if(
_temp255 == 0){ _throw( Null_Exception);} _temp255->hd;}))->loc, te, _temp247,
_temp245,(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp256=
_temp237; if( _temp256 == 0){ _throw( Null_Exception);} _temp256->hd;}))->body,((
struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp257= _temp237; if(
_temp257 == 0){ _throw( Null_Exception);} _temp257->hd;}))->where_clause, 1); te=
Cyc_Tcenv_set_fallthru( te, _temp247, _temp245,( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp258= _temp237; if( _temp258 == 0){ _throw(
Null_Exception);} _temp258->hd;}));}}} Cyc_Tcpat_check_catch_overlap( s0->loc,
_temp121); return; _LL44: { struct _tagged_string unimp_msg_part; if( new_block){
te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp259=( void*) _temp127->r;
struct Cyc_Absyn_Vardecl* _temp283; int _temp285; int* _temp287; struct Cyc_Absyn_Exp*
_temp288; struct Cyc_Core_Opt* _temp290; struct Cyc_Core_Opt** _temp292; struct
Cyc_Core_Opt* _temp293; struct Cyc_Core_Opt** _temp295; struct Cyc_Absyn_Pat*
_temp296; struct Cyc_List_List* _temp298; struct _tagged_string* _temp300;
struct Cyc_List_List* _temp302; struct _tuple0* _temp304; struct Cyc_Absyn_Fndecl*
_temp306; struct Cyc_Absyn_Structdecl* _temp308; struct Cyc_Absyn_Uniondecl*
_temp310; struct Cyc_Absyn_Tuniondecl* _temp312; struct Cyc_Absyn_Enumdecl*
_temp314; struct Cyc_Absyn_Typedefdecl* _temp316; struct Cyc_List_List* _temp318;
_LL261: if(*(( int*) _temp259) == Cyc_Absyn_Var_d){ _LL284: _temp283=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp259)->f1; goto _LL262;}
else{ goto _LL263;} _LL263: if(*(( int*) _temp259) == Cyc_Absyn_Let_d){ _LL297:
_temp296=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp259)->f1;
goto _LL294; _LL294: _temp293=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp259)->f2; _temp295=&(( struct Cyc_Absyn_Let_d_struct*) _temp259)->f2; goto
_LL291; _LL291: _temp290=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp259)->f3; _temp292=&(( struct Cyc_Absyn_Let_d_struct*) _temp259)->f3; goto
_LL289; _LL289: _temp288=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp259)->f4; goto _LL286; _LL286: _temp285=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp259)->f5; _temp287=&(( struct Cyc_Absyn_Let_d_struct*) _temp259)->f5; goto
_LL264;} else{ goto _LL265;} _LL265: if(*(( int*) _temp259) == Cyc_Absyn_Namespace_d){
_LL301: _temp300=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp259)->f1; goto _LL299; _LL299: _temp298=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp259)->f2; goto _LL266;} else{ goto _LL267;}
_LL267: if(*(( int*) _temp259) == Cyc_Absyn_Using_d){ _LL305: _temp304=( struct
_tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp259)->f1; goto _LL303; _LL303:
_temp302=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp259)->f2;
goto _LL268;} else{ goto _LL269;} _LL269: if(*(( int*) _temp259) == Cyc_Absyn_Fn_d){
_LL307: _temp306=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp259)->f1; goto _LL270;} else{ goto _LL271;} _LL271: if(*(( int*) _temp259)
== Cyc_Absyn_Struct_d){ _LL309: _temp308=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp259)->f1; goto _LL272;} else{ goto
_LL273;} _LL273: if(*(( int*) _temp259) == Cyc_Absyn_Union_d){ _LL311: _temp310=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp259)->f1;
goto _LL274;} else{ goto _LL275;} _LL275: if(*(( int*) _temp259) == Cyc_Absyn_Tunion_d){
_LL313: _temp312=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp259)->f1; goto _LL276;} else{ goto _LL277;} _LL277: if(*(( int*) _temp259)
== Cyc_Absyn_Enum_d){ _LL315: _temp314=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp259)->f1; goto _LL278;} else{ goto _LL279;}
_LL279: if(*(( int*) _temp259) == Cyc_Absyn_Typedef_d){ _LL317: _temp316=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp259)->f1;
goto _LL280;} else{ goto _LL281;} _LL281: if(*(( int*) _temp259) == Cyc_Absyn_ExternC_d){
_LL319: _temp318=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp259)->f1; goto _LL282;} else{ goto _LL260;} _LL262: { struct Cyc_Absyn_Vardecl
_temp322; struct Cyc_List_List* _temp323; struct Cyc_Core_Opt* _temp325; struct
Cyc_Core_Opt** _temp327; struct Cyc_Absyn_Exp* _temp328; void* _temp330; struct
Cyc_Absyn_Tqual _temp332; struct _tuple0* _temp334; struct _tuple0 _temp336;
struct _tagged_string* _temp337; void* _temp339; void* _temp341; struct Cyc_Absyn_Vardecl*
_temp320= _temp283; _temp322=* _temp320; _LL342: _temp341=( void*) _temp322.sc;
goto _LL335; _LL335: _temp334=( struct _tuple0*) _temp322.name; _temp336=*
_temp334; _LL340: _temp339= _temp336.f1; goto _LL338; _LL338: _temp337= _temp336.f2;
goto _LL333; _LL333: _temp332=( struct Cyc_Absyn_Tqual) _temp322.tq; goto _LL331;
_LL331: _temp330=( void*) _temp322.type; goto _LL329; _LL329: _temp328=( struct
Cyc_Absyn_Exp*) _temp322.initializer; goto _LL326; _LL326: _temp325=( struct Cyc_Core_Opt*)
_temp322.rgn; _temp327=&(* _temp320).rgn; goto _LL324; _LL324: _temp323=( struct
Cyc_List_List*) _temp322.attributes; goto _LL321; _LL321: { void* _temp343= Cyc_Tcenv_curr_rgn(
te); goto _LL344; _LL344:* _temp327=({ struct Cyc_Core_Opt* _temp345=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp345->v=( void*)
_temp343; _temp345;});{ void* _temp346= _temp339; struct Cyc_List_List* _temp356;
struct Cyc_List_List* _temp358; _LL348: if( _temp346 ==( void*) Cyc_Absyn_Loc_n){
goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int) _temp346 > 1u?*((
int*) _temp346) == Cyc_Absyn_Rel_n: 0){ _LL357: _temp356=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp346)->f1; if( _temp356 == 0){ goto _LL351;}
else{ goto _LL352;}} else{ goto _LL352;} _LL352: if(( unsigned int) _temp346 > 1u?*((
int*) _temp346) == Cyc_Absyn_Abs_n: 0){ _LL359: _temp358=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp346)->f1; goto _LL353;} else{ goto _LL354;}
_LL354: goto _LL355; _LL349: goto _LL347; _LL351:(* _temp283->name).f1=( void*)
Cyc_Absyn_Loc_n; goto _LL347; _LL353:( void) _throw((( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp360=(
char*)"tcstmt: Abs_n declaration"; struct _tagged_string _temp361; _temp361.curr=
_temp360; _temp361.base= _temp360; _temp361.last_plus_one= _temp360 + 26;
_temp361;}))); _LL355: Cyc_Tcutil_terr( _temp127->loc,( struct _tagged_string)({
char* _temp362=( char*)"cannot declare a qualified local variable"; struct
_tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 42; _temp363;})); goto _LL347; _LL347:;}{
void* _temp364= Cyc_Tcutil_compress( _temp330); struct Cyc_Absyn_Exp* _temp371;
struct Cyc_Absyn_Tqual _temp373; void* _temp375; _LL366: if(( unsigned int)
_temp364 > 4u?*(( int*) _temp364) == Cyc_Absyn_ArrayType: 0){ _LL376: _temp375=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp364)->f1; goto _LL374; _LL374:
_temp373=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp364)->f2; goto _LL372; _LL372: _temp371=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp364)->f3; if( _temp371 == 0){ goto _LL370;}
else{ goto _LL368;}} else{ goto _LL368;} _LL370: if( _temp283->initializer != 0){
goto _LL367;} else{ goto _LL368;} _LL368: goto _LL369; _LL367:{ void* _temp378=(
void*)({ struct Cyc_Absyn_Exp* _temp377= _temp283->initializer; if( _temp377 ==
0){ _throw( Null_Exception);} _temp377->r;}); void* _temp388; struct
_tagged_string _temp390; struct Cyc_List_List* _temp392; struct Cyc_Core_Opt*
_temp394; struct Cyc_List_List* _temp396; _LL380: if(*(( int*) _temp378) == Cyc_Absyn_Const_e){
_LL389: _temp388=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp378)->f1; if((
unsigned int) _temp388 > 1u?*(( int*) _temp388) == Cyc_Absyn_String_c: 0){
_LL391: _temp390=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp388)->f1; goto _LL381;} else{ goto _LL382;}} else{ goto _LL382;} _LL382:
if(*(( int*) _temp378) == Cyc_Absyn_UnresolvedMem_e){ _LL395: _temp394=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp378)->f1; goto
_LL393; _LL393: _temp392=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp378)->f2; goto _LL383;} else{ goto _LL384;} _LL384: if(*(( int*) _temp378)
== Cyc_Absyn_Array_e){ _LL397: _temp396=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp378)->f1; goto _LL385;} else{ goto _LL386;} _LL386: goto _LL387; _LL381:
_temp330=( void*)( _temp283->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp398=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp398[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp399; _temp399.tag= Cyc_Absyn_ArrayType;
_temp399.f1=( void*) _temp375; _temp399.f2= _temp373; _temp399.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp400= _temp390;( unsigned int)(
_temp400.last_plus_one - _temp400.curr);}), 0); _temp399;}); _temp398;}))); goto
_LL379; _LL383: _temp396= _temp392; goto _LL385; _LL385: _temp330=( void*)(
_temp283->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp401=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp401[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp402; _temp402.tag= Cyc_Absyn_ArrayType;
_temp402.f1=( void*) _temp375; _temp402.f2= _temp373; _temp402.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp396), 0); _temp402;}); _temp401;}))); goto _LL379; _LL387: goto _LL379;
_LL379:;} goto _LL365; _LL369: goto _LL365; _LL365:;} Cyc_Tcutil_check_type( s0->loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, _temp330);{
struct Cyc_Tcenv_Tenv* _temp403= Cyc_Tcenv_add_local_var( _temp125->loc, te,
_temp283); goto _LL404; _LL404: if( _temp328 != 0){ Cyc_Tcexp_tcExpInitializer(
_temp403,( void**)& _temp330,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp405= _temp328; if( _temp405 == 0){ _throw( Null_Exception);} _temp405;}));
if( ! Cyc_Tcutil_coerce_assign( _temp403,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp406= _temp328; if( _temp406 == 0){ _throw( Null_Exception);} _temp406;}),
_temp330)){ Cyc_Tcutil_terr( _temp127->loc,({ struct _tagged_string _temp409=*
_temp337; struct _tagged_string _temp410= Cyc_Absynpp_typ2string( _temp330);
struct _tagged_string _temp411= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp408=({ struct Cyc_Absyn_Exp* _temp407= _temp328; if( _temp407 == 0){ _throw(
Null_Exception);} _temp407->topt;}); if( _temp408 == 0){ _throw( Null_Exception);}
_temp408->v;})); xprintf("%.*s declared with type %.*s, initialized with type %.*s",
_temp409.last_plus_one - _temp409.curr, _temp409.curr, _temp410.last_plus_one -
_temp410.curr, _temp410.curr, _temp411.last_plus_one - _temp411.curr, _temp411.curr);}));}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( _temp403, s0), _temp125, 0); return;}}}
_LL264: { struct Cyc_Core_Opt* _temp412=* _temp292; goto _LL413; _LL413: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp288);* _temp292= _temp288->topt;{ struct Cyc_List_List* _temp417;
struct Cyc_List_List* _temp419; struct _tuple5 _temp415= Cyc_Tcpat_tcPat( te,
_temp296,( void**)(( void**)(( void*)({ struct Cyc_Core_Opt* _temp414= _temp288->topt;
if( _temp414 == 0){ _throw( Null_Exception);}& _temp414->v;}))), 0); _LL420:
_temp419= _temp415.f1; goto _LL418; _LL418: _temp417= _temp415.f2; goto _LL416;
_LL416:* _temp295=({ struct Cyc_Core_Opt* _temp421=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp421->v=( void*) _temp417;
_temp421;}); if( _temp412 != 0? ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp422= _temp296->topt; if( _temp422 == 0){ _throw( Null_Exception);} _temp422->v;}),(
void*)({ struct Cyc_Core_Opt* _temp423= _temp412; if( _temp423 == 0){ _throw(
Null_Exception);} _temp423->v;})): 0){ Cyc_Tcutil_terr( _temp127->loc,( struct
_tagged_string)({ char* _temp424=( char*)"type of pattern does not match declared type";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 45; _temp425;}));} if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp426= _temp296->topt; if( _temp426 == 0){
_throw( Null_Exception);} _temp426->v;}),( void*)({ struct Cyc_Core_Opt*
_temp427= _temp288->topt; if( _temp427 == 0){ _throw( Null_Exception);} _temp427->v;}))){
Cyc_Tcutil_terr( _temp127->loc,({ struct _tagged_string _temp430= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp428= _temp296->topt; if( _temp428 == 0){
_throw( Null_Exception);} _temp428->v;})); struct _tagged_string _temp431= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp429= _temp288->topt; if( _temp429 == 0){
_throw( Null_Exception);} _temp429->v;})); xprintf("pattern type %.*s does not match definition type %.*s",
_temp430.last_plus_one - _temp430.curr, _temp430.curr, _temp431.last_plus_one -
_temp431.curr, _temp431.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp296);}* _temp287= Cyc_Tcpat_check_let_pat_exhaustive( _temp296->loc,
_temp296); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp419, _temp417, _temp125,
0, 0); return;}} _LL266: unimp_msg_part=( struct _tagged_string)({ char*
_temp432=( char*)"namespace"; struct _tagged_string _temp433; _temp433.curr=
_temp432; _temp433.base= _temp432; _temp433.last_plus_one= _temp432 + 10;
_temp433;}); goto _LL260; _LL268: unimp_msg_part=( struct _tagged_string)({ char*
_temp434=( char*)"using"; struct _tagged_string _temp435; _temp435.curr=
_temp434; _temp435.base= _temp434; _temp435.last_plus_one= _temp434 + 6;
_temp435;}); goto _LL260; _LL270: unimp_msg_part=( struct _tagged_string)({ char*
_temp436=( char*)"function"; struct _tagged_string _temp437; _temp437.curr=
_temp436; _temp437.base= _temp436; _temp437.last_plus_one= _temp436 + 9;
_temp437;}); goto _LL260; _LL272: unimp_msg_part=( struct _tagged_string)({ char*
_temp438=( char*)"struct"; struct _tagged_string _temp439; _temp439.curr=
_temp438; _temp439.base= _temp438; _temp439.last_plus_one= _temp438 + 7;
_temp439;}); goto _LL260; _LL274: unimp_msg_part=( struct _tagged_string)({ char*
_temp440=( char*)"union"; struct _tagged_string _temp441; _temp441.curr=
_temp440; _temp441.base= _temp440; _temp441.last_plus_one= _temp440 + 6;
_temp441;}); goto _LL260; _LL276: unimp_msg_part=( struct _tagged_string)({ char*
_temp442=( char*)"[x]tunion"; struct _tagged_string _temp443; _temp443.curr=
_temp442; _temp443.base= _temp442; _temp443.last_plus_one= _temp442 + 10;
_temp443;}); goto _LL260; _LL278: unimp_msg_part=( struct _tagged_string)({ char*
_temp444=( char*)"enum"; struct _tagged_string _temp445; _temp445.curr= _temp444;
_temp445.base= _temp444; _temp445.last_plus_one= _temp444 + 5; _temp445;}); goto
_LL260; _LL280: unimp_msg_part=( struct _tagged_string)({ char* _temp446=( char*)"typedef";
struct _tagged_string _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 8; _temp447;}); goto _LL260; _LL282:
unimp_msg_part=( struct _tagged_string)({ char* _temp448=( char*)"extern \"C\"";
struct _tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 11; _temp449;}); goto _LL260; _LL260:;}( void)
_throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp450= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp450.last_plus_one - _temp450.curr, _temp450.curr);})));} _LL46:( void*)(
_temp129->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt(( void*)
_temp129->r, _temp129->loc), Cyc_Absyn_skip_stmt( _temp129->loc), _temp129->loc))->r));
if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ struct Cyc_Absyn_Vardecl
_temp453; struct Cyc_List_List* _temp454; struct Cyc_Core_Opt* _temp456; struct
Cyc_Core_Opt** _temp458; struct Cyc_Absyn_Exp* _temp459; void* _temp461; struct
Cyc_Absyn_Tqual _temp463; struct _tuple0* _temp465; void* _temp467; struct Cyc_Absyn_Vardecl*
_temp451= _temp131; _temp453=* _temp451; _LL468: _temp467=( void*) _temp453.sc;
goto _LL466; _LL466: _temp465=( struct _tuple0*) _temp453.name; goto _LL464;
_LL464: _temp463=( struct Cyc_Absyn_Tqual) _temp453.tq; goto _LL462; _LL462:
_temp461=( void*) _temp453.type; goto _LL460; _LL460: _temp459=( struct Cyc_Absyn_Exp*)
_temp453.initializer; goto _LL457; _LL457: _temp456=( struct Cyc_Core_Opt*)
_temp453.rgn; _temp458=&(* _temp451).rgn; goto _LL455; _LL455: _temp454=( struct
Cyc_List_List*) _temp453.attributes; goto _LL452; _LL452: { void* _temp469= Cyc_Tcenv_curr_rgn(
te); goto _LL470; _LL470: { void* rgn_typ=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp477=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp477[ 0]=({ struct Cyc_Absyn_VarType_struct _temp478; _temp478.tag= Cyc_Absyn_VarType;
_temp478.f1= _temp133; _temp478;}); _temp477;});* _temp458=({ struct Cyc_Core_Opt*
_temp471=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp471->v=( void*) _temp469; _temp471;}); te= Cyc_Tcenv_add_type_vars( s0->loc,
te,({ struct Cyc_List_List* _temp472=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp472->hd=( void*) _temp133; _temp472->tl= 0;
_temp472;})); Cyc_Tcutil_check_type( s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_BoxKind, _temp461); te= Cyc_Tcenv_add_region( te, rgn_typ); if(
! Cyc_Tcutil_unify( _temp461,( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp473=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp473[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp474; _temp474.tag=
Cyc_Absyn_RgnHandleType; _temp474.f1=( void*) rgn_typ; _temp474;}); _temp473;}))){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp475=( char*)"region stmt: type of region handle is wrong!"; struct
_tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 45; _temp476;}));} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_add_local_var( _temp129->loc, Cyc_Tcenv_enter_try( te), _temp131), s0),
_temp129, 0); return;}}} _LL48:( void) _throw((( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp479=( char*)"tcStmt: cut is not implemented";
struct _tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 31; _temp480;}))); _LL50:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp481=( char*)"tcStmt: splice is not implemented"; struct
_tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 34; _temp482;}))); _LL12:;}}