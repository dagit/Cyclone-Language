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
tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef int Cyc_Stdio_fpos_t; extern int Cyc_Stdio_fputc( int,
struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_Stdio_file_string_write(
struct Cyc_Stdio___sFILE* fd, struct _tagged_string src, int src_offset, int
max_count); struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ;
typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length(
struct Cyc_List_List* x); extern char Cyc_List_List_empty[ 15u]; extern struct
Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth[ 8u];
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; typedef struct
Cyc_Position_Segment* Cyc_Position_seg_t; static const int Cyc_Position_Lex= 0;
static const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[ 14u]; typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple0{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple0* Cyc_Absyn_qvar_t;
typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct Cyc_Absyn_PtrInfo;
struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
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
struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct _tuple3{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct _tuple4{
struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ; extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Absyndump_dumptyp(
void*); extern void Cyc_Absyndump_dumpntyp( void* t); extern void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
extern void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*); extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*);
extern void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual, void*, void(* f)(
void*), void*); extern void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List*
fields); typedef struct _tagged_string Cyc_Absyndump_dump_string_t; struct Cyc_Stdio___sFILE**
Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout; void Cyc_Absyndump_ignore( void* x){
return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp3= s;(
unsigned int)( _temp3.last_plus_one - _temp3.curr);}); if( !(( int)({ struct
_tagged_string _temp0= s; char* _temp2= _temp0.curr +( sz - 1); if( _temp0.base
== 0? 1:( _temp2 < _temp0.base? 1: _temp2 >= _temp0.last_plus_one)){ _throw(
Null_Exception);}* _temp2;}))){ -- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos
> 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);}
else{ Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp7= s;(
unsigned int)( _temp7.last_plus_one - _temp7.curr);}); if( !(( int)({ struct
_tagged_string _temp4= s; char* _temp6= _temp4.curr +( sz - 1); if( _temp4.base
== 0? 1:( _temp6 < _temp4.base? 1: _temp6 >= _temp4.last_plus_one)){ _throw(
Null_Exception);}* _temp6;}))){ -- sz;} Cyc_Absyndump_pos += sz; Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char( int c){ ++
Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str(
struct _tagged_string* s){ Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep( void(* f)(
void*), struct Cyc_List_List* l, struct _tagged_string sep){ if( l == 0){
return;} for( 0;({ struct Cyc_List_List* _temp8= l; if( _temp8 == 0){ _throw(
Null_Exception);} _temp8->tl;}) != 0; l=({ struct Cyc_List_List* _temp9= l; if(
_temp9 == 0){ _throw( Null_Exception);} _temp9->tl;})){ f(( void*)({ struct Cyc_List_List*
_temp10= l; if( _temp10 == 0){ _throw( Null_Exception);} _temp10->hd;})); Cyc_Absyndump_dump_nospace(
sep);} f(( void*)({ struct Cyc_List_List* _temp11= l; if( _temp11 == 0){ _throw(
Null_Exception);} _temp11->hd;}));} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;({ struct Cyc_List_List* _temp12= l; if( _temp12 ==
0){ _throw( Null_Exception);} _temp12->tl;}) != 0; l=({ struct Cyc_List_List*
_temp13= l; if( _temp13 == 0){ _throw( Null_Exception);} _temp13->tl;})){ f( env,(
void*)({ struct Cyc_List_List* _temp14= l; if( _temp14 == 0){ _throw(
Null_Exception);} _temp14->hd;})); Cyc_Absyndump_dump_nospace( sep);} f( env,(
void*)({ struct Cyc_List_List* _temp15= l; if( _temp15 == 0){ _throw(
Null_Exception);} _temp15->hd;}));} void Cyc_Absyndump_group( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
f, l, sep); Cyc_Absyndump_dump_nospace( end);} void Cyc_Absyndump_group_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*, void*), void* env, struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep){ if( l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* _temp16= 0; goto _LL17; _LL17:{ void* _temp18=(* v).f1;
struct Cyc_List_List* _temp30; struct Cyc_List_List* _temp32; struct Cyc_List_List*
_temp34; struct Cyc_List_List* _temp36; _LL20: if( _temp18 ==( void*) Cyc_Absyn_Loc_n){
goto _LL21;} else{ goto _LL22;} _LL22: if(( unsigned int) _temp18 > 1u?*(( int*)
_temp18) == Cyc_Absyn_Rel_n: 0){ _LL31: _temp30=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp18)->f1; if( _temp30 == 0){ goto _LL23;}
else{ goto _LL24;}} else{ goto _LL24;} _LL24: if(( unsigned int) _temp18 > 1u?*((
int*) _temp18) == Cyc_Absyn_Abs_n: 0){ _LL33: _temp32=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp18)->f1; if( _temp32 == 0){ goto _LL25;}
else{ goto _LL26;}} else{ goto _LL26;} _LL26: if(( unsigned int) _temp18 > 1u?*((
int*) _temp18) == Cyc_Absyn_Rel_n: 0){ _LL35: _temp34=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp18)->f1; goto _LL27;} else{ goto _LL28;}
_LL28: if(( unsigned int) _temp18 > 1u?*(( int*) _temp18) == Cyc_Absyn_Abs_n: 0){
_LL37: _temp36=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp18)->f1; goto _LL29;} else{ goto _LL19;} _LL21: goto _LL23; _LL23: goto
_LL25; _LL25: Cyc_Absyndump_dump_str((* v).f2); return; _LL27: _temp36= _temp34;
goto _LL29; _LL29: _temp16= _temp36; goto _LL19; _LL19:;} Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_List_List* _temp38= _temp16; if( _temp38 ==
0){ _throw( Null_Exception);} _temp38->hd;})); for( _temp16=({ struct Cyc_List_List*
_temp39= _temp16; if( _temp39 == 0){ _throw( Null_Exception);} _temp39->tl;});
_temp16 != 0; _temp16=({ struct Cyc_List_List* _temp40= _temp16; if( _temp40 ==
0){ _throw( Null_Exception);} _temp40->tl;})){ if( Cyc_Absynpp_qvar_to_Cids){
Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp41=( char*)"::"; struct _tagged_string _temp42;
_temp42.curr= _temp41; _temp42.base= _temp41; _temp42.last_plus_one= _temp41 + 3;
_temp42;}));} Cyc_Absyndump_dump_nospace(*(( struct _tagged_string*)({ struct
Cyc_List_List* _temp43= _temp16; if( _temp43 == 0){ _throw( Null_Exception);}
_temp43->hd;})));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp44=( char*)"_"; struct _tagged_string
_temp45; _temp45.curr= _temp44; _temp45.base= _temp44; _temp45.last_plus_one=
_temp44 + 2; _temp45;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp46=( char*)"::"; struct _tagged_string _temp47;
_temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one= _temp46 + 3;
_temp47;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp48=( char*)"restrict"; struct _tagged_string
_temp49; _temp49.curr= _temp48; _temp49.base= _temp48; _temp49.last_plus_one=
_temp48 + 9; _temp49;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp50=( char*)"volatile"; struct _tagged_string
_temp51; _temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one=
_temp50 + 9; _temp51;}));} if( tq.q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp52=( char*)"const"; struct _tagged_string _temp53;
_temp53.curr= _temp52; _temp53.base= _temp52; _temp53.last_plus_one= _temp52 + 6;
_temp53;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp54= sc; _LL56:
if( _temp54 ==( void*) Cyc_Absyn_Static){ goto _LL57;} else{ goto _LL58;} _LL58:
if( _temp54 ==( void*) Cyc_Absyn_Public){ goto _LL59;} else{ goto _LL60;} _LL60:
if( _temp54 ==( void*) Cyc_Absyn_Extern){ goto _LL61;} else{ goto _LL62;} _LL62:
if( _temp54 ==( void*) Cyc_Absyn_ExternC){ goto _LL63;} else{ goto _LL64;} _LL64:
if( _temp54 ==( void*) Cyc_Absyn_Abstract){ goto _LL65;} else{ goto _LL55;}
_LL57: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp66=( char*)"static";
struct _tagged_string _temp67; _temp67.curr= _temp66; _temp67.base= _temp66;
_temp67.last_plus_one= _temp66 + 7; _temp67;})); return; _LL59: return; _LL61:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp68=( char*)"extern";
struct _tagged_string _temp69; _temp69.curr= _temp68; _temp69.base= _temp68;
_temp69.last_plus_one= _temp68 + 7; _temp69;})); return; _LL63: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp70=( char*)"extern \"C\""; struct
_tagged_string _temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 11; _temp71;})); return; _LL65: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp72=( char*)"abstract"; struct _tagged_string
_temp73; _temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one=
_temp72 + 9; _temp73;})); return; _LL55:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp74= k; _LL76: if( _temp74 ==( void*) Cyc_Absyn_AnyKind){ goto _LL77;}
else{ goto _LL78;} _LL78: if( _temp74 ==( void*) Cyc_Absyn_MemKind){ goto _LL79;}
else{ goto _LL80;} _LL80: if( _temp74 ==( void*) Cyc_Absyn_BoxKind){ goto _LL81;}
else{ goto _LL82;} _LL82: if( _temp74 ==( void*) Cyc_Absyn_RgnKind){ goto _LL83;}
else{ goto _LL84;} _LL84: if( _temp74 ==( void*) Cyc_Absyn_EffKind){ goto _LL85;}
else{ goto _LL75;} _LL77: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp86=( char*)"A"; struct _tagged_string _temp87; _temp87.curr= _temp86;
_temp87.base= _temp86; _temp87.last_plus_one= _temp86 + 2; _temp87;})); return;
_LL79: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp88=( char*)"M";
struct _tagged_string _temp89; _temp89.curr= _temp88; _temp89.base= _temp88;
_temp89.last_plus_one= _temp88 + 2; _temp89;})); return; _LL81: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp90=( char*)"B"; struct _tagged_string
_temp91; _temp91.curr= _temp90; _temp91.base= _temp90; _temp91.last_plus_one=
_temp90 + 2; _temp91;})); return; _LL83: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp92=( char*)"R"; struct _tagged_string _temp93;
_temp93.curr= _temp92; _temp93.base= _temp92; _temp93.last_plus_one= _temp92 + 2;
_temp93;})); return; _LL85: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp94=( char*)"E"; struct _tagged_string _temp95; _temp95.curr= _temp94;
_temp95.base= _temp94; _temp95.last_plus_one= _temp94 + 2; _temp95;})); return;
_LL75:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp,
ts,( struct _tagged_string)({ char* _temp96=( char*)"<"; struct _tagged_string
_temp97; _temp97.curr= _temp96; _temp97.base= _temp96; _temp97.last_plus_one=
_temp96 + 2; _temp97;}),( struct _tagged_string)({ char* _temp98=( char*)">";
struct _tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98;
_temp99.last_plus_one= _temp98 + 2; _temp99;}),( struct _tagged_string)({ char*
_temp100=( char*)","; struct _tagged_string _temp101; _temp101.curr= _temp100;
_temp101.base= _temp100; _temp101.last_plus_one= _temp100 + 2; _temp101;}));}
void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);} void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp102=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp110; void* _temp112;
_LL104: if(( unsigned int) _temp102 > 1u?*(( int*) _temp102) == Cyc_Absyn_Eq_constr:
0){ _LL111: _temp110=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp102)->f1;
if( _temp110 ==( void*) Cyc_Absyn_BoxKind){ goto _LL105;} else{ goto _LL106;}}
else{ goto _LL106;} _LL106: if(( unsigned int) _temp102 > 1u?*(( int*) _temp102)
== Cyc_Absyn_Eq_constr: 0){ _LL113: _temp112=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp102)->f1; goto _LL107;} else{ goto _LL108;} _LL108: goto _LL109; _LL105:
goto _LL103; _LL107: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp114=( char*)"::"; struct _tagged_string _temp115; _temp115.curr= _temp114;
_temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 3; _temp115;})); Cyc_Absyndump_dumpkind(
_temp112); goto _LL103; _LL109: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp116=( char*)"::?"; struct _tagged_string _temp117; _temp117.curr=
_temp116; _temp117.base= _temp116; _temp117.last_plus_one= _temp116 + 4;
_temp117;})); goto _LL103; _LL103:;}} void Cyc_Absyndump_dumptvars( struct Cyc_List_List*
tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({
char* _temp118=( char*)"<"; struct _tagged_string _temp119; _temp119.curr=
_temp118; _temp119.base= _temp118; _temp119.last_plus_one= _temp118 + 2;
_temp119;}),( struct _tagged_string)({ char* _temp120=( char*)">"; struct
_tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 2; _temp121;}),( struct _tagged_string)({
char* _temp122=( char*)","; struct _tagged_string _temp123; _temp123.curr=
_temp122; _temp123.base= _temp122; _temp123.last_plus_one= _temp122 + 2;
_temp123;}));} void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){((
void(*)( void(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar, tvs,( struct _tagged_string)({ char* _temp124=(
char*)"<"; struct _tagged_string _temp125; _temp125.curr= _temp124; _temp125.base=
_temp124; _temp125.last_plus_one= _temp124 + 2; _temp125;}),( struct
_tagged_string)({ char* _temp126=( char*)">"; struct _tagged_string _temp127;
_temp127.curr= _temp126; _temp127.base= _temp126; _temp127.last_plus_one=
_temp126 + 2; _temp127;}),( struct _tagged_string)({ char* _temp128=( char*)",";
struct _tagged_string _temp129; _temp129.curr= _temp128; _temp129.base= _temp128;
_temp129.last_plus_one= _temp128 + 2; _temp129;}));} struct _tuple5{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((*
pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs(
struct Cyc_List_List* ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg, ts,( struct _tagged_string)({
char* _temp130=( char*)"("; struct _tagged_string _temp131; _temp131.curr=
_temp130; _temp131.base= _temp130; _temp131.last_plus_one= _temp130 + 2;
_temp131;}),( struct _tagged_string)({ char* _temp132=( char*)")"; struct
_tagged_string _temp133; _temp133.curr= _temp132; _temp133.base= _temp132;
_temp133.last_plus_one= _temp132 + 2; _temp133;}),( struct _tagged_string)({
char* _temp134=( char*)","; struct _tagged_string _temp135; _temp135.curr=
_temp134; _temp135.base= _temp134; _temp135.last_plus_one= _temp134 + 2;
_temp135;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp136=( char*)" __attribute__(("; struct _tagged_string _temp137; _temp137.curr=
_temp136; _temp137.base= _temp136; _temp137.last_plus_one= _temp136 + 17;
_temp137;})); for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp138= atts;
if( _temp138 == 0){ _throw( Null_Exception);} _temp138->tl;})){ Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp139= atts; if(
_temp139 == 0){ _throw( Null_Exception);} _temp139->hd;}))); if(({ struct Cyc_List_List*
_temp140= atts; if( _temp140 == 0){ _throw( Null_Exception);} _temp140->tl;}) !=
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp141=( char*)",";
struct _tagged_string _temp142; _temp142.curr= _temp141; _temp142.base= _temp141;
_temp142.last_plus_one= _temp141 + 2; _temp142;}));}} Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp143=( char*)")) "; struct _tagged_string
_temp144; _temp144.curr= _temp143; _temp144.base= _temp143; _temp144.last_plus_one=
_temp143 + 4; _temp144;}));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp146=( void*)({ struct Cyc_List_List*
_temp145= tms; if( _temp145 == 0){ _throw( Null_Exception);} _temp145->hd;});
struct Cyc_Absyn_Tqual _temp152; void* _temp154; void* _temp156; _LL148: if((
unsigned int) _temp146 > 1u?*(( int*) _temp146) == Cyc_Absyn_Pointer_mod: 0){
_LL157: _temp156=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp146)->f1;
goto _LL155; _LL155: _temp154=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp146)->f2; goto _LL153; _LL153: _temp152=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp146)->f3; goto _LL149;} else{ goto _LL150;}
_LL150: goto _LL151; _LL149: return 1; _LL151: return 0; _LL147:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp158= Cyc_Tcutil_compress( t); _LL160:
if( _temp158 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL161;} else{ goto _LL162;}
_LL162: goto _LL163; _LL161: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp164=( char*)"`H"; struct _tagged_string _temp165; _temp165.curr= _temp164;
_temp165.base= _temp164; _temp165.last_plus_one= _temp164 + 3; _temp165;}));
goto _LL159; _LL163: Cyc_Absyndump_dumpntyp( t); goto _LL159; _LL159:;} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List* regions= 0;
struct Cyc_List_List* effects= 0;{ void* _temp166= Cyc_Tcutil_compress( t); void*
_temp174; struct Cyc_List_List* _temp176; _LL168: if(( unsigned int) _temp166 >
4u?*(( int*) _temp166) == Cyc_Absyn_AccessEff: 0){ _LL175: _temp174=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp166)->f1; goto _LL169;} else{ goto
_LL170;} _LL170: if(( unsigned int) _temp166 > 4u?*(( int*) _temp166) == Cyc_Absyn_JoinEff:
0){ _LL177: _temp176=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp166)->f1; goto _LL171;} else{ goto _LL172;} _LL172: goto _LL173; _LL169:
regions=({ struct Cyc_List_List* _temp178=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp178->hd=( void*) _temp174; _temp178->tl=
regions; _temp178;}); goto _LL167; _LL171: for( 0; _temp176 != 0; _temp176=({
struct Cyc_List_List* _temp179= _temp176; if( _temp179 == 0){ _throw(
Null_Exception);} _temp179->tl;})){ struct Cyc_List_List* _temp183; struct Cyc_List_List*
_temp185; struct _tuple6 _temp181= Cyc_Absyndump_effects_split(( void*)({ struct
Cyc_List_List* _temp180= _temp176; if( _temp180 == 0){ _throw( Null_Exception);}
_temp180->hd;})); _LL186: _temp185= _temp181.f1; goto _LL184; _LL184: _temp183=
_temp181.f2; goto _LL182; _LL182: regions=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp185,
regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_imp_append)( _temp183, effects);} goto _LL167;
_LL173: effects=({ struct Cyc_List_List* _temp187=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp187->hd=( void*) t; _temp187->tl=
effects; _temp187;}); goto _LL167; _LL167:;} return({ struct _tuple6 _temp188;
_temp188.f1= regions; _temp188.f2= effects; _temp188;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp191; struct Cyc_List_List* _temp193; struct
_tuple6 _temp189= Cyc_Absyndump_effects_split( t); _LL194: _temp193= _temp189.f1;
goto _LL192; _LL192: _temp191= _temp189.f2; goto _LL190; _LL190: _temp193=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp193);
_temp191=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp191); for( 0; _temp191 != 0; _temp191=({ struct Cyc_List_List* _temp195=
_temp191; if( _temp195 == 0){ _throw( Null_Exception);} _temp195->tl;})){ Cyc_Absyndump_dumpntyp((
void*)({ struct Cyc_List_List* _temp196= _temp191; if( _temp196 == 0){ _throw(
Null_Exception);} _temp196->hd;})); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp193 != 0; _temp193=({ struct Cyc_List_List* _temp197=
_temp193; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->tl;})){ Cyc_Absyndump_dumprgn((
void*)({ struct Cyc_List_List* _temp198= _temp193; if( _temp198 == 0){ _throw(
Null_Exception);} _temp198->hd;})); if(({ struct Cyc_List_List* _temp199=
_temp193; if( _temp199 == 0){ _throw( Null_Exception);} _temp199->tl;}) != 0){
Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');} void
Cyc_Absyndump_dumpntyp( void* t){ void* _temp200= t; struct Cyc_Absyn_Exp*
_temp266; struct Cyc_Absyn_Tqual _temp268; void* _temp270; struct Cyc_Absyn_FnInfo
_temp272; struct Cyc_Absyn_PtrInfo _temp274; struct Cyc_Absyn_Tvar* _temp276;
int _temp278; struct Cyc_Core_Opt* _temp280; void* _temp282; int _temp284;
struct Cyc_Core_Opt* _temp286; struct Cyc_Core_Opt _temp288; void* _temp289;
void* _temp291; struct Cyc_Absyn_TunionInfo _temp293; void* _temp295; struct Cyc_List_List*
_temp297; void* _temp299; struct Cyc_Absyn_TunionFieldInfo _temp301; struct Cyc_List_List*
_temp303; void* _temp305; struct Cyc_Absyn_Enumdecl* _temp307; struct _tuple0*
_temp309; void* _temp311; void* _temp313; void* _temp315; void* _temp317; void*
_temp319; void* _temp321; void* _temp323; void* _temp325; void* _temp327; void*
_temp329; void* _temp331; void* _temp333; void* _temp335; void* _temp337; void*
_temp339; void* _temp341; struct Cyc_List_List* _temp343; struct Cyc_Absyn_Structdecl**
_temp345; struct Cyc_List_List* _temp347; struct _tuple0* _temp349; struct Cyc_Absyn_Structdecl**
_temp351; struct Cyc_List_List* _temp353; struct _tuple0* _temp355; struct Cyc_Absyn_Uniondecl**
_temp357; struct Cyc_List_List* _temp359; struct _tuple0* _temp361; struct Cyc_Absyn_Uniondecl**
_temp363; struct Cyc_List_List* _temp365; struct _tuple0* _temp367; struct Cyc_List_List*
_temp369; struct Cyc_List_List* _temp371; struct Cyc_Core_Opt* _temp373; struct
Cyc_List_List* _temp375; struct _tuple0* _temp377; void* _temp379; void*
_temp381; struct Cyc_List_List* _temp383; _LL202: if(( unsigned int) _temp200 >
4u?*(( int*) _temp200) == Cyc_Absyn_ArrayType: 0){ _LL271: _temp270=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp200)->f1; goto _LL269; _LL269: _temp268=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*) _temp200)->f2;
goto _LL267; _LL267: _temp266=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp200)->f3; goto _LL203;} else{ goto _LL204;} _LL204: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_FnType: 0){ _LL273: _temp272=(
struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp200)->f1; goto
_LL205;} else{ goto _LL206;} _LL206: if(( unsigned int) _temp200 > 4u?*(( int*)
_temp200) == Cyc_Absyn_PointerType: 0){ _LL275: _temp274=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp200)->f1; goto _LL207;} else{ goto
_LL208;} _LL208: if( _temp200 ==( void*) Cyc_Absyn_VoidType){ goto _LL209;}
else{ goto _LL210;} _LL210: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200)
== Cyc_Absyn_VarType: 0){ _LL277: _temp276=( struct Cyc_Absyn_Tvar*)(( struct
Cyc_Absyn_VarType_struct*) _temp200)->f1; goto _LL211;} else{ goto _LL212;}
_LL212: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_Evar:
0){ _LL283: _temp282=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp200)->f1;
goto _LL281; _LL281: _temp280=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp200)->f2; if( _temp280 == 0){ goto _LL279;} else{ goto _LL214;} _LL279:
_temp278=( int)(( struct Cyc_Absyn_Evar_struct*) _temp200)->f3; goto _LL213;}
else{ goto _LL214;} _LL214: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200)
== Cyc_Absyn_Evar: 0){ _LL292: _temp291=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp200)->f1; goto _LL287; _LL287: _temp286=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp200)->f2; if( _temp286 == 0){ goto _LL216;} else{
_temp288=* _temp286; _LL290: _temp289=( void*) _temp288.v; goto _LL285;} _LL285:
_temp284=( int)(( struct Cyc_Absyn_Evar_struct*) _temp200)->f3; goto _LL215;}
else{ goto _LL216;} _LL216: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200)
== Cyc_Absyn_TunionType: 0){ _LL294: _temp293=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp200)->f1; _LL300: _temp299=( void*)
_temp293.tunion_info; goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)
_temp293.targs; goto _LL296; _LL296: _temp295=( void*) _temp293.rgn; goto _LL217;}
else{ goto _LL218;} _LL218: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200)
== Cyc_Absyn_TunionFieldType: 0){ _LL302: _temp301=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp200)->f1; _LL306: _temp305=( void*)
_temp301.field_info; goto _LL304; _LL304: _temp303=( struct Cyc_List_List*)
_temp301.targs; goto _LL219;} else{ goto _LL220;} _LL220: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_EnumType: 0){ _LL310: _temp309=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp200)->f1; goto _LL308;
_LL308: _temp307=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp200)->f2; goto _LL221;} else{ goto _LL222;} _LL222: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL314: _temp313=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp313 ==( void*)
Cyc_Absyn_Signed){ goto _LL312;} else{ goto _LL224;} _LL312: _temp311=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp311 ==( void*) Cyc_Absyn_B1){
goto _LL223;} else{ goto _LL224;}} else{ goto _LL224;} _LL224: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL318: _temp317=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp317 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL316;} else{ goto _LL226;} _LL316: _temp315=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp315 ==( void*) Cyc_Absyn_B1){
goto _LL225;} else{ goto _LL226;}} else{ goto _LL226;} _LL226: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL322: _temp321=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp321 ==( void*)
Cyc_Absyn_Signed){ goto _LL320;} else{ goto _LL228;} _LL320: _temp319=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp319 ==( void*) Cyc_Absyn_B2){
goto _LL227;} else{ goto _LL228;}} else{ goto _LL228;} _LL228: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL326: _temp325=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp325 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL324;} else{ goto _LL230;} _LL324: _temp323=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp323 ==( void*) Cyc_Absyn_B2){
goto _LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL330: _temp329=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp329 ==( void*)
Cyc_Absyn_Signed){ goto _LL328;} else{ goto _LL232;} _LL328: _temp327=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp327 ==( void*) Cyc_Absyn_B4){
goto _LL231;} else{ goto _LL232;}} else{ goto _LL232;} _LL232: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL334: _temp333=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp333 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL332;} else{ goto _LL234;} _LL332: _temp331=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp331 ==( void*) Cyc_Absyn_B4){
goto _LL233;} else{ goto _LL234;}} else{ goto _LL234;} _LL234: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL338: _temp337=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp337 ==( void*)
Cyc_Absyn_Signed){ goto _LL336;} else{ goto _LL236;} _LL336: _temp335=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp335 ==( void*) Cyc_Absyn_B8){
goto _LL235;} else{ goto _LL236;}} else{ goto _LL236;} _LL236: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_IntType: 0){ _LL342: _temp341=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp200)->f1; if( _temp341 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL340;} else{ goto _LL238;} _LL340: _temp339=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp200)->f2; if( _temp339 ==( void*) Cyc_Absyn_B8){
goto _LL237;} else{ goto _LL238;}} else{ goto _LL238;} _LL238: if( _temp200 ==(
void*) Cyc_Absyn_FloatType){ goto _LL239;} else{ goto _LL240;} _LL240: if(
_temp200 ==( void*) Cyc_Absyn_DoubleType){ goto _LL241;} else{ goto _LL242;}
_LL242: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_TupleType:
0){ _LL344: _temp343=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp200)->f1; goto _LL243;} else{ goto _LL244;} _LL244: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_StructType: 0){ _LL350: _temp349=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp200)->f1; if(
_temp349 == 0){ goto _LL348;} else{ goto _LL246;} _LL348: _temp347=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp200)->f2; goto _LL346; _LL346:
_temp345=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp200)->f3; goto _LL245;} else{ goto _LL246;} _LL246: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_StructType: 0){ _LL356: _temp355=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp200)->f1; goto
_LL354; _LL354: _temp353=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp200)->f2; goto _LL352; _LL352: _temp351=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp200)->f3; goto _LL247;} else{ goto
_LL248;} _LL248: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_UnionType:
0){ _LL362: _temp361=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp200)->f1; if( _temp361 == 0){ goto _LL360;} else{ goto _LL250;} _LL360:
_temp359=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp200)->f2;
goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp200)->f3; goto _LL249;} else{ goto _LL250;} _LL250: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_UnionType: 0){ _LL368: _temp367=(
struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp200)->f1; goto
_LL366; _LL366: _temp365=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp200)->f2; goto _LL364; _LL364: _temp363=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp200)->f3; goto _LL251;} else{ goto
_LL252;} _LL252: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_AnonStructType:
0){ _LL370: _temp369=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp200)->f1; goto _LL253;} else{ goto _LL254;} _LL254: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_AnonUnionType: 0){ _LL372:
_temp371=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp200)->f1; goto _LL255;} else{ goto _LL256;} _LL256: if(( unsigned int)
_temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_TypedefType: 0){ _LL378: _temp377=(
struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp200)->f1; goto
_LL376; _LL376: _temp375=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp200)->f2; goto _LL374; _LL374: _temp373=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp200)->f3; goto _LL257;} else{ goto _LL258;}
_LL258: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_RgnHandleType:
0){ _LL380: _temp379=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*) _temp200)->f1;
goto _LL259;} else{ goto _LL260;} _LL260: if( _temp200 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL261;} else{ goto _LL262;} _LL262: if(( unsigned int) _temp200 > 4u?*((
int*) _temp200) == Cyc_Absyn_AccessEff: 0){ _LL382: _temp381=( void*)(( struct
Cyc_Absyn_AccessEff_struct*) _temp200)->f1; goto _LL263;} else{ goto _LL264;}
_LL264: if(( unsigned int) _temp200 > 4u?*(( int*) _temp200) == Cyc_Absyn_JoinEff:
0){ _LL384: _temp383=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*)
_temp200)->f1; goto _LL265;} else{ goto _LL201;} _LL203: return; _LL205: return;
_LL207: return; _LL209: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp385=( char*)"void"; struct _tagged_string _temp386; _temp386.curr= _temp385;
_temp386.base= _temp385; _temp386.last_plus_one= _temp385 + 5; _temp386;}));
return; _LL211: Cyc_Absyndump_dump_str( _temp276->name); return; _LL213: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp387=( char*)"%"; struct _tagged_string
_temp388; _temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 2; _temp388;})); Cyc_Absyndump_dumpkind( _temp282); Cyc_Absyndump_dump(
xprintf("(%d)", _temp278)); return; _LL215: Cyc_Absyndump_dumpntyp( _temp289);
return; _LL217:{ void* _temp389= _temp299; struct Cyc_Absyn_UnknownTunionInfo
_temp395; int _temp397; struct _tuple0* _temp399; struct Cyc_Absyn_Tuniondecl*
_temp401; struct Cyc_Absyn_Tuniondecl _temp403; int _temp404; struct Cyc_Core_Opt*
_temp406; struct Cyc_List_List* _temp408; struct _tuple0* _temp410; void*
_temp412; _LL391: if(*(( int*) _temp389) == Cyc_Absyn_UnknownTunion){ _LL396:
_temp395=( struct Cyc_Absyn_UnknownTunionInfo)(( struct Cyc_Absyn_UnknownTunion_struct*)
_temp389)->f1; _LL400: _temp399=( struct _tuple0*) _temp395.name; goto _LL398;
_LL398: _temp397=( int) _temp395.is_xtunion; goto _LL392;} else{ goto _LL393;}
_LL393: if(*(( int*) _temp389) == Cyc_Absyn_KnownTunion){ _LL402: _temp401=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunion_struct*) _temp389)->f1;
_temp403=* _temp401; _LL413: _temp412=( void*) _temp403.sc; goto _LL411; _LL411:
_temp410=( struct _tuple0*) _temp403.name; goto _LL409; _LL409: _temp408=(
struct Cyc_List_List*) _temp403.tvs; goto _LL407; _LL407: _temp406=( struct Cyc_Core_Opt*)
_temp403.fields; goto _LL405; _LL405: _temp404=( int) _temp403.is_xtunion; goto
_LL394;} else{ goto _LL390;} _LL392: _temp410= _temp399; _temp404= _temp397;
goto _LL394; _LL394: if( _temp404){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp414=( char*)"xtunion "; struct _tagged_string _temp415; _temp415.curr=
_temp414; _temp415.base= _temp414; _temp415.last_plus_one= _temp414 + 9;
_temp415;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp416=( char*)"tunion "; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 8;
_temp417;}));}{ void* _temp418= Cyc_Tcutil_compress( _temp295); _LL420: if(
_temp418 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL421;} else{ goto _LL422;} _LL422:
goto _LL423; _LL421: goto _LL419; _LL423: Cyc_Absyndump_dumptyp( _temp295); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp424=( char*)" "; struct _tagged_string
_temp425; _temp425.curr= _temp424; _temp425.base= _temp424; _temp425.last_plus_one=
_temp424 + 2; _temp425;})); goto _LL419; _LL419:;} Cyc_Absyndump_dumpqvar(
_temp410); Cyc_Absyndump_dumptps( _temp297); goto _LL390; _LL390:;} goto _LL201;
_LL219:{ void* _temp426= _temp305; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp432; int _temp434; struct _tuple0* _temp436; struct _tuple0* _temp438;
struct Cyc_Absyn_Tunionfield* _temp440; struct Cyc_Absyn_Tunionfield _temp442;
void* _temp443; struct Cyc_Position_Segment* _temp445; struct Cyc_List_List*
_temp447; struct Cyc_List_List* _temp449; struct _tuple0* _temp451; struct Cyc_Absyn_Tuniondecl*
_temp453; struct Cyc_Absyn_Tuniondecl _temp455; int _temp456; struct Cyc_Core_Opt*
_temp458; struct Cyc_List_List* _temp460; struct _tuple0* _temp462; void*
_temp464; _LL428: if(*(( int*) _temp426) == Cyc_Absyn_UnknownTunionfield){
_LL433: _temp432=( struct Cyc_Absyn_UnknownTunionFieldInfo)(( struct Cyc_Absyn_UnknownTunionfield_struct*)
_temp426)->f1; _LL439: _temp438=( struct _tuple0*) _temp432.tunion_name; goto
_LL437; _LL437: _temp436=( struct _tuple0*) _temp432.field_name; goto _LL435;
_LL435: _temp434=( int) _temp432.is_xtunion; goto _LL429;} else{ goto _LL430;}
_LL430: if(*(( int*) _temp426) == Cyc_Absyn_KnownTunionfield){ _LL454: _temp453=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp426)->f1; _temp455=* _temp453; _LL465: _temp464=( void*) _temp455.sc; goto
_LL463; _LL463: _temp462=( struct _tuple0*) _temp455.name; goto _LL461; _LL461:
_temp460=( struct Cyc_List_List*) _temp455.tvs; goto _LL459; _LL459: _temp458=(
struct Cyc_Core_Opt*) _temp455.fields; goto _LL457; _LL457: _temp456=( int)
_temp455.is_xtunion; goto _LL441; _LL441: _temp440=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp426)->f2; _temp442=* _temp440;
_LL452: _temp451=( struct _tuple0*) _temp442.name; goto _LL450; _LL450: _temp449=(
struct Cyc_List_List*) _temp442.tvs; goto _LL448; _LL448: _temp447=( struct Cyc_List_List*)
_temp442.typs; goto _LL446; _LL446: _temp445=( struct Cyc_Position_Segment*)
_temp442.loc; goto _LL444; _LL444: _temp443=( void*) _temp442.sc; goto _LL431;}
else{ goto _LL427;} _LL429: _temp462= _temp438; _temp456= _temp434; _temp451=
_temp436; goto _LL431; _LL431: if( _temp456){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp466=( char*)"xtunion "; struct _tagged_string
_temp467; _temp467.curr= _temp466; _temp467.base= _temp466; _temp467.last_plus_one=
_temp466 + 9; _temp467;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp468=( char*)"tunion "; struct _tagged_string _temp469; _temp469.curr=
_temp468; _temp469.base= _temp468; _temp469.last_plus_one= _temp468 + 8;
_temp469;}));} Cyc_Absyndump_dumpqvar( _temp462); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp470=( char*)"."; struct _tagged_string _temp471;
_temp471.curr= _temp470; _temp471.base= _temp470; _temp471.last_plus_one=
_temp470 + 2; _temp471;})); Cyc_Absyndump_dumpqvar( _temp451); Cyc_Absyndump_dumptps(
_temp303); goto _LL427; _LL427:;} goto _LL201; _LL221: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp472=( char*)"enum "; struct _tagged_string
_temp473; _temp473.curr= _temp472; _temp473.base= _temp472; _temp473.last_plus_one=
_temp472 + 6; _temp473;})); Cyc_Absyndump_dumpqvar( _temp309); return; _LL223:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp474=( char*)"signed char";
struct _tagged_string _temp475; _temp475.curr= _temp474; _temp475.base= _temp474;
_temp475.last_plus_one= _temp474 + 12; _temp475;})); return; _LL225: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp476=( char*)"char"; struct _tagged_string
_temp477; _temp477.curr= _temp476; _temp477.base= _temp476; _temp477.last_plus_one=
_temp476 + 5; _temp477;})); return; _LL227: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp478=( char*)"short"; struct _tagged_string _temp479;
_temp479.curr= _temp478; _temp479.base= _temp478; _temp479.last_plus_one=
_temp478 + 6; _temp479;})); return; _LL229: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp480=( char*)"unsigned short"; struct _tagged_string
_temp481; _temp481.curr= _temp480; _temp481.base= _temp480; _temp481.last_plus_one=
_temp480 + 15; _temp481;})); return; _LL231: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp482=( char*)"int"; struct _tagged_string _temp483;
_temp483.curr= _temp482; _temp483.base= _temp482; _temp483.last_plus_one=
_temp482 + 4; _temp483;})); return; _LL233: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp484=( char*)"unsigned int"; struct _tagged_string
_temp485; _temp485.curr= _temp484; _temp485.base= _temp484; _temp485.last_plus_one=
_temp484 + 13; _temp485;})); return; _LL235: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp486=( char*)"long long"; struct _tagged_string
_temp487; _temp487.curr= _temp486; _temp487.base= _temp486; _temp487.last_plus_one=
_temp486 + 10; _temp487;})); return; _LL237: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp488=( char*)"unsigned long long"; struct
_tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 19; _temp489;})); return; _LL239: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp490=( char*)"float"; struct _tagged_string
_temp491; _temp491.curr= _temp490; _temp491.base= _temp490; _temp491.last_plus_one=
_temp490 + 6; _temp491;})); return; _LL241: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp492=( char*)"double"; struct _tagged_string
_temp493; _temp493.curr= _temp492; _temp493.base= _temp492; _temp493.last_plus_one=
_temp492 + 7; _temp493;})); return; _LL243: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp343); return; _LL245: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp494=( char*)"struct"; struct _tagged_string
_temp495; _temp495.curr= _temp494; _temp495.base= _temp494; _temp495.last_plus_one=
_temp494 + 7; _temp495;})); Cyc_Absyndump_dumptps( _temp347); return; _LL247:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp496=( char*)"struct";
struct _tagged_string _temp497; _temp497.curr= _temp496; _temp497.base= _temp496;
_temp497.last_plus_one= _temp496 + 7; _temp497;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp498= _temp355; if( _temp498 == 0){
_throw( Null_Exception);} _temp498;})); Cyc_Absyndump_dumptps( _temp353);
return; _LL249: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp499=(
char*)"union"; struct _tagged_string _temp500; _temp500.curr= _temp499; _temp500.base=
_temp499; _temp500.last_plus_one= _temp499 + 6; _temp500;})); Cyc_Absyndump_dumptps(
_temp359); return; _LL251: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp501=( char*)"union"; struct _tagged_string _temp502; _temp502.curr=
_temp501; _temp502.base= _temp501; _temp502.last_plus_one= _temp501 + 6;
_temp502;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct _tuple0*
_temp503= _temp367; if( _temp503 == 0){ _throw( Null_Exception);} _temp503;}));
Cyc_Absyndump_dumptps( _temp365); return; _LL253: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp504=( char*)"struct {"; struct _tagged_string
_temp505; _temp505.curr= _temp504; _temp505.base= _temp504; _temp505.last_plus_one=
_temp504 + 9; _temp505;})); Cyc_Absyndump_dumpstructfields( _temp369); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp506=( char*)"}"; struct _tagged_string
_temp507; _temp507.curr= _temp506; _temp507.base= _temp506; _temp507.last_plus_one=
_temp506 + 2; _temp507;})); return; _LL255: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp508=( char*)"union {"; struct _tagged_string
_temp509; _temp509.curr= _temp508; _temp509.base= _temp508; _temp509.last_plus_one=
_temp508 + 8; _temp509;})); Cyc_Absyndump_dumpstructfields( _temp371); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp510=( char*)"}"; struct _tagged_string
_temp511; _temp511.curr= _temp510; _temp511.base= _temp510; _temp511.last_plus_one=
_temp510 + 2; _temp511;})); return; _LL257: Cyc_Absyndump_dumpqvar( _temp377),
Cyc_Absyndump_dumptps( _temp375); return; _LL259: Cyc_Absyndump_dumprgn(
_temp379); return; _LL261: return; _LL263: return; _LL265: return; _LL201:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)({ struct Cyc_Core_Opt* _temp512= vo; if( _temp512 == 0){
_throw( Null_Exception);} _temp512->v;}));}} void Cyc_Absyndump_dumpfunarg(
struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct
Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((* t).f2,(* t).f3,
Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs( struct Cyc_List_List*
args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char(( int)'(');
for( 0; args != 0; args=({ struct Cyc_List_List* _temp513= args; if( _temp513 ==
0){ _throw( Null_Exception);} _temp513->tl;})){ Cyc_Absyndump_dumpfunarg((
struct _tuple1*)({ struct Cyc_List_List* _temp514= args; if( _temp514 == 0){
_throw( Null_Exception);} _temp514->hd;})); if(({ struct Cyc_List_List* _temp515=
args; if( _temp515 == 0){ _throw( Null_Exception);} _temp515->tl;}) != 0? 1:
varargs){ Cyc_Absyndump_dump_char(( int)',');}} if( varargs){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp516=( char*)"..."; struct _tagged_string
_temp517; _temp517.curr= _temp516; _temp517.base= _temp516; _temp517.last_plus_one=
_temp516 + 4; _temp517;}));} if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff((
void*)({ struct Cyc_Core_Opt* _temp518= effopt; if( _temp518 == 0){ _throw(
Null_Exception);} _temp518->v;}));} Cyc_Absyndump_dump_char(( int)')');} void
Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*, void(*
f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual _temp519;
_temp519.q_const= 0; _temp519.q_volatile= 0; _temp519.q_restrict= 0; _temp519;}),
t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp520= d; struct Cyc_Absyn_Exp* _temp526; struct
_tagged_string* _temp528; _LL522: if(*(( int*) _temp520) == Cyc_Absyn_ArrayElement){
_LL527: _temp526=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp520)->f1; goto _LL523;} else{ goto _LL524;} _LL524: if(*(( int*) _temp520)
== Cyc_Absyn_FieldName){ _LL529: _temp528=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp520)->f1; goto _LL525;} else{ goto _LL521;} _LL523: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp530=( char*)".["; struct _tagged_string
_temp531; _temp531.curr= _temp530; _temp531.base= _temp530; _temp531.last_plus_one=
_temp530 + 3; _temp531;})); Cyc_Absyndump_dumpexp( _temp526); Cyc_Absyndump_dump_char((
int)']'); goto _LL521; _LL525: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp528); goto _LL521; _LL521:;} struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp532=(
char*)""; struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base=
_temp532; _temp533.last_plus_one= _temp532 + 1; _temp533;}),( struct
_tagged_string)({ char* _temp534=( char*)"="; struct _tagged_string _temp535;
_temp535.curr= _temp534; _temp535.base= _temp534; _temp535.last_plus_one=
_temp534 + 2; _temp535;}),( struct _tagged_string)({ char* _temp536=( char*)"=";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 2; _temp537;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp538=(
char*)""; struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base=
_temp538; _temp539.last_plus_one= _temp538 + 1; _temp539;}),( struct
_tagged_string)({ char* _temp540=( char*)""; struct _tagged_string _temp541;
_temp541.curr= _temp540; _temp541.base= _temp540; _temp541.last_plus_one=
_temp540 + 1; _temp541;}),( struct _tagged_string)({ char* _temp542=( char*)",";
struct _tagged_string _temp543; _temp543.curr= _temp542; _temp543.base= _temp542;
_temp543.last_plus_one= _temp542 + 2; _temp543;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp544=( char*)"("; struct _tagged_string _temp545; _temp545.curr= _temp544;
_temp545.base= _temp544; _temp545.last_plus_one= _temp544 + 2; _temp545;}));}{
void* _temp546=( void*) e->r; void* _temp638; char _temp640; void* _temp642;
void* _temp644; short _temp646; void* _temp648; void* _temp650; int _temp652;
void* _temp654; void* _temp656; int _temp658; void* _temp660; void* _temp662;
long long _temp664; void* _temp666; void* _temp668; struct _tagged_string
_temp670; void* _temp672; void* _temp674; struct _tagged_string _temp676; struct
_tuple0* _temp678; void* _temp680; struct _tuple0* _temp682; struct Cyc_List_List*
_temp684; void* _temp686; struct Cyc_Absyn_Exp* _temp688; struct Cyc_Core_Opt*
_temp690; struct Cyc_Absyn_Exp* _temp692; void* _temp694; struct Cyc_Absyn_Exp*
_temp696; void* _temp698; struct Cyc_Absyn_Exp* _temp700; void* _temp702; struct
Cyc_Absyn_Exp* _temp704; void* _temp706; struct Cyc_Absyn_Exp* _temp708; struct
Cyc_Absyn_Exp* _temp710; struct Cyc_Absyn_Exp* _temp712; struct Cyc_Absyn_Exp*
_temp714; struct Cyc_Absyn_Exp* _temp716; struct Cyc_Absyn_Exp* _temp718; struct
Cyc_List_List* _temp720; struct Cyc_Absyn_Exp* _temp722; struct Cyc_List_List*
_temp724; struct Cyc_Absyn_Exp* _temp726; struct Cyc_Absyn_Exp* _temp728; struct
Cyc_Absyn_Exp* _temp730; struct Cyc_List_List* _temp732; struct Cyc_Absyn_Exp*
_temp734; struct Cyc_Absyn_Exp* _temp736; void* _temp738; struct Cyc_Absyn_Exp*
_temp740; struct Cyc_Absyn_Exp* _temp742; struct Cyc_Absyn_Exp* _temp744; void*
_temp746; struct Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Exp* _temp750; struct
_tagged_string* _temp752; struct Cyc_Absyn_Exp* _temp754; struct _tagged_string*
_temp756; struct Cyc_Absyn_Exp* _temp758; struct Cyc_Absyn_Exp* _temp760; struct
Cyc_Absyn_Exp* _temp762; struct Cyc_List_List* _temp764; struct Cyc_List_List*
_temp766; struct _tuple1* _temp768; struct Cyc_List_List* _temp770; struct Cyc_Absyn_Exp*
_temp772; struct Cyc_Absyn_Exp* _temp774; struct Cyc_Absyn_Vardecl* _temp776;
struct Cyc_Absyn_Structdecl* _temp778; struct Cyc_List_List* _temp780; struct
Cyc_Core_Opt* _temp782; struct _tuple0* _temp784; struct Cyc_List_List* _temp786;
void* _temp788; struct Cyc_Absyn_Tunionfield* _temp790; struct Cyc_Absyn_Tuniondecl*
_temp792; struct Cyc_List_List* _temp794; struct Cyc_Core_Opt* _temp796; struct
Cyc_Core_Opt* _temp798; struct Cyc_Absyn_Enumfield* _temp800; struct Cyc_Absyn_Enumdecl*
_temp802; struct _tuple0* _temp804; void* _temp806; struct Cyc_Absyn_Exp*
_temp808; struct Cyc_List_List* _temp810; struct Cyc_Core_Opt* _temp812; struct
Cyc_Absyn_Stmt* _temp814; struct Cyc_Absyn_Fndecl* _temp816; struct Cyc_Absyn_Exp*
_temp818; _LL548: if(*(( int*) _temp546) == Cyc_Absyn_Const_e){ _LL639: _temp638=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp546)->f1; if(( unsigned int)
_temp638 > 1u?*(( int*) _temp638) == Cyc_Absyn_Char_c: 0){ _LL643: _temp642=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp638)->f1; goto _LL641; _LL641:
_temp640=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp638)->f2; goto _LL549;}
else{ goto _LL550;}} else{ goto _LL550;} _LL550: if(*(( int*) _temp546) == Cyc_Absyn_Const_e){
_LL645: _temp644=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp546)->f1; if((
unsigned int) _temp644 > 1u?*(( int*) _temp644) == Cyc_Absyn_Short_c: 0){ _LL649:
_temp648=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp644)->f1; goto _LL647;
_LL647: _temp646=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp644)->f2;
goto _LL551;} else{ goto _LL552;}} else{ goto _LL552;} _LL552: if(*(( int*)
_temp546) == Cyc_Absyn_Const_e){ _LL651: _temp650=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp546)->f1; if(( unsigned int) _temp650 > 1u?*(( int*) _temp650) == Cyc_Absyn_Int_c:
0){ _LL655: _temp654=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp650)->f1;
if( _temp654 ==( void*) Cyc_Absyn_Signed){ goto _LL653;} else{ goto _LL554;}
_LL653: _temp652=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp650)->f2; goto
_LL553;} else{ goto _LL554;}} else{ goto _LL554;} _LL554: if(*(( int*) _temp546)
== Cyc_Absyn_Const_e){ _LL657: _temp656=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp546)->f1; if(( unsigned int) _temp656 > 1u?*(( int*) _temp656) == Cyc_Absyn_Int_c:
0){ _LL661: _temp660=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp656)->f1;
if( _temp660 ==( void*) Cyc_Absyn_Unsigned){ goto _LL659;} else{ goto _LL556;}
_LL659: _temp658=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp656)->f2; goto
_LL555;} else{ goto _LL556;}} else{ goto _LL556;} _LL556: if(*(( int*) _temp546)
== Cyc_Absyn_Const_e){ _LL663: _temp662=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp546)->f1; if(( unsigned int) _temp662 > 1u?*(( int*) _temp662) == Cyc_Absyn_LongLong_c:
0){ _LL667: _temp666=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp662)->f1;
goto _LL665; _LL665: _temp664=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp662)->f2; goto _LL557;} else{ goto _LL558;}} else{ goto _LL558;} _LL558:
if(*(( int*) _temp546) == Cyc_Absyn_Const_e){ _LL669: _temp668=( void*)(( struct
Cyc_Absyn_Const_e_struct*) _temp546)->f1; if(( unsigned int) _temp668 > 1u?*((
int*) _temp668) == Cyc_Absyn_Float_c: 0){ _LL671: _temp670=( struct
_tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp668)->f1; goto _LL559;}
else{ goto _LL560;}} else{ goto _LL560;} _LL560: if(*(( int*) _temp546) == Cyc_Absyn_Const_e){
_LL673: _temp672=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp546)->f1; if(
_temp672 ==( void*) Cyc_Absyn_Null_c){ goto _LL561;} else{ goto _LL562;}} else{
goto _LL562;} _LL562: if(*(( int*) _temp546) == Cyc_Absyn_Const_e){ _LL675:
_temp674=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp546)->f1; if((
unsigned int) _temp674 > 1u?*(( int*) _temp674) == Cyc_Absyn_String_c: 0){
_LL677: _temp676=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp674)->f1; goto _LL563;} else{ goto _LL564;}} else{ goto _LL564;} _LL564:
if(*(( int*) _temp546) == Cyc_Absyn_UnknownId_e){ _LL679: _temp678=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp546)->f1; goto _LL565;}
else{ goto _LL566;} _LL566: if(*(( int*) _temp546) == Cyc_Absyn_Var_e){ _LL683:
_temp682=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp546)->f1;
goto _LL681; _LL681: _temp680=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp546)->f2; goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp546)
== Cyc_Absyn_Primop_e){ _LL687: _temp686=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp546)->f1; goto _LL685; _LL685: _temp684=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp546)->f2; goto _LL569;} else{ goto _LL570;}
_LL570: if(*(( int*) _temp546) == Cyc_Absyn_AssignOp_e){ _LL693: _temp692=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp546)->f1;
goto _LL691; _LL691: _temp690=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp546)->f2; goto _LL689; _LL689: _temp688=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp546)->f3; goto _LL571;} else{ goto _LL572;}
_LL572: if(*(( int*) _temp546) == Cyc_Absyn_Increment_e){ _LL697: _temp696=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp546)->f1;
goto _LL695; _LL695: _temp694=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f2; if( _temp694 ==( void*) Cyc_Absyn_PreInc){ goto _LL573;} else{
goto _LL574;}} else{ goto _LL574;} _LL574: if(*(( int*) _temp546) == Cyc_Absyn_Increment_e){
_LL701: _temp700=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f1; goto _LL699; _LL699: _temp698=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f2; if( _temp698 ==( void*) Cyc_Absyn_PreDec){ goto _LL575;} else{
goto _LL576;}} else{ goto _LL576;} _LL576: if(*(( int*) _temp546) == Cyc_Absyn_Increment_e){
_LL705: _temp704=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f1; goto _LL703; _LL703: _temp702=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f2; if( _temp702 ==( void*) Cyc_Absyn_PostInc){ goto _LL577;} else{
goto _LL578;}} else{ goto _LL578;} _LL578: if(*(( int*) _temp546) == Cyc_Absyn_Increment_e){
_LL709: _temp708=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f1; goto _LL707; _LL707: _temp706=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp546)->f2; if( _temp706 ==( void*) Cyc_Absyn_PostDec){ goto _LL579;} else{
goto _LL580;}} else{ goto _LL580;} _LL580: if(*(( int*) _temp546) == Cyc_Absyn_Conditional_e){
_LL715: _temp714=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp546)->f1; goto _LL713; _LL713: _temp712=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp546)->f2; goto _LL711; _LL711: _temp710=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp546)->f3;
goto _LL581;} else{ goto _LL582;} _LL582: if(*(( int*) _temp546) == Cyc_Absyn_SeqExp_e){
_LL719: _temp718=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp546)->f1; goto _LL717; _LL717: _temp716=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp546)->f2; goto _LL583;} else{ goto _LL584;}
_LL584: if(*(( int*) _temp546) == Cyc_Absyn_UnknownCall_e){ _LL723: _temp722=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp546)->f1;
goto _LL721; _LL721: _temp720=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp546)->f2; goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp546)
== Cyc_Absyn_FnCall_e){ _LL727: _temp726=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp546)->f1; goto _LL725; _LL725: _temp724=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp546)->f2; goto _LL587;} else{ goto _LL588;}
_LL588: if(*(( int*) _temp546) == Cyc_Absyn_Throw_e){ _LL729: _temp728=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp546)->f1; goto _LL589;}
else{ goto _LL590;} _LL590: if(*(( int*) _temp546) == Cyc_Absyn_NoInstantiate_e){
_LL731: _temp730=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp546)->f1; goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp546)
== Cyc_Absyn_Instantiate_e){ _LL735: _temp734=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp546)->f1; goto _LL733; _LL733: _temp732=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp546)->f2;
goto _LL593;} else{ goto _LL594;} _LL594: if(*(( int*) _temp546) == Cyc_Absyn_Cast_e){
_LL739: _temp738=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp546)->f1; goto
_LL737; _LL737: _temp736=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp546)->f2; goto _LL595;} else{ goto _LL596;} _LL596: if(*(( int*) _temp546)
== Cyc_Absyn_Address_e){ _LL741: _temp740=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp546)->f1; goto _LL597;} else{ goto _LL598;} _LL598: if(*(( int*) _temp546)
== Cyc_Absyn_New_e){ _LL745: _temp744=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp546)->f1; goto _LL743; _LL743: _temp742=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp546)->f2; goto _LL599;} else{ goto _LL600;} _LL600:
if(*(( int*) _temp546) == Cyc_Absyn_Sizeoftyp_e){ _LL747: _temp746=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp546)->f1; goto _LL601;} else{ goto
_LL602;} _LL602: if(*(( int*) _temp546) == Cyc_Absyn_Sizeofexp_e){ _LL749:
_temp748=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp546)->f1; goto _LL603;} else{ goto _LL604;} _LL604: if(*(( int*) _temp546)
== Cyc_Absyn_Deref_e){ _LL751: _temp750=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp546)->f1; goto _LL605;} else{ goto _LL606;} _LL606: if(*(( int*) _temp546)
== Cyc_Absyn_StructMember_e){ _LL755: _temp754=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp546)->f1; goto _LL753; _LL753: _temp752=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp546)->f2;
goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp546) == Cyc_Absyn_StructArrow_e){
_LL759: _temp758=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp546)->f1; goto _LL757; _LL757: _temp756=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp546)->f2; goto _LL609;} else{ goto _LL610;}
_LL610: if(*(( int*) _temp546) == Cyc_Absyn_Subscript_e){ _LL763: _temp762=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp546)->f1;
goto _LL761; _LL761: _temp760=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp546)->f2; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp546)
== Cyc_Absyn_Tuple_e){ _LL765: _temp764=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp546)->f1; goto _LL613;} else{ goto _LL614;} _LL614: if(*(( int*) _temp546)
== Cyc_Absyn_CompoundLit_e){ _LL769: _temp768=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp546)->f1; goto _LL767; _LL767: _temp766=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp546)->f2; goto _LL615;} else{ goto _LL616;}
_LL616: if(*(( int*) _temp546) == Cyc_Absyn_Array_e){ _LL771: _temp770=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp546)->f1; goto _LL617;}
else{ goto _LL618;} _LL618: if(*(( int*) _temp546) == Cyc_Absyn_Comprehension_e){
_LL777: _temp776=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp546)->f1; goto _LL775; _LL775: _temp774=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp546)->f2; goto _LL773; _LL773: _temp772=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp546)->f3;
goto _LL619;} else{ goto _LL620;} _LL620: if(*(( int*) _temp546) == Cyc_Absyn_Struct_e){
_LL785: _temp784=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp546)->f1; goto _LL783; _LL783: _temp782=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp546)->f2; goto _LL781; _LL781: _temp780=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp546)->f3; goto
_LL779; _LL779: _temp778=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp546)->f4; goto _LL621;} else{ goto _LL622;} _LL622: if(*(( int*) _temp546)
== Cyc_Absyn_AnonStruct_e){ _LL789: _temp788=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp546)->f1; goto _LL787; _LL787: _temp786=( struct Cyc_List_List*)(( struct
Cyc_Absyn_AnonStruct_e_struct*) _temp546)->f2; goto _LL623;} else{ goto _LL624;}
_LL624: if(*(( int*) _temp546) == Cyc_Absyn_Tunion_e){ _LL799: _temp798=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp546)->f1; goto _LL797;
_LL797: _temp796=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp546)->f2; goto _LL795; _LL795: _temp794=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp546)->f3; goto _LL793; _LL793: _temp792=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp546)->f4;
goto _LL791; _LL791: _temp790=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp546)->f5; goto _LL625;} else{ goto _LL626;} _LL626: if(*(( int*) _temp546)
== Cyc_Absyn_Enum_e){ _LL805: _temp804=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp546)->f1; goto _LL803; _LL803: _temp802=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp546)->f2; goto _LL801; _LL801: _temp800=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp546)->f3;
goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp546) == Cyc_Absyn_Malloc_e){
_LL809: _temp808=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp546)->f1; goto _LL807; _LL807: _temp806=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp546)->f2; goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp546)
== Cyc_Absyn_UnresolvedMem_e){ _LL813: _temp812=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp546)->f1; goto _LL811; _LL811: _temp810=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp546)->f2;
goto _LL631;} else{ goto _LL632;} _LL632: if(*(( int*) _temp546) == Cyc_Absyn_StmtExp_e){
_LL815: _temp814=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp546)->f1; goto _LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp546)
== Cyc_Absyn_Codegen_e){ _LL817: _temp816=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp546)->f1; goto _LL635;} else{ goto _LL636;}
_LL636: if(*(( int*) _temp546) == Cyc_Absyn_Fill_e){ _LL819: _temp818=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp546)->f1; goto _LL637;}
else{ goto _LL547;} _LL549: Cyc_Absyndump_dump_char(( int)'\''); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp640)); Cyc_Absyndump_dump_char(( int)'\''); goto
_LL547; _LL551: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp820=(
char*)"(short)"; struct _tagged_string _temp821; _temp821.curr= _temp820;
_temp821.base= _temp820; _temp821.last_plus_one= _temp820 + 8; _temp821;})); Cyc_Absyndump_dump_nospace(
Cyc_Core_string_of_int(( int) _temp646)); goto _LL547; _LL553: Cyc_Absyndump_dump(
Cyc_Core_string_of_int( _temp652)); goto _LL547; _LL555: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp658)); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp822=(
char*)"u"; struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base=
_temp822; _temp823.last_plus_one= _temp822 + 2; _temp823;})); goto _LL547;
_LL557: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp824=( char*)"<<FIX LONG LONG CONSTANT>>";
struct _tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 27; _temp825;})); goto _LL547; _LL559: Cyc_Absyndump_dump(
_temp670); goto _LL547; _LL561: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp826=( char*)"null"; struct _tagged_string _temp827; _temp827.curr=
_temp826; _temp827.base= _temp826; _temp827.last_plus_one= _temp826 + 5;
_temp827;})); goto _LL547; _LL563: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp676)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL547; _LL565: _temp682= _temp678; goto _LL567; _LL567: Cyc_Absyndump_dumpqvar(
_temp682); goto _LL547; _LL569: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp686); if( ! Cyc_Absyn_is_format_prim( _temp686)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp684)){ case 1: _LL828: if( _temp686 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp830= _temp684; if( _temp830 == 0){ _throw(
Null_Exception);} _temp830->hd;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp831=( char*)".size"; struct _tagged_string _temp832; _temp832.curr=
_temp831; _temp832.base= _temp831; _temp832.last_plus_one= _temp831 + 6;
_temp832;}));} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp833= _temp684; if(
_temp833 == 0){ _throw( Null_Exception);} _temp833->hd;}));} break; case 2:
_LL829: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp835= _temp684; if( _temp835 == 0){ _throw( Null_Exception);} _temp835->hd;}));
Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp837=({ struct Cyc_List_List* _temp836= _temp684; if(
_temp836 == 0){ _throw( Null_Exception);} _temp836->tl;}); if( _temp837 == 0){
_throw( Null_Exception);} _temp837->hd;})); break; default: _LL834:( void)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp839=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp839[ 0]=({ struct Cyc_Core_Failure_struct
_temp840; _temp840.tag= Cyc_Core_Failure; _temp840.f1=( struct _tagged_string)({
char* _temp841=( char*)"Absyndump -- Bad number of arguments to primop"; struct
_tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 47; _temp842;}); _temp840;}); _temp839;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp843=( char*)"("; struct _tagged_string _temp844;
_temp844.curr= _temp843; _temp844.base= _temp843; _temp844.last_plus_one=
_temp843 + 2; _temp844;})); Cyc_Absyndump_dumpexps_prec( 20, _temp684); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp845=( char*)")"; struct _tagged_string
_temp846; _temp846.curr= _temp845; _temp846.base= _temp845; _temp846.last_plus_one=
_temp845 + 2; _temp846;}));} goto _LL547;} _LL571: Cyc_Absyndump_dumpexp_prec(
myprec, _temp692); if( _temp690 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)({ struct Cyc_Core_Opt* _temp847= _temp690; if( _temp847 == 0){ _throw(
Null_Exception);} _temp847->v;})));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp848=( char*)"="; struct _tagged_string _temp849;
_temp849.curr= _temp848; _temp849.base= _temp848; _temp849.last_plus_one=
_temp848 + 2; _temp849;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp688); goto
_LL547; _LL573: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp850=(
char*)"++"; struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base=
_temp850; _temp851.last_plus_one= _temp850 + 3; _temp851;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp696); goto _LL547; _LL575: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp852=( char*)"--"; struct _tagged_string _temp853;
_temp853.curr= _temp852; _temp853.base= _temp852; _temp853.last_plus_one=
_temp852 + 3; _temp853;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp700); goto
_LL547; _LL577: Cyc_Absyndump_dumpexp_prec( myprec, _temp704); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp854=( char*)"++"; struct _tagged_string
_temp855; _temp855.curr= _temp854; _temp855.base= _temp854; _temp855.last_plus_one=
_temp854 + 3; _temp855;})); goto _LL547; _LL579: Cyc_Absyndump_dumpexp_prec(
myprec, _temp708); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp856=(
char*)"--"; struct _tagged_string _temp857; _temp857.curr= _temp856; _temp857.base=
_temp856; _temp857.last_plus_one= _temp856 + 3; _temp857;})); goto _LL547;
_LL581: Cyc_Absyndump_dumpexp_prec( myprec, _temp714); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp712); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp710); goto _LL547; _LL583:
Cyc_Absyndump_dumpexp_prec( myprec, _temp718); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp716); goto _LL547; _LL585: _temp726=
_temp722; _temp724= _temp720; goto _LL587; _LL587: Cyc_Absyndump_dumpexp_prec(
myprec, _temp726); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp858=( char*)"("; struct _tagged_string _temp859; _temp859.curr= _temp858;
_temp859.base= _temp858; _temp859.last_plus_one= _temp858 + 2; _temp859;})); Cyc_Absyndump_dumpexps_prec(
20, _temp724); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp860=( char*)")"; struct _tagged_string _temp861; _temp861.curr= _temp860;
_temp861.base= _temp860; _temp861.last_plus_one= _temp860 + 2; _temp861;}));
goto _LL547; _LL589: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp862=( char*)"throw"; struct _tagged_string _temp863; _temp863.curr=
_temp862; _temp863.base= _temp862; _temp863.last_plus_one= _temp862 + 6;
_temp863;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp728); goto _LL547; _LL591:
_temp734= _temp730; goto _LL593; _LL593: Cyc_Absyndump_dumpexp_prec( inprec,
_temp734); goto _LL547; _LL595: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp738); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp736); goto _LL547; _LL597: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp740); goto _LL547; _LL599: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp864=( char*)"new "; struct _tagged_string _temp865;
_temp865.curr= _temp864; _temp865.base= _temp864; _temp865.last_plus_one=
_temp864 + 5; _temp865;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp742); goto
_LL547; _LL601: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp866=(
char*)"sizeof("; struct _tagged_string _temp867; _temp867.curr= _temp866;
_temp867.base= _temp866; _temp867.last_plus_one= _temp866 + 8; _temp867;})); Cyc_Absyndump_dumptyp(
_temp746); Cyc_Absyndump_dump_char(( int)')'); goto _LL547; _LL603: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp868=( char*)"sizeof("; struct _tagged_string
_temp869; _temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 8; _temp869;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp748); Cyc_Absyndump_dump_char((
int)')'); goto _LL547; _LL605: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp750); goto _LL547; _LL607: Cyc_Absyndump_dumpexp_prec( myprec,
_temp754); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp752); goto _LL547; _LL609: Cyc_Absyndump_dumpexp_prec( myprec, _temp758);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp870=( char*)"->";
struct _tagged_string _temp871; _temp871.curr= _temp870; _temp871.base= _temp870;
_temp871.last_plus_one= _temp870 + 3; _temp871;})); Cyc_Absyndump_dump_nospace(*
_temp756); goto _LL547; _LL611: Cyc_Absyndump_dumpexp_prec( myprec, _temp762);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp760); Cyc_Absyndump_dump_char((
int)']'); goto _LL547; _LL613: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp872=( char*)"$("; struct _tagged_string _temp873; _temp873.curr=
_temp872; _temp873.base= _temp872; _temp873.last_plus_one= _temp872 + 3;
_temp873;})); Cyc_Absyndump_dumpexps_prec( 20, _temp764); Cyc_Absyndump_dump_char((
int)')'); goto _LL547; _LL615: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp768).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp766,( struct _tagged_string)({ char* _temp874=( char*)"{"; struct
_tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 2; _temp875;}),( struct _tagged_string)({
char* _temp876=( char*)"}"; struct _tagged_string _temp877; _temp877.curr=
_temp876; _temp877.base= _temp876; _temp877.last_plus_one= _temp876 + 2;
_temp877;}),( struct _tagged_string)({ char* _temp878=( char*)","; struct
_tagged_string _temp879; _temp879.curr= _temp878; _temp879.base= _temp878;
_temp879.last_plus_one= _temp878 + 2; _temp879;})); goto _LL547; _LL617:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp770,( struct _tagged_string)({ char* _temp880=( char*)"{";
struct _tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 2; _temp881;}),( struct _tagged_string)({
char* _temp882=( char*)"}"; struct _tagged_string _temp883; _temp883.curr=
_temp882; _temp883.base= _temp882; _temp883.last_plus_one= _temp882 + 2;
_temp883;}),( struct _tagged_string)({ char* _temp884=( char*)","; struct
_tagged_string _temp885; _temp885.curr= _temp884; _temp885.base= _temp884;
_temp885.last_plus_one= _temp884 + 2; _temp885;})); goto _LL547; _LL619: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp886=( char*)"new {for"; struct
_tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 9; _temp887;})); Cyc_Absyndump_dump_str((*
_temp776->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp774); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp772);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL547; _LL621: Cyc_Absyndump_dumpqvar(
_temp784);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp780,( struct
_tagged_string)({ char* _temp888=( char*)"{"; struct _tagged_string _temp889;
_temp889.curr= _temp888; _temp889.base= _temp888; _temp889.last_plus_one=
_temp888 + 2; _temp889;}),( struct _tagged_string)({ char* _temp890=( char*)"}";
struct _tagged_string _temp891; _temp891.curr= _temp890; _temp891.base= _temp890;
_temp891.last_plus_one= _temp890 + 2; _temp891;}),( struct _tagged_string)({
char* _temp892=( char*)","; struct _tagged_string _temp893; _temp893.curr=
_temp892; _temp893.base= _temp892; _temp893.last_plus_one= _temp892 + 2;
_temp893;})); goto _LL547; _LL623:(( void(*)( void(* f)( struct _tuple7*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp786,(
struct _tagged_string)({ char* _temp894=( char*)"{"; struct _tagged_string
_temp895; _temp895.curr= _temp894; _temp895.base= _temp894; _temp895.last_plus_one=
_temp894 + 2; _temp895;}),( struct _tagged_string)({ char* _temp896=( char*)"}";
struct _tagged_string _temp897; _temp897.curr= _temp896; _temp897.base= _temp896;
_temp897.last_plus_one= _temp896 + 2; _temp897;}),( struct _tagged_string)({
char* _temp898=( char*)","; struct _tagged_string _temp899; _temp899.curr=
_temp898; _temp899.base= _temp898; _temp899.last_plus_one= _temp898 + 2;
_temp899;})); goto _LL547; _LL625: Cyc_Absyndump_dumpqvar( _temp790->name); if(
_temp794 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpexp, _temp794,( struct
_tagged_string)({ char* _temp900=( char*)"("; struct _tagged_string _temp901;
_temp901.curr= _temp900; _temp901.base= _temp900; _temp901.last_plus_one=
_temp900 + 2; _temp901;}),( struct _tagged_string)({ char* _temp902=( char*)")";
struct _tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 2; _temp903;}),( struct _tagged_string)({
char* _temp904=( char*)","; struct _tagged_string _temp905; _temp905.curr=
_temp904; _temp905.base= _temp904; _temp905.last_plus_one= _temp904 + 2;
_temp905;}));} goto _LL547; _LL627: Cyc_Absyndump_dumpqvar( _temp804); goto
_LL547; _LL629: if( _temp808 != 0){ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp906=( char*)"rmalloc("; struct _tagged_string _temp907; _temp907.curr=
_temp906; _temp907.base= _temp906; _temp907.last_plus_one= _temp906 + 9;
_temp907;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp908= _temp808; if( _temp908 == 0){ _throw( Null_Exception);} _temp908;}));
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp909=( char*)","; struct
_tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 2; _temp910;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp911=( char*)"malloc("; struct _tagged_string
_temp912; _temp912.curr= _temp911; _temp912.base= _temp911; _temp912.last_plus_one=
_temp911 + 8; _temp912;}));} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp913=( char*)"sizeof("; struct _tagged_string _temp914; _temp914.curr=
_temp913; _temp914.base= _temp913; _temp914.last_plus_one= _temp913 + 8;
_temp914;})); Cyc_Absyndump_dumptyp( _temp806); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp915=( char*)"))"; struct _tagged_string _temp916;
_temp916.curr= _temp915; _temp916.base= _temp915; _temp916.last_plus_one=
_temp915 + 3; _temp916;})); goto _LL547; _LL631:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp810,( struct _tagged_string)({ char* _temp917=( char*)"{"; struct
_tagged_string _temp918; _temp918.curr= _temp917; _temp918.base= _temp917;
_temp918.last_plus_one= _temp917 + 2; _temp918;}),( struct _tagged_string)({
char* _temp919=( char*)"}"; struct _tagged_string _temp920; _temp920.curr=
_temp919; _temp920.base= _temp919; _temp920.last_plus_one= _temp919 + 2;
_temp920;}),( struct _tagged_string)({ char* _temp921=( char*)","; struct
_tagged_string _temp922; _temp922.curr= _temp921; _temp922.base= _temp921;
_temp922.last_plus_one= _temp921 + 2; _temp922;})); goto _LL547; _LL633: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp923=( char*)"({"; struct _tagged_string
_temp924; _temp924.curr= _temp923; _temp924.base= _temp923; _temp924.last_plus_one=
_temp923 + 3; _temp924;})); Cyc_Absyndump_dumpstmt( _temp814); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp925=( char*)"})"; struct _tagged_string
_temp926; _temp926.curr= _temp925; _temp926.base= _temp925; _temp926.last_plus_one=
_temp925 + 3; _temp926;})); goto _LL547; _LL635: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp927=( char*)"codegen("; struct _tagged_string
_temp928; _temp928.curr= _temp927; _temp928.base= _temp927; _temp928.last_plus_one=
_temp927 + 9; _temp928;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp929=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp929->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp930=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp930[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp931; _temp931.tag= Cyc_Absyn_Fn_d;
_temp931.f1= _temp816; _temp931;}); _temp930;})); _temp929->loc= e->loc;
_temp929;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp932=(
char*)")"; struct _tagged_string _temp933; _temp933.curr= _temp932; _temp933.base=
_temp932; _temp933.last_plus_one= _temp932 + 2; _temp933;})); goto _LL547;
_LL637: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp934=( char*)"fill(";
struct _tagged_string _temp935; _temp935.curr= _temp934; _temp935.base= _temp934;
_temp935.last_plus_one= _temp934 + 6; _temp935;})); Cyc_Absyndump_dumpexp(
_temp818); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp936=( char*)")";
struct _tagged_string _temp937; _temp937.curr= _temp936; _temp937.base= _temp936;
_temp937.last_plus_one= _temp936 + 2; _temp937;})); goto _LL547; _LL547:;} if(
inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}} void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec( 0, e);} void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List* scs){ for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp938= scs; if( _temp938 == 0){ _throw( Null_Exception);} _temp938->tl;})){
struct Cyc_Absyn_Switch_clause* _temp940=( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp939= scs; if( _temp939 == 0){ _throw( Null_Exception);}
_temp939->hd;}); goto _LL941; _LL941: if( _temp940->where_clause == 0?( void*)(
_temp940->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp942=( char*)"default:"; struct
_tagged_string _temp943; _temp943.curr= _temp942; _temp943.base= _temp942;
_temp943.last_plus_one= _temp942 + 9; _temp943;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp944=( char*)"case"; struct _tagged_string
_temp945; _temp945.curr= _temp944; _temp945.base= _temp944; _temp945.last_plus_one=
_temp944 + 5; _temp945;})); Cyc_Absyndump_dumppat( _temp940->pattern); if(
_temp940->where_clause != 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp946=( char*)"&&"; struct _tagged_string _temp947; _temp947.curr= _temp946;
_temp947.base= _temp946; _temp947.last_plus_one= _temp946 + 3; _temp947;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp948= _temp940->where_clause;
if( _temp948 == 0){ _throw( Null_Exception);} _temp948;}));} Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp949=( char*)":"; struct _tagged_string
_temp950; _temp950.curr= _temp949; _temp950.base= _temp949; _temp950.last_plus_one=
_temp949 + 2; _temp950;}));} Cyc_Absyndump_dumpstmt( _temp940->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp951=( void*) s->r; struct Cyc_Absyn_Exp*
_temp995; struct Cyc_Absyn_Stmt* _temp997; struct Cyc_Absyn_Stmt* _temp999;
struct Cyc_Absyn_Exp* _temp1001; struct Cyc_Absyn_Exp* _temp1003; struct Cyc_Absyn_Stmt*
_temp1005; struct Cyc_Absyn_Stmt* _temp1007; struct Cyc_Absyn_Exp* _temp1009;
struct Cyc_Absyn_Stmt* _temp1011; struct _tuple2 _temp1013; struct Cyc_Absyn_Stmt*
_temp1015; struct Cyc_Absyn_Exp* _temp1017; struct Cyc_Absyn_Stmt* _temp1019;
struct Cyc_Absyn_Stmt* _temp1021; struct Cyc_Absyn_Stmt* _temp1023; struct
_tagged_string* _temp1025; struct Cyc_Absyn_Switch_clause** _temp1027; struct
Cyc_List_List* _temp1029; struct Cyc_Absyn_Switch_clause** _temp1031; struct Cyc_List_List*
_temp1033; struct Cyc_Absyn_Stmt* _temp1035; struct _tuple2 _temp1037; struct
Cyc_Absyn_Stmt* _temp1039; struct Cyc_Absyn_Exp* _temp1041; struct _tuple2
_temp1043; struct Cyc_Absyn_Stmt* _temp1045; struct Cyc_Absyn_Exp* _temp1047;
struct Cyc_Absyn_Exp* _temp1049; struct Cyc_List_List* _temp1051; struct Cyc_Absyn_Exp*
_temp1053; struct Cyc_Absyn_Stmt* _temp1055; struct Cyc_Absyn_Decl* _temp1057;
struct Cyc_Absyn_Stmt* _temp1059; struct Cyc_Absyn_Stmt* _temp1061; struct Cyc_Absyn_Stmt*
_temp1063; struct _tagged_string* _temp1065; struct _tuple2 _temp1067; struct
Cyc_Absyn_Stmt* _temp1069; struct Cyc_Absyn_Exp* _temp1071; struct Cyc_Absyn_Stmt*
_temp1073; struct Cyc_List_List* _temp1075; struct Cyc_Absyn_Stmt* _temp1077;
struct Cyc_Absyn_Stmt* _temp1079; struct Cyc_Absyn_Vardecl* _temp1081; struct
Cyc_Absyn_Tvar* _temp1083; _LL953: if( _temp951 ==( void*) Cyc_Absyn_Skip_s){
goto _LL954;} else{ goto _LL955;} _LL955: if(( unsigned int) _temp951 > 1u?*((
int*) _temp951) == Cyc_Absyn_Exp_s: 0){ _LL996: _temp995=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp951)->f1; goto _LL956;} else{ goto _LL957;}
_LL957: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Seq_s:
0){ _LL1000: _temp999=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp951)->f1; goto _LL998; _LL998: _temp997=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp951)->f2; goto _LL958;} else{ goto _LL959;} _LL959:
if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Return_s: 0){
_LL1002: _temp1001=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp951)->f1; if( _temp1001 == 0){ goto _LL960;} else{ goto _LL961;}} else{
goto _LL961;} _LL961: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) ==
Cyc_Absyn_Return_s: 0){ _LL1004: _temp1003=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp951)->f1; goto _LL962;} else{ goto _LL963;} _LL963: if(( unsigned int)
_temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_IfThenElse_s: 0){ _LL1010:
_temp1009=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp951)->f1; goto _LL1008; _LL1008: _temp1007=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp951)->f2; goto _LL1006; _LL1006:
_temp1005=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp951)->f3; goto _LL964;} else{ goto _LL965;} _LL965: if(( unsigned int)
_temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_While_s: 0){ _LL1014: _temp1013=(
struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp951)->f1; _LL1018:
_temp1017= _temp1013.f1; goto _LL1016; _LL1016: _temp1015= _temp1013.f2; goto
_LL1012; _LL1012: _temp1011=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp951)->f2; goto _LL966;} else{ goto _LL967;} _LL967: if(( unsigned int)
_temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Break_s: 0){ _LL1020: _temp1019=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp951)->f1; goto
_LL968;} else{ goto _LL969;} _LL969: if(( unsigned int) _temp951 > 1u?*(( int*)
_temp951) == Cyc_Absyn_Continue_s: 0){ _LL1022: _temp1021=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp951)->f1; goto _LL970;} else{ goto
_LL971;} _LL971: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Goto_s:
0){ _LL1026: _temp1025=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp951)->f1; goto _LL1024; _LL1024: _temp1023=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp951)->f2; goto _LL972;} else{ goto _LL973;}
_LL973: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Fallthru_s:
0){ _LL1030: _temp1029=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp951)->f1; if( _temp1029 == 0){ goto _LL1028;} else{ goto _LL975;} _LL1028:
_temp1027=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp951)->f2; goto _LL974;} else{ goto _LL975;} _LL975: if(( unsigned int)
_temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Fallthru_s: 0){ _LL1034:
_temp1033=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp951)->f1; goto _LL1032; _LL1032: _temp1031=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp951)->f2; goto _LL976;} else{ goto
_LL977;} _LL977: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_For_s:
0){ _LL1050: _temp1049=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp951)->f1; goto _LL1044; _LL1044: _temp1043=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp951)->f2; _LL1048: _temp1047= _temp1043.f1; goto _LL1046; _LL1046:
_temp1045= _temp1043.f2; goto _LL1038; _LL1038: _temp1037=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp951)->f3; _LL1042: _temp1041= _temp1037.f1;
goto _LL1040; _LL1040: _temp1039= _temp1037.f2; goto _LL1036; _LL1036: _temp1035=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp951)->f4; goto
_LL978;} else{ goto _LL979;} _LL979: if(( unsigned int) _temp951 > 1u?*(( int*)
_temp951) == Cyc_Absyn_Switch_s: 0){ _LL1054: _temp1053=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp951)->f1; goto _LL1052; _LL1052:
_temp1051=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp951)->f2;
goto _LL980;} else{ goto _LL981;} _LL981: if(( unsigned int) _temp951 > 1u?*((
int*) _temp951) == Cyc_Absyn_Decl_s: 0){ _LL1058: _temp1057=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp951)->f1; goto _LL1056; _LL1056: _temp1055=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp951)->f2; goto
_LL982;} else{ goto _LL983;} _LL983: if(( unsigned int) _temp951 > 1u?*(( int*)
_temp951) == Cyc_Absyn_Cut_s: 0){ _LL1060: _temp1059=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp951)->f1; goto _LL984;} else{ goto _LL985;}
_LL985: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Splice_s:
0){ _LL1062: _temp1061=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp951)->f1; goto _LL986;} else{ goto _LL987;} _LL987: if(( unsigned int)
_temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Label_s: 0){ _LL1066: _temp1065=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp951)->f1; goto
_LL1064; _LL1064: _temp1063=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp951)->f2; goto _LL988;} else{ goto _LL989;} _LL989: if(( unsigned int)
_temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Do_s: 0){ _LL1074: _temp1073=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp951)->f1; goto
_LL1068; _LL1068: _temp1067=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp951)->f2; _LL1072: _temp1071= _temp1067.f1; goto _LL1070; _LL1070:
_temp1069= _temp1067.f2; goto _LL990;} else{ goto _LL991;} _LL991: if((
unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_TryCatch_s: 0){
_LL1078: _temp1077=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp951)->f1; goto _LL1076; _LL1076: _temp1075=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp951)->f2; goto _LL992;} else{ goto
_LL993;} _LL993: if(( unsigned int) _temp951 > 1u?*(( int*) _temp951) == Cyc_Absyn_Region_s:
0){ _LL1084: _temp1083=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp951)->f1; goto _LL1082; _LL1082: _temp1081=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp951)->f2; goto _LL1080; _LL1080:
_temp1079=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp951)->f3; goto _LL994;} else{ goto _LL952;} _LL954: Cyc_Absyndump_dump_semi();
goto _LL952; _LL956: Cyc_Absyndump_dumpexp( _temp995); Cyc_Absyndump_dump_semi();
goto _LL952; _LL958: if( Cyc_Absynpp_is_declaration( _temp999)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp999); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp999);} if( Cyc_Absynpp_is_declaration(
_temp997)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp997);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp997);}
goto _LL952; _LL960: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1085=( char*)"return;"; struct _tagged_string _temp1086; _temp1086.curr=
_temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one= _temp1085 + 8;
_temp1086;})); goto _LL952; _LL962: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1087=( char*)"return"; struct _tagged_string _temp1088; _temp1088.curr=
_temp1087; _temp1088.base= _temp1087; _temp1088.last_plus_one= _temp1087 + 7;
_temp1088;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1089= _temp1003; if( _temp1089 == 0){ _throw( Null_Exception);} _temp1089;}));
Cyc_Absyndump_dump_semi(); goto _LL952; _LL964: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1090=( char*)"if("; struct _tagged_string _temp1091;
_temp1091.curr= _temp1090; _temp1091.base= _temp1090; _temp1091.last_plus_one=
_temp1090 + 4; _temp1091;})); Cyc_Absyndump_dumpexp( _temp1009); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1092=( char*)"){"; struct _tagged_string
_temp1093; _temp1093.curr= _temp1092; _temp1093.base= _temp1092; _temp1093.last_plus_one=
_temp1092 + 3; _temp1093;})); Cyc_Absyndump_dumpstmt( _temp1007); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp1094=( void*) _temp1005->r; _LL1096: if( _temp1094 ==(
void*) Cyc_Absyn_Skip_s){ goto _LL1097;} else{ goto _LL1098;} _LL1098: goto
_LL1099; _LL1097: goto _LL1095; _LL1099: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1100=( char*)"else{"; struct _tagged_string
_temp1101; _temp1101.curr= _temp1100; _temp1101.base= _temp1100; _temp1101.last_plus_one=
_temp1100 + 6; _temp1101;})); Cyc_Absyndump_dumpstmt( _temp1005); Cyc_Absyndump_dump_char((
int)'}'); goto _LL1095; _LL1095:;} goto _LL952; _LL966: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1102=( char*)"while("; struct _tagged_string
_temp1103; _temp1103.curr= _temp1102; _temp1103.base= _temp1102; _temp1103.last_plus_one=
_temp1102 + 7; _temp1103;})); Cyc_Absyndump_dumpexp( _temp1017); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1104=( char*)") {"; struct _tagged_string
_temp1105; _temp1105.curr= _temp1104; _temp1105.base= _temp1104; _temp1105.last_plus_one=
_temp1104 + 4; _temp1105;})); Cyc_Absyndump_dumpstmt( _temp1011); Cyc_Absyndump_dump_char((
int)'}'); goto _LL952; _LL968: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1106=( char*)"break;"; struct _tagged_string _temp1107; _temp1107.curr=
_temp1106; _temp1107.base= _temp1106; _temp1107.last_plus_one= _temp1106 + 7;
_temp1107;})); goto _LL952; _LL970: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1108=( char*)"continue;"; struct _tagged_string _temp1109; _temp1109.curr=
_temp1108; _temp1109.base= _temp1108; _temp1109.last_plus_one= _temp1108 + 10;
_temp1109;})); goto _LL952; _LL972: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1110=( char*)"goto"; struct _tagged_string _temp1111; _temp1111.curr=
_temp1110; _temp1111.base= _temp1110; _temp1111.last_plus_one= _temp1110 + 5;
_temp1111;})); Cyc_Absyndump_dump_str( _temp1025); Cyc_Absyndump_dump_semi();
goto _LL952; _LL974: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1112=( char*)"fallthru;"; struct _tagged_string _temp1113; _temp1113.curr=
_temp1112; _temp1113.base= _temp1112; _temp1113.last_plus_one= _temp1112 + 10;
_temp1113;})); goto _LL952; _LL976: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1114=( char*)"fallthru("; struct _tagged_string _temp1115; _temp1115.curr=
_temp1114; _temp1115.base= _temp1114; _temp1115.last_plus_one= _temp1114 + 10;
_temp1115;})); Cyc_Absyndump_dumpexps_prec( 20, _temp1033); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1116=( char*)");"; struct _tagged_string
_temp1117; _temp1117.curr= _temp1116; _temp1117.base= _temp1116; _temp1117.last_plus_one=
_temp1116 + 3; _temp1117;})); goto _LL952; _LL978: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1118=( char*)"for("; struct _tagged_string
_temp1119; _temp1119.curr= _temp1118; _temp1119.base= _temp1118; _temp1119.last_plus_one=
_temp1118 + 5; _temp1119;})); Cyc_Absyndump_dumpexp( _temp1049); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp1047); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1041); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1120=( char*)"){"; struct _tagged_string _temp1121; _temp1121.curr=
_temp1120; _temp1121.base= _temp1120; _temp1121.last_plus_one= _temp1120 + 3;
_temp1121;})); Cyc_Absyndump_dumpstmt( _temp1035); Cyc_Absyndump_dump_char(( int)'}');
goto _LL952; _LL980: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1122=( char*)"switch("; struct _tagged_string _temp1123; _temp1123.curr=
_temp1122; _temp1123.base= _temp1122; _temp1123.last_plus_one= _temp1122 + 8;
_temp1123;})); Cyc_Absyndump_dumpexp( _temp1053); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1124=( char*)"){"; struct _tagged_string
_temp1125; _temp1125.curr= _temp1124; _temp1125.base= _temp1124; _temp1125.last_plus_one=
_temp1124 + 3; _temp1125;})); Cyc_Absyndump_dumpswitchclauses( _temp1051); Cyc_Absyndump_dump_char((
int)'}'); goto _LL952; _LL982: Cyc_Absyndump_dumpdecl( _temp1057); Cyc_Absyndump_dumpstmt(
_temp1055); goto _LL952; _LL984: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1126=( char*)"cut"; struct _tagged_string _temp1127; _temp1127.curr=
_temp1126; _temp1127.base= _temp1126; _temp1127.last_plus_one= _temp1126 + 4;
_temp1127;})); Cyc_Absyndump_dumpstmt( _temp1059); goto _LL952; _LL986: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1128=( char*)"splice"; struct _tagged_string
_temp1129; _temp1129.curr= _temp1128; _temp1129.base= _temp1128; _temp1129.last_plus_one=
_temp1128 + 7; _temp1129;})); Cyc_Absyndump_dumpstmt( _temp1061); goto _LL952;
_LL988: if( Cyc_Absynpp_is_declaration( _temp1063)){ Cyc_Absyndump_dump_str(
_temp1065); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1130=( char*)": {"; struct _tagged_string _temp1131; _temp1131.curr=
_temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one= _temp1130 + 4;
_temp1131;})); Cyc_Absyndump_dumpstmt( _temp1063); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1065); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1063);} goto _LL952; _LL990: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1132=( char*)"do {"; struct _tagged_string
_temp1133; _temp1133.curr= _temp1132; _temp1133.base= _temp1132; _temp1133.last_plus_one=
_temp1132 + 5; _temp1133;})); Cyc_Absyndump_dumpstmt( _temp1073); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1134=( char*)"} while ("; struct
_tagged_string _temp1135; _temp1135.curr= _temp1134; _temp1135.base= _temp1134;
_temp1135.last_plus_one= _temp1134 + 10; _temp1135;})); Cyc_Absyndump_dumpexp(
_temp1071); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1136=( char*)");"; struct _tagged_string _temp1137; _temp1137.curr=
_temp1136; _temp1137.base= _temp1136; _temp1137.last_plus_one= _temp1136 + 3;
_temp1137;})); goto _LL952; _LL992: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1138=( char*)"try"; struct _tagged_string _temp1139; _temp1139.curr=
_temp1138; _temp1139.base= _temp1138; _temp1139.last_plus_one= _temp1138 + 4;
_temp1139;})); Cyc_Absyndump_dumpstmt( _temp1077); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1140=( char*)"catch {"; struct _tagged_string
_temp1141; _temp1141.curr= _temp1140; _temp1141.base= _temp1140; _temp1141.last_plus_one=
_temp1140 + 8; _temp1141;})); Cyc_Absyndump_dumpswitchclauses( _temp1075); Cyc_Absyndump_dump_char((
int)'}'); goto _LL952; _LL994: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1142=( char*)"region<"; struct _tagged_string _temp1143; _temp1143.curr=
_temp1142; _temp1143.base= _temp1142; _temp1143.last_plus_one= _temp1142 + 8;
_temp1143;})); Cyc_Absyndump_dumptvar( _temp1083); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1144=( char*)"> "; struct _tagged_string _temp1145;
_temp1145.curr= _temp1144; _temp1145.base= _temp1144; _temp1145.last_plus_one=
_temp1144 + 3; _temp1145;})); Cyc_Absyndump_dumpqvar( _temp1081->name); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1146=( char*)"{"; struct _tagged_string
_temp1147; _temp1147.curr= _temp1146; _temp1147.base= _temp1146; _temp1147.last_plus_one=
_temp1146 + 2; _temp1147;})); Cyc_Absyndump_dumpstmt( _temp1079); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1148=( char*)"}"; struct _tagged_string
_temp1149; _temp1149.curr= _temp1148; _temp1149.base= _temp1148; _temp1149.last_plus_one=
_temp1148 + 2; _temp1149;})); goto _LL952; _LL952:;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char*
_temp1150=( char*)""; struct _tagged_string _temp1151; _temp1151.curr= _temp1150;
_temp1151.base= _temp1150; _temp1151.last_plus_one= _temp1150 + 1; _temp1151;}),(
struct _tagged_string)({ char* _temp1152=( char*)"="; struct _tagged_string
_temp1153; _temp1153.curr= _temp1152; _temp1153.base= _temp1152; _temp1153.last_plus_one=
_temp1152 + 2; _temp1153;}),( struct _tagged_string)({ char* _temp1154=( char*)"=";
struct _tagged_string _temp1155; _temp1155.curr= _temp1154; _temp1155.base=
_temp1154; _temp1155.last_plus_one= _temp1154 + 2; _temp1155;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1156=(
void*) p->r; int _temp1190; void* _temp1192; int _temp1194; void* _temp1196;
char _temp1198; struct _tagged_string _temp1200; struct Cyc_Absyn_Vardecl*
_temp1202; struct Cyc_List_List* _temp1204; struct Cyc_Absyn_Pat* _temp1206;
struct Cyc_Absyn_Vardecl* _temp1208; struct _tuple0* _temp1210; struct Cyc_List_List*
_temp1212; struct Cyc_List_List* _temp1214; struct _tuple0* _temp1216; struct
Cyc_List_List* _temp1218; struct Cyc_List_List* _temp1220; struct _tuple0*
_temp1222; struct Cyc_List_List* _temp1224; struct Cyc_List_List* _temp1226;
struct Cyc_Core_Opt* _temp1228; struct Cyc_Absyn_Structdecl* _temp1230; struct
Cyc_List_List* _temp1232; struct Cyc_List_List* _temp1234; struct Cyc_Absyn_Tunionfield*
_temp1236; struct Cyc_Absyn_Tuniondecl* _temp1238; struct Cyc_Absyn_Enumfield*
_temp1240; struct Cyc_Absyn_Enumdecl* _temp1242; _LL1158: if( _temp1156 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1159;} else{ goto _LL1160;} _LL1160: if( _temp1156 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1161;} else{ goto _LL1162;} _LL1162: if((
unsigned int) _temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Int_p: 0){
_LL1193: _temp1192=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1156)->f1;
if( _temp1192 ==( void*) Cyc_Absyn_Signed){ goto _LL1191;} else{ goto _LL1164;}
_LL1191: _temp1190=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp1156)->f2; goto
_LL1163;} else{ goto _LL1164;} _LL1164: if(( unsigned int) _temp1156 > 2u?*((
int*) _temp1156) == Cyc_Absyn_Int_p: 0){ _LL1197: _temp1196=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1156)->f1; if( _temp1196 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1195;} else{ goto _LL1166;} _LL1195: _temp1194=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1156)->f2; goto _LL1165;} else{ goto _LL1166;} _LL1166: if(( unsigned int)
_temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Char_p: 0){ _LL1199: _temp1198=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp1156)->f1; goto _LL1167;} else{
goto _LL1168;} _LL1168: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156)
== Cyc_Absyn_Float_p: 0){ _LL1201: _temp1200=( struct _tagged_string)(( struct
Cyc_Absyn_Float_p_struct*) _temp1156)->f1; goto _LL1169;} else{ goto _LL1170;}
_LL1170: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Var_p:
0){ _LL1203: _temp1202=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1156)->f1; goto _LL1171;} else{ goto _LL1172;} _LL1172: if(( unsigned int)
_temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Tuple_p: 0){ _LL1205: _temp1204=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp1156)->f1; goto
_LL1173;} else{ goto _LL1174;} _LL1174: if(( unsigned int) _temp1156 > 2u?*((
int*) _temp1156) == Cyc_Absyn_Pointer_p: 0){ _LL1207: _temp1206=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp1156)->f1; goto _LL1175;} else{ goto
_LL1176;} _LL1176: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Reference_p:
0){ _LL1209: _temp1208=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1156)->f1; goto _LL1177;} else{ goto _LL1178;} _LL1178: if(( unsigned int)
_temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_UnknownId_p: 0){ _LL1211:
_temp1210=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1156)->f1;
goto _LL1179;} else{ goto _LL1180;} _LL1180: if(( unsigned int) _temp1156 > 2u?*((
int*) _temp1156) == Cyc_Absyn_UnknownCall_p: 0){ _LL1217: _temp1216=( struct
_tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp1156)->f1; goto _LL1215;
_LL1215: _temp1214=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1156)->f2; goto _LL1213; _LL1213: _temp1212=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1156)->f3; goto _LL1181;} else{
goto _LL1182;} _LL1182: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156)
== Cyc_Absyn_UnknownFields_p: 0){ _LL1223: _temp1222=( struct _tuple0*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp1156)->f1; goto _LL1221; _LL1221:
_temp1220=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1156)->f2; goto _LL1219; _LL1219: _temp1218=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1156)->f3; goto _LL1183;} else{
goto _LL1184;} _LL1184: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156)
== Cyc_Absyn_Struct_p: 0){ _LL1231: _temp1230=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1156)->f1; goto _LL1229; _LL1229:
_temp1228=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp1156)->f2;
goto _LL1227; _LL1227: _temp1226=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1156)->f3; goto _LL1225; _LL1225: _temp1224=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1156)->f4; goto _LL1185;} else{ goto
_LL1186;} _LL1186: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Tunion_p:
0){ _LL1239: _temp1238=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1156)->f1; goto _LL1237; _LL1237: _temp1236=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1156)->f2; goto _LL1235; _LL1235:
_temp1234=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1156)->f3; goto _LL1233; _LL1233: _temp1232=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1156)->f4; goto _LL1187;} else{ goto
_LL1188;} _LL1188: if(( unsigned int) _temp1156 > 2u?*(( int*) _temp1156) == Cyc_Absyn_Enum_p:
0){ _LL1243: _temp1242=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1156)->f1; goto _LL1241; _LL1241: _temp1240=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1156)->f2; goto _LL1189;} else{ goto
_LL1157;} _LL1159: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1157; _LL1161:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1244=( char*)"null";
struct _tagged_string _temp1245; _temp1245.curr= _temp1244; _temp1245.base=
_temp1244; _temp1245.last_plus_one= _temp1244 + 5; _temp1245;})); goto _LL1157;
_LL1163: Cyc_Absyndump_dump( xprintf("%d", _temp1190)); goto _LL1157; _LL1165:
Cyc_Absyndump_dump( xprintf("%u",( unsigned int) _temp1194)); goto _LL1157;
_LL1167: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1246=( char*)"'";
struct _tagged_string _temp1247; _temp1247.curr= _temp1246; _temp1247.base=
_temp1246; _temp1247.last_plus_one= _temp1246 + 2; _temp1247;})); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape( _temp1198)); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1248=( char*)"'"; struct _tagged_string _temp1249;
_temp1249.curr= _temp1248; _temp1249.base= _temp1248; _temp1249.last_plus_one=
_temp1248 + 2; _temp1249;})); goto _LL1157; _LL1169: Cyc_Absyndump_dump(
_temp1200); goto _LL1157; _LL1171: Cyc_Absyndump_dumpqvar( _temp1202->name);
goto _LL1157; _LL1173:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1204,( struct
_tagged_string)({ char* _temp1250=( char*)"$("; struct _tagged_string _temp1251;
_temp1251.curr= _temp1250; _temp1251.base= _temp1250; _temp1251.last_plus_one=
_temp1250 + 3; _temp1251;}),( struct _tagged_string)({ char* _temp1252=( char*)")";
struct _tagged_string _temp1253; _temp1253.curr= _temp1252; _temp1253.base=
_temp1252; _temp1253.last_plus_one= _temp1252 + 2; _temp1253;}),( struct
_tagged_string)({ char* _temp1254=( char*)","; struct _tagged_string _temp1255;
_temp1255.curr= _temp1254; _temp1255.base= _temp1254; _temp1255.last_plus_one=
_temp1254 + 2; _temp1255;})); goto _LL1157; _LL1175: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1256=( char*)"&"; struct _tagged_string _temp1257;
_temp1257.curr= _temp1256; _temp1257.base= _temp1256; _temp1257.last_plus_one=
_temp1256 + 2; _temp1257;})); Cyc_Absyndump_dumppat( _temp1206); goto _LL1157;
_LL1177: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1258=( char*)"*";
struct _tagged_string _temp1259; _temp1259.curr= _temp1258; _temp1259.base=
_temp1258; _temp1259.last_plus_one= _temp1258 + 2; _temp1259;})); Cyc_Absyndump_dumpqvar(
_temp1208->name); goto _LL1157; _LL1179: Cyc_Absyndump_dumpqvar( _temp1210);
goto _LL1157; _LL1181: Cyc_Absyndump_dumpqvar( _temp1216); Cyc_Absyndump_dumptvars(
_temp1214);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1212,( struct
_tagged_string)({ char* _temp1260=( char*)"("; struct _tagged_string _temp1261;
_temp1261.curr= _temp1260; _temp1261.base= _temp1260; _temp1261.last_plus_one=
_temp1260 + 2; _temp1261;}),( struct _tagged_string)({ char* _temp1262=( char*)")";
struct _tagged_string _temp1263; _temp1263.curr= _temp1262; _temp1263.base=
_temp1262; _temp1263.last_plus_one= _temp1262 + 2; _temp1263;}),( struct
_tagged_string)({ char* _temp1264=( char*)","; struct _tagged_string _temp1265;
_temp1265.curr= _temp1264; _temp1265.base= _temp1264; _temp1265.last_plus_one=
_temp1264 + 2; _temp1265;})); goto _LL1157; _LL1183: Cyc_Absyndump_dumpqvar(
_temp1222); Cyc_Absyndump_dumptvars( _temp1220);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1218,( struct _tagged_string)({ char* _temp1266=( char*)"{"; struct
_tagged_string _temp1267; _temp1267.curr= _temp1266; _temp1267.base= _temp1266;
_temp1267.last_plus_one= _temp1266 + 2; _temp1267;}),( struct _tagged_string)({
char* _temp1268=( char*)"}"; struct _tagged_string _temp1269; _temp1269.curr=
_temp1268; _temp1269.base= _temp1268; _temp1269.last_plus_one= _temp1268 + 2;
_temp1269;}),( struct _tagged_string)({ char* _temp1270=( char*)","; struct
_tagged_string _temp1271; _temp1271.curr= _temp1270; _temp1271.base= _temp1270;
_temp1271.last_plus_one= _temp1270 + 2; _temp1271;})); goto _LL1157; _LL1185:
if( _temp1230->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct
Cyc_Core_Opt* _temp1272= _temp1230->name; if( _temp1272 == 0){ _throw(
Null_Exception);} _temp1272->v;}));} Cyc_Absyndump_dumptvars( _temp1226);(( void(*)(
void(* f)( struct _tuple8*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp, _temp1224,( struct _tagged_string)({ char* _temp1273=(
char*)"{"; struct _tagged_string _temp1274; _temp1274.curr= _temp1273; _temp1274.base=
_temp1273; _temp1274.last_plus_one= _temp1273 + 2; _temp1274;}),( struct
_tagged_string)({ char* _temp1275=( char*)"}"; struct _tagged_string _temp1276;
_temp1276.curr= _temp1275; _temp1276.base= _temp1275; _temp1276.last_plus_one=
_temp1275 + 2; _temp1276;}),( struct _tagged_string)({ char* _temp1277=( char*)",";
struct _tagged_string _temp1278; _temp1278.curr= _temp1277; _temp1278.base=
_temp1277; _temp1278.last_plus_one= _temp1277 + 2; _temp1278;})); goto _LL1157;
_LL1187: Cyc_Absyndump_dumpqvar( _temp1236->name); Cyc_Absyndump_dumptvars(
_temp1234); if( _temp1232 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1232,( struct _tagged_string)({ char* _temp1279=( char*)"("; struct
_tagged_string _temp1280; _temp1280.curr= _temp1279; _temp1280.base= _temp1279;
_temp1280.last_plus_one= _temp1279 + 2; _temp1280;}),( struct _tagged_string)({
char* _temp1281=( char*)")"; struct _tagged_string _temp1282; _temp1282.curr=
_temp1281; _temp1282.base= _temp1281; _temp1282.last_plus_one= _temp1281 + 2;
_temp1282;}),( struct _tagged_string)({ char* _temp1283=( char*)","; struct
_tagged_string _temp1284; _temp1284.curr= _temp1283; _temp1284.base= _temp1283;
_temp1284.last_plus_one= _temp1283 + 2; _temp1284;}));} goto _LL1157; _LL1189:
Cyc_Absyndump_dumpqvar( _temp1240->name); goto _LL1157; _LL1157:;} void Cyc_Absyndump_dumptunionfield(
struct Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs
!= 0){ Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,( struct _tagged_string)({ char* _temp1285=(
char*)","; struct _tagged_string _temp1286; _temp1286.curr= _temp1285; _temp1286.base=
_temp1285; _temp1286.last_plus_one= _temp1285 + 2; _temp1286;}));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1287=( char*)" = ";
struct _tagged_string _temp1288; _temp1288.curr= _temp1287; _temp1288.base=
_temp1287; _temp1288.last_plus_one= _temp1287 + 4; _temp1288;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1289= ef->tag; if( _temp1289
== 0){ _throw( Null_Exception);} _temp1289;}));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield, fields,( struct _tagged_string)({ char* _temp1290=(
char*)","; struct _tagged_string _temp1291; _temp1291.curr= _temp1290; _temp1291.base=
_temp1290; _temp1291.last_plus_one= _temp1290 + 2; _temp1291;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=({ struct Cyc_List_List*
_temp1292= fields; if( _temp1292 == 0){ _throw( Null_Exception);} _temp1292->tl;})){
struct Cyc_Absyn_Structfield _temp1296; struct Cyc_List_List* _temp1297; struct
Cyc_Core_Opt* _temp1299; void* _temp1301; struct Cyc_Absyn_Tqual _temp1303;
struct _tagged_string* _temp1305; struct Cyc_Absyn_Structfield* _temp1294=(
struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp1293= fields; if(
_temp1293 == 0){ _throw( Null_Exception);} _temp1293->hd;}); _temp1296=*
_temp1294; _LL1306: _temp1305=( struct _tagged_string*) _temp1296.name; goto
_LL1304; _LL1304: _temp1303=( struct Cyc_Absyn_Tqual) _temp1296.tq; goto _LL1302;
_LL1302: _temp1301=( void*) _temp1296.type; goto _LL1300; _LL1300: _temp1299=(
struct Cyc_Core_Opt*) _temp1296.width; goto _LL1298; _LL1298: _temp1297=( struct
Cyc_List_List*) _temp1296.attributes; goto _LL1295; _LL1295:(( void(*)( struct
Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1303, _temp1301, Cyc_Absyndump_dump_str,
_temp1305); Cyc_Absyndump_dumpatts( _temp1297); if( _temp1299 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp1307= _temp1299; if( _temp1307 == 0){ _throw( Null_Exception);} _temp1307->v;}));}
Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl*
td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars( td->tvs);}
static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpatts(
fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1308=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1332; struct Cyc_Absyn_Structdecl* _temp1334; struct Cyc_Absyn_Uniondecl*
_temp1336; struct Cyc_Absyn_Vardecl* _temp1338; struct Cyc_Absyn_Vardecl
_temp1340; struct Cyc_List_List* _temp1341; struct Cyc_Core_Opt* _temp1343;
struct Cyc_Absyn_Exp* _temp1345; void* _temp1347; struct Cyc_Absyn_Tqual
_temp1349; struct _tuple0* _temp1351; void* _temp1353; struct Cyc_Absyn_Tuniondecl*
_temp1355; struct Cyc_Absyn_Tuniondecl _temp1357; int _temp1358; struct Cyc_Core_Opt*
_temp1360; struct Cyc_List_List* _temp1362; struct _tuple0* _temp1364; void*
_temp1366; struct Cyc_Absyn_Enumdecl* _temp1368; struct Cyc_Absyn_Enumdecl
_temp1370; struct Cyc_Core_Opt* _temp1371; struct _tuple0* _temp1373; void*
_temp1375; int _temp1377; struct Cyc_Absyn_Exp* _temp1379; struct Cyc_Core_Opt*
_temp1381; struct Cyc_Core_Opt* _temp1383; struct Cyc_Absyn_Pat* _temp1385;
struct Cyc_Absyn_Typedefdecl* _temp1387; struct Cyc_List_List* _temp1389; struct
_tagged_string* _temp1391; struct Cyc_List_List* _temp1393; struct _tuple0*
_temp1395; struct Cyc_List_List* _temp1397; _LL1310: if(*(( int*) _temp1308) ==
Cyc_Absyn_Fn_d){ _LL1333: _temp1332=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1308)->f1; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(*(( int*)
_temp1308) == Cyc_Absyn_Struct_d){ _LL1335: _temp1334=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1308)->f1; goto _LL1313;} else{ goto
_LL1314;} _LL1314: if(*(( int*) _temp1308) == Cyc_Absyn_Union_d){ _LL1337:
_temp1336=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp1308)->f1; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(*(( int*)
_temp1308) == Cyc_Absyn_Var_d){ _LL1339: _temp1338=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1308)->f1; _temp1340=* _temp1338; _LL1354:
_temp1353=( void*) _temp1340.sc; goto _LL1352; _LL1352: _temp1351=( struct
_tuple0*) _temp1340.name; goto _LL1350; _LL1350: _temp1349=( struct Cyc_Absyn_Tqual)
_temp1340.tq; goto _LL1348; _LL1348: _temp1347=( void*) _temp1340.type; goto
_LL1346; _LL1346: _temp1345=( struct Cyc_Absyn_Exp*) _temp1340.initializer; goto
_LL1344; _LL1344: _temp1343=( struct Cyc_Core_Opt*) _temp1340.rgn; goto _LL1342;
_LL1342: _temp1341=( struct Cyc_List_List*) _temp1340.attributes; goto _LL1317;}
else{ goto _LL1318;} _LL1318: if(*(( int*) _temp1308) == Cyc_Absyn_Tunion_d){
_LL1356: _temp1355=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1308)->f1; _temp1357=* _temp1355; _LL1367: _temp1366=( void*) _temp1357.sc;
goto _LL1365; _LL1365: _temp1364=( struct _tuple0*) _temp1357.name; goto _LL1363;
_LL1363: _temp1362=( struct Cyc_List_List*) _temp1357.tvs; goto _LL1361; _LL1361:
_temp1360=( struct Cyc_Core_Opt*) _temp1357.fields; goto _LL1359; _LL1359:
_temp1358=( int) _temp1357.is_xtunion; goto _LL1319;} else{ goto _LL1320;}
_LL1320: if(*(( int*) _temp1308) == Cyc_Absyn_Enum_d){ _LL1369: _temp1368=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp1308)->f1;
_temp1370=* _temp1368; _LL1376: _temp1375=( void*) _temp1370.sc; goto _LL1374;
_LL1374: _temp1373=( struct _tuple0*) _temp1370.name; goto _LL1372; _LL1372:
_temp1371=( struct Cyc_Core_Opt*) _temp1370.fields; goto _LL1321;} else{ goto
_LL1322;} _LL1322: if(*(( int*) _temp1308) == Cyc_Absyn_Let_d){ _LL1386:
_temp1385=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1308)->f1;
goto _LL1384; _LL1384: _temp1383=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1308)->f2; goto _LL1382; _LL1382: _temp1381=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1308)->f3; goto _LL1380; _LL1380: _temp1379=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1308)->f4; goto
_LL1378; _LL1378: _temp1377=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1308)->f5;
goto _LL1323;} else{ goto _LL1324;} _LL1324: if(*(( int*) _temp1308) == Cyc_Absyn_Typedef_d){
_LL1388: _temp1387=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1308)->f1; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(*(( int*)
_temp1308) == Cyc_Absyn_Namespace_d){ _LL1392: _temp1391=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1308)->f1; goto _LL1390; _LL1390:
_temp1389=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1308)->f2; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(*(( int*)
_temp1308) == Cyc_Absyn_Using_d){ _LL1396: _temp1395=( struct _tuple0*)(( struct
Cyc_Absyn_Using_d_struct*) _temp1308)->f1; goto _LL1394; _LL1394: _temp1393=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1308)->f2; goto
_LL1329;} else{ goto _LL1330;} _LL1330: if(*(( int*) _temp1308) == Cyc_Absyn_ExternC_d){
_LL1398: _temp1397=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1308)->f1; goto _LL1331;} else{ goto _LL1309;} _LL1311: if( _temp1332->is_inline){
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1399=( char*)"inline";
struct _tagged_string _temp1400; _temp1400.curr= _temp1399; _temp1400.base=
_temp1399; _temp1400.last_plus_one= _temp1399 + 7; _temp1400;}));} Cyc_Absyndump_dumpscope((
void*) _temp1332->sc);{ void* t=( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1402=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1402[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1403; _temp1403.tag= Cyc_Absyn_FnType;
_temp1403.f1=({ struct Cyc_Absyn_FnInfo _temp1404; _temp1404.tvars= _temp1332->tvs;
_temp1404.effect= _temp1332->effect; _temp1404.ret_typ=( void*)(( void*)
_temp1332->ret_type); _temp1404.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1332->args); _temp1404.varargs= _temp1332->varargs; _temp1404.attributes= 0;
_temp1404;}); _temp1403;}); _temp1402;});(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Absyn_Fndecl*), struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1401; _temp1401.q_const= 0; _temp1401.q_volatile= 0;
_temp1401.q_restrict= 0; _temp1401;}), t, Cyc_Absyndump_dump_atts_qvar,
_temp1332); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1332->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1309;} _LL1313:
Cyc_Absyndump_dumpscope(( void*) _temp1334->sc); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1405=( char*)"struct"; struct _tagged_string
_temp1406; _temp1406.curr= _temp1405; _temp1406.base= _temp1405; _temp1406.last_plus_one=
_temp1405 + 7; _temp1406;})); if( _temp1334->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp1407= _temp1334->name; if(
_temp1407 == 0){ _throw( Null_Exception);} _temp1407->v;}));} Cyc_Absyndump_dumptvars(
_temp1334->tvs); if( _temp1334->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1408= _temp1334->fields; if( _temp1408 == 0){ _throw(
Null_Exception);} _temp1408->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1409=( char*)"}"; struct _tagged_string _temp1410; _temp1410.curr=
_temp1409; _temp1410.base= _temp1409; _temp1410.last_plus_one= _temp1409 + 2;
_temp1410;})); Cyc_Absyndump_dumpatts( _temp1334->attributes); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1411=( char*)";"; struct _tagged_string
_temp1412; _temp1412.curr= _temp1411; _temp1412.base= _temp1411; _temp1412.last_plus_one=
_temp1411 + 2; _temp1412;}));} goto _LL1309; _LL1315: Cyc_Absyndump_dumpscope((
void*) _temp1336->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1413=( char*)"union"; struct _tagged_string _temp1414; _temp1414.curr=
_temp1413; _temp1414.base= _temp1413; _temp1414.last_plus_one= _temp1413 + 6;
_temp1414;})); if( _temp1336->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)({ struct Cyc_Core_Opt* _temp1415= _temp1336->name; if( _temp1415 == 0){
_throw( Null_Exception);} _temp1415->v;}));} Cyc_Absyndump_dumptvars( _temp1336->tvs);
if( _temp1336->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1416= _temp1336->fields; if( _temp1416 == 0){ _throw( Null_Exception);}
_temp1416->v;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1417=(
char*)"}"; struct _tagged_string _temp1418; _temp1418.curr= _temp1417; _temp1418.base=
_temp1417; _temp1418.last_plus_one= _temp1417 + 2; _temp1418;})); Cyc_Absyndump_dumpatts(
_temp1336->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1419=( char*)";"; struct _tagged_string _temp1420; _temp1420.curr=
_temp1419; _temp1420.base= _temp1419; _temp1420.last_plus_one= _temp1419 + 2;
_temp1420;}));} goto _LL1309; _LL1317: Cyc_Absyndump_dumpscope( _temp1353);((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*), struct
_tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1349, _temp1347, Cyc_Absyndump_dumpqvar,
_temp1351); Cyc_Absyndump_dumpatts( _temp1341); if( _temp1345 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1421= _temp1345; if( _temp1421 == 0){ _throw( Null_Exception);} _temp1421;}));}
Cyc_Absyndump_dump_semi(); goto _LL1309; _LL1319: Cyc_Absyndump_dumpscope(
_temp1366); if( _temp1358){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1422=( char*)"xtunion "; struct _tagged_string _temp1423; _temp1423.curr=
_temp1422; _temp1423.base= _temp1422; _temp1423.last_plus_one= _temp1422 + 9;
_temp1423;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1424=( char*)"tunion "; struct _tagged_string _temp1425; _temp1425.curr=
_temp1424; _temp1425.base= _temp1424; _temp1425.last_plus_one= _temp1424 + 8;
_temp1425;}));} Cyc_Absyndump_dumpqvar( _temp1364); Cyc_Absyndump_dumptvars(
_temp1362); if( _temp1360 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1426= _temp1360; if( _temp1426 == 0){ _throw( Null_Exception);} _temp1426->v;}));
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1427=( char*)"};";
struct _tagged_string _temp1428; _temp1428.curr= _temp1427; _temp1428.base=
_temp1427; _temp1428.last_plus_one= _temp1427 + 3; _temp1428;}));} goto _LL1309;
_LL1321: Cyc_Absyndump_dumpscope( _temp1375); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1429=( char*)"enum "; struct _tagged_string
_temp1430; _temp1430.curr= _temp1429; _temp1430.base= _temp1429; _temp1430.last_plus_one=
_temp1429 + 6; _temp1430;})); Cyc_Absyndump_dumpqvar( _temp1373); if( _temp1371
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1431= _temp1371; if(
_temp1431 == 0){ _throw( Null_Exception);} _temp1431->v;})); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1432=( char*)"};"; struct _tagged_string
_temp1433; _temp1433.curr= _temp1432; _temp1433.base= _temp1432; _temp1433.last_plus_one=
_temp1432 + 3; _temp1433;}));} return; _LL1323: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1434=( char*)"let"; struct _tagged_string _temp1435;
_temp1435.curr= _temp1434; _temp1435.base= _temp1434; _temp1435.last_plus_one=
_temp1434 + 4; _temp1435;})); Cyc_Absyndump_dumppat( _temp1385); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1379); Cyc_Absyndump_dump_semi(); goto
_LL1309; _LL1325: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1436=(
char*)"typedef"; struct _tagged_string _temp1437; _temp1437.curr= _temp1436;
_temp1437.base= _temp1436; _temp1437.last_plus_one= _temp1436 + 8; _temp1437;}));((
void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1438; _temp1438.q_const= 0; _temp1438.q_volatile= 0; _temp1438.q_restrict=
0; _temp1438;}),( void*) _temp1387->defn, Cyc_Absyndump_dumptypedefname,
_temp1387); Cyc_Absyndump_dump_semi(); goto _LL1309; _LL1327: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1439=( char*)"namespace"; struct
_tagged_string _temp1440; _temp1440.curr= _temp1439; _temp1440.base= _temp1439;
_temp1440.last_plus_one= _temp1439 + 10; _temp1440;})); Cyc_Absyndump_dump_str(
_temp1391); Cyc_Absyndump_dump_char(( int)'{'); for( 0; _temp1389 != 0;
_temp1389=({ struct Cyc_List_List* _temp1441= _temp1389; if( _temp1441 == 0){
_throw( Null_Exception);} _temp1441->tl;})){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp1442= _temp1389; if( _temp1442 == 0){ _throw(
Null_Exception);} _temp1442->hd;}));} Cyc_Absyndump_dump_char(( int)'}'); goto
_LL1309; _LL1329: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1443=(
char*)"using"; struct _tagged_string _temp1444; _temp1444.curr= _temp1443;
_temp1444.base= _temp1443; _temp1444.last_plus_one= _temp1443 + 6; _temp1444;}));
Cyc_Absyndump_dumpqvar( _temp1395); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1393 != 0; _temp1393=({ struct Cyc_List_List* _temp1445= _temp1393; if(
_temp1445 == 0){ _throw( Null_Exception);} _temp1445->tl;})){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp1446= _temp1393; if(
_temp1446 == 0){ _throw( Null_Exception);} _temp1446->hd;}));} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1309; _LL1331: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1447=( char*)"extern \"C\" {"; struct _tagged_string _temp1448;
_temp1448.curr= _temp1447; _temp1448.base= _temp1447; _temp1448.last_plus_one=
_temp1447 + 13; _temp1448;})); for( 0; _temp1397 != 0; _temp1397=({ struct Cyc_List_List*
_temp1449= _temp1397; if( _temp1449 == 0){ _throw( Null_Exception);} _temp1449->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1450= _temp1397; if( _temp1450 == 0){ _throw( Null_Exception);} _temp1450->hd;}));}
Cyc_Absyndump_dump_char(( int)'}'); goto _LL1309; _LL1309:;} static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp* e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if(
i != 1){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1452=( void*)({
struct Cyc_List_List* _temp1451= tms; if( _temp1451 == 0){ _throw(
Null_Exception);} _temp1451->hd;}); struct Cyc_Absyn_Tqual _temp1470; void*
_temp1472; void* _temp1474; struct Cyc_Absyn_Exp* _temp1476; struct Cyc_Absyn_Tqual
_temp1478; void* _temp1480; void* _temp1482; struct Cyc_Absyn_Exp* _temp1484;
struct Cyc_Absyn_Tqual _temp1486; void* _temp1488; void* _temp1490; struct Cyc_Absyn_Tqual
_temp1492; void* _temp1494; struct Cyc_Absyn_Tvar* _temp1496; void* _temp1498;
struct Cyc_Absyn_Exp* _temp1500; struct Cyc_Absyn_Tqual _temp1502; void*
_temp1504; struct Cyc_Absyn_Tvar* _temp1506; void* _temp1508; struct Cyc_Absyn_Exp*
_temp1510; struct Cyc_Absyn_Tqual _temp1512; void* _temp1514; struct Cyc_Absyn_Tvar*
_temp1516; void* _temp1518; struct Cyc_Absyn_Tqual _temp1520; void* _temp1522;
void* _temp1524; _LL1454: if(( unsigned int) _temp1452 > 1u?*(( int*) _temp1452)
== Cyc_Absyn_Pointer_mod: 0){ _LL1475: _temp1474=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1452)->f1; if(( unsigned int) _temp1474 > 1u?*(( int*) _temp1474) == Cyc_Absyn_Nullable_ps:
0){ _LL1477: _temp1476=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1474)->f1; goto _LL1473;} else{ goto _LL1456;} _LL1473: _temp1472=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f2; if( _temp1472 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL1471;} else{ goto _LL1456;} _LL1471: _temp1470=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f3;
goto _LL1455;} else{ goto _LL1456;} _LL1456: if(( unsigned int) _temp1452 > 1u?*((
int*) _temp1452) == Cyc_Absyn_Pointer_mod: 0){ _LL1483: _temp1482=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f1; if(( unsigned int)
_temp1482 > 1u?*(( int*) _temp1482) == Cyc_Absyn_NonNullable_ps: 0){ _LL1485:
_temp1484=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1482)->f1; goto _LL1481;} else{ goto _LL1458;} _LL1481: _temp1480=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f2; if( _temp1480 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL1479;} else{ goto _LL1458;} _LL1479: _temp1478=(
struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f3;
goto _LL1457;} else{ goto _LL1458;} _LL1458: if(( unsigned int) _temp1452 > 1u?*((
int*) _temp1452) == Cyc_Absyn_Pointer_mod: 0){ _LL1491: _temp1490=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f1; if( _temp1490 ==( void*)
Cyc_Absyn_TaggedArray_ps){ goto _LL1489;} else{ goto _LL1460;} _LL1489:
_temp1488=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f2; if(
_temp1488 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1487;} else{ goto _LL1460;}
_LL1487: _temp1486=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1452)->f3; goto _LL1459;} else{ goto _LL1460;} _LL1460: if(( unsigned int)
_temp1452 > 1u?*(( int*) _temp1452) == Cyc_Absyn_Pointer_mod: 0){ _LL1499:
_temp1498=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f1; if((
unsigned int) _temp1498 > 1u?*(( int*) _temp1498) == Cyc_Absyn_Nullable_ps: 0){
_LL1501: _temp1500=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp1498)->f1; goto _LL1495;} else{ goto _LL1462;} _LL1495: _temp1494=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f2; if(( unsigned int)
_temp1494 > 4u?*(( int*) _temp1494) == Cyc_Absyn_VarType: 0){ _LL1497: _temp1496=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp1494)->f1; goto
_LL1493;} else{ goto _LL1462;} _LL1493: _temp1492=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f3; goto _LL1461;} else{ goto
_LL1462;} _LL1462: if(( unsigned int) _temp1452 > 1u?*(( int*) _temp1452) == Cyc_Absyn_Pointer_mod:
0){ _LL1509: _temp1508=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1452)->f1; if(( unsigned int) _temp1508 > 1u?*(( int*) _temp1508) == Cyc_Absyn_NonNullable_ps:
0){ _LL1511: _temp1510=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp1508)->f1; goto _LL1505;} else{ goto _LL1464;} _LL1505: _temp1504=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f2; if(( unsigned int)
_temp1504 > 4u?*(( int*) _temp1504) == Cyc_Absyn_VarType: 0){ _LL1507: _temp1506=(
struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp1504)->f1; goto
_LL1503;} else{ goto _LL1464;} _LL1503: _temp1502=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f3; goto _LL1463;} else{ goto
_LL1464;} _LL1464: if(( unsigned int) _temp1452 > 1u?*(( int*) _temp1452) == Cyc_Absyn_Pointer_mod:
0){ _LL1519: _temp1518=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1452)->f1; if( _temp1518 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1515;}
else{ goto _LL1466;} _LL1515: _temp1514=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1452)->f2; if(( unsigned int) _temp1514 > 4u?*(( int*) _temp1514) == Cyc_Absyn_VarType:
0){ _LL1517: _temp1516=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp1514)->f1; goto _LL1513;} else{ goto _LL1466;} _LL1513: _temp1512=( struct
Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f3; goto
_LL1465;} else{ goto _LL1466;} _LL1466: if(( unsigned int) _temp1452 > 1u?*((
int*) _temp1452) == Cyc_Absyn_Pointer_mod: 0){ _LL1525: _temp1524=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f1; goto _LL1523; _LL1523:
_temp1522=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1452)->f2; goto
_LL1521; _LL1521: _temp1520=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1452)->f3; goto _LL1467;} else{ goto _LL1468;} _LL1468: goto _LL1469;
_LL1455: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1476);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1526= tms; if( _temp1526 ==
0){ _throw( Null_Exception);} _temp1526->tl;}), f, a); return; _LL1457: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1484);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1527= tms; if( _temp1527 == 0){ _throw( Null_Exception);} _temp1527->tl;}),
f, a); return; _LL1459: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct
Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({
struct Cyc_List_List* _temp1528= tms; if( _temp1528 == 0){ _throw(
Null_Exception);} _temp1528->tl;}), f, a); return; _LL1461: Cyc_Absyndump_dump_char((
int)'*'); Cyc_Absyndump_dump_upperbound( _temp1500); Cyc_Absyndump_dump_str(
_temp1496->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1529= tms; if(
_temp1529 == 0){ _throw( Null_Exception);} _temp1529->tl;}), f, a); return;
_LL1463: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1510); Cyc_Absyndump_dump_str( _temp1506->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List*
_temp1530= tms; if( _temp1530 == 0){ _throw( Null_Exception);} _temp1530->tl;}),
f, a); return; _LL1465: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1516->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1531= tms; if(
_temp1531 == 0){ _throw( Null_Exception);} _temp1531->tl;}), f, a); return;
_LL1467:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1532=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1532[ 0]=({ struct Cyc_Core_Impossible_struct _temp1533; _temp1533.tag= Cyc_Core_Impossible;
_temp1533.f1=( struct _tagged_string)({ char* _temp1534=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1535; _temp1535.curr= _temp1534; _temp1535.base=
_temp1534; _temp1535.last_plus_one= _temp1534 + 25; _temp1535;}); _temp1533;});
_temp1532;})); _LL1469: { int next_is_pointer= 0; if(({ struct Cyc_List_List*
_temp1536= tms; if( _temp1536 == 0){ _throw( Null_Exception);} _temp1536->tl;})
!= 0){ void* _temp1539=( void*)({ struct Cyc_List_List* _temp1538=({ struct Cyc_List_List*
_temp1537= tms; if( _temp1537 == 0){ _throw( Null_Exception);} _temp1537->tl;});
if( _temp1538 == 0){ _throw( Null_Exception);} _temp1538->hd;}); struct Cyc_Absyn_Tqual
_temp1545; void* _temp1547; void* _temp1549; _LL1541: if(( unsigned int)
_temp1539 > 1u?*(( int*) _temp1539) == Cyc_Absyn_Pointer_mod: 0){ _LL1550:
_temp1549=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1539)->f1; goto
_LL1548; _LL1548: _temp1547=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1539)->f2; goto _LL1546; _LL1546: _temp1545=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1539)->f3; goto _LL1542;} else{ goto
_LL1543;} _LL1543: goto _LL1544; _LL1542: next_is_pointer= 1; goto _LL1540;
_LL1544: goto _LL1540; _LL1540:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)(({ struct Cyc_List_List* _temp1551= tms; if( _temp1551 ==
0){ _throw( Null_Exception);} _temp1551->tl;}), f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1553=( void*)({ struct Cyc_List_List* _temp1552= tms; if(
_temp1552 == 0){ _throw( Null_Exception);} _temp1552->hd;}); struct Cyc_Absyn_Exp*
_temp1569; void* _temp1571; struct Cyc_Core_Opt* _temp1573; int _temp1575;
struct Cyc_List_List* _temp1577; void* _temp1579; struct Cyc_Position_Segment*
_temp1581; struct Cyc_List_List* _temp1583; int _temp1585; struct Cyc_Position_Segment*
_temp1587; struct Cyc_List_List* _temp1589; struct Cyc_List_List* _temp1591;
struct Cyc_Position_Segment* _temp1593; struct Cyc_Absyn_Tqual _temp1595; void*
_temp1597; void* _temp1599; _LL1555: if( _temp1553 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL1556;} else{ goto _LL1557;} _LL1557: if(( unsigned int) _temp1553 > 1u?*((
int*) _temp1553) == Cyc_Absyn_ConstArray_mod: 0){ _LL1570: _temp1569=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1553)->f1; goto
_LL1558;} else{ goto _LL1559;} _LL1559: if(( unsigned int) _temp1553 > 1u?*((
int*) _temp1553) == Cyc_Absyn_Function_mod: 0){ _LL1572: _temp1571=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp1553)->f1; if(*(( int*) _temp1571)
== Cyc_Absyn_WithTypes){ _LL1578: _temp1577=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp1571)->f1; goto _LL1576; _LL1576: _temp1575=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp1571)->f2; goto _LL1574; _LL1574:
_temp1573=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1571)->f3; goto _LL1560;} else{ goto _LL1561;}} else{ goto _LL1561;}
_LL1561: if(( unsigned int) _temp1553 > 1u?*(( int*) _temp1553) == Cyc_Absyn_Function_mod:
0){ _LL1580: _temp1579=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1553)->f1; if(*(( int*) _temp1579) == Cyc_Absyn_NoTypes){ _LL1584:
_temp1583=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1579)->f1;
goto _LL1582; _LL1582: _temp1581=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1579)->f2; goto _LL1562;} else{ goto _LL1563;}} else{ goto _LL1563;}
_LL1563: if(( unsigned int) _temp1553 > 1u?*(( int*) _temp1553) == Cyc_Absyn_TypeParams_mod:
0){ _LL1590: _temp1589=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1553)->f1; goto _LL1588; _LL1588: _temp1587=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1553)->f2; goto _LL1586; _LL1586:
_temp1585=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1553)->f3; goto
_LL1564;} else{ goto _LL1565;} _LL1565: if(( unsigned int) _temp1553 > 1u?*((
int*) _temp1553) == Cyc_Absyn_Attributes_mod: 0){ _LL1594: _temp1593=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1553)->f1;
goto _LL1592; _LL1592: _temp1591=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1553)->f2; goto _LL1566;} else{ goto _LL1567;} _LL1567: if(( unsigned int)
_temp1553 > 1u?*(( int*) _temp1553) == Cyc_Absyn_Pointer_mod: 0){ _LL1600:
_temp1599=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1553)->f1; goto
_LL1598; _LL1598: _temp1597=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1553)->f2; goto _LL1596; _LL1596: _temp1595=( struct Cyc_Absyn_Tqual)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1553)->f3; goto _LL1568;} else{ goto
_LL1554;} _LL1556: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1601=(
char*)"[]"; struct _tagged_string _temp1602; _temp1602.curr= _temp1601;
_temp1602.base= _temp1601; _temp1602.last_plus_one= _temp1601 + 3; _temp1602;}));
goto _LL1554; _LL1558: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1569); Cyc_Absyndump_dump_char(( int)']'); goto _LL1554; _LL1560: Cyc_Absyndump_dumpfunargs(
_temp1577, _temp1575, _temp1573); goto _LL1554; _LL1562:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1583,( struct _tagged_string)({ char* _temp1603=( char*)"("; struct
_tagged_string _temp1604; _temp1604.curr= _temp1603; _temp1604.base= _temp1603;
_temp1604.last_plus_one= _temp1603 + 2; _temp1604;}),( struct _tagged_string)({
char* _temp1605=( char*)")"; struct _tagged_string _temp1606; _temp1606.curr=
_temp1605; _temp1606.base= _temp1605; _temp1606.last_plus_one= _temp1605 + 2;
_temp1606;}),( struct _tagged_string)({ char* _temp1607=( char*)","; struct
_tagged_string _temp1608; _temp1608.curr= _temp1607; _temp1608.base= _temp1607;
_temp1608.last_plus_one= _temp1607 + 2; _temp1608;})); goto _LL1554; _LL1564:
if( _temp1585){ Cyc_Absyndump_dumpkindedtvars( _temp1589);} else{ Cyc_Absyndump_dumptvars(
_temp1589);} goto _LL1554; _LL1566: Cyc_Absyndump_dumpatts( _temp1591); goto
_LL1554; _LL1568:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1609=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1609[ 0]=({ struct Cyc_Core_Impossible_struct _temp1610; _temp1610.tag= Cyc_Core_Impossible;
_temp1610.f1=( struct _tagged_string)({ char* _temp1611=( char*)"dumptms";
struct _tagged_string _temp1612; _temp1612.curr= _temp1611; _temp1612.base=
_temp1611; _temp1612.last_plus_one= _temp1611 + 8; _temp1612;}); _temp1610;});
_temp1609;})); _LL1554:;} return;} _LL1453:;}} void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq, void* t, void(* f)( void*), void* a){ struct Cyc_List_List*
_temp1615; void* _temp1617; struct Cyc_Absyn_Tqual _temp1619; struct _tuple4
_temp1613= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual) tq, t); _LL1620:
_temp1619= _temp1613.f1; goto _LL1618; _LL1618: _temp1617= _temp1613.f2; goto
_LL1616; _LL1616: _temp1615= _temp1613.f3; goto _LL1614; _LL1614: Cyc_Absyndump_dumptq(
_temp1619); Cyc_Absyndump_dumpntyp( _temp1617);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1615), f, a);} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl=({ struct Cyc_List_List*
_temp1621= tdl; if( _temp1621 == 0){ _throw( Null_Exception);} _temp1621->tl;})){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1622= tdl; if( _temp1622 == 0){ _throw( Null_Exception);} _temp1622->hd;}));}}