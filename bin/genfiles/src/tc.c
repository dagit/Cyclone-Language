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
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_String_strcmp( struct
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
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att=
2; static const int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att=
4; static const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int
Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att= 6; static const
int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att= 8; static
const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att= 10;
static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
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
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type( void* t); extern
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default(
void*); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**,
struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, int var_default_init, struct Cyc_List_List* ds); extern
struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*);
extern char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
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
_tagged_string* msg); static char _temp2[ 1u]=""; static struct _tagged_string
Cyc_Tc_tc_msg_c={ _temp2, _temp2, _temp2 + 1u}; static struct _tagged_string*
Cyc_Tc_tc_msg=( struct _tagged_string*)& Cyc_Tc_tc_msg_c; static struct Cyc_List_List*
Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List* atts){ void* _temp3=
Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp9; struct Cyc_List_List*
_temp11; struct Cyc_List_List** _temp13; int _temp14; struct Cyc_List_List*
_temp16; void* _temp18; struct Cyc_Core_Opt* _temp20; struct Cyc_List_List*
_temp22; _LL5: if(( unsigned int) _temp3 > 4u?*(( int*) _temp3) == Cyc_Absyn_FnType:
0){ _LL10: _temp9=(( struct Cyc_Absyn_FnType_struct*) _temp3)->f1; _LL23:
_temp22= _temp9.tvars; goto _LL21; _LL21: _temp20= _temp9.effect; goto _LL19;
_LL19: _temp18=( void*) _temp9.ret_typ; goto _LL17; _LL17: _temp16= _temp9.args;
goto _LL15; _LL15: _temp14= _temp9.varargs; goto _LL12; _LL12: _temp11= _temp9.attributes;
_temp13=&((( struct Cyc_Absyn_FnType_struct*) _temp3)->f1).attributes; goto _LL6;}
else{ goto _LL7;} _LL7: goto _LL8; _LL6: { struct Cyc_List_List* _temp24= 0;
goto _LL25; _LL25: for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ void* _temp26=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd;
int _temp42; _LL28: if(( unsigned int) _temp26 > 16u?*(( int*) _temp26) == Cyc_Absyn_Regparm_att:
0){ _LL43: _temp42=(( struct Cyc_Absyn_Regparm_att_struct*) _temp26)->f1; goto
_LL29;} else{ goto _LL30;} _LL30: if( _temp26 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL31;} else{ goto _LL32;} _LL32: if( _temp26 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp26 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp26 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL37;} else{ goto _LL38;} _LL38: if( _temp26 ==( void*) Cyc_Absyn_Const_att){
goto _LL39;} else{ goto _LL40;} _LL40: goto _LL41; _LL29: goto _LL31; _LL31:
goto _LL33; _LL33: goto _LL35; _LL35: goto _LL37; _LL37: goto _LL39; _LL39:*
_temp13=({ struct Cyc_List_List* _temp44=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp44->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp44->tl=* _temp13; _temp44;}); continue; _LL41:
_temp24=({ struct Cyc_List_List* _temp45=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp45->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); _temp45->tl= _temp24; _temp45;}); continue; _LL27:;}
return _temp24;} _LL8: return(( struct Cyc_List_List*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(({ char* _temp46="transfer_fn_type_atts"; struct
_tagged_string _temp47; _temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one=
_temp46 + 22; _temp47;})); _LL4:;} struct _tuple3{ void* f1; int f2; } ; static
void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl* vd, int
check_var_init){ struct _tagged_string* v=(* vd->name).f2; void* t=( void*) vd->type;
void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{ void*
_temp48=(* vd->name).f1; struct Cyc_List_List* _temp56; struct Cyc_List_List*
_temp58; _LL50: if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Rel_n:
0){ _LL57: _temp56=(( struct Cyc_Absyn_Rel_n_struct*) _temp48)->f1; if( _temp56
== 0){ goto _LL51;} else{ goto _LL52;}} else{ goto _LL52;} _LL52: if((
unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Abs_n: 0){ _LL59:
_temp58=(( struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; if( _temp58 == 0){ goto
_LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: goto _LL55; _LL51: goto
_LL49; _LL53: goto _LL49; _LL55: Cyc_Tcutil_terr( loc,({ char* _temp60="qualified declarations are not implemented";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 43; _temp61;})); return; _LL49:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp62=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp62[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp63; _temp63.tag= Cyc_Absyn_Abs_n; _temp63.f1= te->ns; _temp63;}); _temp62;});{
void* _temp64= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp71; struct
Cyc_Absyn_Tqual _temp73; void* _temp75; _LL66: if(( unsigned int) _temp64 > 4u?*((
int*) _temp64) == Cyc_Absyn_ArrayType: 0){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp64)->f1; goto _LL74; _LL74: _temp73=(( struct Cyc_Absyn_ArrayType_struct*)
_temp64)->f2; goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_ArrayType_struct*)
_temp64)->f3; if( _temp71 == 0){ goto _LL70;} else{ goto _LL68;}} else{ goto
_LL68;} _LL70: if( vd->initializer != 0){ goto _LL67;} else{ goto _LL68;} _LL68:
goto _LL69; _LL67:{ void* _temp77=( void*)(( struct Cyc_Absyn_Exp*) _check_null(
vd->initializer))->r; void* _temp87; struct _tagged_string _temp89; struct Cyc_List_List*
_temp91; struct Cyc_Core_Opt* _temp93; struct Cyc_List_List* _temp95; _LL79: if(*((
int*) _temp77) == Cyc_Absyn_Const_e){ _LL88: _temp87=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp77)->f1; if(( unsigned int) _temp87 > 1u?*(( int*) _temp87) == Cyc_Absyn_String_c:
0){ _LL90: _temp89=(( struct Cyc_Absyn_String_c_struct*) _temp87)->f1; goto
_LL80;} else{ goto _LL81;}} else{ goto _LL81;} _LL81: if(*(( int*) _temp77) ==
Cyc_Absyn_UnresolvedMem_e){ _LL94: _temp93=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp77)->f1; goto _LL92; _LL92: _temp91=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp77)->f2; goto _LL82;} else{ goto _LL83;} _LL83: if(*(( int*) _temp77) ==
Cyc_Absyn_Array_e){ _LL96: _temp95=(( struct Cyc_Absyn_Array_e_struct*) _temp77)->f1;
goto _LL84;} else{ goto _LL85;} _LL85: goto _LL86; _LL80: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp97=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp97[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp98; _temp98.tag= Cyc_Absyn_ArrayType; _temp98.f1=(
void*) _temp75; _temp98.f2= _temp73; _temp98.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(({
struct _tagged_string _temp99= _temp89;( unsigned int)( _temp99.last_plus_one -
_temp99.curr);}), 0); _temp98;}); _temp97;}))); goto _LL78; _LL82: _temp95=
_temp91; goto _LL84; _LL84: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp100=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp100[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp101; _temp101.tag= Cyc_Absyn_ArrayType;
_temp101.f1=( void*) _temp75; _temp101.f2= _temp73; _temp101.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp95), 0); _temp101;}); _temp100;}))); goto _LL78; _LL86: goto _LL78; _LL78:;}
goto _LL65; _LL69: goto _LL65; _LL65:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,({ char* _temp102="extern declaration should not have initializer";
struct _tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 47; _temp103;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp104=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int
_temp124; struct _tagged_string _temp126; _LL106: if(( unsigned int) _temp104 >
16u?*(( int*) _temp104) == Cyc_Absyn_Aligned_att: 0){ _LL125: _temp124=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp104)->f1; goto _LL107;} else{ goto _LL108;}
_LL108: if(( unsigned int) _temp104 > 16u?*(( int*) _temp104) == Cyc_Absyn_Section_att:
0){ _LL127: _temp126=(( struct Cyc_Absyn_Section_att_struct*) _temp104)->f1;
goto _LL109;} else{ goto _LL110;} _LL110: if( _temp104 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL111;} else{ goto _LL112;} _LL112: if( _temp104 ==( void*) Cyc_Absyn_Shared_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp104 ==( void*) Cyc_Absyn_Unused_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp104 ==( void*) Cyc_Absyn_Weak_att){
goto _LL117;} else{ goto _LL118;} _LL118: if( _temp104 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL119;} else{ goto _LL120;} _LL120: if( _temp104 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL121;} else{ goto _LL122;} _LL122: goto _LL123; _LL107: continue; _LL109:
continue; _LL111: continue; _LL113: continue; _LL115: continue; _LL117:
continue; _LL119: continue; _LL121: continue; _LL123: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp128= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp129= Cyc_Absynpp_qvar2string(
vd->name); xprintf("bad attribute %.*s for variable %.*s", _temp128.last_plus_one
- _temp128.curr, _temp128.curr, _temp129.last_plus_one - _temp129.curr, _temp129.curr);}));
goto _LL105; _LL105:;} if( vd->initializer == 0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp130= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp131= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp130.last_plus_one - _temp130.curr, _temp130.curr, _temp131.last_plus_one -
_temp131.curr, _temp131.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp132=(
struct Cyc_Absyn_Exp*) _check_null( vd->initializer); goto _LL133; _LL133: {
void* _temp134= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp132); goto
_LL135; _LL135: if( ! Cyc_Tcutil_coerce_assign( te, _temp132, t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp136= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp137= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp138= Cyc_Absynpp_typ2string( _temp134); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp136.last_plus_one - _temp136.curr, _temp136.curr, _temp137.last_plus_one -
_temp137.curr, _temp137.curr, _temp138.last_plus_one - _temp138.curr, _temp138.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, _temp132)){ Cyc_Tcutil_terr( loc,({ char*
_temp139="initializer is not a constant expression"; struct _tagged_string
_temp140; _temp140.curr= _temp139; _temp140.base= _temp139; _temp140.last_plus_one=
_temp139 + 41; _temp140;}));}}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp141=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp161; int _temp163; _LL143: if(( unsigned int)
_temp141 > 16u?*(( int*) _temp141) == Cyc_Absyn_Regparm_att: 0){ _LL162:
_temp161=(( struct Cyc_Absyn_Regparm_att_struct*) _temp141)->f1; goto _LL144;}
else{ goto _LL145;} _LL145: if( _temp141 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL146;} else{ goto _LL147;} _LL147: if( _temp141 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL148;} else{ goto _LL149;} _LL149: if( _temp141 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL150;} else{ goto _LL151;} _LL151: if( _temp141 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL152;} else{ goto _LL153;} _LL153: if( _temp141 ==( void*) Cyc_Absyn_Const_att){
goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp141 > 16u?*((
int*) _temp141) == Cyc_Absyn_Aligned_att: 0){ _LL164: _temp163=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp141)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if( _temp141 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL158;} else{ goto _LL159;} _LL159: goto _LL160;
_LL144: goto _LL146; _LL146: goto _LL148; _LL148: goto _LL150; _LL150: goto
_LL152; _LL152: goto _LL154; _LL154:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp165="tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp166; _temp166.curr= _temp165; _temp166.base= _temp165;
_temp166.last_plus_one= _temp165 + 51; _temp166;})); goto _LL142; _LL156: goto
_LL158; _LL158: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp167= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_temp167.last_plus_one - _temp167.curr, _temp167.curr);})); goto _LL142; _LL160:
continue; _LL142:;}}}{ struct _handler_cons _temp168; _push_handler(& _temp168);{
int _temp170= 0; if( setjmp( _temp168.handler)){ _temp170= 1;} if( ! _temp170){{
struct _tuple3* _temp171=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL172; _LL172: {
void* _temp173=(* _temp171).f1; void* _temp179; _LL175: if(*(( int*) _temp173)
== Cyc_Tcenv_VarRes){ _LL180: _temp179=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp173)->f1; goto _LL176;} else{ goto _LL177;} _LL177: goto _LL178; _LL176: {
struct Cyc_Absyn_Global_b_struct* _temp183=({ struct Cyc_Absyn_Global_b_struct*
_temp181=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp181[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp182; _temp182.tag= Cyc_Absyn_Global_b;
_temp182.f1= vd; _temp182;}); _temp181;}); goto _LL184; _LL184: { void* _temp185=
Cyc_Tcdecl_merge_binding( _temp179,( void*) _temp183, loc, Cyc_Tc_tc_msg); goto
_LL186; _LL186: if( _temp185 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL174;}
if( _temp185 == _temp179?(* _temp171).f2: 0){ goto _LL174;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp187=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp187->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp188=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp188[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp189; _temp189.tag= Cyc_Tcenv_VarRes; _temp189.f1=( void*) _temp185;
_temp189;}); _temp188;}); _temp187->f2= 1; _temp187;})); goto _LL174;}} _LL178:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp190="tcVardecl: a variable is being declared with a wrong name";
struct _tagged_string _temp191; _temp191.curr= _temp190; _temp191.base= _temp190;
_temp191.last_plus_one= _temp190 + 58; _temp191;})); goto _LL174; _LL174:;}};
_pop_handler();} else{ void* _temp169=( void*) _exn_thrown; void* _temp193=
_temp169; _LL195: if( _temp193 == Cyc_Dict_Absent){ goto _LL196;} else{ goto
_LL197;} _LL197: goto _LL198; _LL196: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp199=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp199->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp200=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp200[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp201; _temp201.tag= Cyc_Tcenv_VarRes;
_temp201.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp202=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp202[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp203; _temp203.tag= Cyc_Absyn_Global_b;
_temp203.f1= vd; _temp203;}); _temp202;})); _temp201;}); _temp200;}); _temp199->f2=
0; _temp199;})); goto _LL194; _LL198:( void) _throw( _temp193); _LL194:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp204=(*
fd->name).f1; struct Cyc_List_List* _temp212; struct Cyc_List_List* _temp214;
_LL206: if(( unsigned int) _temp204 > 1u?*(( int*) _temp204) == Cyc_Absyn_Rel_n:
0){ _LL213: _temp212=(( struct Cyc_Absyn_Rel_n_struct*) _temp204)->f1; if(
_temp212 == 0){ goto _LL207;} else{ goto _LL208;}} else{ goto _LL208;} _LL208:
if(( unsigned int) _temp204 > 1u?*(( int*) _temp204) == Cyc_Absyn_Abs_n: 0){
_LL215: _temp214=(( struct Cyc_Absyn_Abs_n_struct*) _temp204)->f1; goto _LL209;}
else{ goto _LL210;} _LL210: goto _LL211; _LL207: goto _LL205; _LL209:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp216=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp216[ 0]=({ struct
Cyc_Core_Impossible_struct _temp217; _temp217.tag= Cyc_Core_Impossible; _temp217.f1=({
char* _temp218="tc: Abs_n in tcFndecl"; struct _tagged_string _temp219; _temp219.curr=
_temp218; _temp219.base= _temp218; _temp219.last_plus_one= _temp218 + 22;
_temp219;}); _temp217;}); _temp216;})); _LL211: Cyc_Tcutil_terr( loc,({ char*
_temp220="qualified declarations are not implemented"; struct _tagged_string
_temp221; _temp221.curr= _temp220; _temp221.base= _temp220; _temp221.last_plus_one=
_temp220 + 43; _temp221;})); return; _LL205:;}(* fd->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp222=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp222[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp223; _temp223.tag= Cyc_Absyn_Abs_n; _temp223.f1= te->ns; _temp223;});
_temp222;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp224=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp232; _LL226: if(
_temp224 ==( void*) Cyc_Absyn_Packed_att){ goto _LL227;} else{ goto _LL228;}
_LL228: if(( unsigned int) _temp224 > 16u?*(( int*) _temp224) == Cyc_Absyn_Aligned_att:
0){ _LL233: _temp232=(( struct Cyc_Absyn_Aligned_att_struct*) _temp224)->f1;
goto _LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL227: goto _LL229;
_LL229: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp234= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s for function",
_temp234.last_plus_one - _temp234.curr, _temp234.curr);})); goto _LL225; _LL231:
goto _LL225; _LL225:;}}{ struct _handler_cons _temp235; _push_handler(& _temp235);{
int _temp237= 0; if( setjmp( _temp235.handler)){ _temp237= 1;} if( ! _temp237){{
struct _tuple3* _temp238=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL239; _LL239: {
void* _temp240=(* _temp238).f1; void* _temp246; _LL242: if(*(( int*) _temp240)
== Cyc_Tcenv_VarRes){ _LL247: _temp246=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp240)->f1; goto _LL243;} else{ goto _LL244;} _LL244: goto _LL245; _LL243: {
struct Cyc_Absyn_Funname_b_struct* _temp250=({ struct Cyc_Absyn_Funname_b_struct*
_temp248=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp248[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp249; _temp249.tag= Cyc_Absyn_Funname_b;
_temp249.f1= fd; _temp249;}); _temp248;}); goto _LL251; _LL251: { void* _temp252=
Cyc_Tcdecl_merge_binding( _temp246,( void*) _temp250, loc, Cyc_Tc_tc_msg); goto
_LL253; _LL253: if( _temp252 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL241;}
if( _temp252 == _temp246?(* _temp238).f2: 0){ goto _LL241;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp254=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp254->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp255=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp255[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp256; _temp256.tag= Cyc_Tcenv_VarRes; _temp256.f1=( void*) _temp252;
_temp256;}); _temp255;}); _temp254->f2= 1; _temp254;})); goto _LL241;}} _LL245:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp257="tcFndecl: a variable is being declared with a wrong name";
struct _tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 57; _temp258;})); goto _LL241; _LL241:;}};
_pop_handler();} else{ void* _temp236=( void*) _exn_thrown; void* _temp260=
_temp236; _LL262: if( _temp260 == Cyc_Dict_Absent){ goto _LL263;} else{ goto
_LL264;} _LL264: goto _LL265; _LL263: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp266=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp266->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp267=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp267[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp268; _temp268.tag= Cyc_Tcenv_VarRes;
_temp268.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp269=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp269[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp270; _temp270.tag= Cyc_Absyn_Funname_b;
_temp270.f1= fd; _temp270;}); _temp269;})); _temp268;}); _temp267;}); _temp266->f2=
0; _temp266;})); goto _LL261; _LL265:( void) _throw( _temp260); _LL261:;}}} te->le=({
struct Cyc_Core_Opt* _temp271=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp271->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp272=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp272[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp273; _temp273.tag= Cyc_Tcenv_Outermost;
_temp273.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp273;}); _temp272;}));
_temp271;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,({ char* _temp274="function has goto statements to undefined labels";
struct _tagged_string _temp275; _temp275.curr= _temp274; _temp275.base= _temp274;
_temp275.last_plus_one= _temp274 + 49; _temp275;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp276=(* td->name).f1; struct Cyc_List_List*
_temp284; struct Cyc_List_List* _temp286; _LL278: if(( unsigned int) _temp276 >
1u?*(( int*) _temp276) == Cyc_Absyn_Rel_n: 0){ _LL285: _temp284=(( struct Cyc_Absyn_Rel_n_struct*)
_temp276)->f1; if( _temp284 == 0){ goto _LL279;} else{ goto _LL280;}} else{ goto
_LL280;} _LL280: if(( unsigned int) _temp276 > 1u?*(( int*) _temp276) == Cyc_Absyn_Abs_n:
0){ _LL287: _temp286=(( struct Cyc_Absyn_Abs_n_struct*) _temp276)->f1; if(
_temp286 == 0){ goto _LL281;} else{ goto _LL282;}} else{ goto _LL282;} _LL282:
goto _LL283; _LL279: goto _LL277; _LL281: goto _LL277; _LL283: Cyc_Tcutil_terr(
loc,({ char* _temp288="qualified declarations are not implemented"; struct
_tagged_string _temp289; _temp289.curr= _temp288; _temp289.base= _temp288;
_temp289.last_plus_one= _temp288 + 43; _temp289;})); return; _LL277:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp290=* v; xprintf("redeclaration of typedef %.*s",
_temp290.last_plus_one - _temp290.curr, _temp290.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp291=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp291[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp292; _temp292.tag= Cyc_Absyn_Abs_n; _temp292.f1= te->ns; _temp292;});
_temp291;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp293= _new_region();
struct _RegionHandle* uprev_rgn=& _temp293; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp294= fields; goto
_LL295; _LL295: for( 0; _temp294 != 0; _temp294=(( struct Cyc_List_List*)
_check_null( _temp294))->tl){ struct Cyc_Absyn_Structfield _temp298; struct Cyc_List_List*
_temp299; struct Cyc_Core_Opt* _temp301; void* _temp303; struct Cyc_Absyn_Tqual
_temp305; struct _tagged_string* _temp307; struct Cyc_Absyn_Structfield*
_temp296=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp294))->hd; _temp298=* _temp296; _LL308: _temp307= _temp298.name; goto
_LL306; _LL306: _temp305= _temp298.tq; goto _LL304; _LL304: _temp303=( void*)
_temp298.type; goto _LL302; _LL302: _temp301= _temp298.width; goto _LL300;
_LL300: _temp299= _temp298.attributes; goto _LL297; _LL297: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp307)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp309=* _temp307;
struct _tagged_string _temp310= obj; xprintf("duplicate field %.*s in %.*s",
_temp309.last_plus_one - _temp309.curr, _temp309.curr, _temp310.last_plus_one -
_temp310.curr, _temp310.curr);}));} if( Cyc_String_strcmp(* _temp307,({ char*
_temp311=""; struct _tagged_string _temp312; _temp312.curr= _temp311; _temp312.base=
_temp311; _temp312.last_plus_one= _temp311 + 1; _temp312;})) != 0){ prev_fields=({
struct Cyc_List_List* _temp313=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp313->hd=( void*) _temp307;
_temp313->tl= prev_fields; _temp313;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, _temp303); if( _temp301 != 0){ unsigned int w= 0; if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp301))->v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp314=* _temp307; xprintf("bitfield %.*s does not have constant width",
_temp314.last_plus_one - _temp314.curr, _temp314.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp301))->v);}{
void* _temp315= Cyc_Tcutil_compress( _temp303); void* _temp321; void* _temp323;
_LL317: if(( unsigned int) _temp315 > 4u?*(( int*) _temp315) == Cyc_Absyn_IntType:
0){ _LL324: _temp323=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp315)->f1;
goto _LL322; _LL322: _temp321=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp315)->f2; goto _LL318;} else{ goto _LL319;} _LL319: goto _LL320; _LL318:{
void* _temp325= _temp321; _LL327: if( _temp325 ==( void*) Cyc_Absyn_B1){ goto
_LL328;} else{ goto _LL329;} _LL329: if( _temp325 ==( void*) Cyc_Absyn_B2){ goto
_LL330;} else{ goto _LL331;} _LL331: if( _temp325 ==( void*) Cyc_Absyn_B4){ goto
_LL332;} else{ goto _LL333;} _LL333: if( _temp325 ==( void*) Cyc_Absyn_B8){ goto
_LL334;} else{ goto _LL326;} _LL328: if( w > 8){ Cyc_Tcutil_terr( loc,({ char*
_temp335="bitfield larger than type"; struct _tagged_string _temp336; _temp336.curr=
_temp335; _temp336.base= _temp335; _temp336.last_plus_one= _temp335 + 26;
_temp336;}));} goto _LL326; _LL330: if( w > 16){ Cyc_Tcutil_terr( loc,({ char*
_temp337="bitfield larger than type"; struct _tagged_string _temp338; _temp338.curr=
_temp337; _temp338.base= _temp337; _temp338.last_plus_one= _temp337 + 26;
_temp338;}));} goto _LL326; _LL332: if( w > 32){ Cyc_Tcutil_terr( loc,({ char*
_temp339="bitfield larger than type"; struct _tagged_string _temp340; _temp340.curr=
_temp339; _temp340.base= _temp339; _temp340.last_plus_one= _temp339 + 26;
_temp340;}));} goto _LL326; _LL334: if( w > 64){ Cyc_Tcutil_terr( loc,({ char*
_temp341="bitfield larger than type"; struct _tagged_string _temp342; _temp342.curr=
_temp341; _temp342.base= _temp341; _temp342.last_plus_one= _temp341 + 26;
_temp342;}));} goto _LL326; _LL326:;} goto _LL316; _LL320: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp343=* _temp307; struct _tagged_string _temp344= Cyc_Absynpp_typ2string(
_temp303); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp343.last_plus_one - _temp343.curr, _temp343.curr, _temp344.last_plus_one -
_temp344.curr, _temp344.curr);})); goto _LL316; _LL316:;}}}}; _pop_region(&
_temp293);} static void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp345=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL346; _LL346: if( sd->name == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp347= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp347.last_plus_one - _temp347.curr, _temp347.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp348=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp356; _LL350: if( _temp348 ==( void*) Cyc_Absyn_Packed_att){
goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp348 > 16u?*((
int*) _temp348) == Cyc_Absyn_Aligned_att: 0){ _LL357: _temp356=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp348)->f1; goto _LL353;} else{ goto _LL354;} _LL354: goto _LL355; _LL351:
continue; _LL353: continue; _LL355: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp358= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp359= obj; struct
_tagged_string _temp360=* _temp345; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp358.last_plus_one - _temp358.curr, _temp358.curr, _temp359.last_plus_one -
_temp359.curr, _temp359.curr, _temp360.last_plus_one - _temp360.curr, _temp360.curr);}));
goto _LL349; _LL349:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp361=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
goto _LL362; _LL362: { void* _temp363=( void*) _temp361->v; void* _temp371;
_LL365: if( _temp363 ==( void*) Cyc_Absyn_No_constr){ goto _LL366;} else{ goto
_LL367;} _LL367: if(( unsigned int) _temp363 > 1u?*(( int*) _temp363) == Cyc_Absyn_Eq_constr:
0){ _LL372: _temp371=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp363)->f1;
if( _temp371 ==( void*) Cyc_Absyn_MemKind){ goto _LL368;} else{ goto _LL369;}}
else{ goto _LL369;} _LL369: goto _LL370; _LL366:( void*)( _temp361->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp373=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp373[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp374; _temp374.tag= Cyc_Absyn_Eq_constr; _temp374.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp374;}); _temp373;}))); continue; _LL368:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp375= obj; struct
_tagged_string _temp376=* _temp345; struct _tagged_string _temp377=*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp375.last_plus_one - _temp375.curr, _temp375.curr, _temp376.last_plus_one -
_temp376.curr, _temp376.curr, _temp377.last_plus_one - _temp377.curr, _temp377.curr);}));
continue; _LL370: continue; _LL364:;}}}{ void* _temp378=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp386; struct Cyc_List_List* _temp388; _LL380: if(( unsigned int) _temp378 >
1u?*(( int*) _temp378) == Cyc_Absyn_Rel_n: 0){ _LL387: _temp386=(( struct Cyc_Absyn_Rel_n_struct*)
_temp378)->f1; if( _temp386 == 0){ goto _LL381;} else{ goto _LL382;}} else{ goto
_LL382;} _LL382: if(( unsigned int) _temp378 > 1u?*(( int*) _temp378) == Cyc_Absyn_Abs_n:
0){ _LL389: _temp388=(( struct Cyc_Absyn_Abs_n_struct*) _temp378)->f1; if(
_temp388 == 0){ goto _LL383;} else{ goto _LL384;}} else{ goto _LL384;} _LL384:
goto _LL385; _LL381: goto _LL379; _LL383: goto _LL379; _LL385: Cyc_Tcutil_terr(
loc,({ char* _temp390="qualified declarations are not implemented"; struct
_tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 43; _temp391;})); return; _LL379:;}(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp392=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp392[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp393; _temp393.tag= Cyc_Absyn_Abs_n; _temp393.f1= te->ns; _temp393;});
_temp392;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs);} struct _tuple4{
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp394=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL395; _LL395: { struct _tagged_string obj=({ char* _temp450="struct"; struct
_tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 7; _temp451;}); Cyc_Tc_tcStructOrUniondecl(
te, ge, obj, loc, sd);{ struct Cyc_List_List* _temp396= sd->tvs; goto _LL397;
_LL397:{ struct _tuple4 _temp399=({ struct _tuple4 _temp398; _temp398.f1= sd->fields;
_temp398.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp394); _temp398;});
struct Cyc_Core_Opt* _temp409; struct Cyc_Core_Opt* _temp411; struct Cyc_Core_Opt*
_temp413; struct Cyc_Core_Opt* _temp415; struct Cyc_Core_Opt _temp417; struct
Cyc_List_List* _temp418; struct Cyc_Core_Opt* _temp420; struct Cyc_Core_Opt
_temp422; struct Cyc_Absyn_Structdecl** _temp423; struct Cyc_Core_Opt* _temp425;
struct Cyc_Core_Opt _temp427; struct Cyc_List_List* _temp428; struct Cyc_Core_Opt*
_temp430; struct Cyc_Core_Opt _temp432; struct Cyc_Absyn_Structdecl** _temp433;
struct Cyc_Core_Opt* _temp435; _LL401: _LL412: _temp411= _temp399.f1; if(
_temp411 == 0){ goto _LL410;} else{ goto _LL403;} _LL410: _temp409= _temp399.f2;
if( _temp409 == 0){ goto _LL402;} else{ goto _LL403;} _LL403: _LL416: _temp415=
_temp399.f1; if( _temp415 == 0){ goto _LL405;} else{ _temp417=* _temp415; _LL419:
_temp418=( struct Cyc_List_List*) _temp417.v; goto _LL414;} _LL414: _temp413=
_temp399.f2; if( _temp413 == 0){ goto _LL404;} else{ goto _LL405;} _LL405:
_LL426: _temp425= _temp399.f1; if( _temp425 == 0){ goto _LL407;} else{ _temp427=*
_temp425; _LL429: _temp428=( struct Cyc_List_List*) _temp427.v; goto _LL421;}
_LL421: _temp420= _temp399.f2; if( _temp420 == 0){ goto _LL407;} else{ _temp422=*
_temp420; _LL424: _temp423=( struct Cyc_Absyn_Structdecl**) _temp422.v; goto
_LL406;} _LL407: _LL436: _temp435= _temp399.f1; if( _temp435 == 0){ goto _LL431;}
else{ goto _LL400;} _LL431: _temp430= _temp399.f2; if( _temp430 == 0){ goto
_LL400;} else{ _temp432=* _temp430; _LL434: _temp433=( struct Cyc_Absyn_Structdecl**)
_temp432.v; goto _LL408;} _LL402: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp394,({ struct Cyc_Absyn_Structdecl**
_temp437=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp437[ 0]= sd; _temp437;})); goto _LL400; _LL404: { struct Cyc_Absyn_Structdecl**
_temp440=({ struct Cyc_Absyn_Structdecl** _temp438=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp438[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp439=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp439->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp439->name= sd->name;
_temp439->tvs= _temp396; _temp439->fields= 0; _temp439->attributes= 0; _temp439;});
_temp438;}); goto _LL441; _LL441: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp394, _temp440); Cyc_Tc_tcStructFields(
te, ge, loc, obj, _temp418, _temp396);* _temp440= sd; goto _LL400;} _LL406: {
struct Cyc_Absyn_Structdecl* _temp442=* _temp423; goto _LL443; _LL443:* _temp423=({
struct Cyc_Absyn_Structdecl* _temp444=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp444->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp444->name= sd->name; _temp444->tvs= _temp396; _temp444->fields= 0; _temp444->attributes=
0; _temp444;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp428, _temp396);*
_temp423= _temp442; _temp433= _temp423; goto _LL408;} _LL408: { struct Cyc_Absyn_Structdecl*
_temp445= Cyc_Tcdecl_merge_structdecl(* _temp433, sd, loc, Cyc_Tc_tc_msg); goto
_LL446; _LL446: if( _temp445 == 0){ return;} else{* _temp433=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp445); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp445);
goto _LL400;}} _LL400:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp394,({ struct _tuple3* _temp447=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp447->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp448=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp448[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp449; _temp449.tag= Cyc_Tcenv_StructRes;
_temp449.f1= sd; _temp449;}); _temp448;}); _temp447->f2= 1; _temp447;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* _temp452=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)).f2; goto _LL453; _LL453: { struct _tagged_string
obj=({ char* _temp505="union"; struct _tagged_string _temp506; _temp506.curr=
_temp505; _temp506.base= _temp505; _temp506.last_plus_one= _temp505 + 6;
_temp506;}); Cyc_Tc_tcStructOrUniondecl( te, ge, obj, loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp454= ud->tvs; goto _LL455; _LL455: { struct
_tuple4 _temp457=({ struct _tuple4 _temp456; _temp456.f1= ud->fields; _temp456.f2=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->uniondecls, _temp452); _temp456;}); struct Cyc_Core_Opt*
_temp467; struct Cyc_Core_Opt* _temp469; struct Cyc_Core_Opt* _temp471; struct
Cyc_Core_Opt* _temp473; struct Cyc_Core_Opt _temp475; struct Cyc_List_List*
_temp476; struct Cyc_Core_Opt* _temp478; struct Cyc_Core_Opt _temp480; struct
Cyc_Absyn_Uniondecl** _temp481; struct Cyc_Core_Opt* _temp483; struct Cyc_Core_Opt
_temp485; struct Cyc_List_List* _temp486; struct Cyc_Core_Opt* _temp488; struct
Cyc_Core_Opt _temp490; struct Cyc_Absyn_Uniondecl** _temp491; struct Cyc_Core_Opt*
_temp493; _LL459: _LL470: _temp469= _temp457.f1; if( _temp469 == 0){ goto _LL468;}
else{ goto _LL461;} _LL468: _temp467= _temp457.f2; if( _temp467 == 0){ goto
_LL460;} else{ goto _LL461;} _LL461: _LL474: _temp473= _temp457.f1; if( _temp473
== 0){ goto _LL463;} else{ _temp475=* _temp473; _LL477: _temp476=( struct Cyc_List_List*)
_temp475.v; goto _LL472;} _LL472: _temp471= _temp457.f2; if( _temp471 == 0){
goto _LL462;} else{ goto _LL463;} _LL463: _LL484: _temp483= _temp457.f1; if(
_temp483 == 0){ goto _LL465;} else{ _temp485=* _temp483; _LL487: _temp486=(
struct Cyc_List_List*) _temp485.v; goto _LL479;} _LL479: _temp478= _temp457.f2;
if( _temp478 == 0){ goto _LL465;} else{ _temp480=* _temp478; _LL482: _temp481=(
struct Cyc_Absyn_Uniondecl**) _temp480.v; goto _LL464;} _LL465: _LL494: _temp493=
_temp457.f1; if( _temp493 == 0){ goto _LL489;} else{ goto _LL458;} _LL489:
_temp488= _temp457.f2; if( _temp488 == 0){ goto _LL458;} else{ _temp490=*
_temp488; _LL492: _temp491=( struct Cyc_Absyn_Uniondecl**) _temp490.v; goto
_LL466;} _LL460: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp452,({ struct Cyc_Absyn_Uniondecl** _temp495=( struct Cyc_Absyn_Uniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*)); _temp495[ 0]= ud; _temp495;}));
goto _LL458; _LL462: { struct Cyc_Absyn_Uniondecl** _temp498=({ struct Cyc_Absyn_Uniondecl**
_temp496=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp496[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp497=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp497->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp497->name= ud->name; _temp497->tvs= _temp454; _temp497->fields=
0; _temp497->attributes= ud->attributes; _temp497;}); _temp496;}); goto _LL499;
_LL499: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp452, _temp498); Cyc_Tc_tcStructFields( te, ge, loc, obj,
_temp476, _temp454);* _temp498= ud; goto _LL458;} _LL464: { struct Cyc_Absyn_Uniondecl*
_temp500=* _temp481; goto _LL501; _LL501:* _temp481=({ struct Cyc_Absyn_Uniondecl*
_temp502=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp502->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp502->name= ud->name;
_temp502->tvs= _temp454; _temp502->fields= 0; _temp502->attributes= ud->attributes;
_temp502;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp486, _temp454);*
_temp481= _temp500; _temp491= _temp481; goto _LL466;} _LL466: { struct Cyc_Absyn_Uniondecl*
_temp503= Cyc_Tcdecl_merge_uniondecl(* _temp491, ud, loc, Cyc_Tc_tc_msg); goto
_LL504; _LL504: if( _temp503 == 0){ return;} else{* _temp491=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp503); goto _LL458;}} _LL458:;}}}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp507= fields; goto _LL508; _LL508: for( 0; _temp507 != 0;
_temp507=(( struct Cyc_List_List*) _check_null( _temp507))->tl){ struct Cyc_Absyn_Tunionfield*
_temp509=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp507))->hd; goto _LL510; _LL510:{ struct Cyc_List_List* tvs= _temp509->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Tvar* _temp511=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd; goto _LL512; _LL512: { struct Cyc_Absyn_Conref* _temp513=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp511->kind); goto _LL514; _LL514: { void* _temp515=( void*) _temp513->v;
void* _temp523; _LL517: if( _temp515 ==( void*) Cyc_Absyn_No_constr){ goto
_LL518;} else{ goto _LL519;} _LL519: if(( unsigned int) _temp515 > 1u?*(( int*)
_temp515) == Cyc_Absyn_Eq_constr: 0){ _LL524: _temp523=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp515)->f1; if( _temp523 ==( void*) Cyc_Absyn_MemKind){ goto _LL520;} else{
goto _LL521;}} else{ goto _LL521;} _LL521: goto _LL522; _LL518:( void*)(
_temp513->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp525=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp525[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp526; _temp526.tag= Cyc_Absyn_Eq_constr;
_temp526.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp526;}); _temp525;})));
goto _LL516; _LL520: Cyc_Tcutil_terr( _temp509->loc,({ struct _tagged_string
_temp527=*(* _temp509->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp527.last_plus_one - _temp527.curr, _temp527.curr);})); goto _LL516; _LL522:
goto _LL516; _LL516:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp509->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List*
typs= _temp509->typs; for( 0; typs != 0; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl){ Cyc_Tcutil_check_type( _temp509->loc, te, alltvs,(
void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( typs))->hd)).f2);}}{ void* _temp528=(* _temp509->name).f1; struct
Cyc_List_List* _temp538; struct Cyc_List_List* _temp540; struct Cyc_List_List*
_temp542; _LL530: if(( unsigned int) _temp528 > 1u?*(( int*) _temp528) == Cyc_Absyn_Rel_n:
0){ _LL539: _temp538=(( struct Cyc_Absyn_Rel_n_struct*) _temp528)->f1; if(
_temp538 == 0){ goto _LL531;} else{ goto _LL532;}} else{ goto _LL532;} _LL532:
if(( unsigned int) _temp528 > 1u?*(( int*) _temp528) == Cyc_Absyn_Rel_n: 0){
_LL541: _temp540=(( struct Cyc_Absyn_Rel_n_struct*) _temp528)->f1; goto _LL533;}
else{ goto _LL534;} _LL534: if(( unsigned int) _temp528 > 1u?*(( int*) _temp528)
== Cyc_Absyn_Abs_n: 0){ _LL543: _temp542=(( struct Cyc_Absyn_Abs_n_struct*)
_temp528)->f1; goto _LL535;} else{ goto _LL536;} _LL536: if( _temp528 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL537;} else{ goto _LL529;} _LL531: if( is_xtunion){(*
_temp509->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp544=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp544[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp545; _temp545.tag= Cyc_Absyn_Abs_n;
_temp545.f1= te->ns; _temp545;}); _temp544;});} else{(* _temp509->name).f1=(*
name).f1;} goto _LL529; _LL533: Cyc_Tcutil_terr( _temp509->loc,({ char* _temp546="qualified declarations are not allowed";
struct _tagged_string _temp547; _temp547.curr= _temp546; _temp547.base= _temp546;
_temp547.last_plus_one= _temp546 + 39; _temp547;})); goto _LL529; _LL535: goto
_LL529; _LL537:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp548=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp548[ 0]=({ struct Cyc_Core_Impossible_struct _temp549; _temp549.tag= Cyc_Core_Impossible;
_temp549.f1=({ char* _temp550="tcTunionFields: Loc_n"; struct _tagged_string
_temp551; _temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 22; _temp551;}); _temp549;}); _temp548;})); goto _LL529; _LL529:;}}}}{
struct Cyc_List_List* fields2; if( is_xtunion){ int _temp552= 1; goto _LL553;
_LL553: { struct Cyc_List_List* _temp554= Cyc_Tcdecl_sort_xtunion_fields( fields,&
_temp552,(* name).f2, loc, Cyc_Tc_tc_msg); goto _LL555; _LL555: if( _temp552){
fields2= _temp554;} else{ fields2= 0;}}} else{ struct _RegionHandle _temp556=
_new_region(); struct _RegionHandle* uprev_rgn=& _temp556; _push_region(
uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=
fields; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct Cyc_Absyn_Tunionfield* _temp557=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( fs))->hd; goto _LL558; _LL558: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
_temp557->name).f2)){ Cyc_Tcutil_terr( _temp557->loc,({ struct _tagged_string
_temp559=*(* _temp557->name).f2; struct _tagged_string _temp560= obj; xprintf("duplicate field name %.*s in %.*s",
_temp559.last_plus_one - _temp559.curr, _temp559.curr, _temp560.last_plus_one -
_temp560.curr, _temp560.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp561=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp561->hd=( void*)(* _temp557->name).f2; _temp561->tl= prev_fields; _temp561;});}
if(( void*) _temp557->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp562=*(* _temp557->name).f2; xprintf("ignoring scope of field %.*s",
_temp562.last_plus_one - _temp562.curr, _temp562.curr);}));( void*)( _temp557->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp556);}{
struct Cyc_List_List* _temp563= fields; goto _LL564; _LL564: for( 0; _temp563 !=
0; _temp563=(( struct Cyc_List_List*) _check_null( _temp563))->tl){ struct Cyc_Absyn_Tunionfield*
_temp565=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp563))->hd; goto _LL566; _LL566: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp565->name).f2,({ struct _tuple3* _temp567=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp567->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp568=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp568[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp569; _temp569.tag= Cyc_Tcenv_TunionRes; _temp569.f1=
tudres; _temp569.f2= _temp565; _temp569;}); _temp568;}); _temp567->f2= 1;
_temp567;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=({ char* _temp570="xtunion"; struct _tagged_string
_temp571; _temp571.curr= _temp570; _temp571.base= _temp570; _temp571.last_plus_one=
_temp570 + 8; _temp571;});} else{ obj=({ char* _temp572="tunion"; struct
_tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 7; _temp573;});}{ struct Cyc_List_List* tvs=
tud->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct
Cyc_List_List*) _check_null( tvs2))->tl){ struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind);
void* _temp574=( void*) c->v; void* _temp582; _LL576: if( _temp574 ==( void*)
Cyc_Absyn_No_constr){ goto _LL577;} else{ goto _LL578;} _LL578: if((
unsigned int) _temp574 > 1u?*(( int*) _temp574) == Cyc_Absyn_Eq_constr: 0){
_LL583: _temp582=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp574)->f1;
if( _temp582 ==( void*) Cyc_Absyn_MemKind){ goto _LL579;} else{ goto _LL580;}}
else{ goto _LL580;} _LL580: goto _LL581; _LL577:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp584=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp584[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp585; _temp585.tag= Cyc_Absyn_Eq_constr; _temp585.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp585;}); _temp584;}))); goto _LL575;
_LL579: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp586= obj; struct
_tagged_string _temp587=* v; struct _tagged_string _temp588=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp586.last_plus_one - _temp586.curr, _temp586.curr, _temp587.last_plus_one -
_temp587.curr, _temp587.curr, _temp588.last_plus_one - _temp588.curr, _temp588.curr);}));
goto _LL575; _LL581: goto _LL575; _LL575:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp589; _push_handler(& _temp589);{ int _temp591= 0; if( setjmp( _temp589.handler)){
_temp591= 1;} if( ! _temp591){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp590=( void*) _exn_thrown; void*
_temp593= _temp590; _LL595: if( _temp593 == Cyc_Dict_Absent){ goto _LL596;}
else{ goto _LL597;} _LL597: goto _LL598; _LL596: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp599= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp599.last_plus_one - _temp599.curr, _temp599.curr);})); return; _LL598:(
void) _throw( _temp593); _LL594:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp600=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp600[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp601; _temp601.tag= Cyc_Absyn_Abs_n;
_temp601.f1= te->ns; _temp601;}); _temp600;});}} else{{ void* _temp602=(* tud->name).f1;
struct Cyc_List_List* _temp610; struct Cyc_List_List* _temp612; _LL604: if((
unsigned int) _temp602 > 1u?*(( int*) _temp602) == Cyc_Absyn_Rel_n: 0){ _LL611:
_temp610=(( struct Cyc_Absyn_Rel_n_struct*) _temp602)->f1; if( _temp610 == 0){
goto _LL605;} else{ goto _LL606;}} else{ goto _LL606;} _LL606: if(( unsigned int)
_temp602 > 1u?*(( int*) _temp602) == Cyc_Absyn_Abs_n: 0){ _LL613: _temp612=((
struct Cyc_Absyn_Abs_n_struct*) _temp602)->f1; goto _LL607;} else{ goto _LL608;}
_LL608: goto _LL609; _LL605:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp614=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp614[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp615; _temp615.tag= Cyc_Absyn_Abs_n;
_temp615.f1= te->ns; _temp615;}); _temp614;}); goto _LL603; _LL607: goto _LL609;
_LL609: Cyc_Tcutil_terr( loc,({ char* _temp616="qualified declarations are not implemented";
struct _tagged_string _temp617; _temp617.curr= _temp616; _temp617.base= _temp616;
_temp617.last_plus_one= _temp616 + 43; _temp617;})); return; _LL603:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp619=({ struct
_tuple4 _temp618; _temp618.f1= tud->fields; _temp618.f2= tud_opt; _temp618;});
struct Cyc_Core_Opt* _temp629; struct Cyc_Core_Opt* _temp631; struct Cyc_Core_Opt*
_temp633; struct Cyc_Core_Opt* _temp635; struct Cyc_Core_Opt _temp637; struct
Cyc_List_List* _temp638; struct Cyc_List_List** _temp640; struct Cyc_Core_Opt*
_temp641; struct Cyc_Core_Opt _temp643; struct Cyc_Absyn_Tuniondecl** _temp644;
struct Cyc_Core_Opt* _temp646; struct Cyc_Core_Opt _temp648; struct Cyc_List_List*
_temp649; struct Cyc_List_List** _temp651; struct Cyc_Core_Opt* _temp652; struct
Cyc_Core_Opt _temp654; struct Cyc_Absyn_Tuniondecl** _temp655; struct Cyc_Core_Opt*
_temp657; _LL621: _LL632: _temp631= _temp619.f1; if( _temp631 == 0){ goto _LL630;}
else{ goto _LL623;} _LL630: _temp629= _temp619.f2; if( _temp629 == 0){ goto
_LL622;} else{ goto _LL623;} _LL623: _LL636: _temp635= _temp619.f1; if( _temp635
== 0){ goto _LL625;} else{ _temp637=* _temp635; _LL639: _temp638=( struct Cyc_List_List*)
_temp637.v; _temp640=&(* _temp619.f1).v; goto _LL634;} _LL634: _temp633=
_temp619.f2; if( _temp633 == 0){ goto _LL624;} else{ goto _LL625;} _LL625:
_LL647: _temp646= _temp619.f1; if( _temp646 == 0){ goto _LL627;} else{ _temp648=*
_temp646; _LL650: _temp649=( struct Cyc_List_List*) _temp648.v; _temp651=&(*
_temp619.f1).v; goto _LL642;} _LL642: _temp641= _temp619.f2; if( _temp641 == 0){
goto _LL627;} else{ _temp643=* _temp641; _LL645: _temp644=( struct Cyc_Absyn_Tuniondecl**)
_temp643.v; goto _LL626;} _LL627: _LL658: _temp657= _temp619.f1; if( _temp657 ==
0){ goto _LL653;} else{ goto _LL620;} _LL653: _temp652= _temp619.f2; if(
_temp652 == 0){ goto _LL620;} else{ _temp654=* _temp652; _LL656: _temp655=(
struct Cyc_Absyn_Tuniondecl**) _temp654.v; goto _LL628;} _LL622: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp659=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp659[ 0]= tud; _temp659;}));
goto _LL620; _LL624: { struct Cyc_Absyn_Tuniondecl** _temp662=({ struct Cyc_Absyn_Tuniondecl**
_temp660=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp660[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp661=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp661->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp661->name= tud->name; _temp661->tvs= tvs; _temp661->fields=
0; _temp661->is_xtunion= tud->is_xtunion; _temp661;}); _temp660;}); goto _LL663;
_LL663: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp662);* _temp640= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp640, tvs, tud);* _temp662= tud; goto
_LL620;} _LL626: { struct Cyc_Absyn_Tuniondecl* _temp664=* _temp644; goto _LL665;
_LL665:* _temp644=({ struct Cyc_Absyn_Tuniondecl* _temp666=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp666->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp666->name= tud->name; _temp666->tvs= tvs; _temp666->fields=
0; _temp666->is_xtunion= tud->is_xtunion; _temp666;});* _temp651= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp651, tvs, tud);* _temp644=
_temp664; _temp655= _temp644; goto _LL628;} _LL628: { struct Cyc_Absyn_Tuniondecl*
_temp667= Cyc_Tcdecl_merge_tuniondecl(* _temp655, tud, loc, Cyc_Tc_tc_msg); goto
_LL668; _LL668: if( _temp667 == 0){ return;} else{* _temp655=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp667); goto _LL620;}} _LL620:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp669=(* ed->name).f1; struct Cyc_List_List* _temp677; struct Cyc_List_List*
_temp679; _LL671: if(( unsigned int) _temp669 > 1u?*(( int*) _temp669) == Cyc_Absyn_Rel_n:
0){ _LL678: _temp677=(( struct Cyc_Absyn_Rel_n_struct*) _temp669)->f1; if(
_temp677 == 0){ goto _LL672;} else{ goto _LL673;}} else{ goto _LL673;} _LL673:
if(( unsigned int) _temp669 > 1u?*(( int*) _temp669) == Cyc_Absyn_Abs_n: 0){
_LL680: _temp679=(( struct Cyc_Absyn_Abs_n_struct*) _temp669)->f1; if( _temp679
== 0){ goto _LL674;} else{ goto _LL675;}} else{ goto _LL675;} _LL675: goto
_LL676; _LL672: goto _LL670; _LL674: goto _LL670; _LL676: Cyc_Tcutil_terr( loc,({
char* _temp681="qualified declarations are not implemented"; struct
_tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 43; _temp682;})); return; _LL670:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp683=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp683[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp684; _temp684.tag= Cyc_Absyn_Abs_n; _temp684.f1= te->ns; _temp684;});
_temp683;}); if( ed->fields != 0){ struct _RegionHandle _temp685= _new_region();
struct _RegionHandle* uprev_rgn=& _temp685; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp686=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL687; _LL687: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp686->name).f2)){ Cyc_Tcutil_terr(
_temp686->loc,({ struct _tagged_string _temp688=*(* _temp686->name).f2; xprintf("duplicate field name %.*s",
_temp688.last_plus_one - _temp688.curr, _temp688.curr);}));} else{ prev_fields=({
struct Cyc_List_List* _temp689=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp689->hd=( void*)(* _temp686->name).f2;
_temp689->tl= prev_fields; _temp689;});} if( _temp686->tag == 0){ _temp686->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp686->loc);} else{ if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp686->tag))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp690=* v; struct
_tagged_string _temp691=*(* _temp686->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_temp690.last_plus_one - _temp690.curr, _temp690.curr, _temp691.last_plus_one -
_temp691.curr, _temp691.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp686->tag)); tag_count= t1 + 1;(*
_temp686->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp692=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp692[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp693; _temp693.tag= Cyc_Absyn_Abs_n;
_temp693.f1= te->ns; _temp693;}); _temp692;});}}}; _pop_region(& _temp685);}{
struct _handler_cons _temp694; _push_handler(& _temp694);{ int _temp696= 0; if(
setjmp( _temp694.handler)){ _temp696= 1;} if( ! _temp696){{ struct Cyc_Absyn_Enumdecl**
_temp697=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v); goto _LL698; _LL698: {
struct Cyc_Absyn_Enumdecl* _temp699= Cyc_Tcdecl_merge_enumdecl(* _temp697, ed,
loc, Cyc_Tc_tc_msg); goto _LL700; _LL700: if( _temp699 == 0){ _npop_handler( 0u);
return;}* _temp697=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp699);}};
_pop_handler();} else{ void* _temp695=( void*) _exn_thrown; void* _temp702=
_temp695; _LL704: if( _temp702 == Cyc_Dict_Absent){ goto _LL705;} else{ goto
_LL706;} _LL706: goto _LL707; _LL705: { struct Cyc_Absyn_Enumdecl** _temp709=({
struct Cyc_Absyn_Enumdecl** _temp708=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp708[ 0]= ed; _temp708;}); goto _LL710;
_LL710: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v, _temp709); goto _LL703;} _LL707:( void) _throw( _temp702);
_LL703:;}}} if( ed->fields != 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp711=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL712; _LL712: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp711->name).f2,({ struct _tuple3* _temp713=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp713->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp714=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp714[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp715; _temp715.tag= Cyc_Tcenv_EnumRes;
_temp715.f1= ed; _temp715.f2= _temp711; _temp715;}); _temp714;}); _temp713->f2=
1; _temp713;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp716= sc; _LL718: if( _temp716 ==( void*) Cyc_Absyn_Static){
goto _LL719;} else{ goto _LL720;} _LL720: if( _temp716 ==( void*) Cyc_Absyn_Abstract){
goto _LL721;} else{ goto _LL722;} _LL722: if( _temp716 ==( void*) Cyc_Absyn_Public){
goto _LL723;} else{ goto _LL724;} _LL724: if( _temp716 ==( void*) Cyc_Absyn_Extern){
goto _LL725;} else{ goto _LL726;} _LL726: if( _temp716 ==( void*) Cyc_Absyn_ExternC){
goto _LL727;} else{ goto _LL717;} _LL719: Cyc_Tcutil_warn( loc,({ char* _temp728="static declaration nested within extern \"C\"";
struct _tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 44; _temp729;})); return 0; _LL721: Cyc_Tcutil_warn(
loc,({ char* _temp730="abstract declaration nested within extern \"C\""; struct
_tagged_string _temp731; _temp731.curr= _temp730; _temp731.base= _temp730;
_temp731.last_plus_one= _temp730 + 46; _temp731;})); return 0; _LL723: return 1;
_LL725: return 1; _LL727: Cyc_Tcutil_warn( loc,({ char* _temp732="nested extern \"C\" declaration";
struct _tagged_string _temp733; _temp733.curr= _temp732; _temp733.base= _temp732;
_temp733.last_plus_one= _temp732 + 30; _temp733;})); return 1; _LL717:;} static
void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int
in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* _temp734= ds0; goto _LL735; _LL735: for( 0;
_temp734 != 0; _temp734=(( struct Cyc_List_List*) _check_null( _temp734))->tl){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp734))->hd; struct Cyc_Position_Segment* loc= d->loc; void*
_temp736=( void*) d->r; int _temp762; struct Cyc_Absyn_Exp* _temp764; struct Cyc_Core_Opt*
_temp766; struct Cyc_Core_Opt* _temp768; struct Cyc_Absyn_Pat* _temp770; struct
Cyc_List_List* _temp772; struct Cyc_Absyn_Vardecl* _temp774; struct Cyc_Absyn_Fndecl*
_temp776; struct Cyc_Absyn_Typedefdecl* _temp778; struct Cyc_Absyn_Structdecl*
_temp780; struct Cyc_Absyn_Uniondecl* _temp782; struct Cyc_Absyn_Tuniondecl*
_temp784; struct Cyc_Absyn_Enumdecl* _temp786; struct Cyc_List_List* _temp788;
struct _tagged_string* _temp790; struct Cyc_List_List* _temp792; struct _tuple0*
_temp794; struct _tuple0 _temp796; struct _tagged_string* _temp797; void*
_temp799; struct Cyc_List_List* _temp801; _LL738: if(*(( int*) _temp736) == Cyc_Absyn_Let_d){
_LL771: _temp770=(( struct Cyc_Absyn_Let_d_struct*) _temp736)->f1; goto _LL769;
_LL769: _temp768=(( struct Cyc_Absyn_Let_d_struct*) _temp736)->f2; goto _LL767;
_LL767: _temp766=(( struct Cyc_Absyn_Let_d_struct*) _temp736)->f3; goto _LL765;
_LL765: _temp764=(( struct Cyc_Absyn_Let_d_struct*) _temp736)->f4; goto _LL763;
_LL763: _temp762=(( struct Cyc_Absyn_Let_d_struct*) _temp736)->f5; goto _LL739;}
else{ goto _LL740;} _LL740: if(*(( int*) _temp736) == Cyc_Absyn_Letv_d){ _LL773:
_temp772=(( struct Cyc_Absyn_Letv_d_struct*) _temp736)->f1; goto _LL741;} else{
goto _LL742;} _LL742: if(*(( int*) _temp736) == Cyc_Absyn_Var_d){ _LL775:
_temp774=(( struct Cyc_Absyn_Var_d_struct*) _temp736)->f1; goto _LL743;} else{
goto _LL744;} _LL744: if(*(( int*) _temp736) == Cyc_Absyn_Fn_d){ _LL777:
_temp776=(( struct Cyc_Absyn_Fn_d_struct*) _temp736)->f1; goto _LL745;} else{
goto _LL746;} _LL746: if(*(( int*) _temp736) == Cyc_Absyn_Typedef_d){ _LL779:
_temp778=(( struct Cyc_Absyn_Typedef_d_struct*) _temp736)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: if(*(( int*) _temp736) == Cyc_Absyn_Struct_d){
_LL781: _temp780=(( struct Cyc_Absyn_Struct_d_struct*) _temp736)->f1; goto
_LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp736) == Cyc_Absyn_Union_d){
_LL783: _temp782=(( struct Cyc_Absyn_Union_d_struct*) _temp736)->f1; goto _LL751;}
else{ goto _LL752;} _LL752: if(*(( int*) _temp736) == Cyc_Absyn_Tunion_d){
_LL785: _temp784=(( struct Cyc_Absyn_Tunion_d_struct*) _temp736)->f1; goto
_LL753;} else{ goto _LL754;} _LL754: if(*(( int*) _temp736) == Cyc_Absyn_Enum_d){
_LL787: _temp786=(( struct Cyc_Absyn_Enum_d_struct*) _temp736)->f1; goto _LL755;}
else{ goto _LL756;} _LL756: if(*(( int*) _temp736) == Cyc_Absyn_Namespace_d){
_LL791: _temp790=(( struct Cyc_Absyn_Namespace_d_struct*) _temp736)->f1; goto
_LL789; _LL789: _temp788=(( struct Cyc_Absyn_Namespace_d_struct*) _temp736)->f2;
goto _LL757;} else{ goto _LL758;} _LL758: if(*(( int*) _temp736) == Cyc_Absyn_Using_d){
_LL795: _temp794=(( struct Cyc_Absyn_Using_d_struct*) _temp736)->f1; _temp796=*
_temp794; _LL800: _temp799= _temp796.f1; goto _LL798; _LL798: _temp797= _temp796.f2;
goto _LL793; _LL793: _temp792=(( struct Cyc_Absyn_Using_d_struct*) _temp736)->f2;
goto _LL759;} else{ goto _LL760;} _LL760: if(*(( int*) _temp736) == Cyc_Absyn_ExternC_d){
_LL802: _temp801=(( struct Cyc_Absyn_ExternC_d_struct*) _temp736)->f1; goto
_LL761;} else{ goto _LL737;} _LL739: Cyc_Tcutil_terr( loc,({ char* _temp803="top level let-declarations are not implemented";
struct _tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 47; _temp804;})); goto _LL737; _LL741: Cyc_Tcutil_terr(
loc,({ char* _temp805="top level let-declarations are not implemented"; struct
_tagged_string _temp806; _temp806.curr= _temp805; _temp806.base= _temp805;
_temp806.last_plus_one= _temp805 + 47; _temp806;})); goto _LL737; _LL743: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp774->sc): 0){( void*)(
_temp774->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp774, check_var_init); goto _LL737; _LL745: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp776->sc): 0){( void*)( _temp776->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp776); goto _LL737; _LL747: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp778); goto _LL737; _LL749: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp780->sc): 0){( void*)( _temp780->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp780); goto _LL737; _LL751: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp782->sc): 0){( void*)( _temp782->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp782);
goto _LL737; _LL753: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp784->sc): 0){( void*)( _temp784->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp784); goto _LL737; _LL755: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp786->sc): 0){( void*)( _temp786->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp786);
goto _LL737; _LL757: { struct Cyc_List_List* _temp807= te->ns; goto _LL808;
_LL808: { struct Cyc_List_List* _temp810=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp807,({ struct Cyc_List_List*
_temp809=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp809->hd=( void*) _temp790; _temp809->tl= 0; _temp809;})); goto _LL811;
_LL811: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge->namespaces, _temp790)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp790); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp810, Cyc_Tcenv_empty_genv());}
te->ns= _temp810; Cyc_Tc_tc_decls( te, _temp788, in_externC, check_var_init); te->ns=
_temp807; goto _LL737;}} _LL759: { struct _tagged_string* first; struct Cyc_List_List*
rest;{ void* _temp812= _temp799; struct Cyc_List_List* _temp824; struct Cyc_List_List*
_temp826; struct Cyc_List_List* _temp828; struct Cyc_List_List _temp830; struct
Cyc_List_List* _temp831; struct _tagged_string* _temp833; struct Cyc_List_List*
_temp835; struct Cyc_List_List _temp837; struct Cyc_List_List* _temp838; struct
_tagged_string* _temp840; _LL814: if( _temp812 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL815;} else{ goto _LL816;} _LL816: if(( unsigned int) _temp812 > 1u?*(( int*)
_temp812) == Cyc_Absyn_Rel_n: 0){ _LL825: _temp824=(( struct Cyc_Absyn_Rel_n_struct*)
_temp812)->f1; if( _temp824 == 0){ goto _LL817;} else{ goto _LL818;}} else{ goto
_LL818;} _LL818: if(( unsigned int) _temp812 > 1u?*(( int*) _temp812) == Cyc_Absyn_Abs_n:
0){ _LL827: _temp826=(( struct Cyc_Absyn_Abs_n_struct*) _temp812)->f1; if(
_temp826 == 0){ goto _LL819;} else{ goto _LL820;}} else{ goto _LL820;} _LL820:
if(( unsigned int) _temp812 > 1u?*(( int*) _temp812) == Cyc_Absyn_Rel_n: 0){
_LL829: _temp828=(( struct Cyc_Absyn_Rel_n_struct*) _temp812)->f1; if( _temp828
== 0){ goto _LL822;} else{ _temp830=* _temp828; _LL834: _temp833=( struct
_tagged_string*) _temp830.hd; goto _LL832; _LL832: _temp831= _temp830.tl; goto
_LL821;}} else{ goto _LL822;} _LL822: if(( unsigned int) _temp812 > 1u?*(( int*)
_temp812) == Cyc_Absyn_Abs_n: 0){ _LL836: _temp835=(( struct Cyc_Absyn_Abs_n_struct*)
_temp812)->f1; if( _temp835 == 0){ goto _LL813;} else{ _temp837=* _temp835;
_LL841: _temp840=( struct _tagged_string*) _temp837.hd; goto _LL839; _LL839:
_temp838= _temp837.tl; goto _LL823;}} else{ goto _LL813;} _LL815: goto _LL817;
_LL817: goto _LL819; _LL819: first= _temp797; rest= 0; goto _LL813; _LL821:
_temp840= _temp833; _temp838= _temp831; goto _LL823; _LL823: first= _temp840;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp838,({ struct Cyc_List_List* _temp842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp842->hd=( void*) _temp797;
_temp842->tl= 0; _temp842;})); goto _LL813; _LL813:;}{ struct Cyc_List_List*
_temp843= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); goto _LL844;
_LL844: ge->availables=({ struct Cyc_List_List* _temp845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp845->hd=( void*) _temp843;
_temp845->tl= ge->availables; _temp845;}); Cyc_Tc_tc_decls( te, _temp792,
in_externC, check_var_init); ge->availables=(( struct Cyc_List_List*)
_check_null( ge->availables))->tl; goto _LL737;}} _LL761: Cyc_Tc_tc_decls( te,
_temp801, 1, check_var_init); goto _LL737; _LL737:;}} static char _temp848[ 4u]="Cyc";
static struct _tagged_string Cyc_Tc_cyc_string={ _temp848, _temp848, _temp848 +
4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc(
struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int check_var_init, struct Cyc_List_List*
ds){ if( add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp849=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp849->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp850=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp850->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp851=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp851[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp852; _temp852.tag= Cyc_Absyn_Namespace_d;
_temp852.f1= Cyc_Tc_cyc_ns; _temp852.f2= ds; _temp852;}); _temp851;})); _temp850->loc=
0; _temp850;}); _temp849->tl= 0; _temp849;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t; static
int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d);
static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple6{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp853=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp863; struct Cyc_List_List* _temp865; struct Cyc_List_List**
_temp867; struct _tuple0* _temp868; struct Cyc_List_List* _temp870; struct Cyc_List_List**
_temp872; struct _tagged_string* _temp873; _LL855: if(*(( int*) _temp853) == Cyc_Absyn_Var_d){
_LL864: _temp863=(( struct Cyc_Absyn_Var_d_struct*) _temp853)->f1; goto _LL856;}
else{ goto _LL857;} _LL857: if(*(( int*) _temp853) == Cyc_Absyn_Using_d){ _LL869:
_temp868=(( struct Cyc_Absyn_Using_d_struct*) _temp853)->f1; goto _LL866; _LL866:
_temp865=(( struct Cyc_Absyn_Using_d_struct*) _temp853)->f2; _temp867=&(( struct
Cyc_Absyn_Using_d_struct*) _temp853)->f2; goto _LL858;} else{ goto _LL859;}
_LL859: if(*(( int*) _temp853) == Cyc_Absyn_Namespace_d){ _LL874: _temp873=((
struct Cyc_Absyn_Namespace_d_struct*) _temp853)->f1; goto _LL871; _LL871:
_temp870=(( struct Cyc_Absyn_Namespace_d_struct*) _temp853)->f2; _temp872=&((
struct Cyc_Absyn_Namespace_d_struct*) _temp853)->f2; goto _LL860;} else{ goto
_LL861;} _LL861: goto _LL862; _LL856: if(( void*) _temp863->sc !=( void*) Cyc_Absyn_Extern){
return 1;}{ struct _tuple0 _temp877; struct _tagged_string* _temp878; void*
_temp880; struct _tuple0* _temp875= _temp863->name; _temp877=* _temp875; _LL881:
_temp880= _temp877.f1; goto _LL879; _LL879: _temp878= _temp877.f2; goto _LL876;
_LL876: { struct Cyc_List_List* ns;{ void* _temp882= _temp880; struct Cyc_List_List*
_temp890; struct Cyc_List_List* _temp892; _LL884: if( _temp882 ==( void*) Cyc_Absyn_Loc_n){
goto _LL885;} else{ goto _LL886;} _LL886: if(( unsigned int) _temp882 > 1u?*((
int*) _temp882) == Cyc_Absyn_Rel_n: 0){ _LL891: _temp890=(( struct Cyc_Absyn_Rel_n_struct*)
_temp882)->f1; goto _LL887;} else{ goto _LL888;} _LL888: if(( unsigned int)
_temp882 > 1u?*(( int*) _temp882) == Cyc_Absyn_Abs_n: 0){ _LL893: _temp892=((
struct Cyc_Absyn_Abs_n_struct*) _temp882)->f1; goto _LL889;} else{ goto _LL883;}
_LL885: ns= 0; goto _LL883; _LL887: ns= _temp890; goto _LL883; _LL889: ns=
_temp892; goto _LL883; _LL883:;}{ struct _tuple6* _temp894=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
goto _LL895; _LL895: { struct Cyc_Tcenv_Genv* _temp896=(* _temp894).f1; goto
_LL897; _LL897: { int _temp898=(*(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp896->ordinaries, _temp878)).f2;
goto _LL899; _LL899: if( ! _temp898){(* _temp894).f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((* _temp894).f2,
_temp878);} return _temp898;}}}}} _LL858: _temp872= _temp867; goto _LL860;
_LL860:* _temp872= Cyc_Tc_treeshake_f( env,* _temp872); return 1; _LL862: return
1; _LL854:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set* set,
struct _tagged_string* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp900=(* nsenv).f1; goto _LL901; _LL901: { struct Cyc_Set_Set* _temp902=(*
nsenv).f2; goto _LL903; _LL903: _temp900->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple3*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
_temp902, _temp900->ordinaries);}} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp904=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp904->f1= ge; _temp904->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp904;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp905=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
goto _LL906; _LL906: { struct Cyc_List_List* _temp907= Cyc_Tc_treeshake_f(
_temp905, ds); goto _LL908; _LL908:(( void(*)( void(* f)( struct Cyc_List_List*,
struct _tuple6*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
_temp905); return _temp907;}}