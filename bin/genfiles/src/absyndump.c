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
12u]; extern void* Cyc_Tcutil_compress( void* t); void Cyc_Absyndump_dumptyp(
void*); void Cyc_Absyndump_dumpntyp( void* t); void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*); void Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*); void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*); void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void*
a); void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual, void*, void(* f)( void*),
void*); void Cyc_Absyndump_dumpstructfields( struct Cyc_List_List* fields);
typedef struct _tagged_string Cyc_Absyndump_dump_string_t; struct Cyc_Stdio___sFILE**
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
_temp116 + 2; _temp117;}));} void Cyc_Absyndump_dump_callconv( struct Cyc_List_List*
atts){ for( 0; atts != 0; atts=(( struct Cyc_List_List*) _check_null( atts))->tl){
void* _temp118=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL120:
if( _temp118 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL121;} else{ goto _LL122;}
_LL122: if( _temp118 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL123;} else{ goto
_LL124;} _LL124: if( _temp118 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL125;}
else{ goto _LL126;} _LL126: goto _LL127; _LL121: Cyc_Absyndump_dump(({ char*
_temp128="_stdcall"; struct _tagged_string _temp129; _temp129.curr= _temp128;
_temp129.base= _temp128; _temp129.last_plus_one= _temp128 + 9; _temp129;}));
return; _LL123: Cyc_Absyndump_dump(({ char* _temp130="_cdecl"; struct
_tagged_string _temp131; _temp131.curr= _temp130; _temp131.base= _temp130;
_temp131.last_plus_one= _temp130 + 7; _temp131;})); return; _LL125: Cyc_Absyndump_dump(({
char* _temp132="_fastcall"; struct _tagged_string _temp133; _temp133.curr=
_temp132; _temp133.base= _temp132; _temp133.last_plus_one= _temp132 + 10;
_temp133;})); return; _LL127: goto _LL119; _LL119:;}} void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List* atts){ int hasatt= 0;{ struct Cyc_List_List* atts2= atts;
for( 0; atts2 != 0; atts2=(( struct Cyc_List_List*) _check_null( atts2))->tl){
void* _temp134=( void*)(( struct Cyc_List_List*) _check_null( atts))->hd; _LL136:
if( _temp134 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL137;} else{ goto _LL138;}
_LL138: if( _temp134 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL139;} else{ goto
_LL140;} _LL140: if( _temp134 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL141;}
else{ goto _LL142;} _LL142: goto _LL143; _LL137: goto _LL135; _LL139: goto
_LL135; _LL141: goto _LL135; _LL143: hasatt= 1; goto _LL135; _LL135:;}} if( !
hasatt){ return;} Cyc_Absyndump_dump(({ char* _temp144="__declspec("; struct
_tagged_string _temp145; _temp145.curr= _temp144; _temp145.base= _temp144;
_temp145.last_plus_one= _temp144 + 12; _temp145;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ void* _temp146=( void*)((
struct Cyc_List_List*) _check_null( atts))->hd; _LL148: if( _temp146 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL149;} else{ goto _LL150;} _LL150: if( _temp146
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL151;} else{ goto _LL152;} _LL152: if(
_temp146 ==( void*) Cyc_Absyn_Fastcall_att){ goto _LL153;} else{ goto _LL154;}
_LL154: goto _LL155; _LL149: goto _LL147; _LL151: goto _LL147; _LL153: goto
_LL147; _LL155: Cyc_Absyndump_dump( Cyc_Absyn_attribute2string(( void*)(( struct
Cyc_List_List*) _check_null( atts))->hd)); goto _LL147; _LL147:;} Cyc_Absyndump_dump_char((
int)')');} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if( atts ==
0){ return;} if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump_noncallconv( atts);
return;} Cyc_Absyndump_dump(({ char* _temp156=" __attribute__(("; struct
_tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 17; _temp157;})); for( 0; atts != 0; atts=((
struct Cyc_List_List*) _check_null( atts))->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*)(( struct Cyc_List_List*) _check_null( atts))->hd)); if((( struct Cyc_List_List*)
_check_null( atts))->tl != 0){ Cyc_Absyndump_dump(({ char* _temp158=","; struct
_tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 2; _temp159;}));}} Cyc_Absyndump_dump(({ char*
_temp160=")) "; struct _tagged_string _temp161; _temp161.curr= _temp160;
_temp161.base= _temp160; _temp161.last_plus_one= _temp160 + 4; _temp161;}));}
int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List* tms){ if( tms == 0){
return 0;}{ void* _temp162=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Tqual _temp168; void* _temp170; void* _temp172; _LL164: if((
unsigned int) _temp162 > 1u?*(( int*) _temp162) == Cyc_Absyn_Pointer_mod: 0){
_LL173: _temp172=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp162)->f1;
goto _LL171; _LL171: _temp170=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f2; goto _LL169; _LL169: _temp168=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp162)->f3; goto _LL165;} else{ goto _LL166;} _LL166: goto _LL167; _LL165:
return 1; _LL167: return 0; _LL163:;}} static void Cyc_Absyndump_dumprgn( void*
t){ void* _temp174= Cyc_Tcutil_compress( t); _LL176: if( _temp174 ==( void*) Cyc_Absyn_HeapRgn){
goto _LL177;} else{ goto _LL178;} _LL178: goto _LL179; _LL177: Cyc_Absyndump_dump(({
char* _temp180="`H"; struct _tagged_string _temp181; _temp181.curr= _temp180;
_temp181.base= _temp180; _temp181.last_plus_one= _temp180 + 3; _temp181;}));
goto _LL175; _LL179: Cyc_Absyndump_dumpntyp( t); goto _LL175; _LL175:;} struct
_tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; static struct
_tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List* regions= 0;
struct Cyc_List_List* effects= 0;{ void* _temp182= Cyc_Tcutil_compress( t); void*
_temp190; struct Cyc_List_List* _temp192; _LL184: if(( unsigned int) _temp182 >
4u?*(( int*) _temp182) == Cyc_Absyn_AccessEff: 0){ _LL191: _temp190=( void*)((
struct Cyc_Absyn_AccessEff_struct*) _temp182)->f1; goto _LL185;} else{ goto
_LL186;} _LL186: if(( unsigned int) _temp182 > 4u?*(( int*) _temp182) == Cyc_Absyn_JoinEff:
0){ _LL193: _temp192=(( struct Cyc_Absyn_JoinEff_struct*) _temp182)->f1; goto
_LL187;} else{ goto _LL188;} _LL188: goto _LL189; _LL185: regions=({ struct Cyc_List_List*
_temp194=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp194->hd=( void*) _temp190; _temp194->tl= regions; _temp194;}); goto _LL183;
_LL187: for( 0; _temp192 != 0; _temp192=(( struct Cyc_List_List*) _check_null(
_temp192))->tl){ struct Cyc_List_List* _temp197; struct Cyc_List_List* _temp199;
struct _tuple6 _temp195= Cyc_Absyndump_effects_split(( void*)(( struct Cyc_List_List*)
_check_null( _temp192))->hd); _LL200: _temp199= _temp195.f1; goto _LL198; _LL198:
_temp197= _temp195.f2; goto _LL196; _LL196: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp199, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp197, effects);} goto _LL183;
_LL189: effects=({ struct Cyc_List_List* _temp201=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp201->hd=( void*) t; _temp201->tl=
effects; _temp201;}); goto _LL183; _LL183:;} return({ struct _tuple6 _temp202;
_temp202.f1= regions; _temp202.f2= effects; _temp202;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp205; struct Cyc_List_List* _temp207; struct
_tuple6 _temp203= Cyc_Absyndump_effects_split( t); _LL208: _temp207= _temp203.f1;
goto _LL206; _LL206: _temp205= _temp203.f2; goto _LL204; _LL204: _temp207=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp207);
_temp205=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp205); for( 0; _temp205 != 0; _temp205=(( struct Cyc_List_List*) _check_null(
_temp205))->tl){ Cyc_Absyndump_dumpntyp(( void*)(( struct Cyc_List_List*)
_check_null( _temp205))->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp207 != 0; _temp207=(( struct Cyc_List_List*) _check_null(
_temp207))->tl){ Cyc_Absyndump_dumprgn(( void*)(( struct Cyc_List_List*)
_check_null( _temp207))->hd); if((( struct Cyc_List_List*) _check_null( _temp207))->tl
!= 0){ Cyc_Absyndump_dump_char(( int)',');}} Cyc_Absyndump_dump_char(( int)'}');}
void Cyc_Absyndump_dumpntyp( void* t){ void* _temp209= t; struct Cyc_Absyn_Exp*
_temp279; struct Cyc_Absyn_Tqual _temp281; void* _temp283; struct Cyc_Absyn_FnInfo
_temp285; struct Cyc_Absyn_PtrInfo _temp287; struct Cyc_Absyn_Tvar* _temp289;
int _temp291; struct Cyc_Core_Opt* _temp293; void* _temp295; int _temp297;
struct Cyc_Core_Opt* _temp299; struct Cyc_Core_Opt _temp301; void* _temp302;
void* _temp304; struct Cyc_Absyn_TunionInfo _temp306; void* _temp308; struct Cyc_List_List*
_temp310; void* _temp312; struct Cyc_Absyn_TunionFieldInfo _temp314; struct Cyc_List_List*
_temp316; void* _temp318; struct Cyc_Absyn_Enumdecl* _temp320; struct _tuple0*
_temp322; void* _temp324; void* _temp326; void* _temp328; void* _temp330; void*
_temp332; void* _temp334; void* _temp336; void* _temp338; void* _temp340; void*
_temp342; void* _temp344; void* _temp346; void* _temp349; void* _temp351; void*
_temp353; void* _temp355; void* _temp358; void* _temp360; void* _temp362; void*
_temp364; struct Cyc_List_List* _temp366; struct Cyc_Absyn_Structdecl** _temp368;
struct Cyc_List_List* _temp370; struct _tuple0* _temp372; struct Cyc_Absyn_Structdecl**
_temp374; struct Cyc_List_List* _temp376; struct _tuple0* _temp378; struct Cyc_Absyn_Uniondecl**
_temp380; struct Cyc_List_List* _temp382; struct _tuple0* _temp384; struct Cyc_Absyn_Uniondecl**
_temp386; struct Cyc_List_List* _temp388; struct _tuple0* _temp390; struct Cyc_List_List*
_temp392; struct Cyc_List_List* _temp394; struct Cyc_Core_Opt* _temp396; struct
Cyc_List_List* _temp398; struct _tuple0* _temp400; void* _temp402; void*
_temp404; struct Cyc_List_List* _temp406; _LL211: if(( unsigned int) _temp209 >
4u?*(( int*) _temp209) == Cyc_Absyn_ArrayType: 0){ _LL284: _temp283=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp209)->f1; goto _LL282; _LL282: _temp281=((
struct Cyc_Absyn_ArrayType_struct*) _temp209)->f2; goto _LL280; _LL280: _temp279=((
struct Cyc_Absyn_ArrayType_struct*) _temp209)->f3; goto _LL212;} else{ goto
_LL213;} _LL213: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_FnType:
0){ _LL286: _temp285=(( struct Cyc_Absyn_FnType_struct*) _temp209)->f1; goto
_LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_PointerType: 0){ _LL288: _temp287=(( struct Cyc_Absyn_PointerType_struct*)
_temp209)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if( _temp209 ==( void*)
Cyc_Absyn_VoidType){ goto _LL218;} else{ goto _LL219;} _LL219: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_VarType: 0){ _LL290: _temp289=((
struct Cyc_Absyn_VarType_struct*) _temp209)->f1; goto _LL220;} else{ goto _LL221;}
_LL221: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_Evar:
0){ _LL296: _temp295=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp209)->f1;
goto _LL294; _LL294: _temp293=(( struct Cyc_Absyn_Evar_struct*) _temp209)->f2;
if( _temp293 == 0){ goto _LL292;} else{ goto _LL223;} _LL292: _temp291=(( struct
Cyc_Absyn_Evar_struct*) _temp209)->f3; goto _LL222;} else{ goto _LL223;} _LL223:
if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_Evar: 0){
_LL305: _temp304=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp209)->f1; goto
_LL300; _LL300: _temp299=(( struct Cyc_Absyn_Evar_struct*) _temp209)->f2; if(
_temp299 == 0){ goto _LL225;} else{ _temp301=* _temp299; _LL303: _temp302=( void*)
_temp301.v; goto _LL298;} _LL298: _temp297=(( struct Cyc_Absyn_Evar_struct*)
_temp209)->f3; goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_TunionType: 0){ _LL307: _temp306=((
struct Cyc_Absyn_TunionType_struct*) _temp209)->f1; _LL313: _temp312=( void*)
_temp306.tunion_info; goto _LL311; _LL311: _temp310= _temp306.targs; goto _LL309;
_LL309: _temp308=( void*) _temp306.rgn; goto _LL226;} else{ goto _LL227;} _LL227:
if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_TunionFieldType:
0){ _LL315: _temp314=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp209)->f1;
_LL319: _temp318=( void*) _temp314.field_info; goto _LL317; _LL317: _temp316=
_temp314.targs; goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_EnumType: 0){ _LL323: _temp322=((
struct Cyc_Absyn_EnumType_struct*) _temp209)->f1; goto _LL321; _LL321: _temp320=((
struct Cyc_Absyn_EnumType_struct*) _temp209)->f2; goto _LL230;} else{ goto
_LL231;} _LL231: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL327: _temp326=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp326 ==( void*) Cyc_Absyn_Signed){ goto _LL325;} else{ goto _LL233;}
_LL325: _temp324=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp324 ==( void*) Cyc_Absyn_B4){ goto _LL232;} else{ goto _LL233;}} else{ goto
_LL233;} _LL233: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL331: _temp330=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp330 ==( void*) Cyc_Absyn_Signed){ goto _LL329;} else{ goto _LL235;}
_LL329: _temp328=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp328 ==( void*) Cyc_Absyn_B1){ goto _LL234;} else{ goto _LL235;}} else{ goto
_LL235;} _LL235: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL335: _temp334=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp334 ==( void*) Cyc_Absyn_Unsigned){ goto _LL333;} else{ goto _LL237;}
_LL333: _temp332=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp332 ==( void*) Cyc_Absyn_B1){ goto _LL236;} else{ goto _LL237;}} else{ goto
_LL237;} _LL237: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL339: _temp338=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp338 ==( void*) Cyc_Absyn_Signed){ goto _LL337;} else{ goto _LL239;}
_LL337: _temp336=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp336 ==( void*) Cyc_Absyn_B2){ goto _LL238;} else{ goto _LL239;}} else{ goto
_LL239;} _LL239: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL343: _temp342=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp342 ==( void*) Cyc_Absyn_Unsigned){ goto _LL341;} else{ goto _LL241;}
_LL341: _temp340=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp340 ==( void*) Cyc_Absyn_B2){ goto _LL240;} else{ goto _LL241;}} else{ goto
_LL241;} _LL241: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL347: _temp346=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp346 ==( void*) Cyc_Absyn_Unsigned){ goto _LL345;} else{ goto _LL243;}
_LL345: _temp344=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp344 ==( void*) Cyc_Absyn_B4){ goto _LL242;} else{ goto _LL243;}} else{ goto
_LL243;} _LL243: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL352: _temp351=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp351 ==( void*) Cyc_Absyn_Signed){ goto _LL350;} else{ goto _LL245;}
_LL350: _temp349=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp349 ==( void*) Cyc_Absyn_B8){ goto _LL348;} else{ goto _LL245;}} else{ goto
_LL245;} _LL348: if( Cyc_Absynpp_to_VC){ goto _LL244;} else{ goto _LL245;}
_LL245: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL356: _temp355=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp355 ==( void*) Cyc_Absyn_Signed){ goto _LL354;} else{ goto _LL247;}
_LL354: _temp353=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp353 ==( void*) Cyc_Absyn_B8){ goto _LL246;} else{ goto _LL247;}} else{ goto
_LL247;} _LL247: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp360 ==( void*) Cyc_Absyn_Unsigned){ goto _LL359;} else{ goto _LL249;}
_LL359: _temp358=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp358 ==( void*) Cyc_Absyn_B8){ goto _LL357;} else{ goto _LL249;}} else{ goto
_LL249;} _LL357: if( Cyc_Absynpp_to_VC){ goto _LL248;} else{ goto _LL249;}
_LL249: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_IntType:
0){ _LL365: _temp364=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f1;
if( _temp364 ==( void*) Cyc_Absyn_Unsigned){ goto _LL363;} else{ goto _LL251;}
_LL363: _temp362=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp209)->f2; if(
_temp362 ==( void*) Cyc_Absyn_B8){ goto _LL250;} else{ goto _LL251;}} else{ goto
_LL251;} _LL251: if( _temp209 ==( void*) Cyc_Absyn_FloatType){ goto _LL252;}
else{ goto _LL253;} _LL253: if( _temp209 ==( void*) Cyc_Absyn_DoubleType){ goto
_LL254;} else{ goto _LL255;} _LL255: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_TupleType: 0){ _LL367: _temp366=(( struct Cyc_Absyn_TupleType_struct*)
_temp209)->f1; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_StructType: 0){ _LL373: _temp372=((
struct Cyc_Absyn_StructType_struct*) _temp209)->f1; if( _temp372 == 0){ goto
_LL371;} else{ goto _LL259;} _LL371: _temp370=(( struct Cyc_Absyn_StructType_struct*)
_temp209)->f2; goto _LL369; _LL369: _temp368=(( struct Cyc_Absyn_StructType_struct*)
_temp209)->f3; goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_StructType: 0){ _LL379: _temp378=((
struct Cyc_Absyn_StructType_struct*) _temp209)->f1; goto _LL377; _LL377:
_temp376=(( struct Cyc_Absyn_StructType_struct*) _temp209)->f2; goto _LL375;
_LL375: _temp374=(( struct Cyc_Absyn_StructType_struct*) _temp209)->f3; goto
_LL260;} else{ goto _LL261;} _LL261: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_UnionType: 0){ _LL385: _temp384=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f1; if( _temp384 == 0){ goto _LL383;} else{ goto _LL263;} _LL383:
_temp382=(( struct Cyc_Absyn_UnionType_struct*) _temp209)->f2; goto _LL381;
_LL381: _temp380=(( struct Cyc_Absyn_UnionType_struct*) _temp209)->f3; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_UnionType: 0){ _LL391: _temp390=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f1; goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f2; goto _LL387; _LL387: _temp386=(( struct Cyc_Absyn_UnionType_struct*)
_temp209)->f3; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_AnonStructType: 0){ _LL393:
_temp392=(( struct Cyc_Absyn_AnonStructType_struct*) _temp209)->f1; goto _LL266;}
else{ goto _LL267;} _LL267: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209)
== Cyc_Absyn_AnonUnionType: 0){ _LL395: _temp394=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp209)->f1; goto _LL268;} else{ goto _LL269;} _LL269: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_TypedefType: 0){ _LL401: _temp400=((
struct Cyc_Absyn_TypedefType_struct*) _temp209)->f1; goto _LL399; _LL399:
_temp398=(( struct Cyc_Absyn_TypedefType_struct*) _temp209)->f2; goto _LL397;
_LL397: _temp396=(( struct Cyc_Absyn_TypedefType_struct*) _temp209)->f3; goto
_LL270;} else{ goto _LL271;} _LL271: if(( unsigned int) _temp209 > 4u?*(( int*)
_temp209) == Cyc_Absyn_RgnHandleType: 0){ _LL403: _temp402=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp209)->f1; goto _LL272;} else{ goto _LL273;} _LL273: if( _temp209 ==( void*)
Cyc_Absyn_HeapRgn){ goto _LL274;} else{ goto _LL275;} _LL275: if(( unsigned int)
_temp209 > 4u?*(( int*) _temp209) == Cyc_Absyn_AccessEff: 0){ _LL405: _temp404=(
void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp209)->f1; goto _LL276;} else{
goto _LL277;} _LL277: if(( unsigned int) _temp209 > 4u?*(( int*) _temp209) ==
Cyc_Absyn_JoinEff: 0){ _LL407: _temp406=(( struct Cyc_Absyn_JoinEff_struct*)
_temp209)->f1; goto _LL278;} else{ goto _LL210;} _LL212: return; _LL214: return;
_LL216: return; _LL218: Cyc_Absyndump_dump(({ char* _temp408="void"; struct
_tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 5; _temp409;})); return; _LL220: Cyc_Absyndump_dump_str(
_temp289->name); return; _LL222: Cyc_Absyndump_dump(({ char* _temp410="%";
struct _tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 2; _temp411;})); Cyc_Absyndump_dumpkind(
_temp295); Cyc_Absyndump_dump( xprintf("(%d)", _temp291)); return; _LL224: Cyc_Absyndump_dumpntyp(
_temp302); return; _LL226:{ void* _temp412= _temp312; struct Cyc_Absyn_UnknownTunionInfo
_temp418; int _temp420; struct _tuple0* _temp422; struct Cyc_Absyn_Tuniondecl*
_temp424; struct Cyc_Absyn_Tuniondecl _temp426; int _temp427; struct Cyc_Core_Opt*
_temp429; struct Cyc_List_List* _temp431; struct _tuple0* _temp433; void*
_temp435; _LL414: if(*(( int*) _temp412) == Cyc_Absyn_UnknownTunion){ _LL419:
_temp418=(( struct Cyc_Absyn_UnknownTunion_struct*) _temp412)->f1; _LL423:
_temp422= _temp418.name; goto _LL421; _LL421: _temp420= _temp418.is_xtunion;
goto _LL415;} else{ goto _LL416;} _LL416: if(*(( int*) _temp412) == Cyc_Absyn_KnownTunion){
_LL425: _temp424=(( struct Cyc_Absyn_KnownTunion_struct*) _temp412)->f1;
_temp426=* _temp424; _LL436: _temp435=( void*) _temp426.sc; goto _LL434; _LL434:
_temp433= _temp426.name; goto _LL432; _LL432: _temp431= _temp426.tvs; goto
_LL430; _LL430: _temp429= _temp426.fields; goto _LL428; _LL428: _temp427=
_temp426.is_xtunion; goto _LL417;} else{ goto _LL413;} _LL415: _temp433=
_temp422; _temp427= _temp420; goto _LL417; _LL417: if( _temp427){ Cyc_Absyndump_dump(({
char* _temp437="xtunion "; struct _tagged_string _temp438; _temp438.curr=
_temp437; _temp438.base= _temp437; _temp438.last_plus_one= _temp437 + 9;
_temp438;}));} else{ Cyc_Absyndump_dump(({ char* _temp439="tunion "; struct
_tagged_string _temp440; _temp440.curr= _temp439; _temp440.base= _temp439;
_temp440.last_plus_one= _temp439 + 8; _temp440;}));}{ void* _temp441= Cyc_Tcutil_compress(
_temp308); _LL443: if( _temp441 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL444;}
else{ goto _LL445;} _LL445: goto _LL446; _LL444: goto _LL442; _LL446: Cyc_Absyndump_dumptyp(
_temp308); Cyc_Absyndump_dump(({ char* _temp447=" "; struct _tagged_string
_temp448; _temp448.curr= _temp447; _temp448.base= _temp447; _temp448.last_plus_one=
_temp447 + 2; _temp448;})); goto _LL442; _LL442:;} Cyc_Absyndump_dumpqvar(
_temp433); Cyc_Absyndump_dumptps( _temp310); goto _LL413; _LL413:;} goto _LL210;
_LL228:{ void* _temp449= _temp318; struct Cyc_Absyn_UnknownTunionFieldInfo
_temp455; int _temp457; struct _tuple0* _temp459; struct _tuple0* _temp461;
struct Cyc_Absyn_Tunionfield* _temp463; struct Cyc_Absyn_Tunionfield _temp465;
void* _temp466; struct Cyc_Position_Segment* _temp468; struct Cyc_List_List*
_temp470; struct Cyc_List_List* _temp472; struct _tuple0* _temp474; struct Cyc_Absyn_Tuniondecl*
_temp476; struct Cyc_Absyn_Tuniondecl _temp478; int _temp479; struct Cyc_Core_Opt*
_temp481; struct Cyc_List_List* _temp483; struct _tuple0* _temp485; void*
_temp487; _LL451: if(*(( int*) _temp449) == Cyc_Absyn_UnknownTunionfield){
_LL456: _temp455=(( struct Cyc_Absyn_UnknownTunionfield_struct*) _temp449)->f1;
_LL462: _temp461= _temp455.tunion_name; goto _LL460; _LL460: _temp459= _temp455.field_name;
goto _LL458; _LL458: _temp457= _temp455.is_xtunion; goto _LL452;} else{ goto
_LL453;} _LL453: if(*(( int*) _temp449) == Cyc_Absyn_KnownTunionfield){ _LL477:
_temp476=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp449)->f1; _temp478=*
_temp476; _LL488: _temp487=( void*) _temp478.sc; goto _LL486; _LL486: _temp485=
_temp478.name; goto _LL484; _LL484: _temp483= _temp478.tvs; goto _LL482; _LL482:
_temp481= _temp478.fields; goto _LL480; _LL480: _temp479= _temp478.is_xtunion;
goto _LL464; _LL464: _temp463=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp449)->f2; _temp465=* _temp463; _LL475: _temp474= _temp465.name; goto _LL473;
_LL473: _temp472= _temp465.tvs; goto _LL471; _LL471: _temp470= _temp465.typs;
goto _LL469; _LL469: _temp468= _temp465.loc; goto _LL467; _LL467: _temp466=(
void*) _temp465.sc; goto _LL454;} else{ goto _LL450;} _LL452: _temp485= _temp461;
_temp479= _temp457; _temp474= _temp459; goto _LL454; _LL454: if( _temp479){ Cyc_Absyndump_dump(({
char* _temp489="xtunion "; struct _tagged_string _temp490; _temp490.curr=
_temp489; _temp490.base= _temp489; _temp490.last_plus_one= _temp489 + 9;
_temp490;}));} else{ Cyc_Absyndump_dump(({ char* _temp491="tunion "; struct
_tagged_string _temp492; _temp492.curr= _temp491; _temp492.base= _temp491;
_temp492.last_plus_one= _temp491 + 8; _temp492;}));} Cyc_Absyndump_dumpqvar(
_temp485); Cyc_Absyndump_dump(({ char* _temp493="."; struct _tagged_string
_temp494; _temp494.curr= _temp493; _temp494.base= _temp493; _temp494.last_plus_one=
_temp493 + 2; _temp494;})); Cyc_Absyndump_dumpqvar( _temp474); Cyc_Absyndump_dumptps(
_temp316); goto _LL450; _LL450:;} goto _LL210; _LL230: Cyc_Absyndump_dump(({
char* _temp495="enum "; struct _tagged_string _temp496; _temp496.curr= _temp495;
_temp496.base= _temp495; _temp496.last_plus_one= _temp495 + 6; _temp496;})); Cyc_Absyndump_dumpqvar(
_temp322); return; _LL232: Cyc_Absyndump_dump(({ char* _temp497="int"; struct
_tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 4; _temp498;})); return; _LL234: Cyc_Absyndump_dump(({
char* _temp499="signed char"; struct _tagged_string _temp500; _temp500.curr=
_temp499; _temp500.base= _temp499; _temp500.last_plus_one= _temp499 + 12;
_temp500;})); return; _LL236: Cyc_Absyndump_dump(({ char* _temp501="char";
struct _tagged_string _temp502; _temp502.curr= _temp501; _temp502.base= _temp501;
_temp502.last_plus_one= _temp501 + 5; _temp502;})); return; _LL238: Cyc_Absyndump_dump(({
char* _temp503="short"; struct _tagged_string _temp504; _temp504.curr= _temp503;
_temp504.base= _temp503; _temp504.last_plus_one= _temp503 + 6; _temp504;}));
return; _LL240: Cyc_Absyndump_dump(({ char* _temp505="unsigned short"; struct
_tagged_string _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 15; _temp506;})); return; _LL242: Cyc_Absyndump_dump(({
char* _temp507="unsigned int"; struct _tagged_string _temp508; _temp508.curr=
_temp507; _temp508.base= _temp507; _temp508.last_plus_one= _temp507 + 13;
_temp508;})); return; _LL244: Cyc_Absyndump_dump(({ char* _temp509="__int64";
struct _tagged_string _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 8; _temp510;})); return; _LL246: Cyc_Absyndump_dump(({
char* _temp511="long long"; struct _tagged_string _temp512; _temp512.curr=
_temp511; _temp512.base= _temp511; _temp512.last_plus_one= _temp511 + 10;
_temp512;})); return; _LL248: Cyc_Absyndump_dump(({ char* _temp513="unsigned __int64";
struct _tagged_string _temp514; _temp514.curr= _temp513; _temp514.base= _temp513;
_temp514.last_plus_one= _temp513 + 17; _temp514;})); return; _LL250: Cyc_Absyndump_dump(({
char* _temp515="unsigned long long"; struct _tagged_string _temp516; _temp516.curr=
_temp515; _temp516.base= _temp515; _temp516.last_plus_one= _temp515 + 19;
_temp516;})); return; _LL252: Cyc_Absyndump_dump(({ char* _temp517="float";
struct _tagged_string _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 6; _temp518;})); return; _LL254: Cyc_Absyndump_dump(({
char* _temp519="double"; struct _tagged_string _temp520; _temp520.curr= _temp519;
_temp520.base= _temp519; _temp520.last_plus_one= _temp519 + 7; _temp520;}));
return; _LL256: Cyc_Absyndump_dump_char(( int)'$'); Cyc_Absyndump_dumpargs(
_temp366); return; _LL258: Cyc_Absyndump_dump(({ char* _temp521="struct"; struct
_tagged_string _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 7; _temp522;})); Cyc_Absyndump_dumptps(
_temp370); return; _LL260: Cyc_Absyndump_dump(({ char* _temp523="struct"; struct
_tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 7; _temp524;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp378)); Cyc_Absyndump_dumptps( _temp376);
return; _LL262: Cyc_Absyndump_dump(({ char* _temp525="union"; struct
_tagged_string _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 6; _temp526;})); Cyc_Absyndump_dumptps(
_temp382); return; _LL264: Cyc_Absyndump_dump(({ char* _temp527="union"; struct
_tagged_string _temp528; _temp528.curr= _temp527; _temp528.base= _temp527;
_temp528.last_plus_one= _temp527 + 6; _temp528;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*) _check_null( _temp390)); Cyc_Absyndump_dumptps( _temp388);
return; _LL266: Cyc_Absyndump_dump(({ char* _temp529="struct {"; struct
_tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 9; _temp530;})); Cyc_Absyndump_dumpstructfields(
_temp392); Cyc_Absyndump_dump(({ char* _temp531="}"; struct _tagged_string
_temp532; _temp532.curr= _temp531; _temp532.base= _temp531; _temp532.last_plus_one=
_temp531 + 2; _temp532;})); return; _LL268: Cyc_Absyndump_dump(({ char* _temp533="union {";
struct _tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 8; _temp534;})); Cyc_Absyndump_dumpstructfields(
_temp394); Cyc_Absyndump_dump(({ char* _temp535="}"; struct _tagged_string
_temp536; _temp536.curr= _temp535; _temp536.base= _temp535; _temp536.last_plus_one=
_temp535 + 2; _temp536;})); return; _LL270:( Cyc_Absyndump_dumpqvar( _temp400),
Cyc_Absyndump_dumptps( _temp398)); return; _LL272: Cyc_Absyndump_dumprgn(
_temp402); return; _LL274: return; _LL276: return; _LL278: return; _LL210:;}
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
char* _temp537="..."; struct _tagged_string _temp538; _temp538.curr= _temp537;
_temp538.base= _temp537; _temp538.last_plus_one= _temp537 + 4; _temp538;}));}
if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)((
struct Cyc_Core_Opt*) _check_null( effopt))->v);} Cyc_Absyndump_dump_char(( int)')');}
void Cyc_Absyndump_dumptyp( void* t){(( void(*)( struct Cyc_Absyn_Tqual, void*,
void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp539; _temp539.q_const= 0; _temp539.q_volatile= 0; _temp539.q_restrict= 0;
_temp539;}), t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp540= d; struct Cyc_Absyn_Exp* _temp546; struct
_tagged_string* _temp548; _LL542: if(*(( int*) _temp540) == Cyc_Absyn_ArrayElement){
_LL547: _temp546=(( struct Cyc_Absyn_ArrayElement_struct*) _temp540)->f1; goto
_LL543;} else{ goto _LL544;} _LL544: if(*(( int*) _temp540) == Cyc_Absyn_FieldName){
_LL549: _temp548=(( struct Cyc_Absyn_FieldName_struct*) _temp540)->f1; goto
_LL545;} else{ goto _LL541;} _LL543: Cyc_Absyndump_dump(({ char* _temp550=".[";
struct _tagged_string _temp551; _temp551.curr= _temp550; _temp551.base= _temp550;
_temp551.last_plus_one= _temp550 + 3; _temp551;})); Cyc_Absyndump_dumpexp(
_temp546); Cyc_Absyndump_dump_char(( int)']'); goto _LL541; _LL545: Cyc_Absyndump_dump_char((
int)'.'); Cyc_Absyndump_dump_nospace(* _temp548); goto _LL541; _LL541:;} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; void Cyc_Absyndump_dumpde(
struct _tuple7* de){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpdesignator,(* de).f1,({ char*
_temp552=""; struct _tagged_string _temp553; _temp553.curr= _temp552; _temp553.base=
_temp552; _temp553.last_plus_one= _temp552 + 1; _temp553;}),({ char* _temp554="=";
struct _tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 2; _temp555;}),({ char* _temp556="="; struct
_tagged_string _temp557; _temp557.curr= _temp556; _temp557.base= _temp556;
_temp557.last_plus_one= _temp556 + 2; _temp557;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,({ char* _temp558=""; struct
_tagged_string _temp559; _temp559.curr= _temp558; _temp559.base= _temp558;
_temp559.last_plus_one= _temp558 + 1; _temp559;}),({ char* _temp560=""; struct
_tagged_string _temp561; _temp561.curr= _temp560; _temp561.base= _temp560;
_temp561.last_plus_one= _temp560 + 1; _temp561;}),({ char* _temp562=","; struct
_tagged_string _temp563; _temp563.curr= _temp562; _temp563.base= _temp562;
_temp563.last_plus_one= _temp562 + 2; _temp563;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(({ char* _temp564="("; struct
_tagged_string _temp565; _temp565.curr= _temp564; _temp565.base= _temp564;
_temp565.last_plus_one= _temp564 + 2; _temp565;}));}{ void* _temp566=( void*) e->r;
void* _temp658; char _temp660; void* _temp662; void* _temp664; short _temp666;
void* _temp668; void* _temp670; int _temp672; void* _temp674; void* _temp676;
int _temp678; void* _temp680; void* _temp682; long long _temp684; void* _temp686;
void* _temp688; struct _tagged_string _temp690; void* _temp692; void* _temp694;
struct _tagged_string _temp696; struct _tuple0* _temp698; void* _temp700; struct
_tuple0* _temp702; struct Cyc_List_List* _temp704; void* _temp706; struct Cyc_Absyn_Exp*
_temp708; struct Cyc_Core_Opt* _temp710; struct Cyc_Absyn_Exp* _temp712; void*
_temp714; struct Cyc_Absyn_Exp* _temp716; void* _temp718; struct Cyc_Absyn_Exp*
_temp720; void* _temp722; struct Cyc_Absyn_Exp* _temp724; void* _temp726; struct
Cyc_Absyn_Exp* _temp728; struct Cyc_Absyn_Exp* _temp730; struct Cyc_Absyn_Exp*
_temp732; struct Cyc_Absyn_Exp* _temp734; struct Cyc_Absyn_Exp* _temp736; struct
Cyc_Absyn_Exp* _temp738; struct Cyc_List_List* _temp740; struct Cyc_Absyn_Exp*
_temp742; struct Cyc_List_List* _temp744; struct Cyc_Absyn_Exp* _temp746; struct
Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Exp* _temp750; struct Cyc_List_List*
_temp752; struct Cyc_Absyn_Exp* _temp754; struct Cyc_Absyn_Exp* _temp756; void*
_temp758; struct Cyc_Absyn_Exp* _temp760; struct Cyc_Absyn_Exp* _temp762; struct
Cyc_Absyn_Exp* _temp764; void* _temp766; struct Cyc_Absyn_Exp* _temp768; struct
Cyc_Absyn_Exp* _temp770; struct _tagged_string* _temp772; struct Cyc_Absyn_Exp*
_temp774; struct _tagged_string* _temp776; struct Cyc_Absyn_Exp* _temp778;
struct Cyc_Absyn_Exp* _temp780; struct Cyc_Absyn_Exp* _temp782; struct Cyc_List_List*
_temp784; struct Cyc_List_List* _temp786; struct _tuple1* _temp788; struct Cyc_List_List*
_temp790; struct Cyc_Absyn_Exp* _temp792; struct Cyc_Absyn_Exp* _temp794; struct
Cyc_Absyn_Vardecl* _temp796; struct Cyc_Absyn_Structdecl* _temp798; struct Cyc_List_List*
_temp800; struct Cyc_Core_Opt* _temp802; struct _tuple0* _temp804; struct Cyc_List_List*
_temp806; void* _temp808; struct Cyc_Absyn_Tunionfield* _temp810; struct Cyc_Absyn_Tuniondecl*
_temp812; struct Cyc_List_List* _temp814; struct Cyc_Core_Opt* _temp816; struct
Cyc_Core_Opt* _temp818; struct Cyc_Absyn_Enumfield* _temp820; struct Cyc_Absyn_Enumdecl*
_temp822; struct _tuple0* _temp824; void* _temp826; struct Cyc_Absyn_Exp*
_temp828; struct Cyc_List_List* _temp830; struct Cyc_Core_Opt* _temp832; struct
Cyc_Absyn_Stmt* _temp834; struct Cyc_Absyn_Fndecl* _temp836; struct Cyc_Absyn_Exp*
_temp838; _LL568: if(*(( int*) _temp566) == Cyc_Absyn_Const_e){ _LL659: _temp658=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp566)->f1; if(( unsigned int)
_temp658 > 1u?*(( int*) _temp658) == Cyc_Absyn_Char_c: 0){ _LL663: _temp662=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp658)->f1; goto _LL661; _LL661:
_temp660=(( struct Cyc_Absyn_Char_c_struct*) _temp658)->f2; goto _LL569;} else{
goto _LL570;}} else{ goto _LL570;} _LL570: if(*(( int*) _temp566) == Cyc_Absyn_Const_e){
_LL665: _temp664=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp566)->f1; if((
unsigned int) _temp664 > 1u?*(( int*) _temp664) == Cyc_Absyn_Short_c: 0){ _LL669:
_temp668=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp664)->f1; goto _LL667;
_LL667: _temp666=(( struct Cyc_Absyn_Short_c_struct*) _temp664)->f2; goto _LL571;}
else{ goto _LL572;}} else{ goto _LL572;} _LL572: if(*(( int*) _temp566) == Cyc_Absyn_Const_e){
_LL671: _temp670=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp566)->f1; if((
unsigned int) _temp670 > 1u?*(( int*) _temp670) == Cyc_Absyn_Int_c: 0){ _LL675:
_temp674=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp670)->f1; if( _temp674
==( void*) Cyc_Absyn_Signed){ goto _LL673;} else{ goto _LL574;} _LL673: _temp672=((
struct Cyc_Absyn_Int_c_struct*) _temp670)->f2; goto _LL573;} else{ goto _LL574;}}
else{ goto _LL574;} _LL574: if(*(( int*) _temp566) == Cyc_Absyn_Const_e){ _LL677:
_temp676=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp566)->f1; if((
unsigned int) _temp676 > 1u?*(( int*) _temp676) == Cyc_Absyn_Int_c: 0){ _LL681:
_temp680=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp676)->f1; if( _temp680
==( void*) Cyc_Absyn_Unsigned){ goto _LL679;} else{ goto _LL576;} _LL679:
_temp678=(( struct Cyc_Absyn_Int_c_struct*) _temp676)->f2; goto _LL575;} else{
goto _LL576;}} else{ goto _LL576;} _LL576: if(*(( int*) _temp566) == Cyc_Absyn_Const_e){
_LL683: _temp682=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp566)->f1; if((
unsigned int) _temp682 > 1u?*(( int*) _temp682) == Cyc_Absyn_LongLong_c: 0){
_LL687: _temp686=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp682)->f1;
goto _LL685; _LL685: _temp684=(( struct Cyc_Absyn_LongLong_c_struct*) _temp682)->f2;
goto _LL577;} else{ goto _LL578;}} else{ goto _LL578;} _LL578: if(*(( int*)
_temp566) == Cyc_Absyn_Const_e){ _LL689: _temp688=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp566)->f1; if(( unsigned int) _temp688 > 1u?*(( int*) _temp688) == Cyc_Absyn_Float_c:
0){ _LL691: _temp690=(( struct Cyc_Absyn_Float_c_struct*) _temp688)->f1; goto
_LL579;} else{ goto _LL580;}} else{ goto _LL580;} _LL580: if(*(( int*) _temp566)
== Cyc_Absyn_Const_e){ _LL693: _temp692=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp566)->f1; if( _temp692 ==( void*) Cyc_Absyn_Null_c){ goto _LL581;} else{
goto _LL582;}} else{ goto _LL582;} _LL582: if(*(( int*) _temp566) == Cyc_Absyn_Const_e){
_LL695: _temp694=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp566)->f1; if((
unsigned int) _temp694 > 1u?*(( int*) _temp694) == Cyc_Absyn_String_c: 0){
_LL697: _temp696=(( struct Cyc_Absyn_String_c_struct*) _temp694)->f1; goto
_LL583;} else{ goto _LL584;}} else{ goto _LL584;} _LL584: if(*(( int*) _temp566)
== Cyc_Absyn_UnknownId_e){ _LL699: _temp698=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp566)->f1; goto _LL585;} else{ goto _LL586;} _LL586: if(*(( int*) _temp566)
== Cyc_Absyn_Var_e){ _LL703: _temp702=(( struct Cyc_Absyn_Var_e_struct*)
_temp566)->f1; goto _LL701; _LL701: _temp700=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp566)->f2; goto _LL587;} else{ goto _LL588;} _LL588: if(*(( int*) _temp566)
== Cyc_Absyn_Primop_e){ _LL707: _temp706=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp566)->f1; goto _LL705; _LL705: _temp704=(( struct Cyc_Absyn_Primop_e_struct*)
_temp566)->f2; goto _LL589;} else{ goto _LL590;} _LL590: if(*(( int*) _temp566)
== Cyc_Absyn_AssignOp_e){ _LL713: _temp712=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp566)->f1; goto _LL711; _LL711: _temp710=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp566)->f2; goto _LL709; _LL709: _temp708=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp566)->f3; goto _LL591;} else{ goto _LL592;} _LL592: if(*(( int*) _temp566)
== Cyc_Absyn_Increment_e){ _LL717: _temp716=(( struct Cyc_Absyn_Increment_e_struct*)
_temp566)->f1; goto _LL715; _LL715: _temp714=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp566)->f2; if( _temp714 ==( void*) Cyc_Absyn_PreInc){ goto _LL593;} else{
goto _LL594;}} else{ goto _LL594;} _LL594: if(*(( int*) _temp566) == Cyc_Absyn_Increment_e){
_LL721: _temp720=(( struct Cyc_Absyn_Increment_e_struct*) _temp566)->f1; goto
_LL719; _LL719: _temp718=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp566)->f2; if( _temp718 ==( void*) Cyc_Absyn_PreDec){ goto _LL595;} else{
goto _LL596;}} else{ goto _LL596;} _LL596: if(*(( int*) _temp566) == Cyc_Absyn_Increment_e){
_LL725: _temp724=(( struct Cyc_Absyn_Increment_e_struct*) _temp566)->f1; goto
_LL723; _LL723: _temp722=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp566)->f2; if( _temp722 ==( void*) Cyc_Absyn_PostInc){ goto _LL597;} else{
goto _LL598;}} else{ goto _LL598;} _LL598: if(*(( int*) _temp566) == Cyc_Absyn_Increment_e){
_LL729: _temp728=(( struct Cyc_Absyn_Increment_e_struct*) _temp566)->f1; goto
_LL727; _LL727: _temp726=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp566)->f2; if( _temp726 ==( void*) Cyc_Absyn_PostDec){ goto _LL599;} else{
goto _LL600;}} else{ goto _LL600;} _LL600: if(*(( int*) _temp566) == Cyc_Absyn_Conditional_e){
_LL735: _temp734=(( struct Cyc_Absyn_Conditional_e_struct*) _temp566)->f1; goto
_LL733; _LL733: _temp732=(( struct Cyc_Absyn_Conditional_e_struct*) _temp566)->f2;
goto _LL731; _LL731: _temp730=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp566)->f3; goto _LL601;} else{ goto _LL602;} _LL602: if(*(( int*) _temp566)
== Cyc_Absyn_SeqExp_e){ _LL739: _temp738=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp566)->f1; goto _LL737; _LL737: _temp736=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp566)->f2; goto _LL603;} else{ goto _LL604;} _LL604: if(*(( int*) _temp566)
== Cyc_Absyn_UnknownCall_e){ _LL743: _temp742=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp566)->f1; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp566)->f2; goto _LL605;} else{ goto _LL606;} _LL606: if(*(( int*) _temp566)
== Cyc_Absyn_FnCall_e){ _LL747: _temp746=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp566)->f1; goto _LL745; _LL745: _temp744=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp566)->f2; goto _LL607;} else{ goto _LL608;} _LL608: if(*(( int*) _temp566)
== Cyc_Absyn_Throw_e){ _LL749: _temp748=(( struct Cyc_Absyn_Throw_e_struct*)
_temp566)->f1; goto _LL609;} else{ goto _LL610;} _LL610: if(*(( int*) _temp566)
== Cyc_Absyn_NoInstantiate_e){ _LL751: _temp750=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp566)->f1; goto _LL611;} else{ goto _LL612;} _LL612: if(*(( int*) _temp566)
== Cyc_Absyn_Instantiate_e){ _LL755: _temp754=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp566)->f1; goto _LL753; _LL753: _temp752=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp566)->f2; goto _LL613;} else{ goto _LL614;} _LL614: if(*(( int*) _temp566)
== Cyc_Absyn_Cast_e){ _LL759: _temp758=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp566)->f1; goto _LL757; _LL757: _temp756=(( struct Cyc_Absyn_Cast_e_struct*)
_temp566)->f2; goto _LL615;} else{ goto _LL616;} _LL616: if(*(( int*) _temp566)
== Cyc_Absyn_Address_e){ _LL761: _temp760=(( struct Cyc_Absyn_Address_e_struct*)
_temp566)->f1; goto _LL617;} else{ goto _LL618;} _LL618: if(*(( int*) _temp566)
== Cyc_Absyn_New_e){ _LL765: _temp764=(( struct Cyc_Absyn_New_e_struct*)
_temp566)->f1; goto _LL763; _LL763: _temp762=(( struct Cyc_Absyn_New_e_struct*)
_temp566)->f2; goto _LL619;} else{ goto _LL620;} _LL620: if(*(( int*) _temp566)
== Cyc_Absyn_Sizeoftyp_e){ _LL767: _temp766=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp566)->f1; goto _LL621;} else{ goto _LL622;} _LL622: if(*(( int*) _temp566)
== Cyc_Absyn_Sizeofexp_e){ _LL769: _temp768=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp566)->f1; goto _LL623;} else{ goto _LL624;} _LL624: if(*(( int*) _temp566)
== Cyc_Absyn_Deref_e){ _LL771: _temp770=(( struct Cyc_Absyn_Deref_e_struct*)
_temp566)->f1; goto _LL625;} else{ goto _LL626;} _LL626: if(*(( int*) _temp566)
== Cyc_Absyn_StructMember_e){ _LL775: _temp774=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp566)->f1; goto _LL773; _LL773: _temp772=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp566)->f2; goto _LL627;} else{ goto _LL628;} _LL628: if(*(( int*) _temp566)
== Cyc_Absyn_StructArrow_e){ _LL779: _temp778=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp566)->f1; goto _LL777; _LL777: _temp776=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp566)->f2; goto _LL629;} else{ goto _LL630;} _LL630: if(*(( int*) _temp566)
== Cyc_Absyn_Subscript_e){ _LL783: _temp782=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp566)->f1; goto _LL781; _LL781: _temp780=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp566)->f2; goto _LL631;} else{ goto _LL632;} _LL632: if(*(( int*) _temp566)
== Cyc_Absyn_Tuple_e){ _LL785: _temp784=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp566)->f1; goto _LL633;} else{ goto _LL634;} _LL634: if(*(( int*) _temp566)
== Cyc_Absyn_CompoundLit_e){ _LL789: _temp788=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp566)->f1; goto _LL787; _LL787: _temp786=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp566)->f2; goto _LL635;} else{ goto _LL636;} _LL636: if(*(( int*) _temp566)
== Cyc_Absyn_Array_e){ _LL791: _temp790=(( struct Cyc_Absyn_Array_e_struct*)
_temp566)->f1; goto _LL637;} else{ goto _LL638;} _LL638: if(*(( int*) _temp566)
== Cyc_Absyn_Comprehension_e){ _LL797: _temp796=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp566)->f1; goto _LL795; _LL795: _temp794=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp566)->f2; goto _LL793; _LL793: _temp792=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp566)->f3; goto _LL639;} else{ goto _LL640;} _LL640: if(*(( int*) _temp566)
== Cyc_Absyn_Struct_e){ _LL805: _temp804=(( struct Cyc_Absyn_Struct_e_struct*)
_temp566)->f1; goto _LL803; _LL803: _temp802=(( struct Cyc_Absyn_Struct_e_struct*)
_temp566)->f2; goto _LL801; _LL801: _temp800=(( struct Cyc_Absyn_Struct_e_struct*)
_temp566)->f3; goto _LL799; _LL799: _temp798=(( struct Cyc_Absyn_Struct_e_struct*)
_temp566)->f4; goto _LL641;} else{ goto _LL642;} _LL642: if(*(( int*) _temp566)
== Cyc_Absyn_AnonStruct_e){ _LL809: _temp808=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp566)->f1; goto _LL807; _LL807: _temp806=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp566)->f2; goto _LL643;} else{ goto _LL644;} _LL644: if(*(( int*) _temp566)
== Cyc_Absyn_Tunion_e){ _LL819: _temp818=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp566)->f1; goto _LL817; _LL817: _temp816=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp566)->f2; goto _LL815; _LL815: _temp814=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp566)->f3; goto _LL813; _LL813: _temp812=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp566)->f4; goto _LL811; _LL811: _temp810=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp566)->f5; goto _LL645;} else{ goto _LL646;} _LL646: if(*(( int*) _temp566)
== Cyc_Absyn_Enum_e){ _LL825: _temp824=(( struct Cyc_Absyn_Enum_e_struct*)
_temp566)->f1; goto _LL823; _LL823: _temp822=(( struct Cyc_Absyn_Enum_e_struct*)
_temp566)->f2; goto _LL821; _LL821: _temp820=(( struct Cyc_Absyn_Enum_e_struct*)
_temp566)->f3; goto _LL647;} else{ goto _LL648;} _LL648: if(*(( int*) _temp566)
== Cyc_Absyn_Malloc_e){ _LL829: _temp828=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp566)->f1; goto _LL827; _LL827: _temp826=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp566)->f2; goto _LL649;} else{ goto _LL650;} _LL650: if(*(( int*) _temp566)
== Cyc_Absyn_UnresolvedMem_e){ _LL833: _temp832=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp566)->f1; goto _LL831; _LL831: _temp830=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp566)->f2; goto _LL651;} else{ goto _LL652;} _LL652: if(*(( int*) _temp566)
== Cyc_Absyn_StmtExp_e){ _LL835: _temp834=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp566)->f1; goto _LL653;} else{ goto _LL654;} _LL654: if(*(( int*) _temp566)
== Cyc_Absyn_Codegen_e){ _LL837: _temp836=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp566)->f1; goto _LL655;} else{ goto _LL656;} _LL656: if(*(( int*) _temp566)
== Cyc_Absyn_Fill_e){ _LL839: _temp838=(( struct Cyc_Absyn_Fill_e_struct*)
_temp566)->f1; goto _LL657;} else{ goto _LL567;} _LL569: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp660)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL567; _LL571: Cyc_Absyndump_dump(({ char* _temp840="(short)";
struct _tagged_string _temp841; _temp841.curr= _temp840; _temp841.base= _temp840;
_temp841.last_plus_one= _temp840 + 8; _temp841;})); Cyc_Absyndump_dump_nospace(
Cyc_Core_string_of_int(( int) _temp666)); goto _LL567; _LL573: Cyc_Absyndump_dump(
Cyc_Core_string_of_int( _temp672)); goto _LL567; _LL575: Cyc_Absyndump_dump( Cyc_Core_string_of_int(
_temp678)); Cyc_Absyndump_dump_nospace(({ char* _temp842="u"; struct
_tagged_string _temp843; _temp843.curr= _temp842; _temp843.base= _temp842;
_temp843.last_plus_one= _temp842 + 2; _temp843;})); goto _LL567; _LL577: Cyc_Absyndump_dump(({
char* _temp844="<<FIX LONG LONG CONSTANT>>"; struct _tagged_string _temp845;
_temp845.curr= _temp844; _temp845.base= _temp844; _temp845.last_plus_one=
_temp844 + 27; _temp845;})); goto _LL567; _LL579: Cyc_Absyndump_dump( _temp690);
goto _LL567; _LL581: Cyc_Absyndump_dump(({ char* _temp846="null"; struct
_tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 5; _temp847;})); goto _LL567; _LL583: Cyc_Absyndump_dump_char((
int)'"'); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_string_escape( _temp696)); Cyc_Absyndump_dump_char((
int)'"'); goto _LL567; _LL585: _temp702= _temp698; goto _LL587; _LL587: Cyc_Absyndump_dumpqvar(
_temp702); goto _LL567; _LL589: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp706); if( ! Cyc_Absyn_is_format_prim( _temp706)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp704)){ case 1: _LL848: if( _temp706 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp704))->hd); Cyc_Absyndump_dump(({ char*
_temp850=".size"; struct _tagged_string _temp851; _temp851.curr= _temp850;
_temp851.base= _temp850; _temp851.last_plus_one= _temp850 + 6; _temp851;}));}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp704))->hd);} break; case 2: _LL849: Cyc_Absyndump_dumpexp_prec(
myprec,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp704))->hd);
Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp704))->tl))->hd); break; default: _LL852:( void) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp854=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp854[ 0]=({ struct Cyc_Core_Failure_struct
_temp855; _temp855.tag= Cyc_Core_Failure; _temp855.f1=({ char* _temp856="Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp857; _temp857.curr= _temp856; _temp857.base= _temp856;
_temp857.last_plus_one= _temp856 + 47; _temp857;}); _temp855;}); _temp854;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(({ char* _temp858="(";
struct _tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 2; _temp859;})); Cyc_Absyndump_dumpexps_prec(
20, _temp704); Cyc_Absyndump_dump_nospace(({ char* _temp860=")"; struct
_tagged_string _temp861; _temp861.curr= _temp860; _temp861.base= _temp860;
_temp861.last_plus_one= _temp860 + 2; _temp861;}));} goto _LL567;} _LL591: Cyc_Absyndump_dumpexp_prec(
myprec, _temp712); if( _temp710 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp710))->v));} Cyc_Absyndump_dump_nospace(({
char* _temp862="="; struct _tagged_string _temp863; _temp863.curr= _temp862;
_temp863.base= _temp862; _temp863.last_plus_one= _temp862 + 2; _temp863;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp708); goto _LL567; _LL593: Cyc_Absyndump_dump(({ char* _temp864="++";
struct _tagged_string _temp865; _temp865.curr= _temp864; _temp865.base= _temp864;
_temp865.last_plus_one= _temp864 + 3; _temp865;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp716); goto _LL567; _LL595: Cyc_Absyndump_dump(({ char* _temp866="--";
struct _tagged_string _temp867; _temp867.curr= _temp866; _temp867.base= _temp866;
_temp867.last_plus_one= _temp866 + 3; _temp867;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp720); goto _LL567; _LL597: Cyc_Absyndump_dumpexp_prec( myprec,
_temp724); Cyc_Absyndump_dump(({ char* _temp868="++"; struct _tagged_string
_temp869; _temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 3; _temp869;})); goto _LL567; _LL599: Cyc_Absyndump_dumpexp_prec(
myprec, _temp728); Cyc_Absyndump_dump(({ char* _temp870="--"; struct
_tagged_string _temp871; _temp871.curr= _temp870; _temp871.base= _temp870;
_temp871.last_plus_one= _temp870 + 3; _temp871;})); goto _LL567; _LL601: Cyc_Absyndump_dumpexp_prec(
myprec, _temp734); Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dumpexp_prec(
0, _temp732); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp730); goto _LL567; _LL603: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumpexp_prec(
myprec, _temp738); Cyc_Absyndump_dump_char(( int)','); Cyc_Absyndump_dumpexp_prec(
myprec, _temp736); Cyc_Absyndump_dump_char(( int)')'); goto _LL567; _LL605:
_temp746= _temp742; _temp744= _temp740; goto _LL607; _LL607: Cyc_Absyndump_dumpexp_prec(
myprec, _temp746); Cyc_Absyndump_dump_nospace(({ char* _temp872="("; struct
_tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 2; _temp873;})); Cyc_Absyndump_dumpexps_prec(
20, _temp744); Cyc_Absyndump_dump_nospace(({ char* _temp874=")"; struct
_tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 2; _temp875;})); goto _LL567; _LL609: Cyc_Absyndump_dump(({
char* _temp876="throw"; struct _tagged_string _temp877; _temp877.curr= _temp876;
_temp877.base= _temp876; _temp877.last_plus_one= _temp876 + 6; _temp877;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp748); goto _LL567; _LL611: _temp754= _temp750; goto _LL613; _LL613:
Cyc_Absyndump_dumpexp_prec( inprec, _temp754); goto _LL567; _LL615: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp( _temp758); Cyc_Absyndump_dump_char(( int)')');
Cyc_Absyndump_dumpexp_prec( myprec, _temp756); goto _LL567; _LL617: Cyc_Absyndump_dump_char((
int)'&'); Cyc_Absyndump_dumpexp_prec( myprec, _temp760); goto _LL567; _LL619:
Cyc_Absyndump_dump(({ char* _temp878="new "; struct _tagged_string _temp879;
_temp879.curr= _temp878; _temp879.base= _temp878; _temp879.last_plus_one=
_temp878 + 5; _temp879;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp762); goto
_LL567; _LL621: Cyc_Absyndump_dump(({ char* _temp880="sizeof("; struct
_tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 8; _temp881;})); Cyc_Absyndump_dumptyp(
_temp766); Cyc_Absyndump_dump_char(( int)')'); goto _LL567; _LL623: Cyc_Absyndump_dump(({
char* _temp882="sizeof("; struct _tagged_string _temp883; _temp883.curr=
_temp882; _temp883.base= _temp882; _temp883.last_plus_one= _temp882 + 8;
_temp883;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp768); Cyc_Absyndump_dump_char((
int)')'); goto _LL567; _LL625: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp770); goto _LL567; _LL627: Cyc_Absyndump_dumpexp_prec( myprec,
_temp774); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp772); goto _LL567; _LL629: Cyc_Absyndump_dumpexp_prec( myprec, _temp778);
Cyc_Absyndump_dump_nospace(({ char* _temp884="->"; struct _tagged_string
_temp885; _temp885.curr= _temp884; _temp885.base= _temp884; _temp885.last_plus_one=
_temp884 + 3; _temp885;})); Cyc_Absyndump_dump_nospace(* _temp776); goto _LL567;
_LL631: Cyc_Absyndump_dumpexp_prec( myprec, _temp782); Cyc_Absyndump_dump_char((
int)'['); Cyc_Absyndump_dumpexp( _temp780); Cyc_Absyndump_dump_char(( int)']');
goto _LL567; _LL633: Cyc_Absyndump_dump(({ char* _temp886="$("; struct
_tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 3; _temp887;})); Cyc_Absyndump_dumpexps_prec(
20, _temp784); Cyc_Absyndump_dump_char(( int)')'); goto _LL567; _LL635: Cyc_Absyndump_dump_char((
int)'('); Cyc_Absyndump_dumptyp((* _temp788).f3); Cyc_Absyndump_dump_char(( int)')');((
void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp786,({ char* _temp888="{"; struct _tagged_string
_temp889; _temp889.curr= _temp888; _temp889.base= _temp888; _temp889.last_plus_one=
_temp888 + 2; _temp889;}),({ char* _temp890="}"; struct _tagged_string _temp891;
_temp891.curr= _temp890; _temp891.base= _temp890; _temp891.last_plus_one=
_temp890 + 2; _temp891;}),({ char* _temp892=","; struct _tagged_string _temp893;
_temp893.curr= _temp892; _temp893.base= _temp892; _temp893.last_plus_one=
_temp892 + 2; _temp893;})); goto _LL567; _LL637:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp790,({ char* _temp894="{"; struct _tagged_string _temp895; _temp895.curr=
_temp894; _temp895.base= _temp894; _temp895.last_plus_one= _temp894 + 2;
_temp895;}),({ char* _temp896="}"; struct _tagged_string _temp897; _temp897.curr=
_temp896; _temp897.base= _temp896; _temp897.last_plus_one= _temp896 + 2;
_temp897;}),({ char* _temp898=","; struct _tagged_string _temp899; _temp899.curr=
_temp898; _temp899.base= _temp898; _temp899.last_plus_one= _temp898 + 2;
_temp899;})); goto _LL567; _LL639: Cyc_Absyndump_dump(({ char* _temp900="new {for";
struct _tagged_string _temp901; _temp901.curr= _temp900; _temp901.base= _temp900;
_temp901.last_plus_one= _temp900 + 9; _temp901;})); Cyc_Absyndump_dump_str((*
_temp796->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp794); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp792);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL567; _LL641: Cyc_Absyndump_dumpqvar(
_temp804);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp800,({ char* _temp902="{";
struct _tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 2; _temp903;}),({ char* _temp904="}"; struct
_tagged_string _temp905; _temp905.curr= _temp904; _temp905.base= _temp904;
_temp905.last_plus_one= _temp904 + 2; _temp905;}),({ char* _temp906=","; struct
_tagged_string _temp907; _temp907.curr= _temp906; _temp907.base= _temp906;
_temp907.last_plus_one= _temp906 + 2; _temp907;})); goto _LL567; _LL643:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp806,({ char* _temp908="{"; struct _tagged_string
_temp909; _temp909.curr= _temp908; _temp909.base= _temp908; _temp909.last_plus_one=
_temp908 + 2; _temp909;}),({ char* _temp910="}"; struct _tagged_string _temp911;
_temp911.curr= _temp910; _temp911.base= _temp910; _temp911.last_plus_one=
_temp910 + 2; _temp911;}),({ char* _temp912=","; struct _tagged_string _temp913;
_temp913.curr= _temp912; _temp913.base= _temp912; _temp913.last_plus_one=
_temp912 + 2; _temp913;})); goto _LL567; _LL645: Cyc_Absyndump_dumpqvar(
_temp810->name); if( _temp814 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpexp,
_temp814,({ char* _temp914="("; struct _tagged_string _temp915; _temp915.curr=
_temp914; _temp915.base= _temp914; _temp915.last_plus_one= _temp914 + 2;
_temp915;}),({ char* _temp916=")"; struct _tagged_string _temp917; _temp917.curr=
_temp916; _temp917.base= _temp916; _temp917.last_plus_one= _temp916 + 2;
_temp917;}),({ char* _temp918=","; struct _tagged_string _temp919; _temp919.curr=
_temp918; _temp919.base= _temp918; _temp919.last_plus_one= _temp918 + 2;
_temp919;}));} goto _LL567; _LL647: Cyc_Absyndump_dumpqvar( _temp824); goto
_LL567; _LL649: if( _temp828 != 0){ Cyc_Absyndump_dump(({ char* _temp920="rmalloc(";
struct _tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 9; _temp921;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp828)); Cyc_Absyndump_dump(({ char*
_temp922=","; struct _tagged_string _temp923; _temp923.curr= _temp922; _temp923.base=
_temp922; _temp923.last_plus_one= _temp922 + 2; _temp923;}));} else{ Cyc_Absyndump_dump(({
char* _temp924="malloc("; struct _tagged_string _temp925; _temp925.curr=
_temp924; _temp925.base= _temp924; _temp925.last_plus_one= _temp924 + 8;
_temp925;}));} Cyc_Absyndump_dump(({ char* _temp926="sizeof("; struct
_tagged_string _temp927; _temp927.curr= _temp926; _temp927.base= _temp926;
_temp927.last_plus_one= _temp926 + 8; _temp927;})); Cyc_Absyndump_dumptyp(
_temp826); Cyc_Absyndump_dump(({ char* _temp928="))"; struct _tagged_string
_temp929; _temp929.curr= _temp928; _temp929.base= _temp928; _temp929.last_plus_one=
_temp928 + 3; _temp929;})); goto _LL567; _LL651:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp830,({ char* _temp930="{"; struct _tagged_string _temp931; _temp931.curr=
_temp930; _temp931.base= _temp930; _temp931.last_plus_one= _temp930 + 2;
_temp931;}),({ char* _temp932="}"; struct _tagged_string _temp933; _temp933.curr=
_temp932; _temp933.base= _temp932; _temp933.last_plus_one= _temp932 + 2;
_temp933;}),({ char* _temp934=","; struct _tagged_string _temp935; _temp935.curr=
_temp934; _temp935.base= _temp934; _temp935.last_plus_one= _temp934 + 2;
_temp935;})); goto _LL567; _LL653: Cyc_Absyndump_dump_nospace(({ char* _temp936="({";
struct _tagged_string _temp937; _temp937.curr= _temp936; _temp937.base= _temp936;
_temp937.last_plus_one= _temp936 + 3; _temp937;})); Cyc_Absyndump_dumpstmt(
_temp834); Cyc_Absyndump_dump_nospace(({ char* _temp938="})"; struct
_tagged_string _temp939; _temp939.curr= _temp938; _temp939.base= _temp938;
_temp939.last_plus_one= _temp938 + 3; _temp939;})); goto _LL567; _LL655: Cyc_Absyndump_dump(({
char* _temp940="codegen("; struct _tagged_string _temp941; _temp941.curr=
_temp940; _temp941.base= _temp940; _temp941.last_plus_one= _temp940 + 9;
_temp941;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl* _temp942=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp942->r=( void*)((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp943=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp943[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp944; _temp944.tag= Cyc_Absyn_Fn_d; _temp944.f1= _temp836; _temp944;});
_temp943;})); _temp942->loc= e->loc; _temp942;})); Cyc_Absyndump_dump(({ char*
_temp945=")"; struct _tagged_string _temp946; _temp946.curr= _temp945; _temp946.base=
_temp945; _temp946.last_plus_one= _temp945 + 2; _temp946;})); goto _LL567;
_LL657: Cyc_Absyndump_dump(({ char* _temp947="fill("; struct _tagged_string
_temp948; _temp948.curr= _temp947; _temp948.base= _temp947; _temp948.last_plus_one=
_temp947 + 6; _temp948;})); Cyc_Absyndump_dumpexp( _temp838); Cyc_Absyndump_dump(({
char* _temp949=")"; struct _tagged_string _temp950; _temp950.curr= _temp949;
_temp950.base= _temp949; _temp950.last_plus_one= _temp949 + 2; _temp950;}));
goto _LL567; _LL567:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}}
void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause*
_temp951=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; goto _LL952; _LL952: if( _temp951->where_clause == 0?( void*)(
_temp951->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(({
char* _temp953="default:"; struct _tagged_string _temp954; _temp954.curr=
_temp953; _temp954.base= _temp953; _temp954.last_plus_one= _temp953 + 9;
_temp954;}));} else{ Cyc_Absyndump_dump(({ char* _temp955="case"; struct
_tagged_string _temp956; _temp956.curr= _temp955; _temp956.base= _temp955;
_temp956.last_plus_one= _temp955 + 5; _temp956;})); Cyc_Absyndump_dumppat(
_temp951->pattern); if( _temp951->where_clause != 0){ Cyc_Absyndump_dump(({ char*
_temp957="&&"; struct _tagged_string _temp958; _temp958.curr= _temp957; _temp958.base=
_temp957; _temp958.last_plus_one= _temp957 + 3; _temp958;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _check_null( _temp951->where_clause));} Cyc_Absyndump_dump_nospace(({
char* _temp959=":"; struct _tagged_string _temp960; _temp960.curr= _temp959;
_temp960.base= _temp959; _temp960.last_plus_one= _temp959 + 2; _temp960;}));}
Cyc_Absyndump_dumpstmt( _temp951->body);}} void Cyc_Absyndump_dumpstmt( struct
Cyc_Absyn_Stmt* s){ void* _temp961=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1005; struct Cyc_Absyn_Stmt* _temp1007; struct Cyc_Absyn_Stmt* _temp1009;
struct Cyc_Absyn_Exp* _temp1011; struct Cyc_Absyn_Exp* _temp1013; struct Cyc_Absyn_Stmt*
_temp1015; struct Cyc_Absyn_Stmt* _temp1017; struct Cyc_Absyn_Exp* _temp1019;
struct Cyc_Absyn_Stmt* _temp1021; struct _tuple2 _temp1023; struct Cyc_Absyn_Stmt*
_temp1025; struct Cyc_Absyn_Exp* _temp1027; struct Cyc_Absyn_Stmt* _temp1029;
struct Cyc_Absyn_Stmt* _temp1031; struct Cyc_Absyn_Stmt* _temp1033; struct
_tagged_string* _temp1035; struct Cyc_Absyn_Switch_clause** _temp1037; struct
Cyc_List_List* _temp1039; struct Cyc_Absyn_Switch_clause** _temp1041; struct Cyc_List_List*
_temp1043; struct Cyc_Absyn_Stmt* _temp1045; struct _tuple2 _temp1047; struct
Cyc_Absyn_Stmt* _temp1049; struct Cyc_Absyn_Exp* _temp1051; struct _tuple2
_temp1053; struct Cyc_Absyn_Stmt* _temp1055; struct Cyc_Absyn_Exp* _temp1057;
struct Cyc_Absyn_Exp* _temp1059; struct Cyc_List_List* _temp1061; struct Cyc_Absyn_Exp*
_temp1063; struct Cyc_Absyn_Stmt* _temp1065; struct Cyc_Absyn_Decl* _temp1067;
struct Cyc_Absyn_Stmt* _temp1069; struct Cyc_Absyn_Stmt* _temp1071; struct Cyc_Absyn_Stmt*
_temp1073; struct _tagged_string* _temp1075; struct _tuple2 _temp1077; struct
Cyc_Absyn_Stmt* _temp1079; struct Cyc_Absyn_Exp* _temp1081; struct Cyc_Absyn_Stmt*
_temp1083; struct Cyc_List_List* _temp1085; struct Cyc_Absyn_Stmt* _temp1087;
struct Cyc_Absyn_Stmt* _temp1089; struct Cyc_Absyn_Vardecl* _temp1091; struct
Cyc_Absyn_Tvar* _temp1093; _LL963: if( _temp961 ==( void*) Cyc_Absyn_Skip_s){
goto _LL964;} else{ goto _LL965;} _LL965: if(( unsigned int) _temp961 > 1u?*((
int*) _temp961) == Cyc_Absyn_Exp_s: 0){ _LL1006: _temp1005=(( struct Cyc_Absyn_Exp_s_struct*)
_temp961)->f1; goto _LL966;} else{ goto _LL967;} _LL967: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Seq_s: 0){ _LL1010: _temp1009=((
struct Cyc_Absyn_Seq_s_struct*) _temp961)->f1; goto _LL1008; _LL1008: _temp1007=((
struct Cyc_Absyn_Seq_s_struct*) _temp961)->f2; goto _LL968;} else{ goto _LL969;}
_LL969: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Return_s:
0){ _LL1012: _temp1011=(( struct Cyc_Absyn_Return_s_struct*) _temp961)->f1; if(
_temp1011 == 0){ goto _LL970;} else{ goto _LL971;}} else{ goto _LL971;} _LL971:
if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Return_s: 0){
_LL1014: _temp1013=(( struct Cyc_Absyn_Return_s_struct*) _temp961)->f1; goto
_LL972;} else{ goto _LL973;} _LL973: if(( unsigned int) _temp961 > 1u?*(( int*)
_temp961) == Cyc_Absyn_IfThenElse_s: 0){ _LL1020: _temp1019=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp961)->f1; goto _LL1018; _LL1018: _temp1017=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp961)->f2; goto _LL1016; _LL1016: _temp1015=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp961)->f3; goto _LL974;} else{ goto _LL975;} _LL975: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_While_s: 0){ _LL1024: _temp1023=((
struct Cyc_Absyn_While_s_struct*) _temp961)->f1; _LL1028: _temp1027= _temp1023.f1;
goto _LL1026; _LL1026: _temp1025= _temp1023.f2; goto _LL1022; _LL1022: _temp1021=((
struct Cyc_Absyn_While_s_struct*) _temp961)->f2; goto _LL976;} else{ goto _LL977;}
_LL977: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Break_s:
0){ _LL1030: _temp1029=(( struct Cyc_Absyn_Break_s_struct*) _temp961)->f1; goto
_LL978;} else{ goto _LL979;} _LL979: if(( unsigned int) _temp961 > 1u?*(( int*)
_temp961) == Cyc_Absyn_Continue_s: 0){ _LL1032: _temp1031=(( struct Cyc_Absyn_Continue_s_struct*)
_temp961)->f1; goto _LL980;} else{ goto _LL981;} _LL981: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Goto_s: 0){ _LL1036: _temp1035=((
struct Cyc_Absyn_Goto_s_struct*) _temp961)->f1; goto _LL1034; _LL1034: _temp1033=((
struct Cyc_Absyn_Goto_s_struct*) _temp961)->f2; goto _LL982;} else{ goto _LL983;}
_LL983: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Fallthru_s:
0){ _LL1040: _temp1039=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp961)->f1;
if( _temp1039 == 0){ goto _LL1038;} else{ goto _LL985;} _LL1038: _temp1037=((
struct Cyc_Absyn_Fallthru_s_struct*) _temp961)->f2; goto _LL984;} else{ goto
_LL985;} _LL985: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Fallthru_s:
0){ _LL1044: _temp1043=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp961)->f1;
goto _LL1042; _LL1042: _temp1041=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp961)->f2; goto _LL986;} else{ goto _LL987;} _LL987: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_For_s: 0){ _LL1060: _temp1059=((
struct Cyc_Absyn_For_s_struct*) _temp961)->f1; goto _LL1054; _LL1054: _temp1053=((
struct Cyc_Absyn_For_s_struct*) _temp961)->f2; _LL1058: _temp1057= _temp1053.f1;
goto _LL1056; _LL1056: _temp1055= _temp1053.f2; goto _LL1048; _LL1048: _temp1047=((
struct Cyc_Absyn_For_s_struct*) _temp961)->f3; _LL1052: _temp1051= _temp1047.f1;
goto _LL1050; _LL1050: _temp1049= _temp1047.f2; goto _LL1046; _LL1046: _temp1045=((
struct Cyc_Absyn_For_s_struct*) _temp961)->f4; goto _LL988;} else{ goto _LL989;}
_LL989: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Switch_s:
0){ _LL1064: _temp1063=(( struct Cyc_Absyn_Switch_s_struct*) _temp961)->f1; goto
_LL1062; _LL1062: _temp1061=(( struct Cyc_Absyn_Switch_s_struct*) _temp961)->f2;
goto _LL990;} else{ goto _LL991;} _LL991: if(( unsigned int) _temp961 > 1u?*((
int*) _temp961) == Cyc_Absyn_Decl_s: 0){ _LL1068: _temp1067=(( struct Cyc_Absyn_Decl_s_struct*)
_temp961)->f1; goto _LL1066; _LL1066: _temp1065=(( struct Cyc_Absyn_Decl_s_struct*)
_temp961)->f2; goto _LL992;} else{ goto _LL993;} _LL993: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Cut_s: 0){ _LL1070: _temp1069=((
struct Cyc_Absyn_Cut_s_struct*) _temp961)->f1; goto _LL994;} else{ goto _LL995;}
_LL995: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Splice_s:
0){ _LL1072: _temp1071=(( struct Cyc_Absyn_Splice_s_struct*) _temp961)->f1; goto
_LL996;} else{ goto _LL997;} _LL997: if(( unsigned int) _temp961 > 1u?*(( int*)
_temp961) == Cyc_Absyn_Label_s: 0){ _LL1076: _temp1075=(( struct Cyc_Absyn_Label_s_struct*)
_temp961)->f1; goto _LL1074; _LL1074: _temp1073=(( struct Cyc_Absyn_Label_s_struct*)
_temp961)->f2; goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Do_s: 0){ _LL1084: _temp1083=((
struct Cyc_Absyn_Do_s_struct*) _temp961)->f1; goto _LL1078; _LL1078: _temp1077=((
struct Cyc_Absyn_Do_s_struct*) _temp961)->f2; _LL1082: _temp1081= _temp1077.f1;
goto _LL1080; _LL1080: _temp1079= _temp1077.f2; goto _LL1000;} else{ goto
_LL1001;} _LL1001: if(( unsigned int) _temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_TryCatch_s:
0){ _LL1088: _temp1087=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp961)->f1;
goto _LL1086; _LL1086: _temp1085=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp961)->f2; goto _LL1002;} else{ goto _LL1003;} _LL1003: if(( unsigned int)
_temp961 > 1u?*(( int*) _temp961) == Cyc_Absyn_Region_s: 0){ _LL1094: _temp1093=((
struct Cyc_Absyn_Region_s_struct*) _temp961)->f1; goto _LL1092; _LL1092:
_temp1091=(( struct Cyc_Absyn_Region_s_struct*) _temp961)->f2; goto _LL1090;
_LL1090: _temp1089=(( struct Cyc_Absyn_Region_s_struct*) _temp961)->f3; goto
_LL1004;} else{ goto _LL962;} _LL964: Cyc_Absyndump_dump_semi(); goto _LL962;
_LL966: Cyc_Absyndump_dumpexp( _temp1005); Cyc_Absyndump_dump_semi(); goto
_LL962; _LL968: if( Cyc_Absynpp_is_declaration( _temp1009)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp1009); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp1009);} if( Cyc_Absynpp_is_declaration(
_temp1007)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1007); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt(
_temp1007);} goto _LL962; _LL970: Cyc_Absyndump_dump(({ char* _temp1095="return;";
struct _tagged_string _temp1096; _temp1096.curr= _temp1095; _temp1096.base=
_temp1095; _temp1096.last_plus_one= _temp1095 + 8; _temp1096;})); goto _LL962;
_LL972: Cyc_Absyndump_dump(({ char* _temp1097="return"; struct _tagged_string
_temp1098; _temp1098.curr= _temp1097; _temp1098.base= _temp1097; _temp1098.last_plus_one=
_temp1097 + 7; _temp1098;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)
_check_null( _temp1013)); Cyc_Absyndump_dump_semi(); goto _LL962; _LL974: Cyc_Absyndump_dump(({
char* _temp1099="if("; struct _tagged_string _temp1100; _temp1100.curr=
_temp1099; _temp1100.base= _temp1099; _temp1100.last_plus_one= _temp1099 + 4;
_temp1100;})); Cyc_Absyndump_dumpexp( _temp1019); Cyc_Absyndump_dump_nospace(({
char* _temp1101="){"; struct _tagged_string _temp1102; _temp1102.curr= _temp1101;
_temp1102.base= _temp1101; _temp1102.last_plus_one= _temp1101 + 3; _temp1102;}));
Cyc_Absyndump_dumpstmt( _temp1017); Cyc_Absyndump_dump_char(( int)'}');{ void*
_temp1103=( void*) _temp1015->r; _LL1105: if( _temp1103 ==( void*) Cyc_Absyn_Skip_s){
goto _LL1106;} else{ goto _LL1107;} _LL1107: goto _LL1108; _LL1106: goto _LL1104;
_LL1108: Cyc_Absyndump_dump(({ char* _temp1109="else{"; struct _tagged_string
_temp1110; _temp1110.curr= _temp1109; _temp1110.base= _temp1109; _temp1110.last_plus_one=
_temp1109 + 6; _temp1110;})); Cyc_Absyndump_dumpstmt( _temp1015); Cyc_Absyndump_dump_char((
int)'}'); goto _LL1104; _LL1104:;} goto _LL962; _LL976: Cyc_Absyndump_dump(({
char* _temp1111="while("; struct _tagged_string _temp1112; _temp1112.curr=
_temp1111; _temp1112.base= _temp1111; _temp1112.last_plus_one= _temp1111 + 7;
_temp1112;})); Cyc_Absyndump_dumpexp( _temp1027); Cyc_Absyndump_dump_nospace(({
char* _temp1113=") {"; struct _tagged_string _temp1114; _temp1114.curr=
_temp1113; _temp1114.base= _temp1113; _temp1114.last_plus_one= _temp1113 + 4;
_temp1114;})); Cyc_Absyndump_dumpstmt( _temp1021); Cyc_Absyndump_dump_char(( int)'}');
goto _LL962; _LL978: Cyc_Absyndump_dump(({ char* _temp1115="break;"; struct
_tagged_string _temp1116; _temp1116.curr= _temp1115; _temp1116.base= _temp1115;
_temp1116.last_plus_one= _temp1115 + 7; _temp1116;})); goto _LL962; _LL980: Cyc_Absyndump_dump(({
char* _temp1117="continue;"; struct _tagged_string _temp1118; _temp1118.curr=
_temp1117; _temp1118.base= _temp1117; _temp1118.last_plus_one= _temp1117 + 10;
_temp1118;})); goto _LL962; _LL982: Cyc_Absyndump_dump(({ char* _temp1119="goto";
struct _tagged_string _temp1120; _temp1120.curr= _temp1119; _temp1120.base=
_temp1119; _temp1120.last_plus_one= _temp1119 + 5; _temp1120;})); Cyc_Absyndump_dump_str(
_temp1035); Cyc_Absyndump_dump_semi(); goto _LL962; _LL984: Cyc_Absyndump_dump(({
char* _temp1121="fallthru;"; struct _tagged_string _temp1122; _temp1122.curr=
_temp1121; _temp1122.base= _temp1121; _temp1122.last_plus_one= _temp1121 + 10;
_temp1122;})); goto _LL962; _LL986: Cyc_Absyndump_dump(({ char* _temp1123="fallthru(";
struct _tagged_string _temp1124; _temp1124.curr= _temp1123; _temp1124.base=
_temp1123; _temp1124.last_plus_one= _temp1123 + 10; _temp1124;})); Cyc_Absyndump_dumpexps_prec(
20, _temp1043); Cyc_Absyndump_dump_nospace(({ char* _temp1125=");"; struct
_tagged_string _temp1126; _temp1126.curr= _temp1125; _temp1126.base= _temp1125;
_temp1126.last_plus_one= _temp1125 + 3; _temp1126;})); goto _LL962; _LL988: Cyc_Absyndump_dump(({
char* _temp1127="for("; struct _tagged_string _temp1128; _temp1128.curr=
_temp1127; _temp1128.base= _temp1127; _temp1128.last_plus_one= _temp1127 + 5;
_temp1128;})); Cyc_Absyndump_dumpexp( _temp1059); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp1057); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp( _temp1051); Cyc_Absyndump_dump_nospace(({
char* _temp1129="){"; struct _tagged_string _temp1130; _temp1130.curr= _temp1129;
_temp1130.base= _temp1129; _temp1130.last_plus_one= _temp1129 + 3; _temp1130;}));
Cyc_Absyndump_dumpstmt( _temp1045); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL962; _LL990: Cyc_Absyndump_dump(({ char* _temp1131="switch("; struct
_tagged_string _temp1132; _temp1132.curr= _temp1131; _temp1132.base= _temp1131;
_temp1132.last_plus_one= _temp1131 + 8; _temp1132;})); Cyc_Absyndump_dumpexp(
_temp1063); Cyc_Absyndump_dump_nospace(({ char* _temp1133="){"; struct
_tagged_string _temp1134; _temp1134.curr= _temp1133; _temp1134.base= _temp1133;
_temp1134.last_plus_one= _temp1133 + 3; _temp1134;})); Cyc_Absyndump_dumpswitchclauses(
_temp1061); Cyc_Absyndump_dump_char(( int)'}'); goto _LL962; _LL992: Cyc_Absyndump_dumpdecl(
_temp1067); Cyc_Absyndump_dumpstmt( _temp1065); goto _LL962; _LL994: Cyc_Absyndump_dump(({
char* _temp1135="cut"; struct _tagged_string _temp1136; _temp1136.curr=
_temp1135; _temp1136.base= _temp1135; _temp1136.last_plus_one= _temp1135 + 4;
_temp1136;})); Cyc_Absyndump_dumpstmt( _temp1069); goto _LL962; _LL996: Cyc_Absyndump_dump(({
char* _temp1137="splice"; struct _tagged_string _temp1138; _temp1138.curr=
_temp1137; _temp1138.base= _temp1137; _temp1138.last_plus_one= _temp1137 + 7;
_temp1138;})); Cyc_Absyndump_dumpstmt( _temp1071); goto _LL962; _LL998: if( Cyc_Absynpp_is_declaration(
_temp1073)){ Cyc_Absyndump_dump_str( _temp1075); Cyc_Absyndump_dump_nospace(({
char* _temp1139=": {"; struct _tagged_string _temp1140; _temp1140.curr=
_temp1139; _temp1140.base= _temp1139; _temp1140.last_plus_one= _temp1139 + 4;
_temp1140;})); Cyc_Absyndump_dumpstmt( _temp1073); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dump_str( _temp1075); Cyc_Absyndump_dump_char(( int)':');
Cyc_Absyndump_dumpstmt( _temp1073);} goto _LL962; _LL1000: Cyc_Absyndump_dump(({
char* _temp1141="do {"; struct _tagged_string _temp1142; _temp1142.curr=
_temp1141; _temp1142.base= _temp1141; _temp1142.last_plus_one= _temp1141 + 5;
_temp1142;})); Cyc_Absyndump_dumpstmt( _temp1083); Cyc_Absyndump_dump_nospace(({
char* _temp1143="} while ("; struct _tagged_string _temp1144; _temp1144.curr=
_temp1143; _temp1144.base= _temp1143; _temp1144.last_plus_one= _temp1143 + 10;
_temp1144;})); Cyc_Absyndump_dumpexp( _temp1081); Cyc_Absyndump_dump_nospace(({
char* _temp1145=");"; struct _tagged_string _temp1146; _temp1146.curr= _temp1145;
_temp1146.base= _temp1145; _temp1146.last_plus_one= _temp1145 + 3; _temp1146;}));
goto _LL962; _LL1002: Cyc_Absyndump_dump(({ char* _temp1147="try"; struct
_tagged_string _temp1148; _temp1148.curr= _temp1147; _temp1148.base= _temp1147;
_temp1148.last_plus_one= _temp1147 + 4; _temp1148;})); Cyc_Absyndump_dumpstmt(
_temp1087); Cyc_Absyndump_dump(({ char* _temp1149="catch {"; struct
_tagged_string _temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149;
_temp1150.last_plus_one= _temp1149 + 8; _temp1150;})); Cyc_Absyndump_dumpswitchclauses(
_temp1085); Cyc_Absyndump_dump_char(( int)'}'); goto _LL962; _LL1004: Cyc_Absyndump_dump(({
char* _temp1151="region<"; struct _tagged_string _temp1152; _temp1152.curr=
_temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one= _temp1151 + 8;
_temp1152;})); Cyc_Absyndump_dumptvar( _temp1093); Cyc_Absyndump_dump(({ char*
_temp1153="> "; struct _tagged_string _temp1154; _temp1154.curr= _temp1153;
_temp1154.base= _temp1153; _temp1154.last_plus_one= _temp1153 + 3; _temp1154;}));
Cyc_Absyndump_dumpqvar( _temp1091->name); Cyc_Absyndump_dump(({ char* _temp1155="{";
struct _tagged_string _temp1156; _temp1156.curr= _temp1155; _temp1156.base=
_temp1155; _temp1156.last_plus_one= _temp1155 + 2; _temp1156;})); Cyc_Absyndump_dumpstmt(
_temp1089); Cyc_Absyndump_dump(({ char* _temp1157="}"; struct _tagged_string
_temp1158; _temp1158.curr= _temp1157; _temp1158.base= _temp1157; _temp1158.last_plus_one=
_temp1157 + 2; _temp1158;})); goto _LL962; _LL962:;} struct _tuple8{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,({ char* _temp1159=""; struct
_tagged_string _temp1160; _temp1160.curr= _temp1159; _temp1160.base= _temp1159;
_temp1160.last_plus_one= _temp1159 + 1; _temp1160;}),({ char* _temp1161="=";
struct _tagged_string _temp1162; _temp1162.curr= _temp1161; _temp1162.base=
_temp1161; _temp1162.last_plus_one= _temp1161 + 2; _temp1162;}),({ char*
_temp1163="="; struct _tagged_string _temp1164; _temp1164.curr= _temp1163;
_temp1164.base= _temp1163; _temp1164.last_plus_one= _temp1163 + 2; _temp1164;}));
Cyc_Absyndump_dumppat((* dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*
p){ void* _temp1165=( void*) p->r; int _temp1199; void* _temp1201; int _temp1203;
void* _temp1205; char _temp1207; struct _tagged_string _temp1209; struct Cyc_Absyn_Vardecl*
_temp1211; struct Cyc_List_List* _temp1213; struct Cyc_Absyn_Pat* _temp1215;
struct Cyc_Absyn_Vardecl* _temp1217; struct _tuple0* _temp1219; struct Cyc_List_List*
_temp1221; struct Cyc_List_List* _temp1223; struct _tuple0* _temp1225; struct
Cyc_List_List* _temp1227; struct Cyc_List_List* _temp1229; struct _tuple0*
_temp1231; struct Cyc_List_List* _temp1233; struct Cyc_List_List* _temp1235;
struct Cyc_Core_Opt* _temp1237; struct Cyc_Absyn_Structdecl* _temp1239; struct
Cyc_List_List* _temp1241; struct Cyc_List_List* _temp1243; struct Cyc_Absyn_Tunionfield*
_temp1245; struct Cyc_Absyn_Tuniondecl* _temp1247; struct Cyc_Absyn_Enumfield*
_temp1249; struct Cyc_Absyn_Enumdecl* _temp1251; _LL1167: if( _temp1165 ==( void*)
Cyc_Absyn_Wild_p){ goto _LL1168;} else{ goto _LL1169;} _LL1169: if( _temp1165 ==(
void*) Cyc_Absyn_Null_p){ goto _LL1170;} else{ goto _LL1171;} _LL1171: if((
unsigned int) _temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Int_p: 0){
_LL1202: _temp1201=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1165)->f1;
if( _temp1201 ==( void*) Cyc_Absyn_Signed){ goto _LL1200;} else{ goto _LL1173;}
_LL1200: _temp1199=(( struct Cyc_Absyn_Int_p_struct*) _temp1165)->f2; goto
_LL1172;} else{ goto _LL1173;} _LL1173: if(( unsigned int) _temp1165 > 2u?*((
int*) _temp1165) == Cyc_Absyn_Int_p: 0){ _LL1206: _temp1205=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1165)->f1; if( _temp1205 ==( void*) Cyc_Absyn_Unsigned){
goto _LL1204;} else{ goto _LL1175;} _LL1204: _temp1203=(( struct Cyc_Absyn_Int_p_struct*)
_temp1165)->f2; goto _LL1174;} else{ goto _LL1175;} _LL1175: if(( unsigned int)
_temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Char_p: 0){ _LL1208: _temp1207=((
struct Cyc_Absyn_Char_p_struct*) _temp1165)->f1; goto _LL1176;} else{ goto
_LL1177;} _LL1177: if(( unsigned int) _temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Float_p:
0){ _LL1210: _temp1209=(( struct Cyc_Absyn_Float_p_struct*) _temp1165)->f1; goto
_LL1178;} else{ goto _LL1179;} _LL1179: if(( unsigned int) _temp1165 > 2u?*((
int*) _temp1165) == Cyc_Absyn_Var_p: 0){ _LL1212: _temp1211=(( struct Cyc_Absyn_Var_p_struct*)
_temp1165)->f1; goto _LL1180;} else{ goto _LL1181;} _LL1181: if(( unsigned int)
_temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Tuple_p: 0){ _LL1214: _temp1213=((
struct Cyc_Absyn_Tuple_p_struct*) _temp1165)->f1; goto _LL1182;} else{ goto
_LL1183;} _LL1183: if(( unsigned int) _temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Pointer_p:
0){ _LL1216: _temp1215=(( struct Cyc_Absyn_Pointer_p_struct*) _temp1165)->f1;
goto _LL1184;} else{ goto _LL1185;} _LL1185: if(( unsigned int) _temp1165 > 2u?*((
int*) _temp1165) == Cyc_Absyn_Reference_p: 0){ _LL1218: _temp1217=(( struct Cyc_Absyn_Reference_p_struct*)
_temp1165)->f1; goto _LL1186;} else{ goto _LL1187;} _LL1187: if(( unsigned int)
_temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_UnknownId_p: 0){ _LL1220:
_temp1219=(( struct Cyc_Absyn_UnknownId_p_struct*) _temp1165)->f1; goto _LL1188;}
else{ goto _LL1189;} _LL1189: if(( unsigned int) _temp1165 > 2u?*(( int*)
_temp1165) == Cyc_Absyn_UnknownCall_p: 0){ _LL1226: _temp1225=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1165)->f1; goto _LL1224; _LL1224: _temp1223=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1165)->f2; goto _LL1222; _LL1222: _temp1221=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1165)->f3; goto _LL1190;} else{ goto _LL1191;} _LL1191: if(( unsigned int)
_temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_UnknownFields_p: 0){ _LL1232:
_temp1231=(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp1165)->f1; goto
_LL1230; _LL1230: _temp1229=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1165)->f2; goto _LL1228; _LL1228: _temp1227=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1165)->f3; goto _LL1192;} else{ goto _LL1193;} _LL1193: if(( unsigned int)
_temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Struct_p: 0){ _LL1240:
_temp1239=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f1; goto _LL1238;
_LL1238: _temp1237=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f2; goto
_LL1236; _LL1236: _temp1235=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f3;
goto _LL1234; _LL1234: _temp1233=(( struct Cyc_Absyn_Struct_p_struct*) _temp1165)->f4;
goto _LL1194;} else{ goto _LL1195;} _LL1195: if(( unsigned int) _temp1165 > 2u?*((
int*) _temp1165) == Cyc_Absyn_Tunion_p: 0){ _LL1248: _temp1247=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1165)->f1; goto _LL1246; _LL1246: _temp1245=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1165)->f2; goto _LL1244; _LL1244: _temp1243=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1165)->f3; goto _LL1242; _LL1242: _temp1241=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1165)->f4; goto _LL1196;} else{ goto _LL1197;} _LL1197: if(( unsigned int)
_temp1165 > 2u?*(( int*) _temp1165) == Cyc_Absyn_Enum_p: 0){ _LL1252: _temp1251=((
struct Cyc_Absyn_Enum_p_struct*) _temp1165)->f1; goto _LL1250; _LL1250:
_temp1249=(( struct Cyc_Absyn_Enum_p_struct*) _temp1165)->f2; goto _LL1198;}
else{ goto _LL1166;} _LL1168: Cyc_Absyndump_dump_char(( int)'_'); goto _LL1166;
_LL1170: Cyc_Absyndump_dump(({ char* _temp1253="null"; struct _tagged_string
_temp1254; _temp1254.curr= _temp1253; _temp1254.base= _temp1253; _temp1254.last_plus_one=
_temp1253 + 5; _temp1254;})); goto _LL1166; _LL1172: Cyc_Absyndump_dump( xprintf("%d",
_temp1199)); goto _LL1166; _LL1174: Cyc_Absyndump_dump( xprintf("%u",(
unsigned int) _temp1203)); goto _LL1166; _LL1176: Cyc_Absyndump_dump(({ char*
_temp1255="'"; struct _tagged_string _temp1256; _temp1256.curr= _temp1255;
_temp1256.base= _temp1255; _temp1256.last_plus_one= _temp1255 + 2; _temp1256;}));
Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp1207)); Cyc_Absyndump_dump_nospace(({
char* _temp1257="'"; struct _tagged_string _temp1258; _temp1258.curr= _temp1257;
_temp1258.base= _temp1257; _temp1258.last_plus_one= _temp1257 + 2; _temp1258;}));
goto _LL1166; _LL1178: Cyc_Absyndump_dump( _temp1209); goto _LL1166; _LL1180:
Cyc_Absyndump_dumpqvar( _temp1211->name); goto _LL1166; _LL1182:(( void(*)( void(*
f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1213,({ char* _temp1259="$("; struct _tagged_string _temp1260; _temp1260.curr=
_temp1259; _temp1260.base= _temp1259; _temp1260.last_plus_one= _temp1259 + 3;
_temp1260;}),({ char* _temp1261=")"; struct _tagged_string _temp1262; _temp1262.curr=
_temp1261; _temp1262.base= _temp1261; _temp1262.last_plus_one= _temp1261 + 2;
_temp1262;}),({ char* _temp1263=","; struct _tagged_string _temp1264; _temp1264.curr=
_temp1263; _temp1264.base= _temp1263; _temp1264.last_plus_one= _temp1263 + 2;
_temp1264;})); goto _LL1166; _LL1184: Cyc_Absyndump_dump(({ char* _temp1265="&";
struct _tagged_string _temp1266; _temp1266.curr= _temp1265; _temp1266.base=
_temp1265; _temp1266.last_plus_one= _temp1265 + 2; _temp1266;})); Cyc_Absyndump_dumppat(
_temp1215); goto _LL1166; _LL1186: Cyc_Absyndump_dump(({ char* _temp1267="*";
struct _tagged_string _temp1268; _temp1268.curr= _temp1267; _temp1268.base=
_temp1267; _temp1268.last_plus_one= _temp1267 + 2; _temp1268;})); Cyc_Absyndump_dumpqvar(
_temp1217->name); goto _LL1166; _LL1188: Cyc_Absyndump_dumpqvar( _temp1219);
goto _LL1166; _LL1190: Cyc_Absyndump_dumpqvar( _temp1225); Cyc_Absyndump_dumptvars(
_temp1223);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1221,({ char* _temp1269="(";
struct _tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base=
_temp1269; _temp1270.last_plus_one= _temp1269 + 2; _temp1270;}),({ char*
_temp1271=")"; struct _tagged_string _temp1272; _temp1272.curr= _temp1271;
_temp1272.base= _temp1271; _temp1272.last_plus_one= _temp1271 + 2; _temp1272;}),({
char* _temp1273=","; struct _tagged_string _temp1274; _temp1274.curr= _temp1273;
_temp1274.base= _temp1273; _temp1274.last_plus_one= _temp1273 + 2; _temp1274;}));
goto _LL1166; _LL1192: Cyc_Absyndump_dumpqvar( _temp1231); Cyc_Absyndump_dumptvars(
_temp1229);(( void(*)( void(* f)( struct _tuple8*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp, _temp1227,({ char* _temp1275="{";
struct _tagged_string _temp1276; _temp1276.curr= _temp1275; _temp1276.base=
_temp1275; _temp1276.last_plus_one= _temp1275 + 2; _temp1276;}),({ char*
_temp1277="}"; struct _tagged_string _temp1278; _temp1278.curr= _temp1277;
_temp1278.base= _temp1277; _temp1278.last_plus_one= _temp1277 + 2; _temp1278;}),({
char* _temp1279=","; struct _tagged_string _temp1280; _temp1280.curr= _temp1279;
_temp1280.base= _temp1279; _temp1280.last_plus_one= _temp1279 + 2; _temp1280;}));
goto _LL1166; _LL1194: if( _temp1239->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1239->name))->v);}
Cyc_Absyndump_dumptvars( _temp1235);(( void(*)( void(* f)( struct _tuple8*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1233,({ char* _temp1281="{"; struct _tagged_string _temp1282; _temp1282.curr=
_temp1281; _temp1282.base= _temp1281; _temp1282.last_plus_one= _temp1281 + 2;
_temp1282;}),({ char* _temp1283="}"; struct _tagged_string _temp1284; _temp1284.curr=
_temp1283; _temp1284.base= _temp1283; _temp1284.last_plus_one= _temp1283 + 2;
_temp1284;}),({ char* _temp1285=","; struct _tagged_string _temp1286; _temp1286.curr=
_temp1285; _temp1286.base= _temp1285; _temp1286.last_plus_one= _temp1285 + 2;
_temp1286;})); goto _LL1166; _LL1196: Cyc_Absyndump_dumpqvar( _temp1245->name);
Cyc_Absyndump_dumptvars( _temp1243); if( _temp1241 != 0){(( void(*)( void(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp1241,({ char* _temp1287="("; struct _tagged_string _temp1288; _temp1288.curr=
_temp1287; _temp1288.base= _temp1287; _temp1288.last_plus_one= _temp1287 + 2;
_temp1288;}),({ char* _temp1289=")"; struct _tagged_string _temp1290; _temp1290.curr=
_temp1289; _temp1290.base= _temp1289; _temp1290.last_plus_one= _temp1289 + 2;
_temp1290;}),({ char* _temp1291=","; struct _tagged_string _temp1292; _temp1292.curr=
_temp1291; _temp1292.base= _temp1291; _temp1292.last_plus_one= _temp1291 + 2;
_temp1292;}));} goto _LL1166; _LL1198: Cyc_Absyndump_dumpqvar( _temp1249->name);
goto _LL1166; _LL1166:;} void Cyc_Absyndump_dumptunionfield( struct Cyc_Absyn_Tunionfield*
ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs != 0){ Cyc_Absyndump_dumpargs(
ef->typs);}} void Cyc_Absyndump_dumptunionfields( struct Cyc_List_List* fields){((
void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* l,
struct _tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumptunionfield,
fields,({ char* _temp1293=","; struct _tagged_string _temp1294; _temp1294.curr=
_temp1293; _temp1294.base= _temp1293; _temp1294.last_plus_one= _temp1293 + 2;
_temp1294;}));} void Cyc_Absyndump_dumpenumfield( struct Cyc_Absyn_Enumfield* ef){
Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag != 0){ Cyc_Absyndump_dump(({ char*
_temp1295=" = "; struct _tagged_string _temp1296; _temp1296.curr= _temp1295;
_temp1296.base= _temp1295; _temp1296.last_plus_one= _temp1295 + 4; _temp1296;}));
Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( ef->tag));}} void
Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)( void(* f)(
struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield, fields,({ char* _temp1297=",";
struct _tagged_string _temp1298; _temp1298.curr= _temp1297; _temp1298.base=
_temp1297; _temp1298.last_plus_one= _temp1297 + 2; _temp1298;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields=(( struct Cyc_List_List*)
_check_null( fields))->tl){ struct Cyc_Absyn_Structfield _temp1301; struct Cyc_List_List*
_temp1302; struct Cyc_Core_Opt* _temp1304; void* _temp1306; struct Cyc_Absyn_Tqual
_temp1308; struct _tagged_string* _temp1310; struct Cyc_Absyn_Structfield*
_temp1299=( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd; _temp1301=* _temp1299; _LL1311: _temp1310= _temp1301.name; goto
_LL1309; _LL1309: _temp1308= _temp1301.tq; goto _LL1307; _LL1307: _temp1306=(
void*) _temp1301.type; goto _LL1305; _LL1305: _temp1304= _temp1301.width; goto
_LL1303; _LL1303: _temp1302= _temp1301.attributes; goto _LL1300; _LL1300:(( void(*)(
struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tagged_string*), struct
_tagged_string*)) Cyc_Absyndump_dumptqtd)( _temp1308, _temp1306, Cyc_Absyndump_dump_str,
_temp1310); Cyc_Absyndump_dumpatts( _temp1302); if( _temp1304 != 0){ Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)(( struct Cyc_Core_Opt*)
_check_null( _temp1304))->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
struct _tuple9{ void* f1; struct _tuple0* f2; } ; static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple9* pr){{ void* _temp1312=(* pr).f1; _LL1314: if( _temp1312 ==( void*)
Cyc_Absyn_Unused_att){ goto _LL1315;} else{ goto _LL1316;} _LL1316: if(
_temp1312 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1317;} else{ goto _LL1318;}
_LL1318: if( _temp1312 ==( void*) Cyc_Absyn_Cdecl_att){ goto _LL1319;} else{
goto _LL1320;} _LL1320: if( _temp1312 ==( void*) Cyc_Absyn_Fastcall_att){ goto
_LL1321;} else{ goto _LL1322;} _LL1322: goto _LL1323; _LL1315: goto _LL1313;
_LL1317: Cyc_Absyndump_dump(({ char* _temp1324="_stdcall"; struct _tagged_string
_temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one=
_temp1324 + 9; _temp1325;})); goto _LL1313; _LL1319: Cyc_Absyndump_dump(({ char*
_temp1326="_cdecl"; struct _tagged_string _temp1327; _temp1327.curr= _temp1326;
_temp1327.base= _temp1326; _temp1327.last_plus_one= _temp1326 + 7; _temp1327;}));
goto _LL1313; _LL1321: Cyc_Absyndump_dump(({ char* _temp1328="_fastcall"; struct
_tagged_string _temp1329; _temp1329.curr= _temp1328; _temp1329.base= _temp1328;
_temp1329.last_plus_one= _temp1328 + 10; _temp1329;})); goto _LL1313; _LL1323:
goto _LL1313; _LL1313:;} Cyc_Absyndump_dumpqvar((* pr).f2);} static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl* fd){ Cyc_Absyndump_dump_callconv( fd->attributes); Cyc_Absyndump_dumpqvar(
fd->name);} static void Cyc_Absyndump_dumpids( struct Cyc_List_List* vds){ for(
0; vds != 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd)->name);
if((( struct Cyc_List_List*) _check_null( vds))->tl != 0){ Cyc_Absyndump_dump_char((
int)',');}}} void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void*
_temp1330=( void*) d->r; struct Cyc_Absyn_Fndecl* _temp1356; struct Cyc_Absyn_Structdecl*
_temp1358; struct Cyc_Absyn_Uniondecl* _temp1360; struct Cyc_Absyn_Vardecl*
_temp1362; struct Cyc_Absyn_Vardecl _temp1364; struct Cyc_List_List* _temp1365;
struct Cyc_Core_Opt* _temp1367; struct Cyc_Absyn_Exp* _temp1369; void* _temp1371;
struct Cyc_Absyn_Tqual _temp1373; struct _tuple0* _temp1375; void* _temp1377;
struct Cyc_Absyn_Tuniondecl* _temp1379; struct Cyc_Absyn_Tuniondecl _temp1381;
int _temp1382; struct Cyc_Core_Opt* _temp1384; struct Cyc_List_List* _temp1386;
struct _tuple0* _temp1388; void* _temp1390; struct Cyc_Absyn_Enumdecl* _temp1392;
struct Cyc_Absyn_Enumdecl _temp1394; struct Cyc_Core_Opt* _temp1395; struct
_tuple0* _temp1397; void* _temp1399; int _temp1401; struct Cyc_Absyn_Exp*
_temp1403; struct Cyc_Core_Opt* _temp1405; struct Cyc_Core_Opt* _temp1407;
struct Cyc_Absyn_Pat* _temp1409; struct Cyc_List_List* _temp1411; struct Cyc_Absyn_Typedefdecl*
_temp1413; struct Cyc_List_List* _temp1415; struct _tagged_string* _temp1417;
struct Cyc_List_List* _temp1419; struct _tuple0* _temp1421; struct Cyc_List_List*
_temp1423; _LL1332: if(*(( int*) _temp1330) == Cyc_Absyn_Fn_d){ _LL1357:
_temp1356=(( struct Cyc_Absyn_Fn_d_struct*) _temp1330)->f1; goto _LL1333;} else{
goto _LL1334;} _LL1334: if(*(( int*) _temp1330) == Cyc_Absyn_Struct_d){ _LL1359:
_temp1358=(( struct Cyc_Absyn_Struct_d_struct*) _temp1330)->f1; goto _LL1335;}
else{ goto _LL1336;} _LL1336: if(*(( int*) _temp1330) == Cyc_Absyn_Union_d){
_LL1361: _temp1360=(( struct Cyc_Absyn_Union_d_struct*) _temp1330)->f1; goto
_LL1337;} else{ goto _LL1338;} _LL1338: if(*(( int*) _temp1330) == Cyc_Absyn_Var_d){
_LL1363: _temp1362=(( struct Cyc_Absyn_Var_d_struct*) _temp1330)->f1; _temp1364=*
_temp1362; _LL1378: _temp1377=( void*) _temp1364.sc; goto _LL1376; _LL1376:
_temp1375= _temp1364.name; goto _LL1374; _LL1374: _temp1373= _temp1364.tq; goto
_LL1372; _LL1372: _temp1371=( void*) _temp1364.type; goto _LL1370; _LL1370:
_temp1369= _temp1364.initializer; goto _LL1368; _LL1368: _temp1367= _temp1364.rgn;
goto _LL1366; _LL1366: _temp1365= _temp1364.attributes; goto _LL1339;} else{
goto _LL1340;} _LL1340: if(*(( int*) _temp1330) == Cyc_Absyn_Tunion_d){ _LL1380:
_temp1379=(( struct Cyc_Absyn_Tunion_d_struct*) _temp1330)->f1; _temp1381=*
_temp1379; _LL1391: _temp1390=( void*) _temp1381.sc; goto _LL1389; _LL1389:
_temp1388= _temp1381.name; goto _LL1387; _LL1387: _temp1386= _temp1381.tvs; goto
_LL1385; _LL1385: _temp1384= _temp1381.fields; goto _LL1383; _LL1383: _temp1382=
_temp1381.is_xtunion; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(*(( int*)
_temp1330) == Cyc_Absyn_Enum_d){ _LL1393: _temp1392=(( struct Cyc_Absyn_Enum_d_struct*)
_temp1330)->f1; _temp1394=* _temp1392; _LL1400: _temp1399=( void*) _temp1394.sc;
goto _LL1398; _LL1398: _temp1397= _temp1394.name; goto _LL1396; _LL1396:
_temp1395= _temp1394.fields; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(*((
int*) _temp1330) == Cyc_Absyn_Let_d){ _LL1410: _temp1409=(( struct Cyc_Absyn_Let_d_struct*)
_temp1330)->f1; goto _LL1408; _LL1408: _temp1407=(( struct Cyc_Absyn_Let_d_struct*)
_temp1330)->f2; goto _LL1406; _LL1406: _temp1405=(( struct Cyc_Absyn_Let_d_struct*)
_temp1330)->f3; goto _LL1404; _LL1404: _temp1403=(( struct Cyc_Absyn_Let_d_struct*)
_temp1330)->f4; goto _LL1402; _LL1402: _temp1401=(( struct Cyc_Absyn_Let_d_struct*)
_temp1330)->f5; goto _LL1345;} else{ goto _LL1346;} _LL1346: if(*(( int*)
_temp1330) == Cyc_Absyn_Letv_d){ _LL1412: _temp1411=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1330)->f1; goto _LL1347;} else{ goto _LL1348;} _LL1348: if(*(( int*)
_temp1330) == Cyc_Absyn_Typedef_d){ _LL1414: _temp1413=(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1330)->f1; goto _LL1349;} else{ goto _LL1350;} _LL1350: if(*(( int*)
_temp1330) == Cyc_Absyn_Namespace_d){ _LL1418: _temp1417=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1330)->f1; goto _LL1416; _LL1416: _temp1415=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1330)->f2; goto _LL1351;} else{ goto _LL1352;} _LL1352: if(*(( int*)
_temp1330) == Cyc_Absyn_Using_d){ _LL1422: _temp1421=(( struct Cyc_Absyn_Using_d_struct*)
_temp1330)->f1; goto _LL1420; _LL1420: _temp1419=(( struct Cyc_Absyn_Using_d_struct*)
_temp1330)->f2; goto _LL1353;} else{ goto _LL1354;} _LL1354: if(*(( int*)
_temp1330) == Cyc_Absyn_ExternC_d){ _LL1424: _temp1423=(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1330)->f1; goto _LL1355;} else{ goto _LL1331;} _LL1333: if( Cyc_Absynpp_to_VC){
Cyc_Absyndump_dumpatts( _temp1356->attributes);} if( _temp1356->is_inline){ if(
Cyc_Absynpp_to_VC){ Cyc_Absyndump_dump(({ char* _temp1425="__inline"; struct
_tagged_string _temp1426; _temp1426.curr= _temp1425; _temp1426.base= _temp1425;
_temp1426.last_plus_one= _temp1425 + 9; _temp1426;}));} else{ Cyc_Absyndump_dump(({
char* _temp1427="inline"; struct _tagged_string _temp1428; _temp1428.curr=
_temp1427; _temp1428.base= _temp1427; _temp1428.last_plus_one= _temp1427 + 7;
_temp1428;}));}} Cyc_Absyndump_dumpscope(( void*) _temp1356->sc);{ void* t=(
void*)({ struct Cyc_Absyn_FnType_struct* _temp1430=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1430[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1431; _temp1431.tag= Cyc_Absyn_FnType; _temp1431.f1=({ struct Cyc_Absyn_FnInfo
_temp1432; _temp1432.tvars= _temp1356->tvs; _temp1432.effect= _temp1356->effect;
_temp1432.ret_typ=( void*)(( void*) _temp1356->ret_type); _temp1432.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1356->args); _temp1432.varargs=
_temp1356->varargs; _temp1432.attributes= 0; _temp1432;}); _temp1431;});
_temp1430;});(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)(({ struct Cyc_Absyn_Tqual
_temp1429; _temp1429.q_const= 0; _temp1429.q_volatile= 0; _temp1429.q_restrict=
0; _temp1429;}), t, Cyc_Absynpp_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_temp1356); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1356->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1331;} _LL1335:
Cyc_Absyndump_dumpscope(( void*) _temp1358->sc); Cyc_Absyndump_dump(({ char*
_temp1433="struct"; struct _tagged_string _temp1434; _temp1434.curr= _temp1433;
_temp1434.base= _temp1433; _temp1434.last_plus_one= _temp1433 + 7; _temp1434;}));
if( _temp1358->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)(( struct
Cyc_Core_Opt*) _check_null( _temp1358->name))->v);} Cyc_Absyndump_dumptvars(
_temp1358->tvs); if( _temp1358->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1358->fields))->v); Cyc_Absyndump_dump(({
char* _temp1435="}"; struct _tagged_string _temp1436; _temp1436.curr= _temp1435;
_temp1436.base= _temp1435; _temp1436.last_plus_one= _temp1435 + 2; _temp1436;}));
Cyc_Absyndump_dumpatts( _temp1358->attributes); Cyc_Absyndump_dump(({ char*
_temp1437=";"; struct _tagged_string _temp1438; _temp1438.curr= _temp1437;
_temp1438.base= _temp1437; _temp1438.last_plus_one= _temp1437 + 2; _temp1438;}));}
goto _LL1331; _LL1337: Cyc_Absyndump_dumpscope(( void*) _temp1360->sc); Cyc_Absyndump_dump(({
char* _temp1439="union"; struct _tagged_string _temp1440; _temp1440.curr=
_temp1439; _temp1440.base= _temp1439; _temp1440.last_plus_one= _temp1439 + 6;
_temp1440;})); if( _temp1360->name != 0){ Cyc_Absyndump_dumpqvar(( struct
_tuple0*)(( struct Cyc_Core_Opt*) _check_null( _temp1360->name))->v);} Cyc_Absyndump_dumptvars(
_temp1360->tvs); if( _temp1360->fields == 0){ Cyc_Absyndump_dump_semi();} else{
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1360->fields))->v); Cyc_Absyndump_dump(({
char* _temp1441="}"; struct _tagged_string _temp1442; _temp1442.curr= _temp1441;
_temp1442.base= _temp1441; _temp1442.last_plus_one= _temp1441 + 2; _temp1442;}));
Cyc_Absyndump_dumpatts( _temp1360->attributes); Cyc_Absyndump_dump(({ char*
_temp1443=";"; struct _tagged_string _temp1444; _temp1444.curr= _temp1443;
_temp1444.base= _temp1443; _temp1444.last_plus_one= _temp1443 + 2; _temp1444;}));}
goto _LL1331; _LL1339: if( Cyc_Absynpp_to_VC){ Cyc_Absyndump_dumpatts( _temp1365);
Cyc_Absyndump_dumpscope( _temp1377);{ struct Cyc_List_List* _temp1447; void*
_temp1449; struct Cyc_Absyn_Tqual _temp1451; struct _tuple4 _temp1445= Cyc_Absynpp_to_tms(
_temp1373, _temp1371); _LL1452: _temp1451= _temp1445.f1; goto _LL1450; _LL1450:
_temp1449= _temp1445.f2; goto _LL1448; _LL1448: _temp1447= _temp1445.f3; goto
_LL1446; _LL1446: Cyc_Absyndump_dumptq( _temp1451);{ void* call_conv=( void*)
Cyc_Absyn_Unused_att;{ struct Cyc_List_List* tms2= _temp1447; for( 0; tms2 != 0;
tms2=(( struct Cyc_List_List*) _check_null( tms2))->tl){ void* _temp1453=( void*)((
struct Cyc_List_List*) _check_null( tms2))->hd; struct Cyc_List_List* _temp1459;
struct Cyc_Position_Segment* _temp1461; _LL1455: if(( unsigned int) _temp1453 >
1u?*(( int*) _temp1453) == Cyc_Absyn_Attributes_mod: 0){ _LL1462: _temp1461=((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1453)->f1; goto _LL1460; _LL1460:
_temp1459=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1453)->f2; goto
_LL1456;} else{ goto _LL1457;} _LL1457: goto _LL1458; _LL1456: for( 0; _temp1459
!= 0; _temp1459=(( struct Cyc_List_List*) _check_null( _temp1459))->tl){ void*
_temp1463=( void*)(( struct Cyc_List_List*) _check_null( _temp1459))->hd;
_LL1465: if( _temp1463 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL1466;} else{
goto _LL1467;} _LL1467: if( _temp1463 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL1468;} else{ goto _LL1469;} _LL1469: if( _temp1463 ==( void*) Cyc_Absyn_Fastcall_att){
goto _LL1470;} else{ goto _LL1471;} _LL1471: goto _LL1472; _LL1466: call_conv=(
void*) Cyc_Absyn_Stdcall_att; goto _LL1464; _LL1468: call_conv=( void*) Cyc_Absyn_Cdecl_att;
goto _LL1464; _LL1470: call_conv=( void*) Cyc_Absyn_Fastcall_att; goto _LL1464;
_LL1472: goto _LL1464; _LL1464:;} goto _LL1454; _LL1458: goto _LL1454; _LL1454:;}}
Cyc_Absyndump_dumptq( _temp1451); Cyc_Absyndump_dumpntyp( _temp1449);{ struct
_tuple9 _temp1474=({ struct _tuple9 _temp1473; _temp1473.f1= call_conv;
_temp1473.f2= _temp1375; _temp1473;}); goto _LL1475; _LL1475:(( void(*)( struct
Cyc_List_List* tms, void(* f)( struct _tuple9*), struct _tuple9* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1447),
Cyc_Absyndump_dump_callconv_qvar,& _temp1474);}}}} else{ Cyc_Absyndump_dumpscope(
_temp1377);(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)( struct _tuple0*),
struct _tuple0*)) Cyc_Absyndump_dumptqtd)( _temp1373, _temp1371, Cyc_Absyndump_dumpqvar,
_temp1375); Cyc_Absyndump_dumpatts( _temp1365);} if( _temp1369 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*) _check_null( _temp1369));}
Cyc_Absyndump_dump_semi(); goto _LL1331; _LL1341: Cyc_Absyndump_dumpscope(
_temp1390); if( _temp1382){ Cyc_Absyndump_dump(({ char* _temp1476="xtunion ";
struct _tagged_string _temp1477; _temp1477.curr= _temp1476; _temp1477.base=
_temp1476; _temp1477.last_plus_one= _temp1476 + 9; _temp1477;}));} else{ Cyc_Absyndump_dump(({
char* _temp1478="tunion "; struct _tagged_string _temp1479; _temp1479.curr=
_temp1478; _temp1479.base= _temp1478; _temp1479.last_plus_one= _temp1478 + 8;
_temp1479;}));} Cyc_Absyndump_dumpqvar( _temp1388); Cyc_Absyndump_dumptvars(
_temp1386); if( _temp1384 == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumptunionfields(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp1384))->v); Cyc_Absyndump_dump_nospace(({ char* _temp1480="};";
struct _tagged_string _temp1481; _temp1481.curr= _temp1480; _temp1481.base=
_temp1480; _temp1481.last_plus_one= _temp1480 + 3; _temp1481;}));} goto _LL1331;
_LL1343: Cyc_Absyndump_dumpscope( _temp1399); Cyc_Absyndump_dump(({ char*
_temp1482="enum "; struct _tagged_string _temp1483; _temp1483.curr= _temp1482;
_temp1483.base= _temp1482; _temp1483.last_plus_one= _temp1482 + 6; _temp1483;}));
Cyc_Absyndump_dumpqvar( _temp1397); if( _temp1395 == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields(( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1395))->v); Cyc_Absyndump_dump_nospace(({
char* _temp1484="};"; struct _tagged_string _temp1485; _temp1485.curr= _temp1484;
_temp1485.base= _temp1484; _temp1485.last_plus_one= _temp1484 + 3; _temp1485;}));}
return; _LL1345: Cyc_Absyndump_dump(({ char* _temp1486="let"; struct
_tagged_string _temp1487; _temp1487.curr= _temp1486; _temp1487.base= _temp1486;
_temp1487.last_plus_one= _temp1486 + 4; _temp1487;})); Cyc_Absyndump_dumppat(
_temp1409); Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp( _temp1403);
Cyc_Absyndump_dump_semi(); goto _LL1331; _LL1347: Cyc_Absyndump_dump(({ char*
_temp1488="let "; struct _tagged_string _temp1489; _temp1489.curr= _temp1488;
_temp1489.base= _temp1488; _temp1489.last_plus_one= _temp1488 + 5; _temp1489;}));
Cyc_Absyndump_dumpids( _temp1411); Cyc_Absyndump_dump_semi(); goto _LL1331;
_LL1349: Cyc_Absyndump_dump(({ char* _temp1490="typedef"; struct _tagged_string
_temp1491; _temp1491.curr= _temp1490; _temp1491.base= _temp1490; _temp1491.last_plus_one=
_temp1490 + 8; _temp1491;}));(( void(*)( struct Cyc_Absyn_Tqual, void*, void(* f)(
struct Cyc_Absyn_Typedefdecl*), struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _temp1492; _temp1492.q_const= 0; _temp1492.q_volatile= 0;
_temp1492.q_restrict= 0; _temp1492;}),( void*) _temp1413->defn, Cyc_Absyndump_dumptypedefname,
_temp1413); Cyc_Absyndump_dump_semi(); goto _LL1331; _LL1351: Cyc_Absyndump_dump(({
char* _temp1493="namespace"; struct _tagged_string _temp1494; _temp1494.curr=
_temp1493; _temp1494.base= _temp1493; _temp1494.last_plus_one= _temp1493 + 10;
_temp1494;})); Cyc_Absyndump_dump_str( _temp1417); Cyc_Absyndump_dump_char(( int)'{');
for( 0; _temp1415 != 0; _temp1415=(( struct Cyc_List_List*) _check_null(
_temp1415))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1415))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1331;
_LL1353: Cyc_Absyndump_dump(({ char* _temp1495="using"; struct _tagged_string
_temp1496; _temp1496.curr= _temp1495; _temp1496.base= _temp1495; _temp1496.last_plus_one=
_temp1495 + 6; _temp1496;})); Cyc_Absyndump_dumpqvar( _temp1421); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1419 != 0; _temp1419=(( struct Cyc_List_List*)
_check_null( _temp1419))->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp1419))->hd);} Cyc_Absyndump_dump_char((
int)'}'); goto _LL1331; _LL1355: Cyc_Absyndump_dump(({ char* _temp1497="extern \"C\" {";
struct _tagged_string _temp1498; _temp1498.curr= _temp1497; _temp1498.base=
_temp1497; _temp1498.last_plus_one= _temp1497 + 13; _temp1498;})); for( 0;
_temp1423 != 0; _temp1423=(( struct Cyc_List_List*) _check_null( _temp1423))->tl){
Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*)
_check_null( _temp1423))->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1331;
_LL1331:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp* e){
unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i != 1){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char(( int)'}');}} void
Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)( void*), void* a){
if( tms == 0){ f( a); return;}{ void* _temp1499=( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd; struct Cyc_Absyn_Tqual _temp1517; void* _temp1519; void*
_temp1521; struct Cyc_Absyn_Exp* _temp1523; struct Cyc_Absyn_Tqual _temp1525;
void* _temp1527; void* _temp1529; struct Cyc_Absyn_Exp* _temp1531; struct Cyc_Absyn_Tqual
_temp1533; void* _temp1535; void* _temp1537; struct Cyc_Absyn_Tqual _temp1539;
void* _temp1541; struct Cyc_Absyn_Tvar* _temp1543; void* _temp1545; struct Cyc_Absyn_Exp*
_temp1547; struct Cyc_Absyn_Tqual _temp1549; void* _temp1551; struct Cyc_Absyn_Tvar*
_temp1553; void* _temp1555; struct Cyc_Absyn_Exp* _temp1557; struct Cyc_Absyn_Tqual
_temp1559; void* _temp1561; struct Cyc_Absyn_Tvar* _temp1563; void* _temp1565;
struct Cyc_Absyn_Tqual _temp1567; void* _temp1569; void* _temp1571; _LL1501: if((
unsigned int) _temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){
_LL1522: _temp1521=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1;
if(( unsigned int) _temp1521 > 1u?*(( int*) _temp1521) == Cyc_Absyn_Nullable_ps:
0){ _LL1524: _temp1523=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1521)->f1;
goto _LL1520;} else{ goto _LL1503;} _LL1520: _temp1519=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; if( _temp1519 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1518;}
else{ goto _LL1503;} _LL1518: _temp1517=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1502;} else{ goto _LL1503;} _LL1503: if(( unsigned int)
_temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){ _LL1530:
_temp1529=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1; if((
unsigned int) _temp1529 > 1u?*(( int*) _temp1529) == Cyc_Absyn_NonNullable_ps: 0){
_LL1532: _temp1531=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1529)->f1;
goto _LL1528;} else{ goto _LL1505;} _LL1528: _temp1527=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; if( _temp1527 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1526;}
else{ goto _LL1505;} _LL1526: _temp1525=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1504;} else{ goto _LL1505;} _LL1505: if(( unsigned int)
_temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){ _LL1538:
_temp1537=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1; if(
_temp1537 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1536;} else{ goto
_LL1507;} _LL1536: _temp1535=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; if( _temp1535 ==( void*) Cyc_Absyn_HeapRgn){ goto _LL1534;}
else{ goto _LL1507;} _LL1534: _temp1533=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1506;} else{ goto _LL1507;} _LL1507: if(( unsigned int)
_temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){ _LL1546:
_temp1545=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1; if((
unsigned int) _temp1545 > 1u?*(( int*) _temp1545) == Cyc_Absyn_Nullable_ps: 0){
_LL1548: _temp1547=(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1545)->f1; goto
_LL1542;} else{ goto _LL1509;} _LL1542: _temp1541=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; if(( unsigned int) _temp1541 > 4u?*(( int*) _temp1541) == Cyc_Absyn_VarType:
0){ _LL1544: _temp1543=(( struct Cyc_Absyn_VarType_struct*) _temp1541)->f1; goto
_LL1540;} else{ goto _LL1509;} _LL1540: _temp1539=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1508;} else{ goto _LL1509;} _LL1509: if(( unsigned int)
_temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){ _LL1556:
_temp1555=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1; if((
unsigned int) _temp1555 > 1u?*(( int*) _temp1555) == Cyc_Absyn_NonNullable_ps: 0){
_LL1558: _temp1557=(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1555)->f1;
goto _LL1552;} else{ goto _LL1511;} _LL1552: _temp1551=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; if(( unsigned int) _temp1551 > 4u?*(( int*) _temp1551) == Cyc_Absyn_VarType:
0){ _LL1554: _temp1553=(( struct Cyc_Absyn_VarType_struct*) _temp1551)->f1; goto
_LL1550;} else{ goto _LL1511;} _LL1550: _temp1549=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1510;} else{ goto _LL1511;} _LL1511: if(( unsigned int)
_temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){ _LL1566:
_temp1565=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1; if(
_temp1565 ==( void*) Cyc_Absyn_TaggedArray_ps){ goto _LL1562;} else{ goto
_LL1513;} _LL1562: _temp1561=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; if(( unsigned int) _temp1561 > 4u?*(( int*) _temp1561) == Cyc_Absyn_VarType:
0){ _LL1564: _temp1563=(( struct Cyc_Absyn_VarType_struct*) _temp1561)->f1; goto
_LL1560;} else{ goto _LL1513;} _LL1560: _temp1559=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1512;} else{ goto _LL1513;} _LL1513: if(( unsigned int)
_temp1499 > 1u?*(( int*) _temp1499) == Cyc_Absyn_Pointer_mod: 0){ _LL1572:
_temp1571=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1499)->f1; goto
_LL1570; _LL1570: _temp1569=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f2; goto _LL1568; _LL1568: _temp1567=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1499)->f3; goto _LL1514;} else{ goto _LL1515;} _LL1515: goto _LL1516;
_LL1502: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1523);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1504: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1531);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f, a);
return; _LL1506: Cyc_Absyndump_dump_char(( int)'?');(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); return; _LL1508: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1547); Cyc_Absyndump_dump_str( _temp1543->name);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*) _check_null( tms))->tl, f, a); return; _LL1510: Cyc_Absyndump_dump_char((
int)'@'); Cyc_Absyndump_dump_upperbound( _temp1557); Cyc_Absyndump_dump_str(
_temp1553->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1512: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1563->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*) _check_null( tms))->tl, f,
a); return; _LL1514:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1573=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1573[ 0]=({ struct Cyc_Core_Impossible_struct _temp1574; _temp1574.tag= Cyc_Core_Impossible;
_temp1574.f1=({ char* _temp1575="dumptms: bad Pointer_mod"; struct
_tagged_string _temp1576; _temp1576.curr= _temp1575; _temp1576.base= _temp1575;
_temp1576.last_plus_one= _temp1575 + 25; _temp1576;}); _temp1574;}); _temp1573;}));
_LL1516: { int next_is_pointer= 0; if((( struct Cyc_List_List*) _check_null( tms))->tl
!= 0){ void* _temp1577=( void*)(( struct Cyc_List_List*) _check_null((( struct
Cyc_List_List*) _check_null( tms))->tl))->hd; struct Cyc_Absyn_Tqual _temp1583;
void* _temp1585; void* _temp1587; _LL1579: if(( unsigned int) _temp1577 > 1u?*((
int*) _temp1577) == Cyc_Absyn_Pointer_mod: 0){ _LL1588: _temp1587=( void*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1577)->f1; goto _LL1586; _LL1586:
_temp1585=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1577)->f2; goto
_LL1584; _LL1584: _temp1583=(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1577)->f3;
goto _LL1580;} else{ goto _LL1581;} _LL1581: goto _LL1582; _LL1580:
next_is_pointer= 1; goto _LL1578; _LL1582: goto _LL1578; _LL1578:;} if(
next_is_pointer){ Cyc_Absyndump_dump_char(( int)'(');}(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)((( struct Cyc_List_List*)
_check_null( tms))->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1589=( void*)(( struct Cyc_List_List*) _check_null( tms))->hd;
struct Cyc_Absyn_Exp* _temp1605; void* _temp1607; struct Cyc_Core_Opt* _temp1609;
int _temp1611; struct Cyc_List_List* _temp1613; void* _temp1615; struct Cyc_Position_Segment*
_temp1617; struct Cyc_List_List* _temp1619; int _temp1621; struct Cyc_Position_Segment*
_temp1623; struct Cyc_List_List* _temp1625; struct Cyc_List_List* _temp1627;
struct Cyc_Position_Segment* _temp1629; struct Cyc_Absyn_Tqual _temp1631; void*
_temp1633; void* _temp1635; _LL1591: if( _temp1589 ==( void*) Cyc_Absyn_Carray_mod){
goto _LL1592;} else{ goto _LL1593;} _LL1593: if(( unsigned int) _temp1589 > 1u?*((
int*) _temp1589) == Cyc_Absyn_ConstArray_mod: 0){ _LL1606: _temp1605=(( struct
Cyc_Absyn_ConstArray_mod_struct*) _temp1589)->f1; goto _LL1594;} else{ goto
_LL1595;} _LL1595: if(( unsigned int) _temp1589 > 1u?*(( int*) _temp1589) == Cyc_Absyn_Function_mod:
0){ _LL1608: _temp1607=( void*)(( struct Cyc_Absyn_Function_mod_struct*)
_temp1589)->f1; if(*(( int*) _temp1607) == Cyc_Absyn_WithTypes){ _LL1614:
_temp1613=(( struct Cyc_Absyn_WithTypes_struct*) _temp1607)->f1; goto _LL1612;
_LL1612: _temp1611=(( struct Cyc_Absyn_WithTypes_struct*) _temp1607)->f2; goto
_LL1610; _LL1610: _temp1609=(( struct Cyc_Absyn_WithTypes_struct*) _temp1607)->f3;
goto _LL1596;} else{ goto _LL1597;}} else{ goto _LL1597;} _LL1597: if((
unsigned int) _temp1589 > 1u?*(( int*) _temp1589) == Cyc_Absyn_Function_mod: 0){
_LL1616: _temp1615=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1589)->f1;
if(*(( int*) _temp1615) == Cyc_Absyn_NoTypes){ _LL1620: _temp1619=(( struct Cyc_Absyn_NoTypes_struct*)
_temp1615)->f1; goto _LL1618; _LL1618: _temp1617=(( struct Cyc_Absyn_NoTypes_struct*)
_temp1615)->f2; goto _LL1598;} else{ goto _LL1599;}} else{ goto _LL1599;}
_LL1599: if(( unsigned int) _temp1589 > 1u?*(( int*) _temp1589) == Cyc_Absyn_TypeParams_mod:
0){ _LL1626: _temp1625=(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp1589)->f1;
goto _LL1624; _LL1624: _temp1623=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1589)->f2; goto _LL1622; _LL1622: _temp1621=(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1589)->f3; goto _LL1600;} else{ goto _LL1601;} _LL1601: if(( unsigned int)
_temp1589 > 1u?*(( int*) _temp1589) == Cyc_Absyn_Attributes_mod: 0){ _LL1630:
_temp1629=(( struct Cyc_Absyn_Attributes_mod_struct*) _temp1589)->f1; goto
_LL1628; _LL1628: _temp1627=(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1589)->f2; goto _LL1602;} else{ goto _LL1603;} _LL1603: if(( unsigned int)
_temp1589 > 1u?*(( int*) _temp1589) == Cyc_Absyn_Pointer_mod: 0){ _LL1636:
_temp1635=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1589)->f1; goto
_LL1634; _LL1634: _temp1633=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1589)->f2; goto _LL1632; _LL1632: _temp1631=(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1589)->f3; goto _LL1604;} else{ goto _LL1590;} _LL1592: Cyc_Absyndump_dump(({
char* _temp1637="[]"; struct _tagged_string _temp1638; _temp1638.curr= _temp1637;
_temp1638.base= _temp1637; _temp1638.last_plus_one= _temp1637 + 3; _temp1638;}));
goto _LL1590; _LL1594: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1605); Cyc_Absyndump_dump_char(( int)']'); goto _LL1590; _LL1596: Cyc_Absyndump_dumpfunargs(
_temp1613, _temp1611, _temp1609); goto _LL1590; _LL1598:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1619,({ char* _temp1639="("; struct _tagged_string _temp1640; _temp1640.curr=
_temp1639; _temp1640.base= _temp1639; _temp1640.last_plus_one= _temp1639 + 2;
_temp1640;}),({ char* _temp1641=")"; struct _tagged_string _temp1642; _temp1642.curr=
_temp1641; _temp1642.base= _temp1641; _temp1642.last_plus_one= _temp1641 + 2;
_temp1642;}),({ char* _temp1643=","; struct _tagged_string _temp1644; _temp1644.curr=
_temp1643; _temp1644.base= _temp1643; _temp1644.last_plus_one= _temp1643 + 2;
_temp1644;})); goto _LL1590; _LL1600: if( _temp1621){ Cyc_Absyndump_dumpkindedtvars(
_temp1625);} else{ Cyc_Absyndump_dumptvars( _temp1625);} goto _LL1590; _LL1602:
Cyc_Absyndump_dumpatts( _temp1627); goto _LL1590; _LL1604:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1645=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1645[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1646; _temp1646.tag= Cyc_Core_Impossible;
_temp1646.f1=({ char* _temp1647="dumptms"; struct _tagged_string _temp1648;
_temp1648.curr= _temp1647; _temp1648.base= _temp1647; _temp1648.last_plus_one=
_temp1647 + 8; _temp1648;}); _temp1646;}); _temp1645;})); _LL1590:;} return;}
_LL1500:;}} void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual tq, void* t,
void(* f)( void*), void* a){ struct Cyc_List_List* _temp1651; void* _temp1653;
struct Cyc_Absyn_Tqual _temp1655; struct _tuple4 _temp1649= Cyc_Absynpp_to_tms(
tq, t); _LL1656: _temp1655= _temp1649.f1; goto _LL1654; _LL1654: _temp1653=
_temp1649.f2; goto _LL1652; _LL1652: _temp1651= _temp1649.f3; goto _LL1650;
_LL1650: Cyc_Absyndump_dumptq( _temp1655); Cyc_Absyndump_dumpntyp( _temp1653);((
void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp1651),
f, a);} void Cyc_Absyndump_dumpdecllist2file( struct Cyc_List_List* tdl, struct
Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;* Cyc_Absyndump_dump_file= f; for( 0;
tdl != 0; tdl=(( struct Cyc_List_List*) _check_null( tdl))->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null( tdl))->hd);}}