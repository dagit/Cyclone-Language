#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef unsigned char Cyc_u_char; typedef
unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int
Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint;
typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
unsigned char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef unsigned char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); extern int system( unsigned char*); struct
Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern unsigned char Cyc_Stdio_FileCloseError[ 19u];
extern unsigned char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern unsigned char Cyc_Position_Nocontext[ 14u]; typedef
struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string*
Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef
void* Cyc_Absyn_nmspace_t; struct _tuple0{ void* f1; struct _tagged_string* f2;
} ; typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t; typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t; typedef struct Cyc_Absyn_TunionFieldInfo
Cyc_Absyn_tunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void*
Cyc_Absyn_incrementor_t; typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
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
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
12; static const int Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att=
14; static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int
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
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
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
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
extern unsigned char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct
Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct
Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); extern
void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern
struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes= 0;
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_string*, struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern unsigned char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type(
void* t); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default(
void*); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**,
struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, int var_default_init, struct Cyc_List_List* ds); extern
struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*);
extern unsigned char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl* base; struct Cyc_Absyn_Tunionfield* field; } ;
typedef struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_xtunionfielddecl_t;
extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern void* Cyc_Tcdecl_merge_binding( void*
d0, void* d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg);
extern struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); static unsigned char _temp2[ 1u]=""; static struct
_tagged_string Cyc_Tc_tc_msg_c={ _temp2, _temp2, _temp2 + 1u}; static struct
_tagged_string* Cyc_Tc_tc_msg=( struct _tagged_string*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp3= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp9;
struct Cyc_List_List* _temp11; struct Cyc_List_List** _temp13; struct Cyc_Absyn_VarargInfo*
_temp14; int _temp16; struct Cyc_List_List* _temp18; void* _temp20; struct Cyc_Core_Opt*
_temp22; struct Cyc_List_List* _temp24; _LL5: if(( unsigned int) _temp3 > 4u?*((
int*) _temp3) == Cyc_Absyn_FnType: 0){ _LL10: _temp9=(( struct Cyc_Absyn_FnType_struct*)
_temp3)->f1; _LL25: _temp24= _temp9.tvars; goto _LL23; _LL23: _temp22= _temp9.effect;
goto _LL21; _LL21: _temp20=( void*) _temp9.ret_typ; goto _LL19; _LL19: _temp18=
_temp9.args; goto _LL17; _LL17: _temp16= _temp9.c_varargs; goto _LL15; _LL15:
_temp14= _temp9.cyc_varargs; goto _LL12; _LL12: _temp11= _temp9.attributes;
_temp13=&((( struct Cyc_Absyn_FnType_struct*) _temp3)->f1).attributes; goto _LL6;}
else{ goto _LL7;} _LL7: goto _LL8; _LL6: { struct Cyc_List_List* _temp26= 0;
goto _LL27; _LL27: for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ void* _temp28=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd;
int _temp44; _LL30: if(( unsigned int) _temp28 > 16u?*(( int*) _temp28) == Cyc_Absyn_Regparm_att:
0){ _LL45: _temp44=(( struct Cyc_Absyn_Regparm_att_struct*) _temp28)->f1; goto
_LL31;} else{ goto _LL32;} _LL32: if( _temp28 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp28 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp28 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL37;} else{ goto _LL38;} _LL38: if( _temp28 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL39;} else{ goto _LL40;} _LL40: if( _temp28 ==( void*) Cyc_Absyn_Const_att){
goto _LL41;} else{ goto _LL42;} _LL42: goto _LL43; _LL31: goto _LL33; _LL33:
goto _LL35; _LL35: goto _LL37; _LL37: goto _LL39; _LL39: goto _LL41; _LL41:*
_temp13=({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp46->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp46->tl=* _temp13; _temp46;}); continue; _LL43:
_temp26=({ struct Cyc_List_List* _temp47=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp47->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp47->tl= _temp26; _temp47;}); continue; _LL29:;}
return _temp26;} _LL8: return(( struct Cyc_List_List*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ unsigned char* _temp48="transfer_fn_type_atts"; struct
_tagged_string _temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 22; _temp49;})); _LL4:;} struct _tuple3{ void* f1; int f2; } ; static
void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl* vd, int
check_var_init){ struct _tagged_string* v=(* vd->name).f2; void* t=( void*) vd->type;
void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{ void*
_temp50=(* vd->name).f1; struct Cyc_List_List* _temp58; struct Cyc_List_List*
_temp60; _LL52: if(( unsigned int) _temp50 > 1u?*(( int*) _temp50) == Cyc_Absyn_Rel_n:
0){ _LL59: _temp58=(( struct Cyc_Absyn_Rel_n_struct*) _temp50)->f1; if( _temp58
== 0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if((
unsigned int) _temp50 > 1u?*(( int*) _temp50) == Cyc_Absyn_Abs_n: 0){ _LL61:
_temp60=(( struct Cyc_Absyn_Abs_n_struct*) _temp50)->f1; if( _temp60 == 0){ goto
_LL55;} else{ goto _LL56;}} else{ goto _LL56;} _LL56: goto _LL57; _LL53: goto
_LL51; _LL55: goto _LL51; _LL57: Cyc_Tcutil_terr( loc,({ unsigned char* _temp62="qualified declarations are not implemented";
struct _tagged_string _temp63; _temp63.curr= _temp62; _temp63.base= _temp62;
_temp63.last_plus_one= _temp62 + 43; _temp63;})); return; _LL51:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp64=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp64[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp65; _temp65.tag= Cyc_Absyn_Abs_n; _temp65.f1= te->ns; _temp65;}); _temp64;});{
void* _temp66= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp73; struct
Cyc_Absyn_Tqual _temp75; void* _temp77; _LL68: if(( unsigned int) _temp66 > 4u?*((
int*) _temp66) == Cyc_Absyn_ArrayType: 0){ _LL78: _temp77=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp66)->f1; goto _LL76; _LL76: _temp75=(( struct Cyc_Absyn_ArrayType_struct*)
_temp66)->f2; goto _LL74; _LL74: _temp73=(( struct Cyc_Absyn_ArrayType_struct*)
_temp66)->f3; if( _temp73 == 0){ goto _LL72;} else{ goto _LL70;}} else{ goto
_LL70;} _LL72: if( vd->initializer != 0){ goto _LL69;} else{ goto _LL70;} _LL70:
goto _LL71; _LL69:{ void* _temp79=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp89; struct _tagged_string _temp91; struct Cyc_List_List*
_temp93; struct Cyc_Core_Opt* _temp95; struct Cyc_List_List* _temp97; _LL81: if(*((
int*) _temp79) == Cyc_Absyn_Const_e){ _LL90: _temp89=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp79)->f1; if(( unsigned int) _temp89 > 1u?*(( int*) _temp89) == Cyc_Absyn_String_c:
0){ _LL92: _temp91=(( struct Cyc_Absyn_String_c_struct*) _temp89)->f1; goto
_LL82;} else{ goto _LL83;}} else{ goto _LL83;} _LL83: if(*(( int*) _temp79) ==
Cyc_Absyn_UnresolvedMem_e){ _LL96: _temp95=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp79)->f1; goto _LL94; _LL94: _temp93=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp79)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp79) ==
Cyc_Absyn_Array_e){ _LL98: _temp97=(( struct Cyc_Absyn_Array_e_struct*) _temp79)->f1;
goto _LL86;} else{ goto _LL87;} _LL87: goto _LL88; _LL82: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp99=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp99[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp100; _temp100.tag= Cyc_Absyn_ArrayType; _temp100.f1=(
void*) _temp77; _temp100.f2= _temp75; _temp100.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(({
struct _tagged_string _temp101= _temp91;( unsigned int)( _temp101.last_plus_one
- _temp101.curr);}), 0); _temp100;}); _temp99;}))); goto _LL80; _LL84: _temp97=
_temp93; goto _LL86; _LL86: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp102=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp102[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp103; _temp103.tag= Cyc_Absyn_ArrayType;
_temp103.f1=( void*) _temp77; _temp103.f2= _temp75; _temp103.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp97), 0); _temp103;}); _temp102;}))); goto _LL80; _LL88: goto _LL80; _LL80:;}
goto _LL67; _LL71: goto _LL67; _LL67:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp104="extern declaration should not have initializer";
struct _tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 47; _temp105;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp106=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int
_temp126; struct _tagged_string _temp128; _LL108: if(( unsigned int) _temp106 >
16u?*(( int*) _temp106) == Cyc_Absyn_Aligned_att: 0){ _LL127: _temp126=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp106)->f1; goto _LL109;} else{ goto _LL110;}
_LL110: if(( unsigned int) _temp106 > 16u?*(( int*) _temp106) == Cyc_Absyn_Section_att:
0){ _LL129: _temp128=(( struct Cyc_Absyn_Section_att_struct*) _temp106)->f1;
goto _LL111;} else{ goto _LL112;} _LL112: if( _temp106 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp106 ==( void*) Cyc_Absyn_Shared_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp106 ==( void*) Cyc_Absyn_Unused_att){
goto _LL117;} else{ goto _LL118;} _LL118: if( _temp106 ==( void*) Cyc_Absyn_Weak_att){
goto _LL119;} else{ goto _LL120;} _LL120: if( _temp106 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL121;} else{ goto _LL122;} _LL122: if( _temp106 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL123;} else{ goto _LL124;} _LL124: goto _LL125; _LL109: continue; _LL111:
continue; _LL113: continue; _LL115: continue; _LL117: continue; _LL119:
continue; _LL121: continue; _LL123: continue; _LL125: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp130= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp131= Cyc_Absynpp_qvar2string(
vd->name); xprintf("bad attribute %.*s for variable %.*s", _temp130.last_plus_one
- _temp130.curr, _temp130.curr, _temp131.last_plus_one - _temp131.curr, _temp131.curr);}));
goto _LL107; _LL107:;} if( vd->initializer == 0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp132= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp133= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp132.last_plus_one - _temp132.curr, _temp132.curr, _temp133.last_plus_one -
_temp133.curr, _temp133.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp134=(
struct Cyc_Absyn_Exp*) _check_null( vd->initializer); goto _LL135; _LL135: {
void* _temp136= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp134); goto
_LL137; _LL137: if( ! Cyc_Tcutil_coerce_assign( te, _temp134, t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp138= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp139= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp140= Cyc_Absynpp_typ2string( _temp136); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp138.last_plus_one - _temp138.curr, _temp138.curr, _temp139.last_plus_one -
_temp139.curr, _temp139.curr, _temp140.last_plus_one - _temp140.curr, _temp140.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, _temp134)){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp141="initializer is not a constant expression"; struct
_tagged_string _temp142; _temp142.curr= _temp141; _temp142.base= _temp141;
_temp142.last_plus_one= _temp141 + 41; _temp142;}));}}}} else{ for( 0; atts != 0;
atts=(( struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp143=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp163; int _temp165;
_LL145: if(( unsigned int) _temp143 > 16u?*(( int*) _temp143) == Cyc_Absyn_Regparm_att:
0){ _LL164: _temp163=(( struct Cyc_Absyn_Regparm_att_struct*) _temp143)->f1;
goto _LL146;} else{ goto _LL147;} _LL147: if( _temp143 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL148;} else{ goto _LL149;} _LL149: if( _temp143 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL150;} else{ goto _LL151;} _LL151: if( _temp143 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL152;} else{ goto _LL153;} _LL153: if( _temp143 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL154;} else{ goto _LL155;} _LL155: if( _temp143 ==( void*) Cyc_Absyn_Const_att){
goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int) _temp143 > 16u?*((
int*) _temp143) == Cyc_Absyn_Aligned_att: 0){ _LL166: _temp165=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp143)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if( _temp143 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL160;} else{ goto _LL161;} _LL161: goto _LL162;
_LL146: goto _LL148; _LL148: goto _LL150; _LL150: goto _LL152; _LL152: goto
_LL154; _LL154: goto _LL156; _LL156:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
unsigned char* _temp167="tcVardecl: fn type attributes in function var decl";
struct _tagged_string _temp168; _temp168.curr= _temp167; _temp168.base= _temp167;
_temp168.last_plus_one= _temp167 + 51; _temp168;})); goto _LL144; _LL158: goto
_LL160; _LL160: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp169= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_temp169.last_plus_one - _temp169.curr, _temp169.curr);})); goto _LL144; _LL162:
continue; _LL144:;}}}{ struct _handler_cons _temp170; _push_handler(& _temp170);{
int _temp172= 0; if( setjmp( _temp170.handler)){ _temp172= 1;} if( ! _temp172){{
struct _tuple3* _temp173=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL174; _LL174: {
void* _temp175=(* _temp173).f1; void* _temp181; _LL177: if(*(( int*) _temp175)
== Cyc_Tcenv_VarRes){ _LL182: _temp181=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp175)->f1; goto _LL178;} else{ goto _LL179;} _LL179: goto _LL180; _LL178: {
struct Cyc_Absyn_Global_b_struct* _temp185=({ struct Cyc_Absyn_Global_b_struct*
_temp183=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp183[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp184; _temp184.tag= Cyc_Absyn_Global_b;
_temp184.f1= vd; _temp184;}); _temp183;}); goto _LL186; _LL186: { void* _temp187=
Cyc_Tcdecl_merge_binding( _temp181,( void*) _temp185, loc, Cyc_Tc_tc_msg); goto
_LL188; _LL188: if( _temp187 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL176;}
if( _temp187 == _temp181?(* _temp173).f2: 0){ goto _LL176;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp189=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp189->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp190=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp190[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp191; _temp191.tag= Cyc_Tcenv_VarRes; _temp191.f1=( void*) _temp187;
_temp191;}); _temp190;}); _temp189->f2= 1; _temp189;})); goto _LL176;}} _LL180:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp192="tcVardecl: a variable is being declared with a wrong name";
struct _tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 58; _temp193;})); goto _LL176; _LL176:;}};
_pop_handler();} else{ void* _temp171=( void*) _exn_thrown; void* _temp195=
_temp171; _LL197: if( _temp195 == Cyc_Dict_Absent){ goto _LL198;} else{ goto
_LL199;} _LL199: goto _LL200; _LL198: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp201=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp201->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp202=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp202[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp203; _temp203.tag= Cyc_Tcenv_VarRes;
_temp203.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp204=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp204[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp205; _temp205.tag= Cyc_Absyn_Global_b;
_temp205.f1= vd; _temp205;}); _temp204;})); _temp203;}); _temp202;}); _temp201->f2=
0; _temp201;})); goto _LL196; _LL200:( void) _throw( _temp195); _LL196:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp206=(*
fd->name).f1; struct Cyc_List_List* _temp214; struct Cyc_List_List* _temp216;
_LL208: if(( unsigned int) _temp206 > 1u?*(( int*) _temp206) == Cyc_Absyn_Rel_n:
0){ _LL215: _temp214=(( struct Cyc_Absyn_Rel_n_struct*) _temp206)->f1; if(
_temp214 == 0){ goto _LL209;} else{ goto _LL210;}} else{ goto _LL210;} _LL210:
if(( unsigned int) _temp206 > 1u?*(( int*) _temp206) == Cyc_Absyn_Abs_n: 0){
_LL217: _temp216=(( struct Cyc_Absyn_Abs_n_struct*) _temp206)->f1; goto _LL211;}
else{ goto _LL212;} _LL212: goto _LL213; _LL209: goto _LL207; _LL211:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp218=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp218[ 0]=({ struct
Cyc_Core_Impossible_struct _temp219; _temp219.tag= Cyc_Core_Impossible; _temp219.f1=({
unsigned char* _temp220="tc: Abs_n in tcFndecl"; struct _tagged_string _temp221;
_temp221.curr= _temp220; _temp221.base= _temp220; _temp221.last_plus_one=
_temp220 + 22; _temp221;}); _temp219;}); _temp218;})); _LL213: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp222="qualified declarations are not implemented";
struct _tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 43; _temp223;})); return; _LL207:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp224=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp224[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp225; _temp225.tag= Cyc_Absyn_Abs_n; _temp225.f1= te->ns; _temp225;});
_temp224;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp226=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp234; _LL228: if(
_temp226 ==( void*) Cyc_Absyn_Packed_att){ goto _LL229;} else{ goto _LL230;}
_LL230: if(( unsigned int) _temp226 > 16u?*(( int*) _temp226) == Cyc_Absyn_Aligned_att:
0){ _LL235: _temp234=(( struct Cyc_Absyn_Aligned_att_struct*) _temp226)->f1;
goto _LL231;} else{ goto _LL232;} _LL232: goto _LL233; _LL229: goto _LL231;
_LL231: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp236= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s for function",
_temp236.last_plus_one - _temp236.curr, _temp236.curr);})); goto _LL227; _LL233:
goto _LL227; _LL227:;}}{ struct _handler_cons _temp237; _push_handler(& _temp237);{
int _temp239= 0; if( setjmp( _temp237.handler)){ _temp239= 1;} if( ! _temp239){{
struct _tuple3* _temp240=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL241; _LL241: {
void* _temp242=(* _temp240).f1; void* _temp248; _LL244: if(*(( int*) _temp242)
== Cyc_Tcenv_VarRes){ _LL249: _temp248=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp242)->f1; goto _LL245;} else{ goto _LL246;} _LL246: goto _LL247; _LL245: {
struct Cyc_Absyn_Funname_b_struct* _temp252=({ struct Cyc_Absyn_Funname_b_struct*
_temp250=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp250[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp251; _temp251.tag= Cyc_Absyn_Funname_b;
_temp251.f1= fd; _temp251;}); _temp250;}); goto _LL253; _LL253: { void* _temp254=
Cyc_Tcdecl_merge_binding( _temp248,( void*) _temp252, loc, Cyc_Tc_tc_msg); goto
_LL255; _LL255: if( _temp254 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL243;}
if( _temp254 == _temp248?(* _temp240).f2: 0){ goto _LL243;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp256=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp256->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp257=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp257[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp258; _temp258.tag= Cyc_Tcenv_VarRes; _temp258.f1=( void*) _temp254;
_temp258;}); _temp257;}); _temp256->f2= 1; _temp256;})); goto _LL243;}} _LL247:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ unsigned char* _temp259="tcFndecl: a variable is being declared with a wrong name";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 57; _temp260;})); goto _LL243; _LL243:;}};
_pop_handler();} else{ void* _temp238=( void*) _exn_thrown; void* _temp262=
_temp238; _LL264: if( _temp262 == Cyc_Dict_Absent){ goto _LL265;} else{ goto
_LL266;} _LL266: goto _LL267; _LL265: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp268=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp268->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp269=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp269[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp270; _temp270.tag= Cyc_Tcenv_VarRes;
_temp270.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp271=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp271[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp272; _temp272.tag= Cyc_Absyn_Funname_b;
_temp272.f1= fd; _temp272;}); _temp271;})); _temp270;}); _temp269;}); _temp268->f2=
0; _temp268;})); goto _LL263; _LL267:( void) _throw( _temp262); _LL263:;}}} te->le=({
struct Cyc_Core_Opt* _temp273=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp273->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp274=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp274[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp275; _temp275.tag= Cyc_Tcenv_Outermost;
_temp275.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp275;}); _temp274;}));
_temp273;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp276="function has goto statements to undefined labels";
struct _tagged_string _temp277; _temp277.curr= _temp276; _temp277.base= _temp276;
_temp277.last_plus_one= _temp276 + 49; _temp277;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp278=(* td->name).f1; struct Cyc_List_List*
_temp286; struct Cyc_List_List* _temp288; _LL280: if(( unsigned int) _temp278 >
1u?*(( int*) _temp278) == Cyc_Absyn_Rel_n: 0){ _LL287: _temp286=(( struct Cyc_Absyn_Rel_n_struct*)
_temp278)->f1; if( _temp286 == 0){ goto _LL281;} else{ goto _LL282;}} else{ goto
_LL282;} _LL282: if(( unsigned int) _temp278 > 1u?*(( int*) _temp278) == Cyc_Absyn_Abs_n:
0){ _LL289: _temp288=(( struct Cyc_Absyn_Abs_n_struct*) _temp278)->f1; if(
_temp288 == 0){ goto _LL283;} else{ goto _LL284;}} else{ goto _LL284;} _LL284:
goto _LL285; _LL281: goto _LL279; _LL283: goto _LL279; _LL285: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp290="qualified declarations are not implemented";
struct _tagged_string _temp291; _temp291.curr= _temp290; _temp291.base= _temp290;
_temp291.last_plus_one= _temp290 + 43; _temp291;})); return; _LL279:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp292=* v; xprintf("redeclaration of typedef %.*s",
_temp292.last_plus_one - _temp292.curr, _temp292.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp293=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp293[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp294; _temp294.tag= Cyc_Absyn_Abs_n; _temp294.f1= te->ns; _temp294;});
_temp293;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp295= _new_region();
struct _RegionHandle* uprev_rgn=& _temp295; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp296= fields; goto
_LL297; _LL297: for( 0; _temp296 != 0; _temp296=(( struct Cyc_List_List*)
_check_null( _temp296))->tl){ struct Cyc_Absyn_Structfield _temp300; struct Cyc_List_List*
_temp301; struct Cyc_Core_Opt* _temp303; void* _temp305; struct Cyc_Absyn_Tqual
_temp307; struct _tagged_string* _temp309; struct Cyc_Absyn_Structfield*
_temp298=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp296))->hd; _temp300=* _temp298; _LL310: _temp309= _temp300.name; goto
_LL308; _LL308: _temp307= _temp300.tq; goto _LL306; _LL306: _temp305=( void*)
_temp300.type; goto _LL304; _LL304: _temp303= _temp300.width; goto _LL302;
_LL302: _temp301= _temp300.attributes; goto _LL299; _LL299: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp309)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp311=* _temp309;
struct _tagged_string _temp312= obj; xprintf("duplicate field %.*s in %.*s",
_temp311.last_plus_one - _temp311.curr, _temp311.curr, _temp312.last_plus_one -
_temp312.curr, _temp312.curr);}));} if( Cyc_String_strcmp(* _temp309,({
unsigned char* _temp313=""; struct _tagged_string _temp314; _temp314.curr=
_temp313; _temp314.base= _temp313; _temp314.last_plus_one= _temp313 + 1;
_temp314;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp315=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp315->hd=( void*)
_temp309; _temp315->tl= prev_fields; _temp315;});} Cyc_Tcutil_check_type( loc,
te, tvs,( void*) Cyc_Absyn_MemKind, _temp305); if( _temp303 != 0){ unsigned int
w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp303))->v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp316=* _temp309; xprintf("bitfield %.*s does not have constant width",
_temp316.last_plus_one - _temp316.curr, _temp316.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp303))->v);}{
void* _temp317= Cyc_Tcutil_compress( _temp305); void* _temp323; void* _temp325;
_LL319: if(( unsigned int) _temp317 > 4u?*(( int*) _temp317) == Cyc_Absyn_IntType:
0){ _LL326: _temp325=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp317)->f1;
goto _LL324; _LL324: _temp323=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp317)->f2; goto _LL320;} else{ goto _LL321;} _LL321: goto _LL322; _LL320:{
void* _temp327= _temp323; _LL329: if( _temp327 ==( void*) Cyc_Absyn_B1){ goto
_LL330;} else{ goto _LL331;} _LL331: if( _temp327 ==( void*) Cyc_Absyn_B2){ goto
_LL332;} else{ goto _LL333;} _LL333: if( _temp327 ==( void*) Cyc_Absyn_B4){ goto
_LL334;} else{ goto _LL335;} _LL335: if( _temp327 ==( void*) Cyc_Absyn_B8){ goto
_LL336;} else{ goto _LL328;} _LL330: if( w > 8){ Cyc_Tcutil_terr( loc,({
unsigned char* _temp337="bitfield larger than type"; struct _tagged_string
_temp338; _temp338.curr= _temp337; _temp338.base= _temp337; _temp338.last_plus_one=
_temp337 + 26; _temp338;}));} goto _LL328; _LL332: if( w > 16){ Cyc_Tcutil_terr(
loc,({ unsigned char* _temp339="bitfield larger than type"; struct
_tagged_string _temp340; _temp340.curr= _temp339; _temp340.base= _temp339;
_temp340.last_plus_one= _temp339 + 26; _temp340;}));} goto _LL328; _LL334: if( w
> 32){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp341="bitfield larger than type";
struct _tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 26; _temp342;}));} goto _LL328; _LL336: if( w
> 64){ Cyc_Tcutil_terr( loc,({ unsigned char* _temp343="bitfield larger than type";
struct _tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 26; _temp344;}));} goto _LL328; _LL328:;}
goto _LL318; _LL322: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp345=*
_temp309; struct _tagged_string _temp346= Cyc_Absynpp_typ2string( _temp305);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp345.last_plus_one
- _temp345.curr, _temp345.curr, _temp346.last_plus_one - _temp346.curr, _temp346.curr);}));
goto _LL318; _LL318:;}}}}; _pop_region(& _temp295);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* _temp347=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f2; goto _LL348; _LL348: if( sd->name == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp349= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp349.last_plus_one - _temp349.curr, _temp349.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp350=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp358; _LL352: if( _temp350 ==( void*) Cyc_Absyn_Packed_att){
goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int) _temp350 > 16u?*((
int*) _temp350) == Cyc_Absyn_Aligned_att: 0){ _LL359: _temp358=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp350)->f1; goto _LL355;} else{ goto _LL356;} _LL356: goto _LL357; _LL353:
continue; _LL355: continue; _LL357: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp360= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp361= obj; struct
_tagged_string _temp362=* _temp347; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp360.last_plus_one - _temp360.curr, _temp360.curr, _temp361.last_plus_one -
_temp361.curr, _temp361.curr, _temp362.last_plus_one - _temp362.curr, _temp362.curr);}));
goto _LL351; _LL351:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp363=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
goto _LL364; _LL364: { void* _temp365=( void*) _temp363->v; void* _temp373;
_LL367: if( _temp365 ==( void*) Cyc_Absyn_No_constr){ goto _LL368;} else{ goto
_LL369;} _LL369: if(( unsigned int) _temp365 > 1u?*(( int*) _temp365) == Cyc_Absyn_Eq_constr:
0){ _LL374: _temp373=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp365)->f1;
if( _temp373 ==( void*) Cyc_Absyn_MemKind){ goto _LL370;} else{ goto _LL371;}}
else{ goto _LL371;} _LL371: goto _LL372; _LL368:( void*)( _temp363->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp375=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp375[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp376; _temp376.tag= Cyc_Absyn_Eq_constr; _temp376.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp376;}); _temp375;}))); continue; _LL370:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp377= obj; struct
_tagged_string _temp378=* _temp347; struct _tagged_string _temp379=*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp377.last_plus_one - _temp377.curr, _temp377.curr, _temp378.last_plus_one -
_temp378.curr, _temp378.curr, _temp379.last_plus_one - _temp379.curr, _temp379.curr);}));
continue; _LL372: continue; _LL366:;}}}{ void* _temp380=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp388; struct Cyc_List_List* _temp390; _LL382: if(( unsigned int) _temp380 >
1u?*(( int*) _temp380) == Cyc_Absyn_Rel_n: 0){ _LL389: _temp388=(( struct Cyc_Absyn_Rel_n_struct*)
_temp380)->f1; if( _temp388 == 0){ goto _LL383;} else{ goto _LL384;}} else{ goto
_LL384;} _LL384: if(( unsigned int) _temp380 > 1u?*(( int*) _temp380) == Cyc_Absyn_Abs_n:
0){ _LL391: _temp390=(( struct Cyc_Absyn_Abs_n_struct*) _temp380)->f1; if(
_temp390 == 0){ goto _LL385;} else{ goto _LL386;}} else{ goto _LL386;} _LL386:
goto _LL387; _LL383: goto _LL381; _LL385: goto _LL381; _LL387: Cyc_Tcutil_terr(
loc,({ unsigned char* _temp392="qualified declarations are not implemented";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 43; _temp393;})); return; _LL381:;}(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp394=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp394[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp395; _temp395.tag= Cyc_Absyn_Abs_n; _temp395.f1= te->ns; _temp395;});
_temp394;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs);} struct _tuple4{
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp396=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL397; _LL397: { struct _tagged_string obj=({ unsigned char* _temp452="struct";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 7; _temp453;}); Cyc_Tc_tcStructOrUniondecl(
te, ge, obj, loc, sd);{ struct Cyc_List_List* _temp398= sd->tvs; goto _LL399;
_LL399:{ struct _tuple4 _temp401=({ struct _tuple4 _temp400; _temp400.f1= sd->fields;
_temp400.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp396); _temp400;});
struct Cyc_Core_Opt* _temp411; struct Cyc_Core_Opt* _temp413; struct Cyc_Core_Opt*
_temp415; struct Cyc_Core_Opt* _temp417; struct Cyc_Core_Opt _temp419; struct
Cyc_List_List* _temp420; struct Cyc_Core_Opt* _temp422; struct Cyc_Core_Opt
_temp424; struct Cyc_Absyn_Structdecl** _temp425; struct Cyc_Core_Opt* _temp427;
struct Cyc_Core_Opt _temp429; struct Cyc_List_List* _temp430; struct Cyc_Core_Opt*
_temp432; struct Cyc_Core_Opt _temp434; struct Cyc_Absyn_Structdecl** _temp435;
struct Cyc_Core_Opt* _temp437; _LL403: _LL414: _temp413= _temp401.f1; if(
_temp413 == 0){ goto _LL412;} else{ goto _LL405;} _LL412: _temp411= _temp401.f2;
if( _temp411 == 0){ goto _LL404;} else{ goto _LL405;} _LL405: _LL418: _temp417=
_temp401.f1; if( _temp417 == 0){ goto _LL407;} else{ _temp419=* _temp417; _LL421:
_temp420=( struct Cyc_List_List*) _temp419.v; goto _LL416;} _LL416: _temp415=
_temp401.f2; if( _temp415 == 0){ goto _LL406;} else{ goto _LL407;} _LL407:
_LL428: _temp427= _temp401.f1; if( _temp427 == 0){ goto _LL409;} else{ _temp429=*
_temp427; _LL431: _temp430=( struct Cyc_List_List*) _temp429.v; goto _LL423;}
_LL423: _temp422= _temp401.f2; if( _temp422 == 0){ goto _LL409;} else{ _temp424=*
_temp422; _LL426: _temp425=( struct Cyc_Absyn_Structdecl**) _temp424.v; goto
_LL408;} _LL409: _LL438: _temp437= _temp401.f1; if( _temp437 == 0){ goto _LL433;}
else{ goto _LL402;} _LL433: _temp432= _temp401.f2; if( _temp432 == 0){ goto
_LL402;} else{ _temp434=* _temp432; _LL436: _temp435=( struct Cyc_Absyn_Structdecl**)
_temp434.v; goto _LL410;} _LL404: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp396,({ struct Cyc_Absyn_Structdecl**
_temp439=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp439[ 0]= sd; _temp439;})); goto _LL402; _LL406: { struct Cyc_Absyn_Structdecl**
_temp442=({ struct Cyc_Absyn_Structdecl** _temp440=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp440[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp441=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp441->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp441->name= sd->name;
_temp441->tvs= _temp398; _temp441->fields= 0; _temp441->attributes= 0; _temp441;});
_temp440;}); goto _LL443; _LL443: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp396, _temp442); Cyc_Tc_tcStructFields(
te, ge, loc, obj, _temp420, _temp398);* _temp442= sd; goto _LL402;} _LL408: {
struct Cyc_Absyn_Structdecl* _temp444=* _temp425; goto _LL445; _LL445:* _temp425=({
struct Cyc_Absyn_Structdecl* _temp446=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp446->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp446->name= sd->name; _temp446->tvs= _temp398; _temp446->fields= 0; _temp446->attributes=
0; _temp446;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp430, _temp398);*
_temp425= _temp444; _temp435= _temp425; goto _LL410;} _LL410: { struct Cyc_Absyn_Structdecl*
_temp447= Cyc_Tcdecl_merge_structdecl(* _temp435, sd, loc, Cyc_Tc_tc_msg); goto
_LL448; _LL448: if( _temp447 == 0){ return;} else{* _temp435=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp447); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp447);
goto _LL402;}} _LL402:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp396,({ struct _tuple3* _temp449=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp449->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp450=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp450[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp451; _temp451.tag= Cyc_Tcenv_StructRes;
_temp451.f1= sd; _temp451;}); _temp450;}); _temp449->f2= 1; _temp449;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* _temp454=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)).f2; goto _LL455; _LL455: { struct _tagged_string
obj=({ unsigned char* _temp507="union"; struct _tagged_string _temp508; _temp508.curr=
_temp507; _temp508.base= _temp507; _temp508.last_plus_one= _temp507 + 6;
_temp508;}); Cyc_Tc_tcStructOrUniondecl( te, ge, obj, loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp456= ud->tvs; goto _LL457; _LL457: { struct
_tuple4 _temp459=({ struct _tuple4 _temp458; _temp458.f1= ud->fields; _temp458.f2=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->uniondecls, _temp454); _temp458;}); struct Cyc_Core_Opt*
_temp469; struct Cyc_Core_Opt* _temp471; struct Cyc_Core_Opt* _temp473; struct
Cyc_Core_Opt* _temp475; struct Cyc_Core_Opt _temp477; struct Cyc_List_List*
_temp478; struct Cyc_Core_Opt* _temp480; struct Cyc_Core_Opt _temp482; struct
Cyc_Absyn_Uniondecl** _temp483; struct Cyc_Core_Opt* _temp485; struct Cyc_Core_Opt
_temp487; struct Cyc_List_List* _temp488; struct Cyc_Core_Opt* _temp490; struct
Cyc_Core_Opt _temp492; struct Cyc_Absyn_Uniondecl** _temp493; struct Cyc_Core_Opt*
_temp495; _LL461: _LL472: _temp471= _temp459.f1; if( _temp471 == 0){ goto _LL470;}
else{ goto _LL463;} _LL470: _temp469= _temp459.f2; if( _temp469 == 0){ goto
_LL462;} else{ goto _LL463;} _LL463: _LL476: _temp475= _temp459.f1; if( _temp475
== 0){ goto _LL465;} else{ _temp477=* _temp475; _LL479: _temp478=( struct Cyc_List_List*)
_temp477.v; goto _LL474;} _LL474: _temp473= _temp459.f2; if( _temp473 == 0){
goto _LL464;} else{ goto _LL465;} _LL465: _LL486: _temp485= _temp459.f1; if(
_temp485 == 0){ goto _LL467;} else{ _temp487=* _temp485; _LL489: _temp488=(
struct Cyc_List_List*) _temp487.v; goto _LL481;} _LL481: _temp480= _temp459.f2;
if( _temp480 == 0){ goto _LL467;} else{ _temp482=* _temp480; _LL484: _temp483=(
struct Cyc_Absyn_Uniondecl**) _temp482.v; goto _LL466;} _LL467: _LL496: _temp495=
_temp459.f1; if( _temp495 == 0){ goto _LL491;} else{ goto _LL460;} _LL491:
_temp490= _temp459.f2; if( _temp490 == 0){ goto _LL460;} else{ _temp492=*
_temp490; _LL494: _temp493=( struct Cyc_Absyn_Uniondecl**) _temp492.v; goto
_LL468;} _LL462: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp454,({ struct Cyc_Absyn_Uniondecl** _temp497=( struct Cyc_Absyn_Uniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*)); _temp497[ 0]= ud; _temp497;}));
goto _LL460; _LL464: { struct Cyc_Absyn_Uniondecl** _temp500=({ struct Cyc_Absyn_Uniondecl**
_temp498=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp498[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp499=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp499->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp499->name= ud->name; _temp499->tvs= _temp456; _temp499->fields=
0; _temp499->attributes= ud->attributes; _temp499;}); _temp498;}); goto _LL501;
_LL501: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp454, _temp500); Cyc_Tc_tcStructFields( te, ge, loc, obj,
_temp478, _temp456);* _temp500= ud; goto _LL460;} _LL466: { struct Cyc_Absyn_Uniondecl*
_temp502=* _temp483; goto _LL503; _LL503:* _temp483=({ struct Cyc_Absyn_Uniondecl*
_temp504=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp504->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp504->name= ud->name;
_temp504->tvs= _temp456; _temp504->fields= 0; _temp504->attributes= ud->attributes;
_temp504;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp488, _temp456);*
_temp483= _temp502; _temp493= _temp483; goto _LL468;} _LL468: { struct Cyc_Absyn_Uniondecl*
_temp505= Cyc_Tcdecl_merge_uniondecl(* _temp493, ud, loc, Cyc_Tc_tc_msg); goto
_LL506; _LL506: if( _temp505 == 0){ return;} else{* _temp493=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp505); goto _LL460;}} _LL460:;}}}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp509= fields; goto _LL510; _LL510: for( 0; _temp509 != 0;
_temp509=(( struct Cyc_List_List*) _check_null( _temp509))->tl){ struct Cyc_Absyn_Tunionfield*
_temp511=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp509))->hd; goto _LL512; _LL512:{ struct Cyc_List_List* tvs= _temp511->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Tvar* _temp513=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd; goto _LL514; _LL514: { struct Cyc_Absyn_Conref* _temp515=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp513->kind); goto _LL516; _LL516: { void* _temp517=( void*) _temp515->v;
void* _temp525; _LL519: if( _temp517 ==( void*) Cyc_Absyn_No_constr){ goto
_LL520;} else{ goto _LL521;} _LL521: if(( unsigned int) _temp517 > 1u?*(( int*)
_temp517) == Cyc_Absyn_Eq_constr: 0){ _LL526: _temp525=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp517)->f1; if( _temp525 ==( void*) Cyc_Absyn_MemKind){ goto _LL522;} else{
goto _LL523;}} else{ goto _LL523;} _LL523: goto _LL524; _LL520:( void*)(
_temp515->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp527=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp527[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp528; _temp528.tag= Cyc_Absyn_Eq_constr;
_temp528.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp528;}); _temp527;})));
goto _LL518; _LL522: Cyc_Tcutil_terr( _temp511->loc,({ struct _tagged_string
_temp529=*(* _temp511->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp529.last_plus_one - _temp529.curr, _temp529.curr);})); goto _LL518; _LL524:
goto _LL518; _LL518:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp511->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List*
typs= _temp511->typs; for( 0; typs != 0; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl){ Cyc_Tcutil_check_type( _temp511->loc, te, alltvs,(
void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( typs))->hd)).f2);}}{ void* _temp530=(* _temp511->name).f1; struct
Cyc_List_List* _temp540; struct Cyc_List_List* _temp542; struct Cyc_List_List*
_temp544; _LL532: if(( unsigned int) _temp530 > 1u?*(( int*) _temp530) == Cyc_Absyn_Rel_n:
0){ _LL541: _temp540=(( struct Cyc_Absyn_Rel_n_struct*) _temp530)->f1; if(
_temp540 == 0){ goto _LL533;} else{ goto _LL534;}} else{ goto _LL534;} _LL534:
if(( unsigned int) _temp530 > 1u?*(( int*) _temp530) == Cyc_Absyn_Rel_n: 0){
_LL543: _temp542=(( struct Cyc_Absyn_Rel_n_struct*) _temp530)->f1; goto _LL535;}
else{ goto _LL536;} _LL536: if(( unsigned int) _temp530 > 1u?*(( int*) _temp530)
== Cyc_Absyn_Abs_n: 0){ _LL545: _temp544=(( struct Cyc_Absyn_Abs_n_struct*)
_temp530)->f1; goto _LL537;} else{ goto _LL538;} _LL538: if( _temp530 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL539;} else{ goto _LL531;} _LL533: if( is_xtunion){(*
_temp511->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp546=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp546[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp547; _temp547.tag= Cyc_Absyn_Abs_n;
_temp547.f1= te->ns; _temp547;}); _temp546;});} else{(* _temp511->name).f1=(*
name).f1;} goto _LL531; _LL535: Cyc_Tcutil_terr( _temp511->loc,({ unsigned char*
_temp548="qualified declarations are not allowed"; struct _tagged_string
_temp549; _temp549.curr= _temp548; _temp549.base= _temp548; _temp549.last_plus_one=
_temp548 + 39; _temp549;})); goto _LL531; _LL537: goto _LL531; _LL539:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp550=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp550[ 0]=({ struct
Cyc_Core_Impossible_struct _temp551; _temp551.tag= Cyc_Core_Impossible; _temp551.f1=({
unsigned char* _temp552="tcTunionFields: Loc_n"; struct _tagged_string _temp553;
_temp553.curr= _temp552; _temp553.base= _temp552; _temp553.last_plus_one=
_temp552 + 22; _temp553;}); _temp551;}); _temp550;})); goto _LL531; _LL531:;}}}}{
struct Cyc_List_List* fields2; if( is_xtunion){ int _temp554= 1; goto _LL555;
_LL555: { struct Cyc_List_List* _temp556= Cyc_Tcdecl_sort_xtunion_fields( fields,&
_temp554,(* name).f2, loc, Cyc_Tc_tc_msg); goto _LL557; _LL557: if( _temp554){
fields2= _temp556;} else{ fields2= 0;}}} else{ struct _RegionHandle _temp558=
_new_region(); struct _RegionHandle* uprev_rgn=& _temp558; _push_region(
uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=
fields; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct Cyc_Absyn_Tunionfield* _temp559=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( fs))->hd; goto _LL560; _LL560: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
_temp559->name).f2)){ Cyc_Tcutil_terr( _temp559->loc,({ struct _tagged_string
_temp561=*(* _temp559->name).f2; struct _tagged_string _temp562= obj; xprintf("duplicate field name %.*s in %.*s",
_temp561.last_plus_one - _temp561.curr, _temp561.curr, _temp562.last_plus_one -
_temp562.curr, _temp562.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp563=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp563->hd=( void*)(* _temp559->name).f2; _temp563->tl= prev_fields; _temp563;});}
if(( void*) _temp559->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp564=*(* _temp559->name).f2; xprintf("ignoring scope of field %.*s",
_temp564.last_plus_one - _temp564.curr, _temp564.curr);}));( void*)( _temp559->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp558);}{
struct Cyc_List_List* _temp565= fields; goto _LL566; _LL566: for( 0; _temp565 !=
0; _temp565=(( struct Cyc_List_List*) _check_null( _temp565))->tl){ struct Cyc_Absyn_Tunionfield*
_temp567=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp565))->hd; goto _LL568; _LL568: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp567->name).f2,({ struct _tuple3* _temp569=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp569->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp570=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp570[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp571; _temp571.tag= Cyc_Tcenv_TunionRes; _temp571.f1=
tudres; _temp571.f2= _temp567; _temp571;}); _temp570;}); _temp569->f2= 1;
_temp569;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=({ unsigned char* _temp572="xtunion"; struct
_tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 8; _temp573;});} else{ obj=({ unsigned char*
_temp574="tunion"; struct _tagged_string _temp575; _temp575.curr= _temp574;
_temp575.base= _temp574; _temp575.last_plus_one= _temp574 + 7; _temp575;});}{
struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0;
tvs2 != 0; tvs2=(( struct Cyc_List_List*) _check_null( tvs2))->tl){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind);
void* _temp576=( void*) c->v; void* _temp584; _LL578: if( _temp576 ==( void*)
Cyc_Absyn_No_constr){ goto _LL579;} else{ goto _LL580;} _LL580: if((
unsigned int) _temp576 > 1u?*(( int*) _temp576) == Cyc_Absyn_Eq_constr: 0){
_LL585: _temp584=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp576)->f1;
if( _temp584 ==( void*) Cyc_Absyn_MemKind){ goto _LL581;} else{ goto _LL582;}}
else{ goto _LL582;} _LL582: goto _LL583; _LL579:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp586=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp586[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp587; _temp587.tag= Cyc_Absyn_Eq_constr; _temp587.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp587;}); _temp586;}))); goto _LL577;
_LL581: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp588= obj; struct
_tagged_string _temp589=* v; struct _tagged_string _temp590=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp588.last_plus_one - _temp588.curr, _temp588.curr, _temp589.last_plus_one -
_temp589.curr, _temp589.curr, _temp590.last_plus_one - _temp590.curr, _temp590.curr);}));
goto _LL577; _LL583: goto _LL577; _LL577:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp591; _push_handler(& _temp591);{ int _temp593= 0; if( setjmp( _temp591.handler)){
_temp593= 1;} if( ! _temp593){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp592=( void*) _exn_thrown; void*
_temp595= _temp592; _LL597: if( _temp595 == Cyc_Dict_Absent){ goto _LL598;}
else{ goto _LL599;} _LL599: goto _LL600; _LL598: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp601= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp601.last_plus_one - _temp601.curr, _temp601.curr);})); return; _LL600:(
void) _throw( _temp595); _LL596:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp602=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp602[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp603; _temp603.tag= Cyc_Absyn_Abs_n;
_temp603.f1= te->ns; _temp603;}); _temp602;});}} else{{ void* _temp604=(* tud->name).f1;
struct Cyc_List_List* _temp612; struct Cyc_List_List* _temp614; _LL606: if((
unsigned int) _temp604 > 1u?*(( int*) _temp604) == Cyc_Absyn_Rel_n: 0){ _LL613:
_temp612=(( struct Cyc_Absyn_Rel_n_struct*) _temp604)->f1; if( _temp612 == 0){
goto _LL607;} else{ goto _LL608;}} else{ goto _LL608;} _LL608: if(( unsigned int)
_temp604 > 1u?*(( int*) _temp604) == Cyc_Absyn_Abs_n: 0){ _LL615: _temp614=((
struct Cyc_Absyn_Abs_n_struct*) _temp604)->f1; goto _LL609;} else{ goto _LL610;}
_LL610: goto _LL611; _LL607:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp616=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp616[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp617; _temp617.tag= Cyc_Absyn_Abs_n;
_temp617.f1= te->ns; _temp617;}); _temp616;}); goto _LL605; _LL609: goto _LL611;
_LL611: Cyc_Tcutil_terr( loc,({ unsigned char* _temp618="qualified declarations are not implemented";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 43; _temp619;})); return; _LL605:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp621=({ struct
_tuple4 _temp620; _temp620.f1= tud->fields; _temp620.f2= tud_opt; _temp620;});
struct Cyc_Core_Opt* _temp631; struct Cyc_Core_Opt* _temp633; struct Cyc_Core_Opt*
_temp635; struct Cyc_Core_Opt* _temp637; struct Cyc_Core_Opt _temp639; struct
Cyc_List_List* _temp640; struct Cyc_List_List** _temp642; struct Cyc_Core_Opt*
_temp643; struct Cyc_Core_Opt _temp645; struct Cyc_Absyn_Tuniondecl** _temp646;
struct Cyc_Core_Opt* _temp648; struct Cyc_Core_Opt _temp650; struct Cyc_List_List*
_temp651; struct Cyc_List_List** _temp653; struct Cyc_Core_Opt* _temp654; struct
Cyc_Core_Opt _temp656; struct Cyc_Absyn_Tuniondecl** _temp657; struct Cyc_Core_Opt*
_temp659; _LL623: _LL634: _temp633= _temp621.f1; if( _temp633 == 0){ goto _LL632;}
else{ goto _LL625;} _LL632: _temp631= _temp621.f2; if( _temp631 == 0){ goto
_LL624;} else{ goto _LL625;} _LL625: _LL638: _temp637= _temp621.f1; if( _temp637
== 0){ goto _LL627;} else{ _temp639=* _temp637; _LL641: _temp640=( struct Cyc_List_List*)
_temp639.v; _temp642=&(* _temp621.f1).v; goto _LL636;} _LL636: _temp635=
_temp621.f2; if( _temp635 == 0){ goto _LL626;} else{ goto _LL627;} _LL627:
_LL649: _temp648= _temp621.f1; if( _temp648 == 0){ goto _LL629;} else{ _temp650=*
_temp648; _LL652: _temp651=( struct Cyc_List_List*) _temp650.v; _temp653=&(*
_temp621.f1).v; goto _LL644;} _LL644: _temp643= _temp621.f2; if( _temp643 == 0){
goto _LL629;} else{ _temp645=* _temp643; _LL647: _temp646=( struct Cyc_Absyn_Tuniondecl**)
_temp645.v; goto _LL628;} _LL629: _LL660: _temp659= _temp621.f1; if( _temp659 ==
0){ goto _LL655;} else{ goto _LL622;} _LL655: _temp654= _temp621.f2; if(
_temp654 == 0){ goto _LL622;} else{ _temp656=* _temp654; _LL658: _temp657=(
struct Cyc_Absyn_Tuniondecl**) _temp656.v; goto _LL630;} _LL624: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp661=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp661[ 0]= tud; _temp661;}));
goto _LL622; _LL626: { struct Cyc_Absyn_Tuniondecl** _temp664=({ struct Cyc_Absyn_Tuniondecl**
_temp662=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp662[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp663=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp663->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp663->name= tud->name; _temp663->tvs= tvs; _temp663->fields=
0; _temp663->is_xtunion= tud->is_xtunion; _temp663;}); _temp662;}); goto _LL665;
_LL665: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp664);* _temp642= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp642, tvs, tud);* _temp664= tud; goto
_LL622;} _LL628: { struct Cyc_Absyn_Tuniondecl* _temp666=* _temp646; goto _LL667;
_LL667:* _temp646=({ struct Cyc_Absyn_Tuniondecl* _temp668=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp668->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp668->name= tud->name; _temp668->tvs= tvs; _temp668->fields=
0; _temp668->is_xtunion= tud->is_xtunion; _temp668;});* _temp653= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp653, tvs, tud);* _temp646=
_temp666; _temp657= _temp646; goto _LL630;} _LL630: { struct Cyc_Absyn_Tuniondecl*
_temp669= Cyc_Tcdecl_merge_tuniondecl(* _temp657, tud, loc, Cyc_Tc_tc_msg); goto
_LL670; _LL670: if( _temp669 == 0){ return;} else{* _temp657=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp669); goto _LL622;}} _LL622:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp671=(* ed->name).f1; struct Cyc_List_List* _temp679; struct Cyc_List_List*
_temp681; _LL673: if(( unsigned int) _temp671 > 1u?*(( int*) _temp671) == Cyc_Absyn_Rel_n:
0){ _LL680: _temp679=(( struct Cyc_Absyn_Rel_n_struct*) _temp671)->f1; if(
_temp679 == 0){ goto _LL674;} else{ goto _LL675;}} else{ goto _LL675;} _LL675:
if(( unsigned int) _temp671 > 1u?*(( int*) _temp671) == Cyc_Absyn_Abs_n: 0){
_LL682: _temp681=(( struct Cyc_Absyn_Abs_n_struct*) _temp671)->f1; if( _temp681
== 0){ goto _LL676;} else{ goto _LL677;}} else{ goto _LL677;} _LL677: goto
_LL678; _LL674: goto _LL672; _LL676: goto _LL672; _LL678: Cyc_Tcutil_terr( loc,({
unsigned char* _temp683="qualified declarations are not implemented"; struct
_tagged_string _temp684; _temp684.curr= _temp683; _temp684.base= _temp683;
_temp684.last_plus_one= _temp683 + 43; _temp684;})); return; _LL672:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp685=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp685[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp686; _temp686.tag= Cyc_Absyn_Abs_n; _temp686.f1= te->ns; _temp686;});
_temp685;}); if( ed->fields != 0){ struct _RegionHandle _temp687= _new_region();
struct _RegionHandle* uprev_rgn=& _temp687; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp688=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL689; _LL689: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp688->name).f2)){ Cyc_Tcutil_terr(
_temp688->loc,({ struct _tagged_string _temp690=*(* _temp688->name).f2; xprintf("duplicate field name %.*s",
_temp690.last_plus_one - _temp690.curr, _temp690.curr);}));} else{ prev_fields=({
struct Cyc_List_List* _temp691=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp691->hd=( void*)(* _temp688->name).f2;
_temp691->tl= prev_fields; _temp691;});} if( _temp688->tag == 0){ _temp688->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp688->loc);} else{ if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp688->tag))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp692=* v; struct
_tagged_string _temp693=*(* _temp688->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_temp692.last_plus_one - _temp692.curr, _temp692.curr, _temp693.last_plus_one -
_temp693.curr, _temp693.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp688->tag)); tag_count= t1 + 1;(*
_temp688->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp694=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp694[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp695; _temp695.tag= Cyc_Absyn_Abs_n;
_temp695.f1= te->ns; _temp695;}); _temp694;});}}}; _pop_region(& _temp687);}{
struct _handler_cons _temp696; _push_handler(& _temp696);{ int _temp698= 0; if(
setjmp( _temp696.handler)){ _temp698= 1;} if( ! _temp698){{ struct Cyc_Absyn_Enumdecl**
_temp699=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v); goto _LL700; _LL700: {
struct Cyc_Absyn_Enumdecl* _temp701= Cyc_Tcdecl_merge_enumdecl(* _temp699, ed,
loc, Cyc_Tc_tc_msg); goto _LL702; _LL702: if( _temp701 == 0){ _npop_handler( 0u);
return;}* _temp699=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp701);}};
_pop_handler();} else{ void* _temp697=( void*) _exn_thrown; void* _temp704=
_temp697; _LL706: if( _temp704 == Cyc_Dict_Absent){ goto _LL707;} else{ goto
_LL708;} _LL708: goto _LL709; _LL707: { struct Cyc_Absyn_Enumdecl** _temp711=({
struct Cyc_Absyn_Enumdecl** _temp710=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp710[ 0]= ed; _temp710;}); goto _LL712;
_LL712: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v, _temp711); goto _LL705;} _LL709:( void) _throw( _temp704);
_LL705:;}}} if( ed->fields != 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp713=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL714; _LL714: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp713->name).f2,({ struct _tuple3* _temp715=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp715->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp716=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp716[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp717; _temp717.tag= Cyc_Tcenv_EnumRes;
_temp717.f1= ed; _temp717.f2= _temp713; _temp717;}); _temp716;}); _temp715->f2=
1; _temp715;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp718= sc; _LL720: if( _temp718 ==( void*) Cyc_Absyn_Static){
goto _LL721;} else{ goto _LL722;} _LL722: if( _temp718 ==( void*) Cyc_Absyn_Abstract){
goto _LL723;} else{ goto _LL724;} _LL724: if( _temp718 ==( void*) Cyc_Absyn_Public){
goto _LL725;} else{ goto _LL726;} _LL726: if( _temp718 ==( void*) Cyc_Absyn_Extern){
goto _LL727;} else{ goto _LL728;} _LL728: if( _temp718 ==( void*) Cyc_Absyn_ExternC){
goto _LL729;} else{ goto _LL719;} _LL721: Cyc_Tcutil_warn( loc,({ unsigned char*
_temp730="static declaration nested within extern \"C\""; struct _tagged_string
_temp731; _temp731.curr= _temp730; _temp731.base= _temp730; _temp731.last_plus_one=
_temp730 + 44; _temp731;})); return 0; _LL723: Cyc_Tcutil_warn( loc,({
unsigned char* _temp732="abstract declaration nested within extern \"C\"";
struct _tagged_string _temp733; _temp733.curr= _temp732; _temp733.base= _temp732;
_temp733.last_plus_one= _temp732 + 46; _temp733;})); return 0; _LL725: return 1;
_LL727: return 1; _LL729: Cyc_Tcutil_warn( loc,({ unsigned char* _temp734="nested extern \"C\" declaration";
struct _tagged_string _temp735; _temp735.curr= _temp734; _temp735.base= _temp734;
_temp735.last_plus_one= _temp734 + 30; _temp735;})); return 1; _LL719:;} static
void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int
in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* _temp736= ds0; goto _LL737; _LL737: for( 0;
_temp736 != 0; _temp736=(( struct Cyc_List_List*) _check_null( _temp736))->tl){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp736))->hd; struct Cyc_Position_Segment* loc= d->loc; void*
_temp738=( void*) d->r; int _temp764; struct Cyc_Absyn_Exp* _temp766; struct Cyc_Core_Opt*
_temp768; struct Cyc_Core_Opt* _temp770; struct Cyc_Absyn_Pat* _temp772; struct
Cyc_List_List* _temp774; struct Cyc_Absyn_Vardecl* _temp776; struct Cyc_Absyn_Fndecl*
_temp778; struct Cyc_Absyn_Typedefdecl* _temp780; struct Cyc_Absyn_Structdecl*
_temp782; struct Cyc_Absyn_Uniondecl* _temp784; struct Cyc_Absyn_Tuniondecl*
_temp786; struct Cyc_Absyn_Enumdecl* _temp788; struct Cyc_List_List* _temp790;
struct _tagged_string* _temp792; struct Cyc_List_List* _temp794; struct _tuple0*
_temp796; struct _tuple0 _temp798; struct _tagged_string* _temp799; void*
_temp801; struct Cyc_List_List* _temp803; _LL740: if(*(( int*) _temp738) == Cyc_Absyn_Let_d){
_LL773: _temp772=(( struct Cyc_Absyn_Let_d_struct*) _temp738)->f1; goto _LL771;
_LL771: _temp770=(( struct Cyc_Absyn_Let_d_struct*) _temp738)->f2; goto _LL769;
_LL769: _temp768=(( struct Cyc_Absyn_Let_d_struct*) _temp738)->f3; goto _LL767;
_LL767: _temp766=(( struct Cyc_Absyn_Let_d_struct*) _temp738)->f4; goto _LL765;
_LL765: _temp764=(( struct Cyc_Absyn_Let_d_struct*) _temp738)->f5; goto _LL741;}
else{ goto _LL742;} _LL742: if(*(( int*) _temp738) == Cyc_Absyn_Letv_d){ _LL775:
_temp774=(( struct Cyc_Absyn_Letv_d_struct*) _temp738)->f1; goto _LL743;} else{
goto _LL744;} _LL744: if(*(( int*) _temp738) == Cyc_Absyn_Var_d){ _LL777:
_temp776=(( struct Cyc_Absyn_Var_d_struct*) _temp738)->f1; goto _LL745;} else{
goto _LL746;} _LL746: if(*(( int*) _temp738) == Cyc_Absyn_Fn_d){ _LL779:
_temp778=(( struct Cyc_Absyn_Fn_d_struct*) _temp738)->f1; goto _LL747;} else{
goto _LL748;} _LL748: if(*(( int*) _temp738) == Cyc_Absyn_Typedef_d){ _LL781:
_temp780=(( struct Cyc_Absyn_Typedef_d_struct*) _temp738)->f1; goto _LL749;}
else{ goto _LL750;} _LL750: if(*(( int*) _temp738) == Cyc_Absyn_Struct_d){
_LL783: _temp782=(( struct Cyc_Absyn_Struct_d_struct*) _temp738)->f1; goto
_LL751;} else{ goto _LL752;} _LL752: if(*(( int*) _temp738) == Cyc_Absyn_Union_d){
_LL785: _temp784=(( struct Cyc_Absyn_Union_d_struct*) _temp738)->f1; goto _LL753;}
else{ goto _LL754;} _LL754: if(*(( int*) _temp738) == Cyc_Absyn_Tunion_d){
_LL787: _temp786=(( struct Cyc_Absyn_Tunion_d_struct*) _temp738)->f1; goto
_LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp738) == Cyc_Absyn_Enum_d){
_LL789: _temp788=(( struct Cyc_Absyn_Enum_d_struct*) _temp738)->f1; goto _LL757;}
else{ goto _LL758;} _LL758: if(*(( int*) _temp738) == Cyc_Absyn_Namespace_d){
_LL793: _temp792=(( struct Cyc_Absyn_Namespace_d_struct*) _temp738)->f1; goto
_LL791; _LL791: _temp790=(( struct Cyc_Absyn_Namespace_d_struct*) _temp738)->f2;
goto _LL759;} else{ goto _LL760;} _LL760: if(*(( int*) _temp738) == Cyc_Absyn_Using_d){
_LL797: _temp796=(( struct Cyc_Absyn_Using_d_struct*) _temp738)->f1; _temp798=*
_temp796; _LL802: _temp801= _temp798.f1; goto _LL800; _LL800: _temp799= _temp798.f2;
goto _LL795; _LL795: _temp794=(( struct Cyc_Absyn_Using_d_struct*) _temp738)->f2;
goto _LL761;} else{ goto _LL762;} _LL762: if(*(( int*) _temp738) == Cyc_Absyn_ExternC_d){
_LL804: _temp803=(( struct Cyc_Absyn_ExternC_d_struct*) _temp738)->f1; goto
_LL763;} else{ goto _LL739;} _LL741: Cyc_Tcutil_terr( loc,({ unsigned char*
_temp805="top level let-declarations are not implemented"; struct _tagged_string
_temp806; _temp806.curr= _temp805; _temp806.base= _temp805; _temp806.last_plus_one=
_temp805 + 47; _temp806;})); goto _LL739; _LL743: Cyc_Tcutil_terr( loc,({
unsigned char* _temp807="top level let-declarations are not implemented"; struct
_tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 47; _temp808;})); goto _LL739; _LL745: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp776->sc): 0){( void*)(
_temp776->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp776, check_var_init); goto _LL739; _LL747: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp778->sc): 0){( void*)( _temp778->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp778); goto _LL739; _LL749: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp780); goto _LL739; _LL751: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp782->sc): 0){( void*)( _temp782->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp782); goto _LL739; _LL753: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp784->sc): 0){( void*)( _temp784->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp784);
goto _LL739; _LL755: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp786->sc): 0){( void*)( _temp786->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp786); goto _LL739; _LL757: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp788->sc): 0){( void*)( _temp788->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp788);
goto _LL739; _LL759: { struct Cyc_List_List* _temp809= te->ns; goto _LL810;
_LL810: { struct Cyc_List_List* _temp812=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp809,({ struct Cyc_List_List*
_temp811=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp811->hd=( void*) _temp792; _temp811->tl= 0; _temp811;})); goto _LL813;
_LL813: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge->namespaces, _temp792)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp792); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp812, Cyc_Tcenv_empty_genv());}
te->ns= _temp812; Cyc_Tc_tc_decls( te, _temp790, in_externC, check_var_init); te->ns=
_temp809; goto _LL739;}} _LL761: { struct _tagged_string* first; struct Cyc_List_List*
rest;{ void* _temp814= _temp801; struct Cyc_List_List* _temp826; struct Cyc_List_List*
_temp828; struct Cyc_List_List* _temp830; struct Cyc_List_List _temp832; struct
Cyc_List_List* _temp833; struct _tagged_string* _temp835; struct Cyc_List_List*
_temp837; struct Cyc_List_List _temp839; struct Cyc_List_List* _temp840; struct
_tagged_string* _temp842; _LL816: if( _temp814 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL817;} else{ goto _LL818;} _LL818: if(( unsigned int) _temp814 > 1u?*(( int*)
_temp814) == Cyc_Absyn_Rel_n: 0){ _LL827: _temp826=(( struct Cyc_Absyn_Rel_n_struct*)
_temp814)->f1; if( _temp826 == 0){ goto _LL819;} else{ goto _LL820;}} else{ goto
_LL820;} _LL820: if(( unsigned int) _temp814 > 1u?*(( int*) _temp814) == Cyc_Absyn_Abs_n:
0){ _LL829: _temp828=(( struct Cyc_Absyn_Abs_n_struct*) _temp814)->f1; if(
_temp828 == 0){ goto _LL821;} else{ goto _LL822;}} else{ goto _LL822;} _LL822:
if(( unsigned int) _temp814 > 1u?*(( int*) _temp814) == Cyc_Absyn_Rel_n: 0){
_LL831: _temp830=(( struct Cyc_Absyn_Rel_n_struct*) _temp814)->f1; if( _temp830
== 0){ goto _LL824;} else{ _temp832=* _temp830; _LL836: _temp835=( struct
_tagged_string*) _temp832.hd; goto _LL834; _LL834: _temp833= _temp832.tl; goto
_LL823;}} else{ goto _LL824;} _LL824: if(( unsigned int) _temp814 > 1u?*(( int*)
_temp814) == Cyc_Absyn_Abs_n: 0){ _LL838: _temp837=(( struct Cyc_Absyn_Abs_n_struct*)
_temp814)->f1; if( _temp837 == 0){ goto _LL815;} else{ _temp839=* _temp837;
_LL843: _temp842=( struct _tagged_string*) _temp839.hd; goto _LL841; _LL841:
_temp840= _temp839.tl; goto _LL825;}} else{ goto _LL815;} _LL817: goto _LL819;
_LL819: goto _LL821; _LL821: first= _temp799; rest= 0; goto _LL815; _LL823:
_temp842= _temp835; _temp840= _temp833; goto _LL825; _LL825: first= _temp842;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp840,({ struct Cyc_List_List* _temp844=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp844->hd=( void*) _temp799;
_temp844->tl= 0; _temp844;})); goto _LL815; _LL815:;}{ struct Cyc_List_List*
_temp845= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); goto _LL846;
_LL846: ge->availables=({ struct Cyc_List_List* _temp847=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp847->hd=( void*) _temp845;
_temp847->tl= ge->availables; _temp847;}); Cyc_Tc_tc_decls( te, _temp794,
in_externC, check_var_init); ge->availables=(( struct Cyc_List_List*)
_check_null( ge->availables))->tl; goto _LL739;}} _LL763: Cyc_Tc_tc_decls( te,
_temp803, 1, check_var_init); goto _LL739; _LL739:;}} static unsigned char
_temp850[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string={ _temp850,
_temp850, _temp850 + 4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string;
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int
check_var_init, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({ struct
Cyc_List_List* _temp851=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp851->hd=( void*)({ struct Cyc_Absyn_Decl* _temp852=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp852->r=( void*)(( void*)({
struct Cyc_Absyn_Namespace_d_struct* _temp853=( struct Cyc_Absyn_Namespace_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct)); _temp853[ 0]=({ struct
Cyc_Absyn_Namespace_d_struct _temp854; _temp854.tag= Cyc_Absyn_Namespace_d;
_temp854.f1= Cyc_Tc_cyc_ns; _temp854.f2= ds; _temp854;}); _temp853;})); _temp852->loc=
0; _temp852;}); _temp851->tl= 0; _temp851;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t; static
int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d);
static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple6{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp855=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp865; struct Cyc_List_List* _temp867; struct Cyc_List_List**
_temp869; struct _tuple0* _temp870; struct Cyc_List_List* _temp872; struct Cyc_List_List**
_temp874; struct _tagged_string* _temp875; _LL857: if(*(( int*) _temp855) == Cyc_Absyn_Var_d){
_LL866: _temp865=(( struct Cyc_Absyn_Var_d_struct*) _temp855)->f1; goto _LL858;}
else{ goto _LL859;} _LL859: if(*(( int*) _temp855) == Cyc_Absyn_Using_d){ _LL871:
_temp870=(( struct Cyc_Absyn_Using_d_struct*) _temp855)->f1; goto _LL868; _LL868:
_temp867=(( struct Cyc_Absyn_Using_d_struct*) _temp855)->f2; _temp869=&(( struct
Cyc_Absyn_Using_d_struct*) _temp855)->f2; goto _LL860;} else{ goto _LL861;}
_LL861: if(*(( int*) _temp855) == Cyc_Absyn_Namespace_d){ _LL876: _temp875=((
struct Cyc_Absyn_Namespace_d_struct*) _temp855)->f1; goto _LL873; _LL873:
_temp872=(( struct Cyc_Absyn_Namespace_d_struct*) _temp855)->f2; _temp874=&((
struct Cyc_Absyn_Namespace_d_struct*) _temp855)->f2; goto _LL862;} else{ goto
_LL863;} _LL863: goto _LL864; _LL858: if(( void*) _temp865->sc !=( void*) Cyc_Absyn_Extern){
return 1;}{ struct _tuple0 _temp879; struct _tagged_string* _temp880; void*
_temp882; struct _tuple0* _temp877= _temp865->name; _temp879=* _temp877; _LL883:
_temp882= _temp879.f1; goto _LL881; _LL881: _temp880= _temp879.f2; goto _LL878;
_LL878: { struct Cyc_List_List* ns;{ void* _temp884= _temp882; struct Cyc_List_List*
_temp892; struct Cyc_List_List* _temp894; _LL886: if( _temp884 ==( void*) Cyc_Absyn_Loc_n){
goto _LL887;} else{ goto _LL888;} _LL888: if(( unsigned int) _temp884 > 1u?*((
int*) _temp884) == Cyc_Absyn_Rel_n: 0){ _LL893: _temp892=(( struct Cyc_Absyn_Rel_n_struct*)
_temp884)->f1; goto _LL889;} else{ goto _LL890;} _LL890: if(( unsigned int)
_temp884 > 1u?*(( int*) _temp884) == Cyc_Absyn_Abs_n: 0){ _LL895: _temp894=((
struct Cyc_Absyn_Abs_n_struct*) _temp884)->f1; goto _LL891;} else{ goto _LL885;}
_LL887: ns= 0; goto _LL885; _LL889: ns= _temp892; goto _LL885; _LL891: ns=
_temp894; goto _LL885; _LL885:;}{ struct _tuple6* _temp896=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
goto _LL897; _LL897: { struct Cyc_Tcenv_Genv* _temp898=(* _temp896).f1; goto
_LL899; _LL899: { int _temp900=(*(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp898->ordinaries, _temp880)).f2;
goto _LL901; _LL901: if( ! _temp900){(* _temp896).f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((* _temp896).f2,
_temp880);} return _temp900;}}}}} _LL860: _temp874= _temp869; goto _LL862;
_LL862:* _temp874= Cyc_Tc_treeshake_f( env,* _temp874); return 1; _LL864: return
1; _LL856:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set* set,
struct _tagged_string* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp902=(* nsenv).f1; goto _LL903; _LL903: { struct Cyc_Set_Set* _temp904=(*
nsenv).f2; goto _LL905; _LL905: _temp902->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple3*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
_temp904, _temp902->ordinaries);}} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp906=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp906->f1= ge; _temp906->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp906;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp907=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
goto _LL908; _LL908: { struct Cyc_List_List* _temp909= Cyc_Tc_treeshake_f(
_temp907, ds); goto _LL910; _LL910:(( void(*)( void(* f)( struct Cyc_List_List*,
struct _tuple6*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
_temp907); return _temp909;}}