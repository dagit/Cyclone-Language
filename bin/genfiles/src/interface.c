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
for( 0; l != 0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ if( ! Cyc_Interface_check_public_type(
seen, obj, name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){
_temp17= 0;}} return _temp17;} static int Cyc_Interface_check_public_structdecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp19; _push_handler(& _temp19);{ int _temp21= 0; if( setjmp(
_temp19.handler)){ _temp21= 1;} if( ! _temp21){{ int _temp22=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp22;}; _pop_handler();} else{ void* _temp20=( void*) _exn_thrown;
void* _temp24= _temp20; _LL26: if( _temp24 == Cyc_Dict_Absent){ goto _LL27;}
else{ goto _LL28;} _LL28: goto _LL29; _LL27: { int _temp30= 1; goto _LL31; _LL31:
seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp30); if( d->fields != 0){
struct Cyc_List_List* _temp32=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL33; _LL33: for( 0; _temp32 != 0; _temp32=((
struct Cyc_List_List*) _check_null( _temp32))->tl){ if( ! Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp34=( char*)"struct"; struct
_tagged_string _temp35; _temp35.curr= _temp34; _temp35.base= _temp34; _temp35.last_plus_one=
_temp34 + 7; _temp35;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp32))->hd)->type)){ _temp30= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp30);
return _temp30;} _LL29:( void) _throw( _temp24); _LL25:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl* d){ struct
_handler_cons _temp36; _push_handler(& _temp36);{ int _temp38= 0; if( setjmp(
_temp36.handler)){ _temp38= 1;} if( ! _temp38){{ int _temp39=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp39;}; _pop_handler();} else{ void* _temp37=( void*) _exn_thrown;
void* _temp41= _temp37; _LL43: if( _temp41 == Cyc_Dict_Absent){ goto _LL44;}
else{ goto _LL45;} _LL45: goto _LL46; _LL44: { int _temp47= 1; goto _LL48; _LL48:
seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp47); if( d->fields != 0){
struct Cyc_List_List* _temp49=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL50; _LL50: for( 0; _temp49 != 0; _temp49=((
struct Cyc_List_List*) _check_null( _temp49))->tl){ if( ! Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp51=( char*)"union"; struct
_tagged_string _temp52; _temp52.curr= _temp51; _temp52.base= _temp51; _temp52.last_plus_one=
_temp51 + 6; _temp52;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d->name))->v,( void*)(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*)
_check_null( _temp49))->hd)->type)){ _temp47= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp47);
return _temp47;} _LL46:( void) _throw( _temp41); _LL42:;}}} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static void* Cyc_Interface_get_type1(
struct _tuple10* x){ return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl* d){ struct
_handler_cons _temp53; _push_handler(& _temp53);{ int _temp55= 0; if( setjmp(
_temp53.handler)){ _temp55= 1;} if( ! _temp55){{ int _temp56=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions, d->name);
_npop_handler( 0u); return _temp56;}; _pop_handler();} else{ void* _temp54=(
void*) _exn_thrown; void* _temp58= _temp54; _LL60: if( _temp58 == Cyc_Dict_Absent){
goto _LL61;} else{ goto _LL62;} _LL62: goto _LL63; _LL61: { int _temp64= 1; goto
_LL65; _LL65: seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp64); if( d->fields != 0){ struct Cyc_List_List* _temp66=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; goto _LL67; _LL67: for( 0;
_temp66 != 0; _temp66=(( struct Cyc_List_List*) _check_null( _temp66))->tl){ if(
!(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct
_tuple0* name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_types)(
seen,( struct _tagged_string)({ char* _temp68=( char*)"[x]tunion"; struct
_tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68; _temp69.last_plus_one=
_temp68 + 10; _temp69;}), d->name, Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp66))->hd)->typs)){ _temp64= 0;}}} seen->tunions=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tuple0* key, int data))
Cyc_Dict_insert)( seen->tunions, d->name, _temp64); return _temp64;} _LL63:(
void) _throw( _temp58); _LL59:;}}} static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl* d){ return 1;}
static int Cyc_Interface_check_public_typedefdecl( struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Typedefdecl* d){ return Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp70=( char*)"typedef"; struct
_tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 8; _temp71;}), d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl* d){ return Cyc_Interface_check_public_type(
seen,( struct _tagged_string)({ char* _temp72=( char*)"variable"; struct
_tagged_string _temp73; _temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one=
_temp72 + 9; _temp73;}), d->name,( void*) d->type);} static void* Cyc_Interface_get_type2(
struct _tuple1* x){ return(* x).f3;} static int Cyc_Interface_check_public_type(
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void* t){ void* _temp74= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp96; struct Cyc_Absyn_Conref* _temp98; struct Cyc_Absyn_Tqual _temp100;
struct Cyc_Absyn_Conref* _temp102; void* _temp104; void* _temp106; struct Cyc_Absyn_Exp*
_temp108; struct Cyc_Absyn_Tqual _temp110; void* _temp112; struct Cyc_Core_Opt*
_temp114; struct Cyc_Core_Opt _temp116; void* _temp117; struct Cyc_List_List*
_temp119; struct _tuple0* _temp121; struct Cyc_Absyn_FnInfo _temp123; struct Cyc_List_List*
_temp125; int _temp127; struct Cyc_List_List* _temp129; void* _temp131; struct
Cyc_Core_Opt* _temp133; struct Cyc_List_List* _temp135; struct Cyc_List_List*
_temp137; struct Cyc_Absyn_Structdecl** _temp139; struct Cyc_Absyn_Structdecl*
_temp141; struct Cyc_List_List* _temp142; struct _tuple0* _temp144; struct Cyc_Absyn_Uniondecl**
_temp146; struct Cyc_Absyn_Uniondecl* _temp148; struct Cyc_List_List* _temp149;
struct _tuple0* _temp151; struct Cyc_Absyn_TunionInfo _temp153; void* _temp155;
struct Cyc_List_List* _temp157; void* _temp159; struct Cyc_Absyn_Tuniondecl*
_temp161; struct Cyc_Absyn_TunionFieldInfo _temp163; struct Cyc_List_List*
_temp165; void* _temp167; struct Cyc_Absyn_Tunionfield* _temp169; struct Cyc_Absyn_Tuniondecl*
_temp171; _LL76: if(( unsigned int) _temp74 > 4u?*(( int*) _temp74) == Cyc_Absyn_PointerType:
0){ _LL97: _temp96=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp74)->f1; _LL107: _temp106=( void*) _temp96.elt_typ; goto _LL105; _LL105:
_temp104=( void*) _temp96.rgn_typ; goto _LL103; _LL103: _temp102=( struct Cyc_Absyn_Conref*)
_temp96.nullable; goto _LL101; _LL101: _temp100=( struct Cyc_Absyn_Tqual)
_temp96.tq; goto _LL99; _LL99: _temp98=( struct Cyc_Absyn_Conref*) _temp96.bounds;
goto _LL77;} else{ goto _LL78;} _LL78: if(( unsigned int) _temp74 > 4u?*(( int*)
_temp74) == Cyc_Absyn_ArrayType: 0){ _LL113: _temp112=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp74)->f1; goto _LL111; _LL111: _temp110=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp74)->f2; goto _LL109; _LL109: _temp108=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp74)->f3; goto
_LL79;} else{ goto _LL80;} _LL80: if(( unsigned int) _temp74 > 4u?*(( int*)
_temp74) == Cyc_Absyn_TypedefType: 0){ _LL122: _temp121=( struct _tuple0*)((
struct Cyc_Absyn_TypedefType_struct*) _temp74)->f1; goto _LL120; _LL120:
_temp119=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp74)->f2; goto _LL115; _LL115: _temp114=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp74)->f3; if( _temp114 == 0){ goto _LL82;} else{ _temp116=* _temp114; _LL118:
_temp117=( void*) _temp116.v; goto _LL81;}} else{ goto _LL82;} _LL82: if((
unsigned int) _temp74 > 4u?*(( int*) _temp74) == Cyc_Absyn_FnType: 0){ _LL124:
_temp123=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp74)->f1;
_LL136: _temp135=( struct Cyc_List_List*) _temp123.tvars; goto _LL134; _LL134:
_temp133=( struct Cyc_Core_Opt*) _temp123.effect; goto _LL132; _LL132: _temp131=(
void*) _temp123.ret_typ; goto _LL130; _LL130: _temp129=( struct Cyc_List_List*)
_temp123.args; goto _LL128; _LL128: _temp127=( int) _temp123.varargs; goto
_LL126; _LL126: _temp125=( struct Cyc_List_List*) _temp123.attributes; goto
_LL83;} else{ goto _LL84;} _LL84: if(( unsigned int) _temp74 > 4u?*(( int*)
_temp74) == Cyc_Absyn_TupleType: 0){ _LL138: _temp137=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp74)->f1; goto _LL85;} else{ goto _LL86;}
_LL86: if(( unsigned int) _temp74 > 4u?*(( int*) _temp74) == Cyc_Absyn_StructType:
0){ _LL145: _temp144=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp74)->f1; goto _LL143; _LL143: _temp142=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp74)->f2; goto _LL140; _LL140: _temp139=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp74)->f3;
if( _temp139 == 0){ goto _LL88;} else{ _temp141=* _temp139; goto _LL87;}} else{
goto _LL88;} _LL88: if(( unsigned int) _temp74 > 4u?*(( int*) _temp74) == Cyc_Absyn_UnionType:
0){ _LL152: _temp151=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp74)->f1; goto _LL150; _LL150: _temp149=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp74)->f2; goto _LL147; _LL147: _temp146=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp74)->f3;
if( _temp146 == 0){ goto _LL90;} else{ _temp148=* _temp146; goto _LL89;}} else{
goto _LL90;} _LL90: if(( unsigned int) _temp74 > 4u?*(( int*) _temp74) == Cyc_Absyn_TunionType:
0){ _LL154: _temp153=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp74)->f1; _LL160: _temp159=( void*) _temp153.tunion_info; if(*(( int*)
_temp159) == Cyc_Absyn_KnownTunion){ _LL162: _temp161=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp159)->f1; goto _LL158;} else{ goto
_LL92;} _LL158: _temp157=( struct Cyc_List_List*) _temp153.targs; goto _LL156;
_LL156: _temp155=( void*) _temp153.rgn; goto _LL91;} else{ goto _LL92;} _LL92:
if(( unsigned int) _temp74 > 4u?*(( int*) _temp74) == Cyc_Absyn_TunionFieldType:
0){ _LL164: _temp163=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp74)->f1; _LL168: _temp167=( void*) _temp163.field_info; if(*(( int*)
_temp167) == Cyc_Absyn_KnownTunionfield){ _LL172: _temp171=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp167)->f1; goto _LL170; _LL170:
_temp169=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp167)->f2; goto _LL166;} else{ goto _LL94;} _LL166: _temp165=( struct Cyc_List_List*)
_temp163.targs; goto _LL93;} else{ goto _LL94;} _LL94: goto _LL95; _LL77: return
Cyc_Interface_check_public_type( seen, obj, name, _temp106); _LL79: return Cyc_Interface_check_public_type(
seen, obj, name, _temp112); _LL81: return Cyc_Interface_check_public_type( seen,
obj, name, _temp117); _LL83: return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple1*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,
Cyc_Interface_get_type2, _temp129)? Cyc_Interface_check_public_type( seen, obj,
name, _temp131): 0; _LL85: return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_types)( seen, obj, name,
Cyc_Interface_get_type1, _temp137); _LL87: if(( void*) _temp141->sc ==( void*)
Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,( struct _tagged_string)({
char* _temp173=( char*)"struct"; struct _tagged_string _temp174; _temp174.curr=
_temp173; _temp174.base= _temp173; _temp174.last_plus_one= _temp173 + 7;
_temp174;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp141->name))->v);
return 0;} return(( int(*)( struct Cyc_Interface_Seen* seen, struct
_tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_types)( seen, obj, name,( void*(*)( void*)) Cyc_Core_identity,
_temp142)? Cyc_Interface_check_public_structdecl( seen, _temp141): 0; _LL89: if((
void*) _temp148->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
name,( struct _tagged_string)({ char* _temp175=( char*)"union"; struct
_tagged_string _temp176; _temp176.curr= _temp175; _temp176.base= _temp175;
_temp176.last_plus_one= _temp175 + 6; _temp176;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp148->name))->v); return 0;} return(( int(*)(
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_types)(
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp149)? Cyc_Interface_check_public_uniondecl(
seen, _temp148): 0; _LL91: if(( void*) _temp161->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,( struct _tagged_string)({ char* _temp177=(
char*)"[x]tunion"; struct _tagged_string _temp178; _temp178.curr= _temp177;
_temp178.base= _temp177; _temp178.last_plus_one= _temp177 + 10; _temp178;}),
_temp161->name); return 0;} return(( int(*)( struct Cyc_Interface_Seen* seen,
struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List*
l)) Cyc_Interface_check_public_types)( seen, obj, name,( void*(*)( void*)) Cyc_Core_identity,
_temp157)? Cyc_Interface_check_public_tuniondecl( seen, _temp161): 0; _LL93: if((
void*) _temp171->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
name,( struct _tagged_string)({ char* _temp179=( char*)"[x]tunion"; struct
_tagged_string _temp180; _temp180.curr= _temp179; _temp180.base= _temp179;
_temp180.last_plus_one= _temp179 + 10; _temp180;}), _temp171->name); return 0;}
if(( void*) _temp171->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err(
obj, name,( struct _tagged_string)({ char* _temp181=( char*)"[x]tunion"; struct
_tagged_string _temp182; _temp182.curr= _temp181; _temp182.base= _temp181;
_temp182.last_plus_one= _temp181 + 10; _temp182;}), _temp171->name); return 0;}
return(( int(*)( struct Cyc_Interface_Seen* seen, struct _tagged_string obj,
struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_types)(
seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp165)? Cyc_Interface_check_public_tuniondecl(
seen, _temp171): 0; _LL95: return 1; _LL75:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Interface_Seen* f4; } ;
static void Cyc_Interface_extract_structdecl( struct _tuple11* env, struct
_tagged_string* x, struct Cyc_Absyn_Structdecl** dp){ struct _tuple11 _temp185;
struct Cyc_Interface_Seen* _temp186; int _temp188; struct Cyc_Interface_Ienv*
_temp190; struct Cyc_Interface_Ienv* _temp192; struct _tuple11* _temp183= env;
_temp185=* _temp183; _LL193: _temp192= _temp185.f1; goto _LL191; _LL191:
_temp190= _temp185.f2; goto _LL189; _LL189: _temp188= _temp185.f3; goto _LL187;
_LL187: _temp186= _temp185.f4; goto _LL184; _LL184: { struct Cyc_Absyn_Structdecl*
_temp194=* dp; goto _LL195; _LL195: { void* _temp196=( void*) _temp194->sc;
_LL198: if( _temp196 ==( void*) Cyc_Absyn_Static){ goto _LL199;} else{ goto
_LL200;} _LL200: if( _temp196 ==( void*) Cyc_Absyn_Abstract){ goto _LL201;}
else{ goto _LL202;} _LL202: if( _temp196 ==( void*) Cyc_Absyn_Public){ goto
_LL203;} else{ goto _LL204;} _LL204: if( _temp196 ==( void*) Cyc_Absyn_ExternC){
goto _LL205;} else{ goto _LL206;} _LL206: if( _temp196 ==( void*) Cyc_Absyn_Extern){
goto _LL207;} else{ goto _LL197;} _LL199: if( _temp188? _temp194->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp208=( char*)"static";
struct _tagged_string _temp209; _temp209.curr= _temp208; _temp209.base= _temp208;
_temp209.last_plus_one= _temp208 + 7; _temp209;}),( struct _tagged_string)({
char* _temp210=( char*)"struct"; struct _tagged_string _temp211; _temp211.curr=
_temp210; _temp211.base= _temp210; _temp211.last_plus_one= _temp210 + 7;
_temp211;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp194->name))->v);}
goto _LL197; _LL201: if( _temp194->fields == 0){ if( _temp188){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp212=( char*)"abstract"; struct
_tagged_string _temp213; _temp213.curr= _temp212; _temp213.base= _temp212;
_temp213.last_plus_one= _temp212 + 9; _temp213;}),( struct _tagged_string)({
char* _temp214=( char*)"struct"; struct _tagged_string _temp215; _temp215.curr=
_temp214; _temp215.base= _temp214; _temp215.last_plus_one= _temp214 + 7;
_temp215;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp194->name))->v);}}
else{ _temp194=({ struct Cyc_Absyn_Structdecl* _temp216=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp216[ 0]=* _temp194;
_temp216;}); _temp194->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp186, _temp194)){ _temp190->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp190->structdecls, x, _temp194);} goto _LL197; _LL203: if(
_temp194->fields == 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp217=( char*)"public"; struct _tagged_string _temp218; _temp218.curr=
_temp217; _temp218.base= _temp217; _temp218.last_plus_one= _temp217 + 7;
_temp218;}),( struct _tagged_string)({ char* _temp219=( char*)"struct"; struct
_tagged_string _temp220; _temp220.curr= _temp219; _temp220.base= _temp219;
_temp220.last_plus_one= _temp219 + 7; _temp220;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp194->name))->v); _temp194=({ struct Cyc_Absyn_Structdecl*
_temp221=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp221[ 0]=* _temp194; _temp221;});( void*)( _temp194->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl( _temp186,
_temp194)){ _temp190->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp190->structdecls, x, _temp194);} goto _LL197; _LL205:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp222=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp222[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp223; _temp223.tag= Cyc_Core_InvalidArg; _temp223.f1=(
struct _tagged_string)({ char* _temp224=( char*)"extract_structdecl"; struct
_tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 19; _temp225;}); _temp223;}); _temp222;}));
_LL207: if( Cyc_Interface_check_public_structdecl( _temp186, _temp194)){
_temp192->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
_temp192->structdecls, x, _temp194);} goto _LL197; _LL197:;}}} static void Cyc_Interface_extract_uniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){
struct _tuple11 _temp228; struct Cyc_Interface_Seen* _temp229; int _temp231;
struct Cyc_Interface_Ienv* _temp233; struct Cyc_Interface_Ienv* _temp235; struct
_tuple11* _temp226= env; _temp228=* _temp226; _LL236: _temp235= _temp228.f1;
goto _LL234; _LL234: _temp233= _temp228.f2; goto _LL232; _LL232: _temp231=
_temp228.f3; goto _LL230; _LL230: _temp229= _temp228.f4; goto _LL227; _LL227: {
struct Cyc_Absyn_Uniondecl* _temp237=* dp; goto _LL238; _LL238: { void* _temp239=(
void*) _temp237->sc; _LL241: if( _temp239 ==( void*) Cyc_Absyn_Static){ goto
_LL242;} else{ goto _LL243;} _LL243: if( _temp239 ==( void*) Cyc_Absyn_Abstract){
goto _LL244;} else{ goto _LL245;} _LL245: if( _temp239 ==( void*) Cyc_Absyn_Public){
goto _LL246;} else{ goto _LL247;} _LL247: if( _temp239 ==( void*) Cyc_Absyn_ExternC){
goto _LL248;} else{ goto _LL249;} _LL249: if( _temp239 ==( void*) Cyc_Absyn_Extern){
goto _LL250;} else{ goto _LL240;} _LL242: if( _temp231? _temp237->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp251=( char*)"static";
struct _tagged_string _temp252; _temp252.curr= _temp251; _temp252.base= _temp251;
_temp252.last_plus_one= _temp251 + 7; _temp252;}),( struct _tagged_string)({
char* _temp253=( char*)"union"; struct _tagged_string _temp254; _temp254.curr=
_temp253; _temp254.base= _temp253; _temp254.last_plus_one= _temp253 + 6;
_temp254;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp237->name))->v);}
goto _LL240; _LL244: if( _temp237->fields == 0){ if( _temp231){ Cyc_Interface_fields_err((
struct _tagged_string)({ char* _temp255=( char*)"abstract"; struct
_tagged_string _temp256; _temp256.curr= _temp255; _temp256.base= _temp255;
_temp256.last_plus_one= _temp255 + 9; _temp256;}),( struct _tagged_string)({
char* _temp257=( char*)"union"; struct _tagged_string _temp258; _temp258.curr=
_temp257; _temp258.base= _temp257; _temp258.last_plus_one= _temp257 + 6;
_temp258;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp237->name))->v);}}
else{ _temp237=({ struct Cyc_Absyn_Uniondecl* _temp259=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp259[ 0]=* _temp237;
_temp259;}); _temp237->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp229, _temp237)){ _temp233->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data))
Cyc_Dict_insert)( _temp233->uniondecls, x, _temp237);} goto _LL240; _LL246: if(
_temp237->fields == 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp260=( char*)"public"; struct _tagged_string _temp261; _temp261.curr=
_temp260; _temp261.base= _temp260; _temp261.last_plus_one= _temp260 + 7;
_temp261;}),( struct _tagged_string)({ char* _temp262=( char*)"union"; struct
_tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 6; _temp263;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp237->name))->v); _temp237=({ struct Cyc_Absyn_Uniondecl*
_temp264=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp264[ 0]=* _temp237; _temp264;});( void*)( _temp237->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl( _temp229,
_temp237)){ _temp233->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
_temp233->uniondecls, x, _temp237);} goto _LL240; _LL248:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp265=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp265[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp266; _temp266.tag= Cyc_Core_InvalidArg; _temp266.f1=(
struct _tagged_string)({ char* _temp267=( char*)"extract_uniondecl"; struct
_tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 18; _temp268;}); _temp266;}); _temp265;}));
_LL250: if( Cyc_Interface_check_public_uniondecl( _temp229, _temp237)){ _temp235->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( _temp235->uniondecls, x,
_temp237);} goto _LL240; _LL240:;}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp271; struct Cyc_Interface_Seen* _temp272; int _temp274;
struct Cyc_Interface_Ienv* _temp276; struct Cyc_Interface_Ienv* _temp278; struct
_tuple11* _temp269= env; _temp271=* _temp269; _LL279: _temp278= _temp271.f1;
goto _LL277; _LL277: _temp276= _temp271.f2; goto _LL275; _LL275: _temp274=
_temp271.f3; goto _LL273; _LL273: _temp272= _temp271.f4; goto _LL270; _LL270: {
struct Cyc_Absyn_Tuniondecl* _temp280=* dp; goto _LL281; _LL281: { void*
_temp282=( void*) _temp280->sc; _LL284: if( _temp282 ==( void*) Cyc_Absyn_Static){
goto _LL285;} else{ goto _LL286;} _LL286: if( _temp282 ==( void*) Cyc_Absyn_Abstract){
goto _LL287;} else{ goto _LL288;} _LL288: if( _temp282 ==( void*) Cyc_Absyn_Public){
goto _LL289;} else{ goto _LL290;} _LL290: if( _temp282 ==( void*) Cyc_Absyn_ExternC){
goto _LL291;} else{ goto _LL292;} _LL292: if( _temp282 ==( void*) Cyc_Absyn_Extern){
goto _LL293;} else{ goto _LL283;} _LL285: if( _temp274? _temp280->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp294=( char*)"static";
struct _tagged_string _temp295; _temp295.curr= _temp294; _temp295.base= _temp294;
_temp295.last_plus_one= _temp294 + 7; _temp295;}),( struct _tagged_string)({
char* _temp296=( char*)"tunion"; struct _tagged_string _temp297; _temp297.curr=
_temp296; _temp297.base= _temp296; _temp297.last_plus_one= _temp296 + 7;
_temp297;}), _temp280->name);} goto _LL283; _LL287: if( _temp280->fields == 0){
if( _temp274){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp298=( char*)"abstract"; struct _tagged_string _temp299; _temp299.curr=
_temp298; _temp299.base= _temp298; _temp299.last_plus_one= _temp298 + 9;
_temp299;}),( struct _tagged_string)({ char* _temp300=( char*)"tunion"; struct
_tagged_string _temp301; _temp301.curr= _temp300; _temp301.base= _temp300;
_temp301.last_plus_one= _temp300 + 7; _temp301;}), _temp280->name);}} else{
_temp280=({ struct Cyc_Absyn_Tuniondecl* _temp302=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp302[ 0]=* _temp280;
_temp302;}); _temp280->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp272, _temp280)){ _temp276->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp276->tuniondecls, x, _temp280);} goto _LL283; _LL289: if(
_temp280->fields == 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp303=( char*)"public"; struct _tagged_string _temp304; _temp304.curr=
_temp303; _temp304.base= _temp303; _temp304.last_plus_one= _temp303 + 7;
_temp304;}),( struct _tagged_string)({ char* _temp305=( char*)"tunion"; struct
_tagged_string _temp306; _temp306.curr= _temp305; _temp306.base= _temp305;
_temp306.last_plus_one= _temp305 + 7; _temp306;}), _temp280->name); _temp280=({
struct Cyc_Absyn_Tuniondecl* _temp307=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp307[ 0]=* _temp280; _temp307;});(
void*)( _temp280->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_tuniondecl(
_temp272, _temp280)){ _temp276->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp276->tuniondecls, x, _temp280);} goto _LL283; _LL291:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp308=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp308[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp309; _temp309.tag= Cyc_Core_InvalidArg; _temp309.f1=(
struct _tagged_string)({ char* _temp310=( char*)"extract_tuniondecl"; struct
_tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 19; _temp311;}); _temp309;}); _temp308;}));
_LL293: if( Cyc_Interface_check_public_tuniondecl( _temp272, _temp280)){
_temp278->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp278->tuniondecls, x, _temp280);} goto _LL283; _LL283:;}}} static void Cyc_Interface_extract_enumdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){
struct _tuple11 _temp314; struct Cyc_Interface_Seen* _temp315; int _temp317;
struct Cyc_Interface_Ienv* _temp319; struct Cyc_Interface_Ienv* _temp321; struct
_tuple11* _temp312= env; _temp314=* _temp312; _LL322: _temp321= _temp314.f1;
goto _LL320; _LL320: _temp319= _temp314.f2; goto _LL318; _LL318: _temp317=
_temp314.f3; goto _LL316; _LL316: _temp315= _temp314.f4; goto _LL313; _LL313: {
struct Cyc_Absyn_Enumdecl* _temp323=* dp; goto _LL324; _LL324: { void* _temp325=(
void*) _temp323->sc; _LL327: if( _temp325 ==( void*) Cyc_Absyn_Static){ goto
_LL328;} else{ goto _LL329;} _LL329: if( _temp325 ==( void*) Cyc_Absyn_Abstract){
goto _LL330;} else{ goto _LL331;} _LL331: if( _temp325 ==( void*) Cyc_Absyn_Public){
goto _LL332;} else{ goto _LL333;} _LL333: if( _temp325 ==( void*) Cyc_Absyn_ExternC){
goto _LL334;} else{ goto _LL335;} _LL335: if( _temp325 ==( void*) Cyc_Absyn_Extern){
goto _LL336;} else{ goto _LL326;} _LL328: if( _temp317? _temp323->fields == 0: 0){
Cyc_Interface_fields_err(( struct _tagged_string)({ char* _temp337=( char*)"static";
struct _tagged_string _temp338; _temp338.curr= _temp337; _temp338.base= _temp337;
_temp338.last_plus_one= _temp337 + 7; _temp338;}),( struct _tagged_string)({
char* _temp339=( char*)"enum"; struct _tagged_string _temp340; _temp340.curr=
_temp339; _temp340.base= _temp339; _temp340.last_plus_one= _temp339 + 5;
_temp340;}), _temp323->name);} goto _LL326; _LL330: if( _temp323->fields == 0){
if( _temp317){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp341=( char*)"abstract"; struct _tagged_string _temp342; _temp342.curr=
_temp341; _temp342.base= _temp341; _temp342.last_plus_one= _temp341 + 9;
_temp342;}),( struct _tagged_string)({ char* _temp343=( char*)"enum"; struct
_tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 5; _temp344;}), _temp323->name);}} else{
_temp323=({ struct Cyc_Absyn_Enumdecl* _temp345=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp345[ 0]=* _temp323;
_temp345;}); _temp323->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp315, _temp323)){ _temp319->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp319->enumdecls, x, _temp323);} goto _LL326; _LL332: if(
_temp323->fields == 0){ Cyc_Interface_fields_err(( struct _tagged_string)({ char*
_temp346=( char*)"public"; struct _tagged_string _temp347; _temp347.curr=
_temp346; _temp347.base= _temp346; _temp347.last_plus_one= _temp346 + 7;
_temp347;}),( struct _tagged_string)({ char* _temp348=( char*)"enum"; struct
_tagged_string _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 5; _temp349;}), _temp323->name); _temp323=({
struct Cyc_Absyn_Enumdecl* _temp350=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp350[ 0]=* _temp323; _temp350;});( void*)(
_temp323->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl(
_temp315, _temp323)){ _temp319->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data))
Cyc_Dict_insert)( _temp319->enumdecls, x, _temp323);} goto _LL326; _LL334:( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp351=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp351[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp352; _temp352.tag= Cyc_Core_InvalidArg; _temp352.f1=(
struct _tagged_string)({ char* _temp353=( char*)"extract_enumdecl"; struct
_tagged_string _temp354; _temp354.curr= _temp353; _temp354.base= _temp353;
_temp354.last_plus_one= _temp353 + 17; _temp354;}); _temp352;}); _temp351;}));
_LL336: if( Cyc_Interface_check_public_enumdecl( _temp315, _temp323)){ _temp321->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( _temp321->enumdecls, x,
_temp323);} goto _LL326; _LL326:;}}} static void Cyc_Interface_extract_typedef(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple11 _temp357; struct Cyc_Interface_Seen* _temp358; int _temp360;
struct Cyc_Interface_Ienv* _temp362; struct Cyc_Interface_Ienv* _temp364; struct
_tuple11* _temp355= env; _temp357=* _temp355; _LL365: _temp364= _temp357.f1;
goto _LL363; _LL363: _temp362= _temp357.f2; goto _LL361; _LL361: _temp360=
_temp357.f3; goto _LL359; _LL359: _temp358= _temp357.f4; goto _LL356; _LL356:
if( Cyc_Interface_check_public_typedefdecl( _temp358, d)){ _temp362->typedefdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( _temp362->typedefdecls, x,
d);}} static void Cyc_Interface_extract_ordinarie( struct _tuple11* env, struct
_tagged_string* x, struct _tuple7* v){ struct _tuple11 _temp368; struct Cyc_Interface_Seen*
_temp369; int _temp371; struct Cyc_Interface_Ienv* _temp373; struct Cyc_Interface_Ienv*
_temp375; struct _tuple11* _temp366= env; _temp368=* _temp366; _LL376: _temp375=
_temp368.f1; goto _LL374; _LL374: _temp373= _temp368.f2; goto _LL372; _LL372:
_temp371= _temp368.f3; goto _LL370; _LL370: _temp369= _temp368.f4; goto _LL367;
_LL367: { void* _temp377=(* v).f1; goto _LL378; _LL378: { void* _temp379=
_temp377; void* _temp385; _LL381: if(*(( int*) _temp379) == Cyc_Tcenv_VarRes){
_LL386: _temp385=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp379)->f1; goto
_LL382;} else{ goto _LL383;} _LL383: goto _LL384; _LL382:{ void* _temp387=
_temp385; struct Cyc_Absyn_Fndecl* _temp395; struct Cyc_Absyn_Vardecl* _temp397;
_LL389: if(( unsigned int) _temp387 > 1u?*(( int*) _temp387) == Cyc_Absyn_Funname_b:
0){ _LL396: _temp395=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp387)->f1; goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int)
_temp387 > 1u?*(( int*) _temp387) == Cyc_Absyn_Global_b: 0){ _LL398: _temp397=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp387)->f1;
goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL390: { struct Cyc_Absyn_Vardecl*
_temp401=({ struct Cyc_Absyn_Vardecl* _temp399=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp399->sc=( void*)(( void*)
_temp395->sc); _temp399->name= _temp395->name; _temp399->tq=({ struct Cyc_Absyn_Tqual
_temp400; _temp400.q_const= 0; _temp400.q_volatile= 0; _temp400.q_restrict= 0;
_temp400;}); _temp399->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp395->cached_typ))->v); _temp399->initializer= 0; _temp399->rgn=
0; _temp399->attributes= 0; _temp399;}); goto _LL402; _LL402: _temp371= 0;
_temp397= _temp401; goto _LL392;} _LL392: if( _temp397->initializer != 0){
_temp397=({ struct Cyc_Absyn_Vardecl* _temp403=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp403[ 0]=* _temp397; _temp403;});
_temp397->initializer= 0;}{ void* _temp404=( void*) _temp397->sc; _LL406: if(
_temp404 ==( void*) Cyc_Absyn_Static){ goto _LL407;} else{ goto _LL408;} _LL408:
if( _temp404 ==( void*) Cyc_Absyn_Abstract){ goto _LL409;} else{ goto _LL410;}
_LL410: if( _temp404 ==( void*) Cyc_Absyn_Public){ goto _LL411;} else{ goto
_LL412;} _LL412: if( _temp404 ==( void*) Cyc_Absyn_ExternC){ goto _LL413;} else{
goto _LL414;} _LL414: if( _temp404 ==( void*) Cyc_Absyn_Extern){ goto _LL415;}
else{ goto _LL405;} _LL407: if( _temp371? Cyc_Tcutil_is_function_type(( void*)
_temp397->type): 0){ Cyc_Interface_body_err(( struct _tagged_string)({ char*
_temp416=( char*)"static"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 7;
_temp417;}), _temp397->name);} goto _LL405; _LL409:( void) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp418=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp418[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp419; _temp419.tag= Cyc_Core_InvalidArg; _temp419.f1=(
struct _tagged_string)({ char* _temp420=( char*)"extract_ordinarie"; struct
_tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 18; _temp421;}); _temp419;}); _temp418;}));
_LL411: if( _temp371? Cyc_Tcutil_is_function_type(( void*) _temp397->type): 0){
Cyc_Interface_body_err(( struct _tagged_string)({ char* _temp422=( char*)"public";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 7; _temp423;}), _temp397->name);} if( Cyc_Interface_check_public_vardecl(
_temp369, _temp397)){ _temp373->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp373->vardecls, x, _temp397);} goto _LL405; _LL413: goto _LL415; _LL415: if(
Cyc_Interface_check_public_vardecl( _temp369, _temp397)){ _temp375->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp375->vardecls, x,
_temp397);} goto _LL405; _LL405:;} goto _LL388; _LL394: goto _LL388; _LL388:;}
goto _LL380; _LL384: goto _LL380; _LL380:;}}} struct _tuple12{ struct Cyc_Interface_I*
f1; int f2; struct Cyc_Interface_Seen* f3; } ; static void Cyc_Interface_extract_f(
struct _tuple12* env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){
struct _tuple12 _temp426; struct Cyc_Interface_Seen* _temp427; int _temp429;
struct Cyc_Interface_I* _temp431; struct _tuple12* _temp424= env_f; _temp426=*
_temp424; _LL432: _temp431= _temp426.f1; goto _LL430; _LL430: _temp429= _temp426.f2;
goto _LL428; _LL428: _temp427= _temp426.f3; goto _LL425; _LL425: { struct
_tuple11 _temp434=({ struct _tuple11 _temp433; _temp433.f1=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp431->imports,
ns); _temp433.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp431->exports, ns); _temp433.f3=
_temp429; _temp433.f4= _temp427; _temp433;}); goto _LL435; _LL435:(( void(*)(
void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Structdecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,&
_temp434, ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp434, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp434, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp434, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp434, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp434, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp437=({ struct _tuple12
_temp436; _temp436.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp436.f2= check_complete_defs; _temp436.f3= Cyc_Interface_new_seen();
_temp436;}); goto _LL438; _LL438:(( void(*)( void(* f)( struct _tuple12*, struct
Cyc_List_List*, struct Cyc_Tcenv_Genv*), struct _tuple12* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,& _temp437, ae); return _temp437.f1;}
struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae){ return
Cyc_Interface_gen_extract( ae, 1);} inline static void Cyc_Interface_check_err(
struct _tagged_string* msg1, struct _tagged_string msg2){ Cyc_Tcdecl_merr( 0,
msg1, msg2);} struct _tuple13{ int f1; struct _tagged_string* f2; } ; struct
_tuple14{ int(* f1)( struct _tagged_string*, void*, void*); struct Cyc_Dict_Dict*
f2; struct _tuple13* f3; struct _tagged_string f4; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple14* env, struct _tagged_string* x, void* y1){ struct _tuple14
_temp441; struct _tagged_string _temp442; struct _tuple13* _temp444; struct Cyc_Dict_Dict*
_temp446; int(* _temp448)( struct _tagged_string*, void*, void*); struct
_tuple14* _temp439= env; _temp441=* _temp439; _LL449: _temp448= _temp441.f1;
goto _LL447; _LL447: _temp446= _temp441.f2; goto _LL445; _LL445: _temp444=
_temp441.f3; goto _LL443; _LL443: _temp442= _temp441.f4; goto _LL440; _LL440: {
struct _handler_cons _temp450; _push_handler(& _temp450);{ int _temp452= 0; if(
setjmp( _temp450.handler)){ _temp452= 1;} if( ! _temp452){{ void* _temp453=((
void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp446, x); goto _LL454; _LL454: if( ! _temp448((* _temp444).f2, y1, _temp453)){(*
_temp444).f1= 0;}}; _pop_handler();} else{ void* _temp451=( void*) _exn_thrown;
void* _temp456= _temp451; _LL458: if( _temp456 == Cyc_Dict_Absent){ goto _LL459;}
else{ goto _LL460;} _LL460: goto _LL461; _LL459: Cyc_Interface_check_err((*
_temp444).f2,({ struct _tagged_string _temp462= _temp442; struct _tagged_string
_temp463=* x; xprintf("%.*s %.*s is missing", _temp462.last_plus_one - _temp462.curr,
_temp462.curr, _temp463.last_plus_one - _temp463.curr, _temp463.curr);}));(*
_temp444).f1= 0; goto _LL457; _LL461:( void) _throw( _temp456); _LL457:;}}}}
static void Cyc_Interface_incl_dict( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, void*, void*), struct
Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2){ struct _tuple14 _temp465=({ struct
_tuple14 _temp464; _temp464.f1= f; _temp464.f2= d2; _temp464.f3= genv; _temp464.f4=
objname; _temp464;}); goto _LL466; _LL466:(( void(*)( void(* f)( struct _tuple14*,
struct _tagged_string*, void*), struct _tuple14* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct _tuple14* env, struct _tagged_string* x, void*
y1)) Cyc_Interface_incl_dict_f,& _temp465, d1);} static int Cyc_Interface_incl_structdecl(
struct _tagged_string* msg, struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1){ struct Cyc_Absyn_Structdecl* _temp467= Cyc_Tcdecl_merge_structdecl( d0, d1,
0, msg); goto _LL468; _LL468: if( _temp467 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*)
d0 != _temp467){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp469=
Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp469.last_plus_one - _temp469.curr, _temp469.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct _tagged_string* msg, struct Cyc_Absyn_Uniondecl*
d0, struct Cyc_Absyn_Uniondecl* d1){ struct Cyc_Absyn_Uniondecl* _temp470= Cyc_Tcdecl_merge_uniondecl(
d0, d1, 0, msg); goto _LL471; _LL471: if( _temp470 == 0){ return 0;} if(( struct
Cyc_Absyn_Uniondecl*) d0 != _temp470){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp472= Cyc_Absynpp_qvar2string(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp472.last_plus_one - _temp472.curr, _temp472.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct _tagged_string* msg, struct Cyc_Absyn_Tuniondecl*
d0, struct Cyc_Absyn_Tuniondecl* d1){ struct Cyc_Absyn_Tuniondecl* _temp473= Cyc_Tcdecl_merge_tuniondecl(
d0, d1, 0, msg); goto _LL474; _LL474: if( _temp473 == 0){ return 0;} if(( struct
Cyc_Absyn_Tuniondecl*) d0 != _temp473){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp475= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp475.last_plus_one - _temp475.curr, _temp475.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct _tagged_string* msg, struct Cyc_Absyn_Enumdecl*
d0, struct Cyc_Absyn_Enumdecl* d1){ struct Cyc_Absyn_Enumdecl* _temp476= Cyc_Tcdecl_merge_enumdecl(
d0, d1, 0, msg); goto _LL477; _LL477: if( _temp476 == 0){ return 0;} if(( struct
Cyc_Absyn_Enumdecl*) d0 != _temp476){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp478= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of enum %.*s discloses too much information",
_temp478.last_plus_one - _temp478.curr, _temp478.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_vardecl( struct _tagged_string* msg, struct Cyc_Absyn_Vardecl*
d0, struct Cyc_Absyn_Vardecl* d1){ struct Cyc_Absyn_Vardecl* _temp479= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL480; _LL480: if( _temp479 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp479){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp481= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of var %.*s discloses too much information",
_temp481.last_plus_one - _temp481.curr, _temp481.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct _tagged_string* msg, struct
Cyc_Absyn_Typedefdecl* d0, struct Cyc_Absyn_Typedefdecl* d1){ struct Cyc_Absyn_Typedefdecl*
_temp482= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); goto _LL483; _LL483:
if( _temp482 == 0){ return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 !=
_temp482){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp484= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_temp484.last_plus_one - _temp484.curr, _temp484.curr);})); return 0;} return 1;}
static void Cyc_Interface_incl_ns( struct _tuple13* env, struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2){(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp485=(
char*)"struct"; struct _tagged_string _temp486; _temp486.curr= _temp485;
_temp486.base= _temp485; _temp486.last_plus_one= _temp485 + 7; _temp486;}), Cyc_Interface_incl_structdecl,
ie1->structdecls, ie2->structdecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp487=( char*)"union";
struct _tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 6; _temp488;}), Cyc_Interface_incl_uniondecl,
ie1->uniondecls, ie2->uniondecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp489=(
char*)"tunion"; struct _tagged_string _temp490; _temp490.curr= _temp489;
_temp490.base= _temp489; _temp490.last_plus_one= _temp489 + 7; _temp490;}), Cyc_Interface_incl_tuniondecl,
ie1->tuniondecls, ie2->tuniondecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp491=( char*)"enum";
struct _tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 5; _temp492;}), Cyc_Interface_incl_enumdecl,
ie1->enumdecls, ie2->enumdecls);(( void(*)( struct _tuple13* genv, struct
_tagged_string objname, int(* f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Interface_incl_dict)( env,( struct _tagged_string)({ char* _temp493=( char*)"variable";
struct _tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 9; _temp494;}), Cyc_Interface_incl_vardecl,
ie1->vardecls, ie2->vardecls);} int Cyc_Interface_is_subinterface( struct Cyc_Interface_I*
i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct _handler_cons
_temp495; _push_handler(& _temp495);{ int _temp497= 0; if( setjmp( _temp495.handler)){
_temp497= 1;} if( ! _temp497){{ struct _tuple13 env=({ struct _tuple13 _temp505;
_temp505.f1= 1; _temp505.f2= 0; _temp505;}); if( info != 0){ env.f2=({ struct
_tagged_string* _temp498=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp498[ 0]=({ struct _tagged_string _temp499=(*(( struct
_tuple3*) _check_null( info))).f2; struct _tagged_string _temp500=(*(( struct
_tuple3*) _check_null( info))).f1; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp499.last_plus_one - _temp499.curr, _temp499.curr, _temp500.last_plus_one -
_temp500.curr, _temp500.curr);}); _temp498;});}(( void(*)( void(* f)( struct
_tuple13*, struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct
_tuple13* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)(
Cyc_Interface_incl_ns,& env, i2->exports, i1->exports); if( info != 0){ env.f2=({
struct _tagged_string* _temp501=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string)); _temp501[ 0]=({ struct _tagged_string _temp502=(*((
struct _tuple3*) _check_null( info))).f1; struct _tagged_string _temp503=(*((
struct _tuple3*) _check_null( info))).f2; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp502.last_plus_one - _temp502.curr, _temp502.curr, _temp503.last_plus_one -
_temp503.curr, _temp503.curr);}); _temp501;});}(( void(*)( void(* f)( struct
_tuple13*, struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct
_tuple13* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2_c)(
Cyc_Interface_incl_ns,& env, i1->imports, i2->imports);{ int _temp504= env.f1;
_npop_handler( 0u); return _temp504;}}; _pop_handler();} else{ void* _temp496=(
void*) _exn_thrown; void* _temp507= _temp496; _LL509: if( _temp507 == Cyc_Tcdecl_Incompatible){
goto _LL510;} else{ goto _LL511;} _LL511: goto _LL512; _LL510: return 0; _LL512:(
void) _throw( _temp507); _LL508:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); void**(* f6)(
void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*, int*);
struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f( struct
_tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15 _temp515;
struct _tagged_string* _temp516; void**(* _temp518)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*, int*); void**(* _temp520)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*); struct Cyc_Dict_Dict* _temp522; struct Cyc_Dict_Dict*
_temp524; struct Cyc_Dict_Dict* _temp526; struct Cyc_Dict_Dict** _temp528; int
_temp529; int* _temp531; struct _tuple15* _temp513= env; _temp515=* _temp513;
_LL530: _temp529= _temp515.f1; _temp531=&(* _temp513).f1; goto _LL527; _LL527:
_temp526= _temp515.f2; _temp528=&(* _temp513).f2; goto _LL525; _LL525: _temp524=
_temp515.f3; goto _LL523; _LL523: _temp522= _temp515.f4; goto _LL521; _LL521:
_temp520= _temp515.f5; goto _LL519; _LL519: _temp518= _temp515.f6; goto _LL517;
_LL517: _temp516= _temp515.f7; goto _LL514; _LL514: { void** y;{ struct
_handler_cons _temp532; _push_handler(& _temp532);{ int _temp534= 0; if( setjmp(
_temp532.handler)){ _temp534= 1;} if( ! _temp534){{ void** _temp535=(( void**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp524,
x); goto _LL536; _LL536: { void** _temp537= _temp520( _temp535, y2, 0, _temp516);
goto _LL538; _LL538: if( !(( unsigned int) _temp537)){* _temp531= 0;
_npop_handler( 0u); return;} y=( void**)(( void**) _check_null( _temp537));}};
_pop_handler();} else{ void* _temp533=( void*) _exn_thrown; void* _temp540=
_temp533; _LL542: if( _temp540 == Cyc_Dict_Absent){ goto _LL543;} else{ goto
_LL544;} _LL544: goto _LL545; _LL543: y= y2; goto _LL541; _LL545:( void) _throw(
_temp540); _LL541:;}}}{ struct _handler_cons _temp546; _push_handler(& _temp546);{
int _temp548= 0; if( setjmp( _temp546.handler)){ _temp548= 1;} if( ! _temp548){{
void** _temp549=(( void**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( _temp522, x); goto _LL550; _LL550: { int* _temp552=({
int* _temp551=( int*) GC_malloc_atomic( sizeof( int)); _temp551[ 0]= 1; _temp551;});
goto _LL553; _LL553: { void** _temp554= _temp518( _temp549, y, 0, _temp516,
_temp552); goto _LL555; _LL555: if( !(* _temp552)){* _temp531= 0;} if((
unsigned int) _temp554){* _temp528=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(* _temp528, x,(
void**)(( void**) _check_null( _temp554)));}}}}; _pop_handler();} else{ void*
_temp547=( void*) _exn_thrown; void* _temp557= _temp547; _LL559: if( _temp557 ==
Cyc_Dict_Absent){ goto _LL560;} else{ goto _LL561;} _LL561: goto _LL562; _LL560:
if(* _temp531){* _temp528=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, void** data)) Cyc_Dict_insert)(* _temp528, x, y);}
goto _LL558; _LL562:( void) _throw( _temp557); _LL558:;}}}}} struct Cyc_Core_Opt*
Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*), void**(* sub_f)( void**, void**, struct Cyc_Position_Segment*,
struct _tagged_string*, int*), struct _tagged_string* msg){ struct _tuple15
_temp564=({ struct _tuple15 _temp563; _temp563.f1= 1; _temp563.f2= dic1;
_temp563.f3= dic1; _temp563.f4= excl; _temp563.f5= merge_f; _temp563.f6= sub_f;
_temp563.f7= msg; _temp563;}); goto _LL565; _LL565:(( void(*)( void(* f)( struct
_tuple15*, struct _tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple15* env, struct _tagged_string* x,
void** y2)) Cyc_Interface_compat_merge_dict_f,& _temp564, dic2); if( _temp564.f1){
return({ struct Cyc_Core_Opt* _temp566=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp566->v=( void*) _temp564.f2; _temp566;});}
else{ return 0;}} struct Cyc_Interface_Ienv* Cyc_Interface_compat_merge_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct Cyc_Interface_Ienv*
iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt* _temp567=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Structdecl*(* merge_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Structdecl*(* sub_f)( struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->structdecls, ie2->structdecls, iexcl->structdecls, Cyc_Tcdecl_merge_structdecl,
Cyc_Tcdecl_sub_structdecl, msg); goto _LL568; _LL568: { struct Cyc_Core_Opt*
_temp569=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Uniondecl*(* sub_f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*, struct _tagged_string*,
int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->uniondecls,
ie2->uniondecls, iexcl->uniondecls, Cyc_Tcdecl_merge_uniondecl, Cyc_Tcdecl_sub_uniondecl,
msg); goto _LL570; _LL570: { struct Cyc_Core_Opt* _temp571=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Tuniondecl*(* sub_f)( struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls, Cyc_Tcdecl_merge_tuniondecl,
Cyc_Tcdecl_sub_tuniondecl, msg); goto _LL572; _LL572: { struct Cyc_Core_Opt*
_temp573=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Enumdecl*(* sub_f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*, struct _tagged_string*,
int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->enumdecls,
ie2->enumdecls, iexcl->enumdecls, Cyc_Tcdecl_merge_enumdecl, Cyc_Tcdecl_sub_enumdecl,
msg); goto _LL574; _LL574: { struct Cyc_Core_Opt* _temp575=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Typedefdecl*(* sub_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls, Cyc_Tcdecl_merge_typedefdecl,
Cyc_Tcdecl_sub_typedefdecl, msg); goto _LL576; _LL576: { struct Cyc_Core_Opt*
_temp577=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct Cyc_Absyn_Vardecl*(* sub_f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*, struct _tagged_string*,
int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->vardecls,
ie2->vardecls, iexcl->vardecls, Cyc_Tcdecl_merge_vardecl, Cyc_Tcdecl_sub_vardecl,
msg); goto _LL578; _LL578: if((((( !(( unsigned int) _temp567)? 1: !((
unsigned int) _temp569))? 1: !(( unsigned int) _temp571))? 1: !(( unsigned int)
_temp573))? 1: !(( unsigned int) _temp575))? 1: !(( unsigned int) _temp577)){
return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp579=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp579->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp567))->v; _temp579->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp569))->v;
_temp579->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp571))->v; _temp579->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp573))->v; _temp579->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp575))->v;
_temp579->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp577))->v; _temp579;});}}}}}}} static struct Cyc_Interface_Ienv* Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv* ie= 0; if( ie == 0){ ie=( struct Cyc_Interface_Ienv*)
Cyc_Interface_new_ienv();} return( struct Cyc_Interface_Ienv*)(( struct Cyc_Interface_Ienv*)
_check_null( ie));} struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct
Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; struct _tagged_string* f5; } ; void
Cyc_Interface_compat_merge_ns_f( struct _tuple16* env, struct Cyc_List_List* ns,
struct Cyc_Interface_Ienv* ie2){ struct _tuple16 _temp582; struct _tagged_string*
_temp583; struct Cyc_Dict_Dict* _temp585; struct Cyc_Dict_Dict* _temp587; struct
Cyc_Dict_Dict* _temp589; struct Cyc_Dict_Dict** _temp591; int _temp592; int*
_temp594; struct _tuple16* _temp580= env; _temp582=* _temp580; _LL593: _temp592=
_temp582.f1; _temp594=&(* _temp580).f1; goto _LL590; _LL590: _temp589= _temp582.f2;
_temp591=&(* _temp580).f2; goto _LL588; _LL588: _temp587= _temp582.f3; goto
_LL586; _LL586: _temp585= _temp582.f4; goto _LL584; _LL584: _temp583= _temp582.f5;
goto _LL581; _LL581: { struct Cyc_Interface_Ienv* ie1; struct Cyc_Interface_Ienv*
iexcl;{ struct _handler_cons _temp595; _push_handler(& _temp595);{ int _temp597=
0; if( setjmp( _temp595.handler)){ _temp597= 1;} if( ! _temp597){ iexcl=((
struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( _temp585, ns);; _pop_handler();} else{ void* _temp596=(
void*) _exn_thrown; void* _temp599= _temp596; _LL601: if( _temp599 == Cyc_Dict_Absent){
goto _LL602;} else{ goto _LL603;} _LL603: goto _LL604; _LL602: iexcl= Cyc_Interface_lazy_new_ienv();
goto _LL600; _LL604:( void) _throw( _temp599); _LL600:;}}}{ struct _handler_cons
_temp605; _push_handler(& _temp605);{ int _temp607= 0; if( setjmp( _temp605.handler)){
_temp607= 1;} if( ! _temp607){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp587, ns);{ struct Cyc_Interface_Ienv*
_temp608= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp583); goto _LL609; _LL609: if( _temp608 == 0){* _temp594= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*)(( struct Cyc_Interface_Ienv*)
_check_null( _temp608));}; _pop_handler();} else{ void* _temp606=( void*)
_exn_thrown; void* _temp611= _temp606; _LL613: if( _temp611 == Cyc_Dict_Absent){
goto _LL614;} else{ goto _LL615;} _LL615: goto _LL616; _LL614: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL612; _LL616:( void) _throw( _temp611); _LL612:;}}}{ struct Cyc_Interface_Ienv*
_temp617= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp583); goto
_LL618; _LL618: if( _temp617 == 0){* _temp594= 0;} else{ if(* _temp594){*
_temp591=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp591, ns,( struct
Cyc_Interface_Ienv*)(( struct Cyc_Interface_Ienv*) _check_null( _temp617)));}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, struct _tagged_string*
msg){ struct _tuple16 _temp620=({ struct _tuple16 _temp619; _temp619.f1= 1;
_temp619.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp619.f3=
dic1; _temp619.f4= excl; _temp619.f5= msg; _temp619;}); goto _LL621; _LL621:((
void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*),
struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_compat_merge_ns_f,&
_temp620, dic2); if( _temp620.f1){ return({ struct Cyc_Core_Opt* _temp622=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp622->v=(
void*) _temp620.f2; _temp622;});} else{ return 0;}} struct _tuple17{ int f1;
struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string f4;
struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f( struct
_tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17 _temp625;
struct _tagged_string* _temp626; struct _tagged_string _temp628; struct Cyc_Dict_Dict*
_temp630; struct Cyc_Dict_Dict* _temp632; struct Cyc_Dict_Dict** _temp634; int
_temp635; int* _temp637; struct _tuple17* _temp623= env; _temp625=* _temp623;
_LL636: _temp635= _temp625.f1; _temp637=&(* _temp623).f1; goto _LL633; _LL633:
_temp632= _temp625.f2; _temp634=&(* _temp623).f2; goto _LL631; _LL631: _temp630=
_temp625.f3; goto _LL629; _LL629: _temp628= _temp625.f4; goto _LL627; _LL627:
_temp626= _temp625.f5; goto _LL624; _LL624: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp630, x)){ Cyc_Interface_check_err(
_temp626,({ struct _tagged_string _temp638= _temp628; struct _tagged_string
_temp639=* x; xprintf("%.*s %.*s is exported more than once", _temp638.last_plus_one
- _temp638.curr, _temp638.curr, _temp639.last_plus_one - _temp639.curr, _temp639.curr);}));*
_temp637= 0;} else{ if(* _temp637){* _temp634=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(*
_temp634, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg){ struct _tuple17 _temp641=({ struct _tuple17 _temp640;
_temp640.f1= 1; _temp640.f2= dic1; _temp640.f3= dic1; _temp640.f4= t; _temp640.f5=
msg; _temp640;}); goto _LL642; _LL642:(( void(*)( void(* f)( struct _tuple17*,
struct _tagged_string*, void*), struct _tuple17* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct _tuple17* env, struct _tagged_string* x, void*
y)) Cyc_Interface_disj_merge_dict_f,& _temp641, dic2); if( _temp641.f1){ return({
struct Cyc_Core_Opt* _temp643=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp643->v=( void*) _temp641.f2; _temp643;});} else{ return 0;}}
struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_string* msg){ struct Cyc_Interface_Ienv*
_temp667=({ struct Cyc_Interface_Ienv* _temp644=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp644->structdecls=({ struct
Cyc_Core_Opt* _temp665=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_disj_merge_dict)( ie1->structdecls, ie2->structdecls,( struct
_tagged_string)({ char* _temp663=( char*)"struct"; struct _tagged_string
_temp664; _temp664.curr= _temp663; _temp664.base= _temp663; _temp664.last_plus_one=
_temp663 + 7; _temp664;}), msg); goto _LL666; _LL666: if( _temp665 == 0){ return
0;}( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp665))->v;});
_temp644->uniondecls=({ struct Cyc_Core_Opt* _temp661=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls,
ie2->uniondecls,( struct _tagged_string)({ char* _temp659=( char*)"union";
struct _tagged_string _temp660; _temp660.curr= _temp659; _temp660.base= _temp659;
_temp660.last_plus_one= _temp659 + 6; _temp660;}), msg); goto _LL662; _LL662:
if( _temp661 == 0){ return 0;}( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp661))->v;}); _temp644->tuniondecls=({ struct Cyc_Core_Opt*
_temp657=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls, ie2->tuniondecls,( struct _tagged_string)({ char* _temp655=(
char*)"[x]tunion"; struct _tagged_string _temp656; _temp656.curr= _temp655;
_temp656.base= _temp655; _temp656.last_plus_one= _temp655 + 10; _temp656;}), msg);
goto _LL658; _LL658: if( _temp657 == 0){ return 0;}( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp657))->v;}); _temp644->enumdecls=({
struct Cyc_Core_Opt* _temp653=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->enumdecls, ie2->enumdecls,( struct
_tagged_string)({ char* _temp651=( char*)"enum"; struct _tagged_string _temp652;
_temp652.curr= _temp651; _temp652.base= _temp651; _temp652.last_plus_one=
_temp651 + 5; _temp652;}), msg); goto _LL654; _LL654: if( _temp653 == 0){ return
0;}( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp653))->v;});
_temp644->typedefdecls=({ struct Cyc_Core_Opt* _temp649=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct Cyc_Absyn_Typedefdecl*(* sub_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*, int*), struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp),
Cyc_Tcdecl_merge_typedefdecl, Cyc_Tcdecl_sub_typedefdecl, msg); goto _LL650;
_LL650: if( _temp649 == 0){ return 0;}( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp649))->v;}); _temp644->vardecls=({ struct Cyc_Core_Opt*
_temp647=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->vardecls, ie2->vardecls,( struct _tagged_string)({ char* _temp645=( char*)"variable";
struct _tagged_string _temp646; _temp646.curr= _temp645; _temp646.base= _temp645;
_temp646.last_plus_one= _temp645 + 9; _temp646;}), msg); goto _LL648; _LL648:
if( _temp647 == 0){ return 0;}( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp647))->v;}); _temp644;}); goto _LL668; _LL668: return _temp667;}
struct _tuple18{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_string* f4; } ; void Cyc_Interface_disj_merge_ns_f( struct
_tuple18* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){ struct
_tuple18 _temp671; struct _tagged_string* _temp672; struct Cyc_Dict_Dict*
_temp674; struct Cyc_Dict_Dict* _temp676; struct Cyc_Dict_Dict** _temp678; int
_temp679; int* _temp681; struct _tuple18* _temp669= env; _temp671=* _temp669;
_LL680: _temp679= _temp671.f1; _temp681=&(* _temp669).f1; goto _LL677; _LL677:
_temp676= _temp671.f2; _temp678=&(* _temp669).f2; goto _LL675; _LL675: _temp674=
_temp671.f3; goto _LL673; _LL673: _temp672= _temp671.f4; goto _LL670; _LL670: {
struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons _temp682; _push_handler(&
_temp682);{ int _temp684= 0; if( setjmp( _temp682.handler)){ _temp684= 1;} if( !
_temp684){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( _temp674, ns);; _pop_handler();} else{
void* _temp683=( void*) _exn_thrown; void* _temp686= _temp683; _LL688: if(
_temp686 == Cyc_Dict_Absent){ goto _LL689;} else{ goto _LL690;} _LL690: goto
_LL691; _LL689: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL687; _LL691:( void)
_throw( _temp686); _LL687:;}}}{ struct Cyc_Interface_Ienv* _temp692= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp672); goto _LL693; _LL693: if( _temp692 == 0){* _temp681= 0;}
else{ if(* _temp681){* _temp678=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp678, ns,( struct Cyc_Interface_Ienv*)(( struct Cyc_Interface_Ienv*)
_check_null( _temp692)));}}}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string*
msg){ struct _tuple18 _temp695=({ struct _tuple18 _temp694; _temp694.f1= 1;
_temp694.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp); _temp694.f3=
dic1; _temp694.f4= msg; _temp694;}); goto _LL696; _LL696:(( void(*)( void(* f)(
struct _tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple18* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp695, dic2); if( _temp695.f1){ return({ struct Cyc_Core_Opt* _temp697=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp697->v=(
void*) _temp695.f2; _temp697;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string*
_temp698=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp698[ 0]=({ struct _tagged_string _temp699=(*(( struct _tuple3*) _check_null(
info))).f1; struct _tagged_string _temp700=(*(( struct _tuple3*) _check_null(
info))).f2; xprintf("merging exports of %.*s and %.*s,", _temp699.last_plus_one
- _temp699.curr, _temp699.curr, _temp700.last_plus_one - _temp700.curr, _temp700.curr);});
_temp698;});}{ struct Cyc_Core_Opt* _temp701= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL702; _LL702: if( _temp701 == 0){ return 0;} if( info
!= 0){ msg=({ struct _tagged_string* _temp703=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp703[ 0]=({ struct
_tagged_string _temp704=(*(( struct _tuple3*) _check_null( info))).f1; struct
_tagged_string _temp705=(*(( struct _tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp704.last_plus_one - _temp704.curr, _temp704.curr, _temp705.last_plus_one -
_temp705.curr, _temp705.curr);}); _temp703;});}{ struct Cyc_Core_Opt* _temp706=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp701))->v, msg); goto _LL707; _LL707: if(
_temp706 == 0){ return 0;} return({ struct Cyc_Interface_I* _temp708=( struct
Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp708->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp706))->v;
_temp708->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp701))->v; _temp708;});}}} static struct Cyc_List_List* Cyc_Interface_add_namespace(
struct Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;}
return({ struct Cyc_List_List* _temp709=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp709->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp710=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp710[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp711; _temp711.tag= Cyc_Absyn_Namespace_d;
_temp711.f1=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp711.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp711;}); _temp710;}), 0); _temp709->tl= 0; _temp709;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_string*
x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp712=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp712->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp713=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp713[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp714; _temp714.tag= Cyc_Absyn_Struct_d;
_temp714.f1= d; _temp714;}); _temp713;}), 0); _temp712->tl= tds; _temp712;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp715=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp715[ 0]=* d; _temp715;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp716=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp716->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp717=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp717[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp718; _temp718.tag= Cyc_Absyn_Struct_d;
_temp718.f1= d; _temp718;}); _temp717;}), 0); _temp716->tl= tds; _temp716;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp719=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp719->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp720=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp720[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp721; _temp721.tag= Cyc_Absyn_Union_d;
_temp721.f1= d; _temp721;}); _temp720;}), 0); _temp719->tl= tds; _temp719;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp722=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp722[ 0]=* d; _temp722;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp723=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp723->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp724=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp724[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp725; _temp725.tag= Cyc_Absyn_Union_d;
_temp725.f1= d; _temp725;}); _temp724;}), 0); _temp723->tl= tds; _temp723;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp726=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp726->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp727=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp727[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp728; _temp728.tag= Cyc_Absyn_Tunion_d;
_temp728.f1= d; _temp728;}); _temp727;}), 0); _temp726->tl= tds; _temp726;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp729=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp729[ 0]=* d; _temp729;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp730=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp730->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp731=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp731[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp732; _temp732.tag= Cyc_Absyn_Tunion_d;
_temp732.f1= d; _temp732;}); _temp731;}), 0); _temp730->tl= tds; _temp730;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp733=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp733->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp734=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp734[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp735; _temp735.tag= Cyc_Absyn_Enum_d;
_temp735.f1= d; _temp735;}); _temp734;}), 0); _temp733->tl= tds; _temp733;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp736=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp736[ 0]=* d; _temp736;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp737=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp737->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp738=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp738[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp739; _temp739.tag= Cyc_Absyn_Enum_d;
_temp739.f1= d; _temp739;}); _temp738;}), 0); _temp737->tl= tds; _temp737;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp740=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp740->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp741=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp741[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp742; _temp742.tag= Cyc_Absyn_Typedef_d;
_temp742.f1= d; _temp742;}); _temp741;}), 0); _temp740->tl= tds; _temp740;});}
static struct Cyc_List_List* Cyc_Interface_add_var( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp743=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp743->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp744=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp744[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp745; _temp745.tag= Cyc_Absyn_Var_d;
_temp745.f1= d; _temp745;}); _temp744;}), 0); _temp743->tl= tds; _temp743;});}
static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE* f, struct
Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp746= 0; goto _LL747; _LL747: _temp746=(( struct Cyc_List_List*(*)( struct
Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp746);
_temp746=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp746); _temp746=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp746); _temp746=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp746); _temp746=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp746); _temp746= Cyc_Interface_add_namespace(
_temp746, ns); Cyc_Absynpp_decllist2file( _temp746, f);} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp748= 0; goto _LL749; _LL749: _temp748=(( struct
Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*, struct
Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct
Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp748); _temp748=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp748); _temp748= Cyc_Interface_add_namespace( _temp748, ns); Cyc_Absynpp_decllist2file(
_temp748, f);} static void Cyc_Interface_print_ns_decls( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp750= 0; goto _LL751; _LL751: _temp750=(( struct Cyc_List_List*(*)( struct
Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls, _temp750);
_temp750=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl, ie->uniondecls,
_temp750); _temp750=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp750); _temp750=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp750); _temp750=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_var,
ie->vardecls, _temp750); _temp750=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp750); _temp750= Cyc_Interface_add_namespace(
_temp750, ns); Cyc_Absynpp_decllist2file( _temp750, f);} void Cyc_Interface_print(
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp752= Cyc_Parse_parse_file( f);
goto _LL753; _LL753: Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv*
_temp754= Cyc_Tcenv_tc_init(); goto _LL755; _LL755: Cyc_Tc_tc( _temp754, 0, 0,
_temp752); return Cyc_Interface_gen_extract( _temp754->ae, 0);}}}