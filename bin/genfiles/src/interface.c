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
_tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t;
extern struct _tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl*
Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct
Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d,
void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); extern void* Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct
Cyc_Dict_Dict* d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
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
Cyc_Interface_final(); extern struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae); struct _tuple3{ struct _tagged_string f1; struct
_tagged_string f2; } ; extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info); extern struct Cyc_Interface_I*
Cyc_Interface_merge( struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2,
struct _tuple3* info); extern struct Cyc_Interface_I* Cyc_Interface_merge_list(
struct Cyc_List_List* li, struct Cyc_List_List* linfo); extern void Cyc_Interface_print(
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
extern char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{ char* tag; void*
f1; } ; extern char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{ char* tag;
short f1; } ; extern char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ char*
tag; struct Cyc_Absyn_Stmt* f1; } ; extern char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_String_tok[
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
int Cyc_String_strptrcmp( struct _tagged_string* s1, struct _tagged_string* s2);
extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*);
extern char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_is_function_type( void* t); extern char Cyc_Tcdecl_Incompatible[ 17u];
struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base; struct
Cyc_Absyn_Tunionfield* field; } ; typedef struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_xtunionfielddecl_t; extern void Cyc_Tcdecl_merr( struct Cyc_Position_Segment*
loc, struct _tagged_string* msg0, struct _tagged_string msg1); extern struct Cyc_Absyn_Structdecl*
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
loc, struct _tagged_string* msg); extern struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl* d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1,
struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern void Cyc_Lex_lex_init();
typedef struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Interface_xtunionfielddecl_t;
struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict* vardecls; struct Cyc_Dict_Dict*
xtunionfielddecls; } ; typedef struct Cyc_Interface_Ienv* Cyc_Interface_ienv_t;
struct Cyc_Interface_I{ struct Cyc_Dict_Dict* imports; struct Cyc_Dict_Dict*
exports; } ; static struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){ return({
struct Cyc_Interface_Ienv* _temp0=( struct Cyc_Interface_Ienv*) GC_malloc(
sizeof( struct Cyc_Interface_Ienv)); _temp0->structdecls=(( struct Cyc_Dict_Dict*(*)(
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
struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});} static
struct Cyc_Interface_Ienv* Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv*
ie= 0; if( ie == 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();}
return( struct Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp1= ie; if(
_temp1 == 0){ _throw( Null_Exception);} _temp1;});} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)((
struct Cyc_Interface_Ienv*(*)( void* ignored)) Cyc_Interface_skel2i_f, skel);
_temp2->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(* f)(
void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct Cyc_Interface_Ienv*(*)(
void* ignored)) Cyc_Interface_skel2i_f, skel); _temp2;});} struct Cyc_Interface_I*
Cyc_Interface_empty(){ return({ struct Cyc_Interface_I* _temp3=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp3->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp3->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp3;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
i= Cyc_Interface_empty(); struct Cyc_Interface_Ienv* ie1= Cyc_Interface_new_ienv();
struct Cyc_Absyn_Tuniondecl* tud=({ struct Cyc_Absyn_Tuniondecl* _temp14=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp14[ 0]=* Cyc_Absyn_exn_xed; _temp14;});( void*)( tud->sc=( void*)(( void*)
Cyc_Absyn_Public)); ie1->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
ie1->tuniondecls,(* Cyc_Absyn_exn_name).f2, tud); i->exports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)( i->exports, 0, ie1);{ struct Cyc_Interface_Ienv* ie2=
Cyc_Interface_new_ienv(); struct _tuple0* qmain=({ struct _tuple0* _temp8=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp8->f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp12=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp12[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp13; _temp13.tag= Cyc_Absyn_Abs_n; _temp13.f1= 0; _temp13;}); _temp12;});
_temp8->f2=({ struct _tagged_string* _temp9=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp9[ 0]=( struct _tagged_string)({ char*
_temp10=( char*)"main"; struct _tagged_string _temp11; _temp11.curr= _temp10;
_temp11.base= _temp10; _temp11.last_plus_one= _temp10 + 5; _temp11;}); _temp9;});
_temp8;}); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( qmain, Cyc_Absyn_function_typ(
0, 0, Cyc_Absyn_sint_t,({ struct Cyc_List_List* _temp4=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4->hd=( void*)({ struct _tuple1*
_temp7=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7->f1= 0;
_temp7->f2= Cyc_Absyn_empty_tqual(); _temp7->f3= Cyc_Absyn_uint_t; _temp7;});
_temp4->tl=({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp5->hd=( void*)({ struct _tuple1* _temp6=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6->f1= 0; _temp6->f2=
Cyc_Absyn_empty_tqual(); _temp6->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ(
Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*)
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp6;}); _temp5->tl= 0; _temp5;});
_temp4;}), 0, 0), 0);( void*)( vd->sc=( void*)(( void*) Cyc_Absyn_Extern)); ie2->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( ie2->vardecls,(* qmain).f2,
vd); i->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)( i->exports, 0, ie2);
return i;}} static void Cyc_Interface_err( struct _tagged_string msg){ Cyc_Tcutil_terr(
0, msg);} static void Cyc_Interface_fields_err( struct _tagged_string sc, struct
_tagged_string t, struct _tuple0* n){ Cyc_Interface_err(({ struct _tagged_string
_temp15= sc; struct _tagged_string _temp16= t; struct _tagged_string _temp17=
Cyc_Absynpp_qvar2string( n); xprintf("fields of %.*s %.*s %.*s have never been defined",
_temp15.last_plus_one - _temp15.curr, _temp15.curr, _temp16.last_plus_one -
_temp16.curr, _temp16.curr, _temp17.last_plus_one - _temp17.curr, _temp17.curr);}));}
static void Cyc_Interface_body_err( struct _tagged_string sc, struct _tuple0* n){
Cyc_Interface_err(({ struct _tagged_string _temp18= sc; struct _tagged_string
_temp19= Cyc_Absynpp_qvar2string( n); xprintf("the body of %.*s function %.*s has never been defined",
_temp18.last_plus_one - _temp18.curr, _temp18.curr, _temp19.last_plus_one -
_temp19.curr, _temp19.curr);}));} static void Cyc_Interface_static_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp20= obj1;
struct _tagged_string _temp21= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp22= obj2; struct _tagged_string _temp23= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s", _temp20.last_plus_one
- _temp20.curr, _temp20.curr, _temp21.last_plus_one - _temp21.curr, _temp21.curr,
_temp22.last_plus_one - _temp22.curr, _temp22.curr, _temp23.last_plus_one -
_temp23.curr, _temp23.curr);}));} static void Cyc_Interface_abstract_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp24= obj1;
struct _tagged_string _temp25= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp26= obj2; struct _tagged_string _temp27= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp24.last_plus_one - _temp24.curr, _temp24.curr, _temp25.last_plus_one -
_temp25.curr, _temp25.curr, _temp26.last_plus_one - _temp26.curr, _temp26.curr,
_temp27.last_plus_one - _temp27.curr, _temp27.curr);}));} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; typedef struct Cyc_Interface_Seen* Cyc_Interface_seen_t; static
struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({ struct Cyc_Interface_Seen*
_temp28=( struct Cyc_Interface_Seen*) GC_malloc( sizeof( struct Cyc_Interface_Seen));
_temp28->structs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp28->unions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp28->tunions=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp28;});} typedef struct Cyc_Dict_Dict* Cyc_Interface_aenv_t; static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int res=
1; for( 0; l != 0; l=({ struct Cyc_List_List* _temp29= l; if( _temp29 == 0){
_throw( Null_Exception);} _temp29->tl;})){ if( ! Cyc_Interface_check_public_type(
ae, seen, obj, name, f(( void*)({ struct Cyc_List_List* _temp30= l; if( _temp30
== 0){ _throw( Null_Exception);} _temp30->hd;})))){ res= 0;}} return res;}
static int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp31; _push_handler(& _temp31);{ int _temp33= 0; if( setjmp(
_temp31.handler)){ _temp33= 1;} if( ! _temp33){{ int _temp35=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)({ struct Cyc_Core_Opt* _temp34= d->name; if( _temp34 == 0){ _throw(
Null_Exception);} _temp34->v;})); _npop_handler( 0u); return _temp35;};
_pop_handler();} else{ void* _temp32=( void*) _exn_thrown; void* _temp37=
_temp32; _LL39: if( _temp37 == Cyc_Dict_Absent){ goto _LL40;} else{ goto _LL41;}
_LL41: goto _LL42; _LL40: { int res= 1; seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp43= d->name; if( _temp43 == 0){
_throw( Null_Exception);} _temp43->v;}), res); if( d->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp49= d->fields; if(
_temp49 == 0){ _throw( Null_Exception);} _temp49->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp44= fs; if( _temp44 == 0){ _throw( Null_Exception);}
_temp44->tl;})){ if( ! Cyc_Interface_check_public_type( ae, seen,( struct
_tagged_string)({ char* _temp45=( char*)"struct"; struct _tagged_string _temp46;
_temp46.curr= _temp45; _temp46.base= _temp45; _temp46.last_plus_one= _temp45 + 7;
_temp46;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp47= d->name; if(
_temp47 == 0){ _throw( Null_Exception);} _temp47->v;}),( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp48= fs; if( _temp48 == 0){ _throw( Null_Exception);}
_temp48->hd;}))->type)){ res= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp50= d->name; if( _temp50 == 0){
_throw( Null_Exception);} _temp50->v;}), res); return res;} _LL42:( void) _throw(
_temp37); _LL38:;}}} static int Cyc_Interface_check_public_uniondecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl* d){ struct
_handler_cons _temp51; _push_handler(& _temp51);{ int _temp53= 0; if( setjmp(
_temp51.handler)){ _temp53= 1;} if( ! _temp53){{ int _temp55=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,( struct
_tuple0*)({ struct Cyc_Core_Opt* _temp54= d->name; if( _temp54 == 0){ _throw(
Null_Exception);} _temp54->v;})); _npop_handler( 0u); return _temp55;};
_pop_handler();} else{ void* _temp52=( void*) _exn_thrown; void* _temp57=
_temp52; _LL59: if( _temp57 == Cyc_Dict_Absent){ goto _LL60;} else{ goto _LL61;}
_LL61: goto _LL62; _LL60: { int res= 1; seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp63= d->name; if( _temp63 == 0){
_throw( Null_Exception);} _temp63->v;}), res); if( d->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp69= d->fields; if(
_temp69 == 0){ _throw( Null_Exception);} _temp69->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp64= fs; if( _temp64 == 0){ _throw( Null_Exception);}
_temp64->tl;})){ if( ! Cyc_Interface_check_public_type( ae, seen,( struct
_tagged_string)({ char* _temp65=( char*)"union"; struct _tagged_string _temp66;
_temp66.curr= _temp65; _temp66.base= _temp65; _temp66.last_plus_one= _temp65 + 6;
_temp66;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp67= d->name; if(
_temp67 == 0){ _throw( Null_Exception);} _temp67->v;}),( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp68= fs; if( _temp68 == 0){ _throw( Null_Exception);}
_temp68->hd;}))->type)){ res= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp70= d->name; if( _temp70 == 0){
_throw( Null_Exception);} _temp70->v;}), res); return res;} _LL62:( void) _throw(
_temp57); _LL58:;}}} struct _tuple10{ struct Cyc_Absyn_Tqual f1; void* f2; } ;
static void* Cyc_Interface_get_type1( struct _tuple10* x){ return(* x).f2;}
static int Cyc_Interface_check_public_tuniondecl( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl* d){ struct
_handler_cons _temp71; _push_handler(& _temp71);{ int _temp73= 0; if( setjmp(
_temp71.handler)){ _temp73= 1;} if( ! _temp73){{ int _temp74=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions, d->name);
_npop_handler( 0u); return _temp74;}; _pop_handler();} else{ void* _temp72=(
void*) _exn_thrown; void* _temp76= _temp72; _LL78: if( _temp76 == Cyc_Dict_Absent){
goto _LL79;} else{ goto _LL80;} _LL80: goto _LL81; _LL79: { int res= 1; seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, res); if( d->fields != 0){ struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp86= d->fields; if(
_temp86 == 0){ _throw( Null_Exception);} _temp86->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp82= fs; if( _temp82 == 0){ _throw( Null_Exception);}
_temp82->tl;})){ if( !(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( struct
_tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae,
seen,( struct _tagged_string)({ char* _temp83=( char*)"[x]tunion"; struct
_tagged_string _temp84; _temp84.curr= _temp83; _temp84.base= _temp83; _temp84.last_plus_one=
_temp83 + 10; _temp84;}), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp85= fs; if( _temp85 == 0){ _throw( Null_Exception);}
_temp85->hd;}))->typs)){ res= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions,
d->name, res); return res;} _LL81:( void) _throw( _temp76); _LL77:;}}} static
int Cyc_Interface_check_public_enumdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Enumdecl* d){ return 1;} static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Typedefdecl*
d){ return Cyc_Interface_check_public_type( ae, seen,( struct _tagged_string)({
char* _temp87=( char*)"typedef"; struct _tagged_string _temp88; _temp88.curr=
_temp87; _temp88.base= _temp87; _temp88.last_plus_one= _temp87 + 8; _temp88;}),
d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen,( struct _tagged_string)({
char* _temp89=( char*)"variable"; struct _tagged_string _temp90; _temp90.curr=
_temp89; _temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 9; _temp90;}),
d->name,( void*) d->type);} static void* Cyc_Interface_get_type2( struct _tuple1*
x){ return(* x).f3;} static struct Cyc_List_List* Cyc_Interface_get_abs_ns( void*
ns){ void* _temp91= ns; struct Cyc_List_List* _temp97; _LL93: if(( unsigned int)
_temp91 > 1u?*(( int*) _temp91) == Cyc_Absyn_Abs_n: 0){ _LL98: _temp97=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp91)->f1; goto _LL94;}
else{ goto _LL95;} _LL95: goto _LL96; _LL94: return _temp97; _LL96:( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp99=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp99[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp100; _temp100.tag= Cyc_Core_InvalidArg; _temp100.f1=(
struct _tagged_string)({ char* _temp101=( char*)"get_abs_ns"; struct
_tagged_string _temp102; _temp102.curr= _temp101; _temp102.base= _temp101;
_temp102.last_plus_one= _temp101 + 11; _temp102;}); _temp100;}); _temp99;}));
_LL92:;} static int Cyc_Interface_check_public_type( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void* t){ void* _temp103= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp125; struct Cyc_Absyn_Conref* _temp127; struct Cyc_Absyn_Tqual _temp129;
struct Cyc_Absyn_Conref* _temp131; void* _temp133; void* _temp135; struct Cyc_Absyn_Exp*
_temp137; struct Cyc_Absyn_Tqual _temp139; void* _temp141; struct Cyc_Core_Opt*
_temp143; struct Cyc_Core_Opt _temp145; void* _temp146; struct Cyc_List_List*
_temp148; struct _tuple0* _temp150; struct Cyc_Absyn_FnInfo _temp152; struct Cyc_List_List*
_temp154; int _temp156; struct Cyc_List_List* _temp158; void* _temp160; struct
Cyc_Core_Opt* _temp162; struct Cyc_List_List* _temp164; struct Cyc_List_List*
_temp166; struct Cyc_Absyn_Structdecl** _temp168; struct Cyc_Absyn_Structdecl*
_temp170; struct Cyc_List_List* _temp171; struct _tuple0* _temp173; struct Cyc_Absyn_Uniondecl**
_temp175; struct Cyc_Absyn_Uniondecl* _temp177; struct Cyc_List_List* _temp178;
struct _tuple0* _temp180; struct Cyc_Absyn_TunionInfo _temp182; void* _temp184;
struct Cyc_List_List* _temp186; void* _temp188; struct Cyc_Absyn_Tuniondecl*
_temp190; struct Cyc_Absyn_TunionFieldInfo _temp192; struct Cyc_List_List*
_temp194; void* _temp196; struct Cyc_Absyn_Tunionfield* _temp198; struct Cyc_Absyn_Tuniondecl*
_temp200; _LL105: if(( unsigned int) _temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_PointerType:
0){ _LL126: _temp125=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp103)->f1; _LL136: _temp135=( void*) _temp125.elt_typ; goto _LL134; _LL134:
_temp133=( void*) _temp125.rgn_typ; goto _LL132; _LL132: _temp131=( struct Cyc_Absyn_Conref*)
_temp125.nullable; goto _LL130; _LL130: _temp129=( struct Cyc_Absyn_Tqual)
_temp125.tq; goto _LL128; _LL128: _temp127=( struct Cyc_Absyn_Conref*) _temp125.bounds;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 > 4u?*((
int*) _temp103) == Cyc_Absyn_ArrayType: 0){ _LL142: _temp141=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp103)->f1; goto _LL140; _LL140: _temp139=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp103)->f2;
goto _LL138; _LL138: _temp137=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp103)->f3; goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int)
_temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_TypedefType: 0){ _LL151: _temp150=(
struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp103)->f1; goto
_LL149; _LL149: _temp148=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp103)->f2; goto _LL144; _LL144: _temp143=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp103)->f3; if( _temp143 == 0){ goto _LL111;}
else{ _temp145=* _temp143; _LL147: _temp146=( void*) _temp145.v; goto _LL110;}}
else{ goto _LL111;} _LL111: if(( unsigned int) _temp103 > 4u?*(( int*) _temp103)
== Cyc_Absyn_FnType: 0){ _LL153: _temp152=( struct Cyc_Absyn_FnInfo)(( struct
Cyc_Absyn_FnType_struct*) _temp103)->f1; _LL165: _temp164=( struct Cyc_List_List*)
_temp152.tvars; goto _LL163; _LL163: _temp162=( struct Cyc_Core_Opt*) _temp152.effect;
goto _LL161; _LL161: _temp160=( void*) _temp152.ret_typ; goto _LL159; _LL159:
_temp158=( struct Cyc_List_List*) _temp152.args; goto _LL157; _LL157: _temp156=(
int) _temp152.varargs; goto _LL155; _LL155: _temp154=( struct Cyc_List_List*)
_temp152.attributes; goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int)
_temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_TupleType: 0){ _LL167: _temp166=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp103)->f1; goto
_LL114;} else{ goto _LL115;} _LL115: if(( unsigned int) _temp103 > 4u?*(( int*)
_temp103) == Cyc_Absyn_StructType: 0){ _LL174: _temp173=( struct _tuple0*)((
struct Cyc_Absyn_StructType_struct*) _temp103)->f1; goto _LL172; _LL172:
_temp171=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp103)->f2; goto _LL169; _LL169: _temp168=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp103)->f3; if( _temp168 == 0){ goto
_LL117;} else{ _temp170=* _temp168; goto _LL116;}} else{ goto _LL117;} _LL117:
if(( unsigned int) _temp103 > 4u?*(( int*) _temp103) == Cyc_Absyn_UnionType: 0){
_LL181: _temp180=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp103)->f1; goto _LL179; _LL179: _temp178=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp103)->f2; goto _LL176; _LL176: _temp175=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp103)->f3;
if( _temp175 == 0){ goto _LL119;} else{ _temp177=* _temp175; goto _LL118;}}
else{ goto _LL119;} _LL119: if(( unsigned int) _temp103 > 4u?*(( int*) _temp103)
== Cyc_Absyn_TunionType: 0){ _LL183: _temp182=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp103)->f1; _LL189: _temp188=( void*)
_temp182.tunion_info; if(*(( int*) _temp188) == Cyc_Absyn_KnownTunion){ _LL191:
_temp190=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp188)->f1; goto _LL187;} else{ goto _LL121;} _LL187: _temp186=( struct Cyc_List_List*)
_temp182.targs; goto _LL185; _LL185: _temp184=( void*) _temp182.rgn; goto _LL120;}
else{ goto _LL121;} _LL121: if(( unsigned int) _temp103 > 4u?*(( int*) _temp103)
== Cyc_Absyn_TunionFieldType: 0){ _LL193: _temp192=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp103)->f1; _LL197: _temp196=( void*)
_temp192.field_info; if(*(( int*) _temp196) == Cyc_Absyn_KnownTunionfield){
_LL201: _temp200=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp196)->f1; goto _LL199; _LL199: _temp198=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp196)->f2; goto _LL195;} else{
goto _LL123;} _LL195: _temp194=( struct Cyc_List_List*) _temp192.targs; goto
_LL122;} else{ goto _LL123;} _LL123: goto _LL124; _LL106: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp135); _LL108: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp141); _LL110: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp146); _LL112: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp158)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp160): 0; _LL114: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp166); _LL116: if(( void*)
_temp170->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,(
struct _tagged_string)({ char* _temp202=( char*)"struct"; struct _tagged_string
_temp203; _temp203.curr= _temp202; _temp203.base= _temp202; _temp203.last_plus_one=
_temp202 + 7; _temp203;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp204=
_temp170->name; if( _temp204 == 0){ _throw( Null_Exception);} _temp204->v;}));
return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj,
name,( void*(*)( void*)) Cyc_Core_identity, _temp171)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp170): 0; _LL118: if(( void*) _temp177->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp205=(
char*)"union"; struct _tagged_string _temp206; _temp206.curr= _temp205; _temp206.base=
_temp205; _temp206.last_plus_one= _temp205 + 6; _temp206;}),( struct _tuple0*)({
struct Cyc_Core_Opt* _temp207= _temp177->name; if( _temp207 == 0){ _throw(
Null_Exception);} _temp207->v;})); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp178)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp177): 0; _LL120: { struct _tuple0 _temp210; struct _tagged_string*
_temp211; void* _temp213; struct _tuple0* _temp208= _temp190->name; _temp210=*
_temp208; _LL214: _temp213= _temp210.f1; goto _LL212; _LL212: _temp211= _temp210.f2;
goto _LL209; _LL209: { struct Cyc_List_List* ns= Cyc_Interface_get_abs_ns(
_temp213); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp215;
_push_handler(& _temp215);{ int _temp217= 0; if( setjmp( _temp215.handler)){
_temp217= 1;} if( ! _temp217){{ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae, ns);
tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp211);};
_pop_handler();} else{ void* _temp216=( void*) _exn_thrown; void* _temp219=
_temp216; _LL221: if( _temp219 == Cyc_Dict_Absent){ goto _LL222;} else{ goto
_LL223;} _LL223: goto _LL224; _LL222:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp225=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp225[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp226; _temp226.tag= Cyc_Core_InvalidArg;
_temp226.f1=( struct _tagged_string)({ char* _temp227=( char*)"check_public_type";
struct _tagged_string _temp228; _temp228.curr= _temp227; _temp228.base= _temp227;
_temp228.last_plus_one= _temp227 + 18; _temp228;}); _temp226;}); _temp225;}));
_LL224:( void) _throw( _temp219); _LL220:;}}} if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp229=(
char*)"[x]tunion"; struct _tagged_string _temp230; _temp230.curr= _temp229;
_temp230.base= _temp229; _temp230.last_plus_one= _temp229 + 10; _temp230;}), tud->name);
return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj,
name,( void*(*)( void*)) Cyc_Core_identity, _temp186)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}} _LL122: { struct _tuple0 _temp233; struct _tagged_string*
_temp234; void* _temp236; struct _tuple0* _temp231= _temp200->name; _temp233=*
_temp231; _LL237: _temp236= _temp233.f1; goto _LL235; _LL235: _temp234= _temp233.f2;
goto _LL232; _LL232: { struct Cyc_List_List* ns= Cyc_Interface_get_abs_ns(
_temp236); struct Cyc_Absyn_Tuniondecl* tud;{ struct _handler_cons _temp238;
_push_handler(& _temp238);{ int _temp240= 0; if( setjmp( _temp238.handler)){
_temp240= 1;} if( ! _temp240){{ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( ae, ns);
tud=*(( struct Cyc_Absyn_Tuniondecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls, _temp234);};
_pop_handler();} else{ void* _temp239=( void*) _exn_thrown; void* _temp242=
_temp239; _LL244: if( _temp242 == Cyc_Dict_Absent){ goto _LL245;} else{ goto
_LL246;} _LL246: goto _LL247; _LL245:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp248=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp248[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp249; _temp249.tag= Cyc_Core_InvalidArg;
_temp249.f1=( struct _tagged_string)({ char* _temp250=( char*)"check_public_type";
struct _tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 18; _temp251;}); _temp249;}); _temp248;}));
_LL247:( void) _throw( _temp242); _LL243:;}}} if( tud->fields == 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp252=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp252[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp253; _temp253.tag= Cyc_Core_InvalidArg; _temp253.f1=(
struct _tagged_string)({ char* _temp254=( char*)"check_public_type"; struct
_tagged_string _temp255; _temp255.curr= _temp254; _temp255.base= _temp254;
_temp255.last_plus_one= _temp254 + 18; _temp255;}); _temp253;}); _temp252;}));}{
struct Cyc_Absyn_Tunionfield* tuf1= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp259= tud->fields; if( _temp259 == 0){ _throw(
Null_Exception);} _temp259->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp256= fs; if( _temp256 == 0){ _throw( Null_Exception);} _temp256->tl;})){
int cmp; if(( cmp= Cyc_Absyn_qvar_cmp( _temp198->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp257= fs; if( _temp257 == 0){ _throw( Null_Exception);}
_temp257->hd;}))->name)) >= 0){ if( cmp == 0){ tuf1=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp258= fs; if(
_temp258 == 0){ _throw( Null_Exception);} _temp258->hd;}));} break;}}} if( tuf1
== 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp260=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp260[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp261; _temp261.tag= Cyc_Core_InvalidArg;
_temp261.f1=( struct _tagged_string)({ char* _temp262=( char*)"check_public_type";
struct _tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 18; _temp263;}); _temp261;}); _temp260;}));}{
struct Cyc_Absyn_Tunionfield* tuf=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_Absyn_Tunionfield*
_temp269= tuf1; if( _temp269 == 0){ _throw( Null_Exception);} _temp269;}); if((
void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,(
struct _tagged_string)({ char* _temp264=( char*)"[x]tunion"; struct
_tagged_string _temp265; _temp265.curr= _temp264; _temp265.base= _temp264;
_temp265.last_plus_one= _temp264 + 10; _temp265;}), tud->name); return 0;} if((
void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err( obj,
name,( struct _tagged_string)({ char* _temp266=( char*)"[x]tunion"; struct
_tagged_string _temp267; _temp267.curr= _temp266; _temp267.base= _temp266;
_temp267.last_plus_one= _temp266 + 10; _temp267;}), tud->name); return 0;} if((
void*) tuf->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({
struct _tagged_string _temp268= Cyc_Absynpp_qvar2string( tuf->name); xprintf("field %.*s of",
_temp268.last_plus_one - _temp268.curr, _temp268.curr);}), tud->name); return 0;}
return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_type_list)( ae, seen, obj, name,( void*(*)( void*))
Cyc_Core_identity, _temp194)? Cyc_Interface_check_public_tuniondecl( ae, seen,
tud): 0;}}}} _LL124: return 1; _LL104:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp272; struct Cyc_Interface_I* _temp273; struct Cyc_Interface_Seen*
_temp275; struct Cyc_Dict_Dict* _temp277; int _temp279; struct Cyc_Interface_Ienv*
_temp281; struct Cyc_Interface_Ienv* _temp283; struct _tuple11* _temp270= env;
_temp272=* _temp270; _LL284: _temp283= _temp272.f1; goto _LL282; _LL282:
_temp281= _temp272.f2; goto _LL280; _LL280: _temp279= _temp272.f3; goto _LL278;
_LL278: _temp277= _temp272.f4; goto _LL276; _LL276: _temp275= _temp272.f5; goto
_LL274; _LL274: _temp273= _temp272.f6; goto _LL271; _LL271: { struct Cyc_Absyn_Structdecl*
d=* dp; void* _temp285=( void*) d->sc; _LL287: if( _temp285 ==( void*) Cyc_Absyn_Static){
goto _LL288;} else{ goto _LL289;} _LL289: if( _temp285 ==( void*) Cyc_Absyn_Abstract){
goto _LL290;} else{ goto _LL291;} _LL291: if( _temp285 ==( void*) Cyc_Absyn_Public){
goto _LL292;} else{ goto _LL293;} _LL293: if( _temp285 ==( void*) Cyc_Absyn_ExternC){
goto _LL294;} else{ goto _LL295;} _LL295: if( _temp285 ==( void*) Cyc_Absyn_Extern){
goto _LL296;} else{ goto _LL286;} _LL288: if( _temp279? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp297=( char*)"static"; struct _tagged_string
_temp298; _temp298.curr= _temp297; _temp298.base= _temp297; _temp298.last_plus_one=
_temp297 + 7; _temp298;}),( struct _tagged_string)({ char* _temp299=( char*)"struct";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 7; _temp300;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp301= d->name; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301->v;}));} goto _LL286; _LL290: if( d->fields == 0){ if( _temp279){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp302=( char*)"abstract"; struct
_tagged_string _temp303; _temp303.curr= _temp302; _temp303.base= _temp302;
_temp303.last_plus_one= _temp302 + 9; _temp303;}),( struct _tagged_string)({
char* _temp304=( char*)"struct"; struct _tagged_string _temp305; _temp305.curr=
_temp304; _temp305.base= _temp304; _temp305.last_plus_one= _temp304 + 7;
_temp305;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp306= d->name; if(
_temp306 == 0){ _throw( Null_Exception);} _temp306->v;}));}} else{ d=({ struct
Cyc_Absyn_Structdecl* _temp307=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp307[ 0]=* d; _temp307;}); d->fields=
0;} if( Cyc_Interface_check_public_structdecl( _temp277, _temp275, d)){ _temp281->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( _temp281->structdecls, x,
d);} goto _LL286; _LL292: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp308=( char*)"public"; struct _tagged_string
_temp309; _temp309.curr= _temp308; _temp309.base= _temp308; _temp309.last_plus_one=
_temp308 + 7; _temp309;}),( struct _tagged_string)({ char* _temp310=( char*)"struct";
struct _tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 7; _temp311;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp312= d->name; if( _temp312 == 0){ _throw( Null_Exception);}
_temp312->v;})); d=({ struct Cyc_Absyn_Structdecl* _temp313=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp313[ 0]=* d; _temp313;});(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl(
_temp277, _temp275, d)){ _temp281->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp281->structdecls, x, d);} goto _LL286; _LL294:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp314=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp314[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp315; _temp315.tag= Cyc_Core_InvalidArg; _temp315.f1=(
struct _tagged_string)({ char* _temp316=( char*)"extract_structdecl"; struct
_tagged_string _temp317; _temp317.curr= _temp316; _temp317.base= _temp316;
_temp317.last_plus_one= _temp316 + 19; _temp317;}); _temp315;}); _temp314;}));
_LL296: if( Cyc_Interface_check_public_structdecl( _temp277, _temp275, d)){
_temp283->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp283->structdecls, x, d);} goto _LL286; _LL286:;}} static void Cyc_Interface_extract_uniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple11 _temp320; struct Cyc_Interface_I* _temp321; struct Cyc_Interface_Seen*
_temp323; struct Cyc_Dict_Dict* _temp325; int _temp327; struct Cyc_Interface_Ienv*
_temp329; struct Cyc_Interface_Ienv* _temp331; struct _tuple11* _temp318= env;
_temp320=* _temp318; _LL332: _temp331= _temp320.f1; goto _LL330; _LL330:
_temp329= _temp320.f2; goto _LL328; _LL328: _temp327= _temp320.f3; goto _LL326;
_LL326: _temp325= _temp320.f4; goto _LL324; _LL324: _temp323= _temp320.f5; goto
_LL322; _LL322: _temp321= _temp320.f6; goto _LL319; _LL319: { struct Cyc_Absyn_Uniondecl*
d=* dp; void* _temp333=( void*) d->sc; _LL335: if( _temp333 ==( void*) Cyc_Absyn_Static){
goto _LL336;} else{ goto _LL337;} _LL337: if( _temp333 ==( void*) Cyc_Absyn_Abstract){
goto _LL338;} else{ goto _LL339;} _LL339: if( _temp333 ==( void*) Cyc_Absyn_Public){
goto _LL340;} else{ goto _LL341;} _LL341: if( _temp333 ==( void*) Cyc_Absyn_ExternC){
goto _LL342;} else{ goto _LL343;} _LL343: if( _temp333 ==( void*) Cyc_Absyn_Extern){
goto _LL344;} else{ goto _LL334;} _LL336: if( _temp327? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp345=( char*)"static"; struct _tagged_string
_temp346; _temp346.curr= _temp345; _temp346.base= _temp345; _temp346.last_plus_one=
_temp345 + 7; _temp346;}),( struct _tagged_string)({ char* _temp347=( char*)"union";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 6; _temp348;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp349= d->name; if( _temp349 == 0){ _throw( Null_Exception);}
_temp349->v;}));} goto _LL334; _LL338: if( d->fields == 0){ if( _temp327){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp350=( char*)"abstract"; struct
_tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 9; _temp351;}),( struct _tagged_string)({
char* _temp352=( char*)"union"; struct _tagged_string _temp353; _temp353.curr=
_temp352; _temp353.base= _temp352; _temp353.last_plus_one= _temp352 + 6;
_temp353;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp354= d->name; if(
_temp354 == 0){ _throw( Null_Exception);} _temp354->v;}));}} else{ d=({ struct
Cyc_Absyn_Uniondecl* _temp355=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Uniondecl)); _temp355[ 0]=* d; _temp355;}); d->fields= 0;} if(
Cyc_Interface_check_public_uniondecl( _temp325, _temp323, d)){ _temp329->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp329->uniondecls, x, d);}
goto _LL334; _LL340: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp356=( char*)"public"; struct _tagged_string
_temp357; _temp357.curr= _temp356; _temp357.base= _temp356; _temp357.last_plus_one=
_temp356 + 7; _temp357;}),( struct _tagged_string)({ char* _temp358=( char*)"union";
struct _tagged_string _temp359; _temp359.curr= _temp358; _temp359.base= _temp358;
_temp359.last_plus_one= _temp358 + 6; _temp359;}),( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp360= d->name; if( _temp360 == 0){ _throw( Null_Exception);}
_temp360->v;})); d=({ struct Cyc_Absyn_Uniondecl* _temp361=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp361[ 0]=* d; _temp361;});(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl(
_temp325, _temp323, d)){ _temp329->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp329->uniondecls, x, d);} goto _LL334; _LL342:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp362=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp362[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp363; _temp363.tag= Cyc_Core_InvalidArg; _temp363.f1=(
struct _tagged_string)({ char* _temp364=( char*)"extract_uniondecl"; struct
_tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 18; _temp365;}); _temp363;}); _temp362;}));
_LL344: if( Cyc_Interface_check_public_uniondecl( _temp325, _temp323, d)){
_temp331->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp331->uniondecls, x, d);} goto _LL334; _LL334:;}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp368; struct Cyc_Interface_I* _temp369; struct Cyc_Interface_Seen*
_temp371; struct Cyc_Dict_Dict* _temp373; int _temp375; struct Cyc_Interface_Ienv*
_temp377; struct Cyc_Interface_Ienv* _temp379; struct _tuple11* _temp366= env;
_temp368=* _temp366; _LL380: _temp379= _temp368.f1; goto _LL378; _LL378:
_temp377= _temp368.f2; goto _LL376; _LL376: _temp375= _temp368.f3; goto _LL374;
_LL374: _temp373= _temp368.f4; goto _LL372; _LL372: _temp371= _temp368.f5; goto
_LL370; _LL370: _temp369= _temp368.f6; goto _LL367; _LL367: { struct Cyc_Absyn_Enumdecl*
d=* dp; void* _temp381=( void*) d->sc; _LL383: if( _temp381 ==( void*) Cyc_Absyn_Static){
goto _LL384;} else{ goto _LL385;} _LL385: if( _temp381 ==( void*) Cyc_Absyn_Abstract){
goto _LL386;} else{ goto _LL387;} _LL387: if( _temp381 ==( void*) Cyc_Absyn_Public){
goto _LL388;} else{ goto _LL389;} _LL389: if( _temp381 ==( void*) Cyc_Absyn_ExternC){
goto _LL390;} else{ goto _LL391;} _LL391: if( _temp381 ==( void*) Cyc_Absyn_Extern){
goto _LL392;} else{ goto _LL382;} _LL384: if( _temp375? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp393=( char*)"static"; struct _tagged_string
_temp394; _temp394.curr= _temp393; _temp394.base= _temp393; _temp394.last_plus_one=
_temp393 + 7; _temp394;}),( struct _tagged_string)({ char* _temp395=( char*)"enum";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 5; _temp396;}), d->name);} goto _LL382;
_LL386: if( d->fields == 0){ if( _temp375){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp397=( char*)"abstract"; struct _tagged_string
_temp398; _temp398.curr= _temp397; _temp398.base= _temp397; _temp398.last_plus_one=
_temp397 + 9; _temp398;}),( struct _tagged_string)({ char* _temp399=( char*)"enum";
struct _tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 5; _temp400;}), d->name);}} else{ d=({ struct
Cyc_Absyn_Enumdecl* _temp401=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl)); _temp401[ 0]=* d; _temp401;}); d->fields= 0;} if(
Cyc_Interface_check_public_enumdecl( _temp373, _temp371, d)){ _temp377->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp377->enumdecls, x, d);}
goto _LL382; _LL388: if( d->fields == 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp402=( char*)"public"; struct _tagged_string
_temp403; _temp403.curr= _temp402; _temp403.base= _temp402; _temp403.last_plus_one=
_temp402 + 7; _temp403;}),( struct _tagged_string)({ char* _temp404=( char*)"enum";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 5; _temp405;}), d->name); d=({ struct Cyc_Absyn_Enumdecl*
_temp406=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp406[ 0]=* d; _temp406;});( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Abstract));}
if( Cyc_Interface_check_public_enumdecl( _temp373, _temp371, d)){ _temp377->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp377->enumdecls, x, d);}
goto _LL382; _LL390:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp407=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp407[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp408; _temp408.tag= Cyc_Core_InvalidArg;
_temp408.f1=( struct _tagged_string)({ char* _temp409=( char*)"extract_enumdecl";
struct _tagged_string _temp410; _temp410.curr= _temp409; _temp410.base= _temp409;
_temp410.last_plus_one= _temp409 + 17; _temp410;}); _temp408;}); _temp407;}));
_LL392: if( Cyc_Interface_check_public_enumdecl( _temp373, _temp371, d)){
_temp379->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(
_temp379->enumdecls, x, d);} goto _LL382; _LL382:;}} static void Cyc_Interface_extract_xtunionfielddecl(
struct Cyc_Interface_I* i, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield*
f){ struct Cyc_List_List* ns= 0;{ void* _temp411=(* f->name).f1; struct Cyc_List_List*
_temp419; struct Cyc_List_List* _temp421; _LL413: if(( unsigned int) _temp411 >
1u?*(( int*) _temp411) == Cyc_Absyn_Rel_n: 0){ _LL420: _temp419=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp411)->f1; goto _LL414;} else{ goto _LL415;}
_LL415: if(( unsigned int) _temp411 > 1u?*(( int*) _temp411) == Cyc_Absyn_Abs_n:
0){ _LL422: _temp421=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp411)->f1; goto _LL416;} else{ goto _LL417;} _LL417: goto _LL418; _LL414:
_temp421= _temp419; goto _LL416; _LL416: ns= _temp421; goto _LL412; _LL418: goto
_LL412; _LL412:;}{ struct Cyc_Dict_Dict** dict;{ void* _temp423=( void*) f->sc;
_LL425: if( _temp423 ==( void*) Cyc_Absyn_Static){ goto _LL426;} else{ goto
_LL427;} _LL427: if( _temp423 ==( void*) Cyc_Absyn_Extern){ goto _LL428;} else{
goto _LL429;} _LL429: if( _temp423 ==( void*) Cyc_Absyn_Public){ goto _LL430;}
else{ goto _LL431;} _LL431: goto _LL432; _LL426: return; _LL428: dict=& i->imports;
goto _LL424; _LL430: dict=& i->exports; goto _LL424; _LL432:( void) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp433=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp433[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp434; _temp434.tag= Cyc_Core_InvalidArg; _temp434.f1=(
struct _tagged_string)({ char* _temp435=( char*)"add_xtunionfielddecl"; struct
_tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 21; _temp436;}); _temp434;}); _temp433;}));
_LL424:;}{ struct Cyc_Interface_Ienv* env;{ struct _handler_cons _temp437;
_push_handler(& _temp437);{ int _temp439= 0; if( setjmp( _temp437.handler)){
_temp439= 1;} if( ! _temp439){ env=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict, ns);; _pop_handler();}
else{ void* _temp438=( void*) _exn_thrown; void* _temp441= _temp438; _LL443: if(
_temp441 == Cyc_Dict_Absent){ goto _LL444;} else{ goto _LL445;} _LL445: goto
_LL446; _LL444: env= Cyc_Interface_new_ienv();* dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv*
data)) Cyc_Dict_insert)(* dict, ns, env); goto _LL442; _LL446:( void) _throw(
_temp441); _LL442:;}}} env->xtunionfielddecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Tcdecl_Xtunionfielddecl*
data)) Cyc_Dict_insert)( env->xtunionfielddecls,(* f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl*
_temp447=( struct Cyc_Tcdecl_Xtunionfielddecl*) GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl));
_temp447->base= d; _temp447->field= f; _temp447;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp450; struct Cyc_Interface_I* _temp451; struct Cyc_Interface_Seen*
_temp453; struct Cyc_Dict_Dict* _temp455; int _temp457; struct Cyc_Interface_Ienv*
_temp459; struct Cyc_Interface_Ienv* _temp461; struct _tuple11* _temp448= env;
_temp450=* _temp448; _LL462: _temp461= _temp450.f1; goto _LL460; _LL460:
_temp459= _temp450.f2; goto _LL458; _LL458: _temp457= _temp450.f3; goto _LL456;
_LL456: _temp455= _temp450.f4; goto _LL454; _LL454: _temp453= _temp450.f5; goto
_LL452; _LL452: _temp451= _temp450.f6; goto _LL449; _LL449: { struct Cyc_Absyn_Tuniondecl*
d=* dp; void* _temp463=( void*) d->sc; _LL465: if( _temp463 ==( void*) Cyc_Absyn_Static){
goto _LL466;} else{ goto _LL467;} _LL467: if( _temp463 ==( void*) Cyc_Absyn_Abstract){
goto _LL468;} else{ goto _LL469;} _LL469: if( _temp463 ==( void*) Cyc_Absyn_Public){
goto _LL470;} else{ goto _LL471;} _LL471: if( _temp463 ==( void*) Cyc_Absyn_ExternC){
goto _LL472;} else{ goto _LL473;} _LL473: if( _temp463 ==( void*) Cyc_Absyn_Extern){
goto _LL474;} else{ goto _LL464;} _LL466: if(( ! d->is_xtunion? _temp457: 0)? d->fields
== 0: 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp475=(
char*)"static"; struct _tagged_string _temp476; _temp476.curr= _temp475;
_temp476.base= _temp475; _temp476.last_plus_one= _temp475 + 7; _temp476;}),(
struct _tagged_string)({ char* _temp477=( char*)"tunion"; struct _tagged_string
_temp478; _temp478.curr= _temp477; _temp478.base= _temp477; _temp478.last_plus_one=
_temp477 + 7; _temp478;}), d->name);} goto _LL464; _LL468: if( d->fields == 0){
if( ! d->is_xtunion? _temp457: 0){ Cyc_Interface_fields_err(( struct
_tagged_string)({ char* _temp479=( char*)"abstract"; struct _tagged_string
_temp480; _temp480.curr= _temp479; _temp480.base= _temp479; _temp480.last_plus_one=
_temp479 + 9; _temp480;}),( struct _tagged_string)({ char* _temp481=( char*)"tunion";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 7; _temp482;}), d->name);}} else{ d=({ struct
Cyc_Absyn_Tuniondecl* _temp483=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp483[ 0]=* d; _temp483;}); d->fields=
0;} if( Cyc_Interface_check_public_tuniondecl( _temp455, _temp453, d)){ _temp459->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp459->tuniondecls, x,
d);} goto _LL464; _LL470: d=({ struct Cyc_Absyn_Tuniondecl* _temp484=( struct
Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp484[ 0]=* d; _temp484;}); if( ! d->is_xtunion? d->fields == 0: 0){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp485=( char*)"public"; struct _tagged_string
_temp486; _temp486.curr= _temp485; _temp486.base= _temp485; _temp486.last_plus_one=
_temp485 + 7; _temp486;}),( struct _tagged_string)({ char* _temp487=( char*)"tunion";
struct _tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 7; _temp488;}), d->name);( void*)( d->sc=(
void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp455, _temp453, d)){ if( d->is_xtunion? d->fields != 0: 0){ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp491= d->fields; if(
_temp491 == 0){ _throw( Null_Exception);} _temp491->v;}); d->fields= 0;{ struct
Cyc_List_List* f= fields; for( 0; f != 0; f=({ struct Cyc_List_List* _temp489= f;
if( _temp489 == 0){ _throw( Null_Exception);} _temp489->tl;})){ Cyc_Interface_extract_xtunionfielddecl(
_temp451, d,( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp490= f;
if( _temp490 == 0){ _throw( Null_Exception);} _temp490->hd;}));}}} _temp459->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp459->tuniondecls, x,
d);} goto _LL464; _LL472:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp492=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp492[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp493; _temp493.tag= Cyc_Core_InvalidArg;
_temp493.f1=( struct _tagged_string)({ char* _temp494=( char*)"extract_tuniondecl";
struct _tagged_string _temp495; _temp495.curr= _temp494; _temp495.base= _temp494;
_temp495.last_plus_one= _temp494 + 19; _temp495;}); _temp493;}); _temp492;}));
_LL474: if( Cyc_Interface_check_public_tuniondecl( _temp455, _temp453, d)){ if(
d->is_xtunion? d->fields != 0: 0){ d=({ struct Cyc_Absyn_Tuniondecl* _temp496=(
struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp496[ 0]=* d; _temp496;});{ struct Cyc_List_List* fields=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp499= d->fields; if( _temp499 == 0){ _throw(
Null_Exception);} _temp499->v;}); d->fields= 0;{ struct Cyc_List_List* f= fields;
for( 0; f != 0; f=({ struct Cyc_List_List* _temp497= f; if( _temp497 == 0){
_throw( Null_Exception);} _temp497->tl;})){ Cyc_Interface_extract_xtunionfielddecl(
_temp451, d,( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp498= f;
if( _temp498 == 0){ _throw( Null_Exception);} _temp498->hd;}));}}}} _temp461->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp461->tuniondecls, x,
d);} goto _LL464; _LL464:;}} static void Cyc_Interface_extract_typedef( struct
_tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple11 _temp502; struct Cyc_Interface_I* _temp503; struct Cyc_Interface_Seen*
_temp505; struct Cyc_Dict_Dict* _temp507; int _temp509; struct Cyc_Interface_Ienv*
_temp511; struct Cyc_Interface_Ienv* _temp513; struct _tuple11* _temp500= env;
_temp502=* _temp500; _LL514: _temp513= _temp502.f1; goto _LL512; _LL512:
_temp511= _temp502.f2; goto _LL510; _LL510: _temp509= _temp502.f3; goto _LL508;
_LL508: _temp507= _temp502.f4; goto _LL506; _LL506: _temp505= _temp502.f5; goto
_LL504; _LL504: _temp503= _temp502.f6; goto _LL501; _LL501: if( Cyc_Interface_check_public_typedefdecl(
_temp507, _temp505, d)){ _temp511->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp511->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_string* x, struct _tuple7* v){ struct
_tuple11 _temp517; struct Cyc_Interface_I* _temp518; struct Cyc_Interface_Seen*
_temp520; struct Cyc_Dict_Dict* _temp522; int _temp524; struct Cyc_Interface_Ienv*
_temp526; struct Cyc_Interface_Ienv* _temp528; struct _tuple11* _temp515= env;
_temp517=* _temp515; _LL529: _temp528= _temp517.f1; goto _LL527; _LL527:
_temp526= _temp517.f2; goto _LL525; _LL525: _temp524= _temp517.f3; goto _LL523;
_LL523: _temp522= _temp517.f4; goto _LL521; _LL521: _temp520= _temp517.f5; goto
_LL519; _LL519: _temp518= _temp517.f6; goto _LL516; _LL516: { void* r=(* v).f1;
void* _temp530= r; void* _temp536; _LL532: if(*(( int*) _temp530) == Cyc_Tcenv_VarRes){
_LL537: _temp536=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp530)->f1; goto
_LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL533:{ void* _temp538=
_temp536; struct Cyc_Absyn_Fndecl* _temp546; struct Cyc_Absyn_Vardecl* _temp548;
_LL540: if(( unsigned int) _temp538 > 1u?*(( int*) _temp538) == Cyc_Absyn_Funname_b:
0){ _LL547: _temp546=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp538)->f1; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp538 > 1u?*(( int*) _temp538) == Cyc_Absyn_Global_b: 0){ _LL549: _temp548=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp538)->f1;
goto _LL543;} else{ goto _LL544;} _LL544: goto _LL545; _LL541: { struct Cyc_Absyn_Vardecl*
vd=({ struct Cyc_Absyn_Vardecl* _temp550=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl)); _temp550->sc=( void*)(( void*) _temp546->sc);
_temp550->name= _temp546->name; _temp550->tq=({ struct Cyc_Absyn_Tqual _temp552;
_temp552.q_const= 0; _temp552.q_volatile= 0; _temp552.q_restrict= 0; _temp552;});
_temp550->type=( void*)(( void*)({ struct Cyc_Core_Opt* _temp551= _temp546->cached_typ;
if( _temp551 == 0){ _throw( Null_Exception);} _temp551->v;})); _temp550->initializer=
0; _temp550->rgn= 0; _temp550->attributes= 0; _temp550;}); _temp524= 0; _temp548=
vd; goto _LL543;} _LL543: if( _temp548->initializer != 0){ _temp548=({ struct
Cyc_Absyn_Vardecl* _temp553=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof(
struct Cyc_Absyn_Vardecl)); _temp553[ 0]=* _temp548; _temp553;}); _temp548->initializer=
0;}{ void* _temp554=( void*) _temp548->sc; _LL556: if( _temp554 ==( void*) Cyc_Absyn_Static){
goto _LL557;} else{ goto _LL558;} _LL558: if( _temp554 ==( void*) Cyc_Absyn_Abstract){
goto _LL559;} else{ goto _LL560;} _LL560: if( _temp554 ==( void*) Cyc_Absyn_Public){
goto _LL561;} else{ goto _LL562;} _LL562: if( _temp554 ==( void*) Cyc_Absyn_ExternC){
goto _LL563;} else{ goto _LL564;} _LL564: if( _temp554 ==( void*) Cyc_Absyn_Extern){
goto _LL565;} else{ goto _LL555;} _LL557: if( _temp524? Cyc_Tcutil_is_function_type((
void*) _temp548->type): 0){ Cyc_Interface_body_err(( struct _tagged_string)({
char* _temp566=( char*)"static"; struct _tagged_string _temp567; _temp567.curr=
_temp566; _temp567.base= _temp566; _temp567.last_plus_one= _temp566 + 7;
_temp567;}), _temp548->name);} goto _LL555; _LL559:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp568=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp568[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp569; _temp569.tag= Cyc_Core_InvalidArg; _temp569.f1=(
struct _tagged_string)({ char* _temp570=( char*)"extract_ordinarie"; struct
_tagged_string _temp571; _temp571.curr= _temp570; _temp571.base= _temp570;
_temp571.last_plus_one= _temp570 + 18; _temp571;}); _temp569;}); _temp568;}));
_LL561: if( _temp524? Cyc_Tcutil_is_function_type(( void*) _temp548->type): 0){
Cyc_Interface_body_err(( struct _tagged_string)({ char* _temp572=( char*)"public";
struct _tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 7; _temp573;}), _temp548->name);} if( Cyc_Interface_check_public_vardecl(
_temp522, _temp520, _temp548)){ _temp526->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( _temp526->vardecls, x, _temp548);} goto _LL555; _LL563:
goto _LL565; _LL565: if( Cyc_Interface_check_public_vardecl( _temp522, _temp520,
_temp548)){ _temp528->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp528->vardecls, x, _temp548);} goto _LL555; _LL555:;} goto _LL539; _LL545:
goto _LL539; _LL539:;} goto _LL531; _LL535: goto _LL531; _LL531:;}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp576; struct Cyc_Interface_Seen* _temp577; struct Cyc_Dict_Dict* _temp579;
int _temp581; struct Cyc_Interface_I* _temp583; struct _tuple12* _temp574= env_f;
_temp576=* _temp574; _LL584: _temp583= _temp576.f1; goto _LL582; _LL582:
_temp581= _temp576.f2; goto _LL580; _LL580: _temp579= _temp576.f3; goto _LL578;
_LL578: _temp577= _temp576.f4; goto _LL575; _LL575: { struct _tuple11 env=({
struct _tuple11 _temp585; _temp585.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp583->imports,
ns); _temp585.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp583->exports, ns); _temp585.f3=
_temp581; _temp585.f4= _temp579; _temp585.f5= _temp577; _temp585.f6= _temp583;
_temp585;});(( void(*)( void(* f)( struct _tuple11*, struct _tagged_string*,
struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& env, ge->structdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Uniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,&
env, ge->uniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Tuniondecl**), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,& env, ge->tuniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Enumdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,&
env, ge->enumdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Typedefdecl*), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,& env, ge->typedefs);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct _tuple7*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,&
env, ge->ordinaries);}} static struct Cyc_Interface_I* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_complete_defs){ struct _tuple12 env=({
struct _tuple12 _temp586; _temp586.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict*
skel)) Cyc_Interface_skel2i)( ae); _temp586.f2= check_complete_defs; _temp586.f3=
ae; _temp586.f4= Cyc_Interface_new_seen(); _temp586;});(( void(*)( void(* f)(
struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct
_tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
env, ae); return env.f1;} struct Cyc_Interface_I* Cyc_Interface_extract( struct
Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline static
void Cyc_Interface_check_err( struct _tagged_string* msg1, struct _tagged_string
msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1; struct Cyc_Dict_Dict*
f2; int(* f3)( void*, void*, struct _tagged_string*); struct _tagged_string f4;
struct _tagged_string* f5; } ; static void Cyc_Interface_incl_dict_f( struct
_tuple13* env, struct _tagged_string* x, void* y1){ struct _tuple13 _temp589;
struct _tagged_string* _temp590; struct _tagged_string _temp592; int(* _temp594)(
void*, void*, struct _tagged_string*); struct Cyc_Dict_Dict* _temp596; int
_temp598; int* _temp600; struct _tuple13* _temp587= env; _temp589=* _temp587;
_LL599: _temp598= _temp589.f1; _temp600=&(* _temp587).f1; goto _LL597; _LL597:
_temp596= _temp589.f2; goto _LL595; _LL595: _temp594= _temp589.f3; goto _LL593;
_LL593: _temp592= _temp589.f4; goto _LL591; _LL591: _temp590= _temp589.f5; goto
_LL588; _LL588: { struct _handler_cons _temp601; _push_handler(& _temp601);{ int
_temp603= 0; if( setjmp( _temp601.handler)){ _temp603= 1;} if( ! _temp603){{
void* y2=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp596, x); if( ! _temp594( y1, y2, _temp590)){* _temp600= 0;}}; _pop_handler();}
else{ void* _temp602=( void*) _exn_thrown; void* _temp605= _temp602; _LL607: if(
_temp605 == Cyc_Dict_Absent){ goto _LL608;} else{ goto _LL609;} _LL609: goto
_LL610; _LL608: Cyc_Interface_check_err( _temp590,({ struct _tagged_string
_temp611= _temp592; struct _tagged_string _temp612=* x; xprintf("%.*s %.*s is missing",
_temp611.last_plus_one - _temp611.curr, _temp611.curr, _temp612.last_plus_one -
_temp612.curr, _temp612.curr);}));* _temp600= 0; goto _LL606; _LL610:( void)
_throw( _temp605); _LL606:;}}}} static int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( void*, void*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg){ struct
_tuple13 env=({ struct _tuple13 _temp613; _temp613.f1= 1; _temp613.f2= dic2;
_temp613.f3= incl_f; _temp613.f4= t; _temp613.f5= msg; _temp613;});(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_string*, void*), struct _tuple13*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& env, dic1);
return env.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Structdecl*
d= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); if( d == 0){ return 0;} if((
struct Cyc_Absyn_Structdecl*) d0 != d){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp615= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp614= d1->name; if( _temp614 == 0){ _throw( Null_Exception);} _temp614->v;}));
xprintf("declaration of struct %.*s discloses too much information", _temp615.last_plus_one
- _temp615.curr, _temp615.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct
_tagged_string* msg){ struct Cyc_Absyn_Uniondecl* d= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 !=
d){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp617= Cyc_Absynpp_qvar2string((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp616= d1->name; if( _temp616 == 0){
_throw( Null_Exception);} _temp616->v;})); xprintf("declaration of union %.*s discloses too much information",
_temp617.last_plus_one - _temp617.curr, _temp617.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Tuniondecl*
d= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); if( d == 0){ return 0;} if((
struct Cyc_Absyn_Tuniondecl*) d0 != d){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp618= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp618.last_plus_one - _temp618.curr, _temp618.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Enumdecl*
d= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); if( d == 0){ return 0;} if((
struct Cyc_Absyn_Enumdecl*) d0 != d){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp619= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp619.last_plus_one - _temp619.curr, _temp619.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl*
d1, struct _tagged_string* msg){ struct Cyc_Absyn_Vardecl* d= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); if( d == 0){ return 0;} if(( struct Cyc_Absyn_Vardecl*) d0 != d){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp620= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of variable %.*s discloses too much information",
_temp620.last_plus_one - _temp620.curr, _temp620.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0,
struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Typedefdecl*
d= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); if( d == 0){ return 0;} if((
struct Cyc_Absyn_Typedefdecl*) d0 != d){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp621= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_temp621.last_plus_one - _temp621.curr, _temp621.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_string* msg){ struct
Cyc_Tcdecl_Xtunionfielddecl* d= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1, 0,
msg); if( d == 0){ return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*) d0 != d){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp622= Cyc_Absynpp_qvar2string((
d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp622.last_plus_one - _temp622.curr, _temp622.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ int r1=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl,(
struct _tagged_string)({ char* _temp635=( char*)"struct"; struct _tagged_string
_temp636; _temp636.curr= _temp635; _temp636.base= _temp635; _temp636.last_plus_one=
_temp635 + 7; _temp636;}), msg); int r2=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*, struct
Cyc_Absyn_Uniondecl*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl,( struct _tagged_string)({ char* _temp633=( char*)"union";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 6; _temp634;}), msg); int r3=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl,( struct
_tagged_string)({ char* _temp631=( char*)"tunion"; struct _tagged_string
_temp632; _temp632.curr= _temp631; _temp632.base= _temp631; _temp632.last_plus_one=
_temp631 + 7; _temp632;}), msg); int r4=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*, struct
Cyc_Absyn_Enumdecl*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls,
Cyc_Interface_incl_enumdecl,( struct _tagged_string)({ char* _temp629=( char*)"enum";
struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base= _temp629;
_temp630.last_plus_one= _temp629 + 5; _temp630;}), msg); int r5=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl,( struct _tagged_string)({ char* _temp627=( char*)"typedef";
struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base= _temp627;
_temp628.last_plus_one= _temp627 + 8; _temp628;}), msg) != 0; int r6=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->vardecls,
ie2->vardecls, Cyc_Interface_incl_vardecl,( struct _tagged_string)({ char*
_temp625=( char*)"variable"; struct _tagged_string _temp626; _temp626.curr=
_temp625; _temp626.base= _temp625; _temp626.last_plus_one= _temp625 + 9;
_temp626;}), msg); int r7=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,(
struct _tagged_string)({ char* _temp623=( char*)"xtunionfield"; struct
_tagged_string _temp624; _temp624.curr= _temp623; _temp624.base= _temp623;
_temp624.last_plus_one= _temp623 + 13; _temp624;}), msg); return((((( r1? r2: 0)?
r3: 0)? r4: 0)? r5: 0)? r6: 0)? r7: 0;} struct _tuple14{ int f1; struct Cyc_Dict_Dict*
f2; struct _tagged_string* f3; } ; static void Cyc_Interface_incl_ns_f( struct
_tuple14* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie1){ struct
_tuple14 _temp639; struct _tagged_string* _temp640; struct Cyc_Dict_Dict*
_temp642; int _temp644; int* _temp646; struct _tuple14* _temp637= env; _temp639=*
_temp637; _LL645: _temp644= _temp639.f1; _temp646=&(* _temp637).f1; goto _LL643;
_LL643: _temp642= _temp639.f2; goto _LL641; _LL641: _temp640= _temp639.f3; goto
_LL638; _LL638: { struct Cyc_Interface_Ienv* ie2;{ struct _handler_cons _temp647;
_push_handler(& _temp647);{ int _temp649= 0; if( setjmp( _temp647.handler)){
_temp649= 1;} if( ! _temp649){ ie2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp642, ns);; _pop_handler();}
else{ void* _temp648=( void*) _exn_thrown; void* _temp651= _temp648; _LL653: if(
_temp651 == Cyc_Dict_Absent){ goto _LL654;} else{ goto _LL655;} _LL655: goto
_LL656; _LL654: ie2= Cyc_Interface_lazy_new_ienv(); goto _LL652; _LL656:( void)
_throw( _temp651); _LL652:;}}} if( ! Cyc_Interface_incl_ienv( ie1, ie2, _temp640)){*
_temp646= 0;}}} static int Cyc_Interface_incl_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple14 env=({
struct _tuple14 _temp657; _temp657.f1= 1; _temp657.f2= dic2; _temp657.f3= msg;
_temp657;});(( void(*)( void(* f)( struct _tuple14*, struct Cyc_List_List*,
struct Cyc_Interface_Ienv*), struct _tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& env, dic1); return env.f1;} int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp658; _push_handler(& _temp658);{ int _temp660= 0; if(
setjmp( _temp658.handler)){ _temp660= 1;} if( ! _temp660){{ int res= 1; struct
_tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string* _temp661=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp661[ 0]=({
struct _tagged_string _temp664=({ struct _tuple3* _temp662= info; if( _temp662
== 0){ _throw( Null_Exception);}* _temp662;}).f1; struct _tagged_string _temp665=({
struct _tuple3* _temp663= info; if( _temp663 == 0){ _throw( Null_Exception);}*
_temp663;}).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp664.last_plus_one - _temp664.curr, _temp664.curr, _temp665.last_plus_one -
_temp665.curr, _temp665.curr);}); _temp661;});} if( ! Cyc_Interface_incl_ns( i1->exports,
i2->exports, msg)){ res= 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp666=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp666[ 0]=({ struct _tagged_string _temp669=({ struct _tuple3* _temp667= info;
if( _temp667 == 0){ _throw( Null_Exception);}* _temp667;}).f2; struct
_tagged_string _temp670=({ struct _tuple3* _temp668= info; if( _temp668 == 0){
_throw( Null_Exception);}* _temp668;}).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp669.last_plus_one - _temp669.curr, _temp669.curr, _temp670.last_plus_one -
_temp670.curr, _temp670.curr);}); _temp666;});} if( ! Cyc_Interface_incl_ns( i2->imports,
i1->imports, msg)){ res= 0;}{ int _temp671= res; _npop_handler( 0u); return
_temp671;}}; _pop_handler();} else{ void* _temp659=( void*) _exn_thrown; void*
_temp673= _temp659; _LL675: if( _temp673 == Cyc_Tcdecl_Incompatible){ goto
_LL676;} else{ goto _LL677;} _LL677: goto _LL678; _LL676: return 0; _LL678:(
void) _throw( _temp673); _LL674:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); struct
_tagged_string f6; struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f(
struct _tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15
_temp681; struct _tagged_string* _temp682; struct _tagged_string _temp684; void**(*
_temp686)( void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*);
struct Cyc_Dict_Dict* _temp688; struct Cyc_Dict_Dict* _temp690; struct Cyc_Dict_Dict*
_temp692; struct Cyc_Dict_Dict** _temp694; int _temp695; int* _temp697; struct
_tuple15* _temp679= env; _temp681=* _temp679; _LL696: _temp695= _temp681.f1;
_temp697=&(* _temp679).f1; goto _LL693; _LL693: _temp692= _temp681.f2; _temp694=&(*
_temp679).f2; goto _LL691; _LL691: _temp690= _temp681.f3; goto _LL689; _LL689:
_temp688= _temp681.f4; goto _LL687; _LL687: _temp686= _temp681.f5; goto _LL685;
_LL685: _temp684= _temp681.f6; goto _LL683; _LL683: _temp682= _temp681.f7; goto
_LL680; _LL680: { void** y;{ struct _handler_cons _temp698; _push_handler(&
_temp698);{ int _temp700= 0; if( setjmp( _temp698.handler)){ _temp700= 1;} if( !
_temp700){{ void** y1=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp690, x); void** yt= _temp686( y1,
y2, 0, _temp682); if( !(( unsigned int) yt)){* _temp697= 0; _npop_handler( 0u);
return;} y=( void**)({ void** _temp701= yt; if( _temp701 == 0){ _throw(
Null_Exception);} _temp701;});}; _pop_handler();} else{ void* _temp699=( void*)
_exn_thrown; void* _temp703= _temp699; _LL705: if( _temp703 == Cyc_Dict_Absent){
goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL706: y= y2; goto
_LL704; _LL708:( void) _throw( _temp703); _LL704:;}}}{ struct _handler_cons
_temp709; _push_handler(& _temp709);{ int _temp711= 0; if( setjmp( _temp709.handler)){
_temp711= 1;} if( ! _temp711){{ void** ye=(( void**(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp688, x); void** yt= _temp686(
ye, y, 0, _temp682); if( yt !=( void**) ye){ if(( unsigned int) yt){ Cyc_Interface_check_err(
_temp682,({ struct _tagged_string _temp712= _temp684; struct _tagged_string
_temp713=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp712.last_plus_one - _temp712.curr, _temp712.curr, _temp713.last_plus_one -
_temp713.curr, _temp713.curr);}));}* _temp697= 0;}}; _pop_handler();} else{ void*
_temp710=( void*) _exn_thrown; void* _temp715= _temp710; _LL717: if( _temp715 ==
Cyc_Dict_Absent){ goto _LL718;} else{ goto _LL719;} _LL719: goto _LL720; _LL718:
if(* _temp697){* _temp694=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, void** data)) Cyc_Dict_insert)(* _temp694, x, y);}
goto _LL716; _LL720:( void) _throw( _temp715); _LL716:;}}}}} struct Cyc_Core_Opt*
Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg){
struct _tuple15 env=({ struct _tuple15 _temp722; _temp722.f1= 1; _temp722.f2=
dic1; _temp722.f3= dic1; _temp722.f4= excl; _temp722.f5= merge_f; _temp722.f6= t;
_temp722.f7= msg; _temp722;});(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp721=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp721->v=( void*) env.f2; _temp721;});}
else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_compat_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct Cyc_Interface_Ienv*
iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt* r1=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,(
struct _tagged_string)({ char* _temp743=( char*)"struct"; struct _tagged_string
_temp744; _temp744.curr= _temp743; _temp744.base= _temp743; _temp744.last_plus_one=
_temp743 + 7; _temp744;}), msg); struct Cyc_Core_Opt* r2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl,(
struct _tagged_string)({ char* _temp741=( char*)"union"; struct _tagged_string
_temp742; _temp742.curr= _temp741; _temp742.base= _temp741; _temp742.last_plus_one=
_temp741 + 6; _temp742;}), msg); struct Cyc_Core_Opt* r3=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,(
struct _tagged_string)({ char* _temp739=( char*)"tunion"; struct _tagged_string
_temp740; _temp740.curr= _temp739; _temp740.base= _temp739; _temp740.last_plus_one=
_temp739 + 7; _temp740;}), msg); struct Cyc_Core_Opt* r4=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct Cyc_Absyn_Enumdecl*, struct
Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl,(
struct _tagged_string)({ char* _temp737=( char*)"enum"; struct _tagged_string
_temp738; _temp738.curr= _temp737; _temp738.base= _temp737; _temp738.last_plus_one=
_temp737 + 5; _temp738;}), msg); struct Cyc_Core_Opt* r5=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,(
struct _tagged_string)({ char* _temp735=( char*)"typedef"; struct _tagged_string
_temp736; _temp736.curr= _temp735; _temp736.base= _temp735; _temp736.last_plus_one=
_temp735 + 8; _temp736;}), msg); struct Cyc_Core_Opt* r6=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct Cyc_Absyn_Vardecl*, struct
Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->vardecls, ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl,( struct
_tagged_string)({ char* _temp733=( char*)"variable"; struct _tagged_string
_temp734; _temp734.curr= _temp733; _temp734.base= _temp733; _temp734.last_plus_one=
_temp733 + 9; _temp734;}), msg); struct Cyc_Core_Opt* r7=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)( struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls, iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl,(
struct _tagged_string)({ char* _temp731=( char*)"xtunionfield"; struct
_tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 13; _temp732;}), msg); if(((((( !((
unsigned int) r1)? 1: !(( unsigned int) r2))? 1: !(( unsigned int) r3))? 1: !((
unsigned int) r4))? 1: !(( unsigned int) r5))? 1: !(( unsigned int) r6))? 1: !((
unsigned int) r7)){ return 0;} else{ return({ struct Cyc_Interface_Ienv*
_temp723=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp723->structdecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp730=
r1; if( _temp730 == 0){ _throw( Null_Exception);} _temp730->v;}); _temp723->uniondecls=(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp729= r2; if( _temp729 == 0){
_throw( Null_Exception);} _temp729->v;}); _temp723->tuniondecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp728= r3; if( _temp728 == 0){ _throw( Null_Exception);}
_temp728->v;}); _temp723->enumdecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp727= r4; if( _temp727 == 0){ _throw( Null_Exception);} _temp727->v;});
_temp723->typedefdecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp726=
r5; if( _temp726 == 0){ _throw( Null_Exception);} _temp726->v;}); _temp723->vardecls=(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp725= r6; if( _temp725 == 0){
_throw( Null_Exception);} _temp725->v;}); _temp723->xtunionfielddecls=( struct
Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp724= r7; if( _temp724 == 0){ _throw(
Null_Exception);} _temp724->v;}); _temp723;});}} struct _tuple16{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; struct
_tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct _tuple16*
env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple16
_temp747; struct _tagged_string* _temp748; struct Cyc_Dict_Dict* _temp750;
struct Cyc_Dict_Dict* _temp752; struct Cyc_Dict_Dict* _temp754; struct Cyc_Dict_Dict**
_temp756; int _temp757; int* _temp759; struct _tuple16* _temp745= env; _temp747=*
_temp745; _LL758: _temp757= _temp747.f1; _temp759=&(* _temp745).f1; goto _LL755;
_LL755: _temp754= _temp747.f2; _temp756=&(* _temp745).f2; goto _LL753; _LL753:
_temp752= _temp747.f3; goto _LL751; _LL751: _temp750= _temp747.f4; goto _LL749;
_LL749: _temp748= _temp747.f5; goto _LL746; _LL746: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp760;
_push_handler(& _temp760);{ int _temp762= 0; if( setjmp( _temp760.handler)){
_temp762= 1;} if( ! _temp762){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp750, ns);;
_pop_handler();} else{ void* _temp761=( void*) _exn_thrown; void* _temp764=
_temp761; _LL766: if( _temp764 == Cyc_Dict_Absent){ goto _LL767;} else{ goto
_LL768;} _LL768: goto _LL769; _LL767: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL765; _LL769:( void) _throw( _temp764); _LL765:;}}}{ struct _handler_cons
_temp770; _push_handler(& _temp770);{ int _temp772= 0; if( setjmp( _temp770.handler)){
_temp772= 1;} if( ! _temp772){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp752, ns);{ struct Cyc_Interface_Ienv*
ie= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1, iexcl,
_temp748); if( ie == 0){* _temp759= 0; _npop_handler( 0u); return;} ie1=( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp773= ie; if( _temp773 ==
0){ _throw( Null_Exception);} _temp773;});}; _pop_handler();} else{ void*
_temp771=( void*) _exn_thrown; void* _temp775= _temp771; _LL777: if( _temp775 ==
Cyc_Dict_Absent){ goto _LL778;} else{ goto _LL779;} _LL779: goto _LL780; _LL778:
ie1= Cyc_Interface_lazy_new_ienv(); goto _LL776; _LL780:( void) _throw( _temp775);
_LL776:;}}}{ struct Cyc_Interface_Ienv* ie= Cyc_Interface_compat_merge_ienv( ie1,
ie2, iexcl, _temp748); if( ie == 0){* _temp759= 0;} else{ if(* _temp759){*
_temp756=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp756, ns,( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp781= ie; if( _temp781 ==
0){ _throw( Null_Exception);} _temp781;}));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 env=({ struct _tuple16
_temp783; _temp783.f1= 1; _temp783.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp783.f3= dic1; _temp783.f4= excl; _temp783.f5= msg; _temp783;});(( void(*)(
void(* f)( struct _tuple16*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp782=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp782->v=( void*) env.f2; _temp782;});}
else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict*
f3; struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp786; struct _tagged_string* _temp787; struct _tagged_string _temp789;
struct Cyc_Dict_Dict* _temp791; struct Cyc_Dict_Dict* _temp793; struct Cyc_Dict_Dict**
_temp795; int _temp796; int* _temp798; struct _tuple17* _temp784= env; _temp786=*
_temp784; _LL797: _temp796= _temp786.f1; _temp798=&(* _temp784).f1; goto _LL794;
_LL794: _temp793= _temp786.f2; _temp795=&(* _temp784).f2; goto _LL792; _LL792:
_temp791= _temp786.f3; goto _LL790; _LL790: _temp789= _temp786.f4; goto _LL788;
_LL788: _temp787= _temp786.f5; goto _LL785; _LL785: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp791, x)){ Cyc_Interface_check_err(
_temp787,({ struct _tagged_string _temp799= _temp789; struct _tagged_string
_temp800=* x; xprintf("%.*s %.*s is exported more than once", _temp799.last_plus_one
- _temp799.curr, _temp799.curr, _temp800.last_plus_one - _temp800.curr, _temp800.curr);}));*
_temp798= 0;} else{ if(* _temp798){* _temp795=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(*
_temp795, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg){ struct _tuple17 env=({ struct _tuple17 _temp802; _temp802.f1=
1; _temp802.f2= dic1; _temp802.f3= dic1; _temp802.f4= t; _temp802.f5= msg;
_temp802;});(( void(*)( void(* f)( struct _tuple17*, struct _tagged_string*,
void*), struct _tuple17* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)(
struct _tuple17* env, struct _tagged_string* x, void* y)) Cyc_Interface_disj_merge_dict_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp801=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp801->v=( void*) env.f2; _temp801;});}
else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ struct Cyc_Core_Opt* r1=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,(
struct _tagged_string)({ char* _temp823=( char*)"struct"; struct _tagged_string
_temp824; _temp824.curr= _temp823; _temp824.base= _temp823; _temp824.last_plus_one=
_temp823 + 7; _temp824;}), msg); struct Cyc_Core_Opt* r2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls,
ie2->uniondecls,( struct _tagged_string)({ char* _temp821=( char*)"union";
struct _tagged_string _temp822; _temp822.curr= _temp821; _temp822.base= _temp821;
_temp822.last_plus_one= _temp821 + 6; _temp822;}), msg); struct Cyc_Core_Opt* r3=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls,( struct _tagged_string)({ char* _temp819=(
char*)"[x]tunion"; struct _tagged_string _temp820; _temp820.curr= _temp819;
_temp820.base= _temp819; _temp820.last_plus_one= _temp819 + 10; _temp820;}), msg);
struct Cyc_Core_Opt* r4=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,( struct
_tagged_string)({ char* _temp817=( char*)"enum"; struct _tagged_string _temp818;
_temp818.curr= _temp817; _temp818.base= _temp817; _temp818.last_plus_one=
_temp817 + 5; _temp818;}), msg); struct Cyc_Core_Opt* r5=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl,( struct _tagged_string)({ char* _temp815=( char*)"typedef";
struct _tagged_string _temp816; _temp816.curr= _temp815; _temp816.base= _temp815;
_temp816.last_plus_one= _temp815 + 8; _temp816;}), msg); struct Cyc_Core_Opt* r6=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->vardecls, ie2->vardecls,( struct _tagged_string)({ char* _temp813=( char*)"variable";
struct _tagged_string _temp814; _temp814.curr= _temp813; _temp814.base= _temp813;
_temp814.last_plus_one= _temp813 + 9; _temp814;}), msg); struct Cyc_Core_Opt* r7=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls,( struct _tagged_string)({ char*
_temp811=( char*)"xtunionfield"; struct _tagged_string _temp812; _temp812.curr=
_temp811; _temp812.base= _temp811; _temp812.last_plus_one= _temp811 + 13;
_temp812;}), msg); if(((((( !(( unsigned int) r1)? 1: !(( unsigned int) r2))? 1:
!(( unsigned int) r3))? 1: !(( unsigned int) r4))? 1: !(( unsigned int) r5))? 1:
!(( unsigned int) r6))? 1: !(( unsigned int) r7)){ return 0;} else{ return({
struct Cyc_Interface_Ienv* _temp803=( struct Cyc_Interface_Ienv*) GC_malloc(
sizeof( struct Cyc_Interface_Ienv)); _temp803->structdecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp810= r1; if( _temp810 == 0){ _throw( Null_Exception);}
_temp810->v;}); _temp803->uniondecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp809= r2; if( _temp809 == 0){ _throw( Null_Exception);} _temp809->v;});
_temp803->tuniondecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp808=
r3; if( _temp808 == 0){ _throw( Null_Exception);} _temp808->v;}); _temp803->enumdecls=(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp807= r4; if( _temp807 == 0){
_throw( Null_Exception);} _temp807->v;}); _temp803->typedefdecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp806= r5; if( _temp806 == 0){ _throw( Null_Exception);}
_temp806->v;}); _temp803->vardecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp805= r6; if( _temp805 == 0){ _throw( Null_Exception);} _temp805->v;});
_temp803->xtunionfielddecls=( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp804= r7; if( _temp804 == 0){ _throw( Null_Exception);} _temp804->v;});
_temp803;});}} struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict*
f3; struct _tagged_string* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct
_tuple18* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct
_tuple18 _temp827; struct _tagged_string* _temp828; struct Cyc_Dict_Dict*
_temp830; struct Cyc_Dict_Dict* _temp832; struct Cyc_Dict_Dict** _temp834; int
_temp835; int* _temp837; struct _tuple18* _temp825= env; _temp827=* _temp825;
_LL836: _temp835= _temp827.f1; _temp837=&(* _temp825).f1; goto _LL833; _LL833:
_temp832= _temp827.f2; _temp834=&(* _temp825).f2; goto _LL831; _LL831: _temp830=
_temp827.f3; goto _LL829; _LL829: _temp828= _temp827.f4; goto _LL826; _LL826: {
struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons _temp838; _push_handler(&
_temp838);{ int _temp840= 0; if( setjmp( _temp838.handler)){ _temp840= 1;} if( !
_temp840){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( _temp830, ns);; _pop_handler();} else{
void* _temp839=( void*) _exn_thrown; void* _temp842= _temp839; _LL844: if(
_temp842 == Cyc_Dict_Absent){ goto _LL845;} else{ goto _LL846;} _LL846: goto
_LL847; _LL845: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL843; _LL847:( void)
_throw( _temp842); _LL843:;}}}{ struct Cyc_Interface_Ienv* ie= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp828); if( ie == 0){* _temp837= 0;} else{ if(* _temp837){*
_temp834=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp834, ns,( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp848= ie; if( _temp848 ==
0){ _throw( Null_Exception);} _temp848;}));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string*
msg){ struct _tuple18 env=({ struct _tuple18 _temp850; _temp850.f1= 1; _temp850.f2=
dic1; _temp850.f3= dic1; _temp850.f4= msg; _temp850;});(( void(*)( void(* f)(
struct _tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple18* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
env, dic2); if( env.f1){ return({ struct Cyc_Core_Opt* _temp849=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp849->v=( void*) env.f2; _temp849;});}
else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct _handler_cons
_temp851; _push_handler(& _temp851);{ int _temp853= 0; if( setjmp( _temp851.handler)){
_temp853= 1;} if( ! _temp853){{ struct _tagged_string* msg= 0; if( info != 0){
msg=({ struct _tagged_string* _temp854=( struct _tagged_string*) GC_malloc(
sizeof( struct _tagged_string)); _temp854[ 0]=({ struct _tagged_string _temp857=({
struct _tuple3* _temp855= info; if( _temp855 == 0){ _throw( Null_Exception);}*
_temp855;}).f1; struct _tagged_string _temp858=({ struct _tuple3* _temp856= info;
if( _temp856 == 0){ _throw( Null_Exception);}* _temp856;}).f2; xprintf("merging exports of %.*s and %.*s,",
_temp857.last_plus_one - _temp857.curr, _temp857.curr, _temp858.last_plus_one -
_temp858.curr, _temp858.curr);}); _temp854;});}{ struct Cyc_Core_Opt* exp= Cyc_Interface_disj_merge_ns(
i1->exports, i2->exports, msg); if( exp == 0){ struct Cyc_Interface_I* _temp859=
0; _npop_handler( 0u); return _temp859;} if( info != 0){ msg=({ struct
_tagged_string* _temp860=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp860[ 0]=({ struct _tagged_string _temp863=({ struct
_tuple3* _temp861= info; if( _temp861 == 0){ _throw( Null_Exception);}* _temp861;}).f1;
struct _tagged_string _temp864=({ struct _tuple3* _temp862= info; if( _temp862
== 0){ _throw( Null_Exception);}* _temp862;}).f2; xprintf("merging imports of %.*s and %.*s,",
_temp863.last_plus_one - _temp863.curr, _temp863.curr, _temp864.last_plus_one -
_temp864.curr, _temp864.curr);}); _temp860;});}{ struct Cyc_Core_Opt* imp= Cyc_Interface_compat_merge_ns(
i1->imports, i2->imports,( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp870= exp; if( _temp870 == 0){ _throw( Null_Exception);} _temp870->v;}), msg);
if( imp == 0){ struct Cyc_Interface_I* _temp865= 0; _npop_handler( 0u); return
_temp865;}{ struct Cyc_Interface_I* _temp869=({ struct Cyc_Interface_I* _temp866=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp866->imports=(
struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp868= imp; if( _temp868 == 0){
_throw( Null_Exception);} _temp868->v;}); _temp866->exports=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp867= exp; if( _temp867 == 0){ _throw( Null_Exception);}
_temp867->v;}); _temp866;}); _npop_handler( 0u); return _temp869;}}}};
_pop_handler();} else{ void* _temp852=( void*) _exn_thrown; void* _temp872=
_temp852; _LL874: if( _temp872 == Cyc_Tcdecl_Incompatible){ goto _LL875;} else{
goto _LL876;} _LL876: goto _LL877; _LL875: return 0; _LL877:( void) _throw(
_temp872); _LL873:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)({ struct Cyc_List_List* _temp893= li; if( _temp893 == 0){
_throw( Null_Exception);} _temp893->hd;}); struct _tagged_string* curr_info=
linfo != 0?( struct _tagged_string*)(( struct _tagged_string*)({ struct Cyc_List_List*
_temp892= linfo; if( _temp892 == 0){ _throw( Null_Exception);} _temp892->hd;})):
0; li=({ struct Cyc_List_List* _temp878= li; if( _temp878 == 0){ _throw(
Null_Exception);} _temp878->tl;}); if( linfo != 0){ linfo=({ struct Cyc_List_List*
_temp879= linfo; if( _temp879 == 0){ _throw( Null_Exception);} _temp879->tl;});}
for( 0; li != 0; li=({ struct Cyc_List_List* _temp880= li; if( _temp880 == 0){
_throw( Null_Exception);} _temp880->tl;})){ struct Cyc_Interface_I* i= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)({ struct Cyc_List_List* _temp888= li; if(
_temp888 == 0){ _throw( Null_Exception);} _temp888->hd;}),( curr_info != 0?
linfo != 0: 0)?({ struct _tuple3* _temp889=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp889->f1=({ struct _tagged_string* _temp891= curr_info;
if( _temp891 == 0){ _throw( Null_Exception);}* _temp891;}); _temp889->f2=*((
struct _tagged_string*)({ struct Cyc_List_List* _temp890= linfo; if( _temp890 ==
0){ _throw( Null_Exception);} _temp890->hd;})); _temp889;}): 0); if( i == 0){
return 0;} curr_i=( struct Cyc_Interface_I*)({ struct Cyc_Interface_I* _temp881=
i; if( _temp881 == 0){ _throw( Null_Exception);} _temp881;}); if( curr_info != 0){
curr_info= linfo != 0?({ struct _tagged_string* _temp882=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp882[ 0]=({ struct
_tagged_string _temp885=({ struct _tagged_string* _temp883= curr_info; if(
_temp883 == 0){ _throw( Null_Exception);}* _temp883;}); struct _tagged_string
_temp886=*(( struct _tagged_string*)({ struct Cyc_List_List* _temp884= linfo;
if( _temp884 == 0){ _throw( Null_Exception);} _temp884->hd;})); xprintf("%.*s+%.*s",
_temp885.last_plus_one - _temp885.curr, _temp885.curr, _temp886.last_plus_one -
_temp886.curr, _temp886.curr);}); _temp882;}): 0;} if( linfo != 0){ linfo=({
struct Cyc_List_List* _temp887= linfo; if( _temp887 == 0){ _throw(
Null_Exception);} _temp887->tl;});}} return( struct Cyc_Interface_I*) curr_i;}}
static struct Cyc_List_List* Cyc_Interface_add_namespace( struct Cyc_List_List*
tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;} return({ struct Cyc_List_List*
_temp894=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp894->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp895=( struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp895[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp896; _temp896.tag= Cyc_Absyn_Namespace_d;
_temp896.f1=( struct _tagged_string*)({ struct Cyc_List_List* _temp897= ns; if(
_temp897 == 0){ _throw( Null_Exception);} _temp897->hd;}); _temp896.f2= Cyc_Interface_add_namespace(
tds,({ struct Cyc_List_List* _temp898= ns; if( _temp898 == 0){ _throw(
Null_Exception);} _temp898->tl;})); _temp896;}); _temp895;}), 0); _temp894->tl=
0; _temp894;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl(
struct _tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp899=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp899->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp900=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp900[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp901; _temp901.tag= Cyc_Absyn_Struct_d; _temp901.f1=
d; _temp901;}); _temp900;}), 0); _temp899->tl= tds; _temp899;});} static struct
Cyc_List_List* Cyc_Interface_add_structdecl_header( struct _tagged_string* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({ struct Cyc_Absyn_Structdecl*
_temp902=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp902[ 0]=* d; _temp902;}); d->fields= 0;( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp903=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp903->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp904=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp904[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp905; _temp905.tag= Cyc_Absyn_Struct_d; _temp905.f1=
d; _temp905;}); _temp904;}), 0); _temp903->tl= tds; _temp903;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp906=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp906->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp907=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp907[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp908; _temp908.tag= Cyc_Absyn_Union_d;
_temp908.f1= d; _temp908;}); _temp907;}), 0); _temp906->tl= tds; _temp906;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp909=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp909[ 0]=* d; _temp909;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp910=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp910->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp911=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp911[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp912; _temp912.tag= Cyc_Absyn_Union_d;
_temp912.f1= d; _temp912;}); _temp911;}), 0); _temp910->tl= tds; _temp910;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp913=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp913->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp914=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp914[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp915; _temp915.tag= Cyc_Absyn_Tunion_d;
_temp915.f1= d; _temp915;}); _temp914;}), 0); _temp913->tl= tds; _temp913;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp916=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp916[ 0]=* d; _temp916;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp917=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp917->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp918=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp918[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp919; _temp919.tag= Cyc_Absyn_Tunion_d;
_temp919.f1= d; _temp919;}); _temp918;}), 0); _temp917->tl= tds; _temp917;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp920=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp920->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp921=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp921[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp922; _temp922.tag= Cyc_Absyn_Enum_d;
_temp922.f1= d; _temp922;}); _temp921;}), 0); _temp920->tl= tds; _temp920;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp923=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp923[ 0]=* d; _temp923;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp924=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp924->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp925=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp925[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp926; _temp926.tag= Cyc_Absyn_Enum_d;
_temp926.f1= d; _temp926;}); _temp925;}), 0); _temp924->tl= tds; _temp924;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp927=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp927->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp928=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp928[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp929; _temp929.tag= Cyc_Absyn_Typedef_d;
_temp929.f1= d; _temp929;}); _temp928;}), 0); _temp927->tl= tds; _temp927;});}
static struct Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp930=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp930->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp931=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp931[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp932; _temp932.tag= Cyc_Absyn_Var_d;
_temp932.f1= d; _temp932;}); _temp931;}), 0); _temp930->tl= tds; _temp930;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp935; struct Cyc_Absyn_Tunionfield*
_temp936; struct Cyc_Absyn_Tuniondecl* _temp938; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp933= d; _temp935=* _temp933; _LL939: _temp938=( struct Cyc_Absyn_Tuniondecl*)
_temp935.base; goto _LL937; _LL937: _temp936=( struct Cyc_Absyn_Tunionfield*)
_temp935.field; goto _LL934; _LL934: _temp938=({ struct Cyc_Absyn_Tuniondecl*
_temp940=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp940[ 0]=* _temp938; _temp940;}); _temp938->fields=({ struct Cyc_Core_Opt*
_temp941=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp941->v=( void*)({ struct Cyc_List_List* _temp942=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp942->hd=( void*) _temp936;
_temp942->tl= 0; _temp942;}); _temp941;});( void*)( _temp938->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp943=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp943->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp944=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp944[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp945; _temp945.tag= Cyc_Absyn_Tunion_d; _temp945.f1=
_temp938; _temp945;}); _temp944;}), 0); _temp943->tl= tds; _temp943;});} static
void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct Cyc_List_List*
ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List* tds= 0; tds=(( struct
Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*, struct
Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct
Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls,
tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, tds); if( tds != 0){ tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace( tds, ns); Cyc_Absynpp_decllist2file(
tds, f);}} static void Cyc_Interface_print_ns_typedefs( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
tds= 0; tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct
Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef,
ie->typedefdecls, tds); if( tds != 0){ tds=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace(
tds, ns); Cyc_Absynpp_decllist2file( tds, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* tds= 0; tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl,
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
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, tds); tds=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, tds); if( tds != 0){ tds=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( tds); tds= Cyc_Interface_add_namespace(
tds, ns); Cyc_Absynpp_decllist2file( tds, f);}} void Cyc_Interface_print( struct
Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ fprintf( f,"/****** needed (headers) ******/\n");((
void(*)( void(* f)( struct Cyc_Stdio___sFILE*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
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