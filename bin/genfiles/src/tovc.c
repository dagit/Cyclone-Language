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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern int Cyc_Core_ptrcmp( void**, void**);
extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u]; struct
Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag; struct
_tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_rcopy(
struct _RegionHandle*, struct Cyc_List_List* src); extern char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct
Cyc_List_List* y); extern char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_gdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_rempty(
struct _RegionHandle*, int(* comp)( void*, void*)); extern int Cyc_Dict_member(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict* Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict*, void*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[
19u]; extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern int Cyc_String_zstrptrcmp(
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
extern void* Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual tq); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt(
struct _tuple0*, void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc); struct Cyc_Set_Set; typedef struct
Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef
struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_rempty(
struct _RegionHandle* rgn, int(* comp)( void*, void*)); extern struct Cyc_Set_Set*
Cyc_Set_rinsert( struct _RegionHandle* rgn, struct Cyc_Set_Set* s, void* elt);
extern struct Cyc_List_List* Cyc_Set_elements( struct Cyc_Set_Set* s); extern
char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{
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
12u]; extern void* Cyc_Tcutil_compress( void* t); extern struct _tuple0* Cyc_Toc_temp_var();
extern struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls); struct
Cyc_Tovc_BoxingEnv{ struct _RegionHandle* rgn; struct Cyc_List_List* all_locals;
struct Cyc_Dict_Dict* varmap; struct Cyc_List_List* boundvars; struct Cyc_Set_Set**
freevars; } ; typedef struct Cyc_Tovc_BoxingEnv Cyc_Tovc_boxingenv_t; struct Cyc_Tovc_ToExpEnv{
struct _RegionHandle* rgn; struct Cyc_List_List* all_locals; struct Cyc_Dict_Dict*
varmap; struct Cyc_Absyn_Stmt* encloser; struct Cyc_List_List** gen_ds; } ;
typedef struct Cyc_Tovc_ToExpEnv Cyc_Tovc_toexpenv_t; struct _tuple3{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; static void Cyc_Tovc_box_free_vars_exp( struct
Cyc_Tovc_BoxingEnv env, struct Cyc_Absyn_Exp* e){ while( 1) { void* _temp0=(
void*) e->r; void* _temp46; struct _tuple0* _temp48; struct _tuple0 _temp50;
struct _tagged_string* _temp51; void* _temp53; void* _temp55; struct _tuple0*
_temp57; void* _temp59; struct Cyc_Absyn_VarargCallInfo* _temp61; struct Cyc_List_List*
_temp63; struct Cyc_Absyn_Exp* _temp65; struct Cyc_List_List* _temp67; void*
_temp69; struct Cyc_Absyn_Exp* _temp71; struct Cyc_Absyn_Exp* _temp73; struct
Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Exp* _temp77; struct Cyc_Absyn_Exp*
_temp79; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp* _temp83; struct
Cyc_Absyn_Exp* _temp85; struct Cyc_Core_Opt* _temp87; struct Cyc_Absyn_Exp*
_temp89; struct Cyc_Absyn_Exp* _temp91; struct Cyc_List_List* _temp93; struct
Cyc_Absyn_Exp* _temp95; struct Cyc_Absyn_Exp* _temp97; void* _temp99; struct Cyc_Absyn_Exp*
_temp101; struct Cyc_Absyn_Exp* _temp103; struct Cyc_Absyn_Exp* _temp105; struct
_tagged_string* _temp107; struct Cyc_Absyn_Exp* _temp109; struct _tagged_string*
_temp111; struct Cyc_Absyn_Exp* _temp113; void* _temp115; struct Cyc_Absyn_Exp*
_temp117; void* _temp119; struct Cyc_Absyn_Enumfield* _temp121; struct Cyc_Absyn_Enumdecl*
_temp123; struct _tuple0* _temp125; struct Cyc_List_List* _temp127; struct Cyc_Core_Opt*
_temp129; _LL2: if(*(( int*) _temp0) == Cyc_Absyn_Var_e){ _LL49: _temp48=((
struct Cyc_Absyn_Var_e_struct*) _temp0)->f1; _temp50=* _temp48; _LL54: _temp53=
_temp50.f1; if( _temp53 ==( void*) Cyc_Absyn_Loc_n){ goto _LL52;} else{ goto
_LL4;} _LL52: _temp51= _temp50.f2; goto _LL47; _LL47: _temp46=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp0)->f2; goto _LL3;} else{ goto _LL4;} _LL4: if(*((
int*) _temp0) == Cyc_Absyn_Var_e){ _LL58: _temp57=(( struct Cyc_Absyn_Var_e_struct*)
_temp0)->f1; goto _LL56; _LL56: _temp55=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp0)->f2; goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp0) == Cyc_Absyn_Const_e){
_LL60: _temp59=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp0)->f1; goto
_LL7;} else{ goto _LL8;} _LL8: if(*(( int*) _temp0) == Cyc_Absyn_FnCall_e){
_LL66: _temp65=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f1; goto _LL64;
_LL64: _temp63=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f2; goto _LL62;
_LL62: _temp61=(( struct Cyc_Absyn_FnCall_e_struct*) _temp0)->f3; goto _LL9;}
else{ goto _LL10;} _LL10: if(*(( int*) _temp0) == Cyc_Absyn_Primop_e){ _LL70:
_temp69=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp0)->f1; goto _LL68;
_LL68: _temp67=(( struct Cyc_Absyn_Primop_e_struct*) _temp0)->f2; goto _LL11;}
else{ goto _LL12;} _LL12: if(*(( int*) _temp0) == Cyc_Absyn_Conditional_e){
_LL76: _temp75=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f1; goto
_LL74; _LL74: _temp73=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f2;
goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_Conditional_e_struct*) _temp0)->f3;
goto _LL13;} else{ goto _LL14;} _LL14: if(*(( int*) _temp0) == Cyc_Absyn_Subscript_e){
_LL80: _temp79=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f1; goto _LL78;
_LL78: _temp77=(( struct Cyc_Absyn_Subscript_e_struct*) _temp0)->f2; goto _LL15;}
else{ goto _LL16;} _LL16: if(*(( int*) _temp0) == Cyc_Absyn_SeqExp_e){ _LL84:
_temp83=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f1; goto _LL82; _LL82:
_temp81=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp0)->f2; goto _LL17;} else{
goto _LL18;} _LL18: if(*(( int*) _temp0) == Cyc_Absyn_AssignOp_e){ _LL90:
_temp89=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f1; goto _LL88; _LL88:
_temp87=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f2; goto _LL86; _LL86:
_temp85=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp0)->f3; goto _LL19;} else{
goto _LL20;} _LL20: if(*(( int*) _temp0) == Cyc_Absyn_NoInstantiate_e){ _LL92:
_temp91=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp0)->f1; goto _LL21;}
else{ goto _LL22;} _LL22: if(*(( int*) _temp0) == Cyc_Absyn_Instantiate_e){
_LL96: _temp95=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp0)->f1; goto
_LL94; _LL94: _temp93=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp0)->f2;
goto _LL23;} else{ goto _LL24;} _LL24: if(*(( int*) _temp0) == Cyc_Absyn_Cast_e){
_LL100: _temp99=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp0)->f1; goto
_LL98; _LL98: _temp97=(( struct Cyc_Absyn_Cast_e_struct*) _temp0)->f2; goto
_LL25;} else{ goto _LL26;} _LL26: if(*(( int*) _temp0) == Cyc_Absyn_Address_e){
_LL102: _temp101=(( struct Cyc_Absyn_Address_e_struct*) _temp0)->f1; goto _LL27;}
else{ goto _LL28;} _LL28: if(*(( int*) _temp0) == Cyc_Absyn_Sizeofexp_e){ _LL104:
_temp103=(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp0)->f1; goto _LL29;}
else{ goto _LL30;} _LL30: if(*(( int*) _temp0) == Cyc_Absyn_Deref_e){ _LL106:
_temp105=(( struct Cyc_Absyn_Deref_e_struct*) _temp0)->f1; goto _LL31;} else{
goto _LL32;} _LL32: if(*(( int*) _temp0) == Cyc_Absyn_StructMember_e){ _LL110:
_temp109=(( struct Cyc_Absyn_StructMember_e_struct*) _temp0)->f1; goto _LL108;
_LL108: _temp107=(( struct Cyc_Absyn_StructMember_e_struct*) _temp0)->f2; goto
_LL33;} else{ goto _LL34;} _LL34: if(*(( int*) _temp0) == Cyc_Absyn_StructArrow_e){
_LL114: _temp113=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp0)->f1; goto
_LL112; _LL112: _temp111=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp0)->f2;
goto _LL35;} else{ goto _LL36;} _LL36: if(*(( int*) _temp0) == Cyc_Absyn_Increment_e){
_LL118: _temp117=(( struct Cyc_Absyn_Increment_e_struct*) _temp0)->f1; goto
_LL116; _LL116: _temp115=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp0)->f2;
goto _LL37;} else{ goto _LL38;} _LL38: if(*(( int*) _temp0) == Cyc_Absyn_Sizeoftyp_e){
_LL120: _temp119=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp0)->f1;
goto _LL39;} else{ goto _LL40;} _LL40: if(*(( int*) _temp0) == Cyc_Absyn_Enum_e){
_LL126: _temp125=(( struct Cyc_Absyn_Enum_e_struct*) _temp0)->f1; goto _LL124;
_LL124: _temp123=(( struct Cyc_Absyn_Enum_e_struct*) _temp0)->f2; goto _LL122;
_LL122: _temp121=(( struct Cyc_Absyn_Enum_e_struct*) _temp0)->f3; goto _LL41;}
else{ goto _LL42;} _LL42: if(*(( int*) _temp0) == Cyc_Absyn_UnresolvedMem_e){
_LL130: _temp129=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp0)->f1; goto
_LL128; _LL128: _temp127=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp0)->f2;
goto _LL43;} else{ goto _LL44;} _LL44: goto _LL45; _LL3: if((( int(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( env.varmap,
_temp51)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( env.varmap,
_temp51))->r)); continue;} for( 0; env.boundvars != 0; env.boundvars=(( struct
Cyc_List_List*) _check_null( env.boundvars))->tl){ if( Cyc_String_zstrptrcmp(
_temp51,(*(( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
env.boundvars))->hd)->name).f2) == 0){ return;}} for( 0; env.all_locals != 0;
env.all_locals=(( struct Cyc_List_List*) _check_null( env.all_locals))->tl){
struct Cyc_Absyn_Vardecl* _temp131=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( env.all_locals))->hd; goto _LL132; _LL132: if( Cyc_String_zstrptrcmp(
_temp51,(* _temp131->name).f2) == 0){( void*)( e->r=( void*)(( void*)( Cyc_Absyn_deref_exp(
Cyc_Absyn_var_exp( _temp131->name, 0), 0))->r));* env.freevars=(( struct Cyc_Set_Set*(*)(
struct _RegionHandle* rgn, struct Cyc_Set_Set* s, struct Cyc_Absyn_Vardecl* elt))
Cyc_Set_rinsert)( env.rgn,* env.freevars, _temp131); return;}}( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp133=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp133[ 0]=({ struct
Cyc_Core_Impossible_struct _temp134; _temp134.tag= Cyc_Core_Impossible; _temp134.f1=({
struct _tagged_string _temp135=* _temp51; xprintf("unbound variable %.*s found in box_free_vars_exp",
_temp135.last_plus_one - _temp135.curr, _temp135.curr);}); _temp134;}); _temp133;}));
_LL5: return; _LL7: return; _LL9: Cyc_Tovc_box_free_vars_exp( env, _temp65);
_temp67= _temp63; goto _LL11; _LL11: for( 0; _temp67 != 0; _temp67=(( struct Cyc_List_List*)
_check_null( _temp67))->tl){ Cyc_Tovc_box_free_vars_exp( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp67))->hd);} return; _LL13: Cyc_Tovc_box_free_vars_exp(
env, _temp71); _temp79= _temp75; _temp77= _temp73; goto _LL15; _LL15: _temp83=
_temp79; _temp81= _temp77; goto _LL17; _LL17: _temp89= _temp83; _temp85= _temp81;
goto _LL19; _LL19: Cyc_Tovc_box_free_vars_exp( env, _temp85); _temp91= _temp89;
goto _LL21; _LL21: _temp95= _temp91; goto _LL23; _LL23: _temp97= _temp95; goto
_LL25; _LL25: _temp101= _temp97; goto _LL27; _LL27: _temp103= _temp101; goto
_LL29; _LL29: _temp105= _temp103; goto _LL31; _LL31: _temp109= _temp105; goto
_LL33; _LL33: _temp113= _temp109; goto _LL35; _LL35: _temp117= _temp113; goto
_LL37; _LL37: Cyc_Tovc_box_free_vars_exp( env, _temp117); return; _LL39: return;
_LL41: return; _LL43: for( 0; _temp127 != 0; _temp127=(( struct Cyc_List_List*)
_check_null( _temp127))->tl){ Cyc_Tovc_box_free_vars_exp( env,(*(( struct
_tuple3*)(( struct Cyc_List_List*) _check_null( _temp127))->hd)).f2);} return;
_LL45:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp136=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp136[ 0]=({ struct Cyc_Core_Impossible_struct _temp137; _temp137.tag= Cyc_Core_Impossible;
_temp137.f1=({ char* _temp138="bad exp form in Tocv::box_free_vars_exp"; struct
_tagged_string _temp139; _temp139.curr= _temp138; _temp139.base= _temp138;
_temp139.last_plus_one= _temp138 + 40; _temp139;}); _temp137;}); _temp136;}));
_LL1:;}} static void Cyc_Tovc_box_free_vars_stmt( struct Cyc_Tovc_BoxingEnv env,
struct Cyc_Absyn_Stmt* s){ while( 1) { void* _temp140=( void*) s->r; struct Cyc_Absyn_Stmt*
_temp172; struct Cyc_Absyn_Stmt* _temp174; struct Cyc_Absyn_Stmt* _temp176;
struct _tagged_string* _temp178; struct Cyc_Absyn_Stmt* _temp180; struct
_tagged_string* _temp182; struct Cyc_Absyn_Exp* _temp184; struct Cyc_Absyn_Exp*
_temp186; struct Cyc_Absyn_Stmt* _temp188; struct Cyc_Absyn_Stmt* _temp190;
struct Cyc_Absyn_Stmt* _temp192; struct Cyc_Absyn_Stmt* _temp194; struct Cyc_Absyn_Exp*
_temp196; struct Cyc_Absyn_Stmt* _temp198; struct _tuple2 _temp200; struct Cyc_Absyn_Stmt*
_temp202; struct Cyc_Absyn_Exp* _temp204; struct Cyc_Absyn_Stmt* _temp206;
struct _tuple2 _temp208; struct Cyc_Absyn_Stmt* _temp210; struct Cyc_Absyn_Exp*
_temp212; struct _tuple2 _temp214; struct Cyc_Absyn_Stmt* _temp216; struct Cyc_Absyn_Exp*
_temp218; struct Cyc_Absyn_Exp* _temp220; struct _tuple2 _temp222; struct Cyc_Absyn_Stmt*
_temp224; struct Cyc_Absyn_Exp* _temp226; struct Cyc_Absyn_Stmt* _temp228;
struct Cyc_List_List* _temp230; struct Cyc_Absyn_Exp* _temp232; struct Cyc_Absyn_Stmt*
_temp234; struct Cyc_Absyn_Decl* _temp236; _LL142: if( _temp140 ==( void*) Cyc_Absyn_Skip_s){
goto _LL143;} else{ goto _LL144;} _LL144: if(( unsigned int) _temp140 > 1u?*((
int*) _temp140) == Cyc_Absyn_Break_s: 0){ _LL173: _temp172=(( struct Cyc_Absyn_Break_s_struct*)
_temp140)->f1; goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Continue_s: 0){ _LL175: _temp174=((
struct Cyc_Absyn_Continue_s_struct*) _temp140)->f1; goto _LL147;} else{ goto
_LL148;} _LL148: if(( unsigned int) _temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Goto_s:
0){ _LL179: _temp178=(( struct Cyc_Absyn_Goto_s_struct*) _temp140)->f1; goto
_LL177; _LL177: _temp176=(( struct Cyc_Absyn_Goto_s_struct*) _temp140)->f2; goto
_LL149;} else{ goto _LL150;} _LL150: if(( unsigned int) _temp140 > 1u?*(( int*)
_temp140) == Cyc_Absyn_Label_s: 0){ _LL183: _temp182=(( struct Cyc_Absyn_Label_s_struct*)
_temp140)->f1; goto _LL181; _LL181: _temp180=(( struct Cyc_Absyn_Label_s_struct*)
_temp140)->f2; goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Return_s: 0){ _LL185: _temp184=((
struct Cyc_Absyn_Return_s_struct*) _temp140)->f1; goto _LL153;} else{ goto
_LL154;} _LL154: if(( unsigned int) _temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Exp_s:
0){ _LL187: _temp186=(( struct Cyc_Absyn_Exp_s_struct*) _temp140)->f1; goto
_LL155;} else{ goto _LL156;} _LL156: if(( unsigned int) _temp140 > 1u?*(( int*)
_temp140) == Cyc_Absyn_Seq_s: 0){ _LL191: _temp190=(( struct Cyc_Absyn_Seq_s_struct*)
_temp140)->f1; goto _LL189; _LL189: _temp188=(( struct Cyc_Absyn_Seq_s_struct*)
_temp140)->f2; goto _LL157;} else{ goto _LL158;} _LL158: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_IfThenElse_s: 0){ _LL197:
_temp196=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp140)->f1; goto _LL195;
_LL195: _temp194=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp140)->f2; goto
_LL193; _LL193: _temp192=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp140)->f3;
goto _LL159;} else{ goto _LL160;} _LL160: if(( unsigned int) _temp140 > 1u?*((
int*) _temp140) == Cyc_Absyn_While_s: 0){ _LL201: _temp200=(( struct Cyc_Absyn_While_s_struct*)
_temp140)->f1; _LL205: _temp204= _temp200.f1; goto _LL203; _LL203: _temp202=
_temp200.f2; goto _LL199; _LL199: _temp198=(( struct Cyc_Absyn_While_s_struct*)
_temp140)->f2; goto _LL161;} else{ goto _LL162;} _LL162: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_For_s: 0){ _LL221: _temp220=((
struct Cyc_Absyn_For_s_struct*) _temp140)->f1; goto _LL215; _LL215: _temp214=((
struct Cyc_Absyn_For_s_struct*) _temp140)->f2; _LL219: _temp218= _temp214.f1;
goto _LL217; _LL217: _temp216= _temp214.f2; goto _LL209; _LL209: _temp208=((
struct Cyc_Absyn_For_s_struct*) _temp140)->f3; _LL213: _temp212= _temp208.f1;
goto _LL211; _LL211: _temp210= _temp208.f2; goto _LL207; _LL207: _temp206=((
struct Cyc_Absyn_For_s_struct*) _temp140)->f4; goto _LL163;} else{ goto _LL164;}
_LL164: if(( unsigned int) _temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Do_s:
0){ _LL229: _temp228=(( struct Cyc_Absyn_Do_s_struct*) _temp140)->f1; goto
_LL223; _LL223: _temp222=(( struct Cyc_Absyn_Do_s_struct*) _temp140)->f2; _LL227:
_temp226= _temp222.f1; goto _LL225; _LL225: _temp224= _temp222.f2; goto _LL165;}
else{ goto _LL166;} _LL166: if(( unsigned int) _temp140 > 1u?*(( int*) _temp140)
== Cyc_Absyn_Switch_s: 0){ _LL233: _temp232=(( struct Cyc_Absyn_Switch_s_struct*)
_temp140)->f1; goto _LL231; _LL231: _temp230=(( struct Cyc_Absyn_Switch_s_struct*)
_temp140)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if(( unsigned int)
_temp140 > 1u?*(( int*) _temp140) == Cyc_Absyn_Decl_s: 0){ _LL237: _temp236=((
struct Cyc_Absyn_Decl_s_struct*) _temp140)->f1; goto _LL235; _LL235: _temp234=((
struct Cyc_Absyn_Decl_s_struct*) _temp140)->f2; goto _LL169;} else{ goto _LL170;}
_LL170: goto _LL171; _LL143: return; _LL145: return; _LL147: return; _LL149:
return; _LL151: s= _temp180; continue; _LL153: if( _temp184 == 0){ return;}
_temp186=( struct Cyc_Absyn_Exp*) _check_null( _temp184); goto _LL155; _LL155:
Cyc_Tovc_box_free_vars_exp( env, _temp186); return; _LL157: Cyc_Tovc_box_free_vars_stmt(
env, _temp190); s= _temp188; continue; _LL159: Cyc_Tovc_box_free_vars_exp( env,
_temp196); Cyc_Tovc_box_free_vars_stmt( env, _temp194); s= _temp192; continue;
_LL161: Cyc_Tovc_box_free_vars_exp( env, _temp204); s= _temp198; continue;
_LL163: Cyc_Tovc_box_free_vars_exp( env, _temp220); Cyc_Tovc_box_free_vars_exp(
env, _temp218); Cyc_Tovc_box_free_vars_exp( env, _temp212); s= _temp206;
continue; _LL165: Cyc_Tovc_box_free_vars_exp( env, _temp226); s= _temp228;
continue; _LL167: Cyc_Tovc_box_free_vars_exp( env, _temp232); for( 0; _temp230
!= 0; _temp230=(( struct Cyc_List_List*) _check_null( _temp230))->tl){ Cyc_Tovc_box_free_vars_stmt(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp230))->hd)->body);} return; _LL169:{ void* _temp238=( void*) _temp236->r;
struct Cyc_Absyn_Vardecl* _temp244; _LL240: if(*(( int*) _temp238) == Cyc_Absyn_Var_d){
_LL245: _temp244=(( struct Cyc_Absyn_Var_d_struct*) _temp238)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: goto _LL243; _LL241: env.boundvars=({ struct Cyc_List_List*
_temp246=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp246->hd=( void*) _temp244; _temp246->tl= env.boundvars; _temp246;}); env.varmap=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*, struct _tagged_string*)) Cyc_Dict_rdelete_same)(
env.varmap,(* _temp244->name).f2); if( _temp244->initializer != 0){ Cyc_Tovc_box_free_vars_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp244->initializer));} s= _temp234;
continue; _LL243: goto _LL239; _LL239:;} goto _LL171; _LL171:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp247=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp247[ 0]=({ struct
Cyc_Core_Impossible_struct _temp248; _temp248.tag= Cyc_Core_Impossible; _temp248.f1=({
char* _temp249="bad stmt after xlation to C"; struct _tagged_string _temp250;
_temp250.curr= _temp249; _temp250.base= _temp249; _temp250.last_plus_one=
_temp249 + 28; _temp250;}); _temp248;}); _temp247;})); _LL141:;}} struct _tuple4{
struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; static
struct Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_fun( struct Cyc_Tovc_ToExpEnv env, struct
Cyc_Absyn_Stmt* s, void* rettype){ struct Cyc_Set_Set* freevars=(( struct Cyc_Set_Set*(*)(
struct _RegionHandle* rgn, int(* comp)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*)))
Cyc_Set_rempty)( env.rgn,( int(*)( struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Vardecl*))
Cyc_Core_ptrcmp); struct Cyc_Tovc_BoxingEnv _temp252=({ struct Cyc_Tovc_BoxingEnv
_temp251; _temp251.rgn= env.rgn; _temp251.all_locals= env.all_locals; _temp251.varmap=
env.varmap; _temp251.boundvars= 0; _temp251.freevars=& freevars; _temp251;});
goto _LL253; _LL253: Cyc_Tovc_box_free_vars_stmt( _temp252, s);{ struct Cyc_List_List*
params= 0; struct Cyc_List_List* args= 0; struct Cyc_List_List* free_vars=((
struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)( freevars);
for( 0; free_vars != 0; free_vars=(( struct Cyc_List_List*) _check_null(
free_vars))->tl){ struct Cyc_Absyn_Vardecl* vd=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( free_vars))->hd; params=({ struct Cyc_List_List*
_temp254=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp254->hd=( void*)({ struct _tuple4* _temp255=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp255->f1=(* vd->name).f2; _temp255->f2= vd->tq;
_temp255->f3= Cyc_Absyn_cstar_typ(( void*) vd->type, Cyc_Absyn_empty_tqual());
_temp255;}); _temp254->tl= params; _temp254;}); args=({ struct Cyc_List_List*
_temp256=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp256->hd=( void*) Cyc_Absyn_address_exp( Cyc_Absyn_var_exp( vd->name, 0), 0);
_temp256->tl= args; _temp256;});}{ struct _tuple0* funname= Cyc_Toc_temp_var();*
env.gen_ds=({ struct Cyc_List_List* _temp257=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp257->hd=( void*) Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Fn_d_struct* _temp258=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp258[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp259; _temp259.tag= Cyc_Absyn_Fn_d; _temp259.f1=({ struct Cyc_Absyn_Fndecl*
_temp260=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp260->sc=( void*)(( void*) Cyc_Absyn_Static); _temp260->is_inline= 1;
_temp260->name= funname; _temp260->tvs= 0; _temp260->effect= 0; _temp260->ret_type=(
void*) rettype; _temp260->args= params; _temp260->c_varargs= 0; _temp260->cyc_varargs=
0; _temp260->body= s; _temp260->cached_typ= 0; _temp260->param_vardecls= 0;
_temp260->attributes= 0; _temp260;}); _temp259;}); _temp258;}), 0); _temp257->tl=*
env.gen_ds; _temp257;}); return Cyc_Absyn_fncall_exp( Cyc_Absyn_var_exp( funname,
0), args, 0);}}} static void Cyc_Tovc_apply_varmap( struct Cyc_Dict_Dict* varmap,
struct Cyc_Absyn_Exp* e){ void* _temp261=( void*) e->r; void* _temp307; struct
_tuple0* _temp309; struct _tuple0 _temp311; struct _tagged_string* _temp312;
void* _temp314; void* _temp316; struct _tuple0* _temp318; void* _temp320; struct
Cyc_Absyn_VarargCallInfo* _temp322; struct Cyc_List_List* _temp324; struct Cyc_Absyn_Exp*
_temp326; struct Cyc_List_List* _temp328; void* _temp330; struct Cyc_Absyn_Exp*
_temp332; struct Cyc_Absyn_Exp* _temp334; struct Cyc_Absyn_Exp* _temp336; struct
Cyc_Absyn_Exp* _temp338; struct Cyc_Absyn_Exp* _temp340; struct Cyc_Absyn_Exp*
_temp342; struct Cyc_Absyn_Exp* _temp344; struct Cyc_Absyn_Exp* _temp346; struct
Cyc_Core_Opt* _temp348; struct Cyc_Absyn_Exp* _temp350; struct Cyc_Absyn_Exp*
_temp352; struct Cyc_List_List* _temp354; struct Cyc_Absyn_Exp* _temp356; struct
Cyc_Absyn_Exp* _temp358; void* _temp360; struct Cyc_Absyn_Exp* _temp362; struct
Cyc_Absyn_Exp* _temp364; struct Cyc_Absyn_Exp* _temp366; struct _tagged_string*
_temp368; struct Cyc_Absyn_Exp* _temp370; struct _tagged_string* _temp372;
struct Cyc_Absyn_Exp* _temp374; void* _temp376; struct Cyc_Absyn_Exp* _temp378;
void* _temp380; struct Cyc_Absyn_Enumfield* _temp382; struct Cyc_Absyn_Enumdecl*
_temp384; struct _tuple0* _temp386; struct Cyc_List_List* _temp388; struct Cyc_Core_Opt*
_temp390; _LL263: if(*(( int*) _temp261) == Cyc_Absyn_Var_e){ _LL310: _temp309=((
struct Cyc_Absyn_Var_e_struct*) _temp261)->f1; _temp311=* _temp309; _LL315:
_temp314= _temp311.f1; if( _temp314 ==( void*) Cyc_Absyn_Loc_n){ goto _LL313;}
else{ goto _LL265;} _LL313: _temp312= _temp311.f2; goto _LL308; _LL308: _temp307=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp261)->f2; goto _LL264;} else{ goto
_LL265;} _LL265: if(*(( int*) _temp261) == Cyc_Absyn_Var_e){ _LL319: _temp318=((
struct Cyc_Absyn_Var_e_struct*) _temp261)->f1; goto _LL317; _LL317: _temp316=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp261)->f2; goto _LL266;} else{ goto
_LL267;} _LL267: if(*(( int*) _temp261) == Cyc_Absyn_Const_e){ _LL321: _temp320=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp261)->f1; goto _LL268;} else{
goto _LL269;} _LL269: if(*(( int*) _temp261) == Cyc_Absyn_FnCall_e){ _LL327:
_temp326=(( struct Cyc_Absyn_FnCall_e_struct*) _temp261)->f1; goto _LL325;
_LL325: _temp324=(( struct Cyc_Absyn_FnCall_e_struct*) _temp261)->f2; goto
_LL323; _LL323: _temp322=(( struct Cyc_Absyn_FnCall_e_struct*) _temp261)->f3;
goto _LL270;} else{ goto _LL271;} _LL271: if(*(( int*) _temp261) == Cyc_Absyn_Primop_e){
_LL331: _temp330=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp261)->f1;
goto _LL329; _LL329: _temp328=(( struct Cyc_Absyn_Primop_e_struct*) _temp261)->f2;
goto _LL272;} else{ goto _LL273;} _LL273: if(*(( int*) _temp261) == Cyc_Absyn_Conditional_e){
_LL337: _temp336=(( struct Cyc_Absyn_Conditional_e_struct*) _temp261)->f1; goto
_LL335; _LL335: _temp334=(( struct Cyc_Absyn_Conditional_e_struct*) _temp261)->f2;
goto _LL333; _LL333: _temp332=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp261)->f3; goto _LL274;} else{ goto _LL275;} _LL275: if(*(( int*) _temp261)
== Cyc_Absyn_Subscript_e){ _LL341: _temp340=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp261)->f1; goto _LL339; _LL339: _temp338=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp261)->f2; goto _LL276;} else{ goto _LL277;} _LL277: if(*(( int*) _temp261)
== Cyc_Absyn_SeqExp_e){ _LL345: _temp344=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp261)->f1; goto _LL343; _LL343: _temp342=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp261)->f2; goto _LL278;} else{ goto _LL279;} _LL279: if(*(( int*) _temp261)
== Cyc_Absyn_AssignOp_e){ _LL351: _temp350=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp261)->f1; goto _LL349; _LL349: _temp348=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp261)->f2; goto _LL347; _LL347: _temp346=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp261)->f3; goto _LL280;} else{ goto _LL281;} _LL281: if(*(( int*) _temp261)
== Cyc_Absyn_NoInstantiate_e){ _LL353: _temp352=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp261)->f1; goto _LL282;} else{ goto _LL283;} _LL283: if(*(( int*) _temp261)
== Cyc_Absyn_Instantiate_e){ _LL357: _temp356=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp261)->f1; goto _LL355; _LL355: _temp354=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp261)->f2; goto _LL284;} else{ goto _LL285;} _LL285: if(*(( int*) _temp261)
== Cyc_Absyn_Cast_e){ _LL361: _temp360=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp261)->f1; goto _LL359; _LL359: _temp358=(( struct Cyc_Absyn_Cast_e_struct*)
_temp261)->f2; goto _LL286;} else{ goto _LL287;} _LL287: if(*(( int*) _temp261)
== Cyc_Absyn_Address_e){ _LL363: _temp362=(( struct Cyc_Absyn_Address_e_struct*)
_temp261)->f1; goto _LL288;} else{ goto _LL289;} _LL289: if(*(( int*) _temp261)
== Cyc_Absyn_Sizeofexp_e){ _LL365: _temp364=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp261)->f1; goto _LL290;} else{ goto _LL291;} _LL291: if(*(( int*) _temp261)
== Cyc_Absyn_Deref_e){ _LL367: _temp366=(( struct Cyc_Absyn_Deref_e_struct*)
_temp261)->f1; goto _LL292;} else{ goto _LL293;} _LL293: if(*(( int*) _temp261)
== Cyc_Absyn_StructMember_e){ _LL371: _temp370=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp261)->f1; goto _LL369; _LL369: _temp368=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp261)->f2; goto _LL294;} else{ goto _LL295;} _LL295: if(*(( int*) _temp261)
== Cyc_Absyn_StructArrow_e){ _LL375: _temp374=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp261)->f1; goto _LL373; _LL373: _temp372=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp261)->f2; goto _LL296;} else{ goto _LL297;} _LL297: if(*(( int*) _temp261)
== Cyc_Absyn_Increment_e){ _LL379: _temp378=(( struct Cyc_Absyn_Increment_e_struct*)
_temp261)->f1; goto _LL377; _LL377: _temp376=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp261)->f2; goto _LL298;} else{ goto _LL299;} _LL299: if(*(( int*) _temp261)
== Cyc_Absyn_Sizeoftyp_e){ _LL381: _temp380=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp261)->f1; goto _LL300;} else{ goto _LL301;} _LL301: if(*(( int*) _temp261)
== Cyc_Absyn_Enum_e){ _LL387: _temp386=(( struct Cyc_Absyn_Enum_e_struct*)
_temp261)->f1; goto _LL385; _LL385: _temp384=(( struct Cyc_Absyn_Enum_e_struct*)
_temp261)->f2; goto _LL383; _LL383: _temp382=(( struct Cyc_Absyn_Enum_e_struct*)
_temp261)->f3; goto _LL302;} else{ goto _LL303;} _LL303: if(*(( int*) _temp261)
== Cyc_Absyn_UnresolvedMem_e){ _LL391: _temp390=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp261)->f1; goto _LL389; _LL389: _temp388=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp261)->f2; goto _LL304;} else{ goto _LL305;} _LL305: goto _LL306; _LL264:
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)(
varmap, _temp312)){( void*)( e->r=( void*)(( void*)((( struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( varmap,
_temp312))->r));} return; _LL266: return; _LL268: return; _LL270: Cyc_Tovc_apply_varmap(
varmap, _temp326); _temp328= _temp324; goto _LL272; _LL272: for( 0; _temp328 !=
0; _temp328=(( struct Cyc_List_List*) _check_null( _temp328))->tl){ Cyc_Tovc_apply_varmap(
varmap,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp328))->hd);}
return; _LL274: Cyc_Tovc_apply_varmap( varmap, _temp332); _temp340= _temp336;
_temp338= _temp334; goto _LL276; _LL276: _temp344= _temp340; _temp342= _temp338;
goto _LL278; _LL278: _temp350= _temp344; _temp346= _temp342; goto _LL280; _LL280:
Cyc_Tovc_apply_varmap( varmap, _temp346); _temp352= _temp350; goto _LL282;
_LL282: _temp356= _temp352; goto _LL284; _LL284: _temp358= _temp356; goto _LL286;
_LL286: _temp362= _temp358; goto _LL288; _LL288: _temp364= _temp362; goto _LL290;
_LL290: _temp366= _temp364; goto _LL292; _LL292: _temp370= _temp366; goto _LL294;
_LL294: _temp374= _temp370; goto _LL296; _LL296: _temp378= _temp374; goto _LL298;
_LL298: Cyc_Tovc_apply_varmap( varmap, _temp378); return; _LL300: return; _LL302:
return; _LL304: for( 0; _temp388 != 0; _temp388=(( struct Cyc_List_List*)
_check_null( _temp388))->tl){ Cyc_Tovc_apply_varmap( varmap,(*(( struct _tuple3*)((
struct Cyc_List_List*) _check_null( _temp388))->hd)).f2);} return; _LL306:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp392=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp392[ 0]=({ struct
Cyc_Core_Impossible_struct _temp393; _temp393.tag= Cyc_Core_Impossible; _temp393.f1=({
char* _temp394="bad exp form in Tocv::apply_varmap"; struct _tagged_string
_temp395; _temp395.curr= _temp394; _temp395.base= _temp394; _temp395.last_plus_one=
_temp394 + 35; _temp395;}); _temp393;}); _temp392;})); _LL262:;} static struct
Cyc_Absyn_Exp* Cyc_Tovc_stmt_to_exp( struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt*
s){ void* _temp396=( void*) s->r; struct Cyc_Absyn_Exp* _temp410; struct Cyc_Absyn_Stmt*
_temp412; struct Cyc_Absyn_Stmt* _temp414; struct Cyc_Absyn_Stmt* _temp416;
struct Cyc_Absyn_Stmt* _temp418; struct Cyc_Absyn_Exp* _temp420; struct Cyc_Absyn_Stmt*
_temp422; struct Cyc_Absyn_Decl* _temp424; _LL398: if( _temp396 ==( void*) Cyc_Absyn_Skip_s){
goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int) _temp396 > 1u?*((
int*) _temp396) == Cyc_Absyn_Exp_s: 0){ _LL411: _temp410=(( struct Cyc_Absyn_Exp_s_struct*)
_temp396)->f1; goto _LL401;} else{ goto _LL402;} _LL402: if(( unsigned int)
_temp396 > 1u?*(( int*) _temp396) == Cyc_Absyn_Seq_s: 0){ _LL415: _temp414=((
struct Cyc_Absyn_Seq_s_struct*) _temp396)->f1; goto _LL413; _LL413: _temp412=((
struct Cyc_Absyn_Seq_s_struct*) _temp396)->f2; goto _LL403;} else{ goto _LL404;}
_LL404: if(( unsigned int) _temp396 > 1u?*(( int*) _temp396) == Cyc_Absyn_IfThenElse_s:
0){ _LL421: _temp420=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp396)->f1;
goto _LL419; _LL419: _temp418=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp396)->f2;
goto _LL417; _LL417: _temp416=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp396)->f3;
goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int) _temp396 > 1u?*((
int*) _temp396) == Cyc_Absyn_Decl_s: 0){ _LL425: _temp424=(( struct Cyc_Absyn_Decl_s_struct*)
_temp396)->f1; goto _LL423; _LL423: _temp422=(( struct Cyc_Absyn_Decl_s_struct*)
_temp396)->f2; goto _LL407;} else{ goto _LL408;} _LL408: goto _LL409; _LL399:
return Cyc_Absyn_true_exp( 0); _LL401: Cyc_Tovc_apply_varmap( env.varmap,
_temp410); return _temp410; _LL403: return Cyc_Absyn_seq_exp( Cyc_Tovc_stmt_to_exp(
env, _temp414), Cyc_Tovc_stmt_to_exp( env, _temp412), 0); _LL405: Cyc_Tovc_apply_varmap(
env.varmap, _temp420); return Cyc_Absyn_conditional_exp( _temp420, Cyc_Tovc_stmt_to_exp(
env, _temp418), Cyc_Tovc_stmt_to_exp( env, _temp416), 0); _LL407: { void*
_temp426=( void*) _temp424->r; struct Cyc_Absyn_Vardecl* _temp432; _LL428: if(*((
int*) _temp426) == Cyc_Absyn_Var_d){ _LL433: _temp432=(( struct Cyc_Absyn_Var_d_struct*)
_temp426)->f1; goto _LL429;} else{ goto _LL430;} _LL430: goto _LL431; _LL429: {
struct _tagged_string* _temp434=(* _temp432->name).f2; goto _LL435; _LL435: {
struct _tuple0* _temp436= Cyc_Toc_temp_var(); goto _LL437; _LL437: _temp432->name=
_temp436; env.varmap=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct Cyc_Absyn_Exp* data)) Cyc_Dict_insert)( env.varmap,
_temp434, Cyc_Absyn_var_exp( _temp436, 0)); if( _temp432->initializer != 0){ Cyc_Tovc_apply_varmap(
env.varmap,( struct Cyc_Absyn_Exp*) _check_null( _temp432->initializer));{ void*
_temp438=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp432->initializer))->r;
struct Cyc_List_List* _temp444; struct Cyc_Core_Opt* _temp446; _LL440: if(*((
int*) _temp438) == Cyc_Absyn_UnresolvedMem_e){ _LL447: _temp446=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp438)->f1; goto _LL445; _LL445: _temp444=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp438)->f2; goto _LL441;} else{ goto _LL442;} _LL442: goto _LL443; _LL441: {
struct _tuple0* tmp= Cyc_Toc_temp_var(); struct Cyc_Absyn_Stmt* body= Cyc_Absyn_declare_stmt(
tmp,( void*) _temp432->type, _temp432->initializer, Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_Absyn_var_exp( tmp, 0), 0), 0); _temp432->initializer=(
struct Cyc_Absyn_Exp*) Cyc_Tovc_stmt_to_fun( env, body,( void*) _temp432->type);
goto _LL439;} _LL443: goto _LL439; _LL439:;}} env.all_locals=({ struct Cyc_List_List*
_temp448=( struct Cyc_List_List*) _region_malloc( env.rgn, sizeof( struct Cyc_List_List));
_temp448->hd=( void*) _temp432; _temp448->tl= env.all_locals; _temp448;});{
struct Cyc_Absyn_Exp* _temp449= _temp432->initializer; goto _LL450; _LL450:
_temp432->initializer= 0;( void*)(( env.encloser)->r=( void*)(( void*)( Cyc_Absyn_decl_stmt(
_temp424, Cyc_Absyn_new_stmt(( void*)( env.encloser)->r, 0), 0))->r));{ struct
Cyc_Absyn_Exp* _temp451= Cyc_Tovc_stmt_to_exp( env, _temp422); goto _LL452;
_LL452: if( _temp449 != 0){ _temp451= Cyc_Absyn_seq_exp( Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp( _temp432->name, 0),( struct Cyc_Absyn_Exp*) _check_null(
_temp449), 0), _temp451, 0);} return _temp451;}}}} _LL431:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp453=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp453[ 0]=({ struct
Cyc_Core_Impossible_struct _temp454; _temp454.tag= Cyc_Core_Impossible; _temp454.f1=({
char* _temp455="bad local decl in Tovc::stmt_to_exp"; struct _tagged_string
_temp456; _temp456.curr= _temp455; _temp456.base= _temp455; _temp456.last_plus_one=
_temp455 + 36; _temp456;}); _temp454;}); _temp453;})); _LL427:;} _LL409: return
Cyc_Tovc_stmt_to_fun( env, s,( void*) Cyc_Absyn_VoidType); _LL397:;} static void
Cyc_Tovc_stmt_to_vc( struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s);
static void Cyc_Tovc_exp_to_vc( struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Exp*
e){ void* _temp457=( void*) e->r; void* _temp503; void* _temp505; struct _tuple0*
_temp507; struct Cyc_Absyn_VarargCallInfo* _temp509; struct Cyc_List_List*
_temp511; struct Cyc_Absyn_Exp* _temp513; struct Cyc_List_List* _temp515; void*
_temp517; struct Cyc_Absyn_Exp* _temp519; struct Cyc_Absyn_Exp* _temp521; struct
Cyc_Absyn_Exp* _temp523; struct Cyc_Absyn_Exp* _temp525; struct Cyc_Absyn_Exp*
_temp527; struct Cyc_Absyn_Exp* _temp529; struct Cyc_Absyn_Exp* _temp531; struct
Cyc_Absyn_Exp* _temp533; struct Cyc_Core_Opt* _temp535; struct Cyc_Absyn_Exp*
_temp537; struct Cyc_Absyn_Exp* _temp539; struct Cyc_List_List* _temp541; struct
Cyc_Absyn_Exp* _temp543; struct Cyc_Absyn_Exp* _temp545; void* _temp547; struct
Cyc_Absyn_Exp* _temp549; struct Cyc_Absyn_Exp* _temp551; struct Cyc_Absyn_Exp*
_temp553; struct _tagged_string* _temp555; struct Cyc_Absyn_Exp* _temp557;
struct _tagged_string* _temp559; struct Cyc_Absyn_Exp* _temp561; void* _temp563;
struct Cyc_Absyn_Exp* _temp565; void* _temp567; struct Cyc_Absyn_Enumfield*
_temp569; struct Cyc_Absyn_Enumdecl* _temp571; struct _tuple0* _temp573; struct
Cyc_List_List* _temp575; struct Cyc_Core_Opt* _temp577; struct Cyc_Absyn_Stmt*
_temp579; _LL459: if(*(( int*) _temp457) == Cyc_Absyn_Const_e){ _LL504: _temp503=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp457)->f1; goto _LL460;} else{
goto _LL461;} _LL461: if(*(( int*) _temp457) == Cyc_Absyn_Var_e){ _LL508:
_temp507=(( struct Cyc_Absyn_Var_e_struct*) _temp457)->f1; goto _LL506; _LL506:
_temp505=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp457)->f2; goto _LL462;}
else{ goto _LL463;} _LL463: if(*(( int*) _temp457) == Cyc_Absyn_FnCall_e){
_LL514: _temp513=(( struct Cyc_Absyn_FnCall_e_struct*) _temp457)->f1; goto
_LL512; _LL512: _temp511=(( struct Cyc_Absyn_FnCall_e_struct*) _temp457)->f2;
goto _LL510; _LL510: _temp509=(( struct Cyc_Absyn_FnCall_e_struct*) _temp457)->f3;
goto _LL464;} else{ goto _LL465;} _LL465: if(*(( int*) _temp457) == Cyc_Absyn_Primop_e){
_LL518: _temp517=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp457)->f1;
goto _LL516; _LL516: _temp515=(( struct Cyc_Absyn_Primop_e_struct*) _temp457)->f2;
goto _LL466;} else{ goto _LL467;} _LL467: if(*(( int*) _temp457) == Cyc_Absyn_Conditional_e){
_LL524: _temp523=(( struct Cyc_Absyn_Conditional_e_struct*) _temp457)->f1; goto
_LL522; _LL522: _temp521=(( struct Cyc_Absyn_Conditional_e_struct*) _temp457)->f2;
goto _LL520; _LL520: _temp519=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp457)->f3; goto _LL468;} else{ goto _LL469;} _LL469: if(*(( int*) _temp457)
== Cyc_Absyn_Subscript_e){ _LL528: _temp527=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp457)->f1; goto _LL526; _LL526: _temp525=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp457)->f2; goto _LL470;} else{ goto _LL471;} _LL471: if(*(( int*) _temp457)
== Cyc_Absyn_SeqExp_e){ _LL532: _temp531=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp457)->f1; goto _LL530; _LL530: _temp529=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp457)->f2; goto _LL472;} else{ goto _LL473;} _LL473: if(*(( int*) _temp457)
== Cyc_Absyn_AssignOp_e){ _LL538: _temp537=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp457)->f1; goto _LL536; _LL536: _temp535=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp457)->f2; goto _LL534; _LL534: _temp533=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp457)->f3; goto _LL474;} else{ goto _LL475;} _LL475: if(*(( int*) _temp457)
== Cyc_Absyn_NoInstantiate_e){ _LL540: _temp539=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp457)->f1; goto _LL476;} else{ goto _LL477;} _LL477: if(*(( int*) _temp457)
== Cyc_Absyn_Instantiate_e){ _LL544: _temp543=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp457)->f1; goto _LL542; _LL542: _temp541=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp457)->f2; goto _LL478;} else{ goto _LL479;} _LL479: if(*(( int*) _temp457)
== Cyc_Absyn_Cast_e){ _LL548: _temp547=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp457)->f1; goto _LL546; _LL546: _temp545=(( struct Cyc_Absyn_Cast_e_struct*)
_temp457)->f2; goto _LL480;} else{ goto _LL481;} _LL481: if(*(( int*) _temp457)
== Cyc_Absyn_Address_e){ _LL550: _temp549=(( struct Cyc_Absyn_Address_e_struct*)
_temp457)->f1; goto _LL482;} else{ goto _LL483;} _LL483: if(*(( int*) _temp457)
== Cyc_Absyn_Sizeofexp_e){ _LL552: _temp551=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp457)->f1; goto _LL484;} else{ goto _LL485;} _LL485: if(*(( int*) _temp457)
== Cyc_Absyn_Deref_e){ _LL554: _temp553=(( struct Cyc_Absyn_Deref_e_struct*)
_temp457)->f1; goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp457)
== Cyc_Absyn_StructMember_e){ _LL558: _temp557=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp457)->f1; goto _LL556; _LL556: _temp555=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp457)->f2; goto _LL488;} else{ goto _LL489;} _LL489: if(*(( int*) _temp457)
== Cyc_Absyn_StructArrow_e){ _LL562: _temp561=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp457)->f1; goto _LL560; _LL560: _temp559=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp457)->f2; goto _LL490;} else{ goto _LL491;} _LL491: if(*(( int*) _temp457)
== Cyc_Absyn_Increment_e){ _LL566: _temp565=(( struct Cyc_Absyn_Increment_e_struct*)
_temp457)->f1; goto _LL564; _LL564: _temp563=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp457)->f2; goto _LL492;} else{ goto _LL493;} _LL493: if(*(( int*) _temp457)
== Cyc_Absyn_Sizeoftyp_e){ _LL568: _temp567=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp457)->f1; goto _LL494;} else{ goto _LL495;} _LL495: if(*(( int*) _temp457)
== Cyc_Absyn_Enum_e){ _LL574: _temp573=(( struct Cyc_Absyn_Enum_e_struct*)
_temp457)->f1; goto _LL572; _LL572: _temp571=(( struct Cyc_Absyn_Enum_e_struct*)
_temp457)->f2; goto _LL570; _LL570: _temp569=(( struct Cyc_Absyn_Enum_e_struct*)
_temp457)->f3; goto _LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp457)
== Cyc_Absyn_UnresolvedMem_e){ _LL578: _temp577=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp457)->f1; goto _LL576; _LL576: _temp575=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp457)->f2; goto _LL498;} else{ goto _LL499;} _LL499: if(*(( int*) _temp457)
== Cyc_Absyn_StmtExp_e){ _LL580: _temp579=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp457)->f1; goto _LL500;} else{ goto _LL501;} _LL501: goto _LL502; _LL460:
return; _LL462: return; _LL464: Cyc_Tovc_exp_to_vc( env, _temp513); _temp515=
_temp511; goto _LL466; _LL466: for( 0; _temp515 != 0; _temp515=(( struct Cyc_List_List*)
_check_null( _temp515))->tl){ Cyc_Tovc_exp_to_vc( env,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp515))->hd);} return; _LL468: Cyc_Tovc_exp_to_vc(
env, _temp519); _temp527= _temp523; _temp525= _temp521; goto _LL470; _LL470:
_temp531= _temp527; _temp529= _temp525; goto _LL472; _LL472: _temp537= _temp531;
_temp533= _temp529; goto _LL474; _LL474: Cyc_Tovc_exp_to_vc( env, _temp533);
_temp539= _temp537; goto _LL476; _LL476: _temp543= _temp539; goto _LL478; _LL478:
_temp545= _temp543; goto _LL480; _LL480: _temp549= _temp545; goto _LL482; _LL482:
_temp551= _temp549; goto _LL484; _LL484: _temp553= _temp551; goto _LL486; _LL486:
_temp557= _temp553; goto _LL488; _LL488: _temp561= _temp557; goto _LL490; _LL490:
_temp565= _temp561; goto _LL492; _LL492: Cyc_Tovc_exp_to_vc( env, _temp565);
return; _LL494: return; _LL496: return; _LL498: for( 0; _temp575 != 0; _temp575=((
struct Cyc_List_List*) _check_null( _temp575))->tl){ Cyc_Tovc_exp_to_vc( env,(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp575))->hd)).f2);}
return; _LL500: Cyc_Tovc_stmt_to_vc( env, _temp579);( void*)( e->r=( void*)((
void*)( Cyc_Tovc_stmt_to_exp( env, _temp579))->r)); return; _LL502:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp581=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp581[ 0]=({ struct
Cyc_Core_Impossible_struct _temp582; _temp582.tag= Cyc_Core_Impossible; _temp582.f1=({
char* _temp583="bad exp form after xlation to C"; struct _tagged_string _temp584;
_temp584.curr= _temp583; _temp584.base= _temp583; _temp584.last_plus_one=
_temp583 + 32; _temp584;}); _temp582;}); _temp581;})); _LL458:;} static void Cyc_Tovc_stmt_to_vc(
struct Cyc_Tovc_ToExpEnv env, struct Cyc_Absyn_Stmt* s){ while( 1) { env.encloser=
s;{ void* _temp585=( void*) s->r; struct Cyc_Absyn_Stmt* _temp617; struct Cyc_Absyn_Stmt*
_temp619; struct Cyc_Absyn_Stmt* _temp621; struct _tagged_string* _temp623;
struct Cyc_Absyn_Stmt* _temp625; struct _tagged_string* _temp627; struct Cyc_Absyn_Exp*
_temp629; struct Cyc_Absyn_Exp* _temp631; struct Cyc_Absyn_Stmt* _temp633;
struct Cyc_Absyn_Stmt* _temp635; struct Cyc_Absyn_Stmt* _temp637; struct Cyc_Absyn_Stmt*
_temp639; struct Cyc_Absyn_Exp* _temp641; struct Cyc_Absyn_Stmt* _temp643;
struct _tuple2 _temp645; struct Cyc_Absyn_Stmt* _temp647; struct Cyc_Absyn_Exp*
_temp649; struct Cyc_Absyn_Stmt* _temp651; struct _tuple2 _temp653; struct Cyc_Absyn_Stmt*
_temp655; struct Cyc_Absyn_Exp* _temp657; struct _tuple2 _temp659; struct Cyc_Absyn_Stmt*
_temp661; struct Cyc_Absyn_Exp* _temp663; struct Cyc_Absyn_Exp* _temp665; struct
_tuple2 _temp667; struct Cyc_Absyn_Stmt* _temp669; struct Cyc_Absyn_Exp*
_temp671; struct Cyc_Absyn_Stmt* _temp673; struct Cyc_List_List* _temp675;
struct Cyc_Absyn_Exp* _temp677; struct Cyc_Absyn_Stmt* _temp679; struct Cyc_Absyn_Decl*
_temp681; _LL587: if( _temp585 ==( void*) Cyc_Absyn_Skip_s){ goto _LL588;} else{
goto _LL589;} _LL589: if(( unsigned int) _temp585 > 1u?*(( int*) _temp585) ==
Cyc_Absyn_Break_s: 0){ _LL618: _temp617=(( struct Cyc_Absyn_Break_s_struct*)
_temp585)->f1; goto _LL590;} else{ goto _LL591;} _LL591: if(( unsigned int)
_temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_Continue_s: 0){ _LL620: _temp619=((
struct Cyc_Absyn_Continue_s_struct*) _temp585)->f1; goto _LL592;} else{ goto
_LL593;} _LL593: if(( unsigned int) _temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_Goto_s:
0){ _LL624: _temp623=(( struct Cyc_Absyn_Goto_s_struct*) _temp585)->f1; goto
_LL622; _LL622: _temp621=(( struct Cyc_Absyn_Goto_s_struct*) _temp585)->f2; goto
_LL594;} else{ goto _LL595;} _LL595: if(( unsigned int) _temp585 > 1u?*(( int*)
_temp585) == Cyc_Absyn_Label_s: 0){ _LL628: _temp627=(( struct Cyc_Absyn_Label_s_struct*)
_temp585)->f1; goto _LL626; _LL626: _temp625=(( struct Cyc_Absyn_Label_s_struct*)
_temp585)->f2; goto _LL596;} else{ goto _LL597;} _LL597: if(( unsigned int)
_temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_Return_s: 0){ _LL630: _temp629=((
struct Cyc_Absyn_Return_s_struct*) _temp585)->f1; goto _LL598;} else{ goto
_LL599;} _LL599: if(( unsigned int) _temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_Exp_s:
0){ _LL632: _temp631=(( struct Cyc_Absyn_Exp_s_struct*) _temp585)->f1; goto
_LL600;} else{ goto _LL601;} _LL601: if(( unsigned int) _temp585 > 1u?*(( int*)
_temp585) == Cyc_Absyn_Seq_s: 0){ _LL636: _temp635=(( struct Cyc_Absyn_Seq_s_struct*)
_temp585)->f1; goto _LL634; _LL634: _temp633=(( struct Cyc_Absyn_Seq_s_struct*)
_temp585)->f2; goto _LL602;} else{ goto _LL603;} _LL603: if(( unsigned int)
_temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_IfThenElse_s: 0){ _LL642:
_temp641=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp585)->f1; goto _LL640;
_LL640: _temp639=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp585)->f2; goto
_LL638; _LL638: _temp637=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp585)->f3;
goto _LL604;} else{ goto _LL605;} _LL605: if(( unsigned int) _temp585 > 1u?*((
int*) _temp585) == Cyc_Absyn_While_s: 0){ _LL646: _temp645=(( struct Cyc_Absyn_While_s_struct*)
_temp585)->f1; _LL650: _temp649= _temp645.f1; goto _LL648; _LL648: _temp647=
_temp645.f2; goto _LL644; _LL644: _temp643=(( struct Cyc_Absyn_While_s_struct*)
_temp585)->f2; goto _LL606;} else{ goto _LL607;} _LL607: if(( unsigned int)
_temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_For_s: 0){ _LL666: _temp665=((
struct Cyc_Absyn_For_s_struct*) _temp585)->f1; goto _LL660; _LL660: _temp659=((
struct Cyc_Absyn_For_s_struct*) _temp585)->f2; _LL664: _temp663= _temp659.f1;
goto _LL662; _LL662: _temp661= _temp659.f2; goto _LL654; _LL654: _temp653=((
struct Cyc_Absyn_For_s_struct*) _temp585)->f3; _LL658: _temp657= _temp653.f1;
goto _LL656; _LL656: _temp655= _temp653.f2; goto _LL652; _LL652: _temp651=((
struct Cyc_Absyn_For_s_struct*) _temp585)->f4; goto _LL608;} else{ goto _LL609;}
_LL609: if(( unsigned int) _temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_Do_s:
0){ _LL674: _temp673=(( struct Cyc_Absyn_Do_s_struct*) _temp585)->f1; goto
_LL668; _LL668: _temp667=(( struct Cyc_Absyn_Do_s_struct*) _temp585)->f2; _LL672:
_temp671= _temp667.f1; goto _LL670; _LL670: _temp669= _temp667.f2; goto _LL610;}
else{ goto _LL611;} _LL611: if(( unsigned int) _temp585 > 1u?*(( int*) _temp585)
== Cyc_Absyn_Switch_s: 0){ _LL678: _temp677=(( struct Cyc_Absyn_Switch_s_struct*)
_temp585)->f1; goto _LL676; _LL676: _temp675=(( struct Cyc_Absyn_Switch_s_struct*)
_temp585)->f2; goto _LL612;} else{ goto _LL613;} _LL613: if(( unsigned int)
_temp585 > 1u?*(( int*) _temp585) == Cyc_Absyn_Decl_s: 0){ _LL682: _temp681=((
struct Cyc_Absyn_Decl_s_struct*) _temp585)->f1; goto _LL680; _LL680: _temp679=((
struct Cyc_Absyn_Decl_s_struct*) _temp585)->f2; goto _LL614;} else{ goto _LL615;}
_LL615: goto _LL616; _LL588: return; _LL590: return; _LL592: return; _LL594:
return; _LL596: s= _temp625; continue; _LL598: if( _temp629 == 0){ return;}
_temp631=( struct Cyc_Absyn_Exp*) _check_null( _temp629); goto _LL600; _LL600:
Cyc_Tovc_exp_to_vc( env, _temp631); return; _LL602: Cyc_Tovc_stmt_to_vc( env,
_temp635); s= _temp633; continue; _LL604: Cyc_Tovc_exp_to_vc( env, _temp641);
Cyc_Tovc_stmt_to_vc( env, _temp639); s= _temp637; continue; _LL606: Cyc_Tovc_exp_to_vc(
env, _temp649); s= _temp643; continue; _LL608: Cyc_Tovc_exp_to_vc( env, _temp665);
Cyc_Tovc_exp_to_vc( env, _temp663); Cyc_Tovc_exp_to_vc( env, _temp657); s=
_temp651; continue; _LL610: Cyc_Tovc_exp_to_vc( env, _temp671); s= _temp673;
continue; _LL612: Cyc_Tovc_exp_to_vc( env, _temp677); for( 0; _temp675 != 0;
_temp675=(( struct Cyc_List_List*) _check_null( _temp675))->tl){ Cyc_Tovc_stmt_to_vc(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
_temp675))->hd)->body);} return; _LL614:{ void* _temp683=( void*) _temp681->r;
struct Cyc_Absyn_Vardecl* _temp689; _LL685: if(*(( int*) _temp683) == Cyc_Absyn_Var_d){
_LL690: _temp689=(( struct Cyc_Absyn_Var_d_struct*) _temp683)->f1; goto _LL686;}
else{ goto _LL687;} _LL687: goto _LL688; _LL686:{ void* _temp691= Cyc_Tcutil_compress((
void*) _temp689->type); struct Cyc_Absyn_Exp* _temp697; struct Cyc_Absyn_Tqual
_temp699; void* _temp701; _LL693: if(( unsigned int) _temp691 > 4u?*(( int*)
_temp691) == Cyc_Absyn_ArrayType: 0){ _LL702: _temp701=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp691)->f1; goto _LL700; _LL700: _temp699=(( struct Cyc_Absyn_ArrayType_struct*)
_temp691)->f2; goto _LL698; _LL698: _temp697=(( struct Cyc_Absyn_ArrayType_struct*)
_temp691)->f3; goto _LL694;} else{ goto _LL695;} _LL695: goto _LL696; _LL694:
if( _temp689->initializer != 0){ void* _temp703=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp689->initializer))->r; struct Cyc_List_List* _temp709; struct
Cyc_Core_Opt* _temp711; _LL705: if(*(( int*) _temp703) == Cyc_Absyn_UnresolvedMem_e){
_LL712: _temp711=(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp703)->f1;
goto _LL710; _LL710: _temp709=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp703)->f2; goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL706:
_temp689->initializer= 0;{ struct Cyc_Absyn_Exp* arr= Cyc_Absyn_var_exp(
_temp689->name, 0); struct Cyc_Absyn_Stmt* s3= Cyc_Absyn_new_stmt(( void*)
_temp679->r, 0);{ unsigned int i= 0; for( 0; _temp709 != 0;( _temp709=(( struct
Cyc_List_List*) _check_null( _temp709))->tl, ++ i)){ s3= Cyc_Absyn_seq_stmt( Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp( Cyc_Absyn_subscript_exp( arr, Cyc_Absyn_uint_exp( i, 0), 0),(*((
struct _tuple3*)(( struct Cyc_List_List*) _check_null( _temp709))->hd)).f2, 0),
0), s3, 0);}}( void*)( _temp679->r=( void*)(( void*) s3->r)); goto _LL704;}
_LL708:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp713=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp713[ 0]=({ struct Cyc_Core_Impossible_struct _temp714; _temp714.tag= Cyc_Core_Impossible;
_temp714.f1=({ char* _temp715="array type bad initializer"; struct
_tagged_string _temp716; _temp716.curr= _temp715; _temp716.base= _temp715;
_temp716.last_plus_one= _temp715 + 27; _temp716;}); _temp714;}); _temp713;}));
_LL704:;} goto _LL692; _LL696: goto _LL692; _LL692:;} env.all_locals=({ struct
Cyc_List_List* _temp717=( struct Cyc_List_List*) _region_malloc( env.rgn,
sizeof( struct Cyc_List_List)); _temp717->hd=( void*) _temp689; _temp717->tl=
env.all_locals; _temp717;}); if( _temp689->initializer != 0){ Cyc_Tovc_exp_to_vc(
env,( struct Cyc_Absyn_Exp*) _check_null( _temp689->initializer));} s= _temp679;
continue; _LL688: goto _LL684; _LL684:;} goto _LL616; _LL616:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp718=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp718[ 0]=({ struct
Cyc_Core_Impossible_struct _temp719; _temp719.tag= Cyc_Core_Impossible; _temp719.f1=({
char* _temp720="bad stmt after xlation to C"; struct _tagged_string _temp721;
_temp721.curr= _temp720; _temp721.base= _temp720; _temp721.last_plus_one=
_temp720 + 28; _temp721;}); _temp719;}); _temp718;})); _LL586:;}}} struct Cyc_List_List*
Cyc_Tovc_tovc( struct Cyc_List_List* old_ds){ struct Cyc_List_List* _temp722= 0;
goto _LL723; _LL723: for( 0; old_ds != 0; old_ds=(( struct Cyc_List_List*)
_check_null( old_ds))->tl){ struct Cyc_Absyn_Decl* _temp724=( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( old_ds))->hd; goto _LL725; _LL725: { void*
_temp726=( void*) _temp724->r; struct Cyc_Absyn_Fndecl* _temp752; struct Cyc_Absyn_Vardecl*
_temp754; int _temp756; struct Cyc_Absyn_Exp* _temp758; struct Cyc_Core_Opt*
_temp760; struct Cyc_Core_Opt* _temp762; struct Cyc_Absyn_Pat* _temp764; struct
Cyc_List_List* _temp766; struct Cyc_Absyn_Structdecl* _temp768; struct Cyc_Absyn_Uniondecl*
_temp770; struct Cyc_Absyn_Tuniondecl* _temp772; struct Cyc_Absyn_Enumdecl*
_temp774; struct Cyc_Absyn_Typedefdecl* _temp776; struct Cyc_List_List* _temp778;
struct _tagged_string* _temp780; struct Cyc_List_List* _temp782; struct _tuple0*
_temp784; struct Cyc_List_List* _temp786; _LL728: if(*(( int*) _temp726) == Cyc_Absyn_Fn_d){
_LL753: _temp752=(( struct Cyc_Absyn_Fn_d_struct*) _temp726)->f1; goto _LL729;}
else{ goto _LL730;} _LL730: if(*(( int*) _temp726) == Cyc_Absyn_Var_d){ _LL755:
_temp754=(( struct Cyc_Absyn_Var_d_struct*) _temp726)->f1; goto _LL731;} else{
goto _LL732;} _LL732: if(*(( int*) _temp726) == Cyc_Absyn_Let_d){ _LL765:
_temp764=(( struct Cyc_Absyn_Let_d_struct*) _temp726)->f1; goto _LL763; _LL763:
_temp762=(( struct Cyc_Absyn_Let_d_struct*) _temp726)->f2; goto _LL761; _LL761:
_temp760=(( struct Cyc_Absyn_Let_d_struct*) _temp726)->f3; goto _LL759; _LL759:
_temp758=(( struct Cyc_Absyn_Let_d_struct*) _temp726)->f4; goto _LL757; _LL757:
_temp756=(( struct Cyc_Absyn_Let_d_struct*) _temp726)->f5; goto _LL733;} else{
goto _LL734;} _LL734: if(*(( int*) _temp726) == Cyc_Absyn_Letv_d){ _LL767:
_temp766=(( struct Cyc_Absyn_Letv_d_struct*) _temp726)->f1; goto _LL735;} else{
goto _LL736;} _LL736: if(*(( int*) _temp726) == Cyc_Absyn_Struct_d){ _LL769:
_temp768=(( struct Cyc_Absyn_Struct_d_struct*) _temp726)->f1; goto _LL737;}
else{ goto _LL738;} _LL738: if(*(( int*) _temp726) == Cyc_Absyn_Union_d){ _LL771:
_temp770=(( struct Cyc_Absyn_Union_d_struct*) _temp726)->f1; goto _LL739;} else{
goto _LL740;} _LL740: if(*(( int*) _temp726) == Cyc_Absyn_Tunion_d){ _LL773:
_temp772=(( struct Cyc_Absyn_Tunion_d_struct*) _temp726)->f1; goto _LL741;}
else{ goto _LL742;} _LL742: if(*(( int*) _temp726) == Cyc_Absyn_Enum_d){ _LL775:
_temp774=(( struct Cyc_Absyn_Enum_d_struct*) _temp726)->f1; goto _LL743;} else{
goto _LL744;} _LL744: if(*(( int*) _temp726) == Cyc_Absyn_Typedef_d){ _LL777:
_temp776=(( struct Cyc_Absyn_Typedef_d_struct*) _temp726)->f1; goto _LL745;}
else{ goto _LL746;} _LL746: if(*(( int*) _temp726) == Cyc_Absyn_Namespace_d){
_LL781: _temp780=(( struct Cyc_Absyn_Namespace_d_struct*) _temp726)->f1; goto
_LL779; _LL779: _temp778=(( struct Cyc_Absyn_Namespace_d_struct*) _temp726)->f2;
goto _LL747;} else{ goto _LL748;} _LL748: if(*(( int*) _temp726) == Cyc_Absyn_Using_d){
_LL785: _temp784=(( struct Cyc_Absyn_Using_d_struct*) _temp726)->f1; goto _LL783;
_LL783: _temp782=(( struct Cyc_Absyn_Using_d_struct*) _temp726)->f2; goto _LL749;}
else{ goto _LL750;} _LL750: if(*(( int*) _temp726) == Cyc_Absyn_ExternC_d){
_LL787: _temp786=(( struct Cyc_Absyn_ExternC_d_struct*) _temp726)->f1; goto
_LL751;} else{ goto _LL727;} _LL729:{ struct _RegionHandle _temp788= _new_region();
struct _RegionHandle* rgn=& _temp788; _push_region( rgn);{ struct Cyc_List_List**
_temp790=({ struct Cyc_List_List** _temp789=( struct Cyc_List_List**)
_region_malloc( rgn, sizeof( struct Cyc_List_List*)); _temp789[ 0]= 0; _temp789;});
goto _LL791; _LL791: Cyc_Tovc_stmt_to_vc(({ struct Cyc_Tovc_ToExpEnv _temp792;
_temp792.rgn= rgn; _temp792.all_locals=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* src)) Cyc_List_rcopy)( rgn,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp752->param_vardecls))->v); _temp792.varmap=((
struct Cyc_Dict_Dict*(*)( struct _RegionHandle*, int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_rempty)( rgn, Cyc_String_zstrptrcmp);
_temp792.encloser= _temp752->body; _temp792.gen_ds= _temp790; _temp792;}),
_temp752->body); _temp722=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)(* _temp790, _temp722);}; _pop_region(&
_temp788);} goto _LL731; _LL731: goto _LL733; _LL733: goto _LL735; _LL735: goto
_LL737; _LL737: goto _LL739; _LL739: goto _LL741; _LL741: goto _LL743; _LL743:
goto _LL745; _LL745: _temp722=({ struct Cyc_List_List* _temp793=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp793->hd=( void*) _temp724;
_temp793->tl= _temp722; _temp793;}); goto _LL727; _LL747: _temp782= _temp778;
goto _LL749; _LL749: _temp786= _temp782; goto _LL751; _LL751:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp794=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp794[ 0]=({ struct
Cyc_Core_Impossible_struct _temp795; _temp795.tag= Cyc_Core_Impossible; _temp795.f1=({
char* _temp796="nested translation unit after translation to C"; struct
_tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 47; _temp797;}); _temp795;}); _temp794;}));
_LL727:;}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp722);}