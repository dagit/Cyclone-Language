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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( void*); extern
void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
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
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*);
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
des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
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
void* _temp1=( void*) setjmp( _temp0.handler); if( ! _temp1){{ void* _temp2= Cyc_Tcenv_lookup_ordinary(
te, e->loc, q); void* _temp12; struct Cyc_Absyn_Enumfield* _temp14; struct Cyc_Absyn_Enumdecl*
_temp16; struct Cyc_Absyn_Tunionfield* _temp18; struct Cyc_Absyn_Tuniondecl*
_temp20; struct Cyc_Absyn_Structdecl* _temp22; _LL4: if(*(( int*) _temp2) == Cyc_Tcenv_VarRes){
_LL13: _temp12=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp2)->f1; goto
_LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp2) == Cyc_Tcenv_EnumRes){ _LL17:
_temp16=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f1;
goto _LL15; _LL15: _temp14=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp2)->f2; goto _LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp2) == Cyc_Tcenv_TunionRes){
_LL21: _temp20=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp2)->f1; goto _LL19; _LL19: _temp18=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2)->f2; goto _LL9;} else{ goto _LL10;}
_LL10: if(*(( int*) _temp2) == Cyc_Tcenv_StructRes){ _LL23: _temp22=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp2)->f1; goto _LL11;} else{ goto _LL3;}
_LL5:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp24=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp24[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp25; _temp25.tag= Cyc_Absyn_Var_e;
_temp25.f1= q; _temp25.f2=( void*) _temp12; _temp25;}); _temp24;}))); goto _LL3;
_LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp26=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp26[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp27; _temp27.tag= Cyc_Absyn_Enum_e;
_temp27.f1= q; _temp27.f2=( struct Cyc_Absyn_Enumdecl*) _temp16; _temp27.f3=(
struct Cyc_Absyn_Enumfield*) _temp14; _temp27;}); _temp26;}))); goto _LL3; _LL9:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp28=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp28[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp29; _temp29.tag= Cyc_Absyn_Tunion_e;
_temp29.f1= 0; _temp29.f2= 0; _temp29.f3= 0; _temp29.f4= _temp20; _temp29.f5=
_temp18; _temp29;}); _temp28;}))); goto _LL3; _LL11: Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp30= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp30.last_plus_one - _temp30.curr, _temp30.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp31=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp31[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp32; _temp32.tag= Cyc_Absyn_Var_e; _temp32.f1= q; _temp32.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp32;}); _temp31;}))); goto _LL3; _LL3:;};
_pop_handler();} else{ void* _temp34= _temp1; _LL36: if( _temp34 == Cyc_Dict_Absent){
goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL37:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp40=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp40[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp41; _temp41.tag= Cyc_Absyn_Var_e; _temp41.f1= q; _temp41.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp41;}); _temp40;}))); goto _LL35; _LL39:( void)
_throw( _temp34); _LL35:;}}} struct _tuple6{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; static struct _tuple6* Cyc_Tcexp_make_struct_arg( struct
Cyc_Absyn_Exp* e){ return({ struct _tuple6* _temp42=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp42->f1= 0; _temp42->f2= e; _temp42;});} static
void Cyc_Tcexp_resolve_unknown_fn( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Exp* e1, struct Cyc_List_List* es){ void* _temp43=( void*)
e1->r; struct _tuple1* _temp49; _LL45: if(*(( int*) _temp43) == Cyc_Absyn_UnknownId_e){
_LL50: _temp49=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp43)->f1; goto _LL46;} else{ goto _LL47;} _LL47: goto _LL48; _LL46: { struct
_handler_cons _temp51; _push_handler(& _temp51);{ void* _temp52=( void*) setjmp(
_temp51.handler); if( ! _temp52){{ void* _temp53= Cyc_Tcenv_lookup_ordinary( te,
e1->loc, _temp49); void* _temp63; struct Cyc_Absyn_Tunionfield* _temp65; struct
Cyc_Absyn_Tuniondecl* _temp67; struct Cyc_Absyn_Structdecl* _temp69; struct Cyc_Absyn_Enumfield*
_temp71; struct Cyc_Absyn_Enumdecl* _temp73; _LL55: if(*(( int*) _temp53) == Cyc_Tcenv_VarRes){
_LL64: _temp63=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp53)->f1; goto
_LL56;} else{ goto _LL57;} _LL57: if(*(( int*) _temp53) == Cyc_Tcenv_TunionRes){
_LL68: _temp67=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp53)->f1; goto _LL66; _LL66: _temp65=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp53)->f2; goto _LL58;} else{ goto _LL59;}
_LL59: if(*(( int*) _temp53) == Cyc_Tcenv_StructRes){ _LL70: _temp69=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp53)->f1; goto
_LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp53) == Cyc_Tcenv_EnumRes){
_LL74: _temp73=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp53)->f1; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp53)->f2; goto _LL62;} else{ goto _LL54;} _LL56:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp75=(
struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp75[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp76; _temp76.tag= Cyc_Absyn_FnCall_e;
_temp76.f1= e1; _temp76.f2= es; _temp76;}); _temp75;}))); _npop_handler( 0u);
return; _LL58: if( _temp65->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp77= Cyc_Absynpp_qvar2string( _temp65->name); xprintf("%.*s is a constant, not a function",
_temp77.last_plus_one - _temp77.curr, _temp77.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp78=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp78[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp79; _temp79.tag= Cyc_Absyn_Tunion_e; _temp79.f1= 0; _temp79.f2= 0; _temp79.f3=
es; _temp79.f4= _temp67; _temp79.f5= _temp65; _temp79;}); _temp78;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL60: { struct Cyc_List_List*
des=(( struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp69->name != 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp80=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp80[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp81; _temp81.tag= Cyc_Absyn_Struct_e;
_temp81.f1=( struct _tuple1*)({ struct Cyc_Core_Opt* _temp82= _temp69->name; if(
_temp82 == 0){ _throw( Null_Exception);} _temp82->v;}); _temp81.f2= 0; _temp81.f3=
des; _temp81.f4=( struct Cyc_Absyn_Structdecl*) _temp69; _temp81;}); _temp80;})));}
else{ Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp83=( char*)"missing struct name";
struct _tagged_string _temp84; _temp84.curr= _temp83; _temp84.base= _temp83;
_temp84.last_plus_one= _temp83 + 20; _temp84;}));( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp85=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp85[ 0]=({ struct Cyc_Absyn_Struct_e_struct
_temp86; _temp86.tag= Cyc_Absyn_Struct_e; _temp86.f1= _temp49; _temp86.f2= 0;
_temp86.f3= des; _temp86.f4=( struct Cyc_Absyn_Structdecl*) _temp69; _temp86;});
_temp85;})));} _npop_handler( 0u); return;} _LL62: Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp87= Cyc_Absynpp_qvar2string( _temp49); xprintf("%.*s is an enum constructor, not a function",
_temp87.last_plus_one - _temp87.curr, _temp87.curr);})); _npop_handler( 0u);
return; _LL54:;}; _pop_handler();} else{ void* _temp89= _temp52; _LL91: if(
_temp89 == Cyc_Dict_Absent){ goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94;
_LL92: Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp95= Cyc_Absynpp_qvar2string(
_temp49); xprintf("unbound identifier %.*s", _temp95.last_plus_one - _temp95.curr,
_temp95.curr);}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp96=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp96[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp97; _temp97.tag= Cyc_Absyn_FnCall_e;
_temp97.f1= e1; _temp97.f2= es; _temp97;}); _temp96;}))); return; _LL94:( void)
_throw( _temp89); _LL90:;}}} _LL48:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp98=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp98[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp99; _temp99.tag= Cyc_Absyn_FnCall_e;
_temp99.f1= e1; _temp99.f2= es; _temp99;}); _temp98;}))); return; _LL44:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp100=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp100[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp101; _temp101.tag= Cyc_Absyn_Array_e;
_temp101.f1= des; _temp101;}); _temp100;}))); return;}{ void* t=({ void**
_temp132= topt; if( _temp132 == 0){ _throw( Null_Exception);}* _temp132;}); void*
_temp102= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp110; struct Cyc_Absyn_Tqual
_temp112; void* _temp114; struct Cyc_Absyn_Structdecl** _temp116; struct Cyc_List_List*
_temp118; struct _tuple1* _temp120; _LL104: if(( unsigned int) _temp102 > 4u?*((
int*) _temp102) == Cyc_Absyn_ArrayType: 0){ _LL115: _temp114=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp102)->f1; goto _LL113; _LL113: _temp112=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp102)->f2;
goto _LL111; _LL111: _temp110=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp102)->f3; goto _LL105;} else{ goto _LL106;} _LL106: if(( unsigned int)
_temp102 > 4u?*(( int*) _temp102) == Cyc_Absyn_StructType: 0){ _LL121: _temp120=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp102)->f1; goto
_LL119; _LL119: _temp118=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp102)->f2; goto _LL117; _LL117: _temp116=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp102)->f3; goto _LL107;} else{ goto
_LL108;} _LL108: goto _LL109; _LL105:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp122=( struct Cyc_Absyn_Array_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp122[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp123; _temp123.tag= Cyc_Absyn_Array_e; _temp123.f1= des; _temp123;});
_temp122;}))); goto _LL103; _LL107: if( _temp116 == 0? 1: _temp120 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp124=( char*)"Compiler bug: struct type not properly set";
struct _tagged_string _temp125; _temp125.curr= _temp124; _temp125.base= _temp124;
_temp125.last_plus_one= _temp124 + 43; _temp125;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp126=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp126[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp127; _temp127.tag= Cyc_Absyn_Struct_e; _temp127.f1=(
struct _tuple1*)({ struct _tuple1* _temp128= _temp120; if( _temp128 == 0){
_throw( Null_Exception);} _temp128;}); _temp127.f2= 0; _temp127.f3= des;
_temp127.f4=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl**
_temp129= _temp116; if( _temp129 == 0){ _throw( Null_Exception);}* _temp129;});
_temp127;}); _temp126;}))); goto _LL103; _LL109:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Array_e_struct* _temp130=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp130[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp131; _temp131.tag= Cyc_Absyn_Array_e; _temp131.f1= des; _temp131;});
_temp130;}))); goto _LL103; _LL103:;}} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void
Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* es){ for(
0; es != 0; es=({ struct Cyc_List_List* _temp133= es; if( _temp133 == 0){ _throw(
Null_Exception);} _temp133->tl;})){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp134= es; if( _temp134 == 0){ _throw( Null_Exception);}
_temp134->hd;}));}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp(
te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){
Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp136= msg_part; struct
_tagged_string _temp137= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp135= e->topt; if( _temp135 == 0){ _throw( Null_Exception);} _temp135->v;}));
xprintf("test of %.*s has type %.*s instead of integral or * type", _temp136.last_plus_one
- _temp136.curr, _temp136.curr, _temp137.last_plus_one - _temp137.curr, _temp137.curr);}));}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp138= c;
char _temp154; void* _temp156; short _temp158; void* _temp160; long long
_temp162; void* _temp164; struct _tagged_string _temp166; int _temp168; void*
_temp170; struct _tagged_string _temp172; _LL140: if(( unsigned int) _temp138 >
1u?*(( int*) _temp138) == Cyc_Absyn_Char_c: 0){ _LL157: _temp156=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp138)->f1; goto _LL155; _LL155: _temp154=(
char)(( struct Cyc_Absyn_Char_c_struct*) _temp138)->f2; goto _LL141;} else{ goto
_LL142;} _LL142: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_Short_c:
0){ _LL161: _temp160=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp138)->f1;
goto _LL159; _LL159: _temp158=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp138)->f2; goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int)
_temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_LongLong_c: 0){ _LL165: _temp164=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp138)->f1; goto _LL163; _LL163:
_temp162=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp138)->f2; goto
_LL145;} else{ goto _LL146;} _LL146: if(( unsigned int) _temp138 > 1u?*(( int*)
_temp138) == Cyc_Absyn_Float_c: 0){ _LL167: _temp166=( struct _tagged_string)((
struct Cyc_Absyn_Float_c_struct*) _temp138)->f1; goto _LL147;} else{ goto _LL148;}
_LL148: if(( unsigned int) _temp138 > 1u?*(( int*) _temp138) == Cyc_Absyn_Int_c:
0){ _LL171: _temp170=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp138)->f1;
goto _LL169; _LL169: _temp168=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp138)->f2;
goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int) _temp138 > 1u?*((
int*) _temp138) == Cyc_Absyn_String_c: 0){ _LL173: _temp172=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp138)->f1; goto _LL151;}
else{ goto _LL152;} _LL152: if( _temp138 ==( void*) Cyc_Absyn_Null_c){ goto
_LL153;} else{ goto _LL139;} _LL141: t= _temp156 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL139; _LL143: t= _temp160 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL139; _LL145:
t= _temp164 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t;
goto _LL139; _LL147: t= Cyc_Absyn_float_t; goto _LL139; _LL149: if( topt == 0){
t= _temp170 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp175=({ void** _temp174= topt; if( _temp174 == 0){ _throw(
Null_Exception);}* _temp174;}); void* _temp185; void* _temp187; void* _temp189;
void* _temp191; void* _temp193; void* _temp195; _LL177: if(( unsigned int)
_temp175 > 4u?*(( int*) _temp175) == Cyc_Absyn_IntType: 0){ _LL188: _temp187=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; goto _LL186; _LL186:
_temp185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if(
_temp185 ==( void*) Cyc_Absyn_B1){ goto _LL178;} else{ goto _LL179;}} else{ goto
_LL179;} _LL179: if(( unsigned int) _temp175 > 4u?*(( int*) _temp175) == Cyc_Absyn_IntType:
0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1;
goto _LL190; _LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if( _temp189 ==( void*) Cyc_Absyn_B2){ goto _LL180;} else{ goto
_LL181;}} else{ goto _LL181;} _LL181: if(( unsigned int) _temp175 > 4u?*(( int*)
_temp175) == Cyc_Absyn_IntType: 0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f1; goto _LL194; _LL194: _temp193=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; goto _LL182;} else{ goto _LL183;} _LL183: goto _LL184; _LL178: t=
_temp187 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp197=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp197[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp198; _temp198.tag= Cyc_Absyn_Const_e;
_temp198.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp199=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp199[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp200; _temp200.tag= Cyc_Absyn_Char_c;
_temp200.f1=( void*) _temp187; _temp200.f2=( char) _temp168; _temp200;});
_temp199;})); _temp198;}); _temp197;}))); goto _LL176; _LL180: t= _temp191 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp201=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp202; _temp202.tag= Cyc_Absyn_Const_e; _temp202.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp203=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp203[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp204; _temp204.tag= Cyc_Absyn_Short_c; _temp204.f1=( void*) _temp191;
_temp204.f2=( short) _temp168; _temp204;}); _temp203;})); _temp202;}); _temp201;})));
goto _LL176; _LL182: t= _temp195 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL176; _LL184: t= _temp170 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL176; _LL176:;} goto _LL139; _LL151: {
int len=( int)({ struct _tagged_string _temp253= _temp172;( unsigned int)(
_temp253.last_plus_one - _temp253.curr);}); struct Cyc_Absyn_Const_e_struct* r=({
struct Cyc_Absyn_Const_e_struct* _temp249=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp249[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp250; _temp250.tag= Cyc_Absyn_Const_e; _temp250.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp251=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp251[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp252; _temp252.tag= Cyc_Absyn_Int_c; _temp252.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp252.f2= len; _temp252;}); _temp251;})); _temp250;}); _temp249;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) r, loc); elen->topt=({ struct
Cyc_Core_Opt* _temp205=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp205->v=( void*) Cyc_Absyn_uint_t; _temp205;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp206=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp206[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp207; _temp207.tag= Cyc_Absyn_Upper_b;
_temp207.f1= elen; _temp207;}); _temp206;})); if( topt != 0){ void* _temp209=
Cyc_Tcutil_compress(({ void** _temp208= topt; if( _temp208 == 0){ _throw(
Null_Exception);}* _temp208;})); struct Cyc_Absyn_Exp* _temp217; struct Cyc_Absyn_Tqual
_temp219; void* _temp221; struct Cyc_Absyn_PtrInfo _temp223; struct Cyc_Absyn_Conref*
_temp225; struct Cyc_Absyn_Tqual _temp227; struct Cyc_Absyn_Conref* _temp229;
void* _temp231; void* _temp233; _LL211: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_ArrayType: 0){ _LL222: _temp221=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp209)->f1; goto _LL220; _LL220: _temp219=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp209)->f2; goto _LL218; _LL218: _temp217=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp209)->f3; goto
_LL212;} else{ goto _LL213;} _LL213: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_PointerType: 0){ _LL224: _temp223=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp209)->f1; _LL234: _temp233=( void*)
_temp223.elt_typ; goto _LL232; _LL232: _temp231=( void*) _temp223.rgn_typ; goto
_LL230; _LL230: _temp229=( struct Cyc_Absyn_Conref*) _temp223.nullable; goto
_LL228; _LL228: _temp227=( struct Cyc_Absyn_Tqual) _temp223.tq; goto _LL226;
_LL226: _temp225=( struct Cyc_Absyn_Conref*) _temp223.bounds; goto _LL214;}
else{ goto _LL215;} _LL215: goto _LL216; _LL212: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp235=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp235[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp236; _temp236.tag= Cyc_Absyn_ArrayType;
_temp236.f1=( void*) Cyc_Absyn_uchar_t; _temp236.f2= _temp219; _temp236.f3=(
struct Cyc_Absyn_Exp*) elen; _temp236;}); _temp235;}); _LL214: if( ! Cyc_Tcutil_unify(({
void** _temp237= topt; if( _temp237 == 0){ _throw( Null_Exception);}* _temp237;}),
t)? Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp238= topt; if(
_temp238 == 0){ _throw( Null_Exception);}* _temp238;})): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp239=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp239->v=( void*) t; _temp239;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp240= topt; if( _temp240 == 0){ _throw( Null_Exception);}* _temp240;})); t=({
void** _temp241= topt; if( _temp241 == 0){ _throw( Null_Exception);}* _temp241;});}
else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp242=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp242[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp243; _temp243.tag= Cyc_Absyn_Upper_b;
_temp243.f1= elen; _temp243;}); _temp242;})); if( ! Cyc_Tcutil_unify(({ void**
_temp244= topt; if( _temp244 == 0){ _throw( Null_Exception);}* _temp244;}), t)?
Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp245= topt; if( _temp245 ==
0){ _throw( Null_Exception);}* _temp245;})): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp246=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp246->v=( void*) t; _temp246;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp247= topt; if( _temp247 == 0){ _throw( Null_Exception);}* _temp247;})); t=({
void** _temp248= topt; if( _temp248 == 0){ _throw( Null_Exception);}* _temp248;});}}
goto _LL210; _LL216: goto _LL210; _LL210:;} return t;} _LL153: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp254=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp254[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp255; _temp255.tag= Cyc_Absyn_PointerType;
_temp255.f1=({ struct Cyc_Absyn_PtrInfo _temp256; _temp256.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); _temp256.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp256.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp256.tq= Cyc_Absyn_empty_tqual(); _temp256.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp256;}); _temp255;});
_temp254;}); goto _LL139; _LL139:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp257= b; struct Cyc_Absyn_Vardecl* _temp271; struct Cyc_Absyn_Fndecl*
_temp273; struct Cyc_Absyn_Vardecl* _temp275; struct Cyc_Absyn_Vardecl* _temp277;
struct Cyc_Absyn_Vardecl* _temp279; _LL259: if( _temp257 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp257 > 1u?*((
int*) _temp257) == Cyc_Absyn_Global_b: 0){ _LL272: _temp271=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp257)->f1; goto _LL262;} else{ goto
_LL263;} _LL263: if(( unsigned int) _temp257 > 1u?*(( int*) _temp257) == Cyc_Absyn_Funname_b:
0){ _LL274: _temp273=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp257)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp257 > 1u?*(( int*) _temp257) == Cyc_Absyn_Pat_b: 0){ _LL276: _temp275=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*) _temp257)->f1; goto
_LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp257 > 1u?*(( int*)
_temp257) == Cyc_Absyn_Local_b: 0){ _LL278: _temp277=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp257)->f1; goto _LL268;} else{ goto _LL269;}
_LL269: if(( unsigned int) _temp257 > 1u?*(( int*) _temp257) == Cyc_Absyn_Param_b:
0){ _LL280: _temp279=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp257)->f1; goto _LL270;} else{ goto _LL258;} _LL260: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp281= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp281.last_plus_one - _temp281.curr, _temp281.curr);})); _LL262:({ struct
_tuple1* _temp282= q; unsigned int _temp283= 0; if( _temp283 >= 1u){ _throw(
Null_Exception);} _temp282[ _temp283]=({ struct _tuple1* _temp284= _temp271->name;
unsigned int _temp285= 0; if( _temp285 >= 1u){ _throw( Null_Exception);}
_temp284[ _temp285];});}); return( void*) _temp271->type; _LL264:({ struct
_tuple1* _temp286= q; unsigned int _temp287= 0; if( _temp287 >= 1u){ _throw(
Null_Exception);} _temp286[ _temp287]=({ struct _tuple1* _temp288= _temp273->name;
unsigned int _temp289= 0; if( _temp289 >= 1u){ _throw( Null_Exception);}
_temp288[ _temp289];});}); return Cyc_Tcutil_fndecl2typ( _temp273); _LL266:
_temp277= _temp275; goto _LL268; _LL268: _temp279= _temp277; goto _LL270; _LL270:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp279->type; _LL258:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp290=( void*) fmt->r; void* _temp296; struct _tagged_string _temp298;
_LL292: if(*(( int*) _temp290) == Cyc_Absyn_Const_e){ _LL297: _temp296=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp290)->f1; if(( unsigned int) _temp296 > 1u?*((
int*) _temp296) == Cyc_Absyn_String_c: 0){ _LL299: _temp298=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp296)->f1; goto _LL293;}
else{ goto _LL294;}} else{ goto _LL294;} _LL294: goto _LL295; _LL293: desc_types=
type_getter( _temp298, fmt->loc); goto _LL291; _LL295: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp300=( char*)"expecting a literal format string";
struct _tagged_string _temp301; _temp301.curr= _temp300; _temp301.base= _temp300;
_temp301.last_plus_one= _temp300 + 34; _temp301;})); return; _LL291:;} for( 0;
desc_types != 0? args != 0: 0; desc_types=({ struct Cyc_List_List* _temp302=
desc_types; if( _temp302 == 0){ _throw( Null_Exception);} _temp302->tl;}), args=({
struct Cyc_List_List* _temp303= args; if( _temp303 == 0){ _throw( Null_Exception);}
_temp303->tl;})){ void* t=( void*)({ struct Cyc_List_List* _temp308= desc_types;
if( _temp308 == 0){ _throw( Null_Exception);} _temp308->hd;}); struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp307= args; if( _temp307
== 0){ _throw( Null_Exception);} _temp307->hd;}); if( ! Cyc_Tcutil_coerce_arg(
te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp305= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp306= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp304= e->topt; if( _temp304 == 0){ _throw( Null_Exception);} _temp304->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp305.last_plus_one
- _temp305.curr, _temp305.curr, _temp306.last_plus_one - _temp306.curr, _temp306.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp309=( char*)"too few arguments"; struct _tagged_string _temp310; _temp310.curr=
_temp309; _temp310.base= _temp309; _temp310.last_plus_one= _temp309 + 18;
_temp310;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp311= args; if( _temp311 == 0){ _throw( Null_Exception);}
_temp311->hd;}))->loc,( struct _tagged_string)({ char* _temp312=( char*)"too many arguments";
struct _tagged_string _temp313; _temp313.curr= _temp312; _temp313.base= _temp312;
_temp313.last_plus_one= _temp312 + 19; _temp313;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp362= e->topt; if( _temp362 == 0){ _throw( Null_Exception);} _temp362->v;}));
void* _temp314= p; _LL316: if( _temp314 ==( void*) Cyc_Absyn_Plus){ goto _LL317;}
else{ goto _LL318;} _LL318: if( _temp314 ==( void*) Cyc_Absyn_Minus){ goto
_LL319;} else{ goto _LL320;} _LL320: if( _temp314 ==( void*) Cyc_Absyn_Not){
goto _LL321;} else{ goto _LL322;} _LL322: if( _temp314 ==( void*) Cyc_Absyn_Bitnot){
goto _LL323;} else{ goto _LL324;} _LL324: if( _temp314 ==( void*) Cyc_Absyn_Size){
goto _LL325;} else{ goto _LL326;} _LL326: goto _LL327; _LL317: goto _LL319;
_LL319: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp328= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_temp328.last_plus_one - _temp328.curr, _temp328.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp329= e->topt; if( _temp329 == 0){ _throw(
Null_Exception);} _temp329->v;}); _LL321: Cyc_Tcutil_check_contains_assign( e);
if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp330= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp330.last_plus_one - _temp330.curr, _temp330.curr);}));} return Cyc_Absyn_sint_t;
_LL323: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp331= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp331.last_plus_one - _temp331.curr, _temp331.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp332= e->topt; if( _temp332 == 0){ _throw(
Null_Exception);} _temp332->v;}); _LL325:{ void* _temp333= t; struct Cyc_Absyn_Exp*
_temp341; struct Cyc_Absyn_Tqual _temp343; void* _temp345; struct Cyc_Absyn_PtrInfo
_temp347; struct Cyc_Absyn_Conref* _temp349; struct Cyc_Absyn_Tqual _temp351;
struct Cyc_Absyn_Conref* _temp353; void* _temp355; void* _temp357; _LL335: if((
unsigned int) _temp333 > 4u?*(( int*) _temp333) == Cyc_Absyn_ArrayType: 0){
_LL346: _temp345=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp333)->f1;
goto _LL344; _LL344: _temp343=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp333)->f2; goto _LL342; _LL342: _temp341=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp333)->f3; goto _LL336;} else{ goto _LL337;}
_LL337: if(( unsigned int) _temp333 > 4u?*(( int*) _temp333) == Cyc_Absyn_PointerType:
0){ _LL348: _temp347=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp333)->f1; _LL358: _temp357=( void*) _temp347.elt_typ; goto _LL356; _LL356:
_temp355=( void*) _temp347.rgn_typ; goto _LL354; _LL354: _temp353=( struct Cyc_Absyn_Conref*)
_temp347.nullable; goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Tqual)
_temp347.tq; goto _LL350; _LL350: _temp349=( struct Cyc_Absyn_Conref*) _temp347.bounds;
goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL336: goto _LL334;
_LL338: goto _LL334; _LL340: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp359= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_temp359.last_plus_one - _temp359.curr, _temp359.curr);})); goto _LL334; _LL334:;}
return Cyc_Absyn_uint_t; _LL327: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp360=( char*)"Non-unary primop"; struct
_tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 17; _temp361;})); _LL315:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp364= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp363= e1->topt; if( _temp363 == 0){ _throw(
Null_Exception);} _temp363->v;})); xprintf("type %.*s cannot be used here",
_temp364.last_plus_one - _temp364.curr, _temp364.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp366=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp365= e2->topt; if(
_temp365 == 0){ _throw( Null_Exception);} _temp365->v;})); xprintf("type %.*s cannot be used here",
_temp366.last_plus_one - _temp366.curr, _temp366.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp368= e1->topt;
if( _temp368 == 0){ _throw( Null_Exception);} _temp368->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp367= e2->topt; if( _temp367 == 0){ _throw(
Null_Exception);} _temp367->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp371=
e1->topt; if( _temp371 == 0){ _throw( Null_Exception);} _temp371->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp370= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp369= e2->topt; if( _temp369 == 0){ _throw( Null_Exception);} _temp369->v;}));
xprintf("expecting int but found %.*s", _temp370.last_plus_one - _temp370.curr,
_temp370.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({
struct Cyc_Core_Opt* _temp379= e1->topt; if( _temp379 == 0){ _throw(
Null_Exception);} _temp379->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp378= e2->topt; if( _temp378 == 0){ _throw( Null_Exception);} _temp378->v;});
if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if( Cyc_Tcutil_is_tagged_pointer_typ(
t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr( e1->loc,({ struct
_tagged_string _temp374= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp372= e1->topt; if( _temp372 == 0){ _throw( Null_Exception);} _temp372->v;}));
struct _tagged_string _temp375= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp373= e2->topt; if( _temp373 == 0){ _throw( Null_Exception);} _temp373->v;}));
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp374.last_plus_one - _temp374.curr, _temp374.curr, _temp375.last_plus_one -
_temp375.curr, _temp375.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp376= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp377= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp376.last_plus_one - _temp376.curr, _temp376.curr, _temp377.last_plus_one -
_temp377.curr, _temp377.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp420= e1->topt; if( _temp420 == 0){ _throw(
Null_Exception);} _temp420->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp419= e2->topt; if( _temp419 == 0){ _throw(
Null_Exception);} _temp419->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp418= e1->topt; if( _temp418 == 0){ _throw( Null_Exception);} _temp418->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp417= e2->topt; if( _temp417 == 0){
_throw( Null_Exception);} _temp417->v;});{ struct _tuple0 _temp381=({ struct
_tuple0 _temp380; _temp380.f1= Cyc_Tcutil_compress( t1); _temp380.f2= Cyc_Tcutil_compress(
t2); _temp380;}); void* _temp387; struct Cyc_Absyn_PtrInfo _temp389; struct Cyc_Absyn_Conref*
_temp391; struct Cyc_Absyn_Tqual _temp393; struct Cyc_Absyn_Conref* _temp395;
void* _temp397; void* _temp399; void* _temp401; struct Cyc_Absyn_PtrInfo
_temp403; struct Cyc_Absyn_Conref* _temp405; struct Cyc_Absyn_Tqual _temp407;
struct Cyc_Absyn_Conref* _temp409; void* _temp411; void* _temp413; _LL383:
_LL402: _temp401= _temp381.f1; if(( unsigned int) _temp401 > 4u?*(( int*)
_temp401) == Cyc_Absyn_PointerType: 0){ _LL404: _temp403=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp401)->f1; _LL414: _temp413=( void*)
_temp403.elt_typ; goto _LL412; _LL412: _temp411=( void*) _temp403.rgn_typ; goto
_LL410; _LL410: _temp409=( struct Cyc_Absyn_Conref*) _temp403.nullable; goto
_LL408; _LL408: _temp407=( struct Cyc_Absyn_Tqual) _temp403.tq; goto _LL406;
_LL406: _temp405=( struct Cyc_Absyn_Conref*) _temp403.bounds; goto _LL388;}
else{ goto _LL385;} _LL388: _temp387= _temp381.f2; if(( unsigned int) _temp387 >
4u?*(( int*) _temp387) == Cyc_Absyn_PointerType: 0){ _LL390: _temp389=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp387)->f1; _LL400:
_temp399=( void*) _temp389.elt_typ; goto _LL398; _LL398: _temp397=( void*)
_temp389.rgn_typ; goto _LL396; _LL396: _temp395=( struct Cyc_Absyn_Conref*)
_temp389.nullable; goto _LL394; _LL394: _temp393=( struct Cyc_Absyn_Tqual)
_temp389.tq; goto _LL392; _LL392: _temp391=( struct Cyc_Absyn_Conref*) _temp389.bounds;
goto _LL384;} else{ goto _LL385;} _LL385: goto _LL386; _LL384: if( Cyc_Tcutil_unify(
_temp413, _temp399)){ return Cyc_Absyn_sint_t;} goto _LL382; _LL386: goto _LL382;
_LL382:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp415= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp416= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp415.last_plus_one - _temp415.curr, _temp415.curr, _temp416.last_plus_one -
_temp416.curr, _temp416.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp421= p; _LL423: if( _temp421 ==( void*) Cyc_Absyn_Plus){ goto _LL424;}
else{ goto _LL425;} _LL425: if( _temp421 ==( void*) Cyc_Absyn_Minus){ goto
_LL426;} else{ goto _LL427;} _LL427: if( _temp421 ==( void*) Cyc_Absyn_Times){
goto _LL428;} else{ goto _LL429;} _LL429: if( _temp421 ==( void*) Cyc_Absyn_Div){
goto _LL430;} else{ goto _LL431;} _LL431: if( _temp421 ==( void*) Cyc_Absyn_Mod){
goto _LL432;} else{ goto _LL433;} _LL433: if( _temp421 ==( void*) Cyc_Absyn_Bitand){
goto _LL434;} else{ goto _LL435;} _LL435: if( _temp421 ==( void*) Cyc_Absyn_Bitor){
goto _LL436;} else{ goto _LL437;} _LL437: if( _temp421 ==( void*) Cyc_Absyn_Bitxor){
goto _LL438;} else{ goto _LL439;} _LL439: if( _temp421 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL440;} else{ goto _LL441;} _LL441: if( _temp421 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL442;} else{ goto _LL443;} _LL443: if( _temp421 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL444;} else{ goto _LL445;} _LL445: if( _temp421 ==( void*) Cyc_Absyn_Eq){
goto _LL446;} else{ goto _LL447;} _LL447: if( _temp421 ==( void*) Cyc_Absyn_Neq){
goto _LL448;} else{ goto _LL449;} _LL449: if( _temp421 ==( void*) Cyc_Absyn_Gt){
goto _LL450;} else{ goto _LL451;} _LL451: if( _temp421 ==( void*) Cyc_Absyn_Lt){
goto _LL452;} else{ goto _LL453;} _LL453: if( _temp421 ==( void*) Cyc_Absyn_Gte){
goto _LL454;} else{ goto _LL455;} _LL455: if( _temp421 ==( void*) Cyc_Absyn_Lte){
goto _LL456;} else{ goto _LL457;} _LL457: goto _LL458; _LL424: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL426: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL428: goto _LL430;
_LL430: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL432: goto _LL434; _LL434: goto _LL436; _LL436: goto _LL438; _LL438: goto
_LL440; _LL440: goto _LL442; _LL442: goto _LL444; _LL444: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL446: goto _LL448; _LL448: goto _LL450;
_LL450: goto _LL452; _LL452: goto _LL454; _LL454: goto _LL456; _LL456: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL458:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp459=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp459[ 0]=({ struct Cyc_Core_Failure_struct
_temp460; _temp460.tag= Cyc_Core_Failure; _temp460.f1=( struct _tagged_string)({
char* _temp461=( char*)"bad binary primop"; struct _tagged_string _temp462;
_temp462.curr= _temp461; _temp462.base= _temp461; _temp462.last_plus_one=
_temp461 + 18; _temp462;}); _temp460;}); _temp459;})); _LL422:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp463= es; if(
_temp463 == 0){ _throw( Null_Exception);} _temp463->hd;}));} Cyc_Tcexp_tcExpList(
te, es);{ void* t;{ void* _temp464= p; _LL466: if( _temp464 ==( void*) Cyc_Absyn_Printf){
goto _LL467;} else{ goto _LL468;} _LL468: if( _temp464 ==( void*) Cyc_Absyn_Fprintf){
goto _LL469;} else{ goto _LL470;} _LL470: if( _temp464 ==( void*) Cyc_Absyn_Xprintf){
goto _LL471;} else{ goto _LL472;} _LL472: if( _temp464 ==( void*) Cyc_Absyn_Scanf){
goto _LL473;} else{ goto _LL474;} _LL474: if( _temp464 ==( void*) Cyc_Absyn_Fscanf){
goto _LL475;} else{ goto _LL476;} _LL476: if( _temp464 ==( void*) Cyc_Absyn_Sscanf){
goto _LL477;} else{ goto _LL478;} _LL478: goto _LL479; _LL467: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp480=(
char*)"missing format string in printf"; struct _tagged_string _temp481;
_temp481.curr= _temp480; _temp481.base= _temp480; _temp481.last_plus_one=
_temp480 + 32; _temp481;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp482= es; if( _temp482 == 0){ _throw( Null_Exception);}
_temp482->hd;}),({ struct Cyc_List_List* _temp483= es; if( _temp483 == 0){
_throw( Null_Exception);} _temp483->tl;}), Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL465; _LL469: if( es == 0? 1:({ struct Cyc_List_List*
_temp484= es; if( _temp484 == 0){ _throw( Null_Exception);} _temp484->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp485=(
char*)"too few arguments to fprintf"; struct _tagged_string _temp486; _temp486.curr=
_temp485; _temp486.base= _temp485; _temp486.last_plus_one= _temp485 + 29;
_temp486;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp488=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp487= es; if( _temp487 == 0){
_throw( Null_Exception);} _temp487->hd;}))->topt; if( _temp488 == 0){ _throw(
Null_Exception);} _temp488->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp489= es; if( _temp489 == 0){
_throw( Null_Exception);} _temp489->hd;}))->loc,({ struct _tagged_string
_temp492= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp491=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp490= es; if( _temp490 == 0){
_throw( Null_Exception);} _temp490->hd;}))->topt; if( _temp491 == 0){ _throw(
Null_Exception);} _temp491->v;})); xprintf("expecting FILE @ but found %.*s",
_temp492.last_plus_one - _temp492.curr, _temp492.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp494=({ struct Cyc_List_List*
_temp493= es; if( _temp493 == 0){ _throw( Null_Exception);} _temp493->tl;}); if(
_temp494 == 0){ _throw( Null_Exception);} _temp494->hd;}),({ struct Cyc_List_List*
_temp496=({ struct Cyc_List_List* _temp495= es; if( _temp495 == 0){ _throw(
Null_Exception);} _temp495->tl;}); if( _temp496 == 0){ _throw( Null_Exception);}
_temp496->tl;}), Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL465; _LL471: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp497=( char*)"missing format string in xprintf";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 33; _temp498;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp499= es; if( _temp499
== 0){ _throw( Null_Exception);} _temp499->hd;}),({ struct Cyc_List_List*
_temp500= es; if( _temp500 == 0){ _throw( Null_Exception);} _temp500->tl;}), Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL465; _LL473: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp501=( char*)"missing format string in scanf";
struct _tagged_string _temp502; _temp502.curr= _temp501; _temp502.base= _temp501;
_temp502.last_plus_one= _temp501 + 31; _temp502;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp503= es; if( _temp503
== 0){ _throw( Null_Exception);} _temp503->hd;}),({ struct Cyc_List_List*
_temp504= es; if( _temp504 == 0){ _throw( Null_Exception);} _temp504->tl;}), Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL465; _LL475: if( es == 0? 1:({ struct Cyc_List_List*
_temp505= es; if( _temp505 == 0){ _throw( Null_Exception);} _temp505->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp506=(
char*)"too few arguments to fscanf"; struct _tagged_string _temp507; _temp507.curr=
_temp506; _temp507.base= _temp506; _temp507.last_plus_one= _temp506 + 28;
_temp507;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp509=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp508= es; if( _temp508 == 0){
_throw( Null_Exception);} _temp508->hd;}))->topt; if( _temp509 == 0){ _throw(
Null_Exception);} _temp509->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp510= es; if( _temp510 == 0){
_throw( Null_Exception);} _temp510->hd;}))->loc,({ struct _tagged_string
_temp513= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp512=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp511= es; if( _temp511 == 0){
_throw( Null_Exception);} _temp511->hd;}))->topt; if( _temp512 == 0){ _throw(
Null_Exception);} _temp512->v;})); xprintf("expecting FILE @ but found %.*s",
_temp513.last_plus_one - _temp513.curr, _temp513.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp515=({ struct Cyc_List_List*
_temp514= es; if( _temp514 == 0){ _throw( Null_Exception);} _temp514->tl;}); if(
_temp515 == 0){ _throw( Null_Exception);} _temp515->hd;}),({ struct Cyc_List_List*
_temp517=({ struct Cyc_List_List* _temp516= es; if( _temp516 == 0){ _throw(
Null_Exception);} _temp516->tl;}); if( _temp517 == 0){ _throw( Null_Exception);}
_temp517->tl;}), Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL465;
_LL477: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp518=( char*)"sscanf is not supported";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 24; _temp519;})); t= Cyc_Absyn_sint_t; goto
_LL465; _LL479: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL520: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp522=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 35; _temp523;})); case 1: _LL521: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp525= es;
if( _temp525 == 0){ _throw( Null_Exception);} _temp525->hd;})); break; case 2:
_LL524: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp527= es; if( _temp527 == 0){ _throw( Null_Exception);}
_temp527->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp529=({
struct Cyc_List_List* _temp528= es; if( _temp528 == 0){ _throw( Null_Exception);}
_temp528->tl;}); if( _temp529 == 0){ _throw( Null_Exception);} _temp529->hd;}));
break; default: _LL526: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp531=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp532; _temp532.curr= _temp531; _temp532.base= _temp531;
_temp532.last_plus_one= _temp531 + 37; _temp532;}));} goto _LL465; _LL465:;}
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp533=( void*) e->r; void* _temp553; struct Cyc_Absyn_Vardecl* _temp555;
struct _tuple1* _temp557; void* _temp559; struct Cyc_Absyn_Vardecl* _temp561;
struct _tuple1* _temp563; void* _temp565; struct Cyc_Absyn_Vardecl* _temp567;
struct _tuple1* _temp569; void* _temp571; struct Cyc_Absyn_Vardecl* _temp573;
struct _tuple1* _temp575; struct Cyc_Absyn_Exp* _temp577; struct Cyc_Absyn_Exp*
_temp579; struct _tagged_string* _temp581; struct Cyc_Absyn_Exp* _temp583;
struct _tagged_string* _temp585; struct Cyc_Absyn_Exp* _temp587; struct Cyc_Absyn_Exp*
_temp589; _LL535: if(*(( int*) _temp533) == Cyc_Absyn_Var_e){ _LL558: _temp557=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp533)->f1; goto _LL554;
_LL554: _temp553=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp533)->f2; if((
unsigned int) _temp553 > 1u?*(( int*) _temp553) == Cyc_Absyn_Param_b: 0){ _LL556:
_temp555=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp553)->f1; goto _LL536;} else{ goto _LL537;}} else{ goto _LL537;} _LL537:
if(*(( int*) _temp533) == Cyc_Absyn_Var_e){ _LL564: _temp563=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp533)->f1; goto _LL560; _LL560: _temp559=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp533)->f2; if(( unsigned int)
_temp559 > 1u?*(( int*) _temp559) == Cyc_Absyn_Local_b: 0){ _LL562: _temp561=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp559)->f1;
goto _LL538;} else{ goto _LL539;}} else{ goto _LL539;} _LL539: if(*(( int*)
_temp533) == Cyc_Absyn_Var_e){ _LL570: _temp569=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp533)->f1; goto _LL566; _LL566: _temp565=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp533)->f2; if(( unsigned int) _temp565 > 1u?*(( int*) _temp565) == Cyc_Absyn_Pat_b:
0){ _LL568: _temp567=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp565)->f1; goto _LL540;} else{ goto _LL541;}} else{ goto _LL541;} _LL541:
if(*(( int*) _temp533) == Cyc_Absyn_Var_e){ _LL576: _temp575=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp533)->f1; goto _LL572; _LL572: _temp571=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp533)->f2; if(( unsigned int)
_temp571 > 1u?*(( int*) _temp571) == Cyc_Absyn_Global_b: 0){ _LL574: _temp573=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp571)->f1;
goto _LL542;} else{ goto _LL543;}} else{ goto _LL543;} _LL543: if(*(( int*)
_temp533) == Cyc_Absyn_Subscript_e){ _LL580: _temp579=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp533)->f1; goto _LL578; _LL578:
_temp577=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp533)->f2; goto _LL544;} else{ goto _LL545;} _LL545: if(*(( int*) _temp533)
== Cyc_Absyn_StructMember_e){ _LL584: _temp583=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp533)->f1; goto _LL582; _LL582: _temp581=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp533)->f2;
goto _LL546;} else{ goto _LL547;} _LL547: if(*(( int*) _temp533) == Cyc_Absyn_StructArrow_e){
_LL588: _temp587=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp533)->f1; goto _LL586; _LL586: _temp585=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp533)->f2; goto _LL548;} else{ goto _LL549;}
_LL549: if(*(( int*) _temp533) == Cyc_Absyn_Deref_e){ _LL590: _temp589=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp533)->f1; goto _LL550;}
else{ goto _LL551;} _LL551: goto _LL552; _LL536: _temp561= _temp555; goto _LL538;
_LL538: _temp567= _temp561; goto _LL540; _LL540: _temp573= _temp567; goto _LL542;
_LL542: if( !( _temp573->tq).q_const){ return;} goto _LL534; _LL544:{ void*
_temp592= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp591= _temp579->topt;
if( _temp591 == 0){ _throw( Null_Exception);} _temp591->v;})); struct Cyc_Absyn_PtrInfo
_temp602; struct Cyc_Absyn_Conref* _temp604; struct Cyc_Absyn_Tqual _temp606;
struct Cyc_Absyn_Conref* _temp608; void* _temp610; void* _temp612; struct Cyc_Absyn_Exp*
_temp614; struct Cyc_Absyn_Tqual _temp616; void* _temp618; struct Cyc_List_List*
_temp620; _LL594: if(( unsigned int) _temp592 > 4u?*(( int*) _temp592) == Cyc_Absyn_PointerType:
0){ _LL603: _temp602=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp592)->f1; _LL613: _temp612=( void*) _temp602.elt_typ; goto _LL611; _LL611:
_temp610=( void*) _temp602.rgn_typ; goto _LL609; _LL609: _temp608=( struct Cyc_Absyn_Conref*)
_temp602.nullable; goto _LL607; _LL607: _temp606=( struct Cyc_Absyn_Tqual)
_temp602.tq; goto _LL605; _LL605: _temp604=( struct Cyc_Absyn_Conref*) _temp602.bounds;
goto _LL595;} else{ goto _LL596;} _LL596: if(( unsigned int) _temp592 > 4u?*((
int*) _temp592) == Cyc_Absyn_ArrayType: 0){ _LL619: _temp618=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp592)->f1; goto _LL617; _LL617: _temp616=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp592)->f2;
goto _LL615; _LL615: _temp614=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp592)->f3; goto _LL597;} else{ goto _LL598;} _LL598: if(( unsigned int)
_temp592 > 4u?*(( int*) _temp592) == Cyc_Absyn_TupleType: 0){ _LL621: _temp620=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp592)->f1; goto
_LL599;} else{ goto _LL600;} _LL600: goto _LL601; _LL595: _temp616= _temp606;
goto _LL597; _LL597: if( ! _temp616.q_const){ return;} goto _LL593; _LL599: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp577);{ struct _handler_cons
_temp622; _push_handler(& _temp622);{ void* _temp623=( void*) setjmp( _temp622.handler);
if( ! _temp623){{ struct _tuple7 _temp626; void* _temp627; struct Cyc_Absyn_Tqual
_temp629; struct _tuple7* _temp624=(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( _temp620,( int) i); _temp626=* _temp624; _LL630:
_temp629= _temp626.f1; goto _LL628; _LL628: _temp627= _temp626.f2; goto _LL625;
_LL625: if( ! _temp629.q_const){ _npop_handler( 0u); return;}}; _pop_handler();}
else{ void* _temp632= _temp623; _LL634: if( _temp632 == Cyc_List_Nth){ goto
_LL635;} else{ goto _LL636;} _LL636: goto _LL637; _LL635: return; _LL637:( void)
_throw( _temp632); _LL633:;}}} goto _LL593;} _LL601: goto _LL593; _LL593:;} goto
_LL534; _LL546:{ void* _temp639= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp638= _temp583->topt; if( _temp638 == 0){ _throw( Null_Exception);} _temp638->v;}));
struct Cyc_Absyn_Structdecl** _temp645; struct Cyc_List_List* _temp647; struct
_tuple1* _temp649; _LL641: if(( unsigned int) _temp639 > 4u?*(( int*) _temp639)
== Cyc_Absyn_StructType: 0){ _LL650: _temp649=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp639)->f1; goto _LL648; _LL648: _temp647=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp639)->f2; goto _LL646; _LL646: _temp645=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp639)->f3;
goto _LL642;} else{ goto _LL643;} _LL643: goto _LL644; _LL642: { struct Cyc_Absyn_Structfield*
sf= _temp645 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp652= _temp645; if( _temp652 == 0){ _throw(
Null_Exception);}* _temp652;}), _temp581); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp651= sf; if( _temp651 == 0){ _throw( Null_Exception);} _temp651->tq;}).q_const){
return;} goto _LL640;} _LL644: goto _LL640; _LL640:;} goto _LL534; _LL548:{ void*
_temp654= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp653= _temp587->topt;
if( _temp653 == 0){ _throw( Null_Exception);} _temp653->v;})); struct Cyc_Absyn_PtrInfo
_temp660; struct Cyc_Absyn_Conref* _temp662; struct Cyc_Absyn_Tqual _temp664;
struct Cyc_Absyn_Conref* _temp666; void* _temp668; void* _temp670; _LL656: if((
unsigned int) _temp654 > 4u?*(( int*) _temp654) == Cyc_Absyn_PointerType: 0){
_LL661: _temp660=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp654)->f1; _LL671: _temp670=( void*) _temp660.elt_typ; goto _LL669; _LL669:
_temp668=( void*) _temp660.rgn_typ; goto _LL667; _LL667: _temp666=( struct Cyc_Absyn_Conref*)
_temp660.nullable; goto _LL665; _LL665: _temp664=( struct Cyc_Absyn_Tqual)
_temp660.tq; goto _LL663; _LL663: _temp662=( struct Cyc_Absyn_Conref*) _temp660.bounds;
goto _LL657;} else{ goto _LL658;} _LL658: goto _LL659; _LL657:{ void* _temp672=
Cyc_Tcutil_compress( _temp670); struct Cyc_Absyn_Structdecl** _temp678; struct
Cyc_List_List* _temp680; struct _tuple1* _temp682; _LL674: if(( unsigned int)
_temp672 > 4u?*(( int*) _temp672) == Cyc_Absyn_StructType: 0){ _LL683: _temp682=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp672)->f1; goto
_LL681; _LL681: _temp680=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp672)->f2; goto _LL679; _LL679: _temp678=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp672)->f3; goto _LL675;} else{ goto
_LL676;} _LL676: goto _LL677; _LL675: { struct Cyc_Absyn_Structfield* sf=
_temp678 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp685= _temp678; if( _temp685 == 0){ _throw(
Null_Exception);}* _temp685;}), _temp585); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp684= sf; if( _temp684 == 0){ _throw( Null_Exception);} _temp684->tq;}).q_const){
return;} goto _LL673;} _LL677: goto _LL673; _LL673:;} goto _LL655; _LL659: goto
_LL655; _LL655:;} goto _LL534; _LL550:{ void* _temp687= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp686= _temp589->topt; if( _temp686 == 0){
_throw( Null_Exception);} _temp686->v;})); struct Cyc_Absyn_PtrInfo _temp695;
struct Cyc_Absyn_Conref* _temp697; struct Cyc_Absyn_Tqual _temp699; struct Cyc_Absyn_Conref*
_temp701; void* _temp703; void* _temp705; struct Cyc_Absyn_Exp* _temp707; struct
Cyc_Absyn_Tqual _temp709; void* _temp711; _LL689: if(( unsigned int) _temp687 >
4u?*(( int*) _temp687) == Cyc_Absyn_PointerType: 0){ _LL696: _temp695=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp687)->f1; _LL706:
_temp705=( void*) _temp695.elt_typ; goto _LL704; _LL704: _temp703=( void*)
_temp695.rgn_typ; goto _LL702; _LL702: _temp701=( struct Cyc_Absyn_Conref*)
_temp695.nullable; goto _LL700; _LL700: _temp699=( struct Cyc_Absyn_Tqual)
_temp695.tq; goto _LL698; _LL698: _temp697=( struct Cyc_Absyn_Conref*) _temp695.bounds;
goto _LL690;} else{ goto _LL691;} _LL691: if(( unsigned int) _temp687 > 4u?*((
int*) _temp687) == Cyc_Absyn_ArrayType: 0){ _LL712: _temp711=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp687)->f1; goto _LL710; _LL710: _temp709=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp687)->f2;
goto _LL708; _LL708: _temp707=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp687)->f3; goto _LL692;} else{ goto _LL693;} _LL693: goto _LL694; _LL690:
_temp709= _temp699; goto _LL692; _LL692: if( ! _temp709.q_const){ return;} goto
_LL688; _LL694: goto _LL688; _LL688:;} goto _LL534; _LL552: goto _LL534; _LL534:;}
Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp713=( char*)"attempt to write a read-only location";
struct _tagged_string _temp714; _temp714.curr= _temp713; _temp714.base= _temp713;
_temp714.last_plus_one= _temp713 + 38; _temp714;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp715=( char*)"increment/decrement of non-lvalue"; struct _tagged_string
_temp716; _temp716.curr= _temp715; _temp716.base= _temp715; _temp716.last_plus_one=
_temp715 + 34; _temp716;}));} Cyc_Tcexp_check_writable( te, e);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp718= e->topt; if( _temp718 == 0){ _throw(
Null_Exception);} _temp718->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp717= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp717.last_plus_one
- _temp717.curr, _temp717.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp719=( char*)"conditional expression";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 23; _temp720;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp726=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp726->hd=( void*) e3; _temp726->tl=
0; _temp726;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp725=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp725->hd=(
void*) e2; _temp725->tl= l1; _temp725;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp723= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp721= e2->topt; if( _temp721 == 0){ _throw(
Null_Exception);} _temp721->v;})); struct _tagged_string _temp724= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp722= e3->topt; if( _temp722 == 0){ _throw(
Null_Exception);} _temp722->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp723.last_plus_one - _temp723.curr, _temp723.curr, _temp724.last_plus_one -
_temp724.curr, _temp724.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)({ struct Cyc_Core_Opt*
_temp727= e1->topt; if( _temp727 == 0){ _throw( Null_Exception);}& _temp727->v;})),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp750= e1->topt; if( _temp750
== 0){ _throw( Null_Exception);} _temp750->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp749= e2->topt; if( _temp749 == 0){ _throw( Null_Exception);} _temp749->v;});{
void* _temp728= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp734; struct
Cyc_Absyn_Tqual _temp736; void* _temp738; _LL730: if(( unsigned int) _temp728 >
4u?*(( int*) _temp728) == Cyc_Absyn_ArrayType: 0){ _LL739: _temp738=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp728)->f1; goto _LL737; _LL737: _temp736=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp728)->f2;
goto _LL735; _LL735: _temp734=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp728)->f3; goto _LL731;} else{ goto _LL732;} _LL732: goto _LL733; _LL731:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp740=( char*)"cannot assign to an array";
struct _tagged_string _temp741; _temp741.curr= _temp740; _temp741.base= _temp740;
_temp741.last_plus_one= _temp740 + 26; _temp741;})); goto _LL729; _LL733: goto
_LL729; _LL729:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp742=( char*)"assignment to non-lvalue";
struct _tagged_string _temp743; _temp743.curr= _temp742; _temp743.base= _temp742;
_temp743.last_plus_one= _temp742 + 25; _temp743;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp744= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp745= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp744.last_plus_one - _temp744.curr, _temp744.curr, _temp745.last_plus_one -
_temp745.curr, _temp745.curr);}));}} else{ void* p=( void*)({ struct Cyc_Core_Opt*
_temp748= po; if( _temp748 == 0){ _throw( Null_Exception);} _temp748->v;}); void*
t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify(
t_result, t1)? 1: Cyc_Tcutil_coerceable( t_result))){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp746=( char*)"Cannot use this operator in an assignment";
struct _tagged_string _temp747; _temp747.curr= _temp746; _temp747.base= _temp746;
_temp747.last_plus_one= _temp746 + 42; _temp747;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp751= e2->topt; if( _temp751 == 0){ _throw( Null_Exception);} _temp751->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp809= e->topt; if( _temp809 == 0){ _throw( Null_Exception);} _temp809->v;}));
void* _temp752= t; struct Cyc_Absyn_PtrInfo _temp758; struct Cyc_Absyn_Conref*
_temp760; struct Cyc_Absyn_Tqual _temp762; struct Cyc_Absyn_Conref* _temp764;
void* _temp766; void* _temp768; _LL754: if(( unsigned int) _temp752 > 4u?*(( int*)
_temp752) == Cyc_Absyn_PointerType: 0){ _LL759: _temp758=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp752)->f1; _LL769: _temp768=( void*)
_temp758.elt_typ; goto _LL767; _LL767: _temp766=( void*) _temp758.rgn_typ; goto
_LL765; _LL765: _temp764=( struct Cyc_Absyn_Conref*) _temp758.nullable; goto
_LL763; _LL763: _temp762=( struct Cyc_Absyn_Tqual) _temp758.tq; goto _LL761;
_LL761: _temp760=( struct Cyc_Absyn_Conref*) _temp758.bounds; goto _LL755;}
else{ goto _LL756;} _LL756: goto _LL757; _LL755: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp766); Cyc_Tcutil_check_nonzero_bound( loc, _temp760);{ void*
_temp770= Cyc_Tcutil_compress( _temp768); struct Cyc_Absyn_FnInfo _temp776;
struct Cyc_List_List* _temp778; int _temp780; struct Cyc_List_List* _temp782;
void* _temp784; struct Cyc_Core_Opt* _temp786; struct Cyc_List_List* _temp788;
_LL772: if(( unsigned int) _temp770 > 4u?*(( int*) _temp770) == Cyc_Absyn_FnType:
0){ _LL777: _temp776=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp770)->f1; _LL789: _temp788=( struct Cyc_List_List*) _temp776.tvars; goto
_LL787; _LL787: _temp786=( struct Cyc_Core_Opt*) _temp776.effect; goto _LL785;
_LL785: _temp784=( void*) _temp776.ret_typ; goto _LL783; _LL783: _temp782=(
struct Cyc_List_List*) _temp776.args; goto _LL781; _LL781: _temp780=( int)
_temp776.varargs; goto _LL779; _LL779: _temp778=( struct Cyc_List_List*)
_temp776.attributes; goto _LL773;} else{ goto _LL774;} _LL774: goto _LL775;
_LL773: if( _temp780){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp790=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp791; _temp791.curr= _temp790; _temp791.base= _temp790;
_temp791.last_plus_one= _temp790 + 41; _temp791;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp784,({ void** _temp792= topt; if( _temp792 == 0){ _throw( Null_Exception);}*
_temp792;}));} while( es != 0? _temp782 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp799= es; if( _temp799 == 0){
_throw( Null_Exception);} _temp799->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp798= _temp782; if( _temp798 == 0){ _throw(
Null_Exception);} _temp798->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp794= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp793= e1->topt; if( _temp793 == 0){ _throw( Null_Exception);} _temp793->v;}));
struct _tagged_string _temp795= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp794.last_plus_one - _temp794.curr, _temp794.curr, _temp795.last_plus_one -
_temp795.curr, _temp795.curr);}));} es=({ struct Cyc_List_List* _temp796= es;
if( _temp796 == 0){ _throw( Null_Exception);} _temp796->tl;}); _temp782=({
struct Cyc_List_List* _temp797= _temp782; if( _temp797 == 0){ _throw(
Null_Exception);} _temp797->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp800=( char*)"too many arguments for function";
struct _tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 32; _temp801;}));} if( _temp782 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp802=( char*)"too few arguments for function";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 31; _temp803;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp804= _temp786; if( _temp804 == 0){
_throw( Null_Exception);} _temp804->v;})); return _temp784; _LL775: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp805=( char*)"expected pointer to function";
struct _tagged_string _temp806; _temp806.curr= _temp805; _temp806.base= _temp805;
_temp806.last_plus_one= _temp805 + 29; _temp806;})); _LL771:;} _LL757: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp807=( char*)"expected pointer to function";
struct _tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 29; _temp808;})); _LL753:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp811= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp810= e->topt; if( _temp810 == 0){ _throw( Null_Exception);} _temp810->v;}));
xprintf("expected xtunion exn but found %.*s", _temp811.last_plus_one - _temp811.curr,
_temp811.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp812= e->topt; if( _temp812 == 0){ _throw(
Null_Exception);} _temp812->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp813= e->topt; if( _temp813 == 0){ _throw(
Null_Exception);} _temp813->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp869= e->topt; if( _temp869 == 0){ _throw(
Null_Exception);} _temp869->v;}));{ void* _temp814= t1; struct Cyc_Absyn_PtrInfo
_temp820; struct Cyc_Absyn_Conref* _temp822; struct Cyc_Absyn_Tqual _temp824;
struct Cyc_Absyn_Conref* _temp826; void* _temp828; void* _temp830; _LL816: if((
unsigned int) _temp814 > 4u?*(( int*) _temp814) == Cyc_Absyn_PointerType: 0){
_LL821: _temp820=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp814)->f1; _LL831: _temp830=( void*) _temp820.elt_typ; goto _LL829; _LL829:
_temp828=( void*) _temp820.rgn_typ; goto _LL827; _LL827: _temp826=( struct Cyc_Absyn_Conref*)
_temp820.nullable; goto _LL825; _LL825: _temp824=( struct Cyc_Absyn_Tqual)
_temp820.tq; goto _LL823; _LL823: _temp822=( struct Cyc_Absyn_Conref*) _temp820.bounds;
goto _LL817;} else{ goto _LL818;} _LL818: goto _LL819; _LL817:{ void* _temp832=
Cyc_Tcutil_compress( _temp830); struct Cyc_Absyn_FnInfo _temp838; struct Cyc_List_List*
_temp840; int _temp842; struct Cyc_List_List* _temp844; void* _temp846; struct
Cyc_Core_Opt* _temp848; struct Cyc_List_List* _temp850; _LL834: if((
unsigned int) _temp832 > 4u?*(( int*) _temp832) == Cyc_Absyn_FnType: 0){ _LL839:
_temp838=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp832)->f1;
_LL851: _temp850=( struct Cyc_List_List*) _temp838.tvars; goto _LL849; _LL849:
_temp848=( struct Cyc_Core_Opt*) _temp838.effect; goto _LL847; _LL847: _temp846=(
void*) _temp838.ret_typ; goto _LL845; _LL845: _temp844=( struct Cyc_List_List*)
_temp838.args; goto _LL843; _LL843: _temp842=( int) _temp838.varargs; goto
_LL841; _LL841: _temp840=( struct Cyc_List_List*) _temp838.attributes; goto
_LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp850)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp852=( char*)"type instantiation mismatch";
struct _tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 28; _temp853;}));} for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp854= ts; if( _temp854 == 0){ _throw( Null_Exception);}
_temp854->tl;}), _temp850=({ struct Cyc_List_List* _temp855= _temp850; if(
_temp855 == 0){ _throw( Null_Exception);} _temp855->tl;})){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp861= _temp850; if( _temp861 == 0){ _throw(
Null_Exception);} _temp861->hd;}))->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)({ struct Cyc_List_List* _temp856= ts; if( _temp856 == 0){ _throw(
Null_Exception);} _temp856->hd;})); instantiation=({ struct Cyc_List_List*
_temp857=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp857->hd=( void*)({ struct _tuple4* _temp858=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp858->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp860= _temp850; if( _temp860 == 0){ _throw( Null_Exception);} _temp860->hd;});
_temp858->f2=( void*)({ struct Cyc_List_List* _temp859= ts; if( _temp859 == 0){
_throw( Null_Exception);} _temp859->hd;}); _temp858;}); _temp857->tl=
instantiation; _temp857;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp865=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp865[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp866; _temp866.tag= Cyc_Absyn_FnType; _temp866.f1=({ struct Cyc_Absyn_FnInfo
_temp867; _temp867.tvars= 0; _temp867.effect= _temp848; _temp867.ret_typ=( void*)
_temp846; _temp867.args= _temp844; _temp867.varargs= _temp842; _temp867.attributes=
_temp840; _temp867;}); _temp866;}); _temp865;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp862=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp862[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp863; _temp863.tag= Cyc_Absyn_PointerType;
_temp863.f1=({ struct Cyc_Absyn_PtrInfo _temp864; _temp864.elt_typ=( void*)
new_fn_typ; _temp864.rgn_typ=( void*) _temp828; _temp864.nullable= _temp826;
_temp864.tq= _temp824; _temp864.bounds= _temp822; _temp864;}); _temp863;});
_temp862;}); return new_typ;}} _LL837: goto _LL833; _LL833:;} goto _LL815;
_LL819: goto _LL815; _LL815:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp868= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp868.last_plus_one - _temp868.curr, _temp868.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp872= e->topt; if( _temp872 == 0){ _throw(
Null_Exception);} _temp872->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp870= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp871= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp870.last_plus_one - _temp870.curr, _temp870.curr, _temp871.last_plus_one -
_temp871.curr, _temp871.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp874= Cyc_Tcutil_compress(({ void** _temp873= topt;
if( _temp873 == 0){ _throw( Null_Exception);}* _temp873;})); struct Cyc_Absyn_PtrInfo
_temp880; struct Cyc_Absyn_Conref* _temp882; struct Cyc_Absyn_Tqual _temp884;
struct Cyc_Absyn_Conref* _temp886; void* _temp888; void* _temp890; _LL876: if((
unsigned int) _temp874 > 4u?*(( int*) _temp874) == Cyc_Absyn_PointerType: 0){
_LL881: _temp880=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp874)->f1; _LL891: _temp890=( void*) _temp880.elt_typ; goto _LL889; _LL889:
_temp888=( void*) _temp880.rgn_typ; goto _LL887; _LL887: _temp886=( struct Cyc_Absyn_Conref*)
_temp880.nullable; goto _LL885; _LL885: _temp884=( struct Cyc_Absyn_Tqual)
_temp880.tq; goto _LL883; _LL883: _temp882=( struct Cyc_Absyn_Conref*) _temp880.bounds;
goto _LL877;} else{ goto _LL878;} _LL878: goto _LL879; _LL877: topt2=({ void**
_temp892=( void**) GC_malloc( sizeof( void*)); _temp892[ 0]= _temp890; _temp892;});
tq2= _temp884; goto _LL875; _LL879: goto _LL875; _LL875:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp893=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp901; struct Cyc_List_List* _temp903; struct Cyc_Core_Opt* _temp905; struct
_tuple1* _temp907; struct Cyc_List_List* _temp909; _LL895: if(*(( int*) _temp893)
== Cyc_Absyn_Struct_e){ _LL908: _temp907=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp893)->f1; goto _LL906; _LL906: _temp905=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp893)->f2; goto _LL904; _LL904: _temp903=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp893)->f3; goto
_LL902; _LL902: _temp901=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp893)->f4; goto _LL896;} else{ goto _LL897;} _LL897: if(*(( int*) _temp893)
== Cyc_Absyn_Tuple_e){ _LL910: _temp909=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp893)->f1; goto _LL898;} else{ goto _LL899;} _LL899: goto _LL900; _LL896:
goto _LL898; _LL898: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp911=( char*)"& used to allocate"; struct _tagged_string _temp912; _temp912.curr=
_temp911; _temp912.base= _temp911; _temp912.last_plus_one= _temp911 + 19;
_temp912;}));{ void*(* ptr_maker)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)=
Cyc_Absyn_at_typ; if( topt != 0){ void* _temp914= Cyc_Tcutil_compress(({ void**
_temp913= topt; if( _temp913 == 0){ _throw( Null_Exception);}* _temp913;}));
struct Cyc_Absyn_PtrInfo _temp920; struct Cyc_Absyn_Conref* _temp922; struct Cyc_Absyn_Tqual
_temp924; struct Cyc_Absyn_Conref* _temp926; void* _temp928; void* _temp930;
_LL916: if(( unsigned int) _temp914 > 4u?*(( int*) _temp914) == Cyc_Absyn_PointerType:
0){ _LL921: _temp920=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp914)->f1; _LL931: _temp930=( void*) _temp920.elt_typ; goto _LL929; _LL929:
_temp928=( void*) _temp920.rgn_typ; goto _LL927; _LL927: _temp926=( struct Cyc_Absyn_Conref*)
_temp920.nullable; goto _LL925; _LL925: _temp924=( struct Cyc_Absyn_Tqual)
_temp920.tq; goto _LL923; _LL923: _temp922=( struct Cyc_Absyn_Conref*) _temp920.bounds;
goto _LL917;} else{ goto _LL918;} _LL918: goto _LL919; _LL917: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp926)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL915; _LL919: goto _LL915; _LL915:;} return ptr_maker(( void*)({ struct
Cyc_Core_Opt* _temp932= e->topt; if( _temp932 == 0){ _throw( Null_Exception);}
_temp932->v;}),( void*) Cyc_Absyn_HeapRgn, tq2);} _LL900: goto _LL894; _LL894:;}{
void* _temp935; int _temp937; struct _tuple5 _temp933= Cyc_Tcutil_addressof_props(
te, e); _LL938: _temp937= _temp933.f1; goto _LL936; _LL936: _temp935= _temp933.f2;
goto _LL934; _LL934: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp937){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp939= e->topt; if( _temp939 == 0){ _throw( Null_Exception);} _temp939->v;}),
_temp935, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp959= e->topt; if( _temp959 == 0){ _throw( Null_Exception);} _temp959->v;}));
void* _temp940= t; struct Cyc_Absyn_PtrInfo _temp946; struct Cyc_Absyn_Conref*
_temp948; struct Cyc_Absyn_Tqual _temp950; struct Cyc_Absyn_Conref* _temp952;
void* _temp954; void* _temp956; _LL942: if(( unsigned int) _temp940 > 4u?*(( int*)
_temp940) == Cyc_Absyn_PointerType: 0){ _LL947: _temp946=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp940)->f1; _LL957: _temp956=( void*)
_temp946.elt_typ; goto _LL955; _LL955: _temp954=( void*) _temp946.rgn_typ; goto
_LL953; _LL953: _temp952=( struct Cyc_Absyn_Conref*) _temp946.nullable; goto
_LL951; _LL951: _temp950=( struct Cyc_Absyn_Tqual) _temp946.tq; goto _LL949;
_LL949: _temp948=( struct Cyc_Absyn_Conref*) _temp946.bounds; goto _LL943;}
else{ goto _LL944;} _LL944: goto _LL945; _LL943: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp954); Cyc_Tcutil_check_nonzero_bound( loc, _temp948); return
_temp956; _LL945: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp958= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp958.last_plus_one - _temp958.curr, _temp958.curr);})); _LL941:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1025= e->topt; if( _temp1025 == 0){ _throw(
Null_Exception);} _temp1025->v;})); void* _temp960= t; struct Cyc_Absyn_Structdecl**
_temp972; struct Cyc_Absyn_Structdecl* _temp974; struct Cyc_List_List* _temp975;
struct _tuple1* _temp977; struct Cyc_Absyn_Uniondecl** _temp979; struct Cyc_Absyn_Uniondecl*
_temp981; struct Cyc_List_List* _temp982; struct _tuple1* _temp984; struct Cyc_Absyn_Exp*
_temp987; struct Cyc_Absyn_Tqual _temp989; void* _temp991; struct Cyc_Absyn_PtrInfo
_temp996; _LL962: if(( unsigned int) _temp960 > 4u?*(( int*) _temp960) == Cyc_Absyn_StructType:
0){ _LL978: _temp977=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp960)->f1; goto _LL976; _LL976: _temp975=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp960)->f2; goto _LL973; _LL973: _temp972=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp960)->f3;
if( _temp972 == 0){ goto _LL964;} else{ _temp974=* _temp972; goto _LL963;}}
else{ goto _LL964;} _LL964: if(( unsigned int) _temp960 > 4u?*(( int*) _temp960)
== Cyc_Absyn_UnionType: 0){ _LL985: _temp984=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp960)->f1; goto _LL983; _LL983: _temp982=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp960)->f2; goto _LL980; _LL980: _temp979=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp960)->f3;
if( _temp979 == 0){ goto _LL966;} else{ _temp981=* _temp979; goto _LL965;}}
else{ goto _LL966;} _LL966: if(( unsigned int) _temp960 > 4u?*(( int*) _temp960)
== Cyc_Absyn_ArrayType: 0){ _LL992: _temp991=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp960)->f1; goto _LL990; _LL990: _temp989=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp960)->f2; goto _LL988; _LL988: _temp987=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp960)->f3; goto
_LL986;} else{ goto _LL968;} _LL986: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp993=( char*)"size"; struct _tagged_string _temp994;
_temp994.curr= _temp993; _temp994.base= _temp993; _temp994.last_plus_one=
_temp993 + 5; _temp994;})) == 0){ goto _LL967;} else{ goto _LL968;} _LL968: if((
unsigned int) _temp960 > 4u?*(( int*) _temp960) == Cyc_Absyn_PointerType: 0){
_LL997: _temp996=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp960)->f1; goto _LL995;} else{ goto _LL970;} _LL995: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp998=( char*)"size"; struct
_tagged_string _temp999; _temp999.curr= _temp998; _temp999.base= _temp998;
_temp999.last_plus_one= _temp998 + 5; _temp999;})) == 0){ goto _LL969;} else{
goto _LL970;} _LL970: goto _LL971; _LL963: if( _temp977 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp1000=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1001; _temp1001.curr= _temp1000; _temp1001.base=
_temp1000; _temp1001.last_plus_one= _temp1000 + 40; _temp1001;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp974, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1003= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1002= _temp977; if( _temp1002 == 0){ _throw(
Null_Exception);} _temp1002;})); struct _tagged_string _temp1004=* f; xprintf("struct %.*s has no %.*s field",
_temp1003.last_plus_one - _temp1003.curr, _temp1003.curr, _temp1004.last_plus_one
- _temp1004.curr, _temp1004.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1005= _new_region(); struct _RegionHandle* rgn=& _temp1005; struct
_handler_cons _temp1006; _push_handler(& _temp1006);{ void* _temp1007=( void*)
setjmp( _temp1006.handler); if( ! _temp1007){{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp974->tvs, _temp975); t2= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1008= finfo; if( _temp1008 == 0){ _throw(
Null_Exception);} _temp1008->type;}));} _free_region(& _temp1005); _pop_handler();}
else{ _free_region(& _temp1005); _throw( _temp1007);}}} return t2;}} _LL965: if(
_temp984 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1009=( char*)"unresolved union type (COMPILER ERROR)"; struct
_tagged_string _temp1010; _temp1010.curr= _temp1009; _temp1010.base= _temp1009;
_temp1010.last_plus_one= _temp1009 + 39; _temp1010;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp981, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1012= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1011= _temp984; if( _temp1011 == 0){ _throw(
Null_Exception);} _temp1011;})); struct _tagged_string _temp1013=* f; xprintf("union %.*s has no %.*s field",
_temp1012.last_plus_one - _temp1012.curr, _temp1012.curr, _temp1013.last_plus_one
- _temp1013.curr, _temp1013.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp981->tvs, _temp982); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1014= finfo; if( _temp1014 == 0){ _throw(
Null_Exception);} _temp1014->type;})); return t2;}} _LL967:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1015=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1015[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1016; _temp1016.tag= Cyc_Absyn_Primop_e;
_temp1016.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1016.f2=({ struct Cyc_List_List*
_temp1017=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1017->hd=( void*) e; _temp1017->tl= 0; _temp1017;}); _temp1016;});
_temp1015;}))); return Cyc_Absyn_uint_t; _LL969:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1018=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1018[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1019; _temp1019.tag= Cyc_Absyn_Primop_e;
_temp1019.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1019.f2=({ struct Cyc_List_List*
_temp1020=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1020->hd=( void*) e; _temp1020->tl= 0; _temp1020;}); _temp1019;});
_temp1018;}))); return Cyc_Absyn_uint_t; _LL971: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1021=( char*)"size"; struct _tagged_string
_temp1022; _temp1022.curr= _temp1021; _temp1022.base= _temp1021; _temp1022.last_plus_one=
_temp1021 + 5; _temp1022;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1023= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1023.last_plus_one - _temp1023.curr, _temp1023.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1024= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1024.last_plus_one - _temp1024.curr, _temp1024.curr);}));} _LL961:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1079= e->topt; if( _temp1079 == 0){ _throw( Null_Exception);} _temp1079->v;}));
void* _temp1026= t; struct Cyc_Absyn_PtrInfo _temp1032; struct Cyc_Absyn_Conref*
_temp1034; struct Cyc_Absyn_Tqual _temp1036; struct Cyc_Absyn_Conref* _temp1038;
void* _temp1040; void* _temp1042; _LL1028: if(( unsigned int) _temp1026 > 4u?*((
int*) _temp1026) == Cyc_Absyn_PointerType: 0){ _LL1033: _temp1032=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1026)->f1; _LL1043: _temp1042=( void*)
_temp1032.elt_typ; goto _LL1041; _LL1041: _temp1040=( void*) _temp1032.rgn_typ;
goto _LL1039; _LL1039: _temp1038=( struct Cyc_Absyn_Conref*) _temp1032.nullable;
goto _LL1037; _LL1037: _temp1036=( struct Cyc_Absyn_Tqual) _temp1032.tq; goto
_LL1035; _LL1035: _temp1034=( struct Cyc_Absyn_Conref*) _temp1032.bounds; goto
_LL1029;} else{ goto _LL1030;} _LL1030: goto _LL1031; _LL1029: Cyc_Tcutil_check_nonzero_bound(
loc, _temp1034);{ void* _temp1044= Cyc_Tcutil_compress( _temp1042); struct Cyc_Absyn_Structdecl**
_temp1052; struct Cyc_Absyn_Structdecl* _temp1054; struct Cyc_List_List*
_temp1055; struct _tuple1* _temp1057; struct Cyc_Absyn_Uniondecl** _temp1059;
struct Cyc_Absyn_Uniondecl* _temp1061; struct Cyc_List_List* _temp1062; struct
_tuple1* _temp1064; _LL1046: if(( unsigned int) _temp1044 > 4u?*(( int*)
_temp1044) == Cyc_Absyn_StructType: 0){ _LL1058: _temp1057=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp1044)->f1; goto _LL1056; _LL1056:
_temp1055=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1044)->f2; goto _LL1053; _LL1053: _temp1052=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1044)->f3; if( _temp1052 == 0){ goto
_LL1048;} else{ _temp1054=* _temp1052; goto _LL1047;}} else{ goto _LL1048;}
_LL1048: if(( unsigned int) _temp1044 > 4u?*(( int*) _temp1044) == Cyc_Absyn_UnionType:
0){ _LL1065: _temp1064=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1044)->f1; goto _LL1063; _LL1063: _temp1062=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnionType_struct*) _temp1044)->f2; goto _LL1060; _LL1060:
_temp1059=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp1044)->f3; if( _temp1059 == 0){ goto _LL1050;} else{ _temp1061=* _temp1059;
goto _LL1049;}} else{ goto _LL1050;} _LL1050: goto _LL1051; _LL1047: { struct
Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field( _temp1054, f); if(
finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1067= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp1066= _temp1057; if( _temp1066 == 0){ _throw( Null_Exception);} _temp1066;}));
struct _tagged_string _temp1068=* f; xprintf("struct %.*s has no %.*s field",
_temp1067.last_plus_one - _temp1067.curr, _temp1067.curr, _temp1068.last_plus_one
- _temp1068.curr, _temp1068.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1069= _new_region(); struct _RegionHandle* rgn=& _temp1069; struct
_handler_cons _temp1070; _push_handler(& _temp1070);{ void* _temp1071=( void*)
setjmp( _temp1070.handler); if( ! _temp1071){{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle* r3, struct _RegionHandle* r4,
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn,
_temp1054->tvs, _temp1055); t3= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1072= finfo; if( _temp1072 == 0){ _throw(
Null_Exception);} _temp1072->type;}));} _free_region(& _temp1069); _pop_handler();}
else{ _free_region(& _temp1069); _throw( _temp1071);}}} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1040); return t3;}} _LL1049: { struct Cyc_Absyn_Structfield* finfo=
Cyc_Absyn_lookup_union_field( _temp1061, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1074= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1073= _temp1064; if( _temp1073 == 0){ _throw(
Null_Exception);} _temp1073;})); struct _tagged_string _temp1075=* f; xprintf("union %.*s has no %.*s field",
_temp1074.last_plus_one - _temp1074.curr, _temp1074.curr, _temp1075.last_plus_one
- _temp1075.curr, _temp1075.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1061->tvs, _temp1062); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1076= finfo; if( _temp1076 == 0){ _throw(
Null_Exception);} _temp1076->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1040); return t3;}} _LL1051: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1077= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1077.last_plus_one - _temp1077.curr, _temp1077.curr);})); _LL1045:;}
_LL1031: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1078=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1078.last_plus_one - _temp1078.curr, _temp1078.curr);})); _LL1027:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1080; _push_handler(& _temp1080);{ void*
_temp1081=( void*) setjmp( _temp1080.handler); if( ! _temp1081){{ void*
_temp1082=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1082;}; _pop_handler();} else{
void* _temp1084= _temp1081; _LL1086: if( _temp1084 == Cyc_List_Nth){ goto
_LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089; _LL1087: return Cyc_Tcexp_expr_err(
te, loc, xprintf("index is %d but tuple has only %d fields",( int) i,(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1089:( void) _throw(
_temp1084); _LL1085:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1115=
e1->topt; if( _temp1115 == 0){ _throw( Null_Exception);} _temp1115->v;})); void*
t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1114= e2->topt; if(
_temp1114 == 0){ _throw( Null_Exception);} _temp1114->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1090= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1090.last_plus_one - _temp1090.curr, _temp1090.curr);}));}{ void* _temp1091=
t1; struct Cyc_Absyn_PtrInfo _temp1099; struct Cyc_Absyn_Conref* _temp1101;
struct Cyc_Absyn_Tqual _temp1103; struct Cyc_Absyn_Conref* _temp1105; void*
_temp1107; void* _temp1109; struct Cyc_List_List* _temp1111; _LL1093: if((
unsigned int) _temp1091 > 4u?*(( int*) _temp1091) == Cyc_Absyn_PointerType: 0){
_LL1100: _temp1099=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1091)->f1; _LL1110: _temp1109=( void*) _temp1099.elt_typ; goto _LL1108;
_LL1108: _temp1107=( void*) _temp1099.rgn_typ; goto _LL1106; _LL1106: _temp1105=(
struct Cyc_Absyn_Conref*) _temp1099.nullable; goto _LL1104; _LL1104: _temp1103=(
struct Cyc_Absyn_Tqual) _temp1099.tq; goto _LL1102; _LL1102: _temp1101=( struct
Cyc_Absyn_Conref*) _temp1099.bounds; goto _LL1094;} else{ goto _LL1095;} _LL1095:
if(( unsigned int) _temp1091 > 4u?*(( int*) _temp1091) == Cyc_Absyn_TupleType: 0){
_LL1112: _temp1111=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1091)->f1; goto _LL1096;} else{ goto _LL1097;} _LL1097: goto _LL1098;
_LL1094: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1101);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1101);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1107); return _temp1109; _LL1096: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1111, e2); _LL1098: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1113= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1113.last_plus_one - _temp1113.curr, _temp1113.curr);})); _LL1092:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1117= Cyc_Tcutil_compress(({ void**
_temp1116= topt; if( _temp1116 == 0){ _throw( Null_Exception);}* _temp1116;}));
struct Cyc_List_List* _temp1123; _LL1119: if(( unsigned int) _temp1117 > 4u?*((
int*) _temp1117) == Cyc_Absyn_TupleType: 0){ _LL1124: _temp1123=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1117)->f1; goto _LL1120;} else{ goto
_LL1121;} _LL1121: goto _LL1122; _LL1120: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1123) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1125=( char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1126; _temp1126.curr= _temp1125; _temp1126.base=
_temp1125; _temp1126.last_plus_one= _temp1125 + 48; _temp1126;})); goto _LL1118;}
for( 0; es != 0; es=({ struct Cyc_List_List* _temp1127= es; if( _temp1127 == 0){
_throw( Null_Exception);} _temp1127->tl;}), _temp1123=({ struct Cyc_List_List*
_temp1128= _temp1123; if( _temp1128 == 0){ _throw( Null_Exception);} _temp1128->tl;})){
void* topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1135= _temp1123;
if( _temp1135 == 0){ _throw( Null_Exception);} _temp1135->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1129=
es; if( _temp1129 == 0){ _throw( Null_Exception);} _temp1129->hd;})); fields=({
struct Cyc_List_List* _temp1130=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1130->hd=( void*)({ struct _tuple7* _temp1131=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1131->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp1134= _temp1123; if( _temp1134 == 0){
_throw( Null_Exception);} _temp1134->hd;}))).f1; _temp1131->f2=( void*)({ struct
Cyc_Core_Opt* _temp1133=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1132= es; if( _temp1132 == 0){ _throw( Null_Exception);} _temp1132->hd;}))->topt;
if( _temp1133 == 0){ _throw( Null_Exception);} _temp1133->v;}); _temp1131;});
_temp1130->tl= fields; _temp1130;});} done= 1; goto _LL1118; _LL1122: goto
_LL1118; _LL1118:;} if( ! done){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp1136= es; if( _temp1136 == 0){ _throw( Null_Exception);} _temp1136->tl;})){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1137= es; if( _temp1137 == 0){ _throw( Null_Exception);} _temp1137->hd;}));
fields=({ struct Cyc_List_List* _temp1138=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1138->hd=( void*)({ struct _tuple7*
_temp1139=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1139->f1=
Cyc_Absyn_empty_tqual(); _temp1139->f2=( void*)({ struct Cyc_Core_Opt* _temp1141=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1140= es; if( _temp1140 == 0){
_throw( Null_Exception);} _temp1140->hd;}))->topt; if( _temp1141 == 0){ _throw(
Null_Exception);} _temp1141->v;}); _temp1139;}); _temp1138->tl= fields;
_temp1138;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp1142=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1142[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1143; _temp1143.tag= Cyc_Absyn_TupleType;
_temp1143.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1143;}); _temp1142;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1144=( char*)"tcCompoundLit"; struct
_tagged_string _temp1145; _temp1145.curr= _temp1144; _temp1145.base= _temp1144;
_temp1145.last_plus_one= _temp1144 + 14; _temp1145;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp1168=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1168[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1169; _temp1169.tag= Cyc_Absyn_Const_e; _temp1169.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1170=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1170[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1171; _temp1171.tag= Cyc_Absyn_Int_c; _temp1171.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1171.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1171;}); _temp1170;})); _temp1169;}); _temp1168;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1146=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1146->v=( void*) Cyc_Absyn_uint_t; _temp1146;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1166=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1166[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1167; _temp1167.tag= Cyc_Absyn_ArrayType;
_temp1167.f1=( void*) res; _temp1167.f2= Cyc_Absyn_empty_tqual(); _temp1167.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1167;}); _temp1166;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp1147= es2; if(
_temp1147 == 0){ _throw( Null_Exception);} _temp1147->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1148= es2;
if( _temp1148 == 0){ _throw( Null_Exception);} _temp1148->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1149= es; if( _temp1149 == 0){ _throw( Null_Exception);} _temp1149->hd;}))->loc,({
struct _tagged_string _temp1150= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1150.last_plus_one - _temp1150.curr, _temp1150.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp1151= des; if(
_temp1151 == 0){ _throw( Null_Exception);} _temp1151->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1165= des; if( _temp1165
== 0){ _throw( Null_Exception);} _temp1165->hd;}))).f1; if( ds != 0){ void*
_temp1153=( void*)({ struct Cyc_List_List* _temp1152= ds; if( _temp1152 == 0){
_throw( Null_Exception);} _temp1152->hd;}); struct _tagged_string* _temp1159;
struct Cyc_Absyn_Exp* _temp1161; _LL1155: if(*(( int*) _temp1153) == Cyc_Absyn_FieldName){
_LL1160: _temp1159=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1153)->f1; goto _LL1156;} else{ goto _LL1157;} _LL1157: if(*(( int*)
_temp1153) == Cyc_Absyn_ArrayElement){ _LL1162: _temp1161=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1153)->f1; goto _LL1158;} else{ goto
_LL1154;} _LL1156: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1163=( char*)"only array index designators are supported"; struct
_tagged_string _temp1164; _temp1164.curr= _temp1163; _temp1164.base= _temp1163;
_temp1164.last_plus_one= _temp1163 + 43; _temp1164;})); goto _LL1154; _LL1158:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1161);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1161); if( i != offset){ Cyc_Tcutil_terr( _temp1161->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1154;} _LL1154:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1173= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1172= bound->topt; if( _temp1172 == 0){
_throw( Null_Exception);} _temp1172->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1173.last_plus_one - _temp1173.curr, _temp1173.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1174=( char*)"comprehension index variable is not declared const!";
struct _tagged_string _temp1175; _temp1175.curr= _temp1174; _temp1175.base=
_temp1174; _temp1175.last_plus_one= _temp1174 + 52; _temp1175;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_string)({ char* _temp1176=( char*)"bound is not constant"; struct
_tagged_string _temp1177; _temp1177.curr= _temp1176; _temp1177.base= _temp1176;
_temp1177.last_plus_one= _temp1176 + 22; _temp1177;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ char*
_temp1178=( char*)"body is not constant"; struct _tagged_string _temp1179;
_temp1179.curr= _temp1178; _temp1179.base= _temp1178; _temp1179.last_plus_one=
_temp1178 + 21; _temp1179;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2=
0; struct Cyc_Absyn_Tqual* tqopt= 0; if( topt != 0){ void* _temp1181= Cyc_Tcutil_compress(({
void** _temp1180= topt; if( _temp1180 == 0){ _throw( Null_Exception);}*
_temp1180;})); struct Cyc_Absyn_PtrInfo _temp1189; struct Cyc_Absyn_Exp*
_temp1191; struct Cyc_Absyn_Tqual _temp1193; struct Cyc_Absyn_Tqual* _temp1195;
void* _temp1196; void** _temp1198; _LL1183: if(( unsigned int) _temp1181 > 4u?*((
int*) _temp1181) == Cyc_Absyn_PointerType: 0){ _LL1190: _temp1189=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1181)->f1; goto _LL1184;} else{ goto
_LL1185;} _LL1185: if(( unsigned int) _temp1181 > 4u?*(( int*) _temp1181) == Cyc_Absyn_ArrayType:
0){ _LL1197: _temp1196=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1181)->f1;
_temp1198=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1181)->f1; goto _LL1194;
_LL1194: _temp1193=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1181)->f2; _temp1195=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1181)->f2;
goto _LL1192; _LL1192: _temp1191=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1181)->f3; goto _LL1186;} else{ goto _LL1187;} _LL1187: goto _LL1188;
_LL1184: pinfo= _temp1189; topt2=( void**)(( void*)& pinfo.elt_typ); tqopt=(
struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1182; _LL1186: topt2=( void**)
_temp1198; tqopt=( struct Cyc_Absyn_Tqual*) _temp1195; goto _LL1182; _LL1188:
goto _LL1182; _LL1182:;}{ void* t= Cyc_Tcexp_tcExp( te, topt2, body); return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1199=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1199[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1200; _temp1200.tag= Cyc_Absyn_ArrayType;
_temp1200.f1=( void*) t; _temp1200.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual* _temp1201= tqopt; if( _temp1201 == 0){ _throw(
Null_Exception);}* _temp1201;}); _temp1200.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1200;}); _temp1199;});}}} struct _tuple8{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1202=* sd_opt; if( _temp1202 == 0){ _throw(
Null_Exception);} _temp1202;});} else{{ struct _handler_cons _temp1203;
_push_handler(& _temp1203);{ void* _temp1204=( void*) setjmp( _temp1203.handler);
if( ! _temp1204){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1206= _temp1204; _LL1208: if( _temp1206 == Cyc_Dict_Absent){
goto _LL1209;} else{ goto _LL1210;} _LL1210: goto _LL1211; _LL1209: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1212= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1212.last_plus_one - _temp1212.curr, _temp1212.curr);})); return topt != 0?({
void** _temp1213= topt; if( _temp1213 == 0){ _throw( Null_Exception);}*
_temp1213;}):( void*) Cyc_Absyn_VoidType; _LL1211:( void) _throw( _temp1206);
_LL1207:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1214= sd->name; if( _temp1214 == 0){
_throw( Null_Exception);} _temp1214->v;});}}{ struct _RegionHandle _temp1215=
_new_region(); struct _RegionHandle* rgn=& _temp1215; struct _handler_cons
_temp1216; _push_handler(& _temp1216);{ void* _temp1217=( void*) setjmp(
_temp1216.handler); if( ! _temp1217){{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct* res_typ=({
struct Cyc_Absyn_StructType_struct* _temp1235=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1235[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1236; _temp1236.tag= Cyc_Absyn_StructType;
_temp1236.f1=( struct _tuple1*)* tn; _temp1236.f2= typs; _temp1236.f3=({ struct
Cyc_Absyn_Structdecl** _temp1237=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1237[ 0]= sd; _temp1237;});
_temp1236;}); _temp1235;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1218= topt; if( _temp1218 == 0){ _throw( Null_Exception);}*
_temp1218;}));}* otyps=({ struct Cyc_Core_Opt* _temp1219=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1219->v=( void*) typs; _temp1219;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle*
rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl*
sd)) Cyc_Tcutil_resolve_struct_designators)( rgn, loc, args, sd); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp1220= fields; if( _temp1220 == 0){
_throw( Null_Exception);} _temp1220->tl;})){ struct _tuple8 _temp1224; struct
Cyc_Absyn_Exp* _temp1225; struct Cyc_Absyn_Structfield* _temp1227; struct
_tuple8* _temp1222=( struct _tuple8*)({ struct Cyc_List_List* _temp1221= fields;
if( _temp1221 == 0){ _throw( Null_Exception);} _temp1221->hd;}); _temp1224=*
_temp1222; _LL1228: _temp1227= _temp1224.f1; goto _LL1226; _LL1226: _temp1225=
_temp1224.f2; goto _LL1223; _LL1223: { void* inst_fieldtyp= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*) _temp1227->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1225); if( ! Cyc_Tcutil_coerce_arg( te, _temp1225,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1225->loc,({ struct _tagged_string
_temp1230=* _temp1227->name; struct _tagged_string _temp1231= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1232= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1233= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1229= _temp1225->topt; if( _temp1229 == 0){ _throw( Null_Exception);}
_temp1229->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1230.last_plus_one - _temp1230.curr, _temp1230.curr, _temp1231.last_plus_one
- _temp1231.curr, _temp1231.curr, _temp1232.last_plus_one - _temp1232.curr,
_temp1232.curr, _temp1233.last_plus_one - _temp1233.curr, _temp1233.curr);}));}}}{
void* _temp1234=( void*) res_typ; _npop_handler( 0u); return _temp1234;}}}
_free_region(& _temp1215); _pop_handler();} else{ _free_region(& _temp1215);
_throw( _temp1217);}}}} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield* tuf){ struct
_RegionHandle _temp1238= _new_region(); struct _RegionHandle* rgn=& _temp1238;
struct _handler_cons _temp1239; _push_handler(& _temp1239);{ void* _temp1240=(
void*) setjmp( _temp1239.handler); if( ! _temp1240){{ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)(
struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env,
struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn,
tud->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tuf->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst);*
all_ref=({ struct Cyc_Core_Opt* _temp1241=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1241->v=( void*) all_typs; _temp1241;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1242=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1242->v=( void*) exist_typs; _temp1242;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1290=( struct
Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1290[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1291; _temp1291.tag=
Cyc_Absyn_TunionFieldType; _temp1291.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1292; _temp1292.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1293=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1293[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1294; _temp1294.tag= Cyc_Absyn_KnownTunionfield; _temp1294.f1= tud;
_temp1294.f2= tuf; _temp1294;}); _temp1293;})); _temp1292.targs= all_typs;
_temp1292;}); _temp1291;}); _temp1290;}); if( topt != 0){ void* _temp1244= Cyc_Tcutil_compress(({
void** _temp1243= topt; if( _temp1243 == 0){ _throw( Null_Exception);}*
_temp1243;})); struct Cyc_Absyn_TunionFieldInfo _temp1252; struct Cyc_Absyn_TunionInfo
_temp1254; void* _temp1256; struct Cyc_List_List* _temp1258; void* _temp1260;
_LL1246: if(( unsigned int) _temp1244 > 4u?*(( int*) _temp1244) == Cyc_Absyn_TunionFieldType:
0){ _LL1253: _temp1252=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1244)->f1; goto _LL1247;} else{ goto _LL1248;} _LL1248: if(( unsigned int)
_temp1244 > 4u?*(( int*) _temp1244) == Cyc_Absyn_TunionType: 0){ _LL1255:
_temp1254=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1244)->f1; _LL1261: _temp1260=( void*) _temp1254.tunion_info; goto _LL1259;
_LL1259: _temp1258=( struct Cyc_List_List*) _temp1254.targs; goto _LL1257;
_LL1257: _temp1256=( void*) _temp1254.rgn; goto _LL1249;} else{ goto _LL1250;}
_LL1250: goto _LL1251; _LL1247: Cyc_Tcutil_unify(({ void** _temp1262= topt; if(
_temp1262 == 0){ _throw( Null_Exception);}* _temp1262;}), res); goto _LL1245;
_LL1249:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0? _temp1258 != 0: 0;
a=({ struct Cyc_List_List* _temp1263= a; if( _temp1263 == 0){ _throw(
Null_Exception);} _temp1263->tl;}), _temp1258=({ struct Cyc_List_List* _temp1264=
_temp1258; if( _temp1264 == 0){ _throw( Null_Exception);} _temp1264->tl;})){ Cyc_Tcutil_unify((
void*)({ struct Cyc_List_List* _temp1265= a; if( _temp1265 == 0){ _throw(
Null_Exception);} _temp1265->hd;}),( void*)({ struct Cyc_List_List* _temp1266=
_temp1258; if( _temp1266 == 0){ _throw( Null_Exception);} _temp1266->hd;}));}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1267=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1267->v=(
void*) res; _temp1267;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1268=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1268[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1269; _temp1269.tag=
Cyc_Absyn_TunionType; _temp1269.f1=({ struct Cyc_Absyn_TunionInfo _temp1270;
_temp1270.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1271=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1271[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1272; _temp1272.tag=
Cyc_Absyn_KnownTunion; _temp1272.f1= tud; _temp1272;}); _temp1271;})); _temp1270.targs=
all_typs; _temp1270.rgn=( void*) _temp1256; _temp1270;}); _temp1269;});
_temp1268;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1273= res;
_npop_handler( 0u); return _temp1273;}} goto _LL1245; _LL1251: goto _LL1245;
_LL1245:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0; es=({
struct Cyc_List_List* _temp1274= es; if( _temp1274 == 0){ _throw( Null_Exception);}
_temp1274->tl;}), ts=({ struct Cyc_List_List* _temp1275= ts; if( _temp1275 == 0){
_throw( Null_Exception);} _temp1275->tl;})){ struct Cyc_Absyn_Exp* e=( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1283= es; if( _temp1283 == 0){
_throw( Null_Exception);} _temp1283->hd;}); void* t= Cyc_Tcutil_rsubstitute( rgn,
inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1282= ts; if( _temp1282
== 0){ _throw( Null_Exception);} _temp1282->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp1279= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1280= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1281= e->topt == 0?( struct _tagged_string)({ char* _temp1276=( char*)"?";
struct _tagged_string _temp1277; _temp1277.curr= _temp1276; _temp1277.base=
_temp1276; _temp1277.last_plus_one= _temp1276 + 2; _temp1277;}): Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1278= e->topt; if( _temp1278 == 0){ _throw(
Null_Exception);} _temp1278->v;})); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1279.last_plus_one - _temp1279.curr, _temp1279.curr, _temp1280.last_plus_one
- _temp1280.curr, _temp1280.curr, _temp1281.last_plus_one - _temp1281.curr,
_temp1281.curr);}));}} if( es != 0){ void* _temp1286= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1284= es; if( _temp1284 == 0){
_throw( Null_Exception);} _temp1284->hd;}))->loc,({ struct _tagged_string
_temp1285= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1285.last_plus_one - _temp1285.curr, _temp1285.curr);})); _npop_handler( 0u);
return _temp1286;} if( ts != 0){ void* _temp1288= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1287= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1287.last_plus_one - _temp1287.curr, _temp1287.curr);})); _npop_handler( 0u);
return _temp1288;}{ void* _temp1289= res; _npop_handler( 0u); return _temp1289;}}}}
_free_region(& _temp1238); _pop_handler();} else{ _free_region(& _temp1238);
_throw( _temp1240);}}} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1306=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1306[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1307; _temp1307.tag=
Cyc_Absyn_RgnHandleType; _temp1307.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1307;}); _temp1306;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1305= ropt;
if( _temp1305 == 0){ _throw( Null_Exception);} _temp1305;})); void* _temp1295=
Cyc_Tcutil_compress( handle_type); void* _temp1301; _LL1297: if(( unsigned int)
_temp1295 > 4u?*(( int*) _temp1295) == Cyc_Absyn_RgnHandleType: 0){ _LL1302:
_temp1301=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1295)->f1;
goto _LL1298;} else{ goto _LL1299;} _LL1299: goto _LL1300; _LL1298: rgn=
_temp1301; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto _LL1296; _LL1300:
Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1303= ropt; if( _temp1303 == 0){
_throw( Null_Exception);} _temp1303->loc;}),({ struct _tagged_string _temp1304=
Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1304.last_plus_one - _temp1304.curr, _temp1304.curr);})); goto _LL1296;
_LL1296:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1308= t; struct Cyc_Absyn_TunionFieldInfo
_temp1314; struct Cyc_List_List* _temp1316; void* _temp1318; struct Cyc_Absyn_Tunionfield*
_temp1320; struct Cyc_Absyn_Tuniondecl* _temp1322; _LL1310: if(( unsigned int)
_temp1308 > 4u?*(( int*) _temp1308) == Cyc_Absyn_TunionFieldType: 0){ _LL1315:
_temp1314=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1308)->f1; _LL1319: _temp1318=( void*) _temp1314.field_info; if(*(( int*)
_temp1318) == Cyc_Absyn_KnownTunionfield){ _LL1323: _temp1322=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1318)->f1; goto _LL1321; _LL1321:
_temp1320=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1318)->f2; goto _LL1317;} else{ goto _LL1312;} _LL1317: _temp1316=( struct
Cyc_List_List*) _temp1314.targs; goto _LL1311;} else{ goto _LL1312;} _LL1312:
goto _LL1313; _LL1311: if( _temp1320->tvs != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp1324=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1325; _temp1325.curr= _temp1324; _temp1325.base=
_temp1324; _temp1325.last_plus_one= _temp1324 + 50; _temp1325;}));} goto _LL1309;
_LL1313: goto _LL1309; _LL1309:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1327=
Cyc_Tcutil_compress(({ void** _temp1326= topt; if( _temp1326 == 0){ _throw(
Null_Exception);}* _temp1326;})); struct Cyc_Absyn_PtrInfo _temp1333; struct Cyc_Absyn_Conref*
_temp1335; struct Cyc_Absyn_Tqual _temp1337; struct Cyc_Absyn_Conref* _temp1339;
void* _temp1341; void* _temp1343; _LL1329: if(( unsigned int) _temp1327 > 4u?*((
int*) _temp1327) == Cyc_Absyn_PointerType: 0){ _LL1334: _temp1333=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1327)->f1; _LL1344: _temp1343=( void*)
_temp1333.elt_typ; goto _LL1342; _LL1342: _temp1341=( void*) _temp1333.rgn_typ;
goto _LL1340; _LL1340: _temp1339=( struct Cyc_Absyn_Conref*) _temp1333.nullable;
goto _LL1338; _LL1338: _temp1337=( struct Cyc_Absyn_Tqual) _temp1333.tq; goto
_LL1336; _LL1336: _temp1335=( struct Cyc_Absyn_Conref*) _temp1333.bounds; goto
_LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332; _LL1330: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1339)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL1328; _LL1332: goto _LL1328; _LL1328:;} return ptr_maker( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1345=( void*) s->r; struct Cyc_Absyn_Exp* _temp1355;
struct Cyc_Absyn_Stmt* _temp1357; struct Cyc_Absyn_Stmt* _temp1359; struct Cyc_Absyn_Stmt*
_temp1361; struct Cyc_Absyn_Decl* _temp1363; _LL1347: if(( unsigned int)
_temp1345 > 1u?*(( int*) _temp1345) == Cyc_Absyn_Exp_s: 0){ _LL1356: _temp1355=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1345)->f1; goto
_LL1348;} else{ goto _LL1349;} _LL1349: if(( unsigned int) _temp1345 > 1u?*((
int*) _temp1345) == Cyc_Absyn_Seq_s: 0){ _LL1360: _temp1359=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1345)->f1; goto _LL1358; _LL1358: _temp1357=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1345)->f2; goto
_LL1350;} else{ goto _LL1351;} _LL1351: if(( unsigned int) _temp1345 > 1u?*((
int*) _temp1345) == Cyc_Absyn_Decl_s: 0){ _LL1364: _temp1363=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1345)->f1; goto _LL1362; _LL1362:
_temp1361=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1345)->f2;
goto _LL1352;} else{ goto _LL1353;} _LL1353: goto _LL1354; _LL1348: return( void*)({
struct Cyc_Core_Opt* _temp1365= _temp1355->topt; if( _temp1365 == 0){ _throw(
Null_Exception);} _temp1365->v;}); _LL1350: s= _temp1357; continue; _LL1352: s=
_temp1361; continue; _LL1354: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1366=( char*)"statement expression must end with expression";
struct _tagged_string _temp1367; _temp1367.curr= _temp1366; _temp1367.base=
_temp1366; _temp1367.last_plus_one= _temp1366 + 46; _temp1367;})); _LL1346:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1368=( char*)"tcCodegen"; struct
_tagged_string _temp1369; _temp1369.curr= _temp1368; _temp1369.base= _temp1368;
_temp1369.last_plus_one= _temp1368 + 10; _temp1369;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1370=( char*)"tcFill"; struct _tagged_string _temp1371; _temp1371.curr=
_temp1370; _temp1371.base= _temp1370; _temp1371.last_plus_one= _temp1370 + 7;
_temp1371;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1383=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1383[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1384; _temp1384.tag= Cyc_Absyn_RgnHandleType; _temp1384.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1384;}); _temp1383;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1382= rgn_handle; if( _temp1382 == 0){ _throw( Null_Exception);} _temp1382;}));
void* _temp1372= Cyc_Tcutil_compress( handle_type); void* _temp1378; _LL1374:
if(( unsigned int) _temp1372 > 4u?*(( int*) _temp1372) == Cyc_Absyn_RgnHandleType:
0){ _LL1379: _temp1378=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1372)->f1; goto _LL1375;} else{ goto _LL1376;} _LL1376: goto _LL1377;
_LL1375: rgn= _temp1378; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1373; _LL1377: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1380= rgn_handle;
if( _temp1380 == 0){ _throw( Null_Exception);} _temp1380->loc;}),({ struct
_tagged_string _temp1381= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1381.last_plus_one - _temp1381.curr, _temp1381.curr);})); goto _LL1373;
_LL1373:;}{ void* _temp1385=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1397;
struct Cyc_Absyn_Exp* _temp1399; struct Cyc_Absyn_Vardecl* _temp1401; struct Cyc_List_List*
_temp1403; struct Cyc_Core_Opt* _temp1405; struct Cyc_List_List* _temp1407; void*
_temp1409; struct _tagged_string _temp1411; _LL1387: if(*(( int*) _temp1385) ==
Cyc_Absyn_Comprehension_e){ _LL1402: _temp1401=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1385)->f1; goto _LL1400; _LL1400:
_temp1399=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1385)->f2; goto _LL1398; _LL1398: _temp1397=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1385)->f3; goto _LL1388;} else{
goto _LL1389;} _LL1389: if(*(( int*) _temp1385) == Cyc_Absyn_UnresolvedMem_e){
_LL1406: _temp1405=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1385)->f1; goto _LL1404; _LL1404: _temp1403=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1385)->f2; goto _LL1390;} else{
goto _LL1391;} _LL1391: if(*(( int*) _temp1385) == Cyc_Absyn_Array_e){ _LL1408:
_temp1407=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1385)->f1;
goto _LL1392;} else{ goto _LL1393;} _LL1393: if(*(( int*) _temp1385) == Cyc_Absyn_Const_e){
_LL1410: _temp1409=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1385)->f1;
if(( unsigned int) _temp1409 > 1u?*(( int*) _temp1409) == Cyc_Absyn_String_c: 0){
_LL1412: _temp1411=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1409)->f1; goto _LL1394;} else{ goto _LL1395;}} else{ goto _LL1395;}
_LL1395: goto _LL1396; _LL1388: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1413= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1419; struct Cyc_Absyn_Tqual _temp1421; void* _temp1423; _LL1415: if((
unsigned int) _temp1413 > 4u?*(( int*) _temp1413) == Cyc_Absyn_ArrayType: 0){
_LL1424: _temp1423=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1413)->f1;
goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1413)->f2; goto _LL1420; _LL1420: _temp1419=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1413)->f3; goto _LL1416;} else{ goto
_LL1417;} _LL1417: goto _LL1418; _LL1416: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1399)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1433=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1433[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1434; _temp1434.tag= Cyc_Absyn_Upper_b; _temp1434.f1=
_temp1399; _temp1434;}); _temp1433;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1430=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1430[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1431; _temp1431.tag= Cyc_Absyn_PointerType;
_temp1431.f1=({ struct Cyc_Absyn_PtrInfo _temp1432; _temp1432.elt_typ=( void*)
_temp1423; _temp1432.rgn_typ=( void*) rgn; _temp1432.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1432.tq= _temp1421; _temp1432.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1432;});
_temp1431;}); _temp1430;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1425= topt; if( _temp1425 == 0){ _throw( Null_Exception);}* _temp1425;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1426= topt;
if( _temp1426 == 0){ _throw( Null_Exception);}* _temp1426;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1427=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1427->v=( void*) res_typ; _temp1427;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1428= topt; if( _temp1428 == 0){ _throw( Null_Exception);}*
_temp1428;})); res_typ=({ void** _temp1429= topt; if( _temp1429 == 0){ _throw(
Null_Exception);}* _temp1429;});}} return res_typ;} _LL1418: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1435=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1436; _temp1436.curr= _temp1435; _temp1436.base= _temp1435;
_temp1436.last_plus_one= _temp1435 + 45; _temp1436;})); _LL1414:;} _LL1390:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1437=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1437[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1438; _temp1438.tag= Cyc_Absyn_Array_e;
_temp1438.f1= _temp1403; _temp1438;}); _temp1437;}))); _temp1407= _temp1403;
goto _LL1392; _LL1392: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1440=
Cyc_Tcutil_compress(({ void** _temp1439= topt; if( _temp1439 == 0){ _throw(
Null_Exception);}* _temp1439;})); struct Cyc_Absyn_PtrInfo _temp1446; struct Cyc_Absyn_Conref*
_temp1448; struct Cyc_Absyn_Tqual _temp1450; struct Cyc_Absyn_Conref* _temp1452;
void* _temp1454; void* _temp1456; void** _temp1458; _LL1442: if(( unsigned int)
_temp1440 > 4u?*(( int*) _temp1440) == Cyc_Absyn_PointerType: 0){ _LL1447:
_temp1446=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1440)->f1; _LL1457: _temp1456=( void*) _temp1446.elt_typ; _temp1458=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1440)->f1).elt_typ; goto _LL1455;
_LL1455: _temp1454=( void*) _temp1446.rgn_typ; goto _LL1453; _LL1453: _temp1452=(
struct Cyc_Absyn_Conref*) _temp1446.nullable; goto _LL1451; _LL1451: _temp1450=(
struct Cyc_Absyn_Tqual) _temp1446.tq; goto _LL1449; _LL1449: _temp1448=( struct
Cyc_Absyn_Conref*) _temp1446.bounds; goto _LL1443;} else{ goto _LL1444;} _LL1444:
goto _LL1445; _LL1443: elt_typ_opt=( void**) _temp1458; goto _LL1441; _LL1445:
goto _LL1441; _LL1441:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1459= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1465; struct Cyc_Absyn_Tqual _temp1467; void*
_temp1469; _LL1461: if(( unsigned int) _temp1459 > 4u?*(( int*) _temp1459) ==
Cyc_Absyn_ArrayType: 0){ _LL1470: _temp1469=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1459)->f1; goto _LL1468; _LL1468: _temp1467=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1459)->f2; goto _LL1466; _LL1466:
_temp1465=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1459)->f3; goto _LL1462;} else{ goto _LL1463;} _LL1463: goto _LL1464;
_LL1462: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1471=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1471[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1472; _temp1472.tag=
Cyc_Absyn_PointerType; _temp1472.f1=({ struct Cyc_Absyn_PtrInfo _temp1473;
_temp1473.elt_typ=( void*) _temp1469; _temp1473.rgn_typ=( void*) rgn; _temp1473.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1473.tq=
_temp1467; _temp1473.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1474=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1474[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1475; _temp1475.tag= Cyc_Absyn_Upper_b; _temp1475.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1476= _temp1465; if(
_temp1476 == 0){ _throw( Null_Exception);} _temp1476;}); _temp1475;}); _temp1474;}));
_temp1473;}); _temp1472;}); _temp1471;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1477= topt; if( _temp1477 == 0){ _throw( Null_Exception);}*
_temp1477;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1478= topt; if( _temp1478 == 0){ _throw( Null_Exception);}* _temp1478;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1479=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1479->v=( void*) res_typ; _temp1479;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1480= topt; if( _temp1480 == 0){ _throw( Null_Exception);}*
_temp1480;})); res_typ=({ void** _temp1481= topt; if( _temp1481 == 0){ _throw(
Null_Exception);}* _temp1481;});}} goto _LL1460; _LL1464: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1482=( char*)"tcExpNoPromote on Array_e returned non-array type"; struct
_tagged_string _temp1483; _temp1483.curr= _temp1482; _temp1483.base= _temp1482;
_temp1483.last_plus_one= _temp1482 + 50; _temp1483;})); _LL1460:;} return
res_typ;}} _LL1394: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn,
Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1484=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1484[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1485; _temp1485.tag= Cyc_Absyn_Upper_b; _temp1485.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1485;}); _temp1484;}));} _LL1396: { void** topt2=
0; if( topt != 0){ void* _temp1487= Cyc_Tcutil_compress(({ void** _temp1486=
topt; if( _temp1486 == 0){ _throw( Null_Exception);}* _temp1486;})); struct Cyc_Absyn_PtrInfo
_temp1495; struct Cyc_Absyn_Conref* _temp1497; struct Cyc_Absyn_Tqual _temp1499;
struct Cyc_Absyn_Conref* _temp1501; void* _temp1503; void* _temp1505; void**
_temp1507; struct Cyc_Absyn_TunionInfo _temp1508; _LL1489: if(( unsigned int)
_temp1487 > 4u?*(( int*) _temp1487) == Cyc_Absyn_PointerType: 0){ _LL1496:
_temp1495=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1487)->f1; _LL1506: _temp1505=( void*) _temp1495.elt_typ; _temp1507=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1487)->f1).elt_typ; goto _LL1504;
_LL1504: _temp1503=( void*) _temp1495.rgn_typ; goto _LL1502; _LL1502: _temp1501=(
struct Cyc_Absyn_Conref*) _temp1495.nullable; goto _LL1500; _LL1500: _temp1499=(
struct Cyc_Absyn_Tqual) _temp1495.tq; goto _LL1498; _LL1498: _temp1497=( struct
Cyc_Absyn_Conref*) _temp1495.bounds; goto _LL1490;} else{ goto _LL1491;} _LL1491:
if(( unsigned int) _temp1487 > 4u?*(( int*) _temp1487) == Cyc_Absyn_TunionType:
0){ _LL1509: _temp1508=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1487)->f1; goto _LL1492;} else{ goto _LL1493;} _LL1493: goto _LL1494;
_LL1490: topt2=( void**) _temp1507; goto _LL1488; _LL1492: topt2=({ void**
_temp1510=( void**) GC_malloc( sizeof( void*)); _temp1510[ 0]=({ void**
_temp1511= topt; if( _temp1511 == 0){ _throw( Null_Exception);}* _temp1511;});
_temp1510;}); goto _LL1488; _LL1494: goto _LL1488; _LL1488:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1517=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1517[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1518; _temp1518.tag=
Cyc_Absyn_PointerType; _temp1518.f1=({ struct Cyc_Absyn_PtrInfo _temp1519;
_temp1519.elt_typ=( void*) telt; _temp1519.rgn_typ=( void*) rgn; _temp1519.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1519.tq= Cyc_Absyn_empty_tqual();
_temp1519.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1520=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1520[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1521; _temp1521.tag= Cyc_Absyn_Upper_b; _temp1521.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1521;}); _temp1520;})); _temp1519;}); _temp1518;});
_temp1517;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1512= topt;
if( _temp1512 == 0){ _throw( Null_Exception);}* _temp1512;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1513= topt; if( _temp1513 == 0){ _throw(
Null_Exception);}* _temp1513;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1514=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1514->v=(
void*) res_typ; _temp1514;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1515= topt; if( _temp1515 == 0){ _throw( Null_Exception);}* _temp1515;}));
res_typ=({ void** _temp1516= topt; if( _temp1516 == 0){ _throw( Null_Exception);}*
_temp1516;});}} return res_typ;}} _LL1386:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1522= t; struct Cyc_Absyn_Exp*
_temp1528; struct Cyc_Absyn_Tqual _temp1530; void* _temp1532; _LL1524: if((
unsigned int) _temp1522 > 4u?*(( int*) _temp1522) == Cyc_Absyn_ArrayType: 0){
_LL1533: _temp1532=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1522)->f1;
goto _LL1531; _LL1531: _temp1530=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1522)->f2; goto _LL1529; _LL1529: _temp1528=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1522)->f3; goto _LL1525;} else{ goto
_LL1526;} _LL1526: goto _LL1527; _LL1525: { void* _temp1536; int _temp1538;
struct _tuple5 _temp1534= Cyc_Tcutil_addressof_props( te, e); _LL1539: _temp1538=
_temp1534.f1; goto _LL1537; _LL1537: _temp1536= _temp1534.f2; goto _LL1535;
_LL1535: { void* b= _temp1528 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1540=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1540[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1541; _temp1541.tag= Cyc_Absyn_Upper_b; _temp1541.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1542= _temp1528; if(
_temp1542 == 0){ _throw( Null_Exception);} _temp1542;}); _temp1541;}); _temp1540;});
t= Cyc_Absyn_atb_typ( _temp1532, _temp1536, _temp1530, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1527: return t; _LL1523:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1543=( void*) e->r; struct Cyc_List_List* _temp1553;
struct Cyc_Absyn_Exp* _temp1555; struct Cyc_Absyn_Exp* _temp1557; struct Cyc_Absyn_Vardecl*
_temp1559; void* _temp1561; struct _tagged_string _temp1563; _LL1545: if(*(( int*)
_temp1543) == Cyc_Absyn_Array_e){ _LL1554: _temp1553=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1543)->f1; goto _LL1546;} else{ goto
_LL1547;} _LL1547: if(*(( int*) _temp1543) == Cyc_Absyn_Comprehension_e){
_LL1560: _temp1559=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1543)->f1; goto _LL1558; _LL1558: _temp1557=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1543)->f2; goto _LL1556; _LL1556:
_temp1555=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1543)->f3; goto _LL1548;} else{ goto _LL1549;} _LL1549: if(*(( int*)
_temp1543) == Cyc_Absyn_Const_e){ _LL1562: _temp1561=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1543)->f1; if(( unsigned int) _temp1561 > 1u?*(( int*) _temp1561) == Cyc_Absyn_String_c:
0){ _LL1564: _temp1563=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1561)->f1; goto _LL1550;} else{ goto _LL1551;}} else{ goto _LL1551;}
_LL1551: goto _LL1552; _LL1546: return t; _LL1548: return t; _LL1550: return t;
_LL1552: t= Cyc_Tcutil_compress( t);{ void* _temp1565= t; struct Cyc_Absyn_Exp*
_temp1571; struct Cyc_Absyn_Tqual _temp1573; void* _temp1575; _LL1567: if((
unsigned int) _temp1565 > 4u?*(( int*) _temp1565) == Cyc_Absyn_ArrayType: 0){
_LL1576: _temp1575=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1565)->f1;
goto _LL1574; _LL1574: _temp1573=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1565)->f2; goto _LL1572; _LL1572: _temp1571=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1565)->f3; goto _LL1568;} else{ goto
_LL1569;} _LL1569: goto _LL1570; _LL1568: { void* _temp1579; int _temp1581;
struct _tuple5 _temp1577= Cyc_Tcutil_addressof_props( te, e); _LL1582: _temp1581=
_temp1577.f1; goto _LL1580; _LL1580: _temp1579= _temp1577.f2; goto _LL1578;
_LL1578: { void* b= _temp1571 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1583=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1583[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1584; _temp1584.tag= Cyc_Absyn_Upper_b; _temp1584.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1585= _temp1571; if(
_temp1585 == 0){ _throw( Null_Exception);} _temp1585;}); _temp1584;}); _temp1583;});
t= Cyc_Absyn_atb_typ( _temp1575, _temp1579, _temp1573, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1570: return t; _LL1566:;} _LL1544:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1586=( void*) e->r; struct Cyc_Absyn_Exp* _temp1592; _LL1588: if(*(( int*)
_temp1586) == Cyc_Absyn_NoInstantiate_e){ _LL1593: _temp1592=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1586)->f1; goto _LL1589;} else{
goto _LL1590;} _LL1590: goto _LL1591; _LL1589: Cyc_Tcexp_tcExpNoInst( te, topt,
_temp1592);( void*)({ struct Cyc_Core_Opt* _temp1594= _temp1592->topt; if(
_temp1594 == 0){ _throw( Null_Exception);} _temp1594->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1595= _temp1592->topt;
if( _temp1595 == 0){ _throw( Null_Exception);} _temp1595->v;})));}); e->topt=
_temp1592->topt; goto _LL1587; _LL1591: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)({ struct Cyc_Core_Opt* _temp1596= e->topt; if( _temp1596 == 0){ _throw(
Null_Exception);} _temp1596->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1597= e->topt; if( _temp1597 == 0){ _throw(
Null_Exception);} _temp1597->v;})));});{ void* _temp1599= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1598= e->topt; if( _temp1598 == 0){ _throw(
Null_Exception);} _temp1598->v;})); struct Cyc_Absyn_PtrInfo _temp1605; struct
Cyc_Absyn_Conref* _temp1607; struct Cyc_Absyn_Tqual _temp1609; struct Cyc_Absyn_Conref*
_temp1611; void* _temp1613; void* _temp1615; _LL1601: if(( unsigned int)
_temp1599 > 4u?*(( int*) _temp1599) == Cyc_Absyn_PointerType: 0){ _LL1606:
_temp1605=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1599)->f1; _LL1616: _temp1615=( void*) _temp1605.elt_typ; goto _LL1614;
_LL1614: _temp1613=( void*) _temp1605.rgn_typ; goto _LL1612; _LL1612: _temp1611=(
struct Cyc_Absyn_Conref*) _temp1605.nullable; goto _LL1610; _LL1610: _temp1609=(
struct Cyc_Absyn_Tqual) _temp1605.tq; goto _LL1608; _LL1608: _temp1607=( struct
Cyc_Absyn_Conref*) _temp1605.bounds; goto _LL1602;} else{ goto _LL1603;} _LL1603:
goto _LL1604; _LL1602:{ void* _temp1617= Cyc_Tcutil_compress( _temp1615); struct
Cyc_Absyn_FnInfo _temp1623; struct Cyc_List_List* _temp1625; int _temp1627;
struct Cyc_List_List* _temp1629; void* _temp1631; struct Cyc_Core_Opt* _temp1633;
struct Cyc_List_List* _temp1635; _LL1619: if(( unsigned int) _temp1617 > 4u?*((
int*) _temp1617) == Cyc_Absyn_FnType: 0){ _LL1624: _temp1623=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1617)->f1; _LL1636: _temp1635=( struct Cyc_List_List*)
_temp1623.tvars; goto _LL1634; _LL1634: _temp1633=( struct Cyc_Core_Opt*)
_temp1623.effect; goto _LL1632; _LL1632: _temp1631=( void*) _temp1623.ret_typ;
goto _LL1630; _LL1630: _temp1629=( struct Cyc_List_List*) _temp1623.args; goto
_LL1628; _LL1628: _temp1627=( int) _temp1623.varargs; goto _LL1626; _LL1626:
_temp1625=( struct Cyc_List_List*) _temp1623.attributes; goto _LL1620;} else{
goto _LL1621;} _LL1621: goto _LL1622; _LL1620: if( _temp1635 != 0){ struct
_RegionHandle _temp1637= _new_region(); struct _RegionHandle* rgn=& _temp1637;
struct _handler_cons _temp1638; _push_handler(& _temp1638);{ void* _temp1639=(
void*) setjmp( _temp1638.handler); if( ! _temp1639){{ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn, _temp1635); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1646=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1646[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1647; _temp1647.tag= Cyc_Absyn_FnType;
_temp1647.f1=({ struct Cyc_Absyn_FnInfo _temp1648; _temp1648.tvars= 0; _temp1648.effect=
_temp1633; _temp1648.ret_typ=( void*) _temp1631; _temp1648.args= _temp1629;
_temp1648.varargs= _temp1627; _temp1648.attributes= _temp1625; _temp1648;});
_temp1647;}); _temp1646;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1643=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1643[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1644; _temp1644.tag=
Cyc_Absyn_PointerType; _temp1644.f1=({ struct Cyc_Absyn_PtrInfo _temp1645;
_temp1645.elt_typ=( void*) ftyp; _temp1645.rgn_typ=( void*) _temp1613; _temp1645.nullable=
_temp1611; _temp1645.tq= _temp1609; _temp1645.bounds= _temp1607; _temp1645;});
_temp1644;}); _temp1643;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1640=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1640[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1641; _temp1641.tag= Cyc_Absyn_Instantiate_e; _temp1641.f1= inner;
_temp1641.f2= ts; _temp1641;}); _temp1640;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1642=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1642->v=( void*) new_typ; _temp1642;});} _free_region(& _temp1637);
_pop_handler();} else{ _free_region(& _temp1637); _throw( _temp1639);}}} goto
_LL1618; _LL1622: goto _LL1618; _LL1618:;} goto _LL1600; _LL1604: goto _LL1600;
_LL1600:;} goto _LL1587; _LL1587:;} return( void*)({ struct Cyc_Core_Opt*
_temp1649= e->topt; if( _temp1649 == 0){ _throw( Null_Exception);} _temp1649->v;});}
static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc; void* t;{
void* _temp1650=( void*) e->r; struct Cyc_Absyn_Exp* _temp1720; struct _tuple1*
_temp1722; struct Cyc_List_List* _temp1724; struct Cyc_Absyn_Exp* _temp1726;
struct Cyc_List_List* _temp1728; struct Cyc_Core_Opt* _temp1730; void* _temp1732;
void* _temp1734; struct _tuple1* _temp1736; struct Cyc_List_List* _temp1738;
void* _temp1740; void* _temp1742; struct Cyc_Absyn_Exp* _temp1744; struct Cyc_Absyn_Exp*
_temp1746; struct Cyc_Core_Opt* _temp1748; struct Cyc_Absyn_Exp* _temp1750;
struct Cyc_Absyn_Exp* _temp1752; struct Cyc_Absyn_Exp* _temp1754; struct Cyc_Absyn_Exp*
_temp1756; struct Cyc_Absyn_Exp* _temp1758; struct Cyc_Absyn_Exp* _temp1760;
struct Cyc_List_List* _temp1762; struct Cyc_Absyn_Exp* _temp1764; struct Cyc_Absyn_Exp*
_temp1766; struct Cyc_List_List* _temp1768; struct Cyc_Absyn_Exp* _temp1770;
struct Cyc_Absyn_Exp* _temp1772; void* _temp1774; struct Cyc_Absyn_Exp*
_temp1776; struct Cyc_Absyn_Exp* _temp1778; struct Cyc_Absyn_Exp* _temp1780;
struct Cyc_Absyn_Exp* _temp1782; void* _temp1784; struct Cyc_Absyn_Exp*
_temp1786; struct _tagged_string* _temp1788; struct Cyc_Absyn_Exp* _temp1790;
struct _tagged_string* _temp1792; struct Cyc_Absyn_Exp* _temp1794; struct Cyc_Absyn_Exp*
_temp1796; struct Cyc_Absyn_Exp* _temp1798; struct Cyc_List_List* _temp1800;
struct Cyc_List_List* _temp1802; struct _tuple2* _temp1804; struct Cyc_List_List*
_temp1806; struct Cyc_Absyn_Stmt* _temp1808; struct Cyc_Absyn_Fndecl* _temp1810;
struct Cyc_Absyn_Exp* _temp1812; struct Cyc_Absyn_Exp* _temp1814; struct Cyc_Absyn_Exp*
_temp1816; struct Cyc_Absyn_Vardecl* _temp1818; struct Cyc_Absyn_Structdecl*
_temp1820; struct Cyc_Absyn_Structdecl** _temp1822; struct Cyc_List_List*
_temp1823; struct Cyc_Core_Opt* _temp1825; struct Cyc_Core_Opt** _temp1827;
struct _tuple1* _temp1828; struct _tuple1** _temp1830; struct Cyc_Absyn_Tunionfield*
_temp1831; struct Cyc_Absyn_Tuniondecl* _temp1833; struct Cyc_List_List*
_temp1835; struct Cyc_Core_Opt* _temp1837; struct Cyc_Core_Opt** _temp1839;
struct Cyc_Core_Opt* _temp1840; struct Cyc_Core_Opt** _temp1842; struct Cyc_Absyn_Enumfield*
_temp1843; struct Cyc_Absyn_Enumdecl* _temp1845; struct _tuple1* _temp1847; void*
_temp1849; struct Cyc_Absyn_Exp* _temp1851; _LL1652: if(*(( int*) _temp1650) ==
Cyc_Absyn_NoInstantiate_e){ _LL1721: _temp1720=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_NoInstantiate_e_struct*) _temp1650)->f1; goto _LL1653;} else{ goto
_LL1654;} _LL1654: if(*(( int*) _temp1650) == Cyc_Absyn_UnknownId_e){ _LL1723:
_temp1722=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1650)->f1;
goto _LL1655;} else{ goto _LL1656;} _LL1656: if(*(( int*) _temp1650) == Cyc_Absyn_UnknownCall_e){
_LL1727: _temp1726=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1650)->f1; goto _LL1725; _LL1725: _temp1724=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1650)->f2; goto _LL1657;} else{
goto _LL1658;} _LL1658: if(*(( int*) _temp1650) == Cyc_Absyn_UnresolvedMem_e){
_LL1731: _temp1730=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1650)->f1; goto _LL1729; _LL1729: _temp1728=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1650)->f2; goto _LL1659;} else{
goto _LL1660;} _LL1660: if(*(( int*) _temp1650) == Cyc_Absyn_Const_e){ _LL1733:
_temp1732=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1650)->f1; goto
_LL1661;} else{ goto _LL1662;} _LL1662: if(*(( int*) _temp1650) == Cyc_Absyn_Var_e){
_LL1737: _temp1736=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1650)->f1; goto _LL1735; _LL1735: _temp1734=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1650)->f2; goto _LL1663;} else{ goto _LL1664;} _LL1664: if(*(( int*)
_temp1650) == Cyc_Absyn_Primop_e){ _LL1741: _temp1740=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1650)->f1; goto _LL1739; _LL1739: _temp1738=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp1650)->f2; goto _LL1665;} else{ goto
_LL1666;} _LL1666: if(*(( int*) _temp1650) == Cyc_Absyn_Increment_e){ _LL1745:
_temp1744=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1650)->f1; goto _LL1743; _LL1743: _temp1742=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1650)->f2; goto _LL1667;} else{ goto _LL1668;} _LL1668: if(*(( int*)
_temp1650) == Cyc_Absyn_AssignOp_e){ _LL1751: _temp1750=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1650)->f1; goto _LL1749; _LL1749:
_temp1748=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1650)->f2; goto _LL1747; _LL1747: _temp1746=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1650)->f3; goto _LL1669;} else{ goto
_LL1670;} _LL1670: if(*(( int*) _temp1650) == Cyc_Absyn_Conditional_e){ _LL1757:
_temp1756=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1650)->f1; goto _LL1755; _LL1755: _temp1754=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1650)->f2; goto _LL1753; _LL1753:
_temp1752=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1650)->f3; goto _LL1671;} else{ goto _LL1672;} _LL1672: if(*(( int*)
_temp1650) == Cyc_Absyn_SeqExp_e){ _LL1761: _temp1760=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1650)->f1; goto _LL1759; _LL1759:
_temp1758=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1650)->f2; goto _LL1673;} else{ goto _LL1674;} _LL1674: if(*(( int*)
_temp1650) == Cyc_Absyn_FnCall_e){ _LL1765: _temp1764=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1650)->f1; goto _LL1763; _LL1763:
_temp1762=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1650)->f2; goto _LL1675;} else{ goto _LL1676;} _LL1676: if(*(( int*)
_temp1650) == Cyc_Absyn_Throw_e){ _LL1767: _temp1766=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Throw_e_struct*) _temp1650)->f1; goto _LL1677;} else{ goto
_LL1678;} _LL1678: if(*(( int*) _temp1650) == Cyc_Absyn_Instantiate_e){ _LL1771:
_temp1770=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1650)->f1; goto _LL1769; _LL1769: _temp1768=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1650)->f2; goto _LL1679;} else{
goto _LL1680;} _LL1680: if(*(( int*) _temp1650) == Cyc_Absyn_Cast_e){ _LL1775:
_temp1774=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1650)->f1; goto
_LL1773; _LL1773: _temp1772=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1650)->f2; goto _LL1681;} else{ goto _LL1682;} _LL1682: if(*(( int*)
_temp1650) == Cyc_Absyn_Address_e){ _LL1777: _temp1776=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1650)->f1; goto _LL1683;} else{ goto
_LL1684;} _LL1684: if(*(( int*) _temp1650) == Cyc_Absyn_New_e){ _LL1781:
_temp1780=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1650)->f1;
goto _LL1779; _LL1779: _temp1778=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1650)->f2; goto _LL1685;} else{ goto _LL1686;} _LL1686: if(*(( int*)
_temp1650) == Cyc_Absyn_Sizeofexp_e){ _LL1783: _temp1782=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1650)->f1; goto _LL1687;} else{ goto
_LL1688;} _LL1688: if(*(( int*) _temp1650) == Cyc_Absyn_Sizeoftyp_e){ _LL1785:
_temp1784=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1650)->f1; goto
_LL1689;} else{ goto _LL1690;} _LL1690: if(*(( int*) _temp1650) == Cyc_Absyn_Deref_e){
_LL1787: _temp1786=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1650)->f1; goto _LL1691;} else{ goto _LL1692;} _LL1692: if(*(( int*)
_temp1650) == Cyc_Absyn_StructMember_e){ _LL1791: _temp1790=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1650)->f1; goto _LL1789; _LL1789:
_temp1788=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1650)->f2; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1650) == Cyc_Absyn_StructArrow_e){ _LL1795: _temp1794=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1650)->f1; goto _LL1793; _LL1793:
_temp1792=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1650)->f2; goto _LL1695;} else{ goto _LL1696;} _LL1696: if(*(( int*)
_temp1650) == Cyc_Absyn_Subscript_e){ _LL1799: _temp1798=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1650)->f1; goto _LL1797; _LL1797:
_temp1796=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1650)->f2; goto _LL1697;} else{ goto _LL1698;} _LL1698: if(*(( int*)
_temp1650) == Cyc_Absyn_Tuple_e){ _LL1801: _temp1800=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp1650)->f1; goto _LL1699;} else{ goto
_LL1700;} _LL1700: if(*(( int*) _temp1650) == Cyc_Absyn_CompoundLit_e){ _LL1805:
_temp1804=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1650)->f1;
goto _LL1803; _LL1803: _temp1802=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1650)->f2; goto _LL1701;} else{ goto _LL1702;} _LL1702: if(*(( int*)
_temp1650) == Cyc_Absyn_Array_e){ _LL1807: _temp1806=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1650)->f1; goto _LL1703;} else{ goto
_LL1704;} _LL1704: if(*(( int*) _temp1650) == Cyc_Absyn_StmtExp_e){ _LL1809:
_temp1808=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1650)->f1; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(*(( int*)
_temp1650) == Cyc_Absyn_Codegen_e){ _LL1811: _temp1810=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1650)->f1; goto _LL1707;} else{ goto
_LL1708;} _LL1708: if(*(( int*) _temp1650) == Cyc_Absyn_Fill_e){ _LL1813:
_temp1812=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1650)->f1;
goto _LL1709;} else{ goto _LL1710;} _LL1710: if(*(( int*) _temp1650) == Cyc_Absyn_Comprehension_e){
_LL1819: _temp1818=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1650)->f1; goto _LL1817; _LL1817: _temp1816=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1650)->f2; goto _LL1815; _LL1815:
_temp1814=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1650)->f3; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(*(( int*)
_temp1650) == Cyc_Absyn_Struct_e){ _LL1829: _temp1828=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1650)->f1; _temp1830=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1650)->f1; goto _LL1826; _LL1826: _temp1825=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1650)->f2; _temp1827=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1650)->f2; goto _LL1824; _LL1824: _temp1823=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1650)->f3; goto _LL1821; _LL1821:
_temp1820=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1650)->f4; _temp1822=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1650)->f4;
goto _LL1713;} else{ goto _LL1714;} _LL1714: if(*(( int*) _temp1650) == Cyc_Absyn_Tunion_e){
_LL1841: _temp1840=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1650)->f1; _temp1842=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1650)->f1;
goto _LL1838; _LL1838: _temp1837=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1650)->f2; _temp1839=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1650)->f2;
goto _LL1836; _LL1836: _temp1835=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1650)->f3; goto _LL1834; _LL1834: _temp1833=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1650)->f4; goto _LL1832; _LL1832:
_temp1831=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1650)->f5; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(*(( int*)
_temp1650) == Cyc_Absyn_Enum_e){ _LL1848: _temp1847=( struct _tuple1*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp1650)->f1; goto _LL1846; _LL1846: _temp1845=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1650)->f2;
goto _LL1844; _LL1844: _temp1843=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1650)->f3; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(*(( int*)
_temp1650) == Cyc_Absyn_Malloc_e){ _LL1852: _temp1851=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp1650)->f1; goto _LL1850; _LL1850:
_temp1849=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1650)->f2; goto
_LL1719;} else{ goto _LL1651;} _LL1653: Cyc_Tcexp_tcExpNoInst( te, 0, _temp1720);
return; _LL1655: Cyc_Tcexp_resolve_unknown_id( te, e, _temp1722); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1657: Cyc_Tcexp_resolve_unknown_fn( te, e, _temp1726,
_temp1724); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1659: Cyc_Tcexp_resolve_unresolved_mem(
te, loc, topt, e, _temp1728); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1661: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1732, e); goto _LL1651;
_LL1663: t= Cyc_Tcexp_tcVar( te, loc, _temp1736, _temp1734); goto _LL1651;
_LL1665: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1740, _temp1738); goto
_LL1651; _LL1667: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1744, _temp1742);
goto _LL1651; _LL1669: t= Cyc_Tcexp_tcAssignOp( te, loc, topt, _temp1750,
_temp1748, _temp1746); goto _LL1651; _LL1671: t= Cyc_Tcexp_tcConditional( te,
loc, topt, _temp1756, _temp1754, _temp1752); goto _LL1651; _LL1673: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1760, _temp1758); goto _LL1651; _LL1675: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1764, _temp1762); goto _LL1651; _LL1677: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1766); goto _LL1651; _LL1679: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1770, _temp1768); goto _LL1651; _LL1681: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1774, _temp1772); goto _LL1651; _LL1683: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1776); goto _LL1651; _LL1685: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1780, e, _temp1778); goto _LL1651; _LL1687: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1782); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1651;}
_LL1689: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1784); goto _LL1651; _LL1691:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1786); goto _LL1651; _LL1693: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1790, _temp1788); goto _LL1651; _LL1695: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1794, _temp1792); goto _LL1651; _LL1697: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1798, _temp1796); goto _LL1651; _LL1699: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1800); goto _LL1651; _LL1701: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1804, _temp1802); goto _LL1651; _LL1703: { void** elt_topt= 0;
if( topt != 0){ void* _temp1854= Cyc_Tcutil_compress(({ void** _temp1853= topt;
if( _temp1853 == 0){ _throw( Null_Exception);}* _temp1853;})); struct Cyc_Absyn_Exp*
_temp1860; struct Cyc_Absyn_Tqual _temp1862; void* _temp1864; void** _temp1866;
_LL1856: if(( unsigned int) _temp1854 > 4u?*(( int*) _temp1854) == Cyc_Absyn_ArrayType:
0){ _LL1865: _temp1864=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1854)->f1;
_temp1866=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1854)->f1; goto _LL1863;
_LL1863: _temp1862=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1854)->f2; goto _LL1861; _LL1861: _temp1860=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1854)->f3; goto _LL1857;} else{ goto
_LL1858;} _LL1858: goto _LL1859; _LL1857: elt_topt=( void**) _temp1866; goto
_LL1855; _LL1859: goto _LL1855; _LL1855:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1806); goto _LL1651;} _LL1705: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1808); goto _LL1651; _LL1707: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1810); goto _LL1651; _LL1709: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1812);
goto _LL1651; _LL1711: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1818,
_temp1816, _temp1814); goto _LL1651; _LL1713: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1830, _temp1827, _temp1823, _temp1822); goto _LL1651; _LL1715: t= Cyc_Tcexp_tcTunion(
te, loc, topt, e, _temp1842, _temp1839, _temp1835, _temp1833, _temp1831); goto
_LL1651; _LL1717: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1867=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1867[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1868; _temp1868.tag= Cyc_Absyn_EnumType;
_temp1868.f1=({ struct Cyc_Absyn_Enumdecl* _temp1869= _temp1845; if( _temp1869
== 0){ _throw( Null_Exception);} _temp1869->name;}); _temp1868.f2= _temp1845;
_temp1868;}); _temp1867;}); goto _LL1651; _LL1719: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp1851, _temp1849); goto _LL1651; _LL1651:;} e->topt=({ struct Cyc_Core_Opt*
_temp1870=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1870->v=( void*) t; _temp1870;});}