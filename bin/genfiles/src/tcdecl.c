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
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; extern int Cyc_String_strptrcmp( struct _tagged_string* s1, struct
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
if(( void*) f0->sc == _temp212){ struct Cyc_Absyn_Tunionfield* f2=({ struct Cyc_Absyn_Tunionfield*
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
return* f2sp;} static struct Cyc_List_List* Cyc_Tcdecl_sub_xtunion_fields(
struct Cyc_List_List* f0s, struct Cyc_List_List* f1s, struct Cyc_List_List* inst,
int* res, struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List** f2sp=({ struct Cyc_List_List**
_temp283=( struct Cyc_List_List**) GC_malloc( sizeof( struct Cyc_List_List*));
_temp283[ 0]= 0; _temp283;}); struct Cyc_List_List** next= f2sp; int cmp= - 1;
for( 0; f0s != 0? f1s != 0: 0; f1s=({ struct Cyc_List_List* _temp269= f1s; if(
_temp269 == 0){ _throw( Null_Exception);} _temp269->tl;})){ while( f0s != 0?(
cmp= Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp270= f0s; if( _temp270 == 0){ _throw( Null_Exception);} _temp270->hd;}))->name,((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp271= f1s; if(
_temp271 == 0){ _throw( Null_Exception);} _temp271->hd;}))->name)) < 0: 0) { f0s=({
struct Cyc_List_List* _temp272= f0s; if( _temp272 == 0){ _throw( Null_Exception);}
_temp272->tl;});} if( f0s == 0? 1: cmp > 0){ struct Cyc_List_List* node=({
struct Cyc_List_List* _temp274=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp274->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp275= f1s; if( _temp275 == 0){ _throw( Null_Exception);}
_temp275->hd;})); _temp274->tl= 0; _temp274;});* next= node; next=( struct Cyc_List_List**)({
struct Cyc_List_List* _temp273= node; if( _temp273 == 0){ _throw( Null_Exception);}&
_temp273->tl;});} else{ struct Cyc_Absyn_Tunionfield* f2= Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp281= f0s; if(
_temp281 == 0){ _throw( Null_Exception);} _temp281->hd;}),( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp282= f1s; if( _temp282 == 0){ _throw( Null_Exception);}
_temp282->hd;}), inst, t, v, msg); if( f2 == 0){* res= 0;} else{ if( f2 !=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp276= f0s; if( _temp276 == 0){ _throw( Null_Exception);} _temp276->hd;}))){
struct Cyc_List_List* node=({ struct Cyc_List_List* _temp278=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp278->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_Absyn_Tunionfield* _temp279= f2; if( _temp279 == 0){ _throw(
Null_Exception);} _temp279;})); _temp278->tl= 0; _temp278;});* next= node; next=(
struct Cyc_List_List**)({ struct Cyc_List_List* _temp277= node; if( _temp277 ==
0){ _throw( Null_Exception);}& _temp277->tl;});}} f0s=({ struct Cyc_List_List*
_temp280= f0s; if( _temp280 == 0){ _throw( Null_Exception);} _temp280->tl;});}}
if( f1s != 0){* next= f1s;} return* f2sp;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; static struct _tuple8 Cyc_Tcdecl_split( struct
Cyc_List_List* f){ if( f == 0){ return({ struct _tuple8 _temp284; _temp284.f1= 0;
_temp284.f2= 0; _temp284;});} if(({ struct Cyc_List_List* _temp285= f; if(
_temp285 == 0){ _throw( Null_Exception);} _temp285->tl;}) == 0){ return({ struct
_tuple8 _temp286; _temp286.f1= f; _temp286.f2= 0; _temp286;});}{ struct Cyc_List_List*
_temp291; struct Cyc_List_List* _temp293; struct _tuple8 _temp289=(( struct
_tuple8(*)( struct Cyc_List_List* f)) Cyc_Tcdecl_split)(({ struct Cyc_List_List*
_temp288=({ struct Cyc_List_List* _temp287= f; if( _temp287 == 0){ _throw(
Null_Exception);} _temp287->tl;}); if( _temp288 == 0){ _throw( Null_Exception);}
_temp288->tl;})); _LL294: _temp293= _temp289.f1; goto _LL292; _LL292: _temp291=
_temp289.f2; goto _LL290; _LL290: return({ struct _tuple8 _temp295; _temp295.f1=({
struct Cyc_List_List* _temp299=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp299->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp300= f; if( _temp300 == 0){ _throw( Null_Exception);} _temp300->hd;}));
_temp299->tl= _temp293; _temp299;}); _temp295.f2=({ struct Cyc_List_List*
_temp296=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp296->hd=( void*)(( void*)({ struct Cyc_List_List* _temp298=({ struct Cyc_List_List*
_temp297= f; if( _temp297 == 0){ _throw( Null_Exception);} _temp297->tl;}); if(
_temp298 == 0){ _throw( Null_Exception);} _temp298->hd;})); _temp296->tl=
_temp291; _temp296;}); _temp295;});}} struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List* f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct Cyc_List_List* _temp303; struct Cyc_List_List*
_temp305; struct _tuple8 _temp301=(( struct _tuple8(*)( struct Cyc_List_List* f))
Cyc_Tcdecl_split)( f); _LL306: _temp305= _temp301.f1; goto _LL304; _LL304:
_temp303= _temp301.f2; goto _LL302; _LL302: if( _temp305 != 0?({ struct Cyc_List_List*
_temp307= _temp305; if( _temp307 == 0){ _throw( Null_Exception);} _temp307->tl;})
!= 0: 0){ _temp305= Cyc_Tcdecl_sort_xtunion_fields( _temp305, res, v, loc, msg);}
if( _temp303 != 0?({ struct Cyc_List_List* _temp308= _temp303; if( _temp308 == 0){
_throw( Null_Exception);} _temp308->tl;}) != 0: 0){ _temp303= Cyc_Tcdecl_sort_xtunion_fields(
_temp303, res, v, loc, msg);} return Cyc_Tcdecl_merge_xtunion_fields( _temp305,
_temp303, 0, 0, 0, res,({ int* _temp309=( int*) GC_malloc_atomic( sizeof( int));
_temp309[ 0]= 1; _temp309;}),( struct _tagged_string)({ char* _temp310=( char*)"xtunion";
struct _tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 8; _temp311;}),* v, loc, msg);} struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tagged_string v= Cyc_Absynpp_qvar2string( d0->name); struct _tagged_string t=(
struct _tagged_string)({ char* _temp363=( char*)"[x]tunion"; struct
_tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 10; _temp364;}); int res= 1; if( d0->is_xtunion
!= d1->is_xtunion){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp312=
v; struct _tagged_string _temp313= Cyc_Tcdecl_is_x2string( d0->is_xtunion);
struct _tagged_string _temp314= Cyc_Tcdecl_is_x2string( d1->is_xtunion); xprintf("expected %.*s to be a %.*s instead of a %.*s",
_temp312.last_plus_one - _temp312.curr, _temp312.curr, _temp313.last_plus_one -
_temp313.curr, _temp313.curr, _temp314.last_plus_one - _temp314.curr, _temp314.curr);}));
res= 0;} else{ t= Cyc_Tcdecl_is_x2string( d0->is_xtunion);} if( ! Cyc_Tcdecl_check_tvs(
d0->tvs, d1->tvs, t, v, loc, msg)){ return 0;}{ int _temp317; void* _temp319;
struct _tuple3 _temp315= Cyc_Tcdecl_merge_scope(( void*) d0->sc,( void*) d1->sc,
t, v, loc, msg); _LL320: _temp319= _temp315.f1; goto _LL318; _LL318: _temp317=
_temp315.f2; goto _LL316; _LL316: if( ! _temp317){ res= 0;}{ struct Cyc_Absyn_Tuniondecl*
d2;{ struct _tuple6 _temp322=({ struct _tuple6 _temp321; _temp321.f1= d0->fields;
_temp321.f2= d1->fields; _temp321;}); struct Cyc_Core_Opt* _temp330; struct Cyc_Core_Opt*
_temp332; struct Cyc_Core_Opt* _temp334; struct Cyc_Core_Opt* _temp336; struct
Cyc_Core_Opt* _temp338; struct Cyc_Core_Opt _temp340; struct Cyc_List_List*
_temp341; struct Cyc_Core_Opt* _temp343; struct Cyc_Core_Opt _temp345; struct
Cyc_List_List* _temp346; _LL324: _LL333: _temp332= _temp322.f1; goto _LL331;
_LL331: _temp330= _temp322.f2; if( _temp330 == 0){ goto _LL325;} else{ goto
_LL326;} _LL326: _LL337: _temp336= _temp322.f1; if( _temp336 == 0){ goto _LL335;}
else{ goto _LL328;} _LL335: _temp334= _temp322.f2; goto _LL327; _LL328: _LL344:
_temp343= _temp322.f1; if( _temp343 == 0){ goto _LL323;} else{ _temp345=*
_temp343; _LL347: _temp346=( struct Cyc_List_List*) _temp345.v; goto _LL339;}
_LL339: _temp338= _temp322.f2; if( _temp338 == 0){ goto _LL323;} else{ _temp340=*
_temp338; _LL342: _temp341=( struct Cyc_List_List*) _temp340.v; goto _LL329;}
_LL325: d2= d0; goto _LL323; _LL327: d2= d1; goto _LL323; _LL329: { struct Cyc_List_List*
inst= Cyc_Tcdecl_build_tvs_map( d0->tvs, d1->tvs); if( d0->is_xtunion){ int incl=
1; struct Cyc_List_List* f2s= Cyc_Tcdecl_merge_xtunion_fields( _temp346,
_temp341, inst, d0->tvs, d1->tvs,& res,& incl, t, v, loc, msg); if( incl){ d2=
d0;} else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp348=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp348[ 0]=* d0; _temp348;});(
void*)( d2->sc=( void*) _temp319); d2->fields=({ struct Cyc_Core_Opt* _temp349=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp349->v=(
void*) f2s; _temp349;});}} else{ for( 0; _temp346 != 0? _temp341 != 0: 0;
_temp346=({ struct Cyc_List_List* _temp350= _temp346; if( _temp350 == 0){ _throw(
Null_Exception);} _temp350->tl;}), _temp341=({ struct Cyc_List_List* _temp351=
_temp341; if( _temp351 == 0){ _throw( Null_Exception);} _temp351->tl;})){ Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp352= _temp346; if(
_temp352 == 0){ _throw( Null_Exception);} _temp352->hd;}),( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp353= _temp341; if( _temp353 == 0){ _throw(
Null_Exception);} _temp353->hd;}), inst, t, v, msg);} if( _temp346 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp355= t; struct _tagged_string _temp356= v;
struct _tagged_string _temp357=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp354= _temp341; if( _temp354 == 0){ _throw( Null_Exception);} _temp354->hd;}))->name).f2;
xprintf("%.*s %.*s has extra field %.*s", _temp355.last_plus_one - _temp355.curr,
_temp355.curr, _temp356.last_plus_one - _temp356.curr, _temp356.curr, _temp357.last_plus_one
- _temp357.curr, _temp357.curr);})); res= 0;} if( _temp341 != 0){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp359= t; struct _tagged_string _temp360= v;
struct _tagged_string _temp361=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp358= _temp341; if( _temp358 == 0){ _throw( Null_Exception);} _temp358->hd;}))->name).f2;
xprintf("%.*s %.*s is missing field %.*s", _temp359.last_plus_one - _temp359.curr,
_temp359.curr, _temp360.last_plus_one - _temp360.curr, _temp360.curr, _temp361.last_plus_one
- _temp361.curr, _temp361.curr);})); res= 0;} d2= d0;} goto _LL323;} _LL323:;}
if( ! res){ return 0;} if( _temp319 ==( void*) d2->sc){ return( struct Cyc_Absyn_Tuniondecl*)
d2;} else{ d2=({ struct Cyc_Absyn_Tuniondecl* _temp362=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp362[ 0]=* d2; _temp362;});(
void*)( d2->sc=( void*) _temp319); return( struct Cyc_Absyn_Tuniondecl*) d2;}}}}
struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl( struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg){ struct _tagged_string v= Cyc_Absynpp_qvar2string( d0->name);
int res= 1; int _temp369; void* _temp371; struct _tuple3 _temp367= Cyc_Tcdecl_merge_scope((
void*) d0->sc,( void*) d1->sc,( struct _tagged_string)({ char* _temp365=( char*)"enum";
struct _tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 5; _temp366;}), v, loc, msg); _LL372:
_temp371= _temp367.f1; goto _LL370; _LL370: _temp369= _temp367.f2; goto _LL368;
_LL368: if( ! _temp369){ res= 0;}{ struct Cyc_Absyn_Enumdecl* d2;{ struct
_tuple6 _temp374=({ struct _tuple6 _temp373; _temp373.f1= d0->fields; _temp373.f2=
d1->fields; _temp373;}); struct Cyc_Core_Opt* _temp382; struct Cyc_Core_Opt*
_temp384; struct Cyc_Core_Opt* _temp386; struct Cyc_Core_Opt* _temp388; struct
Cyc_Core_Opt* _temp390; struct Cyc_Core_Opt _temp392; struct Cyc_List_List*
_temp393; struct Cyc_Core_Opt* _temp395; struct Cyc_Core_Opt _temp397; struct
Cyc_List_List* _temp398; _LL376: _LL385: _temp384= _temp374.f1; goto _LL383;
_LL383: _temp382= _temp374.f2; if( _temp382 == 0){ goto _LL377;} else{ goto
_LL378;} _LL378: _LL389: _temp388= _temp374.f1; if( _temp388 == 0){ goto _LL387;}
else{ goto _LL380;} _LL387: _temp386= _temp374.f2; goto _LL379; _LL380: _LL396:
_temp395= _temp374.f1; if( _temp395 == 0){ goto _LL375;} else{ _temp397=*
_temp395; _LL399: _temp398=( struct Cyc_List_List*) _temp397.v; goto _LL391;}
_LL391: _temp390= _temp374.f2; if( _temp390 == 0){ goto _LL375;} else{ _temp392=*
_temp390; _LL394: _temp393=( struct Cyc_List_List*) _temp392.v; goto _LL381;}
_LL377: d2= d0; goto _LL375; _LL379: d2= d1; goto _LL375; _LL381: for( 0;
_temp398 != 0? _temp393 != 0: 0; _temp398=({ struct Cyc_List_List* _temp400=
_temp398; if( _temp400 == 0){ _throw( Null_Exception);} _temp400->tl;}),
_temp393=({ struct Cyc_List_List* _temp401= _temp393; if( _temp401 == 0){ _throw(
Null_Exception);} _temp401->tl;})){ struct Cyc_Absyn_Enumfield _temp405; struct
Cyc_Position_Segment* _temp406; struct Cyc_Absyn_Exp* _temp408; struct _tuple0*
_temp410; struct Cyc_Absyn_Enumfield* _temp403=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp402= _temp398; if( _temp402 == 0){ _throw(
Null_Exception);} _temp402->hd;}); _temp405=* _temp403; _LL411: _temp410=(
struct _tuple0*) _temp405.name; goto _LL409; _LL409: _temp408=( struct Cyc_Absyn_Exp*)
_temp405.tag; goto _LL407; _LL407: _temp406=( struct Cyc_Position_Segment*)
_temp405.loc; goto _LL404; _LL404: { struct Cyc_Absyn_Enumfield _temp415; struct
Cyc_Position_Segment* _temp416; struct Cyc_Absyn_Exp* _temp418; struct _tuple0*
_temp420; struct Cyc_Absyn_Enumfield* _temp413=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp412= _temp393; if( _temp412 == 0){ _throw(
Null_Exception);} _temp412->hd;}); _temp415=* _temp413; _LL421: _temp420=(
struct _tuple0*) _temp415.name; goto _LL419; _LL419: _temp418=( struct Cyc_Absyn_Exp*)
_temp415.tag; goto _LL417; _LL417: _temp416=( struct Cyc_Position_Segment*)
_temp415.loc; goto _LL414; _LL414: if( Cyc_String_zstrptrcmp((* _temp420).f2,(*
_temp410).f2) != 0){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp422=
v; struct _tagged_string _temp423=*(* _temp420).f2; struct _tagged_string
_temp424=*(* _temp410).f2; xprintf("enum %.*s: field name mismatch %.*s != %.*s",
_temp422.last_plus_one - _temp422.curr, _temp422.curr, _temp423.last_plus_one -
_temp423.curr, _temp423.curr, _temp424.last_plus_one - _temp424.curr, _temp424.curr);}));
res= 0;} if( Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp425= _temp408; if( _temp425 == 0){ _throw( Null_Exception);}
_temp425;})) != Cyc_Tcdecl_get_uint_const_value(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp426= _temp418; if( _temp426 == 0){ _throw(
Null_Exception);} _temp426;}))){ Cyc_Tcdecl_merr( loc, msg,({ struct
_tagged_string _temp427= v; struct _tagged_string _temp428=*(* _temp420).f2;
xprintf("enum %.*s, field %.*s, value mismatch", _temp427.last_plus_one -
_temp427.curr, _temp427.curr, _temp428.last_plus_one - _temp428.curr, _temp428.curr);}));
res= 0;}}} d2= d0; goto _LL375; _LL375:;} if( ! res){ return 0;} if(( void*) d2->sc
== _temp371){ return( struct Cyc_Absyn_Enumdecl*) d2;} else{ d2=({ struct Cyc_Absyn_Enumdecl*
_temp429=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp429[ 0]=* d2; _temp429;});( void*)( d2->sc=( void*) _temp371); return(
struct Cyc_Absyn_Enumdecl*) d2;}}} inline static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void* sc0, void* t0, struct Cyc_Absyn_Tqual tq0, struct Cyc_List_List* atts0,
void* sc1, void* t1, struct Cyc_Absyn_Tqual tq1, struct Cyc_List_List* atts1,
struct _tagged_string t, struct _tagged_string v, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ int res= 1; int _temp432; void* _temp434;
struct _tuple3 _temp430= Cyc_Tcdecl_merge_scope( sc0, sc1, t, v, loc, msg);
_LL435: _temp434= _temp430.f1; goto _LL433; _LL433: _temp432= _temp430.f2; goto
_LL431; _LL431: if( ! _temp432){ res= 0;} if( ! Cyc_Tcdecl_check_type( t0, t1)){
Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp436= t; struct
_tagged_string _temp437= v; struct _tagged_string _temp438= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp439= Cyc_Absynpp_typ2string( t0); xprintf("%.*s %.*s has type %.*s instead of %.*s",
_temp436.last_plus_one - _temp436.curr, _temp436.curr, _temp437.last_plus_one -
_temp437.curr, _temp437.curr, _temp438.last_plus_one - _temp438.curr, _temp438.curr,
_temp439.last_plus_one - _temp439.curr, _temp439.curr);})); res= 0;} if( ! Cyc_Tcutil_equal_tqual(
tq0, tq1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp440= t;
struct _tagged_string _temp441= v; xprintf("%.*s %.*s has different type qualifiers",
_temp440.last_plus_one - _temp440.curr, _temp440.curr, _temp441.last_plus_one -
_temp441.curr, _temp441.curr);})); res= 0;} if( ! Cyc_Tcutil_same_atts( atts0,
atts1)){ Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp442= t; struct
_tagged_string _temp443= v; xprintf("%.*s %.*s has different attributes",
_temp442.last_plus_one - _temp442.curr, _temp442.curr, _temp443.last_plus_one -
_temp443.curr, _temp443.curr);})); res= 0;} return({ struct _tuple3 _temp444;
_temp444.f1= _temp434; _temp444.f2= res; _temp444;});} struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){ struct
_tagged_string v= Cyc_Absynpp_qvar2string( d0->name); int _temp449; void*
_temp451; struct _tuple3 _temp447= Cyc_Tcdecl_check_var_or_fn_decl(( void*) d0->sc,(
void*) d0->type, d0->tq, d0->attributes,( void*) d1->sc,( void*) d1->type, d1->tq,
d1->attributes,( struct _tagged_string)({ char* _temp445=( char*)"variable";
struct _tagged_string _temp446; _temp446.curr= _temp445; _temp446.base= _temp445;
_temp446.last_plus_one= _temp445 + 9; _temp446;}), v, loc, msg); _LL452:
_temp451= _temp447.f1; goto _LL450; _LL450: _temp449= _temp447.f2; goto _LL448;
_LL448: if( ! _temp449){ return 0;} if(( void*) d0->sc == _temp451){ return(
struct Cyc_Absyn_Vardecl*) d0;} else{ struct Cyc_Absyn_Vardecl* d2=({ struct Cyc_Absyn_Vardecl*
_temp454=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp454[ 0]=* d0; _temp454;});( void*)({ struct Cyc_Absyn_Vardecl* _temp453= d2;
if( _temp453 == 0){ _throw( Null_Exception);} _temp453->sc=( void*) _temp451;});
return d2;}} struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl( struct
Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg){ struct _tagged_string v= Cyc_Absynpp_qvar2string(
d0->name); if( ! Cyc_Tcdecl_check_tvs( d0->tvs, d1->tvs,( struct _tagged_string)({
char* _temp455=( char*)"typedef"; struct _tagged_string _temp456; _temp456.curr=
_temp455; _temp456.base= _temp455; _temp456.last_plus_one= _temp455 + 8;
_temp456;}), v, loc, msg)){ return 0;}{ struct Cyc_List_List* inst= Cyc_Tcdecl_build_tvs_map(
d0->tvs, d1->tvs); void* subst_defn1= Cyc_Tcutil_substitute( inst,( void*) d1->defn);
if( ! Cyc_Tcdecl_check_type(( void*) d0->defn, subst_defn1)){ Cyc_Tcdecl_merr(
loc, msg,({ struct _tagged_string _temp457= v; struct _tagged_string _temp458=
Cyc_Absynpp_typ2string( subst_defn1); struct _tagged_string _temp459= Cyc_Absynpp_typ2string((
void*) d0->defn); xprintf("typedef %.*s does refer to the same type: %.*s != %.*s",
_temp457.last_plus_one - _temp457.curr, _temp457.curr, _temp458.last_plus_one -
_temp458.curr, _temp458.curr, _temp459.last_plus_one - _temp459.curr, _temp459.curr);}));
return 0;} return( struct Cyc_Absyn_Typedefdecl*) d0;}} void* Cyc_Tcdecl_merge_binding(
void* b0, void* b1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg){
struct _tuple4 _temp461=({ struct _tuple4 _temp460; _temp460.f1= b0; _temp460.f2=
b1; _temp460;}); void* _temp475; void* _temp477; void* _temp479; struct Cyc_Absyn_Vardecl*
_temp481; void* _temp483; struct Cyc_Absyn_Vardecl* _temp485; void* _temp487;
struct Cyc_Absyn_Fndecl* _temp489; void* _temp491; struct Cyc_Absyn_Vardecl*
_temp493; void* _temp495; struct Cyc_Absyn_Fndecl* _temp497; void* _temp499;
struct Cyc_Absyn_Fndecl* _temp501; void* _temp503; struct Cyc_Absyn_Vardecl*
_temp505; void* _temp507; struct Cyc_Absyn_Fndecl* _temp509; _LL463: _LL478:
_temp477= _temp461.f1; if( _temp477 ==( void*) Cyc_Absyn_Unresolved_b){ goto
_LL476;} else{ goto _LL465;} _LL476: _temp475= _temp461.f2; if( _temp475 ==(
void*) Cyc_Absyn_Unresolved_b){ goto _LL464;} else{ goto _LL465;} _LL465: _LL484:
_temp483= _temp461.f1; if(( unsigned int) _temp483 > 1u?*(( int*) _temp483) ==
Cyc_Absyn_Global_b: 0){ _LL486: _temp485=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_Absyn_Global_b_struct*) _temp483)->f1; goto _LL480;} else{ goto _LL467;}
_LL480: _temp479= _temp461.f2; if(( unsigned int) _temp479 > 1u?*(( int*)
_temp479) == Cyc_Absyn_Global_b: 0){ _LL482: _temp481=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp479)->f1; goto _LL466;} else{ goto
_LL467;} _LL467: _LL492: _temp491= _temp461.f1; if(( unsigned int) _temp491 > 1u?*((
int*) _temp491) == Cyc_Absyn_Global_b: 0){ _LL494: _temp493=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp491)->f1; goto _LL488;} else{ goto
_LL469;} _LL488: _temp487= _temp461.f2; if(( unsigned int) _temp487 > 1u?*(( int*)
_temp487) == Cyc_Absyn_Funname_b: 0){ _LL490: _temp489=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp487)->f1; goto _LL468;} else{ goto
_LL469;} _LL469: _LL500: _temp499= _temp461.f1; if(( unsigned int) _temp499 > 1u?*((
int*) _temp499) == Cyc_Absyn_Funname_b: 0){ _LL502: _temp501=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp499)->f1; goto _LL496;} else{ goto
_LL471;} _LL496: _temp495= _temp461.f2; if(( unsigned int) _temp495 > 1u?*(( int*)
_temp495) == Cyc_Absyn_Funname_b: 0){ _LL498: _temp497=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp495)->f1; goto _LL470;} else{ goto
_LL471;} _LL471: _LL508: _temp507= _temp461.f1; if(( unsigned int) _temp507 > 1u?*((
int*) _temp507) == Cyc_Absyn_Funname_b: 0){ _LL510: _temp509=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp507)->f1; goto _LL504;} else{ goto
_LL473;} _LL504: _temp503= _temp461.f2; if(( unsigned int) _temp503 > 1u?*(( int*)
_temp503) == Cyc_Absyn_Global_b: 0){ _LL506: _temp505=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp503)->f1; goto _LL472;} else{ goto
_LL473;} _LL473: goto _LL474; _LL464: return( void*) Cyc_Absyn_Unresolved_b;
_LL466: { struct Cyc_Absyn_Vardecl* d2= Cyc_Tcdecl_merge_vardecl( _temp485,
_temp481, loc, msg); if( d2 == 0){ return( void*) Cyc_Absyn_Unresolved_b;} if((
struct Cyc_Absyn_Vardecl*) _temp485 == d2){ return b0;} else{ if(( struct Cyc_Absyn_Vardecl*)
_temp481 == d2){ return b1;} else{ return( void*)({ struct Cyc_Absyn_Global_b_struct*
_temp511=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp511[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp512; _temp512.tag= Cyc_Absyn_Global_b;
_temp512.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_Absyn_Vardecl* _temp513=
d2; if( _temp513 == 0){ _throw( Null_Exception);} _temp513;}); _temp512;});
_temp511;});}}} _LL468: { int _temp519; void* _temp521; struct _tuple3 _temp517=
Cyc_Tcdecl_check_var_or_fn_decl(( void*) _temp493->sc,( void*) _temp493->type,
_temp493->tq, _temp493->attributes,( void*) _temp489->sc,( void*)({ struct Cyc_Core_Opt*
_temp514= _temp489->cached_typ; if( _temp514 == 0){ _throw( Null_Exception);}
_temp514->v;}), Cyc_Absyn_empty_tqual(), _temp489->attributes,( struct
_tagged_string)({ char* _temp515=( char*)"function"; struct _tagged_string
_temp516; _temp516.curr= _temp515; _temp516.base= _temp515; _temp516.last_plus_one=
_temp515 + 9; _temp516;}), Cyc_Absynpp_qvar2string( _temp493->name), loc, msg);
_LL522: _temp521= _temp517.f1; goto _LL520; _LL520: _temp519= _temp517.f2; goto
_LL518; _LL518: if( ! _temp519){ return( void*) Cyc_Absyn_Unresolved_b;} return
b1;} _LL470: Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp523= Cyc_Absynpp_qvar2string(
_temp497->name); xprintf("redefinition of function %.*s", _temp523.last_plus_one
- _temp523.curr, _temp523.curr);})); return( void*) Cyc_Absyn_Unresolved_b;
_LL472: { int _temp529; void* _temp531; struct _tuple3 _temp527= Cyc_Tcdecl_check_var_or_fn_decl((
void*) _temp509->sc,( void*)({ struct Cyc_Core_Opt* _temp524= _temp509->cached_typ;
if( _temp524 == 0){ _throw( Null_Exception);} _temp524->v;}), Cyc_Absyn_empty_tqual(),
_temp509->attributes,( void*) _temp505->sc,( void*) _temp505->type, _temp505->tq,
_temp505->attributes,( struct _tagged_string)({ char* _temp525=( char*)"variable";
struct _tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 9; _temp526;}), Cyc_Absynpp_qvar2string(
_temp509->name), loc, msg); _LL532: _temp531= _temp527.f1; goto _LL530; _LL530:
_temp529= _temp527.f2; goto _LL528; _LL528: if( ! _temp529){ return( void*) Cyc_Absyn_Unresolved_b;}
return b0;} _LL474:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp533=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp533[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp534; _temp534.tag= Cyc_Core_InvalidArg;
_temp534.f1=( struct _tagged_string)({ char* _temp535=( char*)"Tcdecl::merge_binding";
struct _tagged_string _temp536; _temp536.curr= _temp535; _temp536.base= _temp535;
_temp536.last_plus_one= _temp535 + 22; _temp536;}); _temp534;}); _temp533;}));
_LL462:;} struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_sub_structdecl( struct Cyc_Absyn_Structdecl*
ye, struct Cyc_Absyn_Structdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Structdecl* yt= Cyc_Tcdecl_merge_structdecl(
ye, y, loc, msg); if( yt !=( struct Cyc_Absyn_Structdecl*) ye){ if( yt == 0){
return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp538=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp537= y->name;
if( _temp537 == 0){ _throw( Null_Exception);} _temp537->v;})); xprintf("abstract struct %.*s is being used as non-abstract",
_temp538.last_plus_one - _temp538.curr, _temp538.curr);}));} return 0;} struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_sub_uniondecl( struct Cyc_Absyn_Uniondecl* ye,
struct Cyc_Absyn_Uniondecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Uniondecl* yt= Cyc_Tcdecl_merge_uniondecl(
ye, y, loc, msg); if( yt !=( struct Cyc_Absyn_Uniondecl*) ye){ if( yt == 0){
return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp540=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp539= y->name;
if( _temp539 == 0){ _throw( Null_Exception);} _temp539->v;})); xprintf("abstract union %.*s is being used as non-abstract",
_temp540.last_plus_one - _temp540.curr, _temp540.curr);}));} return 0;} struct
Cyc_Absyn_Enumdecl* Cyc_Tcdecl_sub_enumdecl( struct Cyc_Absyn_Enumdecl* ye,
struct Cyc_Absyn_Enumdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Enumdecl* yt= Cyc_Tcdecl_merge_enumdecl(
ye, y, loc, msg); if( yt !=( struct Cyc_Absyn_Enumdecl*) ye){ if( yt == 0){
return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp541=
Cyc_Absynpp_qvar2string( y->name); xprintf("abstract enum %.*s is being used as non-abstract",
_temp541.last_plus_one - _temp541.curr, _temp541.curr);}));} return 0;} struct
Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_sub_typedefdecl( struct Cyc_Absyn_Typedefdecl*
ye, struct Cyc_Absyn_Typedefdecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Typedefdecl* yt= Cyc_Tcdecl_merge_typedefdecl(
ye, y, loc, msg); if( yt !=( struct Cyc_Absyn_Typedefdecl*) ye){ if( yt == 0){
return 0;}* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp542=
Cyc_Absynpp_qvar2string( y->name); xprintf("abstract typedef %.*s is being used as non-abstract",
_temp542.last_plus_one - _temp542.curr, _temp542.curr);}));} return 0;} struct
Cyc_Absyn_Vardecl* Cyc_Tcdecl_sub_vardecl( struct Cyc_Absyn_Vardecl* ye, struct
Cyc_Absyn_Vardecl* y, struct Cyc_Position_Segment* loc, struct _tagged_string*
msg, int* res){ struct Cyc_Absyn_Vardecl* yt= Cyc_Tcdecl_merge_vardecl( ye, y,
loc, msg); if( yt !=( struct Cyc_Absyn_Vardecl*) ye){ if( yt == 0){ return 0;}*
res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string _temp543= Cyc_Absynpp_qvar2string(
y->name); xprintf("abstract var %.*s is being used as non-abstract", _temp543.last_plus_one
- _temp543.curr, _temp543.curr);}));} return 0;} void Cyc_Tcdecl_f(){;} struct
Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_sub_tuniondecl( struct Cyc_Absyn_Tuniondecl* ye,
struct Cyc_Absyn_Tuniondecl* y, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg, int* res){ struct Cyc_Absyn_Tuniondecl* yt= Cyc_Tcdecl_merge_tuniondecl(
ye, y, loc, msg); if( yt !=( struct Cyc_Absyn_Tuniondecl*) ye){ if( yt == 0){
return 0;} if( y->is_xtunion? ye->fields != 0: 0){ yt=({ struct Cyc_Absyn_Tuniondecl*
_temp544=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp544[ 0]=* y; _temp544;});({ struct Cyc_Absyn_Tuniondecl* _temp545= yt; if(
_temp545 == 0){ _throw( Null_Exception);} _temp545->fields=({ struct Cyc_Core_Opt*
_temp546=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp546->v=( void*) Cyc_Tcdecl_sub_xtunion_fields(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp547= ye->fields; if( _temp547 == 0){ _throw(
Null_Exception);} _temp547->v;}),( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp548= y->fields; if( _temp548 == 0){ _throw( Null_Exception);} _temp548->v;}),
Cyc_Tcdecl_build_tvs_map( ye->tvs, y->tvs), res,( struct _tagged_string)({ char*
_temp549=( char*)"xtunion"; struct _tagged_string _temp550; _temp550.curr=
_temp549; _temp550.base= _temp549; _temp550.last_plus_one= _temp549 + 8;
_temp550;}), Cyc_Absynpp_qvar2string( y->name), loc, msg); _temp546;});});
return yt;} else{* res= 0; Cyc_Tcdecl_merr( loc, msg,({ struct _tagged_string
_temp551= Cyc_Absynpp_qvar2string( y->name); xprintf("abstract [x]tunion %.*s is being used as non-abstract",
_temp551.last_plus_one - _temp551.curr, _temp551.curr);}));}} return 0;}