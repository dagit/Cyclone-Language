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
extern char Cyc_Tcdecl_Incompatible[ 17u]; extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern void* Cyc_Tcdecl_merge_binding( void*
d0, void* d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg);
extern struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); static char _temp2[ 1u]=""; static struct _tagged_string
Cyc_Tc_tc_msg_c=( struct _tagged_string){ _temp2, _temp2, _temp2 + 1u}; static
struct _tagged_string* Cyc_Tc_tc_msg=( struct _tagged_string*)& Cyc_Tc_tc_msg_c;
static struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp3= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp9;
struct Cyc_List_List* _temp11; struct Cyc_List_List** _temp13; int _temp14;
struct Cyc_List_List* _temp16; void* _temp18; struct Cyc_Core_Opt* _temp20;
struct Cyc_List_List* _temp22; _LL5: if(( unsigned int) _temp3 > 4u?*(( int*)
_temp3) == Cyc_Absyn_FnType: 0){ _LL10: _temp9=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp3)->f1; _LL23: _temp22=( struct Cyc_List_List*)
_temp9.tvars; goto _LL21; _LL21: _temp20=( struct Cyc_Core_Opt*) _temp9.effect;
goto _LL19; _LL19: _temp18=( void*) _temp9.ret_typ; goto _LL17; _LL17: _temp16=(
struct Cyc_List_List*) _temp9.args; goto _LL15; _LL15: _temp14=( int) _temp9.varargs;
goto _LL12; _LL12: _temp11=( struct Cyc_List_List*) _temp9.attributes; _temp13=&(((
struct Cyc_Absyn_FnType_struct*) _temp3)->f1).attributes; goto _LL6;} else{ goto
_LL7;} _LL7: goto _LL8; _LL6: { struct Cyc_List_List* _temp24= 0; goto _LL25;
_LL25: for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp26= atts; if(
_temp26 == 0){ _throw( Null_Exception);} _temp26->tl;})){ void* _temp28=( void*)({
struct Cyc_List_List* _temp27= atts; if( _temp27 == 0){ _throw( Null_Exception);}
_temp27->hd;}); int _temp42; _LL30: if(( unsigned int) _temp28 > 15u?*(( int*)
_temp28) == Cyc_Absyn_Regparm_att: 0){ _LL43: _temp42=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp28)->f1; goto _LL31;} else{ goto _LL32;} _LL32: if( _temp28 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp28 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp28 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL37;} else{ goto _LL38;} _LL38: if( _temp28 ==( void*) Cyc_Absyn_Const_att){
goto _LL39;} else{ goto _LL40;} _LL40: goto _LL41; _LL31: goto _LL33; _LL33:
goto _LL35; _LL35: goto _LL37; _LL37: goto _LL39; _LL39:* _temp13=({ struct Cyc_List_List*
_temp44=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp44->hd=( void*)(( void*)({ struct Cyc_List_List* _temp45= atts; if( _temp45
== 0){ _throw( Null_Exception);} _temp45->hd;})); _temp44->tl=* _temp13; _temp44;});
continue; _LL41: _temp24=({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp46->hd=( void*)(( void*)({
struct Cyc_List_List* _temp47= atts; if( _temp47 == 0){ _throw( Null_Exception);}
_temp47->hd;})); _temp46->tl= _temp24; _temp46;}); continue; _LL29:;} return
_temp24;} _LL8: return(( struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp48=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 22; _temp49;})); _LL4:;} struct _tuple3{ void* f1; int f2; } ; static
void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl* vd, int
check_var_init){ struct _tagged_string* v=(* vd->name).f2; void* t=( void*) vd->type;
void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{ void*
_temp50=(* vd->name).f1; struct Cyc_List_List* _temp58; struct Cyc_List_List*
_temp60; _LL52: if(( unsigned int) _temp50 > 1u?*(( int*) _temp50) == Cyc_Absyn_Rel_n:
0){ _LL59: _temp58=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp50)->f1; if( _temp58 == 0){ goto _LL53;} else{ goto _LL54;}} else{ goto
_LL54;} _LL54: if(( unsigned int) _temp50 > 1u?*(( int*) _temp50) == Cyc_Absyn_Abs_n:
0){ _LL61: _temp60=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp50)->f1; if( _temp60 == 0){ goto _LL55;} else{ goto _LL56;}} else{ goto
_LL56;} _LL56: goto _LL57; _LL53: goto _LL51; _LL55: goto _LL51; _LL57: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp62=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp63; _temp63.curr= _temp62; _temp63.base= _temp62;
_temp63.last_plus_one= _temp62 + 43; _temp63;})); return; _LL51:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp64=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp64[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp65; _temp65.tag= Cyc_Absyn_Abs_n; _temp65.f1= te->ns; _temp65;}); _temp64;});{
void* _temp66= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp73; struct
Cyc_Absyn_Tqual _temp75; void* _temp77; _LL68: if(( unsigned int) _temp66 > 4u?*((
int*) _temp66) == Cyc_Absyn_ArrayType: 0){ _LL78: _temp77=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp66)->f1; goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp66)->f2; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp66)->f3; if( _temp73 == 0){ goto _LL72;} else{ goto _LL70;}} else{ goto
_LL70;} _LL72: if( vd->initializer != 0){ goto _LL69;} else{ goto _LL70;} _LL70:
goto _LL71; _LL69:{ void* _temp80=( void*)({ struct Cyc_Absyn_Exp* _temp79= vd->initializer;
if( _temp79 == 0){ _throw( Null_Exception);} _temp79->r;}); void* _temp90;
struct _tagged_string _temp92; struct Cyc_List_List* _temp94; struct Cyc_Core_Opt*
_temp96; struct Cyc_List_List* _temp98; _LL82: if(*(( int*) _temp80) == Cyc_Absyn_Const_e){
_LL91: _temp90=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp80)->f1; if((
unsigned int) _temp90 > 1u?*(( int*) _temp90) == Cyc_Absyn_String_c: 0){ _LL93:
_temp92=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp90)->f1;
goto _LL83;} else{ goto _LL84;}} else{ goto _LL84;} _LL84: if(*(( int*) _temp80)
== Cyc_Absyn_UnresolvedMem_e){ _LL97: _temp96=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp80)->f1; goto _LL95; _LL95: _temp94=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp80)->f2;
goto _LL85;} else{ goto _LL86;} _LL86: if(*(( int*) _temp80) == Cyc_Absyn_Array_e){
_LL99: _temp98=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp80)->f1; goto _LL87;} else{ goto _LL88;} _LL88: goto _LL89; _LL83: t=( void*)(
vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp100=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp100[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp101; _temp101.tag= Cyc_Absyn_ArrayType;
_temp101.f1=( void*) _temp77; _temp101.f2= _temp75; _temp101.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp102= _temp92;( unsigned int)(
_temp102.last_plus_one - _temp102.curr);}), 0); _temp101;}); _temp100;}))); goto
_LL81; _LL85: _temp98= _temp94; goto _LL87; _LL87: t=( void*)( vd->type=( void*)((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp103=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp103[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp104; _temp104.tag= Cyc_Absyn_ArrayType; _temp104.f1=(
void*) _temp77; _temp104.f2= _temp75; _temp104.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp98), 0);
_temp104;}); _temp103;}))); goto _LL81; _LL89: goto _LL81; _LL81:;} goto _LL67;
_LL71: goto _LL67; _LL67:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp105=( char*)"extern declaration should not have initializer"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 47; _temp106;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp107= atts; if(
_temp107 == 0){ _throw( Null_Exception);} _temp107->tl;})){ void* _temp109=(
void*)({ struct Cyc_List_List* _temp108= atts; if( _temp108 == 0){ _throw(
Null_Exception);} _temp108->hd;}); int _temp129; struct _tagged_string _temp131;
_LL111: if(( unsigned int) _temp109 > 15u?*(( int*) _temp109) == Cyc_Absyn_Aligned_att:
0){ _LL130: _temp129=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp109)->f1;
goto _LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp109 > 15u?*((
int*) _temp109) == Cyc_Absyn_Section_att: 0){ _LL132: _temp131=( struct
_tagged_string)(( struct Cyc_Absyn_Section_att_struct*) _temp109)->f1; goto
_LL114;} else{ goto _LL115;} _LL115: if( _temp109 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL116;} else{ goto _LL117;} _LL117: if( _temp109 ==( void*) Cyc_Absyn_Shared_att){
goto _LL118;} else{ goto _LL119;} _LL119: if( _temp109 ==( void*) Cyc_Absyn_Unused_att){
goto _LL120;} else{ goto _LL121;} _LL121: if( _temp109 ==( void*) Cyc_Absyn_Weak_att){
goto _LL122;} else{ goto _LL123;} _LL123: if( _temp109 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL124;} else{ goto _LL125;} _LL125: if( _temp109 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL112: continue; _LL114:
continue; _LL116: continue; _LL118: continue; _LL120: continue; _LL122:
continue; _LL124: continue; _LL126: continue; _LL128: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp134= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp133= atts; if( _temp133 == 0){ _throw( Null_Exception);} _temp133->hd;}));
struct _tagged_string _temp135= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp134.last_plus_one - _temp134.curr, _temp134.curr, _temp135.last_plus_one -
_temp135.curr, _temp135.curr);})); goto _LL110; _LL110:;} if( vd->initializer ==
0){ if( check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp136= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp137= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp136.last_plus_one - _temp136.curr, _temp136.curr, _temp137.last_plus_one -
_temp137.curr, _temp137.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp139=(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp138= vd->initializer; if(
_temp138 == 0){ _throw( Null_Exception);} _temp138;}); goto _LL140; _LL140: {
void* _temp141= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp139); goto
_LL142; _LL142: if( ! Cyc_Tcutil_coerce_assign( te, _temp139, t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp143= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp144= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp145= Cyc_Absynpp_typ2string( _temp141); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp143.last_plus_one - _temp143.curr, _temp143.curr, _temp144.last_plus_one -
_temp144.curr, _temp144.curr, _temp145.last_plus_one - _temp145.curr, _temp145.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, _temp139)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp146=( char*)"initializer is not a constant expression";
struct _tagged_string _temp147; _temp147.curr= _temp146; _temp147.base= _temp146;
_temp147.last_plus_one= _temp146 + 41; _temp147;}));}}}} else{ for( 0; atts != 0;
atts=({ struct Cyc_List_List* _temp148= atts; if( _temp148 == 0){ _throw(
Null_Exception);} _temp148->tl;})){ void* _temp150=( void*)({ struct Cyc_List_List*
_temp149= atts; if( _temp149 == 0){ _throw( Null_Exception);} _temp149->hd;});
int _temp168; int _temp170; _LL152: if(( unsigned int) _temp150 > 15u?*(( int*)
_temp150) == Cyc_Absyn_Regparm_att: 0){ _LL169: _temp168=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp150)->f1; goto _LL153;} else{ goto _LL154;} _LL154: if( _temp150 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL155;} else{ goto _LL156;} _LL156: if( _temp150
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL157;} else{ goto _LL158;} _LL158: if(
_temp150 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL159;} else{ goto _LL160;}
_LL160: if( _temp150 ==( void*) Cyc_Absyn_Const_att){ goto _LL161;} else{ goto
_LL162;} _LL162: if(( unsigned int) _temp150 > 15u?*(( int*) _temp150) == Cyc_Absyn_Aligned_att:
0){ _LL171: _temp170=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp150)->f1;
goto _LL163;} else{ goto _LL164;} _LL164: if( _temp150 ==( void*) Cyc_Absyn_Packed_att){
goto _LL165;} else{ goto _LL166;} _LL166: goto _LL167; _LL153: goto _LL155;
_LL155: goto _LL157; _LL157: goto _LL159; _LL159: goto _LL161; _LL161:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp172=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 51; _temp173;})); goto _LL151; _LL163: goto
_LL165; _LL165: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp175= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp174= atts; if( _temp174 == 0){ _throw(
Null_Exception);} _temp174->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp175.last_plus_one - _temp175.curr, _temp175.curr);})); goto _LL151; _LL167:
continue; _LL151:;}}}{ struct _handler_cons _temp176; _push_handler(& _temp176);{
int _temp178= 0; if( setjmp( _temp176.handler)){ _temp178= 1;} if( ! _temp178){{
struct _tuple3* _temp179=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL180; _LL180: {
void* _temp181=(* _temp179).f1; void* _temp187; _LL183: if(*(( int*) _temp181)
== Cyc_Tcenv_VarRes){ _LL188: _temp187=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp181)->f1; goto _LL184;} else{ goto _LL185;} _LL185: goto _LL186; _LL184: {
struct Cyc_Absyn_Global_b_struct* _temp191=({ struct Cyc_Absyn_Global_b_struct*
_temp189=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp189[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp190; _temp190.tag= Cyc_Absyn_Global_b;
_temp190.f1= vd; _temp190;}); _temp189;}); goto _LL192; _LL192: { void* _temp193=
Cyc_Tcdecl_merge_binding( _temp187,( void*) _temp191, loc, Cyc_Tc_tc_msg); goto
_LL194; _LL194: if( _temp193 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL182;}
if( _temp193 == _temp187?(* _temp179).f2: 0){ goto _LL182;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp195=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp195->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp196=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp196[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp197; _temp197.tag= Cyc_Tcenv_VarRes; _temp197.f1=( void*) _temp193;
_temp197;}); _temp196;}); _temp195->f2= 1; _temp195;})); goto _LL182;}} _LL186:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp198=( char*)"tcVardecl: a variable is being declared with a wrong name";
struct _tagged_string _temp199; _temp199.curr= _temp198; _temp199.base= _temp198;
_temp199.last_plus_one= _temp198 + 58; _temp199;})); goto _LL182; _LL182:;}};
_pop_handler();} else{ void* _temp177=( void*) _exn_thrown; void* _temp201=
_temp177; _LL203: if( _temp201 == Cyc_Dict_Absent){ goto _LL204;} else{ goto
_LL205;} _LL205: goto _LL206; _LL204: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp207=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp207->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp208=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp208[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp209; _temp209.tag= Cyc_Tcenv_VarRes;
_temp209.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp210=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp210[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp211; _temp211.tag= Cyc_Absyn_Global_b;
_temp211.f1= vd; _temp211;}); _temp210;})); _temp209;}); _temp208;}); _temp207->f2=
0; _temp207;})); goto _LL202; _LL206:( void) _throw( _temp201); _LL202:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp212=(*
fd->name).f1; struct Cyc_List_List* _temp220; struct Cyc_List_List* _temp222;
_LL214: if(( unsigned int) _temp212 > 1u?*(( int*) _temp212) == Cyc_Absyn_Rel_n:
0){ _LL221: _temp220=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp212)->f1; if( _temp220 == 0){ goto _LL215;} else{ goto _LL216;}} else{ goto
_LL216;} _LL216: if(( unsigned int) _temp212 > 1u?*(( int*) _temp212) == Cyc_Absyn_Abs_n:
0){ _LL223: _temp222=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp212)->f1; goto _LL217;} else{ goto _LL218;} _LL218: goto _LL219; _LL215:
goto _LL213; _LL217:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp224=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp224[ 0]=({ struct Cyc_Core_Impossible_struct _temp225; _temp225.tag= Cyc_Core_Impossible;
_temp225.f1=( struct _tagged_string)({ char* _temp226=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 22; _temp227;}); _temp225;}); _temp224;}));
_LL219: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp228=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 43; _temp229;})); return; _LL213:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp230=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp230[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp231; _temp231.tag= Cyc_Absyn_Abs_n; _temp231.f1= te->ns; _temp231;});
_temp230;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({
struct Cyc_List_List* _temp232= atts; if( _temp232 == 0){ _throw( Null_Exception);}
_temp232->tl;})){ void* _temp234=( void*)({ struct Cyc_List_List* _temp233= atts;
if( _temp233 == 0){ _throw( Null_Exception);} _temp233->hd;}); int _temp242;
_LL236: if( _temp234 ==( void*) Cyc_Absyn_Packed_att){ goto _LL237;} else{ goto
_LL238;} _LL238: if(( unsigned int) _temp234 > 15u?*(( int*) _temp234) == Cyc_Absyn_Aligned_att:
0){ _LL243: _temp242=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp234)->f1;
goto _LL239;} else{ goto _LL240;} _LL240: goto _LL241; _LL237: goto _LL239;
_LL239: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp245= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp244= atts; if( _temp244 == 0){ _throw(
Null_Exception);} _temp244->hd;})); xprintf("bad attribute %.*s for function",
_temp245.last_plus_one - _temp245.curr, _temp245.curr);})); goto _LL235; _LL241:
goto _LL235; _LL235:;}}{ struct _handler_cons _temp246; _push_handler(& _temp246);{
int _temp248= 0; if( setjmp( _temp246.handler)){ _temp248= 1;} if( ! _temp248){{
struct _tuple3* _temp249=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL250; _LL250: {
void* _temp251=(* _temp249).f1; void* _temp257; _LL253: if(*(( int*) _temp251)
== Cyc_Tcenv_VarRes){ _LL258: _temp257=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp251)->f1; goto _LL254;} else{ goto _LL255;} _LL255: goto _LL256; _LL254: {
struct Cyc_Absyn_Funname_b_struct* _temp261=({ struct Cyc_Absyn_Funname_b_struct*
_temp259=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp259[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp260; _temp260.tag= Cyc_Absyn_Funname_b;
_temp260.f1= fd; _temp260;}); _temp259;}); goto _LL262; _LL262: { void* _temp263=
Cyc_Tcdecl_merge_binding( _temp257,( void*) _temp261, loc, Cyc_Tc_tc_msg); goto
_LL264; _LL264: if( _temp263 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL252;}
if( _temp263 == _temp257?(* _temp249).f2: 0){ goto _LL252;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp265=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp265->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp266=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp266[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp267; _temp267.tag= Cyc_Tcenv_VarRes; _temp267.f1=( void*) _temp263;
_temp267;}); _temp266;}); _temp265->f2= 1; _temp265;})); goto _LL252;}} _LL256:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp268=( char*)"tcFndecl: a variable is being declared with a wrong name";
struct _tagged_string _temp269; _temp269.curr= _temp268; _temp269.base= _temp268;
_temp269.last_plus_one= _temp268 + 57; _temp269;})); goto _LL252; _LL252:;}};
_pop_handler();} else{ void* _temp247=( void*) _exn_thrown; void* _temp271=
_temp247; _LL273: if( _temp271 == Cyc_Dict_Absent){ goto _LL274;} else{ goto
_LL275;} _LL275: goto _LL276; _LL274: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp277=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp277->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp278=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp278[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp279; _temp279.tag= Cyc_Tcenv_VarRes;
_temp279.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp280=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp280[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp281; _temp281.tag= Cyc_Absyn_Funname_b;
_temp281.f1= fd; _temp281;}); _temp280;})); _temp279;}); _temp278;}); _temp277->f2=
0; _temp277;})); goto _LL272; _LL276:( void) _throw( _temp271); _LL272:;}}} te->le=({
struct Cyc_Core_Opt* _temp282=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp282->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp283=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp283[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp284; _temp284.tag= Cyc_Tcenv_Outermost;
_temp284.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp284;}); _temp283;}));
_temp282;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp285=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp286; _temp286.curr= _temp285; _temp286.base= _temp285;
_temp286.last_plus_one= _temp285 + 49; _temp286;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp287=(* td->name).f1; struct Cyc_List_List*
_temp295; struct Cyc_List_List* _temp297; _LL289: if(( unsigned int) _temp287 >
1u?*(( int*) _temp287) == Cyc_Absyn_Rel_n: 0){ _LL296: _temp295=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp287)->f1; if( _temp295 == 0){ goto _LL290;}
else{ goto _LL291;}} else{ goto _LL291;} _LL291: if(( unsigned int) _temp287 > 1u?*((
int*) _temp287) == Cyc_Absyn_Abs_n: 0){ _LL298: _temp297=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp287)->f1; if( _temp297 == 0){ goto _LL292;}
else{ goto _LL293;}} else{ goto _LL293;} _LL293: goto _LL294; _LL290: goto
_LL288; _LL292: goto _LL288; _LL294: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp299=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 43; _temp300;})); return; _LL288:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp301=* v; xprintf("redeclaration of typedef %.*s",
_temp301.last_plus_one - _temp301.curr, _temp301.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp302=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp302[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp303; _temp303.tag= Cyc_Absyn_Abs_n; _temp303.f1= te->ns; _temp303;});
_temp302;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp304= _new_region();
struct _RegionHandle* uprev_rgn=& _temp304; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp305= fields; goto
_LL306; _LL306: for( 0; _temp305 != 0; _temp305=({ struct Cyc_List_List*
_temp307= _temp305; if( _temp307 == 0){ _throw( Null_Exception);} _temp307->tl;})){
struct Cyc_Absyn_Structfield _temp311; struct Cyc_List_List* _temp312; struct
Cyc_Core_Opt* _temp314; void* _temp316; struct Cyc_Absyn_Tqual _temp318; struct
_tagged_string* _temp320; struct Cyc_Absyn_Structfield* _temp309=( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp308= _temp305; if( _temp308 == 0){ _throw(
Null_Exception);} _temp308->hd;}); _temp311=* _temp309; _LL321: _temp320=(
struct _tagged_string*) _temp311.name; goto _LL319; _LL319: _temp318=( struct
Cyc_Absyn_Tqual) _temp311.tq; goto _LL317; _LL317: _temp316=( void*) _temp311.type;
goto _LL315; _LL315: _temp314=( struct Cyc_Core_Opt*) _temp311.width; goto
_LL313; _LL313: _temp312=( struct Cyc_List_List*) _temp311.attributes; goto
_LL310; _LL310: if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp320)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp322=* _temp320; struct _tagged_string _temp323= obj; xprintf("duplicate field %.*s in %.*s",
_temp322.last_plus_one - _temp322.curr, _temp322.curr, _temp323.last_plus_one -
_temp323.curr, _temp323.curr);}));} if( Cyc_String_strcmp(* _temp320,( struct
_tagged_string)({ char* _temp324=( char*)""; struct _tagged_string _temp325;
_temp325.curr= _temp324; _temp325.base= _temp324; _temp325.last_plus_one=
_temp324 + 1; _temp325;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp326=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp326->hd=( void*) _temp320; _temp326->tl= prev_fields; _temp326;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp316); if( _temp314 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp327= _temp314; if( _temp327 == 0){ _throw(
Null_Exception);} _temp327->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp328=* _temp320; xprintf("bitfield %.*s does not have constant width",
_temp328.last_plus_one - _temp328.curr, _temp328.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp329= _temp314; if( _temp329
== 0){ _throw( Null_Exception);} _temp329->v;}));}{ void* _temp330= Cyc_Tcutil_compress(
_temp316); void* _temp336; void* _temp338; _LL332: if(( unsigned int) _temp330 >
4u?*(( int*) _temp330) == Cyc_Absyn_IntType: 0){ _LL339: _temp338=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp330)->f1; goto _LL337; _LL337: _temp336=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp330)->f2; goto _LL333;} else{
goto _LL334;} _LL334: goto _LL335; _LL333:{ void* _temp340= _temp336; _LL342:
if( _temp340 ==( void*) Cyc_Absyn_B1){ goto _LL343;} else{ goto _LL344;} _LL344:
if( _temp340 ==( void*) Cyc_Absyn_B2){ goto _LL345;} else{ goto _LL346;} _LL346:
if( _temp340 ==( void*) Cyc_Absyn_B4){ goto _LL347;} else{ goto _LL348;} _LL348:
if( _temp340 ==( void*) Cyc_Absyn_B8){ goto _LL349;} else{ goto _LL341;} _LL343:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp350=(
char*)"bitfield larger than type"; struct _tagged_string _temp351; _temp351.curr=
_temp350; _temp351.base= _temp350; _temp351.last_plus_one= _temp350 + 26;
_temp351;}));} goto _LL341; _LL345: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp352=( char*)"bitfield larger than type"; struct
_tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 26; _temp353;}));} goto _LL341; _LL347: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp354=( char*)"bitfield larger than type";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 26; _temp355;}));} goto _LL341; _LL349: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp356=( char*)"bitfield larger than type";
struct _tagged_string _temp357; _temp357.curr= _temp356; _temp357.base= _temp356;
_temp357.last_plus_one= _temp356 + 26; _temp357;}));} goto _LL341; _LL341:;}
goto _LL331; _LL335: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp358=*
_temp320; struct _tagged_string _temp359= Cyc_Absynpp_typ2string( _temp316);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp358.last_plus_one
- _temp358.curr, _temp358.curr, _temp359.last_plus_one - _temp359.curr, _temp359.curr);}));
goto _LL331; _LL331:;}}}}; _pop_region(& _temp304);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* _temp361=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp360=
sd->name; if( _temp360 == 0){ _throw( Null_Exception);} _temp360->v;}))).f2;
goto _LL362; _LL362: if( sd->name == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp363= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp363.last_plus_one - _temp363.curr, _temp363.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp364=
atts; if( _temp364 == 0){ _throw( Null_Exception);} _temp364->tl;})){ void*
_temp366=( void*)({ struct Cyc_List_List* _temp365= atts; if( _temp365 == 0){
_throw( Null_Exception);} _temp365->hd;}); int _temp374; _LL368: if( _temp366 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL369;} else{ goto _LL370;} _LL370: if((
unsigned int) _temp366 > 15u?*(( int*) _temp366) == Cyc_Absyn_Aligned_att: 0){
_LL375: _temp374=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp366)->f1;
goto _LL371;} else{ goto _LL372;} _LL372: goto _LL373; _LL369: continue; _LL371:
continue; _LL373: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp377= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp376= atts; if( _temp376 == 0){ _throw(
Null_Exception);} _temp376->hd;})); struct _tagged_string _temp378= obj; struct
_tagged_string _temp379=* _temp361; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp377.last_plus_one - _temp377.curr, _temp377.curr, _temp378.last_plus_one -
_temp378.curr, _temp378.curr, _temp379.last_plus_one - _temp379.curr, _temp379.curr);}));
goto _LL367; _LL367:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=({ struct Cyc_List_List* _temp380= tvs; if( _temp380 == 0){ _throw(
Null_Exception);} _temp380->tl;})){ struct Cyc_Absyn_Conref* _temp382=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp381= tvs; if( _temp381 == 0){
_throw( Null_Exception);} _temp381->hd;}))->kind); goto _LL383; _LL383: { void*
_temp384=( void*) _temp382->v; void* _temp392; _LL386: if( _temp384 ==( void*)
Cyc_Absyn_No_constr){ goto _LL387;} else{ goto _LL388;} _LL388: if((
unsigned int) _temp384 > 1u?*(( int*) _temp384) == Cyc_Absyn_Eq_constr: 0){
_LL393: _temp392=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp384)->f1;
if( _temp392 ==( void*) Cyc_Absyn_MemKind){ goto _LL389;} else{ goto _LL390;}}
else{ goto _LL390;} _LL390: goto _LL391; _LL387:( void*)( _temp382->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp394=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp394[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp395; _temp395.tag= Cyc_Absyn_Eq_constr; _temp395.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp395;}); _temp394;}))); continue; _LL389:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp397= obj; struct
_tagged_string _temp398=* _temp361; struct _tagged_string _temp399=*(( struct
Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp396= tvs; if( _temp396 == 0){
_throw( Null_Exception);} _temp396->hd;}))->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp397.last_plus_one - _temp397.curr, _temp397.curr, _temp398.last_plus_one -
_temp398.curr, _temp398.curr, _temp399.last_plus_one - _temp399.curr, _temp399.curr);}));
continue; _LL391: continue; _LL385:;}}}{ void* _temp401=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp400= sd->name; if( _temp400 == 0){ _throw(
Null_Exception);} _temp400->v;}))).f1; struct Cyc_List_List* _temp409; struct
Cyc_List_List* _temp411; _LL403: if(( unsigned int) _temp401 > 1u?*(( int*)
_temp401) == Cyc_Absyn_Rel_n: 0){ _LL410: _temp409=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp401)->f1; if( _temp409 == 0){ goto _LL404;}
else{ goto _LL405;}} else{ goto _LL405;} _LL405: if(( unsigned int) _temp401 > 1u?*((
int*) _temp401) == Cyc_Absyn_Abs_n: 0){ _LL412: _temp411=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp401)->f1; if( _temp411 == 0){ goto _LL406;}
else{ goto _LL407;}} else{ goto _LL407;} _LL407: goto _LL408; _LL404: goto
_LL402; _LL406: goto _LL402; _LL408: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp413=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 43; _temp414;})); return; _LL402:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp415= sd->name; if( _temp415 == 0){
_throw( Null_Exception);} _temp415->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp416=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp416[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp417; _temp417.tag= Cyc_Absyn_Abs_n;
_temp417.f1= te->ns; _temp417;}); _temp416;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* _temp419=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp418=
sd->name; if( _temp418 == 0){ _throw( Null_Exception);} _temp418->v;}))).f2;
goto _LL420; _LL420: { struct _tagged_string obj=( struct _tagged_string)({ char*
_temp477=( char*)"struct"; struct _tagged_string _temp478; _temp478.curr=
_temp477; _temp478.base= _temp477; _temp478.last_plus_one= _temp477 + 7;
_temp478;}); Cyc_Tc_tcStructOrUniondecl( te, ge, obj, loc, sd);{ struct Cyc_List_List*
_temp421= sd->tvs; goto _LL422; _LL422:{ struct _tuple4 _temp424=({ struct
_tuple4 _temp423; _temp423.f1= sd->fields; _temp423.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls,
_temp419); _temp423;}); struct Cyc_Core_Opt* _temp434; struct Cyc_Core_Opt*
_temp436; struct Cyc_Core_Opt* _temp438; struct Cyc_Core_Opt* _temp440; struct
Cyc_Core_Opt _temp442; struct Cyc_List_List* _temp443; struct Cyc_Core_Opt*
_temp445; struct Cyc_Core_Opt _temp447; struct Cyc_Absyn_Structdecl** _temp448;
struct Cyc_Core_Opt* _temp450; struct Cyc_Core_Opt _temp452; struct Cyc_List_List*
_temp453; struct Cyc_Core_Opt* _temp455; struct Cyc_Core_Opt _temp457; struct
Cyc_Absyn_Structdecl** _temp458; struct Cyc_Core_Opt* _temp460; _LL426: _LL437:
_temp436= _temp424.f1; if( _temp436 == 0){ goto _LL435;} else{ goto _LL428;}
_LL435: _temp434= _temp424.f2; if( _temp434 == 0){ goto _LL427;} else{ goto
_LL428;} _LL428: _LL441: _temp440= _temp424.f1; if( _temp440 == 0){ goto _LL430;}
else{ _temp442=* _temp440; _LL444: _temp443=( struct Cyc_List_List*) _temp442.v;
goto _LL439;} _LL439: _temp438= _temp424.f2; if( _temp438 == 0){ goto _LL429;}
else{ goto _LL430;} _LL430: _LL451: _temp450= _temp424.f1; if( _temp450 == 0){
goto _LL432;} else{ _temp452=* _temp450; _LL454: _temp453=( struct Cyc_List_List*)
_temp452.v; goto _LL446;} _LL446: _temp445= _temp424.f2; if( _temp445 == 0){
goto _LL432;} else{ _temp447=* _temp445; _LL449: _temp448=( struct Cyc_Absyn_Structdecl**)
_temp447.v; goto _LL431;} _LL432: _LL461: _temp460= _temp424.f1; if( _temp460 ==
0){ goto _LL456;} else{ goto _LL425;} _LL456: _temp455= _temp424.f2; if(
_temp455 == 0){ goto _LL425;} else{ _temp457=* _temp455; _LL459: _temp458=(
struct Cyc_Absyn_Structdecl**) _temp457.v; goto _LL433;} _LL427: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)( ge->structdecls, _temp419,({
struct Cyc_Absyn_Structdecl** _temp462=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp462[ 0]= sd; _temp462;}));
goto _LL425; _LL429: { struct Cyc_Absyn_Structdecl** _temp465=({ struct Cyc_Absyn_Structdecl**
_temp463=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp463[ 0]=({ struct Cyc_Absyn_Structdecl* _temp464=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp464->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp464->name= sd->name; _temp464->tvs= _temp421; _temp464->fields=
0; _temp464->attributes= 0; _temp464;}); _temp463;}); goto _LL466; _LL466: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)( ge->structdecls, _temp419,
_temp465); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp443, _temp421);*
_temp465= sd; goto _LL425;} _LL431: { struct Cyc_Absyn_Structdecl* _temp467=*
_temp448; goto _LL468; _LL468:* _temp448=({ struct Cyc_Absyn_Structdecl*
_temp469=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp469->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp469->name= sd->name;
_temp469->tvs= _temp421; _temp469->fields= 0; _temp469->attributes= 0; _temp469;});
Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp453, _temp421);* _temp448=
_temp467; _temp458= _temp448; goto _LL433;} _LL433: { struct Cyc_Absyn_Structdecl*
_temp470= Cyc_Tcdecl_merge_structdecl(* _temp458, sd, loc, Cyc_Tc_tc_msg); goto
_LL471; _LL471: if( _temp470 == 0){ return;} else{* _temp458=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp472= _temp470; if( _temp472 == 0){ _throw(
Null_Exception);} _temp472;}); sd=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp473= _temp470; if( _temp473 == 0){ _throw( Null_Exception);} _temp473;});
goto _LL425;}} _LL425:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp419,({ struct _tuple3* _temp474=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp474->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp475=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp475[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp476; _temp476.tag= Cyc_Tcenv_StructRes;
_temp476.f1= sd; _temp476;}); _temp475;}); _temp474->f2= 1; _temp474;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* _temp480=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp479=
ud->name; if( _temp479 == 0){ _throw( Null_Exception);} _temp479->v;}))).f2;
goto _LL481; _LL481: { struct _tagged_string obj=( struct _tagged_string)({ char*
_temp534=( char*)"union"; struct _tagged_string _temp535; _temp535.curr=
_temp534; _temp535.base= _temp534; _temp535.last_plus_one= _temp534 + 6;
_temp535;}); Cyc_Tc_tcStructOrUniondecl( te, ge, obj, loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp482= ud->tvs; goto _LL483; _LL483: { struct
_tuple4 _temp485=({ struct _tuple4 _temp484; _temp484.f1= ud->fields; _temp484.f2=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->uniondecls, _temp480); _temp484;}); struct Cyc_Core_Opt*
_temp495; struct Cyc_Core_Opt* _temp497; struct Cyc_Core_Opt* _temp499; struct
Cyc_Core_Opt* _temp501; struct Cyc_Core_Opt _temp503; struct Cyc_List_List*
_temp504; struct Cyc_Core_Opt* _temp506; struct Cyc_Core_Opt _temp508; struct
Cyc_Absyn_Uniondecl** _temp509; struct Cyc_Core_Opt* _temp511; struct Cyc_Core_Opt
_temp513; struct Cyc_List_List* _temp514; struct Cyc_Core_Opt* _temp516; struct
Cyc_Core_Opt _temp518; struct Cyc_Absyn_Uniondecl** _temp519; struct Cyc_Core_Opt*
_temp521; _LL487: _LL498: _temp497= _temp485.f1; if( _temp497 == 0){ goto _LL496;}
else{ goto _LL489;} _LL496: _temp495= _temp485.f2; if( _temp495 == 0){ goto
_LL488;} else{ goto _LL489;} _LL489: _LL502: _temp501= _temp485.f1; if( _temp501
== 0){ goto _LL491;} else{ _temp503=* _temp501; _LL505: _temp504=( struct Cyc_List_List*)
_temp503.v; goto _LL500;} _LL500: _temp499= _temp485.f2; if( _temp499 == 0){
goto _LL490;} else{ goto _LL491;} _LL491: _LL512: _temp511= _temp485.f1; if(
_temp511 == 0){ goto _LL493;} else{ _temp513=* _temp511; _LL515: _temp514=(
struct Cyc_List_List*) _temp513.v; goto _LL507;} _LL507: _temp506= _temp485.f2;
if( _temp506 == 0){ goto _LL493;} else{ _temp508=* _temp506; _LL510: _temp509=(
struct Cyc_Absyn_Uniondecl**) _temp508.v; goto _LL492;} _LL493: _LL522: _temp521=
_temp485.f1; if( _temp521 == 0){ goto _LL517;} else{ goto _LL486;} _LL517:
_temp516= _temp485.f2; if( _temp516 == 0){ goto _LL486;} else{ _temp518=*
_temp516; _LL520: _temp519=( struct Cyc_Absyn_Uniondecl**) _temp518.v; goto
_LL494;} _LL488: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp480,({ struct Cyc_Absyn_Uniondecl** _temp523=( struct Cyc_Absyn_Uniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*)); _temp523[ 0]= ud; _temp523;}));
goto _LL486; _LL490: { struct Cyc_Absyn_Uniondecl** _temp526=({ struct Cyc_Absyn_Uniondecl**
_temp524=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp524[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp525=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp525->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp525->name= ud->name; _temp525->tvs= _temp482; _temp525->fields=
0; _temp525->attributes= ud->attributes; _temp525;}); _temp524;}); goto _LL527;
_LL527: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp480, _temp526); Cyc_Tc_tcStructFields( te, ge, loc, obj,
_temp504, _temp482);* _temp526= ud; goto _LL486;} _LL492: { struct Cyc_Absyn_Uniondecl*
_temp528=* _temp509; goto _LL529; _LL529:* _temp509=({ struct Cyc_Absyn_Uniondecl*
_temp530=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp530->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp530->name= ud->name;
_temp530->tvs= _temp482; _temp530->fields= 0; _temp530->attributes= ud->attributes;
_temp530;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp514, _temp482);*
_temp509= _temp528; _temp519= _temp509; goto _LL494;} _LL494: { struct Cyc_Absyn_Uniondecl*
_temp531= Cyc_Tcdecl_merge_uniondecl(* _temp519, ud, loc, Cyc_Tc_tc_msg); goto
_LL532; _LL532: if( _temp531 == 0){ return;} else{* _temp519=( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Absyn_Uniondecl* _temp533= _temp531; if( _temp533 == 0){ _throw(
Null_Exception);} _temp533;}); goto _LL486;}} _LL486:;}}}} struct _tuple5{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct _tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp536= fields; goto _LL537; _LL537: for( 0; _temp536 != 0;
_temp536=({ struct Cyc_List_List* _temp538= _temp536; if( _temp538 == 0){ _throw(
Null_Exception);} _temp538->tl;})){ struct Cyc_Absyn_Tunionfield* _temp540=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp539= _temp536; if(
_temp539 == 0){ _throw( Null_Exception);} _temp539->hd;}); goto _LL541; _LL541:{
struct Cyc_List_List* tvs= _temp540->tvs; for( 0; tvs != 0; tvs=({ struct Cyc_List_List*
_temp542= tvs; if( _temp542 == 0){ _throw( Null_Exception);} _temp542->tl;})){
struct Cyc_Absyn_Tvar* _temp544=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp543= tvs; if( _temp543 == 0){ _throw( Null_Exception);} _temp543->hd;});
goto _LL545; _LL545: { struct Cyc_Absyn_Conref* _temp546=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp544->kind); goto
_LL547; _LL547: { void* _temp548=( void*) _temp546->v; void* _temp556; _LL550:
if( _temp548 ==( void*) Cyc_Absyn_No_constr){ goto _LL551;} else{ goto _LL552;}
_LL552: if(( unsigned int) _temp548 > 1u?*(( int*) _temp548) == Cyc_Absyn_Eq_constr:
0){ _LL557: _temp556=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp548)->f1;
if( _temp556 ==( void*) Cyc_Absyn_MemKind){ goto _LL553;} else{ goto _LL554;}}
else{ goto _LL554;} _LL554: goto _LL555; _LL551:( void*)( _temp546->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp558=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp558[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp559; _temp559.tag= Cyc_Absyn_Eq_constr; _temp559.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp559;}); _temp558;}))); goto _LL549;
_LL553: Cyc_Tcutil_terr( _temp540->loc,({ struct _tagged_string _temp560=*(*
_temp540->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp560.last_plus_one - _temp560.curr, _temp560.curr);})); goto _LL549; _LL555:
goto _LL549; _LL549:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp540->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List*
typs= _temp540->typs; for( 0; typs != 0; typs=({ struct Cyc_List_List* _temp561=
typs; if( _temp561 == 0){ _throw( Null_Exception);} _temp561->tl;})){ Cyc_Tcutil_check_type(
_temp540->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)({
struct Cyc_List_List* _temp562= typs; if( _temp562 == 0){ _throw( Null_Exception);}
_temp562->hd;}))).f2);}}{ void* _temp563=(* _temp540->name).f1; struct Cyc_List_List*
_temp573; struct Cyc_List_List* _temp575; struct Cyc_List_List* _temp577; _LL565:
if(( unsigned int) _temp563 > 1u?*(( int*) _temp563) == Cyc_Absyn_Rel_n: 0){
_LL574: _temp573=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp563)->f1; if( _temp573 == 0){ goto _LL566;} else{ goto _LL567;}} else{ goto
_LL567;} _LL567: if(( unsigned int) _temp563 > 1u?*(( int*) _temp563) == Cyc_Absyn_Rel_n:
0){ _LL576: _temp575=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp563)->f1; goto _LL568;} else{ goto _LL569;} _LL569: if(( unsigned int)
_temp563 > 1u?*(( int*) _temp563) == Cyc_Absyn_Abs_n: 0){ _LL578: _temp577=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp563)->f1; goto
_LL570;} else{ goto _LL571;} _LL571: if( _temp563 ==( void*) Cyc_Absyn_Loc_n){
goto _LL572;} else{ goto _LL564;} _LL566: if( is_xtunion){(* _temp540->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp579=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp579[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp580; _temp580.tag= Cyc_Absyn_Abs_n; _temp580.f1= te->ns; _temp580;});
_temp579;});} else{(* _temp540->name).f1=(* name).f1;} goto _LL564; _LL568: Cyc_Tcutil_terr(
_temp540->loc,( struct _tagged_string)({ char* _temp581=( char*)"qualified declarations are not allowed";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 39; _temp582;})); goto _LL564; _LL570: goto
_LL564; _LL572:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp583=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp583[ 0]=({ struct Cyc_Core_Impossible_struct _temp584; _temp584.tag= Cyc_Core_Impossible;
_temp584.f1=( struct _tagged_string)({ char* _temp585=( char*)"tcTunionFields: Loc_n";
struct _tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 22; _temp586;}); _temp584;}); _temp583;}));
goto _LL564; _LL564:;}}}}{ struct Cyc_List_List* fields2; if( is_xtunion){ int
_temp587= 1; goto _LL588; _LL588: { struct Cyc_List_List* _temp589= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp587,(* name).f2, loc, Cyc_Tc_tc_msg); goto _LL590; _LL590: if(
_temp587){ fields2= _temp589;} else{ fields2= 0;}}} else{ struct _RegionHandle
_temp591= _new_region(); struct _RegionHandle* uprev_rgn=& _temp591;
_push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List*
fs= fields; for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp592= fs; if(
_temp592 == 0){ _throw( Null_Exception);} _temp592->tl;})){ struct Cyc_Absyn_Tunionfield*
_temp594=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp593= fs;
if( _temp593 == 0){ _throw( Null_Exception);} _temp593->hd;}); goto _LL595;
_LL595: if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp594->name).f2)){ Cyc_Tcutil_terr(
_temp594->loc,({ struct _tagged_string _temp596=*(* _temp594->name).f2; struct
_tagged_string _temp597= obj; xprintf("duplicate field name %.*s in %.*s",
_temp596.last_plus_one - _temp596.curr, _temp596.curr, _temp597.last_plus_one -
_temp597.curr, _temp597.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp598=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp598->hd=( void*)(* _temp594->name).f2; _temp598->tl= prev_fields; _temp598;});}
if(( void*) _temp594->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp599=*(* _temp594->name).f2; xprintf("ignoring scope of field %.*s",
_temp599.last_plus_one - _temp599.curr, _temp599.curr);}));( void*)( _temp594->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp591);}{
struct Cyc_List_List* _temp600= fields; goto _LL601; _LL601: for( 0; _temp600 !=
0; _temp600=({ struct Cyc_List_List* _temp602= _temp600; if( _temp602 == 0){
_throw( Null_Exception);} _temp602->tl;})){ struct Cyc_Absyn_Tunionfield*
_temp604=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp603=
_temp600; if( _temp603 == 0){ _throw( Null_Exception);} _temp603->hd;}); goto
_LL605; _LL605: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp604->name).f2,({ struct _tuple3* _temp606=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp606->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp607=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp607[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp608; _temp608.tag= Cyc_Tcenv_TunionRes;
_temp608.f1= tudres; _temp608.f2= _temp604; _temp608;}); _temp607;}); _temp606->f2=
1; _temp606;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=( struct _tagged_string)({ char* _temp609=( char*)"xtunion";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 8; _temp610;});} else{ obj=( struct
_tagged_string)({ char* _temp611=( char*)"tunion"; struct _tagged_string
_temp612; _temp612.curr= _temp611; _temp612.base= _temp611; _temp612.last_plus_one=
_temp611 + 7; _temp612;});}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp613= tvs2; if(
_temp613 == 0){ _throw( Null_Exception);} _temp613->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp630= tvs2; if( _temp630 ==
0){ _throw( Null_Exception);} _temp630->hd;}))->kind); void* _temp614=( void*) c->v;
void* _temp622; _LL616: if( _temp614 ==( void*) Cyc_Absyn_No_constr){ goto
_LL617;} else{ goto _LL618;} _LL618: if(( unsigned int) _temp614 > 1u?*(( int*)
_temp614) == Cyc_Absyn_Eq_constr: 0){ _LL623: _temp622=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp614)->f1; if( _temp622 ==( void*) Cyc_Absyn_MemKind){ goto _LL619;} else{
goto _LL620;}} else{ goto _LL620;} _LL620: goto _LL621; _LL617:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp624=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp624[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp625; _temp625.tag= Cyc_Absyn_Eq_constr; _temp625.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp625;}); _temp624;}))); goto _LL615;
_LL619: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp627= obj; struct
_tagged_string _temp628=* v; struct _tagged_string _temp629=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp626= tvs2; if( _temp626 == 0){ _throw( Null_Exception);}
_temp626->hd;}))->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp627.last_plus_one - _temp627.curr, _temp627.curr, _temp628.last_plus_one -
_temp628.curr, _temp628.curr, _temp629.last_plus_one - _temp629.curr, _temp629.curr);}));
goto _LL615; _LL621: goto _LL615; _LL615:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp631; _push_handler(& _temp631);{ int _temp633= 0; if( setjmp( _temp631.handler)){
_temp633= 1;} if( ! _temp633){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp632=( void*) _exn_thrown; void*
_temp635= _temp632; _LL637: if( _temp635 == Cyc_Dict_Absent){ goto _LL638;}
else{ goto _LL639;} _LL639: goto _LL640; _LL638: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp641= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp641.last_plus_one - _temp641.curr, _temp641.curr);})); return; _LL640:(
void) _throw( _temp635); _LL636:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp642= tud_opt; if( _temp642
== 0){ _throw( Null_Exception);} _temp642->v;})))->name;}} else{{ void* _temp643=(*
tud->name).f1; struct Cyc_List_List* _temp651; struct Cyc_List_List* _temp653;
_LL645: if(( unsigned int) _temp643 > 1u?*(( int*) _temp643) == Cyc_Absyn_Rel_n:
0){ _LL652: _temp651=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp643)->f1; if( _temp651 == 0){ goto _LL646;} else{ goto _LL647;}} else{ goto
_LL647;} _LL647: if(( unsigned int) _temp643 > 1u?*(( int*) _temp643) == Cyc_Absyn_Abs_n:
0){ _LL654: _temp653=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp643)->f1; goto _LL648;} else{ goto _LL649;} _LL649: goto _LL650; _LL646:(*
tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp655=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp655[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp656; _temp656.tag= Cyc_Absyn_Abs_n; _temp656.f1= te->ns; _temp656;});
_temp655;}); goto _LL644; _LL648: goto _LL650; _LL650: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp657=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp658; _temp658.curr= _temp657; _temp658.base= _temp657;
_temp658.last_plus_one= _temp657 + 43; _temp658;})); return; _LL644:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp660=({ struct
_tuple4 _temp659; _temp659.f1= tud->fields; _temp659.f2= tud_opt; _temp659;});
struct Cyc_Core_Opt* _temp670; struct Cyc_Core_Opt* _temp672; struct Cyc_Core_Opt*
_temp674; struct Cyc_Core_Opt* _temp676; struct Cyc_Core_Opt _temp678; struct
Cyc_List_List* _temp679; struct Cyc_List_List** _temp681; struct Cyc_Core_Opt*
_temp682; struct Cyc_Core_Opt _temp684; struct Cyc_Absyn_Tuniondecl** _temp685;
struct Cyc_Core_Opt* _temp687; struct Cyc_Core_Opt _temp689; struct Cyc_List_List*
_temp690; struct Cyc_List_List** _temp692; struct Cyc_Core_Opt* _temp693; struct
Cyc_Core_Opt _temp695; struct Cyc_Absyn_Tuniondecl** _temp696; struct Cyc_Core_Opt*
_temp698; _LL662: _LL673: _temp672= _temp660.f1; if( _temp672 == 0){ goto _LL671;}
else{ goto _LL664;} _LL671: _temp670= _temp660.f2; if( _temp670 == 0){ goto
_LL663;} else{ goto _LL664;} _LL664: _LL677: _temp676= _temp660.f1; if( _temp676
== 0){ goto _LL666;} else{ _temp678=* _temp676; _LL680: _temp679=( struct Cyc_List_List*)
_temp678.v; _temp681=&(* _temp660.f1).v; goto _LL675;} _LL675: _temp674=
_temp660.f2; if( _temp674 == 0){ goto _LL665;} else{ goto _LL666;} _LL666:
_LL688: _temp687= _temp660.f1; if( _temp687 == 0){ goto _LL668;} else{ _temp689=*
_temp687; _LL691: _temp690=( struct Cyc_List_List*) _temp689.v; _temp692=&(*
_temp660.f1).v; goto _LL683;} _LL683: _temp682= _temp660.f2; if( _temp682 == 0){
goto _LL668;} else{ _temp684=* _temp682; _LL686: _temp685=( struct Cyc_Absyn_Tuniondecl**)
_temp684.v; goto _LL667;} _LL668: _LL699: _temp698= _temp660.f1; if( _temp698 ==
0){ goto _LL694;} else{ goto _LL661;} _LL694: _temp693= _temp660.f2; if(
_temp693 == 0){ goto _LL661;} else{ _temp695=* _temp693; _LL697: _temp696=(
struct Cyc_Absyn_Tuniondecl**) _temp695.v; goto _LL669;} _LL663: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp700=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp700[ 0]= tud; _temp700;}));
goto _LL661; _LL665: { struct Cyc_Absyn_Tuniondecl** _temp703=({ struct Cyc_Absyn_Tuniondecl**
_temp701=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp701[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp702=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp702->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp702->name= tud->name; _temp702->tvs= tvs; _temp702->fields=
0; _temp702->is_xtunion= tud->is_xtunion; _temp702;}); _temp701;}); goto _LL704;
_LL704: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp703);* _temp681= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp681, tvs, tud);* _temp703= tud; goto
_LL661;} _LL667: { struct Cyc_Absyn_Tuniondecl* _temp705=* _temp685; goto _LL706;
_LL706:* _temp685=({ struct Cyc_Absyn_Tuniondecl* _temp707=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp707->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp707->name= tud->name; _temp707->tvs= tvs; _temp707->fields=
0; _temp707->is_xtunion= tud->is_xtunion; _temp707;});* _temp692= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp692, tvs, tud);* _temp685=
_temp705; _temp696= _temp685; goto _LL669;} _LL669: { struct Cyc_Absyn_Tuniondecl*
_temp708= Cyc_Tcdecl_merge_tuniondecl(* _temp696, tud, loc, Cyc_Tc_tc_msg); goto
_LL709; _LL709: if( _temp708 == 0){ return;} else{* _temp696=( struct Cyc_Absyn_Tuniondecl*)({
struct Cyc_Absyn_Tuniondecl* _temp710= _temp708; if( _temp710 == 0){ _throw(
Null_Exception);} _temp710;}); goto _LL661;}} _LL661:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp711=(* ed->name).f1; struct Cyc_List_List* _temp719; struct Cyc_List_List*
_temp721; _LL713: if(( unsigned int) _temp711 > 1u?*(( int*) _temp711) == Cyc_Absyn_Rel_n:
0){ _LL720: _temp719=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp711)->f1; if( _temp719 == 0){ goto _LL714;} else{ goto _LL715;}} else{ goto
_LL715;} _LL715: if(( unsigned int) _temp711 > 1u?*(( int*) _temp711) == Cyc_Absyn_Abs_n:
0){ _LL722: _temp721=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp711)->f1; if( _temp721 == 0){ goto _LL716;} else{ goto _LL717;}} else{ goto
_LL717;} _LL717: goto _LL718; _LL714: goto _LL712; _LL716: goto _LL712; _LL718:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp723=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp724; _temp724.curr= _temp723; _temp724.base= _temp723;
_temp724.last_plus_one= _temp723 + 43; _temp724;})); return; _LL712:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp725=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp725[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp726; _temp726.tag= Cyc_Absyn_Abs_n; _temp726.f1= te->ns; _temp726;});
_temp725;}); if( ed->fields != 0){ struct _RegionHandle _temp727= _new_region();
struct _RegionHandle* uprev_rgn=& _temp727; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp740= ed->fields; if(
_temp740 == 0){ _throw( Null_Exception);} _temp740->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp728= fs; if( _temp728 == 0){ _throw( Null_Exception);}
_temp728->tl;})){ struct Cyc_Absyn_Enumfield* _temp730=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp729= fs; if( _temp729 == 0){ _throw( Null_Exception);}
_temp729->hd;}); goto _LL731; _LL731: if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
_temp730->name).f2)){ Cyc_Tcutil_terr( _temp730->loc,({ struct _tagged_string
_temp732=*(* _temp730->name).f2; xprintf("duplicate field name %.*s", _temp732.last_plus_one
- _temp732.curr, _temp732.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp733=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp733->hd=( void*)(* _temp730->name).f2; _temp733->tl= prev_fields; _temp733;});}
if( _temp730->tag == 0){ _temp730->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, _temp730->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp734= _temp730->tag; if( _temp734 == 0){ _throw(
Null_Exception);} _temp734;}))){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp735=* v; struct _tagged_string _temp736=*(* _temp730->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_temp735.last_plus_one - _temp735.curr, _temp735.curr, _temp736.last_plus_one -
_temp736.curr, _temp736.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp739= _temp730->tag; if(
_temp739 == 0){ _throw( Null_Exception);} _temp739;})); tag_count= t1 + 1;(*
_temp730->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp737=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp737[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp738; _temp738.tag= Cyc_Absyn_Abs_n;
_temp738.f1= te->ns; _temp738;}); _temp737;});}}}; _pop_region(& _temp727);}{
struct _handler_cons _temp741; _push_handler(& _temp741);{ int _temp743= 0; if(
setjmp( _temp741.handler)){ _temp743= 1;} if( ! _temp743){{ struct Cyc_Absyn_Enumdecl**
_temp744=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v); goto _LL745; _LL745: {
struct Cyc_Absyn_Enumdecl* _temp746= Cyc_Tcdecl_merge_enumdecl(* _temp744, ed,
loc, Cyc_Tc_tc_msg); goto _LL747; _LL747: if( _temp746 == 0){ _npop_handler( 0u);
return;}* _temp744=( struct Cyc_Absyn_Enumdecl*)({ struct Cyc_Absyn_Enumdecl*
_temp748= _temp746; if( _temp748 == 0){ _throw( Null_Exception);} _temp748;});}};
_pop_handler();} else{ void* _temp742=( void*) _exn_thrown; void* _temp750=
_temp742; _LL752: if( _temp750 == Cyc_Dict_Absent){ goto _LL753;} else{ goto
_LL754;} _LL754: goto _LL755; _LL753: { struct Cyc_Absyn_Enumdecl** _temp757=({
struct Cyc_Absyn_Enumdecl** _temp756=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp756[ 0]= ed; _temp756;}); goto _LL758;
_LL758: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v, _temp757); goto _LL751;} _LL755:( void) _throw( _temp750);
_LL751:;}}} if( ed->fields != 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp766= ed->fields; if( _temp766 == 0){ _throw(
Null_Exception);} _temp766->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp759= fs; if( _temp759 == 0){ _throw( Null_Exception);} _temp759->tl;})){
struct Cyc_Absyn_Enumfield* _temp761=( struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List*
_temp760= fs; if( _temp760 == 0){ _throw( Null_Exception);} _temp760->hd;});
goto _LL762; _LL762: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp761->name).f2,({ struct _tuple3* _temp763=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp763->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp764=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp764[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp765; _temp765.tag= Cyc_Tcenv_EnumRes;
_temp765.f1= ed; _temp765.f2= _temp761; _temp765;}); _temp764;}); _temp763->f2=
1; _temp763;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp767= sc; _LL769: if( _temp767 ==( void*) Cyc_Absyn_Static){
goto _LL770;} else{ goto _LL771;} _LL771: if( _temp767 ==( void*) Cyc_Absyn_Abstract){
goto _LL772;} else{ goto _LL773;} _LL773: if( _temp767 ==( void*) Cyc_Absyn_Public){
goto _LL774;} else{ goto _LL775;} _LL775: if( _temp767 ==( void*) Cyc_Absyn_Extern){
goto _LL776;} else{ goto _LL777;} _LL777: if( _temp767 ==( void*) Cyc_Absyn_ExternC){
goto _LL778;} else{ goto _LL768;} _LL770: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp779=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp780; _temp780.curr= _temp779; _temp780.base= _temp779;
_temp780.last_plus_one= _temp779 + 44; _temp780;})); return 0; _LL772: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp781=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp782; _temp782.curr= _temp781; _temp782.base= _temp781;
_temp782.last_plus_one= _temp781 + 46; _temp782;})); return 0; _LL774: return 1;
_LL776: return 1; _LL778: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp783=( char*)"nested extern \"C\" declaration"; struct _tagged_string
_temp784; _temp784.curr= _temp783; _temp784.base= _temp783; _temp784.last_plus_one=
_temp783 + 30; _temp784;})); return 1; _LL768:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* _temp785= ds0; goto _LL786; _LL786: for( 0; _temp785 != 0;
_temp785=({ struct Cyc_List_List* _temp787= _temp785; if( _temp787 == 0){ _throw(
Null_Exception);} _temp787->tl;})){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp893= _temp785; if( _temp893 == 0){ _throw(
Null_Exception);} _temp893->hd;}); struct Cyc_Position_Segment* loc= d->loc;
void* _temp788=( void*) d->r; int _temp812; struct Cyc_Absyn_Exp* _temp814;
struct Cyc_Core_Opt* _temp816; struct Cyc_Core_Opt* _temp818; struct Cyc_Absyn_Pat*
_temp820; struct Cyc_Absyn_Vardecl* _temp822; struct Cyc_Absyn_Fndecl* _temp824;
struct Cyc_Absyn_Typedefdecl* _temp826; struct Cyc_Absyn_Structdecl* _temp828;
struct Cyc_Absyn_Uniondecl* _temp830; struct Cyc_Absyn_Tuniondecl* _temp832;
struct Cyc_Absyn_Enumdecl* _temp834; struct Cyc_List_List* _temp836; struct
_tagged_string* _temp838; struct Cyc_List_List* _temp840; struct _tuple0*
_temp842; struct _tuple0 _temp844; struct _tagged_string* _temp845; void*
_temp847; struct Cyc_List_List* _temp849; _LL790: if(*(( int*) _temp788) == Cyc_Absyn_Let_d){
_LL821: _temp820=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp788)->f1; goto _LL819; _LL819: _temp818=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp788)->f2; goto _LL817; _LL817: _temp816=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp788)->f3; goto _LL815;
_LL815: _temp814=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp788)->f4; goto _LL813; _LL813: _temp812=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp788)->f5; goto _LL791;} else{ goto _LL792;} _LL792: if(*(( int*) _temp788)
== Cyc_Absyn_Var_d){ _LL823: _temp822=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp788)->f1; goto _LL793;} else{ goto _LL794;} _LL794: if(*(( int*) _temp788)
== Cyc_Absyn_Fn_d){ _LL825: _temp824=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp788)->f1; goto _LL795;} else{ goto _LL796;} _LL796: if(*(( int*) _temp788)
== Cyc_Absyn_Typedef_d){ _LL827: _temp826=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp788)->f1; goto _LL797;} else{ goto
_LL798;} _LL798: if(*(( int*) _temp788) == Cyc_Absyn_Struct_d){ _LL829: _temp828=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp788)->f1;
goto _LL799;} else{ goto _LL800;} _LL800: if(*(( int*) _temp788) == Cyc_Absyn_Union_d){
_LL831: _temp830=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp788)->f1; goto _LL801;} else{ goto _LL802;} _LL802: if(*(( int*) _temp788)
== Cyc_Absyn_Tunion_d){ _LL833: _temp832=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp788)->f1; goto _LL803;} else{ goto
_LL804;} _LL804: if(*(( int*) _temp788) == Cyc_Absyn_Enum_d){ _LL835: _temp834=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp788)->f1;
goto _LL805;} else{ goto _LL806;} _LL806: if(*(( int*) _temp788) == Cyc_Absyn_Namespace_d){
_LL839: _temp838=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp788)->f1; goto _LL837; _LL837: _temp836=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp788)->f2; goto _LL807;} else{ goto _LL808;}
_LL808: if(*(( int*) _temp788) == Cyc_Absyn_Using_d){ _LL843: _temp842=( struct
_tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp788)->f1; _temp844=* _temp842;
_LL848: _temp847= _temp844.f1; goto _LL846; _LL846: _temp845= _temp844.f2; goto
_LL841; _LL841: _temp840=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp788)->f2; goto _LL809;} else{ goto _LL810;} _LL810: if(*(( int*) _temp788)
== Cyc_Absyn_ExternC_d){ _LL850: _temp849=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp788)->f1; goto _LL811;} else{ goto _LL789;} _LL791: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp851=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp852; _temp852.curr= _temp851; _temp852.base= _temp851;
_temp852.last_plus_one= _temp851 + 47; _temp852;})); goto _LL789; _LL793: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp822->sc): 0){( void*)(
_temp822->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp822, check_var_init); goto _LL789; _LL795: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp824->sc): 0){( void*)( _temp824->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp824); goto _LL789; _LL797: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp826); goto _LL789; _LL799: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp828->sc): 0){( void*)( _temp828->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp828); goto _LL789; _LL801: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp830->sc): 0){( void*)( _temp830->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp830);
goto _LL789; _LL803: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp832->sc): 0){( void*)( _temp832->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp832); goto _LL789; _LL805: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp834->sc): 0){( void*)( _temp834->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp834);
goto _LL789; _LL807: { struct Cyc_List_List* _temp853= te->ns; goto _LL854;
_LL854: { struct Cyc_List_List* _temp856=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp853,({ struct Cyc_List_List*
_temp855=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp855->hd=( void*) _temp838; _temp855->tl= 0; _temp855;})); goto _LL857;
_LL857: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge->namespaces, _temp838)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp838); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp856, Cyc_Tcenv_empty_genv());}
te->ns= _temp856; Cyc_Tc_tc_decls( te, _temp836, in_externC, check_var_init); te->ns=
_temp853; goto _LL789;}} _LL809: { struct _tagged_string* first; struct Cyc_List_List*
rest;{ void* _temp858= _temp847; struct Cyc_List_List* _temp870; struct Cyc_List_List*
_temp872; struct Cyc_List_List* _temp874; struct Cyc_List_List _temp876; struct
Cyc_List_List* _temp877; struct _tagged_string* _temp879; struct Cyc_List_List*
_temp881; struct Cyc_List_List _temp883; struct Cyc_List_List* _temp884; struct
_tagged_string* _temp886; _LL860: if( _temp858 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL861;} else{ goto _LL862;} _LL862: if(( unsigned int) _temp858 > 1u?*(( int*)
_temp858) == Cyc_Absyn_Rel_n: 0){ _LL871: _temp870=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp858)->f1; if( _temp870 == 0){ goto _LL863;}
else{ goto _LL864;}} else{ goto _LL864;} _LL864: if(( unsigned int) _temp858 > 1u?*((
int*) _temp858) == Cyc_Absyn_Abs_n: 0){ _LL873: _temp872=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp858)->f1; if( _temp872 == 0){ goto _LL865;}
else{ goto _LL866;}} else{ goto _LL866;} _LL866: if(( unsigned int) _temp858 > 1u?*((
int*) _temp858) == Cyc_Absyn_Rel_n: 0){ _LL875: _temp874=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp858)->f1; if( _temp874 == 0){ goto _LL868;}
else{ _temp876=* _temp874; _LL880: _temp879=( struct _tagged_string*) _temp876.hd;
goto _LL878; _LL878: _temp877=( struct Cyc_List_List*) _temp876.tl; goto _LL867;}}
else{ goto _LL868;} _LL868: if(( unsigned int) _temp858 > 1u?*(( int*) _temp858)
== Cyc_Absyn_Abs_n: 0){ _LL882: _temp881=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp858)->f1; if( _temp881 == 0){ goto _LL859;} else{ _temp883=* _temp881;
_LL887: _temp886=( struct _tagged_string*) _temp883.hd; goto _LL885; _LL885:
_temp884=( struct Cyc_List_List*) _temp883.tl; goto _LL869;}} else{ goto _LL859;}
_LL861: goto _LL863; _LL863: goto _LL865; _LL865: first= _temp845; rest= 0; goto
_LL859; _LL867: _temp886= _temp879; _temp884= _temp877; goto _LL869; _LL869:
first= _temp886; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp884,({ struct Cyc_List_List*
_temp888=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp888->hd=( void*) _temp845; _temp888->tl= 0; _temp888;})); goto _LL859;
_LL859:;}{ struct Cyc_List_List* _temp889= Cyc_Tcenv_resolve_namespace( te, loc,
first, rest); goto _LL890; _LL890: ge->availables=({ struct Cyc_List_List*
_temp891=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp891->hd=( void*) _temp889; _temp891->tl= ge->availables; _temp891;}); Cyc_Tc_tc_decls(
te, _temp840, in_externC, check_var_init); ge->availables=({ struct Cyc_List_List*
_temp892= ge->availables; if( _temp892 == 0){ _throw( Null_Exception);} _temp892->tl;});
goto _LL789;}} _LL811: Cyc_Tc_tc_decls( te, _temp849, 1, check_var_init); goto
_LL789; _LL789:;}} static char _temp896[ 4u]="Cyc"; static struct _tagged_string
Cyc_Tc_cyc_string=( struct _tagged_string){ _temp896, _temp896, _temp896 + 4u};
static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc(
struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int check_var_init, struct Cyc_List_List*
ds){ if( add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp897=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp897->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp898=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp898->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp899=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp899[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp900; _temp900.tag= Cyc_Absyn_Namespace_d;
_temp900.f1= Cyc_Tc_cyc_ns; _temp900.f2= ds; _temp900;}); _temp899;})); _temp898->loc=
0; _temp898;}); _temp897->tl= 0; _temp897;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t; static
int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d);
static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple6{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp901=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp911; struct Cyc_List_List* _temp913; struct Cyc_List_List**
_temp915; struct _tuple0* _temp916; struct Cyc_List_List* _temp918; struct Cyc_List_List**
_temp920; struct _tagged_string* _temp921; _LL903: if(*(( int*) _temp901) == Cyc_Absyn_Var_d){
_LL912: _temp911=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp901)->f1; goto _LL904;} else{ goto _LL905;} _LL905: if(*(( int*) _temp901)
== Cyc_Absyn_Using_d){ _LL917: _temp916=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp901)->f1; goto _LL914; _LL914: _temp913=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Using_d_struct*) _temp901)->f2; _temp915=&(( struct Cyc_Absyn_Using_d_struct*)
_temp901)->f2; goto _LL906;} else{ goto _LL907;} _LL907: if(*(( int*) _temp901)
== Cyc_Absyn_Namespace_d){ _LL922: _temp921=( struct _tagged_string*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp901)->f1; goto _LL919; _LL919: _temp918=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp901)->f2;
_temp920=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp901)->f2; goto _LL908;}
else{ goto _LL909;} _LL909: goto _LL910; _LL904: if(( void*) _temp911->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp925; struct
_tagged_string* _temp926; void* _temp928; struct _tuple0* _temp923= _temp911->name;
_temp925=* _temp923; _LL929: _temp928= _temp925.f1; goto _LL927; _LL927:
_temp926= _temp925.f2; goto _LL924; _LL924: { struct Cyc_List_List* ns;{ void*
_temp930= _temp928; struct Cyc_List_List* _temp938; struct Cyc_List_List*
_temp940; _LL932: if( _temp930 ==( void*) Cyc_Absyn_Loc_n){ goto _LL933;} else{
goto _LL934;} _LL934: if(( unsigned int) _temp930 > 1u?*(( int*) _temp930) ==
Cyc_Absyn_Rel_n: 0){ _LL939: _temp938=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp930)->f1; goto _LL935;} else{ goto _LL936;} _LL936: if(( unsigned int)
_temp930 > 1u?*(( int*) _temp930) == Cyc_Absyn_Abs_n: 0){ _LL941: _temp940=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp930)->f1; goto
_LL937;} else{ goto _LL931;} _LL933: ns= 0; goto _LL931; _LL935: ns= _temp938;
goto _LL931; _LL937: ns= _temp940; goto _LL931; _LL931:;}{ struct _tuple6*
_temp942=(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( env, ns); goto _LL943; _LL943: { struct Cyc_Tcenv_Genv*
_temp944=(* _temp942).f1; goto _LL945; _LL945: { int _temp946=(*(( struct
_tuple3*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
_temp944->ordinaries, _temp926)).f2; goto _LL947; _LL947: if( ! _temp946){(*
_temp942).f2=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_insert)((* _temp942).f2, _temp926);} return
_temp946;}}}}} _LL906: _temp920= _temp915; goto _LL908; _LL908:* _temp920= Cyc_Tc_treeshake_f(
env,* _temp920); return 1; _LL910: return 1; _LL902:;} static int Cyc_Tc_treeshake_remove_f(
struct Cyc_Set_Set* set, struct _tagged_string* x, struct _tuple3* y){ return !((
int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( set,
x);} static void Cyc_Tc_treeshake_remove( struct Cyc_List_List* ns, struct
_tuple6* nsenv){ struct Cyc_Tcenv_Genv* _temp948=(* nsenv).f1; goto _LL949;
_LL949: { struct Cyc_Set_Set* _temp950=(* nsenv).f2; goto _LL951; _LL951:
_temp948->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*,
struct _tagged_string*, struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f, _temp950, _temp948->ordinaries);}}
static struct _tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){
return({ struct _tuple6* _temp952=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp952->f1= ge; _temp952->f2=(( struct Cyc_Set_Set*(*)( int(* comp)(
struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp952;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp953=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); goto _LL954; _LL954: { struct Cyc_List_List*
_temp955= Cyc_Tc_treeshake_f( _temp953, ds); goto _LL956; _LL956:(( void(*)(
void(* f)( struct Cyc_List_List*, struct _tuple6*), struct Cyc_Dict_Dict* d))
Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, _temp953); return _temp955;}}