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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u]; extern int Cyc_Character_isdigit(
char c); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern unsigned int Cyc_String_strlen( struct
_tagged_string s); extern struct _tagged_string Cyc_String_implode( struct Cyc_List_List*
c); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[
12u]; extern char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
_tagged_string); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_string s, int i); struct _tuple3{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; char f5; int f6; } ;
struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_string s, int i){ unsigned int _temp0= Cyc_String_strlen( s); goto _LL1;
_LL1: if( i < 0? 1: i >= _temp0){ return 0;}{ struct Cyc_List_List* _temp2= 0;
goto _LL3; _LL3: { char c=' '; for( 0; i < _temp0; i ++){ c=({ struct
_tagged_string _temp4= s; char* _temp6= _temp4.curr + i; if( _temp4.base == 0? 1:(
_temp6 < _temp4.base? 1: _temp6 >= _temp4.last_plus_one)){ _throw(
Null_Exception);}* _temp6;}); switch( c){ case '+': _LL7: goto _LL8; case '-':
_LL8: goto _LL9; case ' ': _LL9: goto _LL10; case '#': _LL10: goto _LL11; case '0':
_LL11: _temp2=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( int) c);
_temp13->tl= _temp2; _temp13;}); continue; default: _LL12: break;} break;} if( i
>= _temp0){ return 0;} _temp2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp2);{ struct Cyc_List_List* _temp15= 0; goto _LL16;
_LL16: c=({ struct _tagged_string _temp17= s; char* _temp19= _temp17.curr + i;
if( _temp17.base == 0? 1:( _temp19 < _temp17.base? 1: _temp19 >= _temp17.last_plus_one)){
_throw( Null_Exception);}* _temp19;}); if( c =='*'){ _temp15=({ struct Cyc_List_List*
_temp20=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp20->hd=( void*)(( int) c); _temp20->tl= _temp15; _temp20;}); i ++;} else{
for( 0; i < _temp0; i ++){ c=({ struct _tagged_string _temp21= s; char* _temp23=
_temp21.curr + i; if( _temp21.base == 0? 1:( _temp23 < _temp21.base? 1: _temp23
>= _temp21.last_plus_one)){ _throw( Null_Exception);}* _temp23;}); if( Cyc_Character_isdigit(
c)){ _temp15=({ struct Cyc_List_List* _temp24=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp24->hd=( void*)(( int) c); _temp24->tl=
_temp15; _temp24;});} else{ break;}}} if( i >= _temp0){ return 0;} _temp15=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp15);{
struct Cyc_List_List* _temp25= 0; goto _LL26; _LL26: c=({ struct _tagged_string
_temp27= s; char* _temp29= _temp27.curr + i; if( _temp27.base == 0? 1:( _temp29
< _temp27.base? 1: _temp29 >= _temp27.last_plus_one)){ _throw( Null_Exception);}*
_temp29;}); if( c =='.'){ _temp25=({ struct Cyc_List_List* _temp30=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp30->hd=( void*)(( int) c);
_temp30->tl= _temp25; _temp30;}); i ++; if( i >= _temp0){ return 0;} c=({ struct
_tagged_string _temp31= s; char* _temp33= _temp31.curr + i; if( _temp31.base ==
0? 1:( _temp33 < _temp31.base? 1: _temp33 >= _temp31.last_plus_one)){ _throw(
Null_Exception);}* _temp33;}); if( c =='*'){ _temp25=({ struct Cyc_List_List*
_temp34=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp34->hd=( void*)(( int) c); _temp34->tl= _temp25; _temp34;}); i ++;} else{
for( 0; i < _temp0; i ++){ c=({ struct _tagged_string _temp35= s; char* _temp37=
_temp35.curr + i; if( _temp35.base == 0? 1:( _temp37 < _temp35.base? 1: _temp37
>= _temp35.last_plus_one)){ _throw( Null_Exception);}* _temp37;}); if( Cyc_Character_isdigit(
c)){ _temp25=({ struct Cyc_List_List* _temp38=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp38->hd=( void*)(( int) c); _temp38->tl=
_temp25; _temp38;});} else{ break;}}}} if( i >= _temp0){ return 0;} _temp25=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp25);{
struct Cyc_List_List* _temp39= 0; goto _LL40; _LL40: c=({ struct _tagged_string
_temp41= s; char* _temp43= _temp41.curr + i; if( _temp41.base == 0? 1:( _temp43
< _temp41.base? 1: _temp43 >= _temp41.last_plus_one)){ _throw( Null_Exception);}*
_temp43;}); switch( c){ case 'h': _LL44: _temp39=({ struct Cyc_List_List*
_temp46=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp46->hd=( void*)(( int) c); _temp46->tl= _temp39; _temp46;}); i ++; if( i >=
_temp0){ return 0;} c=({ struct _tagged_string _temp47= s; char* _temp49=
_temp47.curr + i; if( _temp47.base == 0? 1:( _temp49 < _temp47.base? 1: _temp49
>= _temp47.last_plus_one)){ _throw( Null_Exception);}* _temp49;}); if( c =='h'){
_temp39=({ struct Cyc_List_List* _temp50=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp50->hd=( void*)(( int) c); _temp50->tl=
_temp39; _temp50;}); i ++;} break; case 'l': _LL45: _temp39=({ struct Cyc_List_List*
_temp52=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp52->hd=( void*)(( int) c); _temp52->tl= _temp39; _temp52;}); i ++; if( i >=
_temp0){ return 0;} c=({ struct _tagged_string _temp53= s; char* _temp55=
_temp53.curr + i; if( _temp53.base == 0? 1:( _temp55 < _temp53.base? 1: _temp55
>= _temp53.last_plus_one)){ _throw( Null_Exception);}* _temp55;}); if( c =='l'){
_temp39=({ struct Cyc_List_List* _temp56=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp56->hd=( void*)(( int) c); _temp56->tl=
_temp39; _temp56;}); i ++;} break; case 'j': _LL51: goto _LL57; case 'z': _LL57:
goto _LL58; case 't': _LL58: goto _LL59; case 'L': _LL59: _temp39=({ struct Cyc_List_List*
_temp61=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp61->hd=( void*)(( int) c); _temp61->tl= _temp39; _temp61;}); i ++; break;
default: _LL60: break;} if( i >= _temp0){ return 0;} _temp39=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp39); c=({ struct
_tagged_string _temp63= s; char* _temp65= _temp63.curr + i; if( _temp63.base ==
0? 1:( _temp65 < _temp63.base? 1: _temp65 >= _temp63.last_plus_one)){ _throw(
Null_Exception);}* _temp65;}); switch( c){ case 'd': _LL66: break; case 'i':
_LL67: break; case 'o': _LL68: break; case 'u': _LL69: break; case 'x': _LL70:
break; case 'X': _LL71: break; case 'f': _LL72: break; case 'F': _LL73: break;
case 'e': _LL74: break; case 'E': _LL75: break; case 'g': _LL76: break; case 'G':
_LL77: break; case 'a': _LL78: break; case 'A': _LL79: break; case 'c': _LL80:
break; case 's': _LL81: break; case 'p': _LL82: break; case 'n': _LL83: break;
case '%': _LL84: break; default: _LL85: return 0;} return({ struct Cyc_Core_Opt*
_temp87=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp87->v=( void*)({ struct _tuple3* _temp88=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp88->f1= _temp2; _temp88->f2= _temp15; _temp88->f3=
_temp25; _temp88->f4= _temp39; _temp88->f5= c; _temp88->f6= i + 1; _temp88;});
_temp87;});}}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int _temp89= Cyc_String_strlen(
s); goto _LL90; _LL90: { struct Cyc_List_List* _temp91= 0; goto _LL92; _LL92: {
int i; for( i= 0; i < _temp89; i ++){ if(({ struct _tagged_string _temp93= s;
char* _temp95= _temp93.curr + i; if( _temp93.base == 0? 1:( _temp95 < _temp93.base?
1: _temp95 >= _temp93.last_plus_one)){ _throw( Null_Exception);}* _temp95;}) !='%'){
continue;}{ struct Cyc_Core_Opt* _temp96= Cyc_Formatstr_parse_conversionspecification(
s, i + 1); goto _LL97; _LL97: if( _temp96 == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp98=( char*)"bad format string"; struct
_tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one=
_temp98 + 18; _temp99;})); return 0;}{ struct _tuple3 _temp103; int _temp104;
char _temp106; struct Cyc_List_List* _temp108; struct Cyc_List_List* _temp110;
struct Cyc_List_List* _temp112; struct Cyc_List_List* _temp114; struct _tuple3*
_temp101=( struct _tuple3*)({ struct Cyc_Core_Opt* _temp100= _temp96; if(
_temp100 == 0){ _throw( Null_Exception);} _temp100->v;}); _temp103=* _temp101;
_LL115: _temp114= _temp103.f1; goto _LL113; _LL113: _temp112= _temp103.f2; goto
_LL111; _LL111: _temp110= _temp103.f3; goto _LL109; _LL109: _temp108= _temp103.f4;
goto _LL107; _LL107: _temp106= _temp103.f5; goto _LL105; _LL105: _temp104=
_temp103.f6; goto _LL102; _LL102: i= _temp104 - 1;{ struct Cyc_List_List*
_temp116= _temp108; struct Cyc_List_List _temp123; struct Cyc_List_List*
_temp124; int _temp126; _LL118: if( _temp116 == 0){ goto _LL120;} else{ _temp123=*
_temp116; _LL127: _temp126=( int) _temp123.hd; goto _LL125; _LL125: _temp124=(
struct Cyc_List_List*) _temp123.tl; if( _temp124 == 0){ goto _LL122;} else{ goto
_LL120;}} _LL122: if(( _temp126 =='j'? 1: _temp126 =='z')? 1: _temp126 =='t'){
goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL119: Cyc_Tcutil_terr(
loc, xprintf("length modifier '%c' is not supported", _temp126)); return 0;
_LL121: goto _LL117; _LL117:;}{ struct Cyc_List_List* _temp128= _temp112; struct
Cyc_List_List _temp135; struct Cyc_List_List* _temp136; int _temp138; _LL130:
if( _temp128 == 0){ goto _LL132;} else{ _temp135=* _temp128; _LL139: _temp138=(
int) _temp135.hd; goto _LL137; _LL137: _temp136=( struct Cyc_List_List*)
_temp135.tl; if( _temp136 == 0){ goto _LL134;} else{ goto _LL132;}} _LL134: if(
_temp138 =='*'){ goto _LL131;} else{ goto _LL132;} _LL132: goto _LL133; _LL131:
_temp91=({ struct Cyc_List_List* _temp140=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp140->hd=( void*) Cyc_Absyn_sint_t; _temp140->tl=
_temp91; _temp140;}); goto _LL129; _LL133: goto _LL129; _LL129:;}{ struct Cyc_List_List*
_temp141= _temp110; struct Cyc_List_List _temp148; struct Cyc_List_List*
_temp149; struct Cyc_List_List _temp151; struct Cyc_List_List* _temp152; int
_temp154; int _temp156; _LL143: if( _temp141 == 0){ goto _LL145;} else{ _temp148=*
_temp141; _LL157: _temp156=( int) _temp148.hd; goto _LL150; _LL150: _temp149=(
struct Cyc_List_List*) _temp148.tl; if( _temp149 == 0){ goto _LL145;} else{
_temp151=* _temp149; _LL155: _temp154=( int) _temp151.hd; goto _LL153; _LL153:
_temp152=( struct Cyc_List_List*) _temp151.tl; if( _temp152 == 0){ goto _LL147;}
else{ goto _LL145;}}} _LL147: if( _temp156 =='.'? _temp154 =='*': 0){ goto
_LL144;} else{ goto _LL145;} _LL145: goto _LL146; _LL144: _temp91=({ struct Cyc_List_List*
_temp158=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp158->hd=( void*) Cyc_Absyn_sint_t; _temp158->tl= _temp91; _temp158;}); goto
_LL142; _LL146: goto _LL142; _LL142:;}{ void* t; switch( _temp106){ case 'd':
_LL159: goto _LL160; case 'i': _LL160:{ struct Cyc_List_List* f= _temp114; for(
0; f != 0; f=({ struct Cyc_List_List* _temp162= f; if( _temp162 == 0){ _throw(
Null_Exception);} _temp162->tl;})){ if(( int)({ struct Cyc_List_List* _temp163=
f; if( _temp163 == 0){ _throw( Null_Exception);} _temp163->hd;}) =='#'){ Cyc_Tcutil_terr(
loc, xprintf("flag '#' is not valid with %%%c",( int) _temp106)); return 0;}}}{
struct Cyc_List_List* _temp164= _temp108; struct Cyc_List_List _temp177; struct
Cyc_List_List* _temp178; int _temp180; struct Cyc_List_List _temp183; struct Cyc_List_List*
_temp184; int _temp186; struct Cyc_List_List _temp189; struct Cyc_List_List*
_temp190; struct Cyc_List_List _temp192; struct Cyc_List_List* _temp193; int
_temp195; int _temp197; _LL166: if( _temp164 == 0){ goto _LL167;} else{ goto
_LL168;} _LL168: if( _temp164 == 0){ goto _LL170;} else{ _temp177=* _temp164;
_LL181: _temp180=( int) _temp177.hd; goto _LL179; _LL179: _temp178=( struct Cyc_List_List*)
_temp177.tl; if( _temp178 == 0){ goto _LL176;} else{ goto _LL170;}} _LL176: if(
_temp180 =='l'){ goto _LL169;} else{ goto _LL170;} _LL170: if( _temp164 == 0){
goto _LL172;} else{ _temp183=* _temp164; _LL187: _temp186=( int) _temp183.hd;
goto _LL185; _LL185: _temp184=( struct Cyc_List_List*) _temp183.tl; if( _temp184
== 0){ goto _LL182;} else{ goto _LL172;}} _LL182: if( _temp186 =='h'){ goto
_LL171;} else{ goto _LL172;} _LL172: if( _temp164 == 0){ goto _LL174;} else{
_temp189=* _temp164; _LL198: _temp197=( int) _temp189.hd; goto _LL191; _LL191:
_temp190=( struct Cyc_List_List*) _temp189.tl; if( _temp190 == 0){ goto _LL174;}
else{ _temp192=* _temp190; _LL196: _temp195=( int) _temp192.hd; goto _LL194;
_LL194: _temp193=( struct Cyc_List_List*) _temp192.tl; if( _temp193 == 0){ goto
_LL188;} else{ goto _LL174;}}} _LL188: if( _temp197 =='h'? _temp195 =='h': 0){
goto _LL173;} else{ goto _LL174;} _LL174: goto _LL175; _LL167: t= Cyc_Absyn_sint_t;
goto _LL165; _LL169: t= Cyc_Absyn_slong_t; goto _LL165; _LL171: t= Cyc_Absyn_sshort_t;
goto _LL165; _LL173: t= Cyc_Absyn_schar_t; goto _LL165; _LL175: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp199= Cyc_String_implode( _temp108); int
_temp200=( int) _temp106; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp199.last_plus_one - _temp199.curr, _temp199.curr, _temp200);})); return 0;
_LL165:;} _temp91=({ struct Cyc_List_List* _temp201=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp201->hd=( void*) t; _temp201->tl=
_temp91; _temp201;}); break; case 'u': _LL161:{ struct Cyc_List_List* f=
_temp114; for( 0; f != 0; f=({ struct Cyc_List_List* _temp203= f; if( _temp203
== 0){ _throw( Null_Exception);} _temp203->tl;})){ if(( int)({ struct Cyc_List_List*
_temp204= f; if( _temp204 == 0){ _throw( Null_Exception);} _temp204->hd;}) =='#'){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp205=( char*)"Flag '#' not valid with %%u";
struct _tagged_string _temp206; _temp206.curr= _temp205; _temp206.base= _temp205;
_temp206.last_plus_one= _temp205 + 28; _temp206;})); return 0;}}} goto _LL202;
case 'o': _LL202: goto _LL207; case 'x': _LL207: goto _LL208; case 'X': _LL208:{
struct Cyc_List_List* _temp210= _temp108; struct Cyc_List_List _temp223; struct
Cyc_List_List* _temp224; int _temp226; struct Cyc_List_List _temp229; struct Cyc_List_List*
_temp230; int _temp232; struct Cyc_List_List _temp235; struct Cyc_List_List*
_temp236; struct Cyc_List_List _temp238; struct Cyc_List_List* _temp239; int
_temp241; int _temp243; _LL212: if( _temp210 == 0){ goto _LL213;} else{ goto
_LL214;} _LL214: if( _temp210 == 0){ goto _LL216;} else{ _temp223=* _temp210;
_LL227: _temp226=( int) _temp223.hd; goto _LL225; _LL225: _temp224=( struct Cyc_List_List*)
_temp223.tl; if( _temp224 == 0){ goto _LL222;} else{ goto _LL216;}} _LL222: if(
_temp226 =='l'){ goto _LL215;} else{ goto _LL216;} _LL216: if( _temp210 == 0){
goto _LL218;} else{ _temp229=* _temp210; _LL233: _temp232=( int) _temp229.hd;
goto _LL231; _LL231: _temp230=( struct Cyc_List_List*) _temp229.tl; if( _temp230
== 0){ goto _LL228;} else{ goto _LL218;}} _LL228: if( _temp232 =='h'){ goto
_LL217;} else{ goto _LL218;} _LL218: if( _temp210 == 0){ goto _LL220;} else{
_temp235=* _temp210; _LL244: _temp243=( int) _temp235.hd; goto _LL237; _LL237:
_temp236=( struct Cyc_List_List*) _temp235.tl; if( _temp236 == 0){ goto _LL220;}
else{ _temp238=* _temp236; _LL242: _temp241=( int) _temp238.hd; goto _LL240;
_LL240: _temp239=( struct Cyc_List_List*) _temp238.tl; if( _temp239 == 0){ goto
_LL234;} else{ goto _LL220;}}} _LL234: if( _temp243 =='h'? _temp241 =='h': 0){
goto _LL219;} else{ goto _LL220;} _LL220: goto _LL221; _LL213: t= Cyc_Absyn_uint_t;
goto _LL211; _LL215: t= Cyc_Absyn_ulong_t; goto _LL211; _LL217: t= Cyc_Absyn_ushort_t;
goto _LL211; _LL219: t= Cyc_Absyn_uchar_t; goto _LL211; _LL221: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp245= Cyc_String_implode( _temp108); int
_temp246=( int) _temp106; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp245.last_plus_one - _temp245.curr, _temp245.curr, _temp246);})); return 0;
_LL211:;} _temp91=({ struct Cyc_List_List* _temp247=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp247->hd=( void*) t; _temp247->tl=
_temp91; _temp247;}); break; case 'f': _LL209: goto _LL248; case 'F': _LL248:
goto _LL249; case 'e': _LL249: goto _LL250; case 'E': _LL250: goto _LL251; case
'g': _LL251: goto _LL252; case 'G': _LL252: goto _LL253; case 'a': _LL253: goto
_LL254; case 'A': _LL254: if( _temp108 != 0){ Cyc_Tcutil_terr( loc, xprintf("length modifiers are not allowed with %%%c",(
int) _temp106)); return 0;} _temp91=({ struct Cyc_List_List* _temp256=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp256->hd=( void*)
Cyc_Absyn_double_t; _temp256->tl= _temp91; _temp256;}); break; case 'c': _LL255:{
struct Cyc_List_List* f= _temp114; for( 0; f != 0; f=({ struct Cyc_List_List*
_temp258= f; if( _temp258 == 0){ _throw( Null_Exception);} _temp258->tl;})){ if((
int)({ struct Cyc_List_List* _temp259= f; if( _temp259 == 0){ _throw(
Null_Exception);} _temp259->hd;}) =='#'? 1:( int)({ struct Cyc_List_List*
_temp260= f; if( _temp260 == 0){ _throw( Null_Exception);} _temp260->hd;}) =='0'){
Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%c",( int)({ struct
Cyc_List_List* _temp261= f; if( _temp261 == 0){ _throw( Null_Exception);}
_temp261->hd;}))); return 0;}}} if( _temp108 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp262= Cyc_String_implode( _temp108); xprintf("length modifier '%.*s' not allowed with %%c",
_temp262.last_plus_one - _temp262.curr, _temp262.curr);})); return 0;} if(
_temp110 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp263= Cyc_String_implode(
_temp110); xprintf("precision '%.*s' not allowed with %%c", _temp263.last_plus_one
- _temp263.curr, _temp263.curr);})); return 0;} _temp91=({ struct Cyc_List_List*
_temp264=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp264->hd=( void*) Cyc_Absyn_sint_t; _temp264->tl= _temp91; _temp264;});
break; case 's': _LL257: if( _temp114 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp266= Cyc_String_implode( _temp114); xprintf("flags '%.*s' not allowed with %%s",
_temp266.last_plus_one - _temp266.curr, _temp266.curr);})); return 0;} if(
_temp112 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp267= Cyc_String_implode(
_temp112); xprintf("width '%.*s' not allowed with %%s", _temp267.last_plus_one -
_temp267.curr, _temp267.curr);})); return 0;} if( _temp110 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp268= Cyc_String_implode( _temp110); xprintf("precision '%.*s' not allowed with %%s",
_temp268.last_plus_one - _temp268.curr, _temp268.curr);})); return 0;} if(
_temp108 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp269= Cyc_String_implode(
_temp108); xprintf("length modifier '%.*s' not allowed with %%s", _temp269.last_plus_one
- _temp269.curr, _temp269.curr);})); return 0;} _temp91=({ struct Cyc_List_List*
_temp270=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp270->hd=( void*) Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind));
_temp270->tl= _temp91; _temp270;}); break; case 'p': _LL265: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp272=( char*)"%%p not supported";
struct _tagged_string _temp273; _temp273.curr= _temp272; _temp273.base= _temp272;
_temp273.last_plus_one= _temp272 + 18; _temp273;})); return 0; case 'n': _LL271:{
struct Cyc_List_List* f= _temp114; for( 0; f != 0; f=({ struct Cyc_List_List*
_temp275= f; if( _temp275 == 0){ _throw( Null_Exception);} _temp275->tl;})){ if((
int)({ struct Cyc_List_List* _temp276= f; if( _temp276 == 0){ _throw(
Null_Exception);} _temp276->hd;}) =='#'? 1:( int)({ struct Cyc_List_List*
_temp277= f; if( _temp277 == 0){ _throw( Null_Exception);} _temp277->hd;}) =='0'){
Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%n",( int)({ struct
Cyc_List_List* _temp278= f; if( _temp278 == 0){ _throw( Null_Exception);}
_temp278->hd;}))); return 0;}}} if( _temp110 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp279= Cyc_String_implode( _temp110); xprintf("precision '%.*s' not allowed with %%n",
_temp279.last_plus_one - _temp279.curr, _temp279.curr);})); return 0;}{ struct
Cyc_List_List* _temp280= _temp108; struct Cyc_List_List _temp293; struct Cyc_List_List*
_temp294; int _temp296; struct Cyc_List_List _temp299; struct Cyc_List_List*
_temp300; int _temp302; struct Cyc_List_List _temp305; struct Cyc_List_List*
_temp306; struct Cyc_List_List _temp308; struct Cyc_List_List* _temp309; int
_temp311; int _temp313; _LL282: if( _temp280 == 0){ goto _LL283;} else{ goto
_LL284;} _LL284: if( _temp280 == 0){ goto _LL286;} else{ _temp293=* _temp280;
_LL297: _temp296=( int) _temp293.hd; goto _LL295; _LL295: _temp294=( struct Cyc_List_List*)
_temp293.tl; if( _temp294 == 0){ goto _LL292;} else{ goto _LL286;}} _LL292: if(
_temp296 =='l'){ goto _LL285;} else{ goto _LL286;} _LL286: if( _temp280 == 0){
goto _LL288;} else{ _temp299=* _temp280; _LL303: _temp302=( int) _temp299.hd;
goto _LL301; _LL301: _temp300=( struct Cyc_List_List*) _temp299.tl; if( _temp300
== 0){ goto _LL298;} else{ goto _LL288;}} _LL298: if( _temp302 =='h'){ goto
_LL287;} else{ goto _LL288;} _LL288: if( _temp280 == 0){ goto _LL290;} else{
_temp305=* _temp280; _LL314: _temp313=( int) _temp305.hd; goto _LL307; _LL307:
_temp306=( struct Cyc_List_List*) _temp305.tl; if( _temp306 == 0){ goto _LL290;}
else{ _temp308=* _temp306; _LL312: _temp311=( int) _temp308.hd; goto _LL310;
_LL310: _temp309=( struct Cyc_List_List*) _temp308.tl; if( _temp309 == 0){ goto
_LL304;} else{ goto _LL290;}}} _LL304: if( _temp313 =='h'? _temp311 =='h': 0){
goto _LL289;} else{ goto _LL290;} _LL290: goto _LL291; _LL283: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL281; _LL285: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL281; _LL287: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL281; _LL289: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL281; _LL291: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp315= Cyc_String_implode( _temp108); xprintf("length modifier '%.*s' not allowed with %%n",
_temp315.last_plus_one - _temp315.curr, _temp315.curr);})); return 0; _LL281:;}
_temp91=({ struct Cyc_List_List* _temp316=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp316->hd=( void*) t; _temp316->tl= _temp91;
_temp316;}); break; case '%': _LL274: if( _temp114 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp318= Cyc_String_implode( _temp114); xprintf("flags '%.*s' not allowed with %%%%",
_temp318.last_plus_one - _temp318.curr, _temp318.curr);})); return 0;} if(
_temp112 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp319= Cyc_String_implode(
_temp112); xprintf("width '%.*s' not allowed with %%%%", _temp319.last_plus_one
- _temp319.curr, _temp319.curr);})); return 0;} if( _temp110 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp320= Cyc_String_implode( _temp110); xprintf("precision '%.*s' not allowed with %%%%",
_temp320.last_plus_one - _temp320.curr, _temp320.curr);})); return 0;} if(
_temp108 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp321= Cyc_String_implode(
_temp108); xprintf("length modifier '%.*s' not allowed with %%%%", _temp321.last_plus_one
- _temp321.curr, _temp321.curr);})); return 0;} break; default: _LL317: return 0;}}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp91);}}} struct _tuple4{ int f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; char f4; int f5; } ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat(
struct _tagged_string s, int i){ unsigned int _temp323= Cyc_String_strlen( s);
goto _LL324; _LL324: if( i < 0? 1: i >= _temp323){ return 0;}{ int _temp325= 0;
goto _LL326; _LL326: { char _temp330=({ struct _tagged_string _temp327= s; char*
_temp329= _temp327.curr + i; if( _temp327.base == 0? 1:( _temp329 < _temp327.base?
1: _temp329 >= _temp327.last_plus_one)){ _throw( Null_Exception);}* _temp329;});
goto _LL331; _LL331: if( _temp330 =='*'){ _temp325= 1; i ++; if( i >= _temp323){
return 0;}}{ struct Cyc_List_List* _temp332= 0; goto _LL333; _LL333: for( 0; i <
_temp323; i ++){ _temp330=({ struct _tagged_string _temp334= s; char* _temp336=
_temp334.curr + i; if( _temp334.base == 0? 1:( _temp336 < _temp334.base? 1:
_temp336 >= _temp334.last_plus_one)){ _throw( Null_Exception);}* _temp336;});
if( Cyc_Character_isdigit( _temp330)){ _temp332=({ struct Cyc_List_List*
_temp337=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp337->hd=( void*)(( int) _temp330); _temp337->tl= _temp332; _temp337;});}
else{ break;}} if( i >= _temp323){ return 0;} _temp332=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp332);{ struct Cyc_List_List*
_temp338= 0; goto _LL339; _LL339: _temp330=({ struct _tagged_string _temp340= s;
char* _temp342= _temp340.curr + i; if( _temp340.base == 0? 1:( _temp342 <
_temp340.base? 1: _temp342 >= _temp340.last_plus_one)){ _throw( Null_Exception);}*
_temp342;}); switch( _temp330){ case 'h': _LL343: _temp338=({ struct Cyc_List_List*
_temp345=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp345->hd=( void*)(( int) _temp330); _temp345->tl= _temp338; _temp345;}); i
++; if( i >= _temp323){ return 0;} _temp330=({ struct _tagged_string _temp346= s;
char* _temp348= _temp346.curr + i; if( _temp346.base == 0? 1:( _temp348 <
_temp346.base? 1: _temp348 >= _temp346.last_plus_one)){ _throw( Null_Exception);}*
_temp348;}); if( _temp330 =='h'){ _temp338=({ struct Cyc_List_List* _temp349=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp349->hd=(
void*)(( int) _temp330); _temp349->tl= _temp338; _temp349;}); i ++;} break; case
'l': _LL344: _temp338=({ struct Cyc_List_List* _temp351=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp351->hd=( void*)(( int) _temp330);
_temp351->tl= _temp338; _temp351;}); i ++; if( i >= _temp323){ return 0;}
_temp330=({ struct _tagged_string _temp352= s; char* _temp354= _temp352.curr + i;
if( _temp352.base == 0? 1:( _temp354 < _temp352.base? 1: _temp354 >= _temp352.last_plus_one)){
_throw( Null_Exception);}* _temp354;}); if( _temp330 =='l'){ _temp338=({ struct
Cyc_List_List* _temp355=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp355->hd=( void*)(( int) _temp330); _temp355->tl= _temp338; _temp355;}); i
++;} break; case 'j': _LL350: goto _LL356; case 'z': _LL356: goto _LL357; case 't':
_LL357: goto _LL358; case 'L': _LL358: _temp338=({ struct Cyc_List_List*
_temp360=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp360->hd=( void*)(( int) _temp330); _temp360->tl= _temp338; _temp360;}); i
++; break; default: _LL359: break;} if( i >= _temp323){ return 0;} _temp338=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp338);
_temp330=({ struct _tagged_string _temp362= s; char* _temp364= _temp362.curr + i;
if( _temp362.base == 0? 1:( _temp364 < _temp362.base? 1: _temp364 >= _temp362.last_plus_one)){
_throw( Null_Exception);}* _temp364;}); switch( _temp330){ case 'd': _LL365:
break; case 'i': _LL366: break; case 'o': _LL367: break; case 'u': _LL368:
break; case 'x': _LL369: break; case 'X': _LL370: break; case 'f': _LL371:
break; case 'F': _LL372: break; case 'e': _LL373: break; case 'E': _LL374:
break; case 'g': _LL375: break; case 'G': _LL376: break; case 'a': _LL377:
break; case 'A': _LL378: break; case 'c': _LL379: break; case 's': _LL380:
break; case 'p': _LL381: break; case 'n': _LL382: break; case '%': _LL383:
break; default: _LL384: return 0;} return({ struct Cyc_Core_Opt* _temp386=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp386->v=(
void*)({ struct _tuple4* _temp387=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp387->f1= _temp325; _temp387->f2= _temp332; _temp387->f3=
_temp338; _temp387->f4= _temp330; _temp387->f5= i + 1; _temp387;}); _temp386;});}}}}}
struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ unsigned int _temp388= Cyc_String_strlen( s);
goto _LL389; _LL389: { struct Cyc_List_List* _temp390= 0; goto _LL391; _LL391: {
int i; for( i= 0; i < _temp388; i ++){ if(({ struct _tagged_string _temp392= s;
char* _temp394= _temp392.curr + i; if( _temp392.base == 0? 1:( _temp394 <
_temp392.base? 1: _temp394 >= _temp392.last_plus_one)){ _throw( Null_Exception);}*
_temp394;}) !='%'){ continue;}{ struct Cyc_Core_Opt* _temp395= Cyc_Formatstr_parse_inputformat(
s, i + 1); goto _LL396; _LL396: if( _temp395 == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp397=( char*)"bad format string"; struct
_tagged_string _temp398; _temp398.curr= _temp397; _temp398.base= _temp397;
_temp398.last_plus_one= _temp397 + 18; _temp398;})); return 0;}{ struct _tuple4
_temp402; int _temp403; char _temp405; struct Cyc_List_List* _temp407; struct
Cyc_List_List* _temp409; int _temp411; struct _tuple4* _temp400=( struct _tuple4*)({
struct Cyc_Core_Opt* _temp399= _temp395; if( _temp399 == 0){ _throw(
Null_Exception);} _temp399->v;}); _temp402=* _temp400; _LL412: _temp411=
_temp402.f1; goto _LL410; _LL410: _temp409= _temp402.f2; goto _LL408; _LL408:
_temp407= _temp402.f3; goto _LL406; _LL406: _temp405= _temp402.f4; goto _LL404;
_LL404: _temp403= _temp402.f5; goto _LL401; _LL401: i= _temp403 - 1;{ struct Cyc_List_List*
_temp413= _temp407; struct Cyc_List_List _temp420; struct Cyc_List_List*
_temp421; int _temp423; _LL415: if( _temp413 == 0){ goto _LL417;} else{ _temp420=*
_temp413; _LL424: _temp423=( int) _temp420.hd; goto _LL422; _LL422: _temp421=(
struct Cyc_List_List*) _temp420.tl; if( _temp421 == 0){ goto _LL419;} else{ goto
_LL417;}} _LL419: if(( _temp423 =='j'? 1: _temp423 =='z')? 1: _temp423 =='t'){
goto _LL416;} else{ goto _LL417;} _LL417: goto _LL418; _LL416: Cyc_Tcutil_terr(
loc, xprintf("length modifier '%c' is not supported", _temp423)); return 0;
_LL418: goto _LL414; _LL414:;} if( _temp411){ continue;}{ void* t; switch(
_temp405){ case 'd': _LL425: goto _LL426; case 'i': _LL426:{ struct Cyc_List_List*
_temp428= _temp407; struct Cyc_List_List _temp441; struct Cyc_List_List*
_temp442; int _temp444; struct Cyc_List_List _temp447; struct Cyc_List_List*
_temp448; int _temp450; struct Cyc_List_List _temp453; struct Cyc_List_List*
_temp454; struct Cyc_List_List _temp456; struct Cyc_List_List* _temp457; int
_temp459; int _temp461; _LL430: if( _temp428 == 0){ goto _LL431;} else{ goto
_LL432;} _LL432: if( _temp428 == 0){ goto _LL434;} else{ _temp441=* _temp428;
_LL445: _temp444=( int) _temp441.hd; goto _LL443; _LL443: _temp442=( struct Cyc_List_List*)
_temp441.tl; if( _temp442 == 0){ goto _LL440;} else{ goto _LL434;}} _LL440: if(
_temp444 =='l'){ goto _LL433;} else{ goto _LL434;} _LL434: if( _temp428 == 0){
goto _LL436;} else{ _temp447=* _temp428; _LL451: _temp450=( int) _temp447.hd;
goto _LL449; _LL449: _temp448=( struct Cyc_List_List*) _temp447.tl; if( _temp448
== 0){ goto _LL446;} else{ goto _LL436;}} _LL446: if( _temp450 =='h'){ goto
_LL435;} else{ goto _LL436;} _LL436: if( _temp428 == 0){ goto _LL438;} else{
_temp453=* _temp428; _LL462: _temp461=( int) _temp453.hd; goto _LL455; _LL455:
_temp454=( struct Cyc_List_List*) _temp453.tl; if( _temp454 == 0){ goto _LL438;}
else{ _temp456=* _temp454; _LL460: _temp459=( int) _temp456.hd; goto _LL458;
_LL458: _temp457=( struct Cyc_List_List*) _temp456.tl; if( _temp457 == 0){ goto
_LL452;} else{ goto _LL438;}}} _LL452: if( _temp461 =='h'? _temp459 =='h': 0){
goto _LL437;} else{ goto _LL438;} _LL438: goto _LL439; _LL431: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL429; _LL433: t= Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL429; _LL435: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL429; _LL437: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL429; _LL439: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp463= Cyc_String_implode( _temp407); int
_temp464=( int) _temp405; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp463.last_plus_one - _temp463.curr, _temp463.curr, _temp464);})); return 0;
_LL429:;} _temp390=({ struct Cyc_List_List* _temp465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp465->hd=( void*) t; _temp465->tl=
_temp390; _temp465;}); break; case 'u': _LL427: goto _LL466; case 'o': _LL466:
goto _LL467; case 'x': _LL467: goto _LL468; case 'X': _LL468:{ struct Cyc_List_List*
_temp470= _temp407; struct Cyc_List_List _temp483; struct Cyc_List_List*
_temp484; int _temp486; struct Cyc_List_List _temp489; struct Cyc_List_List*
_temp490; int _temp492; struct Cyc_List_List _temp495; struct Cyc_List_List*
_temp496; struct Cyc_List_List _temp498; struct Cyc_List_List* _temp499; int
_temp501; int _temp503; _LL472: if( _temp470 == 0){ goto _LL473;} else{ goto
_LL474;} _LL474: if( _temp470 == 0){ goto _LL476;} else{ _temp483=* _temp470;
_LL487: _temp486=( int) _temp483.hd; goto _LL485; _LL485: _temp484=( struct Cyc_List_List*)
_temp483.tl; if( _temp484 == 0){ goto _LL482;} else{ goto _LL476;}} _LL482: if(
_temp486 =='l'){ goto _LL475;} else{ goto _LL476;} _LL476: if( _temp470 == 0){
goto _LL478;} else{ _temp489=* _temp470; _LL493: _temp492=( int) _temp489.hd;
goto _LL491; _LL491: _temp490=( struct Cyc_List_List*) _temp489.tl; if( _temp490
== 0){ goto _LL488;} else{ goto _LL478;}} _LL488: if( _temp492 =='h'){ goto
_LL477;} else{ goto _LL478;} _LL478: if( _temp470 == 0){ goto _LL480;} else{
_temp495=* _temp470; _LL504: _temp503=( int) _temp495.hd; goto _LL497; _LL497:
_temp496=( struct Cyc_List_List*) _temp495.tl; if( _temp496 == 0){ goto _LL480;}
else{ _temp498=* _temp496; _LL502: _temp501=( int) _temp498.hd; goto _LL500;
_LL500: _temp499=( struct Cyc_List_List*) _temp498.tl; if( _temp499 == 0){ goto
_LL494;} else{ goto _LL480;}}} _LL494: if( _temp503 =='h'? _temp501 =='h': 0){
goto _LL479;} else{ goto _LL480;} _LL480: goto _LL481; _LL473: t= Cyc_Absyn_at_typ(
Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL471; _LL475: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL471; _LL477: t= Cyc_Absyn_at_typ(
Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL471; _LL479: t= Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL471; _LL481: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp505= Cyc_String_implode( _temp407); int
_temp506=( int) _temp405; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp505.last_plus_one - _temp505.curr, _temp505.curr, _temp506);})); return 0;
_LL471:;} _temp390=({ struct Cyc_List_List* _temp507=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp507->hd=( void*) t; _temp507->tl=
_temp390; _temp507;}); break; case 'f': _LL469: goto _LL508; case 'F': _LL508:
goto _LL509; case 'e': _LL509: goto _LL510; case 'E': _LL510: goto _LL511; case
'g': _LL511: goto _LL512; case 'G': _LL512: goto _LL513; case 'a': _LL513: goto
_LL514; case 'A': _LL514:{ struct Cyc_List_List* _temp516= _temp407; struct Cyc_List_List
_temp525; struct Cyc_List_List* _temp526; int _temp528; _LL518: if( _temp516 ==
0){ goto _LL519;} else{ goto _LL520;} _LL520: if( _temp516 == 0){ goto _LL522;}
else{ _temp525=* _temp516; _LL529: _temp528=( int) _temp525.hd; goto _LL527;
_LL527: _temp526=( struct Cyc_List_List*) _temp525.tl; if( _temp526 == 0){ goto
_LL524;} else{ goto _LL522;}} _LL524: if( _temp528 =='l'){ goto _LL521;} else{
goto _LL522;} _LL522: goto _LL523; _LL519: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL517; _LL521: t= Cyc_Absyn_at_typ( Cyc_Absyn_double_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL517; _LL523: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp530= Cyc_String_implode( _temp407); int
_temp531=( int) _temp405; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp530.last_plus_one - _temp530.curr, _temp530.curr, _temp531);})); return 0;
_LL517:;} _temp390=({ struct Cyc_List_List* _temp532=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp532->hd=( void*) t; _temp532->tl=
_temp390; _temp532;}); break; case 'c': _LL515: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp534=( char*)"%%c is not supported"; struct
_tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 21; _temp535;})); return 0; case 's': _LL533:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp537=( char*)"%%s is not supported";
struct _tagged_string _temp538; _temp538.curr= _temp537; _temp538.base= _temp537;
_temp538.last_plus_one= _temp537 + 21; _temp538;})); return 0; case '[': _LL536:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp540=( char*)"%%[ is not supported";
struct _tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 21; _temp541;})); return 0; case 'p': _LL539:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp543=( char*)"%%p is not supported";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 21; _temp544;})); return 0; case 'n': _LL542:{
struct Cyc_List_List* _temp546= _temp407; struct Cyc_List_List _temp559; struct
Cyc_List_List* _temp560; int _temp562; struct Cyc_List_List _temp565; struct Cyc_List_List*
_temp566; int _temp568; struct Cyc_List_List _temp571; struct Cyc_List_List*
_temp572; struct Cyc_List_List _temp574; struct Cyc_List_List* _temp575; int
_temp577; int _temp579; _LL548: if( _temp546 == 0){ goto _LL549;} else{ goto
_LL550;} _LL550: if( _temp546 == 0){ goto _LL552;} else{ _temp559=* _temp546;
_LL563: _temp562=( int) _temp559.hd; goto _LL561; _LL561: _temp560=( struct Cyc_List_List*)
_temp559.tl; if( _temp560 == 0){ goto _LL558;} else{ goto _LL552;}} _LL558: if(
_temp562 =='l'){ goto _LL551;} else{ goto _LL552;} _LL552: if( _temp546 == 0){
goto _LL554;} else{ _temp565=* _temp546; _LL569: _temp568=( int) _temp565.hd;
goto _LL567; _LL567: _temp566=( struct Cyc_List_List*) _temp565.tl; if( _temp566
== 0){ goto _LL564;} else{ goto _LL554;}} _LL564: if( _temp568 =='h'){ goto
_LL553;} else{ goto _LL554;} _LL554: if( _temp546 == 0){ goto _LL556;} else{
_temp571=* _temp546; _LL580: _temp579=( int) _temp571.hd; goto _LL573; _LL573:
_temp572=( struct Cyc_List_List*) _temp571.tl; if( _temp572 == 0){ goto _LL556;}
else{ _temp574=* _temp572; _LL578: _temp577=( int) _temp574.hd; goto _LL576;
_LL576: _temp575=( struct Cyc_List_List*) _temp574.tl; if( _temp575 == 0){ goto
_LL570;} else{ goto _LL556;}}} _LL570: if( _temp579 =='h'? _temp577 =='h': 0){
goto _LL555;} else{ goto _LL556;} _LL556: goto _LL557; _LL549: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL547; _LL551: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL547; _LL553: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL547; _LL555: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL547; _LL557: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp581= Cyc_String_implode( _temp407); xprintf("length modifier '%.*s' not allowed with %%n",
_temp581.last_plus_one - _temp581.curr, _temp581.curr);})); return 0; _LL547:;}
_temp390=({ struct Cyc_List_List* _temp582=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp582->hd=( void*) t; _temp582->tl= _temp390;
_temp582;}); break; case '%': _LL545: if( _temp411){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp584=( char*)"Assignment suppression (*) is not allowed with %%%%";
struct _tagged_string _temp585; _temp585.curr= _temp584; _temp585.base= _temp584;
_temp585.last_plus_one= _temp584 + 52; _temp585;})); return 0;} if( _temp409 !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp586= Cyc_String_implode(
_temp409); xprintf("width '%.*s' not allowed with %%%%", _temp586.last_plus_one
- _temp586.curr, _temp586.curr);})); return 0;} if( _temp407 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp587= Cyc_String_implode( _temp407); xprintf("length modifier '%.*s' not allowed with %%%%",
_temp587.last_plus_one - _temp587.curr, _temp587.curr);})); return 0;} break;
default: _LL583: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp390);}}}