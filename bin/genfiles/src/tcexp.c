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
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void*
f1; void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg[
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
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rappend( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern char
Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int i);
extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y); struct Cyc_Lineno_Pos{
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
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1*
Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t; extern void*
Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_String_zstrcmp( struct
_tagged_string, struct _tagged_string); struct Cyc_PP_Ppstate; typedef struct
Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern void Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* eff); extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral( struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_rsubstitute( struct _RegionHandle*, struct Cyc_List_List*,
void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); struct
_tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var(
struct _RegionHandle*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct
Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*,
unsigned int i, struct Cyc_Absyn_Conref*); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); struct _tuple5{ int f1; void* f2; } ; extern struct _tuple5 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); struct Cyc_CfFlowInfo_Place;
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
ds); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp();}
static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp0; _push_handler(& _temp0);{
int _temp2= 0; if( setjmp( _temp0.handler)){ _temp2= 1;} if( ! _temp2){{ void*
_temp3= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void* _temp13; struct Cyc_Absyn_Enumfield*
_temp15; struct Cyc_Absyn_Enumdecl* _temp17; struct Cyc_Absyn_Tunionfield*
_temp19; struct Cyc_Absyn_Tuniondecl* _temp21; struct Cyc_Absyn_Structdecl*
_temp23; _LL5: if(*(( int*) _temp3) == Cyc_Tcenv_VarRes){ _LL14: _temp13=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7:
if(*(( int*) _temp3) == Cyc_Tcenv_EnumRes){ _LL18: _temp17=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp3)->f1; goto _LL16; _LL16: _temp15=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp3)->f2;
goto _LL8;} else{ goto _LL9;} _LL9: if(*(( int*) _temp3) == Cyc_Tcenv_TunionRes){
_LL22: _temp21=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp3)->f1; goto _LL20; _LL20: _temp19=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp3)->f2; goto _LL10;} else{ goto _LL11;}
_LL11: if(*(( int*) _temp3) == Cyc_Tcenv_StructRes){ _LL24: _temp23=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp3)->f1; goto _LL12;} else{ goto _LL4;}
_LL6:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp25=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp25[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp26; _temp26.tag= Cyc_Absyn_Var_e;
_temp26.f1= q; _temp26.f2=( void*) _temp13; _temp26;}); _temp25;}))); goto _LL4;
_LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp27=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp27[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp28; _temp28.tag= Cyc_Absyn_Enum_e;
_temp28.f1= q; _temp28.f2=( struct Cyc_Absyn_Enumdecl*) _temp17; _temp28.f3=(
struct Cyc_Absyn_Enumfield*) _temp15; _temp28;}); _temp27;}))); goto _LL4; _LL10:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp29=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp29[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp30; _temp30.tag= Cyc_Absyn_Tunion_e;
_temp30.f1= 0; _temp30.f2= 0; _temp30.f3= 0; _temp30.f4= _temp21; _temp30.f5=
_temp19; _temp30;}); _temp29;}))); goto _LL4; _LL12: Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp31= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp31.last_plus_one - _temp31.curr, _temp31.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp32=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp32[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp33; _temp33.tag= Cyc_Absyn_Var_e; _temp33.f1= q; _temp33.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp33;}); _temp32;}))); goto _LL4; _LL4:;};
_pop_handler();} else{ void* _temp1=( void*) _exn_thrown; void* _temp35= _temp1;
_LL37: if( _temp35 == Cyc_Dict_Absent){ goto _LL38;} else{ goto _LL39;} _LL39:
goto _LL40; _LL38:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp41=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp41[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp42; _temp42.tag= Cyc_Absyn_Var_e;
_temp42.f1= q; _temp42.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp42;});
_temp41;}))); goto _LL36; _LL40:( void) _throw( _temp35); _LL36:;}}} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple6* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple6* _temp43=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp43->f1=
0; _temp43->f2= e; _temp43;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp44=( void*) e1->r; struct _tuple1* _temp50; _LL46:
if(*(( int*) _temp44) == Cyc_Absyn_UnknownId_e){ _LL51: _temp50=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp44)->f1; goto _LL47;} else{ goto
_LL48;} _LL48: goto _LL49; _LL47: { struct _handler_cons _temp52; _push_handler(&
_temp52);{ int _temp54= 0; if( setjmp( _temp52.handler)){ _temp54= 1;} if( !
_temp54){{ void* _temp55= Cyc_Tcenv_lookup_ordinary( te, e1->loc, _temp50); void*
_temp65; struct Cyc_Absyn_Tunionfield* _temp67; struct Cyc_Absyn_Tuniondecl*
_temp69; struct Cyc_Absyn_Structdecl* _temp71; struct Cyc_Absyn_Enumfield*
_temp73; struct Cyc_Absyn_Enumdecl* _temp75; _LL57: if(*(( int*) _temp55) == Cyc_Tcenv_VarRes){
_LL66: _temp65=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp55)->f1; goto
_LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp55) == Cyc_Tcenv_TunionRes){
_LL70: _temp69=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp55)->f1; goto _LL68; _LL68: _temp67=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp55)->f2; goto _LL60;} else{ goto _LL61;}
_LL61: if(*(( int*) _temp55) == Cyc_Tcenv_StructRes){ _LL72: _temp71=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp55)->f1; goto
_LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp55) == Cyc_Tcenv_EnumRes){
_LL76: _temp75=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp55)->f1; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp55)->f2; goto _LL64;} else{ goto _LL56;} _LL58:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp77=(
struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp77[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp78; _temp78.tag= Cyc_Absyn_FnCall_e;
_temp78.f1= e1; _temp78.f2= es; _temp78;}); _temp77;}))); _npop_handler( 0u);
return; _LL60: if( _temp67->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp79= Cyc_Absynpp_qvar2string( _temp67->name); xprintf("%.*s is a constant, not a function",
_temp79.last_plus_one - _temp79.curr, _temp79.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp80=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp80[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp81; _temp81.tag= Cyc_Absyn_Tunion_e; _temp81.f1= 0; _temp81.f2= 0; _temp81.f3=
es; _temp81.f4= _temp69; _temp81.f5= _temp67; _temp81;}); _temp80;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL62: { struct Cyc_List_List*
des=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp71->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp82=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp82[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp83; _temp83.tag= Cyc_Absyn_Struct_e;
_temp83.f1=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp84= _temp71->name; if(
_temp84 == 0){ _throw( Null_Exception);} _temp84->v;}); _temp83.f2= 0; _temp83.f3=
des; _temp83.f4=( struct Cyc_Absyn_Structdecl*) _temp71; _temp83;}); _temp82;})));}
else{ Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp85=( char*)"missing struct name";
struct _tagged_string _temp86; _temp86.curr= _temp85; _temp86.base= _temp85;
_temp86.last_plus_one= _temp85 + 20; _temp86;}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp87=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp87[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp88; _temp88.tag= Cyc_Absyn_Struct_e; _temp88.f1= _temp50; _temp88.f2= 0;
_temp88.f3= des; _temp88.f4=( struct Cyc_Absyn_Structdecl*) _temp71; _temp88;});
_temp87;})));} _npop_handler( 0u); return;} _LL64: Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp89= Cyc_Absynpp_qvar2string( _temp50); xprintf("%.*s is an enum constructor, not a function",
_temp89.last_plus_one - _temp89.curr, _temp89.curr);})); _npop_handler( 0u);
return; _LL56:;}; _pop_handler();} else{ void* _temp53=( void*) _exn_thrown;
void* _temp91= _temp53; _LL93: if( _temp91 == Cyc_Dict_Absent){ goto _LL94;}
else{ goto _LL95;} _LL95: goto _LL96; _LL94: Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp97= Cyc_Absynpp_qvar2string( _temp50); xprintf("unbound identifier %.*s",
_temp97.last_plus_one - _temp97.curr, _temp97.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp98=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp98[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp99; _temp99.tag= Cyc_Absyn_FnCall_e; _temp99.f1= e1; _temp99.f2= es;
_temp99;}); _temp98;}))); return; _LL96:( void) _throw( _temp91); _LL92:;}}}
_LL49:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp100=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp100[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp101; _temp101.tag= Cyc_Absyn_FnCall_e;
_temp101.f1= e1; _temp101.f2= es; _temp101;}); _temp100;}))); return; _LL45:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp102=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp102[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp103; _temp103.tag= Cyc_Absyn_Array_e;
_temp103.f1= des; _temp103;}); _temp102;}))); return;}{ void* t=({ void**
_temp140= topt; if( _temp140 == 0){ _throw( Null_Exception);}* _temp140;}); void*
_temp104= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp114; struct Cyc_Absyn_Tqual
_temp116; void* _temp118; struct Cyc_Absyn_Structdecl** _temp120; struct Cyc_List_List*
_temp122; struct _tuple1* _temp124; struct Cyc_List_List* _temp126; _LL106: if((
unsigned int) _temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_ArrayType: 0){
_LL119: _temp118=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp104)->f1;
goto _LL117; _LL117: _temp116=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp104)->f2; goto _LL115; _LL115: _temp114=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp104)->f3; goto _LL107;} else{ goto _LL108;}
_LL108: if(( unsigned int) _temp104 > 4u?*(( int*) _temp104) == Cyc_Absyn_StructType:
0){ _LL125: _temp124=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp104)->f1; goto _LL123; _LL123: _temp122=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp104)->f2; goto _LL121; _LL121: _temp120=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp104)->f3;
goto _LL109;} else{ goto _LL110;} _LL110: if(( unsigned int) _temp104 > 4u?*((
int*) _temp104) == Cyc_Absyn_AnonStructType: 0){ _LL127: _temp126=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp104)->f1; goto _LL111;} else{ goto
_LL112;} _LL112: goto _LL113; _LL107:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp128=( struct Cyc_Absyn_Array_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp128[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp129; _temp129.tag= Cyc_Absyn_Array_e; _temp129.f1= des; _temp129;});
_temp128;}))); goto _LL105; _LL109: if( _temp120 == 0? 1: _temp124 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp130=( char*)"Compiler bug: struct type not properly set";
struct _tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 43; _temp131;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp132=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp132[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp133; _temp133.tag= Cyc_Absyn_Struct_e; _temp133.f1=(
struct _tuple1*)({ struct _tuple1* _temp134= _temp124; if( _temp134 == 0){
_throw( Null_Exception);} _temp134;}); _temp133.f2= 0; _temp133.f3= des;
_temp133.f4=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl**
_temp135= _temp120; if( _temp135 == 0){ _throw( Null_Exception);}* _temp135;});
_temp133;}); _temp132;}))); goto _LL105; _LL111:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_AnonStruct_e_struct* _temp136=( struct Cyc_Absyn_AnonStruct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct)); _temp136[ 0]=({
struct Cyc_Absyn_AnonStruct_e_struct _temp137; _temp137.tag= Cyc_Absyn_AnonStruct_e;
_temp137.f1=( void*) t; _temp137.f2= des; _temp137;}); _temp136;}))); goto
_LL105; _LL113:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp138=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp138[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp139; _temp139.tag= Cyc_Absyn_Array_e;
_temp139.f1= des; _temp139;}); _temp138;}))); goto _LL105; _LL105:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp141= es; if( _temp141 == 0){ _throw( Null_Exception);} _temp141->tl;})){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp142=
es; if( _temp142 == 0){ _throw( Null_Exception);} _temp142->hd;}));}} void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp( te,( void**)&
Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp144= msg_part; struct _tagged_string
_temp145= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp143= e->topt;
if( _temp143 == 0){ _throw( Null_Exception);} _temp143->v;})); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp144.last_plus_one - _temp144.curr, _temp144.curr, _temp145.last_plus_one -
_temp145.curr, _temp145.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp146= c; char _temp162; void* _temp164; short _temp166;
void* _temp168; long long _temp170; void* _temp172; struct _tagged_string
_temp174; int _temp176; void* _temp178; struct _tagged_string _temp180; _LL148:
if(( unsigned int) _temp146 > 1u?*(( int*) _temp146) == Cyc_Absyn_Char_c: 0){
_LL165: _temp164=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp146)->f1; goto
_LL163; _LL163: _temp162=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp146)->f2;
goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int) _temp146 > 1u?*((
int*) _temp146) == Cyc_Absyn_Short_c: 0){ _LL169: _temp168=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp146)->f1; goto _LL167; _LL167: _temp166=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp146)->f2; goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp146 > 1u?*(( int*) _temp146) == Cyc_Absyn_LongLong_c: 0){ _LL173: _temp172=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp146)->f1; goto _LL171; _LL171:
_temp170=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp146)->f2; goto
_LL153;} else{ goto _LL154;} _LL154: if(( unsigned int) _temp146 > 1u?*(( int*)
_temp146) == Cyc_Absyn_Float_c: 0){ _LL175: _temp174=( struct _tagged_string)((
struct Cyc_Absyn_Float_c_struct*) _temp146)->f1; goto _LL155;} else{ goto _LL156;}
_LL156: if(( unsigned int) _temp146 > 1u?*(( int*) _temp146) == Cyc_Absyn_Int_c:
0){ _LL179: _temp178=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp146)->f1;
goto _LL177; _LL177: _temp176=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp146)->f2;
goto _LL157;} else{ goto _LL158;} _LL158: if(( unsigned int) _temp146 > 1u?*((
int*) _temp146) == Cyc_Absyn_String_c: 0){ _LL181: _temp180=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp146)->f1; goto _LL159;}
else{ goto _LL160;} _LL160: if( _temp146 ==( void*) Cyc_Absyn_Null_c){ goto
_LL161;} else{ goto _LL147;} _LL149: t= _temp164 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL147; _LL151: t= _temp168 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL147; _LL153:
t= _temp172 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t;
goto _LL147; _LL155: t= Cyc_Absyn_float_t; goto _LL147; _LL157: if( topt == 0){
t= _temp178 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp183=({ void** _temp182= topt; if( _temp182 == 0){ _throw(
Null_Exception);}* _temp182;}); void* _temp193; void* _temp195; void* _temp197;
void* _temp199; void* _temp201; void* _temp203; _LL185: if(( unsigned int)
_temp183 > 4u?*(( int*) _temp183) == Cyc_Absyn_IntType: 0){ _LL196: _temp195=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f1; goto _LL194; _LL194:
_temp193=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f2; if(
_temp193 ==( void*) Cyc_Absyn_B1){ goto _LL186;} else{ goto _LL187;}} else{ goto
_LL187;} _LL187: if(( unsigned int) _temp183 > 4u?*(( int*) _temp183) == Cyc_Absyn_IntType:
0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp183)->f1;
goto _LL198; _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp183)->f2; if( _temp197 ==( void*) Cyc_Absyn_B2){ goto _LL188;} else{ goto
_LL189;}} else{ goto _LL189;} _LL189: if(( unsigned int) _temp183 > 4u?*(( int*)
_temp183) == Cyc_Absyn_IntType: 0){ _LL204: _temp203=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp183)->f1; goto _LL202; _LL202: _temp201=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp183)->f2; goto _LL190;} else{ goto _LL191;} _LL191: goto _LL192; _LL186: t=
_temp195 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp205=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp205[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp206; _temp206.tag= Cyc_Absyn_Const_e;
_temp206.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp207=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp207[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp208; _temp208.tag= Cyc_Absyn_Char_c;
_temp208.f1=( void*) _temp195; _temp208.f2=( char) _temp176; _temp208;});
_temp207;})); _temp206;}); _temp205;}))); goto _LL184; _LL188: t= _temp199 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp209=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp209[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp210; _temp210.tag= Cyc_Absyn_Const_e; _temp210.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp211=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp211[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp212; _temp212.tag= Cyc_Absyn_Short_c; _temp212.f1=( void*) _temp199;
_temp212.f2=( short) _temp176; _temp212;}); _temp211;})); _temp210;}); _temp209;})));
goto _LL184; _LL190: t= _temp203 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL184; _LL192: t= _temp178 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL184; _LL184:;} goto _LL147; _LL159: {
int len=( int)({ struct _tagged_string _temp261= _temp180;( unsigned int)(
_temp261.last_plus_one - _temp261.curr);}); struct Cyc_Absyn_Const_e_struct* r=({
struct Cyc_Absyn_Const_e_struct* _temp257=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp257[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp258; _temp258.tag= Cyc_Absyn_Const_e; _temp258.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp259=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp259[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp260; _temp260.tag= Cyc_Absyn_Int_c; _temp260.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp260.f2= len; _temp260;}); _temp259;})); _temp258;}); _temp257;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) r, loc); elen->topt=({ struct
Cyc_Core_Opt* _temp213=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp213->v=( void*) Cyc_Absyn_uint_t; _temp213;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp214=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp214[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp215; _temp215.tag= Cyc_Absyn_Upper_b;
_temp215.f1= elen; _temp215;}); _temp214;})); if( topt != 0){ void* _temp217=
Cyc_Tcutil_compress(({ void** _temp216= topt; if( _temp216 == 0){ _throw(
Null_Exception);}* _temp216;})); struct Cyc_Absyn_Exp* _temp225; struct Cyc_Absyn_Tqual
_temp227; void* _temp229; struct Cyc_Absyn_PtrInfo _temp231; struct Cyc_Absyn_Conref*
_temp233; struct Cyc_Absyn_Tqual _temp235; struct Cyc_Absyn_Conref* _temp237;
void* _temp239; void* _temp241; _LL219: if(( unsigned int) _temp217 > 4u?*(( int*)
_temp217) == Cyc_Absyn_ArrayType: 0){ _LL230: _temp229=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp217)->f1; goto _LL228; _LL228: _temp227=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp217)->f2; goto _LL226; _LL226: _temp225=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp217)->f3; goto
_LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp217 > 4u?*(( int*)
_temp217) == Cyc_Absyn_PointerType: 0){ _LL232: _temp231=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp217)->f1; _LL242: _temp241=( void*)
_temp231.elt_typ; goto _LL240; _LL240: _temp239=( void*) _temp231.rgn_typ; goto
_LL238; _LL238: _temp237=( struct Cyc_Absyn_Conref*) _temp231.nullable; goto
_LL236; _LL236: _temp235=( struct Cyc_Absyn_Tqual) _temp231.tq; goto _LL234;
_LL234: _temp233=( struct Cyc_Absyn_Conref*) _temp231.bounds; goto _LL222;}
else{ goto _LL223;} _LL223: goto _LL224; _LL220: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp243=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp243[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp244; _temp244.tag= Cyc_Absyn_ArrayType;
_temp244.f1=( void*) Cyc_Absyn_uchar_t; _temp244.f2= _temp227; _temp244.f3=(
struct Cyc_Absyn_Exp*) elen; _temp244;}); _temp243;}); _LL222: if( ! Cyc_Tcutil_unify(({
void** _temp245= topt; if( _temp245 == 0){ _throw( Null_Exception);}* _temp245;}),
t)? Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp246= topt; if(
_temp246 == 0){ _throw( Null_Exception);}* _temp246;})): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp247=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp247->v=( void*) t; _temp247;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp248= topt; if( _temp248 == 0){ _throw( Null_Exception);}* _temp248;})); t=({
void** _temp249= topt; if( _temp249 == 0){ _throw( Null_Exception);}* _temp249;});}
else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp250=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp250[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp251; _temp251.tag= Cyc_Absyn_Upper_b;
_temp251.f1= elen; _temp251;}); _temp250;})); if( ! Cyc_Tcutil_unify(({ void**
_temp252= topt; if( _temp252 == 0){ _throw( Null_Exception);}* _temp252;}), t)?
Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp253= topt; if( _temp253 ==
0){ _throw( Null_Exception);}* _temp253;})): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp254=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp254->v=( void*) t; _temp254;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp255= topt; if( _temp255 == 0){ _throw( Null_Exception);}* _temp255;})); t=({
void** _temp256= topt; if( _temp256 == 0){ _throw( Null_Exception);}* _temp256;});}}
goto _LL218; _LL224: goto _LL218; _LL218:;} return t;} _LL161: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp262=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp262[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp263; _temp263.tag= Cyc_Absyn_PointerType;
_temp263.f1=({ struct Cyc_Absyn_PtrInfo _temp264; _temp264.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp264.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp264.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp264.tq= Cyc_Absyn_empty_tqual(); _temp264.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp264;}); _temp263;});
_temp262;}); goto _LL147; _LL147:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp265= b; struct Cyc_Absyn_Vardecl* _temp279; struct Cyc_Absyn_Fndecl*
_temp281; struct Cyc_Absyn_Vardecl* _temp283; struct Cyc_Absyn_Vardecl* _temp285;
struct Cyc_Absyn_Vardecl* _temp287; _LL267: if( _temp265 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp265 > 1u?*((
int*) _temp265) == Cyc_Absyn_Global_b: 0){ _LL280: _temp279=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp265)->f1; goto _LL270;} else{ goto
_LL271;} _LL271: if(( unsigned int) _temp265 > 1u?*(( int*) _temp265) == Cyc_Absyn_Funname_b:
0){ _LL282: _temp281=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp265)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int)
_temp265 > 1u?*(( int*) _temp265) == Cyc_Absyn_Pat_b: 0){ _LL284: _temp283=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*) _temp265)->f1; goto
_LL274;} else{ goto _LL275;} _LL275: if(( unsigned int) _temp265 > 1u?*(( int*)
_temp265) == Cyc_Absyn_Local_b: 0){ _LL286: _temp285=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp265)->f1; goto _LL276;} else{ goto _LL277;}
_LL277: if(( unsigned int) _temp265 > 1u?*(( int*) _temp265) == Cyc_Absyn_Param_b:
0){ _LL288: _temp287=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp265)->f1; goto _LL278;} else{ goto _LL266;} _LL268: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp289= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp289.last_plus_one - _temp289.curr, _temp289.curr);})); _LL270:({ struct
_tuple1* _temp290= q; unsigned int _temp291= 0; if( _temp291 >= 1u){ _throw(
Null_Exception);} _temp290[ _temp291]=({ struct _tuple1* _temp292= _temp279->name;
unsigned int _temp293= 0; if( _temp293 >= 1u){ _throw( Null_Exception);}
_temp292[ _temp293];});}); return( void*) _temp279->type; _LL272:({ struct
_tuple1* _temp294= q; unsigned int _temp295= 0; if( _temp295 >= 1u){ _throw(
Null_Exception);} _temp294[ _temp295]=({ struct _tuple1* _temp296= _temp281->name;
unsigned int _temp297= 0; if( _temp297 >= 1u){ _throw( Null_Exception);}
_temp296[ _temp297];});}); return Cyc_Tcutil_fndecl2typ( _temp281); _LL274:
_temp285= _temp283; goto _LL276; _LL276: _temp287= _temp285; goto _LL278; _LL278:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp287->type; _LL266:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp298=( void*) fmt->r; void* _temp304; struct _tagged_string _temp306;
_LL300: if(*(( int*) _temp298) == Cyc_Absyn_Const_e){ _LL305: _temp304=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp298)->f1; if(( unsigned int) _temp304 > 1u?*((
int*) _temp304) == Cyc_Absyn_String_c: 0){ _LL307: _temp306=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp304)->f1; goto _LL301;}
else{ goto _LL302;}} else{ goto _LL302;} _LL302: goto _LL303; _LL301: desc_types=
type_getter( _temp306, fmt->loc); goto _LL299; _LL303: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp308=( char*)"expecting a literal format string";
struct _tagged_string _temp309; _temp309.curr= _temp308; _temp309.base= _temp308;
_temp309.last_plus_one= _temp308 + 34; _temp309;})); return; _LL299:;} for( 0;
desc_types != 0? args != 0: 0; desc_types=({ struct Cyc_List_List* _temp310=
desc_types; if( _temp310 == 0){ _throw( Null_Exception);} _temp310->tl;}), args=({
struct Cyc_List_List* _temp311= args; if( _temp311 == 0){ _throw( Null_Exception);}
_temp311->tl;})){ void* t=( void*)({ struct Cyc_List_List* _temp316= desc_types;
if( _temp316 == 0){ _throw( Null_Exception);} _temp316->hd;}); struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp315= args; if( _temp315
== 0){ _throw( Null_Exception);} _temp315->hd;}); if( ! Cyc_Tcutil_coerce_arg(
te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp313= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp314= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp312= e->topt; if( _temp312 == 0){ _throw( Null_Exception);} _temp312->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp313.last_plus_one
- _temp313.curr, _temp313.curr, _temp314.last_plus_one - _temp314.curr, _temp314.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp317=( char*)"too few arguments"; struct _tagged_string _temp318; _temp318.curr=
_temp317; _temp318.base= _temp317; _temp318.last_plus_one= _temp317 + 18;
_temp318;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp319= args; if( _temp319 == 0){ _throw( Null_Exception);}
_temp319->hd;}))->loc,( struct _tagged_string)({ char* _temp320=( char*)"too many arguments";
struct _tagged_string _temp321; _temp321.curr= _temp320; _temp321.base= _temp320;
_temp321.last_plus_one= _temp320 + 19; _temp321;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp370= e->topt; if( _temp370 == 0){ _throw( Null_Exception);} _temp370->v;}));
void* _temp322= p; _LL324: if( _temp322 ==( void*) Cyc_Absyn_Plus){ goto _LL325;}
else{ goto _LL326;} _LL326: if( _temp322 ==( void*) Cyc_Absyn_Minus){ goto
_LL327;} else{ goto _LL328;} _LL328: if( _temp322 ==( void*) Cyc_Absyn_Not){
goto _LL329;} else{ goto _LL330;} _LL330: if( _temp322 ==( void*) Cyc_Absyn_Bitnot){
goto _LL331;} else{ goto _LL332;} _LL332: if( _temp322 ==( void*) Cyc_Absyn_Size){
goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL325: goto _LL327;
_LL327: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp336= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_temp336.last_plus_one - _temp336.curr, _temp336.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp337= e->topt; if( _temp337 == 0){ _throw(
Null_Exception);} _temp337->v;}); _LL329: Cyc_Tcutil_check_contains_assign( e);
if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp338= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp338.last_plus_one - _temp338.curr, _temp338.curr);}));} return Cyc_Absyn_sint_t;
_LL331: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp339= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp339.last_plus_one - _temp339.curr, _temp339.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp340= e->topt; if( _temp340 == 0){ _throw(
Null_Exception);} _temp340->v;}); _LL333:{ void* _temp341= t; struct Cyc_Absyn_Exp*
_temp349; struct Cyc_Absyn_Tqual _temp351; void* _temp353; struct Cyc_Absyn_PtrInfo
_temp355; struct Cyc_Absyn_Conref* _temp357; struct Cyc_Absyn_Tqual _temp359;
struct Cyc_Absyn_Conref* _temp361; void* _temp363; void* _temp365; _LL343: if((
unsigned int) _temp341 > 4u?*(( int*) _temp341) == Cyc_Absyn_ArrayType: 0){
_LL354: _temp353=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp341)->f1;
goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp341)->f2; goto _LL350; _LL350: _temp349=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp341)->f3; goto _LL344;} else{ goto _LL345;}
_LL345: if(( unsigned int) _temp341 > 4u?*(( int*) _temp341) == Cyc_Absyn_PointerType:
0){ _LL356: _temp355=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp341)->f1; _LL366: _temp365=( void*) _temp355.elt_typ; goto _LL364; _LL364:
_temp363=( void*) _temp355.rgn_typ; goto _LL362; _LL362: _temp361=( struct Cyc_Absyn_Conref*)
_temp355.nullable; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Tqual)
_temp355.tq; goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Conref*) _temp355.bounds;
goto _LL346;} else{ goto _LL347;} _LL347: goto _LL348; _LL344: goto _LL342;
_LL346: goto _LL342; _LL348: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp367= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_temp367.last_plus_one - _temp367.curr, _temp367.curr);})); goto _LL342; _LL342:;}
return Cyc_Absyn_uint_t; _LL335: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp368=( char*)"Non-unary primop"; struct
_tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 17; _temp369;})); _LL323:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp372= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp371= e1->topt; if( _temp371 == 0){ _throw(
Null_Exception);} _temp371->v;})); xprintf("type %.*s cannot be used here",
_temp372.last_plus_one - _temp372.curr, _temp372.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp374=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp373= e2->topt; if(
_temp373 == 0){ _throw( Null_Exception);} _temp373->v;})); xprintf("type %.*s cannot be used here",
_temp374.last_plus_one - _temp374.curr, _temp374.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp376= e1->topt;
if( _temp376 == 0){ _throw( Null_Exception);} _temp376->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp375= e2->topt; if( _temp375 == 0){ _throw(
Null_Exception);} _temp375->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp436= e1->topt; if( _temp436 == 0){ _throw( Null_Exception);}
_temp436->v;})); void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp435= e2->topt; if( _temp435 == 0){ _throw( Null_Exception);} _temp435->v;}));
void* _temp377= t1; struct Cyc_Absyn_PtrInfo _temp383; struct Cyc_Absyn_Conref*
_temp385; struct Cyc_Absyn_Tqual _temp387; struct Cyc_Absyn_Conref* _temp389;
void* _temp391; void* _temp393; _LL379: if(( unsigned int) _temp377 > 4u?*(( int*)
_temp377) == Cyc_Absyn_PointerType: 0){ _LL384: _temp383=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp377)->f1; _LL394: _temp393=( void*)
_temp383.elt_typ; goto _LL392; _LL392: _temp391=( void*) _temp383.rgn_typ; goto
_LL390; _LL390: _temp389=( struct Cyc_Absyn_Conref*) _temp383.nullable; goto
_LL388; _LL388: _temp387=( struct Cyc_Absyn_Tqual) _temp383.tq; goto _LL386;
_LL386: _temp385=( struct Cyc_Absyn_Conref*) _temp383.bounds; goto _LL380;}
else{ goto _LL381;} _LL381: goto _LL382; _LL380: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp395= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int but found %.*s", _temp395.last_plus_one - _temp395.curr,
_temp395.curr);}));} _temp385=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp385);{ void* _temp396=( void*) _temp385->v;
void* _temp404; void* _temp406; struct Cyc_Absyn_Exp* _temp408; _LL398: if((
unsigned int) _temp396 > 1u?*(( int*) _temp396) == Cyc_Absyn_Eq_constr: 0){
_LL405: _temp404=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp396)->f1;
if( _temp404 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL399;} else{ goto _LL400;}}
else{ goto _LL400;} _LL400: if(( unsigned int) _temp396 > 1u?*(( int*) _temp396)
== Cyc_Absyn_Eq_constr: 0){ _LL407: _temp406=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp396)->f1; if(( unsigned int) _temp406 > 1u?*(( int*) _temp406) == Cyc_Absyn_Upper_b:
0){ _LL409: _temp408=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp406)->f1; goto _LL401;} else{ goto _LL402;}} else{ goto _LL402;} _LL402:
goto _LL403; _LL399: return t1; _LL401: { unsigned int ub_v= Cyc_Evexp_eval_const_uint_exp(
_temp408); _temp389=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp389);{ int is_nullable;{ void* _temp410=( void*)
_temp389->v; int _temp416; _LL412: if(( unsigned int) _temp410 > 1u?*(( int*)
_temp410) == Cyc_Absyn_Eq_constr: 0){ _LL417: _temp416=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp410)->f1; goto _LL413;} else{ goto _LL414;} _LL414: goto _LL415; _LL413:
if( _temp416){ struct Cyc_Absyn_PointerType_struct* new_t1=({ struct Cyc_Absyn_PointerType_struct*
_temp420=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp420[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp421; _temp421.tag= Cyc_Absyn_PointerType;
_temp421.f1=({ struct Cyc_Absyn_PtrInfo _temp422; _temp422.elt_typ=( void*)
_temp393; _temp422.rgn_typ=( void*) _temp391; _temp422.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp422.tq= _temp387; _temp422.bounds=
_temp385; _temp422;}); _temp421;}); _temp420;}); Cyc_Tcutil_warn( e1->loc,({
struct _tagged_string _temp418= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp419= Cyc_Absynpp_typ2string(( void*) new_t1); xprintf("coercing from %.*s to %.*s",
_temp418.last_plus_one - _temp418.curr, _temp418.curr, _temp419.last_plus_one -
_temp419.curr, _temp419.curr);})); Cyc_Tcutil_unchecked_cast( te, e1,( void*)
new_t1);} goto _LL411; _LL415:( void*)( _temp389->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp423=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp423[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp424; _temp424.tag= Cyc_Absyn_Eq_constr; _temp424.f1=(
void*) 0; _temp424;}); _temp423;}))); goto _LL411; _LL411:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int e2_v= Cyc_Evexp_eval_const_uint_exp( e2); if( ub_v < e2_v){
e2_v= ub_v; Cyc_Tcutil_warn( e1->loc, xprintf("pointer arithmetic is out of bounds"));}{
struct Cyc_Absyn_Exp* new_bound= Cyc_Absyn_uint_exp( ub_v - e2_v, 0); new_bound->topt=({
struct Cyc_Core_Opt* _temp425=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp425->v=( void*) Cyc_Absyn_uint_t; _temp425;}); return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp426=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp426[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp427; _temp427.tag= Cyc_Absyn_PointerType;
_temp427.f1=({ struct Cyc_Absyn_PtrInfo _temp428; _temp428.elt_typ=( void*)
_temp393; _temp428.rgn_typ=( void*) _temp391; _temp428.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp428.tq= _temp387; _temp428.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp429=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp429[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp430; _temp430.tag= Cyc_Absyn_Upper_b; _temp430.f1= new_bound; _temp430;});
_temp429;})); _temp428;}); _temp427;}); _temp426;});}} else{ Cyc_Tcutil_terr( e1->loc,(
struct _tagged_string)({ char* _temp431=( char*)"cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.";
struct _tagged_string _temp432; _temp432.curr= _temp431; _temp432.base= _temp431;
_temp432.last_plus_one= _temp431 + 98; _temp432;})); return t1;}}} _LL403:( void*)(
_temp385->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp433=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp433[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp434; _temp434.tag= Cyc_Absyn_Eq_constr;
_temp434.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp434;}); _temp433;})));
return t1; _LL397:;} _LL382: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL378:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp444=
e1->topt; if( _temp444 == 0){ _throw( Null_Exception);} _temp444->v;}); void* t2=(
void*)({ struct Cyc_Core_Opt* _temp443= e2->topt; if( _temp443 == 0){ _throw(
Null_Exception);} _temp443->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){
if( Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){
Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp439= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp437= e1->topt; if( _temp437 == 0){ _throw(
Null_Exception);} _temp437->v;})); struct _tagged_string _temp440= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp438= e2->topt; if( _temp438 == 0){ _throw(
Null_Exception);} _temp438->v;})); xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp439.last_plus_one - _temp439.curr, _temp439.curr, _temp440.last_plus_one -
_temp440.curr, _temp440.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp441= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp442= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp441.last_plus_one - _temp441.curr, _temp441.curr, _temp442.last_plus_one -
_temp442.curr, _temp442.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp485= e1->topt; if( _temp485 == 0){ _throw(
Null_Exception);} _temp485->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp484= e2->topt; if( _temp484 == 0){ _throw(
Null_Exception);} _temp484->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp483= e1->topt; if( _temp483 == 0){ _throw( Null_Exception);} _temp483->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp482= e2->topt; if( _temp482 == 0){
_throw( Null_Exception);} _temp482->v;});{ struct _tuple0 _temp446=({ struct
_tuple0 _temp445; _temp445.f1= Cyc_Tcutil_compress( t1); _temp445.f2= Cyc_Tcutil_compress(
t2); _temp445;}); void* _temp452; struct Cyc_Absyn_PtrInfo _temp454; struct Cyc_Absyn_Conref*
_temp456; struct Cyc_Absyn_Tqual _temp458; struct Cyc_Absyn_Conref* _temp460;
void* _temp462; void* _temp464; void* _temp466; struct Cyc_Absyn_PtrInfo
_temp468; struct Cyc_Absyn_Conref* _temp470; struct Cyc_Absyn_Tqual _temp472;
struct Cyc_Absyn_Conref* _temp474; void* _temp476; void* _temp478; _LL448:
_LL467: _temp466= _temp446.f1; if(( unsigned int) _temp466 > 4u?*(( int*)
_temp466) == Cyc_Absyn_PointerType: 0){ _LL469: _temp468=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp466)->f1; _LL479: _temp478=( void*)
_temp468.elt_typ; goto _LL477; _LL477: _temp476=( void*) _temp468.rgn_typ; goto
_LL475; _LL475: _temp474=( struct Cyc_Absyn_Conref*) _temp468.nullable; goto
_LL473; _LL473: _temp472=( struct Cyc_Absyn_Tqual) _temp468.tq; goto _LL471;
_LL471: _temp470=( struct Cyc_Absyn_Conref*) _temp468.bounds; goto _LL453;}
else{ goto _LL450;} _LL453: _temp452= _temp446.f2; if(( unsigned int) _temp452 >
4u?*(( int*) _temp452) == Cyc_Absyn_PointerType: 0){ _LL455: _temp454=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp452)->f1; _LL465:
_temp464=( void*) _temp454.elt_typ; goto _LL463; _LL463: _temp462=( void*)
_temp454.rgn_typ; goto _LL461; _LL461: _temp460=( struct Cyc_Absyn_Conref*)
_temp454.nullable; goto _LL459; _LL459: _temp458=( struct Cyc_Absyn_Tqual)
_temp454.tq; goto _LL457; _LL457: _temp456=( struct Cyc_Absyn_Conref*) _temp454.bounds;
goto _LL449;} else{ goto _LL450;} _LL450: goto _LL451; _LL449: if( Cyc_Tcutil_unify(
_temp478, _temp464)){ return Cyc_Absyn_sint_t;} goto _LL447; _LL451: goto _LL447;
_LL447:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp480= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp481= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp480.last_plus_one - _temp480.curr, _temp480.curr, _temp481.last_plus_one -
_temp481.curr, _temp481.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp486= p; _LL488: if( _temp486 ==( void*) Cyc_Absyn_Plus){ goto _LL489;}
else{ goto _LL490;} _LL490: if( _temp486 ==( void*) Cyc_Absyn_Minus){ goto
_LL491;} else{ goto _LL492;} _LL492: if( _temp486 ==( void*) Cyc_Absyn_Times){
goto _LL493;} else{ goto _LL494;} _LL494: if( _temp486 ==( void*) Cyc_Absyn_Div){
goto _LL495;} else{ goto _LL496;} _LL496: if( _temp486 ==( void*) Cyc_Absyn_Mod){
goto _LL497;} else{ goto _LL498;} _LL498: if( _temp486 ==( void*) Cyc_Absyn_Bitand){
goto _LL499;} else{ goto _LL500;} _LL500: if( _temp486 ==( void*) Cyc_Absyn_Bitor){
goto _LL501;} else{ goto _LL502;} _LL502: if( _temp486 ==( void*) Cyc_Absyn_Bitxor){
goto _LL503;} else{ goto _LL504;} _LL504: if( _temp486 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL505;} else{ goto _LL506;} _LL506: if( _temp486 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL507;} else{ goto _LL508;} _LL508: if( _temp486 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL509;} else{ goto _LL510;} _LL510: if( _temp486 ==( void*) Cyc_Absyn_Eq){
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp486 ==( void*) Cyc_Absyn_Neq){
goto _LL513;} else{ goto _LL514;} _LL514: if( _temp486 ==( void*) Cyc_Absyn_Gt){
goto _LL515;} else{ goto _LL516;} _LL516: if( _temp486 ==( void*) Cyc_Absyn_Lt){
goto _LL517;} else{ goto _LL518;} _LL518: if( _temp486 ==( void*) Cyc_Absyn_Gte){
goto _LL519;} else{ goto _LL520;} _LL520: if( _temp486 ==( void*) Cyc_Absyn_Lte){
goto _LL521;} else{ goto _LL522;} _LL522: goto _LL523; _LL489: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL491: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL493: goto _LL495;
_LL495: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL497: goto _LL499; _LL499: goto _LL501; _LL501: goto _LL503; _LL503: goto
_LL505; _LL505: goto _LL507; _LL507: goto _LL509; _LL509: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL511: goto _LL513; _LL513: goto _LL515;
_LL515: goto _LL517; _LL517: goto _LL519; _LL519: goto _LL521; _LL521: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL523:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp524=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp524[ 0]=({ struct Cyc_Core_Failure_struct
_temp525; _temp525.tag= Cyc_Core_Failure; _temp525.f1=( struct _tagged_string)({
char* _temp526=( char*)"bad binary primop"; struct _tagged_string _temp527;
_temp527.curr= _temp526; _temp527.base= _temp526; _temp527.last_plus_one=
_temp526 + 18; _temp527;}); _temp525;}); _temp524;})); _LL487:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp528= es; if(
_temp528 == 0){ _throw( Null_Exception);} _temp528->hd;}));} Cyc_Tcexp_tcExpList(
te, es);{ void* t;{ void* _temp529= p; _LL531: if( _temp529 ==( void*) Cyc_Absyn_Printf){
goto _LL532;} else{ goto _LL533;} _LL533: if( _temp529 ==( void*) Cyc_Absyn_Fprintf){
goto _LL534;} else{ goto _LL535;} _LL535: if( _temp529 ==( void*) Cyc_Absyn_Xprintf){
goto _LL536;} else{ goto _LL537;} _LL537: if( _temp529 ==( void*) Cyc_Absyn_Scanf){
goto _LL538;} else{ goto _LL539;} _LL539: if( _temp529 ==( void*) Cyc_Absyn_Fscanf){
goto _LL540;} else{ goto _LL541;} _LL541: if( _temp529 ==( void*) Cyc_Absyn_Sscanf){
goto _LL542;} else{ goto _LL543;} _LL543: goto _LL544; _LL532: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp545=(
char*)"missing format string in printf"; struct _tagged_string _temp546;
_temp546.curr= _temp545; _temp546.base= _temp545; _temp546.last_plus_one=
_temp545 + 32; _temp546;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp547= es; if( _temp547 == 0){ _throw( Null_Exception);}
_temp547->hd;}),({ struct Cyc_List_List* _temp548= es; if( _temp548 == 0){
_throw( Null_Exception);} _temp548->tl;}), Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL530; _LL534: if( es == 0? 1:({ struct Cyc_List_List*
_temp549= es; if( _temp549 == 0){ _throw( Null_Exception);} _temp549->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp550=(
char*)"too few arguments to fprintf"; struct _tagged_string _temp551; _temp551.curr=
_temp550; _temp551.base= _temp550; _temp551.last_plus_one= _temp550 + 29;
_temp551;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp553=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp552= es; if( _temp552 == 0){
_throw( Null_Exception);} _temp552->hd;}))->topt; if( _temp553 == 0){ _throw(
Null_Exception);} _temp553->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp554= es; if( _temp554 == 0){
_throw( Null_Exception);} _temp554->hd;}))->loc,({ struct _tagged_string
_temp557= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp556=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp555= es; if( _temp555 == 0){
_throw( Null_Exception);} _temp555->hd;}))->topt; if( _temp556 == 0){ _throw(
Null_Exception);} _temp556->v;})); xprintf("expecting FILE @ but found %.*s",
_temp557.last_plus_one - _temp557.curr, _temp557.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp559=({ struct Cyc_List_List*
_temp558= es; if( _temp558 == 0){ _throw( Null_Exception);} _temp558->tl;}); if(
_temp559 == 0){ _throw( Null_Exception);} _temp559->hd;}),({ struct Cyc_List_List*
_temp561=({ struct Cyc_List_List* _temp560= es; if( _temp560 == 0){ _throw(
Null_Exception);} _temp560->tl;}); if( _temp561 == 0){ _throw( Null_Exception);}
_temp561->tl;}), Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL530; _LL536: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp562=( char*)"missing format string in xprintf";
struct _tagged_string _temp563; _temp563.curr= _temp562; _temp563.base= _temp562;
_temp563.last_plus_one= _temp562 + 33; _temp563;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp564= es; if( _temp564
== 0){ _throw( Null_Exception);} _temp564->hd;}),({ struct Cyc_List_List*
_temp565= es; if( _temp565 == 0){ _throw( Null_Exception);} _temp565->tl;}), Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL530; _LL538: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp566=( char*)"missing format string in scanf";
struct _tagged_string _temp567; _temp567.curr= _temp566; _temp567.base= _temp566;
_temp567.last_plus_one= _temp566 + 31; _temp567;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp568= es; if( _temp568
== 0){ _throw( Null_Exception);} _temp568->hd;}),({ struct Cyc_List_List*
_temp569= es; if( _temp569 == 0){ _throw( Null_Exception);} _temp569->tl;}), Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL530; _LL540: if( es == 0? 1:({ struct Cyc_List_List*
_temp570= es; if( _temp570 == 0){ _throw( Null_Exception);} _temp570->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp571=(
char*)"too few arguments to fscanf"; struct _tagged_string _temp572; _temp572.curr=
_temp571; _temp572.base= _temp571; _temp572.last_plus_one= _temp571 + 28;
_temp572;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp574=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp573= es; if( _temp573 == 0){
_throw( Null_Exception);} _temp573->hd;}))->topt; if( _temp574 == 0){ _throw(
Null_Exception);} _temp574->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp575= es; if( _temp575 == 0){
_throw( Null_Exception);} _temp575->hd;}))->loc,({ struct _tagged_string
_temp578= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp577=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp576= es; if( _temp576 == 0){
_throw( Null_Exception);} _temp576->hd;}))->topt; if( _temp577 == 0){ _throw(
Null_Exception);} _temp577->v;})); xprintf("expecting FILE @ but found %.*s",
_temp578.last_plus_one - _temp578.curr, _temp578.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp580=({ struct Cyc_List_List*
_temp579= es; if( _temp579 == 0){ _throw( Null_Exception);} _temp579->tl;}); if(
_temp580 == 0){ _throw( Null_Exception);} _temp580->hd;}),({ struct Cyc_List_List*
_temp582=({ struct Cyc_List_List* _temp581= es; if( _temp581 == 0){ _throw(
Null_Exception);} _temp581->tl;}); if( _temp582 == 0){ _throw( Null_Exception);}
_temp582->tl;}), Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL530;
_LL542: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp583=( char*)"sscanf is not supported";
struct _tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 24; _temp584;})); t= Cyc_Absyn_sint_t; goto
_LL530; _LL544: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL585: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp587=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 35; _temp588;})); case 1: _LL586: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp590= es;
if( _temp590 == 0){ _throw( Null_Exception);} _temp590->hd;})); break; case 2:
_LL589: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp592= es; if( _temp592 == 0){ _throw( Null_Exception);}
_temp592->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp594=({
struct Cyc_List_List* _temp593= es; if( _temp593 == 0){ _throw( Null_Exception);}
_temp593->tl;}); if( _temp594 == 0){ _throw( Null_Exception);} _temp594->hd;}));
break; default: _LL591: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp596=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 37; _temp597;}));} goto _LL530; _LL530:;}
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp598=( void*) e->r; void* _temp618; struct Cyc_Absyn_Vardecl* _temp620;
struct _tuple1* _temp622; void* _temp624; struct Cyc_Absyn_Vardecl* _temp626;
struct _tuple1* _temp628; void* _temp630; struct Cyc_Absyn_Vardecl* _temp632;
struct _tuple1* _temp634; void* _temp636; struct Cyc_Absyn_Vardecl* _temp638;
struct _tuple1* _temp640; struct Cyc_Absyn_Exp* _temp642; struct Cyc_Absyn_Exp*
_temp644; struct _tagged_string* _temp646; struct Cyc_Absyn_Exp* _temp648;
struct _tagged_string* _temp650; struct Cyc_Absyn_Exp* _temp652; struct Cyc_Absyn_Exp*
_temp654; _LL600: if(*(( int*) _temp598) == Cyc_Absyn_Var_e){ _LL623: _temp622=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp598)->f1; goto _LL619;
_LL619: _temp618=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp598)->f2; if((
unsigned int) _temp618 > 1u?*(( int*) _temp618) == Cyc_Absyn_Param_b: 0){ _LL621:
_temp620=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp618)->f1; goto _LL601;} else{ goto _LL602;}} else{ goto _LL602;} _LL602:
if(*(( int*) _temp598) == Cyc_Absyn_Var_e){ _LL629: _temp628=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp598)->f1; goto _LL625; _LL625: _temp624=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp598)->f2; if(( unsigned int)
_temp624 > 1u?*(( int*) _temp624) == Cyc_Absyn_Local_b: 0){ _LL627: _temp626=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp624)->f1;
goto _LL603;} else{ goto _LL604;}} else{ goto _LL604;} _LL604: if(*(( int*)
_temp598) == Cyc_Absyn_Var_e){ _LL635: _temp634=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp598)->f1; goto _LL631; _LL631: _temp630=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp598)->f2; if(( unsigned int) _temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Pat_b:
0){ _LL633: _temp632=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp630)->f1; goto _LL605;} else{ goto _LL606;}} else{ goto _LL606;} _LL606:
if(*(( int*) _temp598) == Cyc_Absyn_Var_e){ _LL641: _temp640=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp598)->f1; goto _LL637; _LL637: _temp636=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp598)->f2; if(( unsigned int)
_temp636 > 1u?*(( int*) _temp636) == Cyc_Absyn_Global_b: 0){ _LL639: _temp638=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp636)->f1;
goto _LL607;} else{ goto _LL608;}} else{ goto _LL608;} _LL608: if(*(( int*)
_temp598) == Cyc_Absyn_Subscript_e){ _LL645: _temp644=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp598)->f1; goto _LL643; _LL643:
_temp642=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp598)->f2; goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp598)
== Cyc_Absyn_StructMember_e){ _LL649: _temp648=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp598)->f1; goto _LL647; _LL647: _temp646=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp598)->f2;
goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp598) == Cyc_Absyn_StructArrow_e){
_LL653: _temp652=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp598)->f1; goto _LL651; _LL651: _temp650=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp598)->f2; goto _LL613;} else{ goto _LL614;}
_LL614: if(*(( int*) _temp598) == Cyc_Absyn_Deref_e){ _LL655: _temp654=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp598)->f1; goto _LL615;}
else{ goto _LL616;} _LL616: goto _LL617; _LL601: _temp626= _temp620; goto _LL603;
_LL603: _temp632= _temp626; goto _LL605; _LL605: _temp638= _temp632; goto _LL607;
_LL607: if( !( _temp638->tq).q_const){ return;} goto _LL599; _LL609:{ void*
_temp657= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp656= _temp644->topt;
if( _temp656 == 0){ _throw( Null_Exception);} _temp656->v;})); struct Cyc_Absyn_PtrInfo
_temp667; struct Cyc_Absyn_Conref* _temp669; struct Cyc_Absyn_Tqual _temp671;
struct Cyc_Absyn_Conref* _temp673; void* _temp675; void* _temp677; struct Cyc_Absyn_Exp*
_temp679; struct Cyc_Absyn_Tqual _temp681; void* _temp683; struct Cyc_List_List*
_temp685; _LL659: if(( unsigned int) _temp657 > 4u?*(( int*) _temp657) == Cyc_Absyn_PointerType:
0){ _LL668: _temp667=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp657)->f1; _LL678: _temp677=( void*) _temp667.elt_typ; goto _LL676; _LL676:
_temp675=( void*) _temp667.rgn_typ; goto _LL674; _LL674: _temp673=( struct Cyc_Absyn_Conref*)
_temp667.nullable; goto _LL672; _LL672: _temp671=( struct Cyc_Absyn_Tqual)
_temp667.tq; goto _LL670; _LL670: _temp669=( struct Cyc_Absyn_Conref*) _temp667.bounds;
goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int) _temp657 > 4u?*((
int*) _temp657) == Cyc_Absyn_ArrayType: 0){ _LL684: _temp683=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp657)->f1; goto _LL682; _LL682: _temp681=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp657)->f2;
goto _LL680; _LL680: _temp679=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp657)->f3; goto _LL662;} else{ goto _LL663;} _LL663: if(( unsigned int)
_temp657 > 4u?*(( int*) _temp657) == Cyc_Absyn_TupleType: 0){ _LL686: _temp685=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp657)->f1; goto
_LL664;} else{ goto _LL665;} _LL665: goto _LL666; _LL660: _temp681= _temp671;
goto _LL662; _LL662: if( ! _temp681.q_const){ return;} goto _LL658; _LL664: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp642);{ struct _handler_cons
_temp687; _push_handler(& _temp687);{ int _temp689= 0; if( setjmp( _temp687.handler)){
_temp689= 1;} if( ! _temp689){{ struct _tuple7 _temp692; void* _temp693; struct
Cyc_Absyn_Tqual _temp695; struct _tuple7* _temp690=(( struct _tuple7*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp685,( int) i); _temp692=* _temp690;
_LL696: _temp695= _temp692.f1; goto _LL694; _LL694: _temp693= _temp692.f2; goto
_LL691; _LL691: if( ! _temp695.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp688=( void*) _exn_thrown; void* _temp698=
_temp688; _LL700: if( _temp698 == Cyc_List_Nth){ goto _LL701;} else{ goto _LL702;}
_LL702: goto _LL703; _LL701: return; _LL703:( void) _throw( _temp698); _LL699:;}}}
goto _LL658;} _LL666: goto _LL658; _LL658:;} goto _LL599; _LL611:{ void*
_temp705= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp704= _temp648->topt;
if( _temp704 == 0){ _throw( Null_Exception);} _temp704->v;})); struct Cyc_Absyn_Structdecl**
_temp717; struct Cyc_List_List* _temp719; struct _tuple1* _temp721; struct Cyc_List_List*
_temp723; struct Cyc_List_List* _temp725; struct Cyc_Absyn_Uniondecl** _temp727;
struct Cyc_List_List* _temp729; struct _tuple1* _temp731; _LL707: if((
unsigned int) _temp705 > 4u?*(( int*) _temp705) == Cyc_Absyn_StructType: 0){
_LL722: _temp721=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp705)->f1; goto _LL720; _LL720: _temp719=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp705)->f2; goto _LL718; _LL718: _temp717=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp705)->f3;
goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int) _temp705 > 4u?*((
int*) _temp705) == Cyc_Absyn_AnonUnionType: 0){ _LL724: _temp723=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp705)->f1; goto _LL710;} else{ goto
_LL711;} _LL711: if(( unsigned int) _temp705 > 4u?*(( int*) _temp705) == Cyc_Absyn_AnonStructType:
0){ _LL726: _temp725=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp705)->f1; goto _LL712;} else{ goto _LL713;} _LL713: if(( unsigned int)
_temp705 > 4u?*(( int*) _temp705) == Cyc_Absyn_UnionType: 0){ _LL732: _temp731=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp705)->f1; goto
_LL730; _LL730: _temp729=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp705)->f2; goto _LL728; _LL728: _temp727=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp705)->f3; goto _LL714;} else{ goto
_LL715;} _LL715: goto _LL716; _LL708: { struct Cyc_Absyn_Structfield* sf=
_temp717 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp734= _temp717; if( _temp734 == 0){ _throw(
Null_Exception);}* _temp734;}), _temp646); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp733= sf; if( _temp733 == 0){ _throw( Null_Exception);} _temp733->tq;}).q_const){
return;} goto _LL706;} _LL710: _temp725= _temp723; goto _LL712; _LL712: { struct
Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field( _temp725, _temp646); if( sf
== 0? 1: !({ struct Cyc_Absyn_Structfield* _temp735= sf; if( _temp735 == 0){
_throw( Null_Exception);} _temp735->tq;}).q_const){ return;} goto _LL706;}
_LL714: { struct Cyc_Absyn_Structfield* sf= _temp727 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)({ struct Cyc_Absyn_Uniondecl** _temp737= _temp727;
if( _temp737 == 0){ _throw( Null_Exception);}* _temp737;}), _temp646); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp736= sf; if( _temp736 == 0){ _throw(
Null_Exception);} _temp736->tq;}).q_const){ return;} goto _LL706;} _LL716: goto
_LL706; _LL706:;} goto _LL599; _LL613:{ void* _temp739= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp738= _temp652->topt; if( _temp738 == 0){
_throw( Null_Exception);} _temp738->v;})); struct Cyc_Absyn_PtrInfo _temp745;
struct Cyc_Absyn_Conref* _temp747; struct Cyc_Absyn_Tqual _temp749; struct Cyc_Absyn_Conref*
_temp751; void* _temp753; void* _temp755; _LL741: if(( unsigned int) _temp739 >
4u?*(( int*) _temp739) == Cyc_Absyn_PointerType: 0){ _LL746: _temp745=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp739)->f1; _LL756:
_temp755=( void*) _temp745.elt_typ; goto _LL754; _LL754: _temp753=( void*)
_temp745.rgn_typ; goto _LL752; _LL752: _temp751=( struct Cyc_Absyn_Conref*)
_temp745.nullable; goto _LL750; _LL750: _temp749=( struct Cyc_Absyn_Tqual)
_temp745.tq; goto _LL748; _LL748: _temp747=( struct Cyc_Absyn_Conref*) _temp745.bounds;
goto _LL742;} else{ goto _LL743;} _LL743: goto _LL744; _LL742:{ void* _temp757=
Cyc_Tcutil_compress( _temp755); struct Cyc_Absyn_Structdecl** _temp769; struct
Cyc_List_List* _temp771; struct _tuple1* _temp773; struct Cyc_Absyn_Uniondecl**
_temp775; struct Cyc_List_List* _temp777; struct _tuple1* _temp779; struct Cyc_List_List*
_temp781; struct Cyc_List_List* _temp783; _LL759: if(( unsigned int) _temp757 >
4u?*(( int*) _temp757) == Cyc_Absyn_StructType: 0){ _LL774: _temp773=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp757)->f1; goto _LL772;
_LL772: _temp771=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp757)->f2; goto _LL770; _LL770: _temp769=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp757)->f3; goto _LL760;} else{ goto
_LL761;} _LL761: if(( unsigned int) _temp757 > 4u?*(( int*) _temp757) == Cyc_Absyn_UnionType:
0){ _LL780: _temp779=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp757)->f1; goto _LL778; _LL778: _temp777=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp757)->f2; goto _LL776; _LL776: _temp775=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp757)->f3;
goto _LL762;} else{ goto _LL763;} _LL763: if(( unsigned int) _temp757 > 4u?*((
int*) _temp757) == Cyc_Absyn_AnonStructType: 0){ _LL782: _temp781=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp757)->f1; goto _LL764;} else{ goto
_LL765;} _LL765: if(( unsigned int) _temp757 > 4u?*(( int*) _temp757) == Cyc_Absyn_AnonUnionType:
0){ _LL784: _temp783=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp757)->f1; goto _LL766;} else{ goto _LL767;} _LL767: goto _LL768; _LL760: {
struct Cyc_Absyn_Structfield* sf= _temp769 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl** _temp786= _temp769;
if( _temp786 == 0){ _throw( Null_Exception);}* _temp786;}), _temp650); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp785= sf; if( _temp785 == 0){ _throw(
Null_Exception);} _temp785->tq;}).q_const){ return;} goto _LL758;} _LL762: {
struct Cyc_Absyn_Structfield* sf= _temp775 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)({ struct Cyc_Absyn_Uniondecl** _temp788= _temp775;
if( _temp788 == 0){ _throw( Null_Exception);}* _temp788;}), _temp650); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp787= sf; if( _temp787 == 0){ _throw(
Null_Exception);} _temp787->tq;}).q_const){ return;} goto _LL758;} _LL764:
_temp783= _temp781; goto _LL766; _LL766: { struct Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field(
_temp783, _temp650); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield* _temp789=
sf; if( _temp789 == 0){ _throw( Null_Exception);} _temp789->tq;}).q_const){
return;} goto _LL758;} _LL768: goto _LL758; _LL758:;} goto _LL740; _LL744: goto
_LL740; _LL740:;} goto _LL599; _LL615:{ void* _temp791= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp790= _temp654->topt; if( _temp790 == 0){
_throw( Null_Exception);} _temp790->v;})); struct Cyc_Absyn_PtrInfo _temp799;
struct Cyc_Absyn_Conref* _temp801; struct Cyc_Absyn_Tqual _temp803; struct Cyc_Absyn_Conref*
_temp805; void* _temp807; void* _temp809; struct Cyc_Absyn_Exp* _temp811; struct
Cyc_Absyn_Tqual _temp813; void* _temp815; _LL793: if(( unsigned int) _temp791 >
4u?*(( int*) _temp791) == Cyc_Absyn_PointerType: 0){ _LL800: _temp799=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp791)->f1; _LL810:
_temp809=( void*) _temp799.elt_typ; goto _LL808; _LL808: _temp807=( void*)
_temp799.rgn_typ; goto _LL806; _LL806: _temp805=( struct Cyc_Absyn_Conref*)
_temp799.nullable; goto _LL804; _LL804: _temp803=( struct Cyc_Absyn_Tqual)
_temp799.tq; goto _LL802; _LL802: _temp801=( struct Cyc_Absyn_Conref*) _temp799.bounds;
goto _LL794;} else{ goto _LL795;} _LL795: if(( unsigned int) _temp791 > 4u?*((
int*) _temp791) == Cyc_Absyn_ArrayType: 0){ _LL816: _temp815=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp791)->f1; goto _LL814; _LL814: _temp813=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp791)->f2;
goto _LL812; _LL812: _temp811=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp791)->f3; goto _LL796;} else{ goto _LL797;} _LL797: goto _LL798; _LL794:
_temp813= _temp803; goto _LL796; _LL796: if( ! _temp813.q_const){ return;} goto
_LL792; _LL798: goto _LL792; _LL792:;} goto _LL599; _LL617: goto _LL599; _LL599:;}
Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp817=( char*)"attempt to write a read-only location";
struct _tagged_string _temp818; _temp818.curr= _temp817; _temp818.base= _temp817;
_temp818.last_plus_one= _temp817 + 38; _temp818;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp819=( char*)"increment/decrement of non-lvalue"; struct _tagged_string
_temp820; _temp820.curr= _temp819; _temp820.base= _temp819; _temp820.last_plus_one=
_temp819 + 34; _temp820;}));} Cyc_Tcexp_check_writable( te, e);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp822= e->topt; if( _temp822 == 0){ _throw(
Null_Exception);} _temp822->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp821= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or ? type but found %.*s", _temp821.last_plus_one
- _temp821.curr, _temp821.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp823=( char*)"conditional expression";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 23; _temp824;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp830=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp830->hd=( void*) e3; _temp830->tl=
0; _temp830;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp829=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp829->hd=(
void*) e2; _temp829->tl= l1; _temp829;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp827= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp825= e2->topt; if( _temp825 == 0){ _throw(
Null_Exception);} _temp825->v;})); struct _tagged_string _temp828= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp826= e3->topt; if( _temp826 == 0){ _throw(
Null_Exception);} _temp826->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp827.last_plus_one - _temp827.curr, _temp827.curr, _temp828.last_plus_one -
_temp828.curr, _temp828.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)({ struct Cyc_Core_Opt*
_temp831= e1->topt; if( _temp831 == 0){ _throw( Null_Exception);}& _temp831->v;}))),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp854= e1->topt; if( _temp854
== 0){ _throw( Null_Exception);} _temp854->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp853= e2->topt; if( _temp853 == 0){ _throw( Null_Exception);} _temp853->v;});{
void* _temp832= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp838; struct
Cyc_Absyn_Tqual _temp840; void* _temp842; _LL834: if(( unsigned int) _temp832 >
4u?*(( int*) _temp832) == Cyc_Absyn_ArrayType: 0){ _LL843: _temp842=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp832)->f1; goto _LL841; _LL841: _temp840=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp832)->f2;
goto _LL839; _LL839: _temp838=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp832)->f3; goto _LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp844=( char*)"cannot assign to an array";
struct _tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 26; _temp845;})); goto _LL833; _LL837: goto
_LL833; _LL833:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp846=( char*)"assignment to non-lvalue";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 25; _temp847;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp848= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp849= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp848.last_plus_one - _temp848.curr, _temp848.curr, _temp849.last_plus_one -
_temp849.curr, _temp849.curr);}));}} else{ void* p=( void*)({ struct Cyc_Core_Opt*
_temp852= po; if( _temp852 == 0){ _throw( Null_Exception);} _temp852->v;}); void*
t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify(
t_result, t1)? 1: Cyc_Tcutil_coerceable( t_result))){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp850=( char*)"Cannot use this operator in an assignment";
struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 42; _temp851;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp855= e2->topt; if( _temp855 == 0){ _throw( Null_Exception);} _temp855->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp913= e->topt; if( _temp913 == 0){ _throw( Null_Exception);} _temp913->v;}));
void* _temp856= t; struct Cyc_Absyn_PtrInfo _temp862; struct Cyc_Absyn_Conref*
_temp864; struct Cyc_Absyn_Tqual _temp866; struct Cyc_Absyn_Conref* _temp868;
void* _temp870; void* _temp872; _LL858: if(( unsigned int) _temp856 > 4u?*(( int*)
_temp856) == Cyc_Absyn_PointerType: 0){ _LL863: _temp862=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp856)->f1; _LL873: _temp872=( void*)
_temp862.elt_typ; goto _LL871; _LL871: _temp870=( void*) _temp862.rgn_typ; goto
_LL869; _LL869: _temp868=( struct Cyc_Absyn_Conref*) _temp862.nullable; goto
_LL867; _LL867: _temp866=( struct Cyc_Absyn_Tqual) _temp862.tq; goto _LL865;
_LL865: _temp864=( struct Cyc_Absyn_Conref*) _temp862.bounds; goto _LL859;}
else{ goto _LL860;} _LL860: goto _LL861; _LL859: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp870); Cyc_Tcutil_check_nonzero_bound( loc, _temp864);{ void*
_temp874= Cyc_Tcutil_compress( _temp872); struct Cyc_Absyn_FnInfo _temp880;
struct Cyc_List_List* _temp882; int _temp884; struct Cyc_List_List* _temp886;
void* _temp888; struct Cyc_Core_Opt* _temp890; struct Cyc_List_List* _temp892;
_LL876: if(( unsigned int) _temp874 > 4u?*(( int*) _temp874) == Cyc_Absyn_FnType:
0){ _LL881: _temp880=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp874)->f1; _LL893: _temp892=( struct Cyc_List_List*) _temp880.tvars; goto
_LL891; _LL891: _temp890=( struct Cyc_Core_Opt*) _temp880.effect; goto _LL889;
_LL889: _temp888=( void*) _temp880.ret_typ; goto _LL887; _LL887: _temp886=(
struct Cyc_List_List*) _temp880.args; goto _LL885; _LL885: _temp884=( int)
_temp880.varargs; goto _LL883; _LL883: _temp882=( struct Cyc_List_List*)
_temp880.attributes; goto _LL877;} else{ goto _LL878;} _LL878: goto _LL879;
_LL877: if( _temp884){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp894=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp895; _temp895.curr= _temp894; _temp895.base= _temp894;
_temp895.last_plus_one= _temp894 + 41; _temp895;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp888,({ void** _temp896= topt; if( _temp896 == 0){ _throw( Null_Exception);}*
_temp896;}));} while( es != 0? _temp886 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp903= es; if( _temp903 == 0){
_throw( Null_Exception);} _temp903->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp902= _temp886; if( _temp902 == 0){ _throw(
Null_Exception);} _temp902->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp898= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp897= e1->topt; if( _temp897 == 0){ _throw( Null_Exception);} _temp897->v;}));
struct _tagged_string _temp899= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp898.last_plus_one - _temp898.curr, _temp898.curr, _temp899.last_plus_one -
_temp899.curr, _temp899.curr);}));} es=({ struct Cyc_List_List* _temp900= es;
if( _temp900 == 0){ _throw( Null_Exception);} _temp900->tl;}); _temp886=({
struct Cyc_List_List* _temp901= _temp886; if( _temp901 == 0){ _throw(
Null_Exception);} _temp901->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp904=( char*)"too many arguments for function";
struct _tagged_string _temp905; _temp905.curr= _temp904; _temp905.base= _temp904;
_temp905.last_plus_one= _temp904 + 32; _temp905;}));} if( _temp886 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp906=( char*)"too few arguments for function";
struct _tagged_string _temp907; _temp907.curr= _temp906; _temp907.base= _temp906;
_temp907.last_plus_one= _temp906 + 31; _temp907;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp908= _temp890; if( _temp908 == 0){
_throw( Null_Exception);} _temp908->v;})); return _temp888; _LL879: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp909=( char*)"expected pointer to function";
struct _tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 29; _temp910;})); _LL875:;} _LL861: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp911=( char*)"expected pointer to function";
struct _tagged_string _temp912; _temp912.curr= _temp911; _temp912.base= _temp911;
_temp912.last_plus_one= _temp911 + 29; _temp912;})); _LL857:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp915= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp914= e->topt; if( _temp914 == 0){ _throw( Null_Exception);} _temp914->v;}));
xprintf("expected xtunion exn but found %.*s", _temp915.last_plus_one - _temp915.curr,
_temp915.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp916= e->topt; if( _temp916 == 0){ _throw(
Null_Exception);} _temp916->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp917= e->topt; if( _temp917 == 0){ _throw(
Null_Exception);} _temp917->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp973= e->topt; if( _temp973 == 0){ _throw(
Null_Exception);} _temp973->v;}));{ void* _temp918= t1; struct Cyc_Absyn_PtrInfo
_temp924; struct Cyc_Absyn_Conref* _temp926; struct Cyc_Absyn_Tqual _temp928;
struct Cyc_Absyn_Conref* _temp930; void* _temp932; void* _temp934; _LL920: if((
unsigned int) _temp918 > 4u?*(( int*) _temp918) == Cyc_Absyn_PointerType: 0){
_LL925: _temp924=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp918)->f1; _LL935: _temp934=( void*) _temp924.elt_typ; goto _LL933; _LL933:
_temp932=( void*) _temp924.rgn_typ; goto _LL931; _LL931: _temp930=( struct Cyc_Absyn_Conref*)
_temp924.nullable; goto _LL929; _LL929: _temp928=( struct Cyc_Absyn_Tqual)
_temp924.tq; goto _LL927; _LL927: _temp926=( struct Cyc_Absyn_Conref*) _temp924.bounds;
goto _LL921;} else{ goto _LL922;} _LL922: goto _LL923; _LL921:{ void* _temp936=
Cyc_Tcutil_compress( _temp934); struct Cyc_Absyn_FnInfo _temp942; struct Cyc_List_List*
_temp944; int _temp946; struct Cyc_List_List* _temp948; void* _temp950; struct
Cyc_Core_Opt* _temp952; struct Cyc_List_List* _temp954; _LL938: if((
unsigned int) _temp936 > 4u?*(( int*) _temp936) == Cyc_Absyn_FnType: 0){ _LL943:
_temp942=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp936)->f1;
_LL955: _temp954=( struct Cyc_List_List*) _temp942.tvars; goto _LL953; _LL953:
_temp952=( struct Cyc_Core_Opt*) _temp942.effect; goto _LL951; _LL951: _temp950=(
void*) _temp942.ret_typ; goto _LL949; _LL949: _temp948=( struct Cyc_List_List*)
_temp942.args; goto _LL947; _LL947: _temp946=( int) _temp942.varargs; goto
_LL945; _LL945: _temp944=( struct Cyc_List_List*) _temp942.attributes; goto
_LL939;} else{ goto _LL940;} _LL940: goto _LL941; _LL939: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp954)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp956=( char*)"type instantiation mismatch";
struct _tagged_string _temp957; _temp957.curr= _temp956; _temp957.base= _temp956;
_temp957.last_plus_one= _temp956 + 28; _temp957;}));} for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp958= ts; if( _temp958 == 0){ _throw( Null_Exception);}
_temp958->tl;}), _temp954=({ struct Cyc_List_List* _temp959= _temp954; if(
_temp959 == 0){ _throw( Null_Exception);} _temp959->tl;})){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp965= _temp954; if( _temp965 == 0){ _throw(
Null_Exception);} _temp965->hd;}))->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)({ struct Cyc_List_List* _temp960= ts; if( _temp960 == 0){ _throw(
Null_Exception);} _temp960->hd;})); instantiation=({ struct Cyc_List_List*
_temp961=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp961->hd=( void*)({ struct _tuple4* _temp962=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp962->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp964= _temp954; if( _temp964 == 0){ _throw( Null_Exception);} _temp964->hd;});
_temp962->f2=( void*)({ struct Cyc_List_List* _temp963= ts; if( _temp963 == 0){
_throw( Null_Exception);} _temp963->hd;}); _temp962;}); _temp961->tl=
instantiation; _temp961;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp969=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp969[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp970; _temp970.tag= Cyc_Absyn_FnType; _temp970.f1=({ struct Cyc_Absyn_FnInfo
_temp971; _temp971.tvars= 0; _temp971.effect= _temp952; _temp971.ret_typ=( void*)
_temp950; _temp971.args= _temp948; _temp971.varargs= _temp946; _temp971.attributes=
_temp944; _temp971;}); _temp970;}); _temp969;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp966=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp966[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp967; _temp967.tag= Cyc_Absyn_PointerType;
_temp967.f1=({ struct Cyc_Absyn_PtrInfo _temp968; _temp968.elt_typ=( void*)
new_fn_typ; _temp968.rgn_typ=( void*) _temp932; _temp968.nullable= _temp930;
_temp968.tq= _temp928; _temp968.bounds= _temp926; _temp968;}); _temp967;});
_temp966;}); return new_typ;}} _LL941: goto _LL937; _LL937:;} goto _LL919;
_LL923: goto _LL919; _LL919:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp972= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp972.last_plus_one - _temp972.curr, _temp972.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp976= e->topt; if( _temp976 == 0){ _throw(
Null_Exception);} _temp976->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp974= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp975= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp974.last_plus_one - _temp974.curr, _temp974.curr, _temp975.last_plus_one -
_temp975.curr, _temp975.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp978= Cyc_Tcutil_compress(({ void** _temp977= topt;
if( _temp977 == 0){ _throw( Null_Exception);}* _temp977;})); struct Cyc_Absyn_PtrInfo
_temp984; struct Cyc_Absyn_Conref* _temp986; struct Cyc_Absyn_Tqual _temp988;
struct Cyc_Absyn_Conref* _temp990; void* _temp992; void* _temp994; _LL980: if((
unsigned int) _temp978 > 4u?*(( int*) _temp978) == Cyc_Absyn_PointerType: 0){
_LL985: _temp984=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp978)->f1; _LL995: _temp994=( void*) _temp984.elt_typ; goto _LL993; _LL993:
_temp992=( void*) _temp984.rgn_typ; goto _LL991; _LL991: _temp990=( struct Cyc_Absyn_Conref*)
_temp984.nullable; goto _LL989; _LL989: _temp988=( struct Cyc_Absyn_Tqual)
_temp984.tq; goto _LL987; _LL987: _temp986=( struct Cyc_Absyn_Conref*) _temp984.bounds;
goto _LL981;} else{ goto _LL982;} _LL982: goto _LL983; _LL981: topt2=({ void**
_temp996=( void**) GC_malloc( sizeof( void*)); _temp996[ 0]= _temp994; _temp996;});
tq2= _temp988; goto _LL979; _LL983: goto _LL979; _LL979:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp997=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp1005; struct Cyc_List_List* _temp1007; struct Cyc_Core_Opt* _temp1009;
struct _tuple1* _temp1011; struct Cyc_List_List* _temp1013; _LL999: if(*(( int*)
_temp997) == Cyc_Absyn_Struct_e){ _LL1012: _temp1011=( struct _tuple1*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp997)->f1; goto _LL1010; _LL1010: _temp1009=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp997)->f2; goto
_LL1008; _LL1008: _temp1007=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp997)->f3; goto _LL1006; _LL1006: _temp1005=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp997)->f4; goto _LL1000;} else{ goto
_LL1001;} _LL1001: if(*(( int*) _temp997) == Cyc_Absyn_Tuple_e){ _LL1014:
_temp1013=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp997)->f1;
goto _LL1002;} else{ goto _LL1003;} _LL1003: goto _LL1004; _LL1000: goto _LL1002;
_LL1002: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1015=( char*)"& used to allocate";
struct _tagged_string _temp1016; _temp1016.curr= _temp1015; _temp1016.base=
_temp1015; _temp1016.last_plus_one= _temp1015 + 19; _temp1016;}));{ void*(*
ptr_maker)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1018= Cyc_Tcutil_compress(({ void** _temp1017= topt;
if( _temp1017 == 0){ _throw( Null_Exception);}* _temp1017;})); struct Cyc_Absyn_PtrInfo
_temp1024; struct Cyc_Absyn_Conref* _temp1026; struct Cyc_Absyn_Tqual _temp1028;
struct Cyc_Absyn_Conref* _temp1030; void* _temp1032; void* _temp1034; _LL1020:
if(( unsigned int) _temp1018 > 4u?*(( int*) _temp1018) == Cyc_Absyn_PointerType:
0){ _LL1025: _temp1024=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1018)->f1; _LL1035: _temp1034=( void*) _temp1024.elt_typ; goto _LL1033;
_LL1033: _temp1032=( void*) _temp1024.rgn_typ; goto _LL1031; _LL1031: _temp1030=(
struct Cyc_Absyn_Conref*) _temp1024.nullable; goto _LL1029; _LL1029: _temp1028=(
struct Cyc_Absyn_Tqual) _temp1024.tq; goto _LL1027; _LL1027: _temp1026=( struct
Cyc_Absyn_Conref*) _temp1024.bounds; goto _LL1021;} else{ goto _LL1022;} _LL1022:
goto _LL1023; _LL1021: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1030)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL1019; _LL1023: goto _LL1019;
_LL1019:;} return ptr_maker(( void*)({ struct Cyc_Core_Opt* _temp1036= e->topt;
if( _temp1036 == 0){ _throw( Null_Exception);} _temp1036->v;}),( void*) Cyc_Absyn_HeapRgn,
tq2);} _LL1004: goto _LL998; _LL998:;}{ void* _temp1039; int _temp1041; struct
_tuple5 _temp1037= Cyc_Tcutil_addressof_props( te, e); _LL1042: _temp1041=
_temp1037.f1; goto _LL1040; _LL1040: _temp1039= _temp1037.f2; goto _LL1038;
_LL1038: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if( _temp1041){
tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp1043= e->topt; if( _temp1043 == 0){ _throw( Null_Exception);} _temp1043->v;}),
_temp1039, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1063= e->topt; if( _temp1063 == 0){ _throw( Null_Exception);} _temp1063->v;}));
void* _temp1044= t; struct Cyc_Absyn_PtrInfo _temp1050; struct Cyc_Absyn_Conref*
_temp1052; struct Cyc_Absyn_Tqual _temp1054; struct Cyc_Absyn_Conref* _temp1056;
void* _temp1058; void* _temp1060; _LL1046: if(( unsigned int) _temp1044 > 4u?*((
int*) _temp1044) == Cyc_Absyn_PointerType: 0){ _LL1051: _temp1050=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1044)->f1; _LL1061: _temp1060=( void*)
_temp1050.elt_typ; goto _LL1059; _LL1059: _temp1058=( void*) _temp1050.rgn_typ;
goto _LL1057; _LL1057: _temp1056=( struct Cyc_Absyn_Conref*) _temp1050.nullable;
goto _LL1055; _LL1055: _temp1054=( struct Cyc_Absyn_Tqual) _temp1050.tq; goto
_LL1053; _LL1053: _temp1052=( struct Cyc_Absyn_Conref*) _temp1050.bounds; goto
_LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1047: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1058); Cyc_Tcutil_check_nonzero_bound( loc, _temp1052); return
_temp1060; _LL1049: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1062= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp1062.last_plus_one - _temp1062.curr, _temp1062.curr);})); _LL1045:;}}
static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1139= e->topt; if( _temp1139 == 0){ _throw(
Null_Exception);} _temp1139->v;})); void* _temp1064= t; struct Cyc_Absyn_Structdecl**
_temp1080; struct Cyc_Absyn_Structdecl* _temp1082; struct Cyc_List_List*
_temp1083; struct _tuple1* _temp1085; struct Cyc_List_List* _temp1087; struct
Cyc_List_List* _temp1089; struct Cyc_Absyn_Uniondecl** _temp1091; struct Cyc_Absyn_Uniondecl*
_temp1093; struct Cyc_List_List* _temp1094; struct _tuple1* _temp1096; struct
Cyc_Absyn_Exp* _temp1099; struct Cyc_Absyn_Tqual _temp1101; void* _temp1103;
struct Cyc_Absyn_PtrInfo _temp1108; _LL1066: if(( unsigned int) _temp1064 > 4u?*((
int*) _temp1064) == Cyc_Absyn_StructType: 0){ _LL1086: _temp1085=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1064)->f1; goto _LL1084;
_LL1084: _temp1083=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1064)->f2; goto _LL1081; _LL1081: _temp1080=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1064)->f3; if( _temp1080 == 0){ goto
_LL1068;} else{ _temp1082=* _temp1080; goto _LL1067;}} else{ goto _LL1068;}
_LL1068: if(( unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_AnonStructType:
0){ _LL1088: _temp1087=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1064)->f1; goto _LL1069;} else{ goto _LL1070;} _LL1070: if(( unsigned int)
_temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_AnonUnionType: 0){ _LL1090:
_temp1089=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1064)->f1; goto _LL1071;} else{ goto _LL1072;} _LL1072: if(( unsigned int)
_temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_UnionType: 0){ _LL1097:
_temp1096=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp1064)->f1;
goto _LL1095; _LL1095: _temp1094=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1064)->f2; goto _LL1092; _LL1092: _temp1091=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1064)->f3; if( _temp1091 == 0){ goto
_LL1074;} else{ _temp1093=* _temp1091; goto _LL1073;}} else{ goto _LL1074;}
_LL1074: if(( unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_ArrayType:
0){ _LL1104: _temp1103=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1064)->f1;
goto _LL1102; _LL1102: _temp1101=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1064)->f2; goto _LL1100; _LL1100: _temp1099=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1064)->f3; goto _LL1098;} else{ goto
_LL1076;} _LL1098: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char*
_temp1105=( char*)"size"; struct _tagged_string _temp1106; _temp1106.curr=
_temp1105; _temp1106.base= _temp1105; _temp1106.last_plus_one= _temp1105 + 5;
_temp1106;})) == 0){ goto _LL1075;} else{ goto _LL1076;} _LL1076: if((
unsigned int) _temp1064 > 4u?*(( int*) _temp1064) == Cyc_Absyn_PointerType: 0){
_LL1109: _temp1108=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1064)->f1; goto _LL1107;} else{ goto _LL1078;} _LL1107: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp1110=( char*)"size"; struct
_tagged_string _temp1111; _temp1111.curr= _temp1110; _temp1111.base= _temp1110;
_temp1111.last_plus_one= _temp1110 + 5; _temp1111;})) == 0){ goto _LL1077;}
else{ goto _LL1078;} _LL1078: goto _LL1079; _LL1067: if( _temp1085 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1112=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1113; _temp1113.curr= _temp1112; _temp1113.base=
_temp1112; _temp1113.last_plus_one= _temp1112 + 40; _temp1113;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp1082, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1115= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1114= _temp1085; if( _temp1114 == 0){ _throw(
Null_Exception);} _temp1114;})); struct _tagged_string _temp1116=* f; xprintf("struct %.*s has no %.*s field",
_temp1115.last_plus_one - _temp1115.curr, _temp1115.curr, _temp1116.last_plus_one
- _temp1116.curr, _temp1116.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1117= _new_region(); struct _RegionHandle* rgn=& _temp1117; _push_region(
rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1082->tvs, _temp1083); t2= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_Structfield* _temp1118= finfo; if(
_temp1118 == 0){ _throw( Null_Exception);} _temp1118->type;}));}; _pop_region(&
_temp1117);} return t2;}} _LL1069: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1087, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1119=* f; xprintf("struct has no %.*s field", _temp1119.last_plus_one
- _temp1119.curr, _temp1119.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1120= finfo; if( _temp1120 == 0){ _throw( Null_Exception);} _temp1120->type;});}
_LL1071: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1089, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1121=* f; xprintf("union has no %.*s field", _temp1121.last_plus_one
- _temp1121.curr, _temp1121.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1122= finfo; if( _temp1122 == 0){ _throw( Null_Exception);} _temp1122->type;});}
_LL1073: if( _temp1096 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1123=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp1124; _temp1124.curr= _temp1123; _temp1124.base=
_temp1123; _temp1124.last_plus_one= _temp1123 + 39; _temp1124;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp1093, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1126= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1125= _temp1096; if( _temp1125 == 0){ _throw(
Null_Exception);} _temp1125;})); struct _tagged_string _temp1127=* f; xprintf("union %.*s has no %.*s field",
_temp1126.last_plus_one - _temp1126.curr, _temp1126.curr, _temp1127.last_plus_one
- _temp1127.curr, _temp1127.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1093->tvs, _temp1094); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1128= finfo; if( _temp1128 == 0){ _throw(
Null_Exception);} _temp1128->type;})); return t2;}} _LL1075:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1129=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1129[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1130; _temp1130.tag= Cyc_Absyn_Primop_e;
_temp1130.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1130.f2=({ struct Cyc_List_List*
_temp1131=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1131->hd=( void*) e; _temp1131->tl= 0; _temp1131;}); _temp1130;});
_temp1129;}))); return Cyc_Absyn_uint_t; _LL1077:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1132=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1132[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1133; _temp1133.tag= Cyc_Absyn_Primop_e;
_temp1133.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1133.f2=({ struct Cyc_List_List*
_temp1134=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1134->hd=( void*) e; _temp1134->tl= 0; _temp1134;}); _temp1133;});
_temp1132;}))); return Cyc_Absyn_uint_t; _LL1079: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1135=( char*)"size"; struct _tagged_string
_temp1136; _temp1136.curr= _temp1135; _temp1136.base= _temp1135; _temp1136.last_plus_one=
_temp1135 + 5; _temp1136;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1137= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1137.last_plus_one - _temp1137.curr, _temp1137.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1138= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1138.last_plus_one - _temp1138.curr, _temp1138.curr);}));} _LL1065:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1203= e->topt; if( _temp1203 == 0){ _throw( Null_Exception);} _temp1203->v;}));
void* _temp1140= t; struct Cyc_Absyn_PtrInfo _temp1146; struct Cyc_Absyn_Conref*
_temp1148; struct Cyc_Absyn_Tqual _temp1150; struct Cyc_Absyn_Conref* _temp1152;
void* _temp1154; void* _temp1156; _LL1142: if(( unsigned int) _temp1140 > 4u?*((
int*) _temp1140) == Cyc_Absyn_PointerType: 0){ _LL1147: _temp1146=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1140)->f1; _LL1157: _temp1156=( void*)
_temp1146.elt_typ; goto _LL1155; _LL1155: _temp1154=( void*) _temp1146.rgn_typ;
goto _LL1153; _LL1153: _temp1152=( struct Cyc_Absyn_Conref*) _temp1146.nullable;
goto _LL1151; _LL1151: _temp1150=( struct Cyc_Absyn_Tqual) _temp1146.tq; goto
_LL1149; _LL1149: _temp1148=( struct Cyc_Absyn_Conref*) _temp1146.bounds; goto
_LL1143;} else{ goto _LL1144;} _LL1144: goto _LL1145; _LL1143: Cyc_Tcutil_check_nonzero_bound(
loc, _temp1148);{ void* _temp1158= Cyc_Tcutil_compress( _temp1156); struct Cyc_Absyn_Structdecl**
_temp1170; struct Cyc_Absyn_Structdecl* _temp1172; struct Cyc_List_List*
_temp1173; struct _tuple1* _temp1175; struct Cyc_Absyn_Uniondecl** _temp1177;
struct Cyc_Absyn_Uniondecl* _temp1179; struct Cyc_List_List* _temp1180; struct
_tuple1* _temp1182; struct Cyc_List_List* _temp1184; struct Cyc_List_List*
_temp1186; _LL1160: if(( unsigned int) _temp1158 > 4u?*(( int*) _temp1158) ==
Cyc_Absyn_StructType: 0){ _LL1176: _temp1175=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1158)->f1; goto _LL1174; _LL1174: _temp1173=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1158)->f2; goto _LL1171; _LL1171:
_temp1170=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1158)->f3; if( _temp1170 == 0){ goto _LL1162;} else{ _temp1172=* _temp1170;
goto _LL1161;}} else{ goto _LL1162;} _LL1162: if(( unsigned int) _temp1158 > 4u?*((
int*) _temp1158) == Cyc_Absyn_UnionType: 0){ _LL1183: _temp1182=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp1158)->f1; goto _LL1181; _LL1181:
_temp1180=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1158)->f2; goto _LL1178; _LL1178: _temp1177=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1158)->f3; if( _temp1177 == 0){ goto
_LL1164;} else{ _temp1179=* _temp1177; goto _LL1163;}} else{ goto _LL1164;}
_LL1164: if(( unsigned int) _temp1158 > 4u?*(( int*) _temp1158) == Cyc_Absyn_AnonStructType:
0){ _LL1185: _temp1184=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1158)->f1; goto _LL1165;} else{ goto _LL1166;} _LL1166: if(( unsigned int)
_temp1158 > 4u?*(( int*) _temp1158) == Cyc_Absyn_AnonUnionType: 0){ _LL1187:
_temp1186=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1158)->f1; goto _LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169;
_LL1161: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp1172, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1189= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1188= _temp1175; if( _temp1188 == 0){ _throw( Null_Exception);}
_temp1188;})); struct _tagged_string _temp1190=* f; xprintf("struct %.*s has no %.*s field",
_temp1189.last_plus_one - _temp1189.curr, _temp1189.curr, _temp1190.last_plus_one
- _temp1190.curr, _temp1190.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1191= _new_region(); struct _RegionHandle* rgn=& _temp1191; _push_region(
rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1172->tvs, _temp1173); t3= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_Structfield* _temp1192= finfo; if(
_temp1192 == 0){ _throw( Null_Exception);} _temp1192->type;}));}; _pop_region(&
_temp1191);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1154); return t3;}}
_LL1163: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp1179, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1194= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1193= _temp1182; if( _temp1193 == 0){ _throw( Null_Exception);}
_temp1193;})); struct _tagged_string _temp1195=* f; xprintf("union %.*s has no %.*s field",
_temp1194.last_plus_one - _temp1194.curr, _temp1194.curr, _temp1195.last_plus_one
- _temp1195.curr, _temp1195.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1179->tvs, _temp1180); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1196= finfo; if( _temp1196 == 0){ _throw(
Null_Exception);} _temp1196->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1154); return t3;}} _LL1165: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1184, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1197=* f; xprintf("struct has no %.*s field", _temp1197.last_plus_one
- _temp1197.curr, _temp1197.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1198= finfo; if( _temp1198 == 0){ _throw( Null_Exception);} _temp1198->type;});}
_LL1167: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1186, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1199=* f; xprintf("union has no %.*s field", _temp1199.last_plus_one
- _temp1199.curr, _temp1199.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1200= finfo; if( _temp1200 == 0){ _throw( Null_Exception);} _temp1200->type;});}
_LL1169: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1201=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1201.last_plus_one - _temp1201.curr, _temp1201.curr);})); _LL1159:;}
_LL1145: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1202=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1202.last_plus_one - _temp1202.curr, _temp1202.curr);})); _LL1141:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1204; _push_handler(& _temp1204);{ int
_temp1206= 0; if( setjmp( _temp1204.handler)){ _temp1206= 1;} if( ! _temp1206){{
void* _temp1207=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1207;}; _pop_handler();} else{
void* _temp1205=( void*) _exn_thrown; void* _temp1209= _temp1205; _LL1211: if(
_temp1209 == Cyc_List_Nth){ goto _LL1212;} else{ goto _LL1213;} _LL1213: goto
_LL1214; _LL1212: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1214:(
void) _throw( _temp1209); _LL1210:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1240= e1->topt; if( _temp1240 == 0){ _throw( Null_Exception);} _temp1240->v;}));
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1239= e2->topt;
if( _temp1239 == 0){ _throw( Null_Exception);} _temp1239->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1215= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1215.last_plus_one - _temp1215.curr, _temp1215.curr);}));}{ void* _temp1216=
t1; struct Cyc_Absyn_PtrInfo _temp1224; struct Cyc_Absyn_Conref* _temp1226;
struct Cyc_Absyn_Tqual _temp1228; struct Cyc_Absyn_Conref* _temp1230; void*
_temp1232; void* _temp1234; struct Cyc_List_List* _temp1236; _LL1218: if((
unsigned int) _temp1216 > 4u?*(( int*) _temp1216) == Cyc_Absyn_PointerType: 0){
_LL1225: _temp1224=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1216)->f1; _LL1235: _temp1234=( void*) _temp1224.elt_typ; goto _LL1233;
_LL1233: _temp1232=( void*) _temp1224.rgn_typ; goto _LL1231; _LL1231: _temp1230=(
struct Cyc_Absyn_Conref*) _temp1224.nullable; goto _LL1229; _LL1229: _temp1228=(
struct Cyc_Absyn_Tqual) _temp1224.tq; goto _LL1227; _LL1227: _temp1226=( struct
Cyc_Absyn_Conref*) _temp1224.bounds; goto _LL1219;} else{ goto _LL1220;} _LL1220:
if(( unsigned int) _temp1216 > 4u?*(( int*) _temp1216) == Cyc_Absyn_TupleType: 0){
_LL1237: _temp1236=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1216)->f1; goto _LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223;
_LL1219: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1226);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1226);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1232); return _temp1234; _LL1221: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1236, e2); _LL1223: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1238= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1238.last_plus_one - _temp1238.curr, _temp1238.curr);})); _LL1217:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1242= Cyc_Tcutil_compress(({ void**
_temp1241= topt; if( _temp1241 == 0){ _throw( Null_Exception);}* _temp1241;}));
struct Cyc_List_List* _temp1248; _LL1244: if(( unsigned int) _temp1242 > 4u?*((
int*) _temp1242) == Cyc_Absyn_TupleType: 0){ _LL1249: _temp1248=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1242)->f1; goto _LL1245;} else{ goto
_LL1246;} _LL1246: goto _LL1247; _LL1245: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1248) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1250=( char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base=
_temp1250; _temp1251.last_plus_one= _temp1250 + 48; _temp1251;})); goto _LL1243;}
for( 0; es != 0; es=({ struct Cyc_List_List* _temp1252= es; if( _temp1252 == 0){
_throw( Null_Exception);} _temp1252->tl;}), _temp1248=({ struct Cyc_List_List*
_temp1253= _temp1248; if( _temp1253 == 0){ _throw( Null_Exception);} _temp1253->tl;})){
void* topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1260= _temp1248;
if( _temp1260 == 0){ _throw( Null_Exception);} _temp1260->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1254=
es; if( _temp1254 == 0){ _throw( Null_Exception);} _temp1254->hd;})); fields=({
struct Cyc_List_List* _temp1255=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1255->hd=( void*)({ struct _tuple7* _temp1256=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1256->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp1259= _temp1248; if( _temp1259 == 0){
_throw( Null_Exception);} _temp1259->hd;}))).f1; _temp1256->f2=( void*)({ struct
Cyc_Core_Opt* _temp1258=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1257= es; if( _temp1257 == 0){ _throw( Null_Exception);} _temp1257->hd;}))->topt;
if( _temp1258 == 0){ _throw( Null_Exception);} _temp1258->v;}); _temp1256;});
_temp1255->tl= fields; _temp1255;});} done= 1; goto _LL1243; _LL1247: goto
_LL1243; _LL1243:;} if( ! done){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp1261= es; if( _temp1261 == 0){ _throw( Null_Exception);} _temp1261->tl;})){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1262= es; if( _temp1262 == 0){ _throw( Null_Exception);} _temp1262->hd;}));
fields=({ struct Cyc_List_List* _temp1263=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1263->hd=( void*)({ struct _tuple7*
_temp1264=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1264->f1=
Cyc_Absyn_empty_tqual(); _temp1264->f2=( void*)({ struct Cyc_Core_Opt* _temp1266=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1265= es; if( _temp1265 == 0){
_throw( Null_Exception);} _temp1265->hd;}))->topt; if( _temp1266 == 0){ _throw(
Null_Exception);} _temp1266->v;}); _temp1264;}); _temp1263->tl= fields;
_temp1263;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp1267=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1267[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1268; _temp1268.tag= Cyc_Absyn_TupleType;
_temp1268.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1268;}); _temp1267;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1269=( char*)"tcCompoundLit"; struct
_tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base= _temp1269;
_temp1270.last_plus_one= _temp1269 + 14; _temp1270;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp1293=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1293[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1294; _temp1294.tag= Cyc_Absyn_Const_e; _temp1294.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1295=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1295[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1296; _temp1296.tag= Cyc_Absyn_Int_c; _temp1296.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1296.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1296;}); _temp1295;})); _temp1294;}); _temp1293;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1271=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1271->v=( void*) Cyc_Absyn_uint_t; _temp1271;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1291=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1291[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1292; _temp1292.tag= Cyc_Absyn_ArrayType;
_temp1292.f1=( void*) res; _temp1292.f2= Cyc_Absyn_empty_tqual(); _temp1292.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1292;}); _temp1291;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp1272= es2; if(
_temp1272 == 0){ _throw( Null_Exception);} _temp1272->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1273= es2;
if( _temp1273 == 0){ _throw( Null_Exception);} _temp1273->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1274= es; if( _temp1274 == 0){ _throw( Null_Exception);} _temp1274->hd;}))->loc,({
struct _tagged_string _temp1275= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1275.last_plus_one - _temp1275.curr, _temp1275.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp1276= des; if(
_temp1276 == 0){ _throw( Null_Exception);} _temp1276->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1290= des; if( _temp1290
== 0){ _throw( Null_Exception);} _temp1290->hd;}))).f1; if( ds != 0){ void*
_temp1278=( void*)({ struct Cyc_List_List* _temp1277= ds; if( _temp1277 == 0){
_throw( Null_Exception);} _temp1277->hd;}); struct _tagged_string* _temp1284;
struct Cyc_Absyn_Exp* _temp1286; _LL1280: if(*(( int*) _temp1278) == Cyc_Absyn_FieldName){
_LL1285: _temp1284=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1278)->f1; goto _LL1281;} else{ goto _LL1282;} _LL1282: if(*(( int*)
_temp1278) == Cyc_Absyn_ArrayElement){ _LL1287: _temp1286=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1278)->f1; goto _LL1283;} else{ goto
_LL1279;} _LL1281: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1288=( char*)"only array index designators are supported"; struct
_tagged_string _temp1289; _temp1289.curr= _temp1288; _temp1289.base= _temp1288;
_temp1289.last_plus_one= _temp1288 + 43; _temp1289;})); goto _LL1279; _LL1283:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1286);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1286); if( i != offset){ Cyc_Tcutil_terr( _temp1286->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1279;} _LL1279:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1298= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1297= bound->topt; if( _temp1297 == 0){
_throw( Null_Exception);} _temp1297->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1298.last_plus_one - _temp1298.curr, _temp1298.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1299=( char*)"comprehension index variable is not declared const!";
struct _tagged_string _temp1300; _temp1300.curr= _temp1299; _temp1300.base=
_temp1299; _temp1300.last_plus_one= _temp1299 + 52; _temp1300;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_string)({ char* _temp1301=( char*)"bound is not constant"; struct
_tagged_string _temp1302; _temp1302.curr= _temp1301; _temp1302.base= _temp1301;
_temp1302.last_plus_one= _temp1301 + 22; _temp1302;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ char*
_temp1303=( char*)"body is not constant"; struct _tagged_string _temp1304;
_temp1304.curr= _temp1303; _temp1304.base= _temp1303; _temp1304.last_plus_one=
_temp1303 + 21; _temp1304;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2=
0; struct Cyc_Absyn_Tqual* tqopt= 0; if( topt != 0){ void* _temp1306= Cyc_Tcutil_compress(({
void** _temp1305= topt; if( _temp1305 == 0){ _throw( Null_Exception);}*
_temp1305;})); struct Cyc_Absyn_PtrInfo _temp1314; struct Cyc_Absyn_Exp*
_temp1316; struct Cyc_Absyn_Tqual _temp1318; struct Cyc_Absyn_Tqual* _temp1320;
void* _temp1321; void** _temp1323; _LL1308: if(( unsigned int) _temp1306 > 4u?*((
int*) _temp1306) == Cyc_Absyn_PointerType: 0){ _LL1315: _temp1314=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1306)->f1; goto _LL1309;} else{ goto
_LL1310;} _LL1310: if(( unsigned int) _temp1306 > 4u?*(( int*) _temp1306) == Cyc_Absyn_ArrayType:
0){ _LL1322: _temp1321=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1306)->f1;
_temp1323=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1306)->f1; goto _LL1319;
_LL1319: _temp1318=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1306)->f2; _temp1320=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1306)->f2;
goto _LL1317; _LL1317: _temp1316=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1306)->f3; goto _LL1311;} else{ goto _LL1312;} _LL1312: goto _LL1313;
_LL1309: pinfo= _temp1314; topt2=( void**)(( void**)(( void*)& pinfo.elt_typ));
tqopt=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1307; _LL1311: topt2=( void**)
_temp1323; tqopt=( struct Cyc_Absyn_Tqual*) _temp1320; goto _LL1307; _LL1313:
goto _LL1307; _LL1307:;}{ void* t= Cyc_Tcexp_tcExp( te, topt2, body); return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1324=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1324[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1325; _temp1325.tag= Cyc_Absyn_ArrayType;
_temp1325.f1=( void*) t; _temp1325.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual* _temp1326= tqopt; if( _temp1326 == 0){ _throw(
Null_Exception);}* _temp1326;}); _temp1325.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1325;}); _temp1324;});}}} struct _tuple8{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1327=* sd_opt; if( _temp1327 == 0){ _throw(
Null_Exception);} _temp1327;});} else{{ struct _handler_cons _temp1328;
_push_handler(& _temp1328);{ int _temp1330= 0; if( setjmp( _temp1328.handler)){
_temp1330= 1;} if( ! _temp1330){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);;
_pop_handler();} else{ void* _temp1329=( void*) _exn_thrown; void* _temp1332=
_temp1329; _LL1334: if( _temp1332 == Cyc_Dict_Absent){ goto _LL1335;} else{ goto
_LL1336;} _LL1336: goto _LL1337; _LL1335: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1338= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1338.last_plus_one - _temp1338.curr, _temp1338.curr);})); return topt != 0?({
void** _temp1339= topt; if( _temp1339 == 0){ _throw( Null_Exception);}*
_temp1339;}):( void*) Cyc_Absyn_VoidType; _LL1337:( void) _throw( _temp1332);
_LL1333:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1340= sd->name; if( _temp1340 == 0){
_throw( Null_Exception);} _temp1340->v;});}}{ struct _RegionHandle _temp1341=
_new_region(); struct _RegionHandle* rgn=& _temp1341; _push_region( rgn);{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct* res_typ=({
struct Cyc_Absyn_StructType_struct* _temp1363=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1363[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1364; _temp1364.tag= Cyc_Absyn_StructType;
_temp1364.f1=( struct _tuple1*)* tn; _temp1364.f2= typs; _temp1364.f3=({ struct
Cyc_Absyn_Structdecl** _temp1365=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1365[ 0]= sd; _temp1365;});
_temp1364;}); _temp1363;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1342= topt; if( _temp1342 == 0){ _throw( Null_Exception);}*
_temp1342;}));}* otyps=({ struct Cyc_Core_Opt* _temp1343=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1343->v=( void*) typs; _temp1343;});
if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1344=( char*)"can't build abstract struct"; struct _tagged_string _temp1345;
_temp1345.curr= _temp1344; _temp1345.base= _temp1344; _temp1345.last_plus_one=
_temp1344 + 28; _temp1345;}));{ void* _temp1346=( void*) res_typ; _npop_handler(
0u); return _temp1346;}}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
loc, args,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1362= sd->fields;
if( _temp1362 == 0){ _throw( Null_Exception);} _temp1362->v;})); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp1347= fields; if( _temp1347 == 0){
_throw( Null_Exception);} _temp1347->tl;})){ struct _tuple8 _temp1351; struct
Cyc_Absyn_Exp* _temp1352; struct Cyc_Absyn_Structfield* _temp1354; struct
_tuple8* _temp1349=( struct _tuple8*)({ struct Cyc_List_List* _temp1348= fields;
if( _temp1348 == 0){ _throw( Null_Exception);} _temp1348->hd;}); _temp1351=*
_temp1349; _LL1355: _temp1354= _temp1351.f1; goto _LL1353; _LL1353: _temp1352=
_temp1351.f2; goto _LL1350; _LL1350: { void* inst_fieldtyp= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*) _temp1354->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1352); if( ! Cyc_Tcutil_coerce_arg( te, _temp1352,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1352->loc,({ struct _tagged_string
_temp1357=* _temp1354->name; struct _tagged_string _temp1358= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1359= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1360= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1356= _temp1352->topt; if( _temp1356 == 0){ _throw( Null_Exception);}
_temp1356->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1357.last_plus_one - _temp1357.curr, _temp1357.curr, _temp1358.last_plus_one
- _temp1358.curr, _temp1358.curr, _temp1359.last_plus_one - _temp1359.curr,
_temp1359.curr, _temp1360.last_plus_one - _temp1360.curr, _temp1360.curr);}));}}}{
void* _temp1361=( void*) res_typ; _npop_handler( 0u); return _temp1361;}}};
_pop_region(& _temp1341);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1366= _new_region(); struct _RegionHandle* rgn=&
_temp1366; _push_region( rgn);{ void* _temp1367= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1373; _LL1369: if(( unsigned int) _temp1367 > 4u?*((
int*) _temp1367) == Cyc_Absyn_AnonStructType: 0){ _LL1374: _temp1373=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1367)->f1; goto
_LL1370;} else{ goto _LL1371;} _LL1371: goto _LL1372; _LL1370: { struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1373); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1375= fields; if( _temp1375 == 0){ _throw( Null_Exception);} _temp1375->tl;})){
struct _tuple8 _temp1379; struct Cyc_Absyn_Exp* _temp1380; struct Cyc_Absyn_Structfield*
_temp1382; struct _tuple8* _temp1377=( struct _tuple8*)({ struct Cyc_List_List*
_temp1376= fields; if( _temp1376 == 0){ _throw( Null_Exception);} _temp1376->hd;});
_temp1379=* _temp1377; _LL1383: _temp1382= _temp1379.f1; goto _LL1381; _LL1381:
_temp1380= _temp1379.f2; goto _LL1378; _LL1378: Cyc_Tcexp_tcExpInitializer( te,(
void**)(( void**)(( void*)& _temp1382->type)), _temp1380); if( ! Cyc_Tcutil_coerce_arg(
te, _temp1380,( void*) _temp1382->type)){ Cyc_Tcutil_terr( _temp1380->loc,({
struct _tagged_string _temp1385=* _temp1382->name; struct _tagged_string
_temp1386= Cyc_Absynpp_typ2string(( void*) _temp1382->type); struct
_tagged_string _temp1387= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1384= _temp1380->topt; if( _temp1384 == 0){ _throw( Null_Exception);}
_temp1384->v;})); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1385.last_plus_one - _temp1385.curr, _temp1385.curr, _temp1386.last_plus_one
- _temp1386.curr, _temp1386.curr, _temp1387.last_plus_one - _temp1387.curr,
_temp1387.curr);}));}} goto _LL1368;} _LL1372:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1388=( char*)"tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1389; _temp1389.curr= _temp1388; _temp1389.base=
_temp1388; _temp1389.last_plus_one= _temp1388 + 37; _temp1389;})); goto _LL1368;
_LL1368:;}; _pop_region(& _temp1366);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1390= _new_region(); struct _RegionHandle* rgn=&
_temp1390; _push_region( rgn);{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tud->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tuf->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst);*
all_ref=({ struct Cyc_Core_Opt* _temp1391=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1391->v=( void*) all_typs; _temp1391;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1392=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1392->v=( void*) exist_typs; _temp1392;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1440=( struct
Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1440[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1441; _temp1441.tag=
Cyc_Absyn_TunionFieldType; _temp1441.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1442; _temp1442.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1443=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1443[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1444; _temp1444.tag= Cyc_Absyn_KnownTunionfield; _temp1444.f1= tud;
_temp1444.f2= tuf; _temp1444;}); _temp1443;})); _temp1442.targs= all_typs;
_temp1442;}); _temp1441;}); _temp1440;}); if( topt != 0){ void* _temp1394= Cyc_Tcutil_compress(({
void** _temp1393= topt; if( _temp1393 == 0){ _throw( Null_Exception);}*
_temp1393;})); struct Cyc_Absyn_TunionFieldInfo _temp1402; struct Cyc_Absyn_TunionInfo
_temp1404; void* _temp1406; struct Cyc_List_List* _temp1408; void* _temp1410;
_LL1396: if(( unsigned int) _temp1394 > 4u?*(( int*) _temp1394) == Cyc_Absyn_TunionFieldType:
0){ _LL1403: _temp1402=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1394)->f1; goto _LL1397;} else{ goto _LL1398;} _LL1398: if(( unsigned int)
_temp1394 > 4u?*(( int*) _temp1394) == Cyc_Absyn_TunionType: 0){ _LL1405:
_temp1404=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1394)->f1; _LL1411: _temp1410=( void*) _temp1404.tunion_info; goto _LL1409;
_LL1409: _temp1408=( struct Cyc_List_List*) _temp1404.targs; goto _LL1407;
_LL1407: _temp1406=( void*) _temp1404.rgn; goto _LL1399;} else{ goto _LL1400;}
_LL1400: goto _LL1401; _LL1397: Cyc_Tcutil_unify(({ void** _temp1412= topt; if(
_temp1412 == 0){ _throw( Null_Exception);}* _temp1412;}), res); goto _LL1395;
_LL1399:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0? _temp1408 != 0: 0;
a=({ struct Cyc_List_List* _temp1413= a; if( _temp1413 == 0){ _throw(
Null_Exception);} _temp1413->tl;}), _temp1408=({ struct Cyc_List_List* _temp1414=
_temp1408; if( _temp1414 == 0){ _throw( Null_Exception);} _temp1414->tl;})){ Cyc_Tcutil_unify((
void*)({ struct Cyc_List_List* _temp1415= a; if( _temp1415 == 0){ _throw(
Null_Exception);} _temp1415->hd;}),( void*)({ struct Cyc_List_List* _temp1416=
_temp1408; if( _temp1416 == 0){ _throw( Null_Exception);} _temp1416->hd;}));}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1417=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1417->v=(
void*) res; _temp1417;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1418=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1418[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1419; _temp1419.tag=
Cyc_Absyn_TunionType; _temp1419.f1=({ struct Cyc_Absyn_TunionInfo _temp1420;
_temp1420.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1421=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1421[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1422; _temp1422.tag=
Cyc_Absyn_KnownTunion; _temp1422.f1= tud; _temp1422;}); _temp1421;})); _temp1420.targs=
all_typs; _temp1420.rgn=( void*) _temp1406; _temp1420;}); _temp1419;});
_temp1418;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1423= res;
_npop_handler( 0u); return _temp1423;}} goto _LL1395; _LL1401: goto _LL1395;
_LL1395:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0; es=({
struct Cyc_List_List* _temp1424= es; if( _temp1424 == 0){ _throw( Null_Exception);}
_temp1424->tl;}), ts=({ struct Cyc_List_List* _temp1425= ts; if( _temp1425 == 0){
_throw( Null_Exception);} _temp1425->tl;})){ struct Cyc_Absyn_Exp* e=( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1433= es; if( _temp1433 == 0){
_throw( Null_Exception);} _temp1433->hd;}); void* t= Cyc_Tcutil_rsubstitute( rgn,
inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1432= ts; if( _temp1432
== 0){ _throw( Null_Exception);} _temp1432->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp1429= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1430= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1431= e->topt == 0?( struct _tagged_string)({ char* _temp1426=( char*)"?";
struct _tagged_string _temp1427; _temp1427.curr= _temp1426; _temp1427.base=
_temp1426; _temp1427.last_plus_one= _temp1426 + 2; _temp1427;}): Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1428= e->topt; if( _temp1428 == 0){ _throw(
Null_Exception);} _temp1428->v;})); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1429.last_plus_one - _temp1429.curr, _temp1429.curr, _temp1430.last_plus_one
- _temp1430.curr, _temp1430.curr, _temp1431.last_plus_one - _temp1431.curr,
_temp1431.curr);}));}} if( es != 0){ void* _temp1436= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1434= es; if( _temp1434 == 0){
_throw( Null_Exception);} _temp1434->hd;}))->loc,({ struct _tagged_string
_temp1435= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1435.last_plus_one - _temp1435.curr, _temp1435.curr);})); _npop_handler( 0u);
return _temp1436;} if( ts != 0){ void* _temp1438= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1437= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1437.last_plus_one - _temp1437.curr, _temp1437.curr);})); _npop_handler( 0u);
return _temp1438;}{ void* _temp1439= res; _npop_handler( 0u); return _temp1439;}}}};
_pop_region(& _temp1390);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1456=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1456[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1457; _temp1457.tag=
Cyc_Absyn_RgnHandleType; _temp1457.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1457;}); _temp1456;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1455= ropt;
if( _temp1455 == 0){ _throw( Null_Exception);} _temp1455;})); void* _temp1445=
Cyc_Tcutil_compress( handle_type); void* _temp1451; _LL1447: if(( unsigned int)
_temp1445 > 4u?*(( int*) _temp1445) == Cyc_Absyn_RgnHandleType: 0){ _LL1452:
_temp1451=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1445)->f1;
goto _LL1448;} else{ goto _LL1449;} _LL1449: goto _LL1450; _LL1448: rgn=
_temp1451; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto _LL1446; _LL1450:
Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1453= ropt; if( _temp1453 == 0){
_throw( Null_Exception);} _temp1453->loc;}),({ struct _tagged_string _temp1454=
Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1454.last_plus_one - _temp1454.curr, _temp1454.curr);})); goto _LL1446;
_LL1446:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1458= t; struct Cyc_Absyn_TunionFieldInfo
_temp1464; struct Cyc_List_List* _temp1466; void* _temp1468; struct Cyc_Absyn_Tunionfield*
_temp1470; struct Cyc_Absyn_Tuniondecl* _temp1472; _LL1460: if(( unsigned int)
_temp1458 > 4u?*(( int*) _temp1458) == Cyc_Absyn_TunionFieldType: 0){ _LL1465:
_temp1464=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1458)->f1; _LL1469: _temp1468=( void*) _temp1464.field_info; if(*(( int*)
_temp1468) == Cyc_Absyn_KnownTunionfield){ _LL1473: _temp1472=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1468)->f1; goto _LL1471; _LL1471:
_temp1470=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1468)->f2; goto _LL1467;} else{ goto _LL1462;} _LL1467: _temp1466=( struct
Cyc_List_List*) _temp1464.targs; goto _LL1461;} else{ goto _LL1462;} _LL1462:
goto _LL1463; _LL1461: if( _temp1470->tvs != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp1474=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1475; _temp1475.curr= _temp1474; _temp1475.base=
_temp1474; _temp1475.last_plus_one= _temp1474 + 50; _temp1475;}));} goto _LL1459;
_LL1463: goto _LL1459; _LL1459:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1477=
Cyc_Tcutil_compress(({ void** _temp1476= topt; if( _temp1476 == 0){ _throw(
Null_Exception);}* _temp1476;})); struct Cyc_Absyn_PtrInfo _temp1483; struct Cyc_Absyn_Conref*
_temp1485; struct Cyc_Absyn_Tqual _temp1487; struct Cyc_Absyn_Conref* _temp1489;
void* _temp1491; void* _temp1493; _LL1479: if(( unsigned int) _temp1477 > 4u?*((
int*) _temp1477) == Cyc_Absyn_PointerType: 0){ _LL1484: _temp1483=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1477)->f1; _LL1494: _temp1493=( void*)
_temp1483.elt_typ; goto _LL1492; _LL1492: _temp1491=( void*) _temp1483.rgn_typ;
goto _LL1490; _LL1490: _temp1489=( struct Cyc_Absyn_Conref*) _temp1483.nullable;
goto _LL1488; _LL1488: _temp1487=( struct Cyc_Absyn_Tqual) _temp1483.tq; goto
_LL1486; _LL1486: _temp1485=( struct Cyc_Absyn_Conref*) _temp1483.bounds; goto
_LL1480;} else{ goto _LL1481;} _LL1481: goto _LL1482; _LL1480: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1489)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL1478; _LL1482: goto _LL1478; _LL1478:;} return ptr_maker( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1495=( void*) s->r; struct Cyc_Absyn_Exp* _temp1505;
struct Cyc_Absyn_Stmt* _temp1507; struct Cyc_Absyn_Stmt* _temp1509; struct Cyc_Absyn_Stmt*
_temp1511; struct Cyc_Absyn_Decl* _temp1513; _LL1497: if(( unsigned int)
_temp1495 > 1u?*(( int*) _temp1495) == Cyc_Absyn_Exp_s: 0){ _LL1506: _temp1505=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1495)->f1; goto
_LL1498;} else{ goto _LL1499;} _LL1499: if(( unsigned int) _temp1495 > 1u?*((
int*) _temp1495) == Cyc_Absyn_Seq_s: 0){ _LL1510: _temp1509=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1495)->f1; goto _LL1508; _LL1508: _temp1507=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1495)->f2; goto
_LL1500;} else{ goto _LL1501;} _LL1501: if(( unsigned int) _temp1495 > 1u?*((
int*) _temp1495) == Cyc_Absyn_Decl_s: 0){ _LL1514: _temp1513=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1495)->f1; goto _LL1512; _LL1512:
_temp1511=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1495)->f2;
goto _LL1502;} else{ goto _LL1503;} _LL1503: goto _LL1504; _LL1498: return( void*)({
struct Cyc_Core_Opt* _temp1515= _temp1505->topt; if( _temp1515 == 0){ _throw(
Null_Exception);} _temp1515->v;}); _LL1500: s= _temp1507; continue; _LL1502: s=
_temp1511; continue; _LL1504: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1516=( char*)"statement expression must end with expression";
struct _tagged_string _temp1517; _temp1517.curr= _temp1516; _temp1517.base=
_temp1516; _temp1517.last_plus_one= _temp1516 + 46; _temp1517;})); _LL1496:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1518=( char*)"tcCodegen"; struct
_tagged_string _temp1519; _temp1519.curr= _temp1518; _temp1519.base= _temp1518;
_temp1519.last_plus_one= _temp1518 + 10; _temp1519;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1520=( char*)"tcFill"; struct _tagged_string _temp1521; _temp1521.curr=
_temp1520; _temp1521.base= _temp1520; _temp1521.last_plus_one= _temp1520 + 7;
_temp1521;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1533=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1533[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1534; _temp1534.tag= Cyc_Absyn_RgnHandleType; _temp1534.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1534;}); _temp1533;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1532= rgn_handle; if( _temp1532 == 0){ _throw( Null_Exception);} _temp1532;}));
void* _temp1522= Cyc_Tcutil_compress( handle_type); void* _temp1528; _LL1524:
if(( unsigned int) _temp1522 > 4u?*(( int*) _temp1522) == Cyc_Absyn_RgnHandleType:
0){ _LL1529: _temp1528=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1522)->f1; goto _LL1525;} else{ goto _LL1526;} _LL1526: goto _LL1527;
_LL1525: rgn= _temp1528; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1523; _LL1527: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1530= rgn_handle;
if( _temp1530 == 0){ _throw( Null_Exception);} _temp1530->loc;}),({ struct
_tagged_string _temp1531= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1531.last_plus_one - _temp1531.curr, _temp1531.curr);})); goto _LL1523;
_LL1523:;}{ void* _temp1535=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1547;
struct Cyc_Absyn_Exp* _temp1549; struct Cyc_Absyn_Vardecl* _temp1551; struct Cyc_List_List*
_temp1553; struct Cyc_Core_Opt* _temp1555; struct Cyc_List_List* _temp1557; void*
_temp1559; struct _tagged_string _temp1561; _LL1537: if(*(( int*) _temp1535) ==
Cyc_Absyn_Comprehension_e){ _LL1552: _temp1551=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1535)->f1; goto _LL1550; _LL1550:
_temp1549=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1535)->f2; goto _LL1548; _LL1548: _temp1547=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1535)->f3; goto _LL1538;} else{
goto _LL1539;} _LL1539: if(*(( int*) _temp1535) == Cyc_Absyn_UnresolvedMem_e){
_LL1556: _temp1555=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1535)->f1; goto _LL1554; _LL1554: _temp1553=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1535)->f2; goto _LL1540;} else{
goto _LL1541;} _LL1541: if(*(( int*) _temp1535) == Cyc_Absyn_Array_e){ _LL1558:
_temp1557=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1535)->f1;
goto _LL1542;} else{ goto _LL1543;} _LL1543: if(*(( int*) _temp1535) == Cyc_Absyn_Const_e){
_LL1560: _temp1559=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1535)->f1;
if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_String_c: 0){
_LL1562: _temp1561=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1559)->f1; goto _LL1544;} else{ goto _LL1545;}} else{ goto _LL1545;}
_LL1545: goto _LL1546; _LL1538: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1563= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1569; struct Cyc_Absyn_Tqual _temp1571; void* _temp1573; _LL1565: if((
unsigned int) _temp1563 > 4u?*(( int*) _temp1563) == Cyc_Absyn_ArrayType: 0){
_LL1574: _temp1573=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1563)->f1;
goto _LL1572; _LL1572: _temp1571=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1563)->f2; goto _LL1570; _LL1570: _temp1569=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1563)->f3; goto _LL1566;} else{ goto
_LL1567;} _LL1567: goto _LL1568; _LL1566: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1549)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1583=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1583[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1584; _temp1584.tag= Cyc_Absyn_Upper_b; _temp1584.f1=
_temp1549; _temp1584;}); _temp1583;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1580=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1580[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1581; _temp1581.tag= Cyc_Absyn_PointerType;
_temp1581.f1=({ struct Cyc_Absyn_PtrInfo _temp1582; _temp1582.elt_typ=( void*)
_temp1573; _temp1582.rgn_typ=( void*) rgn; _temp1582.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1582.tq= _temp1571; _temp1582.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1582;});
_temp1581;}); _temp1580;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1575= topt; if( _temp1575 == 0){ _throw( Null_Exception);}* _temp1575;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1576= topt;
if( _temp1576 == 0){ _throw( Null_Exception);}* _temp1576;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1577=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1577->v=( void*) res_typ; _temp1577;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1578= topt; if( _temp1578 == 0){ _throw( Null_Exception);}*
_temp1578;})); res_typ=({ void** _temp1579= topt; if( _temp1579 == 0){ _throw(
Null_Exception);}* _temp1579;});}} return res_typ;} _LL1568: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1585=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1586; _temp1586.curr= _temp1585; _temp1586.base= _temp1585;
_temp1586.last_plus_one= _temp1585 + 45; _temp1586;})); _LL1564:;} _LL1540:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1587=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1587[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1588; _temp1588.tag= Cyc_Absyn_Array_e;
_temp1588.f1= _temp1553; _temp1588;}); _temp1587;}))); _temp1557= _temp1553;
goto _LL1542; _LL1542: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1590=
Cyc_Tcutil_compress(({ void** _temp1589= topt; if( _temp1589 == 0){ _throw(
Null_Exception);}* _temp1589;})); struct Cyc_Absyn_PtrInfo _temp1596; struct Cyc_Absyn_Conref*
_temp1598; struct Cyc_Absyn_Tqual _temp1600; struct Cyc_Absyn_Conref* _temp1602;
void* _temp1604; void* _temp1606; void** _temp1608; _LL1592: if(( unsigned int)
_temp1590 > 4u?*(( int*) _temp1590) == Cyc_Absyn_PointerType: 0){ _LL1597:
_temp1596=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1590)->f1; _LL1607: _temp1606=( void*) _temp1596.elt_typ; _temp1608=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1590)->f1).elt_typ; goto _LL1605;
_LL1605: _temp1604=( void*) _temp1596.rgn_typ; goto _LL1603; _LL1603: _temp1602=(
struct Cyc_Absyn_Conref*) _temp1596.nullable; goto _LL1601; _LL1601: _temp1600=(
struct Cyc_Absyn_Tqual) _temp1596.tq; goto _LL1599; _LL1599: _temp1598=( struct
Cyc_Absyn_Conref*) _temp1596.bounds; goto _LL1593;} else{ goto _LL1594;} _LL1594:
goto _LL1595; _LL1593: elt_typ_opt=( void**) _temp1608; goto _LL1591; _LL1595:
goto _LL1591; _LL1591:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1609= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1615; struct Cyc_Absyn_Tqual _temp1617; void*
_temp1619; _LL1611: if(( unsigned int) _temp1609 > 4u?*(( int*) _temp1609) ==
Cyc_Absyn_ArrayType: 0){ _LL1620: _temp1619=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1609)->f1; goto _LL1618; _LL1618: _temp1617=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1609)->f2; goto _LL1616; _LL1616:
_temp1615=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1609)->f3; goto _LL1612;} else{ goto _LL1613;} _LL1613: goto _LL1614;
_LL1612: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1621=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1621[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1622; _temp1622.tag=
Cyc_Absyn_PointerType; _temp1622.f1=({ struct Cyc_Absyn_PtrInfo _temp1623;
_temp1623.elt_typ=( void*) _temp1619; _temp1623.rgn_typ=( void*) rgn; _temp1623.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1623.tq=
_temp1617; _temp1623.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1624=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1624[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1625; _temp1625.tag= Cyc_Absyn_Upper_b; _temp1625.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1626= _temp1615; if(
_temp1626 == 0){ _throw( Null_Exception);} _temp1626;}); _temp1625;}); _temp1624;}));
_temp1623;}); _temp1622;}); _temp1621;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1627= topt; if( _temp1627 == 0){ _throw( Null_Exception);}*
_temp1627;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1628= topt; if( _temp1628 == 0){ _throw( Null_Exception);}* _temp1628;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1629=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1629->v=( void*) res_typ; _temp1629;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1630= topt; if( _temp1630 == 0){ _throw( Null_Exception);}*
_temp1630;})); res_typ=({ void** _temp1631= topt; if( _temp1631 == 0){ _throw(
Null_Exception);}* _temp1631;});}} goto _LL1610; _LL1614: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1632=( char*)"tcExpNoPromote on Array_e returned non-array type"; struct
_tagged_string _temp1633; _temp1633.curr= _temp1632; _temp1633.base= _temp1632;
_temp1633.last_plus_one= _temp1632 + 50; _temp1633;})); _LL1610:;} return
res_typ;}} _LL1544: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn,
Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1634=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1634[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1635; _temp1635.tag= Cyc_Absyn_Upper_b; _temp1635.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1635;}); _temp1634;}));} _LL1546: { void** topt2=
0; if( topt != 0){ void* _temp1637= Cyc_Tcutil_compress(({ void** _temp1636=
topt; if( _temp1636 == 0){ _throw( Null_Exception);}* _temp1636;})); struct Cyc_Absyn_PtrInfo
_temp1645; struct Cyc_Absyn_Conref* _temp1647; struct Cyc_Absyn_Tqual _temp1649;
struct Cyc_Absyn_Conref* _temp1651; void* _temp1653; void* _temp1655; void**
_temp1657; struct Cyc_Absyn_TunionInfo _temp1658; _LL1639: if(( unsigned int)
_temp1637 > 4u?*(( int*) _temp1637) == Cyc_Absyn_PointerType: 0){ _LL1646:
_temp1645=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1637)->f1; _LL1656: _temp1655=( void*) _temp1645.elt_typ; _temp1657=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1637)->f1).elt_typ; goto _LL1654;
_LL1654: _temp1653=( void*) _temp1645.rgn_typ; goto _LL1652; _LL1652: _temp1651=(
struct Cyc_Absyn_Conref*) _temp1645.nullable; goto _LL1650; _LL1650: _temp1649=(
struct Cyc_Absyn_Tqual) _temp1645.tq; goto _LL1648; _LL1648: _temp1647=( struct
Cyc_Absyn_Conref*) _temp1645.bounds; goto _LL1640;} else{ goto _LL1641;} _LL1641:
if(( unsigned int) _temp1637 > 4u?*(( int*) _temp1637) == Cyc_Absyn_TunionType:
0){ _LL1659: _temp1658=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1637)->f1; goto _LL1642;} else{ goto _LL1643;} _LL1643: goto _LL1644;
_LL1640: topt2=( void**) _temp1657; goto _LL1638; _LL1642: topt2=({ void**
_temp1660=( void**) GC_malloc( sizeof( void*)); _temp1660[ 0]=({ void**
_temp1661= topt; if( _temp1661 == 0){ _throw( Null_Exception);}* _temp1661;});
_temp1660;}); goto _LL1638; _LL1644: goto _LL1638; _LL1638:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1667=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1667[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1668; _temp1668.tag=
Cyc_Absyn_PointerType; _temp1668.f1=({ struct Cyc_Absyn_PtrInfo _temp1669;
_temp1669.elt_typ=( void*) telt; _temp1669.rgn_typ=( void*) rgn; _temp1669.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1669.tq= Cyc_Absyn_empty_tqual();
_temp1669.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1670=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1670[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1671; _temp1671.tag= Cyc_Absyn_Upper_b; _temp1671.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1671;}); _temp1670;})); _temp1669;}); _temp1668;});
_temp1667;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1662= topt;
if( _temp1662 == 0){ _throw( Null_Exception);}* _temp1662;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1663= topt; if( _temp1663 == 0){ _throw(
Null_Exception);}* _temp1663;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1664=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1664->v=(
void*) res_typ; _temp1664;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1665= topt; if( _temp1665 == 0){ _throw( Null_Exception);}* _temp1665;}));
res_typ=({ void** _temp1666= topt; if( _temp1666 == 0){ _throw( Null_Exception);}*
_temp1666;});}} return res_typ;}} _LL1536:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1672= t; struct Cyc_Absyn_Exp*
_temp1678; struct Cyc_Absyn_Tqual _temp1680; void* _temp1682; _LL1674: if((
unsigned int) _temp1672 > 4u?*(( int*) _temp1672) == Cyc_Absyn_ArrayType: 0){
_LL1683: _temp1682=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1672)->f1;
goto _LL1681; _LL1681: _temp1680=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1672)->f2; goto _LL1679; _LL1679: _temp1678=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1672)->f3; goto _LL1675;} else{ goto
_LL1676;} _LL1676: goto _LL1677; _LL1675: { void* _temp1686; int _temp1688;
struct _tuple5 _temp1684= Cyc_Tcutil_addressof_props( te, e); _LL1689: _temp1688=
_temp1684.f1; goto _LL1687; _LL1687: _temp1686= _temp1684.f2; goto _LL1685;
_LL1685: { void* b= _temp1678 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1690=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1690[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1691; _temp1691.tag= Cyc_Absyn_Upper_b; _temp1691.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1692= _temp1678; if(
_temp1692 == 0){ _throw( Null_Exception);} _temp1692;}); _temp1691;}); _temp1690;});
t= Cyc_Absyn_atb_typ( _temp1682, _temp1686, _temp1680, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1677: return t; _LL1673:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1693=( void*) e->r; struct Cyc_List_List* _temp1703;
struct Cyc_Absyn_Exp* _temp1705; struct Cyc_Absyn_Exp* _temp1707; struct Cyc_Absyn_Vardecl*
_temp1709; void* _temp1711; struct _tagged_string _temp1713; _LL1695: if(*(( int*)
_temp1693) == Cyc_Absyn_Array_e){ _LL1704: _temp1703=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1693)->f1; goto _LL1696;} else{ goto
_LL1697;} _LL1697: if(*(( int*) _temp1693) == Cyc_Absyn_Comprehension_e){
_LL1710: _temp1709=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1693)->f1; goto _LL1708; _LL1708: _temp1707=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1693)->f2; goto _LL1706; _LL1706:
_temp1705=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1693)->f3; goto _LL1698;} else{ goto _LL1699;} _LL1699: if(*(( int*)
_temp1693) == Cyc_Absyn_Const_e){ _LL1712: _temp1711=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1693)->f1; if(( unsigned int) _temp1711 > 1u?*(( int*) _temp1711) == Cyc_Absyn_String_c:
0){ _LL1714: _temp1713=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1711)->f1; goto _LL1700;} else{ goto _LL1701;}} else{ goto _LL1701;}
_LL1701: goto _LL1702; _LL1696: return t; _LL1698: return t; _LL1700: return t;
_LL1702: t= Cyc_Tcutil_compress( t);{ void* _temp1715= t; struct Cyc_Absyn_Exp*
_temp1721; struct Cyc_Absyn_Tqual _temp1723; void* _temp1725; _LL1717: if((
unsigned int) _temp1715 > 4u?*(( int*) _temp1715) == Cyc_Absyn_ArrayType: 0){
_LL1726: _temp1725=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1715)->f1;
goto _LL1724; _LL1724: _temp1723=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1715)->f2; goto _LL1722; _LL1722: _temp1721=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1715)->f3; goto _LL1718;} else{ goto
_LL1719;} _LL1719: goto _LL1720; _LL1718: { void* _temp1729; int _temp1731;
struct _tuple5 _temp1727= Cyc_Tcutil_addressof_props( te, e); _LL1732: _temp1731=
_temp1727.f1; goto _LL1730; _LL1730: _temp1729= _temp1727.f2; goto _LL1728;
_LL1728: { void* b= _temp1721 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1733=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1733[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1734; _temp1734.tag= Cyc_Absyn_Upper_b; _temp1734.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1735= _temp1721; if(
_temp1735 == 0){ _throw( Null_Exception);} _temp1735;}); _temp1734;}); _temp1733;});
t= Cyc_Absyn_atb_typ( _temp1725, _temp1729, _temp1723, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1720: return t; _LL1716:;} _LL1694:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1736=( void*) e->r; struct Cyc_Absyn_Exp* _temp1742; _LL1738: if(*(( int*)
_temp1736) == Cyc_Absyn_NoInstantiate_e){ _LL1743: _temp1742=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1736)->f1; goto _LL1739;} else{
goto _LL1740;} _LL1740: goto _LL1741; _LL1739: Cyc_Tcexp_tcExpNoInst( te, topt,
_temp1742);( void*)({ struct Cyc_Core_Opt* _temp1744= _temp1742->topt; if(
_temp1744 == 0){ _throw( Null_Exception);} _temp1744->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1745= _temp1742->topt;
if( _temp1745 == 0){ _throw( Null_Exception);} _temp1745->v;})));}); e->topt=
_temp1742->topt; goto _LL1737; _LL1741: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)({ struct Cyc_Core_Opt* _temp1746= e->topt; if( _temp1746 == 0){ _throw(
Null_Exception);} _temp1746->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1747= e->topt; if( _temp1747 == 0){ _throw(
Null_Exception);} _temp1747->v;})));});{ void* _temp1749= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1748= e->topt; if( _temp1748 == 0){ _throw(
Null_Exception);} _temp1748->v;})); struct Cyc_Absyn_PtrInfo _temp1755; struct
Cyc_Absyn_Conref* _temp1757; struct Cyc_Absyn_Tqual _temp1759; struct Cyc_Absyn_Conref*
_temp1761; void* _temp1763; void* _temp1765; _LL1751: if(( unsigned int)
_temp1749 > 4u?*(( int*) _temp1749) == Cyc_Absyn_PointerType: 0){ _LL1756:
_temp1755=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1749)->f1; _LL1766: _temp1765=( void*) _temp1755.elt_typ; goto _LL1764;
_LL1764: _temp1763=( void*) _temp1755.rgn_typ; goto _LL1762; _LL1762: _temp1761=(
struct Cyc_Absyn_Conref*) _temp1755.nullable; goto _LL1760; _LL1760: _temp1759=(
struct Cyc_Absyn_Tqual) _temp1755.tq; goto _LL1758; _LL1758: _temp1757=( struct
Cyc_Absyn_Conref*) _temp1755.bounds; goto _LL1752;} else{ goto _LL1753;} _LL1753:
goto _LL1754; _LL1752:{ void* _temp1767= Cyc_Tcutil_compress( _temp1765); struct
Cyc_Absyn_FnInfo _temp1773; struct Cyc_List_List* _temp1775; int _temp1777;
struct Cyc_List_List* _temp1779; void* _temp1781; struct Cyc_Core_Opt* _temp1783;
struct Cyc_List_List* _temp1785; _LL1769: if(( unsigned int) _temp1767 > 4u?*((
int*) _temp1767) == Cyc_Absyn_FnType: 0){ _LL1774: _temp1773=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1767)->f1; _LL1786: _temp1785=( struct Cyc_List_List*)
_temp1773.tvars; goto _LL1784; _LL1784: _temp1783=( struct Cyc_Core_Opt*)
_temp1773.effect; goto _LL1782; _LL1782: _temp1781=( void*) _temp1773.ret_typ;
goto _LL1780; _LL1780: _temp1779=( struct Cyc_List_List*) _temp1773.args; goto
_LL1778; _LL1778: _temp1777=( int) _temp1773.varargs; goto _LL1776; _LL1776:
_temp1775=( struct Cyc_List_List*) _temp1773.attributes; goto _LL1770;} else{
goto _LL1771;} _LL1771: goto _LL1772; _LL1770: if( _temp1785 != 0){ struct
_RegionHandle _temp1787= _new_region(); struct _RegionHandle* rgn=& _temp1787;
_push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1785); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1794=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1794[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1795; _temp1795.tag= Cyc_Absyn_FnType; _temp1795.f1=({ struct Cyc_Absyn_FnInfo
_temp1796; _temp1796.tvars= 0; _temp1796.effect= _temp1783; _temp1796.ret_typ=(
void*) _temp1781; _temp1796.args= _temp1779; _temp1796.varargs= _temp1777;
_temp1796.attributes= _temp1775; _temp1796;}); _temp1795;}); _temp1794;})); void*
new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1791=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1791[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1792; _temp1792.tag= Cyc_Absyn_PointerType;
_temp1792.f1=({ struct Cyc_Absyn_PtrInfo _temp1793; _temp1793.elt_typ=( void*)
ftyp; _temp1793.rgn_typ=( void*) _temp1763; _temp1793.nullable= _temp1761;
_temp1793.tq= _temp1759; _temp1793.bounds= _temp1757; _temp1793;}); _temp1792;});
_temp1791;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1788=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1788[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1789; _temp1789.tag= Cyc_Absyn_Instantiate_e;
_temp1789.f1= inner; _temp1789.f2= ts; _temp1789;}); _temp1788;}))); e->topt=({
struct Cyc_Core_Opt* _temp1790=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1790->v=( void*) new_typ; _temp1790;});}; _pop_region(&
_temp1787);} goto _LL1768; _LL1772: goto _LL1768; _LL1768:;} goto _LL1750;
_LL1754: goto _LL1750; _LL1750:;} goto _LL1737; _LL1737:;} return( void*)({
struct Cyc_Core_Opt* _temp1797= e->topt; if( _temp1797 == 0){ _throw(
Null_Exception);} _temp1797->v;});} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1798=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1870; struct _tuple1* _temp1872; struct Cyc_List_List* _temp1874; struct
Cyc_Absyn_Exp* _temp1876; struct Cyc_List_List* _temp1878; struct Cyc_Core_Opt*
_temp1880; void* _temp1882; void* _temp1884; struct _tuple1* _temp1886; struct
Cyc_List_List* _temp1888; void* _temp1890; void* _temp1892; struct Cyc_Absyn_Exp*
_temp1894; struct Cyc_Absyn_Exp* _temp1896; struct Cyc_Core_Opt* _temp1898;
struct Cyc_Absyn_Exp* _temp1900; struct Cyc_Absyn_Exp* _temp1902; struct Cyc_Absyn_Exp*
_temp1904; struct Cyc_Absyn_Exp* _temp1906; struct Cyc_Absyn_Exp* _temp1908;
struct Cyc_Absyn_Exp* _temp1910; struct Cyc_List_List* _temp1912; struct Cyc_Absyn_Exp*
_temp1914; struct Cyc_Absyn_Exp* _temp1916; struct Cyc_List_List* _temp1918;
struct Cyc_Absyn_Exp* _temp1920; struct Cyc_Absyn_Exp* _temp1922; void*
_temp1924; struct Cyc_Absyn_Exp* _temp1926; struct Cyc_Absyn_Exp* _temp1928;
struct Cyc_Absyn_Exp* _temp1930; struct Cyc_Absyn_Exp* _temp1932; void*
_temp1934; struct Cyc_Absyn_Exp* _temp1936; struct _tagged_string* _temp1938;
struct Cyc_Absyn_Exp* _temp1940; struct _tagged_string* _temp1942; struct Cyc_Absyn_Exp*
_temp1944; struct Cyc_Absyn_Exp* _temp1946; struct Cyc_Absyn_Exp* _temp1948;
struct Cyc_List_List* _temp1950; struct Cyc_List_List* _temp1952; struct _tuple2*
_temp1954; struct Cyc_List_List* _temp1956; struct Cyc_Absyn_Stmt* _temp1958;
struct Cyc_Absyn_Fndecl* _temp1960; struct Cyc_Absyn_Exp* _temp1962; struct Cyc_Absyn_Exp*
_temp1964; struct Cyc_Absyn_Exp* _temp1966; struct Cyc_Absyn_Vardecl* _temp1968;
struct Cyc_Absyn_Structdecl* _temp1970; struct Cyc_Absyn_Structdecl** _temp1972;
struct Cyc_List_List* _temp1973; struct Cyc_Core_Opt* _temp1975; struct Cyc_Core_Opt**
_temp1977; struct _tuple1* _temp1978; struct _tuple1** _temp1980; struct Cyc_List_List*
_temp1981; void* _temp1983; struct Cyc_Absyn_Tunionfield* _temp1985; struct Cyc_Absyn_Tuniondecl*
_temp1987; struct Cyc_List_List* _temp1989; struct Cyc_Core_Opt* _temp1991;
struct Cyc_Core_Opt** _temp1993; struct Cyc_Core_Opt* _temp1994; struct Cyc_Core_Opt**
_temp1996; struct Cyc_Absyn_Enumfield* _temp1997; struct Cyc_Absyn_Enumdecl*
_temp1999; struct _tuple1* _temp2001; void* _temp2003; struct Cyc_Absyn_Exp*
_temp2005; _LL1800: if(*(( int*) _temp1798) == Cyc_Absyn_NoInstantiate_e){
_LL1871: _temp1870=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1798)->f1; goto _LL1801;} else{ goto _LL1802;} _LL1802: if(*(( int*)
_temp1798) == Cyc_Absyn_UnknownId_e){ _LL1873: _temp1872=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1798)->f1; goto _LL1803;} else{ goto
_LL1804;} _LL1804: if(*(( int*) _temp1798) == Cyc_Absyn_UnknownCall_e){ _LL1877:
_temp1876=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1798)->f1; goto _LL1875; _LL1875: _temp1874=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1798)->f2; goto _LL1805;} else{
goto _LL1806;} _LL1806: if(*(( int*) _temp1798) == Cyc_Absyn_UnresolvedMem_e){
_LL1881: _temp1880=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1798)->f1; goto _LL1879; _LL1879: _temp1878=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1798)->f2; goto _LL1807;} else{
goto _LL1808;} _LL1808: if(*(( int*) _temp1798) == Cyc_Absyn_Const_e){ _LL1883:
_temp1882=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1798)->f1; goto
_LL1809;} else{ goto _LL1810;} _LL1810: if(*(( int*) _temp1798) == Cyc_Absyn_Var_e){
_LL1887: _temp1886=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1798)->f1; goto _LL1885; _LL1885: _temp1884=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1798)->f2; goto _LL1811;} else{ goto _LL1812;} _LL1812: if(*(( int*)
_temp1798) == Cyc_Absyn_Primop_e){ _LL1891: _temp1890=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1798)->f1; goto _LL1889; _LL1889: _temp1888=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp1798)->f2; goto _LL1813;} else{ goto
_LL1814;} _LL1814: if(*(( int*) _temp1798) == Cyc_Absyn_Increment_e){ _LL1895:
_temp1894=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1798)->f1; goto _LL1893; _LL1893: _temp1892=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1798)->f2; goto _LL1815;} else{ goto _LL1816;} _LL1816: if(*(( int*)
_temp1798) == Cyc_Absyn_AssignOp_e){ _LL1901: _temp1900=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1798)->f1; goto _LL1899; _LL1899:
_temp1898=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1798)->f2; goto _LL1897; _LL1897: _temp1896=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1798)->f3; goto _LL1817;} else{ goto
_LL1818;} _LL1818: if(*(( int*) _temp1798) == Cyc_Absyn_Conditional_e){ _LL1907:
_temp1906=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1798)->f1; goto _LL1905; _LL1905: _temp1904=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1798)->f2; goto _LL1903; _LL1903:
_temp1902=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1798)->f3; goto _LL1819;} else{ goto _LL1820;} _LL1820: if(*(( int*)
_temp1798) == Cyc_Absyn_SeqExp_e){ _LL1911: _temp1910=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1798)->f1; goto _LL1909; _LL1909:
_temp1908=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1798)->f2; goto _LL1821;} else{ goto _LL1822;} _LL1822: if(*(( int*)
_temp1798) == Cyc_Absyn_FnCall_e){ _LL1915: _temp1914=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1798)->f1; goto _LL1913; _LL1913:
_temp1912=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1798)->f2; goto _LL1823;} else{ goto _LL1824;} _LL1824: if(*(( int*)
_temp1798) == Cyc_Absyn_Throw_e){ _LL1917: _temp1916=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Throw_e_struct*) _temp1798)->f1; goto _LL1825;} else{ goto
_LL1826;} _LL1826: if(*(( int*) _temp1798) == Cyc_Absyn_Instantiate_e){ _LL1921:
_temp1920=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1798)->f1; goto _LL1919; _LL1919: _temp1918=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1798)->f2; goto _LL1827;} else{
goto _LL1828;} _LL1828: if(*(( int*) _temp1798) == Cyc_Absyn_Cast_e){ _LL1925:
_temp1924=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1798)->f1; goto
_LL1923; _LL1923: _temp1922=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1798)->f2; goto _LL1829;} else{ goto _LL1830;} _LL1830: if(*(( int*)
_temp1798) == Cyc_Absyn_Address_e){ _LL1927: _temp1926=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1798)->f1; goto _LL1831;} else{ goto
_LL1832;} _LL1832: if(*(( int*) _temp1798) == Cyc_Absyn_New_e){ _LL1931:
_temp1930=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1798)->f1;
goto _LL1929; _LL1929: _temp1928=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1798)->f2; goto _LL1833;} else{ goto _LL1834;} _LL1834: if(*(( int*)
_temp1798) == Cyc_Absyn_Sizeofexp_e){ _LL1933: _temp1932=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1798)->f1; goto _LL1835;} else{ goto
_LL1836;} _LL1836: if(*(( int*) _temp1798) == Cyc_Absyn_Sizeoftyp_e){ _LL1935:
_temp1934=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1798)->f1; goto
_LL1837;} else{ goto _LL1838;} _LL1838: if(*(( int*) _temp1798) == Cyc_Absyn_Deref_e){
_LL1937: _temp1936=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1798)->f1; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1798) == Cyc_Absyn_StructMember_e){ _LL1941: _temp1940=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1798)->f1; goto _LL1939; _LL1939:
_temp1938=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1798)->f2; goto _LL1841;} else{ goto _LL1842;} _LL1842: if(*(( int*)
_temp1798) == Cyc_Absyn_StructArrow_e){ _LL1945: _temp1944=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1798)->f1; goto _LL1943; _LL1943:
_temp1942=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1798)->f2; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(*(( int*)
_temp1798) == Cyc_Absyn_Subscript_e){ _LL1949: _temp1948=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1798)->f1; goto _LL1947; _LL1947:
_temp1946=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1798)->f2; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(*(( int*)
_temp1798) == Cyc_Absyn_Tuple_e){ _LL1951: _temp1950=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp1798)->f1; goto _LL1847;} else{ goto
_LL1848;} _LL1848: if(*(( int*) _temp1798) == Cyc_Absyn_CompoundLit_e){ _LL1955:
_temp1954=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1798)->f1;
goto _LL1953; _LL1953: _temp1952=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1798)->f2; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(*(( int*)
_temp1798) == Cyc_Absyn_Array_e){ _LL1957: _temp1956=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1798)->f1; goto _LL1851;} else{ goto
_LL1852;} _LL1852: if(*(( int*) _temp1798) == Cyc_Absyn_StmtExp_e){ _LL1959:
_temp1958=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1798)->f1; goto _LL1853;} else{ goto _LL1854;} _LL1854: if(*(( int*)
_temp1798) == Cyc_Absyn_Codegen_e){ _LL1961: _temp1960=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1798)->f1; goto _LL1855;} else{ goto
_LL1856;} _LL1856: if(*(( int*) _temp1798) == Cyc_Absyn_Fill_e){ _LL1963:
_temp1962=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1798)->f1;
goto _LL1857;} else{ goto _LL1858;} _LL1858: if(*(( int*) _temp1798) == Cyc_Absyn_Comprehension_e){
_LL1969: _temp1968=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1798)->f1; goto _LL1967; _LL1967: _temp1966=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1798)->f2; goto _LL1965; _LL1965:
_temp1964=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1798)->f3; goto _LL1859;} else{ goto _LL1860;} _LL1860: if(*(( int*)
_temp1798) == Cyc_Absyn_Struct_e){ _LL1979: _temp1978=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f1; _temp1980=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f1; goto _LL1976; _LL1976: _temp1975=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f2; _temp1977=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f2; goto _LL1974; _LL1974: _temp1973=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f3; goto _LL1971; _LL1971:
_temp1970=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1798)->f4; _temp1972=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1798)->f4;
goto _LL1861;} else{ goto _LL1862;} _LL1862: if(*(( int*) _temp1798) == Cyc_Absyn_AnonStruct_e){
_LL1984: _temp1983=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1798)->f1;
goto _LL1982; _LL1982: _temp1981=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1798)->f2; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(*(( int*)
_temp1798) == Cyc_Absyn_Tunion_e){ _LL1995: _temp1994=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f1; _temp1996=&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1798)->f1; goto _LL1992; _LL1992: _temp1991=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f2; _temp1993=&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1798)->f2; goto _LL1990; _LL1990: _temp1989=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f3; goto _LL1988; _LL1988:
_temp1987=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1798)->f4; goto _LL1986; _LL1986: _temp1985=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1798)->f5; goto _LL1865;} else{ goto
_LL1866;} _LL1866: if(*(( int*) _temp1798) == Cyc_Absyn_Enum_e){ _LL2002:
_temp2001=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1798)->f1;
goto _LL2000; _LL2000: _temp1999=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1798)->f2; goto _LL1998; _LL1998: _temp1997=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1798)->f3; goto _LL1867;} else{ goto
_LL1868;} _LL1868: if(*(( int*) _temp1798) == Cyc_Absyn_Malloc_e){ _LL2006:
_temp2005=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1798)->f1; goto _LL2004; _LL2004: _temp2003=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1798)->f2; goto _LL1869;} else{ goto _LL1799;} _LL1801: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1870); return; _LL1803: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1872); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1805: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1876, _temp1874); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1807: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1878); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1809: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1882, e);
goto _LL1799; _LL1811: t= Cyc_Tcexp_tcVar( te, loc, _temp1886, _temp1884); goto
_LL1799; _LL1813: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1890, _temp1888);
goto _LL1799; _LL1815: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1894,
_temp1892); goto _LL1799; _LL1817: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1900, _temp1898, _temp1896); goto _LL1799; _LL1819: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1906, _temp1904, _temp1902); goto _LL1799; _LL1821: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1910, _temp1908); goto _LL1799; _LL1823: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1914, _temp1912); goto _LL1799; _LL1825: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1916); goto _LL1799; _LL1827: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1920, _temp1918); goto _LL1799; _LL1829: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1924, _temp1922); goto _LL1799; _LL1831: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1926); goto _LL1799; _LL1833: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1930, e, _temp1928); goto _LL1799; _LL1835: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1932); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1799;}
_LL1837: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1934); goto _LL1799; _LL1839:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1936); goto _LL1799; _LL1841: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1940, _temp1938); goto _LL1799; _LL1843: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1944, _temp1942); goto _LL1799; _LL1845: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1948, _temp1946); goto _LL1799; _LL1847: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1950); goto _LL1799; _LL1849: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1954, _temp1952); goto _LL1799; _LL1851: { void** elt_topt= 0;
if( topt != 0){ void* _temp2008= Cyc_Tcutil_compress(({ void** _temp2007= topt;
if( _temp2007 == 0){ _throw( Null_Exception);}* _temp2007;})); struct Cyc_Absyn_Exp*
_temp2014; struct Cyc_Absyn_Tqual _temp2016; void* _temp2018; void** _temp2020;
_LL2010: if(( unsigned int) _temp2008 > 4u?*(( int*) _temp2008) == Cyc_Absyn_ArrayType:
0){ _LL2019: _temp2018=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2008)->f1;
_temp2020=&(( struct Cyc_Absyn_ArrayType_struct*) _temp2008)->f1; goto _LL2017;
_LL2017: _temp2016=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2008)->f2; goto _LL2015; _LL2015: _temp2014=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2008)->f3; goto _LL2011;} else{ goto
_LL2012;} _LL2012: goto _LL2013; _LL2011: elt_topt=( void**) _temp2020; goto
_LL2009; _LL2013: goto _LL2009; _LL2009:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1956); goto _LL1799;} _LL1853: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1958); goto _LL1799; _LL1855: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1960); goto _LL1799; _LL1857: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1962);
goto _LL1799; _LL1859: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1968,
_temp1966, _temp1964); goto _LL1799; _LL1861: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1980, _temp1977, _temp1973, _temp1972); goto _LL1799; _LL1863: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1983, _temp1981); goto _LL1799; _LL1865: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1996, _temp1993, _temp1989, _temp1987, _temp1985); goto
_LL1799; _LL1867: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp2021=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2021[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2022; _temp2022.tag= Cyc_Absyn_EnumType;
_temp2022.f1=({ struct Cyc_Absyn_Enumdecl* _temp2023= _temp1999; if( _temp2023
== 0){ _throw( Null_Exception);} _temp2023->name;}); _temp2022.f2= _temp1999;
_temp2022;}); _temp2021;}); goto _LL1799; _LL1869: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp2005, _temp2003); goto _LL1799; _LL1799:;} e->topt=({ struct Cyc_Core_Opt*
_temp2024=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2024->v=( void*) t; _temp2024;});}