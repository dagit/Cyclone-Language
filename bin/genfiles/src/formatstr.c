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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth[ 8u]; typedef int Cyc___int32_t;
typedef unsigned int Cyc___uint32_t; extern int isalnum( int __c)
 __attribute__(( cdecl )) ; extern int isalpha( int __c)  __attribute__(( cdecl
)) ; extern int iscntrl( int __c)  __attribute__(( cdecl )) ; extern int isdigit(
int __c)  __attribute__(( cdecl )) ; extern int isgraph( int __c)
 __attribute__(( cdecl )) ; extern int islower( int __c)  __attribute__(( cdecl
)) ; extern int isprint( int __c)  __attribute__(( cdecl )) ; extern int ispunct(
int __c)  __attribute__(( cdecl )) ; extern int isspace( int __c)
 __attribute__(( cdecl )) ; extern int isupper( int __c)  __attribute__(( cdecl
)) ; extern int isxdigit( int __c)  __attribute__(( cdecl )) ; extern int
tolower( int __c)  __attribute__(( cdecl )) ; extern int toupper( int __c)
 __attribute__(( cdecl )) ; extern int isascii( int __c)  __attribute__(( cdecl
)) ; extern int toascii( int __c)  __attribute__(( cdecl )) ; extern int
_tolower( int __c)  __attribute__(( cdecl )) ; extern int _toupper( int __c)
 __attribute__(( cdecl )) ; struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[
19u]; extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern struct _tagged_string Cyc_String_implode(
struct Cyc_List_List* c); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
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
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t; typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t; typedef struct Cyc_Absyn_TunionFieldInfo
Cyc_Absyn_tunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void*
Cyc_Absyn_incrementor_t; typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
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
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
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
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att=
0; struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Stdcall_att= 0; static const int Cyc_Absyn_Cdecl_att= 1; static const
int Cyc_Absyn_Fastcall_att= 2; static const int Cyc_Absyn_Noreturn_att= 3;
static const int Cyc_Absyn_Const_att= 4; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
6; static const int Cyc_Absyn_Shared_att= 7; static const int Cyc_Absyn_Unused_att=
8; static const int Cyc_Absyn_Weak_att= 9; static const int Cyc_Absyn_Dllimport_att=
10; static const int Cyc_Absyn_Dllexport_att= 11; static const int Cyc_Absyn_No_instrument_function_att=
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
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
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
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct
Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char
Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr[
12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_string s, int i); struct _tuple3{ struct Cyc_List_List* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; char f5; int f6; } ;
struct Cyc_Core_Opt* Cyc_Formatstr_parse_conversionspecification( struct
_tagged_string s, int i){ unsigned int _temp0= Cyc_String_strlen( s); goto _LL1;
_LL1: if( i < 0? 1: i >= _temp0){ return 0;}{ struct Cyc_List_List* _temp2= 0;
goto _LL3; _LL3: { char c=' '; for( 0; i < _temp0; i ++){ c=*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i); switch( c){ case '+': _LL4: goto _LL5; case '-': _LL5: goto
_LL6; case ' ': _LL6: goto _LL7; case '#': _LL7: goto _LL8; case '0': _LL8:
_temp2=({ struct Cyc_List_List* _temp10=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp10->hd=( void*)(( int) c); _temp10->tl=
_temp2; _temp10;}); continue; default: _LL9: break;} break;} if( i >= _temp0){
return 0;} _temp2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp2);{ struct Cyc_List_List* _temp12= 0; goto _LL13; _LL13: c=*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i); if( c =='*'){ _temp12=({ struct Cyc_List_List* _temp14=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp14->hd=(
void*)(( int) c); _temp14->tl= _temp12; _temp14;}); i ++;} else{ for( 0; i <
_temp0; i ++){ c=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( char), i); if( isdigit(( int) c)){ _temp12=({
struct Cyc_List_List* _temp15=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp15->hd=( void*)(( int) c); _temp15->tl= _temp12; _temp15;});}
else{ break;}}} if( i >= _temp0){ return 0;} _temp12=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp12);{ struct Cyc_List_List*
_temp16= 0; goto _LL17; _LL17: c=*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), i);
if( c =='.'){ _temp16=({ struct Cyc_List_List* _temp18=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp18->hd=( void*)(( int) c);
_temp18->tl= _temp16; _temp18;}); i ++; if( i >= _temp0){ return 0;} c=*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i); if( c =='*'){ _temp16=({ struct Cyc_List_List* _temp19=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp19->hd=(
void*)(( int) c); _temp19->tl= _temp16; _temp19;}); i ++;} else{ for( 0; i <
_temp0; i ++){ c=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( char), i); if( isdigit(( int) c)){ _temp16=({
struct Cyc_List_List* _temp20=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp20->hd=( void*)(( int) c); _temp20->tl= _temp16; _temp20;});}
else{ break;}}}} if( i >= _temp0){ return 0;} _temp16=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp16);{ struct Cyc_List_List*
_temp21= 0; goto _LL22; _LL22: c=*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), i);
switch( c){ case 'h': _LL23: _temp21=({ struct Cyc_List_List* _temp25=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp25->hd=( void*)((
int) c); _temp25->tl= _temp21; _temp25;}); i ++; if( i >= _temp0){ return 0;} c=*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( char), i); if( c =='h'){ _temp21=({ struct
Cyc_List_List* _temp26=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp26->hd=( void*)(( int) c); _temp26->tl= _temp21; _temp26;}); i ++;} break;
case 'l': _LL24: _temp21=({ struct Cyc_List_List* _temp28=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp28->hd=( void*)(( int) c);
_temp28->tl= _temp21; _temp28;}); i ++; if( i >= _temp0){ return 0;} c=*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i); if( c =='l'){ _temp21=({ struct Cyc_List_List* _temp29=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp29->hd=(
void*)(( int) c); _temp29->tl= _temp21; _temp29;}); i ++;} break; case 'j':
_LL27: goto _LL30; case 'z': _LL30: goto _LL31; case 't': _LL31: goto _LL32;
case 'L': _LL32: _temp21=({ struct Cyc_List_List* _temp34=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp34->hd=( void*)(( int) c);
_temp34->tl= _temp21; _temp34;}); i ++; break; default: _LL33: break;} if( i >=
_temp0){ return 0;} _temp21=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp21); c=*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( char), i); switch( c){ case
'd': _LL36: break; case 'i': _LL37: break; case 'o': _LL38: break; case 'u':
_LL39: break; case 'x': _LL40: break; case 'X': _LL41: break; case 'f': _LL42:
break; case 'F': _LL43: break; case 'e': _LL44: break; case 'E': _LL45: break;
case 'g': _LL46: break; case 'G': _LL47: break; case 'a': _LL48: break; case 'A':
_LL49: break; case 'c': _LL50: break; case 's': _LL51: break; case 'p': _LL52:
break; case 'n': _LL53: break; case '%': _LL54: break; default: _LL55: return 0;}
return({ struct Cyc_Core_Opt* _temp57=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp57->v=( void*)({ struct _tuple3* _temp58=( struct
_tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp58->f1= _temp2; _temp58->f2=
_temp12; _temp58->f3= _temp16; _temp58->f4= _temp21; _temp58->f5= c; _temp58->f6=
i + 1; _temp58;}); _temp57;});}}}}}} struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int _temp59=
Cyc_String_strlen( s); goto _LL60; _LL60: { struct Cyc_List_List* _temp61= 0;
goto _LL62; _LL62: { int i; for( i= 0; i < _temp59; i ++){ if(*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i) !='%'){ continue;}{ struct Cyc_Core_Opt* _temp63= Cyc_Formatstr_parse_conversionspecification(
s, i + 1); goto _LL64; _LL64: if( _temp63 == 0){ Cyc_Tcutil_terr( loc,({ char*
_temp65="bad format string"; struct _tagged_string _temp66; _temp66.curr=
_temp65; _temp66.base= _temp65; _temp66.last_plus_one= _temp65 + 18; _temp66;}));
return 0;}{ struct _tuple3 _temp69; int _temp70; char _temp72; struct Cyc_List_List*
_temp74; struct Cyc_List_List* _temp76; struct Cyc_List_List* _temp78; struct
Cyc_List_List* _temp80; struct _tuple3* _temp67=( struct _tuple3*)(( struct Cyc_Core_Opt*)
_check_null( _temp63))->v; _temp69=* _temp67; _LL81: _temp80= _temp69.f1; goto
_LL79; _LL79: _temp78= _temp69.f2; goto _LL77; _LL77: _temp76= _temp69.f3; goto
_LL75; _LL75: _temp74= _temp69.f4; goto _LL73; _LL73: _temp72= _temp69.f5; goto
_LL71; _LL71: _temp70= _temp69.f6; goto _LL68; _LL68: i= _temp70 - 1;{ struct
Cyc_List_List* _temp82= _temp74; struct Cyc_List_List _temp89; struct Cyc_List_List*
_temp90; int _temp92; _LL84: if( _temp82 == 0){ goto _LL86;} else{ _temp89=*
_temp82; _LL93: _temp92=( int) _temp89.hd; goto _LL91; _LL91: _temp90= _temp89.tl;
if( _temp90 == 0){ goto _LL88;} else{ goto _LL86;}} _LL88: if(( _temp92 =='j'? 1:
_temp92 =='z')? 1: _temp92 =='t'){ goto _LL85;} else{ goto _LL86;} _LL86: goto
_LL87; _LL85: Cyc_Tcutil_terr( loc, xprintf("length modifier '%c' is not supported",
_temp92)); return 0; _LL87: goto _LL83; _LL83:;}{ struct Cyc_List_List* _temp94=
_temp78; struct Cyc_List_List _temp101; struct Cyc_List_List* _temp102; int
_temp104; _LL96: if( _temp94 == 0){ goto _LL98;} else{ _temp101=* _temp94;
_LL105: _temp104=( int) _temp101.hd; goto _LL103; _LL103: _temp102= _temp101.tl;
if( _temp102 == 0){ goto _LL100;} else{ goto _LL98;}} _LL100: if( _temp104 =='*'){
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: _temp61=({ struct Cyc_List_List*
_temp106=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp106->hd=( void*) Cyc_Absyn_sint_t; _temp106->tl= _temp61; _temp106;}); goto
_LL95; _LL99: goto _LL95; _LL95:;}{ struct Cyc_List_List* _temp107= _temp76;
struct Cyc_List_List _temp114; struct Cyc_List_List* _temp115; struct Cyc_List_List
_temp117; struct Cyc_List_List* _temp118; int _temp120; int _temp122; _LL109:
if( _temp107 == 0){ goto _LL111;} else{ _temp114=* _temp107; _LL123: _temp122=(
int) _temp114.hd; goto _LL116; _LL116: _temp115= _temp114.tl; if( _temp115 == 0){
goto _LL111;} else{ _temp117=* _temp115; _LL121: _temp120=( int) _temp117.hd;
goto _LL119; _LL119: _temp118= _temp117.tl; if( _temp118 == 0){ goto _LL113;}
else{ goto _LL111;}}} _LL113: if( _temp122 =='.'? _temp120 =='*': 0){ goto
_LL110;} else{ goto _LL111;} _LL111: goto _LL112; _LL110: _temp61=({ struct Cyc_List_List*
_temp124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp124->hd=( void*) Cyc_Absyn_sint_t; _temp124->tl= _temp61; _temp124;}); goto
_LL108; _LL112: goto _LL108; _LL108:;}{ void* t; switch( _temp72){ case 'd':
_LL125: goto _LL126; case 'i': _LL126:{ struct Cyc_List_List* f= _temp80; for( 0;
f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc, xprintf("flag '#' is not valid with %%%c",(
int) _temp72)); return 0;}}}{ struct Cyc_List_List* _temp128= _temp74; struct
Cyc_List_List _temp141; struct Cyc_List_List* _temp142; int _temp144; struct Cyc_List_List
_temp147; struct Cyc_List_List* _temp148; int _temp150; struct Cyc_List_List
_temp153; struct Cyc_List_List* _temp154; struct Cyc_List_List _temp156; struct
Cyc_List_List* _temp157; int _temp159; int _temp161; _LL130: if( _temp128 == 0){
goto _LL131;} else{ goto _LL132;} _LL132: if( _temp128 == 0){ goto _LL134;}
else{ _temp141=* _temp128; _LL145: _temp144=( int) _temp141.hd; goto _LL143;
_LL143: _temp142= _temp141.tl; if( _temp142 == 0){ goto _LL140;} else{ goto
_LL134;}} _LL140: if( _temp144 =='l'){ goto _LL133;} else{ goto _LL134;} _LL134:
if( _temp128 == 0){ goto _LL136;} else{ _temp147=* _temp128; _LL151: _temp150=(
int) _temp147.hd; goto _LL149; _LL149: _temp148= _temp147.tl; if( _temp148 == 0){
goto _LL146;} else{ goto _LL136;}} _LL146: if( _temp150 =='h'){ goto _LL135;}
else{ goto _LL136;} _LL136: if( _temp128 == 0){ goto _LL138;} else{ _temp153=*
_temp128; _LL162: _temp161=( int) _temp153.hd; goto _LL155; _LL155: _temp154=
_temp153.tl; if( _temp154 == 0){ goto _LL138;} else{ _temp156=* _temp154; _LL160:
_temp159=( int) _temp156.hd; goto _LL158; _LL158: _temp157= _temp156.tl; if(
_temp157 == 0){ goto _LL152;} else{ goto _LL138;}}} _LL152: if( _temp161 =='h'?
_temp159 =='h': 0){ goto _LL137;} else{ goto _LL138;} _LL138: goto _LL139;
_LL131: t= Cyc_Absyn_sint_t; goto _LL129; _LL133: t= Cyc_Absyn_slong_t; goto
_LL129; _LL135: t= Cyc_Absyn_sshort_t; goto _LL129; _LL137: t= Cyc_Absyn_schar_t;
goto _LL129; _LL139: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp163= Cyc_String_implode(
_temp74); int _temp164=( int) _temp72; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp163.last_plus_one - _temp163.curr, _temp163.curr, _temp164);})); return 0;
_LL129:;} _temp61=({ struct Cyc_List_List* _temp165=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp165->hd=( void*) t; _temp165->tl=
_temp61; _temp165;}); break; case 'u': _LL127:{ struct Cyc_List_List* f= _temp80;
for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){ if(( int)((
struct Cyc_List_List*) _check_null( f))->hd =='#'){ Cyc_Tcutil_terr( loc,({ char*
_temp167="Flag '#' not valid with %%u"; struct _tagged_string _temp168; _temp168.curr=
_temp167; _temp168.base= _temp167; _temp168.last_plus_one= _temp167 + 28;
_temp168;})); return 0;}}} goto _LL166; case 'o': _LL166: goto _LL169; case 'x':
_LL169: goto _LL170; case 'X': _LL170:{ struct Cyc_List_List* _temp172= _temp74;
struct Cyc_List_List _temp185; struct Cyc_List_List* _temp186; int _temp188;
struct Cyc_List_List _temp191; struct Cyc_List_List* _temp192; int _temp194;
struct Cyc_List_List _temp197; struct Cyc_List_List* _temp198; struct Cyc_List_List
_temp200; struct Cyc_List_List* _temp201; int _temp203; int _temp205; _LL174:
if( _temp172 == 0){ goto _LL175;} else{ goto _LL176;} _LL176: if( _temp172 == 0){
goto _LL178;} else{ _temp185=* _temp172; _LL189: _temp188=( int) _temp185.hd;
goto _LL187; _LL187: _temp186= _temp185.tl; if( _temp186 == 0){ goto _LL184;}
else{ goto _LL178;}} _LL184: if( _temp188 =='l'){ goto _LL177;} else{ goto
_LL178;} _LL178: if( _temp172 == 0){ goto _LL180;} else{ _temp191=* _temp172;
_LL195: _temp194=( int) _temp191.hd; goto _LL193; _LL193: _temp192= _temp191.tl;
if( _temp192 == 0){ goto _LL190;} else{ goto _LL180;}} _LL190: if( _temp194 =='h'){
goto _LL179;} else{ goto _LL180;} _LL180: if( _temp172 == 0){ goto _LL182;}
else{ _temp197=* _temp172; _LL206: _temp205=( int) _temp197.hd; goto _LL199;
_LL199: _temp198= _temp197.tl; if( _temp198 == 0){ goto _LL182;} else{ _temp200=*
_temp198; _LL204: _temp203=( int) _temp200.hd; goto _LL202; _LL202: _temp201=
_temp200.tl; if( _temp201 == 0){ goto _LL196;} else{ goto _LL182;}}} _LL196: if(
_temp205 =='h'? _temp203 =='h': 0){ goto _LL181;} else{ goto _LL182;} _LL182:
goto _LL183; _LL175: t= Cyc_Absyn_uint_t; goto _LL173; _LL177: t= Cyc_Absyn_ulong_t;
goto _LL173; _LL179: t= Cyc_Absyn_ushort_t; goto _LL173; _LL181: t= Cyc_Absyn_uchar_t;
goto _LL173; _LL183: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp207= Cyc_String_implode(
_temp74); int _temp208=( int) _temp72; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp207.last_plus_one - _temp207.curr, _temp207.curr, _temp208);})); return 0;
_LL173:;} _temp61=({ struct Cyc_List_List* _temp209=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp209->hd=( void*) t; _temp209->tl=
_temp61; _temp209;}); break; case 'f': _LL171: goto _LL210; case 'F': _LL210:
goto _LL211; case 'e': _LL211: goto _LL212; case 'E': _LL212: goto _LL213; case
'g': _LL213: goto _LL214; case 'G': _LL214: goto _LL215; case 'a': _LL215: goto
_LL216; case 'A': _LL216: if( _temp74 != 0){ Cyc_Tcutil_terr( loc, xprintf("length modifiers are not allowed with %%%c",(
int) _temp72)); return 0;} _temp61=({ struct Cyc_List_List* _temp218=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp218->hd=( void*)
Cyc_Absyn_double_t; _temp218->tl= _temp61; _temp218;}); break; case 'c': _LL217:{
struct Cyc_List_List* f= _temp80; for( 0; f != 0; f=(( struct Cyc_List_List*)
_check_null( f))->tl){ if(( int)(( struct Cyc_List_List*) _check_null( f))->hd
=='#'? 1:( int)(( struct Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr(
loc, xprintf("flag '%c' not allowed with %%c",( int)(( struct Cyc_List_List*)
_check_null( f))->hd)); return 0;}}} if( _temp74 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp220= Cyc_String_implode( _temp74); xprintf("length modifier '%.*s' not allowed with %%c",
_temp220.last_plus_one - _temp220.curr, _temp220.curr);})); return 0;} if(
_temp76 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp221= Cyc_String_implode(
_temp76); xprintf("precision '%.*s' not allowed with %%c", _temp221.last_plus_one
- _temp221.curr, _temp221.curr);})); return 0;} _temp61=({ struct Cyc_List_List*
_temp222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp222->hd=( void*) Cyc_Absyn_sint_t; _temp222->tl= _temp61; _temp222;});
break; case 's': _LL219: if( _temp80 != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp224= Cyc_String_implode( _temp80); xprintf("flags '%.*s' not allowed with %%s",
_temp224.last_plus_one - _temp224.curr, _temp224.curr);})); return 0;} if(
_temp78 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp225= Cyc_String_implode(
_temp78); xprintf("width '%.*s' not allowed with %%s", _temp225.last_plus_one -
_temp225.curr, _temp225.curr);})); return 0;} if( _temp76 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp226= Cyc_String_implode( _temp76); xprintf("precision '%.*s' not allowed with %%s",
_temp226.last_plus_one - _temp226.curr, _temp226.curr);})); return 0;} if(
_temp74 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp227= Cyc_String_implode(
_temp74); xprintf("length modifier '%.*s' not allowed with %%s", _temp227.last_plus_one
- _temp227.curr, _temp227.curr);})); return 0;} _temp61=({ struct Cyc_List_List*
_temp228=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp228->hd=( void*) Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind));
_temp228->tl= _temp61; _temp228;}); break; case 'p': _LL223: Cyc_Tcutil_terr(
loc,({ char* _temp230="%%p not supported"; struct _tagged_string _temp231;
_temp231.curr= _temp230; _temp231.base= _temp230; _temp231.last_plus_one=
_temp230 + 18; _temp231;})); return 0; case 'n': _LL229:{ struct Cyc_List_List*
f= _temp80; for( 0; f != 0; f=(( struct Cyc_List_List*) _check_null( f))->tl){
if(( int)(( struct Cyc_List_List*) _check_null( f))->hd =='#'? 1:( int)(( struct
Cyc_List_List*) _check_null( f))->hd =='0'){ Cyc_Tcutil_terr( loc, xprintf("flag '%c' not allowed with %%n",(
int)(( struct Cyc_List_List*) _check_null( f))->hd)); return 0;}}} if( _temp76
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp233= Cyc_String_implode(
_temp76); xprintf("precision '%.*s' not allowed with %%n", _temp233.last_plus_one
- _temp233.curr, _temp233.curr);})); return 0;}{ struct Cyc_List_List* _temp234=
_temp74; struct Cyc_List_List _temp247; struct Cyc_List_List* _temp248; int
_temp250; struct Cyc_List_List _temp253; struct Cyc_List_List* _temp254; int
_temp256; struct Cyc_List_List _temp259; struct Cyc_List_List* _temp260; struct
Cyc_List_List _temp262; struct Cyc_List_List* _temp263; int _temp265; int
_temp267; _LL236: if( _temp234 == 0){ goto _LL237;} else{ goto _LL238;} _LL238:
if( _temp234 == 0){ goto _LL240;} else{ _temp247=* _temp234; _LL251: _temp250=(
int) _temp247.hd; goto _LL249; _LL249: _temp248= _temp247.tl; if( _temp248 == 0){
goto _LL246;} else{ goto _LL240;}} _LL246: if( _temp250 =='l'){ goto _LL239;}
else{ goto _LL240;} _LL240: if( _temp234 == 0){ goto _LL242;} else{ _temp253=*
_temp234; _LL257: _temp256=( int) _temp253.hd; goto _LL255; _LL255: _temp254=
_temp253.tl; if( _temp254 == 0){ goto _LL252;} else{ goto _LL242;}} _LL252: if(
_temp256 =='h'){ goto _LL241;} else{ goto _LL242;} _LL242: if( _temp234 == 0){
goto _LL244;} else{ _temp259=* _temp234; _LL268: _temp267=( int) _temp259.hd;
goto _LL261; _LL261: _temp260= _temp259.tl; if( _temp260 == 0){ goto _LL244;}
else{ _temp262=* _temp260; _LL266: _temp265=( int) _temp262.hd; goto _LL264;
_LL264: _temp263= _temp262.tl; if( _temp263 == 0){ goto _LL258;} else{ goto
_LL244;}}} _LL258: if( _temp267 =='h'? _temp265 =='h': 0){ goto _LL243;} else{
goto _LL244;} _LL244: goto _LL245; _LL237: t= Cyc_Absyn_at_typ( Cyc_Absyn_sint_t,
Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto
_LL235; _LL239: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL235; _LL241: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL235; _LL243: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL235; _LL245: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp269= Cyc_String_implode( _temp74); xprintf("length modifier '%.*s' not allowed with %%n",
_temp269.last_plus_one - _temp269.curr, _temp269.curr);})); return 0; _LL235:;}
_temp61=({ struct Cyc_List_List* _temp270=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp270->hd=( void*) t; _temp270->tl= _temp61;
_temp270;}); break; case '%': _LL232: if( _temp80 != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp272= Cyc_String_implode( _temp80); xprintf("flags '%.*s' not allowed with %%%%",
_temp272.last_plus_one - _temp272.curr, _temp272.curr);})); return 0;} if(
_temp78 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp273= Cyc_String_implode(
_temp78); xprintf("width '%.*s' not allowed with %%%%", _temp273.last_plus_one -
_temp273.curr, _temp273.curr);})); return 0;} if( _temp76 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp274= Cyc_String_implode( _temp76); xprintf("precision '%.*s' not allowed with %%%%",
_temp274.last_plus_one - _temp274.curr, _temp274.curr);})); return 0;} if(
_temp74 != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp275= Cyc_String_implode(
_temp74); xprintf("length modifier '%.*s' not allowed with %%%%", _temp275.last_plus_one
- _temp275.curr, _temp275.curr);})); return 0;} break; default: _LL271: return 0;}}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp61);}}} struct _tuple4{ int f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; char f4; int f5; } ; struct Cyc_Core_Opt* Cyc_Formatstr_parse_inputformat(
struct _tagged_string s, int i){ unsigned int _temp277= Cyc_String_strlen( s);
goto _LL278; _LL278: if( i < 0? 1: i >= _temp277){ return 0;}{ int _temp279= 0;
goto _LL280; _LL280: { char _temp281=*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), i);
goto _LL282; _LL282: if( _temp281 =='*'){ _temp279= 1; i ++; if( i >= _temp277){
return 0;}}{ struct Cyc_List_List* _temp283= 0; goto _LL284; _LL284: for( 0; i <
_temp277; i ++){ _temp281=*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( char), i); if( isdigit((
int) _temp281)){ _temp283=({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp285->hd=( void*)(( int) _temp281);
_temp285->tl= _temp283; _temp285;});} else{ break;}} if( i >= _temp277){ return
0;} _temp283=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp283);{ struct Cyc_List_List* _temp286= 0; goto _LL287; _LL287: _temp281=*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( char), i); switch( _temp281){ case 'h':
_LL288: _temp286=({ struct Cyc_List_List* _temp290=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp290->hd=( void*)(( int) _temp281);
_temp290->tl= _temp286; _temp290;}); i ++; if( i >= _temp277){ return 0;}
_temp281=*(( char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( s, sizeof( char), i); if( _temp281 =='h'){ _temp286=({
struct Cyc_List_List* _temp291=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp291->hd=( void*)(( int) _temp281); _temp291->tl=
_temp286; _temp291;}); i ++;} break; case 'l': _LL289: _temp286=({ struct Cyc_List_List*
_temp293=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp293->hd=( void*)(( int) _temp281); _temp293->tl= _temp286; _temp293;}); i
++; if( i >= _temp277){ return 0;} _temp281=*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), i);
if( _temp281 =='l'){ _temp286=({ struct Cyc_List_List* _temp294=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp294->hd=( void*)(( int) _temp281);
_temp294->tl= _temp286; _temp294;}); i ++;} break; case 'j': _LL292: goto _LL295;
case 'z': _LL295: goto _LL296; case 't': _LL296: goto _LL297; case 'L': _LL297:
_temp286=({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp299->hd=( void*)(( int) _temp281); _temp299->tl=
_temp286; _temp299;}); i ++; break; default: _LL298: break;} if( i >= _temp277){
return 0;} _temp286=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp286); _temp281=*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( s, sizeof( char), i); switch( _temp281){
case 'd': _LL301: break; case 'i': _LL302: break; case 'o': _LL303: break; case
'u': _LL304: break; case 'x': _LL305: break; case 'X': _LL306: break; case 'f':
_LL307: break; case 'F': _LL308: break; case 'e': _LL309: break; case 'E':
_LL310: break; case 'g': _LL311: break; case 'G': _LL312: break; case 'a':
_LL313: break; case 'A': _LL314: break; case 'c': _LL315: break; case 's':
_LL316: break; case 'p': _LL317: break; case 'n': _LL318: break; case '%':
_LL319: break; default: _LL320: return 0;} return({ struct Cyc_Core_Opt*
_temp322=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp322->v=( void*)({ struct _tuple4* _temp323=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp323->f1= _temp279; _temp323->f2= _temp283;
_temp323->f3= _temp286; _temp323->f4= _temp281; _temp323->f5= i + 1; _temp323;});
_temp322;});}}}}} struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ unsigned int _temp324= Cyc_String_strlen(
s); goto _LL325; _LL325: { struct Cyc_List_List* _temp326= 0; goto _LL327;
_LL327: { int i; for( i= 0; i < _temp324; i ++){ if(*(( char*(*)( struct
_tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)( s,
sizeof( char), i) !='%'){ continue;}{ struct Cyc_Core_Opt* _temp328= Cyc_Formatstr_parse_inputformat(
s, i + 1); goto _LL329; _LL329: if( _temp328 == 0){ Cyc_Tcutil_terr( loc,({ char*
_temp330="bad format string"; struct _tagged_string _temp331; _temp331.curr=
_temp330; _temp331.base= _temp330; _temp331.last_plus_one= _temp330 + 18;
_temp331;})); return 0;}{ struct _tuple4 _temp334; int _temp335; char _temp337;
struct Cyc_List_List* _temp339; struct Cyc_List_List* _temp341; int _temp343;
struct _tuple4* _temp332=( struct _tuple4*)(( struct Cyc_Core_Opt*) _check_null(
_temp328))->v; _temp334=* _temp332; _LL344: _temp343= _temp334.f1; goto _LL342;
_LL342: _temp341= _temp334.f2; goto _LL340; _LL340: _temp339= _temp334.f3; goto
_LL338; _LL338: _temp337= _temp334.f4; goto _LL336; _LL336: _temp335= _temp334.f5;
goto _LL333; _LL333: i= _temp335 - 1;{ struct Cyc_List_List* _temp345= _temp339;
struct Cyc_List_List _temp352; struct Cyc_List_List* _temp353; int _temp355;
_LL347: if( _temp345 == 0){ goto _LL349;} else{ _temp352=* _temp345; _LL356:
_temp355=( int) _temp352.hd; goto _LL354; _LL354: _temp353= _temp352.tl; if(
_temp353 == 0){ goto _LL351;} else{ goto _LL349;}} _LL351: if(( _temp355 =='j'?
1: _temp355 =='z')? 1: _temp355 =='t'){ goto _LL348;} else{ goto _LL349;} _LL349:
goto _LL350; _LL348: Cyc_Tcutil_terr( loc, xprintf("length modifier '%c' is not supported",
_temp355)); return 0; _LL350: goto _LL346; _LL346:;} if( _temp343){ continue;}{
void* t; switch( _temp337){ case 'd': _LL357: goto _LL358; case 'i': _LL358:{
struct Cyc_List_List* _temp360= _temp339; struct Cyc_List_List _temp373; struct
Cyc_List_List* _temp374; int _temp376; struct Cyc_List_List _temp379; struct Cyc_List_List*
_temp380; int _temp382; struct Cyc_List_List _temp385; struct Cyc_List_List*
_temp386; struct Cyc_List_List _temp388; struct Cyc_List_List* _temp389; int
_temp391; int _temp393; _LL362: if( _temp360 == 0){ goto _LL363;} else{ goto
_LL364;} _LL364: if( _temp360 == 0){ goto _LL366;} else{ _temp373=* _temp360;
_LL377: _temp376=( int) _temp373.hd; goto _LL375; _LL375: _temp374= _temp373.tl;
if( _temp374 == 0){ goto _LL372;} else{ goto _LL366;}} _LL372: if( _temp376 =='l'){
goto _LL365;} else{ goto _LL366;} _LL366: if( _temp360 == 0){ goto _LL368;}
else{ _temp379=* _temp360; _LL383: _temp382=( int) _temp379.hd; goto _LL381;
_LL381: _temp380= _temp379.tl; if( _temp380 == 0){ goto _LL378;} else{ goto
_LL368;}} _LL378: if( _temp382 =='h'){ goto _LL367;} else{ goto _LL368;} _LL368:
if( _temp360 == 0){ goto _LL370;} else{ _temp385=* _temp360; _LL394: _temp393=(
int) _temp385.hd; goto _LL387; _LL387: _temp386= _temp385.tl; if( _temp386 == 0){
goto _LL370;} else{ _temp388=* _temp386; _LL392: _temp391=( int) _temp388.hd;
goto _LL390; _LL390: _temp389= _temp388.tl; if( _temp389 == 0){ goto _LL384;}
else{ goto _LL370;}}} _LL384: if( _temp393 =='h'? _temp391 =='h': 0){ goto
_LL369;} else{ goto _LL370;} _LL370: goto _LL371; _LL363: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL361; _LL365: t= Cyc_Absyn_at_typ( Cyc_Absyn_slong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL361; _LL367: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL361; _LL369: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL361; _LL371: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp395= Cyc_String_implode( _temp339); int
_temp396=( int) _temp337; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp395.last_plus_one - _temp395.curr, _temp395.curr, _temp396);})); return 0;
_LL361:;} _temp326=({ struct Cyc_List_List* _temp397=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp397->hd=( void*) t; _temp397->tl=
_temp326; _temp397;}); break; case 'u': _LL359: goto _LL398; case 'o': _LL398:
goto _LL399; case 'x': _LL399: goto _LL400; case 'X': _LL400:{ struct Cyc_List_List*
_temp402= _temp339; struct Cyc_List_List _temp415; struct Cyc_List_List*
_temp416; int _temp418; struct Cyc_List_List _temp421; struct Cyc_List_List*
_temp422; int _temp424; struct Cyc_List_List _temp427; struct Cyc_List_List*
_temp428; struct Cyc_List_List _temp430; struct Cyc_List_List* _temp431; int
_temp433; int _temp435; _LL404: if( _temp402 == 0){ goto _LL405;} else{ goto
_LL406;} _LL406: if( _temp402 == 0){ goto _LL408;} else{ _temp415=* _temp402;
_LL419: _temp418=( int) _temp415.hd; goto _LL417; _LL417: _temp416= _temp415.tl;
if( _temp416 == 0){ goto _LL414;} else{ goto _LL408;}} _LL414: if( _temp418 =='l'){
goto _LL407;} else{ goto _LL408;} _LL408: if( _temp402 == 0){ goto _LL410;}
else{ _temp421=* _temp402; _LL425: _temp424=( int) _temp421.hd; goto _LL423;
_LL423: _temp422= _temp421.tl; if( _temp422 == 0){ goto _LL420;} else{ goto
_LL410;}} _LL420: if( _temp424 =='h'){ goto _LL409;} else{ goto _LL410;} _LL410:
if( _temp402 == 0){ goto _LL412;} else{ _temp427=* _temp402; _LL436: _temp435=(
int) _temp427.hd; goto _LL429; _LL429: _temp428= _temp427.tl; if( _temp428 == 0){
goto _LL412;} else{ _temp430=* _temp428; _LL434: _temp433=( int) _temp430.hd;
goto _LL432; _LL432: _temp431= _temp430.tl; if( _temp431 == 0){ goto _LL426;}
else{ goto _LL412;}}} _LL426: if( _temp435 =='h'? _temp433 =='h': 0){ goto
_LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL405: t= Cyc_Absyn_at_typ(
Cyc_Absyn_uint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL403; _LL407: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL403; _LL409: t= Cyc_Absyn_at_typ(
Cyc_Absyn_ushort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL403; _LL411: t= Cyc_Absyn_at_typ( Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL403; _LL413: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp437= Cyc_String_implode( _temp339); int
_temp438=( int) _temp337; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp437.last_plus_one - _temp437.curr, _temp437.curr, _temp438);})); return 0;
_LL403:;} _temp326=({ struct Cyc_List_List* _temp439=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp439->hd=( void*) t; _temp439->tl=
_temp326; _temp439;}); break; case 'f': _LL401: goto _LL440; case 'F': _LL440:
goto _LL441; case 'e': _LL441: goto _LL442; case 'E': _LL442: goto _LL443; case
'g': _LL443: goto _LL444; case 'G': _LL444: goto _LL445; case 'a': _LL445: goto
_LL446; case 'A': _LL446:{ struct Cyc_List_List* _temp448= _temp339; struct Cyc_List_List
_temp457; struct Cyc_List_List* _temp458; int _temp460; _LL450: if( _temp448 ==
0){ goto _LL451;} else{ goto _LL452;} _LL452: if( _temp448 == 0){ goto _LL454;}
else{ _temp457=* _temp448; _LL461: _temp460=( int) _temp457.hd; goto _LL459;
_LL459: _temp458= _temp457.tl; if( _temp458 == 0){ goto _LL456;} else{ goto
_LL454;}} _LL456: if( _temp460 =='l'){ goto _LL453;} else{ goto _LL454;} _LL454:
goto _LL455; _LL451: t= Cyc_Absyn_at_typ( Cyc_Absyn_float_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL449; _LL453: t= Cyc_Absyn_at_typ(
Cyc_Absyn_double_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL449; _LL455: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp462= Cyc_String_implode(
_temp339); int _temp463=( int) _temp337; xprintf("length modifier '%.*s' is not allowed with %%%c",
_temp462.last_plus_one - _temp462.curr, _temp462.curr, _temp463);})); return 0;
_LL449:;} _temp326=({ struct Cyc_List_List* _temp464=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp464->hd=( void*) t; _temp464->tl=
_temp326; _temp464;}); break; case 'c': _LL447: Cyc_Tcutil_terr( loc,({ char*
_temp466="%%c is not supported"; struct _tagged_string _temp467; _temp467.curr=
_temp466; _temp467.base= _temp466; _temp467.last_plus_one= _temp466 + 21;
_temp467;})); return 0; case 's': _LL465: Cyc_Tcutil_terr( loc,({ char* _temp469="%%s is not supported";
struct _tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 21; _temp470;})); return 0; case '[': _LL468:
Cyc_Tcutil_terr( loc,({ char* _temp472="%%[ is not supported"; struct
_tagged_string _temp473; _temp473.curr= _temp472; _temp473.base= _temp472;
_temp473.last_plus_one= _temp472 + 21; _temp473;})); return 0; case 'p': _LL471:
Cyc_Tcutil_terr( loc,({ char* _temp475="%%p is not supported"; struct
_tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 21; _temp476;})); return 0; case 'n': _LL474:{
struct Cyc_List_List* _temp478= _temp339; struct Cyc_List_List _temp491; struct
Cyc_List_List* _temp492; int _temp494; struct Cyc_List_List _temp497; struct Cyc_List_List*
_temp498; int _temp500; struct Cyc_List_List _temp503; struct Cyc_List_List*
_temp504; struct Cyc_List_List _temp506; struct Cyc_List_List* _temp507; int
_temp509; int _temp511; _LL480: if( _temp478 == 0){ goto _LL481;} else{ goto
_LL482;} _LL482: if( _temp478 == 0){ goto _LL484;} else{ _temp491=* _temp478;
_LL495: _temp494=( int) _temp491.hd; goto _LL493; _LL493: _temp492= _temp491.tl;
if( _temp492 == 0){ goto _LL490;} else{ goto _LL484;}} _LL490: if( _temp494 =='l'){
goto _LL483;} else{ goto _LL484;} _LL484: if( _temp478 == 0){ goto _LL486;}
else{ _temp497=* _temp478; _LL501: _temp500=( int) _temp497.hd; goto _LL499;
_LL499: _temp498= _temp497.tl; if( _temp498 == 0){ goto _LL496;} else{ goto
_LL486;}} _LL496: if( _temp500 =='h'){ goto _LL485;} else{ goto _LL486;} _LL486:
if( _temp478 == 0){ goto _LL488;} else{ _temp503=* _temp478; _LL512: _temp511=(
int) _temp503.hd; goto _LL505; _LL505: _temp504= _temp503.tl; if( _temp504 == 0){
goto _LL488;} else{ _temp506=* _temp504; _LL510: _temp509=( int) _temp506.hd;
goto _LL508; _LL508: _temp507= _temp506.tl; if( _temp507 == 0){ goto _LL502;}
else{ goto _LL488;}}} _LL502: if( _temp511 =='h'? _temp509 =='h': 0){ goto
_LL487;} else{ goto _LL488;} _LL488: goto _LL489; _LL481: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL479; _LL483: t= Cyc_Absyn_at_typ( Cyc_Absyn_ulong_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL479; _LL485: t= Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t, Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual());
goto _LL479; _LL487: t= Cyc_Absyn_at_typ( Cyc_Absyn_schar_t, Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind), Cyc_Absyn_empty_tqual()); goto _LL479; _LL489: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp513= Cyc_String_implode( _temp339); xprintf("length modifier '%.*s' not allowed with %%n",
_temp513.last_plus_one - _temp513.curr, _temp513.curr);})); return 0; _LL479:;}
_temp326=({ struct Cyc_List_List* _temp514=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp514->hd=( void*) t; _temp514->tl= _temp326;
_temp514;}); break; case '%': _LL477: if( _temp343){ Cyc_Tcutil_terr( loc,({
char* _temp516="Assignment suppression (*) is not allowed with %%%%"; struct
_tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 52; _temp517;})); return 0;} if( _temp341 !=
0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp518= Cyc_String_implode(
_temp341); xprintf("width '%.*s' not allowed with %%%%", _temp518.last_plus_one
- _temp518.curr, _temp518.curr);})); return 0;} if( _temp339 != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp519= Cyc_String_implode( _temp339); xprintf("length modifier '%.*s' not allowed with %%%%",
_temp519.last_plus_one - _temp519.curr, _temp519.curr);})); return 0;} break;
default: _LL515: return 0;}}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( _temp326);}}}