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
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void
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
int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Struct_d= 4; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d=
5; struct Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ;
static const int Cyc_Absyn_Tunion_d= 6; struct Cyc_Absyn_Tunion_d_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_Enum_d= 7;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
static const int Cyc_Absyn_Typedef_d= 8; struct Cyc_Absyn_Typedef_d_struct{ int
tag; struct Cyc_Absyn_Typedefdecl* f1; } ; static const int Cyc_Absyn_Namespace_d=
9; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
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
return( struct Cyc_Interface_Ienv*) _check_null( ie);} static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f( void* ignored){ return Cyc_Interface_new_ienv();} static
struct Cyc_Interface_I* Cyc_Interface_skel2i( struct Cyc_Dict_Dict* skel){
return({ struct Cyc_Interface_I* _temp1=( struct Cyc_Interface_I*) GC_malloc(
sizeof( struct Cyc_Interface_I)); _temp1->imports=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(* f)( void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)((
struct Cyc_Interface_Ienv*(*)( void* ignored)) Cyc_Interface_skel2i_f, skel);
_temp1->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(* f)(
void*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct Cyc_Interface_Ienv*(*)(
void* ignored)) Cyc_Interface_skel2i_f, skel); _temp1;});} struct Cyc_Interface_I*
Cyc_Interface_empty(){ return({ struct Cyc_Interface_I* _temp2=( struct Cyc_Interface_I*)
GC_malloc( sizeof( struct Cyc_Interface_I)); _temp2->imports=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp); _temp2->exports=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp2;});} struct Cyc_Interface_I* Cyc_Interface_final(){ struct Cyc_Interface_I*
_temp3= Cyc_Interface_empty(); goto _LL4; _LL4: { struct Cyc_Interface_Ienv*
_temp5= Cyc_Interface_new_ienv(); goto _LL6; _LL6: { struct Cyc_Absyn_Tuniondecl*
_temp8=({ struct Cyc_Absyn_Tuniondecl* _temp7=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp7[ 0]=* Cyc_Absyn_exn_xed;
_temp7;}); goto _LL9; _LL9:( void*)( _temp8->sc=( void*)(( void*) Cyc_Absyn_Public));
_temp5->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)(
_temp5->tuniondecls,(* Cyc_Absyn_exn_name).f2, _temp8); _temp3->exports=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)( _temp3->exports, 0, _temp5);{
struct Cyc_Interface_Ienv* _temp10= Cyc_Interface_new_ienv(); goto _LL11; _LL11: {
struct _tuple0* qmain=({ struct _tuple0* _temp18=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp18->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp22=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp22[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp23; _temp23.tag= Cyc_Absyn_Abs_n;
_temp23.f1= 0; _temp23;}); _temp22;}); _temp18->f2=({ struct _tagged_string*
_temp19=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp19[ 0]=({ char* _temp20="main"; struct _tagged_string _temp21; _temp21.curr=
_temp20; _temp21.base= _temp20; _temp21.last_plus_one= _temp20 + 5; _temp21;});
_temp19;}); _temp18;}); struct Cyc_Absyn_Vardecl* _temp16= Cyc_Absyn_new_vardecl(
qmain, Cyc_Absyn_function_typ( 0, 0, Cyc_Absyn_sint_t,({ struct Cyc_List_List*
_temp12=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp12->hd=( void*)({ struct _tuple1* _temp15=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp15->f1= 0; _temp15->f2= Cyc_Absyn_empty_tqual();
_temp15->f3= Cyc_Absyn_uint_t; _temp15;}); _temp12->tl=({ struct Cyc_List_List*
_temp13=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp13->hd=( void*)({ struct _tuple1* _temp14=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp14->f1= 0; _temp14->f2= Cyc_Absyn_empty_tqual();
_temp14->f3= Cyc_Absyn_tagged_typ( Cyc_Absyn_tagged_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()),( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp14;}); _temp13->tl= 0; _temp13;}); _temp12;}), 0,
0), 0); goto _LL17; _LL17:( void*)( _temp16->sc=( void*)(( void*) Cyc_Absyn_Extern));
_temp10->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( _temp10->vardecls,(*
qmain).f2, _temp16); _temp3->imports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(
_temp3->exports, 0, _temp10); return _temp3;}}}}} static void Cyc_Interface_err(
struct _tagged_string msg){ Cyc_Tcutil_terr( 0, msg);} static void Cyc_Interface_fields_err(
struct _tagged_string sc, struct _tagged_string t, struct _tuple0* n){ Cyc_Interface_err(({
struct _tagged_string _temp24= sc; struct _tagged_string _temp25= t; struct
_tagged_string _temp26= Cyc_Absynpp_qvar2string( n); xprintf("fields of %.*s %.*s %.*s have never been defined",
_temp24.last_plus_one - _temp24.curr, _temp24.curr, _temp25.last_plus_one -
_temp25.curr, _temp25.curr, _temp26.last_plus_one - _temp26.curr, _temp26.curr);}));}
static void Cyc_Interface_body_err( struct _tagged_string sc, struct _tuple0* n){
Cyc_Interface_err(({ struct _tagged_string _temp27= sc; struct _tagged_string
_temp28= Cyc_Absynpp_qvar2string( n); xprintf("the body of %.*s function %.*s has never been defined",
_temp27.last_plus_one - _temp27.curr, _temp27.curr, _temp28.last_plus_one -
_temp28.curr, _temp28.curr);}));} static void Cyc_Interface_static_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp29= obj1;
struct _tagged_string _temp30= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp31= obj2; struct _tagged_string _temp32= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on static %.*s %.*s", _temp29.last_plus_one
- _temp29.curr, _temp29.curr, _temp30.last_plus_one - _temp30.curr, _temp30.curr,
_temp31.last_plus_one - _temp31.curr, _temp31.curr, _temp32.last_plus_one -
_temp32.curr, _temp32.curr);}));} static void Cyc_Interface_abstract_err( struct
_tagged_string obj1, struct _tuple0* name1, struct _tagged_string obj2, struct
_tuple0* name2){ Cyc_Interface_err(({ struct _tagged_string _temp33= obj1;
struct _tagged_string _temp34= Cyc_Absynpp_qvar2string( name1); struct
_tagged_string _temp35= obj2; struct _tagged_string _temp36= Cyc_Absynpp_qvar2string(
name2); xprintf("declaration of %.*s %.*s relies on fields of abstract %.*s %.*s",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr, _temp35.last_plus_one - _temp35.curr, _temp35.curr,
_temp36.last_plus_one - _temp36.curr, _temp36.curr);}));} struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict* structs; struct Cyc_Dict_Dict* unions; struct Cyc_Dict_Dict*
tunions; } ; typedef struct Cyc_Interface_Seen* Cyc_Interface_seen_t; static
struct Cyc_Interface_Seen* Cyc_Interface_new_seen(){ return({ struct Cyc_Interface_Seen*
_temp37=( struct Cyc_Interface_Seen*) GC_malloc( sizeof( struct Cyc_Interface_Seen));
_temp37->structs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*,
struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp); _temp37->unions=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp); _temp37->tunions=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tuple0*, struct _tuple0*))) Cyc_Dict_empty)( Cyc_Absyn_qvar_cmp);
_temp37;});} typedef struct Cyc_Dict_Dict* Cyc_Interface_aenv_t; static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t); static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l){ int
_temp38= 1; goto _LL39; _LL39: for( 0; l != 0; l=(( struct Cyc_List_List*)
_check_null( l))->tl){ if( ! Cyc_Interface_check_public_type( ae, seen, obj,
name, f(( void*)(( struct Cyc_List_List*) _check_null( l))->hd))){ _temp38= 0;}}
return _temp38;} static int Cyc_Interface_check_public_structdecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Structdecl* d){ struct
_handler_cons _temp40; _push_handler(& _temp40);{ int _temp42= 0; if( setjmp(
_temp40.handler)){ _temp42= 1;} if( ! _temp42){{ int _temp43=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v); _npop_handler( 0u);
return _temp43;}; _pop_handler();} else{ void* _temp41=( void*) _exn_thrown;
void* _temp45= _temp41; _LL47: if( _temp45 == Cyc_Dict_Absent){ goto _LL48;}
else{ goto _LL49;} _LL49: goto _LL50; _LL48: { int _temp51= 1; goto _LL52; _LL52:
seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp51); if( d->fields != 0){
struct Cyc_List_List* _temp53=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL54; _LL54: for( 0; _temp53 != 0; _temp53=((
struct Cyc_List_List*) _check_null( _temp53))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ char* _temp55="struct"; struct _tagged_string _temp56; _temp56.curr=
_temp55; _temp56.base= _temp55; _temp56.last_plus_one= _temp55 + 7; _temp56;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp53))->hd)->type)){
_temp51= 0;}}} seen->structs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->structs,( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp51); return
_temp51;} _LL50:( void) _throw( _temp45); _LL46:;}}} static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Uniondecl*
d){ struct _handler_cons _temp57; _push_handler(& _temp57);{ int _temp59= 0; if(
setjmp( _temp57.handler)){ _temp59= 1;} if( ! _temp59){{ int _temp60=(( int(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->unions,(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v);
_npop_handler( 0u); return _temp60;}; _pop_handler();} else{ void* _temp58=(
void*) _exn_thrown; void* _temp62= _temp58; _LL64: if( _temp62 == Cyc_Dict_Absent){
goto _LL65;} else{ goto _LL66;} _LL66: goto _LL67; _LL65: { int _temp68= 1; goto
_LL69; _LL69: seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp68); if( d->fields != 0){
struct Cyc_List_List* _temp70=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( d->fields))->v; goto _LL71; _LL71: for( 0; _temp70 != 0; _temp70=((
struct Cyc_List_List*) _check_null( _temp70))->tl){ if( ! Cyc_Interface_check_public_type(
ae, seen,({ char* _temp72="union"; struct _tagged_string _temp73; _temp73.curr=
_temp72; _temp73.base= _temp72; _temp73.last_plus_one= _temp72 + 6; _temp73;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d->name))->v,( void*)((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( _temp70))->hd)->type)){
_temp68= 0;}}} seen->unions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->unions,( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( d->name))->v, _temp68); return _temp68;}
_LL67:( void) _throw( _temp62); _LL63:;}}} struct _tuple10{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static void* Cyc_Interface_get_type1( struct _tuple10* x){
return(* x).f2;} static int Cyc_Interface_check_public_tuniondecl( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Tuniondecl* d){ struct
_handler_cons _temp74; _push_handler(& _temp74);{ int _temp76= 0; if( setjmp(
_temp74.handler)){ _temp76= 1;} if( ! _temp76){{ int _temp77=(( int(*)( struct
Cyc_Dict_Dict* d, struct _tuple0* key)) Cyc_Dict_lookup)( seen->tunions, d->name);
_npop_handler( 0u); return _temp77;}; _pop_handler();} else{ void* _temp75=(
void*) _exn_thrown; void* _temp79= _temp75; _LL81: if( _temp79 == Cyc_Dict_Absent){
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL82: { int _temp85= 1; goto
_LL86; _LL86: seen->tunions=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions, d->name,
_temp85); if( d->fields != 0){ struct Cyc_List_List* _temp87=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( d->fields))->v; goto _LL88; _LL88: for( 0;
_temp87 != 0; _temp87=(( struct Cyc_List_List*) _check_null( _temp87))->tl){ if(
!(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct
_tagged_string obj, struct _tuple0* name, void*(* f)( struct _tuple10*), struct
Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen,({ char*
_temp89="[x]tunion"; struct _tagged_string _temp90; _temp90.curr= _temp89;
_temp90.base= _temp89; _temp90.last_plus_one= _temp89 + 10; _temp90;}), d->name,
Cyc_Interface_get_type1,(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( _temp87))->hd)->typs)){ _temp85= 0;}}} seen->tunions=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tuple0* key, int data)) Cyc_Dict_insert)( seen->tunions,
d->name, _temp85); return _temp85;} _LL84:( void) _throw( _temp79); _LL80:;}}}
static int Cyc_Interface_check_public_enumdecl( struct Cyc_Dict_Dict* ae, struct
Cyc_Interface_Seen* seen, struct Cyc_Absyn_Enumdecl* d){ return 1;} static int
Cyc_Interface_check_public_typedefdecl( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct Cyc_Absyn_Typedefdecl* d){ return Cyc_Interface_check_public_type(
ae, seen,({ char* _temp91="typedef"; struct _tagged_string _temp92; _temp92.curr=
_temp91; _temp92.base= _temp91; _temp92.last_plus_one= _temp91 + 8; _temp92;}),
d->name,( void*) d->defn);} static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct Cyc_Absyn_Vardecl*
d){ return Cyc_Interface_check_public_type( ae, seen,({ char* _temp93="variable";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 9; _temp94;}), d->name,( void*) d->type);}
static void* Cyc_Interface_get_type2( struct _tuple1* x){ return(* x).f3;}
static struct Cyc_List_List* Cyc_Interface_get_abs_ns( void* ns){ void* _temp95=
ns; struct Cyc_List_List* _temp101; _LL97: if(( unsigned int) _temp95 > 1u?*((
int*) _temp95) == Cyc_Absyn_Abs_n: 0){ _LL102: _temp101=(( struct Cyc_Absyn_Abs_n_struct*)
_temp95)->f1; goto _LL98;} else{ goto _LL99;} _LL99: goto _LL100; _LL98: return
_temp101; _LL100:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp103=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp103[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp104; _temp104.tag= Cyc_Core_InvalidArg;
_temp104.f1=({ char* _temp105="get_abs_ns"; struct _tagged_string _temp106;
_temp106.curr= _temp105; _temp106.base= _temp105; _temp106.last_plus_one=
_temp105 + 11; _temp106;}); _temp104;}); _temp103;})); _LL96:;} static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void* t){ void* _temp107= Cyc_Tcutil_compress( t);
struct Cyc_Absyn_PtrInfo _temp129; struct Cyc_Absyn_Conref* _temp131; struct Cyc_Absyn_Tqual
_temp133; struct Cyc_Absyn_Conref* _temp135; void* _temp137; void* _temp139;
struct Cyc_Absyn_Exp* _temp141; struct Cyc_Absyn_Tqual _temp143; void* _temp145;
struct Cyc_Core_Opt* _temp147; struct Cyc_Core_Opt _temp149; void* _temp150;
struct Cyc_List_List* _temp152; struct _tuple0* _temp154; struct Cyc_Absyn_FnInfo
_temp156; struct Cyc_List_List* _temp158; int _temp160; struct Cyc_List_List*
_temp162; void* _temp164; struct Cyc_Core_Opt* _temp166; struct Cyc_List_List*
_temp168; struct Cyc_List_List* _temp170; struct Cyc_Absyn_Structdecl** _temp172;
struct Cyc_Absyn_Structdecl* _temp174; struct Cyc_List_List* _temp175; struct
_tuple0* _temp177; struct Cyc_Absyn_Uniondecl** _temp179; struct Cyc_Absyn_Uniondecl*
_temp181; struct Cyc_List_List* _temp182; struct _tuple0* _temp184; struct Cyc_Absyn_TunionInfo
_temp186; void* _temp188; struct Cyc_List_List* _temp190; void* _temp192; struct
Cyc_Absyn_Tuniondecl* _temp194; struct Cyc_Absyn_TunionFieldInfo _temp196;
struct Cyc_List_List* _temp198; void* _temp200; struct Cyc_Absyn_Tunionfield*
_temp202; struct Cyc_Absyn_Tuniondecl* _temp204; _LL109: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_PointerType: 0){ _LL130: _temp129=((
struct Cyc_Absyn_PointerType_struct*) _temp107)->f1; _LL140: _temp139=( void*)
_temp129.elt_typ; goto _LL138; _LL138: _temp137=( void*) _temp129.rgn_typ; goto
_LL136; _LL136: _temp135= _temp129.nullable; goto _LL134; _LL134: _temp133=
_temp129.tq; goto _LL132; _LL132: _temp131= _temp129.bounds; goto _LL110;} else{
goto _LL111;} _LL111: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) ==
Cyc_Absyn_ArrayType: 0){ _LL146: _temp145=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp107)->f1; goto _LL144; _LL144: _temp143=(( struct Cyc_Absyn_ArrayType_struct*)
_temp107)->f2; goto _LL142; _LL142: _temp141=(( struct Cyc_Absyn_ArrayType_struct*)
_temp107)->f3; goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TypedefType: 0){ _LL155: _temp154=((
struct Cyc_Absyn_TypedefType_struct*) _temp107)->f1; goto _LL153; _LL153:
_temp152=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f2; goto _LL148;
_LL148: _temp147=(( struct Cyc_Absyn_TypedefType_struct*) _temp107)->f3; if(
_temp147 == 0){ goto _LL115;} else{ _temp149=* _temp147; _LL151: _temp150=( void*)
_temp149.v; goto _LL114;}} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_FnType: 0){ _LL157: _temp156=((
struct Cyc_Absyn_FnType_struct*) _temp107)->f1; _LL169: _temp168= _temp156.tvars;
goto _LL167; _LL167: _temp166= _temp156.effect; goto _LL165; _LL165: _temp164=(
void*) _temp156.ret_typ; goto _LL163; _LL163: _temp162= _temp156.args; goto
_LL161; _LL161: _temp160= _temp156.varargs; goto _LL159; _LL159: _temp158=
_temp156.attributes; goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int)
_temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TupleType: 0){ _LL171: _temp170=((
struct Cyc_Absyn_TupleType_struct*) _temp107)->f1; goto _LL118;} else{ goto
_LL119;} _LL119: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_StructType:
0){ _LL178: _temp177=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f1; goto
_LL176; _LL176: _temp175=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f2;
goto _LL173; _LL173: _temp172=(( struct Cyc_Absyn_StructType_struct*) _temp107)->f3;
if( _temp172 == 0){ goto _LL121;} else{ _temp174=* _temp172; goto _LL120;}}
else{ goto _LL121;} _LL121: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107)
== Cyc_Absyn_UnionType: 0){ _LL185: _temp184=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f1; goto _LL183; _LL183: _temp182=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f2; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_UnionType_struct*)
_temp107)->f3; if( _temp179 == 0){ goto _LL123;} else{ _temp181=* _temp179; goto
_LL122;}} else{ goto _LL123;} _LL123: if(( unsigned int) _temp107 > 4u?*(( int*)
_temp107) == Cyc_Absyn_TunionType: 0){ _LL187: _temp186=(( struct Cyc_Absyn_TunionType_struct*)
_temp107)->f1; _LL193: _temp192=( void*) _temp186.tunion_info; if(*(( int*)
_temp192) == Cyc_Absyn_KnownTunion){ _LL195: _temp194=(( struct Cyc_Absyn_KnownTunion_struct*)
_temp192)->f1; goto _LL191;} else{ goto _LL125;} _LL191: _temp190= _temp186.targs;
goto _LL189; _LL189: _temp188=( void*) _temp186.rgn; goto _LL124;} else{ goto
_LL125;} _LL125: if(( unsigned int) _temp107 > 4u?*(( int*) _temp107) == Cyc_Absyn_TunionFieldType:
0){ _LL197: _temp196=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp107)->f1;
_LL201: _temp200=( void*) _temp196.field_info; if(*(( int*) _temp200) == Cyc_Absyn_KnownTunionfield){
_LL205: _temp204=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp200)->f1;
goto _LL203; _LL203: _temp202=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp200)->f2; goto _LL199;} else{ goto _LL127;} _LL199: _temp198= _temp196.targs;
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL110: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp139); _LL112: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp145); _LL114: return Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp150); _LL116: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple1*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type2, _temp162)? Cyc_Interface_check_public_type(
ae, seen, obj, name, _temp164): 0; _LL118: return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( struct _tuple10*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name, Cyc_Interface_get_type1, _temp170); _LL120: if(( void*)
_temp174->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({
char* _temp206="struct"; struct _tagged_string _temp207; _temp207.curr= _temp206;
_temp207.base= _temp206; _temp207.last_plus_one= _temp206 + 7; _temp207;}),(
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp174->name))->v);
return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen*
seen, struct _tagged_string obj, struct _tuple0* name, void*(* f)( void*),
struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)( ae, seen, obj,
name,( void*(*)( void*)) Cyc_Core_identity, _temp175)? Cyc_Interface_check_public_structdecl(
ae, seen, _temp174): 0; _LL122: if(( void*) _temp181->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ char* _temp208="union"; struct
_tagged_string _temp209; _temp209.curr= _temp208; _temp209.base= _temp208;
_temp209.last_plus_one= _temp208 + 6; _temp209;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp181->name))->v); return 0;} return(( int(*)(
struct Cyc_Dict_Dict* ae, struct Cyc_Interface_Seen* seen, struct _tagged_string
obj, struct _tuple0* name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp182)? Cyc_Interface_check_public_uniondecl(
ae, seen, _temp181): 0; _LL124: { struct _tuple0 _temp212; struct _tagged_string*
_temp213; void* _temp215; struct _tuple0* _temp210= _temp194->name; _temp212=*
_temp210; _LL216: _temp215= _temp212.f1; goto _LL214; _LL214: _temp213= _temp212.f2;
goto _LL211; _LL211: { struct Cyc_List_List* _temp217= Cyc_Interface_get_abs_ns(
_temp215); goto _LL218; _LL218: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp219; _push_handler(& _temp219);{ int _temp221= 0; if( setjmp(
_temp219.handler)){ _temp221= 1;} if( ! _temp221){{ struct Cyc_Tcenv_Genv*
_temp222=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp217); goto _LL223; _LL223: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp222->tuniondecls,
_temp213);}; _pop_handler();} else{ void* _temp220=( void*) _exn_thrown; void*
_temp225= _temp220; _LL227: if( _temp225 == Cyc_Dict_Absent){ goto _LL228;}
else{ goto _LL229;} _LL229: goto _LL230; _LL228:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp231=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp231[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp232; _temp232.tag= Cyc_Core_InvalidArg; _temp232.f1=({
char* _temp233="check_public_type"; struct _tagged_string _temp234; _temp234.curr=
_temp233; _temp234.base= _temp233; _temp234.last_plus_one= _temp233 + 18;
_temp234;}); _temp232;}); _temp231;})); _LL230:( void) _throw( _temp225); _LL226:;}}}
if(( void*) tud->sc ==( void*) Cyc_Absyn_Static){ Cyc_Interface_static_err( obj,
name,({ char* _temp235="[x]tunion"; struct _tagged_string _temp236; _temp236.curr=
_temp235; _temp236.base= _temp235; _temp236.last_plus_one= _temp235 + 10;
_temp236;}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict* ae,
struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0* name,
void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp190)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}} _LL126: { struct _tuple0 _temp239; struct _tagged_string*
_temp240; void* _temp242; struct _tuple0* _temp237= _temp204->name; _temp239=*
_temp237; _LL243: _temp242= _temp239.f1; goto _LL241; _LL241: _temp240= _temp239.f2;
goto _LL238; _LL238: { struct Cyc_List_List* _temp244= Cyc_Interface_get_abs_ns(
_temp242); goto _LL245; _LL245: { struct Cyc_Absyn_Tuniondecl* tud;{ struct
_handler_cons _temp246; _push_handler(& _temp246);{ int _temp248= 0; if( setjmp(
_temp246.handler)){ _temp248= 1;} if( ! _temp248){{ struct Cyc_Tcenv_Genv*
_temp249=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( ae, _temp244); goto _LL250; _LL250: tud=*(( struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp249->tuniondecls,
_temp240);}; _pop_handler();} else{ void* _temp247=( void*) _exn_thrown; void*
_temp252= _temp247; _LL254: if( _temp252 == Cyc_Dict_Absent){ goto _LL255;}
else{ goto _LL256;} _LL256: goto _LL257; _LL255:( void) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp258=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp258[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp259; _temp259.tag= Cyc_Core_InvalidArg; _temp259.f1=({
char* _temp260="check_public_type"; struct _tagged_string _temp261; _temp261.curr=
_temp260; _temp261.base= _temp260; _temp261.last_plus_one= _temp260 + 18;
_temp261;}); _temp259;}); _temp258;})); _LL257:( void) _throw( _temp252); _LL253:;}}}
if( tud->fields == 0){( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp262=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp262[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp263; _temp263.tag= Cyc_Core_InvalidArg;
_temp263.f1=({ char* _temp264="check_public_type"; struct _tagged_string
_temp265; _temp265.curr= _temp264; _temp265.base= _temp264; _temp265.last_plus_one=
_temp264 + 18; _temp265;}); _temp263;}); _temp262;}));}{ struct Cyc_Absyn_Tunionfield*
tuf1= 0;{ struct Cyc_List_List* _temp266=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( tud->fields))->v; goto _LL267; _LL267: for( 0; _temp266 != 0;
_temp266=(( struct Cyc_List_List*) _check_null( _temp266))->tl){ int cmp; if((
cmp= Cyc_Absyn_qvar_cmp( _temp202->name,(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp266))->hd)->name)) >= 0){ if( cmp == 0){
tuf1=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( _temp266))->hd);} break;}}} if( tuf1 == 0){( void)
_throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp268=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp268[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp269; _temp269.tag= Cyc_Core_InvalidArg; _temp269.f1=({
char* _temp270="check_public_type"; struct _tagged_string _temp271; _temp271.curr=
_temp270; _temp271.base= _temp270; _temp271.last_plus_one= _temp270 + 18;
_temp271;}); _temp269;}); _temp268;}));}{ struct Cyc_Absyn_Tunionfield* tuf=(
struct Cyc_Absyn_Tunionfield*) _check_null( tuf1); if(( void*) tud->sc ==( void*)
Cyc_Absyn_Static){ Cyc_Interface_static_err( obj, name,({ char* _temp272="[x]tunion";
struct _tagged_string _temp273; _temp273.curr= _temp272; _temp273.base= _temp272;
_temp273.last_plus_one= _temp272 + 10; _temp273;}), tud->name); return 0;} if((
void*) tud->sc ==( void*) Cyc_Absyn_Abstract){ Cyc_Interface_abstract_err( obj,
name,({ char* _temp274="[x]tunion"; struct _tagged_string _temp275; _temp275.curr=
_temp274; _temp275.base= _temp274; _temp275.last_plus_one= _temp274 + 10;
_temp275;}), tud->name); return 0;} if(( void*) tuf->sc ==( void*) Cyc_Absyn_Static){
Cyc_Interface_static_err( obj, name,({ struct _tagged_string _temp276= Cyc_Absynpp_qvar2string(
tuf->name); xprintf("field %.*s of", _temp276.last_plus_one - _temp276.curr,
_temp276.curr);}), tud->name); return 0;} return(( int(*)( struct Cyc_Dict_Dict*
ae, struct Cyc_Interface_Seen* seen, struct _tagged_string obj, struct _tuple0*
name, void*(* f)( void*), struct Cyc_List_List* l)) Cyc_Interface_check_public_type_list)(
ae, seen, obj, name,( void*(*)( void*)) Cyc_Core_identity, _temp198)? Cyc_Interface_check_public_tuniondecl(
ae, seen, tud): 0;}}}}} _LL128: return 1; _LL108:;} struct _tuple11{ struct Cyc_Interface_Ienv*
f1; struct Cyc_Interface_Ienv* f2; int f3; struct Cyc_Dict_Dict* f4; struct Cyc_Interface_Seen*
f5; struct Cyc_Interface_I* f6; } ; static void Cyc_Interface_extract_structdecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Structdecl** dp){
struct _tuple11 _temp279; struct Cyc_Interface_I* _temp280; struct Cyc_Interface_Seen*
_temp282; struct Cyc_Dict_Dict* _temp284; int _temp286; struct Cyc_Interface_Ienv*
_temp288; struct Cyc_Interface_Ienv* _temp290; struct _tuple11* _temp277= env;
_temp279=* _temp277; _LL291: _temp290= _temp279.f1; goto _LL289; _LL289:
_temp288= _temp279.f2; goto _LL287; _LL287: _temp286= _temp279.f3; goto _LL285;
_LL285: _temp284= _temp279.f4; goto _LL283; _LL283: _temp282= _temp279.f5; goto
_LL281; _LL281: _temp280= _temp279.f6; goto _LL278; _LL278: { struct Cyc_Absyn_Structdecl*
_temp292=* dp; goto _LL293; _LL293: { void* _temp294=( void*) _temp292->sc;
_LL296: if( _temp294 ==( void*) Cyc_Absyn_Static){ goto _LL297;} else{ goto
_LL298;} _LL298: if( _temp294 ==( void*) Cyc_Absyn_Abstract){ goto _LL299;}
else{ goto _LL300;} _LL300: if( _temp294 ==( void*) Cyc_Absyn_Public){ goto
_LL301;} else{ goto _LL302;} _LL302: if( _temp294 ==( void*) Cyc_Absyn_ExternC){
goto _LL303;} else{ goto _LL304;} _LL304: if( _temp294 ==( void*) Cyc_Absyn_Extern){
goto _LL305;} else{ goto _LL295;} _LL297: if( _temp286? _temp292->fields == 0: 0){
Cyc_Interface_fields_err(({ char* _temp306="static"; struct _tagged_string
_temp307; _temp307.curr= _temp306; _temp307.base= _temp306; _temp307.last_plus_one=
_temp306 + 7; _temp307;}),({ char* _temp308="struct"; struct _tagged_string
_temp309; _temp309.curr= _temp308; _temp309.base= _temp308; _temp309.last_plus_one=
_temp308 + 7; _temp309;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp292->name))->v);} goto _LL295; _LL299: if( _temp292->fields == 0){ if(
_temp286){ Cyc_Interface_fields_err(({ char* _temp310="abstract"; struct
_tagged_string _temp311; _temp311.curr= _temp310; _temp311.base= _temp310;
_temp311.last_plus_one= _temp310 + 9; _temp311;}),({ char* _temp312="struct";
struct _tagged_string _temp313; _temp313.curr= _temp312; _temp313.base= _temp312;
_temp313.last_plus_one= _temp312 + 7; _temp313;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp292->name))->v);}} else{ _temp292=({ struct Cyc_Absyn_Structdecl*
_temp314=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp314[ 0]=* _temp292; _temp314;}); _temp292->fields= 0;} if( Cyc_Interface_check_public_structdecl(
_temp284, _temp282, _temp292)){ _temp288->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp288->structdecls, x, _temp292);} goto _LL295;
_LL301: if( _temp292->fields == 0){ Cyc_Interface_fields_err(({ char* _temp315="public";
struct _tagged_string _temp316; _temp316.curr= _temp315; _temp316.base= _temp315;
_temp316.last_plus_one= _temp315 + 7; _temp316;}),({ char* _temp317="struct";
struct _tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 7; _temp318;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp292->name))->v); _temp292=({ struct Cyc_Absyn_Structdecl*
_temp319=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp319[ 0]=* _temp292; _temp319;});( void*)( _temp292->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_structdecl( _temp284,
_temp282, _temp292)){ _temp288->structdecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data))
Cyc_Dict_insert)( _temp288->structdecls, x, _temp292);} goto _LL295; _LL303:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp320=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp320[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp321; _temp321.tag= Cyc_Core_InvalidArg; _temp321.f1=({
char* _temp322="extract_structdecl"; struct _tagged_string _temp323; _temp323.curr=
_temp322; _temp323.base= _temp322; _temp323.last_plus_one= _temp322 + 19;
_temp323;}); _temp321;}); _temp320;})); _LL305: if( Cyc_Interface_check_public_structdecl(
_temp284, _temp282, _temp292)){ _temp290->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl*
data)) Cyc_Dict_insert)( _temp290->structdecls, x, _temp292);} goto _LL295;
_LL295:;}}} static void Cyc_Interface_extract_uniondecl( struct _tuple11* env,
struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){ struct _tuple11
_temp326; struct Cyc_Interface_I* _temp327; struct Cyc_Interface_Seen* _temp329;
struct Cyc_Dict_Dict* _temp331; int _temp333; struct Cyc_Interface_Ienv*
_temp335; struct Cyc_Interface_Ienv* _temp337; struct _tuple11* _temp324= env;
_temp326=* _temp324; _LL338: _temp337= _temp326.f1; goto _LL336; _LL336:
_temp335= _temp326.f2; goto _LL334; _LL334: _temp333= _temp326.f3; goto _LL332;
_LL332: _temp331= _temp326.f4; goto _LL330; _LL330: _temp329= _temp326.f5; goto
_LL328; _LL328: _temp327= _temp326.f6; goto _LL325; _LL325: { struct Cyc_Absyn_Uniondecl*
_temp339=* dp; goto _LL340; _LL340: { void* _temp341=( void*) _temp339->sc;
_LL343: if( _temp341 ==( void*) Cyc_Absyn_Static){ goto _LL344;} else{ goto
_LL345;} _LL345: if( _temp341 ==( void*) Cyc_Absyn_Abstract){ goto _LL346;}
else{ goto _LL347;} _LL347: if( _temp341 ==( void*) Cyc_Absyn_Public){ goto
_LL348;} else{ goto _LL349;} _LL349: if( _temp341 ==( void*) Cyc_Absyn_ExternC){
goto _LL350;} else{ goto _LL351;} _LL351: if( _temp341 ==( void*) Cyc_Absyn_Extern){
goto _LL352;} else{ goto _LL342;} _LL344: if( _temp333? _temp339->fields == 0: 0){
Cyc_Interface_fields_err(({ char* _temp353="static"; struct _tagged_string
_temp354; _temp354.curr= _temp353; _temp354.base= _temp353; _temp354.last_plus_one=
_temp353 + 7; _temp354;}),({ char* _temp355="union"; struct _tagged_string
_temp356; _temp356.curr= _temp355; _temp356.base= _temp355; _temp356.last_plus_one=
_temp355 + 6; _temp356;}),( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
_temp339->name))->v);} goto _LL342; _LL346: if( _temp339->fields == 0){ if(
_temp333){ Cyc_Interface_fields_err(({ char* _temp357="abstract"; struct
_tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 9; _temp358;}),({ char* _temp359="union";
struct _tagged_string _temp360; _temp360.curr= _temp359; _temp360.base= _temp359;
_temp360.last_plus_one= _temp359 + 6; _temp360;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp339->name))->v);}} else{ _temp339=({ struct Cyc_Absyn_Uniondecl*
_temp361=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp361[ 0]=* _temp339; _temp361;}); _temp339->fields= 0;} if( Cyc_Interface_check_public_uniondecl(
_temp331, _temp329, _temp339)){ _temp335->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp335->uniondecls, x, _temp339);} goto _LL342;
_LL348: if( _temp339->fields == 0){ Cyc_Interface_fields_err(({ char* _temp362="public";
struct _tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 7; _temp363;}),({ char* _temp364="union";
struct _tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 6; _temp365;}),( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp339->name))->v); _temp339=({ struct Cyc_Absyn_Uniondecl*
_temp366=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp366[ 0]=* _temp339; _temp366;});( void*)( _temp339->sc=( void*)(( void*)
Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_uniondecl( _temp331,
_temp329, _temp339)){ _temp335->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data))
Cyc_Dict_insert)( _temp335->uniondecls, x, _temp339);} goto _LL342; _LL350:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp367=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp367[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp368; _temp368.tag= Cyc_Core_InvalidArg; _temp368.f1=({
char* _temp369="extract_uniondecl"; struct _tagged_string _temp370; _temp370.curr=
_temp369; _temp370.base= _temp369; _temp370.last_plus_one= _temp369 + 18;
_temp370;}); _temp368;}); _temp367;})); _LL352: if( Cyc_Interface_check_public_uniondecl(
_temp331, _temp329, _temp339)){ _temp337->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl*
data)) Cyc_Dict_insert)( _temp337->uniondecls, x, _temp339);} goto _LL342;
_LL342:;}}} static void Cyc_Interface_extract_enumdecl( struct _tuple11* env,
struct _tagged_string* x, struct Cyc_Absyn_Enumdecl** dp){ struct _tuple11
_temp373; struct Cyc_Interface_I* _temp374; struct Cyc_Interface_Seen* _temp376;
struct Cyc_Dict_Dict* _temp378; int _temp380; struct Cyc_Interface_Ienv*
_temp382; struct Cyc_Interface_Ienv* _temp384; struct _tuple11* _temp371= env;
_temp373=* _temp371; _LL385: _temp384= _temp373.f1; goto _LL383; _LL383:
_temp382= _temp373.f2; goto _LL381; _LL381: _temp380= _temp373.f3; goto _LL379;
_LL379: _temp378= _temp373.f4; goto _LL377; _LL377: _temp376= _temp373.f5; goto
_LL375; _LL375: _temp374= _temp373.f6; goto _LL372; _LL372: { struct Cyc_Absyn_Enumdecl*
_temp386=* dp; goto _LL387; _LL387: { void* _temp388=( void*) _temp386->sc;
_LL390: if( _temp388 ==( void*) Cyc_Absyn_Static){ goto _LL391;} else{ goto
_LL392;} _LL392: if( _temp388 ==( void*) Cyc_Absyn_Abstract){ goto _LL393;}
else{ goto _LL394;} _LL394: if( _temp388 ==( void*) Cyc_Absyn_Public){ goto
_LL395;} else{ goto _LL396;} _LL396: if( _temp388 ==( void*) Cyc_Absyn_ExternC){
goto _LL397;} else{ goto _LL398;} _LL398: if( _temp388 ==( void*) Cyc_Absyn_Extern){
goto _LL399;} else{ goto _LL389;} _LL391: if( _temp380? _temp386->fields == 0: 0){
Cyc_Interface_fields_err(({ char* _temp400="static"; struct _tagged_string
_temp401; _temp401.curr= _temp400; _temp401.base= _temp400; _temp401.last_plus_one=
_temp400 + 7; _temp401;}),({ char* _temp402="enum"; struct _tagged_string
_temp403; _temp403.curr= _temp402; _temp403.base= _temp402; _temp403.last_plus_one=
_temp402 + 5; _temp403;}), _temp386->name);} goto _LL389; _LL393: if( _temp386->fields
== 0){ if( _temp380){ Cyc_Interface_fields_err(({ char* _temp404="abstract";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 9; _temp405;}),({ char* _temp406="enum";
struct _tagged_string _temp407; _temp407.curr= _temp406; _temp407.base= _temp406;
_temp407.last_plus_one= _temp406 + 5; _temp407;}), _temp386->name);}} else{
_temp386=({ struct Cyc_Absyn_Enumdecl* _temp408=( struct Cyc_Absyn_Enumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl)); _temp408[ 0]=* _temp386;
_temp408;}); _temp386->fields= 0;} if( Cyc_Interface_check_public_enumdecl(
_temp378, _temp376, _temp386)){ _temp382->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp382->enumdecls, x, _temp386);} goto _LL389; _LL395:
if( _temp386->fields == 0){ Cyc_Interface_fields_err(({ char* _temp409="public";
struct _tagged_string _temp410; _temp410.curr= _temp409; _temp410.base= _temp409;
_temp410.last_plus_one= _temp409 + 7; _temp410;}),({ char* _temp411="enum";
struct _tagged_string _temp412; _temp412.curr= _temp411; _temp412.base= _temp411;
_temp412.last_plus_one= _temp411 + 5; _temp412;}), _temp386->name); _temp386=({
struct Cyc_Absyn_Enumdecl* _temp413=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp413[ 0]=* _temp386; _temp413;});( void*)(
_temp386->sc=( void*)(( void*) Cyc_Absyn_Abstract));} if( Cyc_Interface_check_public_enumdecl(
_temp378, _temp376, _temp386)){ _temp382->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp382->enumdecls, x, _temp386);} goto _LL389; _LL397:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp414=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp414[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp415; _temp415.tag= Cyc_Core_InvalidArg; _temp415.f1=({
char* _temp416="extract_enumdecl"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 17;
_temp417;}); _temp415;}); _temp414;})); _LL399: if( Cyc_Interface_check_public_enumdecl(
_temp378, _temp376, _temp386)){ _temp384->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl*
data)) Cyc_Dict_insert)( _temp384->enumdecls, x, _temp386);} goto _LL389; _LL389:;}}}
static void Cyc_Interface_extract_xtunionfielddecl( struct Cyc_Interface_I* i,
struct Cyc_Absyn_Tuniondecl* d, struct Cyc_Absyn_Tunionfield* f){ struct Cyc_List_List*
_temp418= 0; goto _LL419; _LL419:{ void* _temp420=(* f->name).f1; struct Cyc_List_List*
_temp428; struct Cyc_List_List* _temp430; _LL422: if(( unsigned int) _temp420 >
1u?*(( int*) _temp420) == Cyc_Absyn_Rel_n: 0){ _LL429: _temp428=(( struct Cyc_Absyn_Rel_n_struct*)
_temp420)->f1; goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int)
_temp420 > 1u?*(( int*) _temp420) == Cyc_Absyn_Abs_n: 0){ _LL431: _temp430=((
struct Cyc_Absyn_Abs_n_struct*) _temp420)->f1; goto _LL425;} else{ goto _LL426;}
_LL426: goto _LL427; _LL423: _temp430= _temp428; goto _LL425; _LL425: _temp418=
_temp430; goto _LL421; _LL427: goto _LL421; _LL421:;}{ struct Cyc_Dict_Dict**
dict;{ void* _temp432=( void*) f->sc; _LL434: if( _temp432 ==( void*) Cyc_Absyn_Static){
goto _LL435;} else{ goto _LL436;} _LL436: if( _temp432 ==( void*) Cyc_Absyn_Extern){
goto _LL437;} else{ goto _LL438;} _LL438: if( _temp432 ==( void*) Cyc_Absyn_Public){
goto _LL439;} else{ goto _LL440;} _LL440: goto _LL441; _LL435: return; _LL437:
dict=& i->imports; goto _LL433; _LL439: dict=& i->exports; goto _LL433; _LL441:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp442=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp442[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp443; _temp443.tag= Cyc_Core_InvalidArg; _temp443.f1=({
char* _temp444="add_xtunionfielddecl"; struct _tagged_string _temp445; _temp445.curr=
_temp444; _temp445.base= _temp444; _temp445.last_plus_one= _temp444 + 21;
_temp445;}); _temp443;}); _temp442;})); _LL433:;}{ struct Cyc_Interface_Ienv*
env;{ struct _handler_cons _temp446; _push_handler(& _temp446);{ int _temp448= 0;
if( setjmp( _temp446.handler)){ _temp448= 1;} if( ! _temp448){ env=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(* dict,
_temp418);; _pop_handler();} else{ void* _temp447=( void*) _exn_thrown; void*
_temp450= _temp447; _LL452: if( _temp450 == Cyc_Dict_Absent){ goto _LL453;}
else{ goto _LL454;} _LL454: goto _LL455; _LL453: env= Cyc_Interface_new_ienv();*
dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* dict, _temp418, env);
goto _LL451; _LL455:( void) _throw( _temp450); _LL451:;}}} env->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Tcdecl_Xtunionfielddecl* data)) Cyc_Dict_insert)( env->xtunionfielddecls,(*
f->name).f2,({ struct Cyc_Tcdecl_Xtunionfielddecl* _temp456=( struct Cyc_Tcdecl_Xtunionfielddecl*)
GC_malloc( sizeof( struct Cyc_Tcdecl_Xtunionfielddecl)); _temp456->base= d;
_temp456->field= f; _temp456;}));}}} static void Cyc_Interface_extract_tuniondecl(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct _tuple11 _temp459; struct Cyc_Interface_I* _temp460; struct Cyc_Interface_Seen*
_temp462; struct Cyc_Dict_Dict* _temp464; int _temp466; struct Cyc_Interface_Ienv*
_temp468; struct Cyc_Interface_Ienv* _temp470; struct _tuple11* _temp457= env;
_temp459=* _temp457; _LL471: _temp470= _temp459.f1; goto _LL469; _LL469:
_temp468= _temp459.f2; goto _LL467; _LL467: _temp466= _temp459.f3; goto _LL465;
_LL465: _temp464= _temp459.f4; goto _LL463; _LL463: _temp462= _temp459.f5; goto
_LL461; _LL461: _temp460= _temp459.f6; goto _LL458; _LL458: { struct Cyc_Absyn_Tuniondecl*
_temp472=* dp; goto _LL473; _LL473: { void* _temp474=( void*) _temp472->sc;
_LL476: if( _temp474 ==( void*) Cyc_Absyn_Static){ goto _LL477;} else{ goto
_LL478;} _LL478: if( _temp474 ==( void*) Cyc_Absyn_Abstract){ goto _LL479;}
else{ goto _LL480;} _LL480: if( _temp474 ==( void*) Cyc_Absyn_Public){ goto
_LL481;} else{ goto _LL482;} _LL482: if( _temp474 ==( void*) Cyc_Absyn_ExternC){
goto _LL483;} else{ goto _LL484;} _LL484: if( _temp474 ==( void*) Cyc_Absyn_Extern){
goto _LL485;} else{ goto _LL475;} _LL477: if(( ! _temp472->is_xtunion? _temp466:
0)? _temp472->fields == 0: 0){ Cyc_Interface_fields_err(({ char* _temp486="static";
struct _tagged_string _temp487; _temp487.curr= _temp486; _temp487.base= _temp486;
_temp487.last_plus_one= _temp486 + 7; _temp487;}),({ char* _temp488="tunion";
struct _tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 7; _temp489;}), _temp472->name);} goto _LL475;
_LL479: if( _temp472->fields == 0){ if( ! _temp472->is_xtunion? _temp466: 0){
Cyc_Interface_fields_err(({ char* _temp490="abstract"; struct _tagged_string
_temp491; _temp491.curr= _temp490; _temp491.base= _temp490; _temp491.last_plus_one=
_temp490 + 9; _temp491;}),({ char* _temp492="tunion"; struct _tagged_string
_temp493; _temp493.curr= _temp492; _temp493.base= _temp492; _temp493.last_plus_one=
_temp492 + 7; _temp493;}), _temp472->name);}} else{ _temp472=({ struct Cyc_Absyn_Tuniondecl*
_temp494=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp494[ 0]=* _temp472; _temp494;}); _temp472->fields= 0;} if( Cyc_Interface_check_public_tuniondecl(
_temp464, _temp462, _temp472)){ _temp468->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( _temp468->tuniondecls, x, _temp472);} goto _LL475;
_LL481: _temp472=({ struct Cyc_Absyn_Tuniondecl* _temp495=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp495[ 0]=* _temp472;
_temp495;}); if( ! _temp472->is_xtunion? _temp472->fields == 0: 0){ Cyc_Interface_fields_err(({
char* _temp496="public"; struct _tagged_string _temp497; _temp497.curr= _temp496;
_temp497.base= _temp496; _temp497.last_plus_one= _temp496 + 7; _temp497;}),({
char* _temp498="tunion"; struct _tagged_string _temp499; _temp499.curr= _temp498;
_temp499.base= _temp498; _temp499.last_plus_one= _temp498 + 7; _temp499;}),
_temp472->name);( void*)( _temp472->sc=( void*)(( void*) Cyc_Absyn_Abstract));}
if( Cyc_Interface_check_public_tuniondecl( _temp464, _temp462, _temp472)){ if(
_temp472->is_xtunion? _temp472->fields != 0: 0){ struct Cyc_List_List* _temp500=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp472->fields))->v;
goto _LL501; _LL501: _temp472->fields= 0;{ struct Cyc_List_List* _temp502=
_temp500; goto _LL503; _LL503: for( 0; _temp502 != 0; _temp502=(( struct Cyc_List_List*)
_check_null( _temp502))->tl){ Cyc_Interface_extract_xtunionfielddecl( _temp460,
_temp472,( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp502))->hd);}}} _temp468->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl* data))
Cyc_Dict_insert)( _temp468->tuniondecls, x, _temp472);} goto _LL475; _LL483:(
void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp504=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp504[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp505; _temp505.tag= Cyc_Core_InvalidArg; _temp505.f1=({
char* _temp506="extract_tuniondecl"; struct _tagged_string _temp507; _temp507.curr=
_temp506; _temp507.base= _temp506; _temp507.last_plus_one= _temp506 + 19;
_temp507;}); _temp505;}); _temp504;})); _LL485: if( Cyc_Interface_check_public_tuniondecl(
_temp464, _temp462, _temp472)){ if( _temp472->is_xtunion? _temp472->fields != 0:
0){ _temp472=({ struct Cyc_Absyn_Tuniondecl* _temp508=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp508[ 0]=* _temp472;
_temp508;});{ struct Cyc_List_List* _temp509=( struct Cyc_List_List*)(( struct
Cyc_Core_Opt*) _check_null( _temp472->fields))->v; goto _LL510; _LL510: _temp472->fields=
0;{ struct Cyc_List_List* _temp511= _temp509; goto _LL512; _LL512: for( 0;
_temp511 != 0; _temp511=(( struct Cyc_List_List*) _check_null( _temp511))->tl){
Cyc_Interface_extract_xtunionfielddecl( _temp460, _temp472,( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( _temp511))->hd);}}}} _temp470->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( _temp470->tuniondecls, x,
_temp472);} goto _LL475; _LL475:;}}} static void Cyc_Interface_extract_typedef(
struct _tuple11* env, struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){
struct _tuple11 _temp515; struct Cyc_Interface_I* _temp516; struct Cyc_Interface_Seen*
_temp518; struct Cyc_Dict_Dict* _temp520; int _temp522; struct Cyc_Interface_Ienv*
_temp524; struct Cyc_Interface_Ienv* _temp526; struct _tuple11* _temp513= env;
_temp515=* _temp513; _LL527: _temp526= _temp515.f1; goto _LL525; _LL525:
_temp524= _temp515.f2; goto _LL523; _LL523: _temp522= _temp515.f3; goto _LL521;
_LL521: _temp520= _temp515.f4; goto _LL519; _LL519: _temp518= _temp515.f5; goto
_LL517; _LL517: _temp516= _temp515.f6; goto _LL514; _LL514: if( Cyc_Interface_check_public_typedefdecl(
_temp520, _temp518, d)){ _temp524->typedefdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl*
data)) Cyc_Dict_insert)( _temp524->typedefdecls, x, d);}} static void Cyc_Interface_extract_ordinarie(
struct _tuple11* env, struct _tagged_string* x, struct _tuple7* v){ struct
_tuple11 _temp530; struct Cyc_Interface_I* _temp531; struct Cyc_Interface_Seen*
_temp533; struct Cyc_Dict_Dict* _temp535; int _temp537; struct Cyc_Interface_Ienv*
_temp539; struct Cyc_Interface_Ienv* _temp541; struct _tuple11* _temp528= env;
_temp530=* _temp528; _LL542: _temp541= _temp530.f1; goto _LL540; _LL540:
_temp539= _temp530.f2; goto _LL538; _LL538: _temp537= _temp530.f3; goto _LL536;
_LL536: _temp535= _temp530.f4; goto _LL534; _LL534: _temp533= _temp530.f5; goto
_LL532; _LL532: _temp531= _temp530.f6; goto _LL529; _LL529: { void* _temp543=(*
v).f1; goto _LL544; _LL544: { void* _temp545= _temp543; void* _temp551; _LL547:
if(*(( int*) _temp545) == Cyc_Tcenv_VarRes){ _LL552: _temp551=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp545)->f1; goto _LL548;} else{ goto _LL549;}
_LL549: goto _LL550; _LL548:{ void* _temp553= _temp551; struct Cyc_Absyn_Fndecl*
_temp561; struct Cyc_Absyn_Vardecl* _temp563; _LL555: if(( unsigned int)
_temp553 > 1u?*(( int*) _temp553) == Cyc_Absyn_Funname_b: 0){ _LL562: _temp561=((
struct Cyc_Absyn_Funname_b_struct*) _temp553)->f1; goto _LL556;} else{ goto
_LL557;} _LL557: if(( unsigned int) _temp553 > 1u?*(( int*) _temp553) == Cyc_Absyn_Global_b:
0){ _LL564: _temp563=(( struct Cyc_Absyn_Global_b_struct*) _temp553)->f1; goto
_LL558;} else{ goto _LL559;} _LL559: goto _LL560; _LL556: { struct Cyc_Absyn_Vardecl*
_temp567=({ struct Cyc_Absyn_Vardecl* _temp565=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp565->sc=( void*)(( void*)
_temp561->sc); _temp565->name= _temp561->name; _temp565->tq=({ struct Cyc_Absyn_Tqual
_temp566; _temp566.q_const= 0; _temp566.q_volatile= 0; _temp566.q_restrict= 0;
_temp566;}); _temp565->type=( void*)(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp561->cached_typ))->v); _temp565->initializer= 0; _temp565->rgn=
0; _temp565->attributes= 0; _temp565;}); goto _LL568; _LL568: _temp537= 0;
_temp563= _temp567; goto _LL558;} _LL558: if( _temp563->initializer != 0){
_temp563=({ struct Cyc_Absyn_Vardecl* _temp569=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp569[ 0]=* _temp563; _temp569;});
_temp563->initializer= 0;}{ void* _temp570=( void*) _temp563->sc; _LL572: if(
_temp570 ==( void*) Cyc_Absyn_Static){ goto _LL573;} else{ goto _LL574;} _LL574:
if( _temp570 ==( void*) Cyc_Absyn_Abstract){ goto _LL575;} else{ goto _LL576;}
_LL576: if( _temp570 ==( void*) Cyc_Absyn_Public){ goto _LL577;} else{ goto
_LL578;} _LL578: if( _temp570 ==( void*) Cyc_Absyn_ExternC){ goto _LL579;} else{
goto _LL580;} _LL580: if( _temp570 ==( void*) Cyc_Absyn_Extern){ goto _LL581;}
else{ goto _LL571;} _LL573: if( _temp537? Cyc_Tcutil_is_function_type(( void*)
_temp563->type): 0){ Cyc_Interface_body_err(({ char* _temp582="static"; struct
_tagged_string _temp583; _temp583.curr= _temp582; _temp583.base= _temp582;
_temp583.last_plus_one= _temp582 + 7; _temp583;}), _temp563->name);} goto _LL571;
_LL575:( void) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp584=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp584[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp585; _temp585.tag= Cyc_Core_InvalidArg;
_temp585.f1=({ char* _temp586="extract_ordinarie"; struct _tagged_string
_temp587; _temp587.curr= _temp586; _temp587.base= _temp586; _temp587.last_plus_one=
_temp586 + 18; _temp587;}); _temp585;}); _temp584;})); _LL577: if( _temp537? Cyc_Tcutil_is_function_type((
void*) _temp563->type): 0){ Cyc_Interface_body_err(({ char* _temp588="public";
struct _tagged_string _temp589; _temp589.curr= _temp588; _temp589.base= _temp588;
_temp589.last_plus_one= _temp588 + 7; _temp589;}), _temp563->name);} if( Cyc_Interface_check_public_vardecl(
_temp535, _temp533, _temp563)){ _temp539->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( _temp539->vardecls, x, _temp563);} goto _LL571; _LL579:
goto _LL581; _LL581: if( Cyc_Interface_check_public_vardecl( _temp535, _temp533,
_temp563)){ _temp541->vardecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)(
_temp541->vardecls, x, _temp563);} goto _LL571; _LL571:;} goto _LL554; _LL560:
goto _LL554; _LL554:;} goto _LL546; _LL550: goto _LL546; _LL546:;}}} struct
_tuple12{ struct Cyc_Interface_I* f1; int f2; struct Cyc_Dict_Dict* f3; struct
Cyc_Interface_Seen* f4; } ; static void Cyc_Interface_extract_f( struct _tuple12*
env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){ struct _tuple12
_temp592; struct Cyc_Interface_Seen* _temp593; struct Cyc_Dict_Dict* _temp595;
int _temp597; struct Cyc_Interface_I* _temp599; struct _tuple12* _temp590= env_f;
_temp592=* _temp590; _LL600: _temp599= _temp592.f1; goto _LL598; _LL598:
_temp597= _temp592.f2; goto _LL596; _LL596: _temp595= _temp592.f3; goto _LL594;
_LL594: _temp593= _temp592.f4; goto _LL591; _LL591: { struct _tuple11 _temp602=({
struct _tuple11 _temp601; _temp601.f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp599->imports,
ns); _temp601.f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp599->exports, ns); _temp601.f3=
_temp597; _temp601.f4= _temp595; _temp601.f5= _temp593; _temp601.f6= _temp599;
_temp601;}); goto _LL603; _LL603:(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Structdecl**), struct _tuple11* env, struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,& _temp602,
ge->structdecls);(( void(*)( void(* f)( struct _tuple11*, struct _tagged_string*,
struct Cyc_Absyn_Uniondecl**), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl,& _temp602, ge->uniondecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,&
_temp602, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple11* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl,& _temp602, ge->enumdecls);((
void(*)( void(* f)( struct _tuple11*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,&
_temp602, ge->typedefs);(( void(*)( void(* f)( struct _tuple11*, struct
_tagged_string*, struct _tuple7*), struct _tuple11* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,& _temp602, ge->ordinaries);}}
static struct Cyc_Interface_I* Cyc_Interface_gen_extract( struct Cyc_Dict_Dict*
ae, int check_complete_defs){ struct _tuple12 _temp605=({ struct _tuple12
_temp604; _temp604.f1=(( struct Cyc_Interface_I*(*)( struct Cyc_Dict_Dict* skel))
Cyc_Interface_skel2i)( ae); _temp604.f2= check_complete_defs; _temp604.f3= ae;
_temp604.f4= Cyc_Interface_new_seen(); _temp604;}); goto _LL606; _LL606:(( void(*)(
void(* f)( struct _tuple12*, struct Cyc_List_List*, struct Cyc_Tcenv_Genv*),
struct _tuple12* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_f,&
_temp605, ae); return _temp605.f1;} struct Cyc_Interface_I* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} inline
static void Cyc_Interface_check_err( struct _tagged_string* msg1, struct
_tagged_string msg2){ Cyc_Tcdecl_merr( 0, msg1, msg2);} struct _tuple13{ int f1;
struct Cyc_Dict_Dict* f2; int(* f3)( void*, void*, struct _tagged_string*);
struct _tagged_string f4; struct _tagged_string* f5; } ; static void Cyc_Interface_incl_dict_f(
struct _tuple13* env, struct _tagged_string* x, void* y1){ struct _tuple13
_temp609; struct _tagged_string* _temp610; struct _tagged_string _temp612; int(*
_temp614)( void*, void*, struct _tagged_string*); struct Cyc_Dict_Dict* _temp616;
int _temp618; int* _temp620; struct _tuple13* _temp607= env; _temp609=* _temp607;
_LL619: _temp618= _temp609.f1; _temp620=&(* _temp607).f1; goto _LL617; _LL617:
_temp616= _temp609.f2; goto _LL615; _LL615: _temp614= _temp609.f3; goto _LL613;
_LL613: _temp612= _temp609.f4; goto _LL611; _LL611: _temp610= _temp609.f5; goto
_LL608; _LL608: { struct _handler_cons _temp621; _push_handler(& _temp621);{ int
_temp623= 0; if( setjmp( _temp621.handler)){ _temp623= 1;} if( ! _temp623){{
void* _temp624=(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( _temp616, x); goto _LL625; _LL625: if( ! _temp614( y1,
_temp624, _temp610)){* _temp620= 0;}}; _pop_handler();} else{ void* _temp622=(
void*) _exn_thrown; void* _temp627= _temp622; _LL629: if( _temp627 == Cyc_Dict_Absent){
goto _LL630;} else{ goto _LL631;} _LL631: goto _LL632; _LL630: Cyc_Interface_check_err(
_temp610,({ struct _tagged_string _temp633= _temp612; struct _tagged_string
_temp634=* x; xprintf("%.*s %.*s is missing", _temp633.last_plus_one - _temp633.curr,
_temp633.curr, _temp634.last_plus_one - _temp634.curr, _temp634.curr);}));*
_temp620= 0; goto _LL628; _LL632:( void) _throw( _temp627); _LL628:;}}}} static
int Cyc_Interface_incl_dict( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, int(* incl_f)( void*, void*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple13 _temp636=({
struct _tuple13 _temp635; _temp635.f1= 1; _temp635.f2= dic2; _temp635.f3= incl_f;
_temp635.f4= t; _temp635.f5= msg; _temp635;}); goto _LL637; _LL637:(( void(*)(
void(* f)( struct _tuple13*, struct _tagged_string*, void*), struct _tuple13*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct _tuple13* env,
struct _tagged_string* x, void* y1)) Cyc_Interface_incl_dict_f,& _temp636, dic1);
return _temp636.f1;} static int Cyc_Interface_incl_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Structdecl*
_temp638= Cyc_Tcdecl_merge_structdecl( d0, d1, 0, msg); goto _LL639; _LL639: if(
_temp638 == 0){ return 0;} if(( struct Cyc_Absyn_Structdecl*) d0 != _temp638){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp640= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of struct %.*s discloses too much information",
_temp640.last_plus_one - _temp640.curr, _temp640.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_uniondecl( struct Cyc_Absyn_Uniondecl* d0, struct
Cyc_Absyn_Uniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Uniondecl*
_temp641= Cyc_Tcdecl_merge_uniondecl( d0, d1, 0, msg); goto _LL642; _LL642: if(
_temp641 == 0){ return 0;} if(( struct Cyc_Absyn_Uniondecl*) d0 != _temp641){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp643= Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( d1->name))->v); xprintf("declaration of union %.*s discloses too much information",
_temp643.last_plus_one - _temp643.curr, _temp643.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_tuniondecl( struct Cyc_Absyn_Tuniondecl* d0,
struct Cyc_Absyn_Tuniondecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Tuniondecl*
_temp644= Cyc_Tcdecl_merge_tuniondecl( d0, d1, 0, msg); goto _LL645; _LL645: if(
_temp644 == 0){ return 0;} if(( struct Cyc_Absyn_Tuniondecl*) d0 != _temp644){
Cyc_Interface_check_err( msg,({ struct _tagged_string _temp646= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of tunion %.*s discloses too much information",
_temp646.last_plus_one - _temp646.curr, _temp646.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_enumdecl( struct Cyc_Absyn_Enumdecl* d0, struct
Cyc_Absyn_Enumdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Enumdecl*
_temp647= Cyc_Tcdecl_merge_enumdecl( d0, d1, 0, msg); goto _LL648; _LL648: if(
_temp647 == 0){ return 0;} if(( struct Cyc_Absyn_Enumdecl*) d0 != _temp647){ Cyc_Interface_check_err(
msg,({ struct _tagged_string _temp649= Cyc_Absynpp_qvar2string( d1->name);
xprintf("declaration of enum %.*s discloses too much information", _temp649.last_plus_one
- _temp649.curr, _temp649.curr);})); return 0;} return 1;} static int Cyc_Interface_incl_vardecl(
struct Cyc_Absyn_Vardecl* d0, struct Cyc_Absyn_Vardecl* d1, struct
_tagged_string* msg){ struct Cyc_Absyn_Vardecl* _temp650= Cyc_Tcdecl_merge_vardecl(
d0, d1, 0, msg); goto _LL651; _LL651: if( _temp650 == 0){ return 0;} if(( struct
Cyc_Absyn_Vardecl*) d0 != _temp650){ Cyc_Interface_check_err( msg,({ struct
_tagged_string _temp652= Cyc_Absynpp_qvar2string( d1->name); xprintf("declaration of variable %.*s discloses too much information",
_temp652.last_plus_one - _temp652.curr, _temp652.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_typedefdecl( struct Cyc_Absyn_Typedefdecl* d0,
struct Cyc_Absyn_Typedefdecl* d1, struct _tagged_string* msg){ struct Cyc_Absyn_Typedefdecl*
_temp653= Cyc_Tcdecl_merge_typedefdecl( d0, d1, 0, msg); goto _LL654; _LL654:
if( _temp653 == 0){ return 0;} if(( struct Cyc_Absyn_Typedefdecl*) d0 !=
_temp653){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp655= Cyc_Absynpp_qvar2string(
d1->name); xprintf("declaration of typedef %.*s discloses too much information",
_temp655.last_plus_one - _temp655.curr, _temp655.curr);})); return 0;} return 1;}
static int Cyc_Interface_incl_xtunionfielddecl( struct Cyc_Tcdecl_Xtunionfielddecl*
d0, struct Cyc_Tcdecl_Xtunionfielddecl* d1, struct _tagged_string* msg){ struct
Cyc_Tcdecl_Xtunionfielddecl* _temp656= Cyc_Tcdecl_merge_xtunionfielddecl( d0, d1,
0, msg); goto _LL657; _LL657: if( _temp656 == 0){ return 0;} if(( struct Cyc_Tcdecl_Xtunionfielddecl*)
d0 != _temp656){ Cyc_Interface_check_err( msg,({ struct _tagged_string _temp658=
Cyc_Absynpp_qvar2string(( d1->field)->name); xprintf("declaration of xtunionfield %.*s discloses too much information",
_temp658.last_plus_one - _temp658.curr, _temp658.curr);})); return 0;} return 1;}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg); static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2, struct
_tagged_string* msg){ int _temp661=(( int(*)( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->structdecls, ie2->structdecls, Cyc_Interface_incl_structdecl,({
char* _temp659="struct"; struct _tagged_string _temp660; _temp660.curr= _temp659;
_temp660.base= _temp659; _temp660.last_plus_one= _temp659 + 7; _temp660;}), msg);
goto _LL662; _LL662: { int _temp665=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Uniondecl*, struct
Cyc_Absyn_Uniondecl*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->uniondecls, ie2->uniondecls,
Cyc_Interface_incl_uniondecl,({ char* _temp663="union"; struct _tagged_string
_temp664; _temp664.curr= _temp663; _temp664.base= _temp663; _temp664.last_plus_one=
_temp663 + 6; _temp664;}), msg); goto _LL666; _LL666: { int _temp669=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct _tagged_string*),
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)(
ie1->tuniondecls, ie2->tuniondecls, Cyc_Interface_incl_tuniondecl,({ char*
_temp667="tunion"; struct _tagged_string _temp668; _temp668.curr= _temp667;
_temp668.base= _temp667; _temp668.last_plus_one= _temp667 + 7; _temp668;}), msg);
goto _LL670; _LL670: { int _temp673=(( int(*)( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct Cyc_Absyn_Enumdecl*, struct
Cyc_Absyn_Enumdecl*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->enumdecls, ie2->enumdecls,
Cyc_Interface_incl_enumdecl,({ char* _temp671="enum"; struct _tagged_string
_temp672; _temp672.curr= _temp671; _temp672.base= _temp671; _temp672.last_plus_one=
_temp671 + 5; _temp672;}), msg); goto _LL674; _LL674: { int _temp677=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct
Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)( struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*, struct
_tagged_string*), struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls, ie2->typedefdecls,(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_strptrcmp),
Cyc_Tcdecl_merge_typedefdecl,({ char* _temp675="typedef"; struct _tagged_string
_temp676; _temp676.curr= _temp675; _temp676.base= _temp675; _temp676.last_plus_one=
_temp675 + 8; _temp676;}), msg) != 0; goto _LL678; _LL678: { int _temp681=(( int(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, int(* incl_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg)) Cyc_Interface_incl_dict)( ie1->vardecls,
ie2->vardecls, Cyc_Interface_incl_vardecl,({ char* _temp679="variable"; struct
_tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 9; _temp680;}), msg); goto _LL682; _LL682: {
int _temp685=(( int(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
int(* incl_f)( struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_incl_dict)( ie1->xtunionfielddecls, ie2->xtunionfielddecls, Cyc_Interface_incl_xtunionfielddecl,({
char* _temp683="xtunionfield"; struct _tagged_string _temp684; _temp684.curr=
_temp683; _temp684.base= _temp683; _temp684.last_plus_one= _temp683 + 13;
_temp684;}), msg); goto _LL686; _LL686: return((((( _temp661? _temp665: 0)?
_temp669: 0)? _temp673: 0)? _temp677: 0)? _temp681: 0)? _temp685: 0;}}}}}}}
struct _tuple14{ int f1; struct Cyc_Dict_Dict* f2; struct _tagged_string* f3; }
; static void Cyc_Interface_incl_ns_f( struct _tuple14* env, struct Cyc_List_List*
ns, struct Cyc_Interface_Ienv* ie1){ struct _tuple14 _temp689; struct
_tagged_string* _temp690; struct Cyc_Dict_Dict* _temp692; int _temp694; int*
_temp696; struct _tuple14* _temp687= env; _temp689=* _temp687; _LL695: _temp694=
_temp689.f1; _temp696=&(* _temp687).f1; goto _LL693; _LL693: _temp692= _temp689.f2;
goto _LL691; _LL691: _temp690= _temp689.f3; goto _LL688; _LL688: { struct Cyc_Interface_Ienv*
ie2;{ struct _handler_cons _temp697; _push_handler(& _temp697);{ int _temp699= 0;
if( setjmp( _temp697.handler)){ _temp699= 1;} if( ! _temp699){ ie2=(( struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp692,
ns);; _pop_handler();} else{ void* _temp698=( void*) _exn_thrown; void* _temp701=
_temp698; _LL703: if( _temp701 == Cyc_Dict_Absent){ goto _LL704;} else{ goto
_LL705;} _LL705: goto _LL706; _LL704: ie2= Cyc_Interface_lazy_new_ienv(); goto
_LL702; _LL706:( void) _throw( _temp701); _LL702:;}}} if( ! Cyc_Interface_incl_ienv(
ie1, ie2, _temp690)){* _temp696= 0;}}} static int Cyc_Interface_incl_ns( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string* msg){
struct _tuple14 _temp708=({ struct _tuple14 _temp707; _temp707.f1= 1; _temp707.f2=
dic2; _temp707.f3= msg; _temp707;}); goto _LL709; _LL709:(( void(*)( void(* f)(
struct _tuple14*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct
_tuple14* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_incl_ns_f,&
_temp708, dic1); return _temp708.f1;} int Cyc_Interface_is_subinterface( struct
Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){ struct
_handler_cons _temp710; _push_handler(& _temp710);{ int _temp712= 0; if( setjmp(
_temp710.handler)){ _temp712= 1;} if( ! _temp712){{ int _temp713= 1; goto _LL714;
_LL714: { struct _tagged_string* msg= 0; if( info != 0){ msg=({ struct
_tagged_string* _temp715=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp715[ 0]=({ struct _tagged_string _temp716=(*(( struct
_tuple3*) _check_null( info))).f1; struct _tagged_string _temp717=(*(( struct
_tuple3*) _check_null( info))).f2; xprintf("checking inclusion of %.*s exports into %.*s exports,",
_temp716.last_plus_one - _temp716.curr, _temp716.curr, _temp717.last_plus_one -
_temp717.curr, _temp717.curr);}); _temp715;});} if( ! Cyc_Interface_incl_ns( i1->exports,
i2->exports, msg)){ _temp713= 0;} if( info != 0){ msg=({ struct _tagged_string*
_temp718=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string));
_temp718[ 0]=({ struct _tagged_string _temp719=(*(( struct _tuple3*) _check_null(
info))).f2; struct _tagged_string _temp720=(*(( struct _tuple3*) _check_null(
info))).f1; xprintf("checking inclusion of %.*s imports into %.*s imports,",
_temp719.last_plus_one - _temp719.curr, _temp719.curr, _temp720.last_plus_one -
_temp720.curr, _temp720.curr);}); _temp718;});} if( ! Cyc_Interface_incl_ns( i2->imports,
i1->imports, msg)){ _temp713= 0;}{ int _temp721= _temp713; _npop_handler( 0u);
return _temp721;}}}; _pop_handler();} else{ void* _temp711=( void*) _exn_thrown;
void* _temp723= _temp711; _LL725: if( _temp723 == Cyc_Tcdecl_Incompatible){ goto
_LL726;} else{ goto _LL727;} _LL727: goto _LL728; _LL726: return 0; _LL728:(
void) _throw( _temp723); _LL724:;}}} struct _tuple15{ int f1; struct Cyc_Dict_Dict*
f2; struct Cyc_Dict_Dict* f3; struct Cyc_Dict_Dict* f4; void**(* f5)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*); struct
_tagged_string f6; struct _tagged_string* f7; } ; void Cyc_Interface_compat_merge_dict_f(
struct _tuple15* env, struct _tagged_string* x, void** y2){ struct _tuple15
_temp731; struct _tagged_string* _temp732; struct _tagged_string _temp734; void**(*
_temp736)( void**, void**, struct Cyc_Position_Segment*, struct _tagged_string*);
struct Cyc_Dict_Dict* _temp738; struct Cyc_Dict_Dict* _temp740; struct Cyc_Dict_Dict*
_temp742; struct Cyc_Dict_Dict** _temp744; int _temp745; int* _temp747; struct
_tuple15* _temp729= env; _temp731=* _temp729; _LL746: _temp745= _temp731.f1;
_temp747=&(* _temp729).f1; goto _LL743; _LL743: _temp742= _temp731.f2; _temp744=&(*
_temp729).f2; goto _LL741; _LL741: _temp740= _temp731.f3; goto _LL739; _LL739:
_temp738= _temp731.f4; goto _LL737; _LL737: _temp736= _temp731.f5; goto _LL735;
_LL735: _temp734= _temp731.f6; goto _LL733; _LL733: _temp732= _temp731.f7; goto
_LL730; _LL730: { void** y;{ struct _handler_cons _temp748; _push_handler(&
_temp748);{ int _temp750= 0; if( setjmp( _temp748.handler)){ _temp750= 1;} if( !
_temp750){{ void** _temp751=(( void**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp740, x); goto _LL752; _LL752: {
void** _temp753= _temp736( _temp751, y2, 0, _temp732); goto _LL754; _LL754: if(
!(( unsigned int) _temp753)){* _temp747= 0; _npop_handler( 0u); return;} y=(
void**) _check_null( _temp753);}}; _pop_handler();} else{ void* _temp749=( void*)
_exn_thrown; void* _temp756= _temp749; _LL758: if( _temp756 == Cyc_Dict_Absent){
goto _LL759;} else{ goto _LL760;} _LL760: goto _LL761; _LL759: y= y2; goto
_LL757; _LL761:( void) _throw( _temp756); _LL757:;}}}{ struct _handler_cons
_temp762; _push_handler(& _temp762);{ int _temp764= 0; if( setjmp( _temp762.handler)){
_temp764= 1;} if( ! _temp764){{ void** _temp765=(( void**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( _temp738, x); goto _LL766;
_LL766: { void** _temp767= _temp736( _temp765, y, 0, _temp732); goto _LL768;
_LL768: if( _temp767 !=( void**) _temp765){ if(( unsigned int) _temp767){ Cyc_Interface_check_err(
_temp732,({ struct _tagged_string _temp769= _temp734; struct _tagged_string
_temp770=* x; xprintf("abstract %.*s %.*s is being imported as non-abstract",
_temp769.last_plus_one - _temp769.curr, _temp769.curr, _temp770.last_plus_one -
_temp770.curr, _temp770.curr);}));}* _temp747= 0;}}}; _pop_handler();} else{
void* _temp763=( void*) _exn_thrown; void* _temp772= _temp763; _LL774: if(
_temp772 == Cyc_Dict_Absent){ goto _LL775;} else{ goto _LL776;} _LL776: goto
_LL777; _LL775: if(* _temp747){* _temp744=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void** data)) Cyc_Dict_insert)(*
_temp744, x, y);} goto _LL773; _LL777:( void) _throw( _temp772); _LL773:;}}}}}
struct Cyc_Core_Opt* Cyc_Interface_compat_merge_dict( struct Cyc_Dict_Dict* dic1,
struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict* excl, void**(* merge_f)( void**,
void**, struct Cyc_Position_Segment*, struct _tagged_string*), struct
_tagged_string t, struct _tagged_string* msg){ struct _tuple15 _temp779=({
struct _tuple15 _temp778; _temp778.f1= 1; _temp778.f2= dic1; _temp778.f3= dic1;
_temp778.f4= excl; _temp778.f5= merge_f; _temp778.f6= t; _temp778.f7= msg;
_temp778;}); goto _LL780; _LL780:(( void(*)( void(* f)( struct _tuple15*, struct
_tagged_string*, void**), struct _tuple15* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)((
void(*)( struct _tuple15* env, struct _tagged_string* x, void** y2)) Cyc_Interface_compat_merge_dict_f,&
_temp779, dic2); if( _temp779.f1){ return({ struct Cyc_Core_Opt* _temp781=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp781->v=(
void*) _temp779.f2; _temp781;});} else{ return 0;}} struct Cyc_Interface_Ienv*
Cyc_Interface_compat_merge_ienv( struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv*
ie2, struct Cyc_Interface_Ienv* iexcl, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp784=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Structdecl*(* merge_f)(
struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->structdecls, ie2->structdecls, iexcl->structdecls,
Cyc_Tcdecl_merge_structdecl,({ char* _temp782="struct"; struct _tagged_string
_temp783; _temp783.curr= _temp782; _temp783.base= _temp782; _temp783.last_plus_one=
_temp782 + 7; _temp783;}), msg); goto _LL785; _LL785: { struct Cyc_Core_Opt*
_temp788=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Uniondecl*(* merge_f)( struct
Cyc_Absyn_Uniondecl*, struct Cyc_Absyn_Uniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->uniondecls, ie2->uniondecls, iexcl->uniondecls,
Cyc_Tcdecl_merge_uniondecl,({ char* _temp786="union"; struct _tagged_string
_temp787; _temp787.curr= _temp786; _temp787.base= _temp786; _temp787.last_plus_one=
_temp786 + 6; _temp787;}), msg); goto _LL789; _LL789: { struct Cyc_Core_Opt*
_temp792=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Tuniondecl*(* merge_f)(
struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->tuniondecls, ie2->tuniondecls, iexcl->tuniondecls,
Cyc_Tcdecl_merge_tuniondecl,({ char* _temp790="tunion"; struct _tagged_string
_temp791; _temp791.curr= _temp790; _temp791.base= _temp790; _temp791.last_plus_one=
_temp790 + 7; _temp791;}), msg); goto _LL793; _LL793: { struct Cyc_Core_Opt*
_temp796=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Enumdecl*(* merge_f)( struct
Cyc_Absyn_Enumdecl*, struct Cyc_Absyn_Enumdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->enumdecls, ie2->enumdecls, iexcl->enumdecls,
Cyc_Tcdecl_merge_enumdecl,({ char* _temp794="enum"; struct _tagged_string
_temp795; _temp795.curr= _temp794; _temp795.base= _temp794; _temp795.last_plus_one=
_temp794 + 5; _temp795;}), msg); goto _LL797; _LL797: { struct Cyc_Core_Opt*
_temp800=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls, iexcl->typedefdecls,
Cyc_Tcdecl_merge_typedefdecl,({ char* _temp798="typedef"; struct _tagged_string
_temp799; _temp799.curr= _temp798; _temp799.base= _temp798; _temp799.last_plus_one=
_temp798 + 8; _temp799;}), msg); goto _LL801; _LL801: { struct Cyc_Core_Opt*
_temp804=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Vardecl*(* merge_f)( struct
Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->vardecls, ie2->vardecls, iexcl->vardecls,
Cyc_Tcdecl_merge_vardecl,({ char* _temp802="variable"; struct _tagged_string
_temp803; _temp803.curr= _temp802; _temp803.base= _temp802; _temp803.last_plus_one=
_temp802 + 9; _temp803;}), msg); goto _LL805; _LL805: { struct Cyc_Core_Opt*
_temp808=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Tcdecl_Xtunionfielddecl*(* merge_f)(
struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct
Cyc_Position_Segment*, struct _tagged_string*), struct _tagged_string t, struct
_tagged_string* msg)) Cyc_Interface_compat_merge_dict)( ie1->xtunionfielddecls,
ie2->xtunionfielddecls, iexcl->xtunionfielddecls, Cyc_Tcdecl_merge_xtunionfielddecl,({
char* _temp806="xtunionfield"; struct _tagged_string _temp807; _temp807.curr=
_temp806; _temp807.base= _temp806; _temp807.last_plus_one= _temp806 + 13;
_temp807;}), msg); goto _LL809; _LL809: if(((((( !(( unsigned int) _temp784)? 1:
!(( unsigned int) _temp788))? 1: !(( unsigned int) _temp792))? 1: !((
unsigned int) _temp796))? 1: !(( unsigned int) _temp800))? 1: !(( unsigned int)
_temp804))? 1: !(( unsigned int) _temp808)){ return 0;} else{ return({ struct
Cyc_Interface_Ienv* _temp810=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof(
struct Cyc_Interface_Ienv)); _temp810->structdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp784))->v; _temp810->uniondecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp788))->v; _temp810->tuniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp792))->v;
_temp810->enumdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp796))->v; _temp810->typedefdecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp800))->v; _temp810->vardecls=( struct Cyc_Dict_Dict*)(( struct
Cyc_Core_Opt*) _check_null( _temp804))->v; _temp810->xtunionfielddecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp808))->v; _temp810;});}}}}}}}}
struct _tuple16{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct Cyc_Dict_Dict* f4; struct _tagged_string* f5; } ; void Cyc_Interface_compat_merge_ns_f(
struct _tuple16* env, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie2){
struct _tuple16 _temp813; struct _tagged_string* _temp814; struct Cyc_Dict_Dict*
_temp816; struct Cyc_Dict_Dict* _temp818; struct Cyc_Dict_Dict* _temp820; struct
Cyc_Dict_Dict** _temp822; int _temp823; int* _temp825; struct _tuple16* _temp811=
env; _temp813=* _temp811; _LL824: _temp823= _temp813.f1; _temp825=&(* _temp811).f1;
goto _LL821; _LL821: _temp820= _temp813.f2; _temp822=&(* _temp811).f2; goto
_LL819; _LL819: _temp818= _temp813.f3; goto _LL817; _LL817: _temp816= _temp813.f4;
goto _LL815; _LL815: _temp814= _temp813.f5; goto _LL812; _LL812: { struct Cyc_Interface_Ienv*
ie1; struct Cyc_Interface_Ienv* iexcl;{ struct _handler_cons _temp826;
_push_handler(& _temp826);{ int _temp828= 0; if( setjmp( _temp826.handler)){
_temp828= 1;} if( ! _temp828){ iexcl=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp816, ns);;
_pop_handler();} else{ void* _temp827=( void*) _exn_thrown; void* _temp830=
_temp827; _LL832: if( _temp830 == Cyc_Dict_Absent){ goto _LL833;} else{ goto
_LL834;} _LL834: goto _LL835; _LL833: iexcl= Cyc_Interface_lazy_new_ienv(); goto
_LL831; _LL835:( void) _throw( _temp830); _LL831:;}}}{ struct _handler_cons
_temp836; _push_handler(& _temp836);{ int _temp838= 0; if( setjmp( _temp836.handler)){
_temp838= 1;} if( ! _temp838){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp818, ns);{ struct Cyc_Interface_Ienv*
_temp839= Cyc_Interface_compat_merge_ienv( Cyc_Interface_lazy_new_ienv(), ie1,
iexcl, _temp814); goto _LL840; _LL840: if( _temp839 == 0){* _temp825= 0;
_npop_handler( 0u); return;} ie1=( struct Cyc_Interface_Ienv*) _check_null(
_temp839);}; _pop_handler();} else{ void* _temp837=( void*) _exn_thrown; void*
_temp842= _temp837; _LL844: if( _temp842 == Cyc_Dict_Absent){ goto _LL845;}
else{ goto _LL846;} _LL846: goto _LL847; _LL845: ie1= Cyc_Interface_lazy_new_ienv();
goto _LL843; _LL847:( void) _throw( _temp842); _LL843:;}}}{ struct Cyc_Interface_Ienv*
_temp848= Cyc_Interface_compat_merge_ienv( ie1, ie2, iexcl, _temp814); goto
_LL849; _LL849: if( _temp848 == 0){* _temp825= 0;} else{ if(* _temp825){*
_temp822=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(* _temp822, ns,( struct
Cyc_Interface_Ienv*) _check_null( _temp848));}}}}} struct Cyc_Core_Opt* Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct Cyc_Dict_Dict*
excl, struct _tagged_string* msg){ struct _tuple16 _temp851=({ struct _tuple16
_temp850; _temp850.f1= 1; _temp850.f2=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
struct Cyc_List_List*, struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp);
_temp850.f3= dic1; _temp850.f4= excl; _temp850.f5= msg; _temp850;}); goto _LL852;
_LL852:(( void(*)( void(* f)( struct _tuple16*, struct Cyc_List_List*, struct
Cyc_Interface_Ienv*), struct _tuple16* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_ns_f,& _temp851, dic2); if( _temp851.f1){ return({
struct Cyc_Core_Opt* _temp853=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp853->v=( void*) _temp851.f2; _temp853;});} else{ return 0;}}
struct _tuple17{ int f1; struct Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3;
struct _tagged_string f4; struct _tagged_string* f5; } ; void Cyc_Interface_disj_merge_dict_f(
struct _tuple17* env, struct _tagged_string* x, void* y){ struct _tuple17
_temp856; struct _tagged_string* _temp857; struct _tagged_string _temp859;
struct Cyc_Dict_Dict* _temp861; struct Cyc_Dict_Dict* _temp863; struct Cyc_Dict_Dict**
_temp865; int _temp866; int* _temp868; struct _tuple17* _temp854= env; _temp856=*
_temp854; _LL867: _temp866= _temp856.f1; _temp868=&(* _temp854).f1; goto _LL864;
_LL864: _temp863= _temp856.f2; _temp865=&(* _temp854).f2; goto _LL862; _LL862:
_temp861= _temp856.f3; goto _LL860; _LL860: _temp859= _temp856.f4; goto _LL858;
_LL858: _temp857= _temp856.f5; goto _LL855; _LL855: if((( int(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_member)( _temp861, x)){ Cyc_Interface_check_err(
_temp857,({ struct _tagged_string _temp869= _temp859; struct _tagged_string
_temp870=* x; xprintf("%.*s %.*s is exported more than once", _temp869.last_plus_one
- _temp869.curr, _temp869.curr, _temp870.last_plus_one - _temp870.curr, _temp870.curr);}));*
_temp868= 0;} else{ if(* _temp868){* _temp865=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(*
_temp865, x, y);}}} struct Cyc_Core_Opt* Cyc_Interface_disj_merge_dict( struct
Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct
_tagged_string* msg){ struct _tuple17 _temp872=({ struct _tuple17 _temp871;
_temp871.f1= 1; _temp871.f2= dic1; _temp871.f3= dic1; _temp871.f4= t; _temp871.f5=
msg; _temp871;}); goto _LL873; _LL873:(( void(*)( void(* f)( struct _tuple17*,
struct _tagged_string*, void*), struct _tuple17* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)(( void(*)( struct _tuple17* env, struct _tagged_string* x, void*
y)) Cyc_Interface_disj_merge_dict_f,& _temp872, dic2); if( _temp872.f1){ return({
struct Cyc_Core_Opt* _temp874=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp874->v=( void*) _temp872.f2; _temp874;});} else{ return 0;}}
struct Cyc_Interface_Ienv* Cyc_Interface_disj_merge_ienv( struct Cyc_Interface_Ienv*
ie1, struct Cyc_Interface_Ienv* ie2, struct _tagged_string* msg){ struct Cyc_Core_Opt*
_temp877=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->structdecls, ie2->structdecls,({ char* _temp875="struct"; struct
_tagged_string _temp876; _temp876.curr= _temp875; _temp876.base= _temp875;
_temp876.last_plus_one= _temp875 + 7; _temp876;}), msg); goto _LL878; _LL878: {
struct Cyc_Core_Opt* _temp881=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t, struct _tagged_string*
msg)) Cyc_Interface_disj_merge_dict)( ie1->uniondecls, ie2->uniondecls,({ char*
_temp879="union"; struct _tagged_string _temp880; _temp880.curr= _temp879;
_temp880.base= _temp879; _temp880.last_plus_one= _temp879 + 6; _temp880;}), msg);
goto _LL882; _LL882: { struct Cyc_Core_Opt* _temp885=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->tuniondecls,
ie2->tuniondecls,({ char* _temp883="[x]tunion"; struct _tagged_string _temp884;
_temp884.curr= _temp883; _temp884.base= _temp883; _temp884.last_plus_one=
_temp883 + 10; _temp884;}), msg); goto _LL886; _LL886: { struct Cyc_Core_Opt*
_temp889=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->enumdecls, ie2->enumdecls,({ char* _temp887="enum"; struct _tagged_string
_temp888; _temp888.curr= _temp887; _temp888.base= _temp887; _temp888.last_plus_one=
_temp887 + 5; _temp888;}), msg); goto _LL890; _LL890: { struct Cyc_Core_Opt*
_temp893=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict*
dic2, struct Cyc_Dict_Dict* excl, struct Cyc_Absyn_Typedefdecl*(* merge_f)(
struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*, struct Cyc_Position_Segment*,
struct _tagged_string*), struct _tagged_string t, struct _tagged_string* msg))
Cyc_Interface_compat_merge_dict)( ie1->typedefdecls, ie2->typedefdecls,(( struct
Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Dict_empty)( Cyc_String_zstrptrcmp), Cyc_Tcdecl_merge_typedefdecl,({ char*
_temp891="typedef"; struct _tagged_string _temp892; _temp892.curr= _temp891;
_temp892.base= _temp891; _temp892.last_plus_one= _temp891 + 8; _temp892;}), msg);
goto _LL894; _LL894: { struct Cyc_Core_Opt* _temp897=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2, struct _tagged_string t,
struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)( ie1->vardecls, ie2->vardecls,({
char* _temp895="variable"; struct _tagged_string _temp896; _temp896.curr=
_temp895; _temp896.base= _temp895; _temp896.last_plus_one= _temp895 + 9;
_temp896;}), msg); goto _LL898; _LL898: { struct Cyc_Core_Opt* _temp901=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* dic1, struct Cyc_Dict_Dict* dic2,
struct _tagged_string t, struct _tagged_string* msg)) Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls, ie2->xtunionfielddecls,({ char* _temp899="xtunionfield";
struct _tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 13; _temp900;}), msg); goto _LL902; _LL902:
if(((((( !(( unsigned int) _temp877)? 1: !(( unsigned int) _temp881))? 1: !((
unsigned int) _temp885))? 1: !(( unsigned int) _temp889))? 1: !(( unsigned int)
_temp893))? 1: !(( unsigned int) _temp897))? 1: !(( unsigned int) _temp901)){
return 0;} else{ return({ struct Cyc_Interface_Ienv* _temp903=( struct Cyc_Interface_Ienv*)
GC_malloc( sizeof( struct Cyc_Interface_Ienv)); _temp903->structdecls=( struct
Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp877))->v; _temp903->uniondecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp881))->v;
_temp903->tuniondecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp885))->v; _temp903->enumdecls=( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp889))->v; _temp903->typedefdecls=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp893))->v;
_temp903->vardecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp897))->v; _temp903->xtunionfielddecls=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*)
_check_null( _temp901))->v; _temp903;});}}}}}}}} struct _tuple18{ int f1; struct
Cyc_Dict_Dict* f2; struct Cyc_Dict_Dict* f3; struct _tagged_string* f4; } ; void
Cyc_Interface_disj_merge_ns_f( struct _tuple18* env, struct Cyc_List_List* ns,
struct Cyc_Interface_Ienv* ie2){ struct _tuple18 _temp906; struct _tagged_string*
_temp907; struct Cyc_Dict_Dict* _temp909; struct Cyc_Dict_Dict* _temp911; struct
Cyc_Dict_Dict** _temp913; int _temp914; int* _temp916; struct _tuple18* _temp904=
env; _temp906=* _temp904; _LL915: _temp914= _temp906.f1; _temp916=&(* _temp904).f1;
goto _LL912; _LL912: _temp911= _temp906.f2; _temp913=&(* _temp904).f2; goto
_LL910; _LL910: _temp909= _temp906.f3; goto _LL908; _LL908: _temp907= _temp906.f4;
goto _LL905; _LL905: { struct Cyc_Interface_Ienv* ie1;{ struct _handler_cons
_temp917; _push_handler(& _temp917);{ int _temp919= 0; if( setjmp( _temp917.handler)){
_temp919= 1;} if( ! _temp919){ ie1=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( _temp909, ns);; _pop_handler();}
else{ void* _temp918=( void*) _exn_thrown; void* _temp921= _temp918; _LL923: if(
_temp921 == Cyc_Dict_Absent){ goto _LL924;} else{ goto _LL925;} _LL925: goto
_LL926; _LL924: ie1= Cyc_Interface_lazy_new_ienv(); goto _LL922; _LL926:( void)
_throw( _temp921); _LL922:;}}}{ struct Cyc_Interface_Ienv* _temp927= Cyc_Interface_disj_merge_ienv(
ie1, ie2, _temp907); goto _LL928; _LL928: if( _temp927 == 0){* _temp916= 0;}
else{ if(* _temp916){* _temp913=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Interface_Ienv* data)) Cyc_Dict_insert)(*
_temp913, ns,( struct Cyc_Interface_Ienv*) _check_null( _temp927));}}}}} struct
Cyc_Core_Opt* Cyc_Interface_disj_merge_ns( struct Cyc_Dict_Dict* dic1, struct
Cyc_Dict_Dict* dic2, struct _tagged_string* msg){ struct _tuple18 _temp930=({
struct _tuple18 _temp929; _temp929.f1= 1; _temp929.f2= dic1; _temp929.f3= dic1;
_temp929.f4= msg; _temp929;}); goto _LL931; _LL931:(( void(*)( void(* f)( struct
_tuple18*, struct Cyc_List_List*, struct Cyc_Interface_Ienv*), struct _tuple18*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_disj_merge_ns_f,&
_temp930, dic2); if( _temp930.f1){ return({ struct Cyc_Core_Opt* _temp932=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp932->v=(
void*) _temp930.f2; _temp932;});} else{ return 0;}} struct Cyc_Interface_I* Cyc_Interface_merge(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple3* info){
struct _handler_cons _temp933; _push_handler(& _temp933);{ int _temp935= 0; if(
setjmp( _temp933.handler)){ _temp935= 1;} if( ! _temp935){{ struct
_tagged_string* msg= 0; if( info != 0){ msg=({ struct _tagged_string* _temp936=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp936[ 0]=({
struct _tagged_string _temp937=(*(( struct _tuple3*) _check_null( info))).f1;
struct _tagged_string _temp938=(*(( struct _tuple3*) _check_null( info))).f2;
xprintf("merging exports of %.*s and %.*s,", _temp937.last_plus_one - _temp937.curr,
_temp937.curr, _temp938.last_plus_one - _temp938.curr, _temp938.curr);});
_temp936;});}{ struct Cyc_Core_Opt* _temp939= Cyc_Interface_disj_merge_ns( i1->exports,
i2->exports, msg); goto _LL940; _LL940: if( _temp939 == 0){ struct Cyc_Interface_I*
_temp941= 0; _npop_handler( 0u); return _temp941;} if( info != 0){ msg=({ struct
_tagged_string* _temp942=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp942[ 0]=({ struct _tagged_string _temp943=(*(( struct
_tuple3*) _check_null( info))).f1; struct _tagged_string _temp944=(*(( struct
_tuple3*) _check_null( info))).f2; xprintf("merging imports of %.*s and %.*s,",
_temp943.last_plus_one - _temp943.curr, _temp943.curr, _temp944.last_plus_one -
_temp944.curr, _temp944.curr);}); _temp942;});}{ struct Cyc_Core_Opt* _temp945=
Cyc_Interface_compat_merge_ns( i1->imports, i2->imports,( struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*) _check_null( _temp939))->v, msg); goto _LL946; _LL946: if(
_temp945 == 0){ struct Cyc_Interface_I* _temp947= 0; _npop_handler( 0u); return
_temp947;}{ struct Cyc_Interface_I* _temp949=({ struct Cyc_Interface_I* _temp948=(
struct Cyc_Interface_I*) GC_malloc( sizeof( struct Cyc_Interface_I)); _temp948->imports=(
struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null( _temp945))->v;
_temp948->exports=( struct Cyc_Dict_Dict*)(( struct Cyc_Core_Opt*) _check_null(
_temp939))->v; _temp948;}); _npop_handler( 0u); return _temp949;}}}};
_pop_handler();} else{ void* _temp934=( void*) _exn_thrown; void* _temp951=
_temp934; _LL953: if( _temp951 == Cyc_Tcdecl_Incompatible){ goto _LL954;} else{
goto _LL955;} _LL955: goto _LL956; _LL954: return 0; _LL956:( void) _throw(
_temp951); _LL952:;}}} struct Cyc_Interface_I* Cyc_Interface_merge_list( struct
Cyc_List_List* li, struct Cyc_List_List* linfo){ if( li == 0){ return( struct
Cyc_Interface_I*) Cyc_Interface_empty();}{ struct Cyc_Interface_I* curr_i=(
struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd; struct
_tagged_string* curr_info= linfo != 0?( struct _tagged_string*)(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( linfo))->hd): 0; li=((
struct Cyc_List_List*) _check_null( li))->tl; if( linfo != 0){ linfo=(( struct
Cyc_List_List*) _check_null( linfo))->tl;} for( 0; li != 0; li=(( struct Cyc_List_List*)
_check_null( li))->tl){ struct Cyc_Interface_I* _temp958= Cyc_Interface_merge(
curr_i,( struct Cyc_Interface_I*)(( struct Cyc_List_List*) _check_null( li))->hd,(
curr_info != 0? linfo != 0: 0)?({ struct _tuple3* _temp957=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp957->f1=*(( struct _tagged_string*)
_check_null( curr_info)); _temp957->f2=*(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( linfo))->hd); _temp957;}): 0); goto _LL959; _LL959: if( _temp958 ==
0){ return 0;} curr_i=( struct Cyc_Interface_I*) _check_null( _temp958); if(
curr_info != 0){ curr_info= linfo != 0?({ struct _tagged_string* _temp960=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp960[ 0]=({
struct _tagged_string _temp961=*(( struct _tagged_string*) _check_null(
curr_info)); struct _tagged_string _temp962=*(( struct _tagged_string*)(( struct
Cyc_List_List*) _check_null( linfo))->hd); xprintf("%.*s+%.*s", _temp961.last_plus_one
- _temp961.curr, _temp961.curr, _temp962.last_plus_one - _temp962.curr, _temp962.curr);});
_temp960;}): 0;} if( linfo != 0){ linfo=(( struct Cyc_List_List*) _check_null(
linfo))->tl;}} return( struct Cyc_Interface_I*) curr_i;}} static struct Cyc_List_List*
Cyc_Interface_add_namespace( struct Cyc_List_List* tds, struct Cyc_List_List* ns){
if( ns == 0){ return tds;} return({ struct Cyc_List_List* _temp963=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp963->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp964=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp964[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp965; _temp965.tag= Cyc_Absyn_Namespace_d;
_temp965.f1=( struct _tagged_string*)(( struct Cyc_List_List*) _check_null( ns))->hd;
_temp965.f2= Cyc_Interface_add_namespace( tds,(( struct Cyc_List_List*)
_check_null( ns))->tl); _temp965;}); _temp964;}), 0); _temp963->tl= 0; _temp963;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl( struct _tagged_string*
x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp966=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp966->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp967=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp967[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp968; _temp968.tag= Cyc_Absyn_Struct_d;
_temp968.f1= d; _temp968;}); _temp967;}), 0); _temp966->tl= tds; _temp966;});}
static struct Cyc_List_List* Cyc_Interface_add_structdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Structdecl* _temp969=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp969[ 0]=* d; _temp969;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp970=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp970->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp971=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp971[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp972; _temp972.tag= Cyc_Absyn_Struct_d;
_temp972.f1= d; _temp972;}); _temp971;}), 0); _temp970->tl= tds; _temp970;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp973=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp973->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp974=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp974[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp975; _temp975.tag= Cyc_Absyn_Union_d;
_temp975.f1= d; _temp975;}); _temp974;}), 0); _temp973->tl= tds; _temp973;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp976=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp976[ 0]=* d; _temp976;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp977=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp977->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp978=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp978[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp979; _temp979.tag= Cyc_Absyn_Union_d;
_temp979.f1= d; _temp979;}); _temp978;}), 0); _temp977->tl= tds; _temp977;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp980=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp980->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp981=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp981[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp982; _temp982.tag= Cyc_Absyn_Tunion_d;
_temp982.f1= d; _temp982;}); _temp981;}), 0); _temp980->tl= tds; _temp980;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp983=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp983[ 0]=* d; _temp983;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp984=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp984->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp985=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp985[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp986; _temp986.tag= Cyc_Absyn_Tunion_d;
_temp986.f1= d; _temp986;}); _temp985;}), 0); _temp984->tl= tds; _temp984;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp987=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp987->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp988=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp988[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp989; _temp989.tag= Cyc_Absyn_Enum_d;
_temp989.f1= d; _temp989;}); _temp988;}), 0); _temp987->tl= tds; _temp987;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp990=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp990[ 0]=* d; _temp990;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp991=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp991->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp992=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp992[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp993; _temp993.tag= Cyc_Absyn_Enum_d;
_temp993.f1= d; _temp993;}); _temp992;}), 0); _temp991->tl= tds; _temp991;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp994=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp994->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp995=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp995[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp996; _temp996.tag= Cyc_Absyn_Typedef_d;
_temp996.f1= d; _temp996;}); _temp995;}), 0); _temp994->tl= tds; _temp994;});}
static struct Cyc_List_List* Cyc_Interface_add_vardecl( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp997->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp998=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp998[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp999; _temp999.tag= Cyc_Absyn_Var_d;
_temp999.f1= d; _temp999;}); _temp998;}), 0); _temp997->tl= tds; _temp997;});}
static struct Cyc_List_List* Cyc_Interface_add_xtunionfielddecl( struct
_tagged_string* x, struct Cyc_Tcdecl_Xtunionfielddecl* d, struct Cyc_List_List*
tds){ struct Cyc_Tcdecl_Xtunionfielddecl _temp1002; struct Cyc_Absyn_Tunionfield*
_temp1003; struct Cyc_Absyn_Tuniondecl* _temp1005; struct Cyc_Tcdecl_Xtunionfielddecl*
_temp1000= d; _temp1002=* _temp1000; _LL1006: _temp1005= _temp1002.base; goto
_LL1004; _LL1004: _temp1003= _temp1002.field; goto _LL1001; _LL1001: _temp1005=({
struct Cyc_Absyn_Tuniondecl* _temp1007=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp1007[ 0]=* _temp1005; _temp1007;});
_temp1005->fields=({ struct Cyc_Core_Opt* _temp1008=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1008->v=( void*)({ struct Cyc_List_List*
_temp1009=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1009->hd=( void*) _temp1003; _temp1009->tl= 0; _temp1009;}); _temp1008;});(
void*)( _temp1005->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp1010=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1010->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp1011=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp1011[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp1012; _temp1012.tag= Cyc_Absyn_Tunion_d;
_temp1012.f1= _temp1005; _temp1012;}); _temp1011;}), 0); _temp1010->tl= tds;
_temp1010;});} static void Cyc_Interface_print_ns_headers( struct Cyc_Stdio___sFILE*
f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv* ie){ struct Cyc_List_List*
_temp1013= 0; goto _LL1014; _LL1014: _temp1013=(( struct Cyc_List_List*(*)(
struct Cyc_List_List*(* f)( struct _tagged_string*, struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*), struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum))
Cyc_Dict_fold)( Cyc_Interface_add_structdecl_header, ie->structdecls, _temp1013);
_temp1013=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct
_tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict*
d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl_header,
ie->uniondecls, _temp1013); _temp1013=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls, _temp1013); _temp1013=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl_header,
ie->enumdecls, _temp1013); if( _temp1013 != 0){ _temp1013=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1013); _temp1013= Cyc_Interface_add_namespace(
_temp1013, ns); Cyc_Absynpp_decllist2file( _temp1013, f);}} static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1015= 0; goto _LL1016; _LL1016: _temp1015=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Typedefdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_typedef, ie->typedefdecls,
_temp1015); if( _temp1015 != 0){ _temp1015=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1015); _temp1015= Cyc_Interface_add_namespace(
_temp1015, ns); Cyc_Absynpp_decllist2file( _temp1015, f);}} static void Cyc_Interface_print_ns_decls(
struct Cyc_Stdio___sFILE* f, struct Cyc_List_List* ns, struct Cyc_Interface_Ienv*
ie){ struct Cyc_List_List* _temp1017= 0; goto _LL1018; _LL1018: _temp1017=((
struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)( struct _tagged_string*,
struct Cyc_Absyn_Structdecl*, struct Cyc_List_List*), struct Cyc_Dict_Dict* d,
struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_structdecl, ie->structdecls,
_temp1017); _temp1017=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(* f)(
struct _tagged_string*, struct Cyc_Absyn_Uniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_uniondecl,
ie->uniondecls, _temp1017); _temp1017=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Tuniondecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_tuniondecl,
ie->tuniondecls, _temp1017); _temp1017=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Enumdecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_enumdecl,
ie->enumdecls, _temp1017); _temp1017=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Absyn_Vardecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_vardecl,
ie->vardecls, _temp1017); _temp1017=(( struct Cyc_List_List*(*)( struct Cyc_List_List*(*
f)( struct _tagged_string*, struct Cyc_Tcdecl_Xtunionfielddecl*, struct Cyc_List_List*),
struct Cyc_Dict_Dict* d, struct Cyc_List_List* accum)) Cyc_Dict_fold)( Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls, _temp1017); if( _temp1017 != 0){ _temp1017=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1017); _temp1017= Cyc_Interface_add_namespace(
_temp1017, ns); Cyc_Absynpp_decllist2file( _temp1017, f);}} void Cyc_Interface_print(
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
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp1019= Cyc_Parse_parse_file(
f); goto _LL1020; _LL1020: Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv*
_temp1021= Cyc_Tcenv_tc_init(); goto _LL1022; _LL1022: Cyc_Tc_tc( _temp1021, 0,
0, _temp1019); return Cyc_Interface_gen_extract( _temp1021->ae, 0);}}}