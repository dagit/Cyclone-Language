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
_temp78;}); _temp77;}))); _npop_handler( 0u); return; _LL60: if( _temp67->typs
== 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp79= Cyc_Absynpp_qvar2string(
_temp67->name); xprintf("%.*s is a constant, not a function", _temp79.last_plus_one
- _temp79.curr, _temp79.curr);}));}( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct*
_temp80=( struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp80[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp81; _temp81.tag= Cyc_Absyn_Tunion_e;
_temp81.f1= 0; _temp81.f2= 0; _temp81.f3= es; _temp81.f4= _temp69; _temp81.f5=
_temp67; _temp81;}); _temp80;}))); _npop_handler( 0u); return; _npop_handler( 0u);
return; _LL62: { struct Cyc_List_List* _temp82=(( struct Cyc_List_List*(*)(
struct _tuple6*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcexp_make_struct_arg, es); goto _LL83; _LL83: if( _temp71->name != 0){(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp84=(
struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp84[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp85; _temp85.tag= Cyc_Absyn_Struct_e;
_temp85.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp71->name))->v;
_temp85.f2= 0; _temp85.f3= _temp82; _temp85.f4=( struct Cyc_Absyn_Structdecl*)
_temp71; _temp85;}); _temp84;})));} else{ Cyc_Tcutil_terr( e->loc,({ char*
_temp86="missing struct name"; struct _tagged_string _temp87; _temp87.curr=
_temp86; _temp87.base= _temp86; _temp87.last_plus_one= _temp86 + 20; _temp87;}));(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp88=(
struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
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
_temp100;}); _temp99;}))); return; _LL97:( void) _throw( _temp92); _LL93:;}}}
_LL49:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp101=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp101[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp102; _temp102.tag= Cyc_Absyn_FnCall_e;
_temp102.f1= e1; _temp102.f2= es; _temp102;}); _temp101;}))); return; _LL45:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp103=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp103[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp104; _temp104.tag= Cyc_Absyn_Array_e;
_temp104.f1= des; _temp104;}); _temp103;}))); return;}{ void* t=*(( void**)
_check_null( topt)); void* _temp105= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp115; struct Cyc_Absyn_Tqual _temp117; void* _temp119; struct Cyc_Absyn_Structdecl**
_temp121; struct Cyc_List_List* _temp123; struct _tuple1* _temp125; struct Cyc_List_List*
_temp127; _LL107: if(( unsigned int) _temp105 > 4u?*(( int*) _temp105) == Cyc_Absyn_ArrayType:
0){ _LL120: _temp119=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp105)->f1;
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
_check_null( e2->topt))->v;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp775= t;
struct Cyc_Absyn_PtrInfo _temp781; struct Cyc_Absyn_Conref* _temp783; struct Cyc_Absyn_Tqual
_temp785; struct Cyc_Absyn_Conref* _temp787; void* _temp789; void* _temp791;
_LL777: if(( unsigned int) _temp775 > 4u?*(( int*) _temp775) == Cyc_Absyn_PointerType:
0){ _LL782: _temp781=(( struct Cyc_Absyn_PointerType_struct*) _temp775)->f1;
_LL792: _temp791=( void*) _temp781.elt_typ; goto _LL790; _LL790: _temp789=( void*)
_temp781.rgn_typ; goto _LL788; _LL788: _temp787= _temp781.nullable; goto _LL786;
_LL786: _temp785= _temp781.tq; goto _LL784; _LL784: _temp783= _temp781.bounds;
goto _LL778;} else{ goto _LL779;} _LL779: goto _LL780; _LL778: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp789); Cyc_Tcutil_check_nonzero_bound( loc, _temp783);{ void*
_temp793= Cyc_Tcutil_compress( _temp791); struct Cyc_Absyn_FnInfo _temp799;
struct Cyc_List_List* _temp801; int _temp803; struct Cyc_List_List* _temp805;
void* _temp807; struct Cyc_Core_Opt* _temp809; struct Cyc_List_List* _temp811;
_LL795: if(( unsigned int) _temp793 > 4u?*(( int*) _temp793) == Cyc_Absyn_FnType:
0){ _LL800: _temp799=(( struct Cyc_Absyn_FnType_struct*) _temp793)->f1; _LL812:
_temp811= _temp799.tvars; goto _LL810; _LL810: _temp809= _temp799.effect; goto
_LL808; _LL808: _temp807=( void*) _temp799.ret_typ; goto _LL806; _LL806:
_temp805= _temp799.args; goto _LL804; _LL804: _temp803= _temp799.varargs; goto
_LL802; _LL802: _temp801= _temp799.attributes; goto _LL796;} else{ goto _LL797;}
_LL797: goto _LL798; _LL796: if( _temp803){ Cyc_Tcutil_terr( loc,({ char*
_temp813="vararg functions (...) not supported yet"; struct _tagged_string
_temp814; _temp814.curr= _temp813; _temp814.base= _temp813; _temp814.last_plus_one=
_temp813 + 41; _temp814;}));} if( topt != 0){ Cyc_Tcutil_unify( _temp807,*((
void**) _check_null( topt)));} while( es != 0? _temp805 != 0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void*
t2=(*(( struct _tuple2*)(( struct Cyc_List_List*) _check_null( _temp805))->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp815= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); struct
_tagged_string _temp816= Cyc_Absynpp_typ2string( t2); xprintf("actual argument has type %.*s but formal has type %.*s",
_temp815.last_plus_one - _temp815.curr, _temp815.curr, _temp816.last_plus_one -
_temp816.curr, _temp816.curr);}));} es=(( struct Cyc_List_List*) _check_null( es))->tl;
_temp805=(( struct Cyc_List_List*) _check_null( _temp805))->tl;} if( es != 0){
Cyc_Tcutil_terr( loc,({ char* _temp817="too many arguments for function"; struct
_tagged_string _temp818; _temp818.curr= _temp817; _temp818.base= _temp817;
_temp818.last_plus_one= _temp817 + 32; _temp818;}));} if( _temp805 != 0){ Cyc_Tcutil_terr(
loc,({ char* _temp819="too few arguments for function"; struct _tagged_string
_temp820; _temp820.curr= _temp819; _temp820.base= _temp819; _temp820.last_plus_one=
_temp819 + 31; _temp820;}));} Cyc_Tcenv_check_effect_accessible( te, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp809))->v); return _temp807; _LL798:
return Cyc_Tcexp_expr_err( te, loc,({ char* _temp821="expected pointer to function";
struct _tagged_string _temp822; _temp822.curr= _temp821; _temp822.base= _temp821;
_temp822.last_plus_one= _temp821 + 29; _temp822;})); _LL794:;} _LL780: return
Cyc_Tcexp_expr_err( te, loc,({ char* _temp823="expected pointer to function";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 29; _temp824;})); _LL776:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp825= Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("expected xtunion exn but found %.*s",
_temp825.last_plus_one - _temp825.curr, _temp825.curr);}));} return Cyc_Absyn_wildtyp();}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*)
Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v)));{ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp826= t1; struct Cyc_Absyn_PtrInfo
_temp832; struct Cyc_Absyn_Conref* _temp834; struct Cyc_Absyn_Tqual _temp836;
struct Cyc_Absyn_Conref* _temp838; void* _temp840; void* _temp842; _LL828: if((
unsigned int) _temp826 > 4u?*(( int*) _temp826) == Cyc_Absyn_PointerType: 0){
_LL833: _temp832=(( struct Cyc_Absyn_PointerType_struct*) _temp826)->f1; _LL843:
_temp842=( void*) _temp832.elt_typ; goto _LL841; _LL841: _temp840=( void*)
_temp832.rgn_typ; goto _LL839; _LL839: _temp838= _temp832.nullable; goto _LL837;
_LL837: _temp836= _temp832.tq; goto _LL835; _LL835: _temp834= _temp832.bounds;
goto _LL829;} else{ goto _LL830;} _LL830: goto _LL831; _LL829:{ void* _temp844=
Cyc_Tcutil_compress( _temp842); struct Cyc_Absyn_FnInfo _temp850; struct Cyc_List_List*
_temp852; int _temp854; struct Cyc_List_List* _temp856; void* _temp858; struct
Cyc_Core_Opt* _temp860; struct Cyc_List_List* _temp862; _LL846: if((
unsigned int) _temp844 > 4u?*(( int*) _temp844) == Cyc_Absyn_FnType: 0){ _LL851:
_temp850=(( struct Cyc_Absyn_FnType_struct*) _temp844)->f1; _LL863: _temp862=
_temp850.tvars; goto _LL861; _LL861: _temp860= _temp850.effect; goto _LL859;
_LL859: _temp858=( void*) _temp850.ret_typ; goto _LL857; _LL857: _temp856=
_temp850.args; goto _LL855; _LL855: _temp854= _temp850.varargs; goto _LL853;
_LL853: _temp852= _temp850.attributes; goto _LL847;} else{ goto _LL848;} _LL848:
goto _LL849; _LL847: { struct Cyc_List_List* instantiation= 0; if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp862)){ return Cyc_Tcexp_expr_err( te, loc,({ char*
_temp864="type instantiation mismatch"; struct _tagged_string _temp865; _temp865.curr=
_temp864; _temp865.base= _temp864; _temp865.last_plus_one= _temp864 + 28;
_temp865;}));} for( 0; ts != 0;( ts=(( struct Cyc_List_List*) _check_null( ts))->tl,
_temp862=(( struct Cyc_List_List*) _check_null( _temp862))->tl)){ void* k=((
void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)((( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp862))->hd)->kind); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te), k,( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd); instantiation=({ struct Cyc_List_List* _temp866=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp866->hd=( void*)({
struct _tuple4* _temp867=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp867->f1=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null(
_temp862))->hd; _temp867->f2=( void*)(( struct Cyc_List_List*) _check_null( ts))->hd;
_temp867;}); _temp866->tl= instantiation; _temp866;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp871=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp871[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp872; _temp872.tag= Cyc_Absyn_FnType; _temp872.f1=({ struct Cyc_Absyn_FnInfo
_temp873; _temp873.tvars= 0; _temp873.effect= _temp860; _temp873.ret_typ=( void*)
_temp858; _temp873.args= _temp856; _temp873.varargs= _temp854; _temp873.attributes=
_temp852; _temp873;}); _temp872;}); _temp871;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp868=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp868[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp869; _temp869.tag= Cyc_Absyn_PointerType;
_temp869.f1=({ struct Cyc_Absyn_PtrInfo _temp870; _temp870.elt_typ=( void*)
new_fn_typ; _temp870.rgn_typ=( void*) _temp840; _temp870.nullable= _temp838;
_temp870.tq= _temp836; _temp870.bounds= _temp834; _temp870;}); _temp869;});
_temp868;}); return new_typ;}} _LL849: goto _LL845; _LL845:;} goto _LL827;
_LL831: goto _LL827; _LL827:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp874= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp874.last_plus_one - _temp874.curr, _temp874.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( ! Cyc_Tcutil_silent_castable(
te, loc, t2, t)? ! Cyc_Tcutil_castable( te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp875= Cyc_Absynpp_typ2string( t2); struct
_tagged_string _temp876= Cyc_Absynpp_typ2string( t); xprintf("cannot cast %.*s to %.*s",
_temp875.last_plus_one - _temp875.curr, _temp875.curr, _temp876.last_plus_one -
_temp876.curr, _temp876.curr);}));} return t;}} static void* Cyc_Tcexp_tcAddress(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ void** _temp877= 0; goto _LL878; _LL878: { struct Cyc_Absyn_Tqual
_temp879= Cyc_Absyn_empty_tqual(); goto _LL880; _LL880: if( topt != 0){ void*
_temp881= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp887; struct Cyc_Absyn_Conref* _temp889; struct Cyc_Absyn_Tqual _temp891;
struct Cyc_Absyn_Conref* _temp893; void* _temp895; void* _temp897; _LL883: if((
unsigned int) _temp881 > 4u?*(( int*) _temp881) == Cyc_Absyn_PointerType: 0){
_LL888: _temp887=(( struct Cyc_Absyn_PointerType_struct*) _temp881)->f1; _LL898:
_temp897=( void*) _temp887.elt_typ; goto _LL896; _LL896: _temp895=( void*)
_temp887.rgn_typ; goto _LL894; _LL894: _temp893= _temp887.nullable; goto _LL892;
_LL892: _temp891= _temp887.tq; goto _LL890; _LL890: _temp889= _temp887.bounds;
goto _LL884;} else{ goto _LL885;} _LL885: goto _LL886; _LL884: _temp877=({ void**
_temp899=( void**) GC_malloc( sizeof( void*)); _temp899[ 0]= _temp897; _temp899;});
_temp879= _temp891; goto _LL882; _LL886: goto _LL882; _LL882:;} Cyc_Tcexp_tcExpNoInst(
te, _temp877, e);{ void* _temp900=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp908; struct Cyc_List_List* _temp910; struct Cyc_Core_Opt* _temp912; struct
_tuple1* _temp914; struct Cyc_List_List* _temp916; _LL902: if(*(( int*) _temp900)
== Cyc_Absyn_Struct_e){ _LL915: _temp914=(( struct Cyc_Absyn_Struct_e_struct*)
_temp900)->f1; goto _LL913; _LL913: _temp912=(( struct Cyc_Absyn_Struct_e_struct*)
_temp900)->f2; goto _LL911; _LL911: _temp910=(( struct Cyc_Absyn_Struct_e_struct*)
_temp900)->f3; goto _LL909; _LL909: _temp908=(( struct Cyc_Absyn_Struct_e_struct*)
_temp900)->f4; goto _LL903;} else{ goto _LL904;} _LL904: if(*(( int*) _temp900)
== Cyc_Absyn_Tuple_e){ _LL917: _temp916=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp900)->f1; goto _LL905;} else{ goto _LL906;} _LL906: goto _LL907; _LL903:
goto _LL905; _LL905: Cyc_Tcutil_warn( loc,({ char* _temp918="& used to allocate";
struct _tagged_string _temp919; _temp919.curr= _temp918; _temp919.base= _temp918;
_temp919.last_plus_one= _temp918 + 19; _temp919;}));{ void*(* _temp920)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL921; _LL921:
if( topt != 0){ void* _temp922= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_PtrInfo _temp928; struct Cyc_Absyn_Conref* _temp930;
struct Cyc_Absyn_Tqual _temp932; struct Cyc_Absyn_Conref* _temp934; void*
_temp936; void* _temp938; _LL924: if(( unsigned int) _temp922 > 4u?*(( int*)
_temp922) == Cyc_Absyn_PointerType: 0){ _LL929: _temp928=(( struct Cyc_Absyn_PointerType_struct*)
_temp922)->f1; _LL939: _temp938=( void*) _temp928.elt_typ; goto _LL937; _LL937:
_temp936=( void*) _temp928.rgn_typ; goto _LL935; _LL935: _temp934= _temp928.nullable;
goto _LL933; _LL933: _temp932= _temp928.tq; goto _LL931; _LL931: _temp930=
_temp928.bounds; goto _LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925:
if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp934)){
_temp920= Cyc_Absyn_star_typ;} goto _LL923; _LL927: goto _LL923; _LL923:;}
return _temp920(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v,(
void*) Cyc_Absyn_HeapRgn, _temp879);} _LL907: goto _LL901; _LL901:;}{ void*
_temp942; int _temp944; struct _tuple5 _temp940= Cyc_Tcutil_addressof_props( te,
e); _LL945: _temp944= _temp940.f1; goto _LL943; _LL943: _temp942= _temp940.f2;
goto _LL941; _LL941: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp944){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp942, tq); return t;}}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp946= t;
struct Cyc_Absyn_PtrInfo _temp952; struct Cyc_Absyn_Conref* _temp954; struct Cyc_Absyn_Tqual
_temp956; struct Cyc_Absyn_Conref* _temp958; void* _temp960; void* _temp962;
_LL948: if(( unsigned int) _temp946 > 4u?*(( int*) _temp946) == Cyc_Absyn_PointerType:
0){ _LL953: _temp952=(( struct Cyc_Absyn_PointerType_struct*) _temp946)->f1;
_LL963: _temp962=( void*) _temp952.elt_typ; goto _LL961; _LL961: _temp960=( void*)
_temp952.rgn_typ; goto _LL959; _LL959: _temp958= _temp952.nullable; goto _LL957;
_LL957: _temp956= _temp952.tq; goto _LL955; _LL955: _temp954= _temp952.bounds;
goto _LL949;} else{ goto _LL950;} _LL950: goto _LL951; _LL949: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp960); Cyc_Tcutil_check_nonzero_bound( loc, _temp954); return
_temp962; _LL951: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp964= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp964.last_plus_one - _temp964.curr, _temp964.curr);})); _LL947:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp965= t;
struct Cyc_Absyn_Structdecl** _temp981; struct Cyc_Absyn_Structdecl* _temp983;
struct Cyc_List_List* _temp984; struct _tuple1* _temp986; struct Cyc_List_List*
_temp988; struct Cyc_List_List* _temp990; struct Cyc_Absyn_Uniondecl** _temp992;
struct Cyc_Absyn_Uniondecl* _temp994; struct Cyc_List_List* _temp995; struct
_tuple1* _temp997; struct Cyc_Absyn_Exp* _temp1000; struct Cyc_Absyn_Tqual
_temp1002; void* _temp1004; struct Cyc_Absyn_PtrInfo _temp1009; _LL967: if((
unsigned int) _temp965 > 4u?*(( int*) _temp965) == Cyc_Absyn_StructType: 0){
_LL987: _temp986=(( struct Cyc_Absyn_StructType_struct*) _temp965)->f1; goto
_LL985; _LL985: _temp984=(( struct Cyc_Absyn_StructType_struct*) _temp965)->f2;
goto _LL982; _LL982: _temp981=(( struct Cyc_Absyn_StructType_struct*) _temp965)->f3;
if( _temp981 == 0){ goto _LL969;} else{ _temp983=* _temp981; goto _LL968;}}
else{ goto _LL969;} _LL969: if(( unsigned int) _temp965 > 4u?*(( int*) _temp965)
== Cyc_Absyn_AnonStructType: 0){ _LL989: _temp988=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp965)->f1; goto _LL970;} else{ goto _LL971;} _LL971: if(( unsigned int)
_temp965 > 4u?*(( int*) _temp965) == Cyc_Absyn_AnonUnionType: 0){ _LL991:
_temp990=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp965)->f1; goto _LL972;}
else{ goto _LL973;} _LL973: if(( unsigned int) _temp965 > 4u?*(( int*) _temp965)
== Cyc_Absyn_UnionType: 0){ _LL998: _temp997=(( struct Cyc_Absyn_UnionType_struct*)
_temp965)->f1; goto _LL996; _LL996: _temp995=(( struct Cyc_Absyn_UnionType_struct*)
_temp965)->f2; goto _LL993; _LL993: _temp992=(( struct Cyc_Absyn_UnionType_struct*)
_temp965)->f3; if( _temp992 == 0){ goto _LL975;} else{ _temp994=* _temp992; goto
_LL974;}} else{ goto _LL975;} _LL975: if(( unsigned int) _temp965 > 4u?*(( int*)
_temp965) == Cyc_Absyn_ArrayType: 0){ _LL1005: _temp1004=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp965)->f1; goto _LL1003; _LL1003: _temp1002=(( struct Cyc_Absyn_ArrayType_struct*)
_temp965)->f2; goto _LL1001; _LL1001: _temp1000=(( struct Cyc_Absyn_ArrayType_struct*)
_temp965)->f3; goto _LL999;} else{ goto _LL977;} _LL999: if( Cyc_String_zstrcmp(*
f,({ char* _temp1006="size"; struct _tagged_string _temp1007; _temp1007.curr=
_temp1006; _temp1007.base= _temp1006; _temp1007.last_plus_one= _temp1006 + 5;
_temp1007;})) == 0){ goto _LL976;} else{ goto _LL977;} _LL977: if(( unsigned int)
_temp965 > 4u?*(( int*) _temp965) == Cyc_Absyn_PointerType: 0){ _LL1010:
_temp1009=(( struct Cyc_Absyn_PointerType_struct*) _temp965)->f1; goto _LL1008;}
else{ goto _LL979;} _LL1008: if( Cyc_String_zstrcmp(* f,({ char* _temp1011="size";
struct _tagged_string _temp1012; _temp1012.curr= _temp1011; _temp1012.base=
_temp1011; _temp1012.last_plus_one= _temp1011 + 5; _temp1012;})) == 0){ goto
_LL978;} else{ goto _LL979;} _LL979: goto _LL980; _LL968: if( _temp986 == 0){
return Cyc_Tcexp_expr_err( te, loc,({ char* _temp1013="unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp1014; _temp1014.curr= _temp1013; _temp1014.base=
_temp1013; _temp1014.last_plus_one= _temp1013 + 40; _temp1014;}));}{ struct Cyc_Absyn_Structfield*
_temp1015= Cyc_Absyn_lookup_struct_field( _temp983, f); goto _LL1016; _LL1016:
if( _temp1015 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1017= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp986));
struct _tagged_string _temp1018=* f; xprintf("struct %.*s has no %.*s field",
_temp1017.last_plus_one - _temp1017.curr, _temp1017.curr, _temp1018.last_plus_one
- _temp1018.curr, _temp1018.curr);}));}{ void* t2;{ struct _RegionHandle
_temp1019= _new_region(); struct _RegionHandle* rgn=& _temp1019; _push_region(
rgn);{ struct Cyc_List_List* _temp1020=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp983->tvs, _temp984); goto _LL1021; _LL1021:
t2= Cyc_Tcutil_rsubstitute( rgn, _temp1020,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1015))->type);}; _pop_region(& _temp1019);} return t2;}}
_LL970: { struct Cyc_Absyn_Structfield* _temp1022= Cyc_Absyn_lookup_field(
_temp988, f); goto _LL1023; _LL1023: if( _temp1022 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1024=* f; xprintf("struct has no %.*s field",
_temp1024.last_plus_one - _temp1024.curr, _temp1024.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1022))->type;} _LL972: { struct
Cyc_Absyn_Structfield* _temp1025= Cyc_Absyn_lookup_field( _temp990, f); goto
_LL1026; _LL1026: if( _temp1025 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1027=* f; xprintf("union has no %.*s field",
_temp1027.last_plus_one - _temp1027.curr, _temp1027.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1025))->type;} _LL974: if(
_temp997 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ char* _temp1028="unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp1029; _temp1029.curr= _temp1028; _temp1029.base=
_temp1028; _temp1029.last_plus_one= _temp1028 + 39; _temp1029;}));}{ struct Cyc_Absyn_Structfield*
_temp1030= Cyc_Absyn_lookup_union_field( _temp994, f); goto _LL1031; _LL1031:
if( _temp1030 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1032= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp997));
struct _tagged_string _temp1033=* f; xprintf("union %.*s has no %.*s field",
_temp1032.last_plus_one - _temp1032.curr, _temp1032.curr, _temp1033.last_plus_one
- _temp1033.curr, _temp1033.curr);}));}{ struct Cyc_List_List* _temp1034=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp994->tvs, _temp995); goto _LL1035; _LL1035: { void* _temp1036= Cyc_Tcutil_substitute(
_temp1034,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1030))->type);
goto _LL1037; _LL1037: return _temp1036;}}} _LL976:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1038=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1038[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1039; _temp1039.tag= Cyc_Absyn_Primop_e;
_temp1039.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1039.f2=({ struct Cyc_List_List*
_temp1040=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1040->hd=( void*) e; _temp1040->tl= 0; _temp1040;}); _temp1039;});
_temp1038;}))); return Cyc_Absyn_uint_t; _LL978:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1041=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1041[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1042; _temp1042.tag= Cyc_Absyn_Primop_e;
_temp1042.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1042.f2=({ struct Cyc_List_List*
_temp1043=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1043->hd=( void*) e; _temp1043->tl= 0; _temp1043;}); _temp1042;});
_temp1041;}))); return Cyc_Absyn_uint_t; _LL980: if( Cyc_String_zstrcmp(* f,({
char* _temp1044="size"; struct _tagged_string _temp1045; _temp1045.curr=
_temp1044; _temp1045.base= _temp1044; _temp1045.last_plus_one= _temp1044 + 5;
_temp1045;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1046= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp1046.last_plus_one - _temp1046.curr, _temp1046.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1047= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp1047.last_plus_one - _temp1047.curr, _temp1047.curr);}));} _LL966:;}}
static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); void* _temp1048= t; struct Cyc_Absyn_PtrInfo
_temp1054; struct Cyc_Absyn_Conref* _temp1056; struct Cyc_Absyn_Tqual _temp1058;
struct Cyc_Absyn_Conref* _temp1060; void* _temp1062; void* _temp1064; _LL1050:
if(( unsigned int) _temp1048 > 4u?*(( int*) _temp1048) == Cyc_Absyn_PointerType:
0){ _LL1055: _temp1054=(( struct Cyc_Absyn_PointerType_struct*) _temp1048)->f1;
_LL1065: _temp1064=( void*) _temp1054.elt_typ; goto _LL1063; _LL1063: _temp1062=(
void*) _temp1054.rgn_typ; goto _LL1061; _LL1061: _temp1060= _temp1054.nullable;
goto _LL1059; _LL1059: _temp1058= _temp1054.tq; goto _LL1057; _LL1057: _temp1056=
_temp1054.bounds; goto _LL1051;} else{ goto _LL1052;} _LL1052: goto _LL1053;
_LL1051: Cyc_Tcutil_check_nonzero_bound( loc, _temp1056);{ void* _temp1066= Cyc_Tcutil_compress(
_temp1064); struct Cyc_Absyn_Structdecl** _temp1078; struct Cyc_Absyn_Structdecl*
_temp1080; struct Cyc_List_List* _temp1081; struct _tuple1* _temp1083; struct
Cyc_Absyn_Uniondecl** _temp1085; struct Cyc_Absyn_Uniondecl* _temp1087; struct
Cyc_List_List* _temp1088; struct _tuple1* _temp1090; struct Cyc_List_List*
_temp1092; struct Cyc_List_List* _temp1094; _LL1068: if(( unsigned int)
_temp1066 > 4u?*(( int*) _temp1066) == Cyc_Absyn_StructType: 0){ _LL1084:
_temp1083=(( struct Cyc_Absyn_StructType_struct*) _temp1066)->f1; goto _LL1082;
_LL1082: _temp1081=(( struct Cyc_Absyn_StructType_struct*) _temp1066)->f2; goto
_LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_StructType_struct*) _temp1066)->f3;
if( _temp1078 == 0){ goto _LL1070;} else{ _temp1080=* _temp1078; goto _LL1069;}}
else{ goto _LL1070;} _LL1070: if(( unsigned int) _temp1066 > 4u?*(( int*)
_temp1066) == Cyc_Absyn_UnionType: 0){ _LL1091: _temp1090=(( struct Cyc_Absyn_UnionType_struct*)
_temp1066)->f1; goto _LL1089; _LL1089: _temp1088=(( struct Cyc_Absyn_UnionType_struct*)
_temp1066)->f2; goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_UnionType_struct*)
_temp1066)->f3; if( _temp1085 == 0){ goto _LL1072;} else{ _temp1087=* _temp1085;
goto _LL1071;}} else{ goto _LL1072;} _LL1072: if(( unsigned int) _temp1066 > 4u?*((
int*) _temp1066) == Cyc_Absyn_AnonStructType: 0){ _LL1093: _temp1092=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1066)->f1; goto _LL1073;} else{ goto
_LL1074;} _LL1074: if(( unsigned int) _temp1066 > 4u?*(( int*) _temp1066) == Cyc_Absyn_AnonUnionType:
0){ _LL1095: _temp1094=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp1066)->f1;
goto _LL1075;} else{ goto _LL1076;} _LL1076: goto _LL1077; _LL1069: { struct Cyc_Absyn_Structfield*
_temp1096= Cyc_Absyn_lookup_struct_field( _temp1080, f); goto _LL1097; _LL1097:
if( _temp1096 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1098= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1083));
struct _tagged_string _temp1099=* f; xprintf("struct %.*s has no %.*s field",
_temp1098.last_plus_one - _temp1098.curr, _temp1098.curr, _temp1099.last_plus_one
- _temp1099.curr, _temp1099.curr);}));}{ void* t3;{ struct _RegionHandle
_temp1100= _new_region(); struct _RegionHandle* rgn=& _temp1100; _push_region(
rgn);{ struct Cyc_List_List* _temp1101=(( struct Cyc_List_List*(*)( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_rzip)( rgn, rgn, _temp1080->tvs, _temp1081); goto _LL1102; _LL1102:
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1101,( void*)(( struct Cyc_Absyn_Structfield*)
_check_null( _temp1096))->type);}; _pop_region(& _temp1100);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1062); return t3;}} _LL1071: { struct Cyc_Absyn_Structfield*
_temp1103= Cyc_Absyn_lookup_union_field( _temp1087, f); goto _LL1104; _LL1104:
if( _temp1103 == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp1105= Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1090));
struct _tagged_string _temp1106=* f; xprintf("union %.*s has no %.*s field",
_temp1105.last_plus_one - _temp1105.curr, _temp1105.curr, _temp1106.last_plus_one
- _temp1106.curr, _temp1106.curr);}));}{ struct Cyc_List_List* _temp1107=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp1087->tvs, _temp1088); goto _LL1108; _LL1108: { void* _temp1109= Cyc_Tcutil_substitute(
_temp1107,( void*)(( struct Cyc_Absyn_Structfield*) _check_null( _temp1103))->type);
goto _LL1110; _LL1110: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1062);
return _temp1109;}}} _LL1073: { struct Cyc_Absyn_Structfield* _temp1111= Cyc_Absyn_lookup_field(
_temp1092, f); goto _LL1112; _LL1112: if( _temp1111 == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1113=* f; xprintf("struct has no %.*s field",
_temp1113.last_plus_one - _temp1113.curr, _temp1113.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1111))->type;} _LL1075: {
struct Cyc_Absyn_Structfield* _temp1114= Cyc_Absyn_lookup_field( _temp1094, f);
goto _LL1115; _LL1115: if( _temp1114 == 0){ return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1116=* f; xprintf("union has no %.*s field",
_temp1116.last_plus_one - _temp1116.curr, _temp1116.curr);}));} return( void*)((
struct Cyc_Absyn_Structfield*) _check_null( _temp1114))->type;} _LL1077: return
Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp1117= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp1117.last_plus_one -
_temp1117.curr, _temp1117.curr);})); _LL1067:;} _LL1053: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp1118= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp1118.last_plus_one - _temp1118.curr, _temp1118.curr);})); _LL1049:;}}
static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp1119; _push_handler(& _temp1119);{ int
_temp1121= 0; if( setjmp( _temp1119.handler)){ _temp1121= 1;} if( ! _temp1121){{
void* _temp1122=(*(( struct _tuple7*(*)( struct Cyc_List_List* x, int i)) Cyc_List_nth)(
ts,( int) i)).f2; _npop_handler( 0u); return _temp1122;}; _pop_handler();} else{
void* _temp1120=( void*) _exn_thrown; void* _temp1124= _temp1120; _LL1126: if(
_temp1124 == Cyc_List_Nth){ goto _LL1127;} else{ goto _LL1128;} _LL1128: goto
_LL1129; _LL1127: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL1129:(
void) _throw( _temp1124); _LL1125:;}}} static void* Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp(
te, 0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return
Cyc_Tcexp_expr_err( te, e2->loc,({ struct _tagged_string _temp1130= Cyc_Absynpp_typ2string(
t2); xprintf("expecting int subscript, found %.*s", _temp1130.last_plus_one -
_temp1130.curr, _temp1130.curr);}));}{ void* _temp1131= t1; struct Cyc_Absyn_PtrInfo
_temp1139; struct Cyc_Absyn_Conref* _temp1141; struct Cyc_Absyn_Tqual _temp1143;
struct Cyc_Absyn_Conref* _temp1145; void* _temp1147; void* _temp1149; struct Cyc_List_List*
_temp1151; _LL1133: if(( unsigned int) _temp1131 > 4u?*(( int*) _temp1131) ==
Cyc_Absyn_PointerType: 0){ _LL1140: _temp1139=(( struct Cyc_Absyn_PointerType_struct*)
_temp1131)->f1; _LL1150: _temp1149=( void*) _temp1139.elt_typ; goto _LL1148;
_LL1148: _temp1147=( void*) _temp1139.rgn_typ; goto _LL1146; _LL1146: _temp1145=
_temp1139.nullable; goto _LL1144; _LL1144: _temp1143= _temp1139.tq; goto _LL1142;
_LL1142: _temp1141= _temp1139.bounds; goto _LL1134;} else{ goto _LL1135;}
_LL1135: if(( unsigned int) _temp1131 > 4u?*(( int*) _temp1131) == Cyc_Absyn_TupleType:
0){ _LL1152: _temp1151=(( struct Cyc_Absyn_TupleType_struct*) _temp1131)->f1;
goto _LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1134: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp1141);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp1141);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp1147); return _temp1149; _LL1136: return Cyc_Tcexp_ithTupleType(
te, loc, _temp1151, e2); _LL1138: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp1153= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp1153.last_plus_one - _temp1153.curr, _temp1153.curr);})); _LL1132:;}}}
static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp1154= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_List_List* _temp1160; _LL1156: if((
unsigned int) _temp1154 > 4u?*(( int*) _temp1154) == Cyc_Absyn_TupleType: 0){
_LL1161: _temp1160=(( struct Cyc_Absyn_TupleType_struct*) _temp1154)->f1; goto
_LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159; _LL1157: if((( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp1160) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,({ char* _temp1162="tuple expression has the wrong number of fields";
struct _tagged_string _temp1163; _temp1163.curr= _temp1162; _temp1163.base=
_temp1162; _temp1163.last_plus_one= _temp1162 + 48; _temp1163;})); goto _LL1155;}
for( 0; es != 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl, _temp1160=((
struct Cyc_List_List*) _check_null( _temp1160))->tl)){ void* _temp1164=(*((
struct _tuple7*)(( struct Cyc_List_List*) _check_null( _temp1160))->hd)).f2;
goto _LL1165; _LL1165: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1164,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd); fields=({
struct Cyc_List_List* _temp1166=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1166->hd=( void*)({ struct _tuple7* _temp1167=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp1167->f1=(*(( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( _temp1160))->hd)).f1; _temp1167->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->topt))->v; _temp1167;}); _temp1166->tl=
fields; _temp1166;});} done= 1; goto _LL1155; _LL1159: goto _LL1155; _LL1155:;}
if( ! done){ for( 0; es != 0; es=(( struct Cyc_List_List*) _check_null( es))->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd); fields=({ struct Cyc_List_List* _temp1168=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1168->hd=( void*)({ struct
_tuple7* _temp1169=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp1169->f1= Cyc_Absyn_empty_tqual(); _temp1169->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->topt))->v;
_temp1169;}); _temp1168->tl= fields; _temp1168;});}} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp1170=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp1170[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp1171; _temp1171.tag= Cyc_Absyn_TupleType;
_temp1171.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1171;}); _temp1170;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,({
char* _temp1172="tcCompoundLit"; struct _tagged_string _temp1173; _temp1173.curr=
_temp1172; _temp1173.base= _temp1172; _temp1173.last_plus_one= _temp1172 + 14;
_temp1173;}));} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_Absyn_Const_e_struct* _temp1178=({ struct Cyc_Absyn_Const_e_struct*
_temp1174=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1174[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1175; _temp1175.tag= Cyc_Absyn_Const_e;
_temp1175.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1176=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1176[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1177; _temp1177.tag= Cyc_Absyn_Int_c;
_temp1177.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1177.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1177;}); _temp1176;}));
_temp1175;}); _temp1174;}); goto _LL1179; _LL1179: { struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp(( void*) _temp1178, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp1180=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1180->v=( void*) Cyc_Absyn_uint_t; _temp1180;});{ void* res_t2=( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp1194=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1194[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1195; _temp1195.tag= Cyc_Absyn_ArrayType;
_temp1195.f1=( void*) res; _temp1195.f2= Cyc_Absyn_empty_tqual(); _temp1195.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1195;}); _temp1194;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2=(( struct Cyc_List_List*) _check_null( es2))->tl){
Cyc_Tcexp_tcExpInitializer( te, elt_topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es2))->hd);}} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp1181= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp1181.last_plus_one - _temp1181.curr, _temp1181.curr);}));}{ int offset= 0;
for( 0; des != 0;( offset ++, des=(( struct Cyc_List_List*) _check_null( des))->tl)){
struct Cyc_List_List* ds=(*(( struct _tuple6*)(( struct Cyc_List_List*)
_check_null( des))->hd)).f1; if( ds != 0){ void* _temp1182=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_string* _temp1188; struct Cyc_Absyn_Exp*
_temp1190; _LL1184: if(*(( int*) _temp1182) == Cyc_Absyn_FieldName){ _LL1189:
_temp1188=(( struct Cyc_Absyn_FieldName_struct*) _temp1182)->f1; goto _LL1185;}
else{ goto _LL1186;} _LL1186: if(*(( int*) _temp1182) == Cyc_Absyn_ArrayElement){
_LL1191: _temp1190=(( struct Cyc_Absyn_ArrayElement_struct*) _temp1182)->f1;
goto _LL1187;} else{ goto _LL1183;} _LL1185: Cyc_Tcutil_terr( loc,({ char*
_temp1192="only array index designators are supported"; struct _tagged_string
_temp1193; _temp1193.curr= _temp1192; _temp1193.base= _temp1192; _temp1193.last_plus_one=
_temp1192 + 43; _temp1193;})); goto _LL1183; _LL1187: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp1190);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp1190);
if( i != offset){ Cyc_Tcutil_terr( _temp1190->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL1183;} _LL1183:;}}} return res_t2;}}} static void*
Cyc_Tcexp_tcComprehension( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound,
struct Cyc_Absyn_Exp* body){ Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ(
te, bound)){ Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp1196= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( bound->topt))->v); xprintf("expecting unsigned int, found %.*s",
_temp1196.last_plus_one - _temp1196.curr, _temp1196.curr);}));} if( !( vd->tq).q_const){((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp1197="comprehension index variable is not declared const!";
struct _tagged_string _temp1198; _temp1198.curr= _temp1197; _temp1198.base=
_temp1197; _temp1198.last_plus_one= _temp1197 + 52; _temp1198;}));} if( te->le
!= 0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var( loc, te,
vd);} else{ if( ! Cyc_Tcutil_is_const_exp( te, bound)){ Cyc_Tcutil_terr( bound->loc,({
char* _temp1199="bound is not constant"; struct _tagged_string _temp1200;
_temp1200.curr= _temp1199; _temp1200.base= _temp1199; _temp1200.last_plus_one=
_temp1199 + 22; _temp1200;}));} if( ! Cyc_Tcutil_is_const_exp( te, body)){ Cyc_Tcutil_terr(
bound->loc,({ char* _temp1201="body is not constant"; struct _tagged_string
_temp1202; _temp1202.curr= _temp1201; _temp1202.base= _temp1201; _temp1202.last_plus_one=
_temp1201 + 21; _temp1202;}));}}{ struct Cyc_Absyn_PtrInfo pinfo; void**
_temp1203= 0; goto _LL1204; _LL1204: { struct Cyc_Absyn_Tqual* _temp1205= 0;
goto _LL1206; _LL1206: if( topt != 0){ void* _temp1207= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1215; struct Cyc_Absyn_Exp*
_temp1217; struct Cyc_Absyn_Tqual _temp1219; struct Cyc_Absyn_Tqual* _temp1221;
void* _temp1222; void** _temp1224; _LL1209: if(( unsigned int) _temp1207 > 4u?*((
int*) _temp1207) == Cyc_Absyn_PointerType: 0){ _LL1216: _temp1215=(( struct Cyc_Absyn_PointerType_struct*)
_temp1207)->f1; goto _LL1210;} else{ goto _LL1211;} _LL1211: if(( unsigned int)
_temp1207 > 4u?*(( int*) _temp1207) == Cyc_Absyn_ArrayType: 0){ _LL1223:
_temp1222=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1207)->f1;
_temp1224=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1207)->f1); goto
_LL1220; _LL1220: _temp1219=(( struct Cyc_Absyn_ArrayType_struct*) _temp1207)->f2;
_temp1221=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1207)->f2; goto _LL1218;
_LL1218: _temp1217=(( struct Cyc_Absyn_ArrayType_struct*) _temp1207)->f3; goto
_LL1212;} else{ goto _LL1213;} _LL1213: goto _LL1214; _LL1210: pinfo= _temp1215;
_temp1203=( void**)(( void**)(( void*)& pinfo.elt_typ)); _temp1205=( struct Cyc_Absyn_Tqual*)&
pinfo.tq; goto _LL1208; _LL1212: _temp1203=( void**) _temp1224; _temp1205=(
struct Cyc_Absyn_Tqual*) _temp1221; goto _LL1208; _LL1214: goto _LL1208; _LL1208:;}{
void* t= Cyc_Tcexp_tcExp( te, _temp1203, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1225=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1225[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1226; _temp1226.tag= Cyc_Absyn_ArrayType;
_temp1226.f1=( void*) t; _temp1226.f2= _temp1205 == 0? Cyc_Absyn_empty_tqual():*((
struct Cyc_Absyn_Tqual*) _check_null( _temp1205)); _temp1226.f3=( struct Cyc_Absyn_Exp*)
bound; _temp1226;}); _temp1225;});}}}} struct _tuple8{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Exp* f2; } ; static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple1** tn, struct
Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct Cyc_Absyn_Structdecl**
sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)
_check_null(* sd_opt);} else{{ struct _handler_cons _temp1227; _push_handler(&
_temp1227);{ int _temp1229= 0; if( setjmp( _temp1227.handler)){ _temp1229= 1;}
if( ! _temp1229){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ void* _temp1228=( void*) _exn_thrown; void* _temp1231= _temp1228; _LL1233:
if( _temp1231 == Cyc_Dict_Absent){ goto _LL1234;} else{ goto _LL1235;} _LL1235:
goto _LL1236; _LL1234: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1237=
Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s", _temp1237.last_plus_one
- _temp1237.curr, _temp1237.curr);})); return topt != 0?*(( void**) _check_null(
topt)):( void*) Cyc_Absyn_VoidType; _LL1236:( void) _throw( _temp1231); _LL1232:;}}}*
sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=( struct
_tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{ struct
_RegionHandle _temp1238= _new_region(); struct _RegionHandle* rgn=& _temp1238;
_push_region( rgn);{ struct Cyc_List_List* _temp1239=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, sd->tvs); goto _LL1240; _LL1240: { struct Cyc_List_List* _temp1241=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1239); goto
_LL1242; _LL1242: { struct Cyc_Absyn_StructType_struct* _temp1246=({ struct Cyc_Absyn_StructType_struct*
_temp1243=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp1243[ 0]=({ struct Cyc_Absyn_StructType_struct _temp1244; _temp1244.tag=
Cyc_Absyn_StructType; _temp1244.f1=( struct _tuple1*)* tn; _temp1244.f2=
_temp1241; _temp1244.f3=({ struct Cyc_Absyn_Structdecl** _temp1245=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1245[ 0]= sd; _temp1245;});
_temp1244;}); _temp1243;}); goto _LL1247; _LL1247: if( topt != 0){ Cyc_Tcutil_unify((
void*) _temp1246,*(( void**) _check_null( topt)));}* otyps=({ struct Cyc_Core_Opt*
_temp1248=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1248->v=( void*) _temp1241; _temp1248;}); if( sd->fields == 0){ Cyc_Tcutil_terr(
loc,({ char* _temp1249="can't build abstract struct"; struct _tagged_string
_temp1250; _temp1250.curr= _temp1249; _temp1250.base= _temp1249; _temp1250.last_plus_one=
_temp1249 + 28; _temp1250;}));{ void* _temp1251=( void*) _temp1246;
_npop_handler( 0u); return _temp1251;}}{ struct Cyc_List_List* fields=(( struct
Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc,
struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields != 0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple8 _temp1254; struct Cyc_Absyn_Exp* _temp1255; struct Cyc_Absyn_Structfield*
_temp1257; struct _tuple8* _temp1252=( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp1254=* _temp1252; _LL1258: _temp1257= _temp1254.f1;
goto _LL1256; _LL1256: _temp1255= _temp1254.f2; goto _LL1253; _LL1253: { void*
_temp1259= Cyc_Tcutil_rsubstitute( rgn, _temp1239,( void*) _temp1257->type);
goto _LL1260; _LL1260: Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1259,
_temp1255); if( ! Cyc_Tcutil_coerce_arg( te, _temp1255, _temp1259)){ Cyc_Tcutil_terr(
_temp1255->loc,({ struct _tagged_string _temp1261=* _temp1257->name; struct
_tagged_string _temp1262= Cyc_Absynpp_qvar2string(* tn); struct _tagged_string
_temp1263= Cyc_Absynpp_typ2string( _temp1259); struct _tagged_string _temp1264=
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1255->topt))->v);
xprintf("field %.*s of struct %.*s expects type %.*s != %.*s", _temp1261.last_plus_one
- _temp1261.curr, _temp1261.curr, _temp1262.last_plus_one - _temp1262.curr,
_temp1262.curr, _temp1263.last_plus_one - _temp1263.curr, _temp1263.curr,
_temp1264.last_plus_one - _temp1264.curr, _temp1264.curr);}));}}}{ void*
_temp1265=( void*) _temp1246; _npop_handler( 0u); return _temp1265;}}}}};
_pop_region(& _temp1238);}} static void* Cyc_Tcexp_tcAnonStruct( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* ts, struct Cyc_List_List* args){{
struct _RegionHandle _temp1266= _new_region(); struct _RegionHandle* rgn=&
_temp1266; _push_region( rgn);{ void* _temp1267= Cyc_Tcutil_compress( ts);
struct Cyc_List_List* _temp1273; _LL1269: if(( unsigned int) _temp1267 > 4u?*((
int*) _temp1267) == Cyc_Absyn_AnonStructType: 0){ _LL1274: _temp1273=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1267)->f1; goto _LL1270;} else{ goto
_LL1271;} _LL1271: goto _LL1272; _LL1270: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1273); for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct _tuple8 _temp1277; struct Cyc_Absyn_Exp*
_temp1278; struct Cyc_Absyn_Structfield* _temp1280; struct _tuple8* _temp1275=(
struct _tuple8*)(( struct Cyc_List_List*) _check_null( fields))->hd; _temp1277=*
_temp1275; _LL1281: _temp1280= _temp1277.f1; goto _LL1279; _LL1279: _temp1278=
_temp1277.f2; goto _LL1276; _LL1276: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1280->type)), _temp1278); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1278,( void*) _temp1280->type)){ Cyc_Tcutil_terr( _temp1278->loc,({ struct
_tagged_string _temp1282=* _temp1280->name; struct _tagged_string _temp1283= Cyc_Absynpp_typ2string((
void*) _temp1280->type); struct _tagged_string _temp1284= Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1278->topt))->v); xprintf("field %.*s of struct expects type %.*s != %.*s",
_temp1282.last_plus_one - _temp1282.curr, _temp1282.curr, _temp1283.last_plus_one
- _temp1283.curr, _temp1283.curr, _temp1284.last_plus_one - _temp1284.curr,
_temp1284.curr);}));}} goto _LL1268;} _LL1272:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ char* _temp1285="tcAnonStruct:  not an AnonStructType";
struct _tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base=
_temp1285; _temp1286.last_plus_one= _temp1285 + 37; _temp1286;})); goto _LL1268;
_LL1268:;}; _pop_region(& _temp1266);} return ts;} static void* Cyc_Tcexp_tcTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref,
struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield*
tuf){ struct _RegionHandle _temp1287= _new_region(); struct _RegionHandle* rgn=&
_temp1287; _push_region( rgn);{ struct Cyc_List_List* _temp1288=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, tud->tvs); goto _LL1289; _LL1289: { struct Cyc_List_List* _temp1290=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tuple4*(* f)( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*), struct _RegionHandle* env, struct Cyc_List_List*
x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var, rgn, tuf->tvs); goto
_LL1291; _LL1291: { struct Cyc_List_List* _temp1292=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, _temp1288); goto _LL1293; _LL1293: { struct Cyc_List_List*
_temp1294=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, _temp1290); goto
_LL1295; _LL1295: { struct Cyc_List_List* _temp1296=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp1288, _temp1290); goto _LL1297; _LL1297:* all_ref=({ struct Cyc_Core_Opt*
_temp1298=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1298->v=( void*) _temp1292; _temp1298;});* exist_ref=({ struct Cyc_Core_Opt*
_temp1299=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1299->v=( void*) _temp1294; _temp1299;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp1335=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp1335[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp1336; _temp1336.tag= Cyc_Absyn_TunionFieldType; _temp1336.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1337; _temp1337.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1338=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1338[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1339; _temp1339.tag= Cyc_Absyn_KnownTunionfield; _temp1339.f1= tud;
_temp1339.f2= tuf; _temp1339;}); _temp1338;})); _temp1337.targs= _temp1292;
_temp1337;}); _temp1336;}); _temp1335;}); if( topt != 0){ void* _temp1300= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_TunionFieldInfo _temp1308; struct
Cyc_Absyn_TunionInfo _temp1310; void* _temp1312; struct Cyc_List_List* _temp1314;
void* _temp1316; _LL1302: if(( unsigned int) _temp1300 > 4u?*(( int*) _temp1300)
== Cyc_Absyn_TunionFieldType: 0){ _LL1309: _temp1308=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1300)->f1; goto _LL1303;} else{ goto _LL1304;} _LL1304: if(( unsigned int)
_temp1300 > 4u?*(( int*) _temp1300) == Cyc_Absyn_TunionType: 0){ _LL1311:
_temp1310=(( struct Cyc_Absyn_TunionType_struct*) _temp1300)->f1; _LL1317:
_temp1316=( void*) _temp1310.tunion_info; goto _LL1315; _LL1315: _temp1314=
_temp1310.targs; goto _LL1313; _LL1313: _temp1312=( void*) _temp1310.rgn; goto
_LL1305;} else{ goto _LL1306;} _LL1306: goto _LL1307; _LL1303: Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res); goto _LL1301; _LL1305:{ struct Cyc_List_List*
a= _temp1292; for( 0; a != 0? _temp1314 != 0: 0;( a=(( struct Cyc_List_List*)
_check_null( a))->tl, _temp1314=(( struct Cyc_List_List*) _check_null( _temp1314))->tl)){
Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*) _check_null( a))->hd,( void*)((
struct Cyc_List_List*) _check_null( _temp1314))->hd);}} if( tuf->typs == 0? es
== 0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1318=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1318->v=( void*) res; _temp1318;});
res=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp1319=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp1319[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp1320; _temp1320.tag= Cyc_Absyn_TunionType;
_temp1320.f1=({ struct Cyc_Absyn_TunionInfo _temp1321; _temp1321.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp1322=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp1322[ 0]=({
struct Cyc_Absyn_KnownTunion_struct _temp1323; _temp1323.tag= Cyc_Absyn_KnownTunion;
_temp1323.f1= tud; _temp1323;}); _temp1322;})); _temp1321.targs= _temp1292;
_temp1321.rgn=( void*) _temp1312; _temp1321;}); _temp1320;}); _temp1319;}); Cyc_Tcutil_unchecked_cast(
te, e, res);{ void* _temp1324= res; _npop_handler( 0u); return _temp1324;}} goto
_LL1301; _LL1307: goto _LL1301; _LL1301:;}{ struct Cyc_List_List* ts= tuf->typs;
for( 0; es != 0? ts != 0: 0;( es=(( struct Cyc_List_List*) _check_null( es))->tl,
ts=(( struct Cyc_List_List*) _check_null( ts))->tl)){ struct Cyc_Absyn_Exp* e=(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd; void* t=
Cyc_Tcutil_rsubstitute( rgn, _temp1296,(*(( struct _tuple7*)(( struct Cyc_List_List*)
_check_null( ts))->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp1327= Cyc_Absynpp_qvar2string( tuf->name); struct
_tagged_string _temp1328= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp1329= e->topt == 0?({ char* _temp1325="?"; struct _tagged_string _temp1326;
_temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 2; _temp1326;}): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp1327.last_plus_one - _temp1327.curr, _temp1327.curr, _temp1328.last_plus_one
- _temp1328.curr, _temp1328.curr, _temp1329.last_plus_one - _temp1329.curr,
_temp1329.curr);}));}} if( es != 0){ void* _temp1331= Cyc_Tcexp_expr_err( te,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd)->loc,({
struct _tagged_string _temp1330= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too many arguments for tunion constructor %.*s",
_temp1330.last_plus_one - _temp1330.curr, _temp1330.curr);})); _npop_handler( 0u);
return _temp1331;} if( ts != 0){ void* _temp1333= Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp1332= Cyc_Absynpp_qvar2string( tuf->name); xprintf("too few arguments for tunion constructor %.*s",
_temp1332.last_plus_one - _temp1332.curr, _temp1332.curr);})); _npop_handler( 0u);
return _temp1333;}{ void* _temp1334= res; _npop_handler( 0u); return _temp1334;}}}}}}}};
_pop_region(& _temp1287);} static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* ropt,
void* t){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt != 0){ void*
expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1349=(
struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct));
_temp1349[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct _temp1350; _temp1350.tag=
Cyc_Absyn_RgnHandleType; _temp1350.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind);
_temp1350;}); _temp1349;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( ropt)); void* _temp1340= Cyc_Tcutil_compress(
handle_type); void* _temp1346; _LL1342: if(( unsigned int) _temp1340 > 4u?*((
int*) _temp1340) == Cyc_Absyn_RgnHandleType: 0){ _LL1347: _temp1346=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp1340)->f1; goto _LL1343;} else{
goto _LL1344;} _LL1344: goto _LL1345; _LL1343: rgn= _temp1346; Cyc_Tcenv_check_rgn_accessible(
te, loc, rgn); goto _LL1341; _LL1345: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)
_check_null( ropt))->loc,({ struct _tagged_string _temp1348= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1348.last_plus_one
- _temp1348.curr, _temp1348.curr);})); goto _LL1341; _LL1341:;} Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t);{ void*
_temp1351= t; struct Cyc_Absyn_TunionFieldInfo _temp1357; struct Cyc_List_List*
_temp1359; void* _temp1361; struct Cyc_Absyn_Tunionfield* _temp1363; struct Cyc_Absyn_Tuniondecl*
_temp1365; _LL1353: if(( unsigned int) _temp1351 > 4u?*(( int*) _temp1351) ==
Cyc_Absyn_TunionFieldType: 0){ _LL1358: _temp1357=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp1351)->f1; _LL1362: _temp1361=( void*) _temp1357.field_info; if(*(( int*)
_temp1361) == Cyc_Absyn_KnownTunionfield){ _LL1366: _temp1365=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1361)->f1; goto _LL1364; _LL1364: _temp1363=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1361)->f2; goto _LL1360;} else{ goto _LL1355;} _LL1360: _temp1359=
_temp1357.targs; goto _LL1354;} else{ goto _LL1355;} _LL1355: goto _LL1356;
_LL1354: if( _temp1363->tvs != 0){ Cyc_Tcutil_terr( loc,({ char* _temp1367="malloc with existential types not yet implemented";
struct _tagged_string _temp1368; _temp1368.curr= _temp1367; _temp1368.base=
_temp1367; _temp1368.last_plus_one= _temp1367 + 50; _temp1368;}));} goto _LL1352;
_LL1356: goto _LL1352; _LL1352:;}{ void*(* _temp1369)( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; goto _LL1370; _LL1370: if( topt !=
0){ void* _temp1371= Cyc_Tcutil_compress(*(( void**) _check_null( topt)));
struct Cyc_Absyn_PtrInfo _temp1377; struct Cyc_Absyn_Conref* _temp1379; struct
Cyc_Absyn_Tqual _temp1381; struct Cyc_Absyn_Conref* _temp1383; void* _temp1385;
void* _temp1387; _LL1373: if(( unsigned int) _temp1371 > 4u?*(( int*) _temp1371)
== Cyc_Absyn_PointerType: 0){ _LL1378: _temp1377=(( struct Cyc_Absyn_PointerType_struct*)
_temp1371)->f1; _LL1388: _temp1387=( void*) _temp1377.elt_typ; goto _LL1386;
_LL1386: _temp1385=( void*) _temp1377.rgn_typ; goto _LL1384; _LL1384: _temp1383=
_temp1377.nullable; goto _LL1382; _LL1382: _temp1381= _temp1377.tq; goto _LL1380;
_LL1380: _temp1379= _temp1377.bounds; goto _LL1374;} else{ goto _LL1375;}
_LL1375: goto _LL1376; _LL1374: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1383)){ _temp1369= Cyc_Absyn_star_typ;} goto _LL1372; _LL1376: goto _LL1372;
_LL1372:;} return _temp1369( t, rgn, Cyc_Absyn_empty_tqual());}} static void*
Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1389=( void*) s->r; struct Cyc_Absyn_Exp* _temp1399;
struct Cyc_Absyn_Stmt* _temp1401; struct Cyc_Absyn_Stmt* _temp1403; struct Cyc_Absyn_Stmt*
_temp1405; struct Cyc_Absyn_Decl* _temp1407; _LL1391: if(( unsigned int)
_temp1389 > 1u?*(( int*) _temp1389) == Cyc_Absyn_Exp_s: 0){ _LL1400: _temp1399=((
struct Cyc_Absyn_Exp_s_struct*) _temp1389)->f1; goto _LL1392;} else{ goto
_LL1393;} _LL1393: if(( unsigned int) _temp1389 > 1u?*(( int*) _temp1389) == Cyc_Absyn_Seq_s:
0){ _LL1404: _temp1403=(( struct Cyc_Absyn_Seq_s_struct*) _temp1389)->f1; goto
_LL1402; _LL1402: _temp1401=(( struct Cyc_Absyn_Seq_s_struct*) _temp1389)->f2;
goto _LL1394;} else{ goto _LL1395;} _LL1395: if(( unsigned int) _temp1389 > 1u?*((
int*) _temp1389) == Cyc_Absyn_Decl_s: 0){ _LL1408: _temp1407=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1389)->f1; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1389)->f2; goto _LL1396;} else{ goto _LL1397;} _LL1397: goto _LL1398;
_LL1392: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1399->topt))->v;
_LL1394: s= _temp1401; continue; _LL1396: s= _temp1405; continue; _LL1398:
return Cyc_Tcexp_expr_err( te, loc,({ char* _temp1409="statement expression must end with expression";
struct _tagged_string _temp1410; _temp1410.curr= _temp1409; _temp1410.base=
_temp1409; _temp1410.last_plus_one= _temp1409 + 46; _temp1410;})); _LL1390:;}}
static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te,
loc,({ char* _temp1411="tcCodegen"; struct _tagged_string _temp1412; _temp1412.curr=
_temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one= _temp1411 + 10;
_temp1412;}));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err(
te, loc,({ char* _temp1413="tcFill"; struct _tagged_string _temp1414; _temp1414.curr=
_temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one= _temp1413 + 7;
_temp1414;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* rgn_handle, struct
Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){ void* rgn=( void*) Cyc_Absyn_HeapRgn;
if( rgn_handle != 0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1424=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1424[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1425; _temp1425.tag= Cyc_Absyn_RgnHandleType; _temp1425.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp1425;}); _temp1424;}); void* handle_type= Cyc_Tcexp_tcExp(
te,( void**)& expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle));
void* _temp1415= Cyc_Tcutil_compress( handle_type); void* _temp1421; _LL1417:
if(( unsigned int) _temp1415 > 4u?*(( int*) _temp1415) == Cyc_Absyn_RgnHandleType:
0){ _LL1422: _temp1421=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1415)->f1; goto _LL1418;} else{ goto _LL1419;} _LL1419: goto _LL1420;
_LL1418: rgn= _temp1421; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1416; _LL1420: Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) _check_null(
rgn_handle))->loc,({ struct _tagged_string _temp1423= Cyc_Absynpp_typ2string(
handle_type); xprintf("expecting region_t type but found %.*s", _temp1423.last_plus_one
- _temp1423.curr, _temp1423.curr);})); goto _LL1416; _LL1416:;}{ void* _temp1426=(
void*) e1->r; struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Exp* _temp1440;
struct Cyc_Absyn_Vardecl* _temp1442; struct Cyc_List_List* _temp1444; struct Cyc_Core_Opt*
_temp1446; struct Cyc_List_List* _temp1448; void* _temp1450; struct
_tagged_string _temp1452; _LL1428: if(*(( int*) _temp1426) == Cyc_Absyn_Comprehension_e){
_LL1443: _temp1442=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1426)->f1;
goto _LL1441; _LL1441: _temp1440=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1426)->f2; goto _LL1439; _LL1439: _temp1438=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1426)->f3; goto _LL1429;} else{ goto _LL1430;} _LL1430: if(*(( int*)
_temp1426) == Cyc_Absyn_UnresolvedMem_e){ _LL1447: _temp1446=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1426)->f1; goto _LL1445; _LL1445: _temp1444=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1426)->f2; goto _LL1431;} else{ goto _LL1432;} _LL1432: if(*(( int*)
_temp1426) == Cyc_Absyn_Array_e){ _LL1449: _temp1448=(( struct Cyc_Absyn_Array_e_struct*)
_temp1426)->f1; goto _LL1433;} else{ goto _LL1434;} _LL1434: if(*(( int*)
_temp1426) == Cyc_Absyn_Const_e){ _LL1451: _temp1450=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1426)->f1; if(( unsigned int) _temp1450 > 1u?*(( int*) _temp1450) == Cyc_Absyn_String_c:
0){ _LL1453: _temp1452=(( struct Cyc_Absyn_String_c_struct*) _temp1450)->f1;
goto _LL1435;} else{ goto _LL1436;}} else{ goto _LL1436;} _LL1436: goto _LL1437;
_LL1429: { void* _temp1454= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); goto
_LL1455; _LL1455: { void* _temp1456= Cyc_Tcutil_compress( _temp1454); struct Cyc_Absyn_Exp*
_temp1462; struct Cyc_Absyn_Tqual _temp1464; void* _temp1466; _LL1458: if((
unsigned int) _temp1456 > 4u?*(( int*) _temp1456) == Cyc_Absyn_ArrayType: 0){
_LL1467: _temp1466=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1456)->f1;
goto _LL1465; _LL1465: _temp1464=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1456)->f2; goto _LL1463; _LL1463: _temp1462=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1456)->f3; goto _LL1459;} else{ goto _LL1460;} _LL1460: goto _LL1461;
_LL1459: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1440)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1472=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1472[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1473; _temp1473.tag= Cyc_Absyn_Upper_b; _temp1473.f1=
_temp1440; _temp1473;}); _temp1472;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1469=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1469[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1470; _temp1470.tag= Cyc_Absyn_PointerType;
_temp1470.f1=({ struct Cyc_Absyn_PtrInfo _temp1471; _temp1471.elt_typ=( void*)
_temp1466; _temp1471.rgn_typ=( void*) rgn; _temp1471.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1471.tq= _temp1464; _temp1471.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp1471;});
_temp1470;}); _temp1469;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**)
_check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*((
void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1468=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1468->v=(
void*) res_typ; _temp1468;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**)
_check_null( topt))); res_typ=*(( void**) _check_null( topt));}} return res_typ;}
_LL1461: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char*
_temp1474="tcNew: comprehension returned non-array type"; struct _tagged_string
_temp1475; _temp1475.curr= _temp1474; _temp1475.base= _temp1474; _temp1475.last_plus_one=
_temp1474 + 45; _temp1475;})); _LL1457:;}} _LL1431:( void*)( e1->r=( void*)((
void*)({ struct Cyc_Absyn_Array_e_struct* _temp1476=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp1476[ 0]=({ struct
Cyc_Absyn_Array_e_struct _temp1477; _temp1477.tag= Cyc_Absyn_Array_e; _temp1477.f1=
_temp1444; _temp1477;}); _temp1476;}))); _temp1448= _temp1444; goto _LL1433;
_LL1433: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1478= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1484; struct Cyc_Absyn_Conref*
_temp1486; struct Cyc_Absyn_Tqual _temp1488; struct Cyc_Absyn_Conref* _temp1490;
void* _temp1492; void* _temp1494; void** _temp1496; _LL1480: if(( unsigned int)
_temp1478 > 4u?*(( int*) _temp1478) == Cyc_Absyn_PointerType: 0){ _LL1485:
_temp1484=(( struct Cyc_Absyn_PointerType_struct*) _temp1478)->f1; _LL1495:
_temp1494=( void*) _temp1484.elt_typ; _temp1496=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1478)->f1).elt_typ; goto _LL1493; _LL1493: _temp1492=( void*) _temp1484.rgn_typ;
goto _LL1491; _LL1491: _temp1490= _temp1484.nullable; goto _LL1489; _LL1489:
_temp1488= _temp1484.tq; goto _LL1487; _LL1487: _temp1486= _temp1484.bounds;
goto _LL1481;} else{ goto _LL1482;} _LL1482: goto _LL1483; _LL1481: elt_typ_opt=(
void**) _temp1496; goto _LL1479; _LL1483: goto _LL1479; _LL1479:;}{ void*
_temp1497= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); goto _LL1498; _LL1498: {
void* res_typ;{ void* _temp1499= Cyc_Tcutil_compress( _temp1497); struct Cyc_Absyn_Exp*
_temp1505; struct Cyc_Absyn_Tqual _temp1507; void* _temp1509; _LL1501: if((
unsigned int) _temp1499 > 4u?*(( int*) _temp1499) == Cyc_Absyn_ArrayType: 0){
_LL1510: _temp1509=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1499)->f1;
goto _LL1508; _LL1508: _temp1507=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1499)->f2; goto _LL1506; _LL1506: _temp1505=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1499)->f3; goto _LL1502;} else{ goto _LL1503;} _LL1503: goto _LL1504;
_LL1502: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1511=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1511[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1512; _temp1512.tag=
Cyc_Absyn_PointerType; _temp1512.f1=({ struct Cyc_Absyn_PtrInfo _temp1513;
_temp1513.elt_typ=( void*) _temp1509; _temp1513.rgn_typ=( void*) rgn; _temp1513.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1513.tq=
_temp1507; _temp1513.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1514=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1514[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1515; _temp1515.tag= Cyc_Absyn_Upper_b; _temp1515.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1505); _temp1515;}); _temp1514;}));
_temp1513;}); _temp1512;}); _temp1511;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*((
void**) _check_null( topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc,
res_typ,*(( void**) _check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp1516=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1516->v=( void*) res_typ; _temp1516;}); Cyc_Tcutil_unchecked_cast( te, e,*((
void**) _check_null( topt))); res_typ=*(( void**) _check_null( topt));}} goto
_LL1500; _LL1504: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp1517="tcExpNoPromote on Array_e returned non-array type"; struct
_tagged_string _temp1518; _temp1518.curr= _temp1517; _temp1518.base= _temp1517;
_temp1518.last_plus_one= _temp1517 + 50; _temp1518;})); _LL1500:;} return
res_typ;}}} _LL1435: { void* _temp1519= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b); goto _LL1520;
_LL1520: { void* _temp1521= Cyc_Tcexp_tcExp( te,( void**)& _temp1519, e1); goto
_LL1522; _LL1522: return Cyc_Absyn_atb_typ( _temp1521, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1523=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1523[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1524; _temp1524.tag= Cyc_Absyn_Upper_b; _temp1524.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1524;}); _temp1523;}));}} _LL1437: { void**
topt2= 0; if( topt != 0){ void* _temp1525= Cyc_Tcutil_compress(*(( void**)
_check_null( topt))); struct Cyc_Absyn_PtrInfo _temp1533; struct Cyc_Absyn_Conref*
_temp1535; struct Cyc_Absyn_Tqual _temp1537; struct Cyc_Absyn_Conref* _temp1539;
void* _temp1541; void* _temp1543; void** _temp1545; struct Cyc_Absyn_TunionInfo
_temp1546; _LL1527: if(( unsigned int) _temp1525 > 4u?*(( int*) _temp1525) ==
Cyc_Absyn_PointerType: 0){ _LL1534: _temp1533=(( struct Cyc_Absyn_PointerType_struct*)
_temp1525)->f1; _LL1544: _temp1543=( void*) _temp1533.elt_typ; _temp1545=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1525)->f1).elt_typ; goto _LL1542;
_LL1542: _temp1541=( void*) _temp1533.rgn_typ; goto _LL1540; _LL1540: _temp1539=
_temp1533.nullable; goto _LL1538; _LL1538: _temp1537= _temp1533.tq; goto _LL1536;
_LL1536: _temp1535= _temp1533.bounds; goto _LL1528;} else{ goto _LL1529;}
_LL1529: if(( unsigned int) _temp1525 > 4u?*(( int*) _temp1525) == Cyc_Absyn_TunionType:
0){ _LL1547: _temp1546=(( struct Cyc_Absyn_TunionType_struct*) _temp1525)->f1;
goto _LL1530;} else{ goto _LL1531;} _LL1531: goto _LL1532; _LL1528: topt2=( void**)
_temp1545; goto _LL1526; _LL1530: topt2=({ void** _temp1548=( void**) GC_malloc(
sizeof( void*)); _temp1548[ 0]=*(( void**) _check_null( topt)); _temp1548;});
goto _LL1526; _LL1532: goto _LL1526; _LL1526:;}{ void* _temp1549= Cyc_Tcexp_tcExp(
te, topt2, e1); goto _LL1550; _LL1550: { void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1552=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1552[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1553; _temp1553.tag=
Cyc_Absyn_PointerType; _temp1553.f1=({ struct Cyc_Absyn_PtrInfo _temp1554;
_temp1554.elt_typ=( void*) _temp1549; _temp1554.rgn_typ=( void*) rgn; _temp1554.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1554.tq= Cyc_Absyn_empty_tqual();
_temp1554.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1555=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1555[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1556; _temp1556.tag= Cyc_Absyn_Upper_b; _temp1556.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1556;}); _temp1555;})); _temp1554;}); _temp1553;});
_temp1552;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(*(( void**) _check_null(
topt)), res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*(( void**)
_check_null( topt))): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1551=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1551->v=( void*) res_typ;
_temp1551;}); Cyc_Tcutil_unchecked_cast( te, e,*(( void**) _check_null( topt)));
res_typ=*(( void**) _check_null( topt));}} return res_typ;}}} _LL1427:;}} void*
Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t= Cyc_Tcutil_compress( t);{
void* _temp1557= t; struct Cyc_Absyn_Exp* _temp1563; struct Cyc_Absyn_Tqual
_temp1565; void* _temp1567; _LL1559: if(( unsigned int) _temp1557 > 4u?*(( int*)
_temp1557) == Cyc_Absyn_ArrayType: 0){ _LL1568: _temp1567=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1557)->f1; goto _LL1566; _LL1566: _temp1565=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1557)->f2; goto _LL1564; _LL1564: _temp1563=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1557)->f3; goto _LL1560;} else{ goto _LL1561;} _LL1561: goto _LL1562;
_LL1560: { void* _temp1571; int _temp1573; struct _tuple5 _temp1569= Cyc_Tcutil_addressof_props(
te, e); _LL1574: _temp1573= _temp1569.f1; goto _LL1572; _LL1572: _temp1571=
_temp1569.f2; goto _LL1570; _LL1570: { void* _temp1577= _temp1563 == 0?( void*)
Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1575=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1575[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1576; _temp1576.tag= Cyc_Absyn_Upper_b;
_temp1576.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1563); _temp1576;});
_temp1575;}); goto _LL1578; _LL1578: t= Cyc_Absyn_atb_typ( _temp1567, _temp1571,
_temp1565, _temp1577);( void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=(
void*) t); return t;}} _LL1562: return t; _LL1558:;}} void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); void* _temp1579=( void*) e->r; struct Cyc_List_List* _temp1589;
struct Cyc_Absyn_Exp* _temp1591; struct Cyc_Absyn_Exp* _temp1593; struct Cyc_Absyn_Vardecl*
_temp1595; void* _temp1597; struct _tagged_string _temp1599; _LL1581: if(*(( int*)
_temp1579) == Cyc_Absyn_Array_e){ _LL1590: _temp1589=(( struct Cyc_Absyn_Array_e_struct*)
_temp1579)->f1; goto _LL1582;} else{ goto _LL1583;} _LL1583: if(*(( int*)
_temp1579) == Cyc_Absyn_Comprehension_e){ _LL1596: _temp1595=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1579)->f1; goto _LL1594; _LL1594: _temp1593=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1579)->f2; goto _LL1592; _LL1592: _temp1591=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1579)->f3; goto _LL1584;} else{ goto _LL1585;} _LL1585: if(*(( int*)
_temp1579) == Cyc_Absyn_Const_e){ _LL1598: _temp1597=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1579)->f1; if(( unsigned int) _temp1597 > 1u?*(( int*) _temp1597) == Cyc_Absyn_String_c:
0){ _LL1600: _temp1599=(( struct Cyc_Absyn_String_c_struct*) _temp1597)->f1;
goto _LL1586;} else{ goto _LL1587;}} else{ goto _LL1587;} _LL1587: goto _LL1588;
_LL1582: return t; _LL1584: return t; _LL1586: return t; _LL1588: t= Cyc_Tcutil_compress(
t);{ void* _temp1601= t; struct Cyc_Absyn_Exp* _temp1607; struct Cyc_Absyn_Tqual
_temp1609; void* _temp1611; _LL1603: if(( unsigned int) _temp1601 > 4u?*(( int*)
_temp1601) == Cyc_Absyn_ArrayType: 0){ _LL1612: _temp1611=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1601)->f1; goto _LL1610; _LL1610: _temp1609=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1601)->f2; goto _LL1608; _LL1608: _temp1607=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1601)->f3; goto _LL1604;} else{ goto _LL1605;} _LL1605: goto _LL1606;
_LL1604: { void* _temp1615; int _temp1617; struct _tuple5 _temp1613= Cyc_Tcutil_addressof_props(
te, e); _LL1618: _temp1617= _temp1613.f1; goto _LL1616; _LL1616: _temp1615=
_temp1613.f2; goto _LL1614; _LL1614: { void* b= _temp1607 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1619=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1619[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1620; _temp1620.tag= Cyc_Absyn_Upper_b; _temp1620.f1=(
struct Cyc_Absyn_Exp*) _check_null( _temp1607); _temp1620;}); _temp1619;}); t=
Cyc_Absyn_atb_typ( _temp1611, _temp1615, _temp1609, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1606: return t; _LL1602:;} _LL1580:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1621=( void*) e->r; struct Cyc_Absyn_Exp* _temp1627; _LL1623: if(*(( int*)
_temp1621) == Cyc_Absyn_NoInstantiate_e){ _LL1628: _temp1627=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1621)->f1; goto _LL1624;} else{ goto _LL1625;} _LL1625: goto _LL1626;
_LL1624: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1627);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1627->topt))->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1627->topt))->v))); e->topt=
_temp1627->topt; goto _LL1622; _LL1626: Cyc_Tcexp_tcExpNoInst( te, topt, e);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand(
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v)));{
void* _temp1629= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp1635; struct Cyc_Absyn_Conref*
_temp1637; struct Cyc_Absyn_Tqual _temp1639; struct Cyc_Absyn_Conref* _temp1641;
void* _temp1643; void* _temp1645; _LL1631: if(( unsigned int) _temp1629 > 4u?*((
int*) _temp1629) == Cyc_Absyn_PointerType: 0){ _LL1636: _temp1635=(( struct Cyc_Absyn_PointerType_struct*)
_temp1629)->f1; _LL1646: _temp1645=( void*) _temp1635.elt_typ; goto _LL1644;
_LL1644: _temp1643=( void*) _temp1635.rgn_typ; goto _LL1642; _LL1642: _temp1641=
_temp1635.nullable; goto _LL1640; _LL1640: _temp1639= _temp1635.tq; goto _LL1638;
_LL1638: _temp1637= _temp1635.bounds; goto _LL1632;} else{ goto _LL1633;}
_LL1633: goto _LL1634; _LL1632:{ void* _temp1647= Cyc_Tcutil_compress( _temp1645);
struct Cyc_Absyn_FnInfo _temp1653; struct Cyc_List_List* _temp1655; int
_temp1657; struct Cyc_List_List* _temp1659; void* _temp1661; struct Cyc_Core_Opt*
_temp1663; struct Cyc_List_List* _temp1665; _LL1649: if(( unsigned int)
_temp1647 > 4u?*(( int*) _temp1647) == Cyc_Absyn_FnType: 0){ _LL1654: _temp1653=((
struct Cyc_Absyn_FnType_struct*) _temp1647)->f1; _LL1666: _temp1665= _temp1653.tvars;
goto _LL1664; _LL1664: _temp1663= _temp1653.effect; goto _LL1662; _LL1662:
_temp1661=( void*) _temp1653.ret_typ; goto _LL1660; _LL1660: _temp1659=
_temp1653.args; goto _LL1658; _LL1658: _temp1657= _temp1653.varargs; goto
_LL1656; _LL1656: _temp1655= _temp1653.attributes; goto _LL1650;} else{ goto
_LL1651;} _LL1651: goto _LL1652; _LL1650: if( _temp1665 != 0){ struct
_RegionHandle _temp1667= _new_region(); struct _RegionHandle* rgn=& _temp1667;
_push_region( rgn);{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp1665); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple4*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn, inst,(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1674=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1674[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1675; _temp1675.tag= Cyc_Absyn_FnType; _temp1675.f1=({ struct Cyc_Absyn_FnInfo
_temp1676; _temp1676.tvars= 0; _temp1676.effect= _temp1663; _temp1676.ret_typ=(
void*) _temp1661; _temp1676.args= _temp1659; _temp1676.varargs= _temp1657;
_temp1676.attributes= _temp1655; _temp1676;}); _temp1675;}); _temp1674;})); void*
new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1671=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1671[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1672; _temp1672.tag= Cyc_Absyn_PointerType;
_temp1672.f1=({ struct Cyc_Absyn_PtrInfo _temp1673; _temp1673.elt_typ=( void*)
ftyp; _temp1673.rgn_typ=( void*) _temp1643; _temp1673.nullable= _temp1641;
_temp1673.tq= _temp1639; _temp1673.bounds= _temp1637; _temp1673;}); _temp1672;});
_temp1671;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1668=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1668[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1669; _temp1669.tag= Cyc_Absyn_Instantiate_e;
_temp1669.f1= inner; _temp1669.f2= ts; _temp1669;}); _temp1668;}))); e->topt=({
struct Cyc_Core_Opt* _temp1670=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1670->v=( void*) new_typ; _temp1670;});}; _pop_region(&
_temp1667);} goto _LL1648; _LL1652: goto _LL1648; _LL1648:;} goto _LL1630;
_LL1634: goto _LL1630; _LL1630:;} goto _LL1622; _LL1622:;} return( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1677=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1749; struct _tuple1* _temp1751; struct Cyc_List_List* _temp1753; struct
Cyc_Absyn_Exp* _temp1755; struct Cyc_List_List* _temp1757; struct Cyc_Core_Opt*
_temp1759; void* _temp1761; void* _temp1763; struct _tuple1* _temp1765; struct
Cyc_List_List* _temp1767; void* _temp1769; void* _temp1771; struct Cyc_Absyn_Exp*
_temp1773; struct Cyc_Absyn_Exp* _temp1775; struct Cyc_Core_Opt* _temp1777;
struct Cyc_Absyn_Exp* _temp1779; struct Cyc_Absyn_Exp* _temp1781; struct Cyc_Absyn_Exp*
_temp1783; struct Cyc_Absyn_Exp* _temp1785; struct Cyc_Absyn_Exp* _temp1787;
struct Cyc_Absyn_Exp* _temp1789; struct Cyc_List_List* _temp1791; struct Cyc_Absyn_Exp*
_temp1793; struct Cyc_Absyn_Exp* _temp1795; struct Cyc_List_List* _temp1797;
struct Cyc_Absyn_Exp* _temp1799; struct Cyc_Absyn_Exp* _temp1801; void*
_temp1803; struct Cyc_Absyn_Exp* _temp1805; struct Cyc_Absyn_Exp* _temp1807;
struct Cyc_Absyn_Exp* _temp1809; struct Cyc_Absyn_Exp* _temp1811; void*
_temp1813; struct Cyc_Absyn_Exp* _temp1815; struct _tagged_string* _temp1817;
struct Cyc_Absyn_Exp* _temp1819; struct _tagged_string* _temp1821; struct Cyc_Absyn_Exp*
_temp1823; struct Cyc_Absyn_Exp* _temp1825; struct Cyc_Absyn_Exp* _temp1827;
struct Cyc_List_List* _temp1829; struct Cyc_List_List* _temp1831; struct _tuple2*
_temp1833; struct Cyc_List_List* _temp1835; struct Cyc_Absyn_Stmt* _temp1837;
struct Cyc_Absyn_Fndecl* _temp1839; struct Cyc_Absyn_Exp* _temp1841; struct Cyc_Absyn_Exp*
_temp1843; struct Cyc_Absyn_Exp* _temp1845; struct Cyc_Absyn_Vardecl* _temp1847;
struct Cyc_Absyn_Structdecl* _temp1849; struct Cyc_Absyn_Structdecl** _temp1851;
struct Cyc_List_List* _temp1852; struct Cyc_Core_Opt* _temp1854; struct Cyc_Core_Opt**
_temp1856; struct _tuple1* _temp1857; struct _tuple1** _temp1859; struct Cyc_List_List*
_temp1860; void* _temp1862; struct Cyc_Absyn_Tunionfield* _temp1864; struct Cyc_Absyn_Tuniondecl*
_temp1866; struct Cyc_List_List* _temp1868; struct Cyc_Core_Opt* _temp1870;
struct Cyc_Core_Opt** _temp1872; struct Cyc_Core_Opt* _temp1873; struct Cyc_Core_Opt**
_temp1875; struct Cyc_Absyn_Enumfield* _temp1876; struct Cyc_Absyn_Enumdecl*
_temp1878; struct _tuple1* _temp1880; void* _temp1882; struct Cyc_Absyn_Exp*
_temp1884; _LL1679: if(*(( int*) _temp1677) == Cyc_Absyn_NoInstantiate_e){
_LL1750: _temp1749=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1677)->f1;
goto _LL1680;} else{ goto _LL1681;} _LL1681: if(*(( int*) _temp1677) == Cyc_Absyn_UnknownId_e){
_LL1752: _temp1751=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1677)->f1; goto
_LL1682;} else{ goto _LL1683;} _LL1683: if(*(( int*) _temp1677) == Cyc_Absyn_UnknownCall_e){
_LL1756: _temp1755=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp1677)->f1;
goto _LL1754; _LL1754: _temp1753=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1677)->f2; goto _LL1684;} else{ goto _LL1685;} _LL1685: if(*(( int*)
_temp1677) == Cyc_Absyn_UnresolvedMem_e){ _LL1760: _temp1759=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1677)->f1; goto _LL1758; _LL1758: _temp1757=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1677)->f2; goto _LL1686;} else{ goto _LL1687;} _LL1687: if(*(( int*)
_temp1677) == Cyc_Absyn_Const_e){ _LL1762: _temp1761=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1677)->f1; goto _LL1688;} else{ goto _LL1689;} _LL1689: if(*(( int*)
_temp1677) == Cyc_Absyn_Var_e){ _LL1766: _temp1765=(( struct Cyc_Absyn_Var_e_struct*)
_temp1677)->f1; goto _LL1764; _LL1764: _temp1763=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1677)->f2; goto _LL1690;} else{ goto _LL1691;} _LL1691: if(*(( int*)
_temp1677) == Cyc_Absyn_Primop_e){ _LL1770: _temp1769=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1677)->f1; goto _LL1768; _LL1768: _temp1767=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1677)->f2; goto _LL1692;} else{ goto _LL1693;} _LL1693: if(*(( int*)
_temp1677) == Cyc_Absyn_Increment_e){ _LL1774: _temp1773=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1677)->f1; goto _LL1772; _LL1772: _temp1771=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1677)->f2; goto _LL1694;} else{ goto _LL1695;} _LL1695: if(*(( int*)
_temp1677) == Cyc_Absyn_AssignOp_e){ _LL1780: _temp1779=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1677)->f1; goto _LL1778; _LL1778: _temp1777=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1677)->f2; goto _LL1776; _LL1776: _temp1775=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1677)->f3; goto _LL1696;} else{ goto _LL1697;} _LL1697: if(*(( int*)
_temp1677) == Cyc_Absyn_Conditional_e){ _LL1786: _temp1785=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1677)->f1; goto _LL1784; _LL1784: _temp1783=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1677)->f2; goto _LL1782; _LL1782: _temp1781=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1677)->f3; goto _LL1698;} else{ goto _LL1699;} _LL1699: if(*(( int*)
_temp1677) == Cyc_Absyn_SeqExp_e){ _LL1790: _temp1789=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1677)->f1; goto _LL1788; _LL1788: _temp1787=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1677)->f2; goto _LL1700;} else{ goto _LL1701;} _LL1701: if(*(( int*)
_temp1677) == Cyc_Absyn_FnCall_e){ _LL1794: _temp1793=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1677)->f1; goto _LL1792; _LL1792: _temp1791=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1677)->f2; goto _LL1702;} else{ goto _LL1703;} _LL1703: if(*(( int*)
_temp1677) == Cyc_Absyn_Throw_e){ _LL1796: _temp1795=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1677)->f1; goto _LL1704;} else{ goto _LL1705;} _LL1705: if(*(( int*)
_temp1677) == Cyc_Absyn_Instantiate_e){ _LL1800: _temp1799=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1677)->f1; goto _LL1798; _LL1798: _temp1797=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1677)->f2; goto _LL1706;} else{ goto _LL1707;} _LL1707: if(*(( int*)
_temp1677) == Cyc_Absyn_Cast_e){ _LL1804: _temp1803=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1677)->f1; goto _LL1802; _LL1802: _temp1801=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1677)->f2; goto _LL1708;} else{ goto _LL1709;} _LL1709: if(*(( int*)
_temp1677) == Cyc_Absyn_Address_e){ _LL1806: _temp1805=(( struct Cyc_Absyn_Address_e_struct*)
_temp1677)->f1; goto _LL1710;} else{ goto _LL1711;} _LL1711: if(*(( int*)
_temp1677) == Cyc_Absyn_New_e){ _LL1810: _temp1809=(( struct Cyc_Absyn_New_e_struct*)
_temp1677)->f1; goto _LL1808; _LL1808: _temp1807=(( struct Cyc_Absyn_New_e_struct*)
_temp1677)->f2; goto _LL1712;} else{ goto _LL1713;} _LL1713: if(*(( int*)
_temp1677) == Cyc_Absyn_Sizeofexp_e){ _LL1812: _temp1811=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1677)->f1; goto _LL1714;} else{ goto _LL1715;} _LL1715: if(*(( int*)
_temp1677) == Cyc_Absyn_Sizeoftyp_e){ _LL1814: _temp1813=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1677)->f1; goto _LL1716;} else{ goto _LL1717;} _LL1717: if(*(( int*)
_temp1677) == Cyc_Absyn_Deref_e){ _LL1816: _temp1815=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1677)->f1; goto _LL1718;} else{ goto _LL1719;} _LL1719: if(*(( int*)
_temp1677) == Cyc_Absyn_StructMember_e){ _LL1820: _temp1819=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1677)->f1; goto _LL1818; _LL1818: _temp1817=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1677)->f2; goto _LL1720;} else{ goto _LL1721;} _LL1721: if(*(( int*)
_temp1677) == Cyc_Absyn_StructArrow_e){ _LL1824: _temp1823=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1677)->f1; goto _LL1822; _LL1822: _temp1821=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1677)->f2; goto _LL1722;} else{ goto _LL1723;} _LL1723: if(*(( int*)
_temp1677) == Cyc_Absyn_Subscript_e){ _LL1828: _temp1827=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1677)->f1; goto _LL1826; _LL1826: _temp1825=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1677)->f2; goto _LL1724;} else{ goto _LL1725;} _LL1725: if(*(( int*)
_temp1677) == Cyc_Absyn_Tuple_e){ _LL1830: _temp1829=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1677)->f1; goto _LL1726;} else{ goto _LL1727;} _LL1727: if(*(( int*)
_temp1677) == Cyc_Absyn_CompoundLit_e){ _LL1834: _temp1833=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1677)->f1; goto _LL1832; _LL1832: _temp1831=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1677)->f2; goto _LL1728;} else{ goto _LL1729;} _LL1729: if(*(( int*)
_temp1677) == Cyc_Absyn_Array_e){ _LL1836: _temp1835=(( struct Cyc_Absyn_Array_e_struct*)
_temp1677)->f1; goto _LL1730;} else{ goto _LL1731;} _LL1731: if(*(( int*)
_temp1677) == Cyc_Absyn_StmtExp_e){ _LL1838: _temp1837=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1677)->f1; goto _LL1732;} else{ goto _LL1733;} _LL1733: if(*(( int*)
_temp1677) == Cyc_Absyn_Codegen_e){ _LL1840: _temp1839=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1677)->f1; goto _LL1734;} else{ goto _LL1735;} _LL1735: if(*(( int*)
_temp1677) == Cyc_Absyn_Fill_e){ _LL1842: _temp1841=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1677)->f1; goto _LL1736;} else{ goto _LL1737;} _LL1737: if(*(( int*)
_temp1677) == Cyc_Absyn_Comprehension_e){ _LL1848: _temp1847=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1677)->f1; goto _LL1846; _LL1846: _temp1845=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1677)->f2; goto _LL1844; _LL1844: _temp1843=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1677)->f3; goto _LL1738;} else{ goto _LL1739;} _LL1739: if(*(( int*)
_temp1677) == Cyc_Absyn_Struct_e){ _LL1858: _temp1857=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1677)->f1; _temp1859=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1677)->f1;
goto _LL1855; _LL1855: _temp1854=(( struct Cyc_Absyn_Struct_e_struct*) _temp1677)->f2;
_temp1856=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1677)->f2; goto _LL1853;
_LL1853: _temp1852=(( struct Cyc_Absyn_Struct_e_struct*) _temp1677)->f3; goto
_LL1850; _LL1850: _temp1849=(( struct Cyc_Absyn_Struct_e_struct*) _temp1677)->f4;
_temp1851=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1677)->f4; goto _LL1740;}
else{ goto _LL1741;} _LL1741: if(*(( int*) _temp1677) == Cyc_Absyn_AnonStruct_e){
_LL1863: _temp1862=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp1677)->f1;
goto _LL1861; _LL1861: _temp1860=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1677)->f2; goto _LL1742;} else{ goto _LL1743;} _LL1743: if(*(( int*)
_temp1677) == Cyc_Absyn_Tunion_e){ _LL1874: _temp1873=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1677)->f1; _temp1875=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1677)->f1;
goto _LL1871; _LL1871: _temp1870=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1677)->f2;
_temp1872=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1677)->f2; goto _LL1869;
_LL1869: _temp1868=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1677)->f3; goto
_LL1867; _LL1867: _temp1866=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1677)->f4;
goto _LL1865; _LL1865: _temp1864=(( struct Cyc_Absyn_Tunion_e_struct*) _temp1677)->f5;
goto _LL1744;} else{ goto _LL1745;} _LL1745: if(*(( int*) _temp1677) == Cyc_Absyn_Enum_e){
_LL1881: _temp1880=(( struct Cyc_Absyn_Enum_e_struct*) _temp1677)->f1; goto
_LL1879; _LL1879: _temp1878=(( struct Cyc_Absyn_Enum_e_struct*) _temp1677)->f2;
goto _LL1877; _LL1877: _temp1876=(( struct Cyc_Absyn_Enum_e_struct*) _temp1677)->f3;
goto _LL1746;} else{ goto _LL1747;} _LL1747: if(*(( int*) _temp1677) == Cyc_Absyn_Malloc_e){
_LL1885: _temp1884=(( struct Cyc_Absyn_Malloc_e_struct*) _temp1677)->f1; goto
_LL1883; _LL1883: _temp1882=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1677)->f2; goto _LL1748;} else{ goto _LL1678;} _LL1680: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1749); return; _LL1682: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1751); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1684: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1755, _temp1753); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1686: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1757); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1688: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1761, e);
goto _LL1678; _LL1690: t= Cyc_Tcexp_tcVar( te, loc, _temp1765, _temp1763); goto
_LL1678; _LL1692: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1769, _temp1767);
goto _LL1678; _LL1694: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1773,
_temp1771); goto _LL1678; _LL1696: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1779, _temp1777, _temp1775); goto _LL1678; _LL1698: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1785, _temp1783, _temp1781); goto _LL1678; _LL1700: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1789, _temp1787); goto _LL1678; _LL1702: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1793, _temp1791); goto _LL1678; _LL1704: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1795); goto _LL1678; _LL1706: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1799, _temp1797); goto _LL1678; _LL1708: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1803, _temp1801); goto _LL1678; _LL1710: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1805); goto _LL1678; _LL1712: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1809, e, _temp1807); goto _LL1678; _LL1714: { void* _temp1886= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1811); goto _LL1887; _LL1887: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1886); goto _LL1678;} _LL1716: t= Cyc_Tcexp_tcSizeof( te, loc, topt,
_temp1813); goto _LL1678; _LL1718: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp1815); goto _LL1678; _LL1720: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp1819, _temp1817); goto _LL1678; _LL1722: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp1823, _temp1821); goto _LL1678; _LL1724: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1827, _temp1825); goto _LL1678; _LL1726: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1829); goto _LL1678; _LL1728: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1833, _temp1831); goto _LL1678; _LL1730: { void** elt_topt= 0;
if( topt != 0){ void* _temp1888= Cyc_Tcutil_compress(*(( void**) _check_null(
topt))); struct Cyc_Absyn_Exp* _temp1894; struct Cyc_Absyn_Tqual _temp1896; void*
_temp1898; void** _temp1900; _LL1890: if(( unsigned int) _temp1888 > 4u?*(( int*)
_temp1888) == Cyc_Absyn_ArrayType: 0){ _LL1899: _temp1898=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1888)->f1; _temp1900=&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1888)->f1); goto _LL1897; _LL1897: _temp1896=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1888)->f2; goto _LL1895; _LL1895: _temp1894=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1888)->f3; goto _LL1891;} else{ goto _LL1892;} _LL1892: goto _LL1893;
_LL1891: elt_topt=( void**) _temp1900; goto _LL1889; _LL1893: goto _LL1889;
_LL1889:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp1835); goto _LL1678;}
_LL1732: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp1837); goto _LL1678;
_LL1734: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp1839); goto _LL1678;
_LL1736: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1841); goto _LL1678; _LL1738:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1847, _temp1845, _temp1843);
goto _LL1678; _LL1740: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp1859,
_temp1856, _temp1852, _temp1851); goto _LL1678; _LL1742: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp1862, _temp1860); goto _LL1678; _LL1744: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp1875, _temp1872, _temp1868, _temp1866, _temp1864); goto
_LL1678; _LL1746: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp1901=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1901[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1902; _temp1902.tag= Cyc_Absyn_EnumType;
_temp1902.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp1878))->name;
_temp1902.f2= _temp1878; _temp1902;}); _temp1901;}); goto _LL1678; _LL1748: t=
Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1884, _temp1882); goto _LL1678; _LL1678:;}
e->topt=({ struct Cyc_Core_Opt* _temp1903=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp1903->v=( void*) t; _temp1903;});}