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
struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_to_VC;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_string Cyc_Absynpp_char_escape( char); extern struct _tagged_string Cyc_Absynpp_string_escape(
struct _tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void*
p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); struct
_tuple3{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ;
extern struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); struct
_tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; struct Cyc_List_List* f3; } ;
extern struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual tq, void* t);
extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e);
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
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp0= s;(
unsigned int)( _temp0.last_plus_one - _temp0.curr);}); if( !(( int)*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), sz - 1))){ -- sz;} Cyc_Absyndump_pos += sz + 1; if( Cyc_Absyndump_pos
> 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc(( int)'\n',* Cyc_Absyndump_dump_file);}
else{ Cyc_Stdio_fputc(( int)' ',* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp1= s;(
unsigned int)( _temp1.last_plus_one - _temp1.curr);}); if( !(( int)*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), sz - 1))){ -- sz;} Cyc_Absyndump_pos += sz; Cyc_Stdio_file_string_write(*
Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char( int c){ ++
Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,* Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str(
struct _tagged_string* s){ Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char(( int)';');} void Cyc_Absyndump_dump_sep( void(* f)(
void*), struct Cyc_List_List* l, struct _tagged_string sep){ if( l == 0){
return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0; l=(( struct
Cyc_List_List*) _check_null( l))->tl){ f(( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*)(( struct
Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_dump_sep_c( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep){
if( l == 0){ return;} for( 0;(( struct Cyc_List_List*) _check_null( l))->tl != 0;
l=(( struct Cyc_List_List*) _check_null( l))->tl){ f( env,( void*)(( struct Cyc_List_List*)
_check_null( l))->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*)((
struct Cyc_List_List*) _check_null( l))->hd);} void Cyc_Absyndump_group( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_group_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end, struct
_tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace( end);}
void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ if(
l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* _temp2= 0; goto _LL3; _LL3:{ void* _temp4=(* v).f1; struct
Cyc_List_List* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List*
_temp20; struct Cyc_List_List* _temp22; _LL6: if( _temp4 ==( void*) Cyc_Absyn_Loc_n){
goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp4 > 1u?*(( int*)
_temp4) == Cyc_Absyn_Rel_n: 0){ _LL17: _temp16=(( struct Cyc_Absyn_Rel_n_struct*)
_temp4)->f1; if( _temp16 == 0){ goto _LL9;} else{ goto _LL10;}} else{ goto _LL10;}
_LL10: if(( unsigned int) _temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Abs_n: 0){
_LL19: _temp18=(( struct Cyc_Absyn_Abs_n_struct*) _temp4)->f1; if( _temp18 == 0){
goto _LL11;} else{ goto _LL12;}} else{ goto _LL12;} _LL12: if(( unsigned int)
_temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Rel_n: 0){ _LL21: _temp20=(( struct
Cyc_Absyn_Rel_n_struct*) _temp4)->f1; goto _LL13;} else{ goto _LL14;} _LL14: if((
unsigned int) _temp4 > 1u?*(( int*) _temp4) == Cyc_Absyn_Abs_n: 0){ _LL23:
_temp22=(( struct Cyc_Absyn_Abs_n_struct*) _temp4)->f1; goto _LL15;} else{ goto
_LL5;} _LL7: goto _LL9; _LL9: goto _LL11; _LL11: Cyc_Absyndump_dump_str((* v).f2);
return; _LL13: _temp22= _temp20; goto _LL15; _LL15: _temp2= _temp22; goto _LL5;
_LL5:;} Cyc_Absyndump_dump_str(( struct _tagged_string*)(( struct Cyc_List_List*)
_check_null( _temp2))->hd); for( _temp2=(( struct Cyc_List_List*) _check_null(
_temp2))->tl; _temp2 != 0; _temp2=(( struct Cyc_List_List*) _check_null( _temp2))->tl){
if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char(( int)'_');} else{ Cyc_Absyndump_dump_nospace(({
char* _temp24="::"; struct _tagged_string _temp25; _temp25.curr= _temp24;
_temp25.base= _temp24; _temp25.last_plus_one= _temp24 + 3; _temp25;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*)(( struct Cyc_List_List*) _check_null( _temp2))->hd));}
if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace(({ char* _temp26="_";
struct _tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26;
_temp27.last_plus_one= _temp26 + 2; _temp27;}));} else{ Cyc_Absyndump_dump_nospace(({
char* _temp28="::"; struct _tagged_string _temp29; _temp29.curr= _temp28;
_temp29.base= _temp28; _temp29.last_plus_one= _temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*(*
v).f2);} void Cyc_Absyndump_dumptq( struct Cyc_Absyn_Tqual tq){ if( tq.q_restrict){
Cyc_Absyndump_dump(({ char* _temp30="restrict"; struct _tagged_string _temp31;
_temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one= _temp30 + 9;
_temp31;}));} if( tq.q_volatile){ Cyc_Absyndump_dump(({ char* _temp32="volatile";
struct _tagged_string _temp33; _temp33.curr= _temp32; _temp33.base= _temp32;
_temp33.last_plus_one= _temp32 + 9; _temp33;}));} if( tq.q_const){ Cyc_Absyndump_dump(({
char* _temp34="const"; struct _tagged_string _temp35; _temp35.curr= _temp34;
_temp35.base= _temp34; _temp35.last_plus_one= _temp34 + 6; _temp35;}));}} void
Cyc_Absyndump_dumpscope( void* sc){ void* _temp36= sc; _LL38: if( _temp36 ==(
void*) Cyc_Absyn_Static){ goto _LL39;} else{ goto _LL40;} _LL40: if( _temp36 ==(
void*) Cyc_Absyn_Public){ goto _LL41;} else{ goto _LL42;} _LL42: if( _temp36 ==(
void*) Cyc_Absyn_Extern){ goto _LL43;} else{ goto _LL44;} _LL44: if( _temp36 ==(
void*) Cyc_Absyn_ExternC){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp36 ==(
void*) Cyc_Absyn_Abstract){ goto _LL47;} else{ goto _LL37;} _LL39: Cyc_Absyndump_dump(({
char* _temp48="static"; struct _tagged_string _temp49; _temp49.curr= _temp48;
_temp49.base= _temp48; _temp49.last_plus_one= _temp48 + 7; _temp49;})); return;
_LL41: return; _LL43: Cyc_Absyndump_dump(({ char* _temp50="extern"; struct
_tagged_string _temp51; _temp51.curr= _temp50; _temp51.base= _temp50; _temp51.last_plus_one=
_temp50 + 7; _temp51;})); return; _LL45: Cyc_Absyndump_dump(({ char* _temp52="extern \"C\"";
struct _tagged_string _temp53; _temp53.curr= _temp52; _temp53.base= _temp52;
_temp53.last_plus_one= _temp52 + 11; _temp53;})); return; _LL47: Cyc_Absyndump_dump(({
char* _temp54="abstract"; struct _tagged_string _temp55; _temp55.curr= _temp54;
_temp55.base= _temp54; _temp55.last_plus_one= _temp54 + 9; _temp55;})); return;
_LL37:;} void Cyc_Absyndump_dumpkind( void* k){ void* _temp56= k; _LL58: if(
_temp56 ==( void*) Cyc_Absyn_AnyKind){ goto _LL59;} else{ goto _LL60;} _LL60:
if( _temp56 ==( void*) Cyc_Absyn_MemKind){ goto _LL61;} else{ goto _LL62;} _LL62:
if( _temp56 ==( void*) Cyc_Absyn_BoxKind){ goto _LL63;} else{ goto _LL64;} _LL64:
if( _temp56 ==( void*) Cyc_Absyn_RgnKind){ goto _LL65;} else{ goto _LL66;} _LL66:
if( _temp56 ==( void*) Cyc_Absyn_EffKind){ goto _LL67;} else{ goto _LL57;} _LL59:
Cyc_Absyndump_dump(({ char* _temp68="A"; struct _tagged_string _temp69; _temp69.curr=
_temp68; _temp69.base= _temp68; _temp69.last_plus_one= _temp68 + 2; _temp69;}));
return; _LL61: Cyc_Absyndump_dump(({ char* _temp70="M"; struct _tagged_string
_temp71; _temp71.curr= _temp70; _temp71.base= _temp70; _temp71.last_plus_one=
_temp70 + 2; _temp71;})); return; _LL63: Cyc_Absyndump_dump(({ char* _temp72="B";
struct _tagged_string _temp73; _temp73.curr= _temp72; _temp73.base= _temp72;
_temp73.last_plus_one= _temp72 + 2; _temp73;})); return; _LL65: Cyc_Absyndump_dump(({
char* _temp74="R"; struct _tagged_string _temp75; _temp75.curr= _temp74; _temp75.base=
_temp74; _temp75.last_plus_one= _temp74 + 2; _temp75;})); return; _LL67: Cyc_Absyndump_dump(({
char* _temp76="E"; struct _tagged_string _temp77; _temp77.curr= _temp76; _temp77.base=
_temp76; _temp77.last_plus_one= _temp76 + 2; _temp77;})); return; _LL57:;} void
Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(* f)( void*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp, ts,({
char* _temp78="<"; struct _tagged_string _temp79; _temp79.curr= _temp78; _temp79.base=
_temp78; _temp79.last_plus_one= _temp78 + 2; _temp79;}),({ char* _temp80=">";
struct _tagged_string _temp81; _temp81.curr= _temp80; _temp81.base= _temp80;
_temp81.last_plus_one= _temp80 + 2; _temp81;}),({ char* _temp82=","; struct
_tagged_string _temp83; _temp83.curr= _temp82; _temp83.base= _temp82; _temp83.last_plus_one=
_temp82 + 2; _temp83;}));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp84=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp92; void* _temp94; _LL86: if(( unsigned int) _temp84 >
1u?*(( int*) _temp84) == Cyc_Absyn_Eq_constr: 0){ _LL93: _temp92=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp84)->f1; if( _temp92 ==( void*) Cyc_Absyn_BoxKind){
goto _LL87;} else{ goto _LL88;}} else{ goto _LL88;} _LL88: if(( unsigned int)
_temp84 > 1u?*(( int*) _temp84) == Cyc_Absyn_Eq_constr: 0){ _LL95: _temp94=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp84)->f1; goto _LL89;} else{
goto _LL90;} _LL90: goto _LL91; _LL87: goto _LL85; _LL89: Cyc_Absyndump_dump(({
char* _temp96="::"; struct _tagged_string _temp97; _temp97.curr= _temp96;
_temp97.base= _temp96; _temp97.last_plus_one= _temp96 + 3; _temp97;})); Cyc_Absyndump_dumpkind(
_temp94); goto _LL85; _LL91: Cyc_Absyndump_dump(({ char* _temp98="::?"; struct
_tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98; _temp99.last_plus_one=
_temp98 + 4; _temp99;})); goto _LL85; _LL85:;}} void Cyc_Absyndump_dumptvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptvar, tvs,({
char* _temp100="<"; struct _tagged_string _temp101; _temp101.curr= _temp100;
_temp101.base= _temp100; _temp101.last_plus_one= _temp100 + 2; _temp101;}),({
char* _temp102=">"; struct _tagged_string _temp103; _temp103.curr= _temp102;
_temp103.base= _temp102; _temp103.last_plus_one= _temp102 + 2; _temp103;}),({
char* _temp104=","; struct _tagged_string _temp105; _temp105.curr= _temp104;
_temp105.base= _temp104; _temp105.last_plus_one= _temp104 + 2; _temp105;}));}
void Cyc_Absyndump_dumpkindedtvars( struct Cyc_List_List* tvs){(( void(*)( void(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar, tvs,({ char* _temp106="<"; struct _tagged_string
_temp107; _temp107.curr= _temp106; _temp107.base= _temp106; _temp107.last_plus_one=
_temp106 + 2; _temp107;}),({ char* _temp108=">"; struct _tagged_string _temp109;
_temp109.curr= _temp108; _temp109.base= _temp108; _temp109.last_plus_one=
_temp108 + 2; _temp109;}),({ char* _temp110=","; struct _tagged_string _temp111;
_temp111.curr= _temp110; _temp111.base= _temp110; _temp111.last_plus_one=
_temp110 + 2; _temp111;}));} struct _tuple5{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((* pr).f1,(* pr).f2,( void(*)(
int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List*
ts){(( void(*)( void(* f)( struct _tuple5*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg, ts,({ char* _temp112="("; struct _tagged_string _temp113;
_temp113.curr= _temp112; _temp113.base= _temp112; _temp113.last_plus_one=
_temp112 + 2; _temp113;}),({ char* _temp114=")"; struct _tagged_string _temp115;
_temp115.curr= _temp114; _temp115.base= _temp114; _temp115.last_plus_one=
_temp114 + 2; _temp115;}),({ char* _temp116=","; struct _tagged_string _temp117;
_temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 2; _temp117;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List*
atts){ if( atts == 0){ return;} if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump(({
char* _temp118="__declspec("; struct _tagged_string _temp119; _temp119.curr=
_temp118; _temp119.base= _temp118; _temp119.last_plus_one= _temp118 + 12;
_temp119;})); for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null(
atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd));} Cyc_Absyndump_dump_char(( int)')');}
else{ Cyc_Absyndump_dump(({ char* _temp120=" __attribute__(("; struct
_tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 17; _temp121;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump(({ char* _temp122=","; struct
_tagged_string _temp123; _temp123.curr= _temp122; _temp123.base= _temp122;
_temp123.last_plus_one= _temp122 + 2; _temp123;}));}} Cyc_Absyndump_dump(({ char*
_temp124=")) "; struct _tagged_string _temp125; _temp125.curr= _temp124;
_temp125.base= _temp124; _temp125.last_plus_one= _temp124 + 4; _temp125;}));}}
int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List* tms){ if( tms == 0){
return 0;}{ void* _temp126=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Tqual _temp132; void* _temp134; void* _temp136; _LL128: if((
unsigned int) _temp126 > 1u?*(( int*) _temp126) == Cyc_Absyn_Pointer_mod: 0){
_LL137: _temp136=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp126)->f1;
goto _LL135; _LL135: _temp134=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp126)->f2; goto _LL133; _LL133: _temp132=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp126)->f3; goto _LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL129:
return 1; _LL131: return 0; _LL127:;}} static void Cyc_Absyndump_dumprgn( void*
t){ void* _temp138= Cyc_Tcutil_compress( t); _LL140: if( _temp138 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL141;} else{ goto _LL142;} _LL142: goto _LL143; _LL141: Cyc_Absyndump_dump(({
char* _temp144="`H"; struct _tagged_string _temp145; _temp145.curr= _temp144;
_temp145.base= _temp144; _temp145.last_plus_one= _temp144 + 3; _temp145;}));
goto _LL139; _LL143: Cyc_Absyndump_dumpntyp( t); goto _LL139; _LL139:;} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List* regions= 0;
struct Cyc_List_List* effects= 0;{ void* _temp146= Cyc_Tcutil_compress( t); void*
_temp154; struct Cyc_List_List* _temp156; _LL148: if(( unsigned int) _temp146 >
4u?*(( int*) _temp146) == Cyc_Absyn_AccessEff: 0){ _LL155: _temp154=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp146)->f1; goto _LL149;} else{ goto
_LL150;} _LL150: if(( unsigned int) _temp146 > 4u?*(( int*) _temp146) == Cyc_Absyn_JoinEff:
0){ _LL157: _temp156=(( struct Cyc_Absyn_JoinEff_struct*) _temp146)->f1; goto
_LL151;} else{ goto _LL152;} _LL152: goto _LL153; _LL149: regions=({ struct Cyc_List_List*
_temp158=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp158->hd=( void*) _temp154; _temp158->tl= regions; _temp158;}); goto _LL147;
_LL151: for( 0; _temp156 != 0; _temp156=(( struct Cyc_List_List*) _check_null(
_temp156))->tl){ struct Cyc_List_List* _temp161; struct Cyc_List_List* _temp163;
struct _tuple6 _temp159= Cyc_Absyndump_effects_split(( void*)(( struct Cyc_List_List*)
_check_null( _temp156))->hd); _LL164: _temp163= _temp159.f1; goto _LL162; _LL162:
_temp161= _temp159.f2; goto _LL160; _LL160: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp163, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp161, effects);} goto _LL147;
_LL153: effects=({ struct Cyc_List_List* _temp165=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp165->hd=( void*) t; _temp165->tl=
effects; _temp165;}); goto _LL147; _LL147:;} return({ struct _tuple6 _temp166;
_temp166.f1= regions; _temp166.f2= effects; _temp166;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp169; struct Cyc_List_List* _temp171; struct
_tuple6 _temp167= Cyc_Absyndump_effects_split( t); _LL172: _temp171= _temp167.f1;
goto _LL170; _LL170: _temp169= _temp167.f2; goto _LL168; _LL168: _temp171=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp171);
_temp169=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp169); for( 0; _temp169 != 0; _temp169=(( struct Cyc_List_List*) _check_null(
_temp169))->tl){ Cyc_Absyndump_dumpntyp(( void*)(( struct Cyc_List_List*)
_check_null( _temp169))->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp171 != 0; _temp171=(( struct Cyc_List_List*) _check_null(
_temp171))->tl){ Cyc_Absyndump_dumprgn(( void*)(( struct Cyc_List_List*)
_check_null( _temp171))->hd); if((( struct Cyc_List_List*) _check_null( _temp171))->tl
!= 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');}
void Cyc_Absyndump_dumpntyp( void* t){ void* _temp173= t; struct Cyc_Absyn_Exp*
_temp243; struct Cyc_Absyn_Tqual _temp245; void* _temp247; struct Cyc_Absyn_FnInfo
_temp249; struct Cyc_Absyn_PtrInfo _temp251; struct Cyc_Absyn_Tvar* _temp253;
int _temp255; struct Cyc_Core_Opt* _temp257; void* _temp259; int _temp261;
struct Cyc_Core_Opt* _temp263; struct Cyc_Core_Opt _temp265; void* _temp266;
void* _temp268; struct Cyc_Absyn_TunionInfo _temp270; void* _temp272; struct Cyc_List_List*
_temp274; void* _temp276; struct Cyc_Absyn_TunionFieldInfo _temp278; struct Cyc_List_List*
_temp280; void* _temp282; struct Cyc_Absyn_Enumdecl* _temp284; struct _tuple0*
_temp286; void* _temp288; void* _temp290; void* _temp292; void* _temp294; void*
_temp296; void* _temp298; void* _temp300; void* _temp302; void* _temp304; void*
_temp306; void* _temp308; void* _temp310; void* _temp313; void* _temp315; void*
_temp317; void* _temp319; void* _temp322; void* _temp324; void* _temp326; void*
_temp328; struct Cyc_List_List* _temp330; struct Cyc_Absyn_Structdecl** _temp332;
struct Cyc_List_List* _temp334; struct _tuple0* _temp336; struct Cyc_Absyn_Structdecl**
_temp338; struct Cyc_List_List* _temp340; struct _tuple0* _temp342; struct Cyc_Absyn_Uniondecl**
_temp344; struct Cyc_List_List* _temp346; struct _tuple0* _temp348; struct Cyc_Absyn_Uniondecl**
_temp350; struct Cyc_List_List* _temp352; struct _tuple0* _temp354; struct Cyc_List_List*
_temp356; struct Cyc_List_List* _temp358; struct Cyc_Core_Opt* _temp360; struct
Cyc_List_List* _temp362; struct _tuple0* _temp364; void* _temp366; void*
_temp368; struct Cyc_List_List* _temp370; _LL175: if(( unsigned int) _temp173 >
4u?*(( int*) _temp173) == Cyc_Absyn_ArrayType: 0){ _LL248: _temp247=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp173)->f1; goto _LL246; _LL246: _temp245=((
struct Cyc_Absyn_ArrayType_struct*) _temp173)->f2; goto _LL244; _LL244: _temp243=((
struct Cyc_Absyn_ArrayType_struct*) _temp173)->f3; goto _LL176;} else{ goto
_LL177;} _LL177: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_FnType:
0){ _LL250: _temp249=(( struct Cyc_Absyn_FnType_struct*) _temp173)->f1; goto
_LL178;} else{ goto _LL179;} _LL179: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_PointerType: 0){ _LL252: _temp251=(( struct Cyc_Absyn_PointerType_struct*)
_temp173)->f1; goto _LL180;} else{ goto _LL181;} _LL181: if( _temp173 ==( void*)
Cyc_Absyn_VoidType){ goto _LL182;} else{ goto _LL183;} _LL183: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_VarType: 0){ _LL254: _temp253=((
struct Cyc_Absyn_VarType_struct*) _temp173)->f1; goto _LL184;} else{ goto _LL185;}
_LL185: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_Evar:
0){ _LL260: _temp259=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp173)->f1;
goto _LL258; _LL258: _temp257=(( struct Cyc_Absyn_Evar_struct*) _temp173)->f2;
if( _temp257 == 0){ goto _LL256;} else{ goto _LL187;} _LL256: _temp255=(( struct
Cyc_Absyn_Evar_struct*) _temp173)->f3; goto _LL186;} else{ goto _LL187;} _LL187:
if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_Evar: 0){
_LL269: _temp268=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp173)->f1; goto
_LL264; _LL264: _temp263=(( struct Cyc_Absyn_Evar_struct*) _temp173)->f2; if(
_temp263 == 0){ goto _LL189;} else{ _temp265=* _temp263; _LL267: _temp266=( void*)
_temp265.v; goto _LL262;} _LL262: _temp261=(( struct Cyc_Absyn_Evar_struct*)
_temp173)->f3; goto _LL188;} else{ goto _LL189;} _LL189: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_TunionType: 0){ _LL271: _temp270=((
struct Cyc_Absyn_TunionType_struct*) _temp173)->f1; _LL277: _temp276=( void*)
_temp270.tunion_info; goto _LL275; _LL275: _temp274= _temp270.targs; goto _LL273;
_LL273: _temp272=( void*) _temp270.rgn; goto _LL190;} else{ goto _LL191;} _LL191:
if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_TunionFieldType:
0){ _LL279: _temp278=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp173)->f1;
_LL283: _temp282=( void*) _temp278.field_info; goto _LL281; _LL281: _temp280=
_temp278.targs; goto _LL192;} else{ goto _LL193;} _LL193: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_EnumType: 0){ _LL287: _temp286=((
struct Cyc_Absyn_EnumType_struct*) _temp173)->f1; goto _LL285; _LL285: _temp284=((
struct Cyc_Absyn_EnumType_struct*) _temp173)->f2; goto _LL194;} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL291: _temp290=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp290 ==( void*) Cyc_Absyn_Signed){ goto _LL289;} else{ goto _LL197;}
_LL289: _temp288=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp288 ==( void*) Cyc_Absyn_B4){ goto _LL196;} else{ goto _LL197;}} else{ goto
_LL197;} _LL197: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL295: _temp294=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp294 ==( void*) Cyc_Absyn_Signed){ goto _LL293;} else{ goto _LL199;}
_LL293: _temp292=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp292 ==( void*) Cyc_Absyn_B1){ goto _LL198;} else{ goto _LL199;}} else{ goto
_LL199;} _LL199: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL299: _temp298=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp298 ==( void*) Cyc_Absyn_Unsigned){ goto _LL297;} else{ goto _LL201;}
_LL297: _temp296=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp296 ==( void*) Cyc_Absyn_B1){ goto _LL200;} else{ goto _LL201;}} else{ goto
_LL201;} _LL201: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL303: _temp302=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp302 ==( void*) Cyc_Absyn_Signed){ goto _LL301;} else{ goto _LL203;}
_LL301: _temp300=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp300 ==( void*) Cyc_Absyn_B2){ goto _LL202;} else{ goto _LL203;}} else{ goto
_LL203;} _LL203: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL307: _temp306=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp306 ==( void*) Cyc_Absyn_Unsigned){ goto _LL305;} else{ goto _LL205;}
_LL305: _temp304=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp304 ==( void*) Cyc_Absyn_B2){ goto _LL204;} else{ goto _LL205;}} else{ goto
_LL205;} _LL205: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL311: _temp310=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp310 ==( void*) Cyc_Absyn_Unsigned){ goto _LL309;} else{ goto _LL207;}
_LL309: _temp308=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp308 ==( void*) Cyc_Absyn_B4){ goto _LL206;} else{ goto _LL207;}} else{ goto
_LL207;} _LL207: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL316: _temp315=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp315 ==( void*) Cyc_Absyn_Signed){ goto _LL314;} else{ goto _LL209;}
_LL314: _temp313=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp313 ==( void*) Cyc_Absyn_B8){ goto _LL312;} else{ goto _LL209;}} else{ goto
_LL209;} _LL312: if( Cyc_Absynpp_to_VC){ goto _LL208;} else{ goto _LL209;}
_LL209: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL320: _temp319=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp319 ==( void*) Cyc_Absyn_Signed){ goto _LL318;} else{ goto _LL211;}
_LL318: _temp317=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp317 ==( void*) Cyc_Absyn_B8){ goto _LL210;} else{ goto _LL211;}} else{ goto
_LL211;} _LL211: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL325: _temp324=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp324 ==( void*) Cyc_Absyn_Unsigned){ goto _LL323;} else{ goto _LL213;}
_LL323: _temp322=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp322 ==( void*) Cyc_Absyn_B8){ goto _LL321;} else{ goto _LL213;}} else{ goto
_LL213;} _LL321: if( Cyc_Absynpp_to_VC){ goto _LL212;} else{ goto _LL213;}
_LL213: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_IntType:
0){ _LL329: _temp328=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f1;
if( _temp328 ==( void*) Cyc_Absyn_Unsigned){ goto _LL327;} else{ goto _LL215;}
_LL327: _temp326=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp173)->f2; if(
_temp326 ==( void*) Cyc_Absyn_B8){ goto _LL214;} else{ goto _LL215;}} else{ goto
_LL215;} _LL215: if( _temp173 ==( void*) Cyc_Absyn_FloatType){ goto _LL216;}
else{ goto _LL217;} _LL217: if( _temp173 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL218;} else{ goto _LL219;} _LL219: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_TupleType: 0){ _LL331: _temp330=(( struct Cyc_Absyn_TupleType_struct*)
_temp173)->f1; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_StructType: 0){ _LL337: _temp336=((
struct Cyc_Absyn_StructType_struct*) _temp173)->f1; if( _temp336 == 0){ goto
_LL335;} else{ goto _LL223;} _LL335: _temp334=(( struct Cyc_Absyn_StructType_struct*)
_temp173)->f2; goto _LL333; _LL333: _temp332=(( struct Cyc_Absyn_StructType_struct*)
_temp173)->f3; goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_StructType: 0){ _LL343: _temp342=((
struct Cyc_Absyn_StructType_struct*) _temp173)->f1; goto _LL341; _LL341:
_temp340=(( struct Cyc_Absyn_StructType_struct*) _temp173)->f2; goto _LL339;
_LL339: _temp338=(( struct Cyc_Absyn_StructType_struct*) _temp173)->f3; goto
_LL224;} else{ goto _LL225;} _LL225: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_UnionType: 0){ _LL349: _temp348=(( struct Cyc_Absyn_UnionType_struct*)
_temp173)->f1; if( _temp348 == 0){ goto _LL347;} else{ goto _LL227;} _LL347:
_temp346=(( struct Cyc_Absyn_UnionType_struct*) _temp173)->f2; goto _LL345;
_LL345: _temp344=(( struct Cyc_Absyn_UnionType_struct*) _temp173)->f3; goto
_LL226;} else{ goto _LL227;} _LL227: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_UnionType: 0){ _LL355: _temp354=(( struct Cyc_Absyn_UnionType_struct*)
_temp173)->f1; goto _LL353; _LL353: _temp352=(( struct Cyc_Absyn_UnionType_struct*)
_temp173)->f2; goto _LL351; _LL351: _temp350=(( struct Cyc_Absyn_UnionType_struct*)
_temp173)->f3; goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_AnonStructType: 0){ _LL357:
_temp356=(( struct Cyc_Absyn_AnonStructType_struct*) _temp173)->f1; goto _LL230;}
else{ goto _LL231;} _LL231: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173)
== Cyc_Absyn_AnonUnionType: 0){ _LL359: _temp358=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp173)->f1; goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_TypedefType: 0){ _LL365: _temp364=((
struct Cyc_Absyn_TypedefType_struct*) _temp173)->f1; goto _LL363; _LL363:
_temp362=(( struct Cyc_Absyn_TypedefType_struct*) _temp173)->f2; goto _LL361;
_LL361: _temp360=(( struct Cyc_Absyn_TypedefType_struct*) _temp173)->f3; goto
_LL234;} else{ goto _LL235;} _LL235: if(( unsigned int) _temp173 > 4u?*(( int*)
_temp173) == Cyc_Absyn_RgnHandleType: 0){ _LL367: _temp366=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp173)->f1; goto _LL236;} else{ goto _LL237;} _LL237: if( _temp173 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int)
_temp173 > 4u?*(( int*) _temp173) == Cyc_Absyn_AccessEff: 0){ _LL369: _temp368=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp173)->f1; goto _LL240;} else{
goto _LL241;} _LL241: if(( unsigned int) _temp173 > 4u?*(( int*) _temp173) ==
Cyc_Absyn_JoinEff: 0){ _LL371: _temp370=(( struct Cyc_Absyn_JoinEff_struct*)
_temp173)->f1; goto _LL242;} else{ goto _LL174;} _LL176: return; _LL178: return;
_LL180: return; _LL182: Cyc_Absyndump_dump(({ char* _temp372="void"; struct
_tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 5; _temp373;})); return; _LL184: Cyc_Absyndump_dump_str(
_temp253->name); return; _LL186: Cyc_Absyndump_dump(({ char* _temp374="%";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 2; _temp375;})); Cyc_Absyndump_dumpkind(
_temp259); Cyc_Absyndump_dump( xprintf("(%d)", _temp255)); return; _LL188: Cyc_Absyndump_dumpntyp(
_temp266); return; _LL190:{ void* _temp376= _temp276; struct Cyc_Absyn_UnknownTunionInfo
_temp382; int _temp384; struct _tuple0* _temp386; struct Cyc_Absyn_Tuniondecl*
_temp388; struct Cyc_Absyn_Tuniondecl _temp390; int _temp391; struct Cyc_Core_Opt*
_temp393; struct Cyc_List_List* _temp395; struct _tuple0* _temp397; void*
_temp399; _LL378: if(*(( int*) _temp376) == Cyc_Absyn_UnknownTunion){ _LL383:
_temp382=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp376)->f1; _LL387:
_temp386= _temp382.name; goto _LL385; _LL385: _temp384= _temp382.is_xtunion;
goto _LL379;} else{ goto _LL380;} _LL380: if(*(( int*) _temp376) == Cyc_Absyn_KnownTunion){
_LL389: _temp388=(( struct Cyc_Absyn_KnownTunion_struct*) _temp376)->f1;
_temp390=* _temp388; _LL400: _temp399=( void*) _temp390.sc; goto _LL398; _LL398:
_temp397= _temp390.name; goto _LL396; _LL396: _temp395= _temp390.tvs; goto
_LL394; _LL394: _temp393= _temp390.fields; goto _LL392; _LL392: _temp391=
_temp390.is_xtunion; goto _LL381;} else{ goto _LL377;} _LL379: _temp397=
_temp386; _temp391= _temp384; goto _LL381; _LL381: if( _temp391){ Cyc_Absyndump_dump(({
char* _temp401="xtunion "; struct _tagged_string _temp402; _temp402.curr=
_temp401; _temp402.base= _temp401; _temp402.last_plus_one= _temp401 + 9;
_temp402;}));} else{ Cyc_Absyndump_dump(({ char* _temp403="tunion "; struct
_tagged_string _temp404; _temp404.curr= _temp403; _temp404.base= _temp403;
_temp404.last_plus_one= _temp403 + 8; _temp404;}));}{ void* _temp405= Cyc_Tcutil_compress(
_temp272); _LL407: if( _temp405 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL408;}
else{ goto _LL409;} _LL409: goto _LL410; _LL408: goto _LL406; _LL410: Cyc_Absyndump_dumptyp(
_temp272); Cyc_Absyndump_dump(({ char* _temp411=" "; struct _tagged_string
_temp412; _temp412.curr= _temp411; _temp412.base= _temp411; _temp412.last_plus_one=
_temp411 + 2; _temp412;})); goto _LL406; _LL406:;} Cyc_Absyndump_dumpqvar(
_temp397); Cyc_Absyndump_dumptps( _temp274); goto _LL377; _LL377:;} goto _LL174;
_LL192:{ void* _temp413= _temp282; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp419; int _temp421; struct _tuple0* _temp423; struct _tuple0* _temp425;
struct Cyc_Absyn_Tunionfield* _temp427; struct Cyc_Absyn_Tunionfield _temp429;
void* _temp430; struct Cyc_Position_Segment* _temp432; struct Cyc_List_List*
_temp434; struct Cyc_List_List* _temp436; struct _tuple0* _temp438; struct Cyc_Absyn_Tuniondecl*
_temp440; struct Cyc_Absyn_Tuniondecl _temp442; int _temp443; struct Cyc_Core_Opt*
_temp445; struct Cyc_List_List* _temp447; struct _tuple0* _temp449; void*
_temp451; _LL415: if(*(( int*) _temp413) == Cyc_Absyn_UnknownTunionfield){
_LL420: _temp419=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp413)->f1;
_LL426: _temp425= _temp419.tunion_name; goto _LL424; _LL424: _temp423= _temp419.field_name;
goto _LL422; _LL422: _temp421= _temp419.is_xtunion; goto _LL416;} else{ goto
_LL417;} _LL417: if(*(( int*) _temp413) == Cyc_Absyn_KnownTunionfield){ _LL441:
_temp440=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp413)->f1; _temp442=*
_temp440; _LL452: _temp451=( void*) _temp442.sc; goto _LL450; _LL450: _temp449=
_temp442.name; goto _LL448; _LL448: _temp447= _temp442.tvs; goto _LL446; _LL446:
_temp445= _temp442.fields; goto _LL444; _LL444: _temp443= _temp442.is_xtunion;
goto _LL428; _LL428: _temp427=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp413)->f2; _temp429=* _temp427; _LL439: _temp438= _temp429.name; goto _LL437;
_LL437: _temp436= _temp429.tvs; goto _LL435; _LL435: _temp434= _temp429.typs;
goto _LL433; _LL433: _temp432= _temp429.loc; goto _LL431; _LL431: _temp430=(
void*) _temp429.sc; goto _LL418;} else{ goto _LL414;} _LL416: _temp449= _temp425;
_temp443= _temp421; _temp438= _temp423; goto _LL418; _LL418: if( _temp443){ Cyc_Absyndump_dump(({
char* _temp453="xtunion "; struct _tagged_string _temp454; _temp454.curr=
_temp453; _temp454.base= _temp453; _temp454.last_plus_one= _temp453 + 9;
_temp454;}));} else{ Cyc_Absyndump_dump(({ char* _temp455="tunion "; struct
_tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 8; _temp456;}));} Cyc_Absyndump_dumpqvar(
_temp449); Cyc_Absyndump_dump(({ char* _temp457="."; struct _tagged_string
_temp458; _temp458.curr= _temp457; _temp458.base= _temp457; _temp458.last_plus_one=
_temp457 + 2; _temp458;})); Cyc_Absyndump_dumpqvar( _temp438); Cyc_Absyndump_dumptps(
_temp280); goto _LL414; _LL414:;} goto _LL174; _LL194: Cyc_Absyndump_dump(({
char* _temp459="enum "; struct _tagged_string _temp460; _temp460.curr= _temp459;
_temp460.base= _temp459; _temp460.last_plus_one= _temp459 + 6; _temp460;})); Cyc_Absyndump_dumpqvar(
_temp286); return; _LL196: Cyc_Absyndump_dump(({ char* _temp461="int"; struct
_tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 4; _temp462;})); return; _LL198: Cyc_Absyndump_dump(({
char* _temp463="signed char"; struct _tagged_string _temp464; _temp464.curr=
_temp463; _temp464.base= _temp463; _temp464.last_plus_one= _temp463 + 12;
_temp464;})); return; _LL200: Cyc_Absyndump_dump(({ char* _temp465="char";
struct _tagged_string _temp466; _temp466.curr= _temp465; _temp466.base= _temp465;
_temp466.last_plus_one= _temp465 + 5; _temp466;})); return; _LL202: Cyc_Absyndump_dump(({
char* _temp467="short"; struct _tagged_string _temp468; _temp468.curr= _temp467;
_temp468.base= _temp467; _temp468.last_plus_one= _temp467 + 6; _temp468;}));
return; _LL204: Cyc_Absyndump_dump(({ char* _temp469="unsigned short"; struct
_tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 15; _temp470;})); return; _LL206: Cyc_Absyndump_dump(({
char* _temp471="unsigned int"; struct _tagged_string _temp472; _temp472.curr=
_temp471; _temp472.base= _temp471; _temp472.last_plus_one= _temp471 + 13;
_temp472;})); return; _LL208: Cyc_Absyndump_dump(({ char* _temp473="__int64";
struct _tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 8; _temp474;})); return; _LL210: Cyc_Absyndump_dump(({
char* _temp475="long long"; struct _tagged_string _temp476; _temp476.curr=
_temp475; _temp476.base= _temp475; _temp476.last_plus_one= _temp475 + 10;
_temp476;})); return; _LL212: Cyc_Absyndump_dump(({ char* _temp477="unsigned __int64";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 17; _temp478;})); return; _LL214: Cyc_Absyndump_dump(({
char* _temp479="unsigned long long"; struct _tagged_string _temp480; _temp480.curr=
_temp479; _temp480.base= _temp479; _temp480.last_plus_one= _temp479 + 19;
_temp480;})); return; _LL216: Cyc_Absyndump_dump(({ char* _temp481="float";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 6; _temp482;})); return; _LL218: Cyc_Absyndump_dump(({
char* _temp483="double"; struct _tagged_string _temp484; _temp484.curr= _temp483;
_temp484.base= _temp483; _temp484.last_plus_one= _temp483 + 7; _temp484;}));
return; _LL220: Cyc_Absyndump_dump_char(( int)'$'); Cyc_Absyndump_dumpargs(
_temp330); return; _LL222: Cyc_Absyndump_dump(({ char* _temp485="struct"; struct
_tagged_string _temp486; _temp486.curr= _temp485; _temp486.base= _temp485;
_temp486.last_plus_one= _temp485 + 7; _temp486;})); Cyc_Absyndump_dumptps(
_temp334); return; _LL224: Cyc_Absyndump_dump(({ char* _temp487="struct"; struct
_tagged_string _temp488; _temp488.curr= _temp487; _temp488.base= _temp487;
_temp488.last_plus_one= _temp487 + 7; _temp488;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp342)); Cyc_Absyndump_dumptps( _temp340);
return; _LL226: Cyc_Absyndump_dump(({ char* _temp489="union"; struct
_tagged_string _temp490; _temp490.curr= _temp489; _temp490.base= _temp489;
_temp490.last_plus_one= _temp489 + 6; _temp490;})); Cyc_Absyndump_dumptps(
_temp346); return; _LL228: Cyc_Absyndump_dump(({ char* _temp491="union"; struct
_tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 6; _temp492;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp354)); Cyc_Absyndump_dumptps( _temp352);
return; _LL230: Cyc_Absyndump_dump(({ char* _temp493="struct {"; struct
_tagged_string _temp494; _temp494.curr= _temp493; _temp494.base= _temp493;
_temp494.last_plus_one= _temp493 + 9; _temp494;})); Cyc_Absyndump_dumpstructfields(
_temp356); Cyc_Absyndump_dump(({ char* _temp495="}"; struct _tagged_string
_temp496; _temp496.curr= _temp495; _temp496.base= _temp495; _temp496.last_plus_one=
_temp495 + 2; _temp496;})); return; _LL232: Cyc_Absyndump_dump(({ char* _temp497="union {";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 8; _temp498;})); Cyc_Absyndump_dumpstructfields(
_temp358); Cyc_Absyndump_dump(({ char* _temp499="}"; struct _tagged_string
_temp500; _temp500.curr= _temp499; _temp500.base= _temp499; _temp500.last_plus_one=
_temp499 + 2; _temp500;})); return; _LL234:( Cyc_Absyndump_dumpqvar( _temp364),
Cyc_Absyndump_dumptps( _temp362)); return; _LL236: Cyc_Absyndump_dumprgn(
_temp366); return; _LL238: return; _LL240: return; _LL242: return; _LL174:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*)(( struct Cyc_Core_Opt*) _check_null( vo))->v);}} void
Cyc_Absyndump_dumpfunarg( struct _tuple1* t){(( void(*)( struct Cyc_Absyn_Tqual,
void*, void(* f)( struct Cyc_Core_Opt*), struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)((*
t).f2,(* t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args=(( struct Cyc_List_List*) _check_null( args))->tl){
Cyc_Absyndump_dumpfunarg(( struct _tuple1*)(( struct Cyc_List_List*) _check_null(
args))->hd); if((( struct Cyc_List_List*) _check_null( args))->tl != 0? 1:
varargs){ Cyc_Absyndump_dump_char(( int)',');}} if( varargs){ Cyc_Absyndump_dump(({
char* _temp501="..."; struct _tagged_string _temp502; _temp502.curr= _temp501;
_temp502.base= _temp501; _temp502.last_plus_one= _temp501 + 4; _temp502;}));}
if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v);} Cyc_Absyndump_dump_char(( int)')');}
void Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*,
void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp503; _temp503.q_const= 0; _temp503.q_volatile= 0; _temp503.q_restrict= 0;
_temp503;}), t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp504= d; struct Cyc_Absyn_Exp* _temp510; struct
_tagged_string* _temp512; _LL506: if(*(( int*) _temp504) == Cyc_Absyn_ArrayElement){
_LL511: _temp510=(( struct Cyc_Absyn_ArrayElement_struct*) _temp504)->f1; goto
_LL507;} else{ goto _LL508;} _LL508: if(*(( int*) _temp504) == Cyc_Absyn_FieldName){
_LL513: _temp512=(( struct Cyc_Absyn_FieldName_struct*) _temp504)->f1; goto
_LL509;} else{ goto _LL505;} _LL507: Cyc_Absyndump_dump(({ char* _temp514=".[";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 3; _temp515;})); Cyc_Absyndump_dumpexp(
_temp510); Cyc_Absyndump_dump_char(( int)']'); goto _LL505; _LL509: Cyc_Absyndump_dump_char((
int)'.'); Cyc_Absyndump_dump_nospace(* _temp512); goto _LL505; _LL505:;} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde(
struct _tuple7* de){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(* de).f1,({ char*
_temp516=""; struct _tagged_string _temp517; _temp517.curr= _temp516; _temp517.base=
_temp516; _temp517.last_plus_one= _temp516 + 1; _temp517;}),({ char* _temp518="=";
struct _tagged_string _temp519; _temp519.curr= _temp518; _temp519.base= _temp518;
_temp519.last_plus_one= _temp518 + 2; _temp519;}),({ char* _temp520="="; struct
_tagged_string _temp521; _temp521.curr= _temp520; _temp521.base= _temp520;
_temp521.last_plus_one= _temp520 + 2; _temp521;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,({ char* _temp522=""; struct
_tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 1; _temp523;}),({ char* _temp524=""; struct
_tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 1; _temp525;}),({ char* _temp526=","; struct
_tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 2; _temp527;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(({ char* _temp528="("; struct
_tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 2; _temp529;}));}{ void* _temp530=( void*) e->r;
void* _temp622; char _temp624; void* _temp626; void* _temp628; short _temp630;
void* _temp632; void* _temp634; int _temp636; void* _temp638; void* _temp640;
int _temp642; void* _temp644; void* _temp646; long long _temp648; void* _temp650;
void* _temp652; struct _tagged_string _temp654; void* _temp656; void* _temp658;
struct _tagged_string _temp660; struct _tuple0* _temp662; void* _temp664; struct
_tuple0* _temp666; struct Cyc_List_List* _temp668; void* _temp670; struct Cyc_Absyn_Exp*
_temp672; struct Cyc_Core_Opt* _temp674; struct Cyc_Absyn_Exp* _temp676; void*
_temp678; struct Cyc_Absyn_Exp* _temp680; void* _temp682; struct Cyc_Absyn_Exp*
_temp684; void* _temp686; struct Cyc_Absyn_Exp* _temp688; void* _temp690; struct
Cyc_Absyn_Exp* _temp692; struct Cyc_Absyn_Exp* _temp694; struct Cyc_Absyn_Exp*
_temp696; struct Cyc_Absyn_Exp* _temp698; struct Cyc_Absyn_Exp* _temp700; struct
Cyc_Absyn_Exp* _temp702; struct Cyc_List_List* _temp704; struct Cyc_Absyn_Exp*
_temp706; struct Cyc_List_List* _temp708; struct Cyc_Absyn_Exp* _temp710; struct
Cyc_Absyn_Exp* _temp712; struct Cyc_Absyn_Exp* _temp714; struct Cyc_List_List*
_temp716; struct Cyc_Absyn_Exp* _temp718; struct Cyc_Absyn_Exp* _temp720; void*
_temp722; struct Cyc_Absyn_Exp* _temp724; struct Cyc_Absyn_Exp* _temp726; struct
Cyc_Absyn_Exp* _temp728; void* _temp730; struct Cyc_Absyn_Exp* _temp732; struct
Cyc_Absyn_Exp* _temp734; struct _tagged_string* _temp736; struct Cyc_Absyn_Exp*
_temp738; struct _tagged_string* _temp740; struct Cyc_Absyn_Exp* _temp742;
struct Cyc_Absyn_Exp* _temp744; struct Cyc_Absyn_Exp* _temp746; struct Cyc_List_List*
_temp748; struct Cyc_List_List* _temp750; struct _tuple1* _temp752; struct Cyc_List_List*
_temp754; struct Cyc_Absyn_Exp* _temp756; struct Cyc_Absyn_Exp* _temp758; struct
Cyc_Absyn_Vardecl* _temp760; struct Cyc_Absyn_Structdecl* _temp762; struct Cyc_List_List*
_temp764; struct Cyc_Core_Opt* _temp766; struct _tuple0* _temp768; struct Cyc_List_List*
_temp770; void* _temp772; struct Cyc_Absyn_Tunionfield* _temp774; struct Cyc_Absyn_Tuniondecl*
_temp776; struct Cyc_List_List* _temp778; struct Cyc_Core_Opt* _temp780; struct
Cyc_Core_Opt* _temp782; struct Cyc_Absyn_Enumfield* _temp784; struct Cyc_Absyn_Enumdecl*
_temp786; struct _tuple0* _temp788; void* _temp790; struct Cyc_Absyn_Exp*
_temp792; struct Cyc_List_List* _temp794; struct Cyc_Core_Opt* _temp796; struct
Cyc_Absyn_Stmt* _temp798; struct Cyc_Absyn_Fndecl* _temp800; struct Cyc_Absyn_Exp*
_temp802; _LL532: if(*(( int*) _temp530) == Cyc_Absyn_Const_e){ _LL623: _temp622=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp530)->f1; if(( unsigned int)
_temp622 > 1u?*(( int*) _temp622) == Cyc_Absyn_Char_c: 0){ _LL627: _temp626=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp622)->f1; goto _LL625; _LL625:
_temp624=(( struct Cyc_Absyn_Char_c_struct*) _temp622)->f2; goto _LL533;} else{
goto _LL534;}} else{ goto _LL534;} _LL534: if(*(( int*) _temp530) == Cyc_Absyn_Const_e){
_LL629: _temp628=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp530)->f1; if((
unsigned int) _temp628 > 1u?*(( int*) _temp628) == Cyc_Absyn_Short_c: 0){ _LL633:
_temp632=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp628)->f1; goto _LL631;
_LL631: _temp630=(( struct Cyc_Absyn_Short_c_struct*) _temp628)->f2; goto _LL535;}
else{ goto _LL536;}} else{ goto _LL536;} _LL536: if(*(( int*) _temp530) == Cyc_Absyn_Const_e){
_LL635: _temp634=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp530)->f1; if((
unsigned int) _temp634 > 1u?*(( int*) _temp634) == Cyc_Absyn_Int_c: 0){ _LL639:
_temp638=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp634)->f1; if( _temp638
==( void*) Cyc_Absyn_Signed){ goto _LL637;} else{ goto _LL538;} _LL637: _temp636=((
struct Cyc_Absyn_Int_c_struct*) _temp634)->f2; goto _LL537;} else{ goto _LL538;}}
else{ goto _LL538;} _LL538: if(*(( int*) _temp530) == Cyc_Absyn_Const_e){ _LL641:
_temp640=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp530)->f1; if((
unsigned int) _temp640 > 1u?*(( int*) _temp640) == Cyc_Absyn_Int_c: 0){ _LL645:
_temp644=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp640)->f1; if( _temp644
==( void*) Cyc_Absyn_Unsigned){ goto _LL643;} else{ goto _LL540;} _LL643:
_temp642=(( struct Cyc_Absyn_Int_c_struct*) _temp640)->f2; goto _LL539;} else{
goto _LL540;}} else{ goto _LL540;} _LL540: if(*(( int*) _temp530) == Cyc_Absyn_Const_e){
_LL647: _temp646=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp530)->f1; if((
unsigned int) _temp646 > 1u?*(( int*) _temp646) == Cyc_Absyn_LongLong_c: 0){
_LL651: _temp650=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp646)->f1;
goto _LL649; _LL649: _temp648=(( struct Cyc_Absyn_LongLong_c_struct*) _temp646)->f2;
goto _LL541;} else{ goto _LL542;}} else{ goto _LL542;} _LL542: if(*(( int*)
_temp530) == Cyc_Absyn_Const_e){ _LL653: _temp652=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp530)->f1; if(( unsigned int) _temp652 > 1u?*(( int*) _temp652) == Cyc_Absyn_Float_c:
0){ _LL655: _temp654=(( struct Cyc_Absyn_Float_c_struct*) _temp652)->f1; goto
_LL543;} else{ goto _LL544;}} else{ goto _LL544;} _LL544: if(*(( int*) _temp530)
== Cyc_Absyn_Const_e){ _LL657: _temp656=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp530)->f1; if( _temp656 ==( void*) Cyc_Absyn_Null_c){ goto _LL545;} else{
goto _LL546;}} else{ goto _LL546;} _LL546: if(*(( int*) _temp530) == Cyc_Absyn_Const_e){
_LL659: _temp658=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp530)->f1; if((
unsigned int) _temp658 > 1u?*(( int*) _temp658) == Cyc_Absyn_String_c: 0){
_LL661: _temp660=(( struct Cyc_Absyn_String_c_struct*) _temp658)->f1; goto
_LL547;} else{ goto _LL548;}} else{ goto _LL548;} _LL548: if(*(( int*) _temp530)
== Cyc_Absyn_UnknownId_e){ _LL663: _temp662=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp530)->f1; goto _LL549;} else{ goto _LL550;} _LL550: if(*(( int*) _temp530)
== Cyc_Absyn_Var_e){ _LL667: _temp666=(( struct Cyc_Absyn_Var_e_struct*)
_temp530)->f1; goto _LL665; _LL665: _temp664=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp530)->f2; goto _LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp530)
== Cyc_Absyn_Primop_e){ _LL671: _temp670=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp530)->f1; goto _LL669; _LL669: _temp668=(( struct Cyc_Absyn_Primop_e_struct*)
_temp530)->f2; goto _LL553;} else{ goto _LL554;} _LL554: if(*(( int*) _temp530)
== Cyc_Absyn_AssignOp_e){ _LL677: _temp676=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp530)->f1; goto _LL675; _LL675: _temp674=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp530)->f2; goto _LL673; _LL673: _temp672=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp530)->f3; goto _LL555;} else{ goto _LL556;} _LL556: if(*(( int*) _temp530)
== Cyc_Absyn_Increment_e){ _LL681: _temp680=(( struct Cyc_Absyn_Increment_e_struct*)
_temp530)->f1; goto _LL679; _LL679: _temp678=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp530)->f2; if( _temp678 ==( void*) Cyc_Absyn_PreInc){ goto _LL557;} else{
goto _LL558;}} else{ goto _LL558;} _LL558: if(*(( int*) _temp530) == Cyc_Absyn_Increment_e){
_LL685: _temp684=(( struct Cyc_Absyn_Increment_e_struct*) _temp530)->f1; goto
_LL683; _LL683: _temp682=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp530)->f2; if( _temp682 ==( void*) Cyc_Absyn_PreDec){ goto _LL559;} else{
goto _LL560;}} else{ goto _LL560;} _LL560: if(*(( int*) _temp530) == Cyc_Absyn_Increment_e){
_LL689: _temp688=(( struct Cyc_Absyn_Increment_e_struct*) _temp530)->f1; goto
_LL687; _LL687: _temp686=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp530)->f2; if( _temp686 ==( void*) Cyc_Absyn_PostInc){ goto _LL561;} else{
goto _LL562;}} else{ goto _LL562;} _LL562: if(*(( int*) _temp530) == Cyc_Absyn_Increment_e){
_LL693: _temp692=(( struct Cyc_Absyn_Increment_e_struct*) _temp530)->f1; goto
_LL691; _LL691: _temp690=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp530)->f2; if( _temp690 ==( void*) Cyc_Absyn_PostDec){ goto _LL563;} else{
goto _LL564;}} else{ goto _LL564;} _LL564: if(*(( int*) _temp530) == Cyc_Absyn_Conditional_e){
_LL699: _temp698=(( struct Cyc_Absyn_Conditional_e_struct*) _temp530)->f1; goto
_LL697; _LL697: _temp696=(( struct Cyc_Absyn_Conditional_e_struct*) _temp530)->f2;
goto _LL695; _LL695: _temp694=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp530)->f3; goto _LL565;} else{ goto _LL566;} _LL566: if(*(( int*) _temp530)
== Cyc_Absyn_SeqExp_e){ _LL703: _temp702=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp530)->f1; goto _LL701; _LL701: _temp700=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp530)->f2; goto _LL567;} else{ goto _LL568;} _LL568: if(*(( int*) _temp530)
== Cyc_Absyn_UnknownCall_e){ _LL707: _temp706=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp530)->f1; goto _LL705; _LL705: _temp704=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp530)->f2; goto _LL569;} else{ goto _LL570;} _LL570: if(*(( int*) _temp530)
== Cyc_Absyn_FnCall_e){ _LL711: _temp710=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp530)->f1; goto _LL709; _LL709: _temp708=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp530)->f2; goto _LL571;} else{ goto _LL572;} _LL572: if(*(( int*) _temp530)
== Cyc_Absyn_Throw_e){ _LL713: _temp712=(( struct Cyc_Absyn_Throw_e_struct*)
_temp530)->f1; goto _LL573;} else{ goto _LL574;} _LL574: if(*(( int*) _temp530)
== Cyc_Absyn_NoInstantiate_e){ _LL715: _temp714=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp530)->f1; goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp530)
== Cyc_Absyn_Instantiate_e){ _LL719: _temp718=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp530)->f1; goto _LL717; _LL717: _temp716=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp530)->f2; goto _LL577;} else{ goto _LL578;} _LL578: if(*(( int*) _temp530)
== Cyc_Absyn_Cast_e){ _LL723: _temp722=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp530)->f1; goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_Cast_e_struct*)
_temp530)->f2; goto _LL579;} else{ goto _LL580;} _LL580: if(*(( int*) _temp530)
== Cyc_Absyn_Address_e){ _LL725: _temp724=(( struct Cyc_Absyn_Address_e_struct*)
_temp530)->f1; goto _LL581;} else{ goto _LL582;} _LL582: if(*(( int*) _temp530)
== Cyc_Absyn_New_e){ _LL729: _temp728=(( struct Cyc_Absyn_New_e_struct*)
_temp530)->f1; goto _LL727; _LL727: _temp726=(( struct Cyc_Absyn_New_e_struct*)
_temp530)->f2; goto _LL583;} else{ goto _LL584;} _LL584: if(*(( int*) _temp530)
== Cyc_Absyn_Sizeoftyp_e){ _LL731: _temp730=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp530)->f1; goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp530)
== Cyc_Absyn_Sizeofexp_e){ _LL733: _temp732=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp530)->f1; goto _LL587;} else{ goto _LL588;} _LL588: if(*(( int*) _temp530)
== Cyc_Absyn_Deref_e){ _LL735: _temp734=(( struct Cyc_Absyn_Deref_e_struct*)
_temp530)->f1; goto _LL589;} else{ goto _LL590;} _LL590: if(*(( int*) _temp530)
== Cyc_Absyn_StructMember_e){ _LL739: _temp738=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp530)->f1; goto _LL737; _LL737: _temp736=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp530)->f2; goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp530)
== Cyc_Absyn_StructArrow_e){ _LL743: _temp742=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp530)->f1; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp530)->f2; goto _LL593;} else{ goto _LL594;} _LL594: if(*(( int*) _temp530)
== Cyc_Absyn_Subscript_e){ _LL747: _temp746=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp530)->f1; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp530)->f2; goto _LL595;} else{ goto _LL596;} _LL596: if(*(( int*) _temp530)
== Cyc_Absyn_Tuple_e){ _LL749: _temp748=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp530)->f1; goto _LL597;} else{ goto _LL598;} _LL598: if(*(( int*) _temp530)
== Cyc_Absyn_CompoundLit_e){ _LL753: _temp752=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp530)->f1; goto _LL751; _LL751: _temp750=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp530)->f2; goto _LL599;} else{ goto _LL600;} _LL600: if(*(( int*) _temp530)
== Cyc_Absyn_Array_e){ _LL755: _temp754=(( struct Cyc_Absyn_Array_e_struct*)
_temp530)->f1; goto _LL601;} else{ goto _LL602;} _LL602: if(*(( int*) _temp530)
== Cyc_Absyn_Comprehension_e){ _LL761: _temp760=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp530)->f1; goto _LL759; _LL759: _temp758=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp530)->f2; goto _LL757; _LL757: _temp756=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp530)->f3; goto _LL603;} else{ goto _LL604;} _LL604: if(*(( int*) _temp530)
== Cyc_Absyn_Struct_e){ _LL769: _temp768=(( struct Cyc_Absyn_Struct_e_struct*)
_temp530)->f1; goto _LL767; _LL767: _temp766=(( struct Cyc_Absyn_Struct_e_struct*)
_temp530)->f2; goto _LL765; _LL765: _temp764=(( struct Cyc_Absyn_Struct_e_struct*)
_temp530)->f3; goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_Struct_e_struct*)
_temp530)->f4; goto _LL605;} else{ goto _LL606;} _LL606: if(*(( int*) _temp530)
== Cyc_Absyn_AnonStruct_e){ _LL773: _temp772=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp530)->f1; goto _LL771; _LL771: _temp770=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp530)->f2; goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp530)
== Cyc_Absyn_Tunion_e){ _LL783: _temp782=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp530)->f1; goto _LL781; _LL781: _temp780=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp530)->f2; goto _LL779; _LL779: _temp778=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp530)->f3; goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp530)->f4; goto _LL775; _LL775: _temp774=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp530)->f5; goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp530)
== Cyc_Absyn_Enum_e){ _LL789: _temp788=(( struct Cyc_Absyn_Enum_e_struct*)
_temp530)->f1; goto _LL787; _LL787: _temp786=(( struct Cyc_Absyn_Enum_e_struct*)
_temp530)->f2; goto _LL785; _LL785: _temp784=(( struct Cyc_Absyn_Enum_e_struct*)
_temp530)->f3; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp530)
== Cyc_Absyn_Malloc_e){ _LL793: _temp792=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp530)->f1; goto _LL791; _LL791: _temp790=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp530)->f2; goto _LL613;} else{ goto _LL614;} _LL614: if(*(( int*) _temp530)
== Cyc_Absyn_UnresolvedMem_e){ _LL797: _temp796=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp530)->f1; goto _LL795; _LL795: _temp794=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp530)->f2; goto _LL615;} else{ goto _LL616;} _LL616: if(*(( int*) _temp530)
== Cyc_Absyn_StmtExp_e){ _LL799: _temp798=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp530)->f1; goto _LL617;} else{ goto _LL618;} _LL618: if(*(( int*) _temp530)
== Cyc_Absyn_Codegen_e){ _LL801: _temp800=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp530)->f1; goto _LL619;} else{ goto _LL620;} _LL620: if(*(( int*) _temp530)
== Cyc_Absyn_Fill_e){ _LL803: _temp802=(( struct Cyc_Absyn_Fill_e_struct*)
_temp530)->f1; goto _LL621;} else{ goto _LL531;} _LL533: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp624)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL531; _LL535: Cyc_Absyndump_dump(({ char* _temp804="(short)";
struct _tagged_string _temp805; _temp805.curr= _temp804; _temp805.base= _temp804;
_temp805.last_plus_one= _temp804 + 8; _temp805;})); Cyc_Absyndump_dump_nospace(
Cyc_Core_string_of_int(( int) _temp630)); goto _LL531; _LL537: Cyc_Absyndump_dump(
Cyc_Core_string_of_int( _temp636)); goto _LL531; _LL539: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp642)); Cyc_Absyndump_dump_nospace(({ char* _temp806="u"; struct
_tagged_string _temp807; _temp807.curr= _temp806; _temp807.base= _temp806;
_temp807.last_plus_one= _temp806 + 2; _temp807;})); goto _LL531; _LL541: Cyc_Absyndump_dump(({
char* _temp808="<<FIX LONG LONG CONSTANT>>"; struct _tagged_string _temp809;
_temp809.curr= _temp808; _temp809.base= _temp808; _temp809.last_plus_one=
_temp808 + 27; _temp809;})); goto _LL531; _LL543: Cyc_Absyndump_dump( _temp654);
goto _LL531; _LL545: Cyc_Absyndump_dump(({ char* _temp810="null"; struct
_tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 5; _temp811;})); goto _LL531; _LL547: Cyc_Absyndump_dump_char((
int)'"'); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_string_escape( _temp660)); Cyc_Absyndump_dump_char((
int)'"'); goto _LL531; _LL549: _temp666= _temp662; goto _LL551; _LL551: Cyc_Absyndump_dumpqvar(
_temp666); goto _LL531; _LL553: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp670); if( ! Cyc_Absyn_is_format_prim( _temp670)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp668)){ case 1: _LL812: if( _temp670 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp668))->hd); Cyc_Absyndump_dump(({ char*
_temp814=".size"; struct _tagged_string _temp815; _temp815.curr= _temp814;
_temp815.base= _temp814; _temp815.last_plus_one= _temp814 + 6; _temp815;}));}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp668))->hd);} break; case 2: _LL813: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp668))->hd);
Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp668))->tl))->hd); break; default: _LL816:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp818=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp818[ 0]=({ struct Cyc_Core_Failure_struct
_temp819; _temp819.tag= Cyc_Core_Failure; _temp819.f1=({ char* _temp820="Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 47; _temp821;}); _temp819;}); _temp818;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(({ char* _temp822="(";
struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 2; _temp823;})); Cyc_Absyndump_dumpexps_prec(
20, _temp668); Cyc_Absyndump_dump_nospace(({ char* _temp824=")"; struct
_tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 2; _temp825;}));} goto _LL531;} _LL555: Cyc_Absyndump_dumpexp_prec(
myprec, _temp676); if( _temp674 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp674))->v));} Cyc_Absyndump_dump_nospace(({
char* _temp826="="; struct _tagged_string _temp827; _temp827.curr= _temp826;
_temp827.base= _temp826; _temp827.last_plus_one= _temp826 + 2; _temp827;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp672); goto _LL531; _LL557: Cyc_Absyndump_dump(({ char* _temp828="++";
struct _tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 3; _temp829;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp680); goto _LL531; _LL559: Cyc_Absyndump_dump(({ char* _temp830="--";
struct _tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 3; _temp831;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp684); goto _LL531; _LL561: Cyc_Absyndump_dumpexp_prec( myprec,
_temp688); Cyc_Absyndump_dump(({ char* _temp832="++"; struct _tagged_string
_temp833; _temp833.curr= _temp832; _temp833.base= _temp832; _temp833.last_plus_one=
_temp832 + 3; _temp833;})); goto _LL531; _LL563: Cyc_Absyndump_dumpexp_prec(
myprec, _temp692); Cyc_Absyndump_dump(({ char* _temp834="--"; struct
_tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 3; _temp835;})); goto _LL531; _LL565: Cyc_Absyndump_dumpexp_prec(
myprec, _temp698); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp696); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp694); goto _LL531; _LL567: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp702); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp700); Cyc_Absyndump_dump_char(( int)')'); goto _LL531; _LL569:
_temp710= _temp706; _temp708= _temp704; goto _LL571; _LL571: Cyc_Absyndump_dumpexp_prec(
myprec, _temp710); Cyc_Absyndump_dump_nospace(({ char* _temp836="("; struct
_tagged_string _temp837; _temp837.curr= _temp836; _temp837.base= _temp836;
_temp837.last_plus_one= _temp836 + 2; _temp837;})); Cyc_Absyndump_dumpexps_prec(
20, _temp708); Cyc_Absyndump_dump_nospace(({ char* _temp838=")"; struct
_tagged_string _temp839; _temp839.curr= _temp838; _temp839.base= _temp838;
_temp839.last_plus_one= _temp838 + 2; _temp839;})); goto _LL531; _LL573: Cyc_Absyndump_dump(({
char* _temp840="throw"; struct _tagged_string _temp841; _temp841.curr= _temp840;
_temp841.base= _temp840; _temp841.last_plus_one= _temp840 + 6; _temp841;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp712); goto _LL531; _LL575: _temp718= _temp714; goto _LL577; _LL577:
Cyc_Absyndump_dumpexp_prec( inprec, _temp718); goto _LL531; _LL579: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp722); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp720); goto _LL531; _LL581: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp724); goto _LL531; _LL583:
Cyc_Absyndump_dump(({ char* _temp842="new "; struct _tagged_string _temp843;
_temp843.curr= _temp842; _temp843.base= _temp842; _temp843.last_plus_one=
_temp842 + 5; _temp843;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp726); goto
_LL531; _LL585: Cyc_Absyndump_dump(({ char* _temp844="sizeof("; struct
_tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 8; _temp845;})); Cyc_Absyndump_dumptyp(
_temp730); Cyc_Absyndump_dump_char(( int)')'); goto _LL531; _LL587: Cyc_Absyndump_dump(({
char* _temp846="sizeof("; struct _tagged_string _temp847; _temp847.curr=
_temp846; _temp847.base= _temp846; _temp847.last_plus_one= _temp846 + 8;
_temp847;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp732); Cyc_Absyndump_dump_char((
int)')'); goto _LL531; _LL589: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp734); goto _LL531; _LL591: Cyc_Absyndump_dumpexp_prec( myprec,
_temp738); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp736); goto _LL531; _LL593: Cyc_Absyndump_dumpexp_prec( myprec, _temp742);
Cyc_Absyndump_dump_nospace(({ char* _temp848="->"; struct _tagged_string
_temp849; _temp849.curr= _temp848; _temp849.base= _temp848; _temp849.last_plus_one=
_temp848 + 3; _temp849;})); Cyc_Absyndump_dump_nospace(* _temp740); goto _LL531;
_LL595: Cyc_Absyndump_dumpexp_prec( myprec, _temp746); Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp744); Cyc_Absyndump_dump_char(( int)']');
goto _LL531; _LL597: Cyc_Absyndump_dump(({ char* _temp850="$("; struct
_tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 3; _temp851;})); Cyc_Absyndump_dumpexps_prec(
20, _temp748); Cyc_Absyndump_dump_char(( int)')'); goto _LL531; _LL599: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp((* _temp752).f3); Cyc_Absyndump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp750,({ char* _temp852="{"; struct _tagged_string
_temp853; _temp853.curr= _temp852; _temp853.base= _temp852; _temp853.last_plus_one=
_temp852 + 2; _temp853;}),({ char* _temp854="}"; struct _tagged_string _temp855;
_temp855.curr= _temp854; _temp855.base= _temp854; _temp855.last_plus_one=
_temp854 + 2; _temp855;}),({ char* _temp856=","; struct _tagged_string _temp857;
_temp857.curr= _temp856; _temp857.base= _temp856; _temp857.last_plus_one=
_temp856 + 2; _temp857;})); goto _LL531; _LL601:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp754,({ char* _temp858="{"; struct _tagged_string _temp859; _temp859.curr=
_temp858; _temp859.base= _temp858; _temp859.last_plus_one= _temp858 + 2;
_temp859;}),({ char* _temp860="}"; struct _tagged_string _temp861; _temp861.curr=
_temp860; _temp861.base= _temp860; _temp861.last_plus_one= _temp860 + 2;
_temp861;}),({ char* _temp862=","; struct _tagged_string _temp863; _temp863.curr=
_temp862; _temp863.base= _temp862; _temp863.last_plus_one= _temp862 + 2;
_temp863;})); goto _LL531; _LL603: Cyc_Absyndump_dump(({ char* _temp864="new {for";
struct _tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 9; _temp865;})); Cyc_Absyndump_dump_str((*
_temp760->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp758); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp756);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL531; _LL605: Cyc_Absyndump_dumpqvar(
_temp768);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp764,({ char* _temp866="{";
struct _tagged_string _temp867; _temp867.curr= _temp866; _temp867.base= _temp866;
_temp867.last_plus_one= _temp866 + 2; _temp867;}),({ char* _temp868="}"; struct
_tagged_string _temp869; _temp869.curr= _temp868; _temp869.base= _temp868;
_temp869.last_plus_one= _temp868 + 2; _temp869;}),({ char* _temp870=","; struct
_tagged_string _temp871; _temp871.curr= _temp870; _temp871.base= _temp870;
_temp871.last_plus_one= _temp870 + 2; _temp871;})); goto _LL531; _LL607:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp770,({ char* _temp872="{"; struct _tagged_string
_temp873; _temp873.curr= _temp872; _temp873.base= _temp872; _temp873.last_plus_one=
_temp872 + 2; _temp873;}),({ char* _temp874="}"; struct _tagged_string _temp875;
_temp875.curr= _temp874; _temp875.base= _temp874; _temp875.last_plus_one=
_temp874 + 2; _temp875;}),({ char* _temp876=","; struct _tagged_string _temp877;
_temp877.curr= _temp876; _temp877.base= _temp876; _temp877.last_plus_one=
_temp876 + 2; _temp877;})); goto _LL531; _LL609: Cyc_Absyndump_dumpqvar(
_temp774->name); if( _temp778 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpexp,
_temp778,({ char* _temp878="("; struct _tagged_string _temp879; _temp879.curr=
_temp878; _temp879.base= _temp878; _temp879.last_plus_one= _temp878 + 2;
_temp879;}),({ char* _temp880=")"; struct _tagged_string _temp881; _temp881.curr=
_temp880; _temp881.base= _temp880; _temp881.last_plus_one= _temp880 + 2;
_temp881;}),({ char* _temp882=","; struct _tagged_string _temp883; _temp883.curr=
_temp882; _temp883.base= _temp882; _temp883.last_plus_one= _temp882 + 2;
_temp883;}));} goto _LL531; _LL611: Cyc_Absyndump_dumpqvar( _temp788); goto
_LL531; _LL613: if( _temp792 != 0){ Cyc_Absyndump_dump(({ char* _temp884="rmalloc(";
struct _tagged_string _temp885; _temp885.curr= _temp884; _temp885.base= _temp884;
_temp885.last_plus_one= _temp884 + 9; _temp885;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp792)); Cyc_Absyndump_dump(({ char*
_temp886=","; struct _tagged_string _temp887; _temp887.curr= _temp886; _temp887.base=
_temp886; _temp887.last_plus_one= _temp886 + 2; _temp887;}));} else{ Cyc_Absyndump_dump(({
char* _temp888="malloc("; struct _tagged_string _temp889; _temp889.curr=
_temp888; _temp889.base= _temp888; _temp889.last_plus_one= _temp888 + 8;
_temp889;}));} Cyc_Absyndump_dump(({ char* _temp890="sizeof("; struct
_tagged_string _temp891; _temp891.curr= _temp890; _temp891.base= _temp890;
_temp891.last_plus_one= _temp890 + 8; _temp891;})); Cyc_Absyndump_dumptyp(
_temp790); Cyc_Absyndump_dump(({ char* _temp892="))"; struct _tagged_string
_temp893; _temp893.curr= _temp892; _temp893.base= _temp892; _temp893.last_plus_one=
_temp892 + 3; _temp893;})); goto _LL531; _LL615:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp794,({ char* _temp894="{"; struct _tagged_string _temp895; _temp895.curr=
_temp894; _temp895.base= _temp894; _temp895.last_plus_one= _temp894 + 2;
_temp895;}),({ char* _temp896="}"; struct _tagged_string _temp897; _temp897.curr=
_temp896; _temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 2;
_temp897;}),({ char* _temp898=","; struct _tagged_string _temp899; _temp899.curr=
_temp898; _temp899.base= _temp898; _temp899.last_plus_one= _temp898 + 2;
_temp899;})); goto _LL531; _LL617: Cyc_Absyndump_dump_nospace(({ char* _temp900="({";
struct _tagged_string _temp901; _temp901.curr= _temp900; _temp901.base= _temp900;
_temp901.last_plus_one= _temp900 + 3; _temp901;})); Cyc_Absyndump_dumpstmt(
_temp798); Cyc_Absyndump_dump_nospace(({ char* _temp902="})"; struct
_tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 3; _temp903;})); goto _LL531; _LL619: Cyc_Absyndump_dump(({
char* _temp904="codegen("; struct _tagged_string _temp905; _temp905.curr=
_temp904; _temp905.base= _temp904; _temp905.last_plus_one= _temp904 + 9;
_temp905;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl* _temp906=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp906->r=( void*)((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp907=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp907[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp908; _temp908.tag= Cyc_Absyn_Fn_d; _temp908.f1= _temp800; _temp908;});
_temp907;})); _temp906->loc= e->loc; _temp906;})); Cyc_Absyndump_dump(({ char*
_temp909=")"; struct _tagged_string _temp910; _temp910.curr= _temp909; _temp910.base=
_temp909; _temp910.last_plus_one= _temp909 + 2; _temp910;})); goto _LL531;
_LL621: Cyc_Absyndump_dump(({ char* _temp911="fill("; struct _tagged_string
_temp912; _temp912.curr= _temp911; _temp912.base= _temp911; _temp912.last_plus_one=
_temp911 + 6; _temp912;})); Cyc_Absyndump_dumpexp( _temp802); Cyc_Absyndump_dump(({
char* _temp913=")"; struct _tagged_string _temp914; _temp914.curr= _temp913;
_temp914.base= _temp913; _temp914.last_plus_one= _temp913 + 2; _temp914;}));
goto _LL531; _LL531:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}}
void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp915=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL916; _LL916: if( _temp915->where_clause == 0?( void*)(
_temp915->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(({
char* _temp917="default:"; struct _tagged_string _temp918; _temp918.curr=
_temp917; _temp918.base= _temp917; _temp918.last_plus_one= _temp917 + 9;
_temp918;}));} else{ Cyc_Absyndump_dump(({ char* _temp919="case"; struct
_tagged_string _temp920; _temp920.curr= _temp919; _temp920.base= _temp919;
_temp920.last_plus_one= _temp919 + 5; _temp920;})); Cyc_Absyndump_dumppat(
_temp915->pattern); if( _temp915->where_clause != 0){ Cyc_Absyndump_dump(({ char*
_temp921="&&"; struct _tagged_string _temp922; _temp922.curr= _temp921; _temp922.base=
_temp921; _temp922.last_plus_one= _temp921 + 3; _temp922;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp915->where_clause));} Cyc_Absyndump_dump_nospace(({
char* _temp923=":"; struct _tagged_string _temp924; _temp924.curr= _temp923;
_temp924.base= _temp923; _temp924.last_plus_one= _temp923 + 2; _temp924;}));}
Cyc_Absyndump_dumpstmt( _temp915->body);}} void Cyc_Absyndump_dumpstmt( struct
Cyc_Absyn_Stmt* s){ void* _temp925=( void*) s->r; struct Cyc_Absyn_Exp* _temp969;
struct Cyc_Absyn_Stmt* _temp971; struct Cyc_Absyn_Stmt* _temp973; struct Cyc_Absyn_Exp*
_temp975; struct Cyc_Absyn_Exp* _temp977; struct Cyc_Absyn_Stmt* _temp979;
struct Cyc_Absyn_Stmt* _temp981; struct Cyc_Absyn_Exp* _temp983; struct Cyc_Absyn_Stmt*
_temp985; struct _tuple2 _temp987; struct Cyc_Absyn_Stmt* _temp989; struct Cyc_Absyn_Exp*
_temp991; struct Cyc_Absyn_Stmt* _temp993; struct Cyc_Absyn_Stmt* _temp995;
struct Cyc_Absyn_Stmt* _temp997; struct _tagged_string* _temp999; struct Cyc_Absyn_Switch_clause**
_temp1001; struct Cyc_List_List* _temp1003; struct Cyc_Absyn_Switch_clause**
_temp1005; struct Cyc_List_List* _temp1007; struct Cyc_Absyn_Stmt* _temp1009;
struct _tuple2 _temp1011; struct Cyc_Absyn_Stmt* _temp1013; struct Cyc_Absyn_Exp*
_temp1015; struct _tuple2 _temp1017; struct Cyc_Absyn_Stmt* _temp1019; struct
Cyc_Absyn_Exp* _temp1021; struct Cyc_Absyn_Exp* _temp1023; struct Cyc_List_List*
_temp1025; struct Cyc_Absyn_Exp* _temp1027; struct Cyc_Absyn_Stmt* _temp1029;
struct Cyc_Absyn_Decl* _temp1031; struct Cyc_Absyn_Stmt* _temp1033; struct Cyc_Absyn_Stmt*
_temp1035; struct Cyc_Absyn_Stmt* _temp1037; struct _tagged_string* _temp1039;
struct _tuple2 _temp1041; struct Cyc_Absyn_Stmt* _temp1043; struct Cyc_Absyn_Exp*
_temp1045; struct Cyc_Absyn_Stmt* _temp1047; struct Cyc_List_List* _temp1049;
struct Cyc_Absyn_Stmt* _temp1051; struct Cyc_Absyn_Stmt* _temp1053; struct Cyc_Absyn_Vardecl*
_temp1055; struct Cyc_Absyn_Tvar* _temp1057; _LL927: if( _temp925 ==( void*) Cyc_Absyn_Skip_s){
goto _LL928;} else{ goto _LL929;} _LL929: if(( unsigned int) _temp925 > 1u?*((
int*) _temp925) == Cyc_Absyn_Exp_s: 0){ _LL970: _temp969=(( struct Cyc_Absyn_Exp_s_struct*)
_temp925)->f1; goto _LL930;} else{ goto _LL931;} _LL931: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Seq_s: 0){ _LL974: _temp973=((
struct Cyc_Absyn_Seq_s_struct*) _temp925)->f1; goto _LL972; _LL972: _temp971=((
struct Cyc_Absyn_Seq_s_struct*) _temp925)->f2; goto _LL932;} else{ goto _LL933;}
_LL933: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Return_s:
0){ _LL976: _temp975=(( struct Cyc_Absyn_Return_s_struct*) _temp925)->f1; if(
_temp975 == 0){ goto _LL934;} else{ goto _LL935;}} else{ goto _LL935;} _LL935:
if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Return_s: 0){
_LL978: _temp977=(( struct Cyc_Absyn_Return_s_struct*) _temp925)->f1; goto
_LL936;} else{ goto _LL937;} _LL937: if(( unsigned int) _temp925 > 1u?*(( int*)
_temp925) == Cyc_Absyn_IfThenElse_s: 0){ _LL984: _temp983=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp925)->f1; goto _LL982; _LL982: _temp981=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp925)->f2; goto _LL980; _LL980: _temp979=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp925)->f3; goto _LL938;} else{ goto _LL939;} _LL939: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_While_s: 0){ _LL988: _temp987=((
struct Cyc_Absyn_While_s_struct*) _temp925)->f1; _LL992: _temp991= _temp987.f1;
goto _LL990; _LL990: _temp989= _temp987.f2; goto _LL986; _LL986: _temp985=((
struct Cyc_Absyn_While_s_struct*) _temp925)->f2; goto _LL940;} else{ goto _LL941;}
_LL941: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Break_s:
0){ _LL994: _temp993=(( struct Cyc_Absyn_Break_s_struct*) _temp925)->f1; goto
_LL942;} else{ goto _LL943;} _LL943: if(( unsigned int) _temp925 > 1u?*(( int*)
_temp925) == Cyc_Absyn_Continue_s: 0){ _LL996: _temp995=(( struct Cyc_Absyn_Continue_s_struct*)
_temp925)->f1; goto _LL944;} else{ goto _LL945;} _LL945: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Goto_s: 0){ _LL1000: _temp999=((
struct Cyc_Absyn_Goto_s_struct*) _temp925)->f1; goto _LL998; _LL998: _temp997=((
struct Cyc_Absyn_Goto_s_struct*) _temp925)->f2; goto _LL946;} else{ goto _LL947;}
_LL947: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Fallthru_s:
0){ _LL1004: _temp1003=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp925)->f1;
if( _temp1003 == 0){ goto _LL1002;} else{ goto _LL949;} _LL1002: _temp1001=((
struct Cyc_Absyn_Fallthru_s_struct*) _temp925)->f2; goto _LL948;} else{ goto
_LL949;} _LL949: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Fallthru_s:
0){ _LL1008: _temp1007=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp925)->f1;
goto _LL1006; _LL1006: _temp1005=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp925)->f2; goto _LL950;} else{ goto _LL951;} _LL951: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_For_s: 0){ _LL1024: _temp1023=((
struct Cyc_Absyn_For_s_struct*) _temp925)->f1; goto _LL1018; _LL1018: _temp1017=((
struct Cyc_Absyn_For_s_struct*) _temp925)->f2; _LL1022: _temp1021= _temp1017.f1;
goto _LL1020; _LL1020: _temp1019= _temp1017.f2; goto _LL1012; _LL1012: _temp1011=((
struct Cyc_Absyn_For_s_struct*) _temp925)->f3; _LL1016: _temp1015= _temp1011.f1;
goto _LL1014; _LL1014: _temp1013= _temp1011.f2; goto _LL1010; _LL1010: _temp1009=((
struct Cyc_Absyn_For_s_struct*) _temp925)->f4; goto _LL952;} else{ goto _LL953;}
_LL953: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Switch_s:
0){ _LL1028: _temp1027=(( struct Cyc_Absyn_Switch_s_struct*) _temp925)->f1; goto
_LL1026; _LL1026: _temp1025=(( struct Cyc_Absyn_Switch_s_struct*) _temp925)->f2;
goto _LL954;} else{ goto _LL955;} _LL955: if(( unsigned int) _temp925 > 1u?*((
int*) _temp925) == Cyc_Absyn_Decl_s: 0){ _LL1032: _temp1031=(( struct Cyc_Absyn_Decl_s_struct*)
_temp925)->f1; goto _LL1030; _LL1030: _temp1029=(( struct Cyc_Absyn_Decl_s_struct*)
_temp925)->f2; goto _LL956;} else{ goto _LL957;} _LL957: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Cut_s: 0){ _LL1034: _temp1033=((
struct Cyc_Absyn_Cut_s_struct*) _temp925)->f1; goto _LL958;} else{ goto _LL959;}
_LL959: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Splice_s:
0){ _LL1036: _temp1035=(( struct Cyc_Absyn_Splice_s_struct*) _temp925)->f1; goto
_LL960;} else{ goto _LL961;} _LL961: if(( unsigned int) _temp925 > 1u?*(( int*)
_temp925) == Cyc_Absyn_Label_s: 0){ _LL1040: _temp1039=(( struct Cyc_Absyn_Label_s_struct*)
_temp925)->f1; goto _LL1038; _LL1038: _temp1037=(( struct Cyc_Absyn_Label_s_struct*)
_temp925)->f2; goto _LL962;} else{ goto _LL963;} _LL963: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Do_s: 0){ _LL1048: _temp1047=((
struct Cyc_Absyn_Do_s_struct*) _temp925)->f1; goto _LL1042; _LL1042: _temp1041=((
struct Cyc_Absyn_Do_s_struct*) _temp925)->f2; _LL1046: _temp1045= _temp1041.f1;
goto _LL1044; _LL1044: _temp1043= _temp1041.f2; goto _LL964;} else{ goto _LL965;}
_LL965: if(( unsigned int) _temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_TryCatch_s:
0){ _LL1052: _temp1051=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp925)->f1;
goto _LL1050; _LL1050: _temp1049=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp925)->f2; goto _LL966;} else{ goto _LL967;} _LL967: if(( unsigned int)
_temp925 > 1u?*(( int*) _temp925) == Cyc_Absyn_Region_s: 0){ _LL1058: _temp1057=((
struct Cyc_Absyn_Region_s_struct*) _temp925)->f1; goto _LL1056; _LL1056:
_temp1055=(( struct Cyc_Absyn_Region_s_struct*) _temp925)->f2; goto _LL1054;
_LL1054: _temp1053=(( struct Cyc_Absyn_Region_s_struct*) _temp925)->f3; goto
_LL968;} else{ goto _LL926;} _LL928: Cyc_Absyndump_dump_semi(); goto _LL926;
_LL930: Cyc_Absyndump_dumpexp( _temp969); Cyc_Absyndump_dump_semi(); goto _LL926;
_LL932: if( Cyc_Absynpp_is_declaration( _temp973)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp973); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp973);} if( Cyc_Absynpp_is_declaration(
_temp971)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp971);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp971);}
goto _LL926; _LL934: Cyc_Absyndump_dump(({ char* _temp1059="return;"; struct
_tagged_string _temp1060; _temp1060.curr= _temp1059; _temp1060.base= _temp1059;
_temp1060.last_plus_one= _temp1059 + 8; _temp1060;})); goto _LL926; _LL936: Cyc_Absyndump_dump(({
char* _temp1061="return"; struct _tagged_string _temp1062; _temp1062.curr=
_temp1061; _temp1062.base= _temp1061; _temp1062.last_plus_one= _temp1061 + 7;
_temp1062;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null(
_temp977)); Cyc_Absyndump_dump_semi(); goto _LL926; _LL938: Cyc_Absyndump_dump(({
char* _temp1063="if("; struct _tagged_string _temp1064; _temp1064.curr=
_temp1063; _temp1064.base= _temp1063; _temp1064.last_plus_one= _temp1063 + 4;
_temp1064;})); Cyc_Absyndump_dumpexp( _temp983); Cyc_Absyndump_dump_nospace(({
char* _temp1065="){"; struct _tagged_string _temp1066; _temp1066.curr= _temp1065;
_temp1066.base= _temp1065; _temp1066.last_plus_one= _temp1065 + 3; _temp1066;}));
Cyc_Absyndump_dumpstmt( _temp981); Cyc_Absyndump_dump_char(( int)'}');{ void*
_temp1067=( void*) _temp979->r; _LL1069: if( _temp1067 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1070;} else{ goto _LL1071;} _LL1071: goto _LL1072; _LL1070: goto _LL1068;
_LL1072: Cyc_Absyndump_dump(({ char* _temp1073="else{"; struct _tagged_string
_temp1074; _temp1074.curr= _temp1073; _temp1074.base= _temp1073; _temp1074.last_plus_one=
_temp1073 + 6; _temp1074;})); Cyc_Absyndump_dumpstmt( _temp979); Cyc_Absyndump_dump_char((
int)'}'); goto _LL1068; _LL1068:;} goto _LL926; _LL940: Cyc_Absyndump_dump(({
char* _temp1075="while("; struct _tagged_string _temp1076; _temp1076.curr=
_temp1075; _temp1076.base= _temp1075; _temp1076.last_plus_one= _temp1075 + 7;
_temp1076;})); Cyc_Absyndump_dumpexp( _temp991); Cyc_Absyndump_dump_nospace(({
char* _temp1077=") {"; struct _tagged_string _temp1078; _temp1078.curr=
_temp1077; _temp1078.base= _temp1077; _temp1078.last_plus_one= _temp1077 + 4;
_temp1078;})); Cyc_Absyndump_dumpstmt( _temp985); Cyc_Absyndump_dump_char(( int)'}');
goto _LL926; _LL942: Cyc_Absyndump_dump(({ char* _temp1079="break;"; struct
_tagged_string _temp1080; _temp1080.curr= _temp1079; _temp1080.base= _temp1079;
_temp1080.last_plus_one= _temp1079 + 7; _temp1080;})); goto _LL926; _LL944: Cyc_Absyndump_dump(({
char* _temp1081="continue;"; struct _tagged_string _temp1082; _temp1082.curr=
_temp1081; _temp1082.base= _temp1081; _temp1082.last_plus_one= _temp1081 + 10;
_temp1082;})); goto _LL926; _LL946: Cyc_Absyndump_dump(({ char* _temp1083="goto";
struct _tagged_string _temp1084; _temp1084.curr= _temp1083; _temp1084.base=
_temp1083; _temp1084.last_plus_one= _temp1083 + 5; _temp1084;})); Cyc_Absyndump_dump_str(
_temp999); Cyc_Absyndump_dump_semi(); goto _LL926; _LL948: Cyc_Absyndump_dump(({
char* _temp1085="fallthru;"; struct _tagged_string _temp1086; _temp1086.curr=
_temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one= _temp1085 + 10;
_temp1086;})); goto _LL926; _LL950: Cyc_Absyndump_dump(({ char* _temp1087="fallthru(";
struct _tagged_string _temp1088; _temp1088.curr= _temp1087; _temp1088.base=
_temp1087; _temp1088.last_plus_one= _temp1087 + 10; _temp1088;})); Cyc_Absyndump_dumpexps_prec(
20, _temp1007); Cyc_Absyndump_dump_nospace(({ char* _temp1089=");"; struct
_tagged_string _temp1090; _temp1090.curr= _temp1089; _temp1090.base= _temp1089;
_temp1090.last_plus_one= _temp1089 + 3; _temp1090;})); goto _LL926; _LL952: Cyc_Absyndump_dump(({
char* _temp1091="for("; struct _tagged_string _temp1092; _temp1092.curr=
_temp1091; _temp1092.base= _temp1091; _temp1092.last_plus_one= _temp1091 + 5;
_temp1092;})); Cyc_Absyndump_dumpexp( _temp1023); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1021); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp1015); Cyc_Absyndump_dump_nospace(({
char* _temp1093="){"; struct _tagged_string _temp1094; _temp1094.curr= _temp1093;
_temp1094.base= _temp1093; _temp1094.last_plus_one= _temp1093 + 3; _temp1094;}));
Cyc_Absyndump_dumpstmt( _temp1009); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL926; _LL954: Cyc_Absyndump_dump(({ char* _temp1095="switch("; struct
_tagged_string _temp1096; _temp1096.curr= _temp1095; _temp1096.base= _temp1095;
_temp1096.last_plus_one= _temp1095 + 8; _temp1096;})); Cyc_Absyndump_dumpexp(
_temp1027); Cyc_Absyndump_dump_nospace(({ char* _temp1097="){"; struct
_tagged_string _temp1098; _temp1098.curr= _temp1097; _temp1098.base= _temp1097;
_temp1098.last_plus_one= _temp1097 + 3; _temp1098;})); Cyc_Absyndump_dumpswitchclauses(
_temp1025); Cyc_Absyndump_dump_char(( int)'}'); goto _LL926; _LL956: Cyc_Absyndump_dumpdecl(
_temp1031); Cyc_Absyndump_dumpstmt( _temp1029); goto _LL926; _LL958: Cyc_Absyndump_dump(({
char* _temp1099="cut"; struct _tagged_string _temp1100; _temp1100.curr=
_temp1099; _temp1100.base= _temp1099; _temp1100.last_plus_one= _temp1099 + 4;
_temp1100;})); Cyc_Absyndump_dumpstmt( _temp1033); goto _LL926; _LL960: Cyc_Absyndump_dump(({
char* _temp1101="splice"; struct _tagged_string _temp1102; _temp1102.curr=
_temp1101; _temp1102.base= _temp1101; _temp1102.last_plus_one= _temp1101 + 7;
_temp1102;})); Cyc_Absyndump_dumpstmt( _temp1035); goto _LL926; _LL962: if( Cyc_Absynpp_is_declaration(
_temp1037)){ Cyc_Absyndump_dump_str( _temp1039); Cyc_Absyndump_dump_nospace(({
char* _temp1103=": {"; struct _tagged_string _temp1104; _temp1104.curr=
_temp1103; _temp1104.base= _temp1103; _temp1104.last_plus_one= _temp1103 + 4;
_temp1104;})); Cyc_Absyndump_dumpstmt( _temp1037); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1039); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1037);} goto _LL926; _LL964: Cyc_Absyndump_dump(({
char* _temp1105="do {"; struct _tagged_string _temp1106; _temp1106.curr=
_temp1105; _temp1106.base= _temp1105; _temp1106.last_plus_one= _temp1105 + 5;
_temp1106;})); Cyc_Absyndump_dumpstmt( _temp1047); Cyc_Absyndump_dump_nospace(({
char* _temp1107="} while ("; struct _tagged_string _temp1108; _temp1108.curr=
_temp1107; _temp1108.base= _temp1107; _temp1108.last_plus_one= _temp1107 + 10;
_temp1108;})); Cyc_Absyndump_dumpexp( _temp1045); Cyc_Absyndump_dump_nospace(({
char* _temp1109=");"; struct _tagged_string _temp1110; _temp1110.curr= _temp1109;
_temp1110.base= _temp1109; _temp1110.last_plus_one= _temp1109 + 3; _temp1110;}));
goto _LL926; _LL966: Cyc_Absyndump_dump(({ char* _temp1111="try"; struct
_tagged_string _temp1112; _temp1112.curr= _temp1111; _temp1112.base= _temp1111;
_temp1112.last_plus_one= _temp1111 + 4; _temp1112;})); Cyc_Absyndump_dumpstmt(
_temp1051); Cyc_Absyndump_dump(({ char* _temp1113="catch {"; struct
_tagged_string _temp1114; _temp1114.curr= _temp1113; _temp1114.base= _temp1113;
_temp1114.last_plus_one= _temp1113 + 8; _temp1114;})); Cyc_Absyndump_dumpswitchclauses(
_temp1049); Cyc_Absyndump_dump_char(( int)'}'); goto _LL926; _LL968: Cyc_Absyndump_dump(({
char* _temp1115="region<"; struct _tagged_string _temp1116; _temp1116.curr=
_temp1115; _temp1116.base= _temp1115; _temp1116.last_plus_one= _temp1115 + 8;
_temp1116;})); Cyc_Absyndump_dumptvar( _temp1057); Cyc_Absyndump_dump(({ char*
_temp1117="> "; struct _tagged_string _temp1118; _temp1118.curr= _temp1117;
_temp1118.base= _temp1117; _temp1118.last_plus_one= _temp1117 + 3; _temp1118;}));
Cyc_Absyndump_dumpqvar( _temp1055->name); Cyc_Absyndump_dump(({ char* _temp1119="{";
struct _tagged_string _temp1120; _temp1120.curr= _temp1119; _temp1120.base=
_temp1119; _temp1120.last_plus_one= _temp1119 + 2; _temp1120;})); Cyc_Absyndump_dumpstmt(
_temp1053); Cyc_Absyndump_dump(({ char* _temp1121="}"; struct _tagged_string
_temp1122; _temp1122.curr= _temp1121; _temp1122.base= _temp1121; _temp1122.last_plus_one=
_temp1121 + 2; _temp1122;})); goto _LL926; _LL926:;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,({ char* _temp1123=""; struct
_tagged_string _temp1124; _temp1124.curr= _temp1123; _temp1124.base= _temp1123;
_temp1124.last_plus_one= _temp1123 + 1; _temp1124;}),({ char* _temp1125="=";
struct _tagged_string _temp1126; _temp1126.curr= _temp1125; _temp1126.base=
_temp1125; _temp1126.last_plus_one= _temp1125 + 2; _temp1126;}),({ char*
_temp1127="="; struct _tagged_string _temp1128; _temp1128.curr= _temp1127;
_temp1128.base= _temp1127; _temp1128.last_plus_one= _temp1127 + 2; _temp1128;}));
Cyc_Absyndump_dumppat((* dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*
p){ void* _temp1129=( void*) p->r; int _temp1163; void* _temp1165; int _temp1167;
void* _temp1169; char _temp1171; struct _tagged_string _temp1173; struct Cyc_Absyn_Vardecl*
_temp1175; struct Cyc_List_List* _temp1177; struct Cyc_Absyn_Pat* _temp1179;
struct Cyc_Absyn_Vardecl* _temp1181; struct _tuple0* _temp1183; struct Cyc_List_List*
_temp1185; struct Cyc_List_List* _temp1187; struct _tuple0* _temp1189; struct
Cyc_List_List* _temp1191; struct Cyc_List_List* _temp1193; struct _tuple0*
_temp1195; struct Cyc_List_List* _temp1197; struct Cyc_List_List* _temp1199;
struct Cyc_Core_Opt* _temp1201; struct Cyc_Absyn_Structdecl* _temp1203; struct
Cyc_List_List* _temp1205; struct Cyc_List_List* _temp1207; struct Cyc_Absyn_Tunionfield*
_temp1209; struct Cyc_Absyn_Tuniondecl* _temp1211; struct Cyc_Absyn_Enumfield*
_temp1213; struct Cyc_Absyn_Enumdecl* _temp1215; _LL1131: if( _temp1129 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1132;} else{ goto _LL1133;} _LL1133: if( _temp1129 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1134;} else{ goto _LL1135;} _LL1135: if((
unsigned int) _temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Int_p: 0){
_LL1166: _temp1165=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1129)->f1;
if( _temp1165 ==( void*) Cyc_Absyn_Signed){ goto _LL1164;} else{ goto _LL1137;}
_LL1164: _temp1163=(( struct Cyc_Absyn_Int_p_struct*) _temp1129)->f2; goto
_LL1136;} else{ goto _LL1137;} _LL1137: if(( unsigned int) _temp1129 > 2u?*((
int*) _temp1129) == Cyc_Absyn_Int_p: 0){ _LL1170: _temp1169=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1129)->f1; if( _temp1169 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1168;} else{ goto _LL1139;} _LL1168: _temp1167=(( struct Cyc_Absyn_Int_p_struct*)
_temp1129)->f2; goto _LL1138;} else{ goto _LL1139;} _LL1139: if(( unsigned int)
_temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Char_p: 0){ _LL1172: _temp1171=((
struct Cyc_Absyn_Char_p_struct*) _temp1129)->f1; goto _LL1140;} else{ goto
_LL1141;} _LL1141: if(( unsigned int) _temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Float_p:
0){ _LL1174: _temp1173=(( struct Cyc_Absyn_Float_p_struct*) _temp1129)->f1; goto
_LL1142;} else{ goto _LL1143;} _LL1143: if(( unsigned int) _temp1129 > 2u?*((
int*) _temp1129) == Cyc_Absyn_Var_p: 0){ _LL1176: _temp1175=(( struct Cyc_Absyn_Var_p_struct*)
_temp1129)->f1; goto _LL1144;} else{ goto _LL1145;} _LL1145: if(( unsigned int)
_temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Tuple_p: 0){ _LL1178: _temp1177=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1129)->f1; goto _LL1146;} else{ goto
_LL1147;} _LL1147: if(( unsigned int) _temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Pointer_p:
0){ _LL1180: _temp1179=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1129)->f1;
goto _LL1148;} else{ goto _LL1149;} _LL1149: if(( unsigned int) _temp1129 > 2u?*((
int*) _temp1129) == Cyc_Absyn_Reference_p: 0){ _LL1182: _temp1181=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1129)->f1; goto _LL1150;} else{ goto _LL1151;} _LL1151: if(( unsigned int)
_temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_UnknownId_p: 0){ _LL1184:
_temp1183=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1129)->f1; goto _LL1152;}
else{ goto _LL1153;} _LL1153: if(( unsigned int) _temp1129 > 2u?*(( int*)
_temp1129) == Cyc_Absyn_UnknownCall_p: 0){ _LL1190: _temp1189=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1129)->f1; goto _LL1188; _LL1188: _temp1187=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1129)->f2; goto _LL1186; _LL1186: _temp1185=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1129)->f3; goto _LL1154;} else{ goto _LL1155;} _LL1155: if(( unsigned int)
_temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_UnknownFields_p: 0){ _LL1196:
_temp1195=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1129)->f1; goto
_LL1194; _LL1194: _temp1193=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1129)->f2; goto _LL1192; _LL1192: _temp1191=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1129)->f3; goto _LL1156;} else{ goto _LL1157;} _LL1157: if(( unsigned int)
_temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Struct_p: 0){ _LL1204:
_temp1203=(( struct Cyc_Absyn_Struct_p_struct*) _temp1129)->f1; goto _LL1202;
_LL1202: _temp1201=(( struct Cyc_Absyn_Struct_p_struct*) _temp1129)->f2; goto
_LL1200; _LL1200: _temp1199=(( struct Cyc_Absyn_Struct_p_struct*) _temp1129)->f3;
goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Struct_p_struct*) _temp1129)->f4;
goto _LL1158;} else{ goto _LL1159;} _LL1159: if(( unsigned int) _temp1129 > 2u?*((
int*) _temp1129) == Cyc_Absyn_Tunion_p: 0){ _LL1212: _temp1211=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1129)->f1; goto _LL1210; _LL1210: _temp1209=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1129)->f2; goto _LL1208; _LL1208: _temp1207=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1129)->f3; goto _LL1206; _LL1206: _temp1205=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1129)->f4; goto _LL1160;} else{ goto _LL1161;} _LL1161: if(( unsigned int)
_temp1129 > 2u?*(( int*) _temp1129) == Cyc_Absyn_Enum_p: 0){ _LL1216: _temp1215=((
struct Cyc_Absyn_Enum_p_struct*) _temp1129)->f1; goto _LL1214; _LL1214:
_temp1213=(( struct Cyc_Absyn_Enum_p_struct*) _temp1129)->f2; goto _LL1162;}
else{ goto _LL1130;} _LL1132: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1130;
_LL1134: Cyc_Absyndump_dump(({ char* _temp1217="null"; struct _tagged_string
_temp1218; _temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 5; _temp1218;})); goto _LL1130; _LL1136: Cyc_Absyndump_dump( xprintf("%d",
_temp1163)); goto _LL1130; _LL1138: Cyc_Absyndump_dump( xprintf("%u",(
unsigned int) _temp1167)); goto _LL1130; _LL1140: Cyc_Absyndump_dump(({ char*
_temp1219="'"; struct _tagged_string _temp1220; _temp1220.curr= _temp1219;
_temp1220.base= _temp1219; _temp1220.last_plus_one= _temp1219 + 2; _temp1220;}));
Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp1171)); Cyc_Absyndump_dump_nospace(({
char* _temp1221="'"; struct _tagged_string _temp1222; _temp1222.curr= _temp1221;
_temp1222.base= _temp1221; _temp1222.last_plus_one= _temp1221 + 2; _temp1222;}));
goto _LL1130; _LL1142: Cyc_Absyndump_dump( _temp1173); goto _LL1130; _LL1144:
Cyc_Absyndump_dumpqvar( _temp1175->name); goto _LL1130; _LL1146:(( void(*)( void(*
f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1177,({ char* _temp1223="$("; struct _tagged_string _temp1224; _temp1224.curr=
_temp1223; _temp1224.base= _temp1223; _temp1224.last_plus_one= _temp1223 + 3;
_temp1224;}),({ char* _temp1225=")"; struct _tagged_string _temp1226; _temp1226.curr=
_temp1225; _temp1226.base= _temp1225; _temp1226.last_plus_one= _temp1225 + 2;
_temp1226;}),({ char* _temp1227=","; struct _tagged_string _temp1228; _temp1228.curr=
_temp1227; _temp1228.base= _temp1227; _temp1228.last_plus_one= _temp1227 + 2;
_temp1228;})); goto _LL1130; _LL1148: Cyc_Absyndump_dump(({ char* _temp1229="&";
struct _tagged_string _temp1230; _temp1230.curr= _temp1229; _temp1230.base=
_temp1229; _temp1230.last_plus_one= _temp1229 + 2; _temp1230;})); Cyc_Absyndump_dumppat(
_temp1179); goto _LL1130; _LL1150: Cyc_Absyndump_dump(({ char* _temp1231="*";
struct _tagged_string _temp1232; _temp1232.curr= _temp1231; _temp1232.base=
_temp1231; _temp1232.last_plus_one= _temp1231 + 2; _temp1232;})); Cyc_Absyndump_dumpqvar(
_temp1181->name); goto _LL1130; _LL1152: Cyc_Absyndump_dumpqvar( _temp1183);
goto _LL1130; _LL1154: Cyc_Absyndump_dumpqvar( _temp1189); Cyc_Absyndump_dumptvars(
_temp1187);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1185,({ char* _temp1233="(";
struct _tagged_string _temp1234; _temp1234.curr= _temp1233; _temp1234.base=
_temp1233; _temp1234.last_plus_one= _temp1233 + 2; _temp1234;}),({ char*
_temp1235=")"; struct _tagged_string _temp1236; _temp1236.curr= _temp1235;
_temp1236.base= _temp1235; _temp1236.last_plus_one= _temp1235 + 2; _temp1236;}),({
char* _temp1237=","; struct _tagged_string _temp1238; _temp1238.curr= _temp1237;
_temp1238.base= _temp1237; _temp1238.last_plus_one= _temp1237 + 2; _temp1238;}));
goto _LL1130; _LL1156: Cyc_Absyndump_dumpqvar( _temp1195); Cyc_Absyndump_dumptvars(
_temp1193);(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1191,({ char* _temp1239="{";
struct _tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base=
_temp1239; _temp1240.last_plus_one= _temp1239 + 2; _temp1240;}),({ char*
_temp1241="}"; struct _tagged_string _temp1242; _temp1242.curr= _temp1241;
_temp1242.base= _temp1241; _temp1242.last_plus_one= _temp1241 + 2; _temp1242;}),({
char* _temp1243=","; struct _tagged_string _temp1244; _temp1244.curr= _temp1243;
_temp1244.base= _temp1243; _temp1244.last_plus_one= _temp1243 + 2; _temp1244;}));
goto _LL1130; _LL1158: if( _temp1203->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1203->name))->v);}
Cyc_Absyndump_dumptvars( _temp1199);(( void(*)( void(* f)( struct _tuple8*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1197,({ char* _temp1245="{"; struct _tagged_string _temp1246; _temp1246.curr=
_temp1245; _temp1246.base= _temp1245; _temp1246.last_plus_one= _temp1245 + 2;
_temp1246;}),({ char* _temp1247="}"; struct _tagged_string _temp1248; _temp1248.curr=
_temp1247; _temp1248.base= _temp1247; _temp1248.last_plus_one= _temp1247 + 2;
_temp1248;}),({ char* _temp1249=","; struct _tagged_string _temp1250; _temp1250.curr=
_temp1249; _temp1250.base= _temp1249; _temp1250.last_plus_one= _temp1249 + 2;
_temp1250;})); goto _LL1130; _LL1160: Cyc_Absyndump_dumpqvar( _temp1209->name);
Cyc_Absyndump_dumptvars( _temp1207); if( _temp1205 != 0){(( void(*)( void(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1205,({ char* _temp1251="("; struct _tagged_string _temp1252; _temp1252.curr=
_temp1251; _temp1252.base= _temp1251; _temp1252.last_plus_one= _temp1251 + 2;
_temp1252;}),({ char* _temp1253=")"; struct _tagged_string _temp1254; _temp1254.curr=
_temp1253; _temp1254.base= _temp1253; _temp1254.last_plus_one= _temp1253 + 2;
_temp1254;}),({ char* _temp1255=","; struct _tagged_string _temp1256; _temp1256.curr=
_temp1255; _temp1256.base= _temp1255; _temp1256.last_plus_one= _temp1255 + 2;
_temp1256;}));} goto _LL1130; _LL1162: Cyc_Absyndump_dumpqvar( _temp1213->name);
goto _LL1130; _LL1130:;} void Cyc_Absyndump_dumptunionfield( struct Cyc_Absyn_Tunionfield*
ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs != 0){ Cyc_Absyndump_dumpargs(
ef->typs);}} void Cyc_Absyndump_dumptunionfields( struct Cyc_List_List* fields){((
void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumptunionfield,
fields,({ char* _temp1257=","; struct _tagged_string _temp1258; _temp1258.curr=
_temp1257; _temp1258.base= _temp1257; _temp1258.last_plus_one= _temp1257 + 2;
_temp1258;}));} void Cyc_Absyndump_dumpenumfield( struct Cyc_Absyn_Enumfield* ef){
Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag != 0){ Cyc_Absyndump_dump(({ char*
_temp1259=" = "; struct _tagged_string _temp1260; _temp1260.curr= _temp1259;
_temp1260.base= _temp1259; _temp1260.last_plus_one= _temp1259 + 4; _temp1260;}));
Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void
Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)( void(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield, fields,({ char* _temp1261=",";
struct _tagged_string _temp1262; _temp1262.curr= _temp1261; _temp1262.base=
_temp1261; _temp1262.last_plus_one= _temp1261 + 2; _temp1262;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct Cyc_Absyn_Structfield _temp1265; struct Cyc_List_List*
_temp1266; struct Cyc_Core_Opt* _temp1268; void* _temp1270; struct Cyc_Absyn_Tqual
_temp1272; struct _tagged_string* _temp1274; struct Cyc_Absyn_Structfield*
_temp1263=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1265=* _temp1263; _LL1275: _temp1274= _temp1265.name; goto
_LL1273; _LL1273: _temp1272= _temp1265.tq; goto _LL1271; _LL1271: _temp1270=(
void*) _temp1265.type; goto _LL1269; _LL1269: _temp1268= _temp1265.width; goto
_LL1267; _LL1267: _temp1266= _temp1265.attributes; goto _LL1264; _LL1264:(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1272, _temp1270, Cyc_Absyndump_dump_str,
_temp1274); Cyc_Absyndump_dumpatts( _temp1266); if( _temp1268 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1268))->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
static void Cyc_Absyndump_dump_fn_qvar( struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char((
int)',');}}} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp1276=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp1302; struct Cyc_Absyn_Structdecl*
_temp1304; struct Cyc_Absyn_Uniondecl* _temp1306; struct Cyc_Absyn_Vardecl*
_temp1308; struct Cyc_Absyn_Vardecl _temp1310; struct Cyc_List_List* _temp1311;
struct Cyc_Core_Opt* _temp1313; struct Cyc_Absyn_Exp* _temp1315; void* _temp1317;
struct Cyc_Absyn_Tqual _temp1319; struct _tuple0* _temp1321; void* _temp1323;
struct Cyc_Absyn_Tuniondecl* _temp1325; struct Cyc_Absyn_Tuniondecl _temp1327;
int _temp1328; struct Cyc_Core_Opt* _temp1330; struct Cyc_List_List* _temp1332;
struct _tuple0* _temp1334; void* _temp1336; struct Cyc_Absyn_Enumdecl* _temp1338;
struct Cyc_Absyn_Enumdecl _temp1340; struct Cyc_Core_Opt* _temp1341; struct
_tuple0* _temp1343; void* _temp1345; int _temp1347; struct Cyc_Absyn_Exp*
_temp1349; struct Cyc_Core_Opt* _temp1351; struct Cyc_Core_Opt* _temp1353;
struct Cyc_Absyn_Pat* _temp1355; struct Cyc_List_List* _temp1357; struct Cyc_Absyn_Typedefdecl*
_temp1359; struct Cyc_List_List* _temp1361; struct _tagged_string* _temp1363;
struct Cyc_List_List* _temp1365; struct _tuple0* _temp1367; struct Cyc_List_List*
_temp1369; _LL1278: if(*(( int*) _temp1276) == Cyc_Absyn_Fn_d){ _LL1303:
_temp1302=(( struct Cyc_Absyn_Fn_d_struct*) _temp1276)->f1; goto _LL1279;} else{
goto _LL1280;} _LL1280: if(*(( int*) _temp1276) == Cyc_Absyn_Struct_d){ _LL1305:
_temp1304=(( struct Cyc_Absyn_Struct_d_struct*) _temp1276)->f1; goto _LL1281;}
else{ goto _LL1282;} _LL1282: if(*(( int*) _temp1276) == Cyc_Absyn_Union_d){
_LL1307: _temp1306=(( struct Cyc_Absyn_Union_d_struct*) _temp1276)->f1; goto
_LL1283;} else{ goto _LL1284;} _LL1284: if(*(( int*) _temp1276) == Cyc_Absyn_Var_d){
_LL1309: _temp1308=(( struct Cyc_Absyn_Var_d_struct*) _temp1276)->f1; _temp1310=*
_temp1308; _LL1324: _temp1323=( void*) _temp1310.sc; goto _LL1322; _LL1322:
_temp1321= _temp1310.name; goto _LL1320; _LL1320: _temp1319= _temp1310.tq; goto
_LL1318; _LL1318: _temp1317=( void*) _temp1310.type; goto _LL1316; _LL1316:
_temp1315= _temp1310.initializer; goto _LL1314; _LL1314: _temp1313= _temp1310.rgn;
goto _LL1312; _LL1312: _temp1311= _temp1310.attributes; goto _LL1285;} else{
goto _LL1286;} _LL1286: if(*(( int*) _temp1276) == Cyc_Absyn_Tunion_d){ _LL1326:
_temp1325=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1276)->f1; _temp1327=*
_temp1325; _LL1337: _temp1336=( void*) _temp1327.sc; goto _LL1335; _LL1335:
_temp1334= _temp1327.name; goto _LL1333; _LL1333: _temp1332= _temp1327.tvs; goto
_LL1331; _LL1331: _temp1330= _temp1327.fields; goto _LL1329; _LL1329: _temp1328=
_temp1327.is_xtunion; goto _LL1287;} else{ goto _LL1288;} _LL1288: if(*(( int*)
_temp1276) == Cyc_Absyn_Enum_d){ _LL1339: _temp1338=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1276)->f1; _temp1340=* _temp1338; _LL1346: _temp1345=( void*) _temp1340.sc;
goto _LL1344; _LL1344: _temp1343= _temp1340.name; goto _LL1342; _LL1342:
_temp1341= _temp1340.fields; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(*((
int*) _temp1276) == Cyc_Absyn_Let_d){ _LL1356: _temp1355=(( struct Cyc_Absyn_Let_d_struct*)
_temp1276)->f1; goto _LL1354; _LL1354: _temp1353=(( struct Cyc_Absyn_Let_d_struct*)
_temp1276)->f2; goto _LL1352; _LL1352: _temp1351=(( struct Cyc_Absyn_Let_d_struct*)
_temp1276)->f3; goto _LL1350; _LL1350: _temp1349=(( struct Cyc_Absyn_Let_d_struct*)
_temp1276)->f4; goto _LL1348; _LL1348: _temp1347=(( struct Cyc_Absyn_Let_d_struct*)
_temp1276)->f5; goto _LL1291;} else{ goto _LL1292;} _LL1292: if(*(( int*)
_temp1276) == Cyc_Absyn_Letv_d){ _LL1358: _temp1357=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1276)->f1; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(*(( int*)
_temp1276) == Cyc_Absyn_Typedef_d){ _LL1360: _temp1359=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1276)->f1; goto _LL1295;} else{ goto _LL1296;} _LL1296: if(*(( int*)
_temp1276) == Cyc_Absyn_Namespace_d){ _LL1364: _temp1363=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1276)->f1; goto _LL1362; _LL1362: _temp1361=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1276)->f2; goto _LL1297;} else{ goto _LL1298;} _LL1298: if(*(( int*)
_temp1276) == Cyc_Absyn_Using_d){ _LL1368: _temp1367=(( struct Cyc_Absyn_Using_d_struct*)
_temp1276)->f1; goto _LL1366; _LL1366: _temp1365=(( struct Cyc_Absyn_Using_d_struct*)
_temp1276)->f2; goto _LL1299;} else{ goto _LL1300;} _LL1300: if(*(( int*)
_temp1276) == Cyc_Absyn_ExternC_d){ _LL1370: _temp1369=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1276)->f1; goto _LL1301;} else{ goto _LL1277;} _LL1279: if( Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1302->attributes);} if( _temp1302->is_inline){ if(
Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump(({ char* _temp1371="__inline"; struct
_tagged_string _temp1372; _temp1372.curr= _temp1371; _temp1372.base= _temp1371;
_temp1372.last_plus_one= _temp1371 + 9; _temp1372;}));} else{ Cyc_Absyndump_dump(({
char* _temp1373="inline"; struct _tagged_string _temp1374; _temp1374.curr=
_temp1373; _temp1374.base= _temp1373; _temp1374.last_plus_one= _temp1373 + 7;
_temp1374;}));}} Cyc_Absyndump_dumpscope(( void*) _temp1302->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1376=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1376[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1377; _temp1377.tag= Cyc_Absyn_FnType; _temp1377.f1=({ struct Cyc_Absyn_FnInfo
_temp1378; _temp1378.tvars= _temp1302->tvs; _temp1378.effect= _temp1302->effect;
_temp1378.ret_typ=( void*)(( void*) _temp1302->ret_type); _temp1378.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1302->args); _temp1378.varargs=
_temp1302->varargs; _temp1378.attributes= 0; _temp1378;}); _temp1377;});
_temp1376;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1375; _temp1375.q_const= 0; _temp1375.q_volatile= 0; _temp1375.q_restrict=
0; _temp1375;}), t, Cyc_Absynpp_to_VC? Cyc_Absyndump_dump_fn_qvar: Cyc_Absyndump_dump_atts_qvar,
_temp1302); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1302->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1277;} _LL1281:
Cyc_Absyndump_dumpscope(( void*) _temp1304->sc); Cyc_Absyndump_dump(({ char*
_temp1379="struct"; struct _tagged_string _temp1380; _temp1380.curr= _temp1379;
_temp1380.base= _temp1379; _temp1380.last_plus_one= _temp1379 + 7; _temp1380;}));
if( _temp1304->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1304->name))->v);} Cyc_Absyndump_dumptvars(
_temp1304->tvs); if( _temp1304->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1304->fields))->v); Cyc_Absyndump_dump(({
char* _temp1381="}"; struct _tagged_string _temp1382; _temp1382.curr= _temp1381;
_temp1382.base= _temp1381; _temp1382.last_plus_one= _temp1381 + 2; _temp1382;}));
Cyc_Absyndump_dumpatts( _temp1304->attributes); Cyc_Absyndump_dump(({ char*
_temp1383=";"; struct _tagged_string _temp1384; _temp1384.curr= _temp1383;
_temp1384.base= _temp1383; _temp1384.last_plus_one= _temp1383 + 2; _temp1384;}));}
goto _LL1277; _LL1283: Cyc_Absyndump_dumpscope(( void*) _temp1306->sc); Cyc_Absyndump_dump(({
char* _temp1385="union"; struct _tagged_string _temp1386; _temp1386.curr=
_temp1385; _temp1386.base= _temp1385; _temp1386.last_plus_one= _temp1385 + 6;
_temp1386;})); if( _temp1306->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1306->name))->v);} Cyc_Absyndump_dumptvars(
_temp1306->tvs); if( _temp1306->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1306->fields))->v); Cyc_Absyndump_dump(({
char* _temp1387="}"; struct _tagged_string _temp1388; _temp1388.curr= _temp1387;
_temp1388.base= _temp1387; _temp1388.last_plus_one= _temp1387 + 2; _temp1388;}));
Cyc_Absyndump_dumpatts( _temp1306->attributes); Cyc_Absyndump_dump(({ char*
_temp1389=";"; struct _tagged_string _temp1390; _temp1390.curr= _temp1389;
_temp1390.base= _temp1389; _temp1390.last_plus_one= _temp1389 + 2; _temp1390;}));}
goto _LL1277; _LL1285: if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dumpatts( _temp1311);}
Cyc_Absyndump_dumpscope( _temp1323);(( void(*)( struct Cyc_Absyn_Tqual, void*,
void(* f)( struct _tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)(
_temp1319, _temp1317, Cyc_Absyndump_dumpqvar, _temp1321); if( ! Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1311);} if( _temp1315 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1315));}
Cyc_Absyndump_dump_semi(); goto _LL1277; _LL1287: Cyc_Absyndump_dumpscope(
_temp1336); if( _temp1328){ Cyc_Absyndump_dump(({ char* _temp1391="xtunion ";
struct _tagged_string _temp1392; _temp1392.curr= _temp1391; _temp1392.base=
_temp1391; _temp1392.last_plus_one= _temp1391 + 9; _temp1392;}));} else{ Cyc_Absyndump_dump(({
char* _temp1393="tunion "; struct _tagged_string _temp1394; _temp1394.curr=
_temp1393; _temp1394.base= _temp1393; _temp1394.last_plus_one= _temp1393 + 8;
_temp1394;}));} Cyc_Absyndump_dumpqvar( _temp1334); Cyc_Absyndump_dumptvars(
_temp1332); if( _temp1330 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1330))->v); Cyc_Absyndump_dump_nospace(({ char* _temp1395="};";
struct _tagged_string _temp1396; _temp1396.curr= _temp1395; _temp1396.base=
_temp1395; _temp1396.last_plus_one= _temp1395 + 3; _temp1396;}));} goto _LL1277;
_LL1289: Cyc_Absyndump_dumpscope( _temp1345); Cyc_Absyndump_dump(({ char*
_temp1397="enum "; struct _tagged_string _temp1398; _temp1398.curr= _temp1397;
_temp1398.base= _temp1397; _temp1398.last_plus_one= _temp1397 + 6; _temp1398;}));
Cyc_Absyndump_dumpqvar( _temp1343); if( _temp1341 == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1341))->v); Cyc_Absyndump_dump_nospace(({
char* _temp1399="};"; struct _tagged_string _temp1400; _temp1400.curr= _temp1399;
_temp1400.base= _temp1399; _temp1400.last_plus_one= _temp1399 + 3; _temp1400;}));}
return; _LL1291: Cyc_Absyndump_dump(({ char* _temp1401="let"; struct
_tagged_string _temp1402; _temp1402.curr= _temp1401; _temp1402.base= _temp1401;
_temp1402.last_plus_one= _temp1401 + 4; _temp1402;})); Cyc_Absyndump_dumppat(
_temp1355); Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp( _temp1349);
Cyc_Absyndump_dump_semi(); goto _LL1277; _LL1293: Cyc_Absyndump_dump(({ char*
_temp1403="let "; struct _tagged_string _temp1404; _temp1404.curr= _temp1403;
_temp1404.base= _temp1403; _temp1404.last_plus_one= _temp1403 + 5; _temp1404;}));
Cyc_Absyndump_dumpids( _temp1357); Cyc_Absyndump_dump_semi(); goto _LL1277;
_LL1295: Cyc_Absyndump_dump(({ char* _temp1405="typedef"; struct _tagged_string
_temp1406; _temp1406.curr= _temp1405; _temp1406.base= _temp1405; _temp1406.last_plus_one=
_temp1405 + 8; _temp1406;}));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1407; _temp1407.q_const= 0; _temp1407.q_volatile= 0;
_temp1407.q_restrict= 0; _temp1407;}),( void*) _temp1359->defn, Cyc_Absyndump_dumptypedefname,
_temp1359); Cyc_Absyndump_dump_semi(); goto _LL1277; _LL1297: Cyc_Absyndump_dump(({
char* _temp1408="namespace"; struct _tagged_string _temp1409; _temp1409.curr=
_temp1408; _temp1409.base= _temp1408; _temp1409.last_plus_one= _temp1408 + 10;
_temp1409;})); Cyc_Absyndump_dump_str( _temp1363); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1361 != 0; _temp1361=(( struct Cyc_List_List*) _check_null(
_temp1361))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1361))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1277;
_LL1299: Cyc_Absyndump_dump(({ char* _temp1410="using"; struct _tagged_string
_temp1411; _temp1411.curr= _temp1410; _temp1411.base= _temp1410; _temp1411.last_plus_one=
_temp1410 + 6; _temp1411;})); Cyc_Absyndump_dumpqvar( _temp1367); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1365 != 0; _temp1365=(( struct Cyc_List_List*)
_check_null( _temp1365))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1365))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1277; _LL1301: Cyc_Absyndump_dump(({ char* _temp1412="extern \"C\" {";
struct _tagged_string _temp1413; _temp1413.curr= _temp1412; _temp1413.base=
_temp1412; _temp1413.last_plus_one= _temp1412 + 13; _temp1413;})); for( 0;
_temp1369 != 0; _temp1369=(( struct Cyc_List_List*) _check_null( _temp1369))->tl){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1369))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1277;
_LL1277:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i != 1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms == 0){ f( a); return;}{ void* _temp1414=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1432; void* _temp1434; void*
_temp1436; struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Tqual _temp1440;
void* _temp1442; void* _temp1444; struct Cyc_Absyn_Exp* _temp1446; struct Cyc_Absyn_Tqual
_temp1448; void* _temp1450; void* _temp1452; struct Cyc_Absyn_Tqual _temp1454;
void* _temp1456; struct Cyc_Absyn_Tvar* _temp1458; void* _temp1460; struct Cyc_Absyn_Exp*
_temp1462; struct Cyc_Absyn_Tqual _temp1464; void* _temp1466; struct Cyc_Absyn_Tvar*
_temp1468; void* _temp1470; struct Cyc_Absyn_Exp* _temp1472; struct Cyc_Absyn_Tqual
_temp1474; void* _temp1476; struct Cyc_Absyn_Tvar* _temp1478; void* _temp1480;
struct Cyc_Absyn_Tqual _temp1482; void* _temp1484; void* _temp1486; _LL1416: if((
unsigned int) _temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){
_LL1437: _temp1436=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1;
if(( unsigned int) _temp1436 > 1u?*(( int*) _temp1436) == Cyc_Absyn_Nullable_ps:
0){ _LL1439: _temp1438=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1436)->f1;
goto _LL1435;} else{ goto _LL1418;} _LL1435: _temp1434=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; if( _temp1434 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1433;}
else{ goto _LL1418;} _LL1433: _temp1432=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1417;} else{ goto _LL1418;} _LL1418: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){ _LL1445:
_temp1444=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1; if((
unsigned int) _temp1444 > 1u?*(( int*) _temp1444) == Cyc_Absyn_NonNullable_ps: 0){
_LL1447: _temp1446=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1444)->f1;
goto _LL1443;} else{ goto _LL1420;} _LL1443: _temp1442=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; if( _temp1442 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1441;}
else{ goto _LL1420;} _LL1441: _temp1440=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1419;} else{ goto _LL1420;} _LL1420: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){ _LL1453:
_temp1452=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1; if(
_temp1452 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1451;} else{ goto
_LL1422;} _LL1451: _temp1450=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; if( _temp1450 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1449;}
else{ goto _LL1422;} _LL1449: _temp1448=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1421;} else{ goto _LL1422;} _LL1422: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){ _LL1461:
_temp1460=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1; if((
unsigned int) _temp1460 > 1u?*(( int*) _temp1460) == Cyc_Absyn_Nullable_ps: 0){
_LL1463: _temp1462=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1460)->f1; goto
_LL1457;} else{ goto _LL1424;} _LL1457: _temp1456=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; if(( unsigned int) _temp1456 > 4u?*(( int*) _temp1456) == Cyc_Absyn_VarType:
0){ _LL1459: _temp1458=(( struct Cyc_Absyn_VarType_struct*) _temp1456)->f1; goto
_LL1455;} else{ goto _LL1424;} _LL1455: _temp1454=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1423;} else{ goto _LL1424;} _LL1424: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){ _LL1471:
_temp1470=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1; if((
unsigned int) _temp1470 > 1u?*(( int*) _temp1470) == Cyc_Absyn_NonNullable_ps: 0){
_LL1473: _temp1472=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1470)->f1;
goto _LL1467;} else{ goto _LL1426;} _LL1467: _temp1466=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; if(( unsigned int) _temp1466 > 4u?*(( int*) _temp1466) == Cyc_Absyn_VarType:
0){ _LL1469: _temp1468=(( struct Cyc_Absyn_VarType_struct*) _temp1466)->f1; goto
_LL1465;} else{ goto _LL1426;} _LL1465: _temp1464=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){ _LL1481:
_temp1480=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1; if(
_temp1480 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1477;} else{ goto
_LL1428;} _LL1477: _temp1476=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; if(( unsigned int) _temp1476 > 4u?*(( int*) _temp1476) == Cyc_Absyn_VarType:
0){ _LL1479: _temp1478=(( struct Cyc_Absyn_VarType_struct*) _temp1476)->f1; goto
_LL1475;} else{ goto _LL1428;} _LL1475: _temp1474=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1427;} else{ goto _LL1428;} _LL1428: if(( unsigned int)
_temp1414 > 1u?*(( int*) _temp1414) == Cyc_Absyn_Pointer_mod: 0){ _LL1487:
_temp1486=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1414)->f1; goto
_LL1485; _LL1485: _temp1484=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f2; goto _LL1483; _LL1483: _temp1482=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1414)->f3; goto _LL1429;} else{ goto _LL1430;} _LL1430: goto _LL1431;
_LL1417: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1438);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1419: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1446);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1421: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1423: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1462); Cyc_Absyndump_dump_str( _temp1458->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1425: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1472); Cyc_Absyndump_dump_str(
_temp1468->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1427: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1478->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1429:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1488=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1488[ 0]=({ struct Cyc_Core_Impossible_struct _temp1489; _temp1489.tag= Cyc_Core_Impossible;
_temp1489.f1=({ char* _temp1490="dumptms: bad Pointer_mod"; struct
_tagged_string _temp1491; _temp1491.curr= _temp1490; _temp1491.base= _temp1490;
_temp1491.last_plus_one= _temp1490 + 25; _temp1491;}); _temp1489;}); _temp1488;}));
_LL1431: { int next_is_pointer= 0; if((( struct Cyc_List_List*) _check_null( tms))->tl
!= 0){ void* _temp1492=( void*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual _temp1498;
void* _temp1500; void* _temp1502; _LL1494: if(( unsigned int) _temp1492 > 1u?*((
int*) _temp1492) == Cyc_Absyn_Pointer_mod: 0){ _LL1503: _temp1502=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1492)->f1; goto _LL1501; _LL1501:
_temp1500=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1492)->f2; goto
_LL1499; _LL1499: _temp1498=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1492)->f3;
goto _LL1495;} else{ goto _LL1496;} _LL1496: goto _LL1497; _LL1495:
next_is_pointer= 1; goto _LL1493; _LL1497: goto _LL1493; _LL1493:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1504=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1520; void* _temp1522; struct Cyc_Core_Opt* _temp1524;
int _temp1526; struct Cyc_List_List* _temp1528; void* _temp1530; struct Cyc_Position_Segment*
_temp1532; struct Cyc_List_List* _temp1534; int _temp1536; struct Cyc_Position_Segment*
_temp1538; struct Cyc_List_List* _temp1540; struct Cyc_List_List* _temp1542;
struct Cyc_Position_Segment* _temp1544; struct Cyc_Absyn_Tqual _temp1546; void*
_temp1548; void* _temp1550; _LL1506: if( _temp1504 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL1507;} else{ goto _LL1508;} _LL1508: if(( unsigned int) _temp1504 > 1u?*((
int*) _temp1504) == Cyc_Absyn_ConstArray_mod: 0){ _LL1521: _temp1520=(( struct
Cyc_Absyn_ConstArray_mod_struct*) _temp1504)->f1; goto _LL1509;} else{ goto
_LL1510;} _LL1510: if(( unsigned int) _temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_Function_mod:
0){ _LL1523: _temp1522=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1504)->f1; if(*(( int*) _temp1522) == Cyc_Absyn_WithTypes){ _LL1529:
_temp1528=(( struct Cyc_Absyn_WithTypes_struct*) _temp1522)->f1; goto _LL1527;
_LL1527: _temp1526=(( struct Cyc_Absyn_WithTypes_struct*) _temp1522)->f2; goto
_LL1525; _LL1525: _temp1524=(( struct Cyc_Absyn_WithTypes_struct*) _temp1522)->f3;
goto _LL1511;} else{ goto _LL1512;}} else{ goto _LL1512;} _LL1512: if((
unsigned int) _temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_Function_mod: 0){
_LL1531: _temp1530=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1504)->f1;
if(*(( int*) _temp1530) == Cyc_Absyn_NoTypes){ _LL1535: _temp1534=(( struct Cyc_Absyn_NoTypes_struct*)
_temp1530)->f1; goto _LL1533; _LL1533: _temp1532=(( struct Cyc_Absyn_NoTypes_struct*)
_temp1530)->f2; goto _LL1513;} else{ goto _LL1514;}} else{ goto _LL1514;}
_LL1514: if(( unsigned int) _temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_TypeParams_mod:
0){ _LL1541: _temp1540=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1504)->f1;
goto _LL1539; _LL1539: _temp1538=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1504)->f2; goto _LL1537; _LL1537: _temp1536=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1504)->f3; goto _LL1515;} else{ goto _LL1516;} _LL1516: if(( unsigned int)
_temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_Attributes_mod: 0){ _LL1545:
_temp1544=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1504)->f1; goto
_LL1543; _LL1543: _temp1542=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1504)->f2; goto _LL1517;} else{ goto _LL1518;} _LL1518: if(( unsigned int)
_temp1504 > 1u?*(( int*) _temp1504) == Cyc_Absyn_Pointer_mod: 0){ _LL1551:
_temp1550=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1504)->f1; goto
_LL1549; _LL1549: _temp1548=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1504)->f2; goto _LL1547; _LL1547: _temp1546=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1504)->f3; goto _LL1519;} else{ goto _LL1505;} _LL1507: Cyc_Absyndump_dump(({
char* _temp1552="[]"; struct _tagged_string _temp1553; _temp1553.curr= _temp1552;
_temp1553.base= _temp1552; _temp1553.last_plus_one= _temp1552 + 3; _temp1553;}));
goto _LL1505; _LL1509: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1520); Cyc_Absyndump_dump_char(( int)']'); goto _LL1505; _LL1511: Cyc_Absyndump_dumpfunargs(
_temp1528, _temp1526, _temp1524); goto _LL1505; _LL1513:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1534,({ char* _temp1554="("; struct _tagged_string _temp1555; _temp1555.curr=
_temp1554; _temp1555.base= _temp1554; _temp1555.last_plus_one= _temp1554 + 2;
_temp1555;}),({ char* _temp1556=")"; struct _tagged_string _temp1557; _temp1557.curr=
_temp1556; _temp1557.base= _temp1556; _temp1557.last_plus_one= _temp1556 + 2;
_temp1557;}),({ char* _temp1558=","; struct _tagged_string _temp1559; _temp1559.curr=
_temp1558; _temp1559.base= _temp1558; _temp1559.last_plus_one= _temp1558 + 2;
_temp1559;})); goto _LL1505; _LL1515: if( _temp1536){ Cyc_Absyndump_dumpkindedtvars(
_temp1540);} else{ Cyc_Absyndump_dumptvars( _temp1540);} goto _LL1505; _LL1517:
Cyc_Absyndump_dumpatts( _temp1542); goto _LL1505; _LL1519:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1560=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1560[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1561; _temp1561.tag= Cyc_Core_Impossible;
_temp1561.f1=({ char* _temp1562="dumptms"; struct _tagged_string _temp1563;
_temp1563.curr= _temp1562; _temp1563.base= _temp1562; _temp1563.last_plus_one=
_temp1562 + 8; _temp1563;}); _temp1561;}); _temp1560;})); _LL1505:;} return;}
_LL1415:;}} void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual tq, void* t,
void(* f)( void*), void* a){ struct Cyc_List_List* _temp1566; void* _temp1568;
struct Cyc_Absyn_Tqual _temp1570; struct _tuple4 _temp1564= Cyc_Absynpp_to_tms(
tq, t); _LL1571: _temp1570= _temp1564.f1; goto _LL1569; _LL1569: _temp1568=
_temp1564.f2; goto _LL1567; _LL1567: _temp1566= _temp1564.f3; goto _LL1565;
_LL1565: Cyc_Absyndump_dumptq( _temp1570); Cyc_Absyndump_dumpntyp( _temp1568);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1566),
f, a);} void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct
Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0;
tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}}