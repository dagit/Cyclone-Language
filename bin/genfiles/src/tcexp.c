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
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
int _temp2= 0; if( setjmp( _temp0.handler)){ _temp2= 1;} if( ! _temp2){{ void*
_temp3= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void* _temp13; struct Cyc_Absyn_Enumfield*
_temp15; struct Cyc_Absyn_Enumdecl* _temp17; struct Cyc_Absyn_Tunionfield*
_temp19; struct Cyc_Absyn_Tuniondecl* _temp21; struct Cyc_Absyn_Structdecl*
_temp23; _LL5: if(*(( int*) _temp3) == Cyc_Tcenv_VarRes){ _LL14: _temp13=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7:
if(*(( int*) _temp3) == Cyc_Tcenv_EnumRes){ _LL18: _temp17=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp3)->f1; goto _LL16; _LL16: _temp15=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp3)->f2;
goto _LL8;} else{ goto _LL9;} _LL9: if(*(( int*) _temp3) == Cyc_Tcenv_TunionRes){
_LL22: _temp21=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp3)->f1; goto _LL20; _LL20: _temp19=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp3)->f2; goto _LL10;} else{ goto _LL11;}
_LL11: if(*(( int*) _temp3) == Cyc_Tcenv_StructRes){ _LL24: _temp23=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp3)->f1; goto _LL12;} else{ goto _LL4;}
_LL6:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp25=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
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
if(*(( int*) _temp44) == Cyc_Absyn_UnknownId_e){ _LL51: _temp50=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp44)->f1; goto _LL47;} else{ goto
_LL48;} _LL48: goto _LL49; _LL47: { struct _handler_cons _temp52; _push_handler(&
_temp52);{ int _temp54= 0; if( setjmp( _temp52.handler)){ _temp54= 1;} if( !
_temp54){{ void* _temp55= Cyc_Tcenv_lookup_ordinary( te, e1->loc, _temp50); void*
_temp65; struct Cyc_Absyn_Tunionfield* _temp67; struct Cyc_Absyn_Tuniondecl*
_temp69; struct Cyc_Absyn_Structdecl* _temp71; struct Cyc_Absyn_Enumfield*
_temp73; struct Cyc_Absyn_Enumdecl* _temp75; _LL57: if(*(( int*) _temp55) == Cyc_Tcenv_VarRes){
_LL66: _temp65=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp55)->f1; goto
_LL58;} else{ goto _LL59;} _LL59: if(*(( int*) _temp55) == Cyc_Tcenv_TunionRes){
_LL70: _temp69=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp55)->f1; goto _LL68; _LL68: _temp67=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp55)->f2; goto _LL60;} else{ goto _LL61;}
_LL61: if(*(( int*) _temp55) == Cyc_Tcenv_StructRes){ _LL72: _temp71=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*) _temp55)->f1; goto
_LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp55) == Cyc_Tcenv_EnumRes){
_LL76: _temp75=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp55)->f1; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp55)->f2; goto _LL64;} else{ goto _LL56;} _LL58:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp77=(
struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp77[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp78; _temp78.tag= Cyc_Absyn_FnCall_e;
_temp78.f1= e1; _temp78.f2= es; _temp78;}); _temp77;}))); _npop_handler( 0u);
return; _LL60: if( _temp67->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp79= Cyc_Absynpp_qvar2string( _temp67->name); xprintf("%.*s is a constant, not a function",
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
_temp85; _temp85.tag= Cyc_Absyn_Struct_e; _temp85.f1=( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp86= _temp71->name; if( _temp86 == 0){ _throw( Null_Exception);}
_temp86->v;}); _temp85.f2= 0; _temp85.f3= _temp82; _temp85.f4=( struct Cyc_Absyn_Structdecl*)
_temp71; _temp85;}); _temp84;})));} else{ Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ char* _temp87=( char*)"missing struct name"; struct
_tagged_string _temp88; _temp88.curr= _temp87; _temp88.base= _temp87; _temp88.last_plus_one=
_temp87 + 20; _temp88;}));( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp89=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp89[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp90; _temp90.tag= Cyc_Absyn_Struct_e;
_temp90.f1= _temp50; _temp90.f2= 0; _temp90.f3= _temp82; _temp90.f4=( struct Cyc_Absyn_Structdecl*)
_temp71; _temp90;}); _temp89;})));} _npop_handler( 0u); return;} _LL64: Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp91= Cyc_Absynpp_qvar2string( _temp50);
xprintf("%.*s is an enum constructor, not a function", _temp91.last_plus_one -
_temp91.curr, _temp91.curr);})); _npop_handler( 0u); return; _LL56:;};
_pop_handler();} else{ void* _temp53=( void*) _exn_thrown; void* _temp93=
_temp53; _LL95: if( _temp93 == Cyc_Dict_Absent){ goto _LL96;} else{ goto _LL97;}
_LL97: goto _LL98; _LL96: Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string
_temp99= Cyc_Absynpp_qvar2string( _temp50); xprintf("unbound identifier %.*s",
_temp99.last_plus_one - _temp99.curr, _temp99.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp100=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp100[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp101; _temp101.tag= Cyc_Absyn_FnCall_e; _temp101.f1=
e1; _temp101.f2= es; _temp101;}); _temp100;}))); return; _LL98:( void) _throw(
_temp93); _LL94:;}}} _LL49:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp102=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp102[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp103; _temp103.tag= Cyc_Absyn_FnCall_e;
_temp103.f1= e1; _temp103.f2= es; _temp103;}); _temp102;}))); return; _LL45:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp104=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp104[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp105; _temp105.tag= Cyc_Absyn_Array_e;
_temp105.f1= des; _temp105;}); _temp104;}))); return;}{ void* t=({ void**
_temp142= topt; if( _temp142 == 0){ _throw( Null_Exception);}* _temp142;}); void*
_temp106= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp116; struct Cyc_Absyn_Tqual
_temp118; void* _temp120; struct Cyc_Absyn_Structdecl** _temp122; struct Cyc_List_List*
_temp124; struct _tuple1* _temp126; struct Cyc_List_List* _temp128; _LL108: if((
unsigned int) _temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_ArrayType: 0){
_LL121: _temp120=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp106)->f1;
goto _LL119; _LL119: _temp118=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp106)->f2; goto _LL117; _LL117: _temp116=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp106)->f3; goto _LL109;} else{ goto _LL110;}
_LL110: if(( unsigned int) _temp106 > 4u?*(( int*) _temp106) == Cyc_Absyn_StructType:
0){ _LL127: _temp126=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp106)->f1; goto _LL125; _LL125: _temp124=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp106)->f2; goto _LL123; _LL123: _temp122=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp106)->f3;
goto _LL111;} else{ goto _LL112;} _LL112: if(( unsigned int) _temp106 > 4u?*((
int*) _temp106) == Cyc_Absyn_AnonStructType: 0){ _LL129: _temp128=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp106)->f1; goto _LL113;} else{ goto
_LL114;} _LL114: goto _LL115; _LL109:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp130=( struct Cyc_Absyn_Array_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp130[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp131; _temp131.tag= Cyc_Absyn_Array_e; _temp131.f1= des; _temp131;});
_temp130;}))); goto _LL107; _LL111: if( _temp122 == 0? 1: _temp126 == 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp132=( char*)"Compiler bug: struct type not properly set";
struct _tagged_string _temp133; _temp133.curr= _temp132; _temp133.base= _temp132;
_temp133.last_plus_one= _temp132 + 43; _temp133;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp134=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp134[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp135; _temp135.tag= Cyc_Absyn_Struct_e; _temp135.f1=(
struct _tuple1*)({ struct _tuple1* _temp136= _temp126; if( _temp136 == 0){
_throw( Null_Exception);} _temp136;}); _temp135.f2= 0; _temp135.f3= des;
_temp135.f4=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl**
_temp137= _temp122; if( _temp137 == 0){ _throw( Null_Exception);}* _temp137;});
_temp135;}); _temp134;}))); goto _LL107; _LL113:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_AnonStruct_e_struct* _temp138=( struct Cyc_Absyn_AnonStruct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct)); _temp138[ 0]=({
struct Cyc_Absyn_AnonStruct_e_struct _temp139; _temp139.tag= Cyc_Absyn_AnonStruct_e;
_temp139.f1=( void*) t; _temp139.f2= des; _temp139;}); _temp138;}))); goto
_LL107; _LL115:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp140=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp140[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp141; _temp141.tag= Cyc_Absyn_Array_e;
_temp141.f1= des; _temp141;}); _temp140;}))); goto _LL107; _LL107:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es=({ struct Cyc_List_List*
_temp143= es; if( _temp143 == 0){ _throw( Null_Exception);} _temp143->tl;})){
Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp144=
es; if( _temp144 == 0){ _throw( Null_Exception);} _temp144->hd;}));}} void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp( te,( void**)&
Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp146= msg_part; struct _tagged_string
_temp147= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp145= e->topt;
if( _temp145 == 0){ _throw( Null_Exception);} _temp145->v;})); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp146.last_plus_one - _temp146.curr, _temp146.curr, _temp147.last_plus_one -
_temp147.curr, _temp147.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp148= c; char _temp164; void* _temp166; short _temp168;
void* _temp170; long long _temp172; void* _temp174; struct _tagged_string
_temp176; int _temp178; void* _temp180; struct _tagged_string _temp182; _LL150:
if(( unsigned int) _temp148 > 1u?*(( int*) _temp148) == Cyc_Absyn_Char_c: 0){
_LL167: _temp166=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp148)->f1; goto
_LL165; _LL165: _temp164=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp148)->f2;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp148 > 1u?*((
int*) _temp148) == Cyc_Absyn_Short_c: 0){ _LL171: _temp170=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp148)->f1; goto _LL169; _LL169: _temp168=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp148)->f2; goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int)
_temp148 > 1u?*(( int*) _temp148) == Cyc_Absyn_LongLong_c: 0){ _LL175: _temp174=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp148)->f1; goto _LL173; _LL173:
_temp172=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp148)->f2; goto
_LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp148 > 1u?*(( int*)
_temp148) == Cyc_Absyn_Float_c: 0){ _LL177: _temp176=( struct _tagged_string)((
struct Cyc_Absyn_Float_c_struct*) _temp148)->f1; goto _LL157;} else{ goto _LL158;}
_LL158: if(( unsigned int) _temp148 > 1u?*(( int*) _temp148) == Cyc_Absyn_Int_c:
0){ _LL181: _temp180=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp148)->f1;
goto _LL179; _LL179: _temp178=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp148)->f2;
goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int) _temp148 > 1u?*((
int*) _temp148) == Cyc_Absyn_String_c: 0){ _LL183: _temp182=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp148)->f1; goto _LL161;}
else{ goto _LL162;} _LL162: if( _temp148 ==( void*) Cyc_Absyn_Null_c){ goto
_LL163;} else{ goto _LL149;} _LL151: t= _temp166 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL149; _LL153: t= _temp170 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL149; _LL155:
t= _temp174 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t;
goto _LL149; _LL157: t= Cyc_Absyn_float_t; goto _LL149; _LL159: if( topt == 0){
t= _temp180 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp185=({ void** _temp184= topt; if( _temp184 == 0){ _throw(
Null_Exception);}* _temp184;}); void* _temp195; void* _temp197; void* _temp199;
void* _temp201; void* _temp203; void* _temp205; _LL187: if(( unsigned int)
_temp185 > 4u?*(( int*) _temp185) == Cyc_Absyn_IntType: 0){ _LL198: _temp197=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp185)->f1; goto _LL196; _LL196:
_temp195=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp185)->f2; if(
_temp195 ==( void*) Cyc_Absyn_B1){ goto _LL188;} else{ goto _LL189;}} else{ goto
_LL189;} _LL189: if(( unsigned int) _temp185 > 4u?*(( int*) _temp185) == Cyc_Absyn_IntType:
0){ _LL202: _temp201=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp185)->f1;
goto _LL200; _LL200: _temp199=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp185)->f2; if( _temp199 ==( void*) Cyc_Absyn_B2){ goto _LL190;} else{ goto
_LL191;}} else{ goto _LL191;} _LL191: if(( unsigned int) _temp185 > 4u?*(( int*)
_temp185) == Cyc_Absyn_IntType: 0){ _LL206: _temp205=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp185)->f1; goto _LL204; _LL204: _temp203=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp185)->f2; goto _LL192;} else{ goto _LL193;} _LL193: goto _LL194; _LL188: t=
_temp197 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp207=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp207[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp208; _temp208.tag= Cyc_Absyn_Const_e;
_temp208.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp209=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp209[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp210; _temp210.tag= Cyc_Absyn_Char_c;
_temp210.f1=( void*) _temp197; _temp210.f2=( char) _temp178; _temp210;});
_temp209;})); _temp208;}); _temp207;}))); goto _LL186; _LL190: t= _temp201 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp211=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp211[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp212; _temp212.tag= Cyc_Absyn_Const_e; _temp212.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp213=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp213[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp214; _temp214.tag= Cyc_Absyn_Short_c; _temp214.f1=( void*) _temp201;
_temp214.f2=( short) _temp178; _temp214;}); _temp213;})); _temp212;}); _temp211;})));
goto _LL186; _LL192: t= _temp205 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL186; _LL194: t= _temp180 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL186; _LL186:;} goto _LL149; _LL161: {
int len=( int)({ struct _tagged_string _temp265= _temp182;( unsigned int)(
_temp265.last_plus_one - _temp265.curr);}); struct Cyc_Absyn_Const_e_struct*
_temp219=({ struct Cyc_Absyn_Const_e_struct* _temp215=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp215[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp216; _temp216.tag= Cyc_Absyn_Const_e; _temp216.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp217=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp217[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp218; _temp218.tag= Cyc_Absyn_Int_c; _temp218.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp218.f2= len; _temp218;}); _temp217;})); _temp216;}); _temp215;}); goto
_LL220; _LL220: { struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*)
_temp219, loc); elen->topt=({ struct Cyc_Core_Opt* _temp221=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp221->v=( void*) Cyc_Absyn_uint_t;
_temp221;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp222=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp222[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp223; _temp223.tag= Cyc_Absyn_Upper_b;
_temp223.f1= elen; _temp223;}); _temp222;})); if( topt != 0){ void* _temp225=
Cyc_Tcutil_compress(({ void** _temp224= topt; if( _temp224 == 0){ _throw(
Null_Exception);}* _temp224;})); struct Cyc_Absyn_Exp* _temp233; struct Cyc_Absyn_Tqual
_temp235; void* _temp237; struct Cyc_Absyn_PtrInfo _temp239; struct Cyc_Absyn_Conref*
_temp241; struct Cyc_Absyn_Tqual _temp243; struct Cyc_Absyn_Conref* _temp245;
void* _temp247; void* _temp249; _LL227: if(( unsigned int) _temp225 > 4u?*(( int*)
_temp225) == Cyc_Absyn_ArrayType: 0){ _LL238: _temp237=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp225)->f1; goto _LL236; _LL236: _temp235=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp225)->f2; goto _LL234; _LL234: _temp233=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp225)->f3; goto
_LL228;} else{ goto _LL229;} _LL229: if(( unsigned int) _temp225 > 4u?*(( int*)
_temp225) == Cyc_Absyn_PointerType: 0){ _LL240: _temp239=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp225)->f1; _LL250: _temp249=( void*)
_temp239.elt_typ; goto _LL248; _LL248: _temp247=( void*) _temp239.rgn_typ; goto
_LL246; _LL246: _temp245=( struct Cyc_Absyn_Conref*) _temp239.nullable; goto
_LL244; _LL244: _temp243=( struct Cyc_Absyn_Tqual) _temp239.tq; goto _LL242;
_LL242: _temp241=( struct Cyc_Absyn_Conref*) _temp239.bounds; goto _LL230;}
else{ goto _LL231;} _LL231: goto _LL232; _LL228: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp251=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp251[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp252; _temp252.tag= Cyc_Absyn_ArrayType;
_temp252.f1=( void*) Cyc_Absyn_uchar_t; _temp252.f2= _temp235; _temp252.f3=(
struct Cyc_Absyn_Exp*) elen; _temp252;}); _temp251;}); _LL230: if( ! Cyc_Tcutil_unify(({
void** _temp253= topt; if( _temp253 == 0){ _throw( Null_Exception);}* _temp253;}),
t)? Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp254= topt; if(
_temp254 == 0){ _throw( Null_Exception);}* _temp254;})): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp255=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp255->v=( void*) t; _temp255;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp256= topt; if( _temp256 == 0){ _throw( Null_Exception);}* _temp256;})); t=({
void** _temp257= topt; if( _temp257 == 0){ _throw( Null_Exception);}* _temp257;});}
else{ t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind),
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp258=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp258[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp259; _temp259.tag= Cyc_Absyn_Upper_b;
_temp259.f1= elen; _temp259;}); _temp258;})); if( ! Cyc_Tcutil_unify(({ void**
_temp260= topt; if( _temp260 == 0){ _throw( Null_Exception);}* _temp260;}), t)?
Cyc_Tcutil_silent_castable( te, loc, t,({ void** _temp261= topt; if( _temp261 ==
0){ _throw( Null_Exception);}* _temp261;})): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp262=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp262->v=( void*) t; _temp262;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp263= topt; if( _temp263 == 0){ _throw( Null_Exception);}* _temp263;})); t=({
void** _temp264= topt; if( _temp264 == 0){ _throw( Null_Exception);}* _temp264;});}}
goto _LL226; _LL232: goto _LL226; _LL226:;} return t;}} _LL163: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp266=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp266[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp267; _temp267.tag= Cyc_Absyn_PointerType;
_temp267.f1=({ struct Cyc_Absyn_PtrInfo _temp268; _temp268.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp268.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp268.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp268.tq= Cyc_Absyn_empty_tqual(); _temp268.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp268;}); _temp267;});
_temp266;}); goto _LL149; _LL149:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp269= b; struct Cyc_Absyn_Vardecl* _temp283; struct Cyc_Absyn_Fndecl*
_temp285; struct Cyc_Absyn_Vardecl* _temp287; struct Cyc_Absyn_Vardecl* _temp289;
struct Cyc_Absyn_Vardecl* _temp291; _LL271: if( _temp269 ==( void*) Cyc_Absyn_Unresolved_b){
goto _LL272;} else{ goto _LL273;} _LL273: if(( unsigned int) _temp269 > 1u?*((
int*) _temp269) == Cyc_Absyn_Global_b: 0){ _LL284: _temp283=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp269)->f1; goto _LL274;} else{ goto
_LL275;} _LL275: if(( unsigned int) _temp269 > 1u?*(( int*) _temp269) == Cyc_Absyn_Funname_b:
0){ _LL286: _temp285=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp269)->f1; goto _LL276;} else{ goto _LL277;} _LL277: if(( unsigned int)
_temp269 > 1u?*(( int*) _temp269) == Cyc_Absyn_Pat_b: 0){ _LL288: _temp287=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*) _temp269)->f1; goto
_LL278;} else{ goto _LL279;} _LL279: if(( unsigned int) _temp269 > 1u?*(( int*)
_temp269) == Cyc_Absyn_Local_b: 0){ _LL290: _temp289=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp269)->f1; goto _LL280;} else{ goto _LL281;}
_LL281: if(( unsigned int) _temp269 > 1u?*(( int*) _temp269) == Cyc_Absyn_Param_b:
0){ _LL292: _temp291=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp269)->f1; goto _LL282;} else{ goto _LL270;} _LL272: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp293= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp293.last_plus_one - _temp293.curr, _temp293.curr);})); _LL274:({ struct
_tuple1* _temp294= q; unsigned int _temp295= 0; if( _temp295 >= 1u){ _throw(
Null_Exception);} _temp294[ _temp295]=({ struct _tuple1* _temp296= _temp283->name;
unsigned int _temp297= 0; if( _temp297 >= 1u){ _throw( Null_Exception);}
_temp296[ _temp297];});}); return( void*) _temp283->type; _LL276:({ struct
_tuple1* _temp298= q; unsigned int _temp299= 0; if( _temp299 >= 1u){ _throw(
Null_Exception);} _temp298[ _temp299]=({ struct _tuple1* _temp300= _temp285->name;
unsigned int _temp301= 0; if( _temp301 >= 1u){ _throw( Null_Exception);}
_temp300[ _temp301];});}); return Cyc_Tcutil_fndecl2typ( _temp285); _LL278:
_temp289= _temp287; goto _LL280; _LL280: _temp291= _temp289; goto _LL282; _LL282:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp291->type; _LL270:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp302=( void*) fmt->r; void* _temp308; struct _tagged_string _temp310;
_LL304: if(*(( int*) _temp302) == Cyc_Absyn_Const_e){ _LL309: _temp308=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp302)->f1; if(( unsigned int) _temp308 > 1u?*((
int*) _temp308) == Cyc_Absyn_String_c: 0){ _LL311: _temp310=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp308)->f1; goto _LL305;}
else{ goto _LL306;}} else{ goto _LL306;} _LL306: goto _LL307; _LL305: desc_types=
type_getter( _temp310, fmt->loc); goto _LL303; _LL307: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp312=( char*)"expecting a literal format string";
struct _tagged_string _temp313; _temp313.curr= _temp312; _temp313.base= _temp312;
_temp313.last_plus_one= _temp312 + 34; _temp313;})); return; _LL303:;} for( 0;
desc_types != 0? args != 0: 0; desc_types=({ struct Cyc_List_List* _temp314=
desc_types; if( _temp314 == 0){ _throw( Null_Exception);} _temp314->tl;}), args=({
struct Cyc_List_List* _temp315= args; if( _temp315 == 0){ _throw( Null_Exception);}
_temp315->tl;})){ void* t=( void*)({ struct Cyc_List_List* _temp320= desc_types;
if( _temp320 == 0){ _throw( Null_Exception);} _temp320->hd;}); struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp319= args; if( _temp319
== 0){ _throw( Null_Exception);} _temp319->hd;}); if( ! Cyc_Tcutil_coerce_arg(
te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp317= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp318= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp316= e->topt; if( _temp316 == 0){ _throw( Null_Exception);} _temp316->v;}));
xprintf("descriptor has type %.*s but argument has type %.*s", _temp317.last_plus_one
- _temp317.curr, _temp317.curr, _temp318.last_plus_one - _temp318.curr, _temp318.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp321=( char*)"too few arguments"; struct _tagged_string _temp322; _temp322.curr=
_temp321; _temp322.base= _temp321; _temp322.last_plus_one= _temp321 + 18;
_temp322;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp323= args; if( _temp323 == 0){ _throw( Null_Exception);}
_temp323->hd;}))->loc,( struct _tagged_string)({ char* _temp324=( char*)"too many arguments";
struct _tagged_string _temp325; _temp325.curr= _temp324; _temp325.base= _temp324;
_temp325.last_plus_one= _temp324 + 19; _temp325;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp374= e->topt; if( _temp374 == 0){ _throw( Null_Exception);} _temp374->v;}));
void* _temp326= p; _LL328: if( _temp326 ==( void*) Cyc_Absyn_Plus){ goto _LL329;}
else{ goto _LL330;} _LL330: if( _temp326 ==( void*) Cyc_Absyn_Minus){ goto
_LL331;} else{ goto _LL332;} _LL332: if( _temp326 ==( void*) Cyc_Absyn_Not){
goto _LL333;} else{ goto _LL334;} _LL334: if( _temp326 ==( void*) Cyc_Absyn_Bitnot){
goto _LL335;} else{ goto _LL336;} _LL336: if( _temp326 ==( void*) Cyc_Absyn_Size){
goto _LL337;} else{ goto _LL338;} _LL338: goto _LL339; _LL329: goto _LL331;
_LL331: if( ! Cyc_Tcutil_is_numeric( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp340= Cyc_Absynpp_typ2string( t); xprintf("expecting arithmetic type but found %.*s",
_temp340.last_plus_one - _temp340.curr, _temp340.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp341= e->topt; if( _temp341 == 0){ _throw(
Null_Exception);} _temp341->v;}); _LL333: Cyc_Tcutil_check_contains_assign( e);
if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp342= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp342.last_plus_one - _temp342.curr, _temp342.curr);}));} return Cyc_Absyn_sint_t;
_LL335: if( ! Cyc_Tcutil_is_integral( e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp343= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp343.last_plus_one - _temp343.curr, _temp343.curr);}));} return( void*)({
struct Cyc_Core_Opt* _temp344= e->topt; if( _temp344 == 0){ _throw(
Null_Exception);} _temp344->v;}); _LL337:{ void* _temp345= t; struct Cyc_Absyn_Exp*
_temp353; struct Cyc_Absyn_Tqual _temp355; void* _temp357; struct Cyc_Absyn_PtrInfo
_temp359; struct Cyc_Absyn_Conref* _temp361; struct Cyc_Absyn_Tqual _temp363;
struct Cyc_Absyn_Conref* _temp365; void* _temp367; void* _temp369; _LL347: if((
unsigned int) _temp345 > 4u?*(( int*) _temp345) == Cyc_Absyn_ArrayType: 0){
_LL358: _temp357=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp345)->f1;
goto _LL356; _LL356: _temp355=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp345)->f2; goto _LL354; _LL354: _temp353=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp345)->f3; goto _LL348;} else{ goto _LL349;}
_LL349: if(( unsigned int) _temp345 > 4u?*(( int*) _temp345) == Cyc_Absyn_PointerType:
0){ _LL360: _temp359=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp345)->f1; _LL370: _temp369=( void*) _temp359.elt_typ; goto _LL368; _LL368:
_temp367=( void*) _temp359.rgn_typ; goto _LL366; _LL366: _temp365=( struct Cyc_Absyn_Conref*)
_temp359.nullable; goto _LL364; _LL364: _temp363=( struct Cyc_Absyn_Tqual)
_temp359.tq; goto _LL362; _LL362: _temp361=( struct Cyc_Absyn_Conref*) _temp359.bounds;
goto _LL350;} else{ goto _LL351;} _LL351: goto _LL352; _LL348: goto _LL346;
_LL350: goto _LL346; _LL352: Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp371= Cyc_Absynpp_typ2string( t); xprintf("size requires pointer or array type, not %.*s",
_temp371.last_plus_one - _temp371.curr, _temp371.curr);})); goto _LL346; _LL346:;}
return Cyc_Absyn_uint_t; _LL339: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp372=( char*)"Non-unary primop"; struct
_tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 17; _temp373;})); _LL327:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp376= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp375= e1->topt; if( _temp375 == 0){ _throw(
Null_Exception);} _temp375->v;})); xprintf("type %.*s cannot be used here",
_temp376.last_plus_one - _temp376.curr, _temp376.curr);})); return Cyc_Absyn_wildtyp();}
if( ! checker( e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp378=
Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp377= e2->topt; if(
_temp377 == 0){ _throw( Null_Exception);} _temp377->v;})); xprintf("type %.*s cannot be used here",
_temp378.last_plus_one - _temp378.curr, _temp378.curr);})); return Cyc_Absyn_wildtyp();}{
void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp380= e1->topt;
if( _temp380 == 0){ _throw( Null_Exception);} _temp380->v;})); void* t2= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp379= e2->topt; if( _temp379 == 0){ _throw(
Null_Exception);} _temp379->v;})); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)({ struct
Cyc_Core_Opt* _temp448= e1->topt; if( _temp448 == 0){ _throw( Null_Exception);}
_temp448->v;})); void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp447= e2->topt; if( _temp447 == 0){ _throw( Null_Exception);} _temp447->v;}));
void* _temp381= t1; struct Cyc_Absyn_PtrInfo _temp387; struct Cyc_Absyn_Conref*
_temp389; struct Cyc_Absyn_Tqual _temp391; struct Cyc_Absyn_Conref* _temp393;
void* _temp395; void* _temp397; _LL383: if(( unsigned int) _temp381 > 4u?*(( int*)
_temp381) == Cyc_Absyn_PointerType: 0){ _LL388: _temp387=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp381)->f1; _LL398: _temp397=( void*)
_temp387.elt_typ; goto _LL396; _LL396: _temp395=( void*) _temp387.rgn_typ; goto
_LL394; _LL394: _temp393=( struct Cyc_Absyn_Conref*) _temp387.nullable; goto
_LL392; _LL392: _temp391=( struct Cyc_Absyn_Tqual) _temp387.tq; goto _LL390;
_LL390: _temp389=( struct Cyc_Absyn_Conref*) _temp387.bounds; goto _LL384;}
else{ goto _LL385;} _LL385: goto _LL386; _LL384: if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp399= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int but found %.*s", _temp399.last_plus_one - _temp399.curr,
_temp399.curr);}));} _temp389=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp389);{ void* _temp400=( void*) _temp389->v;
void* _temp408; void* _temp410; struct Cyc_Absyn_Exp* _temp412; _LL402: if((
unsigned int) _temp400 > 1u?*(( int*) _temp400) == Cyc_Absyn_Eq_constr: 0){
_LL409: _temp408=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp400)->f1;
if( _temp408 ==( void*) Cyc_Absyn_Unknown_b){ goto _LL403;} else{ goto _LL404;}}
else{ goto _LL404;} _LL404: if(( unsigned int) _temp400 > 1u?*(( int*) _temp400)
== Cyc_Absyn_Eq_constr: 0){ _LL411: _temp410=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp400)->f1; if(( unsigned int) _temp410 > 1u?*(( int*) _temp410) == Cyc_Absyn_Upper_b:
0){ _LL413: _temp412=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp410)->f1; goto _LL405;} else{ goto _LL406;}} else{ goto _LL406;} _LL406:
goto _LL407; _LL403: return t1; _LL405: { unsigned int _temp414= Cyc_Evexp_eval_const_uint_exp(
_temp412); goto _LL415; _LL415: _temp393=(( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp393);{ int is_nullable;{
void* _temp416=( void*) _temp393->v; int _temp422; _LL418: if(( unsigned int)
_temp416 > 1u?*(( int*) _temp416) == Cyc_Absyn_Eq_constr: 0){ _LL423: _temp422=(
int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp416)->f1; goto _LL419;} else{
goto _LL420;} _LL420: goto _LL421; _LL419: if( _temp422){ struct Cyc_Absyn_PointerType_struct*
_temp427=({ struct Cyc_Absyn_PointerType_struct* _temp424=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp424[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp425; _temp425.tag= Cyc_Absyn_PointerType;
_temp425.f1=({ struct Cyc_Absyn_PtrInfo _temp426; _temp426.elt_typ=( void*)
_temp397; _temp426.rgn_typ=( void*) _temp395; _temp426.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp426.tq= _temp391; _temp426.bounds=
_temp389; _temp426;}); _temp425;}); _temp424;}); goto _LL428; _LL428: Cyc_Tcutil_warn(
e1->loc,({ struct _tagged_string _temp429= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp430= Cyc_Absynpp_typ2string(( void*) _temp427); xprintf("coercing from %.*s to %.*s",
_temp429.last_plus_one - _temp429.curr, _temp429.curr, _temp430.last_plus_one -
_temp430.curr, _temp430.curr);})); Cyc_Tcutil_unchecked_cast( te, e1,( void*)
_temp427);} goto _LL417; _LL421:( void*)( _temp393->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp431=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp431[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp432; _temp432.tag= Cyc_Absyn_Eq_constr; _temp432.f1=(
void*) 0; _temp432;}); _temp431;}))); goto _LL417; _LL417:;} if( Cyc_Tcutil_is_const_exp(
te, e2)){ unsigned int _temp433= Cyc_Evexp_eval_const_uint_exp( e2); goto _LL434;
_LL434: if( _temp414 < _temp433){ _temp433= _temp414; Cyc_Tcutil_warn( e1->loc,
xprintf("pointer arithmetic is out of bounds"));}{ struct Cyc_Absyn_Exp*
_temp435= Cyc_Absyn_uint_exp( _temp414 - _temp433, 0); goto _LL436; _LL436:
_temp435->topt=({ struct Cyc_Core_Opt* _temp437=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp437->v=( void*) Cyc_Absyn_uint_t;
_temp437;}); return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp438=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp438[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp439; _temp439.tag= Cyc_Absyn_PointerType;
_temp439.f1=({ struct Cyc_Absyn_PtrInfo _temp440; _temp440.elt_typ=( void*)
_temp397; _temp440.rgn_typ=( void*) _temp395; _temp440.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp440.tq= _temp391; _temp440.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp441=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp441[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp442; _temp442.tag= Cyc_Absyn_Upper_b; _temp442.f1= _temp435; _temp442;});
_temp441;})); _temp440;}); _temp439;}); _temp438;});}} else{ Cyc_Tcutil_terr( e1->loc,(
struct _tagged_string)({ char* _temp443=( char*)"cannot statically check pointer arithmetic.\n   suggest you cast the first expression to a ? type.";
struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base= _temp443;
_temp444.last_plus_one= _temp443 + 98; _temp444;})); return t1;}}} _LL407:( void*)(
_temp389->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp445=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp445[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp446; _temp446.tag= Cyc_Absyn_Eq_constr;
_temp446.f1=( void*)(( void*) Cyc_Absyn_Unknown_b); _temp446;}); _temp445;})));
return t1; _LL401:;} _LL386: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL382:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)({ struct Cyc_Core_Opt* _temp456=
e1->topt; if( _temp456 == 0){ _throw( Null_Exception);} _temp456->v;}); void* t2=(
void*)({ struct Cyc_Core_Opt* _temp455= e2->topt; if( _temp455 == 0){ _throw(
Null_Exception);} _temp455->v;}); if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){
if( Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){
Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp451= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp449= e1->topt; if( _temp449 == 0){ _throw(
Null_Exception);} _temp449->v;})); struct _tagged_string _temp452= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp450= e2->topt; if( _temp450 == 0){ _throw(
Null_Exception);} _temp450->v;})); xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp451.last_plus_one - _temp451.curr, _temp451.curr, _temp452.last_plus_one -
_temp452.curr, _temp452.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp453= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp454= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp453.last_plus_one - _temp453.curr, _temp453.curr, _temp454.last_plus_one -
_temp454.curr, _temp454.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_numeric);}} static void* Cyc_Tcexp_tcAnyBinop( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric( e1); int
e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp497= e1->topt; if( _temp497 == 0){ _throw(
Null_Exception);} _temp497->v;})); void* t2= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp496= e2->topt; if( _temp496 == 0){ _throw(
Null_Exception);} _temp496->v;})); if( e1_is_num? e2_is_num: 0){ return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)({ struct Cyc_Core_Opt*
_temp495= e1->topt; if( _temp495 == 0){ _throw( Null_Exception);} _temp495->v;});
void* t2=( void*)({ struct Cyc_Core_Opt* _temp494= e2->topt; if( _temp494 == 0){
_throw( Null_Exception);} _temp494->v;});{ struct _tuple0 _temp458=({ struct
_tuple0 _temp457; _temp457.f1= Cyc_Tcutil_compress( t1); _temp457.f2= Cyc_Tcutil_compress(
t2); _temp457;}); void* _temp464; struct Cyc_Absyn_PtrInfo _temp466; struct Cyc_Absyn_Conref*
_temp468; struct Cyc_Absyn_Tqual _temp470; struct Cyc_Absyn_Conref* _temp472;
void* _temp474; void* _temp476; void* _temp478; struct Cyc_Absyn_PtrInfo
_temp480; struct Cyc_Absyn_Conref* _temp482; struct Cyc_Absyn_Tqual _temp484;
struct Cyc_Absyn_Conref* _temp486; void* _temp488; void* _temp490; _LL460:
_LL479: _temp478= _temp458.f1; if(( unsigned int) _temp478 > 4u?*(( int*)
_temp478) == Cyc_Absyn_PointerType: 0){ _LL481: _temp480=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp478)->f1; _LL491: _temp490=( void*)
_temp480.elt_typ; goto _LL489; _LL489: _temp488=( void*) _temp480.rgn_typ; goto
_LL487; _LL487: _temp486=( struct Cyc_Absyn_Conref*) _temp480.nullable; goto
_LL485; _LL485: _temp484=( struct Cyc_Absyn_Tqual) _temp480.tq; goto _LL483;
_LL483: _temp482=( struct Cyc_Absyn_Conref*) _temp480.bounds; goto _LL465;}
else{ goto _LL462;} _LL465: _temp464= _temp458.f2; if(( unsigned int) _temp464 >
4u?*(( int*) _temp464) == Cyc_Absyn_PointerType: 0){ _LL467: _temp466=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp464)->f1; _LL477:
_temp476=( void*) _temp466.elt_typ; goto _LL475; _LL475: _temp474=( void*)
_temp466.rgn_typ; goto _LL473; _LL473: _temp472=( struct Cyc_Absyn_Conref*)
_temp466.nullable; goto _LL471; _LL471: _temp470=( struct Cyc_Absyn_Tqual)
_temp466.tq; goto _LL469; _LL469: _temp468=( struct Cyc_Absyn_Conref*) _temp466.bounds;
goto _LL461;} else{ goto _LL462;} _LL462: goto _LL463; _LL461: if( Cyc_Tcutil_unify(
_temp490, _temp476)){ return Cyc_Absyn_sint_t;} goto _LL459; _LL463: goto _LL459;
_LL459:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp492= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp493= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp492.last_plus_one - _temp492.curr, _temp492.curr, _temp493.last_plus_one -
_temp493.curr, _temp493.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp498= p; _LL500: if( _temp498 ==( void*) Cyc_Absyn_Plus){ goto _LL501;}
else{ goto _LL502;} _LL502: if( _temp498 ==( void*) Cyc_Absyn_Minus){ goto
_LL503;} else{ goto _LL504;} _LL504: if( _temp498 ==( void*) Cyc_Absyn_Times){
goto _LL505;} else{ goto _LL506;} _LL506: if( _temp498 ==( void*) Cyc_Absyn_Div){
goto _LL507;} else{ goto _LL508;} _LL508: if( _temp498 ==( void*) Cyc_Absyn_Mod){
goto _LL509;} else{ goto _LL510;} _LL510: if( _temp498 ==( void*) Cyc_Absyn_Bitand){
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp498 ==( void*) Cyc_Absyn_Bitor){
goto _LL513;} else{ goto _LL514;} _LL514: if( _temp498 ==( void*) Cyc_Absyn_Bitxor){
goto _LL515;} else{ goto _LL516;} _LL516: if( _temp498 ==( void*) Cyc_Absyn_Bitlshift){
goto _LL517;} else{ goto _LL518;} _LL518: if( _temp498 ==( void*) Cyc_Absyn_Bitlrshift){
goto _LL519;} else{ goto _LL520;} _LL520: if( _temp498 ==( void*) Cyc_Absyn_Bitarshift){
goto _LL521;} else{ goto _LL522;} _LL522: if( _temp498 ==( void*) Cyc_Absyn_Eq){
goto _LL523;} else{ goto _LL524;} _LL524: if( _temp498 ==( void*) Cyc_Absyn_Neq){
goto _LL525;} else{ goto _LL526;} _LL526: if( _temp498 ==( void*) Cyc_Absyn_Gt){
goto _LL527;} else{ goto _LL528;} _LL528: if( _temp498 ==( void*) Cyc_Absyn_Lt){
goto _LL529;} else{ goto _LL530;} _LL530: if( _temp498 ==( void*) Cyc_Absyn_Gte){
goto _LL531;} else{ goto _LL532;} _LL532: if( _temp498 ==( void*) Cyc_Absyn_Lte){
goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL501: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL503: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL505: goto _LL507;
_LL507: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL509: goto _LL511; _LL511: goto _LL513; _LL513: goto _LL515; _LL515: goto
_LL517; _LL517: goto _LL519; _LL519: goto _LL521; _LL521: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL523: goto _LL525; _LL525: goto _LL527;
_LL527: goto _LL529; _LL529: goto _LL531; _LL531: goto _LL533; _LL533: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL535:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp536=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp536[ 0]=({ struct Cyc_Core_Failure_struct
_temp537; _temp537.tag= Cyc_Core_Failure; _temp537.f1=( struct _tagged_string)({
char* _temp538=( char*)"bad binary primop"; struct _tagged_string _temp539;
_temp539.curr= _temp538; _temp539.base= _temp538; _temp539.last_plus_one=
_temp538 + 18; _temp539;}); _temp537;}); _temp536;})); _LL499:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp540= es; if(
_temp540 == 0){ _throw( Null_Exception);} _temp540->hd;}));} Cyc_Tcexp_tcExpList(
te, es);{ void* t;{ void* _temp541= p; _LL543: if( _temp541 ==( void*) Cyc_Absyn_Printf){
goto _LL544;} else{ goto _LL545;} _LL545: if( _temp541 ==( void*) Cyc_Absyn_Fprintf){
goto _LL546;} else{ goto _LL547;} _LL547: if( _temp541 ==( void*) Cyc_Absyn_Xprintf){
goto _LL548;} else{ goto _LL549;} _LL549: if( _temp541 ==( void*) Cyc_Absyn_Scanf){
goto _LL550;} else{ goto _LL551;} _LL551: if( _temp541 ==( void*) Cyc_Absyn_Fscanf){
goto _LL552;} else{ goto _LL553;} _LL553: if( _temp541 ==( void*) Cyc_Absyn_Sscanf){
goto _LL554;} else{ goto _LL555;} _LL555: goto _LL556; _LL544: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp557=(
char*)"missing format string in printf"; struct _tagged_string _temp558;
_temp558.curr= _temp557; _temp558.base= _temp557; _temp558.last_plus_one=
_temp557 + 32; _temp558;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp559= es; if( _temp559 == 0){ _throw( Null_Exception);}
_temp559->hd;}),({ struct Cyc_List_List* _temp560= es; if( _temp560 == 0){
_throw( Null_Exception);} _temp560->tl;}), Cyc_Formatstr_get_format_typs); t=(
void*) Cyc_Absyn_VoidType; goto _LL542; _LL546: if( es == 0? 1:({ struct Cyc_List_List*
_temp561= es; if( _temp561 == 0){ _throw( Null_Exception);} _temp561->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp562=(
char*)"too few arguments to fprintf"; struct _tagged_string _temp563; _temp563.curr=
_temp562; _temp563.base= _temp562; _temp563.last_plus_one= _temp562 + 29;
_temp563;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp565=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp564= es; if( _temp564 == 0){
_throw( Null_Exception);} _temp564->hd;}))->topt; if( _temp565 == 0){ _throw(
Null_Exception);} _temp565->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp566= es; if( _temp566 == 0){
_throw( Null_Exception);} _temp566->hd;}))->loc,({ struct _tagged_string
_temp569= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp568=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp567= es; if( _temp567 == 0){
_throw( Null_Exception);} _temp567->hd;}))->topt; if( _temp568 == 0){ _throw(
Null_Exception);} _temp568->v;})); xprintf("expecting FILE @ but found %.*s",
_temp569.last_plus_one - _temp569.curr, _temp569.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp571=({ struct Cyc_List_List*
_temp570= es; if( _temp570 == 0){ _throw( Null_Exception);} _temp570->tl;}); if(
_temp571 == 0){ _throw( Null_Exception);} _temp571->hd;}),({ struct Cyc_List_List*
_temp573=({ struct Cyc_List_List* _temp572= es; if( _temp572 == 0){ _throw(
Null_Exception);} _temp572->tl;}); if( _temp573 == 0){ _throw( Null_Exception);}
_temp573->tl;}), Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL542; _LL548: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp574=( char*)"missing format string in xprintf";
struct _tagged_string _temp575; _temp575.curr= _temp574; _temp575.base= _temp574;
_temp575.last_plus_one= _temp574 + 33; _temp575;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp576= es; if( _temp576
== 0){ _throw( Null_Exception);} _temp576->hd;}),({ struct Cyc_List_List*
_temp577= es; if( _temp577 == 0){ _throw( Null_Exception);} _temp577->tl;}), Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL542; _LL550: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp578=( char*)"missing format string in scanf";
struct _tagged_string _temp579; _temp579.curr= _temp578; _temp579.base= _temp578;
_temp579.last_plus_one= _temp578 + 31; _temp579;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp580= es; if( _temp580
== 0){ _throw( Null_Exception);} _temp580->hd;}),({ struct Cyc_List_List*
_temp581= es; if( _temp581 == 0){ _throw( Null_Exception);} _temp581->tl;}), Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL542; _LL552: if( es == 0? 1:({ struct Cyc_List_List*
_temp582= es; if( _temp582 == 0){ _throw( Null_Exception);} _temp582->tl;}) == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp583=(
char*)"too few arguments to fscanf"; struct _tagged_string _temp584; _temp584.curr=
_temp583; _temp584.base= _temp583; _temp584.last_plus_one= _temp583 + 28;
_temp584;}));} if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt* _temp586=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp585= es; if( _temp585 == 0){
_throw( Null_Exception);} _temp585->hd;}))->topt; if( _temp586 == 0){ _throw(
Null_Exception);} _temp586->v;}), Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp587= es; if( _temp587 == 0){
_throw( Null_Exception);} _temp587->hd;}))->loc,({ struct _tagged_string
_temp590= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt* _temp589=((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp588= es; if( _temp588 == 0){
_throw( Null_Exception);} _temp588->hd;}))->topt; if( _temp589 == 0){ _throw(
Null_Exception);} _temp589->v;})); xprintf("expecting FILE @ but found %.*s",
_temp590.last_plus_one - _temp590.curr, _temp590.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp592=({ struct Cyc_List_List*
_temp591= es; if( _temp591 == 0){ _throw( Null_Exception);} _temp591->tl;}); if(
_temp592 == 0){ _throw( Null_Exception);} _temp592->hd;}),({ struct Cyc_List_List*
_temp594=({ struct Cyc_List_List* _temp593= es; if( _temp593 == 0){ _throw(
Null_Exception);} _temp593->tl;}); if( _temp594 == 0){ _throw( Null_Exception);}
_temp594->tl;}), Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL542;
_LL554: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp595=( char*)"sscanf is not supported";
struct _tagged_string _temp596; _temp596.curr= _temp595; _temp596.base= _temp595;
_temp596.last_plus_one= _temp595 + 24; _temp596;})); t= Cyc_Absyn_sint_t; goto
_LL542; _LL556: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL597: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp599=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 35; _temp600;})); case 1: _LL598: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp602= es;
if( _temp602 == 0){ _throw( Null_Exception);} _temp602->hd;})); break; case 2:
_LL601: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp604= es; if( _temp604 == 0){ _throw( Null_Exception);}
_temp604->hd;}),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp606=({
struct Cyc_List_List* _temp605= es; if( _temp605 == 0){ _throw( Null_Exception);}
_temp605->tl;}); if( _temp606 == 0){ _throw( Null_Exception);} _temp606->hd;}));
break; default: _LL603: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp608=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp609; _temp609.curr= _temp608; _temp609.base= _temp608;
_temp609.last_plus_one= _temp608 + 37; _temp609;}));} goto _LL542; _LL542:;}
return t;}} struct _tuple7{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp610=( void*) e->r; void* _temp630; struct Cyc_Absyn_Vardecl* _temp632;
struct _tuple1* _temp634; void* _temp636; struct Cyc_Absyn_Vardecl* _temp638;
struct _tuple1* _temp640; void* _temp642; struct Cyc_Absyn_Vardecl* _temp644;
struct _tuple1* _temp646; void* _temp648; struct Cyc_Absyn_Vardecl* _temp650;
struct _tuple1* _temp652; struct Cyc_Absyn_Exp* _temp654; struct Cyc_Absyn_Exp*
_temp656; struct _tagged_string* _temp658; struct Cyc_Absyn_Exp* _temp660;
struct _tagged_string* _temp662; struct Cyc_Absyn_Exp* _temp664; struct Cyc_Absyn_Exp*
_temp666; _LL612: if(*(( int*) _temp610) == Cyc_Absyn_Var_e){ _LL635: _temp634=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp610)->f1; goto _LL631;
_LL631: _temp630=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp610)->f2; if((
unsigned int) _temp630 > 1u?*(( int*) _temp630) == Cyc_Absyn_Param_b: 0){ _LL633:
_temp632=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp630)->f1; goto _LL613;} else{ goto _LL614;}} else{ goto _LL614;} _LL614:
if(*(( int*) _temp610) == Cyc_Absyn_Var_e){ _LL641: _temp640=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp610)->f1; goto _LL637; _LL637: _temp636=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp610)->f2; if(( unsigned int)
_temp636 > 1u?*(( int*) _temp636) == Cyc_Absyn_Local_b: 0){ _LL639: _temp638=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp636)->f1;
goto _LL615;} else{ goto _LL616;}} else{ goto _LL616;} _LL616: if(*(( int*)
_temp610) == Cyc_Absyn_Var_e){ _LL647: _temp646=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp610)->f1; goto _LL643; _LL643: _temp642=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp610)->f2; if(( unsigned int) _temp642 > 1u?*(( int*) _temp642) == Cyc_Absyn_Pat_b:
0){ _LL645: _temp644=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp642)->f1; goto _LL617;} else{ goto _LL618;}} else{ goto _LL618;} _LL618:
if(*(( int*) _temp610) == Cyc_Absyn_Var_e){ _LL653: _temp652=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp610)->f1; goto _LL649; _LL649: _temp648=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp610)->f2; if(( unsigned int)
_temp648 > 1u?*(( int*) _temp648) == Cyc_Absyn_Global_b: 0){ _LL651: _temp650=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp648)->f1;
goto _LL619;} else{ goto _LL620;}} else{ goto _LL620;} _LL620: if(*(( int*)
_temp610) == Cyc_Absyn_Subscript_e){ _LL657: _temp656=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp610)->f1; goto _LL655; _LL655:
_temp654=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp610)->f2; goto _LL621;} else{ goto _LL622;} _LL622: if(*(( int*) _temp610)
== Cyc_Absyn_StructMember_e){ _LL661: _temp660=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp610)->f1; goto _LL659; _LL659: _temp658=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp610)->f2;
goto _LL623;} else{ goto _LL624;} _LL624: if(*(( int*) _temp610) == Cyc_Absyn_StructArrow_e){
_LL665: _temp664=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp610)->f1; goto _LL663; _LL663: _temp662=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp610)->f2; goto _LL625;} else{ goto _LL626;}
_LL626: if(*(( int*) _temp610) == Cyc_Absyn_Deref_e){ _LL667: _temp666=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp610)->f1; goto _LL627;}
else{ goto _LL628;} _LL628: goto _LL629; _LL613: _temp638= _temp632; goto _LL615;
_LL615: _temp644= _temp638; goto _LL617; _LL617: _temp650= _temp644; goto _LL619;
_LL619: if( !( _temp650->tq).q_const){ return;} goto _LL611; _LL621:{ void*
_temp669= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp668= _temp656->topt;
if( _temp668 == 0){ _throw( Null_Exception);} _temp668->v;})); struct Cyc_Absyn_PtrInfo
_temp679; struct Cyc_Absyn_Conref* _temp681; struct Cyc_Absyn_Tqual _temp683;
struct Cyc_Absyn_Conref* _temp685; void* _temp687; void* _temp689; struct Cyc_Absyn_Exp*
_temp691; struct Cyc_Absyn_Tqual _temp693; void* _temp695; struct Cyc_List_List*
_temp697; _LL671: if(( unsigned int) _temp669 > 4u?*(( int*) _temp669) == Cyc_Absyn_PointerType:
0){ _LL680: _temp679=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp669)->f1; _LL690: _temp689=( void*) _temp679.elt_typ; goto _LL688; _LL688:
_temp687=( void*) _temp679.rgn_typ; goto _LL686; _LL686: _temp685=( struct Cyc_Absyn_Conref*)
_temp679.nullable; goto _LL684; _LL684: _temp683=( struct Cyc_Absyn_Tqual)
_temp679.tq; goto _LL682; _LL682: _temp681=( struct Cyc_Absyn_Conref*) _temp679.bounds;
goto _LL672;} else{ goto _LL673;} _LL673: if(( unsigned int) _temp669 > 4u?*((
int*) _temp669) == Cyc_Absyn_ArrayType: 0){ _LL696: _temp695=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp669)->f1; goto _LL694; _LL694: _temp693=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp669)->f2;
goto _LL692; _LL692: _temp691=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp669)->f3; goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int)
_temp669 > 4u?*(( int*) _temp669) == Cyc_Absyn_TupleType: 0){ _LL698: _temp697=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp669)->f1; goto
_LL676;} else{ goto _LL677;} _LL677: goto _LL678; _LL672: _temp693= _temp683;
goto _LL674; _LL674: if( ! _temp693.q_const){ return;} goto _LL670; _LL676: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp654);{ struct _handler_cons
_temp699; _push_handler(& _temp699);{ int _temp701= 0; if( setjmp( _temp699.handler)){
_temp701= 1;} if( ! _temp701){{ struct _tuple7 _temp704; void* _temp705; struct
Cyc_Absyn_Tqual _temp707; struct _tuple7* _temp702=(( struct _tuple7*(*)( struct
Cyc_List_List* x, int i)) Cyc_List_nth)( _temp697,( int) i); _temp704=* _temp702;
_LL708: _temp707= _temp704.f1; goto _LL706; _LL706: _temp705= _temp704.f2; goto
_LL703; _LL703: if( ! _temp707.q_const){ _npop_handler( 0u); return;}};
_pop_handler();} else{ void* _temp700=( void*) _exn_thrown; void* _temp710=
_temp700; _LL712: if( _temp710 == Cyc_List_Nth){ goto _LL713;} else{ goto _LL714;}
_LL714: goto _LL715; _LL713: return; _LL715:( void) _throw( _temp710); _LL711:;}}}
goto _LL670;} _LL678: goto _LL670; _LL670:;} goto _LL611; _LL623:{ void*
_temp717= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp716= _temp660->topt;
if( _temp716 == 0){ _throw( Null_Exception);} _temp716->v;})); struct Cyc_Absyn_Structdecl**
_temp729; struct Cyc_List_List* _temp731; struct _tuple1* _temp733; struct Cyc_List_List*
_temp735; struct Cyc_List_List* _temp737; struct Cyc_Absyn_Uniondecl** _temp739;
struct Cyc_List_List* _temp741; struct _tuple1* _temp743; _LL719: if((
unsigned int) _temp717 > 4u?*(( int*) _temp717) == Cyc_Absyn_StructType: 0){
_LL734: _temp733=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp717)->f1; goto _LL732; _LL732: _temp731=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp717)->f2; goto _LL730; _LL730: _temp729=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp717)->f3;
goto _LL720;} else{ goto _LL721;} _LL721: if(( unsigned int) _temp717 > 4u?*((
int*) _temp717) == Cyc_Absyn_AnonUnionType: 0){ _LL736: _temp735=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp717)->f1; goto _LL722;} else{ goto
_LL723;} _LL723: if(( unsigned int) _temp717 > 4u?*(( int*) _temp717) == Cyc_Absyn_AnonStructType:
0){ _LL738: _temp737=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp717)->f1; goto _LL724;} else{ goto _LL725;} _LL725: if(( unsigned int)
_temp717 > 4u?*(( int*) _temp717) == Cyc_Absyn_UnionType: 0){ _LL744: _temp743=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp717)->f1; goto
_LL742; _LL742: _temp741=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp717)->f2; goto _LL740; _LL740: _temp739=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp717)->f3; goto _LL726;} else{ goto
_LL727;} _LL727: goto _LL728; _LL720: { struct Cyc_Absyn_Structfield* sf=
_temp729 == 0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl** _temp746= _temp729; if( _temp746 == 0){ _throw(
Null_Exception);}* _temp746;}), _temp658); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield*
_temp745= sf; if( _temp745 == 0){ _throw( Null_Exception);} _temp745->tq;}).q_const){
return;} goto _LL718;} _LL722: _temp737= _temp735; goto _LL724; _LL724: { struct
Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field( _temp737, _temp658); if( sf
== 0? 1: !({ struct Cyc_Absyn_Structfield* _temp747= sf; if( _temp747 == 0){
_throw( Null_Exception);} _temp747->tq;}).q_const){ return;} goto _LL718;}
_LL726: { struct Cyc_Absyn_Structfield* sf= _temp739 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)({ struct Cyc_Absyn_Uniondecl** _temp749= _temp739;
if( _temp749 == 0){ _throw( Null_Exception);}* _temp749;}), _temp658); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp748= sf; if( _temp748 == 0){ _throw(
Null_Exception);} _temp748->tq;}).q_const){ return;} goto _LL718;} _LL728: goto
_LL718; _LL718:;} goto _LL611; _LL625:{ void* _temp751= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp750= _temp664->topt; if( _temp750 == 0){
_throw( Null_Exception);} _temp750->v;})); struct Cyc_Absyn_PtrInfo _temp757;
struct Cyc_Absyn_Conref* _temp759; struct Cyc_Absyn_Tqual _temp761; struct Cyc_Absyn_Conref*
_temp763; void* _temp765; void* _temp767; _LL753: if(( unsigned int) _temp751 >
4u?*(( int*) _temp751) == Cyc_Absyn_PointerType: 0){ _LL758: _temp757=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp751)->f1; _LL768:
_temp767=( void*) _temp757.elt_typ; goto _LL766; _LL766: _temp765=( void*)
_temp757.rgn_typ; goto _LL764; _LL764: _temp763=( struct Cyc_Absyn_Conref*)
_temp757.nullable; goto _LL762; _LL762: _temp761=( struct Cyc_Absyn_Tqual)
_temp757.tq; goto _LL760; _LL760: _temp759=( struct Cyc_Absyn_Conref*) _temp757.bounds;
goto _LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL754:{ void* _temp769=
Cyc_Tcutil_compress( _temp767); struct Cyc_Absyn_Structdecl** _temp781; struct
Cyc_List_List* _temp783; struct _tuple1* _temp785; struct Cyc_Absyn_Uniondecl**
_temp787; struct Cyc_List_List* _temp789; struct _tuple1* _temp791; struct Cyc_List_List*
_temp793; struct Cyc_List_List* _temp795; _LL771: if(( unsigned int) _temp769 >
4u?*(( int*) _temp769) == Cyc_Absyn_StructType: 0){ _LL786: _temp785=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp769)->f1; goto _LL784;
_LL784: _temp783=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp769)->f2; goto _LL782; _LL782: _temp781=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp769)->f3; goto _LL772;} else{ goto
_LL773;} _LL773: if(( unsigned int) _temp769 > 4u?*(( int*) _temp769) == Cyc_Absyn_UnionType:
0){ _LL792: _temp791=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp769)->f1; goto _LL790; _LL790: _temp789=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp769)->f2; goto _LL788; _LL788: _temp787=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp769)->f3;
goto _LL774;} else{ goto _LL775;} _LL775: if(( unsigned int) _temp769 > 4u?*((
int*) _temp769) == Cyc_Absyn_AnonStructType: 0){ _LL794: _temp793=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp769)->f1; goto _LL776;} else{ goto
_LL777;} _LL777: if(( unsigned int) _temp769 > 4u?*(( int*) _temp769) == Cyc_Absyn_AnonUnionType:
0){ _LL796: _temp795=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp769)->f1; goto _LL778;} else{ goto _LL779;} _LL779: goto _LL780; _LL772: {
struct Cyc_Absyn_Structfield* sf= _temp781 == 0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl** _temp798= _temp781;
if( _temp798 == 0){ _throw( Null_Exception);}* _temp798;}), _temp662); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp797= sf; if( _temp797 == 0){ _throw(
Null_Exception);} _temp797->tq;}).q_const){ return;} goto _LL770;} _LL774: {
struct Cyc_Absyn_Structfield* sf= _temp787 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)({ struct Cyc_Absyn_Uniondecl** _temp800= _temp787;
if( _temp800 == 0){ _throw( Null_Exception);}* _temp800;}), _temp662); if( sf ==
0? 1: !({ struct Cyc_Absyn_Structfield* _temp799= sf; if( _temp799 == 0){ _throw(
Null_Exception);} _temp799->tq;}).q_const){ return;} goto _LL770;} _LL776:
_temp795= _temp793; goto _LL778; _LL778: { struct Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field(
_temp795, _temp662); if( sf == 0? 1: !({ struct Cyc_Absyn_Structfield* _temp801=
sf; if( _temp801 == 0){ _throw( Null_Exception);} _temp801->tq;}).q_const){
return;} goto _LL770;} _LL780: goto _LL770; _LL770:;} goto _LL752; _LL756: goto
_LL752; _LL752:;} goto _LL611; _LL627:{ void* _temp803= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp802= _temp666->topt; if( _temp802 == 0){
_throw( Null_Exception);} _temp802->v;})); struct Cyc_Absyn_PtrInfo _temp811;
struct Cyc_Absyn_Conref* _temp813; struct Cyc_Absyn_Tqual _temp815; struct Cyc_Absyn_Conref*
_temp817; void* _temp819; void* _temp821; struct Cyc_Absyn_Exp* _temp823; struct
Cyc_Absyn_Tqual _temp825; void* _temp827; _LL805: if(( unsigned int) _temp803 >
4u?*(( int*) _temp803) == Cyc_Absyn_PointerType: 0){ _LL812: _temp811=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp803)->f1; _LL822:
_temp821=( void*) _temp811.elt_typ; goto _LL820; _LL820: _temp819=( void*)
_temp811.rgn_typ; goto _LL818; _LL818: _temp817=( struct Cyc_Absyn_Conref*)
_temp811.nullable; goto _LL816; _LL816: _temp815=( struct Cyc_Absyn_Tqual)
_temp811.tq; goto _LL814; _LL814: _temp813=( struct Cyc_Absyn_Conref*) _temp811.bounds;
goto _LL806;} else{ goto _LL807;} _LL807: if(( unsigned int) _temp803 > 4u?*((
int*) _temp803) == Cyc_Absyn_ArrayType: 0){ _LL828: _temp827=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp803)->f1; goto _LL826; _LL826: _temp825=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp803)->f2;
goto _LL824; _LL824: _temp823=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp803)->f3; goto _LL808;} else{ goto _LL809;} _LL809: goto _LL810; _LL806:
_temp825= _temp815; goto _LL808; _LL808: if( ! _temp825.q_const){ return;} goto
_LL804; _LL810: goto _LL804; _LL804:;} goto _LL611; _LL629: goto _LL611; _LL611:;}
Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp829=( char*)"attempt to write a read-only location";
struct _tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 38; _temp830;}));} static void* Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue(
e)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp831=( char*)"increment/decrement of non-lvalue"; struct _tagged_string
_temp832; _temp832.curr= _temp831; _temp832.base= _temp831; _temp832.last_plus_one=
_temp831 + 34; _temp832;}));} Cyc_Tcexp_check_writable( te, e);{ void* t=( void*)({
struct Cyc_Core_Opt* _temp834= e->topt; if( _temp834 == 0){ _throw(
Null_Exception);} _temp834->v;}); if( ! Cyc_Tcutil_is_numeric( e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp833= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or ? type but found %.*s", _temp833.last_plus_one
- _temp833.curr, _temp833.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp835=( char*)"conditional expression";
struct _tagged_string _temp836; _temp836.curr= _temp835; _temp836.base= _temp835;
_temp836.last_plus_one= _temp835 + 23; _temp836;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* _temp838=({ struct Cyc_List_List* _temp837=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp837->hd=( void*) e3; _temp837->tl=
0; _temp837;}); goto _LL839; _LL839: { struct Cyc_List_List* _temp841=({ struct
Cyc_List_List* _temp840=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp840->hd=( void*) e2; _temp840->tl= _temp838; _temp840;}); goto _LL842;
_LL842: if( ! Cyc_Tcutil_coerce_list( te, t, _temp841)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp845= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp843= e2->topt; if( _temp843 == 0){ _throw( Null_Exception);} _temp843->v;}));
struct _tagged_string _temp846= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp844= e3->topt; if( _temp844 == 0){ _throw( Null_Exception);} _temp844->v;}));
xprintf("conditional clause types do not match: %.*s != %.*s", _temp845.last_plus_one
- _temp845.curr, _temp845.curr, _temp846.last_plus_one - _temp846.curr, _temp846.curr);}));}
return t;}}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1);
Cyc_Tcexp_tcExp( te,( void**)(( void**)(( void*)({ struct Cyc_Core_Opt* _temp847=
e1->topt; if( _temp847 == 0){ _throw( Null_Exception);}& _temp847->v;}))), e2);{
void* t1=( void*)({ struct Cyc_Core_Opt* _temp874= e1->topt; if( _temp874 == 0){
_throw( Null_Exception);} _temp874->v;}); void* t2=( void*)({ struct Cyc_Core_Opt*
_temp873= e2->topt; if( _temp873 == 0){ _throw( Null_Exception);} _temp873->v;});{
void* _temp848= Cyc_Tcutil_compress( t1); struct Cyc_Absyn_Exp* _temp854; struct
Cyc_Absyn_Tqual _temp856; void* _temp858; _LL850: if(( unsigned int) _temp848 >
4u?*(( int*) _temp848) == Cyc_Absyn_ArrayType: 0){ _LL859: _temp858=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp848)->f1; goto _LL857; _LL857: _temp856=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp848)->f2;
goto _LL855; _LL855: _temp854=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp848)->f3; goto _LL851;} else{ goto _LL852;} _LL852: goto _LL853; _LL851:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp860=( char*)"cannot assign to an array";
struct _tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 26; _temp861;})); goto _LL849; _LL853: goto
_LL849; _LL849:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp862=( char*)"assignment to non-lvalue";
struct _tagged_string _temp863; _temp863.curr= _temp862; _temp863.base= _temp862;
_temp863.last_plus_one= _temp862 + 25; _temp863;}));} Cyc_Tcexp_check_writable(
te, e1); if( po == 0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp864= Cyc_Absynpp_typ2string( t1); struct
_tagged_string _temp865= Cyc_Absynpp_typ2string( t2); xprintf("type mismatch: %.*s != %.*s",
_temp864.last_plus_one - _temp864.curr, _temp864.curr, _temp865.last_plus_one -
_temp865.curr, _temp865.curr);}));}} else{ void* _temp867=( void*)({ struct Cyc_Core_Opt*
_temp866= po; if( _temp866 == 0){ _throw( Null_Exception);} _temp866->v;}); goto
_LL868; _LL868: { void* _temp869= Cyc_Tcexp_tcBinPrimop( te, loc, 0, _temp867,
e1, e2); goto _LL870; _LL870: if( !( Cyc_Tcutil_unify( _temp869, t1)? 1: Cyc_Tcutil_coerceable(
_temp869))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp871=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp872; _temp872.curr= _temp871; _temp872.base= _temp871;
_temp872.last_plus_one= _temp871 + 42; _temp872;}));}}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)({ struct Cyc_Core_Opt*
_temp875= e2->topt; if( _temp875 == 0){ _throw( Null_Exception);} _temp875->v;});}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp933= e->topt; if( _temp933 == 0){ _throw( Null_Exception);} _temp933->v;}));
void* _temp876= t; struct Cyc_Absyn_PtrInfo _temp882; struct Cyc_Absyn_Conref*
_temp884; struct Cyc_Absyn_Tqual _temp886; struct Cyc_Absyn_Conref* _temp888;
void* _temp890; void* _temp892; _LL878: if(( unsigned int) _temp876 > 4u?*(( int*)
_temp876) == Cyc_Absyn_PointerType: 0){ _LL883: _temp882=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp876)->f1; _LL893: _temp892=( void*)
_temp882.elt_typ; goto _LL891; _LL891: _temp890=( void*) _temp882.rgn_typ; goto
_LL889; _LL889: _temp888=( struct Cyc_Absyn_Conref*) _temp882.nullable; goto
_LL887; _LL887: _temp886=( struct Cyc_Absyn_Tqual) _temp882.tq; goto _LL885;
_LL885: _temp884=( struct Cyc_Absyn_Conref*) _temp882.bounds; goto _LL879;}
else{ goto _LL880;} _LL880: goto _LL881; _LL879: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp890); Cyc_Tcutil_check_nonzero_bound( loc, _temp884);{ void*
_temp894= Cyc_Tcutil_compress( _temp892); struct Cyc_Absyn_FnInfo _temp900;
struct Cyc_List_List* _temp902; int _temp904; struct Cyc_List_List* _temp906;
void* _temp908; struct Cyc_Core_Opt* _temp910; struct Cyc_List_List* _temp912;
_LL896: if(( unsigned int) _temp894 > 4u?*(( int*) _temp894) == Cyc_Absyn_FnType:
0){ _LL901: _temp900=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp894)->f1; _LL913: _temp912=( struct Cyc_List_List*) _temp900.tvars; goto
_LL911; _LL911: _temp910=( struct Cyc_Core_Opt*) _temp900.effect; goto _LL909;
_LL909: _temp908=( void*) _temp900.ret_typ; goto _LL907; _LL907: _temp906=(
struct Cyc_List_List*) _temp900.args; goto _LL905; _LL905: _temp904=( int)
_temp900.varargs; goto _LL903; _LL903: _temp902=( struct Cyc_List_List*)
_temp900.attributes; goto _LL897;} else{ goto _LL898;} _LL898: goto _LL899;
_LL897: if( _temp904){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp914=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp915; _temp915.curr= _temp914; _temp915.base= _temp914;
_temp915.last_plus_one= _temp914 + 41; _temp915;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp908,({ void** _temp916= topt; if( _temp916 == 0){ _throw( Null_Exception);}*
_temp916;}));} while( es != 0? _temp906 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp923= es; if( _temp923 == 0){
_throw( Null_Exception);} _temp923->hd;}); void* t2=(*(( struct _tuple2*)({
struct Cyc_List_List* _temp922= _temp906; if( _temp922 == 0){ _throw(
Null_Exception);} _temp922->hd;}))).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1);
if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp918= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp917= e1->topt; if( _temp917 == 0){ _throw( Null_Exception);} _temp917->v;}));
struct _tagged_string _temp919= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp918.last_plus_one - _temp918.curr, _temp918.curr, _temp919.last_plus_one -
_temp919.curr, _temp919.curr);}));} es=({ struct Cyc_List_List* _temp920= es;
if( _temp920 == 0){ _throw( Null_Exception);} _temp920->tl;}); _temp906=({
struct Cyc_List_List* _temp921= _temp906; if( _temp921 == 0){ _throw(
Null_Exception);} _temp921->tl;});} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp924=( char*)"too many arguments for function";
struct _tagged_string _temp925; _temp925.curr= _temp924; _temp925.base= _temp924;
_temp925.last_plus_one= _temp924 + 32; _temp925;}));} if( _temp906 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp926=( char*)"too few arguments for function";
struct _tagged_string _temp927; _temp927.curr= _temp926; _temp927.base= _temp926;
_temp927.last_plus_one= _temp926 + 31; _temp927;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)({ struct Cyc_Core_Opt* _temp928= _temp910; if( _temp928 == 0){
_throw( Null_Exception);} _temp928->v;})); return _temp908; _LL899: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp929=( char*)"expected pointer to function";
struct _tagged_string _temp930; _temp930.curr= _temp929; _temp930.base= _temp929;
_temp930.last_plus_one= _temp929 + 29; _temp930;})); _LL895:;} _LL881: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp931=( char*)"expected pointer to function";
struct _tagged_string _temp932; _temp932.curr= _temp931; _temp932.base= _temp931;
_temp932.last_plus_one= _temp931 + 29; _temp932;})); _LL877:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp935= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp934= e->topt; if( _temp934 == 0){ _throw( Null_Exception);} _temp934->v;}));
xprintf("expected xtunion exn but found %.*s", _temp935.last_plus_one - _temp935.curr,
_temp935.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)({ struct Cyc_Core_Opt* _temp936= e->topt; if( _temp936 == 0){ _throw(
Null_Exception);} _temp936->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp937= e->topt; if( _temp937 == 0){ _throw(
Null_Exception);} _temp937->v;})));});{ void* t1= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp993= e->topt; if( _temp993 == 0){ _throw(
Null_Exception);} _temp993->v;}));{ void* _temp938= t1; struct Cyc_Absyn_PtrInfo
_temp944; struct Cyc_Absyn_Conref* _temp946; struct Cyc_Absyn_Tqual _temp948;
struct Cyc_Absyn_Conref* _temp950; void* _temp952; void* _temp954; _LL940: if((
unsigned int) _temp938 > 4u?*(( int*) _temp938) == Cyc_Absyn_PointerType: 0){
_LL945: _temp944=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp938)->f1; _LL955: _temp954=( void*) _temp944.elt_typ; goto _LL953; _LL953:
_temp952=( void*) _temp944.rgn_typ; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Conref*)
_temp944.nullable; goto _LL949; _LL949: _temp948=( struct Cyc_Absyn_Tqual)
_temp944.tq; goto _LL947; _LL947: _temp946=( struct Cyc_Absyn_Conref*) _temp944.bounds;
goto _LL941;} else{ goto _LL942;} _LL942: goto _LL943; _LL941:{ void* _temp956=
Cyc_Tcutil_compress( _temp954); struct Cyc_Absyn_FnInfo _temp962; struct Cyc_List_List*
_temp964; int _temp966; struct Cyc_List_List* _temp968; void* _temp970; struct
Cyc_Core_Opt* _temp972; struct Cyc_List_List* _temp974; _LL958: if((
unsigned int) _temp956 > 4u?*(( int*) _temp956) == Cyc_Absyn_FnType: 0){ _LL963:
_temp962=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp956)->f1;
_LL975: _temp974=( struct Cyc_List_List*) _temp962.tvars; goto _LL973; _LL973:
_temp972=( struct Cyc_Core_Opt*) _temp962.effect; goto _LL971; _LL971: _temp970=(
void*) _temp962.ret_typ; goto _LL969; _LL969: _temp968=( struct Cyc_List_List*)
_temp962.args; goto _LL967; _LL967: _temp966=( int) _temp962.varargs; goto
_LL965; _LL965: _temp964=( struct Cyc_List_List*) _temp962.attributes; goto
_LL959;} else{ goto _LL960;} _LL960: goto _LL961; _LL959: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp974)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp976=( char*)"type instantiation mismatch";
struct _tagged_string _temp977; _temp977.curr= _temp976; _temp977.base= _temp976;
_temp977.last_plus_one= _temp976 + 28; _temp977;}));} for( 0; ts != 0; ts=({
struct Cyc_List_List* _temp978= ts; if( _temp978 == 0){ _throw( Null_Exception);}
_temp978->tl;}), _temp974=({ struct Cyc_List_List* _temp979= _temp974; if(
_temp979 == 0){ _throw( Null_Exception);} _temp979->tl;})){ void* k=(( void*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp985= _temp974; if( _temp985 == 0){ _throw(
Null_Exception);} _temp985->hd;}))->kind); Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), k,( void*)({ struct Cyc_List_List* _temp980= ts; if( _temp980 == 0){ _throw(
Null_Exception);} _temp980->hd;})); instantiation=({ struct Cyc_List_List*
_temp981=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp981->hd=( void*)({ struct _tuple4* _temp982=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp982->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp984= _temp974; if( _temp984 == 0){ _throw( Null_Exception);} _temp984->hd;});
_temp982->f2=( void*)({ struct Cyc_List_List* _temp983= ts; if( _temp983 == 0){
_throw( Null_Exception);} _temp983->hd;}); _temp982;}); _temp981->tl=
instantiation; _temp981;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp989=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp989[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp990; _temp990.tag= Cyc_Absyn_FnType; _temp990.f1=({ struct Cyc_Absyn_FnInfo
_temp991; _temp991.tvars= 0; _temp991.effect= _temp972; _temp991.ret_typ=( void*)
_temp970; _temp991.args= _temp968; _temp991.varargs= _temp966; _temp991.attributes=
_temp964; _temp991;}); _temp990;}); _temp989;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp986=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp986[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp987; _temp987.tag= Cyc_Absyn_PointerType;
_temp987.f1=({ struct Cyc_Absyn_PtrInfo _temp988; _temp988.elt_typ=( void*)
new_fn_typ; _temp988.rgn_typ=( void*) _temp952; _temp988.nullable= _temp950;
_temp988.tq= _temp948; _temp988.bounds= _temp946; _temp988;}); _temp987;});
_temp986;}); return new_typ;}} _LL961: goto _LL957; _LL957:;} goto _LL939;
_LL943: goto _LL939; _LL939:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp992= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp992.last_plus_one - _temp992.curr, _temp992.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)({
struct Cyc_Core_Opt* _temp996= e->topt; if( _temp996 == 0){ _throw(
Null_Exception);} _temp996->v;}); if( ! Cyc_Tcutil_silent_castable( te, loc, t2,
t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te,
loc,({ struct _tagged_string _temp994= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp995= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp994.last_plus_one - _temp994.curr, _temp994.curr, _temp995.last_plus_one -
_temp995.curr, _temp995.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** _temp997= 0; goto _LL998; _LL998: { struct Cyc_Absyn_Tqual
_temp999= Cyc_Absyn_empty_tqual(); goto _LL1000; _LL1000: if( topt != 0){ void*
_temp1002= Cyc_Tcutil_compress(({ void** _temp1001= topt; if( _temp1001 == 0){
_throw( Null_Exception);}* _temp1001;})); struct Cyc_Absyn_PtrInfo _temp1008;
struct Cyc_Absyn_Conref* _temp1010; struct Cyc_Absyn_Tqual _temp1012; struct Cyc_Absyn_Conref*
_temp1014; void* _temp1016; void* _temp1018; _LL1004: if(( unsigned int)
_temp1002 > 4u?*(( int*) _temp1002) == Cyc_Absyn_PointerType: 0){ _LL1009:
_temp1008=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1002)->f1; _LL1019: _temp1018=( void*) _temp1008.elt_typ; goto _LL1017;
_LL1017: _temp1016=( void*) _temp1008.rgn_typ; goto _LL1015; _LL1015: _temp1014=(
struct Cyc_Absyn_Conref*) _temp1008.nullable; goto _LL1013; _LL1013: _temp1012=(
struct Cyc_Absyn_Tqual) _temp1008.tq; goto _LL1011; _LL1011: _temp1010=( struct
Cyc_Absyn_Conref*) _temp1008.bounds; goto _LL1005;} else{ goto _LL1006;} _LL1006:
goto _LL1007; _LL1005: _temp997=({ void** _temp1020=( void**) GC_malloc( sizeof(
void*)); _temp1020[ 0]= _temp1018; _temp1020;}); _temp999= _temp1012; goto
_LL1003; _LL1007: goto _LL1003; _LL1003:;} Cyc_Tcexp_tcExpNoInst( te, _temp997,
e);{ void* _temp1021=( void*) e->r; struct Cyc_Absyn_Structdecl* _temp1029;
struct Cyc_List_List* _temp1031; struct Cyc_Core_Opt* _temp1033; struct _tuple1*
_temp1035; struct Cyc_List_List* _temp1037; _LL1023: if(*(( int*) _temp1021) ==
Cyc_Absyn_Struct_e){ _LL1036: _temp1035=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f1; goto _LL1034; _LL1034: _temp1033=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1021)->f2; goto _LL1032; _LL1032:
_temp1031=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1021)->f3; goto _LL1030; _LL1030: _temp1029=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1021)->f4; goto _LL1024;} else{ goto
_LL1025;} _LL1025: if(*(( int*) _temp1021) == Cyc_Absyn_Tuple_e){ _LL1038:
_temp1037=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp1021)->f1;
goto _LL1026;} else{ goto _LL1027;} _LL1027: goto _LL1028; _LL1024: goto _LL1026;
_LL1026: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1039=( char*)"& used to allocate";
struct _tagged_string _temp1040; _temp1040.curr= _temp1039; _temp1040.base=
_temp1039; _temp1040.last_plus_one= _temp1039 + 19; _temp1040;}));{ void*(*
_temp1041)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ;
goto _LL1042; _LL1042: if( topt != 0){ void* _temp1044= Cyc_Tcutil_compress(({
void** _temp1043= topt; if( _temp1043 == 0){ _throw( Null_Exception);}*
_temp1043;})); struct Cyc_Absyn_PtrInfo _temp1050; struct Cyc_Absyn_Conref*
_temp1052; struct Cyc_Absyn_Tqual _temp1054; struct Cyc_Absyn_Conref* _temp1056;
void* _temp1058; void* _temp1060; _LL1046: if(( unsigned int) _temp1044 > 4u?*((
int*) _temp1044) == Cyc_Absyn_PointerType: 0){ _LL1051: _temp1050=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1044)->f1; _LL1061: _temp1060=( void*)
_temp1050.elt_typ; goto _LL1059; _LL1059: _temp1058=( void*) _temp1050.rgn_typ;
goto _LL1057; _LL1057: _temp1056=( struct Cyc_Absyn_Conref*) _temp1050.nullable;
goto _LL1055; _LL1055: _temp1054=( struct Cyc_Absyn_Tqual) _temp1050.tq; goto
_LL1053; _LL1053: _temp1052=( struct Cyc_Absyn_Conref*) _temp1050.bounds; goto
_LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1047: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp1056)){ _temp1041= Cyc_Absyn_star_typ;}
goto _LL1045; _LL1049: goto _LL1045; _LL1045:;} return _temp1041(( void*)({
struct Cyc_Core_Opt* _temp1062= e->topt; if( _temp1062 == 0){ _throw(
Null_Exception);} _temp1062->v;}),( void*) Cyc_Absyn_HeapRgn, _temp999);}
_LL1028: goto _LL1022; _LL1022:;}{ void* _temp1065; int _temp1067; struct
_tuple5 _temp1063= Cyc_Tcutil_addressof_props( te, e); _LL1068: _temp1067=
_temp1063.f1; goto _LL1066; _LL1066: _temp1065= _temp1063.f2; goto _LL1064;
_LL1064: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if( _temp1067){
tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)({ struct Cyc_Core_Opt*
_temp1069= e->topt; if( _temp1069 == 0){ _throw( Null_Exception);} _temp1069->v;}),
_temp1065, tq); return t;}}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1089= e->topt; if( _temp1089 == 0){ _throw( Null_Exception);} _temp1089->v;}));
void* _temp1070= t; struct Cyc_Absyn_PtrInfo _temp1076; struct Cyc_Absyn_Conref*
_temp1078; struct Cyc_Absyn_Tqual _temp1080; struct Cyc_Absyn_Conref* _temp1082;
void* _temp1084; void* _temp1086; _LL1072: if(( unsigned int) _temp1070 > 4u?*((
int*) _temp1070) == Cyc_Absyn_PointerType: 0){ _LL1077: _temp1076=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1070)->f1; _LL1087: _temp1086=( void*)
_temp1076.elt_typ; goto _LL1085; _LL1085: _temp1084=( void*) _temp1076.rgn_typ;
goto _LL1083; _LL1083: _temp1082=( struct Cyc_Absyn_Conref*) _temp1076.nullable;
goto _LL1081; _LL1081: _temp1080=( struct Cyc_Absyn_Tqual) _temp1076.tq; goto
_LL1079; _LL1079: _temp1078=( struct Cyc_Absyn_Conref*) _temp1076.bounds; goto
_LL1073;} else{ goto _LL1074;} _LL1074: goto _LL1075; _LL1073: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1084); Cyc_Tcutil_check_nonzero_bound( loc, _temp1078); return
_temp1086; _LL1075: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1088= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp1088.last_plus_one - _temp1088.curr, _temp1088.curr);})); _LL1071:;}}
static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1179= e->topt; if( _temp1179 == 0){ _throw(
Null_Exception);} _temp1179->v;})); void* _temp1090= t; struct Cyc_Absyn_Structdecl**
_temp1106; struct Cyc_Absyn_Structdecl* _temp1108; struct Cyc_List_List*
_temp1109; struct _tuple1* _temp1111; struct Cyc_List_List* _temp1113; struct
Cyc_List_List* _temp1115; struct Cyc_Absyn_Uniondecl** _temp1117; struct Cyc_Absyn_Uniondecl*
_temp1119; struct Cyc_List_List* _temp1120; struct _tuple1* _temp1122; struct
Cyc_Absyn_Exp* _temp1125; struct Cyc_Absyn_Tqual _temp1127; void* _temp1129;
struct Cyc_Absyn_PtrInfo _temp1134; _LL1092: if(( unsigned int) _temp1090 > 4u?*((
int*) _temp1090) == Cyc_Absyn_StructType: 0){ _LL1112: _temp1111=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp1090)->f1; goto _LL1110;
_LL1110: _temp1109=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp1090)->f2; goto _LL1107; _LL1107: _temp1106=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp1090)->f3; if( _temp1106 == 0){ goto
_LL1094;} else{ _temp1108=* _temp1106; goto _LL1093;}} else{ goto _LL1094;}
_LL1094: if(( unsigned int) _temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_AnonStructType:
0){ _LL1114: _temp1113=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1090)->f1; goto _LL1095;} else{ goto _LL1096;} _LL1096: if(( unsigned int)
_temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_AnonUnionType: 0){ _LL1116:
_temp1115=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1090)->f1; goto _LL1097;} else{ goto _LL1098;} _LL1098: if(( unsigned int)
_temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_UnionType: 0){ _LL1123:
_temp1122=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp1090)->f1;
goto _LL1121; _LL1121: _temp1120=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1090)->f2; goto _LL1118; _LL1118: _temp1117=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1090)->f3; if( _temp1117 == 0){ goto
_LL1100;} else{ _temp1119=* _temp1117; goto _LL1099;}} else{ goto _LL1100;}
_LL1100: if(( unsigned int) _temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_ArrayType:
0){ _LL1130: _temp1129=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1090)->f1;
goto _LL1128; _LL1128: _temp1127=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1090)->f2; goto _LL1126; _LL1126: _temp1125=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1090)->f3; goto _LL1124;} else{ goto
_LL1102;} _LL1124: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char*
_temp1131=( char*)"size"; struct _tagged_string _temp1132; _temp1132.curr=
_temp1131; _temp1132.base= _temp1131; _temp1132.last_plus_one= _temp1131 + 5;
_temp1132;})) == 0){ goto _LL1101;} else{ goto _LL1102;} _LL1102: if((
unsigned int) _temp1090 > 4u?*(( int*) _temp1090) == Cyc_Absyn_PointerType: 0){
_LL1135: _temp1134=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1090)->f1; goto _LL1133;} else{ goto _LL1104;} _LL1133: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp1136=( char*)"size"; struct
_tagged_string _temp1137; _temp1137.curr= _temp1136; _temp1137.base= _temp1136;
_temp1137.last_plus_one= _temp1136 + 5; _temp1137;})) == 0){ goto _LL1103;}
else{ goto _LL1104;} _LL1104: goto _LL1105; _LL1093: if( _temp1111 == 0){ return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp1138=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1139; _temp1139.curr= _temp1138; _temp1139.base=
_temp1138; _temp1139.last_plus_one= _temp1138 + 40; _temp1139;}));}{ struct Cyc_Absyn_Structfield*
_temp1140= Cyc_Absyn_lookup_struct_field( _temp1108, f); goto _LL1141; _LL1141:
if( _temp1140 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1143= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp1142= _temp1111; if( _temp1142 == 0){ _throw( Null_Exception);} _temp1142;}));
struct _tagged_string _temp1144=* f; xprintf("struct %.*s has no %.*s field",
_temp1143.last_plus_one - _temp1143.curr, _temp1143.curr, _temp1144.last_plus_one
- _temp1144.curr, _temp1144.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1145= _new_region(); struct _RegionHandle* rgn=& _temp1145; _push_region(
rgn);{ struct Cyc_List_List* _temp1146=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1108->tvs, _temp1109); goto _LL1147; _LL1147:
t2= Cyc_Tcutil_rsubstitute( rgn, _temp1146,( void*)({ struct Cyc_Absyn_Structfield*
_temp1148= _temp1140; if( _temp1148 == 0){ _throw( Null_Exception);} _temp1148->type;}));};
_pop_region(& _temp1145);} return t2;}} _LL1095: { struct Cyc_Absyn_Structfield*
_temp1149= Cyc_Absyn_lookup_field( _temp1113, f); goto _LL1150; _LL1150: if(
_temp1149 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1151=* f; xprintf("struct has no %.*s field", _temp1151.last_plus_one -
_temp1151.curr, _temp1151.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1152= _temp1149; if( _temp1152 == 0){ _throw( Null_Exception);} _temp1152->type;});}
_LL1097: { struct Cyc_Absyn_Structfield* _temp1153= Cyc_Absyn_lookup_field(
_temp1115, f); goto _LL1154; _LL1154: if( _temp1153 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1155=* f; xprintf("union has no %.*s field",
_temp1155.last_plus_one - _temp1155.curr, _temp1155.curr);}));} return( void*)({
struct Cyc_Absyn_Structfield* _temp1156= _temp1153; if( _temp1156 == 0){ _throw(
Null_Exception);} _temp1156->type;});} _LL1099: if( _temp1122 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp1157=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp1158; _temp1158.curr= _temp1157; _temp1158.base=
_temp1157; _temp1158.last_plus_one= _temp1157 + 39; _temp1158;}));}{ struct Cyc_Absyn_Structfield*
_temp1159= Cyc_Absyn_lookup_union_field( _temp1119, f); goto _LL1160; _LL1160:
if( _temp1159 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1162= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct _tuple1*
_temp1161= _temp1122; if( _temp1161 == 0){ _throw( Null_Exception);} _temp1161;}));
struct _tagged_string _temp1163=* f; xprintf("union %.*s has no %.*s field",
_temp1162.last_plus_one - _temp1162.curr, _temp1162.curr, _temp1163.last_plus_one
- _temp1163.curr, _temp1163.curr);}));}{ struct Cyc_List_List* _temp1164=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1119->tvs, _temp1120); goto _LL1165; _LL1165: { void* _temp1167= Cyc_Tcutil_substitute(
_temp1164,( void*)({ struct Cyc_Absyn_Structfield* _temp1166= _temp1159; if(
_temp1166 == 0){ _throw( Null_Exception);} _temp1166->type;})); goto _LL1168;
_LL1168: return _temp1167;}}} _LL1101:( void*)( outer_e->r=( void*)(( void*)({
struct Cyc_Absyn_Primop_e_struct* _temp1169=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1169[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1170; _temp1170.tag= Cyc_Absyn_Primop_e;
_temp1170.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1170.f2=({ struct Cyc_List_List*
_temp1171=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1171->hd=( void*) e; _temp1171->tl= 0; _temp1171;}); _temp1170;});
_temp1169;}))); return Cyc_Absyn_uint_t; _LL1103:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1172=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1172[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1173; _temp1173.tag= Cyc_Absyn_Primop_e;
_temp1173.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1173.f2=({ struct Cyc_List_List*
_temp1174=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1174->hd=( void*) e; _temp1174->tl= 0; _temp1174;}); _temp1173;});
_temp1172;}))); return Cyc_Absyn_uint_t; _LL1105: if( Cyc_String_zstrcmp(* f,(
struct _tagged_string)({ char* _temp1175=( char*)"size"; struct _tagged_string
_temp1176; _temp1176.curr= _temp1175; _temp1176.base= _temp1175; _temp1176.last_plus_one=
_temp1175 + 5; _temp1176;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1177= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1177.last_plus_one - _temp1177.curr, _temp1177.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1178= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1178.last_plus_one - _temp1178.curr, _temp1178.curr);}));} _LL1091:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1257= e->topt; if( _temp1257 == 0){ _throw( Null_Exception);} _temp1257->v;}));
void* _temp1180= t; struct Cyc_Absyn_PtrInfo _temp1186; struct Cyc_Absyn_Conref*
_temp1188; struct Cyc_Absyn_Tqual _temp1190; struct Cyc_Absyn_Conref* _temp1192;
void* _temp1194; void* _temp1196; _LL1182: if(( unsigned int) _temp1180 > 4u?*((
int*) _temp1180) == Cyc_Absyn_PointerType: 0){ _LL1187: _temp1186=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1180)->f1; _LL1197: _temp1196=( void*)
_temp1186.elt_typ; goto _LL1195; _LL1195: _temp1194=( void*) _temp1186.rgn_typ;
goto _LL1193; _LL1193: _temp1192=( struct Cyc_Absyn_Conref*) _temp1186.nullable;
goto _LL1191; _LL1191: _temp1190=( struct Cyc_Absyn_Tqual) _temp1186.tq; goto
_LL1189; _LL1189: _temp1188=( struct Cyc_Absyn_Conref*) _temp1186.bounds; goto
_LL1183;} else{ goto _LL1184;} _LL1184: goto _LL1185; _LL1183: Cyc_Tcutil_check_nonzero_bound(
loc, _temp1188);{ void* _temp1198= Cyc_Tcutil_compress( _temp1196); struct Cyc_Absyn_Structdecl**
_temp1210; struct Cyc_Absyn_Structdecl* _temp1212; struct Cyc_List_List*
_temp1213; struct _tuple1* _temp1215; struct Cyc_Absyn_Uniondecl** _temp1217;
struct Cyc_Absyn_Uniondecl* _temp1219; struct Cyc_List_List* _temp1220; struct
_tuple1* _temp1222; struct Cyc_List_List* _temp1224; struct Cyc_List_List*
_temp1226; _LL1200: if(( unsigned int) _temp1198 > 4u?*(( int*) _temp1198) ==
Cyc_Absyn_StructType: 0){ _LL1216: _temp1215=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp1198)->f1; goto _LL1214; _LL1214: _temp1213=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp1198)->f2; goto _LL1211; _LL1211:
_temp1210=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp1198)->f3; if( _temp1210 == 0){ goto _LL1202;} else{ _temp1212=* _temp1210;
goto _LL1201;}} else{ goto _LL1202;} _LL1202: if(( unsigned int) _temp1198 > 4u?*((
int*) _temp1198) == Cyc_Absyn_UnionType: 0){ _LL1223: _temp1222=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp1198)->f1; goto _LL1221; _LL1221:
_temp1220=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp1198)->f2; goto _LL1218; _LL1218: _temp1217=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp1198)->f3; if( _temp1217 == 0){ goto
_LL1204;} else{ _temp1219=* _temp1217; goto _LL1203;}} else{ goto _LL1204;}
_LL1204: if(( unsigned int) _temp1198 > 4u?*(( int*) _temp1198) == Cyc_Absyn_AnonStructType:
0){ _LL1225: _temp1224=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp1198)->f1; goto _LL1205;} else{ goto _LL1206;} _LL1206: if(( unsigned int)
_temp1198 > 4u?*(( int*) _temp1198) == Cyc_Absyn_AnonUnionType: 0){ _LL1227:
_temp1226=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1198)->f1; goto _LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209;
_LL1201: { struct Cyc_Absyn_Structfield* _temp1228= Cyc_Absyn_lookup_struct_field(
_temp1212, f); goto _LL1229; _LL1229: if( _temp1228 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1231= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1230= _temp1215; if( _temp1230 == 0){ _throw(
Null_Exception);} _temp1230;})); struct _tagged_string _temp1232=* f; xprintf("struct %.*s has no %.*s field",
_temp1231.last_plus_one - _temp1231.curr, _temp1231.curr, _temp1232.last_plus_one
- _temp1232.curr, _temp1232.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1233= _new_region(); struct _RegionHandle* rgn=& _temp1233; _push_region(
rgn);{ struct Cyc_List_List* _temp1234=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1212->tvs, _temp1213); goto _LL1235; _LL1235:
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1234,( void*)({ struct Cyc_Absyn_Structfield*
_temp1236= _temp1228; if( _temp1236 == 0){ _throw( Null_Exception);} _temp1236->type;}));};
_pop_region(& _temp1233);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1194);
return t3;}} _LL1203: { struct Cyc_Absyn_Structfield* _temp1237= Cyc_Absyn_lookup_union_field(
_temp1219, f); goto _LL1238; _LL1238: if( _temp1237 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1240= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp1239= _temp1222; if( _temp1239 == 0){ _throw(
Null_Exception);} _temp1239;})); struct _tagged_string _temp1241=* f; xprintf("union %.*s has no %.*s field",
_temp1240.last_plus_one - _temp1240.curr, _temp1240.curr, _temp1241.last_plus_one
- _temp1241.curr, _temp1241.curr);}));}{ struct Cyc_List_List* _temp1242=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1219->tvs, _temp1220); goto _LL1243; _LL1243: { void* _temp1245= Cyc_Tcutil_substitute(
_temp1242,( void*)({ struct Cyc_Absyn_Structfield* _temp1244= _temp1237; if(
_temp1244 == 0){ _throw( Null_Exception);} _temp1244->type;})); goto _LL1246;
_LL1246: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1194); return _temp1245;}}}
_LL1205: { struct Cyc_Absyn_Structfield* _temp1247= Cyc_Absyn_lookup_field(
_temp1224, f); goto _LL1248; _LL1248: if( _temp1247 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1249=* f; xprintf("struct has no %.*s field",
_temp1249.last_plus_one - _temp1249.curr, _temp1249.curr);}));} return( void*)({
struct Cyc_Absyn_Structfield* _temp1250= _temp1247; if( _temp1250 == 0){ _throw(
Null_Exception);} _temp1250->type;});} _LL1207: { struct Cyc_Absyn_Structfield*
_temp1251= Cyc_Absyn_lookup_field( _temp1226, f); goto _LL1252; _LL1252: if(
_temp1251 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1253=* f; xprintf("union has no %.*s field", _temp1253.last_plus_one -
_temp1253.curr, _temp1253.curr);}));} return( void*)({ struct Cyc_Absyn_Structfield*
_temp1254= _temp1251; if( _temp1254 == 0){ _throw( Null_Exception);} _temp1254->type;});}
_LL1209: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1255=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1255.last_plus_one - _temp1255.curr, _temp1255.curr);})); _LL1199:;}
_LL1185: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1256=
Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1256.last_plus_one - _temp1256.curr, _temp1256.curr);})); _LL1181:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1258; _push_handler(& _temp1258);{ int
_temp1260= 0; if( setjmp( _temp1258.handler)){ _temp1260= 1;} if( ! _temp1260){{
void* _temp1261=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1261;}; _pop_handler();} else{
void* _temp1259=( void*) _exn_thrown; void* _temp1263= _temp1259; _LL1265: if(
_temp1263 == Cyc_List_Nth){ goto _LL1266;} else{ goto _LL1267;} _LL1267: goto
_LL1268; _LL1266: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1268:(
void) _throw( _temp1263); _LL1264:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp1294= e1->topt; if( _temp1294 == 0){ _throw( Null_Exception);} _temp1294->v;}));
void* t2= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1293= e2->topt;
if( _temp1293 == 0){ _throw( Null_Exception);} _temp1293->v;})); if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ return Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string
_temp1269= Cyc_Absynpp_typ2string( t2); xprintf("expecting int subscript, found %.*s",
_temp1269.last_plus_one - _temp1269.curr, _temp1269.curr);}));}{ void* _temp1270=
t1; struct Cyc_Absyn_PtrInfo _temp1278; struct Cyc_Absyn_Conref* _temp1280;
struct Cyc_Absyn_Tqual _temp1282; struct Cyc_Absyn_Conref* _temp1284; void*
_temp1286; void* _temp1288; struct Cyc_List_List* _temp1290; _LL1272: if((
unsigned int) _temp1270 > 4u?*(( int*) _temp1270) == Cyc_Absyn_PointerType: 0){
_LL1279: _temp1278=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1270)->f1; _LL1289: _temp1288=( void*) _temp1278.elt_typ; goto _LL1287;
_LL1287: _temp1286=( void*) _temp1278.rgn_typ; goto _LL1285; _LL1285: _temp1284=(
struct Cyc_Absyn_Conref*) _temp1278.nullable; goto _LL1283; _LL1283: _temp1282=(
struct Cyc_Absyn_Tqual) _temp1278.tq; goto _LL1281; _LL1281: _temp1280=( struct
Cyc_Absyn_Conref*) _temp1278.bounds; goto _LL1273;} else{ goto _LL1274;} _LL1274:
if(( unsigned int) _temp1270 > 4u?*(( int*) _temp1270) == Cyc_Absyn_TupleType: 0){
_LL1291: _temp1290=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp1270)->f1; goto _LL1275;} else{ goto _LL1276;} _LL1276: goto _LL1277;
_LL1273: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1280);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1280);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1286); return _temp1288; _LL1275: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1290, e2); _LL1277: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1292= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1292.last_plus_one - _temp1292.curr, _temp1292.curr);})); _LL1271:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1296= Cyc_Tcutil_compress(({ void**
_temp1295= topt; if( _temp1295 == 0){ _throw( Null_Exception);}* _temp1295;}));
struct Cyc_List_List* _temp1302; _LL1298: if(( unsigned int) _temp1296 > 4u?*((
int*) _temp1296) == Cyc_Absyn_TupleType: 0){ _LL1303: _temp1302=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp1296)->f1; goto _LL1299;} else{ goto
_LL1300;} _LL1300: goto _LL1301; _LL1299: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1302) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp1304=( char*)"tuple expression has the wrong number of fields";
struct _tagged_string _temp1305; _temp1305.curr= _temp1304; _temp1305.base=
_temp1304; _temp1305.last_plus_one= _temp1304 + 48; _temp1305;})); goto _LL1297;}
for( 0; es != 0; es=({ struct Cyc_List_List* _temp1306= es; if( _temp1306 == 0){
_throw( Null_Exception);} _temp1306->tl;}), _temp1302=({ struct Cyc_List_List*
_temp1307= _temp1302; if( _temp1307 == 0){ _throw( Null_Exception);} _temp1307->tl;})){
void* _temp1309=(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1308=
_temp1302; if( _temp1308 == 0){ _throw( Null_Exception);} _temp1308->hd;}))).f2;
goto _LL1310; _LL1310: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1309,(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1311= es; if( _temp1311 == 0){
_throw( Null_Exception);} _temp1311->hd;})); fields=({ struct Cyc_List_List*
_temp1312=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1312->hd=( void*)({ struct _tuple7* _temp1313=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp1313->f1=(*(( struct _tuple7*)({ struct Cyc_List_List*
_temp1316= _temp1302; if( _temp1316 == 0){ _throw( Null_Exception);} _temp1316->hd;}))).f1;
_temp1313->f2=( void*)({ struct Cyc_Core_Opt* _temp1315=(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1314= es; if( _temp1314 == 0){ _throw( Null_Exception);}
_temp1314->hd;}))->topt; if( _temp1315 == 0){ _throw( Null_Exception);}
_temp1315->v;}); _temp1313;}); _temp1312->tl= fields; _temp1312;});} done= 1;
goto _LL1297; _LL1301: goto _LL1297; _LL1297:;} if( ! done){ for( 0; es != 0; es=({
struct Cyc_List_List* _temp1317= es; if( _temp1317 == 0){ _throw( Null_Exception);}
_temp1317->tl;})){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp1318= es; if( _temp1318 == 0){ _throw( Null_Exception);}
_temp1318->hd;})); fields=({ struct Cyc_List_List* _temp1319=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1319->hd=( void*)({ struct
_tuple7* _temp1320=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1320->f1= Cyc_Absyn_empty_tqual(); _temp1320->f2=( void*)({ struct Cyc_Core_Opt*
_temp1322=(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1321= es; if(
_temp1321 == 0){ _throw( Null_Exception);} _temp1321->hd;}))->topt; if(
_temp1322 == 0){ _throw( Null_Exception);} _temp1322->v;}); _temp1320;});
_temp1319->tl= fields; _temp1319;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1323=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1323[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1324; _temp1324.tag= Cyc_Absyn_TupleType;
_temp1324.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1324;}); _temp1323;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1325=( char*)"tcCompoundLit"; struct
_tagged_string _temp1326; _temp1326.curr= _temp1325; _temp1326.base= _temp1325;
_temp1326.last_plus_one= _temp1325 + 14; _temp1326;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* _temp1331=({ struct
Cyc_Absyn_Const_e_struct* _temp1327=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp1327[ 0]=({ struct
Cyc_Absyn_Const_e_struct _temp1328; _temp1328.tag= Cyc_Absyn_Const_e; _temp1328.f1=(
void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1329=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp1329[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp1330; _temp1330.tag= Cyc_Absyn_Int_c; _temp1330.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp1330.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es);
_temp1330;}); _temp1329;})); _temp1328;}); _temp1327;}); goto _LL1332; _LL1332: {
struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp(( void*) _temp1331, loc); sz_exp->topt=({
struct Cyc_Core_Opt* _temp1333=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1333->v=( void*) Cyc_Absyn_uint_t; _temp1333;});{ void*
res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1353=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1353[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1354; _temp1354.tag= Cyc_Absyn_ArrayType;
_temp1354.f1=( void*) res; _temp1354.f2= Cyc_Absyn_empty_tqual(); _temp1354.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1354;}); _temp1353;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=({ struct Cyc_List_List* _temp1334= es2; if(
_temp1334 == 0){ _throw( Null_Exception);} _temp1334->tl;})){ Cyc_Tcexp_tcExpInitializer(
te, elt_topt,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1335= es2;
if( _temp1335 == 0){ _throw( Null_Exception);} _temp1335->hd;}));}} if( ! Cyc_Tcutil_coerce_list(
te, res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp1336= es; if( _temp1336 == 0){ _throw( Null_Exception);} _temp1336->hd;}))->loc,({
struct _tagged_string _temp1337= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1337.last_plus_one - _temp1337.curr, _temp1337.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des=({ struct Cyc_List_List* _temp1338= des; if(
_temp1338 == 0){ _throw( Null_Exception);} _temp1338->tl;})){ struct Cyc_List_List*
ds=(*(( struct _tuple6*)({ struct Cyc_List_List* _temp1352= des; if( _temp1352
== 0){ _throw( Null_Exception);} _temp1352->hd;}))).f1; if( ds != 0){ void*
_temp1340=( void*)({ struct Cyc_List_List* _temp1339= ds; if( _temp1339 == 0){
_throw( Null_Exception);} _temp1339->hd;}); struct _tagged_string* _temp1346;
struct Cyc_Absyn_Exp* _temp1348; _LL1342: if(*(( int*) _temp1340) == Cyc_Absyn_FieldName){
_LL1347: _temp1346=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp1340)->f1; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*(( int*)
_temp1340) == Cyc_Absyn_ArrayElement){ _LL1349: _temp1348=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp1340)->f1; goto _LL1345;} else{ goto
_LL1341;} _LL1343: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1350=( char*)"only array index designators are supported"; struct
_tagged_string _temp1351; _temp1351.curr= _temp1350; _temp1351.base= _temp1350;
_temp1351.last_plus_one= _temp1350 + 43; _temp1351;})); goto _LL1341; _LL1345:
Cyc_Tcexp_tcExpInitializer( te, 0, _temp1348);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
_temp1348); if( i != offset){ Cyc_Tcutil_terr( _temp1348->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1341;} _LL1341:;}}} return res_t2;}}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1356= Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1355= bound->topt; if( _temp1355 == 0){
_throw( Null_Exception);} _temp1355->v;})); xprintf("expecting unsigned int, found %.*s",
_temp1356.last_plus_one - _temp1356.curr, _temp1356.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp1357=( char*)"comprehension index variable is not declared const!";
struct _tagged_string _temp1358; _temp1358.curr= _temp1357; _temp1358.base=
_temp1357; _temp1358.last_plus_one= _temp1357 + 52; _temp1358;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,(
struct _tagged_string)({ char* _temp1359=( char*)"bound is not constant"; struct
_tagged_string _temp1360; _temp1360.curr= _temp1359; _temp1360.base= _temp1359;
_temp1360.last_plus_one= _temp1359 + 22; _temp1360;}));} if( ! Cyc_Tcutil_is_const_exp(
te, body)){ Cyc_Tcutil_terr( bound->loc,( struct _tagged_string)({ char*
_temp1361=( char*)"body is not constant"; struct _tagged_string _temp1362;
_temp1362.curr= _temp1361; _temp1362.base= _temp1361; _temp1362.last_plus_one=
_temp1361 + 21; _temp1362;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void**
_temp1363= 0; goto _LL1364; _LL1364: { struct Cyc_Absyn_Tqual* _temp1365= 0;
goto _LL1366; _LL1366: if( topt != 0){ void* _temp1368= Cyc_Tcutil_compress(({
void** _temp1367= topt; if( _temp1367 == 0){ _throw( Null_Exception);}*
_temp1367;})); struct Cyc_Absyn_PtrInfo _temp1376; struct Cyc_Absyn_Exp*
_temp1378; struct Cyc_Absyn_Tqual _temp1380; struct Cyc_Absyn_Tqual* _temp1382;
void* _temp1383; void** _temp1385; _LL1370: if(( unsigned int) _temp1368 > 4u?*((
int*) _temp1368) == Cyc_Absyn_PointerType: 0){ _LL1377: _temp1376=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1368)->f1; goto _LL1371;} else{ goto
_LL1372;} _LL1372: if(( unsigned int) _temp1368 > 4u?*(( int*) _temp1368) == Cyc_Absyn_ArrayType:
0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1368)->f1;
_temp1385=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1368)->f1; goto _LL1381;
_LL1381: _temp1380=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1368)->f2; _temp1382=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1368)->f2;
goto _LL1379; _LL1379: _temp1378=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1368)->f3; goto _LL1373;} else{ goto _LL1374;} _LL1374: goto _LL1375;
_LL1371: pinfo= _temp1376; _temp1363=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1365=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1369; _LL1373: _temp1363=(
void**) _temp1385; _temp1365=( struct Cyc_Absyn_Tqual*) _temp1382; goto _LL1369;
_LL1375: goto _LL1369; _LL1369:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1363, body);
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1386=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1386[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1387; _temp1387.tag= Cyc_Absyn_ArrayType;
_temp1387.f1=( void*) t; _temp1387.f2= _temp1365 == 0? Cyc_Absyn_empty_tqual():({
struct Cyc_Absyn_Tqual* _temp1388= _temp1365; if( _temp1388 == 0){ _throw(
Null_Exception);}* _temp1388;}); _temp1387.f3=( struct Cyc_Absyn_Exp*) bound;
_temp1387;}); _temp1386;});}}}} struct _tuple8{ struct Cyc_Absyn_Structfield* f1;
struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp1389=* sd_opt; if( _temp1389 == 0){ _throw(
Null_Exception);} _temp1389;});} else{{ struct _handler_cons _temp1390;
_push_handler(& _temp1390);{ int _temp1392= 0; if( setjmp( _temp1390.handler)){
_temp1392= 1;} if( ! _temp1392){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);;
_pop_handler();} else{ void* _temp1391=( void*) _exn_thrown; void* _temp1394=
_temp1391; _LL1396: if( _temp1394 == Cyc_Dict_Absent){ goto _LL1397;} else{ goto
_LL1398;} _LL1398: goto _LL1399; _LL1397: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1400= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp1400.last_plus_one - _temp1400.curr, _temp1400.curr);})); return topt != 0?({
void** _temp1401= topt; if( _temp1401 == 0){ _throw( Null_Exception);}*
_temp1401;}):( void*) Cyc_Absyn_VoidType; _LL1399:( void) _throw( _temp1394);
_LL1395:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=(
struct _tuple1*)({ struct Cyc_Core_Opt* _temp1402= sd->name; if( _temp1402 == 0){
_throw( Null_Exception);} _temp1402->v;});}}{ struct _RegionHandle _temp1403=
_new_region(); struct _RegionHandle* rgn=& _temp1403; _push_region( rgn);{
struct Cyc_List_List* _temp1404=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); goto _LL1405; _LL1405: { struct Cyc_List_List* _temp1406=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1404); goto
_LL1407; _LL1407: { struct Cyc_Absyn_StructType_struct* _temp1411=({ struct Cyc_Absyn_StructType_struct*
_temp1408=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1408[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1409; _temp1409.tag=
Cyc_Absyn_StructType; _temp1409.f1=( struct _tuple1*)* tn; _temp1409.f2=
_temp1406; _temp1409.f3=({ struct Cyc_Absyn_Structdecl** _temp1410=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1410[ 0]= sd; _temp1410;});
_temp1409;}); _temp1408;}); goto _LL1412; _LL1412: if( topt != 0){ Cyc_Tcutil_unify((
void*) _temp1411,({ void** _temp1413= topt; if( _temp1413 == 0){ _throw(
Null_Exception);}* _temp1413;}));}* otyps=({ struct Cyc_Core_Opt* _temp1414=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1414->v=(
void*) _temp1406; _temp1414;}); if( sd->fields == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp1415=( char*)"can't build abstract struct";
struct _tagged_string _temp1416; _temp1416.curr= _temp1415; _temp1416.base=
_temp1415; _temp1416.last_plus_one= _temp1415 + 28; _temp1416;}));{ void*
_temp1417=( void*) _temp1411; _npop_handler( 0u); return _temp1417;}}{ struct
Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, loc, args,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1435= sd->fields; if( _temp1435 == 0){ _throw(
Null_Exception);} _temp1435->v;})); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1418= fields; if( _temp1418 == 0){ _throw( Null_Exception);} _temp1418->tl;})){
struct _tuple8 _temp1422; struct Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Structfield*
_temp1425; struct _tuple8* _temp1420=( struct _tuple8*)({ struct Cyc_List_List*
_temp1419= fields; if( _temp1419 == 0){ _throw( Null_Exception);} _temp1419->hd;});
_temp1422=* _temp1420; _LL1426: _temp1425= _temp1422.f1; goto _LL1424; _LL1424:
_temp1423= _temp1422.f2; goto _LL1421; _LL1421: { void* _temp1427= Cyc_Tcutil_rsubstitute(
rgn, _temp1404,( void*) _temp1425->type); goto _LL1428; _LL1428: Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp1427, _temp1423); if( ! Cyc_Tcutil_coerce_arg( te, _temp1423,
_temp1427)){ Cyc_Tcutil_terr( _temp1423->loc,({ struct _tagged_string _temp1430=*
_temp1425->name; struct _tagged_string _temp1431= Cyc_Absynpp_qvar2string(* tn);
struct _tagged_string _temp1432= Cyc_Absynpp_typ2string( _temp1427); struct
_tagged_string _temp1433= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1429= _temp1423->topt; if( _temp1429 == 0){ _throw( Null_Exception);}
_temp1429->v;})); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp1430.last_plus_one - _temp1430.curr, _temp1430.curr, _temp1431.last_plus_one
- _temp1431.curr, _temp1431.curr, _temp1432.last_plus_one - _temp1432.curr,
_temp1432.curr, _temp1433.last_plus_one - _temp1433.curr, _temp1433.curr);}));}}}{
void* _temp1434=( void*) _temp1411; _npop_handler( 0u); return _temp1434;}}}}};
_pop_region(& _temp1403);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1436= _new_region(); struct _RegionHandle* rgn=&
_temp1436; _push_region( rgn);{ void* _temp1437= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1443; _LL1439: if(( unsigned int) _temp1437 > 4u?*((
int*) _temp1437) == Cyc_Absyn_AnonStructType: 0){ _LL1444: _temp1443=( struct
Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*) _temp1437)->f1; goto
_LL1440;} else{ goto _LL1441;} _LL1441: goto _LL1442; _LL1440: { struct Cyc_List_List*
fields=(( struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1443); for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1445= fields; if( _temp1445 == 0){ _throw( Null_Exception);} _temp1445->tl;})){
struct _tuple8 _temp1449; struct Cyc_Absyn_Exp* _temp1450; struct Cyc_Absyn_Structfield*
_temp1452; struct _tuple8* _temp1447=( struct _tuple8*)({ struct Cyc_List_List*
_temp1446= fields; if( _temp1446 == 0){ _throw( Null_Exception);} _temp1446->hd;});
_temp1449=* _temp1447; _LL1453: _temp1452= _temp1449.f1; goto _LL1451; _LL1451:
_temp1450= _temp1449.f2; goto _LL1448; _LL1448: Cyc_Tcexp_tcExpInitializer( te,(
void**)(( void**)(( void*)& _temp1452->type)), _temp1450); if( ! Cyc_Tcutil_coerce_arg(
te, _temp1450,( void*) _temp1452->type)){ Cyc_Tcutil_terr( _temp1450->loc,({
struct _tagged_string _temp1455=* _temp1452->name; struct _tagged_string
_temp1456= Cyc_Absynpp_typ2string(( void*) _temp1452->type); struct
_tagged_string _temp1457= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp1454= _temp1450->topt; if( _temp1454 == 0){ _throw( Null_Exception);}
_temp1454->v;})); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1455.last_plus_one - _temp1455.curr, _temp1455.curr, _temp1456.last_plus_one
- _temp1456.curr, _temp1456.curr, _temp1457.last_plus_one - _temp1457.curr,
_temp1457.curr);}));}} goto _LL1438;} _LL1442:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1458=( char*)"tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1459; _temp1459.curr= _temp1458; _temp1459.base=
_temp1458; _temp1459.last_plus_one= _temp1458 + 37; _temp1459;})); goto _LL1438;
_LL1438:;}; _pop_region(& _temp1436);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1460= _new_region(); struct _RegionHandle* rgn=&
_temp1460; _push_region( rgn);{ struct Cyc_List_List* _temp1461=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tud->tvs); goto _LL1462; _LL1462: { struct Cyc_List_List* _temp1463=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn, tuf->tvs); goto
_LL1464; _LL1464: { struct Cyc_List_List* _temp1465=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, _temp1461); goto _LL1466; _LL1466: { struct Cyc_List_List*
_temp1467=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1463); goto
_LL1468; _LL1468: { struct Cyc_List_List* _temp1469=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp1461, _temp1463); goto _LL1470; _LL1470:* all_ref=({ struct Cyc_Core_Opt*
_temp1471=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1471->v=( void*) _temp1465; _temp1471;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1472=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1472->v=( void*) _temp1467; _temp1472;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1520=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1520[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1521; _temp1521.tag= Cyc_Absyn_TunionFieldType; _temp1521.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1522; _temp1522.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1523=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1523[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1524; _temp1524.tag= Cyc_Absyn_KnownTunionfield; _temp1524.f1= tud;
_temp1524.f2= tuf; _temp1524;}); _temp1523;})); _temp1522.targs= _temp1465;
_temp1522;}); _temp1521;}); _temp1520;}); if( topt != 0){ void* _temp1474= Cyc_Tcutil_compress(({
void** _temp1473= topt; if( _temp1473 == 0){ _throw( Null_Exception);}*
_temp1473;})); struct Cyc_Absyn_TunionFieldInfo _temp1482; struct Cyc_Absyn_TunionInfo
_temp1484; void* _temp1486; struct Cyc_List_List* _temp1488; void* _temp1490;
_LL1476: if(( unsigned int) _temp1474 > 4u?*(( int*) _temp1474) == Cyc_Absyn_TunionFieldType:
0){ _LL1483: _temp1482=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1474)->f1; goto _LL1477;} else{ goto _LL1478;} _LL1478: if(( unsigned int)
_temp1474 > 4u?*(( int*) _temp1474) == Cyc_Absyn_TunionType: 0){ _LL1485:
_temp1484=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1474)->f1; _LL1491: _temp1490=( void*) _temp1484.tunion_info; goto _LL1489;
_LL1489: _temp1488=( struct Cyc_List_List*) _temp1484.targs; goto _LL1487;
_LL1487: _temp1486=( void*) _temp1484.rgn; goto _LL1479;} else{ goto _LL1480;}
_LL1480: goto _LL1481; _LL1477: Cyc_Tcutil_unify(({ void** _temp1492= topt; if(
_temp1492 == 0){ _throw( Null_Exception);}* _temp1492;}), res); goto _LL1475;
_LL1479:{ struct Cyc_List_List* a= _temp1465; for( 0; a != 0? _temp1488 != 0: 0;
a=({ struct Cyc_List_List* _temp1493= a; if( _temp1493 == 0){ _throw(
Null_Exception);} _temp1493->tl;}), _temp1488=({ struct Cyc_List_List* _temp1494=
_temp1488; if( _temp1494 == 0){ _throw( Null_Exception);} _temp1494->tl;})){ Cyc_Tcutil_unify((
void*)({ struct Cyc_List_List* _temp1495= a; if( _temp1495 == 0){ _throw(
Null_Exception);} _temp1495->hd;}),( void*)({ struct Cyc_List_List* _temp1496=
_temp1488; if( _temp1496 == 0){ _throw( Null_Exception);} _temp1496->hd;}));}}
if( tuf->typs == 0? es == 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1497=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1497->v=(
void*) res; _temp1497;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1498=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1498[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1499; _temp1499.tag=
Cyc_Absyn_TunionType; _temp1499.f1=({ struct Cyc_Absyn_TunionInfo _temp1500;
_temp1500.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1501=( struct Cyc_Absyn_KnownTunion_struct*) GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1501[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1502; _temp1502.tag=
Cyc_Absyn_KnownTunion; _temp1502.f1= tud; _temp1502;}); _temp1501;})); _temp1500.targs=
_temp1465; _temp1500.rgn=( void*) _temp1486; _temp1500;}); _temp1499;});
_temp1498;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1503= res;
_npop_handler( 0u); return _temp1503;}} goto _LL1475; _LL1481: goto _LL1475;
_LL1475:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es != 0? ts != 0: 0; es=({
struct Cyc_List_List* _temp1504= es; if( _temp1504 == 0){ _throw( Null_Exception);}
_temp1504->tl;}), ts=({ struct Cyc_List_List* _temp1505= ts; if( _temp1505 == 0){
_throw( Null_Exception);} _temp1505->tl;})){ struct Cyc_Absyn_Exp* e=( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1513= es; if( _temp1513 == 0){
_throw( Null_Exception);} _temp1513->hd;}); void* t= Cyc_Tcutil_rsubstitute( rgn,
_temp1469,(*(( struct _tuple7*)({ struct Cyc_List_List* _temp1512= ts; if(
_temp1512 == 0){ _throw( Null_Exception);} _temp1512->hd;}))).f2); Cyc_Tcexp_tcExpInitializer(
te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp1509= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1510= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1511= e->topt == 0?( struct _tagged_string)({ char* _temp1506=( char*)"?";
struct _tagged_string _temp1507; _temp1507.curr= _temp1506; _temp1507.base=
_temp1506; _temp1507.last_plus_one= _temp1506 + 2; _temp1507;}): Cyc_Absynpp_typ2string((
void*)({ struct Cyc_Core_Opt* _temp1508= e->topt; if( _temp1508 == 0){ _throw(
Null_Exception);} _temp1508->v;})); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1509.last_plus_one - _temp1509.curr, _temp1509.curr, _temp1510.last_plus_one
- _temp1510.curr, _temp1510.curr, _temp1511.last_plus_one - _temp1511.curr,
_temp1511.curr);}));}} if( es != 0){ void* _temp1516= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp1514= es; if( _temp1514 == 0){
_throw( Null_Exception);} _temp1514->hd;}))->loc,({ struct _tagged_string
_temp1515= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1515.last_plus_one - _temp1515.curr, _temp1515.curr);})); _npop_handler( 0u);
return _temp1516;} if( ts != 0){ void* _temp1518= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1517= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1517.last_plus_one - _temp1517.curr, _temp1517.curr);})); _npop_handler( 0u);
return _temp1518;}{ void* _temp1519= res; _npop_handler( 0u); return _temp1519;}}}}}}}};
_pop_region(& _temp1460);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1536=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1536[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1537; _temp1537.tag=
Cyc_Absyn_RgnHandleType; _temp1537.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1537;}); _temp1536;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1535= ropt;
if( _temp1535 == 0){ _throw( Null_Exception);} _temp1535;})); void* _temp1525=
Cyc_Tcutil_compress( handle_type); void* _temp1531; _LL1527: if(( unsigned int)
_temp1525 > 4u?*(( int*) _temp1525) == Cyc_Absyn_RgnHandleType: 0){ _LL1532:
_temp1531=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp1525)->f1;
goto _LL1528;} else{ goto _LL1529;} _LL1529: goto _LL1530; _LL1528: rgn=
_temp1531; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto _LL1526; _LL1530:
Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1533= ropt; if( _temp1533 == 0){
_throw( Null_Exception);} _temp1533->loc;}),({ struct _tagged_string _temp1534=
Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1534.last_plus_one - _temp1534.curr, _temp1534.curr);})); goto _LL1526;
_LL1526:;} Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),(
void*) Cyc_Absyn_MemKind, t);{ void* _temp1538= t; struct Cyc_Absyn_TunionFieldInfo
_temp1544; struct Cyc_List_List* _temp1546; void* _temp1548; struct Cyc_Absyn_Tunionfield*
_temp1550; struct Cyc_Absyn_Tuniondecl* _temp1552; _LL1540: if(( unsigned int)
_temp1538 > 4u?*(( int*) _temp1538) == Cyc_Absyn_TunionFieldType: 0){ _LL1545:
_temp1544=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1538)->f1; _LL1549: _temp1548=( void*) _temp1544.field_info; if(*(( int*)
_temp1548) == Cyc_Absyn_KnownTunionfield){ _LL1553: _temp1552=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp1548)->f1; goto _LL1551; _LL1551:
_temp1550=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1548)->f2; goto _LL1547;} else{ goto _LL1542;} _LL1547: _temp1546=( struct
Cyc_List_List*) _temp1544.targs; goto _LL1541;} else{ goto _LL1542;} _LL1542:
goto _LL1543; _LL1541: if( _temp1550->tvs != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp1554=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp1555; _temp1555.curr= _temp1554; _temp1555.base=
_temp1554; _temp1555.last_plus_one= _temp1554 + 50; _temp1555;}));} goto _LL1539;
_LL1543: goto _LL1539; _LL1539:;}{ void*(* _temp1556)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL1557; _LL1557: if( topt !=
0){ void* _temp1559= Cyc_Tcutil_compress(({ void** _temp1558= topt; if(
_temp1558 == 0){ _throw( Null_Exception);}* _temp1558;})); struct Cyc_Absyn_PtrInfo
_temp1565; struct Cyc_Absyn_Conref* _temp1567; struct Cyc_Absyn_Tqual _temp1569;
struct Cyc_Absyn_Conref* _temp1571; void* _temp1573; void* _temp1575; _LL1561:
if(( unsigned int) _temp1559 > 4u?*(( int*) _temp1559) == Cyc_Absyn_PointerType:
0){ _LL1566: _temp1565=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1559)->f1; _LL1576: _temp1575=( void*) _temp1565.elt_typ; goto _LL1574;
_LL1574: _temp1573=( void*) _temp1565.rgn_typ; goto _LL1572; _LL1572: _temp1571=(
struct Cyc_Absyn_Conref*) _temp1565.nullable; goto _LL1570; _LL1570: _temp1569=(
struct Cyc_Absyn_Tqual) _temp1565.tq; goto _LL1568; _LL1568: _temp1567=( struct
Cyc_Absyn_Conref*) _temp1565.bounds; goto _LL1562;} else{ goto _LL1563;} _LL1563:
goto _LL1564; _LL1562: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1571)){ _temp1556= Cyc_Absyn_star_typ;} goto _LL1560; _LL1564: goto _LL1560;
_LL1560:;} return _temp1556( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1577=( void*) s->r; struct Cyc_Absyn_Exp* _temp1587;
struct Cyc_Absyn_Stmt* _temp1589; struct Cyc_Absyn_Stmt* _temp1591; struct Cyc_Absyn_Stmt*
_temp1593; struct Cyc_Absyn_Decl* _temp1595; _LL1579: if(( unsigned int)
_temp1577 > 1u?*(( int*) _temp1577) == Cyc_Absyn_Exp_s: 0){ _LL1588: _temp1587=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1577)->f1; goto
_LL1580;} else{ goto _LL1581;} _LL1581: if(( unsigned int) _temp1577 > 1u?*((
int*) _temp1577) == Cyc_Absyn_Seq_s: 0){ _LL1592: _temp1591=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1577)->f1; goto _LL1590; _LL1590: _temp1589=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1577)->f2; goto
_LL1582;} else{ goto _LL1583;} _LL1583: if(( unsigned int) _temp1577 > 1u?*((
int*) _temp1577) == Cyc_Absyn_Decl_s: 0){ _LL1596: _temp1595=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1577)->f1; goto _LL1594; _LL1594:
_temp1593=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1577)->f2;
goto _LL1584;} else{ goto _LL1585;} _LL1585: goto _LL1586; _LL1580: return( void*)({
struct Cyc_Core_Opt* _temp1597= _temp1587->topt; if( _temp1597 == 0){ _throw(
Null_Exception);} _temp1597->v;}); _LL1582: s= _temp1589; continue; _LL1584: s=
_temp1593; continue; _LL1586: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp1598=( char*)"statement expression must end with expression";
struct _tagged_string _temp1599; _temp1599.curr= _temp1598; _temp1599.base=
_temp1598; _temp1599.last_plus_one= _temp1598 + 46; _temp1599;})); _LL1578:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp1600=( char*)"tcCodegen"; struct
_tagged_string _temp1601; _temp1601.curr= _temp1600; _temp1601.base= _temp1600;
_temp1601.last_plus_one= _temp1600 + 10; _temp1601;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1602=( char*)"tcFill"; struct _tagged_string _temp1603; _temp1603.curr=
_temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one= _temp1602 + 7;
_temp1603;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1615=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1615[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1616; _temp1616.tag= Cyc_Absyn_RgnHandleType; _temp1616.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1616;}); _temp1615;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1614= rgn_handle; if( _temp1614 == 0){ _throw( Null_Exception);} _temp1614;}));
void* _temp1604= Cyc_Tcutil_compress( handle_type); void* _temp1610; _LL1606:
if(( unsigned int) _temp1604 > 4u?*(( int*) _temp1604) == Cyc_Absyn_RgnHandleType:
0){ _LL1611: _temp1610=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1604)->f1; goto _LL1607;} else{ goto _LL1608;} _LL1608: goto _LL1609;
_LL1607: rgn= _temp1610; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1605; _LL1609: Cyc_Tcutil_terr(({ struct Cyc_Absyn_Exp* _temp1612= rgn_handle;
if( _temp1612 == 0){ _throw( Null_Exception);} _temp1612->loc;}),({ struct
_tagged_string _temp1613= Cyc_Absynpp_typ2string( handle_type); xprintf("expecting region_t type but found %.*s",
_temp1613.last_plus_one - _temp1613.curr, _temp1613.curr);})); goto _LL1605;
_LL1605:;}{ void* _temp1617=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1629;
struct Cyc_Absyn_Exp* _temp1631; struct Cyc_Absyn_Vardecl* _temp1633; struct Cyc_List_List*
_temp1635; struct Cyc_Core_Opt* _temp1637; struct Cyc_List_List* _temp1639; void*
_temp1641; struct _tagged_string _temp1643; _LL1619: if(*(( int*) _temp1617) ==
Cyc_Absyn_Comprehension_e){ _LL1634: _temp1633=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1617)->f1; goto _LL1632; _LL1632:
_temp1631=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1617)->f2; goto _LL1630; _LL1630: _temp1629=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1617)->f3; goto _LL1620;} else{
goto _LL1621;} _LL1621: if(*(( int*) _temp1617) == Cyc_Absyn_UnresolvedMem_e){
_LL1638: _temp1637=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1617)->f1; goto _LL1636; _LL1636: _temp1635=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1617)->f2; goto _LL1622;} else{
goto _LL1623;} _LL1623: if(*(( int*) _temp1617) == Cyc_Absyn_Array_e){ _LL1640:
_temp1639=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1617)->f1;
goto _LL1624;} else{ goto _LL1625;} _LL1625: if(*(( int*) _temp1617) == Cyc_Absyn_Const_e){
_LL1642: _temp1641=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1617)->f1;
if(( unsigned int) _temp1641 > 1u?*(( int*) _temp1641) == Cyc_Absyn_String_c: 0){
_LL1644: _temp1643=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1641)->f1; goto _LL1626;} else{ goto _LL1627;}} else{ goto _LL1627;}
_LL1627: goto _LL1628; _LL1620: { void* _temp1645= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); goto _LL1646; _LL1646: { void* _temp1647= Cyc_Tcutil_compress(
_temp1645); struct Cyc_Absyn_Exp* _temp1653; struct Cyc_Absyn_Tqual _temp1655;
void* _temp1657; _LL1649: if(( unsigned int) _temp1647 > 4u?*(( int*) _temp1647)
== Cyc_Absyn_ArrayType: 0){ _LL1658: _temp1657=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1647)->f1; goto _LL1656; _LL1656: _temp1655=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1647)->f2; goto _LL1654; _LL1654:
_temp1653=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1647)->f3; goto _LL1650;} else{ goto _LL1651;} _LL1651: goto _LL1652;
_LL1650: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1631)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1667=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1667[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1668; _temp1668.tag= Cyc_Absyn_Upper_b; _temp1668.f1=
_temp1631; _temp1668;}); _temp1667;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1664=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1664[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1665; _temp1665.tag= Cyc_Absyn_PointerType;
_temp1665.f1=({ struct Cyc_Absyn_PtrInfo _temp1666; _temp1666.elt_typ=( void*)
_temp1657; _temp1666.rgn_typ=( void*) rgn; _temp1666.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1666.tq= _temp1655; _temp1666.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1666;});
_temp1665;}); _temp1664;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void**
_temp1659= topt; if( _temp1659 == 0){ _throw( Null_Exception);}* _temp1659;}),
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void** _temp1660= topt;
if( _temp1660 == 0){ _throw( Null_Exception);}* _temp1660;})): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1661=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1661->v=( void*) res_typ; _temp1661;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1662= topt; if( _temp1662 == 0){ _throw( Null_Exception);}*
_temp1662;})); res_typ=({ void** _temp1663= topt; if( _temp1663 == 0){ _throw(
Null_Exception);}* _temp1663;});}} return res_typ;} _LL1652: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1669=( char*)"tcNew: comprehension returned non-array type"; struct
_tagged_string _temp1670; _temp1670.curr= _temp1669; _temp1670.base= _temp1669;
_temp1670.last_plus_one= _temp1669 + 45; _temp1670;})); _LL1648:;}} _LL1622:(
void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp1671=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1671[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1672; _temp1672.tag= Cyc_Absyn_Array_e;
_temp1672.f1= _temp1635; _temp1672;}); _temp1671;}))); _temp1639= _temp1635;
goto _LL1624; _LL1624: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1674=
Cyc_Tcutil_compress(({ void** _temp1673= topt; if( _temp1673 == 0){ _throw(
Null_Exception);}* _temp1673;})); struct Cyc_Absyn_PtrInfo _temp1680; struct Cyc_Absyn_Conref*
_temp1682; struct Cyc_Absyn_Tqual _temp1684; struct Cyc_Absyn_Conref* _temp1686;
void* _temp1688; void* _temp1690; void** _temp1692; _LL1676: if(( unsigned int)
_temp1674 > 4u?*(( int*) _temp1674) == Cyc_Absyn_PointerType: 0){ _LL1681:
_temp1680=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1674)->f1; _LL1691: _temp1690=( void*) _temp1680.elt_typ; _temp1692=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1674)->f1).elt_typ; goto _LL1689;
_LL1689: _temp1688=( void*) _temp1680.rgn_typ; goto _LL1687; _LL1687: _temp1686=(
struct Cyc_Absyn_Conref*) _temp1680.nullable; goto _LL1685; _LL1685: _temp1684=(
struct Cyc_Absyn_Tqual) _temp1680.tq; goto _LL1683; _LL1683: _temp1682=( struct
Cyc_Absyn_Conref*) _temp1680.bounds; goto _LL1677;} else{ goto _LL1678;} _LL1678:
goto _LL1679; _LL1677: elt_typ_opt=( void**) _temp1692; goto _LL1675; _LL1679:
goto _LL1675; _LL1675:;}{ void* _temp1693= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); goto _LL1694; _LL1694: { void* res_typ;{ void* _temp1695= Cyc_Tcutil_compress(
_temp1693); struct Cyc_Absyn_Exp* _temp1701; struct Cyc_Absyn_Tqual _temp1703;
void* _temp1705; _LL1697: if(( unsigned int) _temp1695 > 4u?*(( int*) _temp1695)
== Cyc_Absyn_ArrayType: 0){ _LL1706: _temp1705=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1695)->f1; goto _LL1704; _LL1704: _temp1703=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_ArrayType_struct*) _temp1695)->f2; goto _LL1702; _LL1702:
_temp1701=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1695)->f3; goto _LL1698;} else{ goto _LL1699;} _LL1699: goto _LL1700;
_LL1698: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1707=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1707[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1708; _temp1708.tag=
Cyc_Absyn_PointerType; _temp1708.f1=({ struct Cyc_Absyn_PtrInfo _temp1709;
_temp1709.elt_typ=( void*) _temp1705; _temp1709.rgn_typ=( void*) rgn; _temp1709.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1709.tq=
_temp1703; _temp1709.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1710=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1710[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1711; _temp1711.tag= Cyc_Absyn_Upper_b; _temp1711.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1712= _temp1701; if(
_temp1712 == 0){ _throw( Null_Exception);} _temp1712;}); _temp1711;}); _temp1710;}));
_temp1709;}); _temp1708;}); _temp1707;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({
void** _temp1713= topt; if( _temp1713 == 0){ _throw( Null_Exception);}*
_temp1713;}), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,({ void**
_temp1714= topt; if( _temp1714 == 0){ _throw( Null_Exception);}* _temp1714;})):
0){ e->topt=({ struct Cyc_Core_Opt* _temp1715=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1715->v=( void*) res_typ; _temp1715;}); Cyc_Tcutil_unchecked_cast(
te, e,({ void** _temp1716= topt; if( _temp1716 == 0){ _throw( Null_Exception);}*
_temp1716;})); res_typ=({ void** _temp1717= topt; if( _temp1717 == 0){ _throw(
Null_Exception);}* _temp1717;});}} goto _LL1696; _LL1700: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp1718=( char*)"tcExpNoPromote on Array_e returned non-array type"; struct
_tagged_string _temp1719; _temp1719.curr= _temp1718; _temp1719.base= _temp1718;
_temp1719.last_plus_one= _temp1718 + 50; _temp1719;})); _LL1696:;} return
res_typ;}}} _LL1626: { void* _temp1720= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); goto _LL1721;
_LL1721: { void* _temp1722= Cyc_Tcexp_tcExp( te,( void**)& _temp1720, e1); goto
_LL1723; _LL1723: return Cyc_Absyn_atb_typ( _temp1722, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1724=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1724[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1725; _temp1725.tag= Cyc_Absyn_Upper_b; _temp1725.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1725;}); _temp1724;}));}} _LL1628: { void**
topt2= 0; if( topt != 0){ void* _temp1727= Cyc_Tcutil_compress(({ void**
_temp1726= topt; if( _temp1726 == 0){ _throw( Null_Exception);}* _temp1726;}));
struct Cyc_Absyn_PtrInfo _temp1735; struct Cyc_Absyn_Conref* _temp1737; struct
Cyc_Absyn_Tqual _temp1739; struct Cyc_Absyn_Conref* _temp1741; void* _temp1743;
void* _temp1745; void** _temp1747; struct Cyc_Absyn_TunionInfo _temp1748;
_LL1729: if(( unsigned int) _temp1727 > 4u?*(( int*) _temp1727) == Cyc_Absyn_PointerType:
0){ _LL1736: _temp1735=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1727)->f1; _LL1746: _temp1745=( void*) _temp1735.elt_typ; _temp1747=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1727)->f1).elt_typ; goto _LL1744;
_LL1744: _temp1743=( void*) _temp1735.rgn_typ; goto _LL1742; _LL1742: _temp1741=(
struct Cyc_Absyn_Conref*) _temp1735.nullable; goto _LL1740; _LL1740: _temp1739=(
struct Cyc_Absyn_Tqual) _temp1735.tq; goto _LL1738; _LL1738: _temp1737=( struct
Cyc_Absyn_Conref*) _temp1735.bounds; goto _LL1730;} else{ goto _LL1731;} _LL1731:
if(( unsigned int) _temp1727 > 4u?*(( int*) _temp1727) == Cyc_Absyn_TunionType:
0){ _LL1749: _temp1748=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1727)->f1; goto _LL1732;} else{ goto _LL1733;} _LL1733: goto _LL1734;
_LL1730: topt2=( void**) _temp1747; goto _LL1728; _LL1732: topt2=({ void**
_temp1750=( void**) GC_malloc( sizeof( void*)); _temp1750[ 0]=({ void**
_temp1751= topt; if( _temp1751 == 0){ _throw( Null_Exception);}* _temp1751;});
_temp1750;}); goto _LL1728; _LL1734: goto _LL1728; _LL1728:;}{ void* _temp1752=
Cyc_Tcexp_tcExp( te, topt2, e1); goto _LL1753; _LL1753: { void* res_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1759=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1759[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1760; _temp1760.tag= Cyc_Absyn_PointerType;
_temp1760.f1=({ struct Cyc_Absyn_PtrInfo _temp1761; _temp1761.elt_typ=( void*)
_temp1752; _temp1761.rgn_typ=( void*) rgn; _temp1761.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1761.tq= Cyc_Absyn_empty_tqual(); _temp1761.bounds=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1762=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1762[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1763; _temp1763.tag= Cyc_Absyn_Upper_b; _temp1763.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1763;}); _temp1762;})); _temp1761;}); _temp1760;});
_temp1759;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(({ void** _temp1754= topt;
if( _temp1754 == 0){ _throw( Null_Exception);}* _temp1754;}), res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,({ void** _temp1755= topt; if( _temp1755 == 0){ _throw(
Null_Exception);}* _temp1755;})): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1756=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1756->v=(
void*) res_typ; _temp1756;}); Cyc_Tcutil_unchecked_cast( te, e,({ void**
_temp1757= topt; if( _temp1757 == 0){ _throw( Null_Exception);}* _temp1757;}));
res_typ=({ void** _temp1758= topt; if( _temp1758 == 0){ _throw( Null_Exception);}*
_temp1758;});}} return res_typ;}}} _LL1618:;}} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1764= t; struct Cyc_Absyn_Exp*
_temp1770; struct Cyc_Absyn_Tqual _temp1772; void* _temp1774; _LL1766: if((
unsigned int) _temp1764 > 4u?*(( int*) _temp1764) == Cyc_Absyn_ArrayType: 0){
_LL1775: _temp1774=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1764)->f1;
goto _LL1773; _LL1773: _temp1772=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1764)->f2; goto _LL1771; _LL1771: _temp1770=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1764)->f3; goto _LL1767;} else{ goto
_LL1768;} _LL1768: goto _LL1769; _LL1767: { void* _temp1778; int _temp1780;
struct _tuple5 _temp1776= Cyc_Tcutil_addressof_props( te, e); _LL1781: _temp1780=
_temp1776.f1; goto _LL1779; _LL1779: _temp1778= _temp1776.f2; goto _LL1777;
_LL1777: { void* _temp1785= _temp1770 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1782=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1782[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1783; _temp1783.tag= Cyc_Absyn_Upper_b; _temp1783.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1784= _temp1770; if(
_temp1784 == 0){ _throw( Null_Exception);} _temp1784;}); _temp1783;}); _temp1782;});
goto _LL1786; _LL1786: t= Cyc_Absyn_atb_typ( _temp1774, _temp1778, _temp1772,
_temp1785); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1769: return t;
_LL1765:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1787=( void*) e->r; struct Cyc_List_List* _temp1797; struct Cyc_Absyn_Exp*
_temp1799; struct Cyc_Absyn_Exp* _temp1801; struct Cyc_Absyn_Vardecl* _temp1803;
void* _temp1805; struct _tagged_string _temp1807; _LL1789: if(*(( int*)
_temp1787) == Cyc_Absyn_Array_e){ _LL1798: _temp1797=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1787)->f1; goto _LL1790;} else{ goto
_LL1791;} _LL1791: if(*(( int*) _temp1787) == Cyc_Absyn_Comprehension_e){
_LL1804: _temp1803=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1787)->f1; goto _LL1802; _LL1802: _temp1801=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1787)->f2; goto _LL1800; _LL1800:
_temp1799=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1787)->f3; goto _LL1792;} else{ goto _LL1793;} _LL1793: if(*(( int*)
_temp1787) == Cyc_Absyn_Const_e){ _LL1806: _temp1805=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1787)->f1; if(( unsigned int) _temp1805 > 1u?*(( int*) _temp1805) == Cyc_Absyn_String_c:
0){ _LL1808: _temp1807=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1805)->f1; goto _LL1794;} else{ goto _LL1795;}} else{ goto _LL1795;}
_LL1795: goto _LL1796; _LL1790: return t; _LL1792: return t; _LL1794: return t;
_LL1796: t= Cyc_Tcutil_compress( t);{ void* _temp1809= t; struct Cyc_Absyn_Exp*
_temp1815; struct Cyc_Absyn_Tqual _temp1817; void* _temp1819; _LL1811: if((
unsigned int) _temp1809 > 4u?*(( int*) _temp1809) == Cyc_Absyn_ArrayType: 0){
_LL1820: _temp1819=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1809)->f1;
goto _LL1818; _LL1818: _temp1817=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1809)->f2; goto _LL1816; _LL1816: _temp1815=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1809)->f3; goto _LL1812;} else{ goto
_LL1813;} _LL1813: goto _LL1814; _LL1812: { void* _temp1823; int _temp1825;
struct _tuple5 _temp1821= Cyc_Tcutil_addressof_props( te, e); _LL1826: _temp1825=
_temp1821.f1; goto _LL1824; _LL1824: _temp1823= _temp1821.f2; goto _LL1822;
_LL1822: { void* b= _temp1815 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1827=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1827[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1828; _temp1828.tag= Cyc_Absyn_Upper_b; _temp1828.f1=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1829= _temp1815; if(
_temp1829 == 0){ _throw( Null_Exception);} _temp1829;}); _temp1828;}); _temp1827;});
t= Cyc_Absyn_atb_typ( _temp1819, _temp1823, _temp1817, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1814: return t; _LL1810:;} _LL1788:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1830=( void*) e->r; struct Cyc_Absyn_Exp* _temp1836; _LL1832: if(*(( int*)
_temp1830) == Cyc_Absyn_NoInstantiate_e){ _LL1837: _temp1836=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1830)->f1; goto _LL1833;} else{
goto _LL1834;} _LL1834: goto _LL1835; _LL1833: Cyc_Tcexp_tcExpNoInst( te, topt,
_temp1836);( void*)({ struct Cyc_Core_Opt* _temp1838= _temp1836->topt; if(
_temp1838 == 0){ _throw( Null_Exception);} _temp1838->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp1839= _temp1836->topt;
if( _temp1839 == 0){ _throw( Null_Exception);} _temp1839->v;})));}); e->topt=
_temp1836->topt; goto _LL1831; _LL1835: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)({ struct Cyc_Core_Opt* _temp1840= e->topt; if( _temp1840 == 0){ _throw(
Null_Exception);} _temp1840->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1841= e->topt; if( _temp1841 == 0){ _throw(
Null_Exception);} _temp1841->v;})));});{ void* _temp1843= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp1842= e->topt; if( _temp1842 == 0){ _throw(
Null_Exception);} _temp1842->v;})); struct Cyc_Absyn_PtrInfo _temp1849; struct
Cyc_Absyn_Conref* _temp1851; struct Cyc_Absyn_Tqual _temp1853; struct Cyc_Absyn_Conref*
_temp1855; void* _temp1857; void* _temp1859; _LL1845: if(( unsigned int)
_temp1843 > 4u?*(( int*) _temp1843) == Cyc_Absyn_PointerType: 0){ _LL1850:
_temp1849=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1843)->f1; _LL1860: _temp1859=( void*) _temp1849.elt_typ; goto _LL1858;
_LL1858: _temp1857=( void*) _temp1849.rgn_typ; goto _LL1856; _LL1856: _temp1855=(
struct Cyc_Absyn_Conref*) _temp1849.nullable; goto _LL1854; _LL1854: _temp1853=(
struct Cyc_Absyn_Tqual) _temp1849.tq; goto _LL1852; _LL1852: _temp1851=( struct
Cyc_Absyn_Conref*) _temp1849.bounds; goto _LL1846;} else{ goto _LL1847;} _LL1847:
goto _LL1848; _LL1846:{ void* _temp1861= Cyc_Tcutil_compress( _temp1859); struct
Cyc_Absyn_FnInfo _temp1867; struct Cyc_List_List* _temp1869; int _temp1871;
struct Cyc_List_List* _temp1873; void* _temp1875; struct Cyc_Core_Opt* _temp1877;
struct Cyc_List_List* _temp1879; _LL1863: if(( unsigned int) _temp1861 > 4u?*((
int*) _temp1861) == Cyc_Absyn_FnType: 0){ _LL1868: _temp1867=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1861)->f1; _LL1880: _temp1879=( struct Cyc_List_List*)
_temp1867.tvars; goto _LL1878; _LL1878: _temp1877=( struct Cyc_Core_Opt*)
_temp1867.effect; goto _LL1876; _LL1876: _temp1875=( void*) _temp1867.ret_typ;
goto _LL1874; _LL1874: _temp1873=( struct Cyc_List_List*) _temp1867.args; goto
_LL1872; _LL1872: _temp1871=( int) _temp1867.varargs; goto _LL1870; _LL1870:
_temp1869=( struct Cyc_List_List*) _temp1867.attributes; goto _LL1864;} else{
goto _LL1865;} _LL1865: goto _LL1866; _LL1864: if( _temp1879 != 0){ struct
_RegionHandle _temp1881= _new_region(); struct _RegionHandle* rgn=& _temp1881;
_push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1879); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1888=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1888[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1889; _temp1889.tag= Cyc_Absyn_FnType; _temp1889.f1=({ struct Cyc_Absyn_FnInfo
_temp1890; _temp1890.tvars= 0; _temp1890.effect= _temp1877; _temp1890.ret_typ=(
void*) _temp1875; _temp1890.args= _temp1873; _temp1890.varargs= _temp1871;
_temp1890.attributes= _temp1869; _temp1890;}); _temp1889;}); _temp1888;})); void*
new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1885=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1885[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1886; _temp1886.tag= Cyc_Absyn_PointerType;
_temp1886.f1=({ struct Cyc_Absyn_PtrInfo _temp1887; _temp1887.elt_typ=( void*)
ftyp; _temp1887.rgn_typ=( void*) _temp1857; _temp1887.nullable= _temp1855;
_temp1887.tq= _temp1853; _temp1887.bounds= _temp1851; _temp1887;}); _temp1886;});
_temp1885;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1882=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1882[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1883; _temp1883.tag= Cyc_Absyn_Instantiate_e;
_temp1883.f1= inner; _temp1883.f2= ts; _temp1883;}); _temp1882;}))); e->topt=({
struct Cyc_Core_Opt* _temp1884=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1884->v=( void*) new_typ; _temp1884;});}; _pop_region(&
_temp1881);} goto _LL1862; _LL1866: goto _LL1862; _LL1862:;} goto _LL1844;
_LL1848: goto _LL1844; _LL1844:;} goto _LL1831; _LL1831:;} return( void*)({
struct Cyc_Core_Opt* _temp1891= e->topt; if( _temp1891 == 0){ _throw(
Null_Exception);} _temp1891->v;});} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1892=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1964; struct _tuple1* _temp1966; struct Cyc_List_List* _temp1968; struct
Cyc_Absyn_Exp* _temp1970; struct Cyc_List_List* _temp1972; struct Cyc_Core_Opt*
_temp1974; void* _temp1976; void* _temp1978; struct _tuple1* _temp1980; struct
Cyc_List_List* _temp1982; void* _temp1984; void* _temp1986; struct Cyc_Absyn_Exp*
_temp1988; struct Cyc_Absyn_Exp* _temp1990; struct Cyc_Core_Opt* _temp1992;
struct Cyc_Absyn_Exp* _temp1994; struct Cyc_Absyn_Exp* _temp1996; struct Cyc_Absyn_Exp*
_temp1998; struct Cyc_Absyn_Exp* _temp2000; struct Cyc_Absyn_Exp* _temp2002;
struct Cyc_Absyn_Exp* _temp2004; struct Cyc_List_List* _temp2006; struct Cyc_Absyn_Exp*
_temp2008; struct Cyc_Absyn_Exp* _temp2010; struct Cyc_List_List* _temp2012;
struct Cyc_Absyn_Exp* _temp2014; struct Cyc_Absyn_Exp* _temp2016; void*
_temp2018; struct Cyc_Absyn_Exp* _temp2020; struct Cyc_Absyn_Exp* _temp2022;
struct Cyc_Absyn_Exp* _temp2024; struct Cyc_Absyn_Exp* _temp2026; void*
_temp2028; struct Cyc_Absyn_Exp* _temp2030; struct _tagged_string* _temp2032;
struct Cyc_Absyn_Exp* _temp2034; struct _tagged_string* _temp2036; struct Cyc_Absyn_Exp*
_temp2038; struct Cyc_Absyn_Exp* _temp2040; struct Cyc_Absyn_Exp* _temp2042;
struct Cyc_List_List* _temp2044; struct Cyc_List_List* _temp2046; struct _tuple2*
_temp2048; struct Cyc_List_List* _temp2050; struct Cyc_Absyn_Stmt* _temp2052;
struct Cyc_Absyn_Fndecl* _temp2054; struct Cyc_Absyn_Exp* _temp2056; struct Cyc_Absyn_Exp*
_temp2058; struct Cyc_Absyn_Exp* _temp2060; struct Cyc_Absyn_Vardecl* _temp2062;
struct Cyc_Absyn_Structdecl* _temp2064; struct Cyc_Absyn_Structdecl** _temp2066;
struct Cyc_List_List* _temp2067; struct Cyc_Core_Opt* _temp2069; struct Cyc_Core_Opt**
_temp2071; struct _tuple1* _temp2072; struct _tuple1** _temp2074; struct Cyc_List_List*
_temp2075; void* _temp2077; struct Cyc_Absyn_Tunionfield* _temp2079; struct Cyc_Absyn_Tuniondecl*
_temp2081; struct Cyc_List_List* _temp2083; struct Cyc_Core_Opt* _temp2085;
struct Cyc_Core_Opt** _temp2087; struct Cyc_Core_Opt* _temp2088; struct Cyc_Core_Opt**
_temp2090; struct Cyc_Absyn_Enumfield* _temp2091; struct Cyc_Absyn_Enumdecl*
_temp2093; struct _tuple1* _temp2095; void* _temp2097; struct Cyc_Absyn_Exp*
_temp2099; _LL1894: if(*(( int*) _temp1892) == Cyc_Absyn_NoInstantiate_e){
_LL1965: _temp1964=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1892)->f1; goto _LL1895;} else{ goto _LL1896;} _LL1896: if(*(( int*)
_temp1892) == Cyc_Absyn_UnknownId_e){ _LL1967: _temp1966=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp1892)->f1; goto _LL1897;} else{ goto
_LL1898;} _LL1898: if(*(( int*) _temp1892) == Cyc_Absyn_UnknownCall_e){ _LL1971:
_temp1970=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1892)->f1; goto _LL1969; _LL1969: _temp1968=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1892)->f2; goto _LL1899;} else{
goto _LL1900;} _LL1900: if(*(( int*) _temp1892) == Cyc_Absyn_UnresolvedMem_e){
_LL1975: _temp1974=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1892)->f1; goto _LL1973; _LL1973: _temp1972=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1892)->f2; goto _LL1901;} else{
goto _LL1902;} _LL1902: if(*(( int*) _temp1892) == Cyc_Absyn_Const_e){ _LL1977:
_temp1976=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1892)->f1; goto
_LL1903;} else{ goto _LL1904;} _LL1904: if(*(( int*) _temp1892) == Cyc_Absyn_Var_e){
_LL1981: _temp1980=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1892)->f1; goto _LL1979; _LL1979: _temp1978=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1892)->f2; goto _LL1905;} else{ goto _LL1906;} _LL1906: if(*(( int*)
_temp1892) == Cyc_Absyn_Primop_e){ _LL1985: _temp1984=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1892)->f1; goto _LL1983; _LL1983: _temp1982=( struct Cyc_List_List*)((
struct Cyc_Absyn_Primop_e_struct*) _temp1892)->f2; goto _LL1907;} else{ goto
_LL1908;} _LL1908: if(*(( int*) _temp1892) == Cyc_Absyn_Increment_e){ _LL1989:
_temp1988=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1892)->f1; goto _LL1987; _LL1987: _temp1986=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1892)->f2; goto _LL1909;} else{ goto _LL1910;} _LL1910: if(*(( int*)
_temp1892) == Cyc_Absyn_AssignOp_e){ _LL1995: _temp1994=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1892)->f1; goto _LL1993; _LL1993:
_temp1992=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1892)->f2; goto _LL1991; _LL1991: _temp1990=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1892)->f3; goto _LL1911;} else{ goto
_LL1912;} _LL1912: if(*(( int*) _temp1892) == Cyc_Absyn_Conditional_e){ _LL2001:
_temp2000=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1892)->f1; goto _LL1999; _LL1999: _temp1998=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1892)->f2; goto _LL1997; _LL1997:
_temp1996=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1892)->f3; goto _LL1913;} else{ goto _LL1914;} _LL1914: if(*(( int*)
_temp1892) == Cyc_Absyn_SeqExp_e){ _LL2005: _temp2004=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp1892)->f1; goto _LL2003; _LL2003:
_temp2002=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1892)->f2; goto _LL1915;} else{ goto _LL1916;} _LL1916: if(*(( int*)
_temp1892) == Cyc_Absyn_FnCall_e){ _LL2009: _temp2008=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp1892)->f1; goto _LL2007; _LL2007:
_temp2006=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1892)->f2; goto _LL1917;} else{ goto _LL1918;} _LL1918: if(*(( int*)
_temp1892) == Cyc_Absyn_Throw_e){ _LL2011: _temp2010=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Throw_e_struct*) _temp1892)->f1; goto _LL1919;} else{ goto
_LL1920;} _LL1920: if(*(( int*) _temp1892) == Cyc_Absyn_Instantiate_e){ _LL2015:
_temp2014=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1892)->f1; goto _LL2013; _LL2013: _temp2012=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1892)->f2; goto _LL1921;} else{
goto _LL1922;} _LL1922: if(*(( int*) _temp1892) == Cyc_Absyn_Cast_e){ _LL2019:
_temp2018=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1892)->f1; goto
_LL2017; _LL2017: _temp2016=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1892)->f2; goto _LL1923;} else{ goto _LL1924;} _LL1924: if(*(( int*)
_temp1892) == Cyc_Absyn_Address_e){ _LL2021: _temp2020=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1892)->f1; goto _LL1925;} else{ goto
_LL1926;} _LL1926: if(*(( int*) _temp1892) == Cyc_Absyn_New_e){ _LL2025:
_temp2024=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp1892)->f1;
goto _LL2023; _LL2023: _temp2022=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1892)->f2; goto _LL1927;} else{ goto _LL1928;} _LL1928: if(*(( int*)
_temp1892) == Cyc_Absyn_Sizeofexp_e){ _LL2027: _temp2026=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Sizeofexp_e_struct*) _temp1892)->f1; goto _LL1929;} else{ goto
_LL1930;} _LL1930: if(*(( int*) _temp1892) == Cyc_Absyn_Sizeoftyp_e){ _LL2029:
_temp2028=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1892)->f1; goto
_LL1931;} else{ goto _LL1932;} _LL1932: if(*(( int*) _temp1892) == Cyc_Absyn_Deref_e){
_LL2031: _temp2030=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp1892)->f1; goto _LL1933;} else{ goto _LL1934;} _LL1934: if(*(( int*)
_temp1892) == Cyc_Absyn_StructMember_e){ _LL2035: _temp2034=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1892)->f1; goto _LL2033; _LL2033:
_temp2032=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1892)->f2; goto _LL1935;} else{ goto _LL1936;} _LL1936: if(*(( int*)
_temp1892) == Cyc_Absyn_StructArrow_e){ _LL2039: _temp2038=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1892)->f1; goto _LL2037; _LL2037:
_temp2036=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1892)->f2; goto _LL1937;} else{ goto _LL1938;} _LL1938: if(*(( int*)
_temp1892) == Cyc_Absyn_Subscript_e){ _LL2043: _temp2042=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1892)->f1; goto _LL2041; _LL2041:
_temp2040=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1892)->f2; goto _LL1939;} else{ goto _LL1940;} _LL1940: if(*(( int*)
_temp1892) == Cyc_Absyn_Tuple_e){ _LL2045: _temp2044=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp1892)->f1; goto _LL1941;} else{ goto
_LL1942;} _LL1942: if(*(( int*) _temp1892) == Cyc_Absyn_CompoundLit_e){ _LL2049:
_temp2048=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1892)->f1;
goto _LL2047; _LL2047: _temp2046=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1892)->f2; goto _LL1943;} else{ goto _LL1944;} _LL1944: if(*(( int*)
_temp1892) == Cyc_Absyn_Array_e){ _LL2051: _temp2050=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp1892)->f1; goto _LL1945;} else{ goto
_LL1946;} _LL1946: if(*(( int*) _temp1892) == Cyc_Absyn_StmtExp_e){ _LL2053:
_temp2052=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1892)->f1; goto _LL1947;} else{ goto _LL1948;} _LL1948: if(*(( int*)
_temp1892) == Cyc_Absyn_Codegen_e){ _LL2055: _temp2054=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp1892)->f1; goto _LL1949;} else{ goto
_LL1950;} _LL1950: if(*(( int*) _temp1892) == Cyc_Absyn_Fill_e){ _LL2057:
_temp2056=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1892)->f1;
goto _LL1951;} else{ goto _LL1952;} _LL1952: if(*(( int*) _temp1892) == Cyc_Absyn_Comprehension_e){
_LL2063: _temp2062=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1892)->f1; goto _LL2061; _LL2061: _temp2060=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1892)->f2; goto _LL2059; _LL2059:
_temp2058=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1892)->f3; goto _LL1953;} else{ goto _LL1954;} _LL1954: if(*(( int*)
_temp1892) == Cyc_Absyn_Struct_e){ _LL2073: _temp2072=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1892)->f1; _temp2074=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1892)->f1; goto _LL2070; _LL2070: _temp2069=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1892)->f2; _temp2071=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1892)->f2; goto _LL2068; _LL2068: _temp2067=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1892)->f3; goto _LL2065; _LL2065:
_temp2064=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1892)->f4; _temp2066=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1892)->f4;
goto _LL1955;} else{ goto _LL1956;} _LL1956: if(*(( int*) _temp1892) == Cyc_Absyn_AnonStruct_e){
_LL2078: _temp2077=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1892)->f1;
goto _LL2076; _LL2076: _temp2075=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1892)->f2; goto _LL1957;} else{ goto _LL1958;} _LL1958: if(*(( int*)
_temp1892) == Cyc_Absyn_Tunion_e){ _LL2089: _temp2088=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1892)->f1; _temp2090=&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1892)->f1; goto _LL2086; _LL2086: _temp2085=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1892)->f2; _temp2087=&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1892)->f2; goto _LL2084; _LL2084: _temp2083=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1892)->f3; goto _LL2082; _LL2082:
_temp2081=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1892)->f4; goto _LL2080; _LL2080: _temp2079=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1892)->f5; goto _LL1959;} else{ goto
_LL1960;} _LL1960: if(*(( int*) _temp1892) == Cyc_Absyn_Enum_e){ _LL2096:
_temp2095=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1892)->f1;
goto _LL2094; _LL2094: _temp2093=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1892)->f2; goto _LL2092; _LL2092: _temp2091=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1892)->f3; goto _LL1961;} else{ goto
_LL1962;} _LL1962: if(*(( int*) _temp1892) == Cyc_Absyn_Malloc_e){ _LL2100:
_temp2099=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1892)->f1; goto _LL2098; _LL2098: _temp2097=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1892)->f2; goto _LL1963;} else{ goto _LL1893;} _LL1895: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1964); return; _LL1897: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1966); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1899: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1970, _temp1968); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1901: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1972); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1903: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1976, e);
goto _LL1893; _LL1905: t= Cyc_Tcexp_tcVar( te, loc, _temp1980, _temp1978); goto
_LL1893; _LL1907: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1984, _temp1982);
goto _LL1893; _LL1909: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1988,
_temp1986); goto _LL1893; _LL1911: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1994, _temp1992, _temp1990); goto _LL1893; _LL1913: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp2000, _temp1998, _temp1996); goto _LL1893; _LL1915: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp2004, _temp2002); goto _LL1893; _LL1917: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp2008, _temp2006); goto _LL1893; _LL1919: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp2010); goto _LL1893; _LL1921: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp2014, _temp2012); goto _LL1893; _LL1923: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp2018, _temp2016); goto _LL1893; _LL1925: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp2020); goto _LL1893; _LL1927: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp2024, e, _temp2022); goto _LL1893; _LL1929: { void* _temp2101= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp2026); goto _LL2102; _LL2102: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp2101); goto _LL1893;} _LL1931: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp2028); goto _LL1893; _LL1933: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp2030); goto _LL1893; _LL1935: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp2034, _temp2032); goto _LL1893; _LL1937: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp2038, _temp2036); goto _LL1893; _LL1939: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp2042, _temp2040); goto _LL1893; _LL1941: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp2044); goto _LL1893; _LL1943: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp2048, _temp2046); goto _LL1893; _LL1945: { void** elt_topt= 0;
if( topt != 0){ void* _temp2104= Cyc_Tcutil_compress(({ void** _temp2103= topt;
if( _temp2103 == 0){ _throw( Null_Exception);}* _temp2103;})); struct Cyc_Absyn_Exp*
_temp2110; struct Cyc_Absyn_Tqual _temp2112; void* _temp2114; void** _temp2116;
_LL2106: if(( unsigned int) _temp2104 > 4u?*(( int*) _temp2104) == Cyc_Absyn_ArrayType:
0){ _LL2115: _temp2114=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2104)->f1;
_temp2116=&(( struct Cyc_Absyn_ArrayType_struct*) _temp2104)->f1; goto _LL2113;
_LL2113: _temp2112=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2104)->f2; goto _LL2111; _LL2111: _temp2110=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2104)->f3; goto _LL2107;} else{ goto
_LL2108;} _LL2108: goto _LL2109; _LL2107: elt_topt=( void**) _temp2116; goto
_LL2105; _LL2109: goto _LL2105; _LL2105:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp2050); goto _LL1893;} _LL1947: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp2052); goto _LL1893; _LL1949: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp2054); goto _LL1893; _LL1951: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp2056);
goto _LL1893; _LL1953: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp2062,
_temp2060, _temp2058); goto _LL1893; _LL1955: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp2074, _temp2071, _temp2067, _temp2066); goto _LL1893; _LL1957: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp2077, _temp2075); goto _LL1893; _LL1959: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp2090, _temp2087, _temp2083, _temp2081, _temp2079); goto
_LL1893; _LL1961: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp2117=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2117[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2118; _temp2118.tag= Cyc_Absyn_EnumType;
_temp2118.f1=({ struct Cyc_Absyn_Enumdecl* _temp2119= _temp2093; if( _temp2119
== 0){ _throw( Null_Exception);} _temp2119->name;}); _temp2118.f2= _temp2093;
_temp2118;}); _temp2117;}); goto _LL1893; _LL1963: t= Cyc_Tcexp_tcMalloc( te,
loc, topt, _temp2099, _temp2097); goto _LL1893; _LL1893:;} e->topt=({ struct Cyc_Core_Opt*
_temp2120=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2120->v=( void*) t; _temp2120;});}