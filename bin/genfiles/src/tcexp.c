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
struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void* f1; void* f2; } ;
extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg[ 15u];
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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c(
struct _RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List*
x); extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
char Cyc_List_Nth[ 8u]; extern void* Cyc_List_nth( struct Cyc_List_List* x, int
i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex=
0; static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[ 14u]; typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple1{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple1* Cyc_Absyn_qvar_t;
typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo;
struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo; struct Cyc_Absyn_Exp;
struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
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
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
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
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct
Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern void
Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_is_integral( struct Cyc_Absyn_Exp*); extern int
Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*); struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2;
} ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var( struct _RegionHandle*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_contains_assign( struct
Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
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
if(*(( int*) _temp3) == Cyc_Tcenv_EnumRes){ _LL18: _temp17=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp3)->f1; goto _LL16; _LL16: _temp15=(( struct Cyc_Tcenv_EnumRes_struct*)
_temp3)->f2; goto _LL8;} else{ goto _LL9;} _LL9: if(*(( int*) _temp3) == Cyc_Tcenv_TunionRes){
_LL22: _temp21=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3)->f1; goto _LL20;
_LL20: _temp19=(( struct Cyc_Tcenv_TunionRes_struct*) _temp3)->f2; goto _LL10;}
else{ goto _LL11;} _LL11: if(*(( int*) _temp3) == Cyc_Tcenv_StructRes){ _LL24:
_temp23=(( struct Cyc_Tcenv_StructRes_struct*) _temp3)->f1; goto _LL12;} else{
goto _LL4;} _LL6:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp25=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
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
if(*(( int*) _temp44) == Cyc_Absyn_UnknownId_e){ _LL51: _temp50=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp44)->f1; goto _LL47;} else{ goto _LL48;} _LL48: goto _LL49; _LL47: { struct
_handler_cons _temp52; _push_handler(& _temp52);{ int _temp54= 0; if( setjmp(
_temp52.handler)){ _temp54= 1;} if( ! _temp54){{ void* _temp55= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp50); void* _temp65; struct Cyc_Absyn_Tunionfield* _temp67;
struct Cyc_Absyn_Tuniondecl* _temp69; struct Cyc_Absyn_Structdecl* _temp71;
struct Cyc_Absyn_Enumfield* _temp73; struct Cyc_Absyn_Enumdecl* _temp75; _LL57:
if(*(( int*) _temp55) == Cyc_Tcenv_VarRes){ _LL66: _temp65=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp55)->f1; goto _LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp55) ==
Cyc_Tcenv_TunionRes){ _LL70: _temp69=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp55)->f1; goto _LL68; _LL68: _temp67=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp55)->f2; goto _LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp55) ==
Cyc_Tcenv_StructRes){ _LL72: _temp71=(( struct Cyc_Tcenv_StructRes_struct*)
_temp55)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp55) ==
Cyc_Tcenv_EnumRes){ _LL76: _temp75=(( struct Cyc_Tcenv_EnumRes_struct*) _temp55)->f1;
goto _LL74; _LL74: _temp73=(( struct Cyc_Tcenv_EnumRes_struct*) _temp55)->f2;
goto _LL64;} else{ goto _LL56;} _LL58:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_FnCall_e_struct* _temp77=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp77[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp78; _temp78.tag= Cyc_Absyn_FnCall_e; _temp78.f1= e1; _temp78.f2= es;
_temp78.f3= 0; _temp78;}); _temp77;}))); _npop_handler( 0u); return; _LL60: if(
_temp67->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp79=
Cyc_Absynpp_qvar2string( _temp67->name); xprintf("%.*s is a constant, not a function",
_temp79.last_plus_one - _temp79.curr, _temp79.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp80=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp80[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp81; _temp81.tag= Cyc_Absyn_Tunion_e; _temp81.f1= 0; _temp81.f2= 0; _temp81.f3=
es; _temp81.f4= _temp69; _temp81.f5= _temp67; _temp81;}); _temp80;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL62: { struct Cyc_List_List*
_temp82=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); goto
_LL83; _LL83: if( _temp71->name != 0){( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_e_struct* _temp84=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp85; _temp85.tag= Cyc_Absyn_Struct_e; _temp85.f1=( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( _temp71->name))->v; _temp85.f2= 0; _temp85.f3=
_temp82; _temp85.f4=( struct Cyc_Absyn_Structdecl*) _temp71; _temp85;}); _temp84;})));}
else{ Cyc_Tcutil_terr( e->loc,({ char* _temp86="missing struct name"; struct
_tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86; _temp87.last_plus_one=
_temp86 + 20; _temp87;}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp88=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp88[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp89; _temp89.tag= Cyc_Absyn_Struct_e;
_temp89.f1= _temp50; _temp89.f2= 0; _temp89.f3= _temp82; _temp89.f4=( struct Cyc_Absyn_Structdecl*)
_temp71; _temp89;}); _temp88;})));} _npop_handler( 0u); return;} _LL64: Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp90= Cyc_Absynpp_qvar2string( _temp50);
xprintf("%.*s is an enum constructor, not a function", _temp90.last_plus_one -
_temp90.curr, _temp90.curr);})); _npop_handler( 0u); return; _LL56:;};
_pop_handler();} else{ void* _temp53=( void*) _exn_thrown; void* _temp92=
_temp53; _LL94: if( _temp92 == Cyc_Dict_Absent){ goto _LL95;} else{ goto _LL96;}
_LL96: goto _LL97; _LL95: Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string
_temp98= Cyc_Absynpp_qvar2string( _temp50); xprintf("unbound identifier %.*s",
_temp98.last_plus_one - _temp98.curr, _temp98.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp99=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp99[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp100; _temp100.tag= Cyc_Absyn_FnCall_e; _temp100.f1= e1; _temp100.f2= es;
_temp100.f3= 0; _temp100;}); _temp99;}))); return; _LL97:( void) _throw( _temp92);
_LL93:;}}} _LL49:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp101=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp101[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp102; _temp102.tag= Cyc_Absyn_FnCall_e;
_temp102.f1= e1; _temp102.f2= es; _temp102.f3= 0; _temp102;}); _temp101;})));
return; _LL45:;} static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp103=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp103[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp104; _temp104.tag= Cyc_Absyn_Array_e; _temp104.f1= des; _temp104;});
_temp103;}))); return;}{ void* t=*(( void**) _check_null( topt)); void* _temp105=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp115; struct Cyc_Absyn_Tqual
_temp117; void* _temp119; struct Cyc_Absyn_Structdecl** _temp121; struct Cyc_List_List*
_temp123; struct _tuple1* _temp125; struct Cyc_List_List* _temp127; _LL107: if((
unsigned int) _temp105 > 4u?*(( int*) _temp105) == Cyc_Absyn_ArrayType: 0){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f1;
goto _LL118; _LL118: _temp117=(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f2;
goto _LL116; _LL116: _temp115=(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f3;
goto _LL108;} else{ goto _LL109;} _LL109: if(( unsigned int) _temp105 > 4u?*((
int*) _temp105) == Cyc_Absyn_StructType: 0){ _LL126: _temp125=(( struct Cyc_Absyn_StructType_struct*)
_temp105)->f1; goto _LL124; _LL124: _temp123=(( struct Cyc_Absyn_StructType_struct*)
_temp105)->f2; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_StructType_struct*)
_temp105)->f3; goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int)
_temp105 > 4u?*(( int*) _temp105) == Cyc_Absyn_AnonStructType: 0){ _LL128:
_temp127=(( struct Cyc_Absyn_AnonStructType_struct*) _temp105)->f1; goto _LL112;}
else{ goto _LL113;} _LL113: goto _LL114; _LL108:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp129=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp129[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp130; _temp130.tag= Cyc_Absyn_Array_e; _temp130.f1= des; _temp130;});
_temp129;}))); goto _LL106; _LL110: if( _temp121 == 0? 1: _temp125 == 0){ Cyc_Tcutil_terr(
loc,({ char* _temp131="Compiler bug: struct type not properly set"; struct
_tagged_string _temp132; _temp132.curr= _temp131; _temp132.base= _temp131;
_temp132.last_plus_one= _temp131 + 43; _temp132;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp133=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp133[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp134; _temp134.tag= Cyc_Absyn_Struct_e; _temp134.f1=(
struct _tuple1*) _check_null( _temp125); _temp134.f2= 0; _temp134.f3= des;
_temp134.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp121)); _temp134;}); _temp133;}))); goto _LL106; _LL112:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp135=( struct
Cyc_Absyn_AnonStruct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp135[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp136; _temp136.tag= Cyc_Absyn_AnonStruct_e;
_temp136.f1=( void*) t; _temp136.f2= des; _temp136;}); _temp135;}))); goto
_LL106; _LL114:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp137=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp137[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp138; _temp138.tag= Cyc_Absyn_Array_e;
_temp138.f1= des; _temp138;}); _temp137;}))); goto _LL106; _LL106:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp139= msg_part;
struct _tagged_string _temp140= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp139.last_plus_one - _temp139.curr, _temp139.curr, _temp140.last_plus_one -
_temp140.curr, _temp140.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp141= c; char _temp157; void* _temp159; short _temp161;
void* _temp163; long long _temp165; void* _temp167; struct _tagged_string
_temp169; int _temp171; void* _temp173; struct _tagged_string _temp175; _LL143:
if(( unsigned int) _temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_Char_c: 0){
_LL160: _temp159=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp141)->f1; goto
_LL158; _LL158: _temp157=(( struct Cyc_Absyn_Char_c_struct*) _temp141)->f2; goto
_LL144;} else{ goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?*(( int*)
_temp141) == Cyc_Absyn_Short_c: 0){ _LL164: _temp163=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp141)->f1; goto _LL162; _LL162: _temp161=(( struct Cyc_Absyn_Short_c_struct*)
_temp141)->f2; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_LongLong_c: 0){ _LL168: _temp167=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp141)->f1; goto _LL166; _LL166:
_temp165=(( struct Cyc_Absyn_LongLong_c_struct*) _temp141)->f2; goto _LL148;}
else{ goto _LL149;} _LL149: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141)
== Cyc_Absyn_Float_c: 0){ _LL170: _temp169=(( struct Cyc_Absyn_Float_c_struct*)
_temp141)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_Int_c: 0){ _LL174: _temp173=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp141)->f1; goto _LL172; _LL172:
_temp171=(( struct Cyc_Absyn_Int_c_struct*) _temp141)->f2; goto _LL152;} else{
goto _LL153;} _LL153: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141) ==
Cyc_Absyn_String_c: 0){ _LL176: _temp175=(( struct Cyc_Absyn_String_c_struct*)
_temp141)->f1; goto _LL154;} else{ goto _LL155;} _LL155: if( _temp141 ==( void*)
Cyc_Absyn_Null_c){ goto _LL156;} else{ goto _LL142;} _LL144: t= _temp159 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL142;
_LL146: t= _temp163 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;
goto _LL142; _LL148: t= _temp167 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL142; _LL150: t= Cyc_Absyn_float_t; goto _LL142;
_LL152: if( topt == 0){ t= _temp173 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t;} else{ void* _temp177=*(( void**) _check_null( topt)); void*
_temp187; void* _temp189; void* _temp191; void* _temp193; void* _temp195; void*
_temp197; _LL179: if(( unsigned int) _temp177 > 4u?*(( int*) _temp177) == Cyc_Absyn_IntType:
0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp177)->f1;
goto _LL188; _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f2; if( _temp187 ==( void*) Cyc_Absyn_B1){ goto _LL180;} else{ goto
_LL181;}} else{ goto _LL181;} _LL181: if(( unsigned int) _temp177 > 4u?*(( int*)
_temp177) == Cyc_Absyn_IntType: 0){ _LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f1; goto _LL192; _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f2; if( _temp191 ==( void*) Cyc_Absyn_B2){ goto _LL182;} else{ goto
_LL183;}} else{ goto _LL183;} _LL183: if(( unsigned int) _temp177 > 4u?*(( int*)
_temp177) == Cyc_Absyn_IntType: 0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f1; goto _LL196; _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp177)->f2; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL180: t=
_temp189 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp199=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp199[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp200; _temp200.tag= Cyc_Absyn_Const_e;
_temp200.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp201=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp201[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp202; _temp202.tag= Cyc_Absyn_Char_c;
_temp202.f1=( void*) _temp189; _temp202.f2=( char) _temp171; _temp202;});
_temp201;})); _temp200;}); _temp199;}))); goto _LL178; _LL182: t= _temp193 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp203=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp203[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp204; _temp204.tag= Cyc_Absyn_Const_e; _temp204.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp205=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp205[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp206; _temp206.tag= Cyc_Absyn_Short_c; _temp206.f1=( void*) _temp193;
_temp206.f2=( short) _temp171; _temp206;}); _temp205;})); _temp204;}); _temp203;})));
goto _LL178; _LL184: t= _temp197 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL178; _LL186: t= _temp173 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL178; _LL178:;} goto _LL142; _LL154: {
int len=( int)({ struct _tagged_string _temp248= _temp175;( unsigned int)(
_temp248.last_plus_one - _temp248.curr);}); struct Cyc_Absyn_Const_e_struct*
_temp211=({ struct Cyc_Absyn_Const_e_struct* _temp207=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp207[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp208; _temp208.tag= Cyc_Absyn_Const_e; _temp208.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp209=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp209[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp210; _temp210.tag= Cyc_Absyn_Int_c; _temp210.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp210.f2= len; _temp210;}); _temp209;})); _temp208;}); _temp207;}); goto
_LL212; _LL212: { struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*)
_temp211, loc); elen->topt=({ struct Cyc_Core_Opt* _temp213=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp213->v=( void*) Cyc_Absyn_uint_t;
_temp213;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp214=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp214[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp215; _temp215.tag= Cyc_Absyn_Upper_b;
_temp215.f1= elen; _temp215;}); _temp214;})); if( topt != 0){ void* _temp216=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_Exp*
_temp224; struct Cyc_Absyn_Tqual _temp226; void* _temp228; struct Cyc_Absyn_PtrInfo
_temp230; struct Cyc_Absyn_Conref* _temp232; struct Cyc_Absyn_Tqual _temp234;
struct Cyc_Absyn_Conref* _temp236; void* _temp238; void* _temp240; _LL218: if((
unsigned int) _temp216 > 4u?*(( int*) _temp216) == Cyc_Absyn_ArrayType: 0){
_LL229: _temp228=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp216)->f1;
goto _LL227; _LL227: _temp226=(( struct Cyc_Absyn_ArrayType_struct*) _temp216)->f2;
goto _LL225; _LL225: _temp224=(( struct Cyc_Absyn_ArrayType_struct*) _temp216)->f3;
goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int) _temp216 > 4u?*((
int*) _temp216) == Cyc_Absyn_PointerType: 0){ _LL231: _temp230=(( struct Cyc_Absyn_PointerType_struct*)
_temp216)->f1; _LL241: _temp240=( void*) _temp230.elt_typ; goto _LL239; _LL239:
_temp238=( void*) _temp230.rgn_typ; goto _LL237; _LL237: _temp236= _temp230.nullable;
goto _LL235; _LL235: _temp234= _temp230.tq; goto _LL233; _LL233: _temp232=
_temp230.bounds; goto _LL221;} else{ goto _LL222;} _LL222: goto _LL223; _LL219:
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp242=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp242[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp243; _temp243.tag= Cyc_Absyn_ArrayType; _temp243.f1=(
void*) Cyc_Absyn_uchar_t; _temp243.f2= _temp226; _temp243.f3=( struct Cyc_Absyn_Exp*)
elen; _temp243;}); _temp242;}); _LL221: if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp244=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp244->v=( void*) t; _temp244;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));} else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp245=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp245[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp246; _temp246.tag= Cyc_Absyn_Upper_b;
_temp246.f1= elen; _temp246;}); _temp245;})); if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), t)? Cyc_Tcutil_silent_castable( te, loc, t,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp247=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp247->v=( void*) t; _temp247;});
Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt))); t=*(( void**)
_check_null( topt));}} goto _LL217; _LL223: goto _LL217; _LL217:;} return t;}}
_LL156: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp249=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp249[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp250; _temp250.tag= Cyc_Absyn_PointerType;
_temp250.f1=({ struct Cyc_Absyn_PtrInfo _temp251; _temp251.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp251.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp251.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp251.tq= Cyc_Absyn_empty_tqual(); _temp251.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp251;}); _temp250;});
_temp249;}); goto _LL142; _LL142:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp252= b; struct Cyc_Absyn_Vardecl* _temp266; struct Cyc_Absyn_Fndecl*
_temp268; struct Cyc_Absyn_Vardecl* _temp270; struct Cyc_Absyn_Vardecl* _temp272;
struct Cyc_Absyn_Vardecl* _temp274; _LL254: if( _temp252 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int) _temp252 > 1u?*((
int*) _temp252) == Cyc_Absyn_Global_b: 0){ _LL267: _temp266=(( struct Cyc_Absyn_Global_b_struct*)
_temp252)->f1; goto _LL257;} else{ goto _LL258;} _LL258: if(( unsigned int)
_temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Funname_b: 0){ _LL269: _temp268=((
struct Cyc_Absyn_Funname_b_struct*) _temp252)->f1; goto _LL259;} else{ goto
_LL260;} _LL260: if(( unsigned int) _temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Pat_b:
0){ _LL271: _temp270=(( struct Cyc_Absyn_Pat_b_struct*) _temp252)->f1; goto
_LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp252 > 1u?*(( int*)
_temp252) == Cyc_Absyn_Local_b: 0){ _LL273: _temp272=(( struct Cyc_Absyn_Local_b_struct*)
_temp252)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(( unsigned int)
_temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Param_b: 0){ _LL275: _temp274=((
struct Cyc_Absyn_Param_b_struct*) _temp252)->f1; goto _LL265;} else{ goto _LL253;}
_LL255: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp276=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s", _temp276.last_plus_one
- _temp276.curr, _temp276.curr);})); _LL257: q[ _check_known_subscript_notnull(
1u, 0)]=( _temp266->name)[ _check_known_subscript_notnull( 1u, 0)]; return( void*)
_temp266->type; _LL259: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp268->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp268);
_LL261: _temp272= _temp270; goto _LL263; _LL263: _temp274= _temp272; goto _LL265;
_LL265:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp274->type; _LL253:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp277=( void*) fmt->r; void* _temp283; struct _tagged_string _temp285;
_LL279: if(*(( int*) _temp277) == Cyc_Absyn_Const_e){ _LL284: _temp283=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp277)->f1; if(( unsigned int) _temp283 > 1u?*((
int*) _temp283) == Cyc_Absyn_String_c: 0){ _LL286: _temp285=(( struct Cyc_Absyn_String_c_struct*)
_temp283)->f1; goto _LL280;} else{ goto _LL281;}} else{ goto _LL281;} _LL281:
goto _LL282; _LL280: desc_types= type_getter( _temp285, fmt->loc); goto _LL278;
_LL282: Cyc_Tcutil_terr( fmt->loc,({ char* _temp287="expecting a literal format string";
struct _tagged_string _temp288; _temp288.curr= _temp287; _temp288.base= _temp287;
_temp288.last_plus_one= _temp287 + 34; _temp288;})); return; _LL278:;} for( 0;
desc_types != 0? args != 0: 0;( desc_types=(( struct Cyc_List_List*) _check_null(
desc_types))->tl, args=(( struct Cyc_List_List*) _check_null( args))->tl)){ void*
t=( void*)(( struct Cyc_List_List*) _check_null( desc_types))->hd; struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd; if(
! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp289= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp290= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
xprintf("descriptor has type %.*s but argument has type %.*s", _temp289.last_plus_one
- _temp289.curr, _temp289.curr, _temp290.last_plus_one - _temp290.curr, _temp290.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,({ char* _temp291="too few arguments";
struct _tagged_string _temp292; _temp292.curr= _temp291; _temp292.base= _temp291;
_temp292.last_plus_one= _temp291 + 18; _temp292;}));} if( args != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( args))->hd)->loc,({
char* _temp293="too many arguments"; struct _tagged_string _temp294; _temp294.curr=
_temp293; _temp294.base= _temp293; _temp294.last_plus_one= _temp293 + 19;
_temp294;}));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp295= p; _LL297: if( _temp295 ==( void*) Cyc_Absyn_Plus){ goto _LL298;}
else{ goto _LL299;} _LL299: if( _temp295 ==( void*) Cyc_Absyn_Minus){ goto
_LL300;} else{ goto _LL301;} _LL301: if( _temp295 ==( void*) Cyc_Absyn_Not){
goto _LL302;} else{ goto _LL303;} _LL303: if( _temp295 ==( void*) Cyc_Absyn_Bitnot){
goto _LL304;} else{ goto _LL305;} _LL305: if( _temp295 ==( void*) Cyc_Absyn_Size){
goto _LL306;} else{ goto _LL307;} _LL307: goto _LL308; _LL298: goto _LL300;
_LL300: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp309= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_temp309.last_plus_one - _temp309.curr, _temp309.curr);}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL302: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp310= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp310.last_plus_one - _temp310.curr, _temp310.curr);}));} return Cyc_Absyn_sint_t;
_LL304: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp311= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp311.last_plus_one - _temp311.curr, _temp311.curr);}));} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL306:{ void* _temp312= t;
struct Cyc_Absyn_Exp* _temp320; struct Cyc_Absyn_Tqual _temp322; void* _temp324;
struct Cyc_Absyn_PtrInfo _temp326; struct Cyc_Absyn_Conref* _temp328; struct Cyc_Absyn_Tqual
_temp330; struct Cyc_Absyn_Conref* _temp332; void* _temp334; void* _temp336;
_LL314: if(( unsigned int) _temp312 > 4u?*(( int*) _temp312) == Cyc_Absyn_ArrayType:
0){ _LL325: _temp324=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f1;
goto _LL323; _LL323: _temp322=(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f2;
goto _LL321; _LL321: _temp320=(( struct Cyc_Absyn_ArrayType_struct*) _temp312)->f3;
goto _LL315;} else{ goto _LL316;} _LL316: if(( unsigned int) _temp312 > 4u?*((
int*) _temp312) == Cyc_Absyn_PointerType: 0){ _LL327: _temp326=(( struct Cyc_Absyn_PointerType_struct*)
_temp312)->f1; _LL337: _temp336=( void*) _temp326.elt_typ; goto _LL335; _LL335:
_temp334=( void*) _temp326.rgn_typ; goto _LL333; _LL333: _temp332= _temp326.nullable;
goto _LL331; _LL331: _temp330= _temp326.tq; goto _LL329; _LL329: _temp328=
_temp326.bounds; goto _LL317;} else{ goto _LL318;} _LL318: goto _LL319; _LL315:
goto _LL313; _LL317: goto _LL313; _LL319: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp338= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_temp338.last_plus_one - _temp338.curr, _temp338.curr);})); goto _LL313; _LL313:;}
return Cyc_Absyn_uint_t; _LL308: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp339="Non-unary primop"; struct _tagged_string _temp340; _temp340.curr=
_temp339; _temp340.base= _temp339; _temp340.last_plus_one= _temp339 + 17;
_temp340;})); _LL296:;} static void* Cyc_Tcexp_tcArithBinop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, int(* checker)( struct
Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp341= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); xprintf("type %.*s cannot be used here", _temp341.last_plus_one
- _temp341.curr, _temp341.curr);})); return Cyc_Absyn_wildtyp();} if( ! checker(
e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp342= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); xprintf("type %.*s cannot be used here",
_temp342.last_plus_one - _temp342.curr, _temp342.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
return Cyc_Tcutil_max_arithmetic_type( t1, t2);}} static void* Cyc_Tcexp_tcPlus(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);
void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);
void* _temp343= t1; struct Cyc_Absyn_PtrInfo _temp349; struct Cyc_Absyn_Conref*
_temp351; struct Cyc_Absyn_Tqual _temp353; struct Cyc_Absyn_Conref* _temp355;
void* _temp357; void* _temp359; _LL345: if(( unsigned int) _temp343 > 4u?*(( int*)
_temp343) == Cyc_Absyn_PointerType: 0){ _LL350: _temp349=(( struct Cyc_Absyn_PointerType_struct*)
_temp343)->f1; _LL360: _temp359=( void*) _temp349.elt_typ; goto _LL358; _LL358:
_temp357=( void*) _temp349.rgn_typ; goto _LL356; _LL356: _temp355= _temp349.nullable;
goto _LL354; _LL354: _temp353= _temp349.tq; goto _LL352; _LL352: _temp351=
_temp349.bounds; goto _LL346;} else{ goto _LL347;} _LL347: goto _LL348; _LL346:
if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct
_tagged_string _temp361= Cyc_Absynpp_typ2string( t2); xprintf("expecting int but found %.*s",
_temp361.last_plus_one - _temp361.curr, _temp361.curr);}));} _temp351=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp351);{ void* _temp362=( void*) _temp351->v; void* _temp370; void* _temp372;
struct Cyc_Absyn_Exp* _temp374; _LL364: if(( unsigned int) _temp362 > 1u?*(( int*)
_temp362) == Cyc_Absyn_Eq_constr: 0){ _LL371: _temp370=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp362)->f1; if( _temp370 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL365;} else{
goto _LL366;}} else{ goto _LL366;} _LL366: if(( unsigned int) _temp362 > 1u?*((
int*) _temp362) == Cyc_Absyn_Eq_constr: 0){ _LL373: _temp372=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp362)->f1; if(( unsigned int) _temp372 > 1u?*((
int*) _temp372) == Cyc_Absyn_Upper_b: 0){ _LL375: _temp374=(( struct Cyc_Absyn_Upper_b_struct*)
_temp372)->f1; goto _LL367;} else{ goto _LL368;}} else{ goto _LL368;} _LL368:
goto _LL369; _LL365: return t1; _LL367: { unsigned int _temp376= Cyc_Evexp_eval_const_uint_exp(
_temp374); goto _LL377; _LL377: _temp355=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp355);{ int is_nullable;{
void* _temp378=( void*) _temp355->v; int _temp384; _LL380: if(( unsigned int)
_temp378 > 1u?*(( int*) _temp378) == Cyc_Absyn_Eq_constr: 0){ _LL385: _temp384=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp378)->f1; goto _LL381;} else{
goto _LL382;} _LL382: goto _LL383; _LL381: if( _temp384){ struct Cyc_Absyn_PointerType_struct*
_temp389=({ struct Cyc_Absyn_PointerType_struct* _temp386=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp386[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp387; _temp387.tag= Cyc_Absyn_PointerType;
_temp387.f1=({ struct Cyc_Absyn_PtrInfo _temp388; _temp388.elt_typ=( void*)
_temp359; _temp388.rgn_typ=( void*) _temp357; _temp388.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp388.tq= _temp353; _temp388.bounds=
_temp351; _temp388;}); _temp387;}); _temp386;}); goto _LL390; _LL390: Cyc_Tcutil_warn(
e1->loc,({ struct _tagged_string _temp391= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp392= Cyc_Absynpp_typ2string(( void*) _temp389); xprintf("coercing from %.*s to %.*s",
_temp391.last_plus_one - _temp391.curr, _temp391.curr, _temp392.last_plus_one -
_temp392.curr, _temp392.curr);})); Cyc_Tcutil_unchecked_cast( te, e1,( void*)
_temp389);} goto _LL379; _LL383:( void*)( _temp355->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp393=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp393[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp394; _temp394.tag= Cyc_Absyn_Eq_constr; _temp394.f1=(
void*) 0; _temp394;}); _temp393;}))); goto _LL379; _LL379:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int _temp395= Cyc_Evexp_eval_const_uint_exp( e2); goto _LL396;
_LL396: if( _temp376 < _temp395){ _temp395= _temp376; Cyc_Tcutil_warn( e1->loc,
xprintf("pointer arithmetic is out of bounds"));}{ struct Cyc_Absyn_Exp*
_temp397= Cyc_Absyn_uint_exp( _temp376 - _temp395, 0); goto _LL398; _LL398:
_temp397->topt=({ struct Cyc_Core_Opt* _temp399=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp399->v=( void*) Cyc_Absyn_uint_t;
_temp399;}); return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp400=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp400[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp401; _temp401.tag= Cyc_Absyn_PointerType;
_temp401.f1=({ struct Cyc_Absyn_PtrInfo _temp402; _temp402.elt_typ=( void*)
_temp359; _temp402.rgn_typ=( void*) _temp357; _temp402.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp402.tq= _temp353; _temp402.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp403=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp403[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp404; _temp404.tag= Cyc_Absyn_Upper_b; _temp404.f1= _temp397; _temp404;});
_temp403;})); _temp402;}); _temp401;}); _temp400;});}} else{ Cyc_Tcutil_terr( e1->loc,({
char* _temp405="cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 98; _temp406;})); return t1;}}} _LL369:( void*)(
_temp351->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp407=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp407[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp408; _temp408.tag= Cyc_Absyn_Eq_constr;
_temp408.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp408;}); _temp407;})));
return t1; _LL363:;} _LL348: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL344:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp409= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); struct _tagged_string _temp410= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp409.last_plus_one - _temp409.curr, _temp409.curr, _temp410.last_plus_one -
_temp410.curr, _temp410.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp411= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp412= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp411.last_plus_one - _temp411.curr, _temp411.curr, _temp412.last_plus_one -
_temp412.curr, _temp412.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_BoxKind)): 0){
return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2, t1)){
Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te, e1, t2); return Cyc_Absyn_sint_t;}
else{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ struct _tuple0
_temp414=({ struct _tuple0 _temp413; _temp413.f1= Cyc_Tcutil_compress( t1);
_temp413.f2= Cyc_Tcutil_compress( t2); _temp413;}); void* _temp420; struct Cyc_Absyn_PtrInfo
_temp422; struct Cyc_Absyn_Conref* _temp424; struct Cyc_Absyn_Tqual _temp426;
struct Cyc_Absyn_Conref* _temp428; void* _temp430; void* _temp432; void*
_temp434; struct Cyc_Absyn_PtrInfo _temp436; struct Cyc_Absyn_Conref* _temp438;
struct Cyc_Absyn_Tqual _temp440; struct Cyc_Absyn_Conref* _temp442; void*
_temp444; void* _temp446; _LL416: _LL435: _temp434= _temp414.f1; if((
unsigned int) _temp434 > 4u?*(( int*) _temp434) == Cyc_Absyn_PointerType: 0){
_LL437: _temp436=(( struct Cyc_Absyn_PointerType_struct*) _temp434)->f1; _LL447:
_temp446=( void*) _temp436.elt_typ; goto _LL445; _LL445: _temp444=( void*)
_temp436.rgn_typ; goto _LL443; _LL443: _temp442= _temp436.nullable; goto _LL441;
_LL441: _temp440= _temp436.tq; goto _LL439; _LL439: _temp438= _temp436.bounds;
goto _LL421;} else{ goto _LL418;} _LL421: _temp420= _temp414.f2; if((
unsigned int) _temp420 > 4u?*(( int*) _temp420) == Cyc_Absyn_PointerType: 0){
_LL423: _temp422=(( struct Cyc_Absyn_PointerType_struct*) _temp420)->f1; _LL433:
_temp432=( void*) _temp422.elt_typ; goto _LL431; _LL431: _temp430=( void*)
_temp422.rgn_typ; goto _LL429; _LL429: _temp428= _temp422.nullable; goto _LL427;
_LL427: _temp426= _temp422.tq; goto _LL425; _LL425: _temp424= _temp422.bounds;
goto _LL417;} else{ goto _LL418;} _LL418: goto _LL419; _LL417: if( Cyc_Tcutil_unify(
_temp446, _temp432)){ return Cyc_Absyn_sint_t;} goto _LL415; _LL419: goto _LL415;
_LL415:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp448= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp449= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp448.last_plus_one - _temp448.curr, _temp448.curr, _temp449.last_plus_one -
_temp449.curr, _temp449.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp450= p; _LL452: if( _temp450 ==( void*) Cyc_Absyn_Plus){ goto _LL453;}
else{ goto _LL454;} _LL454: if( _temp450 ==( void*) Cyc_Absyn_Minus){ goto
_LL455;} else{ goto _LL456;} _LL456: if( _temp450 ==( void*) Cyc_Absyn_Times){
goto _LL457;} else{ goto _LL458;} _LL458: if( _temp450 ==( void*) Cyc_Absyn_Div){
goto _LL459;} else{ goto _LL460;} _LL460: if( _temp450 ==( void*) Cyc_Absyn_Mod){
goto _LL461;} else{ goto _LL462;} _LL462: if( _temp450 ==( void*) Cyc_Absyn_Bitand){
goto _LL463;} else{ goto _LL464;} _LL464: if( _temp450 ==( void*) Cyc_Absyn_Bitor){
goto _LL465;} else{ goto _LL466;} _LL466: if( _temp450 ==( void*) Cyc_Absyn_Bitxor){
goto _LL467;} else{ goto _LL468;} _LL468: if( _temp450 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL469;} else{ goto _LL470;} _LL470: if( _temp450 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL471;} else{ goto _LL472;} _LL472: if( _temp450 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL473;} else{ goto _LL474;} _LL474: if( _temp450 ==( void*) Cyc_Absyn_Eq){
goto _LL475;} else{ goto _LL476;} _LL476: if( _temp450 ==( void*) Cyc_Absyn_Neq){
goto _LL477;} else{ goto _LL478;} _LL478: if( _temp450 ==( void*) Cyc_Absyn_Gt){
goto _LL479;} else{ goto _LL480;} _LL480: if( _temp450 ==( void*) Cyc_Absyn_Lt){
goto _LL481;} else{ goto _LL482;} _LL482: if( _temp450 ==( void*) Cyc_Absyn_Gte){
goto _LL483;} else{ goto _LL484;} _LL484: if( _temp450 ==( void*) Cyc_Absyn_Lte){
goto _LL485;} else{ goto _LL486;} _LL486: goto _LL487; _LL453: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL455: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL457: goto _LL459;
_LL459: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL461: goto _LL463; _LL463: goto _LL465; _LL465: goto _LL467; _LL467: goto
_LL469; _LL469: goto _LL471; _LL471: goto _LL473; _LL473: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL475: goto _LL477; _LL477: goto _LL479;
_LL479: goto _LL481; _LL481: goto _LL483; _LL483: goto _LL485; _LL485: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL487:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp488=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp488[ 0]=({ struct Cyc_Core_Failure_struct
_temp489; _temp489.tag= Cyc_Core_Failure; _temp489.f1=({ char* _temp490="bad binary primop";
struct _tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 18; _temp491;}); _temp489;}); _temp488;}));
_LL451:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp492= p; _LL494: if( _temp492
==( void*) Cyc_Absyn_Printf){ goto _LL495;} else{ goto _LL496;} _LL496: if(
_temp492 ==( void*) Cyc_Absyn_Fprintf){ goto _LL497;} else{ goto _LL498;} _LL498:
if( _temp492 ==( void*) Cyc_Absyn_Xprintf){ goto _LL499;} else{ goto _LL500;}
_LL500: if( _temp492 ==( void*) Cyc_Absyn_Scanf){ goto _LL501;} else{ goto
_LL502;} _LL502: if( _temp492 ==( void*) Cyc_Absyn_Fscanf){ goto _LL503;} else{
goto _LL504;} _LL504: if( _temp492 ==( void*) Cyc_Absyn_Sscanf){ goto _LL505;}
else{ goto _LL506;} _LL506: goto _LL507; _LL495: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ char* _temp508="missing format string in printf"; struct
_tagged_string _temp509; _temp509.curr= _temp508; _temp509.base= _temp508;
_temp509.last_plus_one= _temp508 + 32; _temp509;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL493; _LL497: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,({ char*
_temp510="too few arguments to fprintf"; struct _tagged_string _temp511;
_temp511.curr= _temp510; _temp511.base= _temp510; _temp511.last_plus_one=
_temp510 + 29; _temp511;}));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,({ struct _tagged_string _temp512= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v); xprintf("expecting FILE @ but found %.*s",
_temp512.last_plus_one - _temp512.curr, _temp512.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd,(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->tl, Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL493; _LL499: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,({ char*
_temp513="missing format string in xprintf"; struct _tagged_string _temp514;
_temp514.curr= _temp513; _temp514.base= _temp513; _temp514.last_plus_one=
_temp513 + 33; _temp514;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd,(( struct Cyc_List_List*)
_check_null( es))->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto _LL493; _LL501: if( es ==
0){ return Cyc_Tcexp_expr_err( te, loc,({ char* _temp515="missing format string in scanf";
struct _tagged_string _temp516; _temp516.curr= _temp515; _temp516.base= _temp515;
_temp516.last_plus_one= _temp515 + 31; _temp516;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,((
struct Cyc_List_List*) _check_null( es))->tl, Cyc_Formatstr_get_scanf_typs); t=
Cyc_Absyn_sint_t; goto _LL493; _LL503: if( es == 0? 1:(( struct Cyc_List_List*)
_check_null( es))->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,({ char*
_temp517="too few arguments to fscanf"; struct _tagged_string _temp518; _temp518.curr=
_temp517; _temp518.base= _temp517; _temp518.last_plus_one= _temp517 + 28;
_temp518;}));} if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v,
Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc,({ struct _tagged_string _temp519= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v); xprintf("expecting FILE @ but found %.*s",
_temp519.last_plus_one - _temp519.curr, _temp519.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd,(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t;
goto _LL493; _LL505: Cyc_Tcutil_terr( loc,({ char* _temp520="sscanf is not supported";
struct _tagged_string _temp521; _temp521.curr= _temp520; _temp521.base= _temp520;
_temp521.last_plus_one= _temp520 + 24; _temp521;})); t= Cyc_Absyn_sint_t; goto
_LL493; _LL507: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL522: return Cyc_Tcexp_expr_err( te, loc,({ char* _temp524="primitive operator has 0 arguments";
struct _tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 35; _temp525;})); case 1: _LL523: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL526: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL527: return Cyc_Tcexp_expr_err(
te, loc,({ char* _temp529="primitive operator has > 2 arguments"; struct
_tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 37; _temp530;}));} goto _LL493; _LL493:;}
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp531=( void*) e->r; void* _temp551; struct Cyc_Absyn_Vardecl* _temp553;
struct _tuple1* _temp555; void* _temp557; struct Cyc_Absyn_Vardecl* _temp559;
struct _tuple1* _temp561; void* _temp563; struct Cyc_Absyn_Vardecl* _temp565;
struct _tuple1* _temp567; void* _temp569; struct Cyc_Absyn_Vardecl* _temp571;
struct _tuple1* _temp573; struct Cyc_Absyn_Exp* _temp575; struct Cyc_Absyn_Exp*
_temp577; struct _tagged_string* _temp579; struct Cyc_Absyn_Exp* _temp581;
struct _tagged_string* _temp583; struct Cyc_Absyn_Exp* _temp585; struct Cyc_Absyn_Exp*
_temp587; _LL533: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){ _LL556: _temp555=((
struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL552; _LL552: _temp551=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if(( unsigned int)
_temp551 > 1u?*(( int*) _temp551) == Cyc_Absyn_Param_b: 0){ _LL554: _temp553=((
struct Cyc_Absyn_Param_b_struct*) _temp551)->f1; goto _LL534;} else{ goto _LL535;}}
else{ goto _LL535;} _LL535: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){ _LL562:
_temp561=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL558; _LL558:
_temp557=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp557 > 1u?*(( int*) _temp557) == Cyc_Absyn_Local_b: 0){ _LL560:
_temp559=(( struct Cyc_Absyn_Local_b_struct*) _temp557)->f1; goto _LL536;} else{
goto _LL537;}} else{ goto _LL537;} _LL537: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){
_LL568: _temp567=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL564;
_LL564: _temp563=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp563 > 1u?*(( int*) _temp563) == Cyc_Absyn_Pat_b: 0){ _LL566:
_temp565=(( struct Cyc_Absyn_Pat_b_struct*) _temp563)->f1; goto _LL538;} else{
goto _LL539;}} else{ goto _LL539;} _LL539: if(*(( int*) _temp531) == Cyc_Absyn_Var_e){
_LL574: _temp573=(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f1; goto _LL570;
_LL570: _temp569=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp531)->f2; if((
unsigned int) _temp569 > 1u?*(( int*) _temp569) == Cyc_Absyn_Global_b: 0){
_LL572: _temp571=(( struct Cyc_Absyn_Global_b_struct*) _temp569)->f1; goto
_LL540;} else{ goto _LL541;}} else{ goto _LL541;} _LL541: if(*(( int*) _temp531)
== Cyc_Absyn_Subscript_e){ _LL578: _temp577=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp531)->f1; goto _LL576; _LL576: _temp575=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp531)->f2; goto _LL542;} else{ goto _LL543;} _LL543: if(*(( int*) _temp531)
== Cyc_Absyn_StructMember_e){ _LL582: _temp581=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp531)->f1; goto _LL580; _LL580: _temp579=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp531)->f2; goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp531)
== Cyc_Absyn_StructArrow_e){ _LL586: _temp585=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp531)->f1; goto _LL584; _LL584: _temp583=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp531)->f2; goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp531)
== Cyc_Absyn_Deref_e){ _LL588: _temp587=(( struct Cyc_Absyn_Deref_e_struct*)
_temp531)->f1; goto _LL548;} else{ goto _LL549;} _LL549: goto _LL550; _LL534:
_temp559= _temp553; goto _LL536; _LL536: _temp565= _temp559; goto _LL538; _LL538:
_temp571= _temp565; goto _LL540; _LL540: if( !( _temp571->tq).q_const){ return;}
goto _LL532; _LL542:{ void* _temp589= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp577->topt))->v); struct Cyc_Absyn_PtrInfo _temp599; struct Cyc_Absyn_Conref*
_temp601; struct Cyc_Absyn_Tqual _temp603; struct Cyc_Absyn_Conref* _temp605;
void* _temp607; void* _temp609; struct Cyc_Absyn_Exp* _temp611; struct Cyc_Absyn_Tqual
_temp613; void* _temp615; struct Cyc_List_List* _temp617; _LL591: if((
unsigned int) _temp589 > 4u?*(( int*) _temp589) == Cyc_Absyn_PointerType: 0){
_LL600: _temp599=(( struct Cyc_Absyn_PointerType_struct*) _temp589)->f1; _LL610:
_temp609=( void*) _temp599.elt_typ; goto _LL608; _LL608: _temp607=( void*)
_temp599.rgn_typ; goto _LL606; _LL606: _temp605= _temp599.nullable; goto _LL604;
_LL604: _temp603= _temp599.tq; goto _LL602; _LL602: _temp601= _temp599.bounds;
goto _LL592;} else{ goto _LL593;} _LL593: if(( unsigned int) _temp589 > 4u?*((
int*) _temp589) == Cyc_Absyn_ArrayType: 0){ _LL616: _temp615=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp589)->f1; goto _LL614; _LL614: _temp613=((
struct Cyc_Absyn_ArrayType_struct*) _temp589)->f2; goto _LL612; _LL612: _temp611=((
struct Cyc_Absyn_ArrayType_struct*) _temp589)->f3; goto _LL594;} else{ goto
_LL595;} _LL595: if(( unsigned int) _temp589 > 4u?*(( int*) _temp589) == Cyc_Absyn_TupleType:
0){ _LL618: _temp617=(( struct Cyc_Absyn_TupleType_struct*) _temp589)->f1; goto
_LL596;} else{ goto _LL597;} _LL597: goto _LL598; _LL592: _temp613= _temp603;
goto _LL594; _LL594: if( ! _temp613.q_const){ return;} goto _LL590; _LL596: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp575);{ struct _handler_cons
_temp619; _push_handler(& _temp619);{ int _temp621= 0; if( setjmp( _temp619.handler)){
_temp621= 1;} if( ! _temp621){{ struct _tuple7 _temp624; void* _temp625; struct
Cyc_Absyn_Tqual _temp627; struct _tuple7* _temp622=(( struct _tuple7*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp617,( int) i); _temp624=* _temp622;
_LL628: _temp627= _temp624.f1; goto _LL626; _LL626: _temp625= _temp624.f2; goto
_LL623; _LL623: if( ! _temp627.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp620=( void*) _exn_thrown; void* _temp630=
_temp620; _LL632: if( _temp630 == Cyc_List_Nth){ goto _LL633;} else{ goto _LL634;}
_LL634: goto _LL635; _LL633: return; _LL635:( void) _throw( _temp630); _LL631:;}}}
goto _LL590;} _LL598: goto _LL590; _LL590:;} goto _LL532; _LL544:{ void*
_temp636= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp581->topt))->v); struct Cyc_Absyn_Structdecl** _temp648; struct Cyc_List_List*
_temp650; struct _tuple1* _temp652; struct Cyc_List_List* _temp654; struct Cyc_List_List*
_temp656; struct Cyc_Absyn_Uniondecl** _temp658; struct Cyc_List_List* _temp660;
struct _tuple1* _temp662; _LL638: if(( unsigned int) _temp636 > 4u?*(( int*)
_temp636) == Cyc_Absyn_StructType: 0){ _LL653: _temp652=(( struct Cyc_Absyn_StructType_struct*)
_temp636)->f1; goto _LL651; _LL651: _temp650=(( struct Cyc_Absyn_StructType_struct*)
_temp636)->f2; goto _LL649; _LL649: _temp648=(( struct Cyc_Absyn_StructType_struct*)
_temp636)->f3; goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int)
_temp636 > 4u?*(( int*) _temp636) == Cyc_Absyn_AnonUnionType: 0){ _LL655:
_temp654=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp636)->f1; goto _LL641;}
else{ goto _LL642;} _LL642: if(( unsigned int) _temp636 > 4u?*(( int*) _temp636)
== Cyc_Absyn_AnonStructType: 0){ _LL657: _temp656=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp636)->f1; goto _LL643;} else{ goto _LL644;} _LL644: if(( unsigned int)
_temp636 > 4u?*(( int*) _temp636) == Cyc_Absyn_UnionType: 0){ _LL663: _temp662=((
struct Cyc_Absyn_UnionType_struct*) _temp636)->f1; goto _LL661; _LL661: _temp660=((
struct Cyc_Absyn_UnionType_struct*) _temp636)->f2; goto _LL659; _LL659: _temp658=((
struct Cyc_Absyn_UnionType_struct*) _temp636)->f3; goto _LL645;} else{ goto
_LL646;} _LL646: goto _LL647; _LL639: { struct Cyc_Absyn_Structfield* sf=
_temp648 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*((
struct Cyc_Absyn_Structdecl**) _check_null( _temp648)), _temp579); if( sf == 0?
1: !((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL637;} _LL641: _temp656= _temp654; goto _LL643; _LL643: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp656, _temp579); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL637;} _LL645: { struct Cyc_Absyn_Structfield*
sf= _temp658 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp658)), _temp579); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL637;} _LL647: goto _LL637; _LL637:;} goto _LL532; _LL546:{ void*
_temp664= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp585->topt))->v); struct Cyc_Absyn_PtrInfo _temp670; struct Cyc_Absyn_Conref*
_temp672; struct Cyc_Absyn_Tqual _temp674; struct Cyc_Absyn_Conref* _temp676;
void* _temp678; void* _temp680; _LL666: if(( unsigned int) _temp664 > 4u?*(( int*)
_temp664) == Cyc_Absyn_PointerType: 0){ _LL671: _temp670=(( struct Cyc_Absyn_PointerType_struct*)
_temp664)->f1; _LL681: _temp680=( void*) _temp670.elt_typ; goto _LL679; _LL679:
_temp678=( void*) _temp670.rgn_typ; goto _LL677; _LL677: _temp676= _temp670.nullable;
goto _LL675; _LL675: _temp674= _temp670.tq; goto _LL673; _LL673: _temp672=
_temp670.bounds; goto _LL667;} else{ goto _LL668;} _LL668: goto _LL669; _LL667:{
void* _temp682= Cyc_Tcutil_compress( _temp680); struct Cyc_Absyn_Structdecl**
_temp694; struct Cyc_List_List* _temp696; struct _tuple1* _temp698; struct Cyc_Absyn_Uniondecl**
_temp700; struct Cyc_List_List* _temp702; struct _tuple1* _temp704; struct Cyc_List_List*
_temp706; struct Cyc_List_List* _temp708; _LL684: if(( unsigned int) _temp682 >
4u?*(( int*) _temp682) == Cyc_Absyn_StructType: 0){ _LL699: _temp698=(( struct
Cyc_Absyn_StructType_struct*) _temp682)->f1; goto _LL697; _LL697: _temp696=((
struct Cyc_Absyn_StructType_struct*) _temp682)->f2; goto _LL695; _LL695:
_temp694=(( struct Cyc_Absyn_StructType_struct*) _temp682)->f3; goto _LL685;}
else{ goto _LL686;} _LL686: if(( unsigned int) _temp682 > 4u?*(( int*) _temp682)
== Cyc_Absyn_UnionType: 0){ _LL705: _temp704=(( struct Cyc_Absyn_UnionType_struct*)
_temp682)->f1; goto _LL703; _LL703: _temp702=(( struct Cyc_Absyn_UnionType_struct*)
_temp682)->f2; goto _LL701; _LL701: _temp700=(( struct Cyc_Absyn_UnionType_struct*)
_temp682)->f3; goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int)
_temp682 > 4u?*(( int*) _temp682) == Cyc_Absyn_AnonStructType: 0){ _LL707:
_temp706=(( struct Cyc_Absyn_AnonStructType_struct*) _temp682)->f1; goto _LL689;}
else{ goto _LL690;} _LL690: if(( unsigned int) _temp682 > 4u?*(( int*) _temp682)
== Cyc_Absyn_AnonUnionType: 0){ _LL709: _temp708=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp682)->f1; goto _LL691;} else{ goto _LL692;} _LL692: goto _LL693; _LL685: {
struct Cyc_Absyn_Structfield* sf= _temp694 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**) _check_null(
_temp694)), _temp583); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL683;} _LL687: { struct Cyc_Absyn_Structfield*
sf= _temp700 == 0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)*((
struct Cyc_Absyn_Uniondecl**) _check_null( _temp700)), _temp583); if( sf == 0? 1:
!((( struct Cyc_Absyn_Structfield*) _check_null( sf))->tq).q_const){ return;}
goto _LL683;} _LL689: _temp708= _temp706; goto _LL691; _LL691: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp708, _temp583); if( sf == 0? 1: !((( struct Cyc_Absyn_Structfield*)
_check_null( sf))->tq).q_const){ return;} goto _LL683;} _LL693: goto _LL683;
_LL683:;} goto _LL665; _LL669: goto _LL665; _LL665:;} goto _LL532; _LL548:{ void*
_temp710= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp587->topt))->v); struct Cyc_Absyn_PtrInfo _temp718; struct Cyc_Absyn_Conref*
_temp720; struct Cyc_Absyn_Tqual _temp722; struct Cyc_Absyn_Conref* _temp724;
void* _temp726; void* _temp728; struct Cyc_Absyn_Exp* _temp730; struct Cyc_Absyn_Tqual
_temp732; void* _temp734; _LL712: if(( unsigned int) _temp710 > 4u?*(( int*)
_temp710) == Cyc_Absyn_PointerType: 0){ _LL719: _temp718=(( struct Cyc_Absyn_PointerType_struct*)
_temp710)->f1; _LL729: _temp728=( void*) _temp718.elt_typ; goto _LL727; _LL727:
_temp726=( void*) _temp718.rgn_typ; goto _LL725; _LL725: _temp724= _temp718.nullable;
goto _LL723; _LL723: _temp722= _temp718.tq; goto _LL721; _LL721: _temp720=
_temp718.bounds; goto _LL713;} else{ goto _LL714;} _LL714: if(( unsigned int)
_temp710 > 4u?*(( int*) _temp710) == Cyc_Absyn_ArrayType: 0){ _LL735: _temp734=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp710)->f1; goto _LL733; _LL733:
_temp732=(( struct Cyc_Absyn_ArrayType_struct*) _temp710)->f2; goto _LL731;
_LL731: _temp730=(( struct Cyc_Absyn_ArrayType_struct*) _temp710)->f3; goto
_LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL713: _temp732= _temp722;
goto _LL715; _LL715: if( ! _temp732.q_const){ return;} goto _LL711; _LL717: goto
_LL711; _LL711:;} goto _LL532; _LL550: goto _LL532; _LL532:;} Cyc_Tcutil_terr( e->loc,({
char* _temp736="attempt to write a read-only location"; struct _tagged_string
_temp737; _temp737.curr= _temp736; _temp737.base= _temp736; _temp737.last_plus_one=
_temp736 + 38; _temp737;}));} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void*
i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return
Cyc_Tcexp_expr_err( te, loc,({ char* _temp738="increment/decrement of non-lvalue";
struct _tagged_string _temp739; _temp739.curr= _temp738; _temp739.base= _temp738;
_temp739.last_plus_one= _temp738 + 34; _temp739;}));} Cyc_Tcexp_check_writable(
te, e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if(
! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ( t): 0){ Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp740= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic or ? type but found %.*s",
_temp740.last_plus_one - _temp740.curr, _temp740.curr);}));} return t;}} static
void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp*
e3){ Cyc_Tcexp_tcTest( te, e1,({ char* _temp741="conditional expression"; struct
_tagged_string _temp742; _temp742.curr= _temp741; _temp742.base= _temp741;
_temp742.last_plus_one= _temp741 + 23; _temp742;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* _temp744=({ struct Cyc_List_List* _temp743=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp743->hd=( void*) e3; _temp743->tl=
0; _temp743;}); goto _LL745; _LL745: { struct Cyc_List_List* _temp747=({ struct
Cyc_List_List* _temp746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp746->hd=( void*) e2; _temp746->tl= _temp744; _temp746;}); goto _LL748;
_LL748: if( ! Cyc_Tcutil_coerce_list( te, t, _temp747)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp749= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); struct _tagged_string _temp750= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e3->topt))->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp749.last_plus_one - _temp749.curr, _temp749.curr, _temp750.last_plus_one -
_temp750.curr, _temp750.curr);}));} return t;}}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v)), e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;{ void* _temp751= Cyc_Tcutil_compress( t1); struct
Cyc_Absyn_Exp* _temp757; struct Cyc_Absyn_Tqual _temp759; void* _temp761; _LL753:
if(( unsigned int) _temp751 > 4u?*(( int*) _temp751) == Cyc_Absyn_ArrayType: 0){
_LL762: _temp761=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp751)->f1;
goto _LL760; _LL760: _temp759=(( struct Cyc_Absyn_ArrayType_struct*) _temp751)->f2;
goto _LL758; _LL758: _temp757=(( struct Cyc_Absyn_ArrayType_struct*) _temp751)->f3;
goto _LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL754: Cyc_Tcutil_terr(
loc,({ char* _temp763="cannot assign to an array"; struct _tagged_string
_temp764; _temp764.curr= _temp763; _temp764.base= _temp763; _temp764.last_plus_one=
_temp763 + 26; _temp764;})); goto _LL752; _LL756: goto _LL752; _LL752:;} if( !
Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te, loc,({ char* _temp765="assignment to non-lvalue";
struct _tagged_string _temp766; _temp766.curr= _temp765; _temp766.base= _temp765;
_temp766.last_plus_one= _temp765 + 25; _temp766;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp767= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp768= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp767.last_plus_one - _temp767.curr, _temp767.curr, _temp768.last_plus_one -
_temp768.curr, _temp768.curr);}));}} else{ void* _temp769=( void*)(( struct Cyc_Core_Opt*)
_check_null( po))->v; goto _LL770; _LL770: { void* _temp771= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp769, e1, e2); goto _LL772; _LL772: if( !( Cyc_Tcutil_unify(
_temp771, t1)? 1: Cyc_Tcutil_coerceable( _temp771))){ return Cyc_Tcexp_expr_err(
te, loc,({ char* _temp773="Cannot use this operator in an assignment"; struct
_tagged_string _temp774; _temp774.curr= _temp773; _temp774.base= _temp773;
_temp774.last_plus_one= _temp773 + 42; _temp774;}));}}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v;} static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, void* tu, struct
_RegionHandle* r, struct Cyc_List_List* inst, struct Cyc_List_List* fs){ struct
Cyc_List_List* fields; void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp777; struct Cyc_Position_Segment* _temp779; struct Cyc_List_List*
_temp781; struct Cyc_List_List* _temp783; struct _tuple1* _temp785; struct Cyc_Absyn_Tunionfield
_temp775=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL786: _temp785= _temp775.name; goto _LL784; _LL784: _temp783=
_temp775.tvs; goto _LL782; _LL782: _temp781= _temp775.typs; goto _LL780; _LL780:
_temp779= _temp775.loc; goto _LL778; _LL778: _temp777=( void*) _temp775.sc; goto
_LL776; _LL776: if(( _temp781 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp781))->tl != 0)? 1: _temp783 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp781))->hd)).f2);
if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null( fields))->hd);}}}
for( fields= fs; fields != 0; fields=(( struct Cyc_List_List*) _check_null(
fields))->tl){ void* _temp789; struct Cyc_Position_Segment* _temp791; struct Cyc_List_List*
_temp793; struct Cyc_List_List* _temp795; struct _tuple1* _temp797; struct Cyc_Absyn_Tunionfield
_temp787=*(( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd); _LL798: _temp797= _temp787.name; goto _LL796; _LL796: _temp795=
_temp787.tvs; goto _LL794; _LL794: _temp793= _temp787.typs; goto _LL792; _LL792:
_temp791= _temp787.loc; goto _LL790; _LL790: _temp789=( void*) _temp787.sc; goto
_LL788; _LL788: if(( _temp793 == 0? 1:(( struct Cyc_List_List*) _check_null(
_temp793))->tl != 0)? 1: _temp795 != 0){ continue;}{ void* t2= Cyc_Tcutil_rsubstitute(
r, inst,(*(( struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp793))->hd)).f2);
if( Cyc_Tcutil_silent_castable( te, e->loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e, t2); return( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( fields))->hd);}}} Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp799= Cyc_Absynpp_typ2string( tu); struct
_tagged_string _temp800= Cyc_Absynpp_typ2string( t1); xprintf("can't find a field in %.*s to inject a value of type %.*s",
_temp799.last_plus_one - _temp799.curr, _temp799.curr, _temp800.last_plus_one -
_temp800.curr, _temp800.curr);})); return 0;} static void* Cyc_Tcexp_tcFnCall(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp801= t;
struct Cyc_Absyn_PtrInfo _temp807; struct Cyc_Absyn_Conref* _temp809; struct Cyc_Absyn_Tqual
_temp811; struct Cyc_Absyn_Conref* _temp813; void* _temp815; void* _temp817;
_LL803: if(( unsigned int) _temp801 > 4u?*(( int*) _temp801) == Cyc_Absyn_PointerType:
0){ _LL808: _temp807=(( struct Cyc_Absyn_PointerType_struct*) _temp801)->f1;
_LL818: _temp817=( void*) _temp807.elt_typ; goto _LL816; _LL816: _temp815=( void*)
_temp807.rgn_typ; goto _LL814; _LL814: _temp813= _temp807.nullable; goto _LL812;
_LL812: _temp811= _temp807.tq; goto _LL810; _LL810: _temp809= _temp807.bounds;
goto _LL804;} else{ goto _LL805;} _LL805: goto _LL806; _LL804: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp815); Cyc_Tcutil_check_nonzero_bound( loc, _temp809);{ void*
_temp819= Cyc_Tcutil_compress( _temp817); struct Cyc_Absyn_FnInfo _temp825;
struct Cyc_List_List* _temp827; struct Cyc_Absyn_VarargInfo* _temp829; int
_temp831; struct Cyc_List_List* _temp833; void* _temp835; struct Cyc_Core_Opt*
_temp837; struct Cyc_List_List* _temp839; _LL821: if(( unsigned int) _temp819 >
4u?*(( int*) _temp819) == Cyc_Absyn_FnType: 0){ _LL826: _temp825=(( struct Cyc_Absyn_FnType_struct*)
_temp819)->f1; _LL840: _temp839= _temp825.tvars; goto _LL838; _LL838: _temp837=
_temp825.effect; goto _LL836; _LL836: _temp835=( void*) _temp825.ret_typ; goto
_LL834; _LL834: _temp833= _temp825.args; goto _LL832; _LL832: _temp831= _temp825.c_varargs;
goto _LL830; _LL830: _temp829= _temp825.cyc_varargs; goto _LL828; _LL828:
_temp827= _temp825.attributes; goto _LL822;} else{ goto _LL823;} _LL823: goto
_LL824; _LL822: if( _temp831){ Cyc_Tcutil_terr( loc,({ char* _temp841="C-style vararg functions (...) not supported.";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 46; _temp842;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp835,*(( void**) _check_null( topt)));} while( es != 0? _temp833 != 0: 0) {
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd; void* t2=(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp833))->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( !
Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp843= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); struct _tagged_string _temp844= Cyc_Absynpp_typ2string(
t2); xprintf("actual argument has type %.*s but formal has type %.*s", _temp843.last_plus_one
- _temp843.curr, _temp843.curr, _temp844.last_plus_one - _temp844.curr, _temp844.curr);}));}
es=(( struct Cyc_List_List*) _check_null( es))->tl; _temp833=(( struct Cyc_List_List*)
_check_null( _temp833))->tl;} if( _temp833 != 0){ Cyc_Tcutil_terr( loc,({ char*
_temp845="too few arguments for function"; struct _tagged_string _temp846;
_temp846.curr= _temp845; _temp846.base= _temp845; _temp846.last_plus_one=
_temp845 + 31; _temp846;}));} else{ if( es != 0? 1: _temp829 != 0){ if( _temp829
== 0){ Cyc_Tcutil_terr( loc,({ char* _temp847="too many arguments for function";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 32; _temp848;}));} else{ int _temp851; void*
_temp853; void* _temp855; struct Cyc_Absyn_Tqual _temp857; struct Cyc_Core_Opt*
_temp859; struct Cyc_Absyn_VarargInfo _temp849=*(( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp829)); _LL860: _temp859= _temp849.name; goto _LL858; _LL858:
_temp857= _temp849.tq; goto _LL856; _LL856: _temp855=( void*) _temp849.type;
goto _LL854; _LL854: _temp853=( void*) _temp849.rgn; goto _LL852; _LL852:
_temp851= _temp849.inject; goto _LL850; _LL850: te= Cyc_Tcenv_new_block( loc, te);{
struct Cyc_Absyn_VarargCallInfo* _temp862=({ struct Cyc_Absyn_VarargCallInfo*
_temp861=( struct Cyc_Absyn_VarargCallInfo*) GC_malloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp861->num_varargs= 0; _temp861->injectors= 0; _temp861->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp829); _temp861;}); goto _LL863; _LL863:* vararg_call_info=
_temp862; if( ! Cyc_Tcutil_unify( Cyc_Tcenv_curr_rgn( te), _temp853)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp864= Cyc_Absynpp_typ2string( _temp853); struct
_tagged_string _temp865= Cyc_Absynpp_typ2string( Cyc_Tcenv_curr_rgn( te));
xprintf("I have no idea how this could fail, but it did because the region for the varargs (%.*s) doesn't unify with the expected new_block region (%.*s).",
_temp864.last_plus_one - _temp864.curr, _temp864.curr, _temp865.last_plus_one -
_temp865.curr, _temp865.curr);}));} if( ! _temp851){ for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ struct Cyc_Absyn_Exp* e1=( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd;(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp862))->num_varargs ++; Cyc_Tcexp_tcExp( te,( void**)& _temp855,
e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, _temp855)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp866= Cyc_Absynpp_typ2string( _temp855); struct
_tagged_string _temp867= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); xprintf("vararg requires type %.*s but argument has type %.*s",
_temp866.last_plus_one - _temp866.curr, _temp866.curr, _temp867.last_plus_one -
_temp867.curr, _temp867.curr);}));}}} else{ void* _temp868= Cyc_Tcutil_compress(
_temp855); struct Cyc_Absyn_TunionInfo _temp874; void* _temp876; struct Cyc_List_List*
_temp878; void* _temp880; struct Cyc_Absyn_Tuniondecl* _temp882; _LL870: if((
unsigned int) _temp868 > 4u?*(( int*) _temp868) == Cyc_Absyn_TunionType: 0){
_LL875: _temp874=(( struct Cyc_Absyn_TunionType_struct*) _temp868)->f1; _LL881:
_temp880=( void*) _temp874.tunion_info; if(*(( int*) _temp880) == Cyc_Absyn_KnownTunion){
_LL883: _temp882=(( struct Cyc_Absyn_KnownTunion_struct*) _temp880)->f1; goto
_LL879;} else{ goto _LL872;} _LL879: _temp878= _temp874.targs; goto _LL877;
_LL877: _temp876=( void*) _temp874.rgn; goto _LL871;} else{ goto _LL872;} _LL872:
goto _LL873; _LL871: { struct Cyc_Absyn_Tuniondecl* _temp884=* Cyc_Tcenv_lookup_tuniondecl(
te, loc, _temp882->name); goto _LL885; _LL885: if( ! Cyc_Tcutil_unify( _temp876,
_temp853)){ Cyc_Tcutil_terr( loc, xprintf("bad region for injected varargs"));}{
struct Cyc_List_List* fields= 0; if( _temp884->fields == 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp886= Cyc_Absynpp_typ2string( _temp855);
xprintf("can't inject into %.*s", _temp886.last_plus_one - _temp886.curr,
_temp886.curr);}));} else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp884->fields))->v;}{ struct _RegionHandle _temp887= _new_region();
struct _RegionHandle* rgn=& _temp887; _push_region( rgn);{ struct Cyc_List_List*
_temp888=(( struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct
_RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp884->tvs, _temp878); goto _LL889; _LL889: for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp862))->num_varargs ++;{ struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd; Cyc_Tcexp_tcExp( te, 0, e1);{
struct Cyc_Absyn_Tunionfield* _temp890= Cyc_Tcexp_tcInjection( te, e1, _temp855,
rgn, _temp888, fields); goto _LL891; _LL891: if( _temp890 != 0){(( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp862))->injectors=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)((( struct Cyc_Absyn_VarargCallInfo*)
_check_null( _temp862))->injectors,({ struct Cyc_List_List* _temp892=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp892->hd=( void*)((
struct Cyc_Absyn_Tunionfield*) _check_null( _temp890)); _temp892->tl= 0;
_temp892;}));}}}}}; _pop_region(& _temp887);} goto _LL869;}} _LL873: Cyc_Tcutil_terr(
loc,({ char* _temp893="bad inject vararg type"; struct _tagged_string _temp894;
_temp894.curr= _temp893; _temp894.base= _temp893; _temp894.last_plus_one=
_temp893 + 23; _temp894;})); goto _LL869; _LL869:;}}}}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp837))->v); return
_temp835; _LL824: return Cyc_Tcexp_expr_err( te, loc,({ char* _temp895="expected pointer to function";
struct _tagged_string _temp896; _temp896.curr= _temp895; _temp896.base= _temp895;
_temp896.last_plus_one= _temp895 + 29; _temp896;})); _LL820:;} _LL806: return
Cyc_Tcexp_expr_err( te, loc,({ char* _temp897="expected pointer to function";
struct _tagged_string _temp898; _temp898.curr= _temp897; _temp898.base= _temp897;
_temp898.last_plus_one= _temp897 + 29; _temp898;})); _LL802:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp899= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_temp899.last_plus_one - _temp899.curr, _temp899.curr);}));} return Cyc_Absyn_wildtyp();}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp900= t1; struct Cyc_Absyn_PtrInfo
_temp906; struct Cyc_Absyn_Conref* _temp908; struct Cyc_Absyn_Tqual _temp910;
struct Cyc_Absyn_Conref* _temp912; void* _temp914; void* _temp916; _LL902: if((
unsigned int) _temp900 > 4u?*(( int*) _temp900) == Cyc_Absyn_PointerType: 0){
_LL907: _temp906=(( struct Cyc_Absyn_PointerType_struct*) _temp900)->f1; _LL917:
_temp916=( void*) _temp906.elt_typ; goto _LL915; _LL915: _temp914=( void*)
_temp906.rgn_typ; goto _LL913; _LL913: _temp912= _temp906.nullable; goto _LL911;
_LL911: _temp910= _temp906.tq; goto _LL909; _LL909: _temp908= _temp906.bounds;
goto _LL903;} else{ goto _LL904;} _LL904: goto _LL905; _LL903:{ void* _temp918=
Cyc_Tcutil_compress( _temp916); struct Cyc_Absyn_FnInfo _temp924; struct Cyc_List_List*
_temp926; struct Cyc_Absyn_VarargInfo* _temp928; int _temp930; struct Cyc_List_List*
_temp932; void* _temp934; struct Cyc_Core_Opt* _temp936; struct Cyc_List_List*
_temp938; _LL920: if(( unsigned int) _temp918 > 4u?*(( int*) _temp918) == Cyc_Absyn_FnType:
0){ _LL925: _temp924=(( struct Cyc_Absyn_FnType_struct*) _temp918)->f1; _LL939:
_temp938= _temp924.tvars; goto _LL937; _LL937: _temp936= _temp924.effect; goto
_LL935; _LL935: _temp934=( void*) _temp924.ret_typ; goto _LL933; _LL933:
_temp932= _temp924.args; goto _LL931; _LL931: _temp930= _temp924.c_varargs; goto
_LL929; _LL929: _temp928= _temp924.cyc_varargs; goto _LL927; _LL927: _temp926=
_temp924.attributes; goto _LL921;} else{ goto _LL922;} _LL922: goto _LL923;
_LL921: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp938)){ return Cyc_Tcexp_expr_err( te, loc,({ char* _temp940="type instantiation mismatch";
struct _tagged_string _temp941; _temp941.curr= _temp940; _temp941.base= _temp940;
_temp941.last_plus_one= _temp940 + 28; _temp941;}));} for( 0; ts != 0;( ts=((
struct Cyc_List_List*) _check_null( ts))->tl, _temp938=(( struct Cyc_List_List*)
_check_null( _temp938))->tl)){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( _temp938))->hd)->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)(( struct Cyc_List_List*) _check_null( ts))->hd); instantiation=({
struct Cyc_List_List* _temp942=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp942->hd=( void*)({ struct _tuple4* _temp943=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp943->f1=( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp938))->hd; _temp943->f2=( void*)((
struct Cyc_List_List*) _check_null( ts))->hd; _temp943;}); _temp942->tl=
instantiation; _temp942;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp947=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp947[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp948; _temp948.tag= Cyc_Absyn_FnType; _temp948.f1=({ struct Cyc_Absyn_FnInfo
_temp949; _temp949.tvars= 0; _temp949.effect= _temp936; _temp949.ret_typ=( void*)
_temp934; _temp949.args= _temp932; _temp949.c_varargs= _temp930; _temp949.cyc_varargs=
_temp928; _temp949.attributes= _temp926; _temp949;}); _temp948;}); _temp947;}));
void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp944=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp944[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp945; _temp945.tag= Cyc_Absyn_PointerType;
_temp945.f1=({ struct Cyc_Absyn_PtrInfo _temp946; _temp946.elt_typ=( void*)
new_fn_typ; _temp946.rgn_typ=( void*) _temp914; _temp946.nullable= _temp912;
_temp946.tq= _temp910; _temp946.bounds= _temp908; _temp946;}); _temp945;});
_temp944;}); return new_typ;}} _LL923: goto _LL919; _LL919:;} goto _LL901;
_LL905: goto _LL901; _LL901:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp950= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp950.last_plus_one - _temp950.curr, _temp950.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp951= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp952= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp951.last_plus_one - _temp951.curr, _temp951.curr, _temp952.last_plus_one -
_temp952.curr, _temp952.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** _temp953= 0; goto _LL954; _LL954: { struct Cyc_Absyn_Tqual
_temp955= Cyc_Absyn_empty_tqual(); goto _LL956; _LL956: if( topt != 0){ void*
_temp957= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp963; struct Cyc_Absyn_Conref* _temp965; struct Cyc_Absyn_Tqual _temp967;
struct Cyc_Absyn_Conref* _temp969; void* _temp971; void* _temp973; _LL959: if((
unsigned int) _temp957 > 4u?*(( int*) _temp957) == Cyc_Absyn_PointerType: 0){
_LL964: _temp963=(( struct Cyc_Absyn_PointerType_struct*) _temp957)->f1; _LL974:
_temp973=( void*) _temp963.elt_typ; goto _LL972; _LL972: _temp971=( void*)
_temp963.rgn_typ; goto _LL970; _LL970: _temp969= _temp963.nullable; goto _LL968;
_LL968: _temp967= _temp963.tq; goto _LL966; _LL966: _temp965= _temp963.bounds;
goto _LL960;} else{ goto _LL961;} _LL961: goto _LL962; _LL960: _temp953=({ void**
_temp975=( void**) GC_malloc( sizeof( void*)); _temp975[ 0]= _temp973; _temp975;});
_temp955= _temp967; goto _LL958; _LL962: goto _LL958; _LL958:;} Cyc_Tcexp_tcExpNoInst(
te, _temp953, e);{ void* _temp976=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp984; struct Cyc_List_List* _temp986; struct Cyc_Core_Opt* _temp988; struct
_tuple1* _temp990; struct Cyc_List_List* _temp992; _LL978: if(*(( int*) _temp976)
== Cyc_Absyn_Struct_e){ _LL991: _temp990=(( struct Cyc_Absyn_Struct_e_struct*)
_temp976)->f1; goto _LL989; _LL989: _temp988=(( struct Cyc_Absyn_Struct_e_struct*)
_temp976)->f2; goto _LL987; _LL987: _temp986=(( struct Cyc_Absyn_Struct_e_struct*)
_temp976)->f3; goto _LL985; _LL985: _temp984=(( struct Cyc_Absyn_Struct_e_struct*)
_temp976)->f4; goto _LL979;} else{ goto _LL980;} _LL980: if(*(( int*) _temp976)
== Cyc_Absyn_Tuple_e){ _LL993: _temp992=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp976)->f1; goto _LL981;} else{ goto _LL982;} _LL982: goto _LL983; _LL979:
goto _LL981; _LL981: Cyc_Tcutil_warn( loc,({ char* _temp994="& used to allocate";
struct _tagged_string _temp995; _temp995.curr= _temp994; _temp995.base= _temp994;
_temp995.last_plus_one= _temp994 + 19; _temp995;}));{ void*(* _temp996)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL997; _LL997:
if( topt != 0){ void* _temp998= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp1004; struct Cyc_Absyn_Conref* _temp1006;
struct Cyc_Absyn_Tqual _temp1008; struct Cyc_Absyn_Conref* _temp1010; void*
_temp1012; void* _temp1014; _LL1000: if(( unsigned int) _temp998 > 4u?*(( int*)
_temp998) == Cyc_Absyn_PointerType: 0){ _LL1005: _temp1004=(( struct Cyc_Absyn_PointerType_struct*)
_temp998)->f1; _LL1015: _temp1014=( void*) _temp1004.elt_typ; goto _LL1013;
_LL1013: _temp1012=( void*) _temp1004.rgn_typ; goto _LL1011; _LL1011: _temp1010=
_temp1004.nullable; goto _LL1009; _LL1009: _temp1008= _temp1004.tq; goto _LL1007;
_LL1007: _temp1006= _temp1004.bounds; goto _LL1001;} else{ goto _LL1002;}
_LL1002: goto _LL1003; _LL1001: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1010)){ _temp996= Cyc_Absyn_star_typ;} goto _LL999; _LL1003: goto _LL999;
_LL999:;} return _temp996(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp955);} _LL983: goto _LL977; _LL977:;}{ void*
_temp1018; int _temp1020; struct _tuple5 _temp1016= Cyc_Tcutil_addressof_props(
te, e); _LL1021: _temp1020= _temp1016.f1; goto _LL1019; _LL1019: _temp1018=
_temp1016.f2; goto _LL1017; _LL1017: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
if( _temp1020){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp1018, tq); return t;}}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1022= t;
struct Cyc_Absyn_PtrInfo _temp1028; struct Cyc_Absyn_Conref* _temp1030; struct
Cyc_Absyn_Tqual _temp1032; struct Cyc_Absyn_Conref* _temp1034; void* _temp1036;
void* _temp1038; _LL1024: if(( unsigned int) _temp1022 > 4u?*(( int*) _temp1022)
== Cyc_Absyn_PointerType: 0){ _LL1029: _temp1028=(( struct Cyc_Absyn_PointerType_struct*)
_temp1022)->f1; _LL1039: _temp1038=( void*) _temp1028.elt_typ; goto _LL1037;
_LL1037: _temp1036=( void*) _temp1028.rgn_typ; goto _LL1035; _LL1035: _temp1034=
_temp1028.nullable; goto _LL1033; _LL1033: _temp1032= _temp1028.tq; goto _LL1031;
_LL1031: _temp1030= _temp1028.bounds; goto _LL1025;} else{ goto _LL1026;}
_LL1026: goto _LL1027; _LL1025: Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1036); Cyc_Tcutil_check_nonzero_bound( loc, _temp1030); return _temp1038;
_LL1027: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1040=
Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp1040.last_plus_one - _temp1040.curr, _temp1040.curr);})); _LL1023:;}}
static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1041= t;
struct Cyc_Absyn_Structdecl** _temp1057; struct Cyc_Absyn_Structdecl* _temp1059;
struct Cyc_List_List* _temp1060; struct _tuple1* _temp1062; struct Cyc_List_List*
_temp1064; struct Cyc_List_List* _temp1066; struct Cyc_Absyn_Uniondecl**
_temp1068; struct Cyc_Absyn_Uniondecl* _temp1070; struct Cyc_List_List*
_temp1071; struct _tuple1* _temp1073; struct Cyc_Absyn_Exp* _temp1076; struct
Cyc_Absyn_Tqual _temp1078; void* _temp1080; struct Cyc_Absyn_PtrInfo _temp1085;
_LL1043: if(( unsigned int) _temp1041 > 4u?*(( int*) _temp1041) == Cyc_Absyn_StructType:
0){ _LL1063: _temp1062=(( struct Cyc_Absyn_StructType_struct*) _temp1041)->f1;
goto _LL1061; _LL1061: _temp1060=(( struct Cyc_Absyn_StructType_struct*)
_temp1041)->f2; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_StructType_struct*)
_temp1041)->f3; if( _temp1057 == 0){ goto _LL1045;} else{ _temp1059=* _temp1057;
goto _LL1044;}} else{ goto _LL1045;} _LL1045: if(( unsigned int) _temp1041 > 4u?*((
int*) _temp1041) == Cyc_Absyn_AnonStructType: 0){ _LL1065: _temp1064=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1041)->f1; goto _LL1046;} else{ goto
_LL1047;} _LL1047: if(( unsigned int) _temp1041 > 4u?*(( int*) _temp1041) == Cyc_Absyn_AnonUnionType:
0){ _LL1067: _temp1066=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1041)->f1;
goto _LL1048;} else{ goto _LL1049;} _LL1049: if(( unsigned int) _temp1041 > 4u?*((
int*) _temp1041) == Cyc_Absyn_UnionType: 0){ _LL1074: _temp1073=(( struct Cyc_Absyn_UnionType_struct*)
_temp1041)->f1; goto _LL1072; _LL1072: _temp1071=(( struct Cyc_Absyn_UnionType_struct*)
_temp1041)->f2; goto _LL1069; _LL1069: _temp1068=(( struct Cyc_Absyn_UnionType_struct*)
_temp1041)->f3; if( _temp1068 == 0){ goto _LL1051;} else{ _temp1070=* _temp1068;
goto _LL1050;}} else{ goto _LL1051;} _LL1051: if(( unsigned int) _temp1041 > 4u?*((
int*) _temp1041) == Cyc_Absyn_ArrayType: 0){ _LL1081: _temp1080=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1041)->f1; goto _LL1079; _LL1079:
_temp1078=(( struct Cyc_Absyn_ArrayType_struct*) _temp1041)->f2; goto _LL1077;
_LL1077: _temp1076=(( struct Cyc_Absyn_ArrayType_struct*) _temp1041)->f3; goto
_LL1075;} else{ goto _LL1053;} _LL1075: if( Cyc_String_zstrcmp(* f,({ char*
_temp1082="size"; struct _tagged_string _temp1083; _temp1083.curr= _temp1082;
_temp1083.base= _temp1082; _temp1083.last_plus_one= _temp1082 + 5; _temp1083;}))
== 0){ goto _LL1052;} else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1041
> 4u?*(( int*) _temp1041) == Cyc_Absyn_PointerType: 0){ _LL1086: _temp1085=((
struct Cyc_Absyn_PointerType_struct*) _temp1041)->f1; goto _LL1084;} else{ goto
_LL1055;} _LL1084: if( Cyc_String_zstrcmp(* f,({ char* _temp1087="size"; struct
_tagged_string _temp1088; _temp1088.curr= _temp1087; _temp1088.base= _temp1087;
_temp1088.last_plus_one= _temp1087 + 5; _temp1088;})) == 0){ goto _LL1054;}
else{ goto _LL1055;} _LL1055: goto _LL1056; _LL1044: if( _temp1062 == 0){ return
Cyc_Tcexp_expr_err( te, loc,({ char* _temp1089="unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1090; _temp1090.curr= _temp1089; _temp1090.base=
_temp1089; _temp1090.last_plus_one= _temp1089 + 40; _temp1090;}));}{ struct Cyc_Absyn_Structfield*
_temp1091= Cyc_Absyn_lookup_struct_field( _temp1059, f); goto _LL1092; _LL1092:
if( _temp1091 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1093= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1062));
struct _tagged_string _temp1094=* f; xprintf("struct %.*s has no %.*s field",
_temp1093.last_plus_one - _temp1093.curr, _temp1093.curr, _temp1094.last_plus_one
- _temp1094.curr, _temp1094.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1095= _new_region(); struct _RegionHandle* rgn=& _temp1095; _push_region(
rgn);{ struct Cyc_List_List* _temp1096=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1059->tvs, _temp1060); goto _LL1097; _LL1097:
t2= Cyc_Tcutil_rsubstitute( rgn, _temp1096,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1091))->type);}; _pop_region(& _temp1095);} return t2;}}
_LL1046: { struct Cyc_Absyn_Structfield* _temp1098= Cyc_Absyn_lookup_field(
_temp1064, f); goto _LL1099; _LL1099: if( _temp1098 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1100=* f; xprintf("struct has no %.*s field",
_temp1100.last_plus_one - _temp1100.curr, _temp1100.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1098))->type;} _LL1048: {
struct Cyc_Absyn_Structfield* _temp1101= Cyc_Absyn_lookup_field( _temp1066, f);
goto _LL1102; _LL1102: if( _temp1101 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1103=* f; xprintf("union has no %.*s field",
_temp1103.last_plus_one - _temp1103.curr, _temp1103.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1101))->type;} _LL1050: if(
_temp1073 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ char* _temp1104="unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp1105; _temp1105.curr= _temp1104; _temp1105.base=
_temp1104; _temp1105.last_plus_one= _temp1104 + 39; _temp1105;}));}{ struct Cyc_Absyn_Structfield*
_temp1106= Cyc_Absyn_lookup_union_field( _temp1070, f); goto _LL1107; _LL1107:
if( _temp1106 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1108= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1073));
struct _tagged_string _temp1109=* f; xprintf("union %.*s has no %.*s field",
_temp1108.last_plus_one - _temp1108.curr, _temp1108.curr, _temp1109.last_plus_one
- _temp1109.curr, _temp1109.curr);}));}{ struct Cyc_List_List* _temp1110=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1070->tvs, _temp1071); goto _LL1111; _LL1111: { void* _temp1112= Cyc_Tcutil_substitute(
_temp1110,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1106))->type);
goto _LL1113; _LL1113: return _temp1112;}}} _LL1052:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1114=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1114[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1115; _temp1115.tag= Cyc_Absyn_Primop_e;
_temp1115.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1115.f2=({ struct Cyc_List_List*
_temp1116=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1116->hd=( void*) e; _temp1116->tl= 0; _temp1116;}); _temp1115;});
_temp1114;}))); return Cyc_Absyn_uint_t; _LL1054:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1117=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1117[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1118; _temp1118.tag= Cyc_Absyn_Primop_e;
_temp1118.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1118.f2=({ struct Cyc_List_List*
_temp1119=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1119->hd=( void*) e; _temp1119->tl= 0; _temp1119;}); _temp1118;});
_temp1117;}))); return Cyc_Absyn_uint_t; _LL1056: if( Cyc_String_zstrcmp(* f,({
char* _temp1120="size"; struct _tagged_string _temp1121; _temp1121.curr=
_temp1120; _temp1121.base= _temp1120; _temp1121.last_plus_one= _temp1120 + 5;
_temp1121;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1122= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1122.last_plus_one - _temp1122.curr, _temp1122.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1123= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1123.last_plus_one - _temp1123.curr, _temp1123.curr);}));} _LL1042:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1124= t; struct Cyc_Absyn_PtrInfo
_temp1130; struct Cyc_Absyn_Conref* _temp1132; struct Cyc_Absyn_Tqual _temp1134;
struct Cyc_Absyn_Conref* _temp1136; void* _temp1138; void* _temp1140; _LL1126:
if(( unsigned int) _temp1124 > 4u?*(( int*) _temp1124) == Cyc_Absyn_PointerType:
0){ _LL1131: _temp1130=(( struct Cyc_Absyn_PointerType_struct*) _temp1124)->f1;
_LL1141: _temp1140=( void*) _temp1130.elt_typ; goto _LL1139; _LL1139: _temp1138=(
void*) _temp1130.rgn_typ; goto _LL1137; _LL1137: _temp1136= _temp1130.nullable;
goto _LL1135; _LL1135: _temp1134= _temp1130.tq; goto _LL1133; _LL1133: _temp1132=
_temp1130.bounds; goto _LL1127;} else{ goto _LL1128;} _LL1128: goto _LL1129;
_LL1127: Cyc_Tcutil_check_nonzero_bound( loc, _temp1132);{ void* _temp1142= Cyc_Tcutil_compress(
_temp1140); struct Cyc_Absyn_Structdecl** _temp1154; struct Cyc_Absyn_Structdecl*
_temp1156; struct Cyc_List_List* _temp1157; struct _tuple1* _temp1159; struct
Cyc_Absyn_Uniondecl** _temp1161; struct Cyc_Absyn_Uniondecl* _temp1163; struct
Cyc_List_List* _temp1164; struct _tuple1* _temp1166; struct Cyc_List_List*
_temp1168; struct Cyc_List_List* _temp1170; _LL1144: if(( unsigned int)
_temp1142 > 4u?*(( int*) _temp1142) == Cyc_Absyn_StructType: 0){ _LL1160:
_temp1159=(( struct Cyc_Absyn_StructType_struct*) _temp1142)->f1; goto _LL1158;
_LL1158: _temp1157=(( struct Cyc_Absyn_StructType_struct*) _temp1142)->f2; goto
_LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_StructType_struct*) _temp1142)->f3;
if( _temp1154 == 0){ goto _LL1146;} else{ _temp1156=* _temp1154; goto _LL1145;}}
else{ goto _LL1146;} _LL1146: if(( unsigned int) _temp1142 > 4u?*(( int*)
_temp1142) == Cyc_Absyn_UnionType: 0){ _LL1167: _temp1166=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f1; goto _LL1165; _LL1165: _temp1164=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f2; goto _LL1162; _LL1162: _temp1161=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f3; if( _temp1161 == 0){ goto _LL1148;} else{ _temp1163=* _temp1161;
goto _LL1147;}} else{ goto _LL1148;} _LL1148: if(( unsigned int) _temp1142 > 4u?*((
int*) _temp1142) == Cyc_Absyn_AnonStructType: 0){ _LL1169: _temp1168=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1142)->f1; goto _LL1149;} else{ goto
_LL1150;} _LL1150: if(( unsigned int) _temp1142 > 4u?*(( int*) _temp1142) == Cyc_Absyn_AnonUnionType:
0){ _LL1171: _temp1170=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1142)->f1;
goto _LL1151;} else{ goto _LL1152;} _LL1152: goto _LL1153; _LL1145: { struct Cyc_Absyn_Structfield*
_temp1172= Cyc_Absyn_lookup_struct_field( _temp1156, f); goto _LL1173; _LL1173:
if( _temp1172 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1174= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1159));
struct _tagged_string _temp1175=* f; xprintf("struct %.*s has no %.*s field",
_temp1174.last_plus_one - _temp1174.curr, _temp1174.curr, _temp1175.last_plus_one
- _temp1175.curr, _temp1175.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1176= _new_region(); struct _RegionHandle* rgn=& _temp1176; _push_region(
rgn);{ struct Cyc_List_List* _temp1177=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1156->tvs, _temp1157); goto _LL1178; _LL1178:
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1177,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1172))->type);}; _pop_region(& _temp1176);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1138); return t3;}} _LL1147: { struct Cyc_Absyn_Structfield*
_temp1179= Cyc_Absyn_lookup_union_field( _temp1163, f); goto _LL1180; _LL1180:
if( _temp1179 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1181= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1166));
struct _tagged_string _temp1182=* f; xprintf("union %.*s has no %.*s field",
_temp1181.last_plus_one - _temp1181.curr, _temp1181.curr, _temp1182.last_plus_one
- _temp1182.curr, _temp1182.curr);}));}{ struct Cyc_List_List* _temp1183=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1163->tvs, _temp1164); goto _LL1184; _LL1184: { void* _temp1185= Cyc_Tcutil_substitute(
_temp1183,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1179))->type);
goto _LL1186; _LL1186: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1138);
return _temp1185;}}} _LL1149: { struct Cyc_Absyn_Structfield* _temp1187= Cyc_Absyn_lookup_field(
_temp1168, f); goto _LL1188; _LL1188: if( _temp1187 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1189=* f; xprintf("struct has no %.*s field",
_temp1189.last_plus_one - _temp1189.curr, _temp1189.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1187))->type;} _LL1151: {
struct Cyc_Absyn_Structfield* _temp1190= Cyc_Absyn_lookup_field( _temp1170, f);
goto _LL1191; _LL1191: if( _temp1190 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1192=* f; xprintf("union has no %.*s field",
_temp1192.last_plus_one - _temp1192.curr, _temp1192.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1190))->type;} _LL1153: return
Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1193= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp1193.last_plus_one -
_temp1193.curr, _temp1193.curr);})); _LL1143:;} _LL1129: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1194= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1194.last_plus_one - _temp1194.curr, _temp1194.curr);})); _LL1125:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1195; _push_handler(& _temp1195);{ int
_temp1197= 0; if( setjmp( _temp1195.handler)){ _temp1197= 1;} if( ! _temp1197){{
void* _temp1198=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1198;}; _pop_handler();} else{
void* _temp1196=( void*) _exn_thrown; void* _temp1200= _temp1196; _LL1202: if(
_temp1200 == Cyc_List_Nth){ goto _LL1203;} else{ goto _LL1204;} _LL1204: goto
_LL1205; _LL1203: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1205:(
void) _throw( _temp1200); _LL1201:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string _temp1206= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int subscript, found %.*s", _temp1206.last_plus_one -
_temp1206.curr, _temp1206.curr);}));}{ void* _temp1207= t1; struct Cyc_Absyn_PtrInfo
_temp1215; struct Cyc_Absyn_Conref* _temp1217; struct Cyc_Absyn_Tqual _temp1219;
struct Cyc_Absyn_Conref* _temp1221; void* _temp1223; void* _temp1225; struct Cyc_List_List*
_temp1227; _LL1209: if(( unsigned int) _temp1207 > 4u?*(( int*) _temp1207) ==
Cyc_Absyn_PointerType: 0){ _LL1216: _temp1215=(( struct Cyc_Absyn_PointerType_struct*)
_temp1207)->f1; _LL1226: _temp1225=( void*) _temp1215.elt_typ; goto _LL1224;
_LL1224: _temp1223=( void*) _temp1215.rgn_typ; goto _LL1222; _LL1222: _temp1221=
_temp1215.nullable; goto _LL1220; _LL1220: _temp1219= _temp1215.tq; goto _LL1218;
_LL1218: _temp1217= _temp1215.bounds; goto _LL1210;} else{ goto _LL1211;}
_LL1211: if(( unsigned int) _temp1207 > 4u?*(( int*) _temp1207) == Cyc_Absyn_TupleType:
0){ _LL1228: _temp1227=(( struct Cyc_Absyn_TupleType_struct*) _temp1207)->f1;
goto _LL1212;} else{ goto _LL1213;} _LL1213: goto _LL1214; _LL1210: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1217);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1217);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1223); return _temp1225; _LL1212: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1227, e2); _LL1214: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1229= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1229.last_plus_one - _temp1229.curr, _temp1229.curr);})); _LL1208:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1230= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1236; _LL1232: if((
unsigned int) _temp1230 > 4u?*(( int*) _temp1230) == Cyc_Absyn_TupleType: 0){
_LL1237: _temp1236=(( struct Cyc_Absyn_TupleType_struct*) _temp1230)->f1; goto
_LL1233;} else{ goto _LL1234;} _LL1234: goto _LL1235; _LL1233: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1236) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,({ char* _temp1238="tuple expression has the wrong number of fields";
struct _tagged_string _temp1239; _temp1239.curr= _temp1238; _temp1239.base=
_temp1238; _temp1239.last_plus_one= _temp1238 + 48; _temp1239;})); goto _LL1231;}
for( 0; es != 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1236=((
struct Cyc_List_List*) _check_null( _temp1236))->tl)){ void* _temp1240=(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp1236))->hd)).f2;
goto _LL1241; _LL1241: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1240,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1242=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1242->hd=( void*)({ struct _tuple7* _temp1243=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1243->f1=(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( _temp1236))->hd)).f1; _temp1243->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1243;}); _temp1242->tl=
fields; _temp1242;});} done= 1; goto _LL1231; _LL1235: goto _LL1231; _LL1231:;}
if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1244=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1244->hd=( void*)({ struct
_tuple7* _temp1245=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1245->f1= Cyc_Absyn_empty_tqual(); _temp1245->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1245;}); _temp1244->tl= fields; _temp1244;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1246=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1246[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1247; _temp1247.tag= Cyc_Absyn_TupleType;
_temp1247.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1247;}); _temp1246;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,({
char* _temp1248="tcCompoundLit"; struct _tagged_string _temp1249; _temp1249.curr=
_temp1248; _temp1249.base= _temp1248; _temp1249.last_plus_one= _temp1248 + 14;
_temp1249;}));} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_Absyn_Const_e_struct* _temp1254=({ struct Cyc_Absyn_Const_e_struct*
_temp1250=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1250[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1251; _temp1251.tag= Cyc_Absyn_Const_e;
_temp1251.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1252=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1252[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1253; _temp1253.tag= Cyc_Absyn_Int_c;
_temp1253.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1253.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1253;}); _temp1252;}));
_temp1251;}); _temp1250;}); goto _LL1255; _LL1255: { struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1254, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1256=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1256->v=( void*) Cyc_Absyn_uint_t; _temp1256;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1270=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1270[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1271; _temp1271.tag= Cyc_Absyn_ArrayType;
_temp1271.f1=( void*) res; _temp1271.f2= Cyc_Absyn_empty_tqual(); _temp1271.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1271;}); _temp1270;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp1257= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1257.last_plus_one - _temp1257.curr, _temp1257.curr);}));}{ int offset= 0;
for( 0; des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1258=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_string* _temp1264; struct Cyc_Absyn_Exp*
_temp1266; _LL1260: if(*(( int*) _temp1258) == Cyc_Absyn_FieldName){ _LL1265:
_temp1264=(( struct Cyc_Absyn_FieldName_struct*) _temp1258)->f1; goto _LL1261;}
else{ goto _LL1262;} _LL1262: if(*(( int*) _temp1258) == Cyc_Absyn_ArrayElement){
_LL1267: _temp1266=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1258)->f1;
goto _LL1263;} else{ goto _LL1259;} _LL1261: Cyc_Tcutil_terr( loc,({ char*
_temp1268="only array index designators are supported"; struct _tagged_string
_temp1269; _temp1269.curr= _temp1268; _temp1269.base= _temp1268; _temp1269.last_plus_one=
_temp1268 + 43; _temp1269;})); goto _LL1259; _LL1263: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp1266);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1266);
if( i != offset){ Cyc_Tcutil_terr( _temp1266->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1259;} _LL1259:;}}} return res_t2;}}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1272= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_temp1272.last_plus_one - _temp1272.curr, _temp1272.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp1273="comprehension index variable is not declared const!";
struct _tagged_string _temp1274; _temp1274.curr= _temp1273; _temp1274.base=
_temp1273; _temp1274.last_plus_one= _temp1273 + 52; _temp1274;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,({
char* _temp1275="bound is not constant"; struct _tagged_string _temp1276;
_temp1276.curr= _temp1275; _temp1276.base= _temp1275; _temp1276.last_plus_one=
_temp1275 + 22; _temp1276;}));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc,({ char* _temp1277="body is not constant"; struct _tagged_string
_temp1278; _temp1278.curr= _temp1277; _temp1278.base= _temp1277; _temp1278.last_plus_one=
_temp1277 + 21; _temp1278;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void**
_temp1279= 0; goto _LL1280; _LL1280: { struct Cyc_Absyn_Tqual* _temp1281= 0;
goto _LL1282; _LL1282: if( topt != 0){ void* _temp1283= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1291; struct Cyc_Absyn_Exp*
_temp1293; struct Cyc_Absyn_Tqual _temp1295; struct Cyc_Absyn_Tqual* _temp1297;
void* _temp1298; void** _temp1300; _LL1285: if(( unsigned int) _temp1283 > 4u?*((
int*) _temp1283) == Cyc_Absyn_PointerType: 0){ _LL1292: _temp1291=(( struct Cyc_Absyn_PointerType_struct*)
_temp1283)->f1; goto _LL1286;} else{ goto _LL1287;} _LL1287: if(( unsigned int)
_temp1283 > 4u?*(( int*) _temp1283) == Cyc_Absyn_ArrayType: 0){ _LL1299:
_temp1298=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1283)->f1;
_temp1300=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1283)->f1); goto
_LL1296; _LL1296: _temp1295=(( struct Cyc_Absyn_ArrayType_struct*) _temp1283)->f2;
_temp1297=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1283)->f2; goto _LL1294;
_LL1294: _temp1293=(( struct Cyc_Absyn_ArrayType_struct*) _temp1283)->f3; goto
_LL1288;} else{ goto _LL1289;} _LL1289: goto _LL1290; _LL1286: pinfo= _temp1291;
_temp1279=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1281=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1284; _LL1288: _temp1279=( void**) _temp1300; _temp1281=(
struct Cyc_Absyn_Tqual*) _temp1297; goto _LL1284; _LL1290: goto _LL1284; _LL1284:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1279, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1301=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1301[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1302; _temp1302.tag= Cyc_Absyn_ArrayType;
_temp1302.f1=( void*) t; _temp1302.f2= _temp1281 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1281)); _temp1302.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1302;}); _temp1301;});}}}} struct _tuple8{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1303; _push_handler(&
_temp1303);{ int _temp1305= 0; if( setjmp( _temp1303.handler)){ _temp1305= 1;}
if( ! _temp1305){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1304=( void*) _exn_thrown; void* _temp1307= _temp1304; _LL1309:
if( _temp1307 == Cyc_Dict_Absent){ goto _LL1310;} else{ goto _LL1311;} _LL1311:
goto _LL1312; _LL1310: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1313=
Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s", _temp1313.last_plus_one
- _temp1313.curr, _temp1313.curr);})); return topt != 0?*(( void**) _check_null(
topt)):( void*) Cyc_Absyn_VoidType; _LL1312:( void) _throw( _temp1307); _LL1308:;}}}*
sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=( struct
_tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{ struct
_RegionHandle _temp1314= _new_region(); struct _RegionHandle* rgn=& _temp1314;
_push_region( rgn);{ struct Cyc_List_List* _temp1315=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); goto _LL1316; _LL1316: { struct Cyc_List_List* _temp1317=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1315); goto
_LL1318; _LL1318: { struct Cyc_Absyn_StructType_struct* _temp1322=({ struct Cyc_Absyn_StructType_struct*
_temp1319=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1319[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1320; _temp1320.tag=
Cyc_Absyn_StructType; _temp1320.f1=( struct _tuple1*)* tn; _temp1320.f2=
_temp1317; _temp1320.f3=({ struct Cyc_Absyn_Structdecl** _temp1321=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1321[ 0]= sd; _temp1321;});
_temp1320;}); _temp1319;}); goto _LL1323; _LL1323: if( topt != 0){ Cyc_Tcutil_unify((
void*) _temp1322,*(( void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt*
_temp1324=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1324->v=( void*) _temp1317; _temp1324;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,({ char* _temp1325="can't build abstract struct"; struct _tagged_string
_temp1326; _temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 28; _temp1326;}));{ void* _temp1327=( void*) _temp1322;
_npop_handler( 0u); return _temp1327;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple8 _temp1330; struct Cyc_Absyn_Exp* _temp1331; struct Cyc_Absyn_Structfield*
_temp1333; struct _tuple8* _temp1328=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1330=* _temp1328; _LL1334: _temp1333= _temp1330.f1;
goto _LL1332; _LL1332: _temp1331= _temp1330.f2; goto _LL1329; _LL1329: { void*
_temp1335= Cyc_Tcutil_rsubstitute( rgn, _temp1315,( void*) _temp1333->type);
goto _LL1336; _LL1336: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1335,
_temp1331); if( ! Cyc_Tcutil_coerce_arg( te, _temp1331, _temp1335)){ Cyc_Tcutil_terr(
_temp1331->loc,({ struct _tagged_string _temp1337=* _temp1333->name; struct
_tagged_string _temp1338= Cyc_Absynpp_qvar2string(* tn); struct _tagged_string
_temp1339= Cyc_Absynpp_typ2string( _temp1335); struct _tagged_string _temp1340=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1331->topt))->v);
xprintf("field %.*s of struct %.*s expects type %.*s != %.*s", _temp1337.last_plus_one
- _temp1337.curr, _temp1337.curr, _temp1338.last_plus_one - _temp1338.curr,
_temp1338.curr, _temp1339.last_plus_one - _temp1339.curr, _temp1339.curr,
_temp1340.last_plus_one - _temp1340.curr, _temp1340.curr);}));}}}{ void*
_temp1341=( void*) _temp1322; _npop_handler( 0u); return _temp1341;}}}}};
_pop_region(& _temp1314);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1342= _new_region(); struct _RegionHandle* rgn=&
_temp1342; _push_region( rgn);{ void* _temp1343= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1349; _LL1345: if(( unsigned int) _temp1343 > 4u?*((
int*) _temp1343) == Cyc_Absyn_AnonStructType: 0){ _LL1350: _temp1349=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1343)->f1; goto _LL1346;} else{ goto
_LL1347;} _LL1347: goto _LL1348; _LL1346: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1349); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple8 _temp1353; struct Cyc_Absyn_Exp*
_temp1354; struct Cyc_Absyn_Structfield* _temp1356; struct _tuple8* _temp1351=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1353=*
_temp1351; _LL1357: _temp1356= _temp1353.f1; goto _LL1355; _LL1355: _temp1354=
_temp1353.f2; goto _LL1352; _LL1352: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1356->type)), _temp1354); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1354,( void*) _temp1356->type)){ Cyc_Tcutil_terr( _temp1354->loc,({ struct
_tagged_string _temp1358=* _temp1356->name; struct _tagged_string _temp1359= Cyc_Absynpp_typ2string((
void*) _temp1356->type); struct _tagged_string _temp1360= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1354->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1358.last_plus_one - _temp1358.curr, _temp1358.curr, _temp1359.last_plus_one
- _temp1359.curr, _temp1359.curr, _temp1360.last_plus_one - _temp1360.curr,
_temp1360.curr);}));}} goto _LL1344;} _LL1348:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ char* _temp1361="tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1362; _temp1362.curr= _temp1361; _temp1362.base=
_temp1361; _temp1362.last_plus_one= _temp1361 + 37; _temp1362;})); goto _LL1344;
_LL1344:;}; _pop_region(& _temp1342);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1363= _new_region(); struct _RegionHandle* rgn=&
_temp1363; _push_region( rgn);{ struct Cyc_List_List* _temp1364=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tud->tvs); goto _LL1365; _LL1365: { struct Cyc_List_List* _temp1366=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn, tuf->tvs); goto
_LL1367; _LL1367: { struct Cyc_List_List* _temp1368=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, _temp1364); goto _LL1369; _LL1369: { struct Cyc_List_List*
_temp1370=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1366); goto
_LL1371; _LL1371: { struct Cyc_List_List* _temp1372=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp1364, _temp1366); goto _LL1373; _LL1373:* all_ref=({ struct Cyc_Core_Opt*
_temp1374=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1374->v=( void*) _temp1368; _temp1374;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1375=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1375->v=( void*) _temp1370; _temp1375;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1411=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1411[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1412; _temp1412.tag= Cyc_Absyn_TunionFieldType; _temp1412.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1413; _temp1413.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1414=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1414[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1415; _temp1415.tag= Cyc_Absyn_KnownTunionfield; _temp1415.f1= tud;
_temp1415.f2= tuf; _temp1415;}); _temp1414;})); _temp1413.targs= _temp1368;
_temp1413;}); _temp1412;}); _temp1411;}); if( topt != 0){ void* _temp1376= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo _temp1384; struct
Cyc_Absyn_TunionInfo _temp1386; void* _temp1388; struct Cyc_List_List* _temp1390;
void* _temp1392; _LL1378: if(( unsigned int) _temp1376 > 4u?*(( int*) _temp1376)
== Cyc_Absyn_TunionFieldType: 0){ _LL1385: _temp1384=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1376)->f1; goto _LL1379;} else{ goto _LL1380;} _LL1380: if(( unsigned int)
_temp1376 > 4u?*(( int*) _temp1376) == Cyc_Absyn_TunionType: 0){ _LL1387:
_temp1386=(( struct Cyc_Absyn_TunionType_struct*) _temp1376)->f1; _LL1393:
_temp1392=( void*) _temp1386.tunion_info; goto _LL1391; _LL1391: _temp1390=
_temp1386.targs; goto _LL1389; _LL1389: _temp1388=( void*) _temp1386.rgn; goto
_LL1381;} else{ goto _LL1382;} _LL1382: goto _LL1383; _LL1379: Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res); goto _LL1377; _LL1381:{ struct Cyc_List_List*
a= _temp1368; for( 0; a != 0? _temp1390 != 0: 0;( a=(( struct Cyc_List_List*)
_check_null( a))->tl, _temp1390=(( struct Cyc_List_List*) _check_null( _temp1390))->tl)){
Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,( void*)((
struct Cyc_List_List*) _check_null( _temp1390))->hd);}} if( tuf->typs == 0? es
== 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1394=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1394->v=( void*) res; _temp1394;});
res=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp1395=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1395[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1396; _temp1396.tag= Cyc_Absyn_TunionType;
_temp1396.f1=({ struct Cyc_Absyn_TunionInfo _temp1397; _temp1397.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1398=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1398[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1399; _temp1399.tag= Cyc_Absyn_KnownTunion;
_temp1399.f1= tud; _temp1399;}); _temp1398;})); _temp1397.targs= _temp1368;
_temp1397.rgn=( void*) _temp1388; _temp1397;}); _temp1396;}); _temp1395;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1400= res; _npop_handler( 0u); return _temp1400;}} goto
_LL1377; _LL1383: goto _LL1377; _LL1377:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es != 0? ts != 0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1372,(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1403= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1404= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1405= e->topt == 0?({ char* _temp1401="?"; struct _tagged_string _temp1402;
_temp1402.curr= _temp1401; _temp1402.base= _temp1401; _temp1402.last_plus_one=
_temp1401 + 2; _temp1402;}): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1403.last_plus_one - _temp1403.curr, _temp1403.curr, _temp1404.last_plus_one
- _temp1404.curr, _temp1404.curr, _temp1405.last_plus_one - _temp1405.curr,
_temp1405.curr);}));}} if( es != 0){ void* _temp1407= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp1406= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1406.last_plus_one - _temp1406.curr, _temp1406.curr);})); _npop_handler( 0u);
return _temp1407;} if( ts != 0){ void* _temp1409= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1408= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1408.last_plus_one - _temp1408.curr, _temp1408.curr);})); _npop_handler( 0u);
return _temp1409;}{ void* _temp1410= res; _npop_handler( 0u); return _temp1410;}}}}}}}};
_pop_region(& _temp1363);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1425=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1425[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1426; _temp1426.tag=
Cyc_Absyn_RgnHandleType; _temp1426.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1426;}); _temp1425;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void* _temp1416= Cyc_Tcutil_compress(
handle_type); void* _temp1422; _LL1418: if(( unsigned int) _temp1416 > 4u?*((
int*) _temp1416) == Cyc_Absyn_RgnHandleType: 0){ _LL1423: _temp1422=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1416)->f1; goto _LL1419;} else{
goto _LL1420;} _LL1420: goto _LL1421; _LL1419: rgn= _temp1422; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1417; _LL1421: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc,({ struct _tagged_string _temp1424= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1424.last_plus_one
- _temp1424.curr, _temp1424.curr);})); goto _LL1417; _LL1417:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1427= t; struct Cyc_Absyn_TunionFieldInfo _temp1433; struct Cyc_List_List*
_temp1435; void* _temp1437; struct Cyc_Absyn_Tunionfield* _temp1439; struct Cyc_Absyn_Tuniondecl*
_temp1441; _LL1429: if(( unsigned int) _temp1427 > 4u?*(( int*) _temp1427) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1434: _temp1433=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1427)->f1; _LL1438: _temp1437=( void*) _temp1433.field_info; if(*(( int*)
_temp1437) == Cyc_Absyn_KnownTunionfield){ _LL1442: _temp1441=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1437)->f1; goto _LL1440; _LL1440: _temp1439=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1437)->f2; goto _LL1436;} else{ goto _LL1431;} _LL1436: _temp1435=
_temp1433.targs; goto _LL1430;} else{ goto _LL1431;} _LL1431: goto _LL1432;
_LL1430: if( _temp1439->tvs != 0){ Cyc_Tcutil_terr( loc,({ char* _temp1443="malloc with existential types not yet implemented";
struct _tagged_string _temp1444; _temp1444.curr= _temp1443; _temp1444.base=
_temp1443; _temp1444.last_plus_one= _temp1443 + 50; _temp1444;}));} goto _LL1428;
_LL1432: goto _LL1428; _LL1428:;}{ void*(* _temp1445)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL1446; _LL1446: if( topt !=
0){ void* _temp1447= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp1453; struct Cyc_Absyn_Conref* _temp1455; struct
Cyc_Absyn_Tqual _temp1457; struct Cyc_Absyn_Conref* _temp1459; void* _temp1461;
void* _temp1463; _LL1449: if(( unsigned int) _temp1447 > 4u?*(( int*) _temp1447)
== Cyc_Absyn_PointerType: 0){ _LL1454: _temp1453=(( struct Cyc_Absyn_PointerType_struct*)
_temp1447)->f1; _LL1464: _temp1463=( void*) _temp1453.elt_typ; goto _LL1462;
_LL1462: _temp1461=( void*) _temp1453.rgn_typ; goto _LL1460; _LL1460: _temp1459=
_temp1453.nullable; goto _LL1458; _LL1458: _temp1457= _temp1453.tq; goto _LL1456;
_LL1456: _temp1455= _temp1453.bounds; goto _LL1450;} else{ goto _LL1451;}
_LL1451: goto _LL1452; _LL1450: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1459)){ _temp1445= Cyc_Absyn_star_typ;} goto _LL1448; _LL1452: goto _LL1448;
_LL1448:;} return _temp1445( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1465=( void*) s->r; struct Cyc_Absyn_Exp* _temp1475;
struct Cyc_Absyn_Stmt* _temp1477; struct Cyc_Absyn_Stmt* _temp1479; struct Cyc_Absyn_Stmt*
_temp1481; struct Cyc_Absyn_Decl* _temp1483; _LL1467: if(( unsigned int)
_temp1465 > 1u?*(( int*) _temp1465) == Cyc_Absyn_Exp_s: 0){ _LL1476: _temp1475=((
struct Cyc_Absyn_Exp_s_struct*) _temp1465)->f1; goto _LL1468;} else{ goto
_LL1469;} _LL1469: if(( unsigned int) _temp1465 > 1u?*(( int*) _temp1465) == Cyc_Absyn_Seq_s:
0){ _LL1480: _temp1479=(( struct Cyc_Absyn_Seq_s_struct*) _temp1465)->f1; goto
_LL1478; _LL1478: _temp1477=(( struct Cyc_Absyn_Seq_s_struct*) _temp1465)->f2;
goto _LL1470;} else{ goto _LL1471;} _LL1471: if(( unsigned int) _temp1465 > 1u?*((
int*) _temp1465) == Cyc_Absyn_Decl_s: 0){ _LL1484: _temp1483=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1465)->f1; goto _LL1482; _LL1482: _temp1481=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1465)->f2; goto _LL1472;} else{ goto _LL1473;} _LL1473: goto _LL1474;
_LL1468: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1475->topt))->v;
_LL1470: s= _temp1477; continue; _LL1472: s= _temp1481; continue; _LL1474:
return Cyc_Tcexp_expr_err( te, loc,({ char* _temp1485="statement expression must end with expression";
struct _tagged_string _temp1486; _temp1486.curr= _temp1485; _temp1486.base=
_temp1485; _temp1486.last_plus_one= _temp1485 + 46; _temp1486;})); _LL1466:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,({ char* _temp1487="tcCodegen"; struct _tagged_string _temp1488; _temp1488.curr=
_temp1487; _temp1488.base= _temp1487; _temp1488.last_plus_one= _temp1487 + 10;
_temp1488;}));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err(
te, loc,({ char* _temp1489="tcFill"; struct _tagged_string _temp1490; _temp1490.curr=
_temp1489; _temp1490.base= _temp1489; _temp1490.last_plus_one= _temp1489 + 7;
_temp1490;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1500=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1500[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1501; _temp1501.tag= Cyc_Absyn_RgnHandleType; _temp1501.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1501;}); _temp1500;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle));
void* _temp1491= Cyc_Tcutil_compress( handle_type); void* _temp1497; _LL1493:
if(( unsigned int) _temp1491 > 4u?*(( int*) _temp1491) == Cyc_Absyn_RgnHandleType:
0){ _LL1498: _temp1497=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1491)->f1; goto _LL1494;} else{ goto _LL1495;} _LL1495: goto _LL1496;
_LL1494: rgn= _temp1497; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1492; _LL1496: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,({ struct _tagged_string _temp1499= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1499.last_plus_one
- _temp1499.curr, _temp1499.curr);})); goto _LL1492; _LL1492:;}{ void* _temp1502=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1514; struct Cyc_Absyn_Exp* _temp1516;
struct Cyc_Absyn_Vardecl* _temp1518; struct Cyc_List_List* _temp1520; struct Cyc_Core_Opt*
_temp1522; struct Cyc_List_List* _temp1524; void* _temp1526; struct
_tagged_string _temp1528; _LL1504: if(*(( int*) _temp1502) == Cyc_Absyn_Comprehension_e){
_LL1519: _temp1518=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1502)->f1;
goto _LL1517; _LL1517: _temp1516=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1502)->f2; goto _LL1515; _LL1515: _temp1514=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1502)->f3; goto _LL1505;} else{ goto _LL1506;} _LL1506: if(*(( int*)
_temp1502) == Cyc_Absyn_UnresolvedMem_e){ _LL1523: _temp1522=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1502)->f1; goto _LL1521; _LL1521: _temp1520=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1502)->f2; goto _LL1507;} else{ goto _LL1508;} _LL1508: if(*(( int*)
_temp1502) == Cyc_Absyn_Array_e){ _LL1525: _temp1524=(( struct Cyc_Absyn_Array_e_struct*)
_temp1502)->f1; goto _LL1509;} else{ goto _LL1510;} _LL1510: if(*(( int*)
_temp1502) == Cyc_Absyn_Const_e){ _LL1527: _temp1526=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1502)->f1; if(( unsigned int) _temp1526 > 1u?*(( int*) _temp1526) == Cyc_Absyn_String_c:
0){ _LL1529: _temp1528=(( struct Cyc_Absyn_String_c_struct*) _temp1526)->f1;
goto _LL1511;} else{ goto _LL1512;}} else{ goto _LL1512;} _LL1512: goto _LL1513;
_LL1505: { void* _temp1530= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); goto
_LL1531; _LL1531: { void* _temp1532= Cyc_Tcutil_compress( _temp1530); struct Cyc_Absyn_Exp*
_temp1538; struct Cyc_Absyn_Tqual _temp1540; void* _temp1542; _LL1534: if((
unsigned int) _temp1532 > 4u?*(( int*) _temp1532) == Cyc_Absyn_ArrayType: 0){
_LL1543: _temp1542=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1532)->f1;
goto _LL1541; _LL1541: _temp1540=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1532)->f2; goto _LL1539; _LL1539: _temp1538=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1532)->f3; goto _LL1535;} else{ goto _LL1536;} _LL1536: goto _LL1537;
_LL1535: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1516)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1548=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1548[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1549; _temp1549.tag= Cyc_Absyn_Upper_b; _temp1549.f1=
_temp1516; _temp1549;}); _temp1548;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1545=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1545[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1546; _temp1546.tag= Cyc_Absyn_PointerType;
_temp1546.f1=({ struct Cyc_Absyn_PtrInfo _temp1547; _temp1547.elt_typ=( void*)
_temp1542; _temp1547.rgn_typ=( void*) rgn; _temp1547.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1547.tq= _temp1540; _temp1547.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1547;});
_temp1546;}); _temp1545;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1544=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1544->v=(
void*) res_typ; _temp1544;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1537: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char*
_temp1550="tcNew: comprehension returned non-array type"; struct _tagged_string
_temp1551; _temp1551.curr= _temp1550; _temp1551.base= _temp1550; _temp1551.last_plus_one=
_temp1550 + 45; _temp1551;})); _LL1533:;}} _LL1507:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1552=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1552[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1553; _temp1553.tag= Cyc_Absyn_Array_e; _temp1553.f1=
_temp1520; _temp1553;}); _temp1552;}))); _temp1524= _temp1520; goto _LL1509;
_LL1509: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1554= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1560; struct Cyc_Absyn_Conref*
_temp1562; struct Cyc_Absyn_Tqual _temp1564; struct Cyc_Absyn_Conref* _temp1566;
void* _temp1568; void* _temp1570; void** _temp1572; _LL1556: if(( unsigned int)
_temp1554 > 4u?*(( int*) _temp1554) == Cyc_Absyn_PointerType: 0){ _LL1561:
_temp1560=(( struct Cyc_Absyn_PointerType_struct*) _temp1554)->f1; _LL1571:
_temp1570=( void*) _temp1560.elt_typ; _temp1572=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1554)->f1).elt_typ; goto _LL1569; _LL1569: _temp1568=( void*) _temp1560.rgn_typ;
goto _LL1567; _LL1567: _temp1566= _temp1560.nullable; goto _LL1565; _LL1565:
_temp1564= _temp1560.tq; goto _LL1563; _LL1563: _temp1562= _temp1560.bounds;
goto _LL1557;} else{ goto _LL1558;} _LL1558: goto _LL1559; _LL1557: elt_typ_opt=(
void**) _temp1572; goto _LL1555; _LL1559: goto _LL1555; _LL1555:;}{ void*
_temp1573= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); goto _LL1574; _LL1574: {
void* res_typ;{ void* _temp1575= Cyc_Tcutil_compress( _temp1573); struct Cyc_Absyn_Exp*
_temp1581; struct Cyc_Absyn_Tqual _temp1583; void* _temp1585; _LL1577: if((
unsigned int) _temp1575 > 4u?*(( int*) _temp1575) == Cyc_Absyn_ArrayType: 0){
_LL1586: _temp1585=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1575)->f1;
goto _LL1584; _LL1584: _temp1583=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1575)->f2; goto _LL1582; _LL1582: _temp1581=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1575)->f3; goto _LL1578;} else{ goto _LL1579;} _LL1579: goto _LL1580;
_LL1578: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1587=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1587[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1588; _temp1588.tag=
Cyc_Absyn_PointerType; _temp1588.f1=({ struct Cyc_Absyn_PtrInfo _temp1589;
_temp1589.elt_typ=( void*) _temp1585; _temp1589.rgn_typ=( void*) rgn; _temp1589.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1589.tq=
_temp1583; _temp1589.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1590=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1590[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1591; _temp1591.tag= Cyc_Absyn_Upper_b; _temp1591.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1581); _temp1591;}); _temp1590;}));
_temp1589;}); _temp1588;}); _temp1587;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1592=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1592->v=( void*) res_typ; _temp1592;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1576; _LL1580: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp1593="tcExpNoPromote on Array_e returned non-array type"; struct
_tagged_string _temp1594; _temp1594.curr= _temp1593; _temp1594.base= _temp1593;
_temp1594.last_plus_one= _temp1593 + 50; _temp1594;})); _LL1576:;} return
res_typ;}}} _LL1511: { void* _temp1595= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); goto _LL1596;
_LL1596: { void* _temp1597= Cyc_Tcexp_tcExp( te,( void**)& _temp1595, e1); goto
_LL1598; _LL1598: return Cyc_Absyn_atb_typ( _temp1597, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1599=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1599[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1600; _temp1600.tag= Cyc_Absyn_Upper_b; _temp1600.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1600;}); _temp1599;}));}} _LL1513: { void**
topt2= 0; if( topt != 0){ void* _temp1601= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1609; struct Cyc_Absyn_Conref*
_temp1611; struct Cyc_Absyn_Tqual _temp1613; struct Cyc_Absyn_Conref* _temp1615;
void* _temp1617; void* _temp1619; void** _temp1621; struct Cyc_Absyn_TunionInfo
_temp1622; _LL1603: if(( unsigned int) _temp1601 > 4u?*(( int*) _temp1601) ==
Cyc_Absyn_PointerType: 0){ _LL1610: _temp1609=(( struct Cyc_Absyn_PointerType_struct*)
_temp1601)->f1; _LL1620: _temp1619=( void*) _temp1609.elt_typ; _temp1621=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1601)->f1).elt_typ; goto _LL1618;
_LL1618: _temp1617=( void*) _temp1609.rgn_typ; goto _LL1616; _LL1616: _temp1615=
_temp1609.nullable; goto _LL1614; _LL1614: _temp1613= _temp1609.tq; goto _LL1612;
_LL1612: _temp1611= _temp1609.bounds; goto _LL1604;} else{ goto _LL1605;}
_LL1605: if(( unsigned int) _temp1601 > 4u?*(( int*) _temp1601) == Cyc_Absyn_TunionType:
0){ _LL1623: _temp1622=(( struct Cyc_Absyn_TunionType_struct*) _temp1601)->f1;
goto _LL1606;} else{ goto _LL1607;} _LL1607: goto _LL1608; _LL1604: topt2=( void**)
_temp1621; goto _LL1602; _LL1606: topt2=({ void** _temp1624=( void**) GC_malloc(
sizeof( void*)); _temp1624[ 0]=*(( void**) _check_null( topt)); _temp1624;});
goto _LL1602; _LL1608: goto _LL1602; _LL1602:;}{ void* _temp1625= Cyc_Tcexp_tcExp(
te, topt2, e1); goto _LL1626; _LL1626: { void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1628=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1628[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1629; _temp1629.tag=
Cyc_Absyn_PointerType; _temp1629.f1=({ struct Cyc_Absyn_PtrInfo _temp1630;
_temp1630.elt_typ=( void*) _temp1625; _temp1630.rgn_typ=( void*) rgn; _temp1630.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1630.tq= Cyc_Absyn_empty_tqual();
_temp1630.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1631=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1631[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1632; _temp1632.tag= Cyc_Absyn_Upper_b; _temp1632.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1632;}); _temp1631;})); _temp1630;}); _temp1629;});
_temp1628;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1627=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1627->v=( void*) res_typ;
_temp1627;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}}} _LL1503:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1633= t; struct Cyc_Absyn_Exp* _temp1639; struct Cyc_Absyn_Tqual
_temp1641; void* _temp1643; _LL1635: if(( unsigned int) _temp1633 > 4u?*(( int*)
_temp1633) == Cyc_Absyn_ArrayType: 0){ _LL1644: _temp1643=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1633)->f1; goto _LL1642; _LL1642: _temp1641=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1633)->f2; goto _LL1640; _LL1640: _temp1639=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1633)->f3; goto _LL1636;} else{ goto _LL1637;} _LL1637: goto _LL1638;
_LL1636: { void* _temp1647; int _temp1649; struct _tuple5 _temp1645= Cyc_Tcutil_addressof_props(
te, e); _LL1650: _temp1649= _temp1645.f1; goto _LL1648; _LL1648: _temp1647=
_temp1645.f2; goto _LL1646; _LL1646: { void* _temp1653= _temp1639 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1651=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1651[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1652; _temp1652.tag= Cyc_Absyn_Upper_b;
_temp1652.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1639); _temp1652;});
_temp1651;}); goto _LL1654; _LL1654: t= Cyc_Absyn_atb_typ( _temp1643, _temp1647,
_temp1641, _temp1653);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=(
void*) t); return t;}} _LL1638: return t; _LL1634:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1655=( void*) e->r; struct Cyc_List_List* _temp1665;
struct Cyc_Absyn_Exp* _temp1667; struct Cyc_Absyn_Exp* _temp1669; struct Cyc_Absyn_Vardecl*
_temp1671; void* _temp1673; struct _tagged_string _temp1675; _LL1657: if(*(( int*)
_temp1655) == Cyc_Absyn_Array_e){ _LL1666: _temp1665=(( struct Cyc_Absyn_Array_e_struct*)
_temp1655)->f1; goto _LL1658;} else{ goto _LL1659;} _LL1659: if(*(( int*)
_temp1655) == Cyc_Absyn_Comprehension_e){ _LL1672: _temp1671=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1655)->f1; goto _LL1670; _LL1670: _temp1669=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1655)->f2; goto _LL1668; _LL1668: _temp1667=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1655)->f3; goto _LL1660;} else{ goto _LL1661;} _LL1661: if(*(( int*)
_temp1655) == Cyc_Absyn_Const_e){ _LL1674: _temp1673=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1655)->f1; if(( unsigned int) _temp1673 > 1u?*(( int*) _temp1673) == Cyc_Absyn_String_c:
0){ _LL1676: _temp1675=(( struct Cyc_Absyn_String_c_struct*) _temp1673)->f1;
goto _LL1662;} else{ goto _LL1663;}} else{ goto _LL1663;} _LL1663: goto _LL1664;
_LL1658: return t; _LL1660: return t; _LL1662: return t; _LL1664: t= Cyc_Tcutil_compress(
t);{ void* _temp1677= t; struct Cyc_Absyn_Exp* _temp1683; struct Cyc_Absyn_Tqual
_temp1685; void* _temp1687; _LL1679: if(( unsigned int) _temp1677 > 4u?*(( int*)
_temp1677) == Cyc_Absyn_ArrayType: 0){ _LL1688: _temp1687=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1677)->f1; goto _LL1686; _LL1686: _temp1685=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1677)->f2; goto _LL1684; _LL1684: _temp1683=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1677)->f3; goto _LL1680;} else{ goto _LL1681;} _LL1681: goto _LL1682;
_LL1680: { void* _temp1691; int _temp1693; struct _tuple5 _temp1689= Cyc_Tcutil_addressof_props(
te, e); _LL1694: _temp1693= _temp1689.f1; goto _LL1692; _LL1692: _temp1691=
_temp1689.f2; goto _LL1690; _LL1690: { void* b= _temp1683 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1695=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1695[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1696; _temp1696.tag= Cyc_Absyn_Upper_b; _temp1696.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1683); _temp1696;}); _temp1695;}); t=
Cyc_Absyn_atb_typ( _temp1687, _temp1691, _temp1685, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1682: return t; _LL1678:;} _LL1656:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1697=( void*) e->r; struct Cyc_Absyn_Exp* _temp1703; _LL1699: if(*(( int*)
_temp1697) == Cyc_Absyn_NoInstantiate_e){ _LL1704: _temp1703=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1697)->f1; goto _LL1700;} else{ goto _LL1701;} _LL1701: goto _LL1702;
_LL1700: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1703);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1703->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1703->topt))->v))); e->topt=
_temp1703->topt; goto _LL1698; _LL1702: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1705= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1711; struct Cyc_Absyn_Conref*
_temp1713; struct Cyc_Absyn_Tqual _temp1715; struct Cyc_Absyn_Conref* _temp1717;
void* _temp1719; void* _temp1721; _LL1707: if(( unsigned int) _temp1705 > 4u?*((
int*) _temp1705) == Cyc_Absyn_PointerType: 0){ _LL1712: _temp1711=(( struct Cyc_Absyn_PointerType_struct*)
_temp1705)->f1; _LL1722: _temp1721=( void*) _temp1711.elt_typ; goto _LL1720;
_LL1720: _temp1719=( void*) _temp1711.rgn_typ; goto _LL1718; _LL1718: _temp1717=
_temp1711.nullable; goto _LL1716; _LL1716: _temp1715= _temp1711.tq; goto _LL1714;
_LL1714: _temp1713= _temp1711.bounds; goto _LL1708;} else{ goto _LL1709;}
_LL1709: goto _LL1710; _LL1708:{ void* _temp1723= Cyc_Tcutil_compress( _temp1721);
struct Cyc_Absyn_FnInfo _temp1729; struct Cyc_List_List* _temp1731; struct Cyc_Absyn_VarargInfo*
_temp1733; int _temp1735; struct Cyc_List_List* _temp1737; void* _temp1739;
struct Cyc_Core_Opt* _temp1741; struct Cyc_List_List* _temp1743; _LL1725: if((
unsigned int) _temp1723 > 4u?*(( int*) _temp1723) == Cyc_Absyn_FnType: 0){
_LL1730: _temp1729=(( struct Cyc_Absyn_FnType_struct*) _temp1723)->f1; _LL1744:
_temp1743= _temp1729.tvars; goto _LL1742; _LL1742: _temp1741= _temp1729.effect;
goto _LL1740; _LL1740: _temp1739=( void*) _temp1729.ret_typ; goto _LL1738;
_LL1738: _temp1737= _temp1729.args; goto _LL1736; _LL1736: _temp1735= _temp1729.c_varargs;
goto _LL1734; _LL1734: _temp1733= _temp1729.cyc_varargs; goto _LL1732; _LL1732:
_temp1731= _temp1729.attributes; goto _LL1726;} else{ goto _LL1727;} _LL1727:
goto _LL1728; _LL1726: if( _temp1743 != 0){ struct _RegionHandle _temp1745=
_new_region(); struct _RegionHandle* rgn=& _temp1745; _push_region( rgn);{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1743); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1752=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1752[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1753; _temp1753.tag= Cyc_Absyn_FnType; _temp1753.f1=({ struct Cyc_Absyn_FnInfo
_temp1754; _temp1754.tvars= 0; _temp1754.effect= _temp1741; _temp1754.ret_typ=(
void*) _temp1739; _temp1754.args= _temp1737; _temp1754.c_varargs= _temp1735;
_temp1754.cyc_varargs= _temp1733; _temp1754.attributes= _temp1731; _temp1754;});
_temp1753;}); _temp1752;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1749=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1749[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1750; _temp1750.tag=
Cyc_Absyn_PointerType; _temp1750.f1=({ struct Cyc_Absyn_PtrInfo _temp1751;
_temp1751.elt_typ=( void*) ftyp; _temp1751.rgn_typ=( void*) _temp1719; _temp1751.nullable=
_temp1717; _temp1751.tq= _temp1715; _temp1751.bounds= _temp1713; _temp1751;});
_temp1750;}); _temp1749;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1746=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1746[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1747; _temp1747.tag= Cyc_Absyn_Instantiate_e; _temp1747.f1= inner;
_temp1747.f2= ts; _temp1747;}); _temp1746;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1748=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1748->v=( void*) new_typ; _temp1748;});}; _pop_region(& _temp1745);} goto
_LL1724; _LL1728: goto _LL1724; _LL1724:;} goto _LL1706; _LL1710: goto _LL1706;
_LL1706:;} goto _LL1698; _LL1698:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1755=( void*) e->r; struct Cyc_Absyn_Exp* _temp1827; struct
_tuple1* _temp1829; struct Cyc_List_List* _temp1831; struct Cyc_Absyn_Exp*
_temp1833; struct Cyc_List_List* _temp1835; struct Cyc_Core_Opt* _temp1837; void*
_temp1839; void* _temp1841; struct _tuple1* _temp1843; struct Cyc_List_List*
_temp1845; void* _temp1847; void* _temp1849; struct Cyc_Absyn_Exp* _temp1851;
struct Cyc_Absyn_Exp* _temp1853; struct Cyc_Core_Opt* _temp1855; struct Cyc_Absyn_Exp*
_temp1857; struct Cyc_Absyn_Exp* _temp1859; struct Cyc_Absyn_Exp* _temp1861;
struct Cyc_Absyn_Exp* _temp1863; struct Cyc_Absyn_Exp* _temp1865; struct Cyc_Absyn_Exp*
_temp1867; struct Cyc_Absyn_VarargCallInfo* _temp1869; struct Cyc_Absyn_VarargCallInfo**
_temp1871; struct Cyc_List_List* _temp1872; struct Cyc_Absyn_Exp* _temp1874;
struct Cyc_Absyn_Exp* _temp1876; struct Cyc_List_List* _temp1878; struct Cyc_Absyn_Exp*
_temp1880; struct Cyc_Absyn_Exp* _temp1882; void* _temp1884; struct Cyc_Absyn_Exp*
_temp1886; struct Cyc_Absyn_Exp* _temp1888; struct Cyc_Absyn_Exp* _temp1890;
struct Cyc_Absyn_Exp* _temp1892; void* _temp1894; struct Cyc_Absyn_Exp*
_temp1896; struct _tagged_string* _temp1898; struct Cyc_Absyn_Exp* _temp1900;
struct _tagged_string* _temp1902; struct Cyc_Absyn_Exp* _temp1904; struct Cyc_Absyn_Exp*
_temp1906; struct Cyc_Absyn_Exp* _temp1908; struct Cyc_List_List* _temp1910;
struct Cyc_List_List* _temp1912; struct _tuple2* _temp1914; struct Cyc_List_List*
_temp1916; struct Cyc_Absyn_Stmt* _temp1918; struct Cyc_Absyn_Fndecl* _temp1920;
struct Cyc_Absyn_Exp* _temp1922; struct Cyc_Absyn_Exp* _temp1924; struct Cyc_Absyn_Exp*
_temp1926; struct Cyc_Absyn_Vardecl* _temp1928; struct Cyc_Absyn_Structdecl*
_temp1930; struct Cyc_Absyn_Structdecl** _temp1932; struct Cyc_List_List*
_temp1933; struct Cyc_Core_Opt* _temp1935; struct Cyc_Core_Opt** _temp1937;
struct _tuple1* _temp1938; struct _tuple1** _temp1940; struct Cyc_List_List*
_temp1941; void* _temp1943; struct Cyc_Absyn_Tunionfield* _temp1945; struct Cyc_Absyn_Tuniondecl*
_temp1947; struct Cyc_List_List* _temp1949; struct Cyc_Core_Opt* _temp1951;
struct Cyc_Core_Opt** _temp1953; struct Cyc_Core_Opt* _temp1954; struct Cyc_Core_Opt**
_temp1956; struct Cyc_Absyn_Enumfield* _temp1957; struct Cyc_Absyn_Enumdecl*
_temp1959; struct _tuple1* _temp1961; struct _tuple1** _temp1963; void*
_temp1964; struct Cyc_Absyn_Exp* _temp1966; _LL1757: if(*(( int*) _temp1755) ==
Cyc_Absyn_NoInstantiate_e){ _LL1828: _temp1827=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1755)->f1; goto _LL1758;} else{ goto _LL1759;} _LL1759: if(*(( int*)
_temp1755) == Cyc_Absyn_UnknownId_e){ _LL1830: _temp1829=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp1755)->f1; goto _LL1760;} else{ goto _LL1761;} _LL1761: if(*(( int*)
_temp1755) == Cyc_Absyn_UnknownCall_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1755)->f1; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1755)->f2; goto _LL1762;} else{ goto _LL1763;} _LL1763: if(*(( int*)
_temp1755) == Cyc_Absyn_UnresolvedMem_e){ _LL1838: _temp1837=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1755)->f1; goto _LL1836; _LL1836: _temp1835=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1755)->f2; goto _LL1764;} else{ goto _LL1765;} _LL1765: if(*(( int*)
_temp1755) == Cyc_Absyn_Const_e){ _LL1840: _temp1839=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1755)->f1; goto _LL1766;} else{ goto _LL1767;} _LL1767: if(*(( int*)
_temp1755) == Cyc_Absyn_Var_e){ _LL1844: _temp1843=(( struct Cyc_Absyn_Var_e_struct*)
_temp1755)->f1; goto _LL1842; _LL1842: _temp1841=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1755)->f2; goto _LL1768;} else{ goto _LL1769;} _LL1769: if(*(( int*)
_temp1755) == Cyc_Absyn_Primop_e){ _LL1848: _temp1847=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1755)->f1; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1755)->f2; goto _LL1770;} else{ goto _LL1771;} _LL1771: if(*(( int*)
_temp1755) == Cyc_Absyn_Increment_e){ _LL1852: _temp1851=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1755)->f1; goto _LL1850; _LL1850: _temp1849=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1755)->f2; goto _LL1772;} else{ goto _LL1773;} _LL1773: if(*(( int*)
_temp1755) == Cyc_Absyn_AssignOp_e){ _LL1858: _temp1857=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1755)->f1; goto _LL1856; _LL1856: _temp1855=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1755)->f2; goto _LL1854; _LL1854: _temp1853=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1755)->f3; goto _LL1774;} else{ goto _LL1775;} _LL1775: if(*(( int*)
_temp1755) == Cyc_Absyn_Conditional_e){ _LL1864: _temp1863=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1755)->f1; goto _LL1862; _LL1862: _temp1861=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1755)->f2; goto _LL1860; _LL1860: _temp1859=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1755)->f3; goto _LL1776;} else{ goto _LL1777;} _LL1777: if(*(( int*)
_temp1755) == Cyc_Absyn_SeqExp_e){ _LL1868: _temp1867=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1755)->f1; goto _LL1866; _LL1866: _temp1865=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1755)->f2; goto _LL1778;} else{ goto _LL1779;} _LL1779: if(*(( int*)
_temp1755) == Cyc_Absyn_FnCall_e){ _LL1875: _temp1874=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1755)->f1; goto _LL1873; _LL1873: _temp1872=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1755)->f2; goto _LL1870; _LL1870: _temp1869=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1755)->f3; _temp1871=&(( struct Cyc_Absyn_FnCall_e_struct*) _temp1755)->f3;
goto _LL1780;} else{ goto _LL1781;} _LL1781: if(*(( int*) _temp1755) == Cyc_Absyn_Throw_e){
_LL1877: _temp1876=(( struct Cyc_Absyn_Throw_e_struct*) _temp1755)->f1; goto
_LL1782;} else{ goto _LL1783;} _LL1783: if(*(( int*) _temp1755) == Cyc_Absyn_Instantiate_e){
_LL1881: _temp1880=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1755)->f1;
goto _LL1879; _LL1879: _temp1878=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1755)->f2; goto _LL1784;} else{ goto _LL1785;} _LL1785: if(*(( int*)
_temp1755) == Cyc_Absyn_Cast_e){ _LL1885: _temp1884=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1755)->f1; goto _LL1883; _LL1883: _temp1882=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1755)->f2; goto _LL1786;} else{ goto _LL1787;} _LL1787: if(*(( int*)
_temp1755) == Cyc_Absyn_Address_e){ _LL1887: _temp1886=(( struct Cyc_Absyn_Address_e_struct*)
_temp1755)->f1; goto _LL1788;} else{ goto _LL1789;} _LL1789: if(*(( int*)
_temp1755) == Cyc_Absyn_New_e){ _LL1891: _temp1890=(( struct Cyc_Absyn_New_e_struct*)
_temp1755)->f1; goto _LL1889; _LL1889: _temp1888=(( struct Cyc_Absyn_New_e_struct*)
_temp1755)->f2; goto _LL1790;} else{ goto _LL1791;} _LL1791: if(*(( int*)
_temp1755) == Cyc_Absyn_Sizeofexp_e){ _LL1893: _temp1892=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1755)->f1; goto _LL1792;} else{ goto _LL1793;} _LL1793: if(*(( int*)
_temp1755) == Cyc_Absyn_Sizeoftyp_e){ _LL1895: _temp1894=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1755)->f1; goto _LL1794;} else{ goto _LL1795;} _LL1795: if(*(( int*)
_temp1755) == Cyc_Absyn_Deref_e){ _LL1897: _temp1896=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1755)->f1; goto _LL1796;} else{ goto _LL1797;} _LL1797: if(*(( int*)
_temp1755) == Cyc_Absyn_StructMember_e){ _LL1901: _temp1900=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1755)->f1; goto _LL1899; _LL1899: _temp1898=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1755)->f2; goto _LL1798;} else{ goto _LL1799;} _LL1799: if(*(( int*)
_temp1755) == Cyc_Absyn_StructArrow_e){ _LL1905: _temp1904=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1755)->f1; goto _LL1903; _LL1903: _temp1902=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1755)->f2; goto _LL1800;} else{ goto _LL1801;} _LL1801: if(*(( int*)
_temp1755) == Cyc_Absyn_Subscript_e){ _LL1909: _temp1908=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1755)->f1; goto _LL1907; _LL1907: _temp1906=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1755)->f2; goto _LL1802;} else{ goto _LL1803;} _LL1803: if(*(( int*)
_temp1755) == Cyc_Absyn_Tuple_e){ _LL1911: _temp1910=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1755)->f1; goto _LL1804;} else{ goto _LL1805;} _LL1805: if(*(( int*)
_temp1755) == Cyc_Absyn_CompoundLit_e){ _LL1915: _temp1914=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1755)->f1; goto _LL1913; _LL1913: _temp1912=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1755)->f2; goto _LL1806;} else{ goto _LL1807;} _LL1807: if(*(( int*)
_temp1755) == Cyc_Absyn_Array_e){ _LL1917: _temp1916=(( struct Cyc_Absyn_Array_e_struct*)
_temp1755)->f1; goto _LL1808;} else{ goto _LL1809;} _LL1809: if(*(( int*)
_temp1755) == Cyc_Absyn_StmtExp_e){ _LL1919: _temp1918=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1755)->f1; goto _LL1810;} else{ goto _LL1811;} _LL1811: if(*(( int*)
_temp1755) == Cyc_Absyn_Codegen_e){ _LL1921: _temp1920=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1755)->f1; goto _LL1812;} else{ goto _LL1813;} _LL1813: if(*(( int*)
_temp1755) == Cyc_Absyn_Fill_e){ _LL1923: _temp1922=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1755)->f1; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(*(( int*)
_temp1755) == Cyc_Absyn_Comprehension_e){ _LL1929: _temp1928=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1755)->f1; goto _LL1927; _LL1927: _temp1926=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1755)->f2; goto _LL1925; _LL1925: _temp1924=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1755)->f3; goto _LL1816;} else{ goto _LL1817;} _LL1817: if(*(( int*)
_temp1755) == Cyc_Absyn_Struct_e){ _LL1939: _temp1938=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1755)->f1; _temp1940=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1755)->f1;
goto _LL1936; _LL1936: _temp1935=(( struct Cyc_Absyn_Struct_e_struct*) _temp1755)->f2;
_temp1937=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1755)->f2; goto _LL1934;
_LL1934: _temp1933=(( struct Cyc_Absyn_Struct_e_struct*) _temp1755)->f3; goto
_LL1931; _LL1931: _temp1930=(( struct Cyc_Absyn_Struct_e_struct*) _temp1755)->f4;
_temp1932=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1755)->f4; goto _LL1818;}
else{ goto _LL1819;} _LL1819: if(*(( int*) _temp1755) == Cyc_Absyn_AnonStruct_e){
_LL1944: _temp1943=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1755)->f1;
goto _LL1942; _LL1942: _temp1941=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1755)->f2; goto _LL1820;} else{ goto _LL1821;} _LL1821: if(*(( int*)
_temp1755) == Cyc_Absyn_Tunion_e){ _LL1955: _temp1954=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1755)->f1; _temp1956=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1755)->f1;
goto _LL1952; _LL1952: _temp1951=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1755)->f2;
_temp1953=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1755)->f2; goto _LL1950;
_LL1950: _temp1949=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1755)->f3; goto
_LL1948; _LL1948: _temp1947=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1755)->f4;
goto _LL1946; _LL1946: _temp1945=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1755)->f5;
goto _LL1822;} else{ goto _LL1823;} _LL1823: if(*(( int*) _temp1755) == Cyc_Absyn_Enum_e){
_LL1962: _temp1961=(( struct Cyc_Absyn_Enum_e_struct*) _temp1755)->f1; _temp1963=&((
struct Cyc_Absyn_Enum_e_struct*) _temp1755)->f1; goto _LL1960; _LL1960:
_temp1959=(( struct Cyc_Absyn_Enum_e_struct*) _temp1755)->f2; goto _LL1958;
_LL1958: _temp1957=(( struct Cyc_Absyn_Enum_e_struct*) _temp1755)->f3; goto
_LL1824;} else{ goto _LL1825;} _LL1825: if(*(( int*) _temp1755) == Cyc_Absyn_Malloc_e){
_LL1967: _temp1966=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1755)->f1; goto
_LL1965; _LL1965: _temp1964=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1755)->f2; goto _LL1826;} else{ goto _LL1756;} _LL1758: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1827); return; _LL1760: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1829); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1762: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1833, _temp1831); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1764: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1835); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1766: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1839, e);
goto _LL1756; _LL1768: t= Cyc_Tcexp_tcVar( te, loc, _temp1843, _temp1841); goto
_LL1756; _LL1770: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1847, _temp1845);
goto _LL1756; _LL1772: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1851,
_temp1849); goto _LL1756; _LL1774: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1857, _temp1855, _temp1853); goto _LL1756; _LL1776: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1863, _temp1861, _temp1859); goto _LL1756; _LL1778: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1867, _temp1865); goto _LL1756; _LL1780: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1874, _temp1872, _temp1871); goto _LL1756; _LL1782: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1876); goto _LL1756; _LL1784: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1880, _temp1878); goto _LL1756; _LL1786: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1884, _temp1882); goto _LL1756; _LL1788: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1886); goto _LL1756; _LL1790: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1890, e, _temp1888); goto _LL1756; _LL1792: { void* _temp1968= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1892); goto _LL1969; _LL1969: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1968); goto _LL1756;} _LL1794: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1894); goto _LL1756; _LL1796: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp1896); goto _LL1756; _LL1798: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp1900, _temp1898); goto _LL1756; _LL1800: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp1904, _temp1902); goto _LL1756; _LL1802: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1908, _temp1906); goto _LL1756; _LL1804: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1910); goto _LL1756; _LL1806: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1914, _temp1912); goto _LL1756; _LL1808: { void** elt_topt= 0;
if( topt != 0){ void* _temp1970= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp1976; struct Cyc_Absyn_Tqual _temp1978; void*
_temp1980; void** _temp1982; _LL1972: if(( unsigned int) _temp1970 > 4u?*(( int*)
_temp1970) == Cyc_Absyn_ArrayType: 0){ _LL1981: _temp1980=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1970)->f1; _temp1982=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1970)->f1); goto _LL1979; _LL1979: _temp1978=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1970)->f2; goto _LL1977; _LL1977: _temp1976=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1970)->f3; goto _LL1973;} else{ goto _LL1974;} _LL1974: goto _LL1975;
_LL1973: elt_topt=( void**) _temp1982; goto _LL1971; _LL1975: goto _LL1971;
_LL1971:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1916); goto _LL1756;}
_LL1810: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1918); goto _LL1756;
_LL1812: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1920); goto _LL1756;
_LL1814: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1922); goto _LL1756; _LL1816:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1928, _temp1926, _temp1924);
goto _LL1756; _LL1818: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1940,
_temp1937, _temp1933, _temp1932); goto _LL1756; _LL1820: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1943, _temp1941); goto _LL1756; _LL1822: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1956, _temp1953, _temp1949, _temp1947, _temp1945); goto
_LL1756; _LL1824:* _temp1963=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp1957))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1983=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1983[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1984; _temp1984.tag= Cyc_Absyn_EnumType;
_temp1984.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1959))->name;
_temp1984.f2= _temp1959; _temp1984;}); _temp1983;}); goto _LL1756; _LL1826: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1966, _temp1964); goto _LL1756; _LL1756:;}
e->topt=({ struct Cyc_Core_Opt* _temp1985=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1985->v=( void*) t; _temp1985;});}