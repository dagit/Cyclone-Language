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
ropt=({ struct Cyc_Core_Opt* _temp6=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp6->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp6;});{
struct Cyc_List_List* vs2= vs; for( 0; vs2 != 0; vs2=({ struct Cyc_List_List*
_temp0= vs2; if( _temp0 == 0){ _throw( Null_Exception);} _temp0->tl;})){ te2=
Cyc_Tcenv_add_pat_var( loc, te2,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List*
_temp1= vs2; if( _temp1 == 0){ _throw( Null_Exception);} _temp1->hd;}));((
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp2= vs2; if( _temp2 == 0){
_throw( Null_Exception);} _temp2->hd;}))->rgn= ropt;}} if( where_opt != 0){ Cyc_Tcexp_tcTest(
te2,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp3= where_opt; if(
_temp3 == 0){ _throw( Null_Exception);} _temp3;}),( struct _tagged_string)({
char* _temp4=( char*)"switch clause guard"; struct _tagged_string _temp5; _temp5.curr=
_temp4; _temp5.base= _temp4; _temp5.last_plus_one= _temp4 + 20; _temp5;}));} if(
vs != 0){ te2= Cyc_Tcenv_set_encloser( te2, s);} Cyc_Tcstmt_tcStmt( te2, s, 0);
if( vs != 0){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));}}}
void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s0,
int new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{ void* _temp7=( void*) s0->r;
struct Cyc_Absyn_Exp* _temp47; struct Cyc_Absyn_Stmt* _temp49; struct Cyc_Absyn_Stmt*
_temp51; struct Cyc_Absyn_Exp* _temp53; struct Cyc_Absyn_Stmt* _temp55; struct
Cyc_Absyn_Stmt* _temp57; struct Cyc_Absyn_Exp* _temp59; struct Cyc_Absyn_Stmt*
_temp61; struct _tuple2 _temp63; struct Cyc_Absyn_Stmt* _temp65; struct Cyc_Absyn_Exp*
_temp67; struct Cyc_Absyn_Stmt* _temp69; struct _tuple2 _temp71; struct Cyc_Absyn_Stmt*
_temp73; struct Cyc_Absyn_Exp* _temp75; struct _tuple2 _temp77; struct Cyc_Absyn_Stmt*
_temp79; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp* _temp83; struct
_tuple2 _temp85; struct Cyc_Absyn_Stmt* _temp87; struct Cyc_Absyn_Exp* _temp89;
struct Cyc_Absyn_Stmt* _temp91; struct Cyc_Absyn_Stmt* _temp93; struct Cyc_Absyn_Stmt**
_temp95; struct Cyc_Absyn_Stmt* _temp96; struct Cyc_Absyn_Stmt** _temp98; struct
Cyc_Absyn_Stmt* _temp99; struct Cyc_Absyn_Stmt** _temp101; struct _tagged_string*
_temp102; struct Cyc_Absyn_Switch_clause** _temp104; struct Cyc_Absyn_Switch_clause***
_temp106; struct Cyc_List_List* _temp107; struct Cyc_Absyn_Stmt* _temp109;
struct _tagged_string* _temp111; struct Cyc_List_List* _temp113; struct Cyc_Absyn_Exp*
_temp115; struct Cyc_List_List* _temp117; struct Cyc_Absyn_Stmt* _temp119;
struct Cyc_Absyn_Stmt* _temp121; struct Cyc_Absyn_Decl* _temp123; struct Cyc_Absyn_Stmt*
_temp125; struct Cyc_Absyn_Vardecl* _temp127; struct Cyc_Absyn_Tvar* _temp129;
struct Cyc_Absyn_Stmt* _temp131; struct Cyc_Absyn_Stmt* _temp133; _LL9: if(
_temp7 ==( void*) Cyc_Absyn_Skip_s){ goto _LL10;} else{ goto _LL11;} _LL11: if((
unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Exp_s: 0){ _LL48:
_temp47=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp7)->f1;
goto _LL12;} else{ goto _LL13;} _LL13: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_Seq_s: 0){ _LL52: _temp51=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp7)->f1; goto _LL50; _LL50: _temp49=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp7)->f2; goto _LL14;}
else{ goto _LL15;} _LL15: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) ==
Cyc_Absyn_Return_s: 0){ _LL54: _temp53=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp7)->f1; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_IfThenElse_s: 0){ _LL60: _temp59=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp7)->f1; goto _LL58; _LL58: _temp57=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp7)->f2;
goto _LL56; _LL56: _temp55=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp7)->f3; goto _LL18;} else{ goto _LL19;} _LL19: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_While_s: 0){ _LL64: _temp63=( struct _tuple2)((
struct Cyc_Absyn_While_s_struct*) _temp7)->f1; _LL68: _temp67= _temp63.f1; goto
_LL66; _LL66: _temp65= _temp63.f2; goto _LL62; _LL62: _temp61=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp7)->f2; goto _LL20;} else{ goto _LL21;}
_LL21: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_For_s: 0){
_LL84: _temp83=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp7)->f1;
goto _LL78; _LL78: _temp77=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f2; _LL82: _temp81= _temp77.f1; goto _LL80; _LL80: _temp79= _temp77.f2;
goto _LL72; _LL72: _temp71=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f3; _LL76: _temp75= _temp71.f1; goto _LL74; _LL74: _temp73= _temp71.f2;
goto _LL70; _LL70: _temp69=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp7)->f4; goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Do_s: 0){ _LL92: _temp91=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp7)->f1; goto _LL86; _LL86: _temp85=( struct
_tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp7)->f2; _LL90: _temp89= _temp85.f1;
goto _LL88; _LL88: _temp87= _temp85.f2; goto _LL24;} else{ goto _LL25;} _LL25:
if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Break_s: 0){ _LL94:
_temp93=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp7)->f1;
_temp95=&(( struct Cyc_Absyn_Break_s_struct*) _temp7)->f1; goto _LL26;} else{
goto _LL27;} _LL27: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Continue_s:
0){ _LL97: _temp96=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp7)->f1; _temp98=&(( struct Cyc_Absyn_Continue_s_struct*) _temp7)->f1; goto
_LL28;} else{ goto _LL29;} _LL29: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_Goto_s: 0){ _LL103: _temp102=( struct _tagged_string*)((
struct Cyc_Absyn_Goto_s_struct*) _temp7)->f1; goto _LL100; _LL100: _temp99=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; _temp101=&((
struct Cyc_Absyn_Goto_s_struct*) _temp7)->f2; goto _LL30;} else{ goto _LL31;}
_LL31: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Fallthru_s:
0){ _LL108: _temp107=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp7)->f1; goto _LL105; _LL105: _temp104=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp7)->f2; _temp106=&(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp7)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Label_s: 0){ _LL112: _temp111=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp7)->f1; goto _LL110;
_LL110: _temp109=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp7)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Switch_s: 0){ _LL116: _temp115=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp7)->f1; goto _LL114; _LL114: _temp113=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp7)->f2; goto
_LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_TryCatch_s: 0){ _LL120: _temp119=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp7)->f1; goto _LL118; _LL118: _temp117=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*) _temp7)->f2; goto
_LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_Decl_s: 0){ _LL124: _temp123=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp7)->f1; goto _LL122; _LL122: _temp121=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp7)->f2; goto
_LL40;} else{ goto _LL41;} _LL41: if(( unsigned int) _temp7 > 1u?*(( int*)
_temp7) == Cyc_Absyn_Region_s: 0){ _LL130: _temp129=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp7)->f1; goto _LL128; _LL128: _temp127=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*) _temp7)->f2;
goto _LL126; _LL126: _temp125=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp7)->f3; goto _LL42;} else{ goto _LL43;} _LL43: if(( unsigned int) _temp7 >
1u?*(( int*) _temp7) == Cyc_Absyn_Cut_s: 0){ _LL132: _temp131=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp7)->f1; goto _LL44;} else{ goto _LL45;}
_LL45: if(( unsigned int) _temp7 > 1u?*(( int*) _temp7) == Cyc_Absyn_Splice_s: 0){
_LL134: _temp133=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp7)->f1; goto _LL46;} else{ goto _LL8;} _LL10: return; _LL12: Cyc_Tcexp_tcExp(
te, 0, _temp47); return; _LL14: { struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp147=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp147[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp148; _temp148.tag= Cyc_Tcenv_Stmt_j; _temp148.f1= _temp49; _temp148;});
_temp147;})); Cyc_Tcstmt_tcStmt( te2, _temp51, 1); while( 1) { void* _temp135=(
void*) _temp49->r; struct Cyc_Absyn_Stmt* _temp141; struct Cyc_Absyn_Stmt*
_temp143; _LL137: if(( unsigned int) _temp135 > 1u?*(( int*) _temp135) == Cyc_Absyn_Seq_s:
0){ _LL144: _temp143=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp135)->f1; goto _LL142; _LL142: _temp141=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp135)->f2; goto _LL138;} else{ goto _LL139;} _LL139:
goto _LL140; _LL138: Cyc_Tcstmt_decorate_stmt( te, _temp49); te2= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp145=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp145[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp146; _temp146.tag= Cyc_Tcenv_Stmt_j; _temp146.f1= _temp141; _temp146;});
_temp145;})); Cyc_Tcstmt_tcStmt( te2, _temp143, 1); _temp49= _temp141; continue;
_LL140: goto seq_end; _LL136:;} seq_end: Cyc_Tcstmt_tcStmt( te, _temp49, 1);
return;} _LL16: { void* t= Cyc_Tcenv_return_typ( te); if( _temp53 == 0){ void*
_temp149= Cyc_Tcutil_compress( t); _LL151: if( _temp149 ==( void*) Cyc_Absyn_VoidType){
goto _LL152;} else{ goto _LL153;} _LL153: goto _LL154; _LL152: goto _LL150;
_LL154: Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp155= Cyc_Absynpp_typ2string(
t); xprintf("must return a value of type %.*s", _temp155.last_plus_one -
_temp155.curr, _temp155.curr);})); goto _LL150; _LL150:;} else{ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp167= _temp53; if(
_temp167 == 0){ _throw( Null_Exception);} _temp167;}); Cyc_Tcexp_tcExp( te,(
void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){ Cyc_Tcutil_terr( s0->loc,({
struct _tagged_string _temp157= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp156= e->topt; if( _temp156 == 0){ _throw( Null_Exception);} _temp156->v;}));
struct _tagged_string _temp158= Cyc_Absynpp_typ2string( t); xprintf("returns value of type %.*s but requires %.*s",
_temp157.last_plus_one - _temp157.curr, _temp157.curr, _temp158.last_plus_one -
_temp158.curr, _temp158.curr);}));}{ void* _temp159= Cyc_Tcutil_compress( t);
_LL161: if( _temp159 ==( void*) Cyc_Absyn_VoidType){ goto _LL162;} else{ goto
_LL163;} _LL163: goto _LL164; _LL162: Cyc_Tcutil_terr( s0->loc,( struct
_tagged_string)({ char* _temp165=( char*)"function returns void expression";
struct _tagged_string _temp166; _temp166.curr= _temp165; _temp166.base= _temp165;
_temp166.last_plus_one= _temp165 + 33; _temp166;})); goto _LL160; _LL164: goto
_LL160; _LL160:;}} return;} _LL18: Cyc_Tcexp_tcTest( te, _temp59,( struct
_tagged_string)({ char* _temp168=( char*)"if statement"; struct _tagged_string
_temp169; _temp169.curr= _temp168; _temp169.base= _temp168; _temp169.last_plus_one=
_temp168 + 13; _temp169;})); Cyc_Tcstmt_tcStmt( te, _temp57, 1); Cyc_Tcstmt_tcStmt(
te, _temp55, 1); return; _LL20: Cyc_Tcstmt_decorate_stmt( te, _temp65); Cyc_Tcexp_tcTest(
te, _temp67,( struct _tagged_string)({ char* _temp170=( char*)"while loop";
struct _tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 11; _temp171;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp65), _temp61, 1); return; _LL22: Cyc_Tcstmt_decorate_stmt( te, _temp79);
Cyc_Tcstmt_decorate_stmt( te, _temp73); Cyc_Tcexp_tcExp( te, 0, _temp83); Cyc_Tcexp_tcTest(
te, _temp81,( struct _tagged_string)({ char* _temp172=( char*)"for loop"; struct
_tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 9; _temp173;})); te= Cyc_Tcenv_set_in_loop(
te, _temp73); Cyc_Tcstmt_tcStmt( te, _temp69, 1); Cyc_Tcexp_tcExp( te, 0,
_temp75); return; _LL24: Cyc_Tcstmt_decorate_stmt( te, _temp87); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp87), _temp91, 1); Cyc_Tcexp_tcTest( te, _temp89,(
struct _tagged_string)({ char* _temp174=( char*)"do loop"; struct _tagged_string
_temp175; _temp175.curr= _temp174; _temp175.base= _temp174; _temp175.last_plus_one=
_temp174 + 8; _temp175;})); return; _LL26: Cyc_Tcenv_process_break( te, s0,
_temp95); return; _LL28: Cyc_Tcenv_process_continue( te, s0, _temp98); return;
_LL30: Cyc_Tcenv_process_goto( te, s0, _temp102, _temp101); return; _LL32: {
struct _tuple3* trip_opt= Cyc_Tcenv_process_fallthru( te, s0, _temp106); if(
trip_opt == 0){ Cyc_Tcutil_terr( s0->loc,( struct _tagged_string)({ char*
_temp176=( char*)"fallthru not in a non-last case"; struct _tagged_string
_temp177; _temp177.curr= _temp176; _temp177.base= _temp176; _temp177.last_plus_one=
_temp176 + 32; _temp177;})); return;}{ struct _tuple3 _temp181; struct Cyc_List_List*
_temp182; struct Cyc_List_List* _temp184; struct Cyc_Absyn_Switch_clause*
_temp186; struct _tuple3* _temp179=( struct _tuple3*)({ struct _tuple3* _temp178=
trip_opt; if( _temp178 == 0){ _throw( Null_Exception);} _temp178;}); _temp181=*
_temp179; _LL187: _temp186= _temp181.f1; goto _LL185; _LL185: _temp184= _temp181.f2;
goto _LL183; _LL183: _temp182= _temp181.f3; goto _LL180; _LL180: { struct Cyc_List_List*
instantiation=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp184);
_temp182=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*, void*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_substitute,
instantiation, _temp182); for( 0; _temp182 != 0? _temp107 != 0: 0; _temp182=({
struct Cyc_List_List* _temp188= _temp182; if( _temp188 == 0){ _throw(
Null_Exception);} _temp188->tl;}), _temp107=({ struct Cyc_List_List* _temp189=
_temp107; if( _temp189 == 0){ _throw( Null_Exception);} _temp189->tl;})){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp190= _temp107; if(
_temp190 == 0){ _throw( Null_Exception);} _temp190->hd;})); if( ! Cyc_Tcutil_coerce_arg(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp191= _temp107; if(
_temp191 == 0){ _throw( Null_Exception);} _temp191->hd;}),( void*)({ struct Cyc_List_List*
_temp192= _temp182; if( _temp192 == 0){ _throw( Null_Exception);} _temp192->hd;}))){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp196= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp194=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp193= _temp107; if( _temp193 == 0){ _throw( Null_Exception);} _temp193->hd;}))->topt;
if( _temp194 == 0){ _throw( Null_Exception);} _temp194->v;})); struct
_tagged_string _temp197= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_List_List*
_temp195= _temp182; if( _temp195 == 0){ _throw( Null_Exception);} _temp195->hd;}));
xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp196.last_plus_one - _temp196.curr, _temp196.curr, _temp197.last_plus_one -
_temp197.curr, _temp197.curr);}));}} if( _temp107 != 0){ Cyc_Tcutil_terr( s0->loc,(
struct _tagged_string)({ char* _temp198=( char*)"too many arguments to explicit fallthru";
struct _tagged_string _temp199; _temp199.curr= _temp198; _temp199.base= _temp198;
_temp199.last_plus_one= _temp198 + 40; _temp199;}));} if( _temp182 != 0){ Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp200=( char*)"too few arguments to explicit fallthru";
struct _tagged_string _temp201; _temp201.curr= _temp200; _temp201.base= _temp200;
_temp201.last_plus_one= _temp200 + 39; _temp201;}));} return;}}} _LL34: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label( te, _temp111, _temp109),({
struct Cyc_Absyn_Tvar* _temp202=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof(
struct Cyc_Absyn_Tvar)); _temp202->name=({ struct _tagged_string* _temp203=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp203[ 0]=({
struct _tagged_string _temp204=* _temp111; xprintf("`%.*s", _temp204.last_plus_one
- _temp204.curr, _temp204.curr);}); _temp203;}); _temp202->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp202;})),
_temp109, 0); return; _LL36: Cyc_Tcexp_tcExp( te, 0, _temp115);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp224= _temp115->topt; if( _temp224 == 0){ _throw(
Null_Exception);} _temp224->v;}); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp113); for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp205= scs; if( _temp205 == 0){ _throw( Null_Exception);} _temp205->tl;})){
struct Cyc_Absyn_Pat* p=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp223= scs; if( _temp223 == 0){ _throw( Null_Exception);} _temp223->hd;}))->pattern;
struct Cyc_List_List* _temp208; struct Cyc_List_List* _temp210; struct _tuple5
_temp206= Cyc_Tcpat_tcPat( te, p,( void**)& t, 0); _LL211: _temp210= _temp206.f1;
goto _LL209; _LL209: _temp208= _temp206.f2; goto _LL207; _LL207: if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp212= p->topt; if( _temp212 == 0){ _throw(
Null_Exception);} _temp212->v;}), t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp213= scs; if( _temp213 == 0){ _throw( Null_Exception);}
_temp213->hd;}))->loc,({ struct _tagged_string _temp215= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp216= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp214= p->topt; if( _temp214 == 0){ _throw( Null_Exception);} _temp214->v;}));
xprintf("switch on type %.*s, but case expects type %.*s", _temp215.last_plus_one
- _temp215.curr, _temp215.curr, _temp216.last_plus_one - _temp216.curr, _temp216.curr);}));}
else{ Cyc_Tcpat_check_pat_regions( te, p);}(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp217= scs; if( _temp217 == 0){ _throw( Null_Exception);}
_temp217->hd;}))->pat_vars=({ struct Cyc_Core_Opt* _temp218=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp218->v=( void*) _temp208;
_temp218;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp219= scs; if( _temp219 == 0){ _throw( Null_Exception);}
_temp219->hd;}))->loc, te, _temp210, _temp208,(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp220= scs; if( _temp220 == 0){ _throw( Null_Exception);}
_temp220->hd;}))->body,(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp221= scs; if( _temp221 == 0){ _throw( Null_Exception);} _temp221->hd;}))->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp210, _temp208,( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp222= scs; if( _temp222 == 0){ _throw( Null_Exception);}
_temp222->hd;}));}} Cyc_Tcpat_check_switch_exhaustive( s0->loc, _temp113);
return;} _LL38:( void*)( _temp119->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt((
void*) _temp119->r, _temp119->loc), Cyc_Absyn_skip_stmt( _temp119->loc),
_temp119->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_enter_try(
te), s0), _temp119, 1); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp117); for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp225= scs; if( _temp225 == 0){ _throw( Null_Exception);} _temp225->tl;})){
struct Cyc_Absyn_Pat* p=(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp242= scs; if( _temp242 == 0){ _throw( Null_Exception);} _temp242->hd;}))->pattern;
struct Cyc_List_List* _temp228; struct Cyc_List_List* _temp230; struct _tuple5
_temp226= Cyc_Tcpat_tcPat( te, p,( void**)& Cyc_Absyn_exn_typ, 0); _LL231:
_temp230= _temp226.f1; goto _LL229; _LL229: _temp228= _temp226.f2; goto _LL227;
_LL227: if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp232= p->topt;
if( _temp232 == 0){ _throw( Null_Exception);} _temp232->v;}), Cyc_Absyn_exn_typ)){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp233= scs; if( _temp233 == 0){ _throw( Null_Exception);} _temp233->hd;}))->loc,({
struct _tagged_string _temp235= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp234= p->topt; if( _temp234 == 0){ _throw( Null_Exception);} _temp234->v;}));
xprintf("expected xtunion exn but found %.*s", _temp235.last_plus_one - _temp235.curr,
_temp235.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te, p);}(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp236= scs; if( _temp236 == 0){ _throw( Null_Exception);}
_temp236->hd;}))->pat_vars=({ struct Cyc_Core_Opt* _temp237=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=( void*) _temp228;
_temp237;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp238= scs; if( _temp238 == 0){ _throw( Null_Exception);}
_temp238->hd;}))->loc, te, _temp230, _temp228,(( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp239= scs; if( _temp239 == 0){ _throw( Null_Exception);}
_temp239->hd;}))->body,(( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List*
_temp240= scs; if( _temp240 == 0){ _throw( Null_Exception);} _temp240->hd;}))->where_clause,
1); te= Cyc_Tcenv_set_fallthru( te, _temp230, _temp228,( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp241= scs; if( _temp241 == 0){ _throw( Null_Exception);}
_temp241->hd;}));}} Cyc_Tcpat_check_catch_overlap( s0->loc, _temp117); return;
_LL40: { struct _tagged_string unimp_msg_part; if( new_block){ te= Cyc_Tcenv_new_block(
s0->loc, te);}{ void* _temp243=( void*) _temp123->r; struct Cyc_Absyn_Vardecl*
_temp267; int _temp269; int* _temp271; struct Cyc_Absyn_Exp* _temp272; struct
Cyc_Core_Opt* _temp274; struct Cyc_Core_Opt** _temp276; struct Cyc_Core_Opt*
_temp277; struct Cyc_Core_Opt** _temp279; struct Cyc_Absyn_Pat* _temp280; struct
Cyc_List_List* _temp282; struct _tagged_string* _temp284; struct Cyc_List_List*
_temp286; struct _tuple0* _temp288; struct Cyc_Absyn_Fndecl* _temp290; struct
Cyc_Absyn_Structdecl* _temp292; struct Cyc_Absyn_Uniondecl* _temp294; struct Cyc_Absyn_Tuniondecl*
_temp296; struct Cyc_Absyn_Enumdecl* _temp298; struct Cyc_Absyn_Typedefdecl*
_temp300; struct Cyc_List_List* _temp302; _LL245: if(*(( int*) _temp243) == Cyc_Absyn_Var_d){
_LL268: _temp267=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp243)->f1; goto _LL246;} else{ goto _LL247;} _LL247: if(*(( int*) _temp243)
== Cyc_Absyn_Let_d){ _LL281: _temp280=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp243)->f1; goto _LL278; _LL278: _temp277=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp243)->f2; _temp279=&(( struct Cyc_Absyn_Let_d_struct*)
_temp243)->f2; goto _LL275; _LL275: _temp274=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp243)->f3; _temp276=&(( struct Cyc_Absyn_Let_d_struct*)
_temp243)->f3; goto _LL273; _LL273: _temp272=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp243)->f4; goto _LL270; _LL270: _temp269=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp243)->f5; _temp271=&(( struct Cyc_Absyn_Let_d_struct*)
_temp243)->f5; goto _LL248;} else{ goto _LL249;} _LL249: if(*(( int*) _temp243)
== Cyc_Absyn_Namespace_d){ _LL285: _temp284=( struct _tagged_string*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp243)->f1; goto _LL283; _LL283: _temp282=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp243)->f2;
goto _LL250;} else{ goto _LL251;} _LL251: if(*(( int*) _temp243) == Cyc_Absyn_Using_d){
_LL289: _temp288=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp243)->f1;
goto _LL287; _LL287: _temp286=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp243)->f2; goto _LL252;} else{ goto _LL253;} _LL253: if(*(( int*) _temp243)
== Cyc_Absyn_Fn_d){ _LL291: _temp290=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp243)->f1; goto _LL254;} else{ goto _LL255;} _LL255: if(*(( int*) _temp243)
== Cyc_Absyn_Struct_d){ _LL293: _temp292=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp243)->f1; goto _LL256;} else{ goto
_LL257;} _LL257: if(*(( int*) _temp243) == Cyc_Absyn_Union_d){ _LL295: _temp294=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp243)->f1;
goto _LL258;} else{ goto _LL259;} _LL259: if(*(( int*) _temp243) == Cyc_Absyn_Tunion_d){
_LL297: _temp296=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp243)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(*(( int*) _temp243)
== Cyc_Absyn_Enum_d){ _LL299: _temp298=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp243)->f1; goto _LL262;} else{ goto _LL263;}
_LL263: if(*(( int*) _temp243) == Cyc_Absyn_Typedef_d){ _LL301: _temp300=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp243)->f1;
goto _LL264;} else{ goto _LL265;} _LL265: if(*(( int*) _temp243) == Cyc_Absyn_ExternC_d){
_LL303: _temp302=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp243)->f1; goto _LL266;} else{ goto _LL244;} _LL246: { struct Cyc_Absyn_Vardecl
_temp306; struct Cyc_List_List* _temp307; struct Cyc_Core_Opt* _temp309; struct
Cyc_Core_Opt** _temp311; struct Cyc_Absyn_Exp* _temp312; void* _temp314; struct
Cyc_Absyn_Tqual _temp316; struct _tuple0* _temp318; struct _tuple0 _temp320;
struct _tagged_string* _temp321; void* _temp323; void* _temp325; struct Cyc_Absyn_Vardecl*
_temp304= _temp267; _temp306=* _temp304; _LL326: _temp325=( void*) _temp306.sc;
goto _LL319; _LL319: _temp318=( struct _tuple0*) _temp306.name; _temp320=*
_temp318; _LL324: _temp323= _temp320.f1; goto _LL322; _LL322: _temp321= _temp320.f2;
goto _LL317; _LL317: _temp316=( struct Cyc_Absyn_Tqual) _temp306.tq; goto _LL315;
_LL315: _temp314=( void*) _temp306.type; goto _LL313; _LL313: _temp312=( struct
Cyc_Absyn_Exp*) _temp306.initializer; goto _LL310; _LL310: _temp309=( struct Cyc_Core_Opt*)
_temp306.rgn; _temp311=&(* _temp304).rgn; goto _LL308; _LL308: _temp307=( struct
Cyc_List_List*) _temp306.attributes; goto _LL305; _LL305: { void* curr_bl= Cyc_Tcenv_curr_rgn(
te);* _temp311=({ struct Cyc_Core_Opt* _temp327=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp327->v=( void*) curr_bl; _temp327;});{
void* _temp328= _temp323; struct Cyc_List_List* _temp338; struct Cyc_List_List*
_temp340; _LL330: if( _temp328 ==( void*) Cyc_Absyn_Loc_n){ goto _LL331;} else{
goto _LL332;} _LL332: if(( unsigned int) _temp328 > 1u?*(( int*) _temp328) ==
Cyc_Absyn_Rel_n: 0){ _LL339: _temp338=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp328)->f1; if( _temp338 == 0){ goto _LL333;} else{ goto _LL334;}} else{ goto
_LL334;} _LL334: if(( unsigned int) _temp328 > 1u?*(( int*) _temp328) == Cyc_Absyn_Abs_n:
0){ _LL341: _temp340=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp328)->f1; goto _LL335;} else{ goto _LL336;} _LL336: goto _LL337; _LL331:
goto _LL329; _LL333:(* _temp267->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL329;
_LL335:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp342=( char*)"tcstmt: Abs_n declaration";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 26; _temp343;}))); _LL337: Cyc_Tcutil_terr(
_temp123->loc,( struct _tagged_string)({ char* _temp344=( char*)"cannot declare a qualified local variable";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 42; _temp345;})); goto _LL329; _LL329:;}{
void* _temp346= Cyc_Tcutil_compress( _temp314); struct Cyc_Absyn_Exp* _temp353;
struct Cyc_Absyn_Tqual _temp355; void* _temp357; _LL348: if(( unsigned int)
_temp346 > 4u?*(( int*) _temp346) == Cyc_Absyn_ArrayType: 0){ _LL358: _temp357=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp346)->f1; goto _LL356; _LL356:
_temp355=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp346)->f2; goto _LL354; _LL354: _temp353=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp346)->f3; if( _temp353 == 0){ goto _LL352;}
else{ goto _LL350;}} else{ goto _LL350;} _LL352: if( _temp267->initializer != 0){
goto _LL349;} else{ goto _LL350;} _LL350: goto _LL351; _LL349:{ void* _temp360=(
void*)({ struct Cyc_Absyn_Exp* _temp359= _temp267->initializer; if( _temp359 ==
0){ _throw( Null_Exception);} _temp359->r;}); void* _temp370; struct
_tagged_string _temp372; struct Cyc_List_List* _temp374; struct Cyc_Core_Opt*
_temp376; struct Cyc_List_List* _temp378; _LL362: if(*(( int*) _temp360) == Cyc_Absyn_Const_e){
_LL371: _temp370=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp360)->f1; if((
unsigned int) _temp370 > 1u?*(( int*) _temp370) == Cyc_Absyn_String_c: 0){
_LL373: _temp372=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp370)->f1; goto _LL363;} else{ goto _LL364;}} else{ goto _LL364;} _LL364:
if(*(( int*) _temp360) == Cyc_Absyn_UnresolvedMem_e){ _LL377: _temp376=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp360)->f1; goto
_LL375; _LL375: _temp374=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp360)->f2; goto _LL365;} else{ goto _LL366;} _LL366: if(*(( int*) _temp360)
== Cyc_Absyn_Array_e){ _LL379: _temp378=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp360)->f1; goto _LL367;} else{ goto _LL368;} _LL368: goto _LL369; _LL363:
_temp314=( void*)( _temp267->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp380=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp380[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp381; _temp381.tag= Cyc_Absyn_ArrayType;
_temp381.f1=( void*) _temp357; _temp381.f2= _temp355; _temp381.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp382= _temp372;( unsigned int)(
_temp382.last_plus_one - _temp382.curr);}), 0); _temp381;}); _temp380;}))); goto
_LL361; _LL365: _temp378= _temp374; goto _LL367; _LL367: _temp314=( void*)(
_temp267->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp383=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp383[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp384; _temp384.tag= Cyc_Absyn_ArrayType;
_temp384.f1=( void*) _temp357; _temp384.f2= _temp355; _temp384.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp378), 0); _temp384;}); _temp383;}))); goto _LL361; _LL369: goto _LL361;
_LL361:;} goto _LL347; _LL351: goto _LL347; _LL347:;} Cyc_Tcutil_check_type( s0->loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, _temp314);{
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_local_var( _temp121->loc, te, _temp267);
if( _temp312 != 0){ Cyc_Tcexp_tcExpInitializer( te2,( void**)& _temp314,( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp385= _temp312; if( _temp385 == 0){
_throw( Null_Exception);} _temp385;})); if( ! Cyc_Tcutil_coerce_assign( te2,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp386= _temp312; if( _temp386
== 0){ _throw( Null_Exception);} _temp386;}), _temp314)){ Cyc_Tcutil_terr(
_temp123->loc,({ struct _tagged_string _temp389=* _temp321; struct
_tagged_string _temp390= Cyc_Absynpp_typ2string( _temp314); struct
_tagged_string _temp391= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp388=({ struct Cyc_Absyn_Exp* _temp387= _temp312; if( _temp387 == 0){ _throw(
Null_Exception);} _temp387->topt;}); if( _temp388 == 0){ _throw( Null_Exception);}
_temp388->v;})); xprintf("%.*s declared with type %.*s, initialized with type %.*s",
_temp389.last_plus_one - _temp389.curr, _temp389.curr, _temp390.last_plus_one -
_temp390.curr, _temp390.curr, _temp391.last_plus_one - _temp391.curr, _temp391.curr);}));}}
Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( te2, s0), _temp121, 0); return;}}}
_LL248: { struct Cyc_Core_Opt* topt=* _temp276; Cyc_Tcexp_tcExpInitializer( te,
0, _temp272);* _temp276= _temp272->topt;{ struct Cyc_List_List* _temp395; struct
Cyc_List_List* _temp397; struct _tuple5 _temp393= Cyc_Tcpat_tcPat( te, _temp280,(
void**)(( void**)(( void*)({ struct Cyc_Core_Opt* _temp392= _temp272->topt; if(
_temp392 == 0){ _throw( Null_Exception);}& _temp392->v;}))), 0); _LL398:
_temp397= _temp393.f1; goto _LL396; _LL396: _temp395= _temp393.f2; goto _LL394;
_LL394:* _temp279=({ struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp399->v=( void*) _temp395;
_temp399;}); if( topt != 0? ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp400= _temp280->topt; if( _temp400 == 0){ _throw( Null_Exception);} _temp400->v;}),(
void*)({ struct Cyc_Core_Opt* _temp401= topt; if( _temp401 == 0){ _throw(
Null_Exception);} _temp401->v;})): 0){ Cyc_Tcutil_terr( _temp123->loc,( struct
_tagged_string)({ char* _temp402=( char*)"type of pattern does not match declared type";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 45; _temp403;}));} if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp404= _temp280->topt; if( _temp404 == 0){
_throw( Null_Exception);} _temp404->v;}),( void*)({ struct Cyc_Core_Opt*
_temp405= _temp272->topt; if( _temp405 == 0){ _throw( Null_Exception);} _temp405->v;}))){
Cyc_Tcutil_terr( _temp123->loc,({ struct _tagged_string _temp408= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp406= _temp280->topt; if( _temp406 == 0){
_throw( Null_Exception);} _temp406->v;})); struct _tagged_string _temp409= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp407= _temp272->topt; if( _temp407 == 0){
_throw( Null_Exception);} _temp407->v;})); xprintf("pattern type %.*s does not match definition type %.*s",
_temp408.last_plus_one - _temp408.curr, _temp408.curr, _temp409.last_plus_one -
_temp409.curr, _temp409.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp280);}* _temp271= Cyc_Tcpat_check_let_pat_exhaustive( _temp280->loc,
_temp280); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp397, _temp395, _temp121,
0, 0); return;}} _LL250: unimp_msg_part=( struct _tagged_string)({ char*
_temp410=( char*)"namespace"; struct _tagged_string _temp411; _temp411.curr=
_temp410; _temp411.base= _temp410; _temp411.last_plus_one= _temp410 + 10;
_temp411;}); goto _LL244; _LL252: unimp_msg_part=( struct _tagged_string)({ char*
_temp412=( char*)"using"; struct _tagged_string _temp413; _temp413.curr=
_temp412; _temp413.base= _temp412; _temp413.last_plus_one= _temp412 + 6;
_temp413;}); goto _LL244; _LL254: unimp_msg_part=( struct _tagged_string)({ char*
_temp414=( char*)"function"; struct _tagged_string _temp415; _temp415.curr=
_temp414; _temp415.base= _temp414; _temp415.last_plus_one= _temp414 + 9;
_temp415;}); goto _LL244; _LL256: unimp_msg_part=( struct _tagged_string)({ char*
_temp416=( char*)"struct"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 7;
_temp417;}); goto _LL244; _LL258: unimp_msg_part=( struct _tagged_string)({ char*
_temp418=( char*)"union"; struct _tagged_string _temp419; _temp419.curr=
_temp418; _temp419.base= _temp418; _temp419.last_plus_one= _temp418 + 6;
_temp419;}); goto _LL244; _LL260: unimp_msg_part=( struct _tagged_string)({ char*
_temp420=( char*)"[x]tunion"; struct _tagged_string _temp421; _temp421.curr=
_temp420; _temp421.base= _temp420; _temp421.last_plus_one= _temp420 + 10;
_temp421;}); goto _LL244; _LL262: unimp_msg_part=( struct _tagged_string)({ char*
_temp422=( char*)"enum"; struct _tagged_string _temp423; _temp423.curr= _temp422;
_temp423.base= _temp422; _temp423.last_plus_one= _temp422 + 5; _temp423;}); goto
_LL244; _LL264: unimp_msg_part=( struct _tagged_string)({ char* _temp424=( char*)"typedef";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 8; _temp425;}); goto _LL244; _LL266:
unimp_msg_part=( struct _tagged_string)({ char* _temp426=( char*)"extern \"C\"";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 11; _temp427;}); goto _LL244; _LL244:;}( void)
_throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ struct
_tagged_string _temp428= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp428.last_plus_one - _temp428.curr, _temp428.curr);})));} _LL42:( void*)(
_temp125->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt(( void*)
_temp125->r, _temp125->loc), Cyc_Absyn_skip_stmt( _temp125->loc), _temp125->loc))->r));
if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ struct Cyc_Absyn_Vardecl
_temp431; struct Cyc_List_List* _temp432; struct Cyc_Core_Opt* _temp434; struct
Cyc_Core_Opt** _temp436; struct Cyc_Absyn_Exp* _temp437; void* _temp439; struct
Cyc_Absyn_Tqual _temp441; struct _tuple0* _temp443; void* _temp445; struct Cyc_Absyn_Vardecl*
_temp429= _temp127; _temp431=* _temp429; _LL446: _temp445=( void*) _temp431.sc;
goto _LL444; _LL444: _temp443=( struct _tuple0*) _temp431.name; goto _LL442;
_LL442: _temp441=( struct Cyc_Absyn_Tqual) _temp431.tq; goto _LL440; _LL440:
_temp439=( void*) _temp431.type; goto _LL438; _LL438: _temp437=( struct Cyc_Absyn_Exp*)
_temp431.initializer; goto _LL435; _LL435: _temp434=( struct Cyc_Core_Opt*)
_temp431.rgn; _temp436=&(* _temp429).rgn; goto _LL433; _LL433: _temp432=( struct
Cyc_List_List*) _temp431.attributes; goto _LL430; _LL430: { void* curr_bl= Cyc_Tcenv_curr_rgn(
te); void* rgn_typ=( void*)({ struct Cyc_Absyn_VarType_struct* _temp453=( struct
Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp453[ 0]=({ struct Cyc_Absyn_VarType_struct _temp454; _temp454.tag= Cyc_Absyn_VarType;
_temp454.f1= _temp129; _temp454;}); _temp453;});* _temp436=({ struct Cyc_Core_Opt*
_temp447=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp447->v=( void*) curr_bl; _temp447;}); te= Cyc_Tcenv_add_type_vars( s0->loc,
te,({ struct Cyc_List_List* _temp448=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp448->hd=( void*) _temp129; _temp448->tl= 0;
_temp448;})); Cyc_Tcutil_check_type( s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_BoxKind, _temp439); te= Cyc_Tcenv_add_region( te, rgn_typ); if(
! Cyc_Tcutil_unify( _temp439,( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp449=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp449[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp450; _temp450.tag=
Cyc_Absyn_RgnHandleType; _temp450.f1=( void*) rgn_typ; _temp450;}); _temp449;}))){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp451=( char*)"region stmt: type of region handle is wrong!"; struct
_tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 45; _temp452;}));} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_add_local_var( _temp125->loc, Cyc_Tcenv_enter_try( te), _temp127), s0),
_temp125, 0); return;}} _LL44:( void) _throw((( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp455=( char*)"tcStmt: cut is not implemented";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 31; _temp456;}))); _LL46:( void) _throw(((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp457=( char*)"tcStmt: splice is not implemented"; struct
_tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 34; _temp458;}))); _LL8:;}}