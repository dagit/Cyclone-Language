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
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default( void*); extern void*
Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace,
struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
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
e; if( vd->initializer == 0){ if( ! Cyc_Tcutil_supports_default( t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp330= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp331= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp330.last_plus_one - _temp330.curr, _temp330.curr, _temp331.last_plus_one -
_temp331.curr, _temp331.curr);}));}} else{ e=( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp332= vd->initializer; if( _temp332 == 0){ _throw(
Null_Exception);} _temp332;});{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)&
t, e); if( ! Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp333= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp334= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp335= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp333.last_plus_one - _temp333.curr, _temp333.curr, _temp334.last_plus_one -
_temp334.curr, _temp334.curr, _temp335.last_plus_one - _temp335.curr, _temp335.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp336=( char*)"initializer is not a constant expression";
struct _tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 41; _temp337;}));}}}}} else{ for( 0; atts !=
0; atts=({ struct Cyc_List_List* _temp338= atts; if( _temp338 == 0){ _throw(
Null_Exception);} _temp338->tl;})){ void* _temp340=( void*)({ struct Cyc_List_List*
_temp339= atts; if( _temp339 == 0){ _throw( Null_Exception);} _temp339->hd;});
int _temp358; int _temp360; _LL342: if(( unsigned int) _temp340 > 15u?*(( int*)
_temp340) == Cyc_Absyn_Regparm_att: 0){ _LL359: _temp358=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp340)->f1; goto _LL343;} else{ goto _LL344;} _LL344: if( _temp340 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL345;} else{ goto _LL346;} _LL346: if( _temp340
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL347;} else{ goto _LL348;} _LL348: if(
_temp340 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL349;} else{ goto _LL350;}
_LL350: if( _temp340 ==( void*) Cyc_Absyn_Const_att){ goto _LL351;} else{ goto
_LL352;} _LL352: if(( unsigned int) _temp340 > 15u?*(( int*) _temp340) == Cyc_Absyn_Aligned_att:
0){ _LL361: _temp360=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp340)->f1;
goto _LL353;} else{ goto _LL354;} _LL354: if( _temp340 ==( void*) Cyc_Absyn_Packed_att){
goto _LL355;} else{ goto _LL356;} _LL356: goto _LL357; _LL343: goto _LL345;
_LL345: goto _LL347; _LL347: goto _LL349; _LL349: goto _LL351; _LL351:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp362=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 51; _temp363;})); goto _LL341; _LL353: goto
_LL355; _LL355: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp365= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp364= atts; if( _temp364 == 0){ _throw(
Null_Exception);} _temp364->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp365.last_plus_one - _temp365.curr, _temp365.curr);})); goto _LL341; _LL357:
continue; _LL341:;}}} if( !( is_redecl? sc ==( void*) Cyc_Absyn_Extern? 1: Cyc_Tcutil_is_function_type(
t): 0)){ ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp366=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp366->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp367=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp367[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp368; _temp368.tag= Cyc_Tcenv_VarRes;
_temp368.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp369=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp369[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp370; _temp370.tag= Cyc_Absyn_Global_b;
_temp370.f1= vd; _temp370;}); _temp369;})); _temp368;}); _temp367;}); _temp366->f2=
is_redecl; _temp366;}));}}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp371=(* fd->name).f1; struct Cyc_List_List* _temp379; struct Cyc_List_List*
_temp381; _LL373: if(( unsigned int) _temp371 > 1u?*(( int*) _temp371) == Cyc_Absyn_Rel_n:
0){ _LL380: _temp379=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp371)->f1; if( _temp379 == 0){ goto _LL374;} else{ goto _LL375;}} else{ goto
_LL375;} _LL375: if(( unsigned int) _temp371 > 1u?*(( int*) _temp371) == Cyc_Absyn_Abs_n:
0){ _LL382: _temp381=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp371)->f1; goto _LL376;} else{ goto _LL377;} _LL377: goto _LL378; _LL374:
goto _LL372; _LL376:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp383=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp383[ 0]=({ struct Cyc_Core_Impossible_struct _temp384; _temp384.tag= Cyc_Core_Impossible;
_temp384.f1=( struct _tagged_string)({ char* _temp385=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 22; _temp386;}); _temp384;}); _temp383;}));
_LL378: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp387=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 43; _temp388;})); return; _LL372:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp389=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp389[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp390; _temp390.tag= Cyc_Absyn_Abs_n; _temp390.f1= te->ns; _temp390;});
_temp389;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, fd->attributes, 1);{
struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct
Cyc_List_List* _temp391= atts; if( _temp391 == 0){ _throw( Null_Exception);}
_temp391->tl;})){ void* _temp393=( void*)({ struct Cyc_List_List* _temp392= atts;
if( _temp392 == 0){ _throw( Null_Exception);} _temp392->hd;}); int _temp401;
_LL395: if( _temp393 ==( void*) Cyc_Absyn_Packed_att){ goto _LL396;} else{ goto
_LL397;} _LL397: if(( unsigned int) _temp393 > 15u?*(( int*) _temp393) == Cyc_Absyn_Aligned_att:
0){ _LL402: _temp401=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp393)->f1;
goto _LL398;} else{ goto _LL399;} _LL399: goto _LL400; _LL396: goto _LL398;
_LL398: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp404= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp403= atts; if( _temp403 == 0){ _throw(
Null_Exception);} _temp403->hd;})); xprintf("bad attribute %.*s for function",
_temp404.last_plus_one - _temp404.curr, _temp404.curr);})); goto _LL394; _LL400:
goto _LL394; _LL394:;}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp405=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp405->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp406=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp406[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp407; _temp407.tag= Cyc_Tcenv_VarRes;
_temp407.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp408=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp408[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp409; _temp409.tag= Cyc_Absyn_Funname_b;
_temp409.f1= fd; _temp409;}); _temp408;})); _temp407;}); _temp406;}); _temp405->f2=
is_redecl; _temp405;})); te->le=({ struct Cyc_Core_Opt* _temp410=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp410->v=( void*)(( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp411=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp411[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp412; _temp412.tag= Cyc_Tcenv_Outermost; _temp412.f1=(
void*) Cyc_Tcenv_new_fenv( loc, fd); _temp412;}); _temp411;})); _temp410;}); Cyc_Tcstmt_tcStmt(
te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp413=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 49; _temp414;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp415=(* td->name).f1; struct Cyc_List_List*
_temp423; struct Cyc_List_List* _temp425; _LL417: if(( unsigned int) _temp415 >
1u?*(( int*) _temp415) == Cyc_Absyn_Rel_n: 0){ _LL424: _temp423=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp415)->f1; if( _temp423 == 0){ goto _LL418;}
else{ goto _LL419;}} else{ goto _LL419;} _LL419: if(( unsigned int) _temp415 > 1u?*((
int*) _temp415) == Cyc_Absyn_Abs_n: 0){ _LL426: _temp425=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp415)->f1; if( _temp425 == 0){ goto _LL420;}
else{ goto _LL421;}} else{ goto _LL421;} _LL421: goto _LL422; _LL418: goto
_LL416; _LL420: goto _LL416; _LL422: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp427=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 43; _temp428;})); return; _LL416:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp429=* v; xprintf("redeclaration of typedef %.*s",
_temp429.last_plus_one - _temp429.curr, _temp429.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp430=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp430[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp431; _temp431.tag= Cyc_Absyn_Abs_n; _temp431.f1= te->ns; _temp431;});
_temp430;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp432=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 47; _temp433;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp674= sd->name;
if( _temp674 == 0){ _throw( Null_Exception);} _temp674->v;}))).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp434=
atts; if( _temp434 == 0){ _throw( Null_Exception);} _temp434->tl;})){ void*
_temp436=( void*)({ struct Cyc_List_List* _temp435= atts; if( _temp435 == 0){
_throw( Null_Exception);} _temp435->hd;}); int _temp444; _LL438: if( _temp436 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL439;} else{ goto _LL440;} _LL440: if((
unsigned int) _temp436 > 15u?*(( int*) _temp436) == Cyc_Absyn_Aligned_att: 0){
_LL445: _temp444=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp436)->f1;
goto _LL441;} else{ goto _LL442;} _LL442: goto _LL443; _LL439: continue; _LL441:
continue; _LL443: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp447= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp446= atts; if( _temp446 == 0){ _throw(
Null_Exception);} _temp446->hd;})); struct _tagged_string _temp448=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp447.last_plus_one - _temp447.curr, _temp447.curr, _temp448.last_plus_one -
_temp448.curr, _temp448.curr);})); goto _LL437; _LL437:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp449= tvs2;
if( _temp449 == 0){ _throw( Null_Exception);} _temp449->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp465= tvs2; if( _temp465 ==
0){ _throw( Null_Exception);} _temp465->hd;}))->kind); void* _temp450=( void*) c->v;
void* _temp458; _LL452: if( _temp450 ==( void*) Cyc_Absyn_No_constr){ goto
_LL453;} else{ goto _LL454;} _LL454: if(( unsigned int) _temp450 > 1u?*(( int*)
_temp450) == Cyc_Absyn_Eq_constr: 0){ _LL459: _temp458=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp450)->f1; if( _temp458 ==( void*) Cyc_Absyn_MemKind){ goto _LL455;} else{
goto _LL456;}} else{ goto _LL456;} _LL456: goto _LL457; _LL453:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp460=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp460[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp461; _temp461.tag= Cyc_Absyn_Eq_constr; _temp461.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp461;}); _temp460;}))); continue; _LL455:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp463=* v; struct
_tagged_string _temp464=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp462= tvs2; if( _temp462 == 0){ _throw( Null_Exception);} _temp462->hd;}))->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp463.last_plus_one - _temp463.curr, _temp463.curr, _temp464.last_plus_one -
_temp464.curr, _temp464.curr);})); continue; _LL457: continue; _LL451:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp467=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp466= sd->name; if( _temp466 == 0){ _throw(
Null_Exception);} _temp466->v;}))).f1; struct Cyc_List_List* _temp475; struct
Cyc_List_List* _temp477; _LL469: if(( unsigned int) _temp467 > 1u?*(( int*)
_temp467) == Cyc_Absyn_Rel_n: 0){ _LL476: _temp475=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp467)->f1; if( _temp475 == 0){ goto _LL470;}
else{ goto _LL471;}} else{ goto _LL471;} _LL471: if(( unsigned int) _temp467 > 1u?*((
int*) _temp467) == Cyc_Absyn_Abs_n: 0){ _LL478: _temp477=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp467)->f1; if( _temp477 == 0){ goto _LL472;}
else{ goto _LL473;}} else{ goto _LL473;} _LL473: goto _LL474; _LL470: goto
_LL468; _LL472: goto _LL468; _LL474: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp479=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp480; _temp480.curr= _temp479; _temp480.base= _temp479;
_temp480.last_plus_one= _temp479 + 43; _temp480;})); return; _LL468:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp481= sd->name; if( _temp481 == 0){
_throw( Null_Exception);} _temp481->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp482=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp482[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp483; _temp483.tag= Cyc_Absyn_Abs_n;
_temp483.f1= te->ns; _temp483;}); _temp482;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp484=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp490; struct Cyc_Absyn_Structdecl** _temp491; _LL486:
if( _temp484 == 0){ goto _LL487;} else{ goto _LL488;} _LL488: if( _temp484 == 0){
goto _LL485;} else{ _temp490=* _temp484; _LL492: _temp491=( struct Cyc_Absyn_Structdecl**)
_temp490.v; goto _LL489;} _LL487: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp493=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp493[ 0]= sd; _temp493;})); if( sd->fields == 0){ return;} goto _LL485;
_LL489: { struct Cyc_Absyn_Structdecl* sd2=* _temp491; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp494=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp494.last_plus_one - _temp494.curr, _temp494.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp495= x1; if( _temp495 == 0){ _throw( Null_Exception);} _temp495->tl;}), x2=({
struct Cyc_List_List* _temp496= x2; if( _temp496 == 0){ _throw( Null_Exception);}
_temp496->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp505= x1; if( _temp505 == 0){ _throw( Null_Exception);}
_temp505->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp504= x2; if( _temp504 == 0){ _throw( Null_Exception);}
_temp504->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp497=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp497[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp498; _temp498.tag= Cyc_Absyn_Forward_constr; _temp498.f1= c2; _temp498;});
_temp497;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp500=* v; struct _tagged_string _temp501= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp502=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp499= x1; if( _temp499 == 0){ _throw( Null_Exception);} _temp499->hd;}))->name;
struct _tagged_string _temp503= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp500.last_plus_one - _temp500.curr, _temp500.curr, _temp501.last_plus_one -
_temp501.curr, _temp501.curr, _temp502.last_plus_one - _temp502.curr, _temp502.curr,
_temp503.last_plus_one - _temp503.curr, _temp503.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp506=( char*)"struct";
struct _tagged_string _temp507; _temp507.curr= _temp506; _temp507.base= _temp506;
_temp507.last_plus_one= _temp506 + 7; _temp507;}), v));{ struct _tuple7 _temp509=({
struct _tuple7 _temp508; _temp508.f1= sd->fields; _temp508.f2= sd2->fields;
_temp508;}); struct Cyc_Core_Opt* _temp519; struct Cyc_Core_Opt* _temp521;
struct Cyc_Core_Opt* _temp523; struct Cyc_Core_Opt* _temp525; struct Cyc_Core_Opt*
_temp527; struct Cyc_Core_Opt* _temp529; struct Cyc_Core_Opt* _temp531; struct
Cyc_Core_Opt* _temp533; _LL511: _LL522: _temp521= _temp509.f1; if( _temp521 == 0){
goto _LL520;} else{ goto _LL513;} _LL520: _temp519= _temp509.f2; if( _temp519 ==
0){ goto _LL512;} else{ goto _LL513;} _LL513: _LL526: _temp525= _temp509.f1;
goto _LL524; _LL524: _temp523= _temp509.f2; if( _temp523 == 0){ goto _LL514;}
else{ goto _LL515;} _LL515: _LL530: _temp529= _temp509.f1; if( _temp529 == 0){
goto _LL528;} else{ goto _LL517;} _LL528: _temp527= _temp509.f2; goto _LL516;
_LL517: _LL534: _temp533= _temp509.f1; goto _LL532; _LL532: _temp531= _temp509.f2;
goto _LL518; _LL512: return; _LL514:* _temp491= sd; goto _LL510; _LL516: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL518: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp535= tvs0; if( _temp535 == 0){ _throw( Null_Exception);} _temp535->tl;}),
tvs2=({ struct Cyc_List_List* _temp536= tvs2; if( _temp536 == 0){ _throw(
Null_Exception);} _temp536->tl;})){ inst=({ struct Cyc_List_List* _temp537=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp537->hd=(
void*)({ struct _tuple4* _temp538=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp538->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp542= tvs2; if( _temp542 == 0){ _throw( Null_Exception);} _temp542->hd;});
_temp538->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp539=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp539[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp540; _temp540.tag= Cyc_Absyn_VarType; _temp540.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp541= tvs0; if( _temp541 == 0){ _throw( Null_Exception);}
_temp541->hd;}); _temp540;}); _temp539;}); _temp538;}); _temp537->tl= inst;
_temp537;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes, sd2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp543=* v; xprintf("redeclaration of struct %.*s has different attributes",
_temp543.last_plus_one - _temp543.curr, _temp543.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp599= sd->fields; if(
_temp599 == 0){ _throw( Null_Exception);} _temp599->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp598= sd2->fields; if(
_temp598 == 0){ _throw( Null_Exception);} _temp598->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp544= f1s; if( _temp544 == 0){ _throw(
Null_Exception);} _temp544->tl;}), f2s=({ struct Cyc_List_List* _temp545= f2s;
if( _temp545 == 0){ _throw( Null_Exception);} _temp545->tl;})){ struct Cyc_Absyn_Structfield
_temp549; struct Cyc_List_List* _temp550; struct Cyc_Core_Opt* _temp552; void*
_temp554; struct Cyc_Absyn_Tqual _temp556; struct _tagged_string* _temp558;
struct Cyc_Absyn_Structfield* _temp547=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp546= f1s; if( _temp546 == 0){ _throw( Null_Exception);}
_temp546->hd;}); _temp549=* _temp547; _LL559: _temp558=( struct _tagged_string*)
_temp549.name; goto _LL557; _LL557: _temp556=( struct Cyc_Absyn_Tqual) _temp549.tq;
goto _LL555; _LL555: _temp554=( void*) _temp549.type; goto _LL553; _LL553:
_temp552=( struct Cyc_Core_Opt*) _temp549.width; goto _LL551; _LL551: _temp550=(
struct Cyc_List_List*) _temp549.attributes; goto _LL548; _LL548: { struct Cyc_Absyn_Structfield
_temp563; struct Cyc_List_List* _temp564; struct Cyc_Core_Opt* _temp566; void*
_temp568; struct Cyc_Absyn_Tqual _temp570; struct _tagged_string* _temp572;
struct Cyc_Absyn_Structfield* _temp561=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp560= f2s; if( _temp560 == 0){ _throw( Null_Exception);}
_temp560->hd;}); _temp563=* _temp561; _LL573: _temp572=( struct _tagged_string*)
_temp563.name; goto _LL571; _LL571: _temp570=( struct Cyc_Absyn_Tqual) _temp563.tq;
goto _LL569; _LL569: _temp568=( void*) _temp563.type; goto _LL567; _LL567:
_temp566=( struct Cyc_Core_Opt*) _temp563.width; goto _LL565; _LL565: _temp564=(
struct Cyc_List_List*) _temp563.attributes; goto _LL562; _LL562: if( Cyc_String_zstrptrcmp(
_temp558, _temp572) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp574=* v; struct _tagged_string _temp575=* _temp558; struct _tagged_string
_temp576=* _temp572; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp574.last_plus_one - _temp574.curr, _temp574.curr, _temp575.last_plus_one -
_temp575.curr, _temp575.curr, _temp576.last_plus_one - _temp576.curr, _temp576.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp550, _temp564)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp577=* v; struct _tagged_string _temp578=* _temp558; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp577.last_plus_one - _temp577.curr, _temp577.curr, _temp578.last_plus_one -
_temp578.curr, _temp578.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp556,
_temp570)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp579=* v; struct
_tagged_string _temp580=* _temp558; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp579.last_plus_one - _temp579.curr, _temp579.curr, _temp580.last_plus_one -
_temp580.curr, _temp580.curr);}));} if( _temp552 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp581= _temp552; if(
_temp581 == 0){ _throw( Null_Exception);} _temp581->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp582=* v; struct _tagged_string _temp583=*
_temp558; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp582.last_plus_one
- _temp582.curr, _temp582.curr, _temp583.last_plus_one - _temp583.curr, _temp583.curr);}));}
else{ if((( _temp552 != 0? _temp566 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp584= _temp552; if( _temp584
== 0){ _throw( Null_Exception);} _temp584->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp585= _temp566; if( _temp585
== 0){ _throw( Null_Exception);} _temp585->v;})): 0)? 1: _temp552 != _temp566){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp586=* v; struct
_tagged_string _temp587=* _temp558; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp586.last_plus_one - _temp586.curr, _temp586.curr, _temp587.last_plus_one -
_temp587.curr, _temp587.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp554);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp568); if( ! Cyc_Tcutil_unify( _temp554, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp588=* v; struct _tagged_string _temp589=* _temp558;
struct _tagged_string _temp590= Cyc_Absynpp_typ2string( _temp554); struct
_tagged_string _temp591= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp588.last_plus_one - _temp588.curr, _temp588.curr, _temp589.last_plus_one -
_temp589.curr, _temp589.curr, _temp590.last_plus_one - _temp590.curr, _temp590.curr,
_temp591.last_plus_one - _temp591.curr, _temp591.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp593=* v; struct
_tagged_string _temp594=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp592= f2s; if( _temp592 == 0){ _throw( Null_Exception);} _temp592->hd;}))->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp593.last_plus_one
- _temp593.curr, _temp593.curr, _temp594.last_plus_one - _temp594.curr, _temp594.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp596=* v;
struct _tagged_string _temp597=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp595= f1s; if( _temp595 == 0){ _throw( Null_Exception);} _temp595->hd;}))->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp596.last_plus_one
- _temp596.curr, _temp596.curr, _temp597.last_plus_one - _temp597.curr, _temp597.curr);}));}
return;}} _LL510:;} goto _LL485;}} _LL485:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp600=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp600->sc=( void*)(( void*) sd->sc); _temp600->name= sd->name; _temp600->tvs=
tvs; _temp600->fields= 0; _temp600->attributes= 0; _temp600;});{ struct
_RegionHandle _temp601= _new_region(); struct _RegionHandle* sprev_rgn=&
_temp601; _push_region( sprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp670= sd->fields; if( _temp670 == 0){ _throw( Null_Exception);} _temp670->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp602= fs; if( _temp602 == 0){
_throw( Null_Exception);} _temp602->tl;})){ struct Cyc_Absyn_Structfield
_temp606; struct Cyc_List_List* _temp607; struct Cyc_Core_Opt* _temp609; void*
_temp611; struct Cyc_Absyn_Tqual _temp613; struct _tagged_string* _temp615;
struct Cyc_Absyn_Structfield* _temp604=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp603= fs; if( _temp603 == 0){ _throw( Null_Exception);}
_temp603->hd;}); _temp606=* _temp604; _LL616: _temp615=( struct _tagged_string*)
_temp606.name; goto _LL614; _LL614: _temp613=( struct Cyc_Absyn_Tqual) _temp606.tq;
goto _LL612; _LL612: _temp611=( void*) _temp606.type; goto _LL610; _LL610:
_temp609=( struct Cyc_Core_Opt*) _temp606.width; goto _LL608; _LL608: _temp607=(
struct Cyc_List_List*) _temp606.attributes; goto _LL605; _LL605: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp615)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp617=* _temp615;
xprintf("duplicate field %.*s in struct", _temp617.last_plus_one - _temp617.curr,
_temp617.curr);}));} if( Cyc_String_strcmp(* _temp615,( struct _tagged_string)({
char* _temp618=( char*)""; struct _tagged_string _temp619; _temp619.curr=
_temp618; _temp619.base= _temp618; _temp619.last_plus_one= _temp618 + 1;
_temp619;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp620=( struct Cyc_List_List*)
_region_malloc( sprev_rgn, sizeof( struct Cyc_List_List)); _temp620->hd=( void*)
_temp615; _temp620->tl= prev_fields; _temp620;});} Cyc_Tcutil_check_type( loc,
te, tvs,( void*) Cyc_Absyn_MemKind, _temp611); if( _temp609 != 0){ unsigned int
w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp621= _temp609; if( _temp621 == 0){ _throw( Null_Exception);} _temp621->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp622=* _temp615; xprintf("bitfield %.*s does not have constant width",
_temp622.last_plus_one - _temp622.curr, _temp622.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp623= _temp609; if( _temp623
== 0){ _throw( Null_Exception);} _temp623->v;}));}{ void* _temp624= Cyc_Tcutil_compress(
_temp611); void* _temp630; void* _temp632; _LL626: if(( unsigned int) _temp624 >
4u?*(( int*) _temp624) == Cyc_Absyn_IntType: 0){ _LL633: _temp632=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp624)->f1; goto _LL631; _LL631: _temp630=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp624)->f2; goto _LL627;} else{
goto _LL628;} _LL628: goto _LL629; _LL627:{ void* _temp634= _temp630; _LL636:
if( _temp634 ==( void*) Cyc_Absyn_B1){ goto _LL637;} else{ goto _LL638;} _LL638:
if( _temp634 ==( void*) Cyc_Absyn_B2){ goto _LL639;} else{ goto _LL640;} _LL640:
if( _temp634 ==( void*) Cyc_Absyn_B4){ goto _LL641;} else{ goto _LL642;} _LL642:
if( _temp634 ==( void*) Cyc_Absyn_B8){ goto _LL643;} else{ goto _LL635;} _LL637:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp644=(
char*)"bitfield larger than type"; struct _tagged_string _temp645; _temp645.curr=
_temp644; _temp645.base= _temp644; _temp645.last_plus_one= _temp644 + 26;
_temp645;}));} goto _LL635; _LL639: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp646=( char*)"bitfield larger than type"; struct
_tagged_string _temp647; _temp647.curr= _temp646; _temp647.base= _temp646;
_temp647.last_plus_one= _temp646 + 26; _temp647;}));} goto _LL635; _LL641: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp648=( char*)"bitfield larger than type";
struct _tagged_string _temp649; _temp649.curr= _temp648; _temp649.base= _temp648;
_temp649.last_plus_one= _temp648 + 26; _temp649;}));} goto _LL635; _LL643: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp650=( char*)"bitfield larger than type";
struct _tagged_string _temp651; _temp651.curr= _temp650; _temp651.base= _temp650;
_temp651.last_plus_one= _temp650 + 26; _temp651;}));} goto _LL635; _LL635:;}
goto _LL625; _LL629: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp652=*
_temp615; struct _tagged_string _temp653= Cyc_Absynpp_typ2string( _temp611);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp652.last_plus_one
- _temp652.curr, _temp652.curr, _temp653.last_plus_one - _temp653.curr, _temp653.curr);}));
goto _LL625; _LL625:;}} for( 0; _temp607 != 0; _temp607=({ struct Cyc_List_List*
_temp654= _temp607; if( _temp654 == 0){ _throw( Null_Exception);} _temp654->tl;})){
void* _temp656=( void*)({ struct Cyc_List_List* _temp655= _temp607; if( _temp655
== 0){ _throw( Null_Exception);} _temp655->hd;}); int _temp664; _LL658: if(
_temp656 ==( void*) Cyc_Absyn_Packed_att){ goto _LL659;} else{ goto _LL660;}
_LL660: if(( unsigned int) _temp656 > 15u?*(( int*) _temp656) == Cyc_Absyn_Aligned_att:
0){ _LL665: _temp664=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp656)->f1;
goto _LL661;} else{ goto _LL662;} _LL662: goto _LL663; _LL659: continue; _LL661:
continue; _LL663: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp667= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp666= _temp607; if( _temp666 == 0){ _throw(
Null_Exception);} _temp666->hd;})); struct _tagged_string _temp668=* v; struct
_tagged_string _temp669=* _temp615; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp667.last_plus_one - _temp667.curr, _temp667.curr, _temp668.last_plus_one -
_temp668.curr, _temp668.curr, _temp669.last_plus_one - _temp669.curr, _temp669.curr);}));
goto _LL657; _LL657:;}}}; _pop_region(& _temp601);} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple6* _temp671=( struct _tuple6*) GC_malloc(
sizeof( struct _tuple6)); _temp671->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp672=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp672[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp673; _temp673.tag= Cyc_Tcenv_StructRes;
_temp673.f1= sd; _temp673;}); _temp672;}); _temp671->f2= 1; _temp671;}));* sdp=
sd;}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp675=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 46; _temp676;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp914= ud->name;
if( _temp914 == 0){ _throw( Null_Exception);} _temp914->v;}))).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp677=
atts; if( _temp677 == 0){ _throw( Null_Exception);} _temp677->tl;})){ void*
_temp679=( void*)({ struct Cyc_List_List* _temp678= atts; if( _temp678 == 0){
_throw( Null_Exception);} _temp678->hd;}); int _temp687; _LL681: if( _temp679 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL682;} else{ goto _LL683;} _LL683: if((
unsigned int) _temp679 > 15u?*(( int*) _temp679) == Cyc_Absyn_Aligned_att: 0){
_LL688: _temp687=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp679)->f1;
goto _LL684;} else{ goto _LL685;} _LL685: goto _LL686; _LL682: continue; _LL684:
continue; _LL686: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp690= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp689= atts; if( _temp689 == 0){ _throw(
Null_Exception);} _temp689->hd;})); struct _tagged_string _temp691=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp690.last_plus_one - _temp690.curr, _temp690.curr, _temp691.last_plus_one -
_temp691.curr, _temp691.curr);})); goto _LL680; _LL680:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp692= tvs2;
if( _temp692 == 0){ _throw( Null_Exception);} _temp692->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp708= tvs2; if( _temp708 ==
0){ _throw( Null_Exception);} _temp708->hd;}))->kind); void* _temp693=( void*) c->v;
void* _temp701; _LL695: if( _temp693 ==( void*) Cyc_Absyn_No_constr){ goto
_LL696;} else{ goto _LL697;} _LL697: if(( unsigned int) _temp693 > 1u?*(( int*)
_temp693) == Cyc_Absyn_Eq_constr: 0){ _LL702: _temp701=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp693)->f1; if( _temp701 ==( void*) Cyc_Absyn_MemKind){ goto _LL698;} else{
goto _LL699;}} else{ goto _LL699;} _LL699: goto _LL700; _LL696:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp703=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp703[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp704; _temp704.tag= Cyc_Absyn_Eq_constr; _temp704.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp704;}); _temp703;}))); continue; _LL698:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp706=* v; struct
_tagged_string _temp707=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp705= tvs2; if( _temp705 == 0){ _throw( Null_Exception);} _temp705->hd;}))->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp706.last_plus_one
- _temp706.curr, _temp706.curr, _temp707.last_plus_one - _temp707.curr, _temp707.curr);}));
continue; _LL700: continue; _LL694:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp710=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp709= ud->name;
if( _temp709 == 0){ _throw( Null_Exception);} _temp709->v;}))).f1; struct Cyc_List_List*
_temp718; struct Cyc_List_List* _temp720; _LL712: if(( unsigned int) _temp710 >
1u?*(( int*) _temp710) == Cyc_Absyn_Rel_n: 0){ _LL719: _temp718=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp710)->f1; if( _temp718 == 0){ goto _LL713;}
else{ goto _LL714;}} else{ goto _LL714;} _LL714: if(( unsigned int) _temp710 > 1u?*((
int*) _temp710) == Cyc_Absyn_Abs_n: 0){ _LL721: _temp720=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp710)->f1; if( _temp720 == 0){ goto _LL715;}
else{ goto _LL716;}} else{ goto _LL716;} _LL716: goto _LL717; _LL713: goto
_LL711; _LL715: goto _LL711; _LL717: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp722=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 43; _temp723;})); return; _LL711:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp724= ud->name; if( _temp724 == 0){
_throw( Null_Exception);} _temp724->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp725=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp725[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp726; _temp726.tag= Cyc_Absyn_Abs_n;
_temp726.f1= te->ns; _temp726;}); _temp725;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp727=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp733; struct Cyc_Absyn_Uniondecl** _temp734; _LL729: if(
_temp727 == 0){ goto _LL730;} else{ goto _LL731;} _LL731: if( _temp727 == 0){
goto _LL728;} else{ _temp733=* _temp727; _LL735: _temp734=( struct Cyc_Absyn_Uniondecl**)
_temp733.v; goto _LL732;} _LL730: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp736=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp736[ 0]= ud; _temp736;})); if( ud->fields == 0){ return;} goto _LL728;
_LL732: { struct Cyc_Absyn_Uniondecl* ud2=* _temp734; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp737=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp737.last_plus_one - _temp737.curr, _temp737.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp738= x1; if( _temp738 == 0){ _throw( Null_Exception);} _temp738->tl;}), x2=({
struct Cyc_List_List* _temp739= x2; if( _temp739 == 0){ _throw( Null_Exception);}
_temp739->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp748= x1; if( _temp748 == 0){ _throw( Null_Exception);}
_temp748->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp747= x2; if( _temp747 == 0){ _throw( Null_Exception);}
_temp747->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp740=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp740[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp741; _temp741.tag= Cyc_Absyn_Forward_constr; _temp741.f1= c2; _temp741;});
_temp740;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp743=* v; struct _tagged_string _temp744= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp745=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp742= x1; if( _temp742 == 0){ _throw( Null_Exception);} _temp742->hd;}))->name;
struct _tagged_string _temp746= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp743.last_plus_one - _temp743.curr, _temp743.curr, _temp744.last_plus_one -
_temp744.curr, _temp744.curr, _temp745.last_plus_one - _temp745.curr, _temp745.curr,
_temp746.last_plus_one - _temp746.curr, _temp746.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp749=( char*)"union";
struct _tagged_string _temp750; _temp750.curr= _temp749; _temp750.base= _temp749;
_temp750.last_plus_one= _temp749 + 6; _temp750;}), v));{ struct _tuple7 _temp752=({
struct _tuple7 _temp751; _temp751.f1= ud->fields; _temp751.f2= ud2->fields;
_temp751;}); struct Cyc_Core_Opt* _temp762; struct Cyc_Core_Opt* _temp764;
struct Cyc_Core_Opt* _temp766; struct Cyc_Core_Opt* _temp768; struct Cyc_Core_Opt*
_temp770; struct Cyc_Core_Opt* _temp772; struct Cyc_Core_Opt* _temp774; struct
Cyc_Core_Opt* _temp776; _LL754: _LL765: _temp764= _temp752.f1; if( _temp764 == 0){
goto _LL763;} else{ goto _LL756;} _LL763: _temp762= _temp752.f2; if( _temp762 ==
0){ goto _LL755;} else{ goto _LL756;} _LL756: _LL769: _temp768= _temp752.f1;
goto _LL767; _LL767: _temp766= _temp752.f2; if( _temp766 == 0){ goto _LL757;}
else{ goto _LL758;} _LL758: _LL773: _temp772= _temp752.f1; if( _temp772 == 0){
goto _LL771;} else{ goto _LL760;} _LL771: _temp770= _temp752.f2; goto _LL759;
_LL760: _LL777: _temp776= _temp752.f1; goto _LL775; _LL775: _temp774= _temp752.f2;
goto _LL761; _LL755: return; _LL757:* _temp734= ud; goto _LL753; _LL759: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL761: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp778= tvs0; if( _temp778 == 0){ _throw( Null_Exception);} _temp778->tl;}),
tvs2=({ struct Cyc_List_List* _temp779= tvs2; if( _temp779 == 0){ _throw(
Null_Exception);} _temp779->tl;})){ inst=({ struct Cyc_List_List* _temp780=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp780->hd=(
void*)({ struct _tuple4* _temp781=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp781->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp785= tvs2; if( _temp785 == 0){ _throw( Null_Exception);} _temp785->hd;});
_temp781->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp782=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp782[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp783; _temp783.tag= Cyc_Absyn_VarType; _temp783.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp784= tvs0; if( _temp784 == 0){ _throw( Null_Exception);}
_temp784->hd;}); _temp783;}); _temp782;}); _temp781;}); _temp780->tl= inst;
_temp780;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes, ud2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp786=* v; xprintf("redeclaration of union %.*s has different attributes",
_temp786.last_plus_one - _temp786.curr, _temp786.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp842= ud->fields; if(
_temp842 == 0){ _throw( Null_Exception);} _temp842->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp841= ud2->fields; if(
_temp841 == 0){ _throw( Null_Exception);} _temp841->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp787= f1s; if( _temp787 == 0){ _throw(
Null_Exception);} _temp787->tl;}), f2s=({ struct Cyc_List_List* _temp788= f2s;
if( _temp788 == 0){ _throw( Null_Exception);} _temp788->tl;})){ struct Cyc_Absyn_Structfield
_temp792; struct Cyc_List_List* _temp793; struct Cyc_Core_Opt* _temp795; void*
_temp797; struct Cyc_Absyn_Tqual _temp799; struct _tagged_string* _temp801;
struct Cyc_Absyn_Structfield* _temp790=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp789= f1s; if( _temp789 == 0){ _throw( Null_Exception);}
_temp789->hd;}); _temp792=* _temp790; _LL802: _temp801=( struct _tagged_string*)
_temp792.name; goto _LL800; _LL800: _temp799=( struct Cyc_Absyn_Tqual) _temp792.tq;
goto _LL798; _LL798: _temp797=( void*) _temp792.type; goto _LL796; _LL796:
_temp795=( struct Cyc_Core_Opt*) _temp792.width; goto _LL794; _LL794: _temp793=(
struct Cyc_List_List*) _temp792.attributes; goto _LL791; _LL791: { struct Cyc_Absyn_Structfield
_temp806; struct Cyc_List_List* _temp807; struct Cyc_Core_Opt* _temp809; void*
_temp811; struct Cyc_Absyn_Tqual _temp813; struct _tagged_string* _temp815;
struct Cyc_Absyn_Structfield* _temp804=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp803= f2s; if( _temp803 == 0){ _throw( Null_Exception);}
_temp803->hd;}); _temp806=* _temp804; _LL816: _temp815=( struct _tagged_string*)
_temp806.name; goto _LL814; _LL814: _temp813=( struct Cyc_Absyn_Tqual) _temp806.tq;
goto _LL812; _LL812: _temp811=( void*) _temp806.type; goto _LL810; _LL810:
_temp809=( struct Cyc_Core_Opt*) _temp806.width; goto _LL808; _LL808: _temp807=(
struct Cyc_List_List*) _temp806.attributes; goto _LL805; _LL805: if( Cyc_String_zstrptrcmp(
_temp801, _temp815) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp817=* v; struct _tagged_string _temp818=* _temp801; struct _tagged_string
_temp819=* _temp815; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp817.last_plus_one - _temp817.curr, _temp817.curr, _temp818.last_plus_one -
_temp818.curr, _temp818.curr, _temp819.last_plus_one - _temp819.curr, _temp819.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp793, _temp807)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp820=* v; struct _tagged_string _temp821=* _temp801; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp820.last_plus_one - _temp820.curr, _temp820.curr, _temp821.last_plus_one -
_temp821.curr, _temp821.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp799,
_temp813)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp822=* v; struct
_tagged_string _temp823=* _temp801; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp822.last_plus_one - _temp822.curr, _temp822.curr, _temp823.last_plus_one -
_temp823.curr, _temp823.curr);}));} if( _temp795 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp824= _temp795; if(
_temp824 == 0){ _throw( Null_Exception);} _temp824->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp825=* v; struct _tagged_string _temp826=*
_temp801; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp825.last_plus_one
- _temp825.curr, _temp825.curr, _temp826.last_plus_one - _temp826.curr, _temp826.curr);}));}
else{ if((( _temp795 != 0? _temp809 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp827= _temp795; if( _temp827
== 0){ _throw( Null_Exception);} _temp827->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp828= _temp809; if( _temp828
== 0){ _throw( Null_Exception);} _temp828->v;})): 0)? 1: _temp795 != _temp809){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp829=* v; struct
_tagged_string _temp830=* _temp801; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp829.last_plus_one - _temp829.curr, _temp829.curr, _temp830.last_plus_one -
_temp830.curr, _temp830.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp797);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp811); if( ! Cyc_Tcutil_unify( _temp797, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp831=* v; struct _tagged_string _temp832=* _temp801;
struct _tagged_string _temp833= Cyc_Absynpp_typ2string( _temp797); struct
_tagged_string _temp834= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp831.last_plus_one - _temp831.curr, _temp831.curr, _temp832.last_plus_one -
_temp832.curr, _temp832.curr, _temp833.last_plus_one - _temp833.curr, _temp833.curr,
_temp834.last_plus_one - _temp834.curr, _temp834.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp836=* v; struct
_tagged_string _temp837=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp835= f2s; if( _temp835 == 0){ _throw( Null_Exception);} _temp835->hd;}))->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp836.last_plus_one
- _temp836.curr, _temp836.curr, _temp837.last_plus_one - _temp837.curr, _temp837.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp839=* v;
struct _tagged_string _temp840=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp838= f1s; if( _temp838 == 0){ _throw( Null_Exception);} _temp838->hd;}))->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp839.last_plus_one
- _temp839.curr, _temp839.curr, _temp840.last_plus_one - _temp840.curr, _temp840.curr);}));}
return;}} _LL753:;} goto _LL728;}} _LL728:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp843=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp843->sc=( void*)(( void*) ud->sc); _temp843->name= ud->name; _temp843->tvs=
tvs; _temp843->fields= 0; _temp843->attributes= 0; _temp843;});{ struct
_RegionHandle _temp844= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp844; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp913= ud->fields; if( _temp913 == 0){ _throw( Null_Exception);} _temp913->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp845= fs; if( _temp845 == 0){
_throw( Null_Exception);} _temp845->tl;})){ struct Cyc_Absyn_Structfield
_temp849; struct Cyc_List_List* _temp850; struct Cyc_Core_Opt* _temp852; void*
_temp854; struct Cyc_Absyn_Tqual _temp856; struct _tagged_string* _temp858;
struct Cyc_Absyn_Structfield* _temp847=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp846= fs; if( _temp846 == 0){ _throw( Null_Exception);}
_temp846->hd;}); _temp849=* _temp847; _LL859: _temp858=( struct _tagged_string*)
_temp849.name; goto _LL857; _LL857: _temp856=( struct Cyc_Absyn_Tqual) _temp849.tq;
goto _LL855; _LL855: _temp854=( void*) _temp849.type; goto _LL853; _LL853:
_temp852=( struct Cyc_Core_Opt*) _temp849.width; goto _LL851; _LL851: _temp850=(
struct Cyc_List_List*) _temp849.attributes; goto _LL848; _LL848: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp858)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp860=* _temp858;
xprintf("duplicate field %.*s in union", _temp860.last_plus_one - _temp860.curr,
_temp860.curr);}));} prev_fields=({ struct Cyc_List_List* _temp861=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp861->hd=( void*)
_temp858; _temp861->tl= prev_fields; _temp861;}); Cyc_Tcutil_check_type( loc, te,
tvs,( void*) Cyc_Absyn_MemKind, _temp854); if( _temp852 != 0){ unsigned int w= 0;
if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp862= _temp852; if( _temp862 == 0){ _throw( Null_Exception);} _temp862->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp863=* _temp858; xprintf("bitfield %.*s does not have constant width",
_temp863.last_plus_one - _temp863.curr, _temp863.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp864= _temp852; if( _temp864
== 0){ _throw( Null_Exception);} _temp864->v;}));}{ void* _temp865= Cyc_Tcutil_compress(
_temp854); void* _temp871; void* _temp873; _LL867: if(( unsigned int) _temp865 >
4u?*(( int*) _temp865) == Cyc_Absyn_IntType: 0){ _LL874: _temp873=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp865)->f1; goto _LL872; _LL872: _temp871=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp865)->f2; goto _LL868;} else{
goto _LL869;} _LL869: goto _LL870; _LL868:{ void* _temp875= _temp871; _LL877:
if( _temp875 ==( void*) Cyc_Absyn_B1){ goto _LL878;} else{ goto _LL879;} _LL879:
if( _temp875 ==( void*) Cyc_Absyn_B2){ goto _LL880;} else{ goto _LL881;} _LL881:
if( _temp875 ==( void*) Cyc_Absyn_B4){ goto _LL882;} else{ goto _LL883;} _LL883:
if( _temp875 ==( void*) Cyc_Absyn_B8){ goto _LL884;} else{ goto _LL876;} _LL878:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp885=(
char*)"bitfield larger than type"; struct _tagged_string _temp886; _temp886.curr=
_temp885; _temp886.base= _temp885; _temp886.last_plus_one= _temp885 + 26;
_temp886;}));} goto _LL876; _LL880: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp887=( char*)"bitfield larger than type"; struct
_tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 26; _temp888;}));} goto _LL876; _LL882: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp889=( char*)"bitfield larger than type";
struct _tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 26; _temp890;}));} goto _LL876; _LL884: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp891=( char*)"bitfield larger than type";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 26; _temp892;}));} goto _LL876; _LL876:;}
goto _LL866; _LL870: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp893=*
_temp858; struct _tagged_string _temp894= Cyc_Absynpp_typ2string( _temp854);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp893.last_plus_one
- _temp893.curr, _temp893.curr, _temp894.last_plus_one - _temp894.curr, _temp894.curr);}));
goto _LL866; _LL866:;}} if( ! Cyc_Tcutil_bits_only( _temp854)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp895=* _temp858; struct _tagged_string _temp896=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp895.last_plus_one - _temp895.curr, _temp895.curr, _temp896.last_plus_one -
_temp896.curr, _temp896.curr);}));} for( 0; _temp850 != 0; _temp850=({ struct
Cyc_List_List* _temp897= _temp850; if( _temp897 == 0){ _throw( Null_Exception);}
_temp897->tl;})){ void* _temp899=( void*)({ struct Cyc_List_List* _temp898=
_temp850; if( _temp898 == 0){ _throw( Null_Exception);} _temp898->hd;}); int
_temp907; _LL901: if( _temp899 ==( void*) Cyc_Absyn_Packed_att){ goto _LL902;}
else{ goto _LL903;} _LL903: if(( unsigned int) _temp899 > 15u?*(( int*) _temp899)
== Cyc_Absyn_Aligned_att: 0){ _LL908: _temp907=( int)(( struct Cyc_Absyn_Aligned_att_struct*)
_temp899)->f1; goto _LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL902:
continue; _LL904: continue; _LL906: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp910= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp909= _temp850; if( _temp909 == 0){ _throw( Null_Exception);} _temp909->hd;}));
struct _tagged_string _temp911=* v; struct _tagged_string _temp912=* _temp858;
xprintf("bad attribute %.*s in union %.*s, member %.*s", _temp910.last_plus_one
- _temp910.curr, _temp910.curr, _temp911.last_plus_one - _temp911.curr, _temp911.curr,
_temp912.last_plus_one - _temp912.curr, _temp912.curr);})); goto _LL900; _LL900:;}}};
_pop_region(& _temp844);}* udp= ud;}}}} static void Cyc_Tc_tcTuniondecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Tuniondecl* tud){ struct _tagged_string* v=(* tud->name).f2;
struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0;
tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp915= tvs2; if( _temp915 == 0){
_throw( Null_Exception);} _temp915->tl;})){ struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp931= tvs2; if( _temp931 ==
0){ _throw( Null_Exception);} _temp931->hd;}))->kind); void* _temp916=( void*) c->v;
void* _temp924; _LL918: if( _temp916 ==( void*) Cyc_Absyn_No_constr){ goto
_LL919;} else{ goto _LL920;} _LL920: if(( unsigned int) _temp916 > 1u?*(( int*)
_temp916) == Cyc_Absyn_Eq_constr: 0){ _LL925: _temp924=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp916)->f1; if( _temp924 ==( void*) Cyc_Absyn_MemKind){ goto _LL921;} else{
goto _LL922;}} else{ goto _LL922;} _LL922: goto _LL923; _LL919:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp926=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp926[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp927; _temp927.tag= Cyc_Absyn_Eq_constr; _temp927.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp927;}); _temp926;}))); goto _LL917;
_LL921: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp929=* v; struct
_tagged_string _temp930=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp928= tvs2; if( _temp928 == 0){ _throw( Null_Exception);} _temp928->hd;}))->name;
xprintf("[x]tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp929.last_plus_one - _temp929.curr, _temp929.curr, _temp930.last_plus_one -
_temp930.curr, _temp930.curr);})); goto _LL917; _LL923: goto _LL917; _LL917:;}}{
struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp932; _push_handler(& _temp932);{ int _temp934= 0; if( setjmp( _temp932.handler)){
_temp934= 1;} if( ! _temp934){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp933=( void*) _exn_thrown; void*
_temp936= _temp933; _LL938: if( _temp936 == Cyc_Dict_Absent){ goto _LL939;}
else{ goto _LL940;} _LL940: goto _LL941; _LL939: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp942= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp942.last_plus_one - _temp942.curr, _temp942.curr);})); return; _LL941:(
void) _throw( _temp936); _LL937:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp943= tud_opt; if( _temp943
== 0){ _throw( Null_Exception);} _temp943->v;})))->name;}} else{{ void* _temp944=(*
tud->name).f1; struct Cyc_List_List* _temp952; struct Cyc_List_List* _temp954;
_LL946: if(( unsigned int) _temp944 > 1u?*(( int*) _temp944) == Cyc_Absyn_Rel_n:
0){ _LL953: _temp952=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp944)->f1; if( _temp952 == 0){ goto _LL947;} else{ goto _LL948;}} else{ goto
_LL948;} _LL948: if(( unsigned int) _temp944 > 1u?*(( int*) _temp944) == Cyc_Absyn_Abs_n:
0){ _LL955: _temp954=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp944)->f1; goto _LL949;} else{ goto _LL950;} _LL950: goto _LL951; _LL947:(*
tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp956=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp956[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp957; _temp957.tag= Cyc_Absyn_Abs_n; _temp957.f1= te->ns; _temp957;});
_temp956;}); goto _LL945; _LL949: goto _LL951; _LL951: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp958=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp959; _temp959.curr= _temp958; _temp959.base= _temp958;
_temp959.last_plus_one= _temp958 + 43; _temp959;})); return; _LL945:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_List_List* prev_fields;{ struct Cyc_Core_Opt* _temp960=
tud_opt; struct Cyc_Core_Opt _temp966; struct Cyc_Absyn_Tuniondecl** _temp967;
_LL962: if( _temp960 == 0){ goto _LL963;} else{ goto _LL964;} _LL964: if(
_temp960 == 0){ goto _LL961;} else{ _temp966=* _temp960; _LL968: _temp967=(
struct Cyc_Absyn_Tuniondecl**) _temp966.v; goto _LL965;} _LL963: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp969=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp969[ 0]= tud; _temp969;}));
if( tud->fields == 0){ return;} prev_fields= 0; goto _LL961; _LL965: { struct
Cyc_Absyn_Tuniondecl* tud2=* _temp967; struct Cyc_List_List* tvs2= tud2->tvs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp970=* v; xprintf("redeclaration of [x]tunion %.*s has a different number of type parameters",
_temp970.last_plus_one - _temp970.curr, _temp970.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp971= x1; if( _temp971 == 0){ _throw( Null_Exception);} _temp971->tl;}), x2=({
struct Cyc_List_List* _temp972= x2; if( _temp972 == 0){ _throw( Null_Exception);}
_temp972->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp981= x1; if( _temp981 == 0){ _throw( Null_Exception);}
_temp981->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp980= x2; if( _temp980 == 0){ _throw( Null_Exception);}
_temp980->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp973=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp973[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp974; _temp974.tag= Cyc_Absyn_Forward_constr; _temp974.f1= c2; _temp974;});
_temp973;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp976=* v; struct _tagged_string _temp977= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp978=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp975= x1; if( _temp975 == 0){ _throw( Null_Exception);} _temp975->hd;}))->name;
struct _tagged_string _temp979= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of [x]tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp976.last_plus_one - _temp976.curr, _temp976.curr, _temp977.last_plus_one -
_temp977.curr, _temp977.curr, _temp978.last_plus_one - _temp978.curr, _temp978.curr,
_temp979.last_plus_one - _temp979.curr, _temp979.curr);}));}}( void*)( tud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) tud->sc, tud->fields,(
void*) tud2->sc, tud2->fields, loc,( struct _tagged_string)({ char* _temp982=(
char*)"[x]tunion"; struct _tagged_string _temp983; _temp983.curr= _temp982;
_temp983.base= _temp982; _temp983.last_plus_one= _temp982 + 10; _temp983;}), v));
if( tud->is_xtunion != tud2->is_xtunion){ Cyc_Tcutil_terr( loc, xprintf("redeclaration of tunion as xtunion or vice-versa"));}{
struct _tuple7 _temp985=({ struct _tuple7 _temp984; _temp984.f1= tud->fields;
_temp984.f2= tud2->fields; _temp984;}); struct Cyc_Core_Opt* _temp995; struct
Cyc_Core_Opt* _temp997; struct Cyc_Core_Opt* _temp999; struct Cyc_Core_Opt*
_temp1001; struct Cyc_Core_Opt* _temp1003; struct Cyc_Core_Opt* _temp1005;
struct Cyc_Core_Opt* _temp1007; struct Cyc_Core_Opt* _temp1009; _LL987: _LL998:
_temp997= _temp985.f1; if( _temp997 == 0){ goto _LL996;} else{ goto _LL989;}
_LL996: _temp995= _temp985.f2; if( _temp995 == 0){ goto _LL988;} else{ goto
_LL989;} _LL989: _LL1002: _temp1001= _temp985.f1; if( _temp1001 == 0){ goto
_LL1000;} else{ goto _LL991;} _LL1000: _temp999= _temp985.f2; goto _LL990;
_LL991: _LL1006: _temp1005= _temp985.f1; goto _LL1004; _LL1004: _temp1003=
_temp985.f2; if( _temp1003 == 0){ goto _LL992;} else{ goto _LL993;} _LL993:
_LL1010: _temp1009= _temp985.f1; goto _LL1008; _LL1008: _temp1007= _temp985.f2;
goto _LL994; _LL988: return; _LL990: tud->fields= tud2->fields; tud->tvs= tvs2;
return; _LL992: prev_fields= 0;* _temp967= tud; goto _LL986; _LL994: { struct
Cyc_List_List* inst= 0;{ struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0;
tvs0=({ struct Cyc_List_List* _temp1011= tvs0; if( _temp1011 == 0){ _throw(
Null_Exception);} _temp1011->tl;}), tvs2=({ struct Cyc_List_List* _temp1012=
tvs2; if( _temp1012 == 0){ _throw( Null_Exception);} _temp1012->tl;})){ inst=({
struct Cyc_List_List* _temp1013=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1013->hd=( void*)({ struct _tuple4* _temp1014=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp1014->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp1018= tvs2; if( _temp1018 == 0){ _throw(
Null_Exception);} _temp1018->hd;}); _temp1014->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp1015=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp1015[ 0]=({ struct Cyc_Absyn_VarType_struct _temp1016; _temp1016.tag= Cyc_Absyn_VarType;
_temp1016.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1017= tvs0;
if( _temp1017 == 0){ _throw( Null_Exception);} _temp1017->hd;}); _temp1016;});
_temp1015;}); _temp1014;}); _temp1013->tl= inst; _temp1013;});}}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1058= tud->fields; if(
_temp1058 == 0){ _throw( Null_Exception);} _temp1058->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1057= tud2->fields; if(
_temp1057 == 0){ _throw( Null_Exception);} _temp1057->v;});{ struct Cyc_List_List*
fs= f1s; for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1019= fs; if(
_temp1019 == 0){ _throw( Null_Exception);} _temp1019->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1046= fs; if(
_temp1046 == 0){ _throw( Null_Exception);} _temp1046->hd;}); struct Cyc_List_List*
alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars( f->loc,
alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({ struct
Cyc_List_List* _temp1020= typs; if( _temp1020 == 0){ _throw( Null_Exception);}
_temp1020->tl;})){ Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp1021= typs; if( _temp1021 == 0){
_throw( Null_Exception);} _temp1021->hd;}))).f2);}}{ void* _temp1022=(* f->name).f1;
struct Cyc_List_List* _temp1032; struct Cyc_List_List* _temp1034; struct Cyc_List_List*
_temp1036; _LL1024: if(( unsigned int) _temp1022 > 1u?*(( int*) _temp1022) ==
Cyc_Absyn_Rel_n: 0){ _LL1033: _temp1032=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1022)->f1; if( _temp1032 == 0){ goto _LL1025;} else{ goto _LL1026;}} else{
goto _LL1026;} _LL1026: if(( unsigned int) _temp1022 > 1u?*(( int*) _temp1022)
== Cyc_Absyn_Rel_n: 0){ _LL1035: _temp1034=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1022)->f1; goto _LL1027;} else{ goto _LL1028;} _LL1028: if(( unsigned int)
_temp1022 > 1u?*(( int*) _temp1022) == Cyc_Absyn_Abs_n: 0){ _LL1037: _temp1036=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1022)->f1; goto
_LL1029;} else{ goto _LL1030;} _LL1030: if( _temp1022 ==( void*) Cyc_Absyn_Loc_n){
goto _LL1031;} else{ goto _LL1023;} _LL1025: if( tud->is_xtunion){(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1038=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1038[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1039; _temp1039.tag= Cyc_Absyn_Abs_n; _temp1039.f1= te->ns; _temp1039;});
_temp1038;});} else{(* f->name).f1=(* tud->name).f1;} goto _LL1023; _LL1027: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1040=( char*)"qualified declarations are not allowed";
struct _tagged_string _temp1041; _temp1041.curr= _temp1040; _temp1041.base=
_temp1040; _temp1041.last_plus_one= _temp1040 + 39; _temp1041;})); goto _LL1023;
_LL1029: goto _LL1023; _LL1031:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1042=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1042[ 0]=({ struct Cyc_Core_Impossible_struct _temp1043; _temp1043.tag= Cyc_Core_Impossible;
_temp1043.f1=( struct _tagged_string)({ char* _temp1044=( char*)"tcTuniondecl: Loc_n";
struct _tagged_string _temp1045; _temp1045.curr= _temp1044; _temp1045.base=
_temp1044; _temp1045.last_plus_one= _temp1044 + 20; _temp1045;}); _temp1043;});
_temp1042;})); goto _LL1023; _LL1023:;}}} if( ! tud->is_xtunion){ for( 0; f1s !=
0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1047= f1s; if( _temp1047 == 0){
_throw( Null_Exception);} _temp1047->tl;}), f2s=({ struct Cyc_List_List*
_temp1048= f2s; if( _temp1048 == 0){ _throw( Null_Exception);} _temp1048->tl;})){
Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1049= f1s; if( _temp1049 == 0){ _throw( Null_Exception);} _temp1049->hd;}),(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1050= f2s; if(
_temp1050 == 0){ _throw( Null_Exception);} _temp1050->hd;}), inst, te, v);} if(
f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1052=* v; struct
_tagged_string _temp1053=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1051= f1s; if( _temp1051 == 0){ _throw( Null_Exception);} _temp1051->hd;}))->name).f2;
xprintf("redeclaration of tunion %.*s has extra field %.*s", _temp1052.last_plus_one
- _temp1052.curr, _temp1052.curr, _temp1053.last_plus_one - _temp1053.curr,
_temp1053.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1055=* v; struct _tagged_string _temp1056=*(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1054= f2s; if( _temp1054 == 0){ _throw(
Null_Exception);} _temp1054->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp1055.last_plus_one - _temp1055.curr, _temp1055.curr, _temp1056.last_plus_one
- _temp1056.curr, _temp1056.curr);}));} return;} prev_fields= f2s; goto _LL986;}}
_LL986:;} goto _LL961;}} _LL961:;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1094= tud->fields; if( _temp1094 == 0){ _throw(
Null_Exception);} _temp1094->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1059= fs; if( _temp1059 == 0){ _throw( Null_Exception);} _temp1059->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1093= fs; if( _temp1093 == 0){ _throw( Null_Exception);} _temp1093->hd;});
if( tud->is_xtunion){ struct Cyc_List_List* prevs= prev_fields; for( 0; prevs !=
0; prevs=({ struct Cyc_List_List* _temp1060= prevs; if( _temp1060 == 0){ _throw(
Null_Exception);} _temp1060->tl;})){ if( Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1061= prevs; if( _temp1061 == 0){ _throw(
Null_Exception);} _temp1061->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1062= fs; if( _temp1062 == 0){ _throw( Null_Exception);}
_temp1062->hd;}))->name) == 0){ Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1063= prevs; if( _temp1063 == 0){ _throw(
Null_Exception);} _temp1063->hd;}),( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1064= fs; if( _temp1064 == 0){ _throw( Null_Exception);} _temp1064->hd;}),
0, te, v); break;}} if( prevs != 0){ continue;}} else{ struct Cyc_List_List*
prevs= prev_fields; for( 0; prevs != 0; prevs=({ struct Cyc_List_List* _temp1065=
prevs; if( _temp1065 == 0){ _throw( Null_Exception);} _temp1065->tl;})){ if( Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1066= prevs; if(
_temp1066 == 0){ _throw( Null_Exception);} _temp1066->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1067= fs; if( _temp1067 == 0){ _throw( Null_Exception);}
_temp1067->hd;}))->name) == 0){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp1068=*(* f->name).f2; xprintf("duplicate field name %.*s", _temp1068.last_plus_one
- _temp1068.curr, _temp1068.curr);}));}}} prev_fields=({ struct Cyc_List_List*
_temp1069=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1069->hd=( void*) f; _temp1069->tl= prev_fields; _temp1069;});{ struct Cyc_List_List*
tvs= f->tvs; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1070= tvs; if(
_temp1070 == 0){ _throw( Null_Exception);} _temp1070->tl;})){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1084= tvs; if(
_temp1084 == 0){ _throw( Null_Exception);} _temp1084->hd;}); struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp1071=( void*) c->v; void* _temp1079; _LL1073: if(
_temp1071 ==( void*) Cyc_Absyn_No_constr){ goto _LL1074;} else{ goto _LL1075;}
_LL1075: if(( unsigned int) _temp1071 > 1u?*(( int*) _temp1071) == Cyc_Absyn_Eq_constr:
0){ _LL1080: _temp1079=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1071)->f1;
if( _temp1079 ==( void*) Cyc_Absyn_MemKind){ goto _LL1076;} else{ goto _LL1077;}}
else{ goto _LL1077;} _LL1077: goto _LL1078; _LL1074:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1081=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1081[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1082; _temp1082.tag= Cyc_Absyn_Eq_constr;
_temp1082.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1082;}); _temp1081;})));
goto _LL1072; _LL1076: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1083=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1083.last_plus_one
- _temp1083.curr, _temp1083.curr);})); goto _LL1072; _LL1078: goto _LL1072;
_LL1072:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1085= typs; if( _temp1085 == 0){ _throw(
Null_Exception);} _temp1085->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1086= typs; if( _temp1086 == 0){ _throw( Null_Exception);} _temp1086->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1087=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1087[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1088; _temp1088.tag= Cyc_Absyn_Abs_n; _temp1088.f1= te->ns; _temp1088;});
_temp1087;});{ struct Cyc_Absyn_Tuniondecl* tudres= tud; if( tud_opt != 0){
tudres=*(( struct Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp1089=
tud_opt; if( _temp1089 == 0){ _throw( Null_Exception);} _temp1089->v;}));} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(* f->name).f2,({ struct
_tuple6* _temp1090=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp1090->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp1091=( struct
Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp1091[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp1092; _temp1092.tag= Cyc_Tcenv_TunionRes;
_temp1092.f1= tudres; _temp1092.f2= f; _temp1092;}); _temp1091;}); _temp1090->f2=
1; _temp1090;}));}}}}}}} static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl*
ed){ struct _tagged_string* v=(* ed->name).f2;{ void* _temp1095=(* ed->name).f1;
struct Cyc_List_List* _temp1103; struct Cyc_List_List* _temp1105; _LL1097: if((
unsigned int) _temp1095 > 1u?*(( int*) _temp1095) == Cyc_Absyn_Rel_n: 0){
_LL1104: _temp1103=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1095)->f1; if( _temp1103 == 0){ goto _LL1098;} else{ goto _LL1099;}} else{
goto _LL1099;} _LL1099: if(( unsigned int) _temp1095 > 1u?*(( int*) _temp1095)
== Cyc_Absyn_Abs_n: 0){ _LL1106: _temp1105=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1095)->f1; if( _temp1105 == 0){ goto _LL1100;} else{ goto _LL1101;}} else{
goto _LL1101;} _LL1101: goto _LL1102; _LL1098: goto _LL1096; _LL1100: goto
_LL1096; _LL1102: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1107=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base= _temp1107;
_temp1108.last_plus_one= _temp1107 + 43; _temp1108;})); return; _LL1096:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1109=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1109[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1110; _temp1110.tag= Cyc_Absyn_Abs_n; _temp1110.f1= te->ns; _temp1110;});
_temp1109;});{ struct Cyc_Core_Opt* _temp1111=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->enumdecls,
v); struct Cyc_Core_Opt _temp1117; struct Cyc_Absyn_Enumdecl** _temp1118;
_LL1113: if( _temp1111 == 0){ goto _LL1114;} else{ goto _LL1115;} _LL1115: if(
_temp1111 == 0){ goto _LL1112;} else{ _temp1117=* _temp1111; _LL1119: _temp1118=(
struct Cyc_Absyn_Enumdecl**) _temp1117.v; goto _LL1116;} _LL1114: ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct
Cyc_Absyn_Enumdecl** _temp1120=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*)); _temp1120[ 0]= ed; _temp1120;})); goto _LL1112;
_LL1116: { struct Cyc_Absyn_Enumdecl* ed2=* _temp1118;( void*)( ed->sc=( void*)((
void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, 0,( void*) ed2->sc,
0, loc,( struct _tagged_string)({ char* _temp1121=( char*)"enum"; struct
_tagged_string _temp1122; _temp1122.curr= _temp1121; _temp1122.base= _temp1121;
_temp1122.last_plus_one= _temp1121 + 5; _temp1122;}), v));{ struct Cyc_List_List*
f1s= ed->fields; struct Cyc_List_List* f2s= ed2->fields; unsigned int tag_count=
0; for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1123= f1s;
if( _temp1123 == 0){ _throw( Null_Exception);} _temp1123->tl;}), f2s=({ struct
Cyc_List_List* _temp1124= f2s; if( _temp1124 == 0){ _throw( Null_Exception);}
_temp1124->tl;})){ struct Cyc_Absyn_Enumfield _temp1128; struct Cyc_Position_Segment*
_temp1129; struct Cyc_Absyn_Exp* _temp1131; struct Cyc_Absyn_Exp** _temp1133;
struct _tuple0* _temp1134; struct Cyc_Absyn_Enumfield* _temp1126=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp1125= f1s; if( _temp1125 == 0){ _throw(
Null_Exception);} _temp1125->hd;}); _temp1128=* _temp1126; _LL1135: _temp1134=(
struct _tuple0*) _temp1128.name; goto _LL1132; _LL1132: _temp1131=( struct Cyc_Absyn_Exp*)
_temp1128.tag; _temp1133=&(* _temp1126).tag; goto _LL1130; _LL1130: _temp1129=(
struct Cyc_Position_Segment*) _temp1128.loc; goto _LL1127; _LL1127: { struct Cyc_Absyn_Enumfield
_temp1139; struct Cyc_Position_Segment* _temp1140; struct Cyc_Absyn_Exp*
_temp1142; struct _tuple0* _temp1144; struct Cyc_Absyn_Enumfield* _temp1137=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1136= f2s; if(
_temp1136 == 0){ _throw( Null_Exception);} _temp1136->hd;}); _temp1139=*
_temp1137; _LL1145: _temp1144=( struct _tuple0*) _temp1139.name; goto _LL1143;
_LL1143: _temp1142=( struct Cyc_Absyn_Exp*) _temp1139.tag; goto _LL1141; _LL1141:
_temp1140=( struct Cyc_Position_Segment*) _temp1139.loc; goto _LL1138; _LL1138:(*
_temp1134).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1146=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1146[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1147; _temp1147.tag= Cyc_Absyn_Abs_n; _temp1147.f1= te->ns; _temp1147;});
_temp1146;}); if( Cyc_String_zstrptrcmp((* _temp1134).f2,(* _temp1144).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1148=* v; struct
_tagged_string _temp1149=*(* _temp1134).f2; struct _tagged_string _temp1150=*(*
_temp1144).f2; xprintf("redeclaration of enum %.*s: field name mismatch %.*s != %.*s",
_temp1148.last_plus_one - _temp1148.curr, _temp1148.curr, _temp1149.last_plus_one
- _temp1149.curr, _temp1149.curr, _temp1150.last_plus_one - _temp1150.curr,
_temp1150.curr);}));} if(* _temp1133 == 0){* _temp1133=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, _temp1129);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1151=* _temp1133; if(
_temp1151 == 0){ _throw( Null_Exception);} _temp1151;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1152=* v; struct _tagged_string _temp1153=*(*
_temp1134).f2; xprintf("redeclaration of enum %.*s, field %.*s: expression is not constant",
_temp1152.last_plus_one - _temp1152.curr, _temp1152.curr, _temp1153.last_plus_one
- _temp1153.curr, _temp1153.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1157=* _temp1133; if(
_temp1157 == 0){ _throw( Null_Exception);} _temp1157;})); tag_count= t1 + 1; if(
t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1154= _temp1142; if( _temp1154 == 0){ _throw( Null_Exception);} _temp1154;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1155=* v; struct
_tagged_string _temp1156=*(* _temp1134).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1155.last_plus_one - _temp1155.curr, _temp1155.curr, _temp1156.last_plus_one
- _temp1156.curr, _temp1156.curr);}));}}}} return;}} _LL1112:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1158= fs; if( _temp1158 == 0){
_throw( Null_Exception);} _temp1158->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1170= fs; if(
_temp1170 == 0){ _throw( Null_Exception);} _temp1170->hd;}); if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp1159=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp1159.last_plus_one - _temp1159.curr,
_temp1159.curr);}));} prev_fields=({ struct Cyc_List_List* _temp1160=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1160->hd=( void*)(*
f->name).f2; _temp1160->tl= prev_fields; _temp1160;}); if( f->tag == 0){ f->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1161= f->tag; if(
_temp1161 == 0){ _throw( Null_Exception);} _temp1161;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1162=* v; struct _tagged_string _temp1163=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp1162.last_plus_one
- _temp1162.curr, _temp1162.curr, _temp1163.last_plus_one - _temp1163.curr,
_temp1163.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1169= f->tag; if( _temp1169 == 0){
_throw( Null_Exception);} _temp1169;})); tag_count= t1 + 1;(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1164=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1164[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1165; _temp1165.tag= Cyc_Absyn_Abs_n; _temp1165.f1= te->ns; _temp1165;});
_temp1164;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1166=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1166->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1167=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1167[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1168; _temp1168.tag= Cyc_Tcenv_EnumRes;
_temp1168.f1= ed; _temp1168.f2= f; _temp1168;}); _temp1167;}); _temp1166->f2= 1;
_temp1166;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp1171=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp1171[ 0]= ed; _temp1171;}));}}
static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){
void* _temp1172= sc; _LL1174: if( _temp1172 ==( void*) Cyc_Absyn_Static){ goto
_LL1175;} else{ goto _LL1176;} _LL1176: if( _temp1172 ==( void*) Cyc_Absyn_Abstract){
goto _LL1177;} else{ goto _LL1178;} _LL1178: if( _temp1172 ==( void*) Cyc_Absyn_Public){
goto _LL1179;} else{ goto _LL1180;} _LL1180: if( _temp1172 ==( void*) Cyc_Absyn_Extern){
goto _LL1181;} else{ goto _LL1182;} _LL1182: if( _temp1172 ==( void*) Cyc_Absyn_ExternC){
goto _LL1183;} else{ goto _LL1173;} _LL1175: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1184=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1185; _temp1185.curr= _temp1184; _temp1185.base=
_temp1184; _temp1185.last_plus_one= _temp1184 + 44; _temp1185;})); return 0;
_LL1177: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1186=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1187; _temp1187.curr= _temp1186; _temp1187.base=
_temp1186; _temp1187.last_plus_one= _temp1186 + 46; _temp1187;})); return 0;
_LL1179: return 1; _LL1181: return 1; _LL1183: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1188=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1189; _temp1189.curr= _temp1188; _temp1189.base=
_temp1188; _temp1189.last_plus_one= _temp1188 + 30; _temp1189;})); return 1;
_LL1173:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1190= ds; if( _temp1190 == 0){ _throw( Null_Exception);} _temp1190->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1290= ds; if( _temp1290 == 0){ _throw( Null_Exception);} _temp1290->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp1191=( void*) d->r; int
_temp1215; struct Cyc_Absyn_Exp* _temp1217; struct Cyc_Core_Opt* _temp1219;
struct Cyc_Core_Opt* _temp1221; struct Cyc_Absyn_Pat* _temp1223; struct Cyc_Absyn_Vardecl*
_temp1225; struct Cyc_Absyn_Fndecl* _temp1227; struct Cyc_Absyn_Typedefdecl*
_temp1229; struct Cyc_Absyn_Structdecl* _temp1231; struct Cyc_Absyn_Uniondecl*
_temp1233; struct Cyc_Absyn_Tuniondecl* _temp1235; struct Cyc_Absyn_Enumdecl*
_temp1237; struct Cyc_List_List* _temp1239; struct _tagged_string* _temp1241;
struct Cyc_List_List* _temp1243; struct _tuple0* _temp1245; struct _tuple0
_temp1247; struct _tagged_string* _temp1248; void* _temp1250; struct Cyc_List_List*
_temp1252; _LL1193: if(*(( int*) _temp1191) == Cyc_Absyn_Let_d){ _LL1224:
_temp1223=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1191)->f1;
goto _LL1222; _LL1222: _temp1221=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1191)->f2; goto _LL1220; _LL1220: _temp1219=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1191)->f3; goto _LL1218; _LL1218: _temp1217=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1191)->f4; goto
_LL1216; _LL1216: _temp1215=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1191)->f5;
goto _LL1194;} else{ goto _LL1195;} _LL1195: if(*(( int*) _temp1191) == Cyc_Absyn_Var_d){
_LL1226: _temp1225=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1191)->f1; goto _LL1196;} else{ goto _LL1197;} _LL1197: if(*(( int*)
_temp1191) == Cyc_Absyn_Fn_d){ _LL1228: _temp1227=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1191)->f1; goto _LL1198;} else{ goto _LL1199;}
_LL1199: if(*(( int*) _temp1191) == Cyc_Absyn_Typedef_d){ _LL1230: _temp1229=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1191)->f1;
goto _LL1200;} else{ goto _LL1201;} _LL1201: if(*(( int*) _temp1191) == Cyc_Absyn_Struct_d){
_LL1232: _temp1231=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp1191)->f1; goto _LL1202;} else{ goto _LL1203;} _LL1203: if(*(( int*)
_temp1191) == Cyc_Absyn_Union_d){ _LL1234: _temp1233=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp1191)->f1; goto _LL1204;} else{ goto
_LL1205;} _LL1205: if(*(( int*) _temp1191) == Cyc_Absyn_Tunion_d){ _LL1236:
_temp1235=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1191)->f1; goto _LL1206;} else{ goto _LL1207;} _LL1207: if(*(( int*)
_temp1191) == Cyc_Absyn_Enum_d){ _LL1238: _temp1237=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1191)->f1; goto _LL1208;} else{ goto
_LL1209;} _LL1209: if(*(( int*) _temp1191) == Cyc_Absyn_Namespace_d){ _LL1242:
_temp1241=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1191)->f1; goto _LL1240; _LL1240: _temp1239=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1191)->f2; goto _LL1210;} else{ goto
_LL1211;} _LL1211: if(*(( int*) _temp1191) == Cyc_Absyn_Using_d){ _LL1246:
_temp1245=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1191)->f1;
_temp1247=* _temp1245; _LL1251: _temp1250= _temp1247.f1; goto _LL1249; _LL1249:
_temp1248= _temp1247.f2; goto _LL1244; _LL1244: _temp1243=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1191)->f2; goto _LL1212;} else{ goto
_LL1213;} _LL1213: if(*(( int*) _temp1191) == Cyc_Absyn_ExternC_d){ _LL1253:
_temp1252=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1191)->f1; goto _LL1214;} else{ goto _LL1192;} _LL1194: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1254=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp1255; _temp1255.curr= _temp1254; _temp1255.base=
_temp1254; _temp1255.last_plus_one= _temp1254 + 47; _temp1255;})); goto _LL1192;
_LL1196: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1225->sc): 0){(
void*)( _temp1225->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl(
te, ge, loc, _temp1225); goto _LL1192; _LL1198: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1227->sc): 0){( void*)( _temp1227->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1227); goto _LL1192; _LL1200: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1229); goto _LL1192; _LL1202: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1231->sc): 0){( void*)( _temp1231->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1231); goto _LL1192; _LL1204: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1233->sc): 0){( void*)(
_temp1233->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1233); goto _LL1192; _LL1206: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1235->sc): 0){( void*)( _temp1235->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1235); goto _LL1192; _LL1208: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1237->sc): 0){( void*)(
_temp1237->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge,
loc, _temp1237); goto _LL1192; _LL1210: { struct Cyc_List_List* ns= te->ns;
struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1256=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1256->hd=( void*) _temp1241; _temp1256->tl= 0; _temp1256;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1241)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1241); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1239, in_externC); te->ns= ns; goto
_LL1192;} _LL1212: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1257= _temp1250; struct Cyc_List_List* _temp1269; struct Cyc_List_List*
_temp1271; struct Cyc_List_List* _temp1273; struct Cyc_List_List _temp1275;
struct Cyc_List_List* _temp1276; struct _tagged_string* _temp1278; struct Cyc_List_List*
_temp1280; struct Cyc_List_List _temp1282; struct Cyc_List_List* _temp1283;
struct _tagged_string* _temp1285; _LL1259: if( _temp1257 ==( void*) Cyc_Absyn_Loc_n){
goto _LL1260;} else{ goto _LL1261;} _LL1261: if(( unsigned int) _temp1257 > 1u?*((
int*) _temp1257) == Cyc_Absyn_Rel_n: 0){ _LL1270: _temp1269=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1257)->f1; if( _temp1269 == 0){ goto
_LL1262;} else{ goto _LL1263;}} else{ goto _LL1263;} _LL1263: if(( unsigned int)
_temp1257 > 1u?*(( int*) _temp1257) == Cyc_Absyn_Abs_n: 0){ _LL1272: _temp1271=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1257)->f1; if(
_temp1271 == 0){ goto _LL1264;} else{ goto _LL1265;}} else{ goto _LL1265;}
_LL1265: if(( unsigned int) _temp1257 > 1u?*(( int*) _temp1257) == Cyc_Absyn_Rel_n:
0){ _LL1274: _temp1273=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1257)->f1; if( _temp1273 == 0){ goto _LL1267;} else{ _temp1275=* _temp1273;
_LL1279: _temp1278=( struct _tagged_string*) _temp1275.hd; goto _LL1277; _LL1277:
_temp1276=( struct Cyc_List_List*) _temp1275.tl; goto _LL1266;}} else{ goto
_LL1267;} _LL1267: if(( unsigned int) _temp1257 > 1u?*(( int*) _temp1257) == Cyc_Absyn_Abs_n:
0){ _LL1281: _temp1280=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1257)->f1; if( _temp1280 == 0){ goto _LL1258;} else{ _temp1282=* _temp1280;
_LL1286: _temp1285=( struct _tagged_string*) _temp1282.hd; goto _LL1284; _LL1284:
_temp1283=( struct Cyc_List_List*) _temp1282.tl; goto _LL1268;}} else{ goto
_LL1258;} _LL1260: goto _LL1262; _LL1262: goto _LL1264; _LL1264: first=
_temp1248; rest= 0; goto _LL1258; _LL1266: _temp1285= _temp1278; _temp1283=
_temp1276; goto _LL1268; _LL1268: first= _temp1285; rest=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1283,({
struct Cyc_List_List* _temp1287=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1287->hd=( void*) _temp1248; _temp1287->tl= 0;
_temp1287;})); goto _LL1258; _LL1258:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace(
te, loc, first, rest); ge->availables=({ struct Cyc_List_List* _temp1288=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1288->hd=(
void*) ns2; _temp1288->tl= ge->availables; _temp1288;}); Cyc_Tc_tc_decls( te,
_temp1243, in_externC); ge->availables=({ struct Cyc_List_List* _temp1289= ge->availables;
if( _temp1289 == 0){ _throw( Null_Exception);} _temp1289->tl;}); goto _LL1192;}}
_LL1214: Cyc_Tc_tc_decls( te, _temp1252, 1); goto _LL1192; _LL1192:;}} static
char _temp1293[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp1293, _temp1293, _temp1293 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1294=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1294->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1295=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1295->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1296=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1296[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1297; _temp1297.tag=
Cyc_Absyn_Namespace_d; _temp1297.f1= Cyc_Tc_cyc_ns; _temp1297.f2= ds; _temp1297;});
_temp1296;})); _temp1295->loc= 0; _temp1295;}); _temp1294->tl= 0; _temp1294;});}
Cyc_Tc_tc_decls( te, ds, 0);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d); static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple8{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp1298=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1308; struct Cyc_List_List* _temp1310; struct Cyc_List_List**
_temp1312; struct _tuple0* _temp1313; struct Cyc_List_List* _temp1315; struct
Cyc_List_List** _temp1317; struct _tagged_string* _temp1318; _LL1300: if(*(( int*)
_temp1298) == Cyc_Absyn_Var_d){ _LL1309: _temp1308=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1298)->f1; goto _LL1301;} else{ goto
_LL1302;} _LL1302: if(*(( int*) _temp1298) == Cyc_Absyn_Using_d){ _LL1314:
_temp1313=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1298)->f1;
goto _LL1311; _LL1311: _temp1310=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1298)->f2; _temp1312=&(( struct Cyc_Absyn_Using_d_struct*) _temp1298)->f2;
goto _LL1303;} else{ goto _LL1304;} _LL1304: if(*(( int*) _temp1298) == Cyc_Absyn_Namespace_d){
_LL1319: _temp1318=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1298)->f1; goto _LL1316; _LL1316: _temp1315=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1298)->f2; _temp1317=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1298)->f2; goto _LL1305;} else{ goto _LL1306;} _LL1306: goto _LL1307;
_LL1301: if(( void*) _temp1308->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{
struct _tuple0 _temp1322; struct _tagged_string* _temp1323; void* _temp1325;
struct _tuple0* _temp1320= _temp1308->name; _temp1322=* _temp1320; _LL1326:
_temp1325= _temp1322.f1; goto _LL1324; _LL1324: _temp1323= _temp1322.f2; goto
_LL1321; _LL1321: { struct Cyc_List_List* ns;{ void* _temp1327= _temp1325;
struct Cyc_List_List* _temp1335; struct Cyc_List_List* _temp1337; _LL1329: if(
_temp1327 ==( void*) Cyc_Absyn_Loc_n){ goto _LL1330;} else{ goto _LL1331;}
_LL1331: if(( unsigned int) _temp1327 > 1u?*(( int*) _temp1327) == Cyc_Absyn_Rel_n:
0){ _LL1336: _temp1335=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1327)->f1; goto _LL1332;} else{ goto _LL1333;} _LL1333: if(( unsigned int)
_temp1327 > 1u?*(( int*) _temp1327) == Cyc_Absyn_Abs_n: 0){ _LL1338: _temp1337=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1327)->f1; goto
_LL1334;} else{ goto _LL1328;} _LL1330: ns= 0; goto _LL1328; _LL1332: ns=
_temp1335; goto _LL1328; _LL1334: ns= _temp1337; goto _LL1328; _LL1328:;}{
struct _tuple8* nsenv=(( struct _tuple8*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( env, ns); struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; int
needed=(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->ordinaries, _temp1323)).f2; if( ! needed){(* nsenv).f2=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((*
nsenv).f2, _temp1323);} return needed;}}} _LL1303: _temp1317= _temp1312; goto
_LL1305; _LL1305:* _temp1317= Cyc_Tc_treeshake_f( env,* _temp1317); return 1;
_LL1307: return 1; _LL1299:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_string* x, struct _tuple6* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple8* nsenv){ struct Cyc_Tcenv_Genv* ge=(*
nsenv).f1; struct Cyc_Set_Set* set=(* nsenv).f2; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple6*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
set, ge->ordinaries);} static struct _tuple8* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple8* _temp1339=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1339->f1= ge; _temp1339->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp1339;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* env=((
struct Cyc_Dict_Dict*(*)( struct _tuple8*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* ds1= Cyc_Tc_treeshake_f( env, ds);(( void(*)( void(* f)( struct
Cyc_List_List*, struct _tuple8*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
env); return ds1;}