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
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern int Cyc_String_zstrcmp( struct
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
_temp138= topt; if( _temp138 == 0){ _throw( Null_Exception);}* _temp138;}); void*
_temp102= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp112; struct Cyc_Absyn_Tqual
_temp114; void* _temp116; struct Cyc_Absyn_Structdecl** _temp118; struct Cyc_List_List*
_temp120; struct _tuple1* _temp122; struct Cyc_List_List* _temp124; _LL104: if((
unsigned int) _temp102 > 4u?*(( int*) _temp102) == Cyc_Absyn_ArrayType: 0){
_LL117: _temp116=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp102)->f1;
goto _LL115; _LL115: _temp114=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp102)->f2; goto _LL113; _LL113: _temp112=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp102)->f3; goto _LL105;} else{ goto _LL106;}
_LL106: if(( unsigned int) _temp102 > 4u?*(( int*) _temp102) == Cyc_Absyn_StructType:
0){ _LL123: _temp122=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp102)->f1; goto _LL121; _LL121: _temp120=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp102)->f2; goto _LL119; _LL119: _temp118=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp102)->f3;
goto _LL107;} else{ goto _LL108;} _LL108: if(( unsigned int) _temp102 > 4u?*((
int*) _temp102) == Cyc_Absyn_AnonStructType: 0){ _LL125: _temp124=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp102)->f1; goto _LL109;} else{ goto
_LL110;} _LL110: goto _LL111; _LL105:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp126=( struct Cyc_Absyn_Array_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp126[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp127; _temp127.tag= Cyc_Absyn_Array_e; _temp127.f1= des; _temp127;});
_temp126;}))); goto _LL103; _LL107: if( _temp118 == 0? 1: _temp122 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp128=( char*)"Compiler bug: struct type not properly set";
struct _tagged_string _temp129; _temp129.curr= _temp128; _temp129.base= _temp128;
_temp129.last_plus_one= _temp128 + 43; _temp129;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp130=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp130[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp131; _temp131.tag= Cyc_Absyn_Struct_e; _temp131.f1=(
struct _tuple1*)({ struct _tuple1* _temp132= _temp122; if( _temp132 == 0){
_throw( Null_Exception);} _temp132;}); _temp131.f2= 0; _temp131.f3= des;
_temp131.f4=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl**
_temp133= _temp118; if( _temp133 == 0){ _throw( Null_Exception);}* _temp133;});
_temp131;}); _temp130;}))); goto _LL103; _LL109:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_AnonStruct_e_struct* _temp134=( struct Cyc_Absyn_AnonStruct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct)); _temp134[ 0]=({
struct Cyc_Absyn_AnonStruct_e_struct _temp135; _temp135.tag= Cyc_Absyn_AnonStruct_e;
_temp135.f1=( void*) t; _temp135.f2= des; _temp135;}); _temp134;}))); goto
_LL103; _LL111:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp136=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp136[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp137; _temp137.tag= Cyc_Absyn_Array_e;
_temp137.f1= des; _temp137;}); _temp136;}))); goto _LL103; _LL103:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp139= es; if( _temp139 == 0){ _throw( Null_Exception);} _temp139->tl;})){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp140=
es; if( _temp140 == 0){ _throw( Null_Exception);} _temp140->hd;}));}} void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp( te,( void**)&
Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp142= msg_part; struct _tagged_string
_temp143= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp141= e->topt;
if( _temp141 == 0){ _throw( Null_Exception);} _temp141->v;})); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp142.last_plus_one - _temp142.curr, _temp142.curr, _temp143.last_plus_one -
_temp143.curr, _temp143.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp144= c; char _temp160; void* _temp162; short _temp164;
void* _temp166; long long _temp168; void* _temp170; struct _tagged_string
_temp172; int _temp174; void* _temp176; struct _tagged_string _temp178; _LL146:
if(( unsigned int) _temp144 > 1u?*(( int*) _temp144) == Cyc_Absyn_Char_c: 0){
_LL163: _temp162=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp144)->f1; goto
_LL161; _LL161: _temp160=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp144)->f2;
goto _LL147;} else{ goto _LL148;} _LL148: if(( unsigned int) _temp144 > 1u?*((
int*) _temp144) == Cyc_Absyn_Short_c: 0){ _LL167: _temp166=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp144)->f1; goto _LL165; _LL165: _temp164=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp144)->f2; goto _LL149;} else{ goto _LL150;} _LL150: if(( unsigned int)
_temp144 > 1u?*(( int*) _temp144) == Cyc_Absyn_LongLong_c: 0){ _LL171: _temp170=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp144)->f1; goto _LL169; _LL169:
_temp168=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp144)->f2; goto
_LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp144 > 1u?*(( int*)
_temp144) == Cyc_Absyn_Float_c: 0){ _LL173: _temp172=( struct _tagged_string)((
struct Cyc_Absyn_Float_c_struct*) _temp144)->f1; goto _LL153;} else{ goto _LL154;}
_LL154: if(( unsigned int) _temp144 > 1u?*(( int*) _temp144) == Cyc_Absyn_Int_c:
0){ _LL177: _temp176=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp144)->f1;
goto _LL175; _LL175: _temp174=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp144)->f2;
goto _LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp144 > 1u?*((
int*) _temp144) == Cyc_Absyn_String_c: 0){ _LL179: _temp178=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp144)->f1; goto _LL157;}
else{ goto _LL158;} _LL158: if( _temp144 ==( void*) Cyc_Absyn_Null_c){ goto
_LL159;} else{ goto _LL145;} _LL147: t= _temp162 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL145; _LL149: t= _temp166 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL145; _LL151:
t= _temp170 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t;
goto _LL145; _LL153: t= Cyc_Absyn_float_t; goto _LL145; _LL155: if( topt == 0){
t= _temp176 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp181=({ void** _temp180= topt; if( _temp180 == 0){ _throw(
Null_Exception);}* _temp180;}); void* _temp191; void* _temp193; void* _temp195;
void* _temp197; void* _temp199; void* _temp201; _LL183: if(( unsigned int)
_temp181 > 4u?*(( int*) _temp181) == Cyc_Absyn_IntType: 0){ _LL194: _temp193=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp181)->f1; goto _LL192; _LL192:
_temp191=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp181)->f2; if(
_temp191 ==( void*) Cyc_Absyn_B1){ goto _LL184;} else{ goto _LL185;}} else{ goto
_LL185;} _LL185: if(( unsigned int) _temp181 > 4u?*(( int*) _temp181) == Cyc_Absyn_IntType:
0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp181)->f1;
goto _LL196; _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f2; if( _temp195 ==( void*) Cyc_Absyn_B2){ goto _LL186;} else{ goto
_LL187;}} else{ goto _LL187;} _LL187: if(( unsigned int) _temp181 > 4u?*(( int*)
_temp181) == Cyc_Absyn_IntType: 0){ _LL202: _temp201=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f1; goto _LL200; _LL200: _temp199=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp181)->f2; goto _LL188;} else{ goto _LL189;} _LL189: goto _LL190; _LL184: t=
_temp193 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp203=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp203[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp204; _temp204.tag= Cyc_Absyn_Const_e;
_temp204.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp205=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp205[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp206; _temp206.tag= Cyc_Absyn_Char_c;
_temp206.f1=( void*) _temp193; _temp206.f2=( char) _temp174; _temp206;});
_temp205;})); _temp204;}); _temp203;}))); goto _LL182; _LL186: t= _temp197 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp207=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp207[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp208; _temp208.tag= Cyc_Absyn_Const_e; _temp208.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp209=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp209[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp210; _temp210.tag= Cyc_Absyn_Short_c; _temp210.f1=( void*) _temp197;
_temp210.f2=( short) _temp174; _temp210;}); _temp209;})); _temp208;}); _temp207;})));
goto _LL182; _LL188: t= _temp201 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL182; _LL190: t= _temp176 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL182; _LL182:;} goto _LL145; _LL157: {
int len=( int)({ struct _tagged_string _temp259= _temp178;( unsigned int)(
_temp259.last_plus_one - _temp259.curr);}); struct Cyc_Absyn_Const_e_struct* r=({
struct Cyc_Absyn_Const_e_struct* _temp255=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp255[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp256; _temp256.tag= Cyc_Absyn_Const_e; _temp256.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp257=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp257[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp258; _temp258.tag= Cyc_Absyn_Int_c; _temp258.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp258.f2= len; _temp258;}); _temp257;})); _temp256;}); _temp255;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) r, loc); elen->topt=({ struct
Cyc_Core_Opt* _temp211=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp211->v=( void*) Cyc_Absyn_uint_t; _temp211;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp212=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp212[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp213; _temp213.tag= Cyc_Absyn_Upper_b;
_temp213.f1= elen; _temp213;}); _temp212;})); if( topt != 0){ void* _temp215=
Cyc_Tcutil_compress(({ void** _temp214= topt; if( _temp214 == 0){ _throw(
Null_Exception);}* _temp214;})); struct Cyc_Absyn_Exp* _temp223; struct Cyc_Absyn_Tqual
_temp225; void* _temp227; struct Cyc_Absyn_PtrInfo _temp229; struct Cyc_Absyn_Conref*
_temp231; struct Cyc_Absyn_Tqual _temp233; struct Cyc_Absyn_Conref* _temp235;
void* _temp237; void* _temp239; _LL217: if(( unsigned int) _temp215 > 4u?*(( int*)
_temp215) == Cyc_Absyn_ArrayType: 0){ _LL228: _temp227=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp215)->f1; goto _LL226; _LL226: _temp225=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp215)->f2; goto _LL224; _LL224: _temp223=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp215)->f3; goto
_LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp215 > 4u?*(( int*)
_temp215) == Cyc_Absyn_PointerType: 0){ _LL230: _temp229=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp215)->f1; _LL240: _temp239=( void*)
_temp229.elt_typ; goto _LL238; _LL238: _temp237=( void*) _temp229.rgn_typ; goto
_LL236; _LL236: _temp235=( struct Cyc_Absyn_Conref*) _temp229.nullable; goto
_LL234; _LL234: _temp233=( struct Cyc_Absyn_Tqual) _temp229.tq; goto _LL232;
_LL232: _temp231=( struct Cyc_Absyn_Conref*) _temp229.bounds; goto _LL220;}
else{ goto _LL221;} _LL221: goto _LL222; _LL218: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp241=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp242; _temp242.tag= Cyc_Absyn_ArrayType;
_temp242.f1=( void*) Cyc_Absyn_uchar_t; _temp242.f2= _temp225; _temp242.f3=(
struct Cyc_Absyn_Exp*) elen; _temp242;}); _temp241;}); _LL220: if( ! Cyc_Tcutil_unify(({
void** _temp243= topt; if( _temp243 == 0){ _throw( Null_Exception);}* _temp243;}),
t)? Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp244= topt; if(
_temp244 == 0){ _throw( Null_Exception);}* _temp244;})): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp245=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp245->v=( void*) t; _temp245;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp246= topt; if( _temp246 == 0){ _throw( Null_Exception);}* _temp246;})); t=({
void** _temp247= topt; if( _temp247 == 0){ _throw( Null_Exception);}* _temp247;});}
else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp248=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp248[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp249; _temp249.tag= Cyc_Absyn_Upper_b;
_temp249.f1= elen; _temp249;}); _temp248;})); if( ! Cyc_Tcutil_unify(({ void**
_temp250= topt; if( _temp250 == 0){ _throw( Null_Exception);}* _temp250;}), t)?
Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp251= topt; if( _temp251 ==
0){ _throw( Null_Exception);}* _temp251;})): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp252=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp252->v=( void*) t; _temp252;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp253= topt; if( _temp253 == 0){ _throw( Null_Exception);}* _temp253;})); t=({
void** _temp254= topt; if( _temp254 == 0){ _throw( Null_Exception);}* _temp254;});}}
goto _LL216; _LL222: goto _LL216; _LL216:;} return t;} _LL159: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp260=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp260[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp261; _temp261.tag= Cyc_Absyn_PointerType;
_temp261.f1=({ struct Cyc_Absyn_PtrInfo _temp262; _temp262.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp262.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp262.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp262.tq= Cyc_Absyn_empty_tqual(); _temp262.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp262;}); _temp261;});
_temp260;}); goto _LL145; _LL145:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp263= b; struct Cyc_Absyn_Vardecl* _temp277; struct Cyc_Absyn_Fndecl*
_temp279; struct Cyc_Absyn_Vardecl* _temp281; struct Cyc_Absyn_Vardecl* _temp283;
struct Cyc_Absyn_Vardecl* _temp285; _LL265: if( _temp263 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int) _temp263 > 1u?*((
int*) _temp263) == Cyc_Absyn_Global_b: 0){ _LL278: _temp277=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp263)->f1; goto _LL268;} else{ goto
_LL269;} _LL269: if(( unsigned int) _temp263 > 1u?*(( int*) _temp263) == Cyc_Absyn_Funname_b:
0){ _LL280: _temp279=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp263)->f1; goto _LL270;} else{ goto _LL271;} _LL271: if(( unsigned int)
_temp263 > 1u?*(( int*) _temp263) == Cyc_Absyn_Pat_b: 0){ _LL282: _temp281=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*) _temp263)->f1; goto
_LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp263 > 1u?*(( int*)
_temp263) == Cyc_Absyn_Local_b: 0){ _LL284: _temp283=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp263)->f1; goto _LL274;} else{ goto _LL275;}
_LL275: if(( unsigned int) _temp263 > 1u?*(( int*) _temp263) == Cyc_Absyn_Param_b:
0){ _LL286: _temp285=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp263)->f1; goto _LL276;} else{ goto _LL264;} _LL266: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp287= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp287.last_plus_one - _temp287.curr, _temp287.curr);})); _LL268:({ struct
_tuple1* _temp288= q; unsigned int _temp289= 0; if( _temp289 >= 1u){ _throw(
Null_Exception);} _temp288[ _temp289]=({ struct _tuple1* _temp290= _temp277->name;
unsigned int _temp291= 0; if( _temp291 >= 1u){ _throw( Null_Exception);}
_temp290[ _temp291];});}); return( void*) _temp277->type; _LL270:({ struct
_tuple1* _temp292= q; unsigned int _temp293= 0; if( _temp293 >= 1u){ _throw(
Null_Exception);} _temp292[ _temp293]=({ struct _tuple1* _temp294= _temp279->name;
unsigned int _temp295= 0; if( _temp295 >= 1u){ _throw( Null_Exception);}
_temp294[ _temp295];});}); return Cyc_Tcutil_fndecl2typ( _temp279); _LL272:
_temp283= _temp281; goto _LL274; _LL274: _temp285= _temp283; goto _LL276; _LL276:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp285->type; _LL264:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp296=( void*) fmt->r; void* _temp302; struct _tagged_string _temp304;
_LL298: if(*(( int*) _temp296) == Cyc_Absyn_Const_e){ _LL303: _temp302=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp296)->f1; if(( unsigned int) _temp302 > 1u?*((
int*) _temp302) == Cyc_Absyn_String_c: 0){ _LL305: _temp304=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp302)->f1; goto _LL299;}
else{ goto _LL300;}} else{ goto _LL300;} _LL300: goto _LL301; _LL299: desc_types=
type_getter( _temp304, fmt->loc); goto _LL297; _LL301: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp306=( char*)"expecting a literal format string";
struct _tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 34; _temp307;})); return; _LL297:;} for( 0;
desc_types != 0? args != 0: 0; desc_types=({ struct Cyc_List_List* _temp308=
desc_types; if( _temp308 == 0){ _throw( Null_Exception);} _temp308->tl;}), args=({
struct Cyc_List_List* _temp309= args; if( _temp309 == 0){ _throw( Null_Exception);}
_temp309->tl;})){ void* t=( void*)({ struct Cyc_List_List* _temp314= desc_types;
if( _temp314 == 0){ _throw( Null_Exception);} _temp314->hd;}); struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp313= args; if( _temp313
== 0){ _throw( Null_Exception);} _temp313->hd;}); if( ! Cyc_Tcutil_coerce_arg(
te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp311= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp312= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp310= e->topt; if( _temp310 == 0){ _throw( Null_Exception);} _temp310->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp311.last_plus_one
- _temp311.curr, _temp311.curr, _temp312.last_plus_one - _temp312.curr, _temp312.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp315=( char*)"too few arguments"; struct _tagged_string _temp316; _temp316.curr=
_temp315; _temp316.base= _temp315; _temp316.last_plus_one= _temp315 + 18;
_temp316;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp317= args; if( _temp317 == 0){ _throw( Null_Exception);}
_temp317->hd;}))->loc,( struct _tagged_string)({ char* _temp318=( char*)"too many arguments";
struct _tagged_string _temp319; _temp319.curr= _temp318; _temp319.base= _temp318;
_temp319.last_plus_one= _temp318 + 19; _temp319;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp368= e->topt; if( _temp368 == 0){ _throw( Null_Exception);} _temp368->v;}));
void* _temp320= p; _LL322: if( _temp320 ==( void*) Cyc_Absyn_Plus){ goto _LL323;}
else{ goto _LL324;} _LL324: if( _temp320 ==( void*) Cyc_Absyn_Minus){ goto
_LL325;} else{ goto _LL326;} _LL326: if( _temp320 ==( void*) Cyc_Absyn_Not){
goto _LL327;} else{ goto _LL328;} _LL328: if( _temp320 ==( void*) Cyc_Absyn_Bitnot){
goto _LL329;} else{ goto _LL330;} _LL330: if( _temp320 ==( void*) Cyc_Absyn_Size){
goto _LL331;} else{ goto _LL332;} _LL332: goto _LL333; _LL323: goto _LL325;
_LL325: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp334= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_temp334.last_plus_one - _temp334.curr, _temp334.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp335= e->topt; if( _temp335 == 0){ _throw(
Null_Exception);} _temp335->v;}); _LL327: Cyc_Tcutil_check_contains_assign( e);
if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp336= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp336.last_plus_one - _temp336.curr, _temp336.curr);}));} return Cyc_Absyn_sint_t;
_LL329: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp337= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp337.last_plus_one - _temp337.curr, _temp337.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp338= e->topt; if( _temp338 == 0){ _throw(
Null_Exception);} _temp338->v;}); _LL331:{ void* _temp339= t; struct Cyc_Absyn_Exp*
_temp347; struct Cyc_Absyn_Tqual _temp349; void* _temp351; struct Cyc_Absyn_PtrInfo
_temp353; struct Cyc_Absyn_Conref* _temp355; struct Cyc_Absyn_Tqual _temp357;
struct Cyc_Absyn_Conref* _temp359; void* _temp361; void* _temp363; _LL341: if((
unsigned int) _temp339 > 4u?*(( int*) _temp339) == Cyc_Absyn_ArrayType: 0){
_LL352: _temp351=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp339)->f1;
goto _LL350; _LL350: _temp349=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp339)->f2; goto _LL348; _LL348: _temp347=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp339)->f3; goto _LL342;} else{ goto _LL343;}
_LL343: if(( unsigned int) _temp339 > 4u?*(( int*) _temp339) == Cyc_Absyn_PointerType:
0){ _LL354: _temp353=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp339)->f1; _LL364: _temp363=( void*) _temp353.elt_typ; goto _LL362; _LL362:
_temp361=( void*) _temp353.rgn_typ; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Conref*)
_temp353.nullable; goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Tqual)
_temp353.tq; goto _LL356; _LL356: _temp355=( struct Cyc_Absyn_Conref*) _temp353.bounds;
goto _LL344;} else{ goto _LL345;} _LL345: goto _LL346; _LL342: goto _LL340;
_LL344: goto _LL340; _LL346: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp365= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_temp365.last_plus_one - _temp365.curr, _temp365.curr);})); goto _LL340; _LL340:;}
return Cyc_Absyn_uint_t; _LL333: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp366=( char*)"Non-unary primop"; struct
_tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 17; _temp367;})); _LL321:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp370= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp369= e1->topt; if( _temp369 == 0){ _throw(
Null_Exception);} _temp369->v;})); xprintf("type %.*s cannot be used here",
_temp370.last_plus_one - _temp370.curr, _temp370.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp372=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp371= e2->topt; if(
_temp371 == 0){ _throw( Null_Exception);} _temp371->v;})); xprintf("type %.*s cannot be used here",
_temp372.last_plus_one - _temp372.curr, _temp372.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp374= e1->topt;
if( _temp374 == 0){ _throw( Null_Exception);} _temp374->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp373= e2->topt; if( _temp373 == 0){ _throw(
Null_Exception);} _temp373->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp434= e1->topt; if( _temp434 == 0){ _throw( Null_Exception);}
_temp434->v;})); void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp433= e2->topt; if( _temp433 == 0){ _throw( Null_Exception);} _temp433->v;}));
void* _temp375= t1; struct Cyc_Absyn_PtrInfo _temp381; struct Cyc_Absyn_Conref*
_temp383; struct Cyc_Absyn_Tqual _temp385; struct Cyc_Absyn_Conref* _temp387;
void* _temp389; void* _temp391; _LL377: if(( unsigned int) _temp375 > 4u?*(( int*)
_temp375) == Cyc_Absyn_PointerType: 0){ _LL382: _temp381=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp375)->f1; _LL392: _temp391=( void*)
_temp381.elt_typ; goto _LL390; _LL390: _temp389=( void*) _temp381.rgn_typ; goto
_LL388; _LL388: _temp387=( struct Cyc_Absyn_Conref*) _temp381.nullable; goto
_LL386; _LL386: _temp385=( struct Cyc_Absyn_Tqual) _temp381.tq; goto _LL384;
_LL384: _temp383=( struct Cyc_Absyn_Conref*) _temp381.bounds; goto _LL378;}
else{ goto _LL379;} _LL379: goto _LL380; _LL378: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp393= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int but found %.*s", _temp393.last_plus_one - _temp393.curr,
_temp393.curr);}));} _temp383=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp383);{ void* _temp394=( void*) _temp383->v;
void* _temp402; void* _temp404; struct Cyc_Absyn_Exp* _temp406; _LL396: if((
unsigned int) _temp394 > 1u?*(( int*) _temp394) == Cyc_Absyn_Eq_constr: 0){
_LL403: _temp402=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp394)->f1;
if( _temp402 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL397;} else{ goto _LL398;}}
else{ goto _LL398;} _LL398: if(( unsigned int) _temp394 > 1u?*(( int*) _temp394)
== Cyc_Absyn_Eq_constr: 0){ _LL405: _temp404=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp394)->f1; if(( unsigned int) _temp404 > 1u?*(( int*) _temp404) == Cyc_Absyn_Upper_b:
0){ _LL407: _temp406=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp404)->f1; goto _LL399;} else{ goto _LL400;}} else{ goto _LL400;} _LL400:
goto _LL401; _LL397: return t1; _LL399: { unsigned int ub_v= Cyc_Evexp_eval_const_uint_exp(
_temp406); _temp387=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( _temp387);{ int is_nullable;{ void* _temp408=( void*)
_temp387->v; int _temp414; _LL410: if(( unsigned int) _temp408 > 1u?*(( int*)
_temp408) == Cyc_Absyn_Eq_constr: 0){ _LL415: _temp414=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp408)->f1; goto _LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL411:
if( _temp414){ struct Cyc_Absyn_PointerType_struct* new_t1=({ struct Cyc_Absyn_PointerType_struct*
_temp418=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp418[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp419; _temp419.tag= Cyc_Absyn_PointerType;
_temp419.f1=({ struct Cyc_Absyn_PtrInfo _temp420; _temp420.elt_typ=( void*)
_temp391; _temp420.rgn_typ=( void*) _temp389; _temp420.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp420.tq= _temp385; _temp420.bounds=
_temp383; _temp420;}); _temp419;}); _temp418;}); Cyc_Tcutil_warn( e1->loc,({
struct _tagged_string _temp416= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp417= Cyc_Absynpp_typ2string(( void*) new_t1); xprintf("coercing from %.*s to %.*s",
_temp416.last_plus_one - _temp416.curr, _temp416.curr, _temp417.last_plus_one -
_temp417.curr, _temp417.curr);})); Cyc_Tcutil_unchecked_cast( te, e1,( void*)
new_t1);} goto _LL409; _LL413:( void*)( _temp387->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp421=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp421[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp422; _temp422.tag= Cyc_Absyn_Eq_constr; _temp422.f1=(
void*) 0; _temp422;}); _temp421;}))); goto _LL409; _LL409:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int e2_v= Cyc_Evexp_eval_const_uint_exp( e2); if( ub_v < e2_v){
e2_v= ub_v; Cyc_Tcutil_warn( e1->loc, xprintf("pointer arithmetic is out of bounds"));}{
struct Cyc_Absyn_Exp* new_bound= Cyc_Absyn_uint_exp( ub_v - e2_v, 0); new_bound->topt=({
struct Cyc_Core_Opt* _temp423=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp423->v=( void*) Cyc_Absyn_uint_t; _temp423;}); return( void*)({
struct Cyc_Absyn_PointerType_struct* _temp424=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp424[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp425; _temp425.tag= Cyc_Absyn_PointerType;
_temp425.f1=({ struct Cyc_Absyn_PtrInfo _temp426; _temp426.elt_typ=( void*)
_temp391; _temp426.rgn_typ=( void*) _temp389; _temp426.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp426.tq= _temp385; _temp426.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp427=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp427[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp428; _temp428.tag= Cyc_Absyn_Upper_b; _temp428.f1= new_bound; _temp428;});
_temp427;})); _temp426;}); _temp425;}); _temp424;});}} else{ Cyc_Tcutil_terr( e1->loc,(
struct _tagged_string)({ char* _temp429=( char*)"cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.";
struct _tagged_string _temp430; _temp430.curr= _temp429; _temp430.base= _temp429;
_temp430.last_plus_one= _temp429 + 98; _temp430;})); return t1;}}} _LL401:( void*)(
_temp383->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp431=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp431[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp432; _temp432.tag= Cyc_Absyn_Eq_constr;
_temp432.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp432;}); _temp431;})));
return t1; _LL395:;} _LL380: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL376:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp442=
e1->topt; if( _temp442 == 0){ _throw( Null_Exception);} _temp442->v;}); void* t2=(
void*)({ struct Cyc_Core_Opt* _temp441= e2->topt; if( _temp441 == 0){ _throw(
Null_Exception);} _temp441->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){
if( Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){
Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp437= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp435= e1->topt; if( _temp435 == 0){ _throw(
Null_Exception);} _temp435->v;})); struct _tagged_string _temp438= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp436= e2->topt; if( _temp436 == 0){ _throw(
Null_Exception);} _temp436->v;})); xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp437.last_plus_one - _temp437.curr, _temp437.curr, _temp438.last_plus_one -
_temp438.curr, _temp438.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp439= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp440= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp439.last_plus_one - _temp439.curr, _temp439.curr, _temp440.last_plus_one -
_temp440.curr, _temp440.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp483= e1->topt; if( _temp483 == 0){ _throw(
Null_Exception);} _temp483->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp482= e2->topt; if( _temp482 == 0){ _throw(
Null_Exception);} _temp482->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp481= e1->topt; if( _temp481 == 0){ _throw( Null_Exception);} _temp481->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp480= e2->topt; if( _temp480 == 0){
_throw( Null_Exception);} _temp480->v;});{ struct _tuple0 _temp444=({ struct
_tuple0 _temp443; _temp443.f1= Cyc_Tcutil_compress( t1); _temp443.f2= Cyc_Tcutil_compress(
t2); _temp443;}); void* _temp450; struct Cyc_Absyn_PtrInfo _temp452; struct Cyc_Absyn_Conref*
_temp454; struct Cyc_Absyn_Tqual _temp456; struct Cyc_Absyn_Conref* _temp458;
void* _temp460; void* _temp462; void* _temp464; struct Cyc_Absyn_PtrInfo
_temp466; struct Cyc_Absyn_Conref* _temp468; struct Cyc_Absyn_Tqual _temp470;
struct Cyc_Absyn_Conref* _temp472; void* _temp474; void* _temp476; _LL446:
_LL465: _temp464= _temp444.f1; if(( unsigned int) _temp464 > 4u?*(( int*)
_temp464) == Cyc_Absyn_PointerType: 0){ _LL467: _temp466=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp464)->f1; _LL477: _temp476=( void*)
_temp466.elt_typ; goto _LL475; _LL475: _temp474=( void*) _temp466.rgn_typ; goto
_LL473; _LL473: _temp472=( struct Cyc_Absyn_Conref*) _temp466.nullable; goto
_LL471; _LL471: _temp470=( struct Cyc_Absyn_Tqual) _temp466.tq; goto _LL469;
_LL469: _temp468=( struct Cyc_Absyn_Conref*) _temp466.bounds; goto _LL451;}
else{ goto _LL448;} _LL451: _temp450= _temp444.f2; if(( unsigned int) _temp450 >
4u?*(( int*) _temp450) == Cyc_Absyn_PointerType: 0){ _LL453: _temp452=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp450)->f1; _LL463:
_temp462=( void*) _temp452.elt_typ; goto _LL461; _LL461: _temp460=( void*)
_temp452.rgn_typ; goto _LL459; _LL459: _temp458=( struct Cyc_Absyn_Conref*)
_temp452.nullable; goto _LL457; _LL457: _temp456=( struct Cyc_Absyn_Tqual)
_temp452.tq; goto _LL455; _LL455: _temp454=( struct Cyc_Absyn_Conref*) _temp452.bounds;
goto _LL447;} else{ goto _LL448;} _LL448: goto _LL449; _LL447: if( Cyc_Tcutil_unify(
_temp476, _temp462)){ return Cyc_Absyn_sint_t;} goto _LL445; _LL449: goto _LL445;
_LL445:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp478= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp479= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp478.last_plus_one - _temp478.curr, _temp478.curr, _temp479.last_plus_one -
_temp479.curr, _temp479.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp484= p; _LL486: if( _temp484 ==( void*) Cyc_Absyn_Plus){ goto _LL487;}
else{ goto _LL488;} _LL488: if( _temp484 ==( void*) Cyc_Absyn_Minus){ goto
_LL489;} else{ goto _LL490;} _LL490: if( _temp484 ==( void*) Cyc_Absyn_Times){
goto _LL491;} else{ goto _LL492;} _LL492: if( _temp484 ==( void*) Cyc_Absyn_Div){
goto _LL493;} else{ goto _LL494;} _LL494: if( _temp484 ==( void*) Cyc_Absyn_Mod){
goto _LL495;} else{ goto _LL496;} _LL496: if( _temp484 ==( void*) Cyc_Absyn_Bitand){
goto _LL497;} else{ goto _LL498;} _LL498: if( _temp484 ==( void*) Cyc_Absyn_Bitor){
goto _LL499;} else{ goto _LL500;} _LL500: if( _temp484 ==( void*) Cyc_Absyn_Bitxor){
goto _LL501;} else{ goto _LL502;} _LL502: if( _temp484 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL503;} else{ goto _LL504;} _LL504: if( _temp484 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL505;} else{ goto _LL506;} _LL506: if( _temp484 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL507;} else{ goto _LL508;} _LL508: if( _temp484 ==( void*) Cyc_Absyn_Eq){
goto _LL509;} else{ goto _LL510;} _LL510: if( _temp484 ==( void*) Cyc_Absyn_Neq){
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp484 ==( void*) Cyc_Absyn_Gt){
goto _LL513;} else{ goto _LL514;} _LL514: if( _temp484 ==( void*) Cyc_Absyn_Lt){
goto _LL515;} else{ goto _LL516;} _LL516: if( _temp484 ==( void*) Cyc_Absyn_Gte){
goto _LL517;} else{ goto _LL518;} _LL518: if( _temp484 ==( void*) Cyc_Absyn_Lte){
goto _LL519;} else{ goto _LL520;} _LL520: goto _LL521; _LL487: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL489: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL491: goto _LL493;
_LL493: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL495: goto _LL497; _LL497: goto _LL499; _LL499: goto _LL501; _LL501: goto
_LL503; _LL503: goto _LL505; _LL505: goto _LL507; _LL507: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL509: goto _LL511; _LL511: goto _LL513;
_LL513: goto _LL515; _LL515: goto _LL517; _LL517: goto _LL519; _LL519: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL521:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp522=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp522[ 0]=({ struct Cyc_Core_Failure_struct
_temp523; _temp523.tag= Cyc_Core_Failure; _temp523.f1=( struct _tagged_string)({
char* _temp524=( char*)"bad binary primop"; struct _tagged_string _temp525;
_temp525.curr= _temp524; _temp525.base= _temp524; _temp525.last_plus_one=
_temp524 + 18; _temp525;}); _temp523;}); _temp522;})); _LL485:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp526= es; if(
_temp526 == 0){ _throw( Null_Exception);} _temp526->hd;}));} Cyc_Tcexp_tcExpList(
te, es);{ void* t;{ void* _temp527= p; _LL529: if( _temp527 ==( void*) Cyc_Absyn_Printf){
goto _LL530;} else{ goto _LL531;} _LL531: if( _temp527 ==( void*) Cyc_Absyn_Fprintf){
goto _LL532;} else{ goto _LL533;} _LL533: if( _temp527 ==( void*) Cyc_Absyn_Xprintf){
goto _LL534;} else{ goto _LL535;} _LL535: if( _temp527 ==( void*) Cyc_Absyn_Scanf){
goto _LL536;} else{ goto _LL537;} _LL537: if( _temp527 ==( void*) Cyc_Absyn_Fscanf){
goto _LL538;} else{ goto _LL539;} _LL539: if( _temp527 ==( void*) Cyc_Absyn_Sscanf){
goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL530: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp543=(
char*)"missing format string in printf"; struct _tagged_string _temp544;
_temp544.curr= _temp543; _temp544.base= _temp543; _temp544.last_plus_one=
_temp543 + 32; _temp544;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp545= es; if( _temp545 == 0){ _throw( Null_Exception);}
_temp545->hd;}),({ struct Cyc_List_List* _temp546= es; if( _temp546 == 0){
_throw( Null_Exception);} _temp546->tl;}), Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL528; _LL532: if( es == 0? 1:({ struct Cyc_List_List*
_temp547= es; if( _temp547 == 0){ _throw( Null_Exception);} _temp547->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp548=(
char*)"too few arguments to fprintf"; struct _tagged_string _temp549; _temp549.curr=
_temp548; _temp549.base= _temp548; _temp549.last_plus_one= _temp548 + 29;
_temp549;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp551=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp550= es; if( _temp550 == 0){
_throw( Null_Exception);} _temp550->hd;}))->topt; if( _temp551 == 0){ _throw(
Null_Exception);} _temp551->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp552= es; if( _temp552 == 0){
_throw( Null_Exception);} _temp552->hd;}))->loc,({ struct _tagged_string
_temp555= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp554=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp553= es; if( _temp553 == 0){
_throw( Null_Exception);} _temp553->hd;}))->topt; if( _temp554 == 0){ _throw(
Null_Exception);} _temp554->v;})); xprintf("expecting FILE @ but found %.*s",
_temp555.last_plus_one - _temp555.curr, _temp555.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp557=({ struct Cyc_List_List*
_temp556= es; if( _temp556 == 0){ _throw( Null_Exception);} _temp556->tl;}); if(
_temp557 == 0){ _throw( Null_Exception);} _temp557->hd;}),({ struct Cyc_List_List*
_temp559=({ struct Cyc_List_List* _temp558= es; if( _temp558 == 0){ _throw(
Null_Exception);} _temp558->tl;}); if( _temp559 == 0){ _throw( Null_Exception);}
_temp559->tl;}), Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL528; _LL534: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp560=( char*)"missing format string in xprintf";
struct _tagged_string _temp561; _temp561.curr= _temp560; _temp561.base= _temp560;
_temp561.last_plus_one= _temp560 + 33; _temp561;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp562= es; if( _temp562
== 0){ _throw( Null_Exception);} _temp562->hd;}),({ struct Cyc_List_List*
_temp563= es; if( _temp563 == 0){ _throw( Null_Exception);} _temp563->tl;}), Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL528; _LL536: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp564=( char*)"missing format string in scanf";
struct _tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 31; _temp565;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp566= es; if( _temp566
== 0){ _throw( Null_Exception);} _temp566->hd;}),({ struct Cyc_List_List*
_temp567= es; if( _temp567 == 0){ _throw( Null_Exception);} _temp567->tl;}), Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL528; _LL538: if( es == 0? 1:({ struct Cyc_List_List*
_temp568= es; if( _temp568 == 0){ _throw( Null_Exception);} _temp568->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp569=(
char*)"too few arguments to fscanf"; struct _tagged_string _temp570; _temp570.curr=
_temp569; _temp570.base= _temp569; _temp570.last_plus_one= _temp569 + 28;
_temp570;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp572=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp571= es; if( _temp571 == 0){
_throw( Null_Exception);} _temp571->hd;}))->topt; if( _temp572 == 0){ _throw(
Null_Exception);} _temp572->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp573= es; if( _temp573 == 0){
_throw( Null_Exception);} _temp573->hd;}))->loc,({ struct _tagged_string
_temp576= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp575=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp574= es; if( _temp574 == 0){
_throw( Null_Exception);} _temp574->hd;}))->topt; if( _temp575 == 0){ _throw(
Null_Exception);} _temp575->v;})); xprintf("expecting FILE @ but found %.*s",
_temp576.last_plus_one - _temp576.curr, _temp576.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp578=({ struct Cyc_List_List*
_temp577= es; if( _temp577 == 0){ _throw( Null_Exception);} _temp577->tl;}); if(
_temp578 == 0){ _throw( Null_Exception);} _temp578->hd;}),({ struct Cyc_List_List*
_temp580=({ struct Cyc_List_List* _temp579= es; if( _temp579 == 0){ _throw(
Null_Exception);} _temp579->tl;}); if( _temp580 == 0){ _throw( Null_Exception);}
_temp580->tl;}), Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL528;
_LL540: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp581=( char*)"sscanf is not supported";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 24; _temp582;})); t= Cyc_Absyn_sint_t; goto
_LL528; _LL542: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL583: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp585=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 35; _temp586;})); case 1: _LL584: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp588= es;
if( _temp588 == 0){ _throw( Null_Exception);} _temp588->hd;})); break; case 2:
_LL587: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp590= es; if( _temp590 == 0){ _throw( Null_Exception);}
_temp590->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp592=({
struct Cyc_List_List* _temp591= es; if( _temp591 == 0){ _throw( Null_Exception);}
_temp591->tl;}); if( _temp592 == 0){ _throw( Null_Exception);} _temp592->hd;}));
break; default: _LL589: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp594=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp595; _temp595.curr= _temp594; _temp595.base= _temp594;
_temp595.last_plus_one= _temp594 + 37; _temp595;}));} goto _LL528; _LL528:;}
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp596=( void*) e->r; void* _temp616; struct Cyc_Absyn_Vardecl* _temp618;
struct _tuple1* _temp620; void* _temp622; struct Cyc_Absyn_Vardecl* _temp624;
struct _tuple1* _temp626; void* _temp628; struct Cyc_Absyn_Vardecl* _temp630;
struct _tuple1* _temp632; void* _temp634; struct Cyc_Absyn_Vardecl* _temp636;
struct _tuple1* _temp638; struct Cyc_Absyn_Exp* _temp640; struct Cyc_Absyn_Exp*
_temp642; struct _tagged_string* _temp644; struct Cyc_Absyn_Exp* _temp646;
struct _tagged_string* _temp648; struct Cyc_Absyn_Exp* _temp650; struct Cyc_Absyn_Exp*
_temp652; _LL598: if(*(( int*) _temp596) == Cyc_Absyn_Var_e){ _LL621: _temp620=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp596)->f1; goto _LL617;
_LL617: _temp616=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp596)->f2; if((
unsigned int) _temp616 > 1u?*(( int*) _temp616) == Cyc_Absyn_Param_b: 0){ _LL619:
_temp618=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp616)->f1; goto _LL599;} else{ goto _LL600;}} else{ goto _LL600;} _LL600:
if(*(( int*) _temp596) == Cyc_Absyn_Var_e){ _LL627: _temp626=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp596)->f1; goto _LL623; _LL623: _temp622=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp596)->f2; if(( unsigned int)
_temp622 > 1u?*(( int*) _temp622) == Cyc_Absyn_Local_b: 0){ _LL625: _temp624=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp622)->f1;
goto _LL601;} else{ goto _LL602;}} else{ goto _LL602;} _LL602: if(*(( int*)
_temp596) == Cyc_Absyn_Var_e){ _LL633: _temp632=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp596)->f1; goto _LL629; _LL629: _temp628=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp596)->f2; if(( unsigned int) _temp628 > 1u?*(( int*) _temp628) == Cyc_Absyn_Pat_b:
0){ _LL631: _temp630=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp628)->f1; goto _LL603;} else{ goto _LL604;}} else{ goto _LL604;} _LL604:
if(*(( int*) _temp596) == Cyc_Absyn_Var_e){ _LL639: _temp638=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp596)->f1; goto _LL635; _LL635: _temp634=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp596)->f2; if(( unsigned int)
_temp634 > 1u?*(( int*) _temp634) == Cyc_Absyn_Global_b: 0){ _LL637: _temp636=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp634)->f1;
goto _LL605;} else{ goto _LL606;}} else{ goto _LL606;} _LL606: if(*(( int*)
_temp596) == Cyc_Absyn_Subscript_e){ _LL643: _temp642=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp596)->f1; goto _LL641; _LL641:
_temp640=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp596)->f2; goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp596)
== Cyc_Absyn_StructMember_e){ _LL647: _temp646=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp596)->f1; goto _LL645; _LL645: _temp644=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp596)->f2;
goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp596) == Cyc_Absyn_StructArrow_e){
_LL651: _temp650=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp596)->f1; goto _LL649; _LL649: _temp648=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp596)->f2; goto _LL611;} else{ goto _LL612;}
_LL612: if(*(( int*) _temp596) == Cyc_Absyn_Deref_e){ _LL653: _temp652=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp596)->f1; goto _LL613;}
else{ goto _LL614;} _LL614: goto _LL615; _LL599: _temp624= _temp618; goto _LL601;
_LL601: _temp630= _temp624; goto _LL603; _LL603: _temp636= _temp630; goto _LL605;
_LL605: if( !( _temp636->tq).q_const){ return;} goto _LL597; _LL607:{ void*
_temp655= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp654= _temp642->topt;
if( _temp654 == 0){ _throw( Null_Exception);} _temp654->v;})); struct Cyc_Absyn_PtrInfo
_temp665; struct Cyc_Absyn_Conref* _temp667; struct Cyc_Absyn_Tqual _temp669;
struct Cyc_Absyn_Conref* _temp671; void* _temp673; void* _temp675; struct Cyc_Absyn_Exp*
_temp677; struct Cyc_Absyn_Tqual _temp679; void* _temp681; struct Cyc_List_List*
_temp683; _LL657: if(( unsigned int) _temp655 > 4u?*(( int*) _temp655) == Cyc_Absyn_PointerType:
0){ _LL666: _temp665=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp655)->f1; _LL676: _temp675=( void*) _temp665.elt_typ; goto _LL674; _LL674:
_temp673=( void*) _temp665.rgn_typ; goto _LL672; _LL672: _temp671=( struct Cyc_Absyn_Conref*)
_temp665.nullable; goto _LL670; _LL670: _temp669=( struct Cyc_Absyn_Tqual)
_temp665.tq; goto _LL668; _LL668: _temp667=( struct Cyc_Absyn_Conref*) _temp665.bounds;
goto _LL658;} else{ goto _LL659;} _LL659: if(( unsigned int) _temp655 > 4u?*((
int*) _temp655) == Cyc_Absyn_ArrayType: 0){ _LL682: _temp681=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp655)->f1; goto _LL680; _LL680: _temp679=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp655)->f2;
goto _LL678; _LL678: _temp677=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp655)->f3; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp655 > 4u?*(( int*) _temp655) == Cyc_Absyn_TupleType: 0){ _LL684: _temp683=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp655)->f1; goto
_LL662;} else{ goto _LL663;} _LL663: goto _LL664; _LL658: _temp679= _temp669;
goto _LL660; _LL660: if( ! _temp679.q_const){ return;} goto _LL656; _LL662: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp640);{ struct _handler_cons
_temp685; _push_handler(& _temp685);{ void* _temp686=( void*) setjmp( _temp685.handler);
if( ! _temp686){{ struct _tuple7 _temp689; void* _temp690; struct Cyc_Absyn_Tqual
_temp692; struct _tuple7* _temp687=(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( _temp683,( int) i); _temp689=* _temp687; _LL693:
_temp692= _temp689.f1; goto _LL691; _LL691: _temp690= _temp689.f2; goto _LL688;
_LL688: if( ! _temp692.q_const){ _npop_handler( 0u); return;}}; _pop_handler();}
else{ void* _temp695= _temp686; _LL697: if( _temp695 == Cyc_List_Nth){ goto
_LL698;} else{ goto _LL699;} _LL699: goto _LL700; _LL698: return; _LL700:( void)
_throw( _temp695); _LL696:;}}} goto _LL656;} _LL664: goto _LL656; _LL656:;} goto
_LL597; _LL609:{ void* _temp702= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp701= _temp646->topt; if( _temp701 == 0){ _throw( Null_Exception);} _temp701->v;}));
struct Cyc_Absyn_Structdecl** _temp714; struct Cyc_List_List* _temp716; struct
_tuple1* _temp718; struct Cyc_List_List* _temp720; struct Cyc_List_List*
_temp722; struct Cyc_Absyn_Uniondecl** _temp724; struct Cyc_List_List* _temp726;
struct _tuple1* _temp728; _LL704: if(( unsigned int) _temp702 > 4u?*(( int*)
_temp702) == Cyc_Absyn_StructType: 0){ _LL719: _temp718=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp702)->f1; goto _LL717; _LL717:
_temp716=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp702)->f2; goto _LL715; _LL715: _temp714=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp702)->f3; goto _LL705;} else{ goto
_LL706;} _LL706: if(( unsigned int) _temp702 > 4u?*(( int*) _temp702) == Cyc_Absyn_AnonUnionType:
0){ _LL721: _temp720=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp702)->f1; goto _LL707;} else{ goto _LL708;} _LL708: if(( unsigned int)
_temp702 > 4u?*(( int*) _temp702) == Cyc_Absyn_AnonStructType: 0){ _LL723:
_temp722=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp702)->f1; goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int)
_temp702 > 4u?*(( int*) _temp702) == Cyc_Absyn_UnionType: 0){ _LL729: _temp728=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp702)->f1; goto
_LL727; _LL727: _temp726=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp702)->f2; goto _LL725; _LL725: _temp724=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp702)->f3; goto _LL711;} else{ goto
_LL712;} _LL712: goto _LL713; _LL705: { struct Cyc_Absyn_Structfield* sf=
_temp714 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp731= _temp714; if( _temp731 == 0){ _throw(
Null_Exception);}* _temp731;}), _temp644); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp730= sf; if( _temp730 == 0){ _throw( Null_Exception);} _temp730->tq;}).q_const){
return;} goto _LL703;} _LL707: _temp722= _temp720; goto _LL709; _LL709: { struct
Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field( _temp722, _temp644); if( sf
== 0? 1: !({ struct Cyc_Absyn_Structfield* _temp732= sf; if( _temp732 == 0){
_throw( Null_Exception);} _temp732->tq;}).q_const){ return;} goto _LL703;}
_LL711: { struct Cyc_Absyn_Structfield* sf= _temp724 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)({ struct Cyc_Absyn_Uniondecl** _temp734= _temp724;
if( _temp734 == 0){ _throw( Null_Exception);}* _temp734;}), _temp644); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp733= sf; if( _temp733 == 0){ _throw(
Null_Exception);} _temp733->tq;}).q_const){ return;} goto _LL703;} _LL713: goto
_LL703; _LL703:;} goto _LL597; _LL611:{ void* _temp736= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp735= _temp650->topt; if( _temp735 == 0){
_throw( Null_Exception);} _temp735->v;})); struct Cyc_Absyn_PtrInfo _temp742;
struct Cyc_Absyn_Conref* _temp744; struct Cyc_Absyn_Tqual _temp746; struct Cyc_Absyn_Conref*
_temp748; void* _temp750; void* _temp752; _LL738: if(( unsigned int) _temp736 >
4u?*(( int*) _temp736) == Cyc_Absyn_PointerType: 0){ _LL743: _temp742=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp736)->f1; _LL753:
_temp752=( void*) _temp742.elt_typ; goto _LL751; _LL751: _temp750=( void*)
_temp742.rgn_typ; goto _LL749; _LL749: _temp748=( struct Cyc_Absyn_Conref*)
_temp742.nullable; goto _LL747; _LL747: _temp746=( struct Cyc_Absyn_Tqual)
_temp742.tq; goto _LL745; _LL745: _temp744=( struct Cyc_Absyn_Conref*) _temp742.bounds;
goto _LL739;} else{ goto _LL740;} _LL740: goto _LL741; _LL739:{ void* _temp754=
Cyc_Tcutil_compress( _temp752); struct Cyc_Absyn_Structdecl** _temp766; struct
Cyc_List_List* _temp768; struct _tuple1* _temp770; struct Cyc_Absyn_Uniondecl**
_temp772; struct Cyc_List_List* _temp774; struct _tuple1* _temp776; struct Cyc_List_List*
_temp778; struct Cyc_List_List* _temp780; _LL756: if(( unsigned int) _temp754 >
4u?*(( int*) _temp754) == Cyc_Absyn_StructType: 0){ _LL771: _temp770=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp754)->f1; goto _LL769;
_LL769: _temp768=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp754)->f2; goto _LL767; _LL767: _temp766=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp754)->f3; goto _LL757;} else{ goto
_LL758;} _LL758: if(( unsigned int) _temp754 > 4u?*(( int*) _temp754) == Cyc_Absyn_UnionType:
0){ _LL777: _temp776=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp754)->f1; goto _LL775; _LL775: _temp774=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp754)->f2; goto _LL773; _LL773: _temp772=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp754)->f3;
goto _LL759;} else{ goto _LL760;} _LL760: if(( unsigned int) _temp754 > 4u?*((
int*) _temp754) == Cyc_Absyn_AnonStructType: 0){ _LL779: _temp778=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp754)->f1; goto _LL761;} else{ goto
_LL762;} _LL762: if(( unsigned int) _temp754 > 4u?*(( int*) _temp754) == Cyc_Absyn_AnonUnionType:
0){ _LL781: _temp780=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp754)->f1; goto _LL763;} else{ goto _LL764;} _LL764: goto _LL765; _LL757: {
struct Cyc_Absyn_Structfield* sf= _temp766 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl** _temp783= _temp766;
if( _temp783 == 0){ _throw( Null_Exception);}* _temp783;}), _temp648); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp782= sf; if( _temp782 == 0){ _throw(
Null_Exception);} _temp782->tq;}).q_const){ return;} goto _LL755;} _LL759: {
struct Cyc_Absyn_Structfield* sf= _temp772 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)({ struct Cyc_Absyn_Uniondecl** _temp785= _temp772;
if( _temp785 == 0){ _throw( Null_Exception);}* _temp785;}), _temp648); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp784= sf; if( _temp784 == 0){ _throw(
Null_Exception);} _temp784->tq;}).q_const){ return;} goto _LL755;} _LL761:
_temp780= _temp778; goto _LL763; _LL763: { struct Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field(
_temp780, _temp648); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield* _temp786=
sf; if( _temp786 == 0){ _throw( Null_Exception);} _temp786->tq;}).q_const){
return;} goto _LL755;} _LL765: goto _LL755; _LL755:;} goto _LL737; _LL741: goto
_LL737; _LL737:;} goto _LL597; _LL613:{ void* _temp788= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp787= _temp652->topt; if( _temp787 == 0){
_throw( Null_Exception);} _temp787->v;})); struct Cyc_Absyn_PtrInfo _temp796;
struct Cyc_Absyn_Conref* _temp798; struct Cyc_Absyn_Tqual _temp800; struct Cyc_Absyn_Conref*
_temp802; void* _temp804; void* _temp806; struct Cyc_Absyn_Exp* _temp808; struct
Cyc_Absyn_Tqual _temp810; void* _temp812; _LL790: if(( unsigned int) _temp788 >
4u?*(( int*) _temp788) == Cyc_Absyn_PointerType: 0){ _LL797: _temp796=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp788)->f1; _LL807:
_temp806=( void*) _temp796.elt_typ; goto _LL805; _LL805: _temp804=( void*)
_temp796.rgn_typ; goto _LL803; _LL803: _temp802=( struct Cyc_Absyn_Conref*)
_temp796.nullable; goto _LL801; _LL801: _temp800=( struct Cyc_Absyn_Tqual)
_temp796.tq; goto _LL799; _LL799: _temp798=( struct Cyc_Absyn_Conref*) _temp796.bounds;
goto _LL791;} else{ goto _LL792;} _LL792: if(( unsigned int) _temp788 > 4u?*((
int*) _temp788) == Cyc_Absyn_ArrayType: 0){ _LL813: _temp812=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp788)->f1; goto _LL811; _LL811: _temp810=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp788)->f2;
goto _LL809; _LL809: _temp808=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp788)->f3; goto _LL793;} else{ goto _LL794;} _LL794: goto _LL795; _LL791:
_temp810= _temp800; goto _LL793; _LL793: if( ! _temp810.q_const){ return;} goto
_LL789; _LL795: goto _LL789; _LL789:;} goto _LL597; _LL615: goto _LL597; _LL597:;}
Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp814=( char*)"attempt to write a read-only location";
struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 38; _temp815;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp816=( char*)"increment/decrement of non-lvalue"; struct _tagged_string
_temp817; _temp817.curr= _temp816; _temp817.base= _temp816; _temp817.last_plus_one=
_temp816 + 34; _temp817;}));} Cyc_Tcexp_check_writable( te, e);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp819= e->topt; if( _temp819 == 0){ _throw(
Null_Exception);} _temp819->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp818= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or ? type but found %.*s", _temp818.last_plus_one
- _temp818.curr, _temp818.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp820=( char*)"conditional expression";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 23; _temp821;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp827=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp827->hd=( void*) e3; _temp827->tl=
0; _temp827;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp826=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp826->hd=(
void*) e2; _temp826->tl= l1; _temp826;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp824= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp822= e2->topt; if( _temp822 == 0){ _throw(
Null_Exception);} _temp822->v;})); struct _tagged_string _temp825= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp823= e3->topt; if( _temp823 == 0){ _throw(
Null_Exception);} _temp823->v;})); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp824.last_plus_one - _temp824.curr, _temp824.curr, _temp825.last_plus_one -
_temp825.curr, _temp825.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)({ struct Cyc_Core_Opt*
_temp828= e1->topt; if( _temp828 == 0){ _throw( Null_Exception);}& _temp828->v;})),
e2);{ void* t1=( void*)({ struct Cyc_Core_Opt* _temp851= e1->topt; if( _temp851
== 0){ _throw( Null_Exception);} _temp851->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp850= e2->topt; if( _temp850 == 0){ _throw( Null_Exception);} _temp850->v;});{
void* _temp829= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp835; struct
Cyc_Absyn_Tqual _temp837; void* _temp839; _LL831: if(( unsigned int) _temp829 >
4u?*(( int*) _temp829) == Cyc_Absyn_ArrayType: 0){ _LL840: _temp839=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp829)->f1; goto _LL838; _LL838: _temp837=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp829)->f2;
goto _LL836; _LL836: _temp835=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp829)->f3; goto _LL832;} else{ goto _LL833;} _LL833: goto _LL834; _LL832:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp841=( char*)"cannot assign to an array";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 26; _temp842;})); goto _LL830; _LL834: goto
_LL830; _LL830:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp843=( char*)"assignment to non-lvalue";
struct _tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 25; _temp844;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp845= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp846= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp845.last_plus_one - _temp845.curr, _temp845.curr, _temp846.last_plus_one -
_temp846.curr, _temp846.curr);}));}} else{ void* p=( void*)({ struct Cyc_Core_Opt*
_temp849= po; if( _temp849 == 0){ _throw( Null_Exception);} _temp849->v;}); void*
t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0, p, e1, e2); if( !( Cyc_Tcutil_unify(
t_result, t1)? 1: Cyc_Tcutil_coerceable( t_result))){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp847=( char*)"Cannot use this operator in an assignment";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 42; _temp848;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp852= e2->topt; if( _temp852 == 0){ _throw( Null_Exception);} _temp852->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp910= e->topt; if( _temp910 == 0){ _throw( Null_Exception);} _temp910->v;}));
void* _temp853= t; struct Cyc_Absyn_PtrInfo _temp859; struct Cyc_Absyn_Conref*
_temp861; struct Cyc_Absyn_Tqual _temp863; struct Cyc_Absyn_Conref* _temp865;
void* _temp867; void* _temp869; _LL855: if(( unsigned int) _temp853 > 4u?*(( int*)
_temp853) == Cyc_Absyn_PointerType: 0){ _LL860: _temp859=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp853)->f1; _LL870: _temp869=( void*)
_temp859.elt_typ; goto _LL868; _LL868: _temp867=( void*) _temp859.rgn_typ; goto
_LL866; _LL866: _temp865=( struct Cyc_Absyn_Conref*) _temp859.nullable; goto
_LL864; _LL864: _temp863=( struct Cyc_Absyn_Tqual) _temp859.tq; goto _LL862;
_LL862: _temp861=( struct Cyc_Absyn_Conref*) _temp859.bounds; goto _LL856;}
else{ goto _LL857;} _LL857: goto _LL858; _LL856: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp867); Cyc_Tcutil_check_nonzero_bound( loc, _temp861);{ void*
_temp871= Cyc_Tcutil_compress( _temp869); struct Cyc_Absyn_FnInfo _temp877;
struct Cyc_List_List* _temp879; int _temp881; struct Cyc_List_List* _temp883;
void* _temp885; struct Cyc_Core_Opt* _temp887; struct Cyc_List_List* _temp889;
_LL873: if(( unsigned int) _temp871 > 4u?*(( int*) _temp871) == Cyc_Absyn_FnType:
0){ _LL878: _temp877=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp871)->f1; _LL890: _temp889=( struct Cyc_List_List*) _temp877.tvars; goto
_LL888; _LL888: _temp887=( struct Cyc_Core_Opt*) _temp877.effect; goto _LL886;
_LL886: _temp885=( void*) _temp877.ret_typ; goto _LL884; _LL884: _temp883=(
struct Cyc_List_List*) _temp877.args; goto _LL882; _LL882: _temp881=( int)
_temp877.varargs; goto _LL880; _LL880: _temp879=( struct Cyc_List_List*)
_temp877.attributes; goto _LL874;} else{ goto _LL875;} _LL875: goto _LL876;
_LL874: if( _temp881){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp891=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 41; _temp892;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp885,({ void** _temp893= topt; if( _temp893 == 0){ _throw( Null_Exception);}*
_temp893;}));} while( es != 0? _temp883 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp900= es; if( _temp900 == 0){
_throw( Null_Exception);} _temp900->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp899= _temp883; if( _temp899 == 0){ _throw(
Null_Exception);} _temp899->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp895= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp894= e1->topt; if( _temp894 == 0){ _throw( Null_Exception);} _temp894->v;}));
struct _tagged_string _temp896= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp895.last_plus_one - _temp895.curr, _temp895.curr, _temp896.last_plus_one -
_temp896.curr, _temp896.curr);}));} es=({ struct Cyc_List_List* _temp897= es;
if( _temp897 == 0){ _throw( Null_Exception);} _temp897->tl;}); _temp883=({
struct Cyc_List_List* _temp898= _temp883; if( _temp898 == 0){ _throw(
Null_Exception);} _temp898->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp901=( char*)"too many arguments for function";
struct _tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 32; _temp902;}));} if( _temp883 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp903=( char*)"too few arguments for function";
struct _tagged_string _temp904; _temp904.curr= _temp903; _temp904.base= _temp903;
_temp904.last_plus_one= _temp903 + 31; _temp904;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp905= _temp887; if( _temp905 == 0){
_throw( Null_Exception);} _temp905->v;})); return _temp885; _LL876: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp906=( char*)"expected pointer to function";
struct _tagged_string _temp907; _temp907.curr= _temp906; _temp907.base= _temp906;
_temp907.last_plus_one= _temp906 + 29; _temp907;})); _LL872:;} _LL858: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp908=( char*)"expected pointer to function";
struct _tagged_string _temp909; _temp909.curr= _temp908; _temp909.base= _temp908;
_temp909.last_plus_one= _temp908 + 29; _temp909;})); _LL854:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp912= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp911= e->topt; if( _temp911 == 0){ _throw( Null_Exception);} _temp911->v;}));
xprintf("expected xtunion exn but found %.*s", _temp912.last_plus_one - _temp912.curr,
_temp912.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp913= e->topt; if( _temp913 == 0){ _throw(
Null_Exception);} _temp913->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp914= e->topt; if( _temp914 == 0){ _throw(
Null_Exception);} _temp914->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp970= e->topt; if( _temp970 == 0){ _throw(
Null_Exception);} _temp970->v;}));{ void* _temp915= t1; struct Cyc_Absyn_PtrInfo
_temp921; struct Cyc_Absyn_Conref* _temp923; struct Cyc_Absyn_Tqual _temp925;
struct Cyc_Absyn_Conref* _temp927; void* _temp929; void* _temp931; _LL917: if((
unsigned int) _temp915 > 4u?*(( int*) _temp915) == Cyc_Absyn_PointerType: 0){
_LL922: _temp921=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp915)->f1; _LL932: _temp931=( void*) _temp921.elt_typ; goto _LL930; _LL930:
_temp929=( void*) _temp921.rgn_typ; goto _LL928; _LL928: _temp927=( struct Cyc_Absyn_Conref*)
_temp921.nullable; goto _LL926; _LL926: _temp925=( struct Cyc_Absyn_Tqual)
_temp921.tq; goto _LL924; _LL924: _temp923=( struct Cyc_Absyn_Conref*) _temp921.bounds;
goto _LL918;} else{ goto _LL919;} _LL919: goto _LL920; _LL918:{ void* _temp933=
Cyc_Tcutil_compress( _temp931); struct Cyc_Absyn_FnInfo _temp939; struct Cyc_List_List*
_temp941; int _temp943; struct Cyc_List_List* _temp945; void* _temp947; struct
Cyc_Core_Opt* _temp949; struct Cyc_List_List* _temp951; _LL935: if((
unsigned int) _temp933 > 4u?*(( int*) _temp933) == Cyc_Absyn_FnType: 0){ _LL940:
_temp939=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp933)->f1;
_LL952: _temp951=( struct Cyc_List_List*) _temp939.tvars; goto _LL950; _LL950:
_temp949=( struct Cyc_Core_Opt*) _temp939.effect; goto _LL948; _LL948: _temp947=(
void*) _temp939.ret_typ; goto _LL946; _LL946: _temp945=( struct Cyc_List_List*)
_temp939.args; goto _LL944; _LL944: _temp943=( int) _temp939.varargs; goto
_LL942; _LL942: _temp941=( struct Cyc_List_List*) _temp939.attributes; goto
_LL936;} else{ goto _LL937;} _LL937: goto _LL938; _LL936: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp951)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp953=( char*)"type instantiation mismatch";
struct _tagged_string _temp954; _temp954.curr= _temp953; _temp954.base= _temp953;
_temp954.last_plus_one= _temp953 + 28; _temp954;}));} for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp955= ts; if( _temp955 == 0){ _throw( Null_Exception);}
_temp955->tl;}), _temp951=({ struct Cyc_List_List* _temp956= _temp951; if(
_temp956 == 0){ _throw( Null_Exception);} _temp956->tl;})){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp962= _temp951; if( _temp962 == 0){ _throw(
Null_Exception);} _temp962->hd;}))->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)({ struct Cyc_List_List* _temp957= ts; if( _temp957 == 0){ _throw(
Null_Exception);} _temp957->hd;})); instantiation=({ struct Cyc_List_List*
_temp958=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp958->hd=( void*)({ struct _tuple4* _temp959=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp959->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp961= _temp951; if( _temp961 == 0){ _throw( Null_Exception);} _temp961->hd;});
_temp959->f2=( void*)({ struct Cyc_List_List* _temp960= ts; if( _temp960 == 0){
_throw( Null_Exception);} _temp960->hd;}); _temp959;}); _temp958->tl=
instantiation; _temp958;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp966=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp966[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp967; _temp967.tag= Cyc_Absyn_FnType; _temp967.f1=({ struct Cyc_Absyn_FnInfo
_temp968; _temp968.tvars= 0; _temp968.effect= _temp949; _temp968.ret_typ=( void*)
_temp947; _temp968.args= _temp945; _temp968.varargs= _temp943; _temp968.attributes=
_temp941; _temp968;}); _temp967;}); _temp966;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp963=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp963[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp964; _temp964.tag= Cyc_Absyn_PointerType;
_temp964.f1=({ struct Cyc_Absyn_PtrInfo _temp965; _temp965.elt_typ=( void*)
new_fn_typ; _temp965.rgn_typ=( void*) _temp929; _temp965.nullable= _temp927;
_temp965.tq= _temp925; _temp965.bounds= _temp923; _temp965;}); _temp964;});
_temp963;}); return new_typ;}} _LL938: goto _LL934; _LL934:;} goto _LL916;
_LL920: goto _LL916; _LL916:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp969= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp969.last_plus_one - _temp969.curr, _temp969.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp973= e->topt; if( _temp973 == 0){ _throw(
Null_Exception);} _temp973->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp971= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp972= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp971.last_plus_one - _temp971.curr, _temp971.curr, _temp972.last_plus_one -
_temp972.curr, _temp972.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** topt2= 0; struct Cyc_Absyn_Tqual tq2= Cyc_Absyn_empty_tqual();
if( topt != 0){ void* _temp975= Cyc_Tcutil_compress(({ void** _temp974= topt;
if( _temp974 == 0){ _throw( Null_Exception);}* _temp974;})); struct Cyc_Absyn_PtrInfo
_temp981; struct Cyc_Absyn_Conref* _temp983; struct Cyc_Absyn_Tqual _temp985;
struct Cyc_Absyn_Conref* _temp987; void* _temp989; void* _temp991; _LL977: if((
unsigned int) _temp975 > 4u?*(( int*) _temp975) == Cyc_Absyn_PointerType: 0){
_LL982: _temp981=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp975)->f1; _LL992: _temp991=( void*) _temp981.elt_typ; goto _LL990; _LL990:
_temp989=( void*) _temp981.rgn_typ; goto _LL988; _LL988: _temp987=( struct Cyc_Absyn_Conref*)
_temp981.nullable; goto _LL986; _LL986: _temp985=( struct Cyc_Absyn_Tqual)
_temp981.tq; goto _LL984; _LL984: _temp983=( struct Cyc_Absyn_Conref*) _temp981.bounds;
goto _LL978;} else{ goto _LL979;} _LL979: goto _LL980; _LL978: topt2=({ void**
_temp993=( void**) GC_malloc( sizeof( void*)); _temp993[ 0]= _temp991; _temp993;});
tq2= _temp985; goto _LL976; _LL980: goto _LL976; _LL976:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp994=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp1002; struct Cyc_List_List* _temp1004; struct Cyc_Core_Opt* _temp1006;
struct _tuple1* _temp1008; struct Cyc_List_List* _temp1010; _LL996: if(*(( int*)
_temp994) == Cyc_Absyn_Struct_e){ _LL1009: _temp1008=( struct _tuple1*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp994)->f1; goto _LL1007; _LL1007: _temp1006=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp994)->f2; goto
_LL1005; _LL1005: _temp1004=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp994)->f3; goto _LL1003; _LL1003: _temp1002=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp994)->f4; goto _LL997;} else{ goto
_LL998;} _LL998: if(*(( int*) _temp994) == Cyc_Absyn_Tuple_e){ _LL1011:
_temp1010=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp994)->f1;
goto _LL999;} else{ goto _LL1000;} _LL1000: goto _LL1001; _LL997: goto _LL999;
_LL999: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1012=( char*)"& used to allocate";
struct _tagged_string _temp1013; _temp1013.curr= _temp1012; _temp1013.base=
_temp1012; _temp1013.last_plus_one= _temp1012 + 19; _temp1013;}));{ void*(*
ptr_maker)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
if( topt != 0){ void* _temp1015= Cyc_Tcutil_compress(({ void** _temp1014= topt;
if( _temp1014 == 0){ _throw( Null_Exception);}* _temp1014;})); struct Cyc_Absyn_PtrInfo
_temp1021; struct Cyc_Absyn_Conref* _temp1023; struct Cyc_Absyn_Tqual _temp1025;
struct Cyc_Absyn_Conref* _temp1027; void* _temp1029; void* _temp1031; _LL1017:
if(( unsigned int) _temp1015 > 4u?*(( int*) _temp1015) == Cyc_Absyn_PointerType:
0){ _LL1022: _temp1021=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1015)->f1; _LL1032: _temp1031=( void*) _temp1021.elt_typ; goto _LL1030;
_LL1030: _temp1029=( void*) _temp1021.rgn_typ; goto _LL1028; _LL1028: _temp1027=(
struct Cyc_Absyn_Conref*) _temp1021.nullable; goto _LL1026; _LL1026: _temp1025=(
struct Cyc_Absyn_Tqual) _temp1021.tq; goto _LL1024; _LL1024: _temp1023=( struct
Cyc_Absyn_Conref*) _temp1021.bounds; goto _LL1018;} else{ goto _LL1019;} _LL1019:
goto _LL1020; _LL1018: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1027)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL1016; _LL1020: goto _LL1016;
_LL1016:;} return ptr_maker(( void*)({ struct Cyc_Core_Opt* _temp1033= e->topt;
if( _temp1033 == 0){ _throw( Null_Exception);} _temp1033->v;}),( void*) Cyc_Absyn_HeapRgn,
tq2);} _LL1001: goto _LL995; _LL995:;}{ void* _temp1036; int _temp1038; struct
_tuple5 _temp1034= Cyc_Tcutil_addressof_props( te, e); _LL1039: _temp1038=
_temp1034.f1; goto _LL1037; _LL1037: _temp1036= _temp1034.f2; goto _LL1035;
_LL1035: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if( _temp1038){
tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp1040= e->topt; if( _temp1040 == 0){ _throw( Null_Exception);} _temp1040->v;}),
_temp1036, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1060= e->topt; if( _temp1060 == 0){ _throw( Null_Exception);} _temp1060->v;}));
void* _temp1041= t; struct Cyc_Absyn_PtrInfo _temp1047; struct Cyc_Absyn_Conref*
_temp1049; struct Cyc_Absyn_Tqual _temp1051; struct Cyc_Absyn_Conref* _temp1053;
void* _temp1055; void* _temp1057; _LL1043: if(( unsigned int) _temp1041 > 4u?*((
int*) _temp1041) == Cyc_Absyn_PointerType: 0){ _LL1048: _temp1047=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1041)->f1; _LL1058: _temp1057=( void*)
_temp1047.elt_typ; goto _LL1056; _LL1056: _temp1055=( void*) _temp1047.rgn_typ;
goto _LL1054; _LL1054: _temp1053=( struct Cyc_Absyn_Conref*) _temp1047.nullable;
goto _LL1052; _LL1052: _temp1051=( struct Cyc_Absyn_Tqual) _temp1047.tq; goto
_LL1050; _LL1050: _temp1049=( struct Cyc_Absyn_Conref*) _temp1047.bounds; goto
_LL1044;} else{ goto _LL1045;} _LL1045: goto _LL1046; _LL1044: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1055); Cyc_Tcutil_check_nonzero_bound( loc, _temp1049); return
_temp1057; _LL1046: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1059= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp1059.last_plus_one - _temp1059.curr, _temp1059.curr);})); _LL1042:;}}
static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1136= e->topt; if( _temp1136 == 0){ _throw(
Null_Exception);} _temp1136->v;})); void* _temp1061= t; struct Cyc_Absyn_Structdecl**
_temp1077; struct Cyc_Absyn_Structdecl* _temp1079; struct Cyc_List_List*
_temp1080; struct _tuple1* _temp1082; struct Cyc_List_List* _temp1084; struct
Cyc_List_List* _temp1086; struct Cyc_Absyn_Uniondecl** _temp1088; struct Cyc_Absyn_Uniondecl*
_temp1090; struct Cyc_List_List* _temp1091; struct _tuple1* _temp1093; struct
Cyc_Absyn_Exp* _temp1096; struct Cyc_Absyn_Tqual _temp1098; void* _temp1100;
struct Cyc_Absyn_PtrInfo _temp1105; _LL1063: if(( unsigned int) _temp1061 > 4u?*((
int*) _temp1061) == Cyc_Absyn_StructType: 0){ _LL1083: _temp1082=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1061)->f1; goto _LL1081;
_LL1081: _temp1080=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1061)->f2; goto _LL1078; _LL1078: _temp1077=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1061)->f3; if( _temp1077 == 0){ goto
_LL1065;} else{ _temp1079=* _temp1077; goto _LL1064;}} else{ goto _LL1065;}
_LL1065: if(( unsigned int) _temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_AnonStructType:
0){ _LL1085: _temp1084=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1061)->f1; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(( unsigned int)
_temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_AnonUnionType: 0){ _LL1087:
_temp1086=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1061)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(( unsigned int)
_temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_UnionType: 0){ _LL1094:
_temp1093=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp1061)->f1;
goto _LL1092; _LL1092: _temp1091=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1061)->f2; goto _LL1089; _LL1089: _temp1088=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1061)->f3; if( _temp1088 == 0){ goto
_LL1071;} else{ _temp1090=* _temp1088; goto _LL1070;}} else{ goto _LL1071;}
_LL1071: if(( unsigned int) _temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_ArrayType:
0){ _LL1101: _temp1100=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1061)->f1;
goto _LL1099; _LL1099: _temp1098=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1061)->f2; goto _LL1097; _LL1097: _temp1096=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1061)->f3; goto _LL1095;} else{ goto
_LL1073;} _LL1095: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char*
_temp1102=( char*)"size"; struct _tagged_string _temp1103; _temp1103.curr=
_temp1102; _temp1103.base= _temp1102; _temp1103.last_plus_one= _temp1102 + 5;
_temp1103;})) == 0){ goto _LL1072;} else{ goto _LL1073;} _LL1073: if((
unsigned int) _temp1061 > 4u?*(( int*) _temp1061) == Cyc_Absyn_PointerType: 0){
_LL1106: _temp1105=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1061)->f1; goto _LL1104;} else{ goto _LL1075;} _LL1104: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp1107=( char*)"size"; struct
_tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base= _temp1107;
_temp1108.last_plus_one= _temp1107 + 5; _temp1108;})) == 0){ goto _LL1074;}
else{ goto _LL1075;} _LL1075: goto _LL1076; _LL1064: if( _temp1082 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1109=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1110; _temp1110.curr= _temp1109; _temp1110.base=
_temp1109; _temp1110.last_plus_one= _temp1109 + 40; _temp1110;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp1079, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1112= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1111= _temp1082; if( _temp1111 == 0){ _throw(
Null_Exception);} _temp1111;})); struct _tagged_string _temp1113=* f; xprintf("struct %.*s has no %.*s field",
_temp1112.last_plus_one - _temp1112.curr, _temp1112.curr, _temp1113.last_plus_one
- _temp1113.curr, _temp1113.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1114= _new_region(); struct _RegionHandle* rgn=& _temp1114; _push_region(
rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1079->tvs, _temp1080); t2= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_Structfield* _temp1115= finfo; if(
_temp1115 == 0){ _throw( Null_Exception);} _temp1115->type;}));}; _pop_region(&
_temp1114);} return t2;}} _LL1066: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1084, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1116=* f; xprintf("struct has no %.*s field", _temp1116.last_plus_one
- _temp1116.curr, _temp1116.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1117= finfo; if( _temp1117 == 0){ _throw( Null_Exception);} _temp1117->type;});}
_LL1068: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1086, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1118=* f; xprintf("union has no %.*s field", _temp1118.last_plus_one
- _temp1118.curr, _temp1118.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1119= finfo; if( _temp1119 == 0){ _throw( Null_Exception);} _temp1119->type;});}
_LL1070: if( _temp1093 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1120=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp1121; _temp1121.curr= _temp1120; _temp1121.base=
_temp1120; _temp1121.last_plus_one= _temp1120 + 39; _temp1121;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp1090, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1123= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1122= _temp1093; if( _temp1122 == 0){ _throw(
Null_Exception);} _temp1122;})); struct _tagged_string _temp1124=* f; xprintf("union %.*s has no %.*s field",
_temp1123.last_plus_one - _temp1123.curr, _temp1123.curr, _temp1124.last_plus_one
- _temp1124.curr, _temp1124.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1090->tvs, _temp1091); void* t2= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1125= finfo; if( _temp1125 == 0){ _throw(
Null_Exception);} _temp1125->type;})); return t2;}} _LL1072:( void*)( outer_e->r=(
void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1126=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1126[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1127; _temp1127.tag= Cyc_Absyn_Primop_e;
_temp1127.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1127.f2=({ struct Cyc_List_List*
_temp1128=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1128->hd=( void*) e; _temp1128->tl= 0; _temp1128;}); _temp1127;});
_temp1126;}))); return Cyc_Absyn_uint_t; _LL1074:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1129=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1129[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1130; _temp1130.tag= Cyc_Absyn_Primop_e;
_temp1130.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1130.f2=({ struct Cyc_List_List*
_temp1131=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1131->hd=( void*) e; _temp1131->tl= 0; _temp1131;}); _temp1130;});
_temp1129;}))); return Cyc_Absyn_uint_t; _LL1076: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1132=( char*)"size"; struct _tagged_string
_temp1133; _temp1133.curr= _temp1132; _temp1133.base= _temp1132; _temp1133.last_plus_one=
_temp1132 + 5; _temp1133;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1134= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1134.last_plus_one - _temp1134.curr, _temp1134.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1135= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1135.last_plus_one - _temp1135.curr, _temp1135.curr);}));} _LL1062:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1200= e->topt; if( _temp1200 == 0){ _throw( Null_Exception);} _temp1200->v;}));
void* _temp1137= t; struct Cyc_Absyn_PtrInfo _temp1143; struct Cyc_Absyn_Conref*
_temp1145; struct Cyc_Absyn_Tqual _temp1147; struct Cyc_Absyn_Conref* _temp1149;
void* _temp1151; void* _temp1153; _LL1139: if(( unsigned int) _temp1137 > 4u?*((
int*) _temp1137) == Cyc_Absyn_PointerType: 0){ _LL1144: _temp1143=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1137)->f1; _LL1154: _temp1153=( void*)
_temp1143.elt_typ; goto _LL1152; _LL1152: _temp1151=( void*) _temp1143.rgn_typ;
goto _LL1150; _LL1150: _temp1149=( struct Cyc_Absyn_Conref*) _temp1143.nullable;
goto _LL1148; _LL1148: _temp1147=( struct Cyc_Absyn_Tqual) _temp1143.tq; goto
_LL1146; _LL1146: _temp1145=( struct Cyc_Absyn_Conref*) _temp1143.bounds; goto
_LL1140;} else{ goto _LL1141;} _LL1141: goto _LL1142; _LL1140: Cyc_Tcutil_check_nonzero_bound(
loc, _temp1145);{ void* _temp1155= Cyc_Tcutil_compress( _temp1153); struct Cyc_Absyn_Structdecl**
_temp1167; struct Cyc_Absyn_Structdecl* _temp1169; struct Cyc_List_List*
_temp1170; struct _tuple1* _temp1172; struct Cyc_Absyn_Uniondecl** _temp1174;
struct Cyc_Absyn_Uniondecl* _temp1176; struct Cyc_List_List* _temp1177; struct
_tuple1* _temp1179; struct Cyc_List_List* _temp1181; struct Cyc_List_List*
_temp1183; _LL1157: if(( unsigned int) _temp1155 > 4u?*(( int*) _temp1155) ==
Cyc_Absyn_StructType: 0){ _LL1173: _temp1172=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1155)->f1; goto _LL1171; _LL1171: _temp1170=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1155)->f2; goto _LL1168; _LL1168:
_temp1167=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1155)->f3; if( _temp1167 == 0){ goto _LL1159;} else{ _temp1169=* _temp1167;
goto _LL1158;}} else{ goto _LL1159;} _LL1159: if(( unsigned int) _temp1155 > 4u?*((
int*) _temp1155) == Cyc_Absyn_UnionType: 0){ _LL1180: _temp1179=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp1155)->f1; goto _LL1178; _LL1178:
_temp1177=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1155)->f2; goto _LL1175; _LL1175: _temp1174=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1155)->f3; if( _temp1174 == 0){ goto
_LL1161;} else{ _temp1176=* _temp1174; goto _LL1160;}} else{ goto _LL1161;}
_LL1161: if(( unsigned int) _temp1155 > 4u?*(( int*) _temp1155) == Cyc_Absyn_AnonStructType:
0){ _LL1182: _temp1181=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1155)->f1; goto _LL1162;} else{ goto _LL1163;} _LL1163: if(( unsigned int)
_temp1155 > 4u?*(( int*) _temp1155) == Cyc_Absyn_AnonUnionType: 0){ _LL1184:
_temp1183=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1155)->f1; goto _LL1164;} else{ goto _LL1165;} _LL1165: goto _LL1166;
_LL1158: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp1169, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1186= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1185= _temp1172; if( _temp1185 == 0){ _throw( Null_Exception);}
_temp1185;})); struct _tagged_string _temp1187=* f; xprintf("struct %.*s has no %.*s field",
_temp1186.last_plus_one - _temp1186.curr, _temp1186.curr, _temp1187.last_plus_one
- _temp1187.curr, _temp1187.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1188= _new_region(); struct _RegionHandle* rgn=& _temp1188; _push_region(
rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1169->tvs, _temp1170); t3= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*)({ struct Cyc_Absyn_Structfield* _temp1189= finfo; if(
_temp1189 == 0){ _throw( Null_Exception);} _temp1189->type;}));}; _pop_region(&
_temp1188);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1151); return t3;}}
_LL1160: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_union_field(
_temp1176, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1191= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp1190= _temp1179; if( _temp1190 == 0){ _throw( Null_Exception);}
_temp1190;})); struct _tagged_string _temp1192=* f; xprintf("union %.*s has no %.*s field",
_temp1191.last_plus_one - _temp1191.curr, _temp1191.curr, _temp1192.last_plus_one
- _temp1192.curr, _temp1192.curr);}));}{ struct Cyc_List_List* inst=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1176->tvs, _temp1177); void* t3= Cyc_Tcutil_substitute( inst,( void*)({
struct Cyc_Absyn_Structfield* _temp1193= finfo; if( _temp1193 == 0){ _throw(
Null_Exception);} _temp1193->type;})); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1151); return t3;}} _LL1162: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1181, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1194=* f; xprintf("struct has no %.*s field", _temp1194.last_plus_one
- _temp1194.curr, _temp1194.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1195= finfo; if( _temp1195 == 0){ _throw( Null_Exception);} _temp1195->type;});}
_LL1164: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_field(
_temp1183, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1196=* f; xprintf("union has no %.*s field", _temp1196.last_plus_one
- _temp1196.curr, _temp1196.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1197= finfo; if( _temp1197 == 0){ _throw( Null_Exception);} _temp1197->type;});}
_LL1166: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1198=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1198.last_plus_one - _temp1198.curr, _temp1198.curr);})); _LL1156:;}
_LL1142: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1199=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1199.last_plus_one - _temp1199.curr, _temp1199.curr);})); _LL1138:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1201; _push_handler(& _temp1201);{ void*
_temp1202=( void*) setjmp( _temp1201.handler); if( ! _temp1202){{ void*
_temp1203=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1203;}; _pop_handler();} else{
void* _temp1205= _temp1202; _LL1207: if( _temp1205 == Cyc_List_Nth){ goto
_LL1208;} else{ goto _LL1209;} _LL1209: goto _LL1210; _LL1208: return Cyc_Tcexp_expr_err(
te, loc, xprintf("index is %d but tuple has only %d fields",( int) i,(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1210:( void) _throw(
_temp1205); _LL1206:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1236=
e1->topt; if( _temp1236 == 0){ _throw( Null_Exception);} _temp1236->v;})); void*
t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1235= e2->topt; if(
_temp1235 == 0){ _throw( Null_Exception);} _temp1235->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1211= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1211.last_plus_one - _temp1211.curr, _temp1211.curr);}));}{ void* _temp1212=
t1; struct Cyc_Absyn_PtrInfo _temp1220; struct Cyc_Absyn_Conref* _temp1222;
struct Cyc_Absyn_Tqual _temp1224; struct Cyc_Absyn_Conref* _temp1226; void*
_temp1228; void* _temp1230; struct Cyc_List_List* _temp1232; _LL1214: if((
unsigned int) _temp1212 > 4u?*(( int*) _temp1212) == Cyc_Absyn_PointerType: 0){
_LL1221: _temp1220=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1212)->f1; _LL1231: _temp1230=( void*) _temp1220.elt_typ; goto _LL1229;
_LL1229: _temp1228=( void*) _temp1220.rgn_typ; goto _LL1227; _LL1227: _temp1226=(
struct Cyc_Absyn_Conref*) _temp1220.nullable; goto _LL1225; _LL1225: _temp1224=(
struct Cyc_Absyn_Tqual) _temp1220.tq; goto _LL1223; _LL1223: _temp1222=( struct
Cyc_Absyn_Conref*) _temp1220.bounds; goto _LL1215;} else{ goto _LL1216;} _LL1216:
if(( unsigned int) _temp1212 > 4u?*(( int*) _temp1212) == Cyc_Absyn_TupleType: 0){
_LL1233: _temp1232=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1212)->f1; goto _LL1217;} else{ goto _LL1218;} _LL1218: goto _LL1219;
_LL1215: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1222);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1222);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1228); return _temp1230; _LL1217: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1232, e2); _LL1219: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1234= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1234.last_plus_one - _temp1234.curr, _temp1234.curr);})); _LL1213:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1238= Cyc_Tcutil_compress(({ void**
_temp1237= topt; if( _temp1237 == 0){ _throw( Null_Exception);}* _temp1237;}));
struct Cyc_List_List* _temp1244; _LL1240: if(( unsigned int) _temp1238 > 4u?*((
int*) _temp1238) == Cyc_Absyn_TupleType: 0){ _LL1245: _temp1244=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1238)->f1; goto _LL1241;} else{ goto
_LL1242;} _LL1242: goto _LL1243; _LL1241: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1244) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1246=( char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1247; _temp1247.curr= _temp1246; _temp1247.base=
_temp1246; _temp1247.last_plus_one= _temp1246 + 48; _temp1247;})); goto _LL1239;}
for( 0; es != 0; es=({ struct Cyc_List_List* _temp1248= es; if( _temp1248 == 0){
_throw( Null_Exception);} _temp1248->tl;}), _temp1244=({ struct Cyc_List_List*
_temp1249= _temp1244; if( _temp1249 == 0){ _throw( Null_Exception);} _temp1249->tl;})){
void* topt2=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1256= _temp1244;
if( _temp1256 == 0){ _throw( Null_Exception);} _temp1256->hd;}))).f2; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt2,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1250=
es; if( _temp1250 == 0){ _throw( Null_Exception);} _temp1250->hd;})); fields=({
struct Cyc_List_List* _temp1251=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1251->hd=( void*)({ struct _tuple7* _temp1252=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1252->f1=(*(( struct
_tuple7*)({ struct Cyc_List_List* _temp1255= _temp1244; if( _temp1255 == 0){
_throw( Null_Exception);} _temp1255->hd;}))).f1; _temp1252->f2=( void*)({ struct
Cyc_Core_Opt* _temp1254=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1253= es; if( _temp1253 == 0){ _throw( Null_Exception);} _temp1253->hd;}))->topt;
if( _temp1254 == 0){ _throw( Null_Exception);} _temp1254->v;}); _temp1252;});
_temp1251->tl= fields; _temp1251;});} done= 1; goto _LL1239; _LL1243: goto
_LL1239; _LL1239:;} if( ! done){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp1257= es; if( _temp1257 == 0){ _throw( Null_Exception);} _temp1257->tl;})){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1258= es; if( _temp1258 == 0){ _throw( Null_Exception);} _temp1258->hd;}));
fields=({ struct Cyc_List_List* _temp1259=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1259->hd=( void*)({ struct _tuple7*
_temp1260=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1260->f1=
Cyc_Absyn_empty_tqual(); _temp1260->f2=( void*)({ struct Cyc_Core_Opt* _temp1262=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1261= es; if( _temp1261 == 0){
_throw( Null_Exception);} _temp1261->hd;}))->topt; if( _temp1262 == 0){ _throw(
Null_Exception);} _temp1262->v;}); _temp1260;}); _temp1259->tl= fields;
_temp1259;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct* _temp1263=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1263[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1264; _temp1264.tag= Cyc_Absyn_TupleType;
_temp1264.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1264;}); _temp1263;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1265=( char*)"tcCompoundLit"; struct
_tagged_string _temp1266; _temp1266.curr= _temp1265; _temp1266.base= _temp1265;
_temp1266.last_plus_one= _temp1265 + 14; _temp1266;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp1289=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1289[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1290; _temp1290.tag= Cyc_Absyn_Const_e; _temp1290.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1291=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1291[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1292; _temp1292.tag= Cyc_Absyn_Int_c; _temp1292.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1292.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1292;}); _temp1291;})); _temp1290;}); _temp1289;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1267=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1267->v=( void*) Cyc_Absyn_uint_t; _temp1267;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1287=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1287[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1288; _temp1288.tag= Cyc_Absyn_ArrayType;
_temp1288.f1=( void*) res; _temp1288.f2= Cyc_Absyn_empty_tqual(); _temp1288.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1288;}); _temp1287;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp1268= es2; if(
_temp1268 == 0){ _throw( Null_Exception);} _temp1268->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1269= es2;
if( _temp1269 == 0){ _throw( Null_Exception);} _temp1269->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1270= es; if( _temp1270 == 0){ _throw( Null_Exception);} _temp1270->hd;}))->loc,({
struct _tagged_string _temp1271= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1271.last_plus_one - _temp1271.curr, _temp1271.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp1272= des; if(
_temp1272 == 0){ _throw( Null_Exception);} _temp1272->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1286= des; if( _temp1286
== 0){ _throw( Null_Exception);} _temp1286->hd;}))).f1; if( ds != 0){ void*
_temp1274=( void*)({ struct Cyc_List_List* _temp1273= ds; if( _temp1273 == 0){
_throw( Null_Exception);} _temp1273->hd;}); struct _tagged_string* _temp1280;
struct Cyc_Absyn_Exp* _temp1282; _LL1276: if(*(( int*) _temp1274) == Cyc_Absyn_FieldName){
_LL1281: _temp1280=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1274)->f1; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*)
_temp1274) == Cyc_Absyn_ArrayElement){ _LL1283: _temp1282=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1274)->f1; goto _LL1279;} else{ goto
_LL1275;} _LL1277: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1284=( char*)"only array index designators are supported"; struct
_tagged_string _temp1285; _temp1285.curr= _temp1284; _temp1285.base= _temp1284;
_temp1285.last_plus_one= _temp1284 + 43; _temp1285;})); goto _LL1275; _LL1279:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1282);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1282); if( i != offset){ Cyc_Tcutil_terr( _temp1282->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1275;} _LL1275:;}}} return res_t2;}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1294= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1293= bound->topt; if( _temp1293 == 0){
_throw( Null_Exception);} _temp1293->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1294.last_plus_one - _temp1294.curr, _temp1294.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1295=( char*)"comprehension index variable is not declared const!";
struct _tagged_string _temp1296; _temp1296.curr= _temp1295; _temp1296.base=
_temp1295; _temp1296.last_plus_one= _temp1295 + 52; _temp1296;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_string)({ char* _temp1297=( char*)"bound is not constant"; struct
_tagged_string _temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297;
_temp1298.last_plus_one= _temp1297 + 22; _temp1298;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ char*
_temp1299=( char*)"body is not constant"; struct _tagged_string _temp1300;
_temp1300.curr= _temp1299; _temp1300.base= _temp1299; _temp1300.last_plus_one=
_temp1299 + 21; _temp1300;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2=
0; struct Cyc_Absyn_Tqual* tqopt= 0; if( topt != 0){ void* _temp1302= Cyc_Tcutil_compress(({
void** _temp1301= topt; if( _temp1301 == 0){ _throw( Null_Exception);}*
_temp1301;})); struct Cyc_Absyn_PtrInfo _temp1310; struct Cyc_Absyn_Exp*
_temp1312; struct Cyc_Absyn_Tqual _temp1314; struct Cyc_Absyn_Tqual* _temp1316;
void* _temp1317; void** _temp1319; _LL1304: if(( unsigned int) _temp1302 > 4u?*((
int*) _temp1302) == Cyc_Absyn_PointerType: 0){ _LL1311: _temp1310=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1302)->f1; goto _LL1305;} else{ goto
_LL1306;} _LL1306: if(( unsigned int) _temp1302 > 4u?*(( int*) _temp1302) == Cyc_Absyn_ArrayType:
0){ _LL1318: _temp1317=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1302)->f1;
_temp1319=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1302)->f1; goto _LL1315;
_LL1315: _temp1314=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1302)->f2; _temp1316=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1302)->f2;
goto _LL1313; _LL1313: _temp1312=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1302)->f3; goto _LL1307;} else{ goto _LL1308;} _LL1308: goto _LL1309;
_LL1305: pinfo= _temp1310; topt2=( void**)(( void*)& pinfo.elt_typ); tqopt=(
struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1303; _LL1307: topt2=( void**)
_temp1319; tqopt=( struct Cyc_Absyn_Tqual*) _temp1316; goto _LL1303; _LL1309:
goto _LL1303; _LL1303:;}{ void* t= Cyc_Tcexp_tcExp( te, topt2, body); return(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1320=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1320[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1321; _temp1321.tag= Cyc_Absyn_ArrayType;
_temp1321.f1=( void*) t; _temp1321.f2= tqopt == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual* _temp1322= tqopt; if( _temp1322 == 0){ _throw(
Null_Exception);}* _temp1322;}); _temp1321.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1321;}); _temp1320;});}}} struct _tuple8{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1323=* sd_opt; if( _temp1323 == 0){ _throw(
Null_Exception);} _temp1323;});} else{{ struct _handler_cons _temp1324;
_push_handler(& _temp1324);{ void* _temp1325=( void*) setjmp( _temp1324.handler);
if( ! _temp1325){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1327= _temp1325; _LL1329: if( _temp1327 == Cyc_Dict_Absent){
goto _LL1330;} else{ goto _LL1331;} _LL1331: goto _LL1332; _LL1330: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp1333= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1333.last_plus_one - _temp1333.curr, _temp1333.curr);})); return topt != 0?({
void** _temp1334= topt; if( _temp1334 == 0){ _throw( Null_Exception);}*
_temp1334;}):( void*) Cyc_Absyn_VoidType; _LL1332:( void) _throw( _temp1327);
_LL1328:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1335= sd->name; if( _temp1335 == 0){
_throw( Null_Exception);} _temp1335->v;});}}{ struct _RegionHandle _temp1336=
_new_region(); struct _RegionHandle* rgn=& _temp1336; _push_region( rgn);{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct* res_typ=({
struct Cyc_Absyn_StructType_struct* _temp1358=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1358[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1359; _temp1359.tag= Cyc_Absyn_StructType;
_temp1359.f1=( struct _tuple1*)* tn; _temp1359.f2= typs; _temp1359.f3=({ struct
Cyc_Absyn_Structdecl** _temp1360=( struct Cyc_Absyn_Structdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp1360[ 0]= sd; _temp1360;});
_temp1359;}); _temp1358;}); if( topt != 0){ Cyc_Tcutil_unify(( void*) res_typ,({
void** _temp1337= topt; if( _temp1337 == 0){ _throw( Null_Exception);}*
_temp1337;}));}* otyps=({ struct Cyc_Core_Opt* _temp1338=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1338->v=( void*) typs; _temp1338;});
if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1339=( char*)"can't build abstract struct"; struct _tagged_string _temp1340;
_temp1340.curr= _temp1339; _temp1340.base= _temp1339; _temp1340.last_plus_one=
_temp1339 + 28; _temp1340;}));{ void* _temp1341=( void*) res_typ; _npop_handler(
0u); return _temp1341;}}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn,
loc, args,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1357= sd->fields;
if( _temp1357 == 0){ _throw( Null_Exception);} _temp1357->v;})); for( 0; fields
!= 0; fields=({ struct Cyc_List_List* _temp1342= fields; if( _temp1342 == 0){
_throw( Null_Exception);} _temp1342->tl;})){ struct _tuple8 _temp1346; struct
Cyc_Absyn_Exp* _temp1347; struct Cyc_Absyn_Structfield* _temp1349; struct
_tuple8* _temp1344=( struct _tuple8*)({ struct Cyc_List_List* _temp1343= fields;
if( _temp1343 == 0){ _throw( Null_Exception);} _temp1343->hd;}); _temp1346=*
_temp1344; _LL1350: _temp1349= _temp1346.f1; goto _LL1348; _LL1348: _temp1347=
_temp1346.f2; goto _LL1345; _LL1345: { void* inst_fieldtyp= Cyc_Tcutil_rsubstitute(
rgn, inst,( void*) _temp1349->type); Cyc_Tcexp_tcExpInitializer( te,( void**)&
inst_fieldtyp, _temp1347); if( ! Cyc_Tcutil_coerce_arg( te, _temp1347,
inst_fieldtyp)){ Cyc_Tcutil_terr( _temp1347->loc,({ struct _tagged_string
_temp1352=* _temp1349->name; struct _tagged_string _temp1353= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp1354= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp1355= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1351= _temp1347->topt; if( _temp1351 == 0){ _throw( Null_Exception);}
_temp1351->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1352.last_plus_one - _temp1352.curr, _temp1352.curr, _temp1353.last_plus_one
- _temp1353.curr, _temp1353.curr, _temp1354.last_plus_one - _temp1354.curr,
_temp1354.curr, _temp1355.last_plus_one - _temp1355.curr, _temp1355.curr);}));}}}{
void* _temp1356=( void*) res_typ; _npop_handler( 0u); return _temp1356;}}};
_pop_region(& _temp1336);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1361= _new_region(); struct _RegionHandle* rgn=&
_temp1361; _push_region( rgn);{ void* _temp1362= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1368; _LL1364: if(( unsigned int) _temp1362 > 4u?*((
int*) _temp1362) == Cyc_Absyn_AnonStructType: 0){ _LL1369: _temp1368=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1362)->f1; goto
_LL1365;} else{ goto _LL1366;} _LL1366: goto _LL1367; _LL1365: { struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1368); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1370= fields; if( _temp1370 == 0){ _throw( Null_Exception);} _temp1370->tl;})){
struct _tuple8 _temp1374; struct Cyc_Absyn_Exp* _temp1375; struct Cyc_Absyn_Structfield*
_temp1377; struct _tuple8* _temp1372=( struct _tuple8*)({ struct Cyc_List_List*
_temp1371= fields; if( _temp1371 == 0){ _throw( Null_Exception);} _temp1371->hd;});
_temp1374=* _temp1372; _LL1378: _temp1377= _temp1374.f1; goto _LL1376; _LL1376:
_temp1375= _temp1374.f2; goto _LL1373; _LL1373: Cyc_Tcexp_tcExpInitializer( te,(
void**)(( void*)& _temp1377->type), _temp1375); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1375,( void*) _temp1377->type)){ Cyc_Tcutil_terr( _temp1375->loc,({ struct
_tagged_string _temp1380=* _temp1377->name; struct _tagged_string _temp1381= Cyc_Absynpp_typ2string((
void*) _temp1377->type); struct _tagged_string _temp1382= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1379= _temp1375->topt; if( _temp1379 == 0){
_throw( Null_Exception);} _temp1379->v;})); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1380.last_plus_one - _temp1380.curr, _temp1380.curr, _temp1381.last_plus_one
- _temp1381.curr, _temp1381.curr, _temp1382.last_plus_one - _temp1382.curr,
_temp1382.curr);}));}} goto _LL1363;} _LL1367:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1383=( char*)"tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1384; _temp1384.curr= _temp1383; _temp1384.base=
_temp1383; _temp1384.last_plus_one= _temp1383 + 37; _temp1384;})); goto _LL1363;
_LL1363:;}; _pop_region(& _temp1361);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1385= _new_region(); struct _RegionHandle* rgn=&
_temp1385; _push_region( rgn);{ struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tud->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tuf->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(*
f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst);*
all_ref=({ struct Cyc_Core_Opt* _temp1386=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1386->v=( void*) all_typs; _temp1386;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1387=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1387->v=( void*) exist_typs; _temp1387;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1435=( struct
Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1435[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1436; _temp1436.tag=
Cyc_Absyn_TunionFieldType; _temp1436.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1437; _temp1437.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1438=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1438[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1439; _temp1439.tag= Cyc_Absyn_KnownTunionfield; _temp1439.f1= tud;
_temp1439.f2= tuf; _temp1439;}); _temp1438;})); _temp1437.targs= all_typs;
_temp1437;}); _temp1436;}); _temp1435;}); if( topt != 0){ void* _temp1389= Cyc_Tcutil_compress(({
void** _temp1388= topt; if( _temp1388 == 0){ _throw( Null_Exception);}*
_temp1388;})); struct Cyc_Absyn_TunionFieldInfo _temp1397; struct Cyc_Absyn_TunionInfo
_temp1399; void* _temp1401; struct Cyc_List_List* _temp1403; void* _temp1405;
_LL1391: if(( unsigned int) _temp1389 > 4u?*(( int*) _temp1389) == Cyc_Absyn_TunionFieldType:
0){ _LL1398: _temp1397=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1389)->f1; goto _LL1392;} else{ goto _LL1393;} _LL1393: if(( unsigned int)
_temp1389 > 4u?*(( int*) _temp1389) == Cyc_Absyn_TunionType: 0){ _LL1400:
_temp1399=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1389)->f1; _LL1406: _temp1405=( void*) _temp1399.tunion_info; goto _LL1404;
_LL1404: _temp1403=( struct Cyc_List_List*) _temp1399.targs; goto _LL1402;
_LL1402: _temp1401=( void*) _temp1399.rgn; goto _LL1394;} else{ goto _LL1395;}
_LL1395: goto _LL1396; _LL1392: Cyc_Tcutil_unify(({ void** _temp1407= topt; if(
_temp1407 == 0){ _throw( Null_Exception);}* _temp1407;}), res); goto _LL1390;
_LL1394:{ struct Cyc_List_List* a= all_typs; for( 0; a != 0? _temp1403 != 0: 0;
a=({ struct Cyc_List_List* _temp1408= a; if( _temp1408 == 0){ _throw(
Null_Exception);} _temp1408->tl;}), _temp1403=({ struct Cyc_List_List* _temp1409=
_temp1403; if( _temp1409 == 0){ _throw( Null_Exception);} _temp1409->tl;})){ Cyc_Tcutil_unify((
void*)({ struct Cyc_List_List* _temp1410= a; if( _temp1410 == 0){ _throw(
Null_Exception);} _temp1410->hd;}),( void*)({ struct Cyc_List_List* _temp1411=
_temp1403; if( _temp1411 == 0){ _throw( Null_Exception);} _temp1411->hd;}));}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1412=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1412->v=(
void*) res; _temp1412;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1413=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1413[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1414; _temp1414.tag=
Cyc_Absyn_TunionType; _temp1414.f1=({ struct Cyc_Absyn_TunionInfo _temp1415;
_temp1415.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1416=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1416[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1417; _temp1417.tag=
Cyc_Absyn_KnownTunion; _temp1417.f1= tud; _temp1417;}); _temp1416;})); _temp1415.targs=
all_typs; _temp1415.rgn=( void*) _temp1401; _temp1415;}); _temp1414;});
_temp1413;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1418= res;
_npop_handler( 0u); return _temp1418;}} goto _LL1390; _LL1396: goto _LL1390;
_LL1390:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0; es=({
struct Cyc_List_List* _temp1419= es; if( _temp1419 == 0){ _throw( Null_Exception);}
_temp1419->tl;}), ts=({ struct Cyc_List_List* _temp1420= ts; if( _temp1420 == 0){
_throw( Null_Exception);} _temp1420->tl;})){ struct Cyc_Absyn_Exp* e=( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1428= es; if( _temp1428 == 0){
_throw( Null_Exception);} _temp1428->hd;}); void* t= Cyc_Tcutil_rsubstitute( rgn,
inst,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1427= ts; if( _temp1427
== 0){ _throw( Null_Exception);} _temp1427->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp1424= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1425= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1426= e->topt == 0?( struct _tagged_string)({ char* _temp1421=( char*)"?";
struct _tagged_string _temp1422; _temp1422.curr= _temp1421; _temp1422.base=
_temp1421; _temp1422.last_plus_one= _temp1421 + 2; _temp1422;}): Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1423= e->topt; if( _temp1423 == 0){ _throw(
Null_Exception);} _temp1423->v;})); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1424.last_plus_one - _temp1424.curr, _temp1424.curr, _temp1425.last_plus_one
- _temp1425.curr, _temp1425.curr, _temp1426.last_plus_one - _temp1426.curr,
_temp1426.curr);}));}} if( es != 0){ void* _temp1431= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1429= es; if( _temp1429 == 0){
_throw( Null_Exception);} _temp1429->hd;}))->loc,({ struct _tagged_string
_temp1430= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1430.last_plus_one - _temp1430.curr, _temp1430.curr);})); _npop_handler( 0u);
return _temp1431;} if( ts != 0){ void* _temp1433= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1432= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1432.last_plus_one - _temp1432.curr, _temp1432.curr);})); _npop_handler( 0u);
return _temp1433;}{ void* _temp1434= res; _npop_handler( 0u); return _temp1434;}}}};
_pop_region(& _temp1385);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1451=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1451[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1452; _temp1452.tag=
Cyc_Absyn_RgnHandleType; _temp1452.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1452;}); _temp1451;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1450= ropt;
if( _temp1450 == 0){ _throw( Null_Exception);} _temp1450;})); void* _temp1440=
Cyc_Tcutil_compress( handle_type); void* _temp1446; _LL1442: if(( unsigned int)
_temp1440 > 4u?*(( int*) _temp1440) == Cyc_Absyn_RgnHandleType: 0){ _LL1447:
_temp1446=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1440)->f1;
goto _LL1443;} else{ goto _LL1444;} _LL1444: goto _LL1445; _LL1443: rgn=
_temp1446; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto _LL1441; _LL1445:
Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1448= ropt; if( _temp1448 == 0){
_throw( Null_Exception);} _temp1448->loc;}),({ struct _tagged_string _temp1449=
Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1449.last_plus_one - _temp1449.curr, _temp1449.curr);})); goto _LL1441;
_LL1441:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1453= t; struct Cyc_Absyn_TunionFieldInfo
_temp1459; struct Cyc_List_List* _temp1461; void* _temp1463; struct Cyc_Absyn_Tunionfield*
_temp1465; struct Cyc_Absyn_Tuniondecl* _temp1467; _LL1455: if(( unsigned int)
_temp1453 > 4u?*(( int*) _temp1453) == Cyc_Absyn_TunionFieldType: 0){ _LL1460:
_temp1459=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1453)->f1; _LL1464: _temp1463=( void*) _temp1459.field_info; if(*(( int*)
_temp1463) == Cyc_Absyn_KnownTunionfield){ _LL1468: _temp1467=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1463)->f1; goto _LL1466; _LL1466:
_temp1465=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1463)->f2; goto _LL1462;} else{ goto _LL1457;} _LL1462: _temp1461=( struct
Cyc_List_List*) _temp1459.targs; goto _LL1456;} else{ goto _LL1457;} _LL1457:
goto _LL1458; _LL1456: if( _temp1465->tvs != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp1469=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1470; _temp1470.curr= _temp1469; _temp1470.base=
_temp1469; _temp1470.last_plus_one= _temp1469 + 50; _temp1470;}));} goto _LL1454;
_LL1458: goto _LL1454; _LL1454:;}{ void*(* ptr_maker)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp1472=
Cyc_Tcutil_compress(({ void** _temp1471= topt; if( _temp1471 == 0){ _throw(
Null_Exception);}* _temp1471;})); struct Cyc_Absyn_PtrInfo _temp1478; struct Cyc_Absyn_Conref*
_temp1480; struct Cyc_Absyn_Tqual _temp1482; struct Cyc_Absyn_Conref* _temp1484;
void* _temp1486; void* _temp1488; _LL1474: if(( unsigned int) _temp1472 > 4u?*((
int*) _temp1472) == Cyc_Absyn_PointerType: 0){ _LL1479: _temp1478=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1472)->f1; _LL1489: _temp1488=( void*)
_temp1478.elt_typ; goto _LL1487; _LL1487: _temp1486=( void*) _temp1478.rgn_typ;
goto _LL1485; _LL1485: _temp1484=( struct Cyc_Absyn_Conref*) _temp1478.nullable;
goto _LL1483; _LL1483: _temp1482=( struct Cyc_Absyn_Tqual) _temp1478.tq; goto
_LL1481; _LL1481: _temp1480=( struct Cyc_Absyn_Conref*) _temp1478.bounds; goto
_LL1475;} else{ goto _LL1476;} _LL1476: goto _LL1477; _LL1475: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1484)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL1473; _LL1477: goto _LL1473; _LL1473:;} return ptr_maker( t, rgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1490=( void*) s->r; struct Cyc_Absyn_Exp* _temp1500;
struct Cyc_Absyn_Stmt* _temp1502; struct Cyc_Absyn_Stmt* _temp1504; struct Cyc_Absyn_Stmt*
_temp1506; struct Cyc_Absyn_Decl* _temp1508; _LL1492: if(( unsigned int)
_temp1490 > 1u?*(( int*) _temp1490) == Cyc_Absyn_Exp_s: 0){ _LL1501: _temp1500=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1490)->f1; goto
_LL1493;} else{ goto _LL1494;} _LL1494: if(( unsigned int) _temp1490 > 1u?*((
int*) _temp1490) == Cyc_Absyn_Seq_s: 0){ _LL1505: _temp1504=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1490)->f1; goto _LL1503; _LL1503: _temp1502=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1490)->f2; goto
_LL1495;} else{ goto _LL1496;} _LL1496: if(( unsigned int) _temp1490 > 1u?*((
int*) _temp1490) == Cyc_Absyn_Decl_s: 0){ _LL1509: _temp1508=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1490)->f1; goto _LL1507; _LL1507:
_temp1506=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1490)->f2;
goto _LL1497;} else{ goto _LL1498;} _LL1498: goto _LL1499; _LL1493: return( void*)({
struct Cyc_Core_Opt* _temp1510= _temp1500->topt; if( _temp1510 == 0){ _throw(
Null_Exception);} _temp1510->v;}); _LL1495: s= _temp1502; continue; _LL1497: s=
_temp1506; continue; _LL1499: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1511=( char*)"statement expression must end with expression";
struct _tagged_string _temp1512; _temp1512.curr= _temp1511; _temp1512.base=
_temp1511; _temp1512.last_plus_one= _temp1511 + 46; _temp1512;})); _LL1491:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1513=( char*)"tcCodegen"; struct
_tagged_string _temp1514; _temp1514.curr= _temp1513; _temp1514.base= _temp1513;
_temp1514.last_plus_one= _temp1513 + 10; _temp1514;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1515=( char*)"tcFill"; struct _tagged_string _temp1516; _temp1516.curr=
_temp1515; _temp1516.base= _temp1515; _temp1516.last_plus_one= _temp1515 + 7;
_temp1516;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1528=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1528[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1529; _temp1529.tag= Cyc_Absyn_RgnHandleType; _temp1529.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1529;}); _temp1528;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1527= rgn_handle; if( _temp1527 == 0){ _throw( Null_Exception);} _temp1527;}));
void* _temp1517= Cyc_Tcutil_compress( handle_type); void* _temp1523; _LL1519:
if(( unsigned int) _temp1517 > 4u?*(( int*) _temp1517) == Cyc_Absyn_RgnHandleType:
0){ _LL1524: _temp1523=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1517)->f1; goto _LL1520;} else{ goto _LL1521;} _LL1521: goto _LL1522;
_LL1520: rgn= _temp1523; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1518; _LL1522: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1525= rgn_handle;
if( _temp1525 == 0){ _throw( Null_Exception);} _temp1525->loc;}),({ struct
_tagged_string _temp1526= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1526.last_plus_one - _temp1526.curr, _temp1526.curr);})); goto _LL1518;
_LL1518:;}{ void* _temp1530=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1542;
struct Cyc_Absyn_Exp* _temp1544; struct Cyc_Absyn_Vardecl* _temp1546; struct Cyc_List_List*
_temp1548; struct Cyc_Core_Opt* _temp1550; struct Cyc_List_List* _temp1552; void*
_temp1554; struct _tagged_string _temp1556; _LL1532: if(*(( int*) _temp1530) ==
Cyc_Absyn_Comprehension_e){ _LL1547: _temp1546=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1530)->f1; goto _LL1545; _LL1545:
_temp1544=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1530)->f2; goto _LL1543; _LL1543: _temp1542=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1530)->f3; goto _LL1533;} else{
goto _LL1534;} _LL1534: if(*(( int*) _temp1530) == Cyc_Absyn_UnresolvedMem_e){
_LL1551: _temp1550=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1530)->f1; goto _LL1549; _LL1549: _temp1548=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1530)->f2; goto _LL1535;} else{
goto _LL1536;} _LL1536: if(*(( int*) _temp1530) == Cyc_Absyn_Array_e){ _LL1553:
_temp1552=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1530)->f1;
goto _LL1537;} else{ goto _LL1538;} _LL1538: if(*(( int*) _temp1530) == Cyc_Absyn_Const_e){
_LL1555: _temp1554=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1530)->f1;
if(( unsigned int) _temp1554 > 1u?*(( int*) _temp1554) == Cyc_Absyn_String_c: 0){
_LL1557: _temp1556=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1554)->f1; goto _LL1539;} else{ goto _LL1540;}} else{ goto _LL1540;}
_LL1540: goto _LL1541; _LL1533: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1558= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1564; struct Cyc_Absyn_Tqual _temp1566; void* _temp1568; _LL1560: if((
unsigned int) _temp1558 > 4u?*(( int*) _temp1558) == Cyc_Absyn_ArrayType: 0){
_LL1569: _temp1568=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1558)->f1;
goto _LL1567; _LL1567: _temp1566=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1558)->f2; goto _LL1565; _LL1565: _temp1564=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1558)->f3; goto _LL1561;} else{ goto
_LL1562;} _LL1562: goto _LL1563; _LL1561: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1544)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1578=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1578[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1579; _temp1579.tag= Cyc_Absyn_Upper_b; _temp1579.f1=
_temp1544; _temp1579;}); _temp1578;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1575=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1575[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1576; _temp1576.tag= Cyc_Absyn_PointerType;
_temp1576.f1=({ struct Cyc_Absyn_PtrInfo _temp1577; _temp1577.elt_typ=( void*)
_temp1568; _temp1577.rgn_typ=( void*) rgn; _temp1577.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1577.tq= _temp1566; _temp1577.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1577;});
_temp1576;}); _temp1575;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1570= topt; if( _temp1570 == 0){ _throw( Null_Exception);}* _temp1570;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1571= topt;
if( _temp1571 == 0){ _throw( Null_Exception);}* _temp1571;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1572=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1572->v=( void*) res_typ; _temp1572;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1573= topt; if( _temp1573 == 0){ _throw( Null_Exception);}*
_temp1573;})); res_typ=({ void** _temp1574= topt; if( _temp1574 == 0){ _throw(
Null_Exception);}* _temp1574;});}} return res_typ;} _LL1563: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1580=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1581; _temp1581.curr= _temp1580; _temp1581.base= _temp1580;
_temp1581.last_plus_one= _temp1580 + 45; _temp1581;})); _LL1559:;} _LL1535:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1582=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1582[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1583; _temp1583.tag= Cyc_Absyn_Array_e;
_temp1583.f1= _temp1548; _temp1583;}); _temp1582;}))); _temp1552= _temp1548;
goto _LL1537; _LL1537: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1585=
Cyc_Tcutil_compress(({ void** _temp1584= topt; if( _temp1584 == 0){ _throw(
Null_Exception);}* _temp1584;})); struct Cyc_Absyn_PtrInfo _temp1591; struct Cyc_Absyn_Conref*
_temp1593; struct Cyc_Absyn_Tqual _temp1595; struct Cyc_Absyn_Conref* _temp1597;
void* _temp1599; void* _temp1601; void** _temp1603; _LL1587: if(( unsigned int)
_temp1585 > 4u?*(( int*) _temp1585) == Cyc_Absyn_PointerType: 0){ _LL1592:
_temp1591=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1585)->f1; _LL1602: _temp1601=( void*) _temp1591.elt_typ; _temp1603=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1585)->f1).elt_typ; goto _LL1600;
_LL1600: _temp1599=( void*) _temp1591.rgn_typ; goto _LL1598; _LL1598: _temp1597=(
struct Cyc_Absyn_Conref*) _temp1591.nullable; goto _LL1596; _LL1596: _temp1595=(
struct Cyc_Absyn_Tqual) _temp1591.tq; goto _LL1594; _LL1594: _temp1593=( struct
Cyc_Absyn_Conref*) _temp1591.bounds; goto _LL1588;} else{ goto _LL1589;} _LL1589:
goto _LL1590; _LL1588: elt_typ_opt=( void**) _temp1603; goto _LL1586; _LL1590:
goto _LL1586; _LL1586:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1604= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1610; struct Cyc_Absyn_Tqual _temp1612; void*
_temp1614; _LL1606: if(( unsigned int) _temp1604 > 4u?*(( int*) _temp1604) ==
Cyc_Absyn_ArrayType: 0){ _LL1615: _temp1614=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1604)->f1; goto _LL1613; _LL1613: _temp1612=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1604)->f2; goto _LL1611; _LL1611:
_temp1610=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1604)->f3; goto _LL1607;} else{ goto _LL1608;} _LL1608: goto _LL1609;
_LL1607: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1616=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1616[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1617; _temp1617.tag=
Cyc_Absyn_PointerType; _temp1617.f1=({ struct Cyc_Absyn_PtrInfo _temp1618;
_temp1618.elt_typ=( void*) _temp1614; _temp1618.rgn_typ=( void*) rgn; _temp1618.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1618.tq=
_temp1612; _temp1618.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1619=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1619[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1620; _temp1620.tag= Cyc_Absyn_Upper_b; _temp1620.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1621= _temp1610; if(
_temp1621 == 0){ _throw( Null_Exception);} _temp1621;}); _temp1620;}); _temp1619;}));
_temp1618;}); _temp1617;}); _temp1616;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1622= topt; if( _temp1622 == 0){ _throw( Null_Exception);}*
_temp1622;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1623= topt; if( _temp1623 == 0){ _throw( Null_Exception);}* _temp1623;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1624=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1624->v=( void*) res_typ; _temp1624;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1625= topt; if( _temp1625 == 0){ _throw( Null_Exception);}*
_temp1625;})); res_typ=({ void** _temp1626= topt; if( _temp1626 == 0){ _throw(
Null_Exception);}* _temp1626;});}} goto _LL1605; _LL1609: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1627=( char*)"tcExpNoPromote on Array_e returned non-array type"; struct
_tagged_string _temp1628; _temp1628.curr= _temp1627; _temp1628.base= _temp1627;
_temp1628.last_plus_one= _temp1627 + 50; _temp1628;})); _LL1605:;} return
res_typ;}} _LL1539: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, rgn,
Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); void* t= Cyc_Tcexp_tcExp(
te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1629=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1629[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1630; _temp1630.tag= Cyc_Absyn_Upper_b; _temp1630.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1630;}); _temp1629;}));} _LL1541: { void** topt2=
0; if( topt != 0){ void* _temp1632= Cyc_Tcutil_compress(({ void** _temp1631=
topt; if( _temp1631 == 0){ _throw( Null_Exception);}* _temp1631;})); struct Cyc_Absyn_PtrInfo
_temp1640; struct Cyc_Absyn_Conref* _temp1642; struct Cyc_Absyn_Tqual _temp1644;
struct Cyc_Absyn_Conref* _temp1646; void* _temp1648; void* _temp1650; void**
_temp1652; struct Cyc_Absyn_TunionInfo _temp1653; _LL1634: if(( unsigned int)
_temp1632 > 4u?*(( int*) _temp1632) == Cyc_Absyn_PointerType: 0){ _LL1641:
_temp1640=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1632)->f1; _LL1651: _temp1650=( void*) _temp1640.elt_typ; _temp1652=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1632)->f1).elt_typ; goto _LL1649;
_LL1649: _temp1648=( void*) _temp1640.rgn_typ; goto _LL1647; _LL1647: _temp1646=(
struct Cyc_Absyn_Conref*) _temp1640.nullable; goto _LL1645; _LL1645: _temp1644=(
struct Cyc_Absyn_Tqual) _temp1640.tq; goto _LL1643; _LL1643: _temp1642=( struct
Cyc_Absyn_Conref*) _temp1640.bounds; goto _LL1635;} else{ goto _LL1636;} _LL1636:
if(( unsigned int) _temp1632 > 4u?*(( int*) _temp1632) == Cyc_Absyn_TunionType:
0){ _LL1654: _temp1653=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1632)->f1; goto _LL1637;} else{ goto _LL1638;} _LL1638: goto _LL1639;
_LL1635: topt2=( void**) _temp1652; goto _LL1633; _LL1637: topt2=({ void**
_temp1655=( void**) GC_malloc( sizeof( void*)); _temp1655[ 0]=({ void**
_temp1656= topt; if( _temp1656 == 0){ _throw( Null_Exception);}* _temp1656;});
_temp1655;}); goto _LL1633; _LL1639: goto _LL1633; _LL1633:;}{ void* telt= Cyc_Tcexp_tcExp(
te, topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1662=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1662[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1663; _temp1663.tag=
Cyc_Absyn_PointerType; _temp1663.f1=({ struct Cyc_Absyn_PtrInfo _temp1664;
_temp1664.elt_typ=( void*) telt; _temp1664.rgn_typ=( void*) rgn; _temp1664.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1664.tq= Cyc_Absyn_empty_tqual();
_temp1664.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1665=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1665[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1666; _temp1666.tag= Cyc_Absyn_Upper_b; _temp1666.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1666;}); _temp1665;})); _temp1664;}); _temp1663;});
_temp1662;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1657= topt;
if( _temp1657 == 0){ _throw( Null_Exception);}* _temp1657;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1658= topt; if( _temp1658 == 0){ _throw(
Null_Exception);}* _temp1658;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1659=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1659->v=(
void*) res_typ; _temp1659;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1660= topt; if( _temp1660 == 0){ _throw( Null_Exception);}* _temp1660;}));
res_typ=({ void** _temp1661= topt; if( _temp1661 == 0){ _throw( Null_Exception);}*
_temp1661;});}} return res_typ;}} _LL1531:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1667= t; struct Cyc_Absyn_Exp*
_temp1673; struct Cyc_Absyn_Tqual _temp1675; void* _temp1677; _LL1669: if((
unsigned int) _temp1667 > 4u?*(( int*) _temp1667) == Cyc_Absyn_ArrayType: 0){
_LL1678: _temp1677=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1667)->f1;
goto _LL1676; _LL1676: _temp1675=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1667)->f2; goto _LL1674; _LL1674: _temp1673=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1667)->f3; goto _LL1670;} else{ goto
_LL1671;} _LL1671: goto _LL1672; _LL1670: { void* _temp1681; int _temp1683;
struct _tuple5 _temp1679= Cyc_Tcutil_addressof_props( te, e); _LL1684: _temp1683=
_temp1679.f1; goto _LL1682; _LL1682: _temp1681= _temp1679.f2; goto _LL1680;
_LL1680: { void* b= _temp1673 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1685=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1685[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1686; _temp1686.tag= Cyc_Absyn_Upper_b; _temp1686.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1687= _temp1673; if(
_temp1687 == 0){ _throw( Null_Exception);} _temp1687;}); _temp1686;}); _temp1685;});
t= Cyc_Absyn_atb_typ( _temp1677, _temp1681, _temp1675, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1672: return t; _LL1668:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1688=( void*) e->r; struct Cyc_List_List* _temp1698;
struct Cyc_Absyn_Exp* _temp1700; struct Cyc_Absyn_Exp* _temp1702; struct Cyc_Absyn_Vardecl*
_temp1704; void* _temp1706; struct _tagged_string _temp1708; _LL1690: if(*(( int*)
_temp1688) == Cyc_Absyn_Array_e){ _LL1699: _temp1698=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1688)->f1; goto _LL1691;} else{ goto
_LL1692;} _LL1692: if(*(( int*) _temp1688) == Cyc_Absyn_Comprehension_e){
_LL1705: _temp1704=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1688)->f1; goto _LL1703; _LL1703: _temp1702=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1688)->f2; goto _LL1701; _LL1701:
_temp1700=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1688)->f3; goto _LL1693;} else{ goto _LL1694;} _LL1694: if(*(( int*)
_temp1688) == Cyc_Absyn_Const_e){ _LL1707: _temp1706=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1688)->f1; if(( unsigned int) _temp1706 > 1u?*(( int*) _temp1706) == Cyc_Absyn_String_c:
0){ _LL1709: _temp1708=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1706)->f1; goto _LL1695;} else{ goto _LL1696;}} else{ goto _LL1696;}
_LL1696: goto _LL1697; _LL1691: return t; _LL1693: return t; _LL1695: return t;
_LL1697: t= Cyc_Tcutil_compress( t);{ void* _temp1710= t; struct Cyc_Absyn_Exp*
_temp1716; struct Cyc_Absyn_Tqual _temp1718; void* _temp1720; _LL1712: if((
unsigned int) _temp1710 > 4u?*(( int*) _temp1710) == Cyc_Absyn_ArrayType: 0){
_LL1721: _temp1720=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1710)->f1;
goto _LL1719; _LL1719: _temp1718=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1710)->f2; goto _LL1717; _LL1717: _temp1716=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1710)->f3; goto _LL1713;} else{ goto
_LL1714;} _LL1714: goto _LL1715; _LL1713: { void* _temp1724; int _temp1726;
struct _tuple5 _temp1722= Cyc_Tcutil_addressof_props( te, e); _LL1727: _temp1726=
_temp1722.f1; goto _LL1725; _LL1725: _temp1724= _temp1722.f2; goto _LL1723;
_LL1723: { void* b= _temp1716 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1728=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1728[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1729; _temp1729.tag= Cyc_Absyn_Upper_b; _temp1729.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1730= _temp1716; if(
_temp1730 == 0){ _throw( Null_Exception);} _temp1730;}); _temp1729;}); _temp1728;});
t= Cyc_Absyn_atb_typ( _temp1720, _temp1724, _temp1718, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1715: return t; _LL1711:;} _LL1689:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1731=( void*) e->r; struct Cyc_Absyn_Exp* _temp1737; _LL1733: if(*(( int*)
_temp1731) == Cyc_Absyn_NoInstantiate_e){ _LL1738: _temp1737=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1731)->f1; goto _LL1734;} else{
goto _LL1735;} _LL1735: goto _LL1736; _LL1734: Cyc_Tcexp_tcExpNoInst( te, topt,
_temp1737);( void*)({ struct Cyc_Core_Opt* _temp1739= _temp1737->topt; if(
_temp1739 == 0){ _throw( Null_Exception);} _temp1739->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1740= _temp1737->topt;
if( _temp1740 == 0){ _throw( Null_Exception);} _temp1740->v;})));}); e->topt=
_temp1737->topt; goto _LL1732; _LL1736: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)({ struct Cyc_Core_Opt* _temp1741= e->topt; if( _temp1741 == 0){ _throw(
Null_Exception);} _temp1741->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1742= e->topt; if( _temp1742 == 0){ _throw(
Null_Exception);} _temp1742->v;})));});{ void* _temp1744= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1743= e->topt; if( _temp1743 == 0){ _throw(
Null_Exception);} _temp1743->v;})); struct Cyc_Absyn_PtrInfo _temp1750; struct
Cyc_Absyn_Conref* _temp1752; struct Cyc_Absyn_Tqual _temp1754; struct Cyc_Absyn_Conref*
_temp1756; void* _temp1758; void* _temp1760; _LL1746: if(( unsigned int)
_temp1744 > 4u?*(( int*) _temp1744) == Cyc_Absyn_PointerType: 0){ _LL1751:
_temp1750=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1744)->f1; _LL1761: _temp1760=( void*) _temp1750.elt_typ; goto _LL1759;
_LL1759: _temp1758=( void*) _temp1750.rgn_typ; goto _LL1757; _LL1757: _temp1756=(
struct Cyc_Absyn_Conref*) _temp1750.nullable; goto _LL1755; _LL1755: _temp1754=(
struct Cyc_Absyn_Tqual) _temp1750.tq; goto _LL1753; _LL1753: _temp1752=( struct
Cyc_Absyn_Conref*) _temp1750.bounds; goto _LL1747;} else{ goto _LL1748;} _LL1748:
goto _LL1749; _LL1747:{ void* _temp1762= Cyc_Tcutil_compress( _temp1760); struct
Cyc_Absyn_FnInfo _temp1768; struct Cyc_List_List* _temp1770; int _temp1772;
struct Cyc_List_List* _temp1774; void* _temp1776; struct Cyc_Core_Opt* _temp1778;
struct Cyc_List_List* _temp1780; _LL1764: if(( unsigned int) _temp1762 > 4u?*((
int*) _temp1762) == Cyc_Absyn_FnType: 0){ _LL1769: _temp1768=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1762)->f1; _LL1781: _temp1780=( struct Cyc_List_List*)
_temp1768.tvars; goto _LL1779; _LL1779: _temp1778=( struct Cyc_Core_Opt*)
_temp1768.effect; goto _LL1777; _LL1777: _temp1776=( void*) _temp1768.ret_typ;
goto _LL1775; _LL1775: _temp1774=( struct Cyc_List_List*) _temp1768.args; goto
_LL1773; _LL1773: _temp1772=( int) _temp1768.varargs; goto _LL1771; _LL1771:
_temp1770=( struct Cyc_List_List*) _temp1768.attributes; goto _LL1765;} else{
goto _LL1766;} _LL1766: goto _LL1767; _LL1765: if( _temp1780 != 0){ struct
_RegionHandle _temp1782= _new_region(); struct _RegionHandle* rgn=& _temp1782;
_push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1780); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1789=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1789[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1790; _temp1790.tag= Cyc_Absyn_FnType; _temp1790.f1=({ struct Cyc_Absyn_FnInfo
_temp1791; _temp1791.tvars= 0; _temp1791.effect= _temp1778; _temp1791.ret_typ=(
void*) _temp1776; _temp1791.args= _temp1774; _temp1791.varargs= _temp1772;
_temp1791.attributes= _temp1770; _temp1791;}); _temp1790;}); _temp1789;})); void*
new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1786=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1786[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1787; _temp1787.tag= Cyc_Absyn_PointerType;
_temp1787.f1=({ struct Cyc_Absyn_PtrInfo _temp1788; _temp1788.elt_typ=( void*)
ftyp; _temp1788.rgn_typ=( void*) _temp1758; _temp1788.nullable= _temp1756;
_temp1788.tq= _temp1754; _temp1788.bounds= _temp1752; _temp1788;}); _temp1787;});
_temp1786;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1783=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1783[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1784; _temp1784.tag= Cyc_Absyn_Instantiate_e;
_temp1784.f1= inner; _temp1784.f2= ts; _temp1784;}); _temp1783;}))); e->topt=({
struct Cyc_Core_Opt* _temp1785=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1785->v=( void*) new_typ; _temp1785;});}; _pop_region(&
_temp1782);} goto _LL1763; _LL1767: goto _LL1763; _LL1763:;} goto _LL1745;
_LL1749: goto _LL1745; _LL1745:;} goto _LL1732; _LL1732:;} return( void*)({
struct Cyc_Core_Opt* _temp1792= e->topt; if( _temp1792 == 0){ _throw(
Null_Exception);} _temp1792->v;});} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1793=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1865; struct _tuple1* _temp1867; struct Cyc_List_List* _temp1869; struct
Cyc_Absyn_Exp* _temp1871; struct Cyc_List_List* _temp1873; struct Cyc_Core_Opt*
_temp1875; void* _temp1877; void* _temp1879; struct _tuple1* _temp1881; struct
Cyc_List_List* _temp1883; void* _temp1885; void* _temp1887; struct Cyc_Absyn_Exp*
_temp1889; struct Cyc_Absyn_Exp* _temp1891; struct Cyc_Core_Opt* _temp1893;
struct Cyc_Absyn_Exp* _temp1895; struct Cyc_Absyn_Exp* _temp1897; struct Cyc_Absyn_Exp*
_temp1899; struct Cyc_Absyn_Exp* _temp1901; struct Cyc_Absyn_Exp* _temp1903;
struct Cyc_Absyn_Exp* _temp1905; struct Cyc_List_List* _temp1907; struct Cyc_Absyn_Exp*
_temp1909; struct Cyc_Absyn_Exp* _temp1911; struct Cyc_List_List* _temp1913;
struct Cyc_Absyn_Exp* _temp1915; struct Cyc_Absyn_Exp* _temp1917; void*
_temp1919; struct Cyc_Absyn_Exp* _temp1921; struct Cyc_Absyn_Exp* _temp1923;
struct Cyc_Absyn_Exp* _temp1925; struct Cyc_Absyn_Exp* _temp1927; void*
_temp1929; struct Cyc_Absyn_Exp* _temp1931; struct _tagged_string* _temp1933;
struct Cyc_Absyn_Exp* _temp1935; struct _tagged_string* _temp1937; struct Cyc_Absyn_Exp*
_temp1939; struct Cyc_Absyn_Exp* _temp1941; struct Cyc_Absyn_Exp* _temp1943;
struct Cyc_List_List* _temp1945; struct Cyc_List_List* _temp1947; struct _tuple2*
_temp1949; struct Cyc_List_List* _temp1951; struct Cyc_Absyn_Stmt* _temp1953;
struct Cyc_Absyn_Fndecl* _temp1955; struct Cyc_Absyn_Exp* _temp1957; struct Cyc_Absyn_Exp*
_temp1959; struct Cyc_Absyn_Exp* _temp1961; struct Cyc_Absyn_Vardecl* _temp1963;
struct Cyc_Absyn_Structdecl* _temp1965; struct Cyc_Absyn_Structdecl** _temp1967;
struct Cyc_List_List* _temp1968; struct Cyc_Core_Opt* _temp1970; struct Cyc_Core_Opt**
_temp1972; struct _tuple1* _temp1973; struct _tuple1** _temp1975; struct Cyc_List_List*
_temp1976; void* _temp1978; struct Cyc_Absyn_Tunionfield* _temp1980; struct Cyc_Absyn_Tuniondecl*
_temp1982; struct Cyc_List_List* _temp1984; struct Cyc_Core_Opt* _temp1986;
struct Cyc_Core_Opt** _temp1988; struct Cyc_Core_Opt* _temp1989; struct Cyc_Core_Opt**
_temp1991; struct Cyc_Absyn_Enumfield* _temp1992; struct Cyc_Absyn_Enumdecl*
_temp1994; struct _tuple1* _temp1996; void* _temp1998; struct Cyc_Absyn_Exp*
_temp2000; _LL1795: if(*(( int*) _temp1793) == Cyc_Absyn_NoInstantiate_e){
_LL1866: _temp1865=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1793)->f1; goto _LL1796;} else{ goto _LL1797;} _LL1797: if(*(( int*)
_temp1793) == Cyc_Absyn_UnknownId_e){ _LL1868: _temp1867=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1793)->f1; goto _LL1798;} else{ goto
_LL1799;} _LL1799: if(*(( int*) _temp1793) == Cyc_Absyn_UnknownCall_e){ _LL1872:
_temp1871=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1793)->f1; goto _LL1870; _LL1870: _temp1869=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1793)->f2; goto _LL1800;} else{
goto _LL1801;} _LL1801: if(*(( int*) _temp1793) == Cyc_Absyn_UnresolvedMem_e){
_LL1876: _temp1875=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1793)->f1; goto _LL1874; _LL1874: _temp1873=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1793)->f2; goto _LL1802;} else{
goto _LL1803;} _LL1803: if(*(( int*) _temp1793) == Cyc_Absyn_Const_e){ _LL1878:
_temp1877=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1793)->f1; goto
_LL1804;} else{ goto _LL1805;} _LL1805: if(*(( int*) _temp1793) == Cyc_Absyn_Var_e){
_LL1882: _temp1881=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1793)->f1; goto _LL1880; _LL1880: _temp1879=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1793)->f2; goto _LL1806;} else{ goto _LL1807;} _LL1807: if(*(( int*)
_temp1793) == Cyc_Absyn_Primop_e){ _LL1886: _temp1885=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1793)->f1; goto _LL1884; _LL1884: _temp1883=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp1793)->f2; goto _LL1808;} else{ goto
_LL1809;} _LL1809: if(*(( int*) _temp1793) == Cyc_Absyn_Increment_e){ _LL1890:
_temp1889=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1793)->f1; goto _LL1888; _LL1888: _temp1887=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1793)->f2; goto _LL1810;} else{ goto _LL1811;} _LL1811: if(*(( int*)
_temp1793) == Cyc_Absyn_AssignOp_e){ _LL1896: _temp1895=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1793)->f1; goto _LL1894; _LL1894:
_temp1893=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1793)->f2; goto _LL1892; _LL1892: _temp1891=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1793)->f3; goto _LL1812;} else{ goto
_LL1813;} _LL1813: if(*(( int*) _temp1793) == Cyc_Absyn_Conditional_e){ _LL1902:
_temp1901=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1793)->f1; goto _LL1900; _LL1900: _temp1899=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1793)->f2; goto _LL1898; _LL1898:
_temp1897=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1793)->f3; goto _LL1814;} else{ goto _LL1815;} _LL1815: if(*(( int*)
_temp1793) == Cyc_Absyn_SeqExp_e){ _LL1906: _temp1905=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1793)->f1; goto _LL1904; _LL1904:
_temp1903=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1793)->f2; goto _LL1816;} else{ goto _LL1817;} _LL1817: if(*(( int*)
_temp1793) == Cyc_Absyn_FnCall_e){ _LL1910: _temp1909=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1793)->f1; goto _LL1908; _LL1908:
_temp1907=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1793)->f2; goto _LL1818;} else{ goto _LL1819;} _LL1819: if(*(( int*)
_temp1793) == Cyc_Absyn_Throw_e){ _LL1912: _temp1911=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Throw_e_struct*) _temp1793)->f1; goto _LL1820;} else{ goto
_LL1821;} _LL1821: if(*(( int*) _temp1793) == Cyc_Absyn_Instantiate_e){ _LL1916:
_temp1915=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1793)->f1; goto _LL1914; _LL1914: _temp1913=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1793)->f2; goto _LL1822;} else{
goto _LL1823;} _LL1823: if(*(( int*) _temp1793) == Cyc_Absyn_Cast_e){ _LL1920:
_temp1919=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1793)->f1; goto
_LL1918; _LL1918: _temp1917=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1793)->f2; goto _LL1824;} else{ goto _LL1825;} _LL1825: if(*(( int*)
_temp1793) == Cyc_Absyn_Address_e){ _LL1922: _temp1921=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1793)->f1; goto _LL1826;} else{ goto
_LL1827;} _LL1827: if(*(( int*) _temp1793) == Cyc_Absyn_New_e){ _LL1926:
_temp1925=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1793)->f1;
goto _LL1924; _LL1924: _temp1923=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1793)->f2; goto _LL1828;} else{ goto _LL1829;} _LL1829: if(*(( int*)
_temp1793) == Cyc_Absyn_Sizeofexp_e){ _LL1928: _temp1927=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1793)->f1; goto _LL1830;} else{ goto
_LL1831;} _LL1831: if(*(( int*) _temp1793) == Cyc_Absyn_Sizeoftyp_e){ _LL1930:
_temp1929=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1793)->f1; goto
_LL1832;} else{ goto _LL1833;} _LL1833: if(*(( int*) _temp1793) == Cyc_Absyn_Deref_e){
_LL1932: _temp1931=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1793)->f1; goto _LL1834;} else{ goto _LL1835;} _LL1835: if(*(( int*)
_temp1793) == Cyc_Absyn_StructMember_e){ _LL1936: _temp1935=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1793)->f1; goto _LL1934; _LL1934:
_temp1933=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1793)->f2; goto _LL1836;} else{ goto _LL1837;} _LL1837: if(*(( int*)
_temp1793) == Cyc_Absyn_StructArrow_e){ _LL1940: _temp1939=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1793)->f1; goto _LL1938; _LL1938:
_temp1937=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1793)->f2; goto _LL1838;} else{ goto _LL1839;} _LL1839: if(*(( int*)
_temp1793) == Cyc_Absyn_Subscript_e){ _LL1944: _temp1943=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1793)->f1; goto _LL1942; _LL1942:
_temp1941=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1793)->f2; goto _LL1840;} else{ goto _LL1841;} _LL1841: if(*(( int*)
_temp1793) == Cyc_Absyn_Tuple_e){ _LL1946: _temp1945=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp1793)->f1; goto _LL1842;} else{ goto
_LL1843;} _LL1843: if(*(( int*) _temp1793) == Cyc_Absyn_CompoundLit_e){ _LL1950:
_temp1949=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1793)->f1;
goto _LL1948; _LL1948: _temp1947=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1793)->f2; goto _LL1844;} else{ goto _LL1845;} _LL1845: if(*(( int*)
_temp1793) == Cyc_Absyn_Array_e){ _LL1952: _temp1951=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1793)->f1; goto _LL1846;} else{ goto
_LL1847;} _LL1847: if(*(( int*) _temp1793) == Cyc_Absyn_StmtExp_e){ _LL1954:
_temp1953=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1793)->f1; goto _LL1848;} else{ goto _LL1849;} _LL1849: if(*(( int*)
_temp1793) == Cyc_Absyn_Codegen_e){ _LL1956: _temp1955=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1793)->f1; goto _LL1850;} else{ goto
_LL1851;} _LL1851: if(*(( int*) _temp1793) == Cyc_Absyn_Fill_e){ _LL1958:
_temp1957=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1793)->f1;
goto _LL1852;} else{ goto _LL1853;} _LL1853: if(*(( int*) _temp1793) == Cyc_Absyn_Comprehension_e){
_LL1964: _temp1963=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1793)->f1; goto _LL1962; _LL1962: _temp1961=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1793)->f2; goto _LL1960; _LL1960:
_temp1959=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1793)->f3; goto _LL1854;} else{ goto _LL1855;} _LL1855: if(*(( int*)
_temp1793) == Cyc_Absyn_Struct_e){ _LL1974: _temp1973=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1793)->f1; _temp1975=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1793)->f1; goto _LL1971; _LL1971: _temp1970=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1793)->f2; _temp1972=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1793)->f2; goto _LL1969; _LL1969: _temp1968=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1793)->f3; goto _LL1966; _LL1966:
_temp1965=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1793)->f4; _temp1967=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1793)->f4;
goto _LL1856;} else{ goto _LL1857;} _LL1857: if(*(( int*) _temp1793) == Cyc_Absyn_AnonStruct_e){
_LL1979: _temp1978=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1793)->f1;
goto _LL1977; _LL1977: _temp1976=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1793)->f2; goto _LL1858;} else{ goto _LL1859;} _LL1859: if(*(( int*)
_temp1793) == Cyc_Absyn_Tunion_e){ _LL1990: _temp1989=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1793)->f1; _temp1991=&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1793)->f1; goto _LL1987; _LL1987: _temp1986=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1793)->f2; _temp1988=&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1793)->f2; goto _LL1985; _LL1985: _temp1984=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1793)->f3; goto _LL1983; _LL1983:
_temp1982=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1793)->f4; goto _LL1981; _LL1981: _temp1980=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1793)->f5; goto _LL1860;} else{ goto
_LL1861;} _LL1861: if(*(( int*) _temp1793) == Cyc_Absyn_Enum_e){ _LL1997:
_temp1996=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1793)->f1;
goto _LL1995; _LL1995: _temp1994=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1793)->f2; goto _LL1993; _LL1993: _temp1992=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1793)->f3; goto _LL1862;} else{ goto
_LL1863;} _LL1863: if(*(( int*) _temp1793) == Cyc_Absyn_Malloc_e){ _LL2001:
_temp2000=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1793)->f1; goto _LL1999; _LL1999: _temp1998=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1793)->f2; goto _LL1864;} else{ goto _LL1794;} _LL1796: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1865); return; _LL1798: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1867); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1800: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1871, _temp1869); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1802: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1873); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1804: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1877, e);
goto _LL1794; _LL1806: t= Cyc_Tcexp_tcVar( te, loc, _temp1881, _temp1879); goto
_LL1794; _LL1808: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1885, _temp1883);
goto _LL1794; _LL1810: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1889,
_temp1887); goto _LL1794; _LL1812: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1895, _temp1893, _temp1891); goto _LL1794; _LL1814: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1901, _temp1899, _temp1897); goto _LL1794; _LL1816: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1905, _temp1903); goto _LL1794; _LL1818: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1909, _temp1907); goto _LL1794; _LL1820: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1911); goto _LL1794; _LL1822: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1915, _temp1913); goto _LL1794; _LL1824: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1919, _temp1917); goto _LL1794; _LL1826: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1921); goto _LL1794; _LL1828: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1925, e, _temp1923); goto _LL1794; _LL1830: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1927); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1794;}
_LL1832: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1929); goto _LL1794; _LL1834:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1931); goto _LL1794; _LL1836: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1935, _temp1933); goto _LL1794; _LL1838: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1939, _temp1937); goto _LL1794; _LL1840: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1943, _temp1941); goto _LL1794; _LL1842: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1945); goto _LL1794; _LL1844: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1949, _temp1947); goto _LL1794; _LL1846: { void** elt_topt= 0;
if( topt != 0){ void* _temp2003= Cyc_Tcutil_compress(({ void** _temp2002= topt;
if( _temp2002 == 0){ _throw( Null_Exception);}* _temp2002;})); struct Cyc_Absyn_Exp*
_temp2009; struct Cyc_Absyn_Tqual _temp2011; void* _temp2013; void** _temp2015;
_LL2005: if(( unsigned int) _temp2003 > 4u?*(( int*) _temp2003) == Cyc_Absyn_ArrayType:
0){ _LL2014: _temp2013=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2003)->f1;
_temp2015=&(( struct Cyc_Absyn_ArrayType_struct*) _temp2003)->f1; goto _LL2012;
_LL2012: _temp2011=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2003)->f2; goto _LL2010; _LL2010: _temp2009=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2003)->f3; goto _LL2006;} else{ goto
_LL2007;} _LL2007: goto _LL2008; _LL2006: elt_topt=( void**) _temp2015; goto
_LL2004; _LL2008: goto _LL2004; _LL2004:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1951); goto _LL1794;} _LL1848: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1953); goto _LL1794; _LL1850: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1955); goto _LL1794; _LL1852: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1957);
goto _LL1794; _LL1854: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1963,
_temp1961, _temp1959); goto _LL1794; _LL1856: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1975, _temp1972, _temp1968, _temp1967); goto _LL1794; _LL1858: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1978, _temp1976); goto _LL1794; _LL1860: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1991, _temp1988, _temp1984, _temp1982, _temp1980); goto
_LL1794; _LL1862: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp2016=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2016[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2017; _temp2017.tag= Cyc_Absyn_EnumType;
_temp2017.f1=({ struct Cyc_Absyn_Enumdecl* _temp2018= _temp1994; if( _temp2018
== 0){ _throw( Null_Exception);} _temp2018->name;}); _temp2017.f2= _temp1994;
_temp2017;}); _temp2016;}); goto _LL1794; _LL1864: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp2000, _temp1998); goto _LL1794; _LL1794:;} e->topt=({ struct Cyc_Core_Opt*
_temp2019=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2019->v=( void*) t; _temp2019;});}