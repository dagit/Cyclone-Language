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
int _temp40; _LL28: if(( unsigned int) _temp26 > 15u?*(( int*) _temp26) == Cyc_Absyn_Regparm_att:
0){ _LL41: _temp40=(( struct Cyc_Absyn_Regparm_att_struct*) _temp26)->f1; goto
_LL29;} else{ goto _LL30;} _LL30: if( _temp26 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL31;} else{ goto _LL32;} _LL32: if( _temp26 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp26 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp26 ==( void*) Cyc_Absyn_Const_att){
goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL29: goto _LL31; _LL31:
goto _LL33; _LL33: goto _LL35; _LL35: goto _LL37; _LL37:* _temp13=({ struct Cyc_List_List*
_temp42=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp42->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
_temp42->tl=* _temp13; _temp42;}); continue; _LL39: _temp24=({ struct Cyc_List_List*
_temp43=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp43->hd=( void*)(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
_temp43->tl= _temp24; _temp43;}); continue; _LL27:;} return _temp24;} _LL8:
return(( struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
char* _temp44="transfer_fn_type_atts"; struct _tagged_string _temp45; _temp45.curr=
_temp44; _temp45.base= _temp44; _temp45.last_plus_one= _temp44 + 22; _temp45;}));
_LL4:;} struct _tuple3{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd, int check_var_init){ struct _tagged_string* v=(*
vd->name).f2; void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List*
atts= vd->attributes;{ void* _temp46=(* vd->name).f1; struct Cyc_List_List*
_temp54; struct Cyc_List_List* _temp56; _LL48: if(( unsigned int) _temp46 > 1u?*((
int*) _temp46) == Cyc_Absyn_Rel_n: 0){ _LL55: _temp54=(( struct Cyc_Absyn_Rel_n_struct*)
_temp46)->f1; if( _temp54 == 0){ goto _LL49;} else{ goto _LL50;}} else{ goto
_LL50;} _LL50: if(( unsigned int) _temp46 > 1u?*(( int*) _temp46) == Cyc_Absyn_Abs_n:
0){ _LL57: _temp56=(( struct Cyc_Absyn_Abs_n_struct*) _temp46)->f1; if( _temp56
== 0){ goto _LL51;} else{ goto _LL52;}} else{ goto _LL52;} _LL52: goto _LL53;
_LL49: goto _LL47; _LL51: goto _LL47; _LL53: Cyc_Tcutil_terr( loc,({ char*
_temp58="qualified declarations are not implemented"; struct _tagged_string
_temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 43; _temp59;})); return; _LL47:;}(* vd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp60=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp60[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp61; _temp61.tag= Cyc_Absyn_Abs_n;
_temp61.f1= te->ns; _temp61;}); _temp60;});{ void* _temp62= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Tqual _temp71; void* _temp73;
_LL64: if(( unsigned int) _temp62 > 4u?*(( int*) _temp62) == Cyc_Absyn_ArrayType:
0){ _LL74: _temp73=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp62)->f1;
goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_ArrayType_struct*) _temp62)->f2;
goto _LL70; _LL70: _temp69=(( struct Cyc_Absyn_ArrayType_struct*) _temp62)->f3;
if( _temp69 == 0){ goto _LL68;} else{ goto _LL66;}} else{ goto _LL66;} _LL68:
if( vd->initializer != 0){ goto _LL65;} else{ goto _LL66;} _LL66: goto _LL67;
_LL65:{ void* _temp75=( void*)(( struct Cyc_Absyn_Exp*) _check_null( vd->initializer))->r;
void* _temp85; struct _tagged_string _temp87; struct Cyc_List_List* _temp89;
struct Cyc_Core_Opt* _temp91; struct Cyc_List_List* _temp93; _LL77: if(*(( int*)
_temp75) == Cyc_Absyn_Const_e){ _LL86: _temp85=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp75)->f1; if(( unsigned int) _temp85 > 1u?*(( int*) _temp85) == Cyc_Absyn_String_c:
0){ _LL88: _temp87=(( struct Cyc_Absyn_String_c_struct*) _temp85)->f1; goto
_LL78;} else{ goto _LL79;}} else{ goto _LL79;} _LL79: if(*(( int*) _temp75) ==
Cyc_Absyn_UnresolvedMem_e){ _LL92: _temp91=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp75)->f1; goto _LL90; _LL90: _temp89=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp75)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if(*(( int*) _temp75) ==
Cyc_Absyn_Array_e){ _LL94: _temp93=(( struct Cyc_Absyn_Array_e_struct*) _temp75)->f1;
goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL78: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp95=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp95[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp96; _temp96.tag= Cyc_Absyn_ArrayType; _temp96.f1=(
void*) _temp73; _temp96.f2= _temp71; _temp96.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(({
struct _tagged_string _temp97= _temp87;( unsigned int)( _temp97.last_plus_one -
_temp97.curr);}), 0); _temp96;}); _temp95;}))); goto _LL76; _LL80: _temp93=
_temp89; goto _LL82; _LL82: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp98=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp98[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp99; _temp99.tag= Cyc_Absyn_ArrayType;
_temp99.f1=( void*) _temp73; _temp99.f2= _temp71; _temp99.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp93), 0); _temp99;}); _temp98;}))); goto _LL76; _LL84: goto _LL76; _LL76:;}
goto _LL63; _LL67: goto _LL63; _LL63:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,({ char* _temp100="extern declaration should not have initializer";
struct _tagged_string _temp101; _temp101.curr= _temp100; _temp101.base= _temp100;
_temp101.last_plus_one= _temp100 + 47; _temp101;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp102=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; int
_temp122; struct _tagged_string _temp124; _LL104: if(( unsigned int) _temp102 >
15u?*(( int*) _temp102) == Cyc_Absyn_Aligned_att: 0){ _LL123: _temp122=(( struct
Cyc_Absyn_Aligned_att_struct*) _temp102)->f1; goto _LL105;} else{ goto _LL106;}
_LL106: if(( unsigned int) _temp102 > 15u?*(( int*) _temp102) == Cyc_Absyn_Section_att:
0){ _LL125: _temp124=(( struct Cyc_Absyn_Section_att_struct*) _temp102)->f1;
goto _LL107;} else{ goto _LL108;} _LL108: if( _temp102 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL109;} else{ goto _LL110;} _LL110: if( _temp102 ==( void*) Cyc_Absyn_Shared_att){
goto _LL111;} else{ goto _LL112;} _LL112: if( _temp102 ==( void*) Cyc_Absyn_Unused_att){
goto _LL113;} else{ goto _LL114;} _LL114: if( _temp102 ==( void*) Cyc_Absyn_Weak_att){
goto _LL115;} else{ goto _LL116;} _LL116: if( _temp102 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL117;} else{ goto _LL118;} _LL118: if( _temp102 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL119;} else{ goto _LL120;} _LL120: goto _LL121; _LL105: continue; _LL107:
continue; _LL109: continue; _LL111: continue; _LL113: continue; _LL115:
continue; _LL117: continue; _LL119: continue; _LL121: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp126= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp127= Cyc_Absynpp_qvar2string(
vd->name); xprintf("bad attribute %.*s for variable %.*s", _temp126.last_plus_one
- _temp126.curr, _temp126.curr, _temp127.last_plus_one - _temp127.curr, _temp127.curr);}));
goto _LL103; _LL103:;} if( vd->initializer == 0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp128= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp129= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp128.last_plus_one - _temp128.curr, _temp128.curr, _temp129.last_plus_one -
_temp129.curr, _temp129.curr);}));}} else{ struct Cyc_Absyn_Exp* _temp130=(
struct Cyc_Absyn_Exp*) _check_null( vd->initializer); goto _LL131; _LL131: {
void* _temp132= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp130); goto
_LL133; _LL133: if( ! Cyc_Tcutil_coerce_assign( te, _temp130, t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp134= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp135= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp136= Cyc_Absynpp_typ2string( _temp132); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp134.last_plus_one - _temp134.curr, _temp134.curr, _temp135.last_plus_one -
_temp135.curr, _temp135.curr, _temp136.last_plus_one - _temp136.curr, _temp136.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, _temp130)){ Cyc_Tcutil_terr( loc,({ char*
_temp137="initializer is not a constant expression"; struct _tagged_string
_temp138; _temp138.curr= _temp137; _temp138.base= _temp137; _temp138.last_plus_one=
_temp137 + 41; _temp138;}));}}}} else{ for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp139=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp157; int _temp159; _LL141: if(( unsigned int)
_temp139 > 15u?*(( int*) _temp139) == Cyc_Absyn_Regparm_att: 0){ _LL158:
_temp157=(( struct Cyc_Absyn_Regparm_att_struct*) _temp139)->f1; goto _LL142;}
else{ goto _LL143;} _LL143: if( _temp139 ==( void*) Cyc_Absyn_Stdcall_att){ goto
_LL144;} else{ goto _LL145;} _LL145: if( _temp139 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL146;} else{ goto _LL147;} _LL147: if( _temp139 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL148;} else{ goto _LL149;} _LL149: if( _temp139 ==( void*) Cyc_Absyn_Const_att){
goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int) _temp139 > 15u?*((
int*) _temp139) == Cyc_Absyn_Aligned_att: 0){ _LL160: _temp159=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp139)->f1; goto _LL152;} else{ goto _LL153;} _LL153: if( _temp139 ==( void*)
Cyc_Absyn_Packed_att){ goto _LL154;} else{ goto _LL155;} _LL155: goto _LL156;
_LL142: goto _LL144; _LL144: goto _LL146; _LL146: goto _LL148; _LL148: goto
_LL150; _LL150:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char*
_temp161="tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 51; _temp162;})); goto _LL140; _LL152: goto
_LL154; _LL154: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp163= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s in function declaration",
_temp163.last_plus_one - _temp163.curr, _temp163.curr);})); goto _LL140; _LL156:
continue; _LL140:;}}}{ struct _handler_cons _temp164; _push_handler(& _temp164);{
int _temp166= 0; if( setjmp( _temp164.handler)){ _temp166= 1;} if( ! _temp166){{
struct _tuple3* _temp167=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL168; _LL168: {
void* _temp169=(* _temp167).f1; void* _temp175; _LL171: if(*(( int*) _temp169)
== Cyc_Tcenv_VarRes){ _LL176: _temp175=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp169)->f1; goto _LL172;} else{ goto _LL173;} _LL173: goto _LL174; _LL172: {
struct Cyc_Absyn_Global_b_struct* _temp179=({ struct Cyc_Absyn_Global_b_struct*
_temp177=( struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp177[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp178; _temp178.tag= Cyc_Absyn_Global_b;
_temp178.f1= vd; _temp178;}); _temp177;}); goto _LL180; _LL180: { void* _temp181=
Cyc_Tcdecl_merge_binding( _temp175,( void*) _temp179, loc, Cyc_Tc_tc_msg); goto
_LL182; _LL182: if( _temp181 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL170;}
if( _temp181 == _temp175?(* _temp167).f2: 0){ goto _LL170;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp183=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp183->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp184=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp184[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp185; _temp185.tag= Cyc_Tcenv_VarRes; _temp185.f1=( void*) _temp181;
_temp185;}); _temp184;}); _temp183->f2= 1; _temp183;})); goto _LL170;}} _LL174:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp186="tcVardecl: a variable is being declared with a wrong name";
struct _tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 58; _temp187;})); goto _LL170; _LL170:;}};
_pop_handler();} else{ void* _temp165=( void*) _exn_thrown; void* _temp189=
_temp165; _LL191: if( _temp189 == Cyc_Dict_Absent){ goto _LL192;} else{ goto
_LL193;} _LL193: goto _LL194; _LL192: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp195=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp195->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp196=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp196[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp197; _temp197.tag= Cyc_Tcenv_VarRes;
_temp197.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp198=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp198[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp199; _temp199.tag= Cyc_Absyn_Global_b;
_temp199.f1= vd; _temp199;}); _temp198;})); _temp197;}); _temp196;}); _temp195->f2=
0; _temp195;})); goto _LL190; _LL194:( void) _throw( _temp189); _LL190:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp200=(*
fd->name).f1; struct Cyc_List_List* _temp208; struct Cyc_List_List* _temp210;
_LL202: if(( unsigned int) _temp200 > 1u?*(( int*) _temp200) == Cyc_Absyn_Rel_n:
0){ _LL209: _temp208=(( struct Cyc_Absyn_Rel_n_struct*) _temp200)->f1; if(
_temp208 == 0){ goto _LL203;} else{ goto _LL204;}} else{ goto _LL204;} _LL204:
if(( unsigned int) _temp200 > 1u?*(( int*) _temp200) == Cyc_Absyn_Abs_n: 0){
_LL211: _temp210=(( struct Cyc_Absyn_Abs_n_struct*) _temp200)->f1; goto _LL205;}
else{ goto _LL206;} _LL206: goto _LL207; _LL203: goto _LL201; _LL205:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp212=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp212[ 0]=({ struct
Cyc_Core_Impossible_struct _temp213; _temp213.tag= Cyc_Core_Impossible; _temp213.f1=({
char* _temp214="tc: Abs_n in tcFndecl"; struct _tagged_string _temp215; _temp215.curr=
_temp214; _temp215.base= _temp214; _temp215.last_plus_one= _temp214 + 22;
_temp215;}); _temp213;}); _temp212;})); _LL207: Cyc_Tcutil_terr( loc,({ char*
_temp216="qualified declarations are not implemented"; struct _tagged_string
_temp217; _temp217.curr= _temp216; _temp217.base= _temp216; _temp217.last_plus_one=
_temp216 + 43; _temp217;})); return; _LL201:;}(* fd->name).f1=( void*)({ struct
Cyc_Absyn_Abs_n_struct* _temp218=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp218[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp219; _temp219.tag= Cyc_Absyn_Abs_n; _temp219.f1= te->ns; _temp219;});
_temp218;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp220=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; int _temp228; _LL222: if(
_temp220 ==( void*) Cyc_Absyn_Packed_att){ goto _LL223;} else{ goto _LL224;}
_LL224: if(( unsigned int) _temp220 > 15u?*(( int*) _temp220) == Cyc_Absyn_Aligned_att:
0){ _LL229: _temp228=(( struct Cyc_Absyn_Aligned_att_struct*) _temp220)->f1;
goto _LL225;} else{ goto _LL226;} _LL226: goto _LL227; _LL223: goto _LL225;
_LL225: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp230= Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd); xprintf("bad attribute %.*s for function",
_temp230.last_plus_one - _temp230.curr, _temp230.curr);})); goto _LL221; _LL227:
goto _LL221; _LL221:;}}{ struct _handler_cons _temp231; _push_handler(& _temp231);{
int _temp233= 0; if( setjmp( _temp231.handler)){ _temp233= 1;} if( ! _temp233){{
struct _tuple3* _temp234=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); goto _LL235; _LL235: {
void* _temp236=(* _temp234).f1; void* _temp242; _LL238: if(*(( int*) _temp236)
== Cyc_Tcenv_VarRes){ _LL243: _temp242=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp236)->f1; goto _LL239;} else{ goto _LL240;} _LL240: goto _LL241; _LL239: {
struct Cyc_Absyn_Funname_b_struct* _temp246=({ struct Cyc_Absyn_Funname_b_struct*
_temp244=( struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp244[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp245; _temp245.tag= Cyc_Absyn_Funname_b;
_temp245.f1= fd; _temp245;}); _temp244;}); goto _LL247; _LL247: { void* _temp248=
Cyc_Tcdecl_merge_binding( _temp242,( void*) _temp246, loc, Cyc_Tc_tc_msg); goto
_LL249; _LL249: if( _temp248 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL237;}
if( _temp248 == _temp242?(* _temp234).f2: 0){ goto _LL237;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple3*
_temp250=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp250->f1=(
void*)({ struct Cyc_Tcenv_VarRes_struct* _temp251=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp251[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp252; _temp252.tag= Cyc_Tcenv_VarRes; _temp252.f1=( void*) _temp248;
_temp252;}); _temp251;}); _temp250->f2= 1; _temp250;})); goto _LL237;}} _LL241:((
void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({ char* _temp253="tcFndecl: a variable is being declared with a wrong name";
struct _tagged_string _temp254; _temp254.curr= _temp253; _temp254.base= _temp253;
_temp254.last_plus_one= _temp253 + 57; _temp254;})); goto _LL237; _LL237:;}};
_pop_handler();} else{ void* _temp232=( void*) _exn_thrown; void* _temp256=
_temp232; _LL258: if( _temp256 == Cyc_Dict_Absent){ goto _LL259;} else{ goto
_LL260;} _LL260: goto _LL261; _LL259: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp262=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp262->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp263=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp263[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp264; _temp264.tag= Cyc_Tcenv_VarRes;
_temp264.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp265=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp265[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp266; _temp266.tag= Cyc_Absyn_Funname_b;
_temp266.f1= fd; _temp266;}); _temp265;})); _temp264;}); _temp263;}); _temp262->f2=
0; _temp262;})); goto _LL257; _LL261:( void) _throw( _temp256); _LL257:;}}} te->le=({
struct Cyc_Core_Opt* _temp267=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp267->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp268=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp268[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp269; _temp269.tag= Cyc_Tcenv_Outermost;
_temp269.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp269;}); _temp268;}));
_temp267;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,({ char* _temp270="function has goto statements to undefined labels";
struct _tagged_string _temp271; _temp271.curr= _temp270; _temp271.base= _temp270;
_temp271.last_plus_one= _temp270 + 49; _temp271;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp272=(* td->name).f1; struct Cyc_List_List*
_temp280; struct Cyc_List_List* _temp282; _LL274: if(( unsigned int) _temp272 >
1u?*(( int*) _temp272) == Cyc_Absyn_Rel_n: 0){ _LL281: _temp280=(( struct Cyc_Absyn_Rel_n_struct*)
_temp272)->f1; if( _temp280 == 0){ goto _LL275;} else{ goto _LL276;}} else{ goto
_LL276;} _LL276: if(( unsigned int) _temp272 > 1u?*(( int*) _temp272) == Cyc_Absyn_Abs_n:
0){ _LL283: _temp282=(( struct Cyc_Absyn_Abs_n_struct*) _temp272)->f1; if(
_temp282 == 0){ goto _LL277;} else{ goto _LL278;}} else{ goto _LL278;} _LL278:
goto _LL279; _LL275: goto _LL273; _LL277: goto _LL273; _LL279: Cyc_Tcutil_terr(
loc,({ char* _temp284="qualified declarations are not implemented"; struct
_tagged_string _temp285; _temp285.curr= _temp284; _temp285.base= _temp284;
_temp285.last_plus_one= _temp284 + 43; _temp285;})); return; _LL273:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp286=* v; xprintf("redeclaration of typedef %.*s",
_temp286.last_plus_one - _temp286.curr, _temp286.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp287=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp287[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp288; _temp288.tag= Cyc_Absyn_Abs_n; _temp288.f1= te->ns; _temp288;});
_temp287;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp289= _new_region();
struct _RegionHandle* uprev_rgn=& _temp289; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp290= fields; goto
_LL291; _LL291: for( 0; _temp290 != 0; _temp290=(( struct Cyc_List_List*)
_check_null( _temp290))->tl){ struct Cyc_Absyn_Structfield _temp294; struct Cyc_List_List*
_temp295; struct Cyc_Core_Opt* _temp297; void* _temp299; struct Cyc_Absyn_Tqual
_temp301; struct _tagged_string* _temp303; struct Cyc_Absyn_Structfield*
_temp292=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
_temp290))->hd; _temp294=* _temp292; _LL304: _temp303= _temp294.name; goto
_LL302; _LL302: _temp301= _temp294.tq; goto _LL300; _LL300: _temp299=( void*)
_temp294.type; goto _LL298; _LL298: _temp297= _temp294.width; goto _LL296;
_LL296: _temp295= _temp294.attributes; goto _LL293; _LL293: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp303)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp305=* _temp303;
struct _tagged_string _temp306= obj; xprintf("duplicate field %.*s in %.*s",
_temp305.last_plus_one - _temp305.curr, _temp305.curr, _temp306.last_plus_one -
_temp306.curr, _temp306.curr);}));} if( Cyc_String_strcmp(* _temp303,({ char*
_temp307=""; struct _tagged_string _temp308; _temp308.curr= _temp307; _temp308.base=
_temp307; _temp308.last_plus_one= _temp307 + 1; _temp308;})) != 0){ prev_fields=({
struct Cyc_List_List* _temp309=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp309->hd=( void*) _temp303;
_temp309->tl= prev_fields; _temp309;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, _temp299); if( _temp297 != 0){ unsigned int w= 0; if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp297))->v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp310=* _temp303; xprintf("bitfield %.*s does not have constant width",
_temp310.last_plus_one - _temp310.curr, _temp310.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*) _check_null( _temp297))->v);}{
void* _temp311= Cyc_Tcutil_compress( _temp299); void* _temp317; void* _temp319;
_LL313: if(( unsigned int) _temp311 > 4u?*(( int*) _temp311) == Cyc_Absyn_IntType:
0){ _LL320: _temp319=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp311)->f1;
goto _LL318; _LL318: _temp317=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp311)->f2; goto _LL314;} else{ goto _LL315;} _LL315: goto _LL316; _LL314:{
void* _temp321= _temp317; _LL323: if( _temp321 ==( void*) Cyc_Absyn_B1){ goto
_LL324;} else{ goto _LL325;} _LL325: if( _temp321 ==( void*) Cyc_Absyn_B2){ goto
_LL326;} else{ goto _LL327;} _LL327: if( _temp321 ==( void*) Cyc_Absyn_B4){ goto
_LL328;} else{ goto _LL329;} _LL329: if( _temp321 ==( void*) Cyc_Absyn_B8){ goto
_LL330;} else{ goto _LL322;} _LL324: if( w > 8){ Cyc_Tcutil_terr( loc,({ char*
_temp331="bitfield larger than type"; struct _tagged_string _temp332; _temp332.curr=
_temp331; _temp332.base= _temp331; _temp332.last_plus_one= _temp331 + 26;
_temp332;}));} goto _LL322; _LL326: if( w > 16){ Cyc_Tcutil_terr( loc,({ char*
_temp333="bitfield larger than type"; struct _tagged_string _temp334; _temp334.curr=
_temp333; _temp334.base= _temp333; _temp334.last_plus_one= _temp333 + 26;
_temp334;}));} goto _LL322; _LL328: if( w > 32){ Cyc_Tcutil_terr( loc,({ char*
_temp335="bitfield larger than type"; struct _tagged_string _temp336; _temp336.curr=
_temp335; _temp336.base= _temp335; _temp336.last_plus_one= _temp335 + 26;
_temp336;}));} goto _LL322; _LL330: if( w > 64){ Cyc_Tcutil_terr( loc,({ char*
_temp337="bitfield larger than type"; struct _tagged_string _temp338; _temp338.curr=
_temp337; _temp338.base= _temp337; _temp338.last_plus_one= _temp337 + 26;
_temp338;}));} goto _LL322; _LL322:;} goto _LL312; _LL316: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp339=* _temp303; struct _tagged_string _temp340= Cyc_Absynpp_typ2string(
_temp299); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp339.last_plus_one - _temp339.curr, _temp339.curr, _temp340.last_plus_one -
_temp340.curr, _temp340.curr);})); goto _LL312; _LL312:;}}}}; _pop_region(&
_temp289);} static void Cyc_Tc_tcStructOrUniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp341=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL342; _LL342: if( sd->name == 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp343= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp343.last_plus_one - _temp343.curr, _temp343.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=(( struct Cyc_List_List*)
_check_null( atts))->tl){ void* _temp344=( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd; int _temp352; _LL346: if( _temp344 ==( void*) Cyc_Absyn_Packed_att){
goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp344 > 15u?*((
int*) _temp344) == Cyc_Absyn_Aligned_att: 0){ _LL353: _temp352=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp344)->f1; goto _LL349;} else{ goto _LL350;} _LL350: goto _LL351; _LL347:
continue; _LL349: continue; _LL351: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp354= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null( atts))->hd); struct _tagged_string _temp355= obj; struct
_tagged_string _temp356=* _temp341; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp354.last_plus_one - _temp354.curr, _temp354.curr, _temp355.last_plus_one -
_temp355.curr, _temp355.curr, _temp356.last_plus_one - _temp356.curr, _temp356.curr);}));
goto _LL345; _LL345:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct Cyc_Absyn_Conref*
_temp357=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->kind);
goto _LL358; _LL358: { void* _temp359=( void*) _temp357->v; void* _temp367;
_LL361: if( _temp359 ==( void*) Cyc_Absyn_No_constr){ goto _LL362;} else{ goto
_LL363;} _LL363: if(( unsigned int) _temp359 > 1u?*(( int*) _temp359) == Cyc_Absyn_Eq_constr:
0){ _LL368: _temp367=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp359)->f1;
if( _temp367 ==( void*) Cyc_Absyn_MemKind){ goto _LL364;} else{ goto _LL365;}}
else{ goto _LL365;} _LL365: goto _LL366; _LL362:( void*)( _temp357->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp369=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp369[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp370; _temp370.tag= Cyc_Absyn_Eq_constr; _temp370.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp370;}); _temp369;}))); continue; _LL364:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp371= obj; struct
_tagged_string _temp372=* _temp341; struct _tagged_string _temp373=*(( struct
Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp371.last_plus_one - _temp371.curr, _temp371.curr, _temp372.last_plus_one -
_temp372.curr, _temp372.curr, _temp373.last_plus_one - _temp373.curr, _temp373.curr);}));
continue; _LL366: continue; _LL360:;}}}{ void* _temp374=(*(( struct _tuple0*)((
struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1; struct Cyc_List_List*
_temp382; struct Cyc_List_List* _temp384; _LL376: if(( unsigned int) _temp374 >
1u?*(( int*) _temp374) == Cyc_Absyn_Rel_n: 0){ _LL383: _temp382=(( struct Cyc_Absyn_Rel_n_struct*)
_temp374)->f1; if( _temp382 == 0){ goto _LL377;} else{ goto _LL378;}} else{ goto
_LL378;} _LL378: if(( unsigned int) _temp374 > 1u?*(( int*) _temp374) == Cyc_Absyn_Abs_n:
0){ _LL385: _temp384=(( struct Cyc_Absyn_Abs_n_struct*) _temp374)->f1; if(
_temp384 == 0){ goto _LL379;} else{ goto _LL380;}} else{ goto _LL380;} _LL380:
goto _LL381; _LL377: goto _LL375; _LL379: goto _LL375; _LL381: Cyc_Tcutil_terr(
loc,({ char* _temp386="qualified declarations are not implemented"; struct
_tagged_string _temp387; _temp387.curr= _temp386; _temp387.base= _temp386;
_temp387.last_plus_one= _temp386 + 43; _temp387;})); return; _LL375:;}(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp388=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp388[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp389; _temp389.tag= Cyc_Absyn_Abs_n; _temp389.f1= te->ns; _temp389;});
_temp388;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs);} struct _tuple4{
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl* sd){ struct _tagged_string* _temp390=(*((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f2; goto
_LL391; _LL391: { struct _tagged_string obj=({ char* _temp446="struct"; struct
_tagged_string _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 7; _temp447;}); Cyc_Tc_tcStructOrUniondecl(
te, ge, obj, loc, sd);{ struct Cyc_List_List* _temp392= sd->tvs; goto _LL393;
_LL393:{ struct _tuple4 _temp395=({ struct _tuple4 _temp394; _temp394.f1= sd->fields;
_temp394.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp390); _temp394;});
struct Cyc_Core_Opt* _temp405; struct Cyc_Core_Opt* _temp407; struct Cyc_Core_Opt*
_temp409; struct Cyc_Core_Opt* _temp411; struct Cyc_Core_Opt _temp413; struct
Cyc_List_List* _temp414; struct Cyc_Core_Opt* _temp416; struct Cyc_Core_Opt
_temp418; struct Cyc_Absyn_Structdecl** _temp419; struct Cyc_Core_Opt* _temp421;
struct Cyc_Core_Opt _temp423; struct Cyc_List_List* _temp424; struct Cyc_Core_Opt*
_temp426; struct Cyc_Core_Opt _temp428; struct Cyc_Absyn_Structdecl** _temp429;
struct Cyc_Core_Opt* _temp431; _LL397: _LL408: _temp407= _temp395.f1; if(
_temp407 == 0){ goto _LL406;} else{ goto _LL399;} _LL406: _temp405= _temp395.f2;
if( _temp405 == 0){ goto _LL398;} else{ goto _LL399;} _LL399: _LL412: _temp411=
_temp395.f1; if( _temp411 == 0){ goto _LL401;} else{ _temp413=* _temp411; _LL415:
_temp414=( struct Cyc_List_List*) _temp413.v; goto _LL410;} _LL410: _temp409=
_temp395.f2; if( _temp409 == 0){ goto _LL400;} else{ goto _LL401;} _LL401:
_LL422: _temp421= _temp395.f1; if( _temp421 == 0){ goto _LL403;} else{ _temp423=*
_temp421; _LL425: _temp424=( struct Cyc_List_List*) _temp423.v; goto _LL417;}
_LL417: _temp416= _temp395.f2; if( _temp416 == 0){ goto _LL403;} else{ _temp418=*
_temp416; _LL420: _temp419=( struct Cyc_Absyn_Structdecl**) _temp418.v; goto
_LL402;} _LL403: _LL432: _temp431= _temp395.f1; if( _temp431 == 0){ goto _LL427;}
else{ goto _LL396;} _LL427: _temp426= _temp395.f2; if( _temp426 == 0){ goto
_LL396;} else{ _temp428=* _temp426; _LL430: _temp429=( struct Cyc_Absyn_Structdecl**)
_temp428.v; goto _LL404;} _LL398: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp390,({ struct Cyc_Absyn_Structdecl**
_temp433=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp433[ 0]= sd; _temp433;})); goto _LL396; _LL400: { struct Cyc_Absyn_Structdecl**
_temp436=({ struct Cyc_Absyn_Structdecl** _temp434=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp434[ 0]=({ struct Cyc_Absyn_Structdecl*
_temp435=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp435->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp435->name= sd->name;
_temp435->tvs= _temp392; _temp435->fields= 0; _temp435->attributes= 0; _temp435;});
_temp434;}); goto _LL437; _LL437: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, _temp390, _temp436); Cyc_Tc_tcStructFields(
te, ge, loc, obj, _temp414, _temp392);* _temp436= sd; goto _LL396;} _LL402: {
struct Cyc_Absyn_Structdecl* _temp438=* _temp419; goto _LL439; _LL439:* _temp419=({
struct Cyc_Absyn_Structdecl* _temp440=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp440->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp440->name= sd->name; _temp440->tvs= _temp392; _temp440->fields= 0; _temp440->attributes=
0; _temp440;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp424, _temp392);*
_temp419= _temp438; _temp429= _temp419; goto _LL404;} _LL404: { struct Cyc_Absyn_Structdecl*
_temp441= Cyc_Tcdecl_merge_structdecl(* _temp429, sd, loc, Cyc_Tc_tc_msg); goto
_LL442; _LL442: if( _temp441 == 0){ return;} else{* _temp429=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp441); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp441);
goto _LL396;}} _LL396:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
_temp390,({ struct _tuple3* _temp443=( struct _tuple3*) GC_malloc( sizeof(
struct _tuple3)); _temp443->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp444=( struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp444[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp445; _temp445.tag= Cyc_Tcenv_StructRes;
_temp445.f1= sd; _temp445;}); _temp444;}); _temp443->f2= 1; _temp443;}));}}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* _temp448=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( ud->name))->v)).f2; goto _LL449; _LL449: { struct _tagged_string
obj=({ char* _temp501="union"; struct _tagged_string _temp502; _temp502.curr=
_temp501; _temp502.base= _temp501; _temp502.last_plus_one= _temp501 + 6;
_temp502;}); Cyc_Tc_tcStructOrUniondecl( te, ge, obj, loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp450= ud->tvs; goto _LL451; _LL451: { struct
_tuple4 _temp453=({ struct _tuple4 _temp452; _temp452.f1= ud->fields; _temp452.f2=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->uniondecls, _temp448); _temp452;}); struct Cyc_Core_Opt*
_temp463; struct Cyc_Core_Opt* _temp465; struct Cyc_Core_Opt* _temp467; struct
Cyc_Core_Opt* _temp469; struct Cyc_Core_Opt _temp471; struct Cyc_List_List*
_temp472; struct Cyc_Core_Opt* _temp474; struct Cyc_Core_Opt _temp476; struct
Cyc_Absyn_Uniondecl** _temp477; struct Cyc_Core_Opt* _temp479; struct Cyc_Core_Opt
_temp481; struct Cyc_List_List* _temp482; struct Cyc_Core_Opt* _temp484; struct
Cyc_Core_Opt _temp486; struct Cyc_Absyn_Uniondecl** _temp487; struct Cyc_Core_Opt*
_temp489; _LL455: _LL466: _temp465= _temp453.f1; if( _temp465 == 0){ goto _LL464;}
else{ goto _LL457;} _LL464: _temp463= _temp453.f2; if( _temp463 == 0){ goto
_LL456;} else{ goto _LL457;} _LL457: _LL470: _temp469= _temp453.f1; if( _temp469
== 0){ goto _LL459;} else{ _temp471=* _temp469; _LL473: _temp472=( struct Cyc_List_List*)
_temp471.v; goto _LL468;} _LL468: _temp467= _temp453.f2; if( _temp467 == 0){
goto _LL458;} else{ goto _LL459;} _LL459: _LL480: _temp479= _temp453.f1; if(
_temp479 == 0){ goto _LL461;} else{ _temp481=* _temp479; _LL483: _temp482=(
struct Cyc_List_List*) _temp481.v; goto _LL475;} _LL475: _temp474= _temp453.f2;
if( _temp474 == 0){ goto _LL461;} else{ _temp476=* _temp474; _LL478: _temp477=(
struct Cyc_Absyn_Uniondecl**) _temp476.v; goto _LL460;} _LL461: _LL490: _temp489=
_temp453.f1; if( _temp489 == 0){ goto _LL485;} else{ goto _LL454;} _LL485:
_temp484= _temp453.f2; if( _temp484 == 0){ goto _LL454;} else{ _temp486=*
_temp484; _LL488: _temp487=( struct Cyc_Absyn_Uniondecl**) _temp486.v; goto
_LL462;} _LL456: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp448,({ struct Cyc_Absyn_Uniondecl** _temp491=( struct Cyc_Absyn_Uniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*)); _temp491[ 0]= ud; _temp491;}));
goto _LL454; _LL458: { struct Cyc_Absyn_Uniondecl** _temp494=({ struct Cyc_Absyn_Uniondecl**
_temp492=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp492[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp493=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp493->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp493->name= ud->name; _temp493->tvs= _temp450; _temp493->fields=
0; _temp493->attributes= ud->attributes; _temp493;}); _temp492;}); goto _LL495;
_LL495: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, _temp448, _temp494); Cyc_Tc_tcStructFields( te, ge, loc, obj,
_temp472, _temp450);* _temp494= ud; goto _LL454;} _LL460: { struct Cyc_Absyn_Uniondecl*
_temp496=* _temp477; goto _LL497; _LL497:* _temp477=({ struct Cyc_Absyn_Uniondecl*
_temp498=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp498->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp498->name= ud->name;
_temp498->tvs= _temp450; _temp498->fields= 0; _temp498->attributes= ud->attributes;
_temp498;}); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp482, _temp450);*
_temp477= _temp496; _temp487= _temp477; goto _LL462;} _LL462: { struct Cyc_Absyn_Uniondecl*
_temp499= Cyc_Tcdecl_merge_uniondecl(* _temp487, ud, loc, Cyc_Tc_tc_msg); goto
_LL500; _LL500: if( _temp499 == 0){ return;} else{* _temp487=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp499); goto _LL454;}} _LL454:;}}}} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp503= fields; goto _LL504; _LL504: for( 0; _temp503 != 0;
_temp503=(( struct Cyc_List_List*) _check_null( _temp503))->tl){ struct Cyc_Absyn_Tunionfield*
_temp505=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp503))->hd; goto _LL506; _LL506:{ struct Cyc_List_List* tvs= _temp505->tvs;
for( 0; tvs != 0; tvs=(( struct Cyc_List_List*) _check_null( tvs))->tl){ struct
Cyc_Absyn_Tvar* _temp507=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( tvs))->hd; goto _LL508; _LL508: { struct Cyc_Absyn_Conref* _temp509=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp507->kind); goto _LL510; _LL510: { void* _temp511=( void*) _temp509->v;
void* _temp519; _LL513: if( _temp511 ==( void*) Cyc_Absyn_No_constr){ goto
_LL514;} else{ goto _LL515;} _LL515: if(( unsigned int) _temp511 > 1u?*(( int*)
_temp511) == Cyc_Absyn_Eq_constr: 0){ _LL520: _temp519=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp511)->f1; if( _temp519 ==( void*) Cyc_Absyn_MemKind){ goto _LL516;} else{
goto _LL517;}} else{ goto _LL517;} _LL517: goto _LL518; _LL514:( void*)(
_temp509->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp521=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp521[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp522; _temp522.tag= Cyc_Absyn_Eq_constr;
_temp522.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp522;}); _temp521;})));
goto _LL512; _LL516: Cyc_Tcutil_terr( _temp505->loc,({ struct _tagged_string
_temp523=*(* _temp505->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp523.last_plus_one - _temp523.curr, _temp523.curr);})); goto _LL512; _LL518:
goto _LL512; _LL512:;}}}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs,
_temp505->tvs); Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List*
typs= _temp505->typs; for( 0; typs != 0; typs=(( struct Cyc_List_List*)
_check_null( typs))->tl){ Cyc_Tcutil_check_type( _temp505->loc, te, alltvs,(
void*) Cyc_Absyn_MemKind,(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( typs))->hd)).f2);}}{ void* _temp524=(* _temp505->name).f1; struct
Cyc_List_List* _temp534; struct Cyc_List_List* _temp536; struct Cyc_List_List*
_temp538; _LL526: if(( unsigned int) _temp524 > 1u?*(( int*) _temp524) == Cyc_Absyn_Rel_n:
0){ _LL535: _temp534=(( struct Cyc_Absyn_Rel_n_struct*) _temp524)->f1; if(
_temp534 == 0){ goto _LL527;} else{ goto _LL528;}} else{ goto _LL528;} _LL528:
if(( unsigned int) _temp524 > 1u?*(( int*) _temp524) == Cyc_Absyn_Rel_n: 0){
_LL537: _temp536=(( struct Cyc_Absyn_Rel_n_struct*) _temp524)->f1; goto _LL529;}
else{ goto _LL530;} _LL530: if(( unsigned int) _temp524 > 1u?*(( int*) _temp524)
== Cyc_Absyn_Abs_n: 0){ _LL539: _temp538=(( struct Cyc_Absyn_Abs_n_struct*)
_temp524)->f1; goto _LL531;} else{ goto _LL532;} _LL532: if( _temp524 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL533;} else{ goto _LL525;} _LL527: if( is_xtunion){(*
_temp505->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp540=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp540[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp541; _temp541.tag= Cyc_Absyn_Abs_n;
_temp541.f1= te->ns; _temp541;}); _temp540;});} else{(* _temp505->name).f1=(*
name).f1;} goto _LL525; _LL529: Cyc_Tcutil_terr( _temp505->loc,({ char* _temp542="qualified declarations are not allowed";
struct _tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 39; _temp543;})); goto _LL525; _LL531: goto
_LL525; _LL533:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp544=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp544[ 0]=({ struct Cyc_Core_Impossible_struct _temp545; _temp545.tag= Cyc_Core_Impossible;
_temp545.f1=({ char* _temp546="tcTunionFields: Loc_n"; struct _tagged_string
_temp547; _temp547.curr= _temp546; _temp547.base= _temp546; _temp547.last_plus_one=
_temp546 + 22; _temp547;}); _temp545;}); _temp544;})); goto _LL525; _LL525:;}}}}{
struct Cyc_List_List* fields2; if( is_xtunion){ int _temp548= 1; goto _LL549;
_LL549: { struct Cyc_List_List* _temp550= Cyc_Tcdecl_sort_xtunion_fields( fields,&
_temp548,(* name).f2, loc, Cyc_Tc_tc_msg); goto _LL551; _LL551: if( _temp548){
fields2= _temp550;} else{ fields2= 0;}}} else{ struct _RegionHandle _temp552=
_new_region(); struct _RegionHandle* uprev_rgn=& _temp552; _push_region(
uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=
fields; for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){
struct Cyc_Absyn_Tunionfield* _temp553=( struct Cyc_Absyn_Tunionfield*)(( struct
Cyc_List_List*) _check_null( fs))->hd; goto _LL554; _LL554: if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
_temp553->name).f2)){ Cyc_Tcutil_terr( _temp553->loc,({ struct _tagged_string
_temp555=*(* _temp553->name).f2; struct _tagged_string _temp556= obj; xprintf("duplicate field name %.*s in %.*s",
_temp555.last_plus_one - _temp555.curr, _temp555.curr, _temp556.last_plus_one -
_temp556.curr, _temp556.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp557=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp557->hd=( void*)(* _temp553->name).f2; _temp557->tl= prev_fields; _temp557;});}
if(( void*) _temp553->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp558=*(* _temp553->name).f2; xprintf("ignoring scope of field %.*s",
_temp558.last_plus_one - _temp558.curr, _temp558.curr);}));( void*)( _temp553->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp552);}{
struct Cyc_List_List* _temp559= fields; goto _LL560; _LL560: for( 0; _temp559 !=
0; _temp559=(( struct Cyc_List_List*) _check_null( _temp559))->tl){ struct Cyc_Absyn_Tunionfield*
_temp561=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*) _check_null(
_temp559))->hd; goto _LL562; _LL562: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp561->name).f2,({ struct _tuple3* _temp563=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp563->f1=( void*)({ struct
Cyc_Tcenv_TunionRes_struct* _temp564=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp564[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp565; _temp565.tag= Cyc_Tcenv_TunionRes; _temp565.f1=
tudres; _temp565.f2= _temp561; _temp565;}); _temp564;}); _temp563->f2= 1;
_temp563;}));}} return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=({ char* _temp566="xtunion"; struct _tagged_string
_temp567; _temp567.curr= _temp566; _temp567.base= _temp566; _temp567.last_plus_one=
_temp566 + 8; _temp567;});} else{ obj=({ char* _temp568="tunion"; struct
_tagged_string _temp569; _temp569.curr= _temp568; _temp569.base= _temp568;
_temp569.last_plus_one= _temp568 + 7; _temp569;});}{ struct Cyc_List_List* tvs=
tud->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=(( struct
Cyc_List_List*) _check_null( tvs2))->tl){ struct Cyc_Absyn_Conref* c=(( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*) _check_null( tvs2))->hd)->kind);
void* _temp570=( void*) c->v; void* _temp578; _LL572: if( _temp570 ==( void*)
Cyc_Absyn_No_constr){ goto _LL573;} else{ goto _LL574;} _LL574: if((
unsigned int) _temp570 > 1u?*(( int*) _temp570) == Cyc_Absyn_Eq_constr: 0){
_LL579: _temp578=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp570)->f1;
if( _temp578 ==( void*) Cyc_Absyn_MemKind){ goto _LL575;} else{ goto _LL576;}}
else{ goto _LL576;} _LL576: goto _LL577; _LL573:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp580=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp580[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp581; _temp581.tag= Cyc_Absyn_Eq_constr; _temp581.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp581;}); _temp580;}))); goto _LL571;
_LL575: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp582= obj; struct
_tagged_string _temp583=* v; struct _tagged_string _temp584=*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( tvs2))->hd)->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp582.last_plus_one - _temp582.curr, _temp582.curr, _temp583.last_plus_one -
_temp583.curr, _temp583.curr, _temp584.last_plus_one - _temp584.curr, _temp584.curr);}));
goto _LL571; _LL577: goto _LL571; _LL571:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp585; _push_handler(& _temp585);{ int _temp587= 0; if( setjmp( _temp585.handler)){
_temp587= 1;} if( ! _temp587){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp586=( void*) _exn_thrown; void*
_temp589= _temp586; _LL591: if( _temp589 == Cyc_Dict_Absent){ goto _LL592;}
else{ goto _LL593;} _LL593: goto _LL594; _LL592: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp595= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp595.last_plus_one - _temp595.curr, _temp595.curr);})); return; _LL594:(
void) _throw( _temp589); _LL590:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)(( struct Cyc_Core_Opt*) _check_null( tud_opt))->v))->name;}
else{(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp596=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp596[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp597; _temp597.tag= Cyc_Absyn_Abs_n;
_temp597.f1= te->ns; _temp597;}); _temp596;});}} else{{ void* _temp598=(* tud->name).f1;
struct Cyc_List_List* _temp606; struct Cyc_List_List* _temp608; _LL600: if((
unsigned int) _temp598 > 1u?*(( int*) _temp598) == Cyc_Absyn_Rel_n: 0){ _LL607:
_temp606=(( struct Cyc_Absyn_Rel_n_struct*) _temp598)->f1; if( _temp606 == 0){
goto _LL601;} else{ goto _LL602;}} else{ goto _LL602;} _LL602: if(( unsigned int)
_temp598 > 1u?*(( int*) _temp598) == Cyc_Absyn_Abs_n: 0){ _LL609: _temp608=((
struct Cyc_Absyn_Abs_n_struct*) _temp598)->f1; goto _LL603;} else{ goto _LL604;}
_LL604: goto _LL605; _LL601:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp610=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp610[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp611; _temp611.tag= Cyc_Absyn_Abs_n;
_temp611.f1= te->ns; _temp611;}); _temp610;}); goto _LL599; _LL603: goto _LL605;
_LL605: Cyc_Tcutil_terr( loc,({ char* _temp612="qualified declarations are not implemented";
struct _tagged_string _temp613; _temp613.curr= _temp612; _temp613.base= _temp612;
_temp613.last_plus_one= _temp612 + 43; _temp613;})); return; _LL599:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp615=({ struct
_tuple4 _temp614; _temp614.f1= tud->fields; _temp614.f2= tud_opt; _temp614;});
struct Cyc_Core_Opt* _temp625; struct Cyc_Core_Opt* _temp627; struct Cyc_Core_Opt*
_temp629; struct Cyc_Core_Opt* _temp631; struct Cyc_Core_Opt _temp633; struct
Cyc_List_List* _temp634; struct Cyc_List_List** _temp636; struct Cyc_Core_Opt*
_temp637; struct Cyc_Core_Opt _temp639; struct Cyc_Absyn_Tuniondecl** _temp640;
struct Cyc_Core_Opt* _temp642; struct Cyc_Core_Opt _temp644; struct Cyc_List_List*
_temp645; struct Cyc_List_List** _temp647; struct Cyc_Core_Opt* _temp648; struct
Cyc_Core_Opt _temp650; struct Cyc_Absyn_Tuniondecl** _temp651; struct Cyc_Core_Opt*
_temp653; _LL617: _LL628: _temp627= _temp615.f1; if( _temp627 == 0){ goto _LL626;}
else{ goto _LL619;} _LL626: _temp625= _temp615.f2; if( _temp625 == 0){ goto
_LL618;} else{ goto _LL619;} _LL619: _LL632: _temp631= _temp615.f1; if( _temp631
== 0){ goto _LL621;} else{ _temp633=* _temp631; _LL635: _temp634=( struct Cyc_List_List*)
_temp633.v; _temp636=&(* _temp615.f1).v; goto _LL630;} _LL630: _temp629=
_temp615.f2; if( _temp629 == 0){ goto _LL620;} else{ goto _LL621;} _LL621:
_LL643: _temp642= _temp615.f1; if( _temp642 == 0){ goto _LL623;} else{ _temp644=*
_temp642; _LL646: _temp645=( struct Cyc_List_List*) _temp644.v; _temp647=&(*
_temp615.f1).v; goto _LL638;} _LL638: _temp637= _temp615.f2; if( _temp637 == 0){
goto _LL623;} else{ _temp639=* _temp637; _LL641: _temp640=( struct Cyc_Absyn_Tuniondecl**)
_temp639.v; goto _LL622;} _LL623: _LL654: _temp653= _temp615.f1; if( _temp653 ==
0){ goto _LL649;} else{ goto _LL616;} _LL649: _temp648= _temp615.f2; if(
_temp648 == 0){ goto _LL616;} else{ _temp650=* _temp648; _LL652: _temp651=(
struct Cyc_Absyn_Tuniondecl**) _temp650.v; goto _LL624;} _LL618: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp655=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp655[ 0]= tud; _temp655;}));
goto _LL616; _LL620: { struct Cyc_Absyn_Tuniondecl** _temp658=({ struct Cyc_Absyn_Tuniondecl**
_temp656=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp656[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp657=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp657->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp657->name= tud->name; _temp657->tvs= tvs; _temp657->fields=
0; _temp657->is_xtunion= tud->is_xtunion; _temp657;}); _temp656;}); goto _LL659;
_LL659: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp658);* _temp636= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp636, tvs, tud);* _temp658= tud; goto
_LL616;} _LL622: { struct Cyc_Absyn_Tuniondecl* _temp660=* _temp640; goto _LL661;
_LL661:* _temp640=({ struct Cyc_Absyn_Tuniondecl* _temp662=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp662->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp662->name= tud->name; _temp662->tvs= tvs; _temp662->fields=
0; _temp662->is_xtunion= tud->is_xtunion; _temp662;});* _temp647= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp647, tvs, tud);* _temp640=
_temp660; _temp651= _temp640; goto _LL624;} _LL624: { struct Cyc_Absyn_Tuniondecl*
_temp663= Cyc_Tcdecl_merge_tuniondecl(* _temp651, tud, loc, Cyc_Tc_tc_msg); goto
_LL664; _LL664: if( _temp663 == 0){ return;} else{* _temp651=( struct Cyc_Absyn_Tuniondecl*)
_check_null( _temp663); goto _LL616;}} _LL616:;}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp665=(* ed->name).f1; struct Cyc_List_List* _temp673; struct Cyc_List_List*
_temp675; _LL667: if(( unsigned int) _temp665 > 1u?*(( int*) _temp665) == Cyc_Absyn_Rel_n:
0){ _LL674: _temp673=(( struct Cyc_Absyn_Rel_n_struct*) _temp665)->f1; if(
_temp673 == 0){ goto _LL668;} else{ goto _LL669;}} else{ goto _LL669;} _LL669:
if(( unsigned int) _temp665 > 1u?*(( int*) _temp665) == Cyc_Absyn_Abs_n: 0){
_LL676: _temp675=(( struct Cyc_Absyn_Abs_n_struct*) _temp665)->f1; if( _temp675
== 0){ goto _LL670;} else{ goto _LL671;}} else{ goto _LL671;} _LL671: goto
_LL672; _LL668: goto _LL666; _LL670: goto _LL666; _LL672: Cyc_Tcutil_terr( loc,({
char* _temp677="qualified declarations are not implemented"; struct
_tagged_string _temp678; _temp678.curr= _temp677; _temp678.base= _temp677;
_temp678.last_plus_one= _temp677 + 43; _temp678;})); return; _LL666:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp679=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp679[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp680; _temp680.tag= Cyc_Absyn_Abs_n; _temp680.f1= te->ns; _temp680;});
_temp679;}); if( ed->fields != 0){ struct _RegionHandle _temp681= _new_region();
struct _RegionHandle* uprev_rgn=& _temp681; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( ed->fields))->v;
for( 0; fs != 0; fs=(( struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp682=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL683; _LL683: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* _temp682->name).f2)){ Cyc_Tcutil_terr(
_temp682->loc,({ struct _tagged_string _temp684=*(* _temp682->name).f2; xprintf("duplicate field name %.*s",
_temp684.last_plus_one - _temp684.curr, _temp684.curr);}));} else{ prev_fields=({
struct Cyc_List_List* _temp685=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp685->hd=( void*)(* _temp682->name).f2;
_temp685->tl= prev_fields; _temp685;});} if( _temp682->tag == 0){ _temp682->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, _temp682->loc);} else{ if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*) _check_null( _temp682->tag))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp686=* v; struct
_tagged_string _temp687=*(* _temp682->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_temp686.last_plus_one - _temp686.curr, _temp686.curr, _temp687.last_plus_one -
_temp687.curr, _temp687.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp682->tag)); tag_count= t1 + 1;(*
_temp682->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp688=( struct
Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp688[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp689; _temp689.tag= Cyc_Absyn_Abs_n;
_temp689.f1= te->ns; _temp689;}); _temp688;});}}}; _pop_region(& _temp681);}{
struct _handler_cons _temp690; _push_handler(& _temp690);{ int _temp692= 0; if(
setjmp( _temp690.handler)){ _temp692= 1;} if( ! _temp692){{ struct Cyc_Absyn_Enumdecl**
_temp693=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v); goto _LL694; _LL694: {
struct Cyc_Absyn_Enumdecl* _temp695= Cyc_Tcdecl_merge_enumdecl(* _temp693, ed,
loc, Cyc_Tc_tc_msg); goto _LL696; _LL696: if( _temp695 == 0){ _npop_handler( 0u);
return;}* _temp693=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp695);}};
_pop_handler();} else{ void* _temp691=( void*) _exn_thrown; void* _temp698=
_temp691; _LL700: if( _temp698 == Cyc_Dict_Absent){ goto _LL701;} else{ goto
_LL702;} _LL702: goto _LL703; _LL701: { struct Cyc_Absyn_Enumdecl** _temp705=({
struct Cyc_Absyn_Enumdecl** _temp704=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp704[ 0]= ed; _temp704;}); goto _LL706;
_LL706: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v, _temp705); goto _LL699;} _LL703:( void) _throw( _temp698);
_LL699:;}}} if( ed->fields != 0){ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs != 0; fs=((
struct Cyc_List_List*) _check_null( fs))->tl){ struct Cyc_Absyn_Enumfield*
_temp707=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_List_List*) _check_null( fs))->hd;
goto _LL708; _LL708: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(*
_temp707->name).f2,({ struct _tuple3* _temp709=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp709->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp710=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp710[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp711; _temp711.tag= Cyc_Tcenv_EnumRes;
_temp711.f1= ed; _temp711.f2= _temp707; _temp711;}); _temp710;}); _temp709->f2=
1; _temp709;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp712= sc; _LL714: if( _temp712 ==( void*) Cyc_Absyn_Static){
goto _LL715;} else{ goto _LL716;} _LL716: if( _temp712 ==( void*) Cyc_Absyn_Abstract){
goto _LL717;} else{ goto _LL718;} _LL718: if( _temp712 ==( void*) Cyc_Absyn_Public){
goto _LL719;} else{ goto _LL720;} _LL720: if( _temp712 ==( void*) Cyc_Absyn_Extern){
goto _LL721;} else{ goto _LL722;} _LL722: if( _temp712 ==( void*) Cyc_Absyn_ExternC){
goto _LL723;} else{ goto _LL713;} _LL715: Cyc_Tcutil_warn( loc,({ char* _temp724="static declaration nested within extern \"C\"";
struct _tagged_string _temp725; _temp725.curr= _temp724; _temp725.base= _temp724;
_temp725.last_plus_one= _temp724 + 44; _temp725;})); return 0; _LL717: Cyc_Tcutil_warn(
loc,({ char* _temp726="abstract declaration nested within extern \"C\""; struct
_tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 46; _temp727;})); return 0; _LL719: return 1;
_LL721: return 1; _LL723: Cyc_Tcutil_warn( loc,({ char* _temp728="nested extern \"C\" declaration";
struct _tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 30; _temp729;})); return 1; _LL713:;} static
void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int
in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* _temp730= ds0; goto _LL731; _LL731: for( 0;
_temp730 != 0; _temp730=(( struct Cyc_List_List*) _check_null( _temp730))->tl){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp730))->hd; struct Cyc_Position_Segment* loc= d->loc; void*
_temp732=( void*) d->r; int _temp758; struct Cyc_Absyn_Exp* _temp760; struct Cyc_Core_Opt*
_temp762; struct Cyc_Core_Opt* _temp764; struct Cyc_Absyn_Pat* _temp766; struct
Cyc_List_List* _temp768; struct Cyc_Absyn_Vardecl* _temp770; struct Cyc_Absyn_Fndecl*
_temp772; struct Cyc_Absyn_Typedefdecl* _temp774; struct Cyc_Absyn_Structdecl*
_temp776; struct Cyc_Absyn_Uniondecl* _temp778; struct Cyc_Absyn_Tuniondecl*
_temp780; struct Cyc_Absyn_Enumdecl* _temp782; struct Cyc_List_List* _temp784;
struct _tagged_string* _temp786; struct Cyc_List_List* _temp788; struct _tuple0*
_temp790; struct _tuple0 _temp792; struct _tagged_string* _temp793; void*
_temp795; struct Cyc_List_List* _temp797; _LL734: if(*(( int*) _temp732) == Cyc_Absyn_Let_d){
_LL767: _temp766=(( struct Cyc_Absyn_Let_d_struct*) _temp732)->f1; goto _LL765;
_LL765: _temp764=(( struct Cyc_Absyn_Let_d_struct*) _temp732)->f2; goto _LL763;
_LL763: _temp762=(( struct Cyc_Absyn_Let_d_struct*) _temp732)->f3; goto _LL761;
_LL761: _temp760=(( struct Cyc_Absyn_Let_d_struct*) _temp732)->f4; goto _LL759;
_LL759: _temp758=(( struct Cyc_Absyn_Let_d_struct*) _temp732)->f5; goto _LL735;}
else{ goto _LL736;} _LL736: if(*(( int*) _temp732) == Cyc_Absyn_Letv_d){ _LL769:
_temp768=(( struct Cyc_Absyn_Letv_d_struct*) _temp732)->f1; goto _LL737;} else{
goto _LL738;} _LL738: if(*(( int*) _temp732) == Cyc_Absyn_Var_d){ _LL771:
_temp770=(( struct Cyc_Absyn_Var_d_struct*) _temp732)->f1; goto _LL739;} else{
goto _LL740;} _LL740: if(*(( int*) _temp732) == Cyc_Absyn_Fn_d){ _LL773:
_temp772=(( struct Cyc_Absyn_Fn_d_struct*) _temp732)->f1; goto _LL741;} else{
goto _LL742;} _LL742: if(*(( int*) _temp732) == Cyc_Absyn_Typedef_d){ _LL775:
_temp774=(( struct Cyc_Absyn_Typedef_d_struct*) _temp732)->f1; goto _LL743;}
else{ goto _LL744;} _LL744: if(*(( int*) _temp732) == Cyc_Absyn_Struct_d){
_LL777: _temp776=(( struct Cyc_Absyn_Struct_d_struct*) _temp732)->f1; goto
_LL745;} else{ goto _LL746;} _LL746: if(*(( int*) _temp732) == Cyc_Absyn_Union_d){
_LL779: _temp778=(( struct Cyc_Absyn_Union_d_struct*) _temp732)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: if(*(( int*) _temp732) == Cyc_Absyn_Tunion_d){
_LL781: _temp780=(( struct Cyc_Absyn_Tunion_d_struct*) _temp732)->f1; goto
_LL749;} else{ goto _LL750;} _LL750: if(*(( int*) _temp732) == Cyc_Absyn_Enum_d){
_LL783: _temp782=(( struct Cyc_Absyn_Enum_d_struct*) _temp732)->f1; goto _LL751;}
else{ goto _LL752;} _LL752: if(*(( int*) _temp732) == Cyc_Absyn_Namespace_d){
_LL787: _temp786=(( struct Cyc_Absyn_Namespace_d_struct*) _temp732)->f1; goto
_LL785; _LL785: _temp784=(( struct Cyc_Absyn_Namespace_d_struct*) _temp732)->f2;
goto _LL753;} else{ goto _LL754;} _LL754: if(*(( int*) _temp732) == Cyc_Absyn_Using_d){
_LL791: _temp790=(( struct Cyc_Absyn_Using_d_struct*) _temp732)->f1; _temp792=*
_temp790; _LL796: _temp795= _temp792.f1; goto _LL794; _LL794: _temp793= _temp792.f2;
goto _LL789; _LL789: _temp788=(( struct Cyc_Absyn_Using_d_struct*) _temp732)->f2;
goto _LL755;} else{ goto _LL756;} _LL756: if(*(( int*) _temp732) == Cyc_Absyn_ExternC_d){
_LL798: _temp797=(( struct Cyc_Absyn_ExternC_d_struct*) _temp732)->f1; goto
_LL757;} else{ goto _LL733;} _LL735: Cyc_Tcutil_terr( loc,({ char* _temp799="top level let-declarations are not implemented";
struct _tagged_string _temp800; _temp800.curr= _temp799; _temp800.base= _temp799;
_temp800.last_plus_one= _temp799 + 47; _temp800;})); goto _LL733; _LL737: Cyc_Tcutil_terr(
loc,({ char* _temp801="top level let-declarations are not implemented"; struct
_tagged_string _temp802; _temp802.curr= _temp801; _temp802.base= _temp801;
_temp802.last_plus_one= _temp801 + 47; _temp802;})); goto _LL733; _LL739: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp770->sc): 0){( void*)(
_temp770->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp770, check_var_init); goto _LL733; _LL741: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp772->sc): 0){( void*)( _temp772->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp772); goto _LL733; _LL743: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp774); goto _LL733; _LL745: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp776->sc): 0){( void*)( _temp776->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp776); goto _LL733; _LL747: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp778->sc): 0){( void*)( _temp778->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp778);
goto _LL733; _LL749: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp780->sc): 0){( void*)( _temp780->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp780); goto _LL733; _LL751: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp782->sc): 0){( void*)( _temp782->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp782);
goto _LL733; _LL753: { struct Cyc_List_List* _temp803= te->ns; goto _LL804;
_LL804: { struct Cyc_List_List* _temp806=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp803,({ struct Cyc_List_List*
_temp805=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp805->hd=( void*) _temp786; _temp805->tl= 0; _temp805;})); goto _LL807;
_LL807: if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ge->namespaces, _temp786)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp786); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, _temp806, Cyc_Tcenv_empty_genv());}
te->ns= _temp806; Cyc_Tc_tc_decls( te, _temp784, in_externC, check_var_init); te->ns=
_temp803; goto _LL733;}} _LL755: { struct _tagged_string* first; struct Cyc_List_List*
rest;{ void* _temp808= _temp795; struct Cyc_List_List* _temp820; struct Cyc_List_List*
_temp822; struct Cyc_List_List* _temp824; struct Cyc_List_List _temp826; struct
Cyc_List_List* _temp827; struct _tagged_string* _temp829; struct Cyc_List_List*
_temp831; struct Cyc_List_List _temp833; struct Cyc_List_List* _temp834; struct
_tagged_string* _temp836; _LL810: if( _temp808 ==( void*) Cyc_Absyn_Loc_n){ goto
_LL811;} else{ goto _LL812;} _LL812: if(( unsigned int) _temp808 > 1u?*(( int*)
_temp808) == Cyc_Absyn_Rel_n: 0){ _LL821: _temp820=(( struct Cyc_Absyn_Rel_n_struct*)
_temp808)->f1; if( _temp820 == 0){ goto _LL813;} else{ goto _LL814;}} else{ goto
_LL814;} _LL814: if(( unsigned int) _temp808 > 1u?*(( int*) _temp808) == Cyc_Absyn_Abs_n:
0){ _LL823: _temp822=(( struct Cyc_Absyn_Abs_n_struct*) _temp808)->f1; if(
_temp822 == 0){ goto _LL815;} else{ goto _LL816;}} else{ goto _LL816;} _LL816:
if(( unsigned int) _temp808 > 1u?*(( int*) _temp808) == Cyc_Absyn_Rel_n: 0){
_LL825: _temp824=(( struct Cyc_Absyn_Rel_n_struct*) _temp808)->f1; if( _temp824
== 0){ goto _LL818;} else{ _temp826=* _temp824; _LL830: _temp829=( struct
_tagged_string*) _temp826.hd; goto _LL828; _LL828: _temp827= _temp826.tl; goto
_LL817;}} else{ goto _LL818;} _LL818: if(( unsigned int) _temp808 > 1u?*(( int*)
_temp808) == Cyc_Absyn_Abs_n: 0){ _LL832: _temp831=(( struct Cyc_Absyn_Abs_n_struct*)
_temp808)->f1; if( _temp831 == 0){ goto _LL809;} else{ _temp833=* _temp831;
_LL837: _temp836=( struct _tagged_string*) _temp833.hd; goto _LL835; _LL835:
_temp834= _temp833.tl; goto _LL819;}} else{ goto _LL809;} _LL811: goto _LL813;
_LL813: goto _LL815; _LL815: first= _temp793; rest= 0; goto _LL809; _LL817:
_temp836= _temp829; _temp834= _temp827; goto _LL819; _LL819: first= _temp836;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp834,({ struct Cyc_List_List* _temp838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp838->hd=( void*) _temp793;
_temp838->tl= 0; _temp838;})); goto _LL809; _LL809:;}{ struct Cyc_List_List*
_temp839= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); goto _LL840;
_LL840: ge->availables=({ struct Cyc_List_List* _temp841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp841->hd=( void*) _temp839;
_temp841->tl= ge->availables; _temp841;}); Cyc_Tc_tc_decls( te, _temp788,
in_externC, check_var_init); ge->availables=(( struct Cyc_List_List*)
_check_null( ge->availables))->tl; goto _LL733;}} _LL757: Cyc_Tc_tc_decls( te,
_temp797, 1, check_var_init); goto _LL733; _LL733:;}} static char _temp844[ 4u]="Cyc";
static struct _tagged_string Cyc_Tc_cyc_string={ _temp844, _temp844, _temp844 +
4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc(
struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, int check_var_init, struct Cyc_List_List*
ds){ if( add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp845->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp846=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp846->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp847=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp847[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp848; _temp848.tag= Cyc_Absyn_Namespace_d;
_temp848.f1= Cyc_Tc_cyc_ns; _temp848.f2= ds; _temp848;}); _temp847;})); _temp846->loc=
0; _temp846;}); _temp845->tl= 0; _temp845;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t; static
int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d);
static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple6{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp849=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp859; struct Cyc_List_List* _temp861; struct Cyc_List_List**
_temp863; struct _tuple0* _temp864; struct Cyc_List_List* _temp866; struct Cyc_List_List**
_temp868; struct _tagged_string* _temp869; _LL851: if(*(( int*) _temp849) == Cyc_Absyn_Var_d){
_LL860: _temp859=(( struct Cyc_Absyn_Var_d_struct*) _temp849)->f1; goto _LL852;}
else{ goto _LL853;} _LL853: if(*(( int*) _temp849) == Cyc_Absyn_Using_d){ _LL865:
_temp864=(( struct Cyc_Absyn_Using_d_struct*) _temp849)->f1; goto _LL862; _LL862:
_temp861=(( struct Cyc_Absyn_Using_d_struct*) _temp849)->f2; _temp863=&(( struct
Cyc_Absyn_Using_d_struct*) _temp849)->f2; goto _LL854;} else{ goto _LL855;}
_LL855: if(*(( int*) _temp849) == Cyc_Absyn_Namespace_d){ _LL870: _temp869=((
struct Cyc_Absyn_Namespace_d_struct*) _temp849)->f1; goto _LL867; _LL867:
_temp866=(( struct Cyc_Absyn_Namespace_d_struct*) _temp849)->f2; _temp868=&((
struct Cyc_Absyn_Namespace_d_struct*) _temp849)->f2; goto _LL856;} else{ goto
_LL857;} _LL857: goto _LL858; _LL852: if(( void*) _temp859->sc !=( void*) Cyc_Absyn_Extern){
return 1;}{ struct _tuple0 _temp873; struct _tagged_string* _temp874; void*
_temp876; struct _tuple0* _temp871= _temp859->name; _temp873=* _temp871; _LL877:
_temp876= _temp873.f1; goto _LL875; _LL875: _temp874= _temp873.f2; goto _LL872;
_LL872: { struct Cyc_List_List* ns;{ void* _temp878= _temp876; struct Cyc_List_List*
_temp886; struct Cyc_List_List* _temp888; _LL880: if( _temp878 ==( void*) Cyc_Absyn_Loc_n){
goto _LL881;} else{ goto _LL882;} _LL882: if(( unsigned int) _temp878 > 1u?*((
int*) _temp878) == Cyc_Absyn_Rel_n: 0){ _LL887: _temp886=(( struct Cyc_Absyn_Rel_n_struct*)
_temp878)->f1; goto _LL883;} else{ goto _LL884;} _LL884: if(( unsigned int)
_temp878 > 1u?*(( int*) _temp878) == Cyc_Absyn_Abs_n: 0){ _LL889: _temp888=((
struct Cyc_Absyn_Abs_n_struct*) _temp878)->f1; goto _LL885;} else{ goto _LL879;}
_LL881: ns= 0; goto _LL879; _LL883: ns= _temp886; goto _LL879; _LL885: ns=
_temp888; goto _LL879; _LL879:;}{ struct _tuple6* _temp890=(( struct _tuple6*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( env, ns);
goto _LL891; _LL891: { struct Cyc_Tcenv_Genv* _temp892=(* _temp890).f1; goto
_LL893; _LL893: { int _temp894=(*(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup)( _temp892->ordinaries, _temp874)).f2;
goto _LL895; _LL895: if( ! _temp894){(* _temp890).f2=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((* _temp890).f2,
_temp874);} return _temp894;}}}}} _LL854: _temp868= _temp863; goto _LL856;
_LL856:* _temp868= Cyc_Tc_treeshake_f( env,* _temp868); return 1; _LL858: return
1; _LL850:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set* set,
struct _tagged_string* x, struct _tuple3* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple6* nsenv){ struct Cyc_Tcenv_Genv*
_temp896=(* nsenv).f1; goto _LL897; _LL897: { struct Cyc_Set_Set* _temp898=(*
nsenv).f2; goto _LL899; _LL899: _temp896->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple3*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
_temp898, _temp896->ordinaries);}} static struct _tuple6* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple6* _temp900=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp900->f1= ge; _temp900->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp900;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict*
_temp901=(( struct Cyc_Dict_Dict*(*)( struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae);
goto _LL902; _LL902: { struct Cyc_List_List* _temp903= Cyc_Tc_treeshake_f(
_temp901, ds); goto _LL904; _LL904:(( void(*)( void(* f)( struct Cyc_List_List*,
struct _tuple6*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
_temp901); return _temp903;}}