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
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_identity(
void*); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{
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
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void*
r, struct Cyc_Position_Segment* loc); struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern int
Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern void* Cyc_Dict_fold(
void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict* d, void* accum); extern
void Cyc_Dict_iter_c( void(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); extern void Cyc_Dict_iter2_c( void(* f)( void*, void*, void*), void* env,
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(); struct Cyc_Interface_I; typedef struct Cyc_Interface_I* Cyc_Interface_i_t;
extern struct Cyc_Interface_I* Cyc_Interface_empty(); extern struct Cyc_Interface_I*
Cyc_Interface_extract( struct Cyc_Dict_Dict* ae); struct _tuple3{ struct
_tagged_string f1; struct _tagged_string f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info);
extern struct Cyc_Interface_I* Cyc_Interface_merge( struct Cyc_Interface_I* i1,
struct Cyc_Interface_I* i2, struct _tuple3* info); extern void Cyc_Interface_print(
struct Cyc_Interface_I*, struct Cyc_Stdio___sFILE*); extern struct Cyc_Interface_I*
Cyc_Interface_parse( struct Cyc_Stdio___sFILE*); typedef struct Cyc_Interface_I*
Cyc_interface_t; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
var_default_init, struct Cyc_List_List* ds); extern char Cyc_Lexing_Error[ 10u];
struct Cyc_Lexing_Error_struct{ char* tag; struct _tagged_string f1; } ; struct
Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void*
refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_List_List* Cyc_Parse_parse_file( struct
Cyc_Stdio___sFILE* f); typedef void* Cyc_struct_or_union_t; typedef void* Cyc_blockitem_t;
typedef void* Cyc_type_specifier_t; typedef void* Cyc_storage_class_t; struct
Cyc_Declaration_spec; typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t;
struct Cyc_Declarator; typedef struct Cyc_Declarator* Cyc_declarator_t; struct
Cyc_Abstractdeclarator; typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t;
extern char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern char Cyc_AttributeList_tok[
22u]; struct Cyc_AttributeList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_Attribute_tok[ 18u]; struct Cyc_Attribute_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_BlockItem_tok[ 18u]; struct Cyc_BlockItem_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{
char* tag; int f1; } ; extern char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
char* tag; char f1; } ; extern char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_DeclSpec_tok[ 17u];
struct Cyc_DeclSpec_tok_struct{ char* tag; struct Cyc_Declaration_spec* f1; } ;
extern char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple4{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ char* tag; struct _tuple4* f1; } ;
extern char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{ char*
tag; struct Cyc_Declarator* f1; } ; extern char Cyc_DesignatorList_tok[ 23u];
struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_EnumfieldList_tok[ 22u]; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Enumfield_tok[ 18u];
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
extern char Cyc_ExpList_tok[ 16u]; struct Cyc_ExpList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_FieldPattern_tok[ 21u]; struct _tuple5{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{ char* tag;
struct _tuple5* f1; } ; extern char Cyc_FnDecl_tok[ 15u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_IdList_tok[ 15u];
struct Cyc_IdList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_InitDeclList_tok[ 21u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_InitDecl_tok[ 17u]; struct _tuple6{
struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_InitializerList_tok[ 24u];
struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Int_tok[ 12u]; struct _tuple7{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{
char* tag; struct _tuple7* f1; } ; extern char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Okay_tok[ 13u]; extern char Cyc_ParamDeclListBool_tok[
26u]; struct _tuple8{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3;
} ; struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple8* f1; } ;
extern char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDecl_tok[ 18u];
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple1* f1; } ; extern char
Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok[ 16u]; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; extern char Cyc_Pointer_Sort_tok[ 21u];
struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Primop_tok[
15u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Primopopt_tok[
18u]; struct Cyc_Primopopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ;
extern char Cyc_QualId_tok[ 15u]; struct Cyc_QualId_tok_struct{ char* tag;
struct _tuple0* f1; } ; extern char Cyc_QualSpecList_tok[ 21u]; struct _tuple9{
struct Cyc_Absyn_Tqual f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; }
; struct Cyc_QualSpecList_tok_struct{ char* tag; struct _tuple9* f1; } ; extern
char Cyc_Rgn_tok[ 12u]; struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ char* tag; short
f1; } ; extern char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern char Cyc_StorageClass_tok[ 21u]; struct
Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_String_tok[
15u]; struct Cyc_String_tok_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stringopt_tok[ 18u]; struct Cyc_Stringopt_tok_struct{ char* tag;
struct Cyc_Core_Opt* f1; } ; extern char Cyc_StructFieldDeclListList_tok[ 32u];
struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclList_tok[ 28u]; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_StructOrUnion_tok[ 22u];
struct Cyc_StructOrUnion_tok_struct{ char* tag; void* f1; } ; extern char Cyc_SwitchClauseList_tok[
25u]; struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_TunionFieldList_tok[ 24u]; struct Cyc_TunionFieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_TunionField_tok[ 20u];
struct Cyc_TunionField_tok_struct{ char* tag; struct Cyc_Absyn_Tunionfield* f1;
} ; extern char Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_TypeQual_tok[ 17u]; struct Cyc_TypeQual_tok_struct{ char* tag;
struct Cyc_Absyn_Tqual f1; } ; extern char Cyc_TypeSpecifier_tok[ 22u]; struct
Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Type_tok[
13u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; extern
int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
extern char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_is_function_type( void* t); extern char Cyc_Tcdecl_Incompatible[ 17u];
extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment* loc, struct
_tagged_string* msg0, struct _tagged_string msg1); extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Vardecl* Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Typedefdecl* Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_sub_structdecl(
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
extern void Cyc_Lex_lex_init(); struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* tuniondecls;
struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict*
vardecls; } ; typedef struct Cyc_Interface_Ienv* Cyc_Interface_ienv_t; struct
Cyc_Interface_I{ struct Cyc_Dict_Dict* imports; struct Cyc_Dict_Dict* exports; }
; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){ return({ struct
Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof(
struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); _temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});}
static struct Cyc_Interface_Ienv* Cyc_Interface_skel2i_f( void* ignored){ return
Cyc_Interface_new_ienv();} static struct Cyc_Interface_I* Cyc_Interface_skel2i(
struct Cyc_Dict_Dict* skel){ return({ struct Cyc_Interface_I* _temp1=( struct
Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp1->imports=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict*
d)) Cyc_Dict_map)(( struct Cyc_Interface_Ienv*(*)( void* ignored)) Cyc_Interface_skel2i_f,
skel); _temp1->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(*
f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct Cyc_Interface_Ienv*(*)(
void* ignored)) Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I*
Cyc_Interface_empty(){ return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} static void Cyc_Interface_err( struct _tagged_string msg){ Cyc_Tcutil_terr(
0, msg);} static void Cyc_Interface_fields_err( struct _tagged_string sc, struct
_tagged_string t, struct _tuple0* n){ Cyc_Interface_err(({ struct _tagged_string
_temp3= sc; struct _tagged_string _temp4= t; struct _tagged_string _temp5= Cyc_Absynpp_qvar2string(
n); xprintf("fields of %.*s %.*s %.*s have never been defined", _temp3.last_plus_one
- _temp3.curr, _temp3.curr, _temp4.last_plus_one - _temp4.curr, _temp4.curr,
_temp5.last_plus_one - _temp5.curr, _temp5.curr);}));} static void Cyc_Interface_body_err(
struct _tagged_string sc, struct _tuple0* n){ Cyc_Interface_err(({ struct
_tagged_string _temp6= sc; struct _tagged_string _temp7= Cyc_Absynpp_qvar2string(
n); xprintf("the body of %.*s function %.*s has never been defined", _temp6.last_plus_one
- _temp6.curr, _temp6.curr, _temp7.last_plus_one - _temp7.curr, _temp7.curr);}));}
static void Cyc_Interface_static_err( struct _tagged_string obj1, struct _tuple0*
name1, struct _tagged_string obj2, struct _tuple0* name2){ Cyc_Interface_err(({
struct _tagged_string _temp8= obj1; struct _tagged_string _temp9= Cyc_Absynpp_qvar2string(
name1); struct _tagged_string _temp10= obj2; struct _tagged_string _temp11= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s", _temp8.last_plus_one
- _temp8.curr, _temp8.curr, _temp9.last_plus_one - _temp9.curr, _temp9.curr,
_temp10.last_plus_one - _temp10.curr, _temp10.curr, _temp11.last_plus_one -
_temp11.curr, _temp11.curr);}));} static void Cyc_Interface_abstract_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp12= obj1;
struct _tagged_string _temp13= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp14= obj2; struct _tagged_string _temp15= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp12.last_plus_one - _temp12.curr, _temp12.curr, _temp13.last_plus_one -
_temp13.curr, _temp13.curr, _temp14.last_plus_one - _temp14.curr, _temp14.curr,
_temp15.last_plus_one - _temp15.curr, _temp15.curr);}));} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; typedef struct Cyc_Interface_Seen* Cyc_Interface_seen_t; static
struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({ struct Cyc_Interface_Seen*
_temp16=( struct Cyc_Interface_Seen*) GC_malloc( sizeof( struct Cyc_Interface_Seen));
_temp16->structs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp16->unions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp16->tunions=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp16;});} static int Cyc_Interface_check_public_type( struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_types(
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void*(* f)( void*), struct Cyc_List_List* l){ int res= 1; for( 0; l != 0; l=({
struct Cyc_List_List* _temp17= l; if( _temp17 == 0){ _throw( Null_Exception);}
_temp17->tl;})){ if( ! Cyc_Interface_check_public_type( seen, obj, name, f((
void*)({ struct Cyc_List_List* _temp18= l; if( _temp18 == 0){ _throw(
Null_Exception);} _temp18->hd;})))){ res= 0;}} return res;} static int Cyc_Interface_check_public_structdecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp19; _push_handler(& _temp19);{ int _temp21= 0; if( setjmp(
_temp19.handler)){ _temp21= 1;} if( ! _temp21){{ int _temp23=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)({ struct Cyc_Core_Opt* _temp22= d->name; if( _temp22 == 0){ _throw(
Null_Exception);} _temp22->v;})); _npop_handler( 0u); return _temp23;};
_pop_handler();} else{ void* _temp20=( void*) _exn_thrown; void* _temp25=
_temp20; _LL27: if( _temp25 == Cyc_Dict_Absent){ goto _LL28;} else{ goto _LL29;}
_LL29: goto _LL30; _LL28: { int res= 1; seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp31= d->name; if( _temp31 == 0){
_throw( Null_Exception);} _temp31->v;}), res); if( d->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp37= d->fields; if(
_temp37 == 0){ _throw( Null_Exception);} _temp37->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp32= fs; if( _temp32 == 0){ _throw( Null_Exception);}
_temp32->tl;})){ if( ! Cyc_Interface_check_public_type( seen,( struct
_tagged_string)({ char* _temp33=( char*)"struct"; struct _tagged_string _temp34;
_temp34.curr= _temp33; _temp34.base= _temp33; _temp34.last_plus_one= _temp33 + 7;
_temp34;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp35= d->name; if(
_temp35 == 0){ _throw( Null_Exception);} _temp35->v;}),( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp36= fs; if( _temp36 == 0){ _throw( Null_Exception);}
_temp36->hd;}))->type)){ res= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp38= d->name; if( _temp38 == 0){
_throw( Null_Exception);} _temp38->v;}), res); return res;} _LL30:( void) _throw(
_temp25); _LL26:;}}} static int Cyc_Interface_check_public_uniondecl( struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Uniondecl* d){ struct _handler_cons _temp39;
_push_handler(& _temp39);{ int _temp41= 0; if( setjmp( _temp39.handler)){
_temp41= 1;} if( ! _temp41){{ int _temp43=(( int(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp42= d->name; if( _temp42 == 0){ _throw( Null_Exception);}
_temp42->v;})); _npop_handler( 0u); return _temp43;}; _pop_handler();} else{
void* _temp40=( void*) _exn_thrown; void* _temp45= _temp40; _LL47: if( _temp45
== Cyc_Dict_Absent){ goto _LL48;} else{ goto _LL49;} _LL49: goto _LL50; _LL48: {
int res= 1; seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)({
struct Cyc_Core_Opt* _temp51= d->name; if( _temp51 == 0){ _throw( Null_Exception);}
_temp51->v;}), res); if( d->fields != 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp57= d->fields; if( _temp57 == 0){ _throw(
Null_Exception);} _temp57->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp52= fs; if( _temp52 == 0){ _throw( Null_Exception);} _temp52->tl;})){ if( !
Cyc_Interface_check_public_type( seen,( struct _tagged_string)({ char* _temp53=(
char*)"union"; struct _tagged_string _temp54; _temp54.curr= _temp53; _temp54.base=
_temp53; _temp54.last_plus_one= _temp53 + 6; _temp54;}),( struct _tuple0*)({
struct Cyc_Core_Opt* _temp55= d->name; if( _temp55 == 0){ _throw( Null_Exception);}
_temp55->v;}),( void*)(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp56= fs; if( _temp56 == 0){ _throw( Null_Exception);} _temp56->hd;}))->type)){
res= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)({
struct Cyc_Core_Opt* _temp58= d->name; if( _temp58 == 0){ _throw( Null_Exception);}
_temp58->v;}), res); return res;} _LL50:( void) _throw( _temp45); _LL46:;}}}
struct _tuple10{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl* d){ struct
_handler_cons _temp59; _push_handler(& _temp59);{ int _temp61= 0; if( setjmp(
_temp59.handler)){ _temp61= 1;} if( ! _temp61){{ int _temp62=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions, d->name);
_npop_handler( 0u); return _temp62;}; _pop_handler();} else{ void* _temp60=(
void*) _exn_thrown; void* _temp64= _temp60; _LL66: if( _temp64 == Cyc_Dict_Absent){
goto _LL67;} else{ goto _LL68;} _LL68: goto _LL69; _LL67: { int res= 1; seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, res); if( d->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp74= d->fields; if(
_temp74 == 0){ _throw( Null_Exception);} _temp74->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp70= fs; if( _temp70 == 0){ _throw( Null_Exception);}
_temp70->tl;})){ if( !(( int(*)( struct Cyc_Interface_Seen* seen, struct
_tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen,( struct
_tagged_string)({ char* _temp71=( char*)"[x]tunion"; struct _tagged_string
_temp72; _temp72.curr= _temp71; _temp72.base= _temp71; _temp72.last_plus_one=
_temp71 + 10; _temp72;}), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp73= fs; if( _temp73 == 0){ _throw( Null_Exception);}
_temp73->hd;}))->typs)){ res= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions,
d->name, res); return res;} _LL69:( void) _throw( _temp64); _LL65:;}}} static
int Cyc_Interface_check_public_enumdecl( struct Cyc_Interface_Seen* seen, struct
Cyc_Absyn_Enumdecl* d){ return 1;} static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl* d){ return Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp75=( char*)"typedef"; struct
_tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75; _temp76.last_plus_one=
_temp75 + 8; _temp76;}), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl* d){ return Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp77=( char*)"variable"; struct
_tagged_string _temp78; _temp78.curr= _temp77; _temp78.base= _temp77; _temp78.last_plus_one=
_temp77 + 9; _temp78;}), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static int Cyc_Interface_check_public_type(
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void* t){ void* _temp79= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp101; struct Cyc_Absyn_Conref* _temp103; struct Cyc_Absyn_Tqual _temp105;
struct Cyc_Absyn_Conref* _temp107; void* _temp109; void* _temp111; struct Cyc_Absyn_Exp*
_temp113; struct Cyc_Absyn_Tqual _temp115; void* _temp117; struct Cyc_Core_Opt*
_temp119; struct Cyc_Core_Opt _temp121; void* _temp122; struct Cyc_List_List*
_temp124; struct _tuple0* _temp126; struct Cyc_Absyn_FnInfo _temp128; struct Cyc_List_List*
_temp130; int _temp132; struct Cyc_List_List* _temp134; void* _temp136; struct
Cyc_Core_Opt* _temp138; struct Cyc_List_List* _temp140; struct Cyc_List_List*
_temp142; struct Cyc_Absyn_Structdecl** _temp144; struct Cyc_Absyn_Structdecl*
_temp146; struct Cyc_List_List* _temp147; struct _tuple0* _temp149; struct Cyc_Absyn_Uniondecl**
_temp151; struct Cyc_Absyn_Uniondecl* _temp153; struct Cyc_List_List* _temp154;
struct _tuple0* _temp156; struct Cyc_Absyn_TunionInfo _temp158; void* _temp160;
struct Cyc_List_List* _temp162; void* _temp164; struct Cyc_Absyn_Tuniondecl*
_temp166; struct Cyc_Absyn_TunionFieldInfo _temp168; struct Cyc_List_List*
_temp170; void* _temp172; struct Cyc_Absyn_Tunionfield* _temp174; struct Cyc_Absyn_Tuniondecl*
_temp176; _LL81: if(( unsigned int) _temp79 > 4u?*(( int*) _temp79) == Cyc_Absyn_PointerType:
0){ _LL102: _temp101=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp79)->f1; _LL112: _temp111=( void*) _temp101.elt_typ; goto _LL110; _LL110:
_temp109=( void*) _temp101.rgn_typ; goto _LL108; _LL108: _temp107=( struct Cyc_Absyn_Conref*)
_temp101.nullable; goto _LL106; _LL106: _temp105=( struct Cyc_Absyn_Tqual)
_temp101.tq; goto _LL104; _LL104: _temp103=( struct Cyc_Absyn_Conref*) _temp101.bounds;
goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp79 > 4u?*(( int*)
_temp79) == Cyc_Absyn_ArrayType: 0){ _LL118: _temp117=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp79)->f1; goto _LL116; _LL116: _temp115=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp79)->f2; goto _LL114; _LL114: _temp113=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp79)->f3; goto
_LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp79 > 4u?*(( int*)
_temp79) == Cyc_Absyn_TypedefType: 0){ _LL127: _temp126=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp79)->f1; goto _LL125; _LL125:
_temp124=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp79)->f2; goto _LL120; _LL120: _temp119=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp79)->f3; if( _temp119 == 0){ goto _LL87;} else{ _temp121=* _temp119; _LL123:
_temp122=( void*) _temp121.v; goto _LL86;}} else{ goto _LL87;} _LL87: if((
unsigned int) _temp79 > 4u?*(( int*) _temp79) == Cyc_Absyn_FnType: 0){ _LL129:
_temp128=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp79)->f1;
_LL141: _temp140=( struct Cyc_List_List*) _temp128.tvars; goto _LL139; _LL139:
_temp138=( struct Cyc_Core_Opt*) _temp128.effect; goto _LL137; _LL137: _temp136=(
void*) _temp128.ret_typ; goto _LL135; _LL135: _temp134=( struct Cyc_List_List*)
_temp128.args; goto _LL133; _LL133: _temp132=( int) _temp128.varargs; goto
_LL131; _LL131: _temp130=( struct Cyc_List_List*) _temp128.attributes; goto
_LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp79 > 4u?*(( int*)
_temp79) == Cyc_Absyn_TupleType: 0){ _LL143: _temp142=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp79)->f1; goto _LL90;} else{ goto _LL91;}
_LL91: if(( unsigned int) _temp79 > 4u?*(( int*) _temp79) == Cyc_Absyn_StructType:
0){ _LL150: _temp149=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp79)->f1; goto _LL148; _LL148: _temp147=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp79)->f2; goto _LL145; _LL145: _temp144=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp79)->f3;
if( _temp144 == 0){ goto _LL93;} else{ _temp146=* _temp144; goto _LL92;}} else{
goto _LL93;} _LL93: if(( unsigned int) _temp79 > 4u?*(( int*) _temp79) == Cyc_Absyn_UnionType:
0){ _LL157: _temp156=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp79)->f1; goto _LL155; _LL155: _temp154=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp79)->f2; goto _LL152; _LL152: _temp151=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp79)->f3;
if( _temp151 == 0){ goto _LL95;} else{ _temp153=* _temp151; goto _LL94;}} else{
goto _LL95;} _LL95: if(( unsigned int) _temp79 > 4u?*(( int*) _temp79) == Cyc_Absyn_TunionType:
0){ _LL159: _temp158=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp79)->f1; _LL165: _temp164=( void*) _temp158.tunion_info; if(*(( int*)
_temp164) == Cyc_Absyn_KnownTunion){ _LL167: _temp166=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp164)->f1; goto _LL163;} else{ goto
_LL97;} _LL163: _temp162=( struct Cyc_List_List*) _temp158.targs; goto _LL161;
_LL161: _temp160=( void*) _temp158.rgn; goto _LL96;} else{ goto _LL97;} _LL97:
if(( unsigned int) _temp79 > 4u?*(( int*) _temp79) == Cyc_Absyn_TunionFieldType:
0){ _LL169: _temp168=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp79)->f1; _LL173: _temp172=( void*) _temp168.field_info; if(*(( int*)
_temp172) == Cyc_Absyn_KnownTunionfield){ _LL177: _temp176=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp172)->f1; goto _LL175; _LL175:
_temp174=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp172)->f2; goto _LL171;} else{ goto _LL99;} _LL171: _temp170=( struct Cyc_List_List*)
_temp168.targs; goto _LL98;} else{ goto _LL99;} _LL99: goto _LL100; _LL82:
return Cyc_Interface_check_public_type( seen, obj, name, _temp111); _LL84:
return Cyc_Interface_check_public_type( seen, obj, name, _temp117); _LL86:
return Cyc_Interface_check_public_type( seen, obj, name, _temp122); _LL88:
return(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj,
struct _tuple0* name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l))
Cyc_Interface_check_public_types)( seen, obj, name, Cyc_Interface_get_type2,
_temp134)? Cyc_Interface_check_public_type( seen, obj, name, _temp136): 0; _LL90:
return(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj,
struct _tuple0* name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l))
Cyc_Interface_check_public_types)( seen, obj, name, Cyc_Interface_get_type1,
_temp142); _LL92: if(( void*) _temp146->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_string)({ char* _temp178=( char*)"struct"; struct
_tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 7; _temp179;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp180= _temp146->name; if( _temp180 == 0){ _throw(
Null_Exception);} _temp180->v;})); return 0;} return(( int(*)( struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,(
void*(*)( void*)) Cyc_Core_identity, _temp147)? Cyc_Interface_check_public_structdecl(
seen, _temp146): 0; _LL94: if(( void*) _temp153->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp181=(
char*)"union"; struct _tagged_string _temp182; _temp182.curr= _temp181; _temp182.base=
_temp181; _temp182.last_plus_one= _temp181 + 6; _temp182;}),( struct _tuple0*)({
struct Cyc_Core_Opt* _temp183= _temp153->name; if( _temp183 == 0){ _throw(
Null_Exception);} _temp183->v;})); return 0;} return(( int(*)( struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,(
void*(*)( void*)) Cyc_Core_identity, _temp154)? Cyc_Interface_check_public_uniondecl(
seen, _temp153): 0; _LL96: if(( void*) _temp166->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp184=(
char*)"[x]tunion"; struct _tagged_string _temp185; _temp185.curr= _temp184;
_temp185.base= _temp184; _temp185.last_plus_one= _temp184 + 10; _temp185;}),
_temp166->name); return 0;} return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_types)( seen, obj, name,( void*(*)( void*)) Cyc_Core_identity,
_temp162)? Cyc_Interface_check_public_tuniondecl( seen, _temp166): 0; _LL98: if((
void*) _temp176->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
name,( struct _tagged_string)({ char* _temp186=( char*)"[x]tunion"; struct
_tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 10; _temp187;}), _temp176->name); return 0;}
if(( void*) _temp176->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name,( struct _tagged_string)({ char* _temp188=( char*)"[x]tunion"; struct
_tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 10; _temp189;}), _temp176->name); return 0;}
return(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj,
struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_types)(
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp170)? Cyc_Interface_check_public_tuniondecl(
seen, _temp176): 0; _LL100: return 1; _LL80:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Interface_Seen* f4; } ;
static void Cyc_Interface_extract_structdecl( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple11 _temp192;
struct Cyc_Interface_Seen* _temp193; int _temp195; struct Cyc_Interface_Ienv*
_temp197; struct Cyc_Interface_Ienv* _temp199; struct _tuple11* _temp190= env;
_temp192=* _temp190; _LL200: _temp199= _temp192.f1; goto _LL198; _LL198:
_temp197= _temp192.f2; goto _LL196; _LL196: _temp195= _temp192.f3; goto _LL194;
_LL194: _temp193= _temp192.f4; goto _LL191; _LL191: { struct Cyc_Absyn_Structdecl*
d=* dp; void* _temp201=( void*) d->sc; _LL203: if( _temp201 ==( void*) Cyc_Absyn_Static){
goto _LL204;} else{ goto _LL205;} _LL205: if( _temp201 ==( void*) Cyc_Absyn_Abstract){
goto _LL206;} else{ goto _LL207;} _LL207: if( _temp201 ==( void*) Cyc_Absyn_Public){
goto _LL208;} else{ goto _LL209;} _LL209: if( _temp201 ==( void*) Cyc_Absyn_ExternC){
goto _LL210;} else{ goto _LL211;} _LL211: if( _temp201 ==( void*) Cyc_Absyn_Extern){
goto _LL212;} else{ goto _LL202;} _LL204: if( _temp195? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp213=( char*)"static"; struct _tagged_string
_temp214; _temp214.curr= _temp213; _temp214.base= _temp213; _temp214.last_plus_one=
_temp213 + 7; _temp214;}),( struct _tagged_string)({ char* _temp215=( char*)"struct";
struct _tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 7; _temp216;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp217= d->name; if( _temp217 == 0){ _throw( Null_Exception);}
_temp217->v;}));} goto _LL202; _LL206: if( d->fields == 0){ if( _temp195){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp218=( char*)"abstract"; struct
_tagged_string _temp219; _temp219.curr= _temp218; _temp219.base= _temp218;
_temp219.last_plus_one= _temp218 + 9; _temp219;}),( struct _tagged_string)({
char* _temp220=( char*)"struct"; struct _tagged_string _temp221; _temp221.curr=
_temp220; _temp221.base= _temp220; _temp221.last_plus_one= _temp220 + 7;
_temp221;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp222= d->name; if(
_temp222 == 0){ _throw( Null_Exception);} _temp222->v;}));}} else{ d=({ struct
Cyc_Absyn_Structdecl* _temp223=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp223[ 0]=* d; _temp223;}); d->fields=
0;} if( Cyc_Interface_check_public_structdecl( _temp193, d)){ _temp197->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp197->structdecls, x,
d);} goto _LL202; _LL208: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp224=( char*)"public"; struct _tagged_string
_temp225; _temp225.curr= _temp224; _temp225.base= _temp224; _temp225.last_plus_one=
_temp224 + 7; _temp225;}),( struct _tagged_string)({ char* _temp226=( char*)"struct";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 7; _temp227;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp228= d->name; if( _temp228 == 0){ _throw( Null_Exception);}
_temp228->v;})); d=({ struct Cyc_Absyn_Structdecl* _temp229=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp229[ 0]=* d; _temp229;});(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl(
_temp193, d)){ _temp197->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp197->structdecls, x, d);} goto _LL202; _LL210:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp230=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp230[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp231; _temp231.tag= Cyc_Core_InvalidArg; _temp231.f1=(
struct _tagged_string)({ char* _temp232=( char*)"extract_structdecl"; struct
_tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 19; _temp233;}); _temp231;}); _temp230;}));
_LL212: if( Cyc_Interface_check_public_structdecl( _temp193, d)){ _temp199->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp199->structdecls, x,
d);} goto _LL202; _LL202:;}} static void Cyc_Interface_extract_uniondecl( struct
_tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple11 _temp236; struct Cyc_Interface_Seen* _temp237; int _temp239;
struct Cyc_Interface_Ienv* _temp241; struct Cyc_Interface_Ienv* _temp243; struct
_tuple11* _temp234= env; _temp236=* _temp234; _LL244: _temp243= _temp236.f1;
goto _LL242; _LL242: _temp241= _temp236.f2; goto _LL240; _LL240: _temp239=
_temp236.f3; goto _LL238; _LL238: _temp237= _temp236.f4; goto _LL235; _LL235: {
struct Cyc_Absyn_Uniondecl* d=* dp; void* _temp245=( void*) d->sc; _LL247: if(
_temp245 ==( void*) Cyc_Absyn_Static){ goto _LL248;} else{ goto _LL249;} _LL249:
if( _temp245 ==( void*) Cyc_Absyn_Abstract){ goto _LL250;} else{ goto _LL251;}
_LL251: if( _temp245 ==( void*) Cyc_Absyn_Public){ goto _LL252;} else{ goto
_LL253;} _LL253: if( _temp245 ==( void*) Cyc_Absyn_ExternC){ goto _LL254;} else{
goto _LL255;} _LL255: if( _temp245 ==( void*) Cyc_Absyn_Extern){ goto _LL256;}
else{ goto _LL246;} _LL248: if( _temp239? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp257=( char*)"static"; struct _tagged_string
_temp258; _temp258.curr= _temp257; _temp258.base= _temp257; _temp258.last_plus_one=
_temp257 + 7; _temp258;}),( struct _tagged_string)({ char* _temp259=( char*)"union";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 6; _temp260;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp261= d->name; if( _temp261 == 0){ _throw( Null_Exception);}
_temp261->v;}));} goto _LL246; _LL250: if( d->fields == 0){ if( _temp239){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp262=( char*)"abstract"; struct
_tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 9; _temp263;}),( struct _tagged_string)({
char* _temp264=( char*)"union"; struct _tagged_string _temp265; _temp265.curr=
_temp264; _temp265.base= _temp264; _temp265.last_plus_one= _temp264 + 6;
_temp265;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp266= d->name; if(
_temp266 == 0){ _throw( Null_Exception);} _temp266->v;}));}} else{ d=({ struct
Cyc_Absyn_Uniondecl* _temp267=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Uniondecl)); _temp267[ 0]=* d; _temp267;}); d->fields= 0;} if(
Cyc_Interface_check_public_uniondecl( _temp237, d)){ _temp241->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp241->uniondecls, x, d);}
goto _LL246; _LL252: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp268=( char*)"public"; struct _tagged_string
_temp269; _temp269.curr= _temp268; _temp269.base= _temp268; _temp269.last_plus_one=
_temp268 + 7; _temp269;}),( struct _tagged_string)({ char* _temp270=( char*)"union";
struct _tagged_string _temp271; _temp271.curr= _temp270; _temp271.base= _temp270;
_temp271.last_plus_one= _temp270 + 6; _temp271;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp272= d->name; if( _temp272 == 0){ _throw( Null_Exception);}
_temp272->v;})); d=({ struct Cyc_Absyn_Uniondecl* _temp273=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp273[ 0]=* d; _temp273;});(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl(
_temp237, d)){ _temp241->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp241->uniondecls, x, d);} goto _LL246; _LL254:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp274=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp274[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp275; _temp275.tag= Cyc_Core_InvalidArg; _temp275.f1=(
struct _tagged_string)({ char* _temp276=( char*)"extract_uniondecl"; struct
_tagged_string _temp277; _temp277.curr= _temp276; _temp277.base= _temp276;
_temp277.last_plus_one= _temp276 + 18; _temp277;}); _temp275;}); _temp274;}));
_LL256: if( Cyc_Interface_check_public_uniondecl( _temp237, d)){ _temp243->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp243->uniondecls, x, d);}
goto _LL246; _LL246:;}} static void Cyc_Interface_extract_tuniondecl( struct
_tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp280; struct Cyc_Interface_Seen* _temp281; int _temp283;
struct Cyc_Interface_Ienv* _temp285; struct Cyc_Interface_Ienv* _temp287; struct
_tuple11* _temp278= env; _temp280=* _temp278; _LL288: _temp287= _temp280.f1;
goto _LL286; _LL286: _temp285= _temp280.f2; goto _LL284; _LL284: _temp283=
_temp280.f3; goto _LL282; _LL282: _temp281= _temp280.f4; goto _LL279; _LL279: {
struct Cyc_Absyn_Tuniondecl* d=* dp; void* _temp289=( void*) d->sc; _LL291: if(
_temp289 ==( void*) Cyc_Absyn_Static){ goto _LL292;} else{ goto _LL293;} _LL293:
if( _temp289 ==( void*) Cyc_Absyn_Abstract){ goto _LL294;} else{ goto _LL295;}
_LL295: if( _temp289 ==( void*) Cyc_Absyn_Public){ goto _LL296;} else{ goto
_LL297;} _LL297: if( _temp289 ==( void*) Cyc_Absyn_ExternC){ goto _LL298;} else{
goto _LL299;} _LL299: if( _temp289 ==( void*) Cyc_Absyn_Extern){ goto _LL300;}
else{ goto _LL290;} _LL292: if( _temp283? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp301=( char*)"static"; struct _tagged_string
_temp302; _temp302.curr= _temp301; _temp302.base= _temp301; _temp302.last_plus_one=
_temp301 + 7; _temp302;}),( struct _tagged_string)({ char* _temp303=( char*)"tunion";
struct _tagged_string _temp304; _temp304.curr= _temp303; _temp304.base= _temp303;
_temp304.last_plus_one= _temp303 + 7; _temp304;}), d->name);} goto _LL290;
_LL294: if( d->fields == 0){ if( _temp283){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp305=( char*)"abstract"; struct _tagged_string
_temp306; _temp306.curr= _temp305; _temp306.base= _temp305; _temp306.last_plus_one=
_temp305 + 9; _temp306;}),( struct _tagged_string)({ char* _temp307=( char*)"tunion";
struct _tagged_string _temp308; _temp308.curr= _temp307; _temp308.base= _temp307;
_temp308.last_plus_one= _temp307 + 7; _temp308;}), d->name);}} else{ d=({ struct
Cyc_Absyn_Tuniondecl* _temp309=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp309[ 0]=* d; _temp309;}); d->fields=
0;} if( Cyc_Interface_check_public_tuniondecl( _temp281, d)){ _temp285->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp285->tuniondecls, x,
d);} goto _LL290; _LL296: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp310=( char*)"public"; struct _tagged_string
_temp311; _temp311.curr= _temp310; _temp311.base= _temp310; _temp311.last_plus_one=
_temp310 + 7; _temp311;}),( struct _tagged_string)({ char* _temp312=( char*)"tunion";
struct _tagged_string _temp313; _temp313.curr= _temp312; _temp313.base= _temp312;
_temp313.last_plus_one= _temp312 + 7; _temp313;}), d->name); d=({ struct Cyc_Absyn_Tuniondecl*
_temp314=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp314[ 0]=* d; _temp314;});( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));}
if( Cyc_Interface_check_public_tuniondecl( _temp281, d)){ _temp285->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp285->tuniondecls, x,
d);} goto _LL290; _LL298:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp315=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp315[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp316; _temp316.tag= Cyc_Core_InvalidArg;
_temp316.f1=( struct _tagged_string)({ char* _temp317=( char*)"extract_tuniondecl";
struct _tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 19; _temp318;}); _temp316;}); _temp315;}));
_LL300: if( Cyc_Interface_check_public_tuniondecl( _temp281, d)){ _temp287->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp287->tuniondecls, x,
d);} goto _LL290; _LL290:;}} static void Cyc_Interface_extract_enumdecl( struct
_tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){ struct
_tuple11 _temp321; struct Cyc_Interface_Seen* _temp322; int _temp324; struct Cyc_Interface_Ienv*
_temp326; struct Cyc_Interface_Ienv* _temp328; struct _tuple11* _temp319= env;
_temp321=* _temp319; _LL329: _temp328= _temp321.f1; goto _LL327; _LL327:
_temp326= _temp321.f2; goto _LL325; _LL325: _temp324= _temp321.f3; goto _LL323;
_LL323: _temp322= _temp321.f4; goto _LL320; _LL320: { struct Cyc_Absyn_Enumdecl*
d=* dp; void* _temp330=( void*) d->sc; _LL332: if( _temp330 ==( void*) Cyc_Absyn_Static){
goto _LL333;} else{ goto _LL334;} _LL334: if( _temp330 ==( void*) Cyc_Absyn_Abstract){
goto _LL335;} else{ goto _LL336;} _LL336: if( _temp330 ==( void*) Cyc_Absyn_Public){
goto _LL337;} else{ goto _LL338;} _LL338: if( _temp330 ==( void*) Cyc_Absyn_ExternC){
goto _LL339;} else{ goto _LL340;} _LL340: if( _temp330 ==( void*) Cyc_Absyn_Extern){
goto _LL341;} else{ goto _LL331;} _LL333: if( _temp324? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp342=( char*)"static"; struct _tagged_string
_temp343; _temp343.curr= _temp342; _temp343.base= _temp342; _temp343.last_plus_one=
_temp342 + 7; _temp343;}),( struct _tagged_string)({ char* _temp344=( char*)"enum";
struct _tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 5; _temp345;}), d->name);} goto _LL331;
_LL335: if( d->fields == 0){ if( _temp324){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp346=( char*)"abstract"; struct _tagged_string
_temp347; _temp347.curr= _temp346; _temp347.base= _temp346; _temp347.last_plus_one=
_temp346 + 9; _temp347;}),( struct _tagged_string)({ char* _temp348=( char*)"enum";
struct _tagged_string _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 5; _temp349;}), d->name);}} else{ d=({ struct
Cyc_Absyn_Enumdecl* _temp350=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl)); _temp350[ 0]=* d; _temp350;}); d->fields= 0;} if(
Cyc_Interface_check_public_enumdecl( _temp322, d)){ _temp326->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp326->enumdecls, x, d);}
goto _LL331; _LL337: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp351=( char*)"public"; struct _tagged_string
_temp352; _temp352.curr= _temp351; _temp352.base= _temp351; _temp352.last_plus_one=
_temp351 + 7; _temp352;}),( struct _tagged_string)({ char* _temp353=( char*)"enum";
struct _tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 5; _temp354;}), d->name); d=({ struct Cyc_Absyn_Enumdecl*
_temp355=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp355[ 0]=* d; _temp355;});( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));}
if( Cyc_Interface_check_public_enumdecl( _temp322, d)){ _temp326->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp326->enumdecls, x, d);}
goto _LL331; _LL339:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp356=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp356[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp357; _temp357.tag= Cyc_Core_InvalidArg;
_temp357.f1=( struct _tagged_string)({ char* _temp358=( char*)"extract_enumdecl";
struct _tagged_string _temp359; _temp359.curr= _temp358; _temp359.base= _temp358;
_temp359.last_plus_one= _temp358 + 17; _temp359;}); _temp357;}); _temp356;}));
_LL341: if( Cyc_Interface_check_public_enumdecl( _temp322, d)){ _temp328->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp328->enumdecls, x, d);}
goto _LL331; _LL331:;}} static void Cyc_Interface_extract_typedef( struct
_tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple11 _temp362; struct Cyc_Interface_Seen* _temp363; int _temp365;
struct Cyc_Interface_Ienv* _temp367; struct Cyc_Interface_Ienv* _temp369; struct
_tuple11* _temp360= env; _temp362=* _temp360; _LL370: _temp369= _temp362.f1;
goto _LL368; _LL368: _temp367= _temp362.f2; goto _LL366; _LL366: _temp365=
_temp362.f3; goto _LL364; _LL364: _temp363= _temp362.f4; goto _LL361; _LL361:
if( Cyc_Interface_check_public_typedefdecl( _temp363, d)){ _temp367->typedefdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( _temp367->typedefdecls, x,
d);}} static void Cyc_Interface_extract_ordinarie( struct _tuple11* env, struct
_tagged_string* x, struct _tuple7* v){ struct _tuple11 _temp373; struct Cyc_Interface_Seen*
_temp374; int _temp376; struct Cyc_Interface_Ienv* _temp378; struct Cyc_Interface_Ienv*
_temp380; struct _tuple11* _temp371= env; _temp373=* _temp371; _LL381: _temp380=
_temp373.f1; goto _LL379; _LL379: _temp378= _temp373.f2; goto _LL377; _LL377:
_temp376= _temp373.f3; goto _LL375; _LL375: _temp374= _temp373.f4; goto _LL372;
_LL372: { void* r=(* v).f1; void* _temp382= r; void* _temp388; _LL384: if(*((
int*) _temp382) == Cyc_Tcenv_VarRes){ _LL389: _temp388=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp382)->f1; goto _LL385;} else{ goto _LL386;} _LL386: goto _LL387; _LL385:{
void* _temp390= _temp388; struct Cyc_Absyn_Fndecl* _temp398; struct Cyc_Absyn_Vardecl*
_temp400; _LL392: if(( unsigned int) _temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_Funname_b:
0){ _LL399: _temp398=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp390)->f1; goto _LL393;} else{ goto _LL394;} _LL394: if(( unsigned int)
_temp390 > 1u?*(( int*) _temp390) == Cyc_Absyn_Global_b: 0){ _LL401: _temp400=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp390)->f1;
goto _LL395;} else{ goto _LL396;} _LL396: goto _LL397; _LL393: { struct Cyc_Absyn_Vardecl*
vd=({ struct Cyc_Absyn_Vardecl* _temp402=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp402->sc=( void*)(( void*) _temp398->sc);
_temp402->name= _temp398->name; _temp402->tq=({ struct Cyc_Absyn_Tqual _temp404;
_temp404.q_const= 0; _temp404.q_volatile= 0; _temp404.q_restrict= 0; _temp404;});
_temp402->type=( void*)(( void*)({ struct Cyc_Core_Opt* _temp403= _temp398->cached_typ;
if( _temp403 == 0){ _throw( Null_Exception);} _temp403->v;})); _temp402->initializer=
0; _temp402->rgn= 0; _temp402->attributes= 0; _temp402;}); _temp376= 0; _temp400=
vd; goto _LL395;} _LL395: if( _temp400->initializer != 0){ _temp400=({ struct
Cyc_Absyn_Vardecl* _temp405=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Vardecl)); _temp405[ 0]=* _temp400; _temp405;}); _temp400->initializer=
0;}{ void* _temp406=( void*) _temp400->sc; _LL408: if( _temp406 ==( void*) Cyc_Absyn_Static){
goto _LL409;} else{ goto _LL410;} _LL410: if( _temp406 ==( void*) Cyc_Absyn_Abstract){
goto _LL411;} else{ goto _LL412;} _LL412: if( _temp406 ==( void*) Cyc_Absyn_Public){
goto _LL413;} else{ goto _LL414;} _LL414: if( _temp406 ==( void*) Cyc_Absyn_ExternC){
goto _LL415;} else{ goto _LL416;} _LL416: if( _temp406 ==( void*) Cyc_Absyn_Extern){
goto _LL417;} else{ goto _LL407;} _LL409: if( _temp376? Cyc_Tcutil_is_function_type((
void*) _temp400->type): 0){ Cyc_Interface_body_err(( struct _tagged_string)({
char* _temp418=( char*)"static"; struct _tagged_string _temp419; _temp419.curr=
_temp418; _temp419.base= _temp418; _temp419.last_plus_one= _temp418 + 7;
_temp419;}), _temp400->name);} goto _LL407; _LL411:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp420=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp420[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp421; _temp421.tag= Cyc_Core_InvalidArg; _temp421.f1=(
struct _tagged_string)({ char* _temp422=( char*)"extract_ordinarie"; struct
_tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 18; _temp423;}); _temp421;}); _temp420;}));
_LL413: if( _temp376? Cyc_Tcutil_is_function_type(( void*) _temp400->type): 0){
Cyc_Interface_body_err(( struct _tagged_string)({ char* _temp424=( char*)"public";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 7; _temp425;}), _temp400->name);} if( Cyc_Interface_check_public_vardecl(
_temp374, _temp400)){ _temp378->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp378->vardecls, x, _temp400);} goto _LL407; _LL415: goto _LL417; _LL417: if(
Cyc_Interface_check_public_vardecl( _temp374, _temp400)){ _temp380->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp380->vardecls, x,
_temp400);} goto _LL407; _LL407:;} goto _LL391; _LL397: goto _LL391; _LL391:;}
goto _LL383; _LL387: goto _LL383; _LL383:;}} struct _tuple12{ struct Cyc_Interface_I*
f1; int f2; struct Cyc_Interface_Seen* f3; } ; static void Cyc_Interface_extract_f(
struct _tuple12* env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){
struct _tuple12 _temp428; struct Cyc_Interface_Seen* _temp429; int _temp431;
struct Cyc_Interface_I* _temp433; struct _tuple12* _temp426= env_f; _temp428=*
_temp426; _LL434: _temp433= _temp428.f1; goto _LL432; _LL432: _temp431= _temp428.f2;
goto _LL430; _LL430: _temp429= _temp428.f3; goto _LL427; _LL427: { struct
_tuple11 env=({ struct _tuple11 _temp435; _temp435.f1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp433->imports,
ns); _temp435.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp433->exports, ns); _temp435.f3=
_temp431; _temp435.f4= _temp429; _temp435;});(( void(*)( void(* f)( struct
_tuple11*, struct _tagged_string*, struct Cyc_Absyn_Structdecl**), struct
_tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,&
env, ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& env, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
env, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& env, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
env, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& env, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 env=({ struct _tuple12 _temp436;
_temp436.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel)) Cyc_Interface_skel2i)(
ae); _temp436.f2= check_complete_defs; _temp436.f3= Cyc_Interface_new_seen();
_temp436;});(( void(*)( void(* f)( struct _tuple12*, struct Cyc_List_List*,
struct Cyc_Tcenv_Genv*), struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_f,& env, ae); return env.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_string* msg1, struct
_tagged_string msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct _tagged_string* f2; } ; struct _tuple14{ int(* f1)( struct _tagged_string*,
void*, void*); struct Cyc_Dict_Dict* f2; struct _tuple13* f3; struct
_tagged_string f4; } ; static void Cyc_Interface_incl_dict_f( struct _tuple14*
env, struct _tagged_string* x, void* y1){ struct _tuple14 _temp439; struct
_tagged_string _temp440; struct _tuple13* _temp442; struct Cyc_Dict_Dict*
_temp444; int(* _temp446)( struct _tagged_string*, void*, void*); struct
_tuple14* _temp437= env; _temp439=* _temp437; _LL447: _temp446= _temp439.f1;
goto _LL445; _LL445: _temp444= _temp439.f2; goto _LL443; _LL443: _temp442=
_temp439.f3; goto _LL441; _LL441: _temp440= _temp439.f4; goto _LL438; _LL438: {
struct _handler_cons _temp448; _push_handler(& _temp448);{ int _temp450= 0; if(
setjmp( _temp448.handler)){ _temp450= 1;} if( ! _temp450){{ void* y2=(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp444,
x); if( ! _temp446((* _temp442).f2, y1, y2)){(* _temp442).f1= 0;}}; _pop_handler();}
else{ void* _temp449=( void*) _exn_thrown; void* _temp452= _temp449; _LL454: if(
_temp452 == Cyc_Dict_Absent){ goto _LL455;} else{ goto _LL456;} _LL456: goto
_LL457; _LL455: Cyc_Interface_check_err((* _temp442).f2,({ struct _tagged_string
_temp458= _temp440; struct _tagged_string _temp459=* x; xprintf("%.*s %.*s is missing",
_temp458.last_plus_one - _temp458.curr, _temp458.curr, _temp459.last_plus_one -
_temp459.curr, _temp459.curr);}));(* _temp442).f1= 0; goto _LL453; _LL457:( void)
_throw( _temp452); _LL453:;}}}} static void Cyc_Interface_incl_dict( struct
_tuple13* genv, struct _tagged_string objname, int(* f)( struct _tagged_string*,
void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct
_tuple14 env=({ struct _tuple14 _temp460; _temp460.f1= f; _temp460.f2= d2;
_temp460.f3= genv; _temp460.f4= objname; _temp460;});(( void(*)( void(* f)(
struct _tuple14*, struct _tagged_string*, void*), struct _tuple14* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple14* env, struct
_tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& env, d1);} static int
Cyc_Interface_incl_structdecl( struct _tagged_string* msg, struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1){ struct Cyc_Absyn_Structdecl* d= Cyc_Tcdecl_merge_structdecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*) d0
!= d){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp462= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp461= d1->name; if( _temp461 == 0){
_throw( Null_Exception);} _temp461->v;})); xprintf("declaration of struct %.*s discloses too much information",
_temp462.last_plus_one - _temp462.curr, _temp462.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct _tagged_string* msg, struct Cyc_Absyn_Uniondecl*
d0, struct Cyc_Absyn_Uniondecl* d1){ struct Cyc_Absyn_Uniondecl* d= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 !=
d){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp464= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp463= d1->name; if( _temp463 == 0){
_throw( Null_Exception);} _temp463->v;})); xprintf("declaration of union %.*s discloses too much information",
_temp464.last_plus_one - _temp464.curr, _temp464.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct _tagged_string* msg, struct Cyc_Absyn_Tuniondecl*
d0, struct Cyc_Absyn_Tuniondecl* d1){ struct Cyc_Absyn_Tuniondecl* d= Cyc_Tcdecl_merge_tuniondecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0
!= d){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp465= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp465.last_plus_one - _temp465.curr, _temp465.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct _tagged_string* msg, struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1){ struct Cyc_Absyn_Enumdecl* d= Cyc_Tcdecl_merge_enumdecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 !=
d){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp466= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp466.last_plus_one - _temp466.curr, _temp466.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct _tagged_string* msg, struct Cyc_Absyn_Vardecl*
d0, struct Cyc_Absyn_Vardecl* d1){ struct Cyc_Absyn_Vardecl* d= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Vardecl*) d0 != d){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp467= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of var %.*s discloses too much information",
_temp467.last_plus_one - _temp467.curr, _temp467.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct _tagged_string* msg, struct
Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1){ struct Cyc_Absyn_Typedefdecl*
d= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); if( d == 0){ return 0;} if((
struct Cyc_Absyn_Typedefdecl*) d0 != d){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp468= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_temp468.last_plus_one - _temp468.curr, _temp468.curr);})); return 0;} return 1;}
static void Cyc_Interface_incl_ns( struct _tuple13* env, struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2){(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp469=(
char*)"struct"; struct _tagged_string _temp470; _temp470.curr= _temp469;
_temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 7; _temp470;}), Cyc_Interface_incl_structdecl,
ie1->structdecls, ie2->structdecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp471=( char*)"union";
struct _tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 6; _temp472;}), Cyc_Interface_incl_uniondecl,
ie1->uniondecls, ie2->uniondecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp473=(
char*)"tunion"; struct _tagged_string _temp474; _temp474.curr= _temp473;
_temp474.base= _temp473; _temp474.last_plus_one= _temp473 + 7; _temp474;}), Cyc_Interface_incl_tuniondecl,
ie1->tuniondecls, ie2->tuniondecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp475=( char*)"enum";
struct _tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 5; _temp476;}), Cyc_Interface_incl_enumdecl,
ie1->enumdecls, ie2->enumdecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp477=( char*)"variable";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 9; _temp478;}), Cyc_Interface_incl_vardecl,
ie1->vardecls, ie2->vardecls);} int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct _handler_cons
_temp479; _push_handler(& _temp479);{ int _temp481= 0; if( setjmp( _temp479.handler)){
_temp481= 1;} if( ! _temp481){{ struct _tuple13 env=({ struct _tuple13 _temp493;
_temp493.f1= 1; _temp493.f2= 0; _temp493;}); if( info != 0){ env.f2=({ struct
_tagged_string* _temp482=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp482[ 0]=({ struct _tagged_string _temp485=({ struct
_tuple3* _temp483= info; if( _temp483 == 0){ _throw( Null_Exception);}* _temp483;}).f2;
struct _tagged_string _temp486=({ struct _tuple3* _temp484= info; if( _temp484
== 0){ _throw( Null_Exception);}* _temp484;}).f1; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp485.last_plus_one - _temp485.curr, _temp485.curr, _temp486.last_plus_one -
_temp486.curr, _temp486.curr);}); _temp482;});}(( void(*)( void(* f)( struct
_tuple13*, struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct
_tuple13* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)(
Cyc_Interface_incl_ns,& env, i2->exports, i1->exports); if( info != 0){ env.f2=({
struct _tagged_string* _temp487=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp487[ 0]=({ struct _tagged_string _temp490=({
struct _tuple3* _temp488= info; if( _temp488 == 0){ _throw( Null_Exception);}*
_temp488;}).f1; struct _tagged_string _temp491=({ struct _tuple3* _temp489= info;
if( _temp489 == 0){ _throw( Null_Exception);}* _temp489;}).f2; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp490.last_plus_one - _temp490.curr, _temp490.curr, _temp491.last_plus_one -
_temp491.curr, _temp491.curr);}); _temp487;});}(( void(*)( void(* f)( struct
_tuple13*, struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct
_tuple13* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)(
Cyc_Interface_incl_ns,& env, i1->imports, i2->imports);{ int _temp492= env.f1;
_npop_handler( 0u); return _temp492;}}; _pop_handler();} else{ void* _temp480=(
void*) _exn_thrown; void* _temp495= _temp480; _LL497: if( _temp495 == Cyc_Tcdecl_Incompatible){
goto _LL498;} else{ goto _LL499;} _LL499: goto _LL500; _LL498: return 0; _LL500:(
void) _throw( _temp495); _LL496:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); void**(* f6)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*, int*);
struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15 _temp503;
struct _tagged_string* _temp504; void**(* _temp506)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*, int*); void**(* _temp508)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*); struct Cyc_Dict_Dict* _temp510; struct Cyc_Dict_Dict*
_temp512; struct Cyc_Dict_Dict* _temp514; struct Cyc_Dict_Dict** _temp516; int
_temp517; int* _temp519; struct _tuple15* _temp501= env; _temp503=* _temp501;
_LL518: _temp517= _temp503.f1; _temp519=&(* _temp501).f1; goto _LL515; _LL515:
_temp514= _temp503.f2; _temp516=&(* _temp501).f2; goto _LL513; _LL513: _temp512=
_temp503.f3; goto _LL511; _LL511: _temp510= _temp503.f4; goto _LL509; _LL509:
_temp508= _temp503.f5; goto _LL507; _LL507: _temp506= _temp503.f6; goto _LL505;
_LL505: _temp504= _temp503.f7; goto _LL502; _LL502: { void** y;{ struct
_handler_cons _temp520; _push_handler(& _temp520);{ int _temp522= 0; if( setjmp(
_temp520.handler)){ _temp522= 1;} if( ! _temp522){{ void** y1=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp512,
x); void** yt= _temp508( y1, y2, 0, _temp504); if( !(( unsigned int) yt)){*
_temp519= 0; _npop_handler( 0u); return;} y=( void**)({ void** _temp523= yt; if(
_temp523 == 0){ _throw( Null_Exception);} _temp523;});}; _pop_handler();} else{
void* _temp521=( void*) _exn_thrown; void* _temp525= _temp521; _LL527: if(
_temp525 == Cyc_Dict_Absent){ goto _LL528;} else{ goto _LL529;} _LL529: goto
_LL530; _LL528: y= y2; goto _LL526; _LL530:( void) _throw( _temp525); _LL526:;}}}{
struct _handler_cons _temp531; _push_handler(& _temp531);{ int _temp533= 0; if(
setjmp( _temp531.handler)){ _temp533= 1;} if( ! _temp533){{ void** ye=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp510,
x); int* r=({ int* _temp535=( int*) GC_malloc_atomic( sizeof( int)); _temp535[ 0]=
1; _temp535;}); void** yt= _temp506( ye, y, 0, _temp504, r); if( !(* r)){*
_temp519= 0;} if(( unsigned int) yt){* _temp516=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(*
_temp516, x,( void**)({ void** _temp534= yt; if( _temp534 == 0){ _throw(
Null_Exception);} _temp534;}));}}; _pop_handler();} else{ void* _temp532=( void*)
_exn_thrown; void* _temp537= _temp532; _LL539: if( _temp537 == Cyc_Dict_Absent){
goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL540: if(* _temp519){*
_temp516=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void** data)) Cyc_Dict_insert)(* _temp516, x, y);} goto
_LL538; _LL542:( void) _throw( _temp537); _LL538:;}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*, struct
_tagged_string*), void**(* sub_f)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*, int*), struct _tagged_string* msg){ struct _tuple15 env=({
struct _tuple15 _temp544; _temp544.f1= 1; _temp544.f2= dic1; _temp544.f3= dic1;
_temp544.f4= excl; _temp544.f5= merge_f; _temp544.f6= sub_f; _temp544.f7= msg;
_temp544;});(( void(*)( void(* f)( struct _tuple15*, struct _tagged_string*,
void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp543=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp543->v=( void*) env.f2; _temp543;});}
else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_compat_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct Cyc_Interface_Ienv*
iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt* r1=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Structdecl*(* sub_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
Cyc_Tcdecl_sub_structdecl, msg); struct Cyc_Core_Opt* r2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct Cyc_Absyn_Uniondecl*(* sub_f)( struct Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*,
struct Cyc_Position_Segment*, struct _tagged_string*, int*), struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->uniondecls, ie2->uniondecls,
iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl, Cyc_Tcdecl_sub_uniondecl, msg);
struct Cyc_Core_Opt* r3=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(*
merge_f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Tuniondecl*(* sub_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
Cyc_Tcdecl_sub_tuniondecl, msg); struct Cyc_Core_Opt* r4=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct Cyc_Absyn_Enumdecl*, struct
Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct Cyc_Absyn_Enumdecl*(* sub_f)( struct Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*,
struct Cyc_Position_Segment*, struct _tagged_string*, int*), struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->enumdecls, ie2->enumdecls,
iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl, Cyc_Tcdecl_sub_enumdecl, msg);
struct Cyc_Core_Opt* r5=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct Cyc_Absyn_Typedefdecl*(*
sub_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
Cyc_Tcdecl_sub_typedefdecl, msg); struct Cyc_Core_Opt* r6=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct Cyc_Absyn_Vardecl*, struct
Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct Cyc_Absyn_Vardecl*(* sub_f)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*,
struct Cyc_Position_Segment*, struct _tagged_string*, int*), struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->vardecls, ie2->vardecls,
iexcl->vardecls, Cyc_Tcdecl_merge_vardecl, Cyc_Tcdecl_sub_vardecl, msg); if(((((
!(( unsigned int) r1)? 1: !(( unsigned int) r2))? 1: !(( unsigned int) r3))? 1:
!(( unsigned int) r4))? 1: !(( unsigned int) r5))? 1: !(( unsigned int) r6)){
return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp545=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp545->structdecls=( struct
Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp551= r1; if( _temp551 == 0){ _throw(
Null_Exception);} _temp551->v;}); _temp545->uniondecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp550= r2; if( _temp550 == 0){ _throw( Null_Exception);}
_temp550->v;}); _temp545->tuniondecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp549= r3; if( _temp549 == 0){ _throw( Null_Exception);} _temp549->v;});
_temp545->enumdecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp548= r4;
if( _temp548 == 0){ _throw( Null_Exception);} _temp548->v;}); _temp545->typedefdecls=(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp547= r5; if( _temp547 == 0){
_throw( Null_Exception);} _temp547->v;}); _temp545->vardecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp546= r6; if( _temp546 == 0){ _throw( Null_Exception);}
_temp546->v;}); _temp545;});}} static struct Cyc_Interface_Ienv* Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv* ie= 0; if( ie == 0){ ie=( struct Cyc_Interface_Ienv*)
Cyc_Interface_new_ienv();} return( struct Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv*
_temp552= ie; if( _temp552 == 0){ _throw( Null_Exception);} _temp552;});} struct
_tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict*
f4; struct _tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct
_tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct
_tuple16 _temp555; struct _tagged_string* _temp556; struct Cyc_Dict_Dict*
_temp558; struct Cyc_Dict_Dict* _temp560; struct Cyc_Dict_Dict* _temp562; struct
Cyc_Dict_Dict** _temp564; int _temp565; int* _temp567; struct _tuple16* _temp553=
env; _temp555=* _temp553; _LL566: _temp565= _temp555.f1; _temp567=&(* _temp553).f1;
goto _LL563; _LL563: _temp562= _temp555.f2; _temp564=&(* _temp553).f2; goto
_LL561; _LL561: _temp560= _temp555.f3; goto _LL559; _LL559: _temp558= _temp555.f4;
goto _LL557; _LL557: _temp556= _temp555.f5; goto _LL554; _LL554: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp568;
_push_handler(& _temp568);{ int _temp570= 0; if( setjmp( _temp568.handler)){
_temp570= 1;} if( ! _temp570){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp558, ns);;
_pop_handler();} else{ void* _temp569=( void*) _exn_thrown; void* _temp572=
_temp569; _LL574: if( _temp572 == Cyc_Dict_Absent){ goto _LL575;} else{ goto
_LL576;} _LL576: goto _LL577; _LL575: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL573; _LL577:( void) _throw( _temp572); _LL573:;}}}{ struct _handler_cons
_temp578; _push_handler(& _temp578);{ int _temp580= 0; if( setjmp( _temp578.handler)){
_temp580= 1;} if( ! _temp580){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp560, ns);{ struct Cyc_Interface_Ienv*
ie= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1, iexcl,
_temp556); if( ie == 0){* _temp567= 0; _npop_handler( 0u); return;} ie1=( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp581= ie; if( _temp581 ==
0){ _throw( Null_Exception);} _temp581;});}; _pop_handler();} else{ void*
_temp579=( void*) _exn_thrown; void* _temp583= _temp579; _LL585: if( _temp583 ==
Cyc_Dict_Absent){ goto _LL586;} else{ goto _LL587;} _LL587: goto _LL588; _LL586:
ie1= Cyc_Interface_lazy_new_ienv(); goto _LL584; _LL588:( void) _throw( _temp583);
_LL584:;}}}{ struct Cyc_Interface_Ienv* ie= Cyc_Interface_compat_merge_ienv( ie1,
ie2, iexcl, _temp556); if( ie == 0){* _temp567= 0;} else{ if(* _temp567){*
_temp564=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp564, ns,( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp589= ie; if( _temp589 ==
0){ _throw( Null_Exception);} _temp589;}));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 env=({ struct _tuple16
_temp591; _temp591.f1= 1; _temp591.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp591.f3= dic1; _temp591.f4= excl; _temp591.f5= msg; _temp591;});(( void(*)(
void(* f)( struct _tuple16*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp590=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp590->v=( void*) env.f2; _temp590;});}
else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict*
f3; struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp594; struct _tagged_string* _temp595; struct _tagged_string _temp597;
struct Cyc_Dict_Dict* _temp599; struct Cyc_Dict_Dict* _temp601; struct Cyc_Dict_Dict**
_temp603; int _temp604; int* _temp606; struct _tuple17* _temp592= env; _temp594=*
_temp592; _LL605: _temp604= _temp594.f1; _temp606=&(* _temp592).f1; goto _LL602;
_LL602: _temp601= _temp594.f2; _temp603=&(* _temp592).f2; goto _LL600; _LL600:
_temp599= _temp594.f3; goto _LL598; _LL598: _temp597= _temp594.f4; goto _LL596;
_LL596: _temp595= _temp594.f5; goto _LL593; _LL593: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp599, x)){ Cyc_Interface_check_err(
_temp595,({ struct _tagged_string _temp607= _temp597; struct _tagged_string
_temp608=* x; xprintf("%.*s %.*s is exported more than once", _temp607.last_plus_one
- _temp607.curr, _temp607.curr, _temp608.last_plus_one - _temp608.curr, _temp608.curr);}));*
_temp606= 0;} else{ if(* _temp606){* _temp603=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(*
_temp603, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg){ struct _tuple17 env=({ struct _tuple17 _temp610; _temp610.f1=
1; _temp610.f2= dic1; _temp610.f3= dic1; _temp610.f4= t; _temp610.f5= msg;
_temp610;});(( void(*)( void(* f)( struct _tuple17*, struct _tagged_string*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple17* env, struct _tagged_string* x, void* y)) Cyc_Interface_disj_merge_dict_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp609=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp609->v=( void*) env.f2; _temp609;});}
else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ struct Cyc_Interface_Ienv* res=({ struct Cyc_Interface_Ienv*
_temp611=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp611->structdecls=({ struct Cyc_Core_Opt* r=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls,
ie2->structdecls,( struct _tagged_string)({ char* _temp626=( char*)"struct";
struct _tagged_string _temp627; _temp627.curr= _temp626; _temp627.base= _temp626;
_temp627.last_plus_one= _temp626 + 7; _temp627;}), msg); if( r == 0){ return 0;}(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp625= r; if( _temp625 == 0){
_throw( Null_Exception);} _temp625->v;});}); _temp611->uniondecls=({ struct Cyc_Core_Opt*
r=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->uniondecls, ie2->uniondecls,( struct _tagged_string)({ char* _temp623=(
char*)"union"; struct _tagged_string _temp624; _temp624.curr= _temp623; _temp624.base=
_temp623; _temp624.last_plus_one= _temp623 + 6; _temp624;}), msg); if( r == 0){
return 0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp622= r; if(
_temp622 == 0){ _throw( Null_Exception);} _temp622->v;});}); _temp611->tuniondecls=({
struct Cyc_Core_Opt* r=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls,( struct
_tagged_string)({ char* _temp620=( char*)"[x]tunion"; struct _tagged_string
_temp621; _temp621.curr= _temp620; _temp621.base= _temp620; _temp621.last_plus_one=
_temp620 + 10; _temp621;}), msg); if( r == 0){ return 0;}( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp619= r; if( _temp619 == 0){ _throw( Null_Exception);}
_temp619->v;});}); _temp611->enumdecls=({ struct Cyc_Core_Opt* r=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,(
struct _tagged_string)({ char* _temp617=( char*)"enum"; struct _tagged_string
_temp618; _temp618.curr= _temp617; _temp618.base= _temp617; _temp618.last_plus_one=
_temp617 + 5; _temp618;}), msg); if( r == 0){ return 0;}( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp616= r; if( _temp616 == 0){ _throw( Null_Exception);}
_temp616->v;});}); _temp611->typedefdecls=({ struct Cyc_Core_Opt* r=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct
Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Typedefdecl*(* sub_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, Cyc_Tcdecl_sub_typedefdecl, msg); if( r == 0){
return 0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp615= r; if(
_temp615 == 0){ _throw( Null_Exception);} _temp615->v;});}); _temp611->vardecls=({
struct Cyc_Core_Opt* r=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls,( struct
_tagged_string)({ char* _temp613=( char*)"variable"; struct _tagged_string
_temp614; _temp614.curr= _temp613; _temp614.base= _temp613; _temp614.last_plus_one=
_temp613 + 9; _temp614;}), msg); if( r == 0){ return 0;}( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp612= r; if( _temp612 == 0){ _throw( Null_Exception);}
_temp612->v;});}); _temp611;}); return res;} struct _tuple18{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; } ; void Cyc_Interface_disj_merge_ns_f(
struct _tuple18* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple18 _temp630; struct _tagged_string* _temp631; struct Cyc_Dict_Dict*
_temp633; struct Cyc_Dict_Dict* _temp635; struct Cyc_Dict_Dict** _temp637; int
_temp638; int* _temp640; struct _tuple18* _temp628= env; _temp630=* _temp628;
_LL639: _temp638= _temp630.f1; _temp640=&(* _temp628).f1; goto _LL636; _LL636:
_temp635= _temp630.f2; _temp637=&(* _temp628).f2; goto _LL634; _LL634: _temp633=
_temp630.f3; goto _LL632; _LL632: _temp631= _temp630.f4; goto _LL629; _LL629: {
struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons _temp641; _push_handler(&
_temp641);{ int _temp643= 0; if( setjmp( _temp641.handler)){ _temp643= 1;} if( !
_temp643){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( _temp633, ns);; _pop_handler();} else{
void* _temp642=( void*) _exn_thrown; void* _temp645= _temp642; _LL647: if(
_temp645 == Cyc_Dict_Absent){ goto _LL648;} else{ goto _LL649;} _LL649: goto
_LL650; _LL648: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL646; _LL650:( void)
_throw( _temp645); _LL646:;}}}{ struct Cyc_Interface_Ienv* ie= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp631); if( ie == 0){* _temp640= 0;} else{ if(* _temp640){*
_temp637=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp637, ns,( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp651= ie; if( _temp651 ==
0){ _throw( Null_Exception);} _temp651;}));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string*
msg){ struct _tuple18 env=({ struct _tuple18 _temp653; _temp653.f1= 1; _temp653.f2=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*)))
Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp653.f3= dic1; _temp653.f4= msg;
_temp653;});(( void(*)( void(* f)( struct _tuple18*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple18* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_ns_f,& env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt*
_temp652=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp652->v=( void*) env.f2; _temp652;});} else{ return 0;}} struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info){ struct _tagged_string* msg= 0; if( info != 0){ msg=({
struct _tagged_string* _temp654=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp654[ 0]=({ struct _tagged_string _temp657=({
struct _tuple3* _temp655= info; if( _temp655 == 0){ _throw( Null_Exception);}*
_temp655;}).f1; struct _tagged_string _temp658=({ struct _tuple3* _temp656= info;
if( _temp656 == 0){ _throw( Null_Exception);}* _temp656;}).f2; xprintf("merging exports of %.*s and %.*s,",
_temp657.last_plus_one - _temp657.curr, _temp657.curr, _temp658.last_plus_one -
_temp658.curr, _temp658.curr);}); _temp654;});}{ struct Cyc_Core_Opt* exp= Cyc_Interface_disj_merge_ns(
i1->exports, i2->exports, msg); if( exp == 0){ return 0;} if( info != 0){ msg=({
struct _tagged_string* _temp659=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp659[ 0]=({ struct _tagged_string _temp662=({
struct _tuple3* _temp660= info; if( _temp660 == 0){ _throw( Null_Exception);}*
_temp660;}).f1; struct _tagged_string _temp663=({ struct _tuple3* _temp661= info;
if( _temp661 == 0){ _throw( Null_Exception);}* _temp661;}).f2; xprintf("merging imports of %.*s and %.*s,",
_temp662.last_plus_one - _temp662.curr, _temp662.curr, _temp663.last_plus_one -
_temp663.curr, _temp663.curr);}); _temp659;});}{ struct Cyc_Core_Opt* imp= Cyc_Interface_compat_merge_ns(
i1->imports, i2->imports,( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp667= exp; if( _temp667 == 0){ _throw( Null_Exception);} _temp667->v;}), msg);
if( imp == 0){ return 0;} return({ struct Cyc_Interface_I* _temp664=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp664->imports=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp666= imp; if( _temp666 == 0){ _throw( Null_Exception);}
_temp666->v;}); _temp664->exports=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp665= exp; if( _temp665 == 0){ _throw( Null_Exception);} _temp665->v;});
_temp664;});}}} static struct Cyc_List_List* Cyc_Interface_add_namespace( struct
Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;} return({
struct Cyc_List_List* _temp668=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp668->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp669=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp669[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp670; _temp670.tag= Cyc_Absyn_Namespace_d;
_temp670.f1=( struct _tagged_string*)({ struct Cyc_List_List* _temp671= ns; if(
_temp671 == 0){ _throw( Null_Exception);} _temp671->hd;}); _temp670.f2= Cyc_Interface_add_namespace(
tds,({ struct Cyc_List_List* _temp672= ns; if( _temp672 == 0){ _throw(
Null_Exception);} _temp672->tl;})); _temp670;}); _temp669;}), 0); _temp668->tl=
0; _temp668;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl(
struct _tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp673=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp673->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp674=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp674[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp675; _temp675.tag= Cyc_Absyn_Struct_d; _temp675.f1=
d; _temp675;}); _temp674;}), 0); _temp673->tl= tds; _temp673;});} static struct
Cyc_List_List* Cyc_Interface_add_structdecl_header( struct _tagged_string* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({ struct Cyc_Absyn_Structdecl*
_temp676=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp676[ 0]=* d; _temp676;}); d->fields= 0;( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp677=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp677->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp678=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp678[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp679; _temp679.tag= Cyc_Absyn_Struct_d; _temp679.f1=
d; _temp679;}); _temp678;}), 0); _temp677->tl= tds; _temp677;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp680=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp680->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp681=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp681[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp682; _temp682.tag= Cyc_Absyn_Union_d;
_temp682.f1= d; _temp682;}); _temp681;}), 0); _temp680->tl= tds; _temp680;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp683=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp683[ 0]=* d; _temp683;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp684=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp684->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp685=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp685[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp686; _temp686.tag= Cyc_Absyn_Union_d;
_temp686.f1= d; _temp686;}); _temp685;}), 0); _temp684->tl= tds; _temp684;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp687=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp687->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp688=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp688[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp689; _temp689.tag= Cyc_Absyn_Tunion_d;
_temp689.f1= d; _temp689;}); _temp688;}), 0); _temp687->tl= tds; _temp687;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp690=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp690[ 0]=* d; _temp690;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp691=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp691->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp692=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp692[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp693; _temp693.tag= Cyc_Absyn_Tunion_d;
_temp693.f1= d; _temp693;}); _temp692;}), 0); _temp691->tl= tds; _temp691;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp694=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp694->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp695=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp695[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp696; _temp696.tag= Cyc_Absyn_Enum_d;
_temp696.f1= d; _temp696;}); _temp695;}), 0); _temp694->tl= tds; _temp694;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp697=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp697[ 0]=* d; _temp697;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp698=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp698->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp699=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp699[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp700; _temp700.tag= Cyc_Absyn_Enum_d;
_temp700.f1= d; _temp700;}); _temp699;}), 0); _temp698->tl= tds; _temp698;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp701=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp701->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp702=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp702[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp703; _temp703.tag= Cyc_Absyn_Typedef_d;
_temp703.f1= d; _temp703;}); _temp702;}), 0); _temp701->tl= tds; _temp701;});}
static struct Cyc_List_List* Cyc_Interface_add_var( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp704=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp704->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp705=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp705[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp706; _temp706.tag= Cyc_Absyn_Var_d;
_temp706.f1= d; _temp706;}); _temp705;}), 0); _temp704->tl= tds; _temp704;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List* tds= 0;
tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header,
ie->structdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);} static void Cyc_Interface_print_ns_typedefs( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
tds= 0; tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef,
ie->typedefdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);} static void Cyc_Interface_print_ns_decls( struct Cyc_Stdio___sFILE* f,
struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
tds= 0; tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl,
ie->structdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_var,
ie->vardecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);} void Cyc_Interface_print( struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE*
f){ fprintf( f,"/****** needed (headers) ******/\n");(( void(*)( void(* f)(
struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->imports); fprintf( f,"\n/****** provided (headers) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_headers,
f, i->exports); fprintf( f,"\n/****** provided (typedefs) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_typedefs,
f, i->exports); fprintf( f,"\n/****** needed (declarations) ******/\n");(( void(*)(
void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->imports); fprintf( f,"\n/****** provided (declarations) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct Cyc_Stdio___sFILE* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_print_ns_decls,
f, i->exports);} struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* tds= Cyc_Parse_parse_file( f);
Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv* te= Cyc_Tcenv_tc_init();
Cyc_Tc_tc( te, 0, 0, tds); return Cyc_Interface_gen_extract( te->ae, 0);}}}