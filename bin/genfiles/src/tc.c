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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2); extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[ 11u]; extern
int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter(
void(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j=
0; static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
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
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type( void* t); extern
int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct
Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
struct Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string*
v, struct _tagged_string new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} struct _tuple3{ void* f1; void* f2; } ; static
void* Cyc_Tc_scope_redecl_okay( void* s1, struct Cyc_Core_Opt* fields1, void* s2,
struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment* loc, struct
_tagged_string t, struct _tagged_string* v){{ struct _tuple3 _temp5=({ struct
_tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2; _temp4;}); void* _temp35; void*
_temp37; void* _temp39; void* _temp41; void* _temp43; void* _temp45; void*
_temp47; void* _temp49; void* _temp51; void* _temp53; void* _temp55; void*
_temp57; void* _temp59; void* _temp61; void* _temp63; void* _temp65; void*
_temp68; void* _temp70; void* _temp72; void* _temp74; void* _temp76; void*
_temp78; void* _temp80; void* _temp82; void* _temp85; void* _temp87; void*
_temp89; void* _temp91; _LL7: _LL38: _temp37= _temp5.f1; if( _temp37 ==( void*)
Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;} _LL36: _temp35= _temp5.f2;
if( _temp35 ==( void*) Cyc_Absyn_ExternC){ goto _LL8;} else{ goto _LL9;} _LL9:
_LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39= _temp5.f2; if( _temp39 ==(
void*) Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11: _LL46: _temp45=
_temp5.f1; if( _temp45 ==( void*) Cyc_Absyn_Static){ goto _LL44;} else{ goto
_LL13;} _LL44: _temp43= _temp5.f2; if( _temp43 ==( void*) Cyc_Absyn_Static){
goto _LL12;} else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1; goto _LL48;
_LL48: _temp47= _temp5.f2; if( _temp47 ==( void*) Cyc_Absyn_Static){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL54: _temp53= _temp5.f1; if( _temp53 ==( void*) Cyc_Absyn_Static){
goto _LL52;} else{ goto _LL17;} _LL52: _temp51= _temp5.f2; if( _temp51 ==( void*)
Cyc_Absyn_Public){ goto _LL16;} else{ goto _LL17;} _LL17: _LL58: _temp57= _temp5.f1;
if( _temp57 ==( void*) Cyc_Absyn_Abstract){ goto _LL56;} else{ goto _LL19;}
_LL56: _temp55= _temp5.f2; if( _temp55 ==( void*) Cyc_Absyn_Public){ goto _LL18;}
else{ goto _LL19;} _LL19: _LL62: _temp61= _temp5.f1; goto _LL60; _LL60: _temp59=
_temp5.f2; if( _temp59 ==( void*) Cyc_Absyn_Public){ goto _LL20;} else{ goto
_LL21;} _LL21: _LL66: _temp65= _temp5.f1; if( _temp65 ==( void*) Cyc_Absyn_Static){
goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2; if( _temp63 ==( void*)
Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71: _temp70= _temp5.f1;
if( _temp70 ==( void*) Cyc_Absyn_Abstract){ goto _LL69;} else{ goto _LL25;}
_LL69: _temp68= _temp5.f2; if( _temp68 ==( void*) Cyc_Absyn_Extern){ goto _LL67;}
else{ goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;}
_LL25: _LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(
_temp72 ==( void*) Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27:
_LL79: _temp78= _temp5.f1; if( _temp78 ==( void*) Cyc_Absyn_Static){ goto _LL77;}
else{ goto _LL29;} _LL77: _temp76= _temp5.f2; if( _temp76 ==( void*) Cyc_Absyn_Abstract){
goto _LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if( _temp82 ==(
void*) Cyc_Absyn_Public){ goto _LL81;} else{ goto _LL31;} _LL81: _temp80= _temp5.f2;
if( _temp80 ==( void*) Cyc_Absyn_Abstract){ goto _LL30;} else{ goto _LL31;}
_LL31: _LL88: _temp87= _temp5.f1; if( _temp87 ==( void*) Cyc_Absyn_Extern){ goto
_LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2; if( _temp85 ==( void*) Cyc_Absyn_Abstract){
goto _LL84;} else{ goto _LL33;} _LL84: if( fields1 != 0){ goto _LL32;} else{
goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1; goto _LL90; _LL90: _temp89=
_temp5.f2; if( _temp89 ==( void*) Cyc_Absyn_Abstract){ goto _LL34;} else{ goto
_LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1=( void*) Cyc_Absyn_Public; goto _LL6; _LL22:
Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1=( void*) Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct
_tuple5{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tc_field_redecl_okay(
struct Cyc_Absyn_Tunionfield* f1, struct Cyc_Absyn_Tunionfield* f2, struct Cyc_List_List*
inst, struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp131=( struct _tagged_string)({
char* _temp129=( char*)"[x]tunion"; struct _tagged_string _temp130; _temp130.curr=
_temp129; _temp130.base= _temp129; _temp130.last_plus_one= _temp129 + 10;
_temp130;}); struct _tagged_string _temp132=* v; struct _tagged_string _temp133=*(*
f1->name).f2; struct _tagged_string _temp134=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp131.last_plus_one - _temp131.curr, _temp131.curr, _temp132.last_plus_one -
_temp132.curr, _temp132.curr, _temp133.last_plus_one - _temp133.curr, _temp133.curr,
_temp134.last_plus_one - _temp134.curr, _temp134.curr);}));}{ struct Cyc_List_List*
tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp137=( struct
_tagged_string)({ char* _temp135=( char*)"[x]tunion"; struct _tagged_string
_temp136; _temp136.curr= _temp135; _temp136.base= _temp135; _temp136.last_plus_one=
_temp135 + 10; _temp136;}); struct _tagged_string _temp138=* v; struct
_tagged_string _temp139=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp137.last_plus_one - _temp137.curr, _temp137.curr, _temp138.last_plus_one -
_temp138.curr, _temp138.curr, _temp139.last_plus_one - _temp139.curr, _temp139.curr);}));}
for( 0; tvs1 != 0; tvs1=({ struct Cyc_List_List* _temp140= tvs1; if( _temp140 ==
0){ _throw( Null_Exception);} _temp140->tl;}), tvs2=({ struct Cyc_List_List*
_temp141= tvs2; if( _temp141 == 0){ _throw( Null_Exception);} _temp141->tl;})){
inst=({ struct Cyc_List_List* _temp142=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp142->hd=( void*)({ struct _tuple4* _temp143=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp143->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp147= tvs2; if( _temp147 == 0){ _throw( Null_Exception);}
_temp147->hd;}); _temp143->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp144=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp144[ 0]=({ struct Cyc_Absyn_VarType_struct _temp145; _temp145.tag= Cyc_Absyn_VarType;
_temp145.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp146= tvs1;
if( _temp146 == 0){ _throw( Null_Exception);} _temp146->hd;}); _temp145;});
_temp144;}); _temp143;}); _temp142->tl= inst; _temp142;});}{ struct Cyc_List_List*
typs1= f1->typs; struct Cyc_List_List* typs2= f2->typs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( typs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
typs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp150=( struct
_tagged_string)({ char* _temp148=( char*)"[x]tunion"; struct _tagged_string
_temp149; _temp149.curr= _temp148; _temp149.base= _temp148; _temp149.last_plus_one=
_temp148 + 10; _temp149;}); struct _tagged_string _temp151=* v; struct
_tagged_string _temp152=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp150.last_plus_one - _temp150.curr, _temp150.curr, _temp151.last_plus_one -
_temp151.curr, _temp151.curr, _temp152.last_plus_one - _temp152.curr, _temp152.curr);}));}
for( 0; typs1 != 0; typs1=({ struct Cyc_List_List* _temp153= typs1; if( _temp153
== 0){ _throw( Null_Exception);} _temp153->tl;}), typs2=({ struct Cyc_List_List*
_temp154= typs2; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->tl;})){
if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp155= typs1; if( _temp155 == 0){ _throw( Null_Exception);} _temp155->hd;}))).f1,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp156= typs2; if( _temp156 == 0){
_throw( Null_Exception);} _temp156->hd;}))).f1)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp159=( struct _tagged_string)({ char* _temp157=( char*)"[x]tunion";
struct _tagged_string _temp158; _temp158.curr= _temp157; _temp158.base= _temp157;
_temp158.last_plus_one= _temp157 + 10; _temp158;}); struct _tagged_string
_temp160=* v; struct _tagged_string _temp161=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp159.last_plus_one - _temp159.curr, _temp159.curr, _temp160.last_plus_one -
_temp160.curr, _temp160.curr, _temp161.last_plus_one - _temp161.curr, _temp161.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp171= typs2; if( _temp171 == 0){ _throw( Null_Exception);} _temp171->hd;}))).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp162=
typs1; if( _temp162 == 0){ _throw( Null_Exception);} _temp162->hd;}))).f2,
subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp166=( struct
_tagged_string)({ char* _temp163=( char*)"[x]tunion"; struct _tagged_string
_temp164; _temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 10; _temp164;}); struct _tagged_string _temp167=* v; struct
_tagged_string _temp168=*(* f1->name).f2; struct _tagged_string _temp169= Cyc_Absynpp_typ2string((*((
struct _tuple5*)({ struct Cyc_List_List* _temp165= typs1; if( _temp165 == 0){
_throw( Null_Exception);} _temp165->hd;}))).f2); struct _tagged_string _temp170=
Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp166.last_plus_one - _temp166.curr, _temp166.curr, _temp167.last_plus_one -
_temp167.curr, _temp167.curr, _temp168.last_plus_one - _temp168.curr, _temp168.curr,
_temp169.last_plus_one - _temp169.curr, _temp169.curr, _temp170.last_plus_one -
_temp170.curr, _temp170.curr);}));}}}}}} struct _tuple6{ void* f1; int f2; } ;
static int Cyc_Tc_var_redecl_okay( struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct _tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts,
int check_no_fun){ struct Cyc_Core_Opt* ans=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->ordinaries,
name); if( ans == 0){ return 0;} else{ void* sc2; void* t2; struct Cyc_List_List*
atts2;{ void* _temp173=(*(( struct _tuple6*)({ struct Cyc_Core_Opt* _temp172=
ans; if( _temp172 == 0){ _throw( Null_Exception);} _temp172->v;}))).f1; void*
_temp181; struct Cyc_Absyn_Fndecl* _temp183; void* _temp185; struct Cyc_Absyn_Vardecl*
_temp187; _LL175: if(*(( int*) _temp173) == Cyc_Tcenv_VarRes){ _LL182: _temp181=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp173)->f1; if(( unsigned int)
_temp181 > 1u?*(( int*) _temp181) == Cyc_Absyn_Funname_b: 0){ _LL184: _temp183=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*) _temp181)->f1;
goto _LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177: if(*(( int*)
_temp173) == Cyc_Tcenv_VarRes){ _LL186: _temp185=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp173)->f1; if(( unsigned int) _temp185 > 1u?*(( int*) _temp185) == Cyc_Absyn_Global_b:
0){ _LL188: _temp187=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp185)->f1; goto _LL178;} else{ goto _LL179;}} else{ goto _LL179;} _LL179:
goto _LL180; _LL176: if( check_no_fun){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp189=* name; xprintf("redefinition of function %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr);}));} sc2=( void*)
_temp183->sc; t2=( void*)({ struct Cyc_Core_Opt* _temp190= _temp183->cached_typ;
if( _temp190 == 0){ _throw( Null_Exception);} _temp190->v;}); atts2= _temp183->attributes;
goto _LL174; _LL178: sc2=( void*) _temp187->sc; t2=( void*) _temp187->type;
atts2= _temp187->attributes; goto _LL174; _LL180: return 1; _LL174:;} if( sc ==(
void*) Cyc_Absyn_Static? sc2 !=( void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn(
loc,({ struct _tagged_string _temp191=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp191.last_plus_one - _temp191.curr, _temp191.curr);}));} if( sc2 ==( void*)
Cyc_Absyn_Static? sc !=( void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp192=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp192.last_plus_one - _temp192.curr, _temp192.curr);}));} if(( sc2 ==( void*)
Cyc_Absyn_ExternC? sc !=( void*) Cyc_Absyn_ExternC: 0)? 1:( sc2 !=( void*) Cyc_Absyn_ExternC?
sc ==( void*) Cyc_Absyn_ExternC: 0)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp193=* name; xprintf("incompatible redeclaration of %.*s",
_temp193.last_plus_one - _temp193.curr, _temp193.curr);}));} else{ if((( sc !=(
void*) Cyc_Absyn_Extern? sc2 !=( void*) Cyc_Absyn_Extern: 0)? !( sc ==( void*)
Cyc_Absyn_Static? sc2 ==( void*) Cyc_Absyn_Static: 0): 0)? !( sc ==( void*) Cyc_Absyn_Public?
sc2 ==( void*) Cyc_Absyn_Public: 0): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp194=* name; xprintf("incompatible redeclaration of %.*s",
_temp194.last_plus_one - _temp194.curr, _temp194.curr);}));}} if( ! Cyc_Tcutil_unify(
t, t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp195=* name; struct
_tagged_string _temp196= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp197= Cyc_Absynpp_typ2string( t2); xprintf("redeclaration of %.*s at a different type: %.*s != %.*s",
_temp195.last_plus_one - _temp195.curr, _temp195.curr, _temp196.last_plus_one -
_temp196.curr, _temp196.curr, _temp197.last_plus_one - _temp197.curr, _temp197.curr);}));}
if( ! Cyc_Tcutil_same_atts( atts, atts2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp198=* name; xprintf("redeclaration of %.*s with different attributes",
_temp198.last_plus_one - _temp198.curr, _temp198.curr);}));} return 1;}} static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp199= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp205;
struct Cyc_List_List* _temp207; struct Cyc_List_List** _temp209; int _temp210;
struct Cyc_List_List* _temp212; void* _temp214; struct Cyc_Core_Opt* _temp216;
struct Cyc_List_List* _temp218; _LL201: if(( unsigned int) _temp199 > 4u?*(( int*)
_temp199) == Cyc_Absyn_FnType: 0){ _LL206: _temp205=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp199)->f1; _LL219: _temp218=( struct Cyc_List_List*)
_temp205.tvars; goto _LL217; _LL217: _temp216=( struct Cyc_Core_Opt*) _temp205.effect;
goto _LL215; _LL215: _temp214=( void*) _temp205.ret_typ; goto _LL213; _LL213:
_temp212=( struct Cyc_List_List*) _temp205.args; goto _LL211; _LL211: _temp210=(
int) _temp205.varargs; goto _LL208; _LL208: _temp207=( struct Cyc_List_List*)
_temp205.attributes; _temp209=&((( struct Cyc_Absyn_FnType_struct*) _temp199)->f1).attributes;
goto _LL202;} else{ goto _LL203;} _LL203: goto _LL204; _LL202: { struct Cyc_List_List*
res_atts= 0; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp220= atts;
if( _temp220 == 0){ _throw( Null_Exception);} _temp220->tl;})){ void* _temp222=(
void*)({ struct Cyc_List_List* _temp221= atts; if( _temp221 == 0){ _throw(
Null_Exception);} _temp221->hd;}); int _temp236; _LL224: if(( unsigned int)
_temp222 > 15u?*(( int*) _temp222) == Cyc_Absyn_Regparm_att: 0){ _LL237:
_temp236=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp222)->f1; goto
_LL225;} else{ goto _LL226;} _LL226: if( _temp222 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL227;} else{ goto _LL228;} _LL228: if( _temp222 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL229;} else{ goto _LL230;} _LL230: if( _temp222 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL231;} else{ goto _LL232;} _LL232: if( _temp222 ==( void*) Cyc_Absyn_Const_att){
goto _LL233;} else{ goto _LL234;} _LL234: goto _LL235; _LL225: goto _LL227;
_LL227: goto _LL229; _LL229: goto _LL231; _LL231: goto _LL233; _LL233:* _temp209=({
struct Cyc_List_List* _temp238=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp238->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp239= atts; if( _temp239 == 0){ _throw( Null_Exception);} _temp239->hd;}));
_temp238->tl=* _temp209; _temp238;}); continue; _LL235: res_atts=({ struct Cyc_List_List*
_temp240=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp240->hd=( void*)(( void*)({ struct Cyc_List_List* _temp241= atts; if(
_temp241 == 0){ _throw( Null_Exception);} _temp241->hd;})); _temp240->tl=
res_atts; _temp240;}); continue; _LL223:;} return res_atts;} _LL204: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp242=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp243; _temp243.curr= _temp242; _temp243.base= _temp242;
_temp243.last_plus_one= _temp242 + 22; _temp243;})); _LL200:;} static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd){ struct _tagged_string* v=(* vd->name).f2;
void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts=
vd->attributes;{ void* _temp244=(* vd->name).f1; struct Cyc_List_List* _temp252;
struct Cyc_List_List* _temp254; _LL246: if(( unsigned int) _temp244 > 1u?*(( int*)
_temp244) == Cyc_Absyn_Rel_n: 0){ _LL253: _temp252=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp244)->f1; if( _temp252 == 0){ goto _LL247;}
else{ goto _LL248;}} else{ goto _LL248;} _LL248: if(( unsigned int) _temp244 > 1u?*((
int*) _temp244) == Cyc_Absyn_Abs_n: 0){ _LL255: _temp254=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp244)->f1; if( _temp254 == 0){ goto _LL249;}
else{ goto _LL250;}} else{ goto _LL250;} _LL250: goto _LL251; _LL247: goto
_LL245; _LL249: goto _LL245; _LL251: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp256=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 43; _temp257;})); return; _LL245:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp258=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp258[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp259; _temp259.tag= Cyc_Absyn_Abs_n; _temp259.f1= te->ns; _temp259;});
_temp258;});{ void* _temp260= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp267; struct Cyc_Absyn_Tqual _temp269; void* _temp271; _LL262: if((
unsigned int) _temp260 > 4u?*(( int*) _temp260) == Cyc_Absyn_ArrayType: 0){
_LL272: _temp271=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp260)->f1;
goto _LL270; _LL270: _temp269=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp260)->f2; goto _LL268; _LL268: _temp267=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp260)->f3; if( _temp267 == 0){ goto _LL266;}
else{ goto _LL264;}} else{ goto _LL264;} _LL266: if( vd->initializer != 0){ goto
_LL263;} else{ goto _LL264;} _LL264: goto _LL265; _LL263:{ void* _temp274=( void*)({
struct Cyc_Absyn_Exp* _temp273= vd->initializer; if( _temp273 == 0){ _throw(
Null_Exception);} _temp273->r;}); void* _temp284; struct _tagged_string _temp286;
struct Cyc_List_List* _temp288; struct Cyc_Core_Opt* _temp290; struct Cyc_List_List*
_temp292; _LL276: if(*(( int*) _temp274) == Cyc_Absyn_Const_e){ _LL285: _temp284=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp274)->f1; if(( unsigned int)
_temp284 > 1u?*(( int*) _temp284) == Cyc_Absyn_String_c: 0){ _LL287: _temp286=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp284)->f1; goto
_LL277;} else{ goto _LL278;}} else{ goto _LL278;} _LL278: if(*(( int*) _temp274)
== Cyc_Absyn_UnresolvedMem_e){ _LL291: _temp290=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp274)->f1; goto _LL289; _LL289: _temp288=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp274)->f2;
goto _LL279;} else{ goto _LL280;} _LL280: if(*(( int*) _temp274) == Cyc_Absyn_Array_e){
_LL293: _temp292=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp274)->f1; goto _LL281;} else{ goto _LL282;} _LL282: goto _LL283; _LL277: t=(
void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp294=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp294[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp295; _temp295.tag= Cyc_Absyn_ArrayType;
_temp295.f1=( void*) _temp271; _temp295.f2= _temp269; _temp295.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp296= _temp286;( unsigned int)(
_temp296.last_plus_one - _temp296.curr);}), 0); _temp295;}); _temp294;}))); goto
_LL275; _LL279: _temp292= _temp288; goto _LL281; _LL281: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp297=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp297[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp298; _temp298.tag= Cyc_Absyn_ArrayType; _temp298.f1=(
void*) _temp271; _temp298.f2= _temp269; _temp298.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp292), 0);
_temp298;}); _temp297;}))); goto _LL275; _LL283: goto _LL275; _LL275:;} goto
_LL261; _LL265: goto _LL261; _LL261:;} Cyc_Tcutil_check_valid_toplevel_type( loc,
te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts, 0);
if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){ if( vd->initializer
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp299=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 47; _temp300;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp301= atts; if(
_temp301 == 0){ _throw( Null_Exception);} _temp301->tl;})){ void* _temp303=(
void*)({ struct Cyc_List_List* _temp302= atts; if( _temp302 == 0){ _throw(
Null_Exception);} _temp302->hd;}); int _temp323; struct _tagged_string _temp325;
_LL305: if(( unsigned int) _temp303 > 15u?*(( int*) _temp303) == Cyc_Absyn_Aligned_att:
0){ _LL324: _temp323=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp303)->f1;
goto _LL306;} else{ goto _LL307;} _LL307: if(( unsigned int) _temp303 > 15u?*((
int*) _temp303) == Cyc_Absyn_Section_att: 0){ _LL326: _temp325=( struct
_tagged_string)(( struct Cyc_Absyn_Section_att_struct*) _temp303)->f1; goto
_LL308;} else{ goto _LL309;} _LL309: if( _temp303 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL310;} else{ goto _LL311;} _LL311: if( _temp303 ==( void*) Cyc_Absyn_Shared_att){
goto _LL312;} else{ goto _LL313;} _LL313: if( _temp303 ==( void*) Cyc_Absyn_Unused_att){
goto _LL314;} else{ goto _LL315;} _LL315: if( _temp303 ==( void*) Cyc_Absyn_Weak_att){
goto _LL316;} else{ goto _LL317;} _LL317: if( _temp303 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL318;} else{ goto _LL319;} _LL319: if( _temp303 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL320;} else{ goto _LL321;} _LL321: goto _LL322; _LL306: continue; _LL308:
continue; _LL310: continue; _LL312: continue; _LL314: continue; _LL316:
continue; _LL318: continue; _LL320: continue; _LL322: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp328= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp327= atts; if( _temp327 == 0){ _throw( Null_Exception);} _temp327->hd;}));
struct _tagged_string _temp329= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp328.last_plus_one - _temp328.curr, _temp328.curr, _temp329.last_plus_one -
_temp329.curr, _temp329.curr);})); goto _LL304; _LL304:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp330= vd->initializer; if( _temp330 == 0){ _throw( Null_Exception);}
_temp330;});}{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( !
Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp331= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp332= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp333= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp331.last_plus_one - _temp331.curr, _temp331.curr, _temp332.last_plus_one -
_temp332.curr, _temp332.curr, _temp333.last_plus_one - _temp333.curr, _temp333.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp334=( char*)"initializer is not a constant expression";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 41; _temp335;}));}}}} else{ for( 0; atts != 0;
atts=({ struct Cyc_List_List* _temp336= atts; if( _temp336 == 0){ _throw(
Null_Exception);} _temp336->tl;})){ void* _temp338=( void*)({ struct Cyc_List_List*
_temp337= atts; if( _temp337 == 0){ _throw( Null_Exception);} _temp337->hd;});
int _temp356; int _temp358; _LL340: if(( unsigned int) _temp338 > 15u?*(( int*)
_temp338) == Cyc_Absyn_Regparm_att: 0){ _LL357: _temp356=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp338)->f1; goto _LL341;} else{ goto _LL342;} _LL342: if( _temp338 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL343;} else{ goto _LL344;} _LL344: if( _temp338
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL345;} else{ goto _LL346;} _LL346: if(
_temp338 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL347;} else{ goto _LL348;}
_LL348: if( _temp338 ==( void*) Cyc_Absyn_Const_att){ goto _LL349;} else{ goto
_LL350;} _LL350: if(( unsigned int) _temp338 > 15u?*(( int*) _temp338) == Cyc_Absyn_Aligned_att:
0){ _LL359: _temp358=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp338)->f1;
goto _LL351;} else{ goto _LL352;} _LL352: if( _temp338 ==( void*) Cyc_Absyn_Packed_att){
goto _LL353;} else{ goto _LL354;} _LL354: goto _LL355; _LL341: goto _LL343;
_LL343: goto _LL345; _LL345: goto _LL347; _LL347: goto _LL349; _LL349:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp360=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 51; _temp361;})); goto _LL339; _LL351: goto
_LL353; _LL353: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp363= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp362= atts; if( _temp362 == 0){ _throw(
Null_Exception);} _temp362->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp363.last_plus_one - _temp363.curr, _temp363.curr);})); goto _LL339; _LL355:
continue; _LL339:;}}} if( !( is_redecl? sc ==( void*) Cyc_Absyn_Extern? 1: Cyc_Tcutil_is_function_type(
t): 0)){ ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp364=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp364->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp365=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp365[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp366; _temp366.tag= Cyc_Tcenv_VarRes;
_temp366.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp367=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp367[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp368; _temp368.tag= Cyc_Absyn_Global_b;
_temp368.f1= vd; _temp368;}); _temp367;})); _temp366;}); _temp365;}); _temp364->f2=
is_redecl; _temp364;}));}}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp369=(* fd->name).f1; struct Cyc_List_List* _temp377; struct Cyc_List_List*
_temp379; _LL371: if(( unsigned int) _temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Rel_n:
0){ _LL378: _temp377=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp369)->f1; if( _temp377 == 0){ goto _LL372;} else{ goto _LL373;}} else{ goto
_LL373;} _LL373: if(( unsigned int) _temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Abs_n:
0){ _LL380: _temp379=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp369)->f1; goto _LL374;} else{ goto _LL375;} _LL375: goto _LL376; _LL372:
goto _LL370; _LL374:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp381=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp381[ 0]=({ struct Cyc_Core_Impossible_struct _temp382; _temp382.tag= Cyc_Core_Impossible;
_temp382.f1=( struct _tagged_string)({ char* _temp383=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 22; _temp384;}); _temp382;}); _temp381;}));
_LL376: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp385=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 43; _temp386;})); return; _LL370:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp387=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp387[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp388; _temp388.tag= Cyc_Absyn_Abs_n; _temp388.f1= te->ns; _temp388;});
_temp387;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, fd->attributes, 1);{
struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct
Cyc_List_List* _temp389= atts; if( _temp389 == 0){ _throw( Null_Exception);}
_temp389->tl;})){ void* _temp391=( void*)({ struct Cyc_List_List* _temp390= atts;
if( _temp390 == 0){ _throw( Null_Exception);} _temp390->hd;}); int _temp399;
_LL393: if( _temp391 ==( void*) Cyc_Absyn_Packed_att){ goto _LL394;} else{ goto
_LL395;} _LL395: if(( unsigned int) _temp391 > 15u?*(( int*) _temp391) == Cyc_Absyn_Aligned_att:
0){ _LL400: _temp399=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp391)->f1;
goto _LL396;} else{ goto _LL397;} _LL397: goto _LL398; _LL394: goto _LL396;
_LL396: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp402= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp401= atts; if( _temp401 == 0){ _throw(
Null_Exception);} _temp401->hd;})); xprintf("bad attribute %.*s for function",
_temp402.last_plus_one - _temp402.curr, _temp402.curr);})); goto _LL392; _LL398:
goto _LL392; _LL392:;}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp403=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp403->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp404=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp404[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp405; _temp405.tag= Cyc_Tcenv_VarRes;
_temp405.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp406=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp406[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp407; _temp407.tag= Cyc_Absyn_Funname_b;
_temp407.f1= fd; _temp407;}); _temp406;})); _temp405;}); _temp404;}); _temp403->f2=
is_redecl; _temp403;})); te->le=({ struct Cyc_Core_Opt* _temp408=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp408->v=( void*)(( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp409=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp409[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp410; _temp410.tag= Cyc_Tcenv_Outermost; _temp410.f1=(
void*) Cyc_Tcenv_new_fenv( loc, fd); _temp410;}); _temp409;})); _temp408;}); Cyc_Tcstmt_tcStmt(
te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp411=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp412; _temp412.curr= _temp411; _temp412.base= _temp411;
_temp412.last_plus_one= _temp411 + 49; _temp412;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp413=(* td->name).f1; struct Cyc_List_List*
_temp421; struct Cyc_List_List* _temp423; _LL415: if(( unsigned int) _temp413 >
1u?*(( int*) _temp413) == Cyc_Absyn_Rel_n: 0){ _LL422: _temp421=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp413)->f1; if( _temp421 == 0){ goto _LL416;}
else{ goto _LL417;}} else{ goto _LL417;} _LL417: if(( unsigned int) _temp413 > 1u?*((
int*) _temp413) == Cyc_Absyn_Abs_n: 0){ _LL424: _temp423=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp413)->f1; if( _temp423 == 0){ goto _LL418;}
else{ goto _LL419;}} else{ goto _LL419;} _LL419: goto _LL420; _LL416: goto
_LL414; _LL418: goto _LL414; _LL420: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp425=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 43; _temp426;})); return; _LL414:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp427=* v; xprintf("redeclaration of typedef %.*s",
_temp427.last_plus_one - _temp427.curr, _temp427.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp428=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp428[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp429; _temp429.tag= Cyc_Absyn_Abs_n; _temp429.f1= te->ns; _temp429;});
_temp428;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp430=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 47; _temp431;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp672= sd->name;
if( _temp672 == 0){ _throw( Null_Exception);} _temp672->v;}))).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp432=
atts; if( _temp432 == 0){ _throw( Null_Exception);} _temp432->tl;})){ void*
_temp434=( void*)({ struct Cyc_List_List* _temp433= atts; if( _temp433 == 0){
_throw( Null_Exception);} _temp433->hd;}); int _temp442; _LL436: if( _temp434 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL437;} else{ goto _LL438;} _LL438: if((
unsigned int) _temp434 > 15u?*(( int*) _temp434) == Cyc_Absyn_Aligned_att: 0){
_LL443: _temp442=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp434)->f1;
goto _LL439;} else{ goto _LL440;} _LL440: goto _LL441; _LL437: continue; _LL439:
continue; _LL441: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp445= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp444= atts; if( _temp444 == 0){ _throw(
Null_Exception);} _temp444->hd;})); struct _tagged_string _temp446=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp445.last_plus_one - _temp445.curr, _temp445.curr, _temp446.last_plus_one -
_temp446.curr, _temp446.curr);})); goto _LL435; _LL435:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp447= tvs2;
if( _temp447 == 0){ _throw( Null_Exception);} _temp447->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp463= tvs2; if( _temp463 ==
0){ _throw( Null_Exception);} _temp463->hd;}))->kind); void* _temp448=( void*) c->v;
void* _temp456; _LL450: if( _temp448 ==( void*) Cyc_Absyn_No_constr){ goto
_LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp448 > 1u?*(( int*)
_temp448) == Cyc_Absyn_Eq_constr: 0){ _LL457: _temp456=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp448)->f1; if( _temp456 ==( void*) Cyc_Absyn_MemKind){ goto _LL453;} else{
goto _LL454;}} else{ goto _LL454;} _LL454: goto _LL455; _LL451:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp458=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp458[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp459; _temp459.tag= Cyc_Absyn_Eq_constr; _temp459.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp459;}); _temp458;}))); continue; _LL453:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp461=* v; struct
_tagged_string _temp462=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp460= tvs2; if( _temp460 == 0){ _throw( Null_Exception);} _temp460->hd;}))->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp461.last_plus_one - _temp461.curr, _temp461.curr, _temp462.last_plus_one -
_temp462.curr, _temp462.curr);})); continue; _LL455: continue; _LL449:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp465=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp464= sd->name; if( _temp464 == 0){ _throw(
Null_Exception);} _temp464->v;}))).f1; struct Cyc_List_List* _temp473; struct
Cyc_List_List* _temp475; _LL467: if(( unsigned int) _temp465 > 1u?*(( int*)
_temp465) == Cyc_Absyn_Rel_n: 0){ _LL474: _temp473=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp465)->f1; if( _temp473 == 0){ goto _LL468;}
else{ goto _LL469;}} else{ goto _LL469;} _LL469: if(( unsigned int) _temp465 > 1u?*((
int*) _temp465) == Cyc_Absyn_Abs_n: 0){ _LL476: _temp475=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp465)->f1; if( _temp475 == 0){ goto _LL470;}
else{ goto _LL471;}} else{ goto _LL471;} _LL471: goto _LL472; _LL468: goto
_LL466; _LL470: goto _LL466; _LL472: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp477=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 43; _temp478;})); return; _LL466:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp479= sd->name; if( _temp479 == 0){
_throw( Null_Exception);} _temp479->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp480=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp480[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp481; _temp481.tag= Cyc_Absyn_Abs_n;
_temp481.f1= te->ns; _temp481;}); _temp480;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp482=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp488; struct Cyc_Absyn_Structdecl** _temp489; _LL484:
if( _temp482 == 0){ goto _LL485;} else{ goto _LL486;} _LL486: if( _temp482 == 0){
goto _LL483;} else{ _temp488=* _temp482; _LL490: _temp489=( struct Cyc_Absyn_Structdecl**)
_temp488.v; goto _LL487;} _LL485: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp491=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp491[ 0]= sd; _temp491;})); if( sd->fields == 0){ return;} goto _LL483;
_LL487: { struct Cyc_Absyn_Structdecl* sd2=* _temp489; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp492=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp492.last_plus_one - _temp492.curr, _temp492.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp493= x1; if( _temp493 == 0){ _throw( Null_Exception);} _temp493->tl;}), x2=({
struct Cyc_List_List* _temp494= x2; if( _temp494 == 0){ _throw( Null_Exception);}
_temp494->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp503= x1; if( _temp503 == 0){ _throw( Null_Exception);}
_temp503->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp502= x2; if( _temp502 == 0){ _throw( Null_Exception);}
_temp502->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp495=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp495[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp496; _temp496.tag= Cyc_Absyn_Forward_constr; _temp496.f1= c2; _temp496;});
_temp495;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp498=* v; struct _tagged_string _temp499= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp500=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp497= x1; if( _temp497 == 0){ _throw( Null_Exception);} _temp497->hd;}))->name;
struct _tagged_string _temp501= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp498.last_plus_one - _temp498.curr, _temp498.curr, _temp499.last_plus_one -
_temp499.curr, _temp499.curr, _temp500.last_plus_one - _temp500.curr, _temp500.curr,
_temp501.last_plus_one - _temp501.curr, _temp501.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp504=( char*)"struct";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 7; _temp505;}), v));{ struct _tuple7 _temp507=({
struct _tuple7 _temp506; _temp506.f1= sd->fields; _temp506.f2= sd2->fields;
_temp506;}); struct Cyc_Core_Opt* _temp517; struct Cyc_Core_Opt* _temp519;
struct Cyc_Core_Opt* _temp521; struct Cyc_Core_Opt* _temp523; struct Cyc_Core_Opt*
_temp525; struct Cyc_Core_Opt* _temp527; struct Cyc_Core_Opt* _temp529; struct
Cyc_Core_Opt* _temp531; _LL509: _LL520: _temp519= _temp507.f1; if( _temp519 == 0){
goto _LL518;} else{ goto _LL511;} _LL518: _temp517= _temp507.f2; if( _temp517 ==
0){ goto _LL510;} else{ goto _LL511;} _LL511: _LL524: _temp523= _temp507.f1;
goto _LL522; _LL522: _temp521= _temp507.f2; if( _temp521 == 0){ goto _LL512;}
else{ goto _LL513;} _LL513: _LL528: _temp527= _temp507.f1; if( _temp527 == 0){
goto _LL526;} else{ goto _LL515;} _LL526: _temp525= _temp507.f2; goto _LL514;
_LL515: _LL532: _temp531= _temp507.f1; goto _LL530; _LL530: _temp529= _temp507.f2;
goto _LL516; _LL510: return; _LL512:* _temp489= sd; goto _LL508; _LL514: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL516: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp533= tvs0; if( _temp533 == 0){ _throw( Null_Exception);} _temp533->tl;}),
tvs2=({ struct Cyc_List_List* _temp534= tvs2; if( _temp534 == 0){ _throw(
Null_Exception);} _temp534->tl;})){ inst=({ struct Cyc_List_List* _temp535=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp535->hd=(
void*)({ struct _tuple4* _temp536=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp536->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp540= tvs2; if( _temp540 == 0){ _throw( Null_Exception);} _temp540->hd;});
_temp536->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp537=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp537[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp538; _temp538.tag= Cyc_Absyn_VarType; _temp538.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp539= tvs0; if( _temp539 == 0){ _throw( Null_Exception);}
_temp539->hd;}); _temp538;}); _temp537;}); _temp536;}); _temp535->tl= inst;
_temp535;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes, sd2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp541=* v; xprintf("redeclaration of struct %.*s has different attributes",
_temp541.last_plus_one - _temp541.curr, _temp541.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp597= sd->fields; if(
_temp597 == 0){ _throw( Null_Exception);} _temp597->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp596= sd2->fields; if(
_temp596 == 0){ _throw( Null_Exception);} _temp596->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp542= f1s; if( _temp542 == 0){ _throw(
Null_Exception);} _temp542->tl;}), f2s=({ struct Cyc_List_List* _temp543= f2s;
if( _temp543 == 0){ _throw( Null_Exception);} _temp543->tl;})){ struct Cyc_Absyn_Structfield
_temp547; struct Cyc_List_List* _temp548; struct Cyc_Core_Opt* _temp550; void*
_temp552; struct Cyc_Absyn_Tqual _temp554; struct _tagged_string* _temp556;
struct Cyc_Absyn_Structfield* _temp545=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp544= f1s; if( _temp544 == 0){ _throw( Null_Exception);}
_temp544->hd;}); _temp547=* _temp545; _LL557: _temp556=( struct _tagged_string*)
_temp547.name; goto _LL555; _LL555: _temp554=( struct Cyc_Absyn_Tqual) _temp547.tq;
goto _LL553; _LL553: _temp552=( void*) _temp547.type; goto _LL551; _LL551:
_temp550=( struct Cyc_Core_Opt*) _temp547.width; goto _LL549; _LL549: _temp548=(
struct Cyc_List_List*) _temp547.attributes; goto _LL546; _LL546: { struct Cyc_Absyn_Structfield
_temp561; struct Cyc_List_List* _temp562; struct Cyc_Core_Opt* _temp564; void*
_temp566; struct Cyc_Absyn_Tqual _temp568; struct _tagged_string* _temp570;
struct Cyc_Absyn_Structfield* _temp559=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp558= f2s; if( _temp558 == 0){ _throw( Null_Exception);}
_temp558->hd;}); _temp561=* _temp559; _LL571: _temp570=( struct _tagged_string*)
_temp561.name; goto _LL569; _LL569: _temp568=( struct Cyc_Absyn_Tqual) _temp561.tq;
goto _LL567; _LL567: _temp566=( void*) _temp561.type; goto _LL565; _LL565:
_temp564=( struct Cyc_Core_Opt*) _temp561.width; goto _LL563; _LL563: _temp562=(
struct Cyc_List_List*) _temp561.attributes; goto _LL560; _LL560: if( Cyc_String_zstrptrcmp(
_temp556, _temp570) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp572=* v; struct _tagged_string _temp573=* _temp556; struct _tagged_string
_temp574=* _temp570; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp572.last_plus_one - _temp572.curr, _temp572.curr, _temp573.last_plus_one -
_temp573.curr, _temp573.curr, _temp574.last_plus_one - _temp574.curr, _temp574.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp548, _temp562)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp575=* v; struct _tagged_string _temp576=* _temp556; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp575.last_plus_one - _temp575.curr, _temp575.curr, _temp576.last_plus_one -
_temp576.curr, _temp576.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp554,
_temp568)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp577=* v; struct
_tagged_string _temp578=* _temp556; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp577.last_plus_one - _temp577.curr, _temp577.curr, _temp578.last_plus_one -
_temp578.curr, _temp578.curr);}));} if( _temp550 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp579= _temp550; if(
_temp579 == 0){ _throw( Null_Exception);} _temp579->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp580=* v; struct _tagged_string _temp581=*
_temp556; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp580.last_plus_one
- _temp580.curr, _temp580.curr, _temp581.last_plus_one - _temp581.curr, _temp581.curr);}));}
else{ if((( _temp550 != 0? _temp564 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp582= _temp550; if( _temp582
== 0){ _throw( Null_Exception);} _temp582->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp583= _temp564; if( _temp583
== 0){ _throw( Null_Exception);} _temp583->v;})): 0)? 1: _temp550 != _temp564){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp584=* v; struct
_tagged_string _temp585=* _temp556; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp584.last_plus_one - _temp584.curr, _temp584.curr, _temp585.last_plus_one -
_temp585.curr, _temp585.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp552);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp566); if( ! Cyc_Tcutil_unify( _temp552, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp586=* v; struct _tagged_string _temp587=* _temp556;
struct _tagged_string _temp588= Cyc_Absynpp_typ2string( _temp552); struct
_tagged_string _temp589= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp586.last_plus_one - _temp586.curr, _temp586.curr, _temp587.last_plus_one -
_temp587.curr, _temp587.curr, _temp588.last_plus_one - _temp588.curr, _temp588.curr,
_temp589.last_plus_one - _temp589.curr, _temp589.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp591=* v; struct
_tagged_string _temp592=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp590= f2s; if( _temp590 == 0){ _throw( Null_Exception);} _temp590->hd;}))->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp591.last_plus_one
- _temp591.curr, _temp591.curr, _temp592.last_plus_one - _temp592.curr, _temp592.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp594=* v;
struct _tagged_string _temp595=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp593= f1s; if( _temp593 == 0){ _throw( Null_Exception);} _temp593->hd;}))->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp594.last_plus_one
- _temp594.curr, _temp594.curr, _temp595.last_plus_one - _temp595.curr, _temp595.curr);}));}
return;}} _LL508:;} goto _LL483;}} _LL483:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp598=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp598->sc=( void*)(( void*) sd->sc); _temp598->name= sd->name; _temp598->tvs=
tvs; _temp598->fields= 0; _temp598->attributes= 0; _temp598;});{ struct
_RegionHandle _temp599= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp599; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp668= sd->fields; if( _temp668 == 0){ _throw( Null_Exception);} _temp668->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp600= fs; if( _temp600 == 0){
_throw( Null_Exception);} _temp600->tl;})){ struct Cyc_Absyn_Structfield
_temp604; struct Cyc_List_List* _temp605; struct Cyc_Core_Opt* _temp607; void*
_temp609; struct Cyc_Absyn_Tqual _temp611; struct _tagged_string* _temp613;
struct Cyc_Absyn_Structfield* _temp602=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp601= fs; if( _temp601 == 0){ _throw( Null_Exception);}
_temp601->hd;}); _temp604=* _temp602; _LL614: _temp613=( struct _tagged_string*)
_temp604.name; goto _LL612; _LL612: _temp611=( struct Cyc_Absyn_Tqual) _temp604.tq;
goto _LL610; _LL610: _temp609=( void*) _temp604.type; goto _LL608; _LL608:
_temp607=( struct Cyc_Core_Opt*) _temp604.width; goto _LL606; _LL606: _temp605=(
struct Cyc_List_List*) _temp604.attributes; goto _LL603; _LL603: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp613)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp615=* _temp613;
xprintf("duplicate field %.*s in struct", _temp615.last_plus_one - _temp615.curr,
_temp615.curr);}));} if( Cyc_String_strcmp(* _temp613,( struct _tagged_string)({
char* _temp616=( char*)""; struct _tagged_string _temp617; _temp617.curr=
_temp616; _temp617.base= _temp616; _temp617.last_plus_one= _temp616 + 1;
_temp617;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp618=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp618->hd=( void*)
_temp613; _temp618->tl= prev_fields; _temp618;});} Cyc_Tcutil_check_type( loc,
te, tvs,( void*) Cyc_Absyn_MemKind, _temp609); if( _temp607 != 0){ unsigned int
w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp619= _temp607; if( _temp619 == 0){ _throw( Null_Exception);} _temp619->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp620=* _temp613; xprintf("bitfield %.*s does not have constant width",
_temp620.last_plus_one - _temp620.curr, _temp620.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp621= _temp607; if( _temp621
== 0){ _throw( Null_Exception);} _temp621->v;}));}{ void* _temp622= Cyc_Tcutil_compress(
_temp609); void* _temp628; void* _temp630; _LL624: if(( unsigned int) _temp622 >
4u?*(( int*) _temp622) == Cyc_Absyn_IntType: 0){ _LL631: _temp630=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp622)->f1; goto _LL629; _LL629: _temp628=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp622)->f2; goto _LL625;} else{
goto _LL626;} _LL626: goto _LL627; _LL625:{ void* _temp632= _temp628; _LL634:
if( _temp632 ==( void*) Cyc_Absyn_B1){ goto _LL635;} else{ goto _LL636;} _LL636:
if( _temp632 ==( void*) Cyc_Absyn_B2){ goto _LL637;} else{ goto _LL638;} _LL638:
if( _temp632 ==( void*) Cyc_Absyn_B4){ goto _LL639;} else{ goto _LL640;} _LL640:
if( _temp632 ==( void*) Cyc_Absyn_B8){ goto _LL641;} else{ goto _LL633;} _LL635:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp642=(
char*)"bitfield larger than type"; struct _tagged_string _temp643; _temp643.curr=
_temp642; _temp643.base= _temp642; _temp643.last_plus_one= _temp642 + 26;
_temp643;}));} goto _LL633; _LL637: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp644=( char*)"bitfield larger than type"; struct
_tagged_string _temp645; _temp645.curr= _temp644; _temp645.base= _temp644;
_temp645.last_plus_one= _temp644 + 26; _temp645;}));} goto _LL633; _LL639: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp646=( char*)"bitfield larger than type";
struct _tagged_string _temp647; _temp647.curr= _temp646; _temp647.base= _temp646;
_temp647.last_plus_one= _temp646 + 26; _temp647;}));} goto _LL633; _LL641: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp648=( char*)"bitfield larger than type";
struct _tagged_string _temp649; _temp649.curr= _temp648; _temp649.base= _temp648;
_temp649.last_plus_one= _temp648 + 26; _temp649;}));} goto _LL633; _LL633:;}
goto _LL623; _LL627: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp650=*
_temp613; struct _tagged_string _temp651= Cyc_Absynpp_typ2string( _temp609);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp650.last_plus_one
- _temp650.curr, _temp650.curr, _temp651.last_plus_one - _temp651.curr, _temp651.curr);}));
goto _LL623; _LL623:;}} for( 0; _temp605 != 0; _temp605=({ struct Cyc_List_List*
_temp652= _temp605; if( _temp652 == 0){ _throw( Null_Exception);} _temp652->tl;})){
void* _temp654=( void*)({ struct Cyc_List_List* _temp653= _temp605; if( _temp653
== 0){ _throw( Null_Exception);} _temp653->hd;}); int _temp662; _LL656: if(
_temp654 ==( void*) Cyc_Absyn_Packed_att){ goto _LL657;} else{ goto _LL658;}
_LL658: if(( unsigned int) _temp654 > 15u?*(( int*) _temp654) == Cyc_Absyn_Aligned_att:
0){ _LL663: _temp662=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp654)->f1;
goto _LL659;} else{ goto _LL660;} _LL660: goto _LL661; _LL657: continue; _LL659:
continue; _LL661: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp665= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp664= _temp605; if( _temp664 == 0){ _throw(
Null_Exception);} _temp664->hd;})); struct _tagged_string _temp666=* v; struct
_tagged_string _temp667=* _temp613; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp665.last_plus_one - _temp665.curr, _temp665.curr, _temp666.last_plus_one -
_temp666.curr, _temp666.curr, _temp667.last_plus_one - _temp667.curr, _temp667.curr);}));
goto _LL655; _LL655:;}}}; _pop_region(& _temp599);} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple6* _temp669=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp669->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp670=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp670[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp671; _temp671.tag= Cyc_Tcenv_StructRes;
_temp671.f1= sd; _temp671;}); _temp670;}); _temp669->f2= 1; _temp669;}));* sdp=
sd;}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp673=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp674; _temp674.curr= _temp673; _temp674.base= _temp673;
_temp674.last_plus_one= _temp673 + 46; _temp674;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp912= ud->name;
if( _temp912 == 0){ _throw( Null_Exception);} _temp912->v;}))).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp675=
atts; if( _temp675 == 0){ _throw( Null_Exception);} _temp675->tl;})){ void*
_temp677=( void*)({ struct Cyc_List_List* _temp676= atts; if( _temp676 == 0){
_throw( Null_Exception);} _temp676->hd;}); int _temp685; _LL679: if( _temp677 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL680;} else{ goto _LL681;} _LL681: if((
unsigned int) _temp677 > 15u?*(( int*) _temp677) == Cyc_Absyn_Aligned_att: 0){
_LL686: _temp685=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp677)->f1;
goto _LL682;} else{ goto _LL683;} _LL683: goto _LL684; _LL680: continue; _LL682:
continue; _LL684: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp688= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp687= atts; if( _temp687 == 0){ _throw(
Null_Exception);} _temp687->hd;})); struct _tagged_string _temp689=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp688.last_plus_one - _temp688.curr, _temp688.curr, _temp689.last_plus_one -
_temp689.curr, _temp689.curr);})); goto _LL678; _LL678:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp690= tvs2;
if( _temp690 == 0){ _throw( Null_Exception);} _temp690->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp706= tvs2; if( _temp706 ==
0){ _throw( Null_Exception);} _temp706->hd;}))->kind); void* _temp691=( void*) c->v;
void* _temp699; _LL693: if( _temp691 ==( void*) Cyc_Absyn_No_constr){ goto
_LL694;} else{ goto _LL695;} _LL695: if(( unsigned int) _temp691 > 1u?*(( int*)
_temp691) == Cyc_Absyn_Eq_constr: 0){ _LL700: _temp699=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp691)->f1; if( _temp699 ==( void*) Cyc_Absyn_MemKind){ goto _LL696;} else{
goto _LL697;}} else{ goto _LL697;} _LL697: goto _LL698; _LL694:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp701=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp701[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp702; _temp702.tag= Cyc_Absyn_Eq_constr; _temp702.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp702;}); _temp701;}))); continue; _LL696:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp704=* v; struct
_tagged_string _temp705=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp703= tvs2; if( _temp703 == 0){ _throw( Null_Exception);} _temp703->hd;}))->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp704.last_plus_one
- _temp704.curr, _temp704.curr, _temp705.last_plus_one - _temp705.curr, _temp705.curr);}));
continue; _LL698: continue; _LL692:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp708=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp707= ud->name;
if( _temp707 == 0){ _throw( Null_Exception);} _temp707->v;}))).f1; struct Cyc_List_List*
_temp716; struct Cyc_List_List* _temp718; _LL710: if(( unsigned int) _temp708 >
1u?*(( int*) _temp708) == Cyc_Absyn_Rel_n: 0){ _LL717: _temp716=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp708)->f1; if( _temp716 == 0){ goto _LL711;}
else{ goto _LL712;}} else{ goto _LL712;} _LL712: if(( unsigned int) _temp708 > 1u?*((
int*) _temp708) == Cyc_Absyn_Abs_n: 0){ _LL719: _temp718=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp708)->f1; if( _temp718 == 0){ goto _LL713;}
else{ goto _LL714;}} else{ goto _LL714;} _LL714: goto _LL715; _LL711: goto
_LL709; _LL713: goto _LL709; _LL715: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp720=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 43; _temp721;})); return; _LL709:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp722= ud->name; if( _temp722 == 0){
_throw( Null_Exception);} _temp722->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp723=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp723[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp724; _temp724.tag= Cyc_Absyn_Abs_n;
_temp724.f1= te->ns; _temp724;}); _temp723;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp725=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp731; struct Cyc_Absyn_Uniondecl** _temp732; _LL727: if(
_temp725 == 0){ goto _LL728;} else{ goto _LL729;} _LL729: if( _temp725 == 0){
goto _LL726;} else{ _temp731=* _temp725; _LL733: _temp732=( struct Cyc_Absyn_Uniondecl**)
_temp731.v; goto _LL730;} _LL728: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp734=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp734[ 0]= ud; _temp734;})); if( ud->fields == 0){ return;} goto _LL726;
_LL730: { struct Cyc_Absyn_Uniondecl* ud2=* _temp732; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp735=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp735.last_plus_one - _temp735.curr, _temp735.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp736= x1; if( _temp736 == 0){ _throw( Null_Exception);} _temp736->tl;}), x2=({
struct Cyc_List_List* _temp737= x2; if( _temp737 == 0){ _throw( Null_Exception);}
_temp737->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp746= x1; if( _temp746 == 0){ _throw( Null_Exception);}
_temp746->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp745= x2; if( _temp745 == 0){ _throw( Null_Exception);}
_temp745->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp738=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp738[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp739; _temp739.tag= Cyc_Absyn_Forward_constr; _temp739.f1= c2; _temp739;});
_temp738;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp741=* v; struct _tagged_string _temp742= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp743=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp740= x1; if( _temp740 == 0){ _throw( Null_Exception);} _temp740->hd;}))->name;
struct _tagged_string _temp744= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp741.last_plus_one - _temp741.curr, _temp741.curr, _temp742.last_plus_one -
_temp742.curr, _temp742.curr, _temp743.last_plus_one - _temp743.curr, _temp743.curr,
_temp744.last_plus_one - _temp744.curr, _temp744.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp747=( char*)"union";
struct _tagged_string _temp748; _temp748.curr= _temp747; _temp748.base= _temp747;
_temp748.last_plus_one= _temp747 + 6; _temp748;}), v));{ struct _tuple7 _temp750=({
struct _tuple7 _temp749; _temp749.f1= ud->fields; _temp749.f2= ud2->fields;
_temp749;}); struct Cyc_Core_Opt* _temp760; struct Cyc_Core_Opt* _temp762;
struct Cyc_Core_Opt* _temp764; struct Cyc_Core_Opt* _temp766; struct Cyc_Core_Opt*
_temp768; struct Cyc_Core_Opt* _temp770; struct Cyc_Core_Opt* _temp772; struct
Cyc_Core_Opt* _temp774; _LL752: _LL763: _temp762= _temp750.f1; if( _temp762 == 0){
goto _LL761;} else{ goto _LL754;} _LL761: _temp760= _temp750.f2; if( _temp760 ==
0){ goto _LL753;} else{ goto _LL754;} _LL754: _LL767: _temp766= _temp750.f1;
goto _LL765; _LL765: _temp764= _temp750.f2; if( _temp764 == 0){ goto _LL755;}
else{ goto _LL756;} _LL756: _LL771: _temp770= _temp750.f1; if( _temp770 == 0){
goto _LL769;} else{ goto _LL758;} _LL769: _temp768= _temp750.f2; goto _LL757;
_LL758: _LL775: _temp774= _temp750.f1; goto _LL773; _LL773: _temp772= _temp750.f2;
goto _LL759; _LL753: return; _LL755:* _temp732= ud; goto _LL751; _LL757: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL759: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp776= tvs0; if( _temp776 == 0){ _throw( Null_Exception);} _temp776->tl;}),
tvs2=({ struct Cyc_List_List* _temp777= tvs2; if( _temp777 == 0){ _throw(
Null_Exception);} _temp777->tl;})){ inst=({ struct Cyc_List_List* _temp778=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp778->hd=(
void*)({ struct _tuple4* _temp779=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp779->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp783= tvs2; if( _temp783 == 0){ _throw( Null_Exception);} _temp783->hd;});
_temp779->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp780=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp780[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp781; _temp781.tag= Cyc_Absyn_VarType; _temp781.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp782= tvs0; if( _temp782 == 0){ _throw( Null_Exception);}
_temp782->hd;}); _temp781;}); _temp780;}); _temp779;}); _temp778->tl= inst;
_temp778;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes, ud2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp784=* v; xprintf("redeclaration of union %.*s has different attributes",
_temp784.last_plus_one - _temp784.curr, _temp784.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp840= ud->fields; if(
_temp840 == 0){ _throw( Null_Exception);} _temp840->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp839= ud2->fields; if(
_temp839 == 0){ _throw( Null_Exception);} _temp839->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp785= f1s; if( _temp785 == 0){ _throw(
Null_Exception);} _temp785->tl;}), f2s=({ struct Cyc_List_List* _temp786= f2s;
if( _temp786 == 0){ _throw( Null_Exception);} _temp786->tl;})){ struct Cyc_Absyn_Structfield
_temp790; struct Cyc_List_List* _temp791; struct Cyc_Core_Opt* _temp793; void*
_temp795; struct Cyc_Absyn_Tqual _temp797; struct _tagged_string* _temp799;
struct Cyc_Absyn_Structfield* _temp788=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp787= f1s; if( _temp787 == 0){ _throw( Null_Exception);}
_temp787->hd;}); _temp790=* _temp788; _LL800: _temp799=( struct _tagged_string*)
_temp790.name; goto _LL798; _LL798: _temp797=( struct Cyc_Absyn_Tqual) _temp790.tq;
goto _LL796; _LL796: _temp795=( void*) _temp790.type; goto _LL794; _LL794:
_temp793=( struct Cyc_Core_Opt*) _temp790.width; goto _LL792; _LL792: _temp791=(
struct Cyc_List_List*) _temp790.attributes; goto _LL789; _LL789: { struct Cyc_Absyn_Structfield
_temp804; struct Cyc_List_List* _temp805; struct Cyc_Core_Opt* _temp807; void*
_temp809; struct Cyc_Absyn_Tqual _temp811; struct _tagged_string* _temp813;
struct Cyc_Absyn_Structfield* _temp802=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp801= f2s; if( _temp801 == 0){ _throw( Null_Exception);}
_temp801->hd;}); _temp804=* _temp802; _LL814: _temp813=( struct _tagged_string*)
_temp804.name; goto _LL812; _LL812: _temp811=( struct Cyc_Absyn_Tqual) _temp804.tq;
goto _LL810; _LL810: _temp809=( void*) _temp804.type; goto _LL808; _LL808:
_temp807=( struct Cyc_Core_Opt*) _temp804.width; goto _LL806; _LL806: _temp805=(
struct Cyc_List_List*) _temp804.attributes; goto _LL803; _LL803: if( Cyc_String_zstrptrcmp(
_temp799, _temp813) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp815=* v; struct _tagged_string _temp816=* _temp799; struct _tagged_string
_temp817=* _temp813; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp815.last_plus_one - _temp815.curr, _temp815.curr, _temp816.last_plus_one -
_temp816.curr, _temp816.curr, _temp817.last_plus_one - _temp817.curr, _temp817.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp791, _temp805)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp818=* v; struct _tagged_string _temp819=* _temp799; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp818.last_plus_one - _temp818.curr, _temp818.curr, _temp819.last_plus_one -
_temp819.curr, _temp819.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp797,
_temp811)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp820=* v; struct
_tagged_string _temp821=* _temp799; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp820.last_plus_one - _temp820.curr, _temp820.curr, _temp821.last_plus_one -
_temp821.curr, _temp821.curr);}));} if( _temp793 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp822= _temp793; if(
_temp822 == 0){ _throw( Null_Exception);} _temp822->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp823=* v; struct _tagged_string _temp824=*
_temp799; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp823.last_plus_one
- _temp823.curr, _temp823.curr, _temp824.last_plus_one - _temp824.curr, _temp824.curr);}));}
else{ if((( _temp793 != 0? _temp807 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp825= _temp793; if( _temp825
== 0){ _throw( Null_Exception);} _temp825->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp826= _temp807; if( _temp826
== 0){ _throw( Null_Exception);} _temp826->v;})): 0)? 1: _temp793 != _temp807){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp827=* v; struct
_tagged_string _temp828=* _temp799; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp827.last_plus_one - _temp827.curr, _temp827.curr, _temp828.last_plus_one -
_temp828.curr, _temp828.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp795);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp809); if( ! Cyc_Tcutil_unify( _temp795, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp829=* v; struct _tagged_string _temp830=* _temp799;
struct _tagged_string _temp831= Cyc_Absynpp_typ2string( _temp795); struct
_tagged_string _temp832= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp829.last_plus_one - _temp829.curr, _temp829.curr, _temp830.last_plus_one -
_temp830.curr, _temp830.curr, _temp831.last_plus_one - _temp831.curr, _temp831.curr,
_temp832.last_plus_one - _temp832.curr, _temp832.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp834=* v; struct
_tagged_string _temp835=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp833= f2s; if( _temp833 == 0){ _throw( Null_Exception);} _temp833->hd;}))->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp834.last_plus_one
- _temp834.curr, _temp834.curr, _temp835.last_plus_one - _temp835.curr, _temp835.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp837=* v;
struct _tagged_string _temp838=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp836= f1s; if( _temp836 == 0){ _throw( Null_Exception);} _temp836->hd;}))->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp837.last_plus_one
- _temp837.curr, _temp837.curr, _temp838.last_plus_one - _temp838.curr, _temp838.curr);}));}
return;}} _LL751:;} goto _LL726;}} _LL726:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp841=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp841->sc=( void*)(( void*) ud->sc); _temp841->name= ud->name; _temp841->tvs=
tvs; _temp841->fields= 0; _temp841->attributes= 0; _temp841;});{ struct
_RegionHandle _temp842= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp842; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp911= ud->fields; if( _temp911 == 0){ _throw( Null_Exception);} _temp911->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp843= fs; if( _temp843 == 0){
_throw( Null_Exception);} _temp843->tl;})){ struct Cyc_Absyn_Structfield
_temp847; struct Cyc_List_List* _temp848; struct Cyc_Core_Opt* _temp850; void*
_temp852; struct Cyc_Absyn_Tqual _temp854; struct _tagged_string* _temp856;
struct Cyc_Absyn_Structfield* _temp845=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp844= fs; if( _temp844 == 0){ _throw( Null_Exception);}
_temp844->hd;}); _temp847=* _temp845; _LL857: _temp856=( struct _tagged_string*)
_temp847.name; goto _LL855; _LL855: _temp854=( struct Cyc_Absyn_Tqual) _temp847.tq;
goto _LL853; _LL853: _temp852=( void*) _temp847.type; goto _LL851; _LL851:
_temp850=( struct Cyc_Core_Opt*) _temp847.width; goto _LL849; _LL849: _temp848=(
struct Cyc_List_List*) _temp847.attributes; goto _LL846; _LL846: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp856)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp858=* _temp856;
xprintf("duplicate field %.*s in union", _temp858.last_plus_one - _temp858.curr,
_temp858.curr);}));} prev_fields=({ struct Cyc_List_List* _temp859=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp859->hd=( void*)
_temp856; _temp859->tl= prev_fields; _temp859;}); Cyc_Tcutil_check_type( loc, te,
tvs,( void*) Cyc_Absyn_MemKind, _temp852); if( _temp850 != 0){ unsigned int w= 0;
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp860= _temp850; if( _temp860 == 0){ _throw( Null_Exception);} _temp860->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp861=* _temp856; xprintf("bitfield %.*s does not have constant width",
_temp861.last_plus_one - _temp861.curr, _temp861.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp862= _temp850; if( _temp862
== 0){ _throw( Null_Exception);} _temp862->v;}));}{ void* _temp863= Cyc_Tcutil_compress(
_temp852); void* _temp869; void* _temp871; _LL865: if(( unsigned int) _temp863 >
4u?*(( int*) _temp863) == Cyc_Absyn_IntType: 0){ _LL872: _temp871=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp863)->f1; goto _LL870; _LL870: _temp869=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp863)->f2; goto _LL866;} else{
goto _LL867;} _LL867: goto _LL868; _LL866:{ void* _temp873= _temp869; _LL875:
if( _temp873 ==( void*) Cyc_Absyn_B1){ goto _LL876;} else{ goto _LL877;} _LL877:
if( _temp873 ==( void*) Cyc_Absyn_B2){ goto _LL878;} else{ goto _LL879;} _LL879:
if( _temp873 ==( void*) Cyc_Absyn_B4){ goto _LL880;} else{ goto _LL881;} _LL881:
if( _temp873 ==( void*) Cyc_Absyn_B8){ goto _LL882;} else{ goto _LL874;} _LL876:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp883=(
char*)"bitfield larger than type"; struct _tagged_string _temp884; _temp884.curr=
_temp883; _temp884.base= _temp883; _temp884.last_plus_one= _temp883 + 26;
_temp884;}));} goto _LL874; _LL878: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp885=( char*)"bitfield larger than type"; struct
_tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 26; _temp886;}));} goto _LL874; _LL880: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp887=( char*)"bitfield larger than type";
struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 26; _temp888;}));} goto _LL874; _LL882: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp889=( char*)"bitfield larger than type";
struct _tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 26; _temp890;}));} goto _LL874; _LL874:;}
goto _LL864; _LL868: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp891=*
_temp856; struct _tagged_string _temp892= Cyc_Absynpp_typ2string( _temp852);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp891.last_plus_one
- _temp891.curr, _temp891.curr, _temp892.last_plus_one - _temp892.curr, _temp892.curr);}));
goto _LL864; _LL864:;}} if( ! Cyc_Tcutil_bits_only( _temp852)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp893=* _temp856; struct _tagged_string _temp894=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp893.last_plus_one - _temp893.curr, _temp893.curr, _temp894.last_plus_one -
_temp894.curr, _temp894.curr);}));} for( 0; _temp848 != 0; _temp848=({ struct
Cyc_List_List* _temp895= _temp848; if( _temp895 == 0){ _throw( Null_Exception);}
_temp895->tl;})){ void* _temp897=( void*)({ struct Cyc_List_List* _temp896=
_temp848; if( _temp896 == 0){ _throw( Null_Exception);} _temp896->hd;}); int
_temp905; _LL899: if( _temp897 ==( void*) Cyc_Absyn_Packed_att){ goto _LL900;}
else{ goto _LL901;} _LL901: if(( unsigned int) _temp897 > 15u?*(( int*) _temp897)
== Cyc_Absyn_Aligned_att: 0){ _LL906: _temp905=( int)(( struct Cyc_Absyn_Aligned_att_struct*)
_temp897)->f1; goto _LL902;} else{ goto _LL903;} _LL903: goto _LL904; _LL900:
continue; _LL902: continue; _LL904: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp908= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp907= _temp848; if( _temp907 == 0){ _throw( Null_Exception);} _temp907->hd;}));
struct _tagged_string _temp909=* v; struct _tagged_string _temp910=* _temp856;
xprintf("bad attribute %.*s in union %.*s, member %.*s", _temp908.last_plus_one
- _temp908.curr, _temp908.curr, _temp909.last_plus_one - _temp909.curr, _temp909.curr,
_temp910.last_plus_one - _temp910.curr, _temp910.curr);})); goto _LL898; _LL898:;}}};
_pop_region(& _temp842);}* udp= ud;}}}} static void Cyc_Tc_tcTuniondecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Tuniondecl* tud){ struct _tagged_string* v=(* tud->name).f2;
struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0;
tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp913= tvs2; if( _temp913 == 0){
_throw( Null_Exception);} _temp913->tl;})){ struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp929= tvs2; if( _temp929 ==
0){ _throw( Null_Exception);} _temp929->hd;}))->kind); void* _temp914=( void*) c->v;
void* _temp922; _LL916: if( _temp914 ==( void*) Cyc_Absyn_No_constr){ goto
_LL917;} else{ goto _LL918;} _LL918: if(( unsigned int) _temp914 > 1u?*(( int*)
_temp914) == Cyc_Absyn_Eq_constr: 0){ _LL923: _temp922=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp914)->f1; if( _temp922 ==( void*) Cyc_Absyn_MemKind){ goto _LL919;} else{
goto _LL920;}} else{ goto _LL920;} _LL920: goto _LL921; _LL917:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp924=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp924[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp925; _temp925.tag= Cyc_Absyn_Eq_constr; _temp925.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp925;}); _temp924;}))); goto _LL915;
_LL919: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp927=* v; struct
_tagged_string _temp928=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp926= tvs2; if( _temp926 == 0){ _throw( Null_Exception);} _temp926->hd;}))->name;
xprintf("[x]tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp927.last_plus_one - _temp927.curr, _temp927.curr, _temp928.last_plus_one -
_temp928.curr, _temp928.curr);})); goto _LL915; _LL921: goto _LL915; _LL915:;}}{
struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp930; _push_handler(& _temp930);{ void* _temp931=( void*) setjmp( _temp930.handler);
if( ! _temp931){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);;
_pop_handler();} else{ void* _temp933= _temp931; _LL935: if( _temp933 == Cyc_Dict_Absent){
goto _LL936;} else{ goto _LL937;} _LL937: goto _LL938; _LL936: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp939= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp939.last_plus_one - _temp939.curr, _temp939.curr);})); return; _LL938:(
void) _throw( _temp933); _LL934:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp940= tud_opt; if( _temp940
== 0){ _throw( Null_Exception);} _temp940->v;})))->name;}} else{{ void* _temp941=(*
tud->name).f1; struct Cyc_List_List* _temp949; struct Cyc_List_List* _temp951;
_LL943: if(( unsigned int) _temp941 > 1u?*(( int*) _temp941) == Cyc_Absyn_Rel_n:
0){ _LL950: _temp949=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp941)->f1; if( _temp949 == 0){ goto _LL944;} else{ goto _LL945;}} else{ goto
_LL945;} _LL945: if(( unsigned int) _temp941 > 1u?*(( int*) _temp941) == Cyc_Absyn_Abs_n:
0){ _LL952: _temp951=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp941)->f1; goto _LL946;} else{ goto _LL947;} _LL947: goto _LL948; _LL944:(*
tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp953=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp953[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp954; _temp954.tag= Cyc_Absyn_Abs_n; _temp954.f1= te->ns; _temp954;});
_temp953;}); goto _LL942; _LL946: goto _LL948; _LL948: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp955=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp956; _temp956.curr= _temp955; _temp956.base= _temp955;
_temp956.last_plus_one= _temp955 + 43; _temp956;})); return; _LL942:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_List_List* prev_fields;{ struct Cyc_Core_Opt* _temp957=
tud_opt; struct Cyc_Core_Opt _temp963; struct Cyc_Absyn_Tuniondecl** _temp964;
_LL959: if( _temp957 == 0){ goto _LL960;} else{ goto _LL961;} _LL961: if(
_temp957 == 0){ goto _LL958;} else{ _temp963=* _temp957; _LL965: _temp964=(
struct Cyc_Absyn_Tuniondecl**) _temp963.v; goto _LL962;} _LL960: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp966=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp966[ 0]= tud; _temp966;}));
if( tud->fields == 0){ return;} prev_fields= 0; goto _LL958; _LL962: { struct
Cyc_Absyn_Tuniondecl* tud2=* _temp964; struct Cyc_List_List* tvs2= tud2->tvs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp967=* v; xprintf("redeclaration of [x]tunion %.*s has a different number of type parameters",
_temp967.last_plus_one - _temp967.curr, _temp967.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp968= x1; if( _temp968 == 0){ _throw( Null_Exception);} _temp968->tl;}), x2=({
struct Cyc_List_List* _temp969= x2; if( _temp969 == 0){ _throw( Null_Exception);}
_temp969->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp978= x1; if( _temp978 == 0){ _throw( Null_Exception);}
_temp978->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp977= x2; if( _temp977 == 0){ _throw( Null_Exception);}
_temp977->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp970=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp970[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp971; _temp971.tag= Cyc_Absyn_Forward_constr; _temp971.f1= c2; _temp971;});
_temp970;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp973=* v; struct _tagged_string _temp974= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp975=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp972= x1; if( _temp972 == 0){ _throw( Null_Exception);} _temp972->hd;}))->name;
struct _tagged_string _temp976= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of [x]tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp973.last_plus_one - _temp973.curr, _temp973.curr, _temp974.last_plus_one -
_temp974.curr, _temp974.curr, _temp975.last_plus_one - _temp975.curr, _temp975.curr,
_temp976.last_plus_one - _temp976.curr, _temp976.curr);}));}}( void*)( tud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) tud->sc, tud->fields,(
void*) tud2->sc, tud2->fields, loc,( struct _tagged_string)({ char* _temp979=(
char*)"[x]tunion"; struct _tagged_string _temp980; _temp980.curr= _temp979;
_temp980.base= _temp979; _temp980.last_plus_one= _temp979 + 10; _temp980;}), v));
if( tud->is_xtunion != tud2->is_xtunion){ Cyc_Tcutil_terr( loc, xprintf("redeclaration of tunion as xtunion or vice-versa"));}{
struct _tuple7 _temp982=({ struct _tuple7 _temp981; _temp981.f1= tud->fields;
_temp981.f2= tud2->fields; _temp981;}); struct Cyc_Core_Opt* _temp992; struct
Cyc_Core_Opt* _temp994; struct Cyc_Core_Opt* _temp996; struct Cyc_Core_Opt*
_temp998; struct Cyc_Core_Opt* _temp1000; struct Cyc_Core_Opt* _temp1002; struct
Cyc_Core_Opt* _temp1004; struct Cyc_Core_Opt* _temp1006; _LL984: _LL995:
_temp994= _temp982.f1; if( _temp994 == 0){ goto _LL993;} else{ goto _LL986;}
_LL993: _temp992= _temp982.f2; if( _temp992 == 0){ goto _LL985;} else{ goto
_LL986;} _LL986: _LL999: _temp998= _temp982.f1; if( _temp998 == 0){ goto _LL997;}
else{ goto _LL988;} _LL997: _temp996= _temp982.f2; goto _LL987; _LL988: _LL1003:
_temp1002= _temp982.f1; goto _LL1001; _LL1001: _temp1000= _temp982.f2; if(
_temp1000 == 0){ goto _LL989;} else{ goto _LL990;} _LL990: _LL1007: _temp1006=
_temp982.f1; goto _LL1005; _LL1005: _temp1004= _temp982.f2; goto _LL991; _LL985:
return; _LL987: tud->fields= tud2->fields; tud->tvs= tvs2; return; _LL989:
prev_fields= 0;* _temp964= tud; goto _LL983; _LL991: { struct Cyc_List_List*
inst= 0;{ struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp1008= tvs0; if( _temp1008 == 0){ _throw( Null_Exception);} _temp1008->tl;}),
tvs2=({ struct Cyc_List_List* _temp1009= tvs2; if( _temp1009 == 0){ _throw(
Null_Exception);} _temp1009->tl;})){ inst=({ struct Cyc_List_List* _temp1010=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1010->hd=(
void*)({ struct _tuple4* _temp1011=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1011->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1015= tvs2; if( _temp1015 == 0){ _throw( Null_Exception);} _temp1015->hd;});
_temp1011->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp1012=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1012[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1013; _temp1013.tag= Cyc_Absyn_VarType; _temp1013.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1014= tvs0; if( _temp1014
== 0){ _throw( Null_Exception);} _temp1014->hd;}); _temp1013;}); _temp1012;});
_temp1011;}); _temp1010->tl= inst; _temp1010;});}}{ struct Cyc_List_List* f1s=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1055= tud->fields; if(
_temp1055 == 0){ _throw( Null_Exception);} _temp1055->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1054= tud2->fields; if(
_temp1054 == 0){ _throw( Null_Exception);} _temp1054->v;});{ struct Cyc_List_List*
fs= f1s; for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1016= fs; if(
_temp1016 == 0){ _throw( Null_Exception);} _temp1016->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1043= fs; if(
_temp1043 == 0){ _throw( Null_Exception);} _temp1043->hd;}); struct Cyc_List_List*
alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars( f->loc,
alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({ struct
Cyc_List_List* _temp1017= typs; if( _temp1017 == 0){ _throw( Null_Exception);}
_temp1017->tl;})){ Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp1018= typs; if( _temp1018 == 0){
_throw( Null_Exception);} _temp1018->hd;}))).f2);}}{ void* _temp1019=(* f->name).f1;
struct Cyc_List_List* _temp1029; struct Cyc_List_List* _temp1031; struct Cyc_List_List*
_temp1033; _LL1021: if(( unsigned int) _temp1019 > 1u?*(( int*) _temp1019) ==
Cyc_Absyn_Rel_n: 0){ _LL1030: _temp1029=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1019)->f1; if( _temp1029 == 0){ goto _LL1022;} else{ goto _LL1023;}} else{
goto _LL1023;} _LL1023: if(( unsigned int) _temp1019 > 1u?*(( int*) _temp1019)
== Cyc_Absyn_Rel_n: 0){ _LL1032: _temp1031=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1019)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp1019 > 1u?*(( int*) _temp1019) == Cyc_Absyn_Abs_n: 0){ _LL1034: _temp1033=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1019)->f1; goto
_LL1026;} else{ goto _LL1027;} _LL1027: if( _temp1019 ==( void*) Cyc_Absyn_Loc_n){
goto _LL1028;} else{ goto _LL1020;} _LL1022: if( tud->is_xtunion){(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1035=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1035[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1036; _temp1036.tag= Cyc_Absyn_Abs_n; _temp1036.f1= te->ns; _temp1036;});
_temp1035;});} else{(* f->name).f1=(* tud->name).f1;} goto _LL1020; _LL1024: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1037=( char*)"qualified declarations are not allowed";
struct _tagged_string _temp1038; _temp1038.curr= _temp1037; _temp1038.base=
_temp1037; _temp1038.last_plus_one= _temp1037 + 39; _temp1038;})); goto _LL1020;
_LL1026: goto _LL1020; _LL1028:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1039=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1039[ 0]=({ struct Cyc_Core_Impossible_struct _temp1040; _temp1040.tag= Cyc_Core_Impossible;
_temp1040.f1=( struct _tagged_string)({ char* _temp1041=( char*)"tcTuniondecl: Loc_n";
struct _tagged_string _temp1042; _temp1042.curr= _temp1041; _temp1042.base=
_temp1041; _temp1042.last_plus_one= _temp1041 + 20; _temp1042;}); _temp1040;});
_temp1039;})); goto _LL1020; _LL1020:;}}} if( ! tud->is_xtunion){ for( 0; f1s !=
0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1044= f1s; if( _temp1044 == 0){
_throw( Null_Exception);} _temp1044->tl;}), f2s=({ struct Cyc_List_List*
_temp1045= f2s; if( _temp1045 == 0){ _throw( Null_Exception);} _temp1045->tl;})){
Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1046= f1s; if( _temp1046 == 0){ _throw( Null_Exception);} _temp1046->hd;}),(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1047= f2s; if(
_temp1047 == 0){ _throw( Null_Exception);} _temp1047->hd;}), inst, te, v);} if(
f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1049=* v; struct
_tagged_string _temp1050=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1048= f1s; if( _temp1048 == 0){ _throw( Null_Exception);} _temp1048->hd;}))->name).f2;
xprintf("redeclaration of tunion %.*s has extra field %.*s", _temp1049.last_plus_one
- _temp1049.curr, _temp1049.curr, _temp1050.last_plus_one - _temp1050.curr,
_temp1050.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1052=* v; struct _tagged_string _temp1053=*(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1051= f2s; if( _temp1051 == 0){ _throw(
Null_Exception);} _temp1051->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp1052.last_plus_one - _temp1052.curr, _temp1052.curr, _temp1053.last_plus_one
- _temp1053.curr, _temp1053.curr);}));} return;} prev_fields= f2s; goto _LL983;}}
_LL983:;} goto _LL958;}} _LL958:;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1091= tud->fields; if( _temp1091 == 0){ _throw(
Null_Exception);} _temp1091->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1056= fs; if( _temp1056 == 0){ _throw( Null_Exception);} _temp1056->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1090= fs; if( _temp1090 == 0){ _throw( Null_Exception);} _temp1090->hd;});
if( tud->is_xtunion){ struct Cyc_List_List* prevs= prev_fields; for( 0; prevs !=
0; prevs=({ struct Cyc_List_List* _temp1057= prevs; if( _temp1057 == 0){ _throw(
Null_Exception);} _temp1057->tl;})){ if( Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1058= prevs; if( _temp1058 == 0){ _throw(
Null_Exception);} _temp1058->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1059= fs; if( _temp1059 == 0){ _throw( Null_Exception);}
_temp1059->hd;}))->name) == 0){ Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1060= prevs; if( _temp1060 == 0){ _throw(
Null_Exception);} _temp1060->hd;}),( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1061= fs; if( _temp1061 == 0){ _throw( Null_Exception);} _temp1061->hd;}),
0, te, v); break;}} if( prevs != 0){ continue;}} else{ struct Cyc_List_List*
prevs= prev_fields; for( 0; prevs != 0; prevs=({ struct Cyc_List_List* _temp1062=
prevs; if( _temp1062 == 0){ _throw( Null_Exception);} _temp1062->tl;})){ if( Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1063= prevs; if(
_temp1063 == 0){ _throw( Null_Exception);} _temp1063->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1064= fs; if( _temp1064 == 0){ _throw( Null_Exception);}
_temp1064->hd;}))->name) == 0){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp1065=*(* f->name).f2; xprintf("duplicate field name %.*s", _temp1065.last_plus_one
- _temp1065.curr, _temp1065.curr);}));}}} prev_fields=({ struct Cyc_List_List*
_temp1066=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1066->hd=( void*) f; _temp1066->tl= prev_fields; _temp1066;});{ struct Cyc_List_List*
tvs= f->tvs; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1067= tvs; if(
_temp1067 == 0){ _throw( Null_Exception);} _temp1067->tl;})){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1081= tvs; if(
_temp1081 == 0){ _throw( Null_Exception);} _temp1081->hd;}); struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp1068=( void*) c->v; void* _temp1076; _LL1070: if(
_temp1068 ==( void*) Cyc_Absyn_No_constr){ goto _LL1071;} else{ goto _LL1072;}
_LL1072: if(( unsigned int) _temp1068 > 1u?*(( int*) _temp1068) == Cyc_Absyn_Eq_constr:
0){ _LL1077: _temp1076=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1068)->f1;
if( _temp1076 ==( void*) Cyc_Absyn_MemKind){ goto _LL1073;} else{ goto _LL1074;}}
else{ goto _LL1074;} _LL1074: goto _LL1075; _LL1071:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1078=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1078[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1079; _temp1079.tag= Cyc_Absyn_Eq_constr;
_temp1079.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1079;}); _temp1078;})));
goto _LL1069; _LL1073: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1080=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1080.last_plus_one
- _temp1080.curr, _temp1080.curr);})); goto _LL1069; _LL1075: goto _LL1069;
_LL1069:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1082= typs; if( _temp1082 == 0){ _throw(
Null_Exception);} _temp1082->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1083= typs; if( _temp1083 == 0){ _throw( Null_Exception);} _temp1083->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1084=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1084[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1085; _temp1085.tag= Cyc_Absyn_Abs_n; _temp1085.f1= te->ns; _temp1085;});
_temp1084;});{ struct Cyc_Absyn_Tuniondecl* tudres= tud; if( tud_opt != 0){
tudres=*(( struct Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp1086=
tud_opt; if( _temp1086 == 0){ _throw( Null_Exception);} _temp1086->v;}));} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(* f->name).f2,({ struct
_tuple6* _temp1087=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp1087->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp1088=( struct
Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp1088[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp1089; _temp1089.tag= Cyc_Tcenv_TunionRes;
_temp1089.f1= tudres; _temp1089.f2= f; _temp1089;}); _temp1088;}); _temp1087->f2=
1; _temp1087;}));}}}}}}} static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl*
ed){ struct _tagged_string* v=(* ed->name).f2;{ void* _temp1092=(* ed->name).f1;
struct Cyc_List_List* _temp1100; struct Cyc_List_List* _temp1102; _LL1094: if((
unsigned int) _temp1092 > 1u?*(( int*) _temp1092) == Cyc_Absyn_Rel_n: 0){
_LL1101: _temp1100=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1092)->f1; if( _temp1100 == 0){ goto _LL1095;} else{ goto _LL1096;}} else{
goto _LL1096;} _LL1096: if(( unsigned int) _temp1092 > 1u?*(( int*) _temp1092)
== Cyc_Absyn_Abs_n: 0){ _LL1103: _temp1102=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1092)->f1; if( _temp1102 == 0){ goto _LL1097;} else{ goto _LL1098;}} else{
goto _LL1098;} _LL1098: goto _LL1099; _LL1095: goto _LL1093; _LL1097: goto
_LL1093; _LL1099: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1104=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp1105; _temp1105.curr= _temp1104; _temp1105.base= _temp1104;
_temp1105.last_plus_one= _temp1104 + 43; _temp1105;})); return; _LL1093:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1106=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1106[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1107; _temp1107.tag= Cyc_Absyn_Abs_n; _temp1107.f1= te->ns; _temp1107;});
_temp1106;});{ struct Cyc_Core_Opt* _temp1108=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->enumdecls,
v); struct Cyc_Core_Opt _temp1114; struct Cyc_Absyn_Enumdecl** _temp1115;
_LL1110: if( _temp1108 == 0){ goto _LL1111;} else{ goto _LL1112;} _LL1112: if(
_temp1108 == 0){ goto _LL1109;} else{ _temp1114=* _temp1108; _LL1116: _temp1115=(
struct Cyc_Absyn_Enumdecl**) _temp1114.v; goto _LL1113;} _LL1111: ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct
Cyc_Absyn_Enumdecl** _temp1117=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*)); _temp1117[ 0]= ed; _temp1117;})); goto _LL1109;
_LL1113: { struct Cyc_Absyn_Enumdecl* ed2=* _temp1115;( void*)( ed->sc=( void*)((
void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, 0,( void*) ed2->sc,
0, loc,( struct _tagged_string)({ char* _temp1118=( char*)"enum"; struct
_tagged_string _temp1119; _temp1119.curr= _temp1118; _temp1119.base= _temp1118;
_temp1119.last_plus_one= _temp1118 + 5; _temp1119;}), v));{ struct Cyc_List_List*
f1s= ed->fields; struct Cyc_List_List* f2s= ed2->fields; unsigned int tag_count=
0; for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1120= f1s;
if( _temp1120 == 0){ _throw( Null_Exception);} _temp1120->tl;}), f2s=({ struct
Cyc_List_List* _temp1121= f2s; if( _temp1121 == 0){ _throw( Null_Exception);}
_temp1121->tl;})){ struct Cyc_Absyn_Enumfield _temp1125; struct Cyc_Position_Segment*
_temp1126; struct Cyc_Absyn_Exp* _temp1128; struct Cyc_Absyn_Exp** _temp1130;
struct _tuple0* _temp1131; struct Cyc_Absyn_Enumfield* _temp1123=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp1122= f1s; if( _temp1122 == 0){ _throw(
Null_Exception);} _temp1122->hd;}); _temp1125=* _temp1123; _LL1132: _temp1131=(
struct _tuple0*) _temp1125.name; goto _LL1129; _LL1129: _temp1128=( struct Cyc_Absyn_Exp*)
_temp1125.tag; _temp1130=&(* _temp1123).tag; goto _LL1127; _LL1127: _temp1126=(
struct Cyc_Position_Segment*) _temp1125.loc; goto _LL1124; _LL1124: { struct Cyc_Absyn_Enumfield
_temp1136; struct Cyc_Position_Segment* _temp1137; struct Cyc_Absyn_Exp*
_temp1139; struct _tuple0* _temp1141; struct Cyc_Absyn_Enumfield* _temp1134=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1133= f2s; if(
_temp1133 == 0){ _throw( Null_Exception);} _temp1133->hd;}); _temp1136=*
_temp1134; _LL1142: _temp1141=( struct _tuple0*) _temp1136.name; goto _LL1140;
_LL1140: _temp1139=( struct Cyc_Absyn_Exp*) _temp1136.tag; goto _LL1138; _LL1138:
_temp1137=( struct Cyc_Position_Segment*) _temp1136.loc; goto _LL1135; _LL1135:(*
_temp1131).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1143=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1143[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1144; _temp1144.tag= Cyc_Absyn_Abs_n; _temp1144.f1= te->ns; _temp1144;});
_temp1143;}); if( Cyc_String_zstrptrcmp((* _temp1131).f2,(* _temp1141).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1145=* v; struct
_tagged_string _temp1146=*(* _temp1131).f2; struct _tagged_string _temp1147=*(*
_temp1141).f2; xprintf("redeclaration of enum %.*s: field name mismatch %.*s != %.*s",
_temp1145.last_plus_one - _temp1145.curr, _temp1145.curr, _temp1146.last_plus_one
- _temp1146.curr, _temp1146.curr, _temp1147.last_plus_one - _temp1147.curr,
_temp1147.curr);}));} if(* _temp1130 == 0){* _temp1130=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, _temp1126);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1148=* _temp1130; if(
_temp1148 == 0){ _throw( Null_Exception);} _temp1148;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1149=* v; struct _tagged_string _temp1150=*(*
_temp1131).f2; xprintf("redeclaration of enum %.*s, field %.*s: expression is not constant",
_temp1149.last_plus_one - _temp1149.curr, _temp1149.curr, _temp1150.last_plus_one
- _temp1150.curr, _temp1150.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1154=* _temp1130; if(
_temp1154 == 0){ _throw( Null_Exception);} _temp1154;})); tag_count= t1 + 1; if(
t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1151= _temp1139; if( _temp1151 == 0){ _throw( Null_Exception);} _temp1151;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1152=* v; struct
_tagged_string _temp1153=*(* _temp1131).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1152.last_plus_one - _temp1152.curr, _temp1152.curr, _temp1153.last_plus_one
- _temp1153.curr, _temp1153.curr);}));}}}} return;}} _LL1109:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1155= fs; if( _temp1155 == 0){
_throw( Null_Exception);} _temp1155->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1167= fs; if(
_temp1167 == 0){ _throw( Null_Exception);} _temp1167->hd;}); if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp1156=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp1156.last_plus_one - _temp1156.curr,
_temp1156.curr);}));} prev_fields=({ struct Cyc_List_List* _temp1157=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1157->hd=( void*)(*
f->name).f2; _temp1157->tl= prev_fields; _temp1157;}); if( f->tag == 0){ f->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1158= f->tag; if(
_temp1158 == 0){ _throw( Null_Exception);} _temp1158;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1159=* v; struct _tagged_string _temp1160=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp1159.last_plus_one
- _temp1159.curr, _temp1159.curr, _temp1160.last_plus_one - _temp1160.curr,
_temp1160.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1166= f->tag; if( _temp1166 == 0){
_throw( Null_Exception);} _temp1166;})); tag_count= t1 + 1;(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1161=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1161[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1162; _temp1162.tag= Cyc_Absyn_Abs_n; _temp1162.f1= te->ns; _temp1162;});
_temp1161;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1163=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1163->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1164=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1164[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1165; _temp1165.tag= Cyc_Tcenv_EnumRes;
_temp1165.f1= ed; _temp1165.f2= f; _temp1165;}); _temp1164;}); _temp1163->f2= 1;
_temp1163;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp1168=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp1168[ 0]= ed; _temp1168;}));}}
static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){
void* _temp1169= sc; _LL1171: if( _temp1169 ==( void*) Cyc_Absyn_Static){ goto
_LL1172;} else{ goto _LL1173;} _LL1173: if( _temp1169 ==( void*) Cyc_Absyn_Abstract){
goto _LL1174;} else{ goto _LL1175;} _LL1175: if( _temp1169 ==( void*) Cyc_Absyn_Public){
goto _LL1176;} else{ goto _LL1177;} _LL1177: if( _temp1169 ==( void*) Cyc_Absyn_Extern){
goto _LL1178;} else{ goto _LL1179;} _LL1179: if( _temp1169 ==( void*) Cyc_Absyn_ExternC){
goto _LL1180;} else{ goto _LL1170;} _LL1172: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1181=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1182; _temp1182.curr= _temp1181; _temp1182.base=
_temp1181; _temp1182.last_plus_one= _temp1181 + 44; _temp1182;})); return 0;
_LL1174: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1183=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1184; _temp1184.curr= _temp1183; _temp1184.base=
_temp1183; _temp1184.last_plus_one= _temp1183 + 46; _temp1184;})); return 0;
_LL1176: return 1; _LL1178: return 1; _LL1180: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1185=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base=
_temp1185; _temp1186.last_plus_one= _temp1185 + 30; _temp1186;})); return 1;
_LL1170:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1187= ds; if( _temp1187 == 0){ _throw( Null_Exception);} _temp1187->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1287= ds; if( _temp1287 == 0){ _throw( Null_Exception);} _temp1287->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp1188=( void*) d->r; int
_temp1212; struct Cyc_Absyn_Exp* _temp1214; struct Cyc_Core_Opt* _temp1216;
struct Cyc_Core_Opt* _temp1218; struct Cyc_Absyn_Pat* _temp1220; struct Cyc_Absyn_Vardecl*
_temp1222; struct Cyc_Absyn_Fndecl* _temp1224; struct Cyc_Absyn_Typedefdecl*
_temp1226; struct Cyc_Absyn_Structdecl* _temp1228; struct Cyc_Absyn_Uniondecl*
_temp1230; struct Cyc_Absyn_Tuniondecl* _temp1232; struct Cyc_Absyn_Enumdecl*
_temp1234; struct Cyc_List_List* _temp1236; struct _tagged_string* _temp1238;
struct Cyc_List_List* _temp1240; struct _tuple0* _temp1242; struct _tuple0
_temp1244; struct _tagged_string* _temp1245; void* _temp1247; struct Cyc_List_List*
_temp1249; _LL1190: if(*(( int*) _temp1188) == Cyc_Absyn_Let_d){ _LL1221:
_temp1220=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1188)->f1;
goto _LL1219; _LL1219: _temp1218=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1188)->f2; goto _LL1217; _LL1217: _temp1216=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1188)->f3; goto _LL1215; _LL1215: _temp1214=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1188)->f4; goto
_LL1213; _LL1213: _temp1212=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1188)->f5;
goto _LL1191;} else{ goto _LL1192;} _LL1192: if(*(( int*) _temp1188) == Cyc_Absyn_Var_d){
_LL1223: _temp1222=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1188)->f1; goto _LL1193;} else{ goto _LL1194;} _LL1194: if(*(( int*)
_temp1188) == Cyc_Absyn_Fn_d){ _LL1225: _temp1224=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1188)->f1; goto _LL1195;} else{ goto _LL1196;}
_LL1196: if(*(( int*) _temp1188) == Cyc_Absyn_Typedef_d){ _LL1227: _temp1226=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1188)->f1;
goto _LL1197;} else{ goto _LL1198;} _LL1198: if(*(( int*) _temp1188) == Cyc_Absyn_Struct_d){
_LL1229: _temp1228=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp1188)->f1; goto _LL1199;} else{ goto _LL1200;} _LL1200: if(*(( int*)
_temp1188) == Cyc_Absyn_Union_d){ _LL1231: _temp1230=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp1188)->f1; goto _LL1201;} else{ goto
_LL1202;} _LL1202: if(*(( int*) _temp1188) == Cyc_Absyn_Tunion_d){ _LL1233:
_temp1232=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1188)->f1; goto _LL1203;} else{ goto _LL1204;} _LL1204: if(*(( int*)
_temp1188) == Cyc_Absyn_Enum_d){ _LL1235: _temp1234=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1188)->f1; goto _LL1205;} else{ goto
_LL1206;} _LL1206: if(*(( int*) _temp1188) == Cyc_Absyn_Namespace_d){ _LL1239:
_temp1238=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1188)->f1; goto _LL1237; _LL1237: _temp1236=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1188)->f2; goto _LL1207;} else{ goto
_LL1208;} _LL1208: if(*(( int*) _temp1188) == Cyc_Absyn_Using_d){ _LL1243:
_temp1242=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1188)->f1;
_temp1244=* _temp1242; _LL1248: _temp1247= _temp1244.f1; goto _LL1246; _LL1246:
_temp1245= _temp1244.f2; goto _LL1241; _LL1241: _temp1240=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1188)->f2; goto _LL1209;} else{ goto
_LL1210;} _LL1210: if(*(( int*) _temp1188) == Cyc_Absyn_ExternC_d){ _LL1250:
_temp1249=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1188)->f1; goto _LL1211;} else{ goto _LL1189;} _LL1191: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1251=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp1252; _temp1252.curr= _temp1251; _temp1252.base=
_temp1251; _temp1252.last_plus_one= _temp1251 + 47; _temp1252;})); goto _LL1189;
_LL1193: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1222->sc): 0){(
void*)( _temp1222->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl(
te, ge, loc, _temp1222); goto _LL1189; _LL1195: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1224->sc): 0){( void*)( _temp1224->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1224); goto _LL1189; _LL1197: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1226); goto _LL1189; _LL1199: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1228->sc): 0){( void*)( _temp1228->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1228); goto _LL1189; _LL1201: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1230->sc): 0){( void*)(
_temp1230->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1230); goto _LL1189; _LL1203: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1232->sc): 0){( void*)( _temp1232->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1232); goto _LL1189; _LL1205: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1234->sc): 0){( void*)(
_temp1234->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge,
loc, _temp1234); goto _LL1189; _LL1207: { struct Cyc_List_List* ns= te->ns;
struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1253=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1253->hd=( void*) _temp1238; _temp1253->tl= 0; _temp1253;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1238)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1238); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1236, in_externC); te->ns= ns; goto
_LL1189;} _LL1209: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1254= _temp1247; struct Cyc_List_List* _temp1266; struct Cyc_List_List*
_temp1268; struct Cyc_List_List* _temp1270; struct Cyc_List_List _temp1272;
struct Cyc_List_List* _temp1273; struct _tagged_string* _temp1275; struct Cyc_List_List*
_temp1277; struct Cyc_List_List _temp1279; struct Cyc_List_List* _temp1280;
struct _tagged_string* _temp1282; _LL1256: if( _temp1254 ==( void*) Cyc_Absyn_Loc_n){
goto _LL1257;} else{ goto _LL1258;} _LL1258: if(( unsigned int) _temp1254 > 1u?*((
int*) _temp1254) == Cyc_Absyn_Rel_n: 0){ _LL1267: _temp1266=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1254)->f1; if( _temp1266 == 0){ goto
_LL1259;} else{ goto _LL1260;}} else{ goto _LL1260;} _LL1260: if(( unsigned int)
_temp1254 > 1u?*(( int*) _temp1254) == Cyc_Absyn_Abs_n: 0){ _LL1269: _temp1268=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1254)->f1; if(
_temp1268 == 0){ goto _LL1261;} else{ goto _LL1262;}} else{ goto _LL1262;}
_LL1262: if(( unsigned int) _temp1254 > 1u?*(( int*) _temp1254) == Cyc_Absyn_Rel_n:
0){ _LL1271: _temp1270=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1254)->f1; if( _temp1270 == 0){ goto _LL1264;} else{ _temp1272=* _temp1270;
_LL1276: _temp1275=( struct _tagged_string*) _temp1272.hd; goto _LL1274; _LL1274:
_temp1273=( struct Cyc_List_List*) _temp1272.tl; goto _LL1263;}} else{ goto
_LL1264;} _LL1264: if(( unsigned int) _temp1254 > 1u?*(( int*) _temp1254) == Cyc_Absyn_Abs_n:
0){ _LL1278: _temp1277=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1254)->f1; if( _temp1277 == 0){ goto _LL1255;} else{ _temp1279=* _temp1277;
_LL1283: _temp1282=( struct _tagged_string*) _temp1279.hd; goto _LL1281; _LL1281:
_temp1280=( struct Cyc_List_List*) _temp1279.tl; goto _LL1265;}} else{ goto
_LL1255;} _LL1257: goto _LL1259; _LL1259: goto _LL1261; _LL1261: first=
_temp1245; rest= 0; goto _LL1255; _LL1263: _temp1282= _temp1275; _temp1280=
_temp1273; goto _LL1265; _LL1265: first= _temp1282; rest=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1280,({
struct Cyc_List_List* _temp1284=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1284->hd=( void*) _temp1245; _temp1284->tl= 0;
_temp1284;})); goto _LL1255; _LL1255:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace(
te, loc, first, rest); ge->availables=({ struct Cyc_List_List* _temp1285=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1285->hd=(
void*) ns2; _temp1285->tl= ge->availables; _temp1285;}); Cyc_Tc_tc_decls( te,
_temp1240, in_externC); ge->availables=({ struct Cyc_List_List* _temp1286= ge->availables;
if( _temp1286 == 0){ _throw( Null_Exception);} _temp1286->tl;}); goto _LL1189;}}
_LL1211: Cyc_Tc_tc_decls( te, _temp1249, 1); goto _LL1189; _LL1189:;}} static
char _temp1290[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp1290, _temp1290, _temp1290 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1291=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1291->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1292=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1292->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1293=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1293[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1294; _temp1294.tag=
Cyc_Absyn_Namespace_d; _temp1294.f1= Cyc_Tc_cyc_ns; _temp1294.f2= ds; _temp1294;});
_temp1293;})); _temp1292->loc= 0; _temp1292;}); _temp1291->tl= 0; _temp1291;});}
Cyc_Tc_tc_decls( te, ds, 0);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d); static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple8{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp1295=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1305; struct Cyc_List_List* _temp1307; struct Cyc_List_List**
_temp1309; struct _tuple0* _temp1310; struct Cyc_List_List* _temp1312; struct
Cyc_List_List** _temp1314; struct _tagged_string* _temp1315; _LL1297: if(*(( int*)
_temp1295) == Cyc_Absyn_Var_d){ _LL1306: _temp1305=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1295)->f1; goto _LL1298;} else{ goto
_LL1299;} _LL1299: if(*(( int*) _temp1295) == Cyc_Absyn_Using_d){ _LL1311:
_temp1310=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1295)->f1;
goto _LL1308; _LL1308: _temp1307=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1295)->f2; _temp1309=&(( struct Cyc_Absyn_Using_d_struct*) _temp1295)->f2;
goto _LL1300;} else{ goto _LL1301;} _LL1301: if(*(( int*) _temp1295) == Cyc_Absyn_Namespace_d){
_LL1316: _temp1315=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1295)->f1; goto _LL1313; _LL1313: _temp1312=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1295)->f2; _temp1314=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1295)->f2; goto _LL1302;} else{ goto _LL1303;} _LL1303: goto _LL1304;
_LL1298: if(( void*) _temp1305->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{
struct _tuple0 _temp1319; struct _tagged_string* _temp1320; void* _temp1322;
struct _tuple0* _temp1317= _temp1305->name; _temp1319=* _temp1317; _LL1323:
_temp1322= _temp1319.f1; goto _LL1321; _LL1321: _temp1320= _temp1319.f2; goto
_LL1318; _LL1318: { struct Cyc_List_List* ns;{ void* _temp1324= _temp1322;
struct Cyc_List_List* _temp1332; struct Cyc_List_List* _temp1334; _LL1326: if(
_temp1324 ==( void*) Cyc_Absyn_Loc_n){ goto _LL1327;} else{ goto _LL1328;}
_LL1328: if(( unsigned int) _temp1324 > 1u?*(( int*) _temp1324) == Cyc_Absyn_Rel_n:
0){ _LL1333: _temp1332=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1324)->f1; goto _LL1329;} else{ goto _LL1330;} _LL1330: if(( unsigned int)
_temp1324 > 1u?*(( int*) _temp1324) == Cyc_Absyn_Abs_n: 0){ _LL1335: _temp1334=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1324)->f1; goto
_LL1331;} else{ goto _LL1325;} _LL1327: ns= 0; goto _LL1325; _LL1329: ns=
_temp1332; goto _LL1325; _LL1331: ns= _temp1334; goto _LL1325; _LL1325:;}{
struct _tuple8* nsenv=(( struct _tuple8*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( env, ns); struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; int
needed=(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->ordinaries, _temp1320)).f2; if( ! needed){(* nsenv).f2=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((*
nsenv).f2, _temp1320);} return needed;}}} _LL1300: _temp1314= _temp1309; goto
_LL1302; _LL1302:* _temp1314= Cyc_Tc_treeshake_f( env,* _temp1314); return 1;
_LL1304: return 1; _LL1296:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_string* x, struct _tuple6* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple8* nsenv){ struct Cyc_Tcenv_Genv* ge=(*
nsenv).f1; struct Cyc_Set_Set* set=(* nsenv).f2; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple6*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
set, ge->ordinaries);} static struct _tuple8* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple8* _temp1336=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1336->f1= ge; _temp1336->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp1336;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* env=((
struct Cyc_Dict_Dict*(*)( struct _tuple8*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* ds1= Cyc_Tc_treeshake_f( env, ds);(( void(*)( void(* f)( struct
Cyc_List_List*, struct _tuple8*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
env); return ds1;}