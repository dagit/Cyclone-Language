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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_identity( void*);
extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u]; struct
Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag; struct
_tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct
Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
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
void*(* f)( void*), struct Cyc_List_List* l){ int _temp17= 1; goto _LL18; _LL18:
for( 0; l != 0; l=({ struct Cyc_List_List* _temp19= l; if( _temp19 == 0){ _throw(
Null_Exception);} _temp19->tl;})){ if( ! Cyc_Interface_check_public_type( seen,
obj, name, f(( void*)({ struct Cyc_List_List* _temp20= l; if( _temp20 == 0){
_throw( Null_Exception);} _temp20->hd;})))){ _temp17= 0;}} return _temp17;}
static int Cyc_Interface_check_public_structdecl( struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Structdecl* d){ struct _handler_cons _temp21;
_push_handler(& _temp21);{ int _temp23= 0; if( setjmp( _temp21.handler)){
_temp23= 1;} if( ! _temp23){{ int _temp25=(( int(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct _tuple0*)({
struct Cyc_Core_Opt* _temp24= d->name; if( _temp24 == 0){ _throw( Null_Exception);}
_temp24->v;})); _npop_handler( 0u); return _temp25;}; _pop_handler();} else{
void* _temp22=( void*) _exn_thrown; void* _temp27= _temp22; _LL29: if( _temp27
== Cyc_Dict_Absent){ goto _LL30;} else{ goto _LL31;} _LL31: goto _LL32; _LL30: {
int _temp33= 1; goto _LL34; _LL34: seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)({ struct Cyc_Core_Opt* _temp35= d->name; if( _temp35 == 0){
_throw( Null_Exception);} _temp35->v;}), _temp33); if( d->fields != 0){ struct
Cyc_List_List* _temp37=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp36=
d->fields; if( _temp36 == 0){ _throw( Null_Exception);} _temp36->v;}); goto
_LL38; _LL38: for( 0; _temp37 != 0; _temp37=({ struct Cyc_List_List* _temp39=
_temp37; if( _temp39 == 0){ _throw( Null_Exception);} _temp39->tl;})){ if( ! Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp40=( char*)"struct"; struct
_tagged_string _temp41; _temp41.curr= _temp40; _temp41.base= _temp40; _temp41.last_plus_one=
_temp40 + 7; _temp41;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp42= d->name;
if( _temp42 == 0){ _throw( Null_Exception);} _temp42->v;}),( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp43= _temp37; if( _temp43 == 0){ _throw(
Null_Exception);} _temp43->hd;}))->type)){ _temp33= 0;}}} seen->structs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->structs,( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp44= d->name; if( _temp44 == 0){ _throw( Null_Exception);} _temp44->v;}),
_temp33); return _temp33;} _LL32:( void) _throw( _temp27); _LL28:;}}} static int
Cyc_Interface_check_public_uniondecl( struct Cyc_Interface_Seen* seen, struct
Cyc_Absyn_Uniondecl* d){ struct _handler_cons _temp45; _push_handler(& _temp45);{
int _temp47= 0; if( setjmp( _temp45.handler)){ _temp47= 1;} if( ! _temp47){{ int
_temp49=(( int(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)(
seen->unions,( struct _tuple0*)({ struct Cyc_Core_Opt* _temp48= d->name; if(
_temp48 == 0){ _throw( Null_Exception);} _temp48->v;})); _npop_handler( 0u);
return _temp49;}; _pop_handler();} else{ void* _temp46=( void*) _exn_thrown;
void* _temp51= _temp46; _LL53: if( _temp51 == Cyc_Dict_Absent){ goto _LL54;}
else{ goto _LL55;} _LL55: goto _LL56; _LL54: { int _temp57= 1; goto _LL58; _LL58:
seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)({
struct Cyc_Core_Opt* _temp59= d->name; if( _temp59 == 0){ _throw( Null_Exception);}
_temp59->v;}), _temp57); if( d->fields != 0){ struct Cyc_List_List* _temp61=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp60= d->fields; if( _temp60 ==
0){ _throw( Null_Exception);} _temp60->v;}); goto _LL62; _LL62: for( 0; _temp61
!= 0; _temp61=({ struct Cyc_List_List* _temp63= _temp61; if( _temp63 == 0){
_throw( Null_Exception);} _temp63->tl;})){ if( ! Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp64=( char*)"union"; struct
_tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64; _temp65.last_plus_one=
_temp64 + 6; _temp65;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp66= d->name;
if( _temp66 == 0){ _throw( Null_Exception);} _temp66->v;}),( void*)(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp67= _temp61; if( _temp67 == 0){ _throw(
Null_Exception);} _temp67->hd;}))->type)){ _temp57= 0;}}} seen->unions=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)(
seen->unions,( struct _tuple0*)({ struct Cyc_Core_Opt* _temp68= d->name; if(
_temp68 == 0){ _throw( Null_Exception);} _temp68->v;}), _temp57); return _temp57;}
_LL56:( void) _throw( _temp51); _LL52:;}}} struct _tuple10{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static void* Cyc_Interface_get_type1( struct _tuple10* x){
return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl( struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Tuniondecl* d){ struct _handler_cons _temp69;
_push_handler(& _temp69);{ int _temp71= 0; if( setjmp( _temp69.handler)){
_temp71= 1;} if( ! _temp71){{ int _temp72=(( int(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions, d->name); _npop_handler(
0u); return _temp72;}; _pop_handler();} else{ void* _temp70=( void*) _exn_thrown;
void* _temp74= _temp70; _LL76: if( _temp74 == Cyc_Dict_Absent){ goto _LL77;}
else{ goto _LL78;} _LL78: goto _LL79; _LL77: { int _temp80= 1; goto _LL81; _LL81:
seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name, _temp80); if(
d->fields != 0){ struct Cyc_List_List* _temp83=( struct Cyc_List_List*)({ struct
Cyc_Core_Opt* _temp82= d->fields; if( _temp82 == 0){ _throw( Null_Exception);}
_temp82->v;}); goto _LL84; _LL84: for( 0; _temp83 != 0; _temp83=({ struct Cyc_List_List*
_temp85= _temp83; if( _temp85 == 0){ _throw( Null_Exception);} _temp85->tl;})){
if( !(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj,
struct _tuple0* name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l))
Cyc_Interface_check_public_types)( seen,( struct _tagged_string)({ char* _temp86=(
char*)"[x]tunion"; struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base=
_temp86; _temp87.last_plus_one= _temp86 + 10; _temp87;}), d->name, Cyc_Interface_get_type1,((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp88= _temp83; if(
_temp88 == 0){ _throw( Null_Exception);} _temp88->hd;}))->typs)){ _temp80= 0;}}}
seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name, _temp80);
return _temp80;} _LL79:( void) _throw( _temp74); _LL75:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl* d){ return 1;}
static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Typedefdecl* d){ return Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp89=( char*)"typedef"; struct
_tagged_string _temp90; _temp90.curr= _temp89; _temp90.base= _temp89; _temp90.last_plus_one=
_temp89 + 8; _temp90;}), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl* d){ return Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp91=( char*)"variable"; struct
_tagged_string _temp92; _temp92.curr= _temp91; _temp92.base= _temp91; _temp92.last_plus_one=
_temp91 + 9; _temp92;}), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static int Cyc_Interface_check_public_type(
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void* t){ void* _temp93= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp115; struct Cyc_Absyn_Conref* _temp117; struct Cyc_Absyn_Tqual _temp119;
struct Cyc_Absyn_Conref* _temp121; void* _temp123; void* _temp125; struct Cyc_Absyn_Exp*
_temp127; struct Cyc_Absyn_Tqual _temp129; void* _temp131; struct Cyc_Core_Opt*
_temp133; struct Cyc_Core_Opt _temp135; void* _temp136; struct Cyc_List_List*
_temp138; struct _tuple0* _temp140; struct Cyc_Absyn_FnInfo _temp142; struct Cyc_List_List*
_temp144; int _temp146; struct Cyc_List_List* _temp148; void* _temp150; struct
Cyc_Core_Opt* _temp152; struct Cyc_List_List* _temp154; struct Cyc_List_List*
_temp156; struct Cyc_Absyn_Structdecl** _temp158; struct Cyc_Absyn_Structdecl*
_temp160; struct Cyc_List_List* _temp161; struct _tuple0* _temp163; struct Cyc_Absyn_Uniondecl**
_temp165; struct Cyc_Absyn_Uniondecl* _temp167; struct Cyc_List_List* _temp168;
struct _tuple0* _temp170; struct Cyc_Absyn_TunionInfo _temp172; void* _temp174;
struct Cyc_List_List* _temp176; void* _temp178; struct Cyc_Absyn_Tuniondecl*
_temp180; struct Cyc_Absyn_TunionFieldInfo _temp182; struct Cyc_List_List*
_temp184; void* _temp186; struct Cyc_Absyn_Tunionfield* _temp188; struct Cyc_Absyn_Tuniondecl*
_temp190; _LL95: if(( unsigned int) _temp93 > 4u?*(( int*) _temp93) == Cyc_Absyn_PointerType:
0){ _LL116: _temp115=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp93)->f1; _LL126: _temp125=( void*) _temp115.elt_typ; goto _LL124; _LL124:
_temp123=( void*) _temp115.rgn_typ; goto _LL122; _LL122: _temp121=( struct Cyc_Absyn_Conref*)
_temp115.nullable; goto _LL120; _LL120: _temp119=( struct Cyc_Absyn_Tqual)
_temp115.tq; goto _LL118; _LL118: _temp117=( struct Cyc_Absyn_Conref*) _temp115.bounds;
goto _LL96;} else{ goto _LL97;} _LL97: if(( unsigned int) _temp93 > 4u?*(( int*)
_temp93) == Cyc_Absyn_ArrayType: 0){ _LL132: _temp131=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp93)->f1; goto _LL130; _LL130: _temp129=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp93)->f2; goto _LL128; _LL128: _temp127=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp93)->f3; goto
_LL98;} else{ goto _LL99;} _LL99: if(( unsigned int) _temp93 > 4u?*(( int*)
_temp93) == Cyc_Absyn_TypedefType: 0){ _LL141: _temp140=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp93)->f1; goto _LL139; _LL139:
_temp138=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp93)->f2; goto _LL134; _LL134: _temp133=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp93)->f3; if( _temp133 == 0){ goto _LL101;} else{ _temp135=* _temp133;
_LL137: _temp136=( void*) _temp135.v; goto _LL100;}} else{ goto _LL101;} _LL101:
if(( unsigned int) _temp93 > 4u?*(( int*) _temp93) == Cyc_Absyn_FnType: 0){
_LL143: _temp142=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp93)->f1; _LL155: _temp154=( struct Cyc_List_List*) _temp142.tvars; goto
_LL153; _LL153: _temp152=( struct Cyc_Core_Opt*) _temp142.effect; goto _LL151;
_LL151: _temp150=( void*) _temp142.ret_typ; goto _LL149; _LL149: _temp148=(
struct Cyc_List_List*) _temp142.args; goto _LL147; _LL147: _temp146=( int)
_temp142.varargs; goto _LL145; _LL145: _temp144=( struct Cyc_List_List*)
_temp142.attributes; goto _LL102;} else{ goto _LL103;} _LL103: if(( unsigned int)
_temp93 > 4u?*(( int*) _temp93) == Cyc_Absyn_TupleType: 0){ _LL157: _temp156=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp93)->f1; goto
_LL104;} else{ goto _LL105;} _LL105: if(( unsigned int) _temp93 > 4u?*(( int*)
_temp93) == Cyc_Absyn_StructType: 0){ _LL164: _temp163=( struct _tuple0*)((
struct Cyc_Absyn_StructType_struct*) _temp93)->f1; goto _LL162; _LL162: _temp161=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp93)->f2; goto
_LL159; _LL159: _temp158=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp93)->f3; if( _temp158 == 0){ goto _LL107;} else{ _temp160=* _temp158; goto
_LL106;}} else{ goto _LL107;} _LL107: if(( unsigned int) _temp93 > 4u?*(( int*)
_temp93) == Cyc_Absyn_UnionType: 0){ _LL171: _temp170=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp93)->f1; goto _LL169; _LL169: _temp168=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp93)->f2; goto
_LL166; _LL166: _temp165=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp93)->f3; if( _temp165 == 0){ goto _LL109;} else{ _temp167=* _temp165; goto
_LL108;}} else{ goto _LL109;} _LL109: if(( unsigned int) _temp93 > 4u?*(( int*)
_temp93) == Cyc_Absyn_TunionType: 0){ _LL173: _temp172=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp93)->f1; _LL179: _temp178=( void*)
_temp172.tunion_info; if(*(( int*) _temp178) == Cyc_Absyn_KnownTunion){ _LL181:
_temp180=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*)
_temp178)->f1; goto _LL177;} else{ goto _LL111;} _LL177: _temp176=( struct Cyc_List_List*)
_temp172.targs; goto _LL175; _LL175: _temp174=( void*) _temp172.rgn; goto _LL110;}
else{ goto _LL111;} _LL111: if(( unsigned int) _temp93 > 4u?*(( int*) _temp93)
== Cyc_Absyn_TunionFieldType: 0){ _LL183: _temp182=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp93)->f1; _LL187: _temp186=( void*)
_temp182.field_info; if(*(( int*) _temp186) == Cyc_Absyn_KnownTunionfield){
_LL191: _temp190=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp186)->f1; goto _LL189; _LL189: _temp188=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp186)->f2; goto _LL185;} else{
goto _LL113;} _LL185: _temp184=( struct Cyc_List_List*) _temp182.targs; goto
_LL112;} else{ goto _LL113;} _LL113: goto _LL114; _LL96: return Cyc_Interface_check_public_type(
seen, obj, name, _temp125); _LL98: return Cyc_Interface_check_public_type( seen,
obj, name, _temp131); _LL100: return Cyc_Interface_check_public_type( seen, obj,
name, _temp136); _LL102: return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple1*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,
Cyc_Interface_get_type2, _temp148)? Cyc_Interface_check_public_type( seen, obj,
name, _temp150): 0; _LL104: return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,
Cyc_Interface_get_type1, _temp156); _LL106: if(( void*) _temp160->sc ==( void*)
Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,( struct _tagged_string)({
char* _temp192=( char*)"struct"; struct _tagged_string _temp193; _temp193.curr=
_temp192; _temp193.base= _temp192; _temp193.last_plus_one= _temp192 + 7;
_temp193;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp194= _temp160->name;
if( _temp194 == 0){ _throw( Null_Exception);} _temp194->v;})); return 0;} return((
int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct
_tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_types)(
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp161)? Cyc_Interface_check_public_structdecl(
seen, _temp160): 0; _LL108: if(( void*) _temp167->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp195=(
char*)"union"; struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base=
_temp195; _temp196.last_plus_one= _temp195 + 6; _temp196;}),( struct _tuple0*)({
struct Cyc_Core_Opt* _temp197= _temp167->name; if( _temp197 == 0){ _throw(
Null_Exception);} _temp197->v;})); return 0;} return(( int(*)( struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,(
void*(*)( void*)) Cyc_Core_identity, _temp168)? Cyc_Interface_check_public_uniondecl(
seen, _temp167): 0; _LL110: if(( void*) _temp180->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp198=(
char*)"[x]tunion"; struct _tagged_string _temp199; _temp199.curr= _temp198;
_temp199.base= _temp198; _temp199.last_plus_one= _temp198 + 10; _temp199;}),
_temp180->name); return 0;} return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_types)( seen, obj, name,( void*(*)( void*)) Cyc_Core_identity,
_temp176)? Cyc_Interface_check_public_tuniondecl( seen, _temp180): 0; _LL112:
if(( void*) _temp190->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err(
obj, name,( struct _tagged_string)({ char* _temp200=( char*)"[x]tunion"; struct
_tagged_string _temp201; _temp201.curr= _temp200; _temp201.base= _temp200;
_temp201.last_plus_one= _temp200 + 10; _temp201;}), _temp190->name); return 0;}
if(( void*) _temp190->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name,( struct _tagged_string)({ char* _temp202=( char*)"[x]tunion"; struct
_tagged_string _temp203; _temp203.curr= _temp202; _temp203.base= _temp202;
_temp203.last_plus_one= _temp202 + 10; _temp203;}), _temp190->name); return 0;}
return(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj,
struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_types)(
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp184)? Cyc_Interface_check_public_tuniondecl(
seen, _temp190): 0; _LL114: return 1; _LL94:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Interface_Seen* f4; } ;
static void Cyc_Interface_extract_structdecl( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple11 _temp206;
struct Cyc_Interface_Seen* _temp207; int _temp209; struct Cyc_Interface_Ienv*
_temp211; struct Cyc_Interface_Ienv* _temp213; struct _tuple11* _temp204= env;
_temp206=* _temp204; _LL214: _temp213= _temp206.f1; goto _LL212; _LL212:
_temp211= _temp206.f2; goto _LL210; _LL210: _temp209= _temp206.f3; goto _LL208;
_LL208: _temp207= _temp206.f4; goto _LL205; _LL205: { struct Cyc_Absyn_Structdecl*
_temp215=* dp; goto _LL216; _LL216: { void* _temp217=( void*) _temp215->sc;
_LL219: if( _temp217 ==( void*) Cyc_Absyn_Static){ goto _LL220;} else{ goto
_LL221;} _LL221: if( _temp217 ==( void*) Cyc_Absyn_Abstract){ goto _LL222;}
else{ goto _LL223;} _LL223: if( _temp217 ==( void*) Cyc_Absyn_Public){ goto
_LL224;} else{ goto _LL225;} _LL225: if( _temp217 ==( void*) Cyc_Absyn_ExternC){
goto _LL226;} else{ goto _LL227;} _LL227: if( _temp217 ==( void*) Cyc_Absyn_Extern){
goto _LL228;} else{ goto _LL218;} _LL220: if( _temp209? _temp215->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp229=( char*)"static";
struct _tagged_string _temp230; _temp230.curr= _temp229; _temp230.base= _temp229;
_temp230.last_plus_one= _temp229 + 7; _temp230;}),( struct _tagged_string)({
char* _temp231=( char*)"struct"; struct _tagged_string _temp232; _temp232.curr=
_temp231; _temp232.base= _temp231; _temp232.last_plus_one= _temp231 + 7;
_temp232;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp233= _temp215->name;
if( _temp233 == 0){ _throw( Null_Exception);} _temp233->v;}));} goto _LL218;
_LL222: if( _temp215->fields == 0){ if( _temp209){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp234=( char*)"abstract"; struct
_tagged_string _temp235; _temp235.curr= _temp234; _temp235.base= _temp234;
_temp235.last_plus_one= _temp234 + 9; _temp235;}),( struct _tagged_string)({
char* _temp236=( char*)"struct"; struct _tagged_string _temp237; _temp237.curr=
_temp236; _temp237.base= _temp236; _temp237.last_plus_one= _temp236 + 7;
_temp237;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp238= _temp215->name;
if( _temp238 == 0){ _throw( Null_Exception);} _temp238->v;}));}} else{ _temp215=({
struct Cyc_Absyn_Structdecl* _temp239=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp239[ 0]=* _temp215; _temp239;});
_temp215->fields= 0;} if( Cyc_Interface_check_public_structdecl( _temp207,
_temp215)){ _temp211->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp211->structdecls, x, _temp215);} goto _LL218; _LL224: if( _temp215->fields
== 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp240=( char*)"public";
struct _tagged_string _temp241; _temp241.curr= _temp240; _temp241.base= _temp240;
_temp241.last_plus_one= _temp240 + 7; _temp241;}),( struct _tagged_string)({
char* _temp242=( char*)"struct"; struct _tagged_string _temp243; _temp243.curr=
_temp242; _temp243.base= _temp242; _temp243.last_plus_one= _temp242 + 7;
_temp243;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp244= _temp215->name;
if( _temp244 == 0){ _throw( Null_Exception);} _temp244->v;})); _temp215=({
struct Cyc_Absyn_Structdecl* _temp245=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp245[ 0]=* _temp215; _temp245;});(
void*)( _temp215->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl(
_temp207, _temp215)){ _temp211->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp211->structdecls, x, _temp215);} goto _LL218; _LL226:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp246=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp246[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp247; _temp247.tag= Cyc_Core_InvalidArg; _temp247.f1=(
struct _tagged_string)({ char* _temp248=( char*)"extract_structdecl"; struct
_tagged_string _temp249; _temp249.curr= _temp248; _temp249.base= _temp248;
_temp249.last_plus_one= _temp248 + 19; _temp249;}); _temp247;}); _temp246;}));
_LL228: if( Cyc_Interface_check_public_structdecl( _temp207, _temp215)){
_temp213->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp213->structdecls, x, _temp215);} goto _LL218; _LL218:;}}} static void Cyc_Interface_extract_uniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple11 _temp252; struct Cyc_Interface_Seen* _temp253; int _temp255;
struct Cyc_Interface_Ienv* _temp257; struct Cyc_Interface_Ienv* _temp259; struct
_tuple11* _temp250= env; _temp252=* _temp250; _LL260: _temp259= _temp252.f1;
goto _LL258; _LL258: _temp257= _temp252.f2; goto _LL256; _LL256: _temp255=
_temp252.f3; goto _LL254; _LL254: _temp253= _temp252.f4; goto _LL251; _LL251: {
struct Cyc_Absyn_Uniondecl* _temp261=* dp; goto _LL262; _LL262: { void* _temp263=(
void*) _temp261->sc; _LL265: if( _temp263 ==( void*) Cyc_Absyn_Static){ goto
_LL266;} else{ goto _LL267;} _LL267: if( _temp263 ==( void*) Cyc_Absyn_Abstract){
goto _LL268;} else{ goto _LL269;} _LL269: if( _temp263 ==( void*) Cyc_Absyn_Public){
goto _LL270;} else{ goto _LL271;} _LL271: if( _temp263 ==( void*) Cyc_Absyn_ExternC){
goto _LL272;} else{ goto _LL273;} _LL273: if( _temp263 ==( void*) Cyc_Absyn_Extern){
goto _LL274;} else{ goto _LL264;} _LL266: if( _temp255? _temp261->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp275=( char*)"static";
struct _tagged_string _temp276; _temp276.curr= _temp275; _temp276.base= _temp275;
_temp276.last_plus_one= _temp275 + 7; _temp276;}),( struct _tagged_string)({
char* _temp277=( char*)"union"; struct _tagged_string _temp278; _temp278.curr=
_temp277; _temp278.base= _temp277; _temp278.last_plus_one= _temp277 + 6;
_temp278;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp279= _temp261->name;
if( _temp279 == 0){ _throw( Null_Exception);} _temp279->v;}));} goto _LL264;
_LL268: if( _temp261->fields == 0){ if( _temp255){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp280=( char*)"abstract"; struct
_tagged_string _temp281; _temp281.curr= _temp280; _temp281.base= _temp280;
_temp281.last_plus_one= _temp280 + 9; _temp281;}),( struct _tagged_string)({
char* _temp282=( char*)"union"; struct _tagged_string _temp283; _temp283.curr=
_temp282; _temp283.base= _temp282; _temp283.last_plus_one= _temp282 + 6;
_temp283;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp284= _temp261->name;
if( _temp284 == 0){ _throw( Null_Exception);} _temp284->v;}));}} else{ _temp261=({
struct Cyc_Absyn_Uniondecl* _temp285=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp285[ 0]=* _temp261; _temp285;});
_temp261->fields= 0;} if( Cyc_Interface_check_public_uniondecl( _temp253,
_temp261)){ _temp257->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp257->uniondecls, x, _temp261);} goto _LL264; _LL270: if( _temp261->fields
== 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp286=( char*)"public";
struct _tagged_string _temp287; _temp287.curr= _temp286; _temp287.base= _temp286;
_temp287.last_plus_one= _temp286 + 7; _temp287;}),( struct _tagged_string)({
char* _temp288=( char*)"union"; struct _tagged_string _temp289; _temp289.curr=
_temp288; _temp289.base= _temp288; _temp289.last_plus_one= _temp288 + 6;
_temp289;}),( struct _tuple0*)({ struct Cyc_Core_Opt* _temp290= _temp261->name;
if( _temp290 == 0){ _throw( Null_Exception);} _temp290->v;})); _temp261=({
struct Cyc_Absyn_Uniondecl* _temp291=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp291[ 0]=* _temp261; _temp291;});(
void*)( _temp261->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl(
_temp253, _temp261)){ _temp257->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data))
Cyc_Dict_insert)( _temp257->uniondecls, x, _temp261);} goto _LL264; _LL272:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp292=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp292[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp293; _temp293.tag= Cyc_Core_InvalidArg; _temp293.f1=(
struct _tagged_string)({ char* _temp294=( char*)"extract_uniondecl"; struct
_tagged_string _temp295; _temp295.curr= _temp294; _temp295.base= _temp294;
_temp295.last_plus_one= _temp294 + 18; _temp295;}); _temp293;}); _temp292;}));
_LL274: if( Cyc_Interface_check_public_uniondecl( _temp253, _temp261)){ _temp259->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp259->uniondecls, x,
_temp261);} goto _LL264; _LL264:;}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp298; struct Cyc_Interface_Seen* _temp299; int _temp301;
struct Cyc_Interface_Ienv* _temp303; struct Cyc_Interface_Ienv* _temp305; struct
_tuple11* _temp296= env; _temp298=* _temp296; _LL306: _temp305= _temp298.f1;
goto _LL304; _LL304: _temp303= _temp298.f2; goto _LL302; _LL302: _temp301=
_temp298.f3; goto _LL300; _LL300: _temp299= _temp298.f4; goto _LL297; _LL297: {
struct Cyc_Absyn_Tuniondecl* _temp307=* dp; goto _LL308; _LL308: { void*
_temp309=( void*) _temp307->sc; _LL311: if( _temp309 ==( void*) Cyc_Absyn_Static){
goto _LL312;} else{ goto _LL313;} _LL313: if( _temp309 ==( void*) Cyc_Absyn_Abstract){
goto _LL314;} else{ goto _LL315;} _LL315: if( _temp309 ==( void*) Cyc_Absyn_Public){
goto _LL316;} else{ goto _LL317;} _LL317: if( _temp309 ==( void*) Cyc_Absyn_ExternC){
goto _LL318;} else{ goto _LL319;} _LL319: if( _temp309 ==( void*) Cyc_Absyn_Extern){
goto _LL320;} else{ goto _LL310;} _LL312: if( _temp301? _temp307->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp321=( char*)"static";
struct _tagged_string _temp322; _temp322.curr= _temp321; _temp322.base= _temp321;
_temp322.last_plus_one= _temp321 + 7; _temp322;}),( struct _tagged_string)({
char* _temp323=( char*)"tunion"; struct _tagged_string _temp324; _temp324.curr=
_temp323; _temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 7;
_temp324;}), _temp307->name);} goto _LL310; _LL314: if( _temp307->fields == 0){
if( _temp301){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp325=( char*)"abstract"; struct _tagged_string _temp326; _temp326.curr=
_temp325; _temp326.base= _temp325; _temp326.last_plus_one= _temp325 + 9;
_temp326;}),( struct _tagged_string)({ char* _temp327=( char*)"tunion"; struct
_tagged_string _temp328; _temp328.curr= _temp327; _temp328.base= _temp327;
_temp328.last_plus_one= _temp327 + 7; _temp328;}), _temp307->name);}} else{
_temp307=({ struct Cyc_Absyn_Tuniondecl* _temp329=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp329[ 0]=* _temp307;
_temp329;}); _temp307->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp299, _temp307)){ _temp303->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp303->tuniondecls, x, _temp307);} goto _LL310; _LL316: if(
_temp307->fields == 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp330=( char*)"public"; struct _tagged_string _temp331; _temp331.curr=
_temp330; _temp331.base= _temp330; _temp331.last_plus_one= _temp330 + 7;
_temp331;}),( struct _tagged_string)({ char* _temp332=( char*)"tunion"; struct
_tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 7; _temp333;}), _temp307->name); _temp307=({
struct Cyc_Absyn_Tuniondecl* _temp334=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp334[ 0]=* _temp307; _temp334;});(
void*)( _temp307->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp299, _temp307)){ _temp303->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp303->tuniondecls, x, _temp307);} goto _LL310; _LL318:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp335=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp335[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp336; _temp336.tag= Cyc_Core_InvalidArg; _temp336.f1=(
struct _tagged_string)({ char* _temp337=( char*)"extract_tuniondecl"; struct
_tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 19; _temp338;}); _temp336;}); _temp335;}));
_LL320: if( Cyc_Interface_check_public_tuniondecl( _temp299, _temp307)){
_temp305->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp305->tuniondecls, x, _temp307);} goto _LL310; _LL310:;}}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp341; struct Cyc_Interface_Seen* _temp342; int _temp344;
struct Cyc_Interface_Ienv* _temp346; struct Cyc_Interface_Ienv* _temp348; struct
_tuple11* _temp339= env; _temp341=* _temp339; _LL349: _temp348= _temp341.f1;
goto _LL347; _LL347: _temp346= _temp341.f2; goto _LL345; _LL345: _temp344=
_temp341.f3; goto _LL343; _LL343: _temp342= _temp341.f4; goto _LL340; _LL340: {
struct Cyc_Absyn_Enumdecl* _temp350=* dp; goto _LL351; _LL351: { void* _temp352=(
void*) _temp350->sc; _LL354: if( _temp352 ==( void*) Cyc_Absyn_Static){ goto
_LL355;} else{ goto _LL356;} _LL356: if( _temp352 ==( void*) Cyc_Absyn_Abstract){
goto _LL357;} else{ goto _LL358;} _LL358: if( _temp352 ==( void*) Cyc_Absyn_Public){
goto _LL359;} else{ goto _LL360;} _LL360: if( _temp352 ==( void*) Cyc_Absyn_ExternC){
goto _LL361;} else{ goto _LL362;} _LL362: if( _temp352 ==( void*) Cyc_Absyn_Extern){
goto _LL363;} else{ goto _LL353;} _LL355: if( _temp344? _temp350->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp364=( char*)"static";
struct _tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 7; _temp365;}),( struct _tagged_string)({
char* _temp366=( char*)"enum"; struct _tagged_string _temp367; _temp367.curr=
_temp366; _temp367.base= _temp366; _temp367.last_plus_one= _temp366 + 5;
_temp367;}), _temp350->name);} goto _LL353; _LL357: if( _temp350->fields == 0){
if( _temp344){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp368=( char*)"abstract"; struct _tagged_string _temp369; _temp369.curr=
_temp368; _temp369.base= _temp368; _temp369.last_plus_one= _temp368 + 9;
_temp369;}),( struct _tagged_string)({ char* _temp370=( char*)"enum"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 5; _temp371;}), _temp350->name);}} else{
_temp350=({ struct Cyc_Absyn_Enumdecl* _temp372=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp372[ 0]=* _temp350;
_temp372;}); _temp350->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp342, _temp350)){ _temp346->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp346->enumdecls, x, _temp350);} goto _LL353; _LL359: if(
_temp350->fields == 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp373=( char*)"public"; struct _tagged_string _temp374; _temp374.curr=
_temp373; _temp374.base= _temp373; _temp374.last_plus_one= _temp373 + 7;
_temp374;}),( struct _tagged_string)({ char* _temp375=( char*)"enum"; struct
_tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 5; _temp376;}), _temp350->name); _temp350=({
struct Cyc_Absyn_Enumdecl* _temp377=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp377[ 0]=* _temp350; _temp377;});( void*)(
_temp350->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl(
_temp342, _temp350)){ _temp346->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp346->enumdecls, x, _temp350);} goto _LL353; _LL361:( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp378=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp378[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp379; _temp379.tag= Cyc_Core_InvalidArg; _temp379.f1=(
struct _tagged_string)({ char* _temp380=( char*)"extract_enumdecl"; struct
_tagged_string _temp381; _temp381.curr= _temp380; _temp381.base= _temp380;
_temp381.last_plus_one= _temp380 + 17; _temp381;}); _temp379;}); _temp378;}));
_LL363: if( Cyc_Interface_check_public_enumdecl( _temp342, _temp350)){ _temp348->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp348->enumdecls, x,
_temp350);} goto _LL353; _LL353:;}}} static void Cyc_Interface_extract_typedef(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple11 _temp384; struct Cyc_Interface_Seen* _temp385; int _temp387;
struct Cyc_Interface_Ienv* _temp389; struct Cyc_Interface_Ienv* _temp391; struct
_tuple11* _temp382= env; _temp384=* _temp382; _LL392: _temp391= _temp384.f1;
goto _LL390; _LL390: _temp389= _temp384.f2; goto _LL388; _LL388: _temp387=
_temp384.f3; goto _LL386; _LL386: _temp385= _temp384.f4; goto _LL383; _LL383:
if( Cyc_Interface_check_public_typedefdecl( _temp385, d)){ _temp389->typedefdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( _temp389->typedefdecls, x,
d);}} static void Cyc_Interface_extract_ordinarie( struct _tuple11* env, struct
_tagged_string* x, struct _tuple7* v){ struct _tuple11 _temp395; struct Cyc_Interface_Seen*
_temp396; int _temp398; struct Cyc_Interface_Ienv* _temp400; struct Cyc_Interface_Ienv*
_temp402; struct _tuple11* _temp393= env; _temp395=* _temp393; _LL403: _temp402=
_temp395.f1; goto _LL401; _LL401: _temp400= _temp395.f2; goto _LL399; _LL399:
_temp398= _temp395.f3; goto _LL397; _LL397: _temp396= _temp395.f4; goto _LL394;
_LL394: { void* _temp404=(* v).f1; goto _LL405; _LL405: { void* _temp406=
_temp404; void* _temp412; _LL408: if(*(( int*) _temp406) == Cyc_Tcenv_VarRes){
_LL413: _temp412=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp406)->f1; goto
_LL409;} else{ goto _LL410;} _LL410: goto _LL411; _LL409:{ void* _temp414=
_temp412; struct Cyc_Absyn_Fndecl* _temp422; struct Cyc_Absyn_Vardecl* _temp424;
_LL416: if(( unsigned int) _temp414 > 1u?*(( int*) _temp414) == Cyc_Absyn_Funname_b:
0){ _LL423: _temp422=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp414)->f1; goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int)
_temp414 > 1u?*(( int*) _temp414) == Cyc_Absyn_Global_b: 0){ _LL425: _temp424=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp414)->f1;
goto _LL419;} else{ goto _LL420;} _LL420: goto _LL421; _LL417: { struct Cyc_Absyn_Vardecl*
_temp429=({ struct Cyc_Absyn_Vardecl* _temp426=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp426->sc=( void*)(( void*)
_temp422->sc); _temp426->name= _temp422->name; _temp426->tq=({ struct Cyc_Absyn_Tqual
_temp428; _temp428.q_const= 0; _temp428.q_volatile= 0; _temp428.q_restrict= 0;
_temp428;}); _temp426->type=( void*)(( void*)({ struct Cyc_Core_Opt* _temp427=
_temp422->cached_typ; if( _temp427 == 0){ _throw( Null_Exception);} _temp427->v;}));
_temp426->initializer= 0; _temp426->rgn= 0; _temp426->attributes= 0; _temp426;});
goto _LL430; _LL430: _temp398= 0; _temp424= _temp429; goto _LL419;} _LL419: if(
_temp424->initializer != 0){ _temp424=({ struct Cyc_Absyn_Vardecl* _temp431=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp431[ 0]=* _temp424; _temp431;}); _temp424->initializer= 0;}{ void* _temp432=(
void*) _temp424->sc; _LL434: if( _temp432 ==( void*) Cyc_Absyn_Static){ goto
_LL435;} else{ goto _LL436;} _LL436: if( _temp432 ==( void*) Cyc_Absyn_Abstract){
goto _LL437;} else{ goto _LL438;} _LL438: if( _temp432 ==( void*) Cyc_Absyn_Public){
goto _LL439;} else{ goto _LL440;} _LL440: if( _temp432 ==( void*) Cyc_Absyn_ExternC){
goto _LL441;} else{ goto _LL442;} _LL442: if( _temp432 ==( void*) Cyc_Absyn_Extern){
goto _LL443;} else{ goto _LL433;} _LL435: if( _temp398? Cyc_Tcutil_is_function_type((
void*) _temp424->type): 0){ Cyc_Interface_body_err(( struct _tagged_string)({
char* _temp444=( char*)"static"; struct _tagged_string _temp445; _temp445.curr=
_temp444; _temp445.base= _temp444; _temp445.last_plus_one= _temp444 + 7;
_temp445;}), _temp424->name);} goto _LL433; _LL437:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp446=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp446[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp447; _temp447.tag= Cyc_Core_InvalidArg; _temp447.f1=(
struct _tagged_string)({ char* _temp448=( char*)"extract_ordinarie"; struct
_tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 18; _temp449;}); _temp447;}); _temp446;}));
_LL439: if( _temp398? Cyc_Tcutil_is_function_type(( void*) _temp424->type): 0){
Cyc_Interface_body_err(( struct _tagged_string)({ char* _temp450=( char*)"public";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 7; _temp451;}), _temp424->name);} if( Cyc_Interface_check_public_vardecl(
_temp396, _temp424)){ _temp400->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp400->vardecls, x, _temp424);} goto _LL433; _LL441: goto _LL443; _LL443: if(
Cyc_Interface_check_public_vardecl( _temp396, _temp424)){ _temp402->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp402->vardecls, x,
_temp424);} goto _LL433; _LL433:;} goto _LL415; _LL421: goto _LL415; _LL415:;}
goto _LL407; _LL411: goto _LL407; _LL407:;}}} struct _tuple12{ struct Cyc_Interface_I*
f1; int f2; struct Cyc_Interface_Seen* f3; } ; static void Cyc_Interface_extract_f(
struct _tuple12* env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){
struct _tuple12 _temp454; struct Cyc_Interface_Seen* _temp455; int _temp457;
struct Cyc_Interface_I* _temp459; struct _tuple12* _temp452= env_f; _temp454=*
_temp452; _LL460: _temp459= _temp454.f1; goto _LL458; _LL458: _temp457= _temp454.f2;
goto _LL456; _LL456: _temp455= _temp454.f3; goto _LL453; _LL453: { struct
_tuple11 _temp462=({ struct _tuple11 _temp461; _temp461.f1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp459->imports,
ns); _temp461.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp459->exports, ns); _temp461.f3=
_temp457; _temp461.f4= _temp455; _temp461;}); goto _LL463; _LL463:(( void(*)(
void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Structdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,&
_temp462, ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp462, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp462, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp462, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp462, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp462, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp465=({ struct _tuple12
_temp464; _temp464.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp464.f2= check_complete_defs; _temp464.f3= Cyc_Interface_new_seen();
_temp464;}); goto _LL466; _LL466:(( void(*)( void(* f)( struct _tuple12*, struct
Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct _tuple12* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,& _temp465, ae); return _temp465.f1;}
struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae){ return
Cyc_Interface_gen_extract( ae, 1);} inline static void Cyc_Interface_check_err(
struct _tagged_string* msg1, struct _tagged_string msg2){ Cyc_Tcdecl_merr( 0,
msg1, msg2);} struct _tuple13{ int f1; struct _tagged_string* f2; } ; struct
_tuple14{ int(* f1)( struct _tagged_string*, void*, void*); struct Cyc_Dict_Dict*
f2; struct _tuple13* f3; struct _tagged_string f4; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple14* env, struct _tagged_string* x, void* y1){ struct _tuple14
_temp469; struct _tagged_string _temp470; struct _tuple13* _temp472; struct Cyc_Dict_Dict*
_temp474; int(* _temp476)( struct _tagged_string*, void*, void*); struct
_tuple14* _temp467= env; _temp469=* _temp467; _LL477: _temp476= _temp469.f1;
goto _LL475; _LL475: _temp474= _temp469.f2; goto _LL473; _LL473: _temp472=
_temp469.f3; goto _LL471; _LL471: _temp470= _temp469.f4; goto _LL468; _LL468: {
struct _handler_cons _temp478; _push_handler(& _temp478);{ int _temp480= 0; if(
setjmp( _temp478.handler)){ _temp480= 1;} if( ! _temp480){{ void* _temp481=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp474, x); goto _LL482; _LL482: if( ! _temp476((* _temp472).f2, y1, _temp481)){(*
_temp472).f1= 0;}}; _pop_handler();} else{ void* _temp479=( void*) _exn_thrown;
void* _temp484= _temp479; _LL486: if( _temp484 == Cyc_Dict_Absent){ goto _LL487;}
else{ goto _LL488;} _LL488: goto _LL489; _LL487: Cyc_Interface_check_err((*
_temp472).f2,({ struct _tagged_string _temp490= _temp470; struct _tagged_string
_temp491=* x; xprintf("%.*s %.*s is missing", _temp490.last_plus_one - _temp490.curr,
_temp490.curr, _temp491.last_plus_one - _temp491.curr, _temp491.curr);}));(*
_temp472).f1= 0; goto _LL485; _LL489:( void) _throw( _temp484); _LL485:;}}}}
static void Cyc_Interface_incl_dict( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple14 _temp493=({ struct
_tuple14 _temp492; _temp492.f1= f; _temp492.f2= d2; _temp492.f3= genv; _temp492.f4=
objname; _temp492;}); goto _LL494; _LL494:(( void(*)( void(* f)( struct _tuple14*,
struct _tagged_string*, void*), struct _tuple14* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct _tuple14* env, struct _tagged_string* x, void*
y1)) Cyc_Interface_incl_dict_f,& _temp493, d1);} static int Cyc_Interface_incl_structdecl(
struct _tagged_string* msg, struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1){ struct Cyc_Absyn_Structdecl* _temp495= Cyc_Tcdecl_merge_structdecl( d0, d1,
0, msg); goto _LL496; _LL496: if( _temp495 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*)
d0 != _temp495){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp498=
Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp497= d1->name;
if( _temp497 == 0){ _throw( Null_Exception);} _temp497->v;})); xprintf("declaration of struct %.*s discloses too much information",
_temp498.last_plus_one - _temp498.curr, _temp498.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct _tagged_string* msg, struct Cyc_Absyn_Uniondecl*
d0, struct Cyc_Absyn_Uniondecl* d1){ struct Cyc_Absyn_Uniondecl* _temp499= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); goto _LL500; _LL500: if( _temp499 == 0){ return 0;} if(( struct
Cyc_Absyn_Uniondecl*) d0 != _temp499){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp502= Cyc_Absynpp_qvar2string(( struct _tuple0*)({ struct Cyc_Core_Opt*
_temp501= d1->name; if( _temp501 == 0){ _throw( Null_Exception);} _temp501->v;}));
xprintf("declaration of union %.*s discloses too much information", _temp502.last_plus_one
- _temp502.curr, _temp502.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_tuniondecl(
struct _tagged_string* msg, struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl*
d1){ struct Cyc_Absyn_Tuniondecl* _temp503= Cyc_Tcdecl_merge_tuniondecl( d0, d1,
0, msg); goto _LL504; _LL504: if( _temp503 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*)
d0 != _temp503){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp505=
Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp505.last_plus_one - _temp505.curr, _temp505.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct _tagged_string* msg, struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1){ struct Cyc_Absyn_Enumdecl* _temp506= Cyc_Tcdecl_merge_enumdecl(
d0, d1, 0, msg); goto _LL507; _LL507: if( _temp506 == 0){ return 0;} if(( struct
Cyc_Absyn_Enumdecl*) d0 != _temp506){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp508= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp508.last_plus_one - _temp508.curr, _temp508.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct _tagged_string* msg, struct Cyc_Absyn_Vardecl*
d0, struct Cyc_Absyn_Vardecl* d1){ struct Cyc_Absyn_Vardecl* _temp509= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL510; _LL510: if( _temp509 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp509){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp511= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of var %.*s discloses too much information",
_temp511.last_plus_one - _temp511.curr, _temp511.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct _tagged_string* msg, struct
Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1){ struct Cyc_Absyn_Typedefdecl*
_temp512= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); goto _LL513; _LL513:
if( _temp512 == 0){ return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 !=
_temp512){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp514= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_temp514.last_plus_one - _temp514.curr, _temp514.curr);})); return 0;} return 1;}
static void Cyc_Interface_incl_ns( struct _tuple13* env, struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2){(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp515=(
char*)"struct"; struct _tagged_string _temp516; _temp516.curr= _temp515;
_temp516.base= _temp515; _temp516.last_plus_one= _temp515 + 7; _temp516;}), Cyc_Interface_incl_structdecl,
ie1->structdecls, ie2->structdecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp517=( char*)"union";
struct _tagged_string _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 6; _temp518;}), Cyc_Interface_incl_uniondecl,
ie1->uniondecls, ie2->uniondecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp519=(
char*)"tunion"; struct _tagged_string _temp520; _temp520.curr= _temp519;
_temp520.base= _temp519; _temp520.last_plus_one= _temp519 + 7; _temp520;}), Cyc_Interface_incl_tuniondecl,
ie1->tuniondecls, ie2->tuniondecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp521=( char*)"enum";
struct _tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 5; _temp522;}), Cyc_Interface_incl_enumdecl,
ie1->enumdecls, ie2->enumdecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp523=( char*)"variable";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 9; _temp524;}), Cyc_Interface_incl_vardecl,
ie1->vardecls, ie2->vardecls);} int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct _handler_cons
_temp525; _push_handler(& _temp525);{ int _temp527= 0; if( setjmp( _temp525.handler)){
_temp527= 1;} if( ! _temp527){{ struct _tuple13 env=({ struct _tuple13 _temp539;
_temp539.f1= 1; _temp539.f2= 0; _temp539;}); if( info != 0){ env.f2=({ struct
_tagged_string* _temp528=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp528[ 0]=({ struct _tagged_string _temp531=({ struct
_tuple3* _temp529= info; if( _temp529 == 0){ _throw( Null_Exception);}* _temp529;}).f2;
struct _tagged_string _temp532=({ struct _tuple3* _temp530= info; if( _temp530
== 0){ _throw( Null_Exception);}* _temp530;}).f1; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp531.last_plus_one - _temp531.curr, _temp531.curr, _temp532.last_plus_one -
_temp532.curr, _temp532.curr);}); _temp528;});}(( void(*)( void(* f)( struct
_tuple13*, struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct
_tuple13* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)(
Cyc_Interface_incl_ns,& env, i2->exports, i1->exports); if( info != 0){ env.f2=({
struct _tagged_string* _temp533=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp533[ 0]=({ struct _tagged_string _temp536=({
struct _tuple3* _temp534= info; if( _temp534 == 0){ _throw( Null_Exception);}*
_temp534;}).f1; struct _tagged_string _temp537=({ struct _tuple3* _temp535= info;
if( _temp535 == 0){ _throw( Null_Exception);}* _temp535;}).f2; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp536.last_plus_one - _temp536.curr, _temp536.curr, _temp537.last_plus_one -
_temp537.curr, _temp537.curr);}); _temp533;});}(( void(*)( void(* f)( struct
_tuple13*, struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct
_tuple13* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)(
Cyc_Interface_incl_ns,& env, i1->imports, i2->imports);{ int _temp538= env.f1;
_npop_handler( 0u); return _temp538;}}; _pop_handler();} else{ void* _temp526=(
void*) _exn_thrown; void* _temp541= _temp526; _LL543: if( _temp541 == Cyc_Tcdecl_Incompatible){
goto _LL544;} else{ goto _LL545;} _LL545: goto _LL546; _LL544: return 0; _LL546:(
void) _throw( _temp541); _LL542:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); void**(* f6)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*, int*);
struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15 _temp549;
struct _tagged_string* _temp550; void**(* _temp552)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*, int*); void**(* _temp554)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*); struct Cyc_Dict_Dict* _temp556; struct Cyc_Dict_Dict*
_temp558; struct Cyc_Dict_Dict* _temp560; struct Cyc_Dict_Dict** _temp562; int
_temp563; int* _temp565; struct _tuple15* _temp547= env; _temp549=* _temp547;
_LL564: _temp563= _temp549.f1; _temp565=&(* _temp547).f1; goto _LL561; _LL561:
_temp560= _temp549.f2; _temp562=&(* _temp547).f2; goto _LL559; _LL559: _temp558=
_temp549.f3; goto _LL557; _LL557: _temp556= _temp549.f4; goto _LL555; _LL555:
_temp554= _temp549.f5; goto _LL553; _LL553: _temp552= _temp549.f6; goto _LL551;
_LL551: _temp550= _temp549.f7; goto _LL548; _LL548: { void** y;{ struct
_handler_cons _temp566; _push_handler(& _temp566);{ int _temp568= 0; if( setjmp(
_temp566.handler)){ _temp568= 1;} if( ! _temp568){{ void** _temp569=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp558,
x); goto _LL570; _LL570: { void** _temp571= _temp554( _temp569, y2, 0, _temp550);
goto _LL572; _LL572: if( !(( unsigned int) _temp571)){* _temp565= 0;
_npop_handler( 0u); return;} y=( void**)({ void** _temp573= _temp571; if(
_temp573 == 0){ _throw( Null_Exception);} _temp573;});}}; _pop_handler();} else{
void* _temp567=( void*) _exn_thrown; void* _temp575= _temp567; _LL577: if(
_temp575 == Cyc_Dict_Absent){ goto _LL578;} else{ goto _LL579;} _LL579: goto
_LL580; _LL578: y= y2; goto _LL576; _LL580:( void) _throw( _temp575); _LL576:;}}}{
struct _handler_cons _temp581; _push_handler(& _temp581);{ int _temp583= 0; if(
setjmp( _temp581.handler)){ _temp583= 1;} if( ! _temp583){{ void** _temp584=((
void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp556, x); goto _LL585; _LL585: { int* _temp587=({ int* _temp586=( int*)
GC_malloc_atomic( sizeof( int)); _temp586[ 0]= 1; _temp586;}); goto _LL588;
_LL588: { void** _temp589= _temp552( _temp584, y, 0, _temp550, _temp587); goto
_LL590; _LL590: if( !(* _temp587)){* _temp565= 0;} if(( unsigned int) _temp589){*
_temp562=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, void** data)) Cyc_Dict_insert)(* _temp562, x,( void**)({
void** _temp591= _temp589; if( _temp591 == 0){ _throw( Null_Exception);}
_temp591;}));}}}}; _pop_handler();} else{ void* _temp582=( void*) _exn_thrown;
void* _temp593= _temp582; _LL595: if( _temp593 == Cyc_Dict_Absent){ goto _LL596;}
else{ goto _LL597;} _LL597: goto _LL598; _LL596: if(* _temp565){* _temp562=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
void** data)) Cyc_Dict_insert)(* _temp562, x, y);} goto _LL594; _LL598:( void)
_throw( _temp593); _LL594:;}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*, struct
_tagged_string*), void**(* sub_f)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*, int*), struct _tagged_string* msg){ struct _tuple15
_temp600=({ struct _tuple15 _temp599; _temp599.f1= 1; _temp599.f2= dic1;
_temp599.f3= dic1; _temp599.f4= excl; _temp599.f5= merge_f; _temp599.f6= sub_f;
_temp599.f7= msg; _temp599;}); goto _LL601; _LL601:(( void(*)( void(* f)( struct
_tuple15*, struct _tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple15* env, struct _tagged_string* x,
void** y2)) Cyc_Interface_compat_merge_dict_f,& _temp600, dic2); if( _temp600.f1){
return({ struct Cyc_Core_Opt* _temp602=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp602->v=( void*) _temp600.f2; _temp602;});}
else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_compat_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct Cyc_Interface_Ienv*
iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt* _temp603=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Structdecl*(* sub_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
Cyc_Tcdecl_sub_structdecl, msg); goto _LL604; _LL604: { struct Cyc_Core_Opt*
_temp605=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Uniondecl*(* sub_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*, struct _tagged_string*,
int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->uniondecls,
ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl, Cyc_Tcdecl_sub_uniondecl,
msg); goto _LL606; _LL606: { struct Cyc_Core_Opt* _temp607=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Tuniondecl*(* sub_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
Cyc_Tcdecl_sub_tuniondecl, msg); goto _LL608; _LL608: { struct Cyc_Core_Opt*
_temp609=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Enumdecl*(* sub_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*, struct _tagged_string*,
int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->enumdecls,
ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl, Cyc_Tcdecl_sub_enumdecl,
msg); goto _LL610; _LL610: { struct Cyc_Core_Opt* _temp611=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Typedefdecl*(* sub_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
Cyc_Tcdecl_sub_typedefdecl, msg); goto _LL612; _LL612: { struct Cyc_Core_Opt*
_temp613=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Vardecl*(* sub_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*,
int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->vardecls,
ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl, Cyc_Tcdecl_sub_vardecl,
msg); goto _LL614; _LL614: if((((( !(( unsigned int) _temp603)? 1: !((
unsigned int) _temp605))? 1: !(( unsigned int) _temp607))? 1: !(( unsigned int)
_temp609))? 1: !(( unsigned int) _temp611))? 1: !(( unsigned int) _temp613)){
return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp615=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp615->structdecls=( struct
Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp621= _temp603; if( _temp621 == 0){
_throw( Null_Exception);} _temp621->v;}); _temp615->uniondecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp620= _temp605; if( _temp620 == 0){ _throw(
Null_Exception);} _temp620->v;}); _temp615->tuniondecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp619= _temp607; if( _temp619 == 0){ _throw(
Null_Exception);} _temp619->v;}); _temp615->enumdecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp618= _temp609; if( _temp618 == 0){ _throw(
Null_Exception);} _temp618->v;}); _temp615->typedefdecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp617= _temp611; if( _temp617 == 0){ _throw(
Null_Exception);} _temp617->v;}); _temp615->vardecls=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp616= _temp613; if( _temp616 == 0){ _throw(
Null_Exception);} _temp616->v;}); _temp615;});}}}}}}} static struct Cyc_Interface_Ienv*
Cyc_Interface_lazy_new_ienv(){ static struct Cyc_Interface_Ienv* ie= 0; if( ie
== 0){ ie=( struct Cyc_Interface_Ienv*) Cyc_Interface_new_ienv();} return(
struct Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp622= ie; if(
_temp622 == 0){ _throw( Null_Exception);} _temp622;});} struct _tuple16{ int f1;
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4;
struct _tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f( struct
_tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct
_tuple16 _temp625; struct _tagged_string* _temp626; struct Cyc_Dict_Dict*
_temp628; struct Cyc_Dict_Dict* _temp630; struct Cyc_Dict_Dict* _temp632; struct
Cyc_Dict_Dict** _temp634; int _temp635; int* _temp637; struct _tuple16* _temp623=
env; _temp625=* _temp623; _LL636: _temp635= _temp625.f1; _temp637=&(* _temp623).f1;
goto _LL633; _LL633: _temp632= _temp625.f2; _temp634=&(* _temp623).f2; goto
_LL631; _LL631: _temp630= _temp625.f3; goto _LL629; _LL629: _temp628= _temp625.f4;
goto _LL627; _LL627: _temp626= _temp625.f5; goto _LL624; _LL624: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp638;
_push_handler(& _temp638);{ int _temp640= 0; if( setjmp( _temp638.handler)){
_temp640= 1;} if( ! _temp640){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp628, ns);;
_pop_handler();} else{ void* _temp639=( void*) _exn_thrown; void* _temp642=
_temp639; _LL644: if( _temp642 == Cyc_Dict_Absent){ goto _LL645;} else{ goto
_LL646;} _LL646: goto _LL647; _LL645: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL643; _LL647:( void) _throw( _temp642); _LL643:;}}}{ struct _handler_cons
_temp648; _push_handler(& _temp648);{ int _temp650= 0; if( setjmp( _temp648.handler)){
_temp650= 1;} if( ! _temp650){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp630, ns);{ struct Cyc_Interface_Ienv*
_temp651= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp626); goto _LL652; _LL652: if( _temp651 == 0){* _temp637= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv*
_temp653= _temp651; if( _temp653 == 0){ _throw( Null_Exception);} _temp653;});};
_pop_handler();} else{ void* _temp649=( void*) _exn_thrown; void* _temp655=
_temp649; _LL657: if( _temp655 == Cyc_Dict_Absent){ goto _LL658;} else{ goto
_LL659;} _LL659: goto _LL660; _LL658: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL656; _LL660:( void) _throw( _temp655); _LL656:;}}}{ struct Cyc_Interface_Ienv*
_temp661= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp626); goto
_LL662; _LL662: if( _temp661 == 0){* _temp637= 0;} else{ if(* _temp637){*
_temp634=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp634, ns,( struct
Cyc_Interface_Ienv*)({ struct Cyc_Interface_Ienv* _temp663= _temp661; if(
_temp663 == 0){ _throw( Null_Exception);} _temp663;}));}}}}} struct Cyc_Core_Opt*
Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct _tagged_string* msg){ struct _tuple16
_temp665=({ struct _tuple16 _temp664; _temp664.f1= 1; _temp664.f2=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp664.f3= dic1; _temp664.f4= excl; _temp664.f5= msg;
_temp664;}); goto _LL666; _LL666:(( void(*)( void(* f)( struct _tuple16*, struct
Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,& _temp665, dic2); if(
_temp665.f1){ return({ struct Cyc_Core_Opt* _temp667=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp667->v=( void*) _temp665.f2;
_temp667;});} else{ return 0;}} struct _tuple17{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct _tagged_string f4; struct _tagged_string*
f5; } ; void Cyc_Interface_disj_merge_dict_f( struct _tuple17* env, struct
_tagged_string* x, void* y){ struct _tuple17 _temp670; struct _tagged_string*
_temp671; struct _tagged_string _temp673; struct Cyc_Dict_Dict* _temp675; struct
Cyc_Dict_Dict* _temp677; struct Cyc_Dict_Dict** _temp679; int _temp680; int*
_temp682; struct _tuple17* _temp668= env; _temp670=* _temp668; _LL681: _temp680=
_temp670.f1; _temp682=&(* _temp668).f1; goto _LL678; _LL678: _temp677= _temp670.f2;
_temp679=&(* _temp668).f2; goto _LL676; _LL676: _temp675= _temp670.f3; goto
_LL674; _LL674: _temp673= _temp670.f4; goto _LL672; _LL672: _temp671= _temp670.f5;
goto _LL669; _LL669: if((( int(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_member)( _temp675, x)){ Cyc_Interface_check_err(
_temp671,({ struct _tagged_string _temp683= _temp673; struct _tagged_string
_temp684=* x; xprintf("%.*s %.*s is exported more than once", _temp683.last_plus_one
- _temp683.curr, _temp683.curr, _temp684.last_plus_one - _temp684.curr, _temp684.curr);}));*
_temp682= 0;} else{ if(* _temp682){* _temp679=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(*
_temp679, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg){ struct _tuple17 _temp686=({ struct _tuple17 _temp685;
_temp685.f1= 1; _temp685.f2= dic1; _temp685.f3= dic1; _temp685.f4= t; _temp685.f5=
msg; _temp685;}); goto _LL687; _LL687:(( void(*)( void(* f)( struct _tuple17*,
struct _tagged_string*, void*), struct _tuple17* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct _tuple17* env, struct _tagged_string* x, void*
y)) Cyc_Interface_disj_merge_dict_f,& _temp686, dic2); if( _temp686.f1){ return({
struct Cyc_Core_Opt* _temp688=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp688->v=( void*) _temp686.f2; _temp688;});} else{ return 0;}}
struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_string* msg){ struct Cyc_Interface_Ienv*
_temp718=({ struct Cyc_Interface_Ienv* _temp689=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp689->structdecls=({ struct
Cyc_Core_Opt* _temp715=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,( struct
_tagged_string)({ char* _temp713=( char*)"struct"; struct _tagged_string
_temp714; _temp714.curr= _temp713; _temp714.base= _temp713; _temp714.last_plus_one=
_temp713 + 7; _temp714;}), msg); goto _LL716; _LL716: if( _temp715 == 0){ return
0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp717= _temp715; if(
_temp717 == 0){ _throw( Null_Exception);} _temp717->v;});}); _temp689->uniondecls=({
struct Cyc_Core_Opt* _temp710=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls,( struct
_tagged_string)({ char* _temp708=( char*)"union"; struct _tagged_string _temp709;
_temp709.curr= _temp708; _temp709.base= _temp708; _temp709.last_plus_one=
_temp708 + 6; _temp709;}), msg); goto _LL711; _LL711: if( _temp710 == 0){ return
0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp712= _temp710; if(
_temp712 == 0){ _throw( Null_Exception);} _temp712->v;});}); _temp689->tuniondecls=({
struct Cyc_Core_Opt* _temp705=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->tuniondecls, ie2->tuniondecls,(
struct _tagged_string)({ char* _temp703=( char*)"[x]tunion"; struct
_tagged_string _temp704; _temp704.curr= _temp703; _temp704.base= _temp703;
_temp704.last_plus_one= _temp703 + 10; _temp704;}), msg); goto _LL706; _LL706:
if( _temp705 == 0){ return 0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp707= _temp705; if( _temp707 == 0){ _throw( Null_Exception);} _temp707->v;});});
_temp689->enumdecls=({ struct Cyc_Core_Opt* _temp700=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,(
struct _tagged_string)({ char* _temp698=( char*)"enum"; struct _tagged_string
_temp699; _temp699.curr= _temp698; _temp699.base= _temp698; _temp699.last_plus_one=
_temp698 + 5; _temp699;}), msg); goto _LL701; _LL701: if( _temp700 == 0){ return
0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt* _temp702= _temp700; if(
_temp702 == 0){ _throw( Null_Exception);} _temp702->v;});}); _temp689->typedefdecls=({
struct Cyc_Core_Opt* _temp695=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(*
merge_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct Cyc_Absyn_Typedefdecl*(*
sub_f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, Cyc_Tcdecl_sub_typedefdecl, msg); goto _LL696;
_LL696: if( _temp695 == 0){ return 0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp697= _temp695; if( _temp697 == 0){ _throw( Null_Exception);} _temp697->v;});});
_temp689->vardecls=({ struct Cyc_Core_Opt* _temp692=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls,(
struct _tagged_string)({ char* _temp690=( char*)"variable"; struct
_tagged_string _temp691; _temp691.curr= _temp690; _temp691.base= _temp690;
_temp691.last_plus_one= _temp690 + 9; _temp691;}), msg); goto _LL693; _LL693:
if( _temp692 == 0){ return 0;}( struct Cyc_Dict_Dict*)({ struct Cyc_Core_Opt*
_temp694= _temp692; if( _temp694 == 0){ _throw( Null_Exception);} _temp694->v;});});
_temp689;}); goto _LL719; _LL719: return _temp718;} struct _tuple18{ int f1;
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; }
; void Cyc_Interface_disj_merge_ns_f( struct _tuple18* env, struct Cyc_List_List*
ns, struct Cyc_Interface_Ienv* ie2){ struct _tuple18 _temp722; struct
_tagged_string* _temp723; struct Cyc_Dict_Dict* _temp725; struct Cyc_Dict_Dict*
_temp727; struct Cyc_Dict_Dict** _temp729; int _temp730; int* _temp732; struct
_tuple18* _temp720= env; _temp722=* _temp720; _LL731: _temp730= _temp722.f1;
_temp732=&(* _temp720).f1; goto _LL728; _LL728: _temp727= _temp722.f2; _temp729=&(*
_temp720).f2; goto _LL726; _LL726: _temp725= _temp722.f3; goto _LL724; _LL724:
_temp723= _temp722.f4; goto _LL721; _LL721: { struct Cyc_Interface_Ienv* ie1;{
struct _handler_cons _temp733; _push_handler(& _temp733);{ int _temp735= 0; if(
setjmp( _temp733.handler)){ _temp735= 1;} if( ! _temp735){ ie1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp725,
ns);; _pop_handler();} else{ void* _temp734=( void*) _exn_thrown; void* _temp737=
_temp734; _LL739: if( _temp737 == Cyc_Dict_Absent){ goto _LL740;} else{ goto
_LL741;} _LL741: goto _LL742; _LL740: ie1= Cyc_Interface_lazy_new_ienv(); goto
_LL738; _LL742:( void) _throw( _temp737); _LL738:;}}}{ struct Cyc_Interface_Ienv*
_temp743= Cyc_Interface_disj_merge_ienv( ie1, ie2, _temp723); goto _LL744;
_LL744: if( _temp743 == 0){* _temp732= 0;} else{ if(* _temp732){* _temp729=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp729, ns,( struct Cyc_Interface_Ienv*)({
struct Cyc_Interface_Ienv* _temp745= _temp743; if( _temp745 == 0){ _throw(
Null_Exception);} _temp745;}));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string*
msg){ struct _tuple18 _temp747=({ struct _tuple18 _temp746; _temp746.f1= 1;
_temp746.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp746.f3=
dic1; _temp746.f4= msg; _temp746;}); goto _LL748; _LL748:(( void(*)( void(* f)(
struct _tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple18* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp747, dic2); if( _temp747.f1){ return({ struct Cyc_Core_Opt* _temp749=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp749->v=(
void*) _temp747.f2; _temp749;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string*
_temp750=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp750[ 0]=({ struct _tagged_string _temp753=({ struct _tuple3* _temp751= info;
if( _temp751 == 0){ _throw( Null_Exception);}* _temp751;}).f1; struct
_tagged_string _temp754=({ struct _tuple3* _temp752= info; if( _temp752 == 0){
_throw( Null_Exception);}* _temp752;}).f2; xprintf("merging exports of %.*s and %.*s,",
_temp753.last_plus_one - _temp753.curr, _temp753.curr, _temp754.last_plus_one -
_temp754.curr, _temp754.curr);}); _temp750;});}{ struct Cyc_Core_Opt* _temp755=
Cyc_Interface_disj_merge_ns( i1->exports, i2->exports, msg); goto _LL756; _LL756:
if( _temp755 == 0){ return 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp757=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp757[ 0]=({ struct _tagged_string _temp760=({ struct _tuple3* _temp758= info;
if( _temp758 == 0){ _throw( Null_Exception);}* _temp758;}).f1; struct
_tagged_string _temp761=({ struct _tuple3* _temp759= info; if( _temp759 == 0){
_throw( Null_Exception);}* _temp759;}).f2; xprintf("merging imports of %.*s and %.*s,",
_temp760.last_plus_one - _temp760.curr, _temp760.curr, _temp761.last_plus_one -
_temp761.curr, _temp761.curr);}); _temp757;});}{ struct Cyc_Core_Opt* _temp763=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp762= _temp755; if( _temp762 == 0){ _throw(
Null_Exception);} _temp762->v;}), msg); goto _LL764; _LL764: if( _temp763 == 0){
return 0;} return({ struct Cyc_Interface_I* _temp765=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp765->imports=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp767= _temp763; if( _temp767 == 0){ _throw(
Null_Exception);} _temp767->v;}); _temp765->exports=( struct Cyc_Dict_Dict*)({
struct Cyc_Core_Opt* _temp766= _temp755; if( _temp766 == 0){ _throw(
Null_Exception);} _temp766->v;}); _temp765;});}}} static struct Cyc_List_List*
Cyc_Interface_add_namespace( struct Cyc_List_List* tds, struct Cyc_List_List* ns){
if( ns == 0){ return tds;} return({ struct Cyc_List_List* _temp768=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp768->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp769=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp769[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp770; _temp770.tag= Cyc_Absyn_Namespace_d;
_temp770.f1=( struct _tagged_string*)({ struct Cyc_List_List* _temp771= ns; if(
_temp771 == 0){ _throw( Null_Exception);} _temp771->hd;}); _temp770.f2= Cyc_Interface_add_namespace(
tds,({ struct Cyc_List_List* _temp772= ns; if( _temp772 == 0){ _throw(
Null_Exception);} _temp772->tl;})); _temp770;}); _temp769;}), 0); _temp768->tl=
0; _temp768;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl(
struct _tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp773=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp773->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp774=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp774[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp775; _temp775.tag= Cyc_Absyn_Struct_d; _temp775.f1=
d; _temp775;}); _temp774;}), 0); _temp773->tl= tds; _temp773;});} static struct
Cyc_List_List* Cyc_Interface_add_structdecl_header( struct _tagged_string* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({ struct Cyc_Absyn_Structdecl*
_temp776=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp776[ 0]=* d; _temp776;}); d->fields= 0;( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp777=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp777->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp778=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp778[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp779; _temp779.tag= Cyc_Absyn_Struct_d; _temp779.f1=
d; _temp779;}); _temp778;}), 0); _temp777->tl= tds; _temp777;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp780=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp780->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp781=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp781[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp782; _temp782.tag= Cyc_Absyn_Union_d;
_temp782.f1= d; _temp782;}); _temp781;}), 0); _temp780->tl= tds; _temp780;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp783=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp783[ 0]=* d; _temp783;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp784->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp785=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp785[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp786; _temp786.tag= Cyc_Absyn_Union_d;
_temp786.f1= d; _temp786;}); _temp785;}), 0); _temp784->tl= tds; _temp784;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp787->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp788=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp788[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp789; _temp789.tag= Cyc_Absyn_Tunion_d;
_temp789.f1= d; _temp789;}); _temp788;}), 0); _temp787->tl= tds; _temp787;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp790=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp790[ 0]=* d; _temp790;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp791=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp791->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp792=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp792[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp793; _temp793.tag= Cyc_Absyn_Tunion_d;
_temp793.f1= d; _temp793;}); _temp792;}), 0); _temp791->tl= tds; _temp791;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp794=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp794->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp795=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp795[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp796; _temp796.tag= Cyc_Absyn_Enum_d;
_temp796.f1= d; _temp796;}); _temp795;}), 0); _temp794->tl= tds; _temp794;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp797=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp797[ 0]=* d; _temp797;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp798->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp799=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp799[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp800; _temp800.tag= Cyc_Absyn_Enum_d;
_temp800.f1= d; _temp800;}); _temp799;}), 0); _temp798->tl= tds; _temp798;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp801->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp802=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp802[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp803; _temp803.tag= Cyc_Absyn_Typedef_d;
_temp803.f1= d; _temp803;}); _temp802;}), 0); _temp801->tl= tds; _temp801;});}
static struct Cyc_List_List* Cyc_Interface_add_var( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp804=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp804->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp805=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp805[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp806; _temp806.tag= Cyc_Absyn_Var_d;
_temp806.f1= d; _temp806;}); _temp805;}), 0); _temp804->tl= tds; _temp804;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp807= 0; goto _LL808; _LL808: _temp807=(( struct Cyc_List_List*(*)( struct
Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp807);
_temp807=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp807); _temp807=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp807); _temp807=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp807); _temp807=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp807); _temp807= Cyc_Interface_add_namespace(
_temp807, ns); Cyc_Absynpp_decllist2file( _temp807, f);} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp809= 0; goto _LL810; _LL810: _temp809=(( struct
Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*, struct
Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct
Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp809); _temp809=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp809); _temp809= Cyc_Interface_add_namespace( _temp809, ns); Cyc_Absynpp_decllist2file(
_temp809, f);} static void Cyc_Interface_print_ns_decls( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp811= 0; goto _LL812; _LL812: _temp811=(( struct Cyc_List_List*(*)( struct
Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp811);
_temp811=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp811); _temp811=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp811); _temp811=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp811); _temp811=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_var,
ie->vardecls, _temp811); _temp811=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp811); _temp811= Cyc_Interface_add_namespace(
_temp811, ns); Cyc_Absynpp_decllist2file( _temp811, f);} void Cyc_Interface_print(
struct Cyc_Interface_I* i, struct Cyc_Stdio___sFILE* f){ fprintf( f,"/****** needed (headers) ******/\n");((
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp813= Cyc_Parse_parse_file( f);
goto _LL814; _LL814: Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv*
_temp815= Cyc_Tcenv_tc_init(); goto _LL816; _LL816: Cyc_Tc_tc( _temp815, 0, 0,
_temp813); return Cyc_Interface_gen_extract( _temp815->ae, 0);}}}