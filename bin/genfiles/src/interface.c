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
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern void Cyc_List_iter2( void(*
f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern char
Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file;
struct _tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; extern struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs( int, int); static const int Cyc_Position_Lex= 0;
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
14; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 15; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 29; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
30; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 31; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 32; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 33; struct Cyc_Absyn_Fill_e_struct{
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
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc); struct Cyc_Set_Set; typedef struct
Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef
struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
extern void* Cyc_Dict_fold( void*(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); extern void Cyc_Dict_iter2( void(* f)( void*,
void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct Cyc_Dict_Dict*
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
Cyc_Tcenv_tc_init(); struct Cyc_Interface_T; typedef struct Cyc_Interface_T* Cyc_Interface_t;
extern struct Cyc_Interface_T* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
extern int Cyc_Interface_is_subinterface( struct Cyc_Interface_T* i1, struct Cyc_Interface_T*
i2); extern void Cyc_Interface_print( struct Cyc_Interface_T*, struct Cyc_Stdio___sFILE*);
extern struct Cyc_Interface_T* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern int Cyc_Absynpp_expand_typedefs; extern
void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
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
extern char Cyc_Okay_tok[ 13u]; extern char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{
char* tag; int f1; } ; extern char Cyc_Int_tok[ 12u]; struct _tuple3{ void* f1;
int f2; } ; struct Cyc_Int_tok_struct{ char* tag; struct _tuple3* f1; } ; extern
char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{ char* tag; char f1; } ;
extern char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_Short_tok[ 14u]; struct Cyc_Short_tok_struct{
char* tag; short f1; } ; extern char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; extern
char Cyc_Type_tok[ 13u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_ExpList_tok[ 16u];
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_Primop_tok[ 15u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1; }
; extern char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{ char*
tag; struct Cyc_Core_Opt* f1; } ; extern char Cyc_QualId_tok[ 15u]; struct Cyc_QualId_tok_struct{
char* tag; struct _tuple0* f1; } ; extern char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{
char* tag; struct Cyc_Absyn_Stmt* f1; } ; extern char Cyc_SwitchClauseList_tok[
25u]; struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_Pattern_tok[ 16u]; struct Cyc_Pattern_tok_struct{ char*
tag; struct Cyc_Absyn_Pat* f1; } ; extern char Cyc_PatternList_tok[ 20u]; struct
Cyc_PatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern char
Cyc_FnDecl_tok[ 15u]; struct Cyc_FnDecl_tok_struct{ char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern char Cyc_DeclList_tok[ 17u]; struct Cyc_DeclList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_DeclSpec_tok[ 17u]; struct
Cyc_DeclSpec_tok_struct{ char* tag; struct Cyc_Declaration_spec* f1; } ; extern
char Cyc_InitDecl_tok[ 17u]; struct _tuple4{ struct Cyc_Declarator* f1; struct
Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple4*
f1; } ; extern char Cyc_InitDeclList_tok[ 21u]; struct Cyc_InitDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_TypeSpecifier_tok[
22u]; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char
Cyc_QualSpecList_tok[ 21u]; struct _tuple5{ struct Cyc_Absyn_Tqual f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
char* tag; struct _tuple5* f1; } ; extern char Cyc_TypeQual_tok[ 17u]; struct
Cyc_TypeQual_tok_struct{ char* tag; struct Cyc_Absyn_Tqual f1; } ; extern char
Cyc_StructFieldDeclList_tok[ 28u]; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_StructFieldDeclListList_tok[
32u]; struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
char* tag; struct Cyc_Declarator* f1; } ; extern char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple6{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ char* tag; struct _tuple6* f1; } ;
extern char Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_AbstractDeclarator_tok[
27u]; struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_ParamDecl_tok[ 18u]; struct Cyc_ParamDecl_tok_struct{ char*
tag; struct _tuple1* f1; } ; extern char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDeclListBool_tok[
26u]; struct _tuple7{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3;
} ; struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple7* f1; } ;
extern char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; extern char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Designator_tok[ 19u];
struct Cyc_Designator_tok_struct{ char* tag; void* f1; } ; extern char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_TypeModifierList_tok[ 25u]; struct Cyc_TypeModifierList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Rgn_tok[ 12u]; struct
Cyc_Rgn_tok_struct{ char* tag; void* f1; } ; extern char Cyc_InitializerList_tok[
24u]; struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_FieldPattern_tok[ 21u]; struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{ char* tag;
struct _tuple8* f1; } ; extern char Cyc_FieldPatternList_tok[ 25u]; struct Cyc_FieldPatternList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_BlockItem_tok[ 18u];
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Kind_tok[
13u]; struct Cyc_Kind_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Attribute_tok[
18u]; struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ; extern char Cyc_AttributeList_tok[
22u]; struct Cyc_AttributeList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{ char*
tag; struct Cyc_Absyn_Enumfield* f1; } ; extern char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
struct Cyc_Yyltype{ int timestamp; int first_line; int first_column; int
last_line; int last_column; struct _tagged_string text; } ; typedef struct Cyc_Yyltype
Cyc_yyltype; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern int Cyc_Tcutil_is_function_type(
void* t); extern void Cyc_Lex_lex_init(); struct Cyc_Interface_Ienv{ struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* tuniondecls;
struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefdecls; struct Cyc_Dict_Dict*
vardecls; } ; typedef struct Cyc_Interface_Ienv* Cyc_Interface_ienv_t; struct
Cyc_Interface_T{ struct Cyc_Dict_Dict* imports; struct Cyc_Dict_Dict* exports; }
; struct Cyc_Interface_Ienv* Cyc_Interface_new_ienv(){ return({ struct Cyc_Interface_Ienv*
_temp0=( struct Cyc_Interface_Ienv*) GC_malloc( sizeof( struct Cyc_Interface_Ienv));
_temp0->structdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->uniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->tuniondecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->typedefdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp0->vardecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp0;});}
struct _tuple9{ struct Cyc_Interface_Ienv* f1; struct Cyc_Interface_Ienv* f2; }
; static void Cyc_Interface_extract_structdecl( struct _tuple9* env, struct
_tagged_string* x, struct Cyc_Absyn_Structdecl** dp){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; struct Cyc_Absyn_Structdecl*
d=* dp; void* _temp1=( void*) d->sc; _LL3: if( _temp1 ==( void*) Cyc_Absyn_Static){
goto _LL4;} else{ goto _LL5;} _LL5: if( _temp1 ==( void*) Cyc_Absyn_Abstract){
goto _LL6;} else{ goto _LL7;} _LL7: if( _temp1 ==( void*) Cyc_Absyn_Public){
goto _LL8;} else{ goto _LL9;} _LL9: if( _temp1 ==( void*) Cyc_Absyn_ExternC){
goto _LL10;} else{ goto _LL11;} _LL11: if( _temp1 ==( void*) Cyc_Absyn_Extern){
goto _LL12;} else{ goto _LL2;} _LL4: goto _LL2; _LL6: if( d->fields != 0){ d=({
struct Cyc_Absyn_Structdecl* _temp13=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp13[ 0]=* d; _temp13;}); d->fields= 0;}
goto _LL8; _LL8: pub->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)(
pub->structdecls, x, d); goto _LL2; _LL10: goto _LL12; _LL12: ext->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl* data)) Cyc_Dict_insert)( ext->structdecls, x, d);
goto _LL2; _LL2:;} static void Cyc_Interface_extract_uniondecl( struct _tuple9*
env, struct _tagged_string* x, struct Cyc_Absyn_Uniondecl** dp){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; struct Cyc_Absyn_Uniondecl*
d=* dp; void* _temp14=( void*) d->sc; _LL16: if( _temp14 ==( void*) Cyc_Absyn_Static){
goto _LL17;} else{ goto _LL18;} _LL18: if( _temp14 ==( void*) Cyc_Absyn_Abstract){
goto _LL19;} else{ goto _LL20;} _LL20: if( _temp14 ==( void*) Cyc_Absyn_Public){
goto _LL21;} else{ goto _LL22;} _LL22: if( _temp14 ==( void*) Cyc_Absyn_ExternC){
goto _LL23;} else{ goto _LL24;} _LL24: if( _temp14 ==( void*) Cyc_Absyn_Extern){
goto _LL25;} else{ goto _LL15;} _LL17: goto _LL15; _LL19: if( d->fields != 0){ d=({
struct Cyc_Absyn_Uniondecl* _temp26=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp26[ 0]=* d; _temp26;}); d->fields= 0;}
goto _LL21; _LL21: pub->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)(
pub->uniondecls, x, d); goto _LL15; _LL23: goto _LL25; _LL25: ext->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Uniondecl* data)) Cyc_Dict_insert)( ext->uniondecls, x, d);
goto _LL15; _LL15:;} static void Cyc_Interface_extract_tuniondecl( struct
_tuple9* env, struct _tagged_string* x, struct Cyc_Absyn_Tuniondecl** dp){
struct Cyc_Interface_Ienv* ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2;
struct Cyc_Absyn_Tuniondecl* d=* dp; void* _temp27=( void*) d->sc; _LL29: if(
_temp27 ==( void*) Cyc_Absyn_Static){ goto _LL30;} else{ goto _LL31;} _LL31: if(
_temp27 ==( void*) Cyc_Absyn_Abstract){ goto _LL32;} else{ goto _LL33;} _LL33:
if( _temp27 ==( void*) Cyc_Absyn_Public){ goto _LL34;} else{ goto _LL35;} _LL35:
if( _temp27 ==( void*) Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL37;} _LL37:
if( _temp27 ==( void*) Cyc_Absyn_Extern){ goto _LL38;} else{ goto _LL28;} _LL30:
goto _LL28; _LL32: if( d->fields != 0){ d=({ struct Cyc_Absyn_Tuniondecl*
_temp39=( struct Cyc_Absyn_Tuniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp39[ 0]=* d; _temp39;}); d->fields= 0;} goto _LL34; _LL34: pub->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl* data)) Cyc_Dict_insert)( pub->tuniondecls, x, d);
goto _LL28; _LL36: goto _LL38; _LL38: ext->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl*
data)) Cyc_Dict_insert)( ext->tuniondecls, x, d); goto _LL28; _LL28:;} static
void Cyc_Interface_extract_enumdecl( struct _tuple9* env, struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl** dp){ struct Cyc_Interface_Ienv* ext=(* env).f1;
struct Cyc_Interface_Ienv* pub=(* env).f2; struct Cyc_Absyn_Enumdecl* d=* dp;
void* _temp40=( void*) d->sc; _LL42: if( _temp40 ==( void*) Cyc_Absyn_Static){
goto _LL43;} else{ goto _LL44;} _LL44: if( _temp40 ==( void*) Cyc_Absyn_Abstract){
goto _LL45;} else{ goto _LL46;} _LL46: if( _temp40 ==( void*) Cyc_Absyn_Public){
goto _LL47;} else{ goto _LL48;} _LL48: if( _temp40 ==( void*) Cyc_Absyn_ExternC){
goto _LL49;} else{ goto _LL50;} _LL50: if( _temp40 ==( void*) Cyc_Absyn_Extern){
goto _LL51;} else{ goto _LL41;} _LL43: goto _LL41; _LL45: if( d->fields != 0){ d=({
struct Cyc_Absyn_Enumdecl* _temp52=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp52[ 0]=* d; _temp52;}); d->fields= 0;}
goto _LL47; _LL47: pub->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)(
pub->enumdecls, x, d); goto _LL41; _LL49: goto _LL51; _LL51: ext->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl* data)) Cyc_Dict_insert)( ext->enumdecls, x, d); goto
_LL41; _LL41:;} static void Cyc_Interface_extract_typedef( struct _tuple9* env,
struct _tagged_string* x, struct Cyc_Absyn_Typedefdecl* d){ struct Cyc_Interface_Ienv*
pub=(* env).f2; pub->typedefdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)(
pub->typedefdecls, x, d);} struct _tuple10{ struct Cyc_Interface_Ienv* f1;
struct Cyc_Interface_Ienv* f2; int f3; } ; static void Cyc_Interface_extract_ordinarie(
struct _tuple10* env, struct _tagged_string* x, struct _tuple3* v){ struct Cyc_Interface_Ienv*
ext=(* env).f1; struct Cyc_Interface_Ienv* pub=(* env).f2; int
check_fun_definition=(* env).f3; void* r=(* v).f1; void* _temp53= r; void*
_temp59; _LL55: if(*(( int*) _temp53) == Cyc_Tcenv_VarRes){ _LL60: _temp59=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp53)->f1; goto _LL56;} else{ goto
_LL57;} _LL57: goto _LL58; _LL56:{ void* _temp61= _temp59; struct Cyc_Absyn_Fndecl*
_temp69; struct Cyc_Absyn_Vardecl* _temp71; _LL63: if(( unsigned int) _temp61 >
1u?*(( int*) _temp61) == Cyc_Absyn_Funname_b: 0){ _LL70: _temp69=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp61)->f1; goto _LL64;} else{ goto _LL65;}
_LL65: if(( unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Global_b:
0){ _LL72: _temp71=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp61)->f1; goto _LL66;} else{ goto _LL67;} _LL67: goto _LL68; _LL64: { struct
Cyc_Absyn_Vardecl* vd=({ struct Cyc_Absyn_Vardecl* _temp73=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp73->sc=( void*)(( void*)
_temp69->sc); _temp73->name= _temp69->name; _temp73->tq=({ struct Cyc_Absyn_Tqual
_temp75; _temp75.q_const= 0; _temp75.q_volatile= 0; _temp75.q_restrict= 0;
_temp75;}); _temp73->type=( void*)(( void*)({ struct Cyc_Core_Opt* _temp74=
_temp69->cached_typ; if( _temp74 == 0){ _throw( Null_Exception);} _temp74->v;}));
_temp73->initializer= 0; _temp73->rgn= 0; _temp73->attributes= 0; _temp73;});
check_fun_definition= 0; _temp71= vd; goto _LL66;} _LL66: _temp71=({ struct Cyc_Absyn_Vardecl*
_temp76=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp76[ 0]=* _temp71; _temp76;}); _temp71->initializer= 0;{ void* _temp77=(
void*) _temp71->sc; _LL79: if( _temp77 ==( void*) Cyc_Absyn_Static){ goto _LL80;}
else{ goto _LL81;} _LL81: if( _temp77 ==( void*) Cyc_Absyn_Abstract){ goto _LL82;}
else{ goto _LL83;} _LL83: if( _temp77 ==( void*) Cyc_Absyn_Public){ goto _LL84;}
else{ goto _LL85;} _LL85: if( _temp77 ==( void*) Cyc_Absyn_ExternC){ goto _LL86;}
else{ goto _LL87;} _LL87: if( _temp77 ==( void*) Cyc_Absyn_Extern){ goto _LL88;}
else{ goto _LL78;} _LL80: if( check_fun_definition? Cyc_Tcutil_is_function_type((
void*) _temp71->type): 0){ Cyc_Tcutil_terr( Cyc_Position_segment_of_abs( 0, 0),({
struct _tagged_string _temp89= Cyc_Absynpp_qvar2string( _temp71->name); xprintf("static function %.*s has never been defined",
_temp89.last_plus_one - _temp89.curr, _temp89.curr);}));} goto _LL78; _LL82:
goto _LL78; _LL84: if( check_fun_definition? Cyc_Tcutil_is_function_type(( void*)
_temp71->type): 0){ Cyc_Tcutil_terr( Cyc_Position_segment_of_abs( 0, 0),({
struct _tagged_string _temp90= Cyc_Absynpp_qvar2string( _temp71->name); xprintf("public function %.*s has never been defined",
_temp90.last_plus_one - _temp90.curr, _temp90.curr);}));} pub->vardecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Vardecl* data)) Cyc_Dict_insert)( pub->vardecls, x, _temp71);
goto _LL78; _LL86: goto _LL88; _LL88: ext->vardecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Vardecl*
data)) Cyc_Dict_insert)( ext->vardecls, x, _temp71); goto _LL78; _LL78:;} goto
_LL62; _LL68: goto _LL62; _LL62:;} goto _LL54; _LL58: goto _LL54; _LL54:;}
struct _tuple11{ struct Cyc_Interface_T* f1; int f2; } ; static void Cyc_Interface_extract_f(
struct _tuple11* env_f, struct Cyc_List_List* ns, struct Cyc_Tcenv_Genv* ge){
struct Cyc_Interface_T* i=(* env_f).f1; int check_fun_definition=(* env_f).f2;
struct _tuple9* env=({ struct _tuple9* _temp92=( struct _tuple9*) GC_malloc(
sizeof( struct _tuple9)); _temp92->f1=(( struct Cyc_Interface_Ienv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( i->imports, ns);
_temp92->f2=(( struct Cyc_Interface_Ienv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( i->exports, ns); _temp92;});(( void(*)(
void(* f)( struct _tuple9*, struct _tagged_string*, struct Cyc_Absyn_Structdecl**),
struct _tuple9* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_structdecl,
env, ge->structdecls);(( void(*)( void(* f)( struct _tuple9*, struct
_tagged_string*, struct Cyc_Absyn_Uniondecl**), struct _tuple9* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_uniondecl, env, ge->uniondecls);((
void(*)( void(* f)( struct _tuple9*, struct _tagged_string*, struct Cyc_Absyn_Tuniondecl**),
struct _tuple9* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_tuniondecl,
env, ge->tuniondecls);(( void(*)( void(* f)( struct _tuple9*, struct
_tagged_string*, struct Cyc_Absyn_Enumdecl**), struct _tuple9* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_enumdecl, env, ge->enumdecls);((
void(*)( void(* f)( struct _tuple9*, struct _tagged_string*, struct Cyc_Absyn_Typedefdecl*),
struct _tuple9* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)( Cyc_Interface_extract_typedef,
env, ge->typedefs);(( void(*)( void(* f)( struct _tuple10*, struct
_tagged_string*, struct _tuple3*), struct _tuple10* env, struct Cyc_Dict_Dict* d))
Cyc_Dict_iter_c)( Cyc_Interface_extract_ordinarie,({ struct _tuple10* _temp91=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp91->f1=(* env).f1;
_temp91->f2=(* env).f2; _temp91->f3= check_fun_definition; _temp91;}), ge->ordinaries);}
static struct Cyc_Interface_Ienv* Cyc_Interface_extract_new_ienv( void* ignore){
return Cyc_Interface_new_ienv();} static struct Cyc_Interface_T* Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict* ae, int check_fun_definition){ struct Cyc_Interface_T* i=({
struct Cyc_Interface_T* _temp94=( struct Cyc_Interface_T*) GC_malloc( sizeof(
struct Cyc_Interface_T)); _temp94->imports=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Interface_Ienv*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)((
struct Cyc_Interface_Ienv*(*)( struct Cyc_Tcenv_Genv* ignore)) Cyc_Interface_extract_new_ienv,
ae); _temp94->exports=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Interface_Ienv*(*
f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(( struct
Cyc_Interface_Ienv*(*)( struct Cyc_Tcenv_Genv* ignore)) Cyc_Interface_extract_new_ienv,
ae); _temp94;});(( void(*)( void(* f)( struct _tuple11*, struct Cyc_List_List*,
struct Cyc_Tcenv_Genv*), struct _tuple11* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(
Cyc_Interface_extract_f,({ struct _tuple11* _temp93=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp93->f1= i; _temp93->f2=
check_fun_definition; _temp93;}), ae); return i;} struct Cyc_Interface_T* Cyc_Interface_extract(
struct Cyc_Dict_Dict* ae){ return Cyc_Interface_gen_extract( ae, 1);} char Cyc_Interface_Check_False[
10u]="\000\000\000\000False"; static void Cyc_Interface_Check_is_true( int b){
if( ! b){( void) _throw(( void*) Cyc_Interface_Check_False);}} static void Cyc_Interface_Check_box(
void* x, void* y){ Cyc_Interface_Check_is_true( x == y);} static void Cyc_Interface_Check_list(
void(* f)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2){((
void(*)( void(* f)( void*, void*), struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_iter2)( f, l1, l2);} static void Cyc_Interface_Check_opt( void(* f)(
void*, void*), struct Cyc_Core_Opt* o1, struct Cyc_Core_Opt* o2){ if( o1 == 0){
Cyc_Interface_Check_is_true( o2 == 0);} else{ Cyc_Interface_Check_is_true( o2 !=
0); f(( void*)({ struct Cyc_Core_Opt* _temp95= o1; if( _temp95 == 0){ _throw(
Null_Exception);} _temp95->v;}),( void*)({ struct Cyc_Core_Opt* _temp96= o2; if(
_temp96 == 0){ _throw( Null_Exception);} _temp96->v;}));}} static void Cyc_Interface_Check_str(
struct _tagged_string s1, struct _tagged_string s2){ Cyc_Interface_Check_is_true(
Cyc_String_strcmp( s1, s2) == 0);} static void Cyc_Interface_Check_strptr(
struct _tagged_string* n1, struct _tagged_string* n2){ Cyc_Interface_Check_str(*
n1,* n2);} static void Cyc_Interface_Check_tvar( struct Cyc_Absyn_Tvar* t1,
struct Cyc_Absyn_Tvar* t2){;} static void Cyc_Interface_Check_tvar_list( struct
Cyc_List_List* l1, struct Cyc_List_List* l2){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l1, struct Cyc_List_List* l2))
Cyc_Interface_Check_list)( Cyc_Interface_Check_tvar, l1, l2);} static void Cyc_Interface_Check_tqual(
struct Cyc_Absyn_Tqual t1, struct Cyc_Absyn_Tqual t2){;} static void Cyc_Interface_Check_structfield(
struct Cyc_Absyn_Structfield* s1, struct Cyc_Absyn_Structfield* s2){ Cyc_Interface_Check_strptr(
s1->name, s2->name); Cyc_Interface_Check_tqual( s1->tq, s2->tq);} static void
Cyc_Interface_Check_structfield_list( struct Cyc_List_List* l1, struct Cyc_List_List*
l2){(( void(*)( void(* f)( struct Cyc_Absyn_Structfield*, struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_Interface_Check_list)(
Cyc_Interface_Check_structfield, l1, l2);} static void Cyc_Interface_Check_structdecl(
struct Cyc_Absyn_Structdecl* d1, struct Cyc_Absyn_Structdecl* d2){(( void(*)(
void* x, void* y)) Cyc_Interface_Check_box)(( void*) d1->sc,( void*) d2->sc);
Cyc_Interface_Check_tvar_list( d1->tvs, d2->tvs);(( void(*)( void(* f)( struct
Cyc_List_List*, struct Cyc_List_List*), struct Cyc_Core_Opt* o1, struct Cyc_Core_Opt*
o2)) Cyc_Interface_Check_opt)( Cyc_Interface_Check_structfield_list, d1->fields,
d2->fields);} static void Cyc_Interface_Check_uniondecl( struct Cyc_Absyn_Uniondecl*
d1, struct Cyc_Absyn_Uniondecl* d2){;} static void Cyc_Interface_Check_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Absyn_Tuniondecl* d2){;} static void
Cyc_Interface_Check_enumdecl( struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Absyn_Enumdecl*
d2){;} static void Cyc_Interface_Check_typedefdecl( struct Cyc_Absyn_Typedefdecl*
d1, struct Cyc_Absyn_Typedefdecl* d2){;} static void Cyc_Interface_Check_vardecl(
struct Cyc_Absyn_Vardecl* d1, struct Cyc_Absyn_Vardecl* d2){;} static void Cyc_Interface_Check_incl_ns(
struct Cyc_Interface_Ienv* ie1, struct Cyc_Interface_Ienv* ie2){(( void(*)( void(*
f)( struct Cyc_Absyn_Structdecl*, struct Cyc_Absyn_Structdecl*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_structdecl,
ie1->structdecls, ie2->structdecls);(( void(*)( void(* f)( struct Cyc_Absyn_Uniondecl*,
struct Cyc_Absyn_Uniondecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_iter2)( Cyc_Interface_Check_uniondecl, ie1->uniondecls, ie2->uniondecls);((
void(*)( void(* f)( struct Cyc_Absyn_Tuniondecl*, struct Cyc_Absyn_Tuniondecl*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_tuniondecl,
ie1->tuniondecls, ie2->tuniondecls);(( void(*)( void(* f)( struct Cyc_Absyn_Enumdecl*,
struct Cyc_Absyn_Enumdecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_iter2)( Cyc_Interface_Check_enumdecl, ie1->enumdecls, ie2->enumdecls);((
void(*)( void(* f)( struct Cyc_Absyn_Typedefdecl*, struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_typedefdecl,
ie1->typedefdecls, ie2->typedefdecls);(( void(*)( void(* f)( struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2))
Cyc_Dict_iter2)( Cyc_Interface_Check_vardecl, ie1->vardecls, ie2->vardecls);}
int Cyc_Interface_is_subinterface( struct Cyc_Interface_T* i1, struct Cyc_Interface_T*
i2){{ struct _handler_cons _temp97; _push_handler(& _temp97);{ void* _temp98=(
void*) setjmp( _temp97.handler); if( ! _temp98){(( void(*)( void(* f)( struct
Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*), struct Cyc_Dict_Dict* d1,
struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_incl_ns, i1->exports,
i2->exports);(( void(*)( void(* f)( struct Cyc_Interface_Ienv*, struct Cyc_Interface_Ienv*),
struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_iter2)( Cyc_Interface_Check_incl_ns,
i2->imports, i1->imports);; _pop_handler();} else{ void* _temp100= _temp98;
_LL102: if( _temp100 == Cyc_Interface_Check_False){ goto _LL103;} else{ goto
_LL104;} _LL104: if( _temp100 == Cyc_Dict_Absent){ goto _LL105;} else{ goto
_LL106;} _LL106: goto _LL107; _LL103: goto _LL105; _LL105: return 0; _LL107:(
void) _throw( _temp100); _LL101:;}}} return 1;} static struct Cyc_List_List* Cyc_Interface_add_namespace(
struct Cyc_List_List* tds, struct Cyc_List_List* ns){ if( ns == 0){ return tds;}
return({ struct Cyc_List_List* _temp108=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp108->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp109=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp109[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp110; _temp110.tag= Cyc_Absyn_Namespace_d;
_temp110.f1=( struct _tagged_string*)({ struct Cyc_List_List* _temp111= ns; if(
_temp111 == 0){ _throw( Null_Exception);} _temp111->hd;}); _temp110.f2= Cyc_Interface_add_namespace(
tds,({ struct Cyc_List_List* _temp112= ns; if( _temp112 == 0){ _throw(
Null_Exception);} _temp112->tl;})); _temp110;}); _temp109;}), 0); _temp108->tl=
0; _temp108;});} static struct Cyc_List_List* Cyc_Interface_add_structdecl(
struct _tagged_string* x, struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List*
tds){ return({ struct Cyc_List_List* _temp113=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp113->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp114=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp114[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp115; _temp115.tag= Cyc_Absyn_Struct_d; _temp115.f1=
d; _temp115;}); _temp114;}), 0); _temp113->tl= tds; _temp113;});} static struct
Cyc_List_List* Cyc_Interface_add_structdecl_header( struct _tagged_string* x,
struct Cyc_Absyn_Structdecl* d, struct Cyc_List_List* tds){ d=({ struct Cyc_Absyn_Structdecl*
_temp116=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp116[ 0]=* d; _temp116;}); d->fields= 0;( void*)( d->sc=( void*)(( void*)
Cyc_Absyn_Extern)); return({ struct Cyc_List_List* _temp117=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp117->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Struct_d_struct* _temp118=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp118[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp119; _temp119.tag= Cyc_Absyn_Struct_d; _temp119.f1=
d; _temp119;}); _temp118;}), 0); _temp117->tl= tds; _temp117;});} static struct
Cyc_List_List* Cyc_Interface_add_uniondecl( struct _tagged_string* x, struct Cyc_Absyn_Uniondecl*
d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List* _temp120=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp120->hd=( void*)
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct* _temp121=( struct
Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp121[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp122; _temp122.tag= Cyc_Absyn_Union_d;
_temp122.f1= d; _temp122;}); _temp121;}), 0); _temp120->tl= tds; _temp120;});}
static struct Cyc_List_List* Cyc_Interface_add_uniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Uniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Uniondecl* _temp123=( struct Cyc_Absyn_Uniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Uniondecl)); _temp123[ 0]=* d; _temp123;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp124->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp125=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp125[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp126; _temp126.tag= Cyc_Absyn_Union_d;
_temp126.f1= d; _temp126;}); _temp125;}), 0); _temp124->tl= tds; _temp124;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl( struct _tagged_string*
x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ return({ struct
Cyc_List_List* _temp127=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp127->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp128=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp128[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp129; _temp129.tag= Cyc_Absyn_Tunion_d;
_temp129.f1= d; _temp129;}); _temp128;}), 0); _temp127->tl= tds; _temp127;});}
static struct Cyc_List_List* Cyc_Interface_add_tuniondecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Tuniondecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Tuniondecl* _temp130=( struct Cyc_Absyn_Tuniondecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp130[ 0]=* d; _temp130;}); d->fields=
0;( void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp131=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp131->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp132=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp132[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp133; _temp133.tag= Cyc_Absyn_Tunion_d;
_temp133.f1= d; _temp133;}); _temp132;}), 0); _temp131->tl= tds; _temp131;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl( struct _tagged_string*
x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp134=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp134->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp135=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp135[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp136; _temp136.tag= Cyc_Absyn_Enum_d;
_temp136.f1= d; _temp136;}); _temp135;}), 0); _temp134->tl= tds; _temp134;});}
static struct Cyc_List_List* Cyc_Interface_add_enumdecl_header( struct
_tagged_string* x, struct Cyc_Absyn_Enumdecl* d, struct Cyc_List_List* tds){ d=({
struct Cyc_Absyn_Enumdecl* _temp137=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp137[ 0]=* d; _temp137;}); d->fields= 0;(
void*)( d->sc=( void*)(( void*) Cyc_Absyn_Extern)); return({ struct Cyc_List_List*
_temp138=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp138->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp139=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp139[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp140; _temp140.tag= Cyc_Absyn_Enum_d;
_temp140.f1= d; _temp140;}); _temp139;}), 0); _temp138->tl= tds; _temp138;});}
static struct Cyc_List_List* Cyc_Interface_add_typedef( struct _tagged_string* x,
struct Cyc_Absyn_Typedefdecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp141->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp142=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp142[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp143; _temp143.tag= Cyc_Absyn_Typedef_d;
_temp143.f1= d; _temp143;}); _temp142;}), 0); _temp141->tl= tds; _temp141;});}
static struct Cyc_List_List* Cyc_Interface_add_var( struct _tagged_string* x,
struct Cyc_Absyn_Vardecl* d, struct Cyc_List_List* tds){ return({ struct Cyc_List_List*
_temp144=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp144->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp145=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp145[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp146; _temp146.tag= Cyc_Absyn_Var_d;
_temp146.f1= d; _temp146;}); _temp145;}), 0); _temp144->tl= tds; _temp144;});}
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
tds, f);} void Cyc_Interface_print( struct Cyc_Interface_T* i, struct Cyc_Stdio___sFILE*
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
f, i->exports);} struct Cyc_Interface_T* Cyc_Interface_parse( struct Cyc_Stdio___sFILE*
f){ Cyc_Lex_lex_init();{ struct Cyc_List_List* tds= Cyc_Parse_parse_file( f);
Cyc_Absynpp_expand_typedefs= 0;{ struct Cyc_Tcenv_Tenv* te= Cyc_Tcenv_tc_init();
Cyc_Tc_tc( te, 0, tds); tds= Cyc_Tc_treeshake( te, tds); return Cyc_Interface_gen_extract(
te->ae, 0);}}}