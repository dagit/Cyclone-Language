#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
17u]; struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base;
struct Cyc_Absyn_Tunionfield* field; } ; typedef struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_xtunionfielddecl_t; extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_string* msg0, struct _tagged_string msg1); struct _tuple3{
void* f1; int f2; } ; extern struct _tuple3 Cyc_Tcdecl_merge_scope( void* s0,
void* s1, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
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
extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f, int* res, struct
_tagged_string* v, struct Cyc_Position_Segment* loc, struct _tagged_string* msg);
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
sc); char Cyc_Tcdecl_Incompatible[ 17u]="\000\000\000\000Incompatible"; void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment* loc, struct _tagged_string* msg1, struct
_tagged_string msg2){ if( msg1 == 0){( void) _throw(( void*) Cyc_Tcdecl_Incompatible);}
else{ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1=({ struct
_tagged_string* _temp0= msg1; if( _temp0 == 0){ _throw( Null_Exception);}*
_temp0;}); struct _tagged_string _temp2= msg2; xprintf("%.*s %.*s", _temp1.last_plus_one
- _temp1.curr, _temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr);}));}}
static void Cyc_Tcdecl_merge_scope_err( void* s0, void* s1, struct
_tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment* loc,
struct _tagged_string* msg){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp3= t; struct _tagged_string _temp4= v; struct _tagged_string _temp5= Cyc_Absynpp_scope2string(
s1); struct _tagged_string _temp6= Cyc_Absynpp_scope2string( s0); xprintf("%.*s %.*s is %.*s whereas expected scope is %.*s",
_temp3.last_plus_one - _temp3.curr, _temp3.curr, _temp4.last_plus_one - _temp4.curr,
_temp4.curr, _temp5.last_plus_one - _temp5.curr, _temp5.curr, _temp6.last_plus_one
- _temp6.curr, _temp6.curr);}));} struct _tuple4{ void* f1; void* f2; } ; struct
_tuple3 Cyc_Tcdecl_merge_scope( void* s0, void* s1, struct _tagged_string t,
struct _tagged_string v, struct Cyc_Position_Segment* loc, struct _tagged_string*
msg){{ struct _tuple4 _temp8=({ struct _tuple4 _temp7; _temp7.f1= s0; _temp7.f2=
s1; _temp7;}); void* _temp28; void* _temp30; void* _temp32; void* _temp34; void*
_temp36; void* _temp38; void* _temp40; void* _temp42; void* _temp44; void*
_temp46; void* _temp48; void* _temp50; void* _temp52; void* _temp54; void*
_temp56; void* _temp58; _LL10: _LL31: _temp30= _temp8.f1; if( _temp30 ==( void*)
Cyc_Absyn_ExternC){ goto _LL29;} else{ goto _LL12;} _LL29: _temp28= _temp8.f2;
if( _temp28 ==( void*) Cyc_Absyn_ExternC){ goto _LL11;} else{ goto _LL12;} _LL12:
_LL35: _temp34= _temp8.f1; if( _temp34 ==( void*) Cyc_Absyn_ExternC){ goto _LL33;}
else{ goto _LL14;} _LL33: _temp32= _temp8.f2; goto _LL13; _LL14: _LL39: _temp38=
_temp8.f1; goto _LL37; _LL37: _temp36= _temp8.f2; if( _temp36 ==( void*) Cyc_Absyn_ExternC){
goto _LL15;} else{ goto _LL16;} _LL16: _LL43: _temp42= _temp8.f1; goto _LL41;
_LL41: _temp40= _temp8.f2; if( _temp40 ==( void*) Cyc_Absyn_Extern){ goto _LL17;}
else{ goto _LL18;} _LL18: _LL47: _temp46= _temp8.f1; if( _temp46 ==( void*) Cyc_Absyn_Extern){
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
return 0;}{ struct Cyc_List_List* x0= tvs0; struct Cyc_List_List* x1= tvs1; for(
0; x0 != 0; x0=({ struct Cyc_List_List* _temp81= x0; if( _temp81 == 0){ _throw(
Null_Exception);} _temp81->tl;}), x1=({ struct Cyc_List_List* _temp82= x1; if(
_temp82 == 0){ _throw( Null_Exception);} _temp82->tl;})){ struct Cyc_Absyn_Conref*
c0=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp90= x0; if( _temp90 == 0){
_throw( Null_Exception);} _temp90->hd;}))->kind); struct Cyc_Absyn_Conref* c1=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp89= x1; if( _temp89 == 0){
_throw( Null_Exception);} _temp89->hd;}))->kind); if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c0) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp84= t; struct
_tagged_string _temp85= v; struct _tagged_string _temp86= Cyc_Absynpp_ckind2string(
c0); struct _tagged_string _temp87=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp83= x0; if( _temp83 == 0){ _throw( Null_Exception);} _temp83->hd;}))->name;
struct _tagged_string _temp88= Cyc_Absynpp_ckind2string( c1); xprintf("%.*s %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp84.last_plus_one - _temp84.curr, _temp84.curr, _temp85.last_plus_one -
_temp85.curr, _temp85.curr, _temp86.last_plus_one - _temp86.curr, _temp86.curr,
_temp87.last_plus_one - _temp87.curr, _temp87.curr, _temp88.last_plus_one -
_temp88.curr, _temp88.curr);})); return 0;}} return 1;}} inline static int Cyc_Tcdecl_check_atts(
struct Cyc_List_List* atts0, struct Cyc_List_List* atts1, struct _tagged_string
t, struct _tagged_string v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ if( ! Cyc_Tcutil_same_atts( atts0, atts1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp91= t; struct _tagged_string _temp92= v;
xprintf("%.*s %.*s has different attributes", _temp91.last_plus_one - _temp91.curr,
_temp91.curr, _temp92.last_plus_one - _temp92.curr, _temp92.curr);})); return 0;}
else{ return 1;}} struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
inline static struct Cyc_List_List* Cyc_Tcdecl_build_tvs_map( struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1){ struct Cyc_List_List* inst= 0; for( 0; tvs0
!= 0; tvs0=({ struct Cyc_List_List* _temp93= tvs0; if( _temp93 == 0){ _throw(
Null_Exception);} _temp93->tl;}), tvs1=({ struct Cyc_List_List* _temp94= tvs1;
if( _temp94 == 0){ _throw( Null_Exception);} _temp94->tl;})){ inst=({ struct Cyc_List_List*
_temp95=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp95->hd=( void*)({ struct _tuple5* _temp96=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp96->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp100= tvs1; if( _temp100 == 0){ _throw( Null_Exception);} _temp100->hd;});
_temp96->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp97=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp97[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp98; _temp98.tag= Cyc_Absyn_VarType; _temp98.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp99= tvs0; if( _temp99 == 0){ _throw( Null_Exception);}
_temp99->hd;}); _temp98;}); _temp97;}); _temp96;}); _temp95->tl= inst; _temp95;});}
return inst;} struct _tuple6{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; static struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_struct_or_union_decl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct
_tagged_string t, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){
struct _tagged_string v= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp193= d0->name; if( _temp193 == 0){ _throw( Null_Exception);} _temp193->v;}));
int res= 1; if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs, t, v, loc, msg)){
return 0;}{ int _temp103; void* _temp105; struct _tuple3 _temp101= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc, t, v, loc, msg); _LL106: _temp105= _temp101.f1;
goto _LL104; _LL104: _temp103= _temp101.f2; goto _LL102; _LL102: if( ! _temp103){
res= 0;} if( ! Cyc_Tcdecl_check_atts( d0->attributes, d1->attributes, t, v, loc,
msg)){ res= 0;}{ struct Cyc_Absyn_Structdecl* d2;{ struct _tuple6 _temp108=({
struct _tuple6 _temp107; _temp107.f1= d0->fields; _temp107.f2= d1->fields;
_temp107;}); struct Cyc_Core_Opt* _temp116; struct Cyc_Core_Opt* _temp118;
struct Cyc_Core_Opt* _temp120; struct Cyc_Core_Opt* _temp122; struct Cyc_Core_Opt*
_temp124; struct Cyc_Core_Opt _temp126; struct Cyc_List_List* _temp127; struct
Cyc_Core_Opt* _temp129; struct Cyc_Core_Opt _temp131; struct Cyc_List_List*
_temp132; _LL110: _LL119: _temp118= _temp108.f1; goto _LL117; _LL117: _temp116=
_temp108.f2; if( _temp116 == 0){ goto _LL111;} else{ goto _LL112;} _LL112:
_LL123: _temp122= _temp108.f1; if( _temp122 == 0){ goto _LL121;} else{ goto
_LL114;} _LL121: _temp120= _temp108.f2; goto _LL113; _LL114: _LL130: _temp129=
_temp108.f1; if( _temp129 == 0){ goto _LL109;} else{ _temp131=* _temp129; _LL133:
_temp132=( struct Cyc_List_List*) _temp131.v; goto _LL125;} _LL125: _temp124=
_temp108.f2; if( _temp124 == 0){ goto _LL109;} else{ _temp126=* _temp124; _LL128:
_temp127=( struct Cyc_List_List*) _temp126.v; goto _LL115;} _LL111: d2= d0; goto
_LL109; _LL113: d2= d1; goto _LL109; _LL115: { struct Cyc_List_List* inst= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); for( 0; _temp132 != 0? _temp127 != 0: 0; _temp132=({ struct
Cyc_List_List* _temp134= _temp132; if( _temp134 == 0){ _throw( Null_Exception);}
_temp134->tl;}), _temp127=({ struct Cyc_List_List* _temp135= _temp127; if(
_temp135 == 0){ _throw( Null_Exception);} _temp135->tl;})){ struct Cyc_Absyn_Structfield
_temp139; struct Cyc_List_List* _temp140; struct Cyc_Core_Opt* _temp142; void*
_temp144; struct Cyc_Absyn_Tqual _temp146; struct _tagged_string* _temp148;
struct Cyc_Absyn_Structfield* _temp137=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp136= _temp132; if( _temp136 == 0){ _throw( Null_Exception);}
_temp136->hd;}); _temp139=* _temp137; _LL149: _temp148=( struct _tagged_string*)
_temp139.name; goto _LL147; _LL147: _temp146=( struct Cyc_Absyn_Tqual) _temp139.tq;
goto _LL145; _LL145: _temp144=( void*) _temp139.type; goto _LL143; _LL143:
_temp142=( struct Cyc_Core_Opt*) _temp139.width; goto _LL141; _LL141: _temp140=(
struct Cyc_List_List*) _temp139.attributes; goto _LL138; _LL138: { struct Cyc_Absyn_Structfield
_temp153; struct Cyc_List_List* _temp154; struct Cyc_Core_Opt* _temp156; void*
_temp158; struct Cyc_Absyn_Tqual _temp160; struct _tagged_string* _temp162;
struct Cyc_Absyn_Structfield* _temp151=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp150= _temp127; if( _temp150 == 0){ _throw( Null_Exception);}
_temp150->hd;}); _temp153=* _temp151; _LL163: _temp162=( struct _tagged_string*)
_temp153.name; goto _LL161; _LL161: _temp160=( struct Cyc_Absyn_Tqual) _temp153.tq;
goto _LL159; _LL159: _temp158=( void*) _temp153.type; goto _LL157; _LL157:
_temp156=( struct Cyc_Core_Opt*) _temp153.width; goto _LL155; _LL155: _temp154=(
struct Cyc_List_List*) _temp153.attributes; goto _LL152; _LL152: if( Cyc_String_zstrptrcmp(
_temp148, _temp162) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp164= t; struct _tagged_string _temp165= v; struct _tagged_string _temp166=*
_temp148; struct _tagged_string _temp167=* _temp162; xprintf("%.*s %.*s : field name mismatch %.*s != %.*s",
_temp164.last_plus_one - _temp164.curr, _temp164.curr, _temp165.last_plus_one -
_temp165.curr, _temp165.curr, _temp166.last_plus_one - _temp166.curr, _temp166.curr,
_temp167.last_plus_one - _temp167.curr, _temp167.curr);})); return 0;} if( ! Cyc_Tcutil_same_atts(
_temp140, _temp154)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp168= t; struct _tagged_string _temp169= v; struct _tagged_string _temp170=*
_temp148; xprintf("%.*s %.*s : attribute mismatch on field %.*s", _temp168.last_plus_one
- _temp168.curr, _temp168.curr, _temp169.last_plus_one - _temp169.curr, _temp169.curr,
_temp170.last_plus_one - _temp170.curr, _temp170.curr);})); res= 0;} if( ! Cyc_Tcutil_equal_tqual(
_temp146, _temp160)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp171= t; struct _tagged_string _temp172= v; struct _tagged_string _temp173=*
_temp148; xprintf("%.*s %.*s : qualifier mismatch on field %.*s", _temp171.last_plus_one
- _temp171.curr, _temp171.curr, _temp172.last_plus_one - _temp172.curr, _temp172.curr,
_temp173.last_plus_one - _temp173.curr, _temp173.curr);})); res= 0;} if((((
_temp142 != 0? _temp156 != 0: 0)? Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp174= _temp142; if( _temp174 == 0){ _throw(
Null_Exception);} _temp174->v;})) != Cyc_Tcdecl_get_uint_const_value(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp175= _temp156; if( _temp175 == 0){
_throw( Null_Exception);} _temp175->v;})): 0)? 1:( _temp142 == 0? _temp156 != 0:
0))? 1:( _temp142 != 0? _temp156 == 0: 0)){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp176= t; struct _tagged_string _temp177= v; struct
_tagged_string _temp178=* _temp148; xprintf("%.*s %.*s : bitfield mismatch on field %.*s",
_temp176.last_plus_one - _temp176.curr, _temp176.curr, _temp177.last_plus_one -
_temp177.curr, _temp177.curr, _temp178.last_plus_one - _temp178.curr, _temp178.curr);}));
res= 0;}{ void* subst_t1= Cyc_Tcutil_substitute( inst, _temp158); if( ! Cyc_Tcdecl_check_type(
_temp144, subst_t1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp179= t; struct _tagged_string _temp180= v; struct _tagged_string _temp181=*
_temp148; struct _tagged_string _temp182= Cyc_Absynpp_typ2string( _temp144);
struct _tagged_string _temp183= Cyc_Absynpp_typ2string( subst_t1); xprintf("%.*s %.*s : type mismatch on field %.*s: %.*s != %.*s",
_temp179.last_plus_one - _temp179.curr, _temp179.curr, _temp180.last_plus_one -
_temp180.curr, _temp180.curr, _temp181.last_plus_one - _temp181.curr, _temp181.curr,
_temp182.last_plus_one - _temp182.curr, _temp182.curr, _temp183.last_plus_one -
_temp183.curr, _temp183.curr);})); res= 0;}}}} if( _temp132 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp185= t; struct _tagged_string _temp186= v;
struct _tagged_string _temp187=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp184= _temp132; if( _temp184 == 0){ _throw( Null_Exception);} _temp184->hd;}))->name;
xprintf("%.*s %.*s is missing field %.*s", _temp185.last_plus_one - _temp185.curr,
_temp185.curr, _temp186.last_plus_one - _temp186.curr, _temp186.curr, _temp187.last_plus_one
- _temp187.curr, _temp187.curr);})); res= 0;} if( _temp127 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp189= t; struct _tagged_string _temp190= v;
struct _tagged_string _temp191=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp188= _temp127; if( _temp188 == 0){ _throw( Null_Exception);} _temp188->hd;}))->name;
xprintf("%.*s %.*s has extra field %.*s", _temp189.last_plus_one - _temp189.curr,
_temp189.curr, _temp190.last_plus_one - _temp190.curr, _temp190.curr, _temp191.last_plus_one
- _temp191.curr, _temp191.curr);})); res= 0;} d2= d0; goto _LL109;} _LL109:;}
if( ! res){ return 0;} if( _temp105 ==( void*) d2->sc){ return( struct Cyc_Absyn_Structdecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Structdecl* _temp192=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp192[ 0]=* d2; _temp192;});(
void*)( d2->sc=( void*) _temp105); return( struct Cyc_Absyn_Structdecl*) d2;}}}}
struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ return Cyc_Tcdecl_merge_struct_or_union_decl( d0, d1,(
struct _tagged_string)({ char* _temp194=( char*)"struct"; struct _tagged_string
_temp195; _temp195.curr= _temp194; _temp195.base= _temp194; _temp195.last_plus_one=
_temp194 + 7; _temp195;}), loc, msg);} struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ return( struct Cyc_Absyn_Uniondecl*) Cyc_Tcdecl_merge_struct_or_union_decl((
struct Cyc_Absyn_Structdecl*) d0,( struct Cyc_Absyn_Structdecl*) d1,( struct
_tagged_string)({ char* _temp196=( char*)"union"; struct _tagged_string _temp197;
_temp197.curr= _temp196; _temp197.base= _temp196; _temp197.last_plus_one=
_temp196 + 6; _temp197;}), loc, msg);} inline static struct _tagged_string Cyc_Tcdecl_is_x2string(
int is_x){ if( is_x){ return( struct _tagged_string)({ char* _temp198=( char*)"xtunion";
struct _tagged_string _temp199; _temp199.curr= _temp198; _temp199.base= _temp198;
_temp199.last_plus_one= _temp198 + 8; _temp199;});} else{ return( struct
_tagged_string)({ char* _temp200=( char*)"tunion"; struct _tagged_string
_temp201; _temp201.curr= _temp200; _temp201.base= _temp200; _temp201.last_plus_one=
_temp200 + 7; _temp201;});}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_merge_tunionfield( struct
Cyc_Absyn_Tunionfield* f0, struct Cyc_Absyn_Tunionfield* f1, struct Cyc_List_List*
inst, struct _tagged_string t, struct _tagged_string v, struct _tagged_string*
msg){ struct Cyc_Position_Segment* loc= f1->loc; if( Cyc_String_zstrptrcmp((* f0->name).f2,(*
f1->name).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp202=
t; struct _tagged_string _temp203= v; struct _tagged_string _temp204=*(* f1->name).f2;
struct _tagged_string _temp205=*(* f0->name).f2; xprintf("%.*s %.*s: field name mismatch %.*s != %.*s",
_temp202.last_plus_one - _temp202.curr, _temp202.curr, _temp203.last_plus_one -
_temp203.curr, _temp203.curr, _temp204.last_plus_one - _temp204.curr, _temp204.curr,
_temp205.last_plus_one - _temp205.curr, _temp205.curr);})); return 0;}{ struct
_tagged_string name=*(* f0->name).f2; int _temp210; void* _temp212; struct
_tuple3 _temp208= Cyc_Tcdecl_merge_scope(( void*) f0->sc,( void*) f1->sc,({
struct _tagged_string _temp206= t; struct _tagged_string _temp207= v; xprintf("in %.*s %.*s, field",
_temp206.last_plus_one - _temp206.curr, _temp206.curr, _temp207.last_plus_one -
_temp207.curr, _temp207.curr);}), name, loc, msg); _LL213: _temp212= _temp208.f1;
goto _LL211; _LL211: _temp210= _temp208.f2; goto _LL209; _LL209: { struct Cyc_List_List*
tvs0= f0->tvs; struct Cyc_List_List* tvs1= f1->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs0) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp214= t; struct
_tagged_string _temp215= v; struct _tagged_string _temp216= name; xprintf("%.*s %.*s, field %.*s: type parameter number mismatch",
_temp214.last_plus_one - _temp214.curr, _temp214.curr, _temp215.last_plus_one -
_temp215.curr, _temp215.curr, _temp216.last_plus_one - _temp216.curr, _temp216.curr);}));
return 0;}{ struct Cyc_List_List* inst2= Cyc_Tcdecl_build_tvs_map( tvs0, tvs1);
struct Cyc_List_List* typs0= f0->typs; struct Cyc_List_List* typs1= f1->typs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( typs0) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( typs1)){ Cyc_Tcdecl_merr( loc, msg,({
struct _tagged_string _temp217= t; struct _tagged_string _temp218= v; struct
_tagged_string _temp219= name; xprintf("%.*s %.*s, field %.*s: parameter number mismatch",
_temp217.last_plus_one - _temp217.curr, _temp217.curr, _temp218.last_plus_one -
_temp218.curr, _temp218.curr, _temp219.last_plus_one - _temp219.curr, _temp219.curr);}));
_temp210= 0;} for( 0; typs0 != 0; typs0=({ struct Cyc_List_List* _temp220= typs0;
if( _temp220 == 0){ _throw( Null_Exception);} _temp220->tl;}), typs1=({ struct
Cyc_List_List* _temp221= typs1; if( _temp221 == 0){ _throw( Null_Exception);}
_temp221->tl;})){ if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple7*)({ struct
Cyc_List_List* _temp222= typs0; if( _temp222 == 0){ _throw( Null_Exception);}
_temp222->hd;}))).f1,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp223=
typs1; if( _temp223 == 0){ _throw( Null_Exception);} _temp223->hd;}))).f1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp224= t; struct _tagged_string _temp225= v;
struct _tagged_string _temp226= name; xprintf("%.*s %.*s, field %.*s: parameter qualifier",
_temp224.last_plus_one - _temp224.curr, _temp224.curr, _temp225.last_plus_one -
_temp225.curr, _temp225.curr, _temp226.last_plus_one - _temp226.curr, _temp226.curr);}));
_temp210= 0;}{ void* subst_t1= Cyc_Tcutil_substitute((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( inst2, inst),(*((
struct _tuple7*)({ struct Cyc_List_List* _temp234= typs1; if( _temp234 == 0){
_throw( Null_Exception);} _temp234->hd;}))).f2); if( ! Cyc_Tcdecl_check_type((*((
struct _tuple7*)({ struct Cyc_List_List* _temp227= typs0; if( _temp227 == 0){
_throw( Null_Exception);} _temp227->hd;}))).f2, subst_t1)){ Cyc_Tcdecl_merr( loc,
msg,({ struct _tagged_string _temp229= t; struct _tagged_string _temp230= v;
struct _tagged_string _temp231= name; struct _tagged_string _temp232= Cyc_Absynpp_typ2string((*((
struct _tuple7*)({ struct Cyc_List_List* _temp228= typs0; if( _temp228 == 0){
_throw( Null_Exception);} _temp228->hd;}))).f2); struct _tagged_string _temp233=
Cyc_Absynpp_typ2string( subst_t1); xprintf("%.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp229.last_plus_one - _temp229.curr, _temp229.curr, _temp230.last_plus_one -
_temp230.curr, _temp230.curr, _temp231.last_plus_one - _temp231.curr, _temp231.curr,
_temp232.last_plus_one - _temp232.curr, _temp232.curr, _temp233.last_plus_one -
_temp233.curr, _temp233.curr);})); _temp210= 0;}}} if( ! _temp210){ return 0;}
if(( void*) f0->sc != _temp212){ struct Cyc_Absyn_Tunionfield* f2=({ struct Cyc_Absyn_Tunionfield*
_temp236=( struct Cyc_Absyn_Tunionfield*) GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield));
_temp236[ 0]=* f0; _temp236;});( void*)({ struct Cyc_Absyn_Tunionfield* _temp235=
f2; if( _temp235 == 0){ _throw( Null_Exception);} _temp235->sc=( void*) _temp212;});
return f2;} else{ return( struct Cyc_Absyn_Tunionfield*) f0;}}}}} static int Cyc_Tcdecl_substitute_tunionfield_f1(
struct _tagged_string* name, struct Cyc_Absyn_Tvar* x){ return Cyc_String_strptrcmp(
x->name, name) == 0;} static struct _tuple7* Cyc_Tcdecl_substitute_tunionfield_f2(
struct Cyc_List_List* inst, struct _tuple7* x){ struct _tuple7 _temp239; void*
_temp240; struct Cyc_Absyn_Tqual _temp242; struct _tuple7* _temp237= x; _temp239=*
_temp237; _LL243: _temp242= _temp239.f1; goto _LL241; _LL241: _temp240= _temp239.f2;
goto _LL238; _LL238: return({ struct _tuple7* _temp244=( struct _tuple7*)
GC_malloc( sizeof( struct _tuple7)); _temp244->f1= _temp242; _temp244->f2= Cyc_Tcutil_substitute(
inst, _temp240); _temp244;});} static struct Cyc_Absyn_Tunionfield* Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List* d0tvs, struct Cyc_List_List* d1tvs, struct Cyc_List_List*
inst1, struct Cyc_Absyn_Tunionfield* f1){ struct Cyc_Absyn_Tunionfield* f0=({
struct Cyc_Absyn_Tunionfield* _temp250=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp250[ 0]=* f1; _temp250;});
struct Cyc_List_List* tvs0= 0;{ struct Cyc_List_List* tvs= f1->tvs; for( 0; tvs
!= 0; tvs=({ struct Cyc_List_List* _temp245= tvs; if( _temp245 == 0){ _throw(
Null_Exception);} _temp245->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp249= tvs; if( _temp249 == 0){ _throw( Null_Exception);}
_temp249->hd;}); if((( int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*),
struct _tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*),
struct _tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)){ int i= 0; struct _tagged_string base=* tv->name; struct
_tagged_string* new_name; do { new_name=({ struct _tagged_string* _temp246=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp246[ 0]=
Cyc_String_strconcat( base, Cyc_Core_string_of_int( i)); _temp246;}); i ++;} while (((
int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*), struct
_tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
new_name, d0tvs)? 1:(( int(*)( int(* pred)( struct _tagged_string*, struct Cyc_Absyn_Tvar*),
struct _tagged_string* env, struct Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name, d1tvs)); tv=({ struct Cyc_Absyn_Tvar* _temp247=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp247[ 0]=* tv; _temp247;}); tv->name=
new_name;} tvs0=({ struct Cyc_List_List* _temp248=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp248->hd=( void*) tv; _temp248->tl=
tvs0; _temp248;});}} tvs0=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tvs0); f0->tvs= tvs0;{ struct Cyc_List_List* inst2= Cyc_Tcdecl_build_tvs_map(
tvs0, f1->tvs); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( inst2, inst1); f0->typs=((
struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_List_List*, struct
_tuple7*), struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcdecl_substitute_tunionfield_f2, inst, f1->typs); return f0;}} static
struct Cyc_List_List* Cyc_Tcdecl_merge_xtunion_fields( struct Cyc_List_List* f0s,
struct Cyc_List_List* f1s, struct Cyc_List_List* inst, struct Cyc_List_List*
tvs0, struct Cyc_List_List* tvs1, int* res, int* incl, struct _tagged_string t,
struct _tagged_string v, struct Cyc_Position_Segment* loc, struct _tagged_string*
msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List** _temp268=( struct
Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*)); _temp268[ 0]= 0;
_temp268;}); struct Cyc_List_List** next= f2sp; int cmp= - 1; for( 0; f0s != 0?
f1s != 0: 0; f1s=({ struct Cyc_List_List* _temp251= f1s; if( _temp251 == 0){
_throw( Null_Exception);} _temp251->tl;})){ while( f0s != 0?( cmp= Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp252= f0s; if(
_temp252 == 0){ _throw( Null_Exception);} _temp252->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp253= f1s; if( _temp253 == 0){ _throw( Null_Exception);}
_temp253->hd;}))->name)) < 0: 0) { struct Cyc_List_List* node=({ struct Cyc_List_List*
_temp256=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp256->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp257= f0s; if( _temp257 == 0){ _throw( Null_Exception);} _temp257->hd;}));
_temp256->tl= 0; _temp256;});* next= node; next=( struct Cyc_List_List**)({
struct Cyc_List_List* _temp254= node; if( _temp254 == 0){ _throw( Null_Exception);}&
_temp254->tl;}); f0s=({ struct Cyc_List_List* _temp255= f0s; if( _temp255 == 0){
_throw( Null_Exception);} _temp255->tl;});} if( f0s == 0? 1: cmp > 0){* incl= 0;{
struct Cyc_List_List* node=({ struct Cyc_List_List* _temp259=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp259->hd=( void*) Cyc_Tcdecl_substitute_tunionfield(
tvs0, tvs1, inst,( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp260= f1s; if( _temp260 == 0){ _throw( Null_Exception);} _temp260->hd;}));
_temp259->tl= 0; _temp259;});* next= node; next=( struct Cyc_List_List**)({
struct Cyc_List_List* _temp258= node; if( _temp258 == 0){ _throw( Null_Exception);}&
_temp258->tl;});}} else{ struct Cyc_Absyn_Tunionfield* f2= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp266= f0s; if(
_temp266 == 0){ _throw( Null_Exception);} _temp266->hd;}),( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp267= f1s; if( _temp267 == 0){ _throw( Null_Exception);}
_temp267->hd;}), inst, t, v, msg); if( f2 != 0){ if( f2 !=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp261= f0s; if(
_temp261 == 0){ _throw( Null_Exception);} _temp261->hd;}))){* incl= 0;}{ struct
Cyc_List_List* node=({ struct Cyc_List_List* _temp263=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp263->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp264= f2; if( _temp264 == 0){ _throw(
Null_Exception);} _temp264;})); _temp263->tl= 0; _temp263;});* next= node; next=(
struct Cyc_List_List**)({ struct Cyc_List_List* _temp262= node; if( _temp262 ==
0){ _throw( Null_Exception);}& _temp262->tl;});}} else{* res= 0;} f0s=({ struct
Cyc_List_List* _temp265= f0s; if( _temp265 == 0){ _throw( Null_Exception);}
_temp265->tl;});}} if( f1s != 0){* incl= 0;* next= f1s;} else{* next= f0s;}
return* f2sp;} struct _tuple8{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct Cyc_List_List* f){ if( f
== 0){ return({ struct _tuple8 _temp269; _temp269.f1= 0; _temp269.f2= 0;
_temp269;});} if(({ struct Cyc_List_List* _temp270= f; if( _temp270 == 0){
_throw( Null_Exception);} _temp270->tl;}) == 0){ return({ struct _tuple8
_temp271; _temp271.f1= f; _temp271.f2= 0; _temp271;});}{ struct Cyc_List_List*
_temp276; struct Cyc_List_List* _temp278; struct _tuple8 _temp274=(( struct
_tuple8(*)( struct Cyc_List_List* f)) Cyc_Tcdecl_split)(({ struct Cyc_List_List*
_temp273=({ struct Cyc_List_List* _temp272= f; if( _temp272 == 0){ _throw(
Null_Exception);} _temp272->tl;}); if( _temp273 == 0){ _throw( Null_Exception);}
_temp273->tl;})); _LL279: _temp278= _temp274.f1; goto _LL277; _LL277: _temp276=
_temp274.f2; goto _LL275; _LL275: return({ struct _tuple8 _temp280; _temp280.f1=({
struct Cyc_List_List* _temp284=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp284->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp285= f; if( _temp285 == 0){ _throw( Null_Exception);} _temp285->hd;}));
_temp284->tl= _temp278; _temp284;}); _temp280.f2=({ struct Cyc_List_List*
_temp281=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp281->hd=( void*)(( void*)({ struct Cyc_List_List* _temp283=({ struct Cyc_List_List*
_temp282= f; if( _temp282 == 0){ _throw( Null_Exception);} _temp282->tl;}); if(
_temp283 == 0){ _throw( Null_Exception);} _temp283->hd;})); _temp281->tl=
_temp276; _temp281;}); _temp280;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List* _temp288; struct Cyc_List_List*
_temp290; struct _tuple8 _temp286=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL291: _temp290= _temp286.f1; goto _LL289; _LL289:
_temp288= _temp286.f2; goto _LL287; _LL287: if( _temp290 != 0?({ struct Cyc_List_List*
_temp292= _temp290; if( _temp292 == 0){ _throw( Null_Exception);} _temp292->tl;})
!= 0: 0){ _temp290= Cyc_Tcdecl_sort_xtunion_fields( _temp290, res, v, loc, msg);}
if( _temp288 != 0?({ struct Cyc_List_List* _temp293= _temp288; if( _temp293 == 0){
_throw( Null_Exception);} _temp293->tl;}) != 0: 0){ _temp288= Cyc_Tcdecl_sort_xtunion_fields(
_temp288, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp290,
_temp288, 0, 0, 0, res,({ int* _temp294=( int*) GC_malloc_atomic( sizeof( int));
_temp294[ 0]= 1; _temp294;}),( struct _tagged_string)({ char* _temp295=( char*)"xtunion";
struct _tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 8; _temp296;}),* v, loc, msg);} struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tagged_string v= Cyc_Absynpp_qvar2string( d0->name); struct _tagged_string t=(
struct _tagged_string)({ char* _temp348=( char*)"[x]tunion"; struct
_tagged_string _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 10; _temp349;}); int res= 1; if( d0->is_xtunion
!= d1->is_xtunion){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp297=
v; struct _tagged_string _temp298= Cyc_Tcdecl_is_x2string( d0->is_xtunion);
struct _tagged_string _temp299= Cyc_Tcdecl_is_x2string( d1->is_xtunion); xprintf("expected %.*s to be a %.*s instead of a %.*s",
_temp297.last_plus_one - _temp297.curr, _temp297.curr, _temp298.last_plus_one -
_temp298.curr, _temp298.curr, _temp299.last_plus_one - _temp299.curr, _temp299.curr);}));
res= 0;} else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, v, loc, msg)){ return 0;}{ int _temp302; void* _temp304;
struct _tuple3 _temp300= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, v, loc, msg); _LL305: _temp304= _temp300.f1; goto _LL303; _LL303: _temp302=
_temp300.f2; goto _LL301; _LL301: if( ! _temp302){ res= 0;}{ struct Cyc_Absyn_Tuniondecl*
d2;{ struct _tuple6 _temp307=({ struct _tuple6 _temp306; _temp306.f1= d0->fields;
_temp306.f2= d1->fields; _temp306;}); struct Cyc_Core_Opt* _temp315; struct Cyc_Core_Opt*
_temp317; struct Cyc_Core_Opt* _temp319; struct Cyc_Core_Opt* _temp321; struct
Cyc_Core_Opt* _temp323; struct Cyc_Core_Opt _temp325; struct Cyc_List_List*
_temp326; struct Cyc_Core_Opt* _temp328; struct Cyc_Core_Opt _temp330; struct
Cyc_List_List* _temp331; _LL309: _LL318: _temp317= _temp307.f1; goto _LL316;
_LL316: _temp315= _temp307.f2; if( _temp315 == 0){ goto _LL310;} else{ goto
_LL311;} _LL311: _LL322: _temp321= _temp307.f1; if( _temp321 == 0){ goto _LL320;}
else{ goto _LL313;} _LL320: _temp319= _temp307.f2; goto _LL312; _LL313: _LL329:
_temp328= _temp307.f1; if( _temp328 == 0){ goto _LL308;} else{ _temp330=*
_temp328; _LL332: _temp331=( struct Cyc_List_List*) _temp330.v; goto _LL324;}
_LL324: _temp323= _temp307.f2; if( _temp323 == 0){ goto _LL308;} else{ _temp325=*
_temp323; _LL327: _temp326=( struct Cyc_List_List*) _temp325.v; goto _LL314;}
_LL310: d2= d0; goto _LL308; _LL312: d2= d1; goto _LL308; _LL314: { struct Cyc_List_List*
inst= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); if( d0->is_xtunion){ int incl=
1; struct Cyc_List_List* f2s= Cyc_Tcdecl_merge_xtunion_fields( _temp331,
_temp326, inst, d0->tvs, d1->tvs,& res,& incl, t, v, loc, msg); if( incl){ d2=
d0;} else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp333=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp333[ 0]=* d0; _temp333;});(
void*)( d2->sc=( void*) _temp304); d2->fields=({ struct Cyc_Core_Opt* _temp334=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp334->v=(
void*) f2s; _temp334;});}} else{ for( 0; _temp331 != 0? _temp326 != 0: 0;
_temp331=({ struct Cyc_List_List* _temp335= _temp331; if( _temp335 == 0){ _throw(
Null_Exception);} _temp335->tl;}), _temp326=({ struct Cyc_List_List* _temp336=
_temp326; if( _temp336 == 0){ _throw( Null_Exception);} _temp336->tl;})){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp337= _temp331; if(
_temp337 == 0){ _throw( Null_Exception);} _temp337->hd;}),( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp338= _temp326; if( _temp338 == 0){ _throw(
Null_Exception);} _temp338->hd;}), inst, t, v, msg);} if( _temp331 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp340= t; struct _tagged_string _temp341= v;
struct _tagged_string _temp342=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp339= _temp326; if( _temp339 == 0){ _throw( Null_Exception);} _temp339->hd;}))->name).f2;
xprintf("%.*s %.*s has extra field %.*s", _temp340.last_plus_one - _temp340.curr,
_temp340.curr, _temp341.last_plus_one - _temp341.curr, _temp341.curr, _temp342.last_plus_one
- _temp342.curr, _temp342.curr);})); res= 0;} if( _temp326 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp344= t; struct _tagged_string _temp345= v;
struct _tagged_string _temp346=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp343= _temp326; if( _temp343 == 0){ _throw( Null_Exception);} _temp343->hd;}))->name).f2;
xprintf("%.*s %.*s is missing field %.*s", _temp344.last_plus_one - _temp344.curr,
_temp344.curr, _temp345.last_plus_one - _temp345.curr, _temp345.curr, _temp346.last_plus_one
- _temp346.curr, _temp346.curr);})); res= 0;} d2= d0;} goto _LL308;} _LL308:;}
if( ! res){ return 0;} if( _temp304 ==( void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp347=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp347[ 0]=* d2; _temp347;});(
void*)( d2->sc=( void*) _temp304); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}}
struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string v= Cyc_Absynpp_qvar2string( d0->name);
int res= 1; int _temp354; void* _temp356; struct _tuple3 _temp352= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc,( struct _tagged_string)({ char* _temp350=( char*)"enum";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 5; _temp351;}), v, loc, msg); _LL357:
_temp356= _temp352.f1; goto _LL355; _LL355: _temp354= _temp352.f2; goto _LL353;
_LL353: if( ! _temp354){ res= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct
_tuple6 _temp359=({ struct _tuple6 _temp358; _temp358.f1= d0->fields; _temp358.f2=
d1->fields; _temp358;}); struct Cyc_Core_Opt* _temp367; struct Cyc_Core_Opt*
_temp369; struct Cyc_Core_Opt* _temp371; struct Cyc_Core_Opt* _temp373; struct
Cyc_Core_Opt* _temp375; struct Cyc_Core_Opt _temp377; struct Cyc_List_List*
_temp378; struct Cyc_Core_Opt* _temp380; struct Cyc_Core_Opt _temp382; struct
Cyc_List_List* _temp383; _LL361: _LL370: _temp369= _temp359.f1; goto _LL368;
_LL368: _temp367= _temp359.f2; if( _temp367 == 0){ goto _LL362;} else{ goto
_LL363;} _LL363: _LL374: _temp373= _temp359.f1; if( _temp373 == 0){ goto _LL372;}
else{ goto _LL365;} _LL372: _temp371= _temp359.f2; goto _LL364; _LL365: _LL381:
_temp380= _temp359.f1; if( _temp380 == 0){ goto _LL360;} else{ _temp382=*
_temp380; _LL384: _temp383=( struct Cyc_List_List*) _temp382.v; goto _LL376;}
_LL376: _temp375= _temp359.f2; if( _temp375 == 0){ goto _LL360;} else{ _temp377=*
_temp375; _LL379: _temp378=( struct Cyc_List_List*) _temp377.v; goto _LL366;}
_LL362: d2= d0; goto _LL360; _LL364: d2= d1; goto _LL360; _LL366: for( 0;
_temp383 != 0? _temp378 != 0: 0; _temp383=({ struct Cyc_List_List* _temp385=
_temp383; if( _temp385 == 0){ _throw( Null_Exception);} _temp385->tl;}),
_temp378=({ struct Cyc_List_List* _temp386= _temp378; if( _temp386 == 0){ _throw(
Null_Exception);} _temp386->tl;})){ struct Cyc_Absyn_Enumfield _temp390; struct
Cyc_Position_Segment* _temp391; struct Cyc_Absyn_Exp* _temp393; struct _tuple0*
_temp395; struct Cyc_Absyn_Enumfield* _temp388=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp387= _temp383; if( _temp387 == 0){ _throw(
Null_Exception);} _temp387->hd;}); _temp390=* _temp388; _LL396: _temp395=(
struct _tuple0*) _temp390.name; goto _LL394; _LL394: _temp393=( struct Cyc_Absyn_Exp*)
_temp390.tag; goto _LL392; _LL392: _temp391=( struct Cyc_Position_Segment*)
_temp390.loc; goto _LL389; _LL389: { struct Cyc_Absyn_Enumfield _temp400; struct
Cyc_Position_Segment* _temp401; struct Cyc_Absyn_Exp* _temp403; struct _tuple0*
_temp405; struct Cyc_Absyn_Enumfield* _temp398=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp397= _temp378; if( _temp397 == 0){ _throw(
Null_Exception);} _temp397->hd;}); _temp400=* _temp398; _LL406: _temp405=(
struct _tuple0*) _temp400.name; goto _LL404; _LL404: _temp403=( struct Cyc_Absyn_Exp*)
_temp400.tag; goto _LL402; _LL402: _temp401=( struct Cyc_Position_Segment*)
_temp400.loc; goto _LL399; _LL399: if( Cyc_String_zstrptrcmp((* _temp405).f2,(*
_temp395).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp407=
v; struct _tagged_string _temp408=*(* _temp405).f2; struct _tagged_string
_temp409=*(* _temp395).f2; xprintf("enum %.*s: field name mismatch %.*s != %.*s",
_temp407.last_plus_one - _temp407.curr, _temp407.curr, _temp408.last_plus_one -
_temp408.curr, _temp408.curr, _temp409.last_plus_one - _temp409.curr, _temp409.curr);}));
res= 0;} if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp410= _temp393; if( _temp410 == 0){ _throw( Null_Exception);}
_temp410;})) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp411= _temp403; if( _temp411 == 0){ _throw(
Null_Exception);} _temp411;}))){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp412= v; struct _tagged_string _temp413=*(* _temp405).f2;
xprintf("enum %.*s, field %.*s, value mismatch", _temp412.last_plus_one -
_temp412.curr, _temp412.curr, _temp413.last_plus_one - _temp413.curr, _temp413.curr);}));
res= 0;}}} d2= d0; goto _LL360; _LL360:;} if( ! res){ return 0;} if(( void*) d2->sc
== _temp356){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp414=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp414[ 0]=* d2; _temp414;});( void*)( d2->sc=( void*) _temp356); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ int res= 1; int _temp417; void* _temp419;
struct _tuple3 _temp415= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg);
_LL420: _temp419= _temp415.f1; goto _LL418; _LL418: _temp417= _temp415.f2; goto
_LL416; _LL416: if( ! _temp417){ res= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){
Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp421= t; struct
_tagged_string _temp422= v; struct _tagged_string _temp423= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp424= Cyc_Absynpp_typ2string( t0); xprintf("%.*s %.*s has type %.*s instead of %.*s",
_temp421.last_plus_one - _temp421.curr, _temp421.curr, _temp422.last_plus_one -
_temp422.curr, _temp422.curr, _temp423.last_plus_one - _temp423.curr, _temp423.curr,
_temp424.last_plus_one - _temp424.curr, _temp424.curr);})); res= 0;} if( ! Cyc_Tcutil_equal_tqual(
tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp425= t;
struct _tagged_string _temp426= v; xprintf("%.*s %.*s has different type qualifiers",
_temp425.last_plus_one - _temp425.curr, _temp425.curr, _temp426.last_plus_one -
_temp426.curr, _temp426.curr);})); res= 0;} if( ! Cyc_Tcutil_same_atts( atts0,
atts1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp427= t; struct
_tagged_string _temp428= v; xprintf("%.*s %.*s has different attributes",
_temp427.last_plus_one - _temp427.curr, _temp427.curr, _temp428.last_plus_one -
_temp428.curr, _temp428.curr);})); res= 0;} return({ struct _tuple3 _temp429;
_temp429.f1= _temp419; _temp429.f2= res; _temp429;});} struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tagged_string v= Cyc_Absynpp_qvar2string( d0->name); int _temp434; void*
_temp436; struct _tuple3 _temp432= Cyc_Tcdecl_check_var_or_fn_decl(( void*) d0->sc,(
void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*) d1->type, d1->tq,
d1->attributes,( struct _tagged_string)({ char* _temp430=( char*)"variable";
struct _tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 9; _temp431;}), v, loc, msg); _LL437:
_temp436= _temp432.f1; goto _LL435; _LL435: _temp434= _temp432.f2; goto _LL433;
_LL433: if( ! _temp434){ return 0;} if(( void*) d0->sc == _temp436){ return(
struct Cyc_Absyn_Vardecl*) d0;} else{ struct Cyc_Absyn_Vardecl* d2=({ struct Cyc_Absyn_Vardecl*
_temp439=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp439[ 0]=* d0; _temp439;});( void*)({ struct Cyc_Absyn_Vardecl* _temp438= d2;
if( _temp438 == 0){ _throw( Null_Exception);} _temp438->sc=( void*) _temp436;});
return d2;}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl( struct
Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct _tagged_string v= Cyc_Absynpp_qvar2string(
d0->name); if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs,( struct _tagged_string)({
char* _temp440=( char*)"typedef"; struct _tagged_string _temp441; _temp441.curr=
_temp440; _temp441.base= _temp440; _temp441.last_plus_one= _temp440 + 8;
_temp441;}), v, loc, msg)){ return 0;}{ struct Cyc_List_List* inst= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute( inst,( void*) d1->defn);
if( ! Cyc_Tcdecl_check_type(( void*) d0->defn, subst_defn1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp442= v; struct _tagged_string _temp443=
Cyc_Absynpp_typ2string( subst_defn1); struct _tagged_string _temp444= Cyc_Absynpp_typ2string((
void*) d0->defn); xprintf("typedef %.*s does refer to the same type: %.*s != %.*s",
_temp442.last_plus_one - _temp442.curr, _temp442.curr, _temp443.last_plus_one -
_temp443.curr, _temp443.curr, _temp444.last_plus_one - _temp444.curr, _temp444.curr);}));
return 0;} return( struct Cyc_Absyn_Typedefdecl*) d0;}} void* Cyc_Tcdecl_merge_binding(
void* b0, void* b1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){
struct _tuple4 _temp446=({ struct _tuple4 _temp445; _temp445.f1= b0; _temp445.f2=
b1; _temp445;}); void* _temp460; void* _temp462; void* _temp464; struct Cyc_Absyn_Vardecl*
_temp466; void* _temp468; struct Cyc_Absyn_Vardecl* _temp470; void* _temp472;
struct Cyc_Absyn_Fndecl* _temp474; void* _temp476; struct Cyc_Absyn_Vardecl*
_temp478; void* _temp480; struct Cyc_Absyn_Fndecl* _temp482; void* _temp484;
struct Cyc_Absyn_Fndecl* _temp486; void* _temp488; struct Cyc_Absyn_Vardecl*
_temp490; void* _temp492; struct Cyc_Absyn_Fndecl* _temp494; _LL448: _LL463:
_temp462= _temp446.f1; if( _temp462 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL461;} else{ goto _LL450;} _LL461: _temp460= _temp446.f2; if( _temp460 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL449;} else{ goto _LL450;} _LL450: _LL469:
_temp468= _temp446.f1; if(( unsigned int) _temp468 > 1u?*(( int*) _temp468) ==
Cyc_Absyn_Global_b: 0){ _LL471: _temp470=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Global_b_struct*) _temp468)->f1; goto _LL465;} else{ goto _LL452;}
_LL465: _temp464= _temp446.f2; if(( unsigned int) _temp464 > 1u?*(( int*)
_temp464) == Cyc_Absyn_Global_b: 0){ _LL467: _temp466=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp464)->f1; goto _LL451;} else{ goto
_LL452;} _LL452: _LL477: _temp476= _temp446.f1; if(( unsigned int) _temp476 > 1u?*((
int*) _temp476) == Cyc_Absyn_Global_b: 0){ _LL479: _temp478=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp476)->f1; goto _LL473;} else{ goto
_LL454;} _LL473: _temp472= _temp446.f2; if(( unsigned int) _temp472 > 1u?*(( int*)
_temp472) == Cyc_Absyn_Funname_b: 0){ _LL475: _temp474=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp472)->f1; goto _LL453;} else{ goto
_LL454;} _LL454: _LL485: _temp484= _temp446.f1; if(( unsigned int) _temp484 > 1u?*((
int*) _temp484) == Cyc_Absyn_Funname_b: 0){ _LL487: _temp486=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp484)->f1; goto _LL481;} else{ goto
_LL456;} _LL481: _temp480= _temp446.f2; if(( unsigned int) _temp480 > 1u?*(( int*)
_temp480) == Cyc_Absyn_Funname_b: 0){ _LL483: _temp482=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp480)->f1; goto _LL455;} else{ goto
_LL456;} _LL456: _LL493: _temp492= _temp446.f1; if(( unsigned int) _temp492 > 1u?*((
int*) _temp492) == Cyc_Absyn_Funname_b: 0){ _LL495: _temp494=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp492)->f1; goto _LL489;} else{ goto
_LL458;} _LL489: _temp488= _temp446.f2; if(( unsigned int) _temp488 > 1u?*(( int*)
_temp488) == Cyc_Absyn_Global_b: 0){ _LL491: _temp490=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp488)->f1; goto _LL457;} else{ goto
_LL458;} _LL458: goto _LL459; _LL449: return( void*) Cyc_Absyn_Unresolved_b;
_LL451: { struct Cyc_Absyn_Vardecl* d2= Cyc_Tcdecl_merge_vardecl( _temp470,
_temp466, loc, msg); if( d2 == 0){ return( void*) Cyc_Absyn_Unresolved_b;} if((
struct Cyc_Absyn_Vardecl*) _temp470 == d2){ return b0;} else{ if(( struct Cyc_Absyn_Vardecl*)
_temp466 == d2){ return b1;} else{ return( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp496=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp496[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp497; _temp497.tag= Cyc_Absyn_Global_b;
_temp497.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp498=
d2; if( _temp498 == 0){ _throw( Null_Exception);} _temp498;}); _temp497;});
_temp496;});}}} _LL453: { int _temp504; void* _temp506; struct _tuple3 _temp502=
Cyc_Tcdecl_check_var_or_fn_decl(( void*) _temp478->sc,( void*) _temp478->type,
_temp478->tq, _temp478->attributes,( void*) _temp474->sc,( void*)({ struct Cyc_Core_Opt*
_temp499= _temp474->cached_typ; if( _temp499 == 0){ _throw( Null_Exception);}
_temp499->v;}), Cyc_Absyn_empty_tqual(), _temp474->attributes,( struct
_tagged_string)({ char* _temp500=( char*)"function"; struct _tagged_string
_temp501; _temp501.curr= _temp500; _temp501.base= _temp500; _temp501.last_plus_one=
_temp500 + 9; _temp501;}), Cyc_Absynpp_qvar2string( _temp478->name), loc, msg);
_LL507: _temp506= _temp502.f1; goto _LL505; _LL505: _temp504= _temp502.f2; goto
_LL503; _LL503: if( ! _temp504){ return( void*) Cyc_Absyn_Unresolved_b;} return
b1;} _LL455: Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp508= Cyc_Absynpp_qvar2string(
_temp482->name); xprintf("redefinition of function %.*s", _temp508.last_plus_one
- _temp508.curr, _temp508.curr);})); return( void*) Cyc_Absyn_Unresolved_b;
_LL457: { int _temp514; void* _temp516; struct _tuple3 _temp512= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp494->sc,( void*)({ struct Cyc_Core_Opt* _temp509= _temp494->cached_typ;
if( _temp509 == 0){ _throw( Null_Exception);} _temp509->v;}), Cyc_Absyn_empty_tqual(),
_temp494->attributes,( void*) _temp490->sc,( void*) _temp490->type, _temp490->tq,
_temp490->attributes,( struct _tagged_string)({ char* _temp510=( char*)"variable";
struct _tagged_string _temp511; _temp511.curr= _temp510; _temp511.base= _temp510;
_temp511.last_plus_one= _temp510 + 9; _temp511;}), Cyc_Absynpp_qvar2string(
_temp494->name), loc, msg); _LL517: _temp516= _temp512.f1; goto _LL515; _LL515:
_temp514= _temp512.f2; goto _LL513; _LL513: if( ! _temp514){ return( void*) Cyc_Absyn_Unresolved_b;}
return b0;} _LL459:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp518=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp518[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp519; _temp519.tag= Cyc_Core_InvalidArg;
_temp519.f1=( struct _tagged_string)({ char* _temp520=( char*)"Tcdecl::merge_binding";
struct _tagged_string _temp521; _temp521.curr= _temp520; _temp521.base= _temp520;
_temp521.last_plus_one= _temp520 + 22; _temp521;}); _temp519;}); _temp518;}));
_LL447:;} struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct Cyc_Tcdecl_Xtunionfielddecl
_temp524; struct Cyc_Absyn_Tunionfield* _temp525; struct Cyc_Absyn_Tuniondecl*
_temp527; struct Cyc_Tcdecl_Xtunionfielddecl* _temp522= d0; _temp524=* _temp522;
_LL528: _temp527=( struct Cyc_Absyn_Tuniondecl*) _temp524.base; goto _LL526;
_LL526: _temp525=( struct Cyc_Absyn_Tunionfield*) _temp524.field; goto _LL523;
_LL523: { struct Cyc_Tcdecl_Xtunionfielddecl _temp531; struct Cyc_Absyn_Tunionfield*
_temp532; struct Cyc_Absyn_Tuniondecl* _temp534; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp529= d1; _temp531=* _temp529; _LL535: _temp534=( struct Cyc_Absyn_Tuniondecl*)
_temp531.base; goto _LL533; _LL533: _temp532=( struct Cyc_Absyn_Tunionfield*)
_temp531.field; goto _LL530; _LL530: { struct _tagged_string v= Cyc_Absynpp_qvar2string(
_temp525->name); if( Cyc_Absyn_qvar_cmp( _temp527->name, _temp534->name) != 0){
Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp536= Cyc_Absynpp_qvar2string(
_temp527->name); struct _tagged_string _temp537= Cyc_Absynpp_qvar2string(
_temp534->name); struct _tagged_string _temp538= v; xprintf("xtunions %.*s and %.*s have both a field named %.*s",
_temp536.last_plus_one - _temp536.curr, _temp536.curr, _temp537.last_plus_one -
_temp537.curr, _temp537.curr, _temp538.last_plus_one - _temp538.curr, _temp538.curr);}));
return 0;} if( ! Cyc_Tcdecl_check_tvs( _temp527->tvs, _temp534->tvs,( struct
_tagged_string)({ char* _temp539=( char*)"xtunion"; struct _tagged_string
_temp540; _temp540.curr= _temp539; _temp540.base= _temp539; _temp540.last_plus_one=
_temp539 + 8; _temp540;}), Cyc_Absynpp_qvar2string( _temp527->name), loc, msg)){
return 0;}{ struct Cyc_List_List* inst1= Cyc_Tcdecl_build_tvs_map( _temp527->tvs,
_temp534->tvs); struct Cyc_Absyn_Tunionfield* f2= Cyc_Tcdecl_merge_tunionfield(
_temp525, _temp532, inst1,( struct _tagged_string)({ char* _temp543=( char*)"xtunionfield";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 13; _temp544;}), v, msg); if( f2 == 0){
return 0;} if( f2 ==( struct Cyc_Absyn_Tunionfield*) _temp525){ return( struct
Cyc_Tcdecl_Xtunionfielddecl*) d0;} else{ return({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp541=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp541->base= _temp527; _temp541->field=( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp542= f2; if( _temp542 == 0){ _throw(
Null_Exception);} _temp542;}); _temp541;});}}}}}