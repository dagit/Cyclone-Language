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
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
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
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ; char
Cyc_Absyn_EmptyAnnot[ 15u]="\000\000\000\000EmptyAnnot"; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern struct _tuple0* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_exn_typ;
extern void* Cyc_Absyn_string_typ( void* rgn); extern void* Cyc_Absyn_file_typ();
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern struct Cyc_Core_Opt* Cyc_Absyn_void_star_typ_opt(); extern void* Cyc_Absyn_strct(
struct _tagged_string* name); extern void* Cyc_Absyn_strctq( struct _tuple0*
name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name); extern void*
Cyc_Absyn_union_typ( struct _tagged_string* name); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp(
void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
char c, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp( struct _tagged_string s, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp(
struct _tuple0*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct
Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp(
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs,
int is_xtunion, struct Cyc_Position_Segment* loc); extern int Cyc_Absyn_is_format_prim(
void* p); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs,
struct Cyc_List_List*); extern void* Cyc_Absyn_pointer_expand( void*); extern
int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct _tagged_string*); extern
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct _tuple3{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; extern struct _tuple3* Cyc_Absyn_lookup_tuple_field( struct
Cyc_List_List*, int); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_String_zstrptrcmp( struct
_tagged_string*, struct _tagged_string*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[ 11u]; static
const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); static int Cyc_Absyn_zstrlist_cmp( struct Cyc_List_List* ss1, struct
Cyc_List_List* ss2){ return(( int(*)( int(* cmp)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)(
Cyc_String_zstrptrcmp, ss1, ss2);} int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*
vs1, struct Cyc_List_List* vs2){ return Cyc_Absyn_zstrlist_cmp( vs1, vs2);}
struct _tuple4{ void* f1; void* f2; } ; int Cyc_Absyn_qvar_cmp( struct _tuple0*
q1, struct _tuple0* q2){ void* n1=(* q1).f1; void* n2=(* q2).f1;{ struct _tuple4
_temp1=({ struct _tuple4 _temp0; _temp0.f1= n1; _temp0.f2= n2; _temp0;}); void*
_temp17; void* _temp19; void* _temp21; struct Cyc_List_List* _temp23; void*
_temp25; struct Cyc_List_List* _temp27; void* _temp29; struct Cyc_List_List*
_temp31; void* _temp33; struct Cyc_List_List* _temp35; void* _temp37; void*
_temp39; void* _temp41; void* _temp43; void* _temp45; void* _temp47; struct Cyc_List_List*
_temp49; void* _temp51; struct Cyc_List_List* _temp53; void* _temp55; _LL3:
_LL20: _temp19= _temp1.f1; if( _temp19 ==( void*) Cyc_Absyn_Loc_n){ goto _LL18;}
else{ goto _LL5;} _LL18: _temp17= _temp1.f2; if( _temp17 ==( void*) Cyc_Absyn_Loc_n){
goto _LL4;} else{ goto _LL5;} _LL5: _LL26: _temp25= _temp1.f1; if(( unsigned int)
_temp25 > 1u?*(( int*) _temp25) == Cyc_Absyn_Rel_n: 0){ _LL28: _temp27=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp25)->f1; goto _LL22;}
else{ goto _LL7;} _LL22: _temp21= _temp1.f2; if(( unsigned int) _temp21 > 1u?*((
int*) _temp21) == Cyc_Absyn_Rel_n: 0){ _LL24: _temp23=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp21)->f1; goto _LL6;} else{ goto _LL7;} _LL7:
_LL34: _temp33= _temp1.f1; if(( unsigned int) _temp33 > 1u?*(( int*) _temp33) ==
Cyc_Absyn_Abs_n: 0){ _LL36: _temp35=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp33)->f1; goto _LL30;} else{ goto _LL9;} _LL30: _temp29= _temp1.f2; if((
unsigned int) _temp29 > 1u?*(( int*) _temp29) == Cyc_Absyn_Abs_n: 0){ _LL32:
_temp31=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp29)->f1;
goto _LL8;} else{ goto _LL9;} _LL9: _LL40: _temp39= _temp1.f1; if( _temp39 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL38;} else{ goto _LL11;} _LL38: _temp37= _temp1.f2;
goto _LL10; _LL11: _LL44: _temp43= _temp1.f1; goto _LL42; _LL42: _temp41= _temp1.f2;
if( _temp41 ==( void*) Cyc_Absyn_Loc_n){ goto _LL12;} else{ goto _LL13;} _LL13:
_LL48: _temp47= _temp1.f1; if(( unsigned int) _temp47 > 1u?*(( int*) _temp47) ==
Cyc_Absyn_Rel_n: 0){ _LL50: _temp49=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp47)->f1; goto _LL46;} else{ goto _LL15;} _LL46: _temp45= _temp1.f2; goto
_LL14; _LL15: _LL56: _temp55= _temp1.f1; goto _LL52; _LL52: _temp51= _temp1.f2;
if(( unsigned int) _temp51 > 1u?*(( int*) _temp51) == Cyc_Absyn_Rel_n: 0){ _LL54:
_temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp51)->f1;
goto _LL16;} else{ goto _LL2;} _LL4: goto _LL2; _LL6: _temp35= _temp27; _temp31=
_temp23; goto _LL8; _LL8: { int i= Cyc_Absyn_zstrlist_cmp( _temp35, _temp31);
if( i != 0){ return i;} goto _LL2;} _LL10: return - 1; _LL12: return 1; _LL14:
return - 1; _LL16: return 1; _LL2:;} return Cyc_String_zstrptrcmp((* q1).f2,(*
q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct Cyc_Absyn_Tvar*
tv2){ return Cyc_String_zstrptrcmp( tv1->name, tv2->name);} static int Cyc_Absyn_new_type_counter=
0; void* Cyc_Absyn_new_evar( void* k){ return( void*)({ struct Cyc_Absyn_Evar_struct*
_temp57=( struct Cyc_Absyn_Evar_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Evar_struct));
_temp57[ 0]=({ struct Cyc_Absyn_Evar_struct _temp58; _temp58.tag= Cyc_Absyn_Evar;
_temp58.f1=( void*) k; _temp58.f2= 0; _temp58.f3=( Cyc_Absyn_new_type_counter ++);
_temp58;}); _temp57;});} void* Cyc_Absyn_wildtyp(){ return Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind);} struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y){ return({ struct Cyc_Absyn_Tqual
_temp59; _temp59.q_const= x.q_const? 1: y.q_const; _temp59.q_volatile= x.q_volatile?
1: y.q_volatile; _temp59.q_restrict= x.q_restrict? 1: y.q_restrict; _temp59;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp60; _temp60.q_const= 0; _temp60.q_volatile= 0; _temp60.q_restrict= 0;
_temp60;});} int Cyc_Absyn_is_format_prim( void* p){ void* _temp61= p; _LL63:
if( _temp61 ==( void*) Cyc_Absyn_Printf){ goto _LL64;} else{ goto _LL65;} _LL65:
if( _temp61 ==( void*) Cyc_Absyn_Fprintf){ goto _LL66;} else{ goto _LL67;} _LL67:
if( _temp61 ==( void*) Cyc_Absyn_Xprintf){ goto _LL68;} else{ goto _LL69;} _LL69:
if( _temp61 ==( void*) Cyc_Absyn_Scanf){ goto _LL70;} else{ goto _LL71;} _LL71:
if( _temp61 ==( void*) Cyc_Absyn_Fscanf){ goto _LL72;} else{ goto _LL73;} _LL73:
if( _temp61 ==( void*) Cyc_Absyn_Sscanf){ goto _LL74;} else{ goto _LL75;} _LL75:
goto _LL76; _LL64: return 1; _LL66: return 1; _LL68: return 1; _LL70: return 1;
_LL72: return 1; _LL74: return 1; _LL76: return 0; _LL62:;} struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x){ return({ struct Cyc_Absyn_Conref* _temp77=(
struct Cyc_Absyn_Conref*) GC_malloc( sizeof( struct Cyc_Absyn_Conref)); _temp77->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp78=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp78[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp79; _temp79.tag= Cyc_Absyn_Eq_constr; _temp79.f1=(
void*) x; _temp79;}); _temp78;})); _temp77;});} struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(){
return({ struct Cyc_Absyn_Conref* _temp80=( struct Cyc_Absyn_Conref*) GC_malloc(
sizeof( struct Cyc_Absyn_Conref)); _temp80->v=( void*)(( void*) Cyc_Absyn_No_constr);
_temp80;});} static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0u,( void*) 1}; static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={
0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v=( struct Cyc_Absyn_Conref){.v=(
void*)(( void*)& Cyc_Absyn_true_constraint)}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v=(
struct Cyc_Absyn_Conref){.v=( void*)(( void*)& Cyc_Absyn_false_constraint)};
struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v; struct
Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v; struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){ void* _temp83=( void*) x->v;
void* _temp91; struct Cyc_Absyn_Conref* _temp93; struct Cyc_Absyn_Conref**
_temp95; _LL85: if( _temp83 ==( void*) Cyc_Absyn_No_constr){ goto _LL86;} else{
goto _LL87;} _LL87: if(( unsigned int) _temp83 > 1u?*(( int*) _temp83) == Cyc_Absyn_Eq_constr:
0){ _LL92: _temp91=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp83)->f1;
goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp83 > 1u?*(( int*)
_temp83) == Cyc_Absyn_Forward_constr: 0){ _LL94: _temp93=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp83)->f1; _temp95=&(( struct Cyc_Absyn_Forward_constr_struct*)
_temp83)->f1; goto _LL90;} else{ goto _LL84;} _LL86: return x; _LL88: return x;
_LL90: { struct Cyc_Absyn_Conref* z=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)(* _temp95);* _temp95= z; return z;} _LL84:;} void*
Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x){ void* _temp96=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x))->v; void* _temp102; _LL98: if(( unsigned int) _temp96 > 1u?*(( int*) _temp96)
== Cyc_Absyn_Eq_constr: 0){ _LL103: _temp102=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp96)->f1; goto _LL99;} else{ goto _LL100;} _LL100: goto _LL101; _LL99:
return _temp102; _LL101: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp104=( char*)"conref_val"; struct
_tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 11; _temp105;})); _LL97:;} static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_uchar_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_ushort_tt={ 5u,( void*)(( void*) 1u),(
void*)(( void*) 1u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={
5u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulong_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 3u)}; void* Cyc_Absyn_uchar_t=(
void*)& Cyc_Absyn_uchar_tt; void* Cyc_Absyn_ushort_t=( void*)& Cyc_Absyn_ushort_tt;
void* Cyc_Absyn_uint_t=( void*)& Cyc_Absyn_uint_tt; void* Cyc_Absyn_ulong_t=(
void*)& Cyc_Absyn_ulong_tt; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={ 5u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 3u)}; void* Cyc_Absyn_schar_t=( void*)&
Cyc_Absyn_schar_tt; void* Cyc_Absyn_sshort_t=( void*)& Cyc_Absyn_sshort_tt; void*
Cyc_Absyn_sint_t=( void*)& Cyc_Absyn_sint_tt; void* Cyc_Absyn_slong_t=( void*)&
Cyc_Absyn_slong_tt; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=(
void*) 2u; static char _temp116[ 4u]="exn"; static struct _tagged_string Cyc_Absyn_exn_string=(
struct _tagged_string){ _temp116, _temp116, _temp116 + 4u}; static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_abs_null={ 1u, 0}; static struct _tuple0 Cyc_Absyn_exn_name_v=( struct
_tuple0){.f1=( void*)& Cyc_Absyn_abs_null,.f2=& Cyc_Absyn_exn_string}; struct
_tuple0* Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v; static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_xed_v=( struct Cyc_Absyn_Tuniondecl){.sc=( void*)(( void*) 3u),.name=&
Cyc_Absyn_exn_name_v,.tvs= 0,.fields= 0,.is_xtunion= 1}; static struct Cyc_Absyn_KnownTunion_struct
Cyc_Absyn_exn_tinfou={ 1u,& Cyc_Absyn_exn_xed_v}; struct Cyc_Absyn_Tuniondecl*
Cyc_Absyn_exn_xed=& Cyc_Absyn_exn_xed_v; static struct Cyc_Absyn_TunionType_struct
Cyc_Absyn_exn_typ_tt={ 2u,( struct Cyc_Absyn_TunionInfo){.tunion_info=( void*)((
void*)& Cyc_Absyn_exn_tinfou),.targs= 0,.rgn=( void*)(( void*) 3u)}}; void* Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt; static struct Cyc_Core_Opt* Cyc_Absyn_string_t_opt=
0; void* Cyc_Absyn_string_typ( void* rgn){ if( Cyc_Absyn_string_t_opt == 0){
void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_empty_tqual(),(
void*) Cyc_Absyn_Unknown_b); Cyc_Absyn_string_t_opt=({ struct Cyc_Core_Opt*
_temp120=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp120->v=( void*) t; _temp120;});} return( void*)({ struct Cyc_Core_Opt*
_temp121= Cyc_Absyn_string_t_opt; if( _temp121 == 0){ _throw( Null_Exception);}
_temp121->v;});} static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={ 2u,(
void*)(( void*) 0u), 1}; static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0u,( void*)(( void*)& Cyc_Absyn_one_intc)}; static struct Cyc_Absyn_Exp Cyc_Absyn_one_be=(
struct Cyc_Absyn_Exp){.topt= 0,.r=( void*)(( void*)& Cyc_Absyn_one_b_raw),.loc=
0}; static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={ 0u,& Cyc_Absyn_one_be};
void* Cyc_Absyn_bounds_one=( void*)& Cyc_Absyn_one_bt; void* Cyc_Absyn_starb_typ(
void* t, void* r, struct Cyc_Absyn_Tqual tq, void* b){ return( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp125=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp125[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp126; _temp126.tag= Cyc_Absyn_PointerType;
_temp126.f1=({ struct Cyc_Absyn_PtrInfo _temp127; _temp127.elt_typ=( void*) t;
_temp127.rgn_typ=( void*) r; _temp127.nullable= Cyc_Absyn_true_conref; _temp127.tq=
tq; _temp127.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp127;}); _temp126;}); _temp125;});} void* Cyc_Absyn_atb_typ( void* t,
void* r, struct Cyc_Absyn_Tqual tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp128=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp128[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp129; _temp129.tag= Cyc_Absyn_PointerType;
_temp129.f1=({ struct Cyc_Absyn_PtrInfo _temp130; _temp130.elt_typ=( void*) t;
_temp130.rgn_typ=( void*) r; _temp130.nullable= Cyc_Absyn_false_conref; _temp130.tq=
tq; _temp130.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp130;}); _temp129;}); _temp128;});} void* Cyc_Absyn_star_typ( void* t,
void* r, struct Cyc_Absyn_Tqual tq){ return Cyc_Absyn_starb_typ( t, r, tq, Cyc_Absyn_bounds_one);}
void* Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq){ return Cyc_Absyn_starb_typ(
t,( void*) Cyc_Absyn_HeapRgn, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_at_typ(
void* t, void* r, struct Cyc_Absyn_Tqual tq){ return Cyc_Absyn_atb_typ( t, r, tq,
Cyc_Absyn_bounds_one);} void* Cyc_Absyn_tagged_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp131=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp131[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp132; _temp132.tag= Cyc_Absyn_PointerType;
_temp132.f1=({ struct Cyc_Absyn_PtrInfo _temp133; _temp133.elt_typ=( void*) t;
_temp133.rgn_typ=( void*) r; _temp133.nullable= Cyc_Absyn_true_conref; _temp133.tq=
tq; _temp133.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_Unknown_b); _temp133;}); _temp132;}); _temp131;});} static
struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt= 0; static char _temp136[ 8u]="__sFILE";
static struct _tagged_string Cyc_Absyn_sf_str=( struct _tagged_string){ _temp136,
_temp136, _temp136 + 8u}; static struct _tagged_string* Cyc_Absyn_sf=& Cyc_Absyn_sf_str;
static char _temp139[ 4u]="Cyc"; static struct _tagged_string Cyc_Absyn_cyc_str=(
struct _tagged_string){ _temp139, _temp139, _temp139 + 4u}; static struct
_tagged_string* Cyc_Absyn_cyc=& Cyc_Absyn_cyc_str; static char _temp142[ 6u]="Stdio";
static struct _tagged_string Cyc_Absyn_st_str=( struct _tagged_string){ _temp142,
_temp142, _temp142 + 6u}; static struct _tagged_string* Cyc_Absyn_st=& Cyc_Absyn_st_str;
void* Cyc_Absyn_file_typ(){ if( Cyc_Absyn_file_t_opt == 0){ struct _tuple0*
file_t_name=({ struct _tuple0* _temp149=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp149->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp150=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp150[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp151; _temp151.tag= Cyc_Absyn_Abs_n;
_temp151.f1=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp152=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp152->hd=( void*)
Cyc_Absyn_cyc; _temp152->tl=({ struct Cyc_List_List* _temp153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp153->hd=( void*) Cyc_Absyn_st;
_temp153->tl= 0; _temp153;}); _temp152;}); _temp151;}); _temp150;}); _temp149->f2=
Cyc_Absyn_sf; _temp149;}); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp147=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp147->sc=( void*)(( void*) Cyc_Absyn_Abstract); _temp147->name=({ struct Cyc_Core_Opt*
_temp148=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp148->v=( void*) file_t_name; _temp148;}); _temp147->tvs= 0; _temp147->fields=
0; _temp147->attributes= 0; _temp147;}); void* file_struct_typ=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp144=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp144[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp145; _temp145.tag= Cyc_Absyn_StructType;
_temp145.f1=( struct _tuple0*) file_t_name; _temp145.f2= 0; _temp145.f3=({
struct Cyc_Absyn_Structdecl** _temp146=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp146[ 0]= sd;
_temp146;}); _temp145;}); _temp144;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp143=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp143->v=( void*) Cyc_Absyn_at_typ( file_struct_typ,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp143;});} return( void*)({ struct Cyc_Core_Opt*
_temp154= Cyc_Absyn_file_t_opt; if( _temp154 == 0){ _throw( Null_Exception);}
_temp154->v;});} static struct Cyc_Core_Opt* Cyc_Absyn_void_star_t_opt= 0; void*
Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt == 0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp155=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp155->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp155;});} return( void*)({
struct Cyc_Core_Opt* _temp156= Cyc_Absyn_void_star_t_opt; if( _temp156 == 0){
_throw( Null_Exception);} _temp156->v;});} struct Cyc_Core_Opt* Cyc_Absyn_void_star_typ_opt(){
if( Cyc_Absyn_void_star_t_opt == 0){ Cyc_Absyn_void_star_t_opt=({ struct Cyc_Core_Opt*
_temp157=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp157->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp157;});} return Cyc_Absyn_void_star_t_opt;} void*
Cyc_Absyn_strct( struct _tagged_string* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp158=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp158[ 0]=({ struct Cyc_Absyn_StructType_struct _temp159; _temp159.tag= Cyc_Absyn_StructType;
_temp159.f1=({ struct _tuple0* _temp160=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp160[ 0]=({ struct _tuple0 _temp161; _temp161.f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp162=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp162[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp163; _temp163.tag= Cyc_Absyn_Abs_n; _temp163.f1= 0; _temp163;}); _temp162;});
_temp161.f2= name; _temp161;}); _temp160;}); _temp159.f2= 0; _temp159.f3= 0;
_temp159;}); _temp158;});} void* Cyc_Absyn_union_typ( struct _tagged_string*
name){ return( void*)({ struct Cyc_Absyn_UnionType_struct* _temp164=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp164[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp165; _temp165.tag= Cyc_Absyn_UnionType; _temp165.f1=({
struct _tuple0* _temp166=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp166[ 0]=({ struct _tuple0 _temp167; _temp167.f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp168=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp168[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp169; _temp169.tag= Cyc_Absyn_Abs_n;
_temp169.f1= 0; _temp169;}); _temp168;}); _temp167.f2= name; _temp167;});
_temp166;}); _temp165.f2= 0; _temp165.f3= 0; _temp165;}); _temp164;});} void*
Cyc_Absyn_strctq( struct _tuple0* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp170=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp170[ 0]=({ struct Cyc_Absyn_StructType_struct _temp171; _temp171.tag= Cyc_Absyn_StructType;
_temp171.f1=( struct _tuple0*) name; _temp171.f2= 0; _temp171.f3= 0; _temp171;});
_temp170;});} void* Cyc_Absyn_unionq_typ( struct _tuple0* name){ return( void*)({
struct Cyc_Absyn_UnionType_struct* _temp172=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp172[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp173; _temp173.tag= Cyc_Absyn_UnionType; _temp173.f1=(
struct _tuple0*) name; _temp173.f2= 0; _temp173.f3= 0; _temp173;}); _temp172;});}
struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp174=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp174->topt= 0; _temp174->r=( void*) r;
_temp174->loc= loc; _temp174;});} struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_New_e_struct*
_temp175=( struct Cyc_Absyn_New_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_New_e_struct));
_temp175[ 0]=({ struct Cyc_Absyn_New_e_struct _temp176; _temp176.tag= Cyc_Absyn_New_e;
_temp176.f1= rgn_handle; _temp176.f2= e; _temp176;}); _temp175;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp* e){ return({ struct Cyc_Absyn_Exp*
_temp177=( struct Cyc_Absyn_Exp*) GC_malloc( sizeof( struct Cyc_Absyn_Exp));
_temp177->topt= e->topt; _temp177->r=( void*)(( void*) e->r); _temp177->loc= e->loc;
_temp177;});} struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void* c, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp178=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp178[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp179; _temp179.tag= Cyc_Absyn_Const_e;
_temp179.f1=( void*) c; _temp179;}); _temp178;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp180=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp180[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp181; _temp181.tag= Cyc_Absyn_Const_e; _temp181.f1=( void*)(( void*) Cyc_Absyn_Null_c);
_temp181;}); _temp180;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*
s, int i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp182=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp182[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp183; _temp183.tag= Cyc_Absyn_Int_c; _temp183.f1=( void*) s; _temp183.f2= i;
_temp183;}); _temp182;}), seg);} struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
i, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Unsigned,( int) i, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_signed_int_exp( b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_bool_exp( 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(( void*)({ struct
Cyc_Absyn_Char_c_struct* _temp184=( struct Cyc_Absyn_Char_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Char_c_struct)); _temp184[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp185; _temp185.tag= Cyc_Absyn_Char_c; _temp185.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp185.f2= c; _temp185;}); _temp184;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Float_c_struct* _temp186=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp186[ 0]=({ struct Cyc_Absyn_Float_c_struct
_temp187; _temp187.tag= Cyc_Absyn_Float_c; _temp187.f1= f; _temp187;}); _temp186;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(( void*)({ struct
Cyc_Absyn_String_c_struct* _temp188=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp188[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp189; _temp189.tag= Cyc_Absyn_String_c; _temp189.f1=
s; _temp189;}); _temp188;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp190=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp190[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp191; _temp191.tag= Cyc_Absyn_Var_e; _temp191.f1= q; _temp191.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp191;}); _temp190;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp( struct _tuple0* q, void* b, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp192=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp192[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp193; _temp193.tag= Cyc_Absyn_Var_e;
_temp193.f1= q; _temp193.f2=( void*) b; _temp193;}); _temp192;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0* q, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp194=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp194[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp195; _temp195.tag= Cyc_Absyn_UnknownId_e;
_temp195.f1= q; _temp195;}); _temp194;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp196=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp196[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp197; _temp197.tag= Cyc_Absyn_Primop_e; _temp197.f1=(
void*) p; _temp197.f2= es; _temp197;}); _temp196;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp198=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp198->hd=( void*)
e; _temp198->tl= 0; _temp198;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp199=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp199->hd=( void*)
e1; _temp199->tl=({ struct Cyc_List_List* _temp200=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp200->hd=( void*) e2; _temp200->tl=
0; _temp200;}); _temp199;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Eq,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Neq, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gt, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lt,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Gte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lte, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
popt, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_AssignOp_e_struct* _temp201=( struct Cyc_Absyn_AssignOp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp201[ 0]=({ struct
Cyc_Absyn_AssignOp_e_struct _temp202; _temp202.tag= Cyc_Absyn_AssignOp_e;
_temp202.f1= e1; _temp202.f2= popt; _temp202.f3= e2; _temp202;}); _temp201;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp(
e1, 0, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp( struct Cyc_Absyn_Exp*
e, void* i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp203=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp203[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp204; _temp204.tag= Cyc_Absyn_Increment_e;
_temp204.f1= e; _temp204.f2=( void*) i; _temp204;}); _temp203;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostInc, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreInc, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreDec, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostDec, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Conditional_e_struct*
_temp205=( struct Cyc_Absyn_Conditional_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp205[ 0]=({ struct Cyc_Absyn_Conditional_e_struct _temp206; _temp206.tag=
Cyc_Absyn_Conditional_e; _temp206.f1= e1; _temp206.f2= e2; _temp206.f3= e3;
_temp206;}); _temp205;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_SeqExp_e_struct* _temp207=( struct Cyc_Absyn_SeqExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp207[ 0]=({ struct
Cyc_Absyn_SeqExp_e_struct _temp208; _temp208.tag= Cyc_Absyn_SeqExp_e; _temp208.f1=
e1; _temp208.f2= e2; _temp208;}); _temp207;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownCall_e_struct*
_temp209=( struct Cyc_Absyn_UnknownCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct));
_temp209[ 0]=({ struct Cyc_Absyn_UnknownCall_e_struct _temp210; _temp210.tag=
Cyc_Absyn_UnknownCall_e; _temp210.f1= e; _temp210.f2= es; _temp210;}); _temp209;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp211=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp211[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp212; _temp212.tag= Cyc_Absyn_FnCall_e; _temp212.f1=
e; _temp212.f2= es; _temp212;}); _temp211;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_NoInstantiate_e_struct* _temp213=( struct Cyc_Absyn_NoInstantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NoInstantiate_e_struct)); _temp213[ 0]=({
struct Cyc_Absyn_NoInstantiate_e_struct _temp214; _temp214.tag= Cyc_Absyn_NoInstantiate_e;
_temp214.f1= e; _temp214;}); _temp213;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp215=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct));
_temp215[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct _temp216; _temp216.tag=
Cyc_Absyn_Instantiate_e; _temp216.f1= e; _temp216.f2= ts; _temp216;}); _temp215;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void* t, struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_Cast_e_struct* _temp217=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp217[ 0]=({ struct Cyc_Absyn_Cast_e_struct
_temp218; _temp218.tag= Cyc_Absyn_Cast_e; _temp218.f1=( void*) t; _temp218.f2= e;
_temp218;}); _temp217;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Throw_e_struct* _temp219=( struct Cyc_Absyn_Throw_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Throw_e_struct)); _temp219[ 0]=({ struct Cyc_Absyn_Throw_e_struct
_temp220; _temp220.tag= Cyc_Absyn_Throw_e; _temp220.f1= e; _temp220;}); _temp219;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_Address_e_struct* _temp221=( struct Cyc_Absyn_Address_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Address_e_struct)); _temp221[ 0]=({ struct
Cyc_Absyn_Address_e_struct _temp222; _temp222.tag= Cyc_Absyn_Address_e; _temp222.f1=
e; _temp222;}); _temp221;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct* _temp223=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp223[ 0]=({ struct
Cyc_Absyn_Sizeoftyp_e_struct _temp224; _temp224.tag= Cyc_Absyn_Sizeoftyp_e;
_temp224.f1=( void*) t; _temp224;}); _temp223;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Sizeofexp_e_struct*
_temp225=( struct Cyc_Absyn_Sizeofexp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeofexp_e_struct));
_temp225[ 0]=({ struct Cyc_Absyn_Sizeofexp_e_struct _temp226; _temp226.tag= Cyc_Absyn_Sizeofexp_e;
_temp226.f1= e; _temp226;}); _temp225;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Deref_e_struct* _temp227=( struct Cyc_Absyn_Deref_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Deref_e_struct)); _temp227[ 0]=({ struct Cyc_Absyn_Deref_e_struct
_temp228; _temp228.tag= Cyc_Absyn_Deref_e; _temp228.f1= e; _temp228;}); _temp227;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp* e,
struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StructMember_e_struct* _temp229=( struct Cyc_Absyn_StructMember_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructMember_e_struct)); _temp229[ 0]=({
struct Cyc_Absyn_StructMember_e_struct _temp230; _temp230.tag= Cyc_Absyn_StructMember_e;
_temp230.f1= e; _temp230.f2= n; _temp230;}); _temp229;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_StructArrow_e_struct* _temp231=( struct Cyc_Absyn_StructArrow_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct)); _temp231[ 0]=({
struct Cyc_Absyn_StructArrow_e_struct _temp232; _temp232.tag= Cyc_Absyn_StructArrow_e;
_temp232.f1= e; _temp232.f2= n; _temp232;}); _temp231;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp( Cyc_Absyn_structmember_exp(
e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Subscript_e_struct* _temp233=( struct Cyc_Absyn_Subscript_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Subscript_e_struct)); _temp233[ 0]=({ struct
Cyc_Absyn_Subscript_e_struct _temp234; _temp234.tag= Cyc_Absyn_Subscript_e;
_temp234.f1= e1; _temp234.f2= e2; _temp234;}); _temp233;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Tuple_e_struct* _temp235=(
struct Cyc_Absyn_Tuple_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp235[ 0]=({ struct Cyc_Absyn_Tuple_e_struct _temp236; _temp236.tag= Cyc_Absyn_Tuple_e;
_temp236.f1= es; _temp236;}); _temp235;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StmtExp_e_struct* _temp237=( struct Cyc_Absyn_StmtExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct)); _temp237[ 0]=({ struct
Cyc_Absyn_StmtExp_e_struct _temp238; _temp238.tag= Cyc_Absyn_StmtExp_e; _temp238.f1=
s; _temp238;}); _temp237;}), loc);} static char _temp241[ 15u]="Null_Exception";
static struct _tagged_string Cyc_Absyn_ne_str=( struct _tagged_string){ _temp241,
_temp241, _temp241 + 15u}; static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_top_ns={
1u, 0}; static struct _tuple0 Cyc_Absyn_ne=( struct _tuple0){.f1=( void*)& Cyc_Absyn_top_ns,.f2=&
Cyc_Absyn_ne_str}; struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_var_exp(& Cyc_Absyn_ne, loc);}
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct
Cyc_Absyn_Exp* Cyc_Absyn_array_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* dles= 0; for( 0; es != 0; es=({ struct Cyc_List_List*
_temp243= es; if( _temp243 == 0){ _throw( Null_Exception);} _temp243->tl;})){
dles=({ struct Cyc_List_List* _temp244=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp244->hd=( void*)({ struct _tuple5* _temp245=(
struct _tuple5*) GC_malloc( sizeof( struct _tuple5)); _temp245->f1= 0; _temp245->f2=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp246= es; if( _temp246 == 0){
_throw( Null_Exception);} _temp246->hd;}); _temp245;}); _temp244->tl= dles;
_temp244;});} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
dles); return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp247=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp247[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp248; _temp248.tag= Cyc_Absyn_Array_e;
_temp248.f1= dles; _temp248;}); _temp247;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp249=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp249[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp250; _temp250.tag= Cyc_Absyn_UnresolvedMem_e; _temp250.f1= n; _temp250.f2=
dles; _temp250;}); _temp249;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Stmt*
_temp251=( struct Cyc_Absyn_Stmt*) GC_malloc( sizeof( struct Cyc_Absyn_Stmt));
_temp251->r=( void*) s; _temp251->loc= loc; _temp251->non_local_preds= 0;
_temp251->try_depth= 0; _temp251->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot);
_temp251;});} struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*) Cyc_Absyn_Skip_s, loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Exp_s_struct* _temp252=(
struct Cyc_Absyn_Exp_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Exp_s_struct));
_temp252[ 0]=({ struct Cyc_Absyn_Exp_s_struct _temp253; _temp253.tag= Cyc_Absyn_Exp_s;
_temp253.f1= e; _temp253;}); _temp252;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List* ss, struct Cyc_Position_Segment* loc){ if( ss == 0){
return Cyc_Absyn_skip_stmt( loc);} else{ if(({ struct Cyc_List_List* _temp254=
ss; if( _temp254 == 0){ _throw( Null_Exception);} _temp254->tl;}) == 0){ return(
struct Cyc_Absyn_Stmt*)({ struct Cyc_List_List* _temp255= ss; if( _temp255 == 0){
_throw( Null_Exception);} _temp255->hd;});} else{ return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)({ struct Cyc_List_List* _temp256= ss; if( _temp256 == 0){
_throw( Null_Exception);} _temp256->hd;}), Cyc_Absyn_seq_stmts(({ struct Cyc_List_List*
_temp257= ss; if( _temp257 == 0){ _throw( Null_Exception);} _temp257->tl;}), loc),
loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Return_s_struct* _temp258=( struct Cyc_Absyn_Return_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Return_s_struct)); _temp258[ 0]=({ struct
Cyc_Absyn_Return_s_struct _temp259; _temp259.tag= Cyc_Absyn_Return_s; _temp259.f1=
e; _temp259;}); _temp258;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_IfThenElse_s_struct* _temp260=( struct Cyc_Absyn_IfThenElse_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct)); _temp260[ 0]=({
struct Cyc_Absyn_IfThenElse_s_struct _temp261; _temp261.tag= Cyc_Absyn_IfThenElse_s;
_temp261.f1= e; _temp261.f2= s1; _temp261.f3= s2; _temp261;}); _temp260;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_While_s_struct* _temp262=( struct Cyc_Absyn_While_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_While_s_struct)); _temp262[ 0]=({ struct Cyc_Absyn_While_s_struct
_temp263; _temp263.tag= Cyc_Absyn_While_s; _temp263.f1=({ struct _tuple2
_temp264; _temp264.f1= e; _temp264.f2= Cyc_Absyn_skip_stmt( e->loc); _temp264;});
_temp263.f2= s; _temp263;}); _temp262;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Break_s_struct* _temp265=( struct Cyc_Absyn_Break_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Break_s_struct)); _temp265[ 0]=({ struct Cyc_Absyn_Break_s_struct
_temp266; _temp266.tag= Cyc_Absyn_Break_s; _temp266.f1= 0; _temp266;}); _temp265;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Continue_s_struct*
_temp267=( struct Cyc_Absyn_Continue_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp267[ 0]=({ struct Cyc_Absyn_Continue_s_struct _temp268; _temp268.tag= Cyc_Absyn_Continue_s;
_temp268.f1= 0; _temp268;}); _temp267;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_For_s_struct* _temp269=( struct Cyc_Absyn_For_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_For_s_struct)); _temp269[ 0]=({ struct Cyc_Absyn_For_s_struct
_temp270; _temp270.tag= Cyc_Absyn_For_s; _temp270.f1= e1; _temp270.f2=({ struct
_tuple2 _temp271; _temp271.f1= e2; _temp271.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp271;}); _temp270.f3=({ struct _tuple2 _temp272; _temp272.f1= e3; _temp272.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp272;}); _temp270.f4= s; _temp270;});
_temp269;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Switch_s_struct* _temp273=( struct Cyc_Absyn_Switch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp273[ 0]=({ struct
Cyc_Absyn_Switch_s_struct _temp274; _temp274.tag= Cyc_Absyn_Switch_s; _temp274.f1=
e; _temp274.f2= scs; _temp274;}); _temp273;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc){ void* _temp275=( void*) s1->r; _LL277: if( _temp275 ==( void*) Cyc_Absyn_Skip_s){
goto _LL278;} else{ goto _LL279;} _LL279: goto _LL280; _LL278: return s2; _LL280:
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Seq_s_struct* _temp281=(
struct Cyc_Absyn_Seq_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Seq_s_struct));
_temp281[ 0]=({ struct Cyc_Absyn_Seq_s_struct _temp282; _temp282.tag= Cyc_Absyn_Seq_s;
_temp282.f1= s1; _temp282.f2= s2; _temp282;}); _temp281;}), loc); _LL276:;}
struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Fallthru_s_struct*
_temp283=( struct Cyc_Absyn_Fallthru_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct));
_temp283[ 0]=({ struct Cyc_Absyn_Fallthru_s_struct _temp284; _temp284.tag= Cyc_Absyn_Fallthru_s;
_temp284.f1= el; _temp284.f2= 0; _temp284;}); _temp283;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp285=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp285[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp286; _temp286.tag= Cyc_Absyn_Decl_s;
_temp286.f1= d; _temp286.f2= s; _temp286;}); _temp285;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl*
d= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp289=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp289[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp290; _temp290.tag= Cyc_Absyn_Var_d;
_temp290.f1= Cyc_Absyn_new_vardecl( x, t, init); _temp290;}); _temp289;}), loc);
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp287=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp287[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp288; _temp288.tag= Cyc_Absyn_Decl_s;
_temp288.f1= d; _temp288.f2= s; _temp288;}); _temp287;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct* _temp291=(
struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp291[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp292; _temp292.tag= Cyc_Absyn_Cut_s;
_temp292.f1= s; _temp292;}); _temp291;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp293=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp293[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp294; _temp294.tag= Cyc_Absyn_Splice_s; _temp294.f1=
s; _temp294;}); _temp293;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp295=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp295[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp296; _temp296.tag= Cyc_Absyn_Label_s;
_temp296.f1= v; _temp296.f2= s; _temp296;}); _temp295;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Do_s_struct*
_temp297=( struct Cyc_Absyn_Do_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp297[ 0]=({ struct Cyc_Absyn_Do_s_struct _temp298; _temp298.tag= Cyc_Absyn_Do_s;
_temp298.f1= s; _temp298.f2=({ struct _tuple2 _temp299; _temp299.f1= e; _temp299.f2=
Cyc_Absyn_skip_stmt( e->loc); _temp299;}); _temp298;}); _temp297;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_TryCatch_s_struct* _temp300=( struct Cyc_Absyn_TryCatch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct)); _temp300[ 0]=({ struct
Cyc_Absyn_TryCatch_s_struct _temp301; _temp301.tag= Cyc_Absyn_TryCatch_s;
_temp301.f1= s; _temp301.f2= scs; _temp301;}); _temp300;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Goto_s_struct*
_temp302=( struct Cyc_Absyn_Goto_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Goto_s_struct));
_temp302[ 0]=({ struct Cyc_Absyn_Goto_s_struct _temp303; _temp303.tag= Cyc_Absyn_Goto_s;
_temp303.f1= lab; _temp303.f2= 0; _temp303;}); _temp302;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp304=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp304->r=( void*) p; _temp304->topt=
0; _temp304->loc= s; _temp304;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp305=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp305->r=( void*) r; _temp305->loc= loc; _temp305;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Let_d_struct* _temp306=( struct Cyc_Absyn_Let_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp306[ 0]=({ struct Cyc_Absyn_Let_d_struct
_temp307; _temp307.tag= Cyc_Absyn_Let_d; _temp307.f1= p; _temp307.f2= 0;
_temp307.f3= t_opt; _temp307.f4= e; _temp307.f5= 0; _temp307;}); _temp306;}),
loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void*
t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl* _temp308=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp308->sc=( void*)(( void*) Cyc_Absyn_Public); _temp308->name= x; _temp308->tq=
Cyc_Absyn_empty_tqual(); _temp308->type=( void*) t; _temp308->initializer= init;
_temp308->rgn= 0; _temp308->attributes= 0; _temp308;});} struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){
return({ struct Cyc_Absyn_Vardecl* _temp309=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp309->sc=( void*)(( void*)
Cyc_Absyn_Static); _temp309->name= x; _temp309->tq= Cyc_Absyn_empty_tqual();
_temp309->type=( void*) t; _temp309->initializer= init; _temp309->rgn= 0;
_temp309->attributes= 0; _temp309;});} struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp310=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp310[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp311; _temp311.tag= Cyc_Absyn_Struct_d; _temp311.f1=({
struct Cyc_Absyn_Structdecl* _temp312=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp312->sc=( void*) s; _temp312->name=
n; _temp312->tvs= ts; _temp312->fields= fs; _temp312->attributes= atts; _temp312;});
_temp311;}); _temp310;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp313=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp313[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp314; _temp314.tag= Cyc_Absyn_Union_d; _temp314.f1=({ struct Cyc_Absyn_Uniondecl*
_temp315=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp315->sc=( void*) s; _temp315->name= n; _temp315->tvs= ts; _temp315->fields=
fs; _temp315->attributes= atts; _temp315;}); _temp314;}); _temp313;}), loc);}
struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct
Cyc_List_List* ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp316=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp316[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp317; _temp317.tag= Cyc_Absyn_Tunion_d;
_temp317.f1=({ struct Cyc_Absyn_Tuniondecl* _temp318=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp318->sc=( void*) s;
_temp318->name= n; _temp318->tvs= ts; _temp318->fields= fs; _temp318->is_xtunion=
is_xtunion; _temp318;}); _temp317;}); _temp316;}), loc);} static struct _tuple1*
Cyc_Absyn_expand_arg( struct _tuple1* a){ return({ struct _tuple1* _temp319=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp319->f1=(* a).f1;
_temp319->f2=(* a).f2; _temp319->f3= Cyc_Absyn_pointer_expand((* a).f3);
_temp319;});} void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs,
struct Cyc_List_List* atts){ return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp320=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp320[ 0]=({ struct Cyc_Absyn_FnType_struct _temp321; _temp321.tag= Cyc_Absyn_FnType;
_temp321.f1=({ struct Cyc_Absyn_FnInfo _temp322; _temp322.tvars= tvs; _temp322.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp322.effect= eff_typ; _temp322.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp322.varargs= varargs;
_temp322.attributes= atts; _temp322;}); _temp321;}); _temp320;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp323= t; struct Cyc_Absyn_FnInfo _temp329; _LL325: if((
unsigned int) _temp323 > 4u?*(( int*) _temp323) == Cyc_Absyn_FnType: 0){ _LL330:
_temp329=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp323)->f1;
goto _LL326;} else{ goto _LL327;} _LL327: goto _LL328; _LL326: return Cyc_Absyn_at_typ(
t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _LL328: return t; _LL324:;}
int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp* e){ void* _temp331=( void*) e->r;
void* _temp351; struct Cyc_Absyn_Fndecl* _temp353; struct _tuple0* _temp355;
void* _temp357; struct Cyc_Absyn_Vardecl* _temp359; struct _tuple0* _temp361;
void* _temp363; struct Cyc_Absyn_Vardecl* _temp365; struct _tuple0* _temp367;
void* _temp369; struct _tuple0* _temp371; struct Cyc_Absyn_Exp* _temp373; struct
Cyc_Absyn_Exp* _temp375; struct _tagged_string* _temp377; struct Cyc_Absyn_Exp*
_temp379; struct _tagged_string* _temp381; struct Cyc_Absyn_Exp* _temp383;
struct Cyc_Absyn_Exp* _temp385; _LL333: if(*(( int*) _temp331) == Cyc_Absyn_Var_e){
_LL356: _temp355=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp331)->f1;
goto _LL352; _LL352: _temp351=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp331)->f2; if(( unsigned int) _temp351 > 1u?*(( int*) _temp351) == Cyc_Absyn_Funname_b:
0){ _LL354: _temp353=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp351)->f1; goto _LL334;} else{ goto _LL335;}} else{ goto _LL335;} _LL335:
if(*(( int*) _temp331) == Cyc_Absyn_Var_e){ _LL362: _temp361=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp331)->f1; goto _LL358; _LL358: _temp357=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp331)->f2; if(( unsigned int)
_temp357 > 1u?*(( int*) _temp357) == Cyc_Absyn_Global_b: 0){ _LL360: _temp359=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp357)->f1;
goto _LL336;} else{ goto _LL337;}} else{ goto _LL337;} _LL337: if(*(( int*)
_temp331) == Cyc_Absyn_Var_e){ _LL368: _temp367=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp331)->f1; goto _LL364; _LL364: _temp363=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp331)->f2; if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Local_b:
0){ _LL366: _temp365=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp363)->f1; goto _LL338;} else{ goto _LL339;}} else{ goto _LL339;} _LL339:
if(*(( int*) _temp331) == Cyc_Absyn_Var_e){ _LL372: _temp371=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp331)->f1; goto _LL370; _LL370: _temp369=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp331)->f2; goto _LL340;} else{ goto
_LL341;} _LL341: if(*(( int*) _temp331) == Cyc_Absyn_Subscript_e){ _LL376:
_temp375=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp331)->f1; goto _LL374; _LL374: _temp373=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp331)->f2; goto _LL342;} else{ goto _LL343;}
_LL343: if(*(( int*) _temp331) == Cyc_Absyn_StructMember_e){ _LL380: _temp379=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp331)->f1;
goto _LL378; _LL378: _temp377=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp331)->f2; goto _LL344;} else{ goto _LL345;} _LL345: if(*(( int*) _temp331)
== Cyc_Absyn_StructArrow_e){ _LL384: _temp383=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp331)->f1; goto _LL382; _LL382: _temp381=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp331)->f2;
goto _LL346;} else{ goto _LL347;} _LL347: if(*(( int*) _temp331) == Cyc_Absyn_Deref_e){
_LL386: _temp385=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp331)->f1; goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL334:
return 0; _LL336: _temp365= _temp359; goto _LL338; _LL338: { void* _temp387= Cyc_Tcutil_compress((
void*) _temp365->type); struct Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Tqual
_temp395; void* _temp397; _LL389: if(( unsigned int) _temp387 > 4u?*(( int*)
_temp387) == Cyc_Absyn_ArrayType: 0){ _LL398: _temp397=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp387)->f1; goto _LL396; _LL396: _temp395=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp387)->f2; goto _LL394; _LL394: _temp393=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp387)->f3; goto
_LL390;} else{ goto _LL391;} _LL391: goto _LL392; _LL390: return 0; _LL392:
return 1; _LL388:;} _LL340: return 1; _LL342: return 1; _LL344: return Cyc_Absyn_is_lvalue(
_temp379); _LL346: return 1; _LL348: return 1; _LL350: return 0; _LL332:;}
struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*
fields, struct _tagged_string* v){{ struct Cyc_List_List* fs= fields; for( 0; fs
!= 0; fs=({ struct Cyc_List_List* _temp399= fs; if( _temp399 == 0){ _throw(
Null_Exception);} _temp399->tl;})){ struct Cyc_Absyn_Structfield* f=( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp400= fs; if( _temp400 == 0){ _throw( Null_Exception);}
_temp400->hd;}); if( Cyc_String_zstrptrcmp( f->name, v) == 0){ return( struct
Cyc_Absyn_Structfield*) f;}}} return 0;} struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl* sd, struct _tagged_string* v){ return sd->fields ==
0? 0: Cyc_Absyn_lookup_field(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp401= sd->fields; if( _temp401 == 0){ _throw( Null_Exception);} _temp401->v;}),
v);} struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*
ud, struct _tagged_string* v){ return ud->fields == 0? 0: Cyc_Absyn_lookup_field((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp402= ud->fields; if( _temp402
== 0){ _throw( Null_Exception);} _temp402->v;}), v);} struct _tuple3* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List* ts, int i){ for( 0; i != 0; -- i){ if( ts == 0){ return 0;}
ts=({ struct Cyc_List_List* _temp403= ts; if( _temp403 == 0){ _throw(
Null_Exception);} _temp403->tl;});} if( ts == 0){ return 0;} return( struct
_tuple3*)(( struct _tuple3*)({ struct Cyc_List_List* _temp404= ts; if( _temp404
== 0){ _throw( Null_Exception);} _temp404->hd;}));} struct _tagged_string Cyc_Absyn_attribute2string(
void* a){ void* _temp405= a; int _temp443; int _temp445; struct _tagged_string
_temp447; _LL407: if(( unsigned int) _temp405 > 15u?*(( int*) _temp405) == Cyc_Absyn_Regparm_att:
0){ _LL444: _temp443=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp405)->f1;
goto _LL408;} else{ goto _LL409;} _LL409: if( _temp405 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL410;} else{ goto _LL411;} _LL411: if( _temp405 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL412;} else{ goto _LL413;} _LL413: if( _temp405 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL414;} else{ goto _LL415;} _LL415: if( _temp405 ==( void*) Cyc_Absyn_Const_att){
goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int) _temp405 > 15u?*((
int*) _temp405) == Cyc_Absyn_Aligned_att: 0){ _LL446: _temp445=( int)(( struct
Cyc_Absyn_Aligned_att_struct*) _temp405)->f1; goto _LL418;} else{ goto _LL419;}
_LL419: if( _temp405 ==( void*) Cyc_Absyn_Packed_att){ goto _LL420;} else{ goto
_LL421;} _LL421: if(( unsigned int) _temp405 > 15u?*(( int*) _temp405) == Cyc_Absyn_Section_att:
0){ _LL448: _temp447=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp405)->f1; goto _LL422;} else{ goto _LL423;} _LL423: if( _temp405 ==( void*)
Cyc_Absyn_Nocommon_att){ goto _LL424;} else{ goto _LL425;} _LL425: if( _temp405
==( void*) Cyc_Absyn_Shared_att){ goto _LL426;} else{ goto _LL427;} _LL427: if(
_temp405 ==( void*) Cyc_Absyn_Unused_att){ goto _LL428;} else{ goto _LL429;}
_LL429: if( _temp405 ==( void*) Cyc_Absyn_Weak_att){ goto _LL430;} else{ goto
_LL431;} _LL431: if( _temp405 ==( void*) Cyc_Absyn_Dllimport_att){ goto _LL432;}
else{ goto _LL433;} _LL433: if( _temp405 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL434;} else{ goto _LL435;} _LL435: if( _temp405 ==( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL436;} else{ goto _LL437;} _LL437: if( _temp405 ==( void*) Cyc_Absyn_Constructor_att){
goto _LL438;} else{ goto _LL439;} _LL439: if( _temp405 ==( void*) Cyc_Absyn_Destructor_att){
goto _LL440;} else{ goto _LL441;} _LL441: if( _temp405 ==( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL442;} else{ goto _LL406;} _LL408: return xprintf("regparm(%d)", _temp443);
_LL410: return( struct _tagged_string)({ char* _temp449=( char*)"stdcall";
struct _tagged_string _temp450; _temp450.curr= _temp449; _temp450.base= _temp449;
_temp450.last_plus_one= _temp449 + 8; _temp450;}); _LL412: return( struct
_tagged_string)({ char* _temp451=( char*)"cdecl"; struct _tagged_string _temp452;
_temp452.curr= _temp451; _temp452.base= _temp451; _temp452.last_plus_one=
_temp451 + 6; _temp452;}); _LL414: return( struct _tagged_string)({ char*
_temp453=( char*)"noreturn"; struct _tagged_string _temp454; _temp454.curr=
_temp453; _temp454.base= _temp453; _temp454.last_plus_one= _temp453 + 9;
_temp454;}); _LL416: return( struct _tagged_string)({ char* _temp455=( char*)"const";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 6; _temp456;}); _LL418: if( _temp445 == - 1){
return( struct _tagged_string)({ char* _temp457=( char*)"aligned"; struct
_tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 8; _temp458;});} else{ return xprintf("aligned(%d)",
_temp445);} _LL420: return( struct _tagged_string)({ char* _temp459=( char*)"packed";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 7; _temp460;}); _LL422: return({ struct
_tagged_string _temp461= _temp447; xprintf("section(\"%.*s\")", _temp461.last_plus_one
- _temp461.curr, _temp461.curr);}); _LL424: return( struct _tagged_string)({
char* _temp462=( char*)"nocommon"; struct _tagged_string _temp463; _temp463.curr=
_temp462; _temp463.base= _temp462; _temp463.last_plus_one= _temp462 + 9;
_temp463;}); _LL426: return( struct _tagged_string)({ char* _temp464=( char*)"shared";
struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 7; _temp465;}); _LL428: return( struct
_tagged_string)({ char* _temp466=( char*)"unused"; struct _tagged_string
_temp467; _temp467.curr= _temp466; _temp467.base= _temp466; _temp467.last_plus_one=
_temp466 + 7; _temp467;}); _LL430: return( struct _tagged_string)({ char*
_temp468=( char*)"weak"; struct _tagged_string _temp469; _temp469.curr= _temp468;
_temp469.base= _temp468; _temp469.last_plus_one= _temp468 + 5; _temp469;});
_LL432: return( struct _tagged_string)({ char* _temp470=( char*)"dllimport";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 10; _temp471;}); _LL434: return( struct
_tagged_string)({ char* _temp472=( char*)"dllexport"; struct _tagged_string
_temp473; _temp473.curr= _temp472; _temp473.base= _temp472; _temp473.last_plus_one=
_temp472 + 10; _temp473;}); _LL436: return( struct _tagged_string)({ char*
_temp474=( char*)"no_instrument_function"; struct _tagged_string _temp475;
_temp475.curr= _temp474; _temp475.base= _temp474; _temp475.last_plus_one=
_temp474 + 23; _temp475;}); _LL438: return( struct _tagged_string)({ char*
_temp476=( char*)"constructor"; struct _tagged_string _temp477; _temp477.curr=
_temp476; _temp477.base= _temp476; _temp477.last_plus_one= _temp476 + 12;
_temp477;}); _LL440: return( struct _tagged_string)({ char* _temp478=( char*)"destructor";
struct _tagged_string _temp479; _temp479.curr= _temp478; _temp479.base= _temp478;
_temp479.last_plus_one= _temp478 + 11; _temp479;}); _LL442: return( struct
_tagged_string)({ char* _temp480=( char*)"no_check_memory_usage"; struct
_tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 22; _temp481;}); _LL406:;}