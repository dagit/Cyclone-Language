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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_rappend( struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_memq( struct Cyc_List_List*
l, void* x); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void Cyc_Dict_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct _tagged_string
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; static const int Cyc_Tcenv_VarRes= 0;
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
12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); struct Cyc_CfFlowInfo_Place; typedef void* Cyc_CfFlowInfo_local_root_t;
typedef void* Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; static const int Cyc_CfFlowInfo_TupleF=
1; struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_Esc=
0; static const int Cyc_CfFlowInfo_Unesc= 1; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_UnknownIS= 0; struct Cyc_CfFlowInfo_UnknownIS_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfFlowInfo_MustPointTo= 1;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1; } ; static const int Cyc_CfFlowInfo_LeafPI= 0; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI=
2; struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp[ 14u]; static
const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG= 1;
static const int Cyc_CfAbsexp_OneofG= 2; typedef void* Cyc_CfAbsexp_absexp_t;
typedef void* Cyc_CfAbsexp_absexpgroup_t; typedef void* Cyc_CfAbsexp_absop_t;
extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_string Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); char Cyc_CfAbsexp_BadAbsexp[ 14u]; typedef struct Cyc_Dict_Dict*
Cyc_CfAbsexp_dict_t; typedef struct Cyc_Position_Segment* Cyc_CfAbsexp_seg_t;
void* Cyc_CfAbsexp_mkAnyOrderG(){ return( void*) Cyc_CfAbsexp_OrderedG;} static
const int Cyc_CfAbsexp_BottomAE= 0; static const int Cyc_CfAbsexp_SkipAE= 1;
static const int Cyc_CfAbsexp_UseAE= 0; struct Cyc_CfAbsexp_UseAE_struct{ int
tag; void* f1; } ; static const int Cyc_CfAbsexp_AssignAE= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_MallocAE= 2;
struct Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_CfAbsexp_StmtAE= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_CfAbsexp_GroupAE= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_CfAbsexp_UnknownOp=
0; static const int Cyc_CfAbsexp_AddressOp= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_DerefOp= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1; } ; static const int Cyc_CfAbsexp_MemberOp= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_CfAbsexp_LocalOp= 3;
struct Cyc_CfAbsexp_LocalOp_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfAbsexp_MallocOp= 4; struct Cyc_CfAbsexp_MallocOp_struct{
int tag; struct Cyc_Absyn_Exp* f1; int f2; } ; static void Cyc_CfAbsexp_ok_address_arg(
void* ao){ void* _temp0= ao; struct Cyc_Absyn_Vardecl* _temp10; void* _temp12;
void* _temp14; int _temp16; struct Cyc_Absyn_Exp* _temp18; _LL2: if((
unsigned int) _temp0 > 1u?*(( int*) _temp0) == Cyc_CfAbsexp_LocalOp: 0){ _LL11:
_temp10=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 1u?*((
int*) _temp0) == Cyc_CfAbsexp_MemberOp: 0){ _LL15: _temp14=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp0)->f1; goto _LL13; _LL13: _temp12=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp0)->f2; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?*((
int*) _temp0) == Cyc_CfAbsexp_MallocOp: 0){ _LL19: _temp18=( struct Cyc_Absyn_Exp*)((
struct Cyc_CfAbsexp_MallocOp_struct*) _temp0)->f1; goto _LL17; _LL17: _temp16=(
int)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp0)->f2; goto _LL7;} else{ goto
_LL8;} _LL8: goto _LL9; _LL3: return; _LL5: return; _LL7: return; _LL9:( void)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL1:;} static void Cyc_CfAbsexp_ok_member_arg(
void* ao){ void* _temp20= ao; struct Cyc_Absyn_Vardecl* _temp32; void* _temp34;
void* _temp36; int _temp38; struct Cyc_Absyn_Exp* _temp40; void* _temp42; _LL22:
if(( unsigned int) _temp20 > 1u?*(( int*) _temp20) == Cyc_CfAbsexp_LocalOp: 0){
_LL33: _temp32=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp20)->f1; goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp20
> 1u?*(( int*) _temp20) == Cyc_CfAbsexp_MemberOp: 0){ _LL37: _temp36=( void*)((
struct Cyc_CfAbsexp_MemberOp_struct*) _temp20)->f1; goto _LL35; _LL35: _temp34=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp20)->f2; goto _LL25;} else{
goto _LL26;} _LL26: if(( unsigned int) _temp20 > 1u?*(( int*) _temp20) == Cyc_CfAbsexp_MallocOp:
0){ _LL41: _temp40=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp20)->f1; goto _LL39; _LL39: _temp38=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp20)->f2; goto _LL27;} else{ goto _LL28;} _LL28: if(( unsigned int) _temp20
> 1u?*(( int*) _temp20) == Cyc_CfAbsexp_DerefOp: 0){ _LL43: _temp42=( void*)((
struct Cyc_CfAbsexp_DerefOp_struct*) _temp20)->f1; goto _LL29;} else{ goto _LL30;}
_LL30: goto _LL31; _LL23: return; _LL25: return; _LL27: return; _LL29: return;
_LL31:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL21:;} static void Cyc_CfAbsexp_ok_deref_arg(
void* ao){ void* _temp44= ao; struct Cyc_Absyn_Vardecl* _temp54; void* _temp56;
void* _temp58; void* _temp60; _LL46: if(( unsigned int) _temp44 > 1u?*(( int*)
_temp44) == Cyc_CfAbsexp_LocalOp: 0){ _LL55: _temp54=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfAbsexp_LocalOp_struct*) _temp44)->f1; goto _LL47;} else{ goto _LL48;}
_LL48: if(( unsigned int) _temp44 > 1u?*(( int*) _temp44) == Cyc_CfAbsexp_MemberOp:
0){ _LL59: _temp58=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp44)->f1;
goto _LL57; _LL57: _temp56=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp44)->f2; goto _LL49;} else{ goto _LL50;} _LL50: if(( unsigned int) _temp44
> 1u?*(( int*) _temp44) == Cyc_CfAbsexp_DerefOp: 0){ _LL61: _temp60=( void*)((
struct Cyc_CfAbsexp_DerefOp_struct*) _temp44)->f1; goto _LL51;} else{ goto _LL52;}
_LL52: goto _LL53; _LL47: return; _LL49: return; _LL51: return; _LL53:( void)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL45:;} static void Cyc_CfAbsexp_ok_lvalue(
void* ao){ void* _temp62= ao; void* _temp70; _LL64: if( _temp62 ==( void*) Cyc_CfAbsexp_UnknownOp){
goto _LL65;} else{ goto _LL66;} _LL66: if(( unsigned int) _temp62 > 1u?*(( int*)
_temp62) == Cyc_CfAbsexp_AddressOp: 0){ _LL71: _temp70=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp62)->f1; goto _LL67;} else{ goto _LL68;} _LL68: goto _LL69; _LL65: goto
_LL67; _LL67:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL69: return;
_LL63:;} static int Cyc_CfAbsexp_num_mallocpts= 0; static struct Cyc_Dict_Dict**
Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int( struct Cyc_Absyn_Exp*
e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*))) Cyc_Dict_empty)((
int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp); Cyc_CfAbsexp_mallocpt_dict=({
struct Cyc_Dict_Dict** _temp72=( struct Cyc_Dict_Dict**) GC_malloc( sizeof(
struct Cyc_Dict_Dict*) * 1); _temp72[ 0]= d; _temp72;});}{ int i= 0; if( !(( int(*)(
struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key, int* ans_place)) Cyc_Dict_lookup_bool)(({
struct Cyc_Dict_Dict** _temp73= Cyc_CfAbsexp_mallocpt_dict; if( _temp73 == 0){
_throw( Null_Exception);}* _temp73;}), e,& i)){({ struct Cyc_Dict_Dict** _temp74=
Cyc_CfAbsexp_mallocpt_dict; if( _temp74 == 0){ _throw( Null_Exception);}*
_temp74=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp*
key, int data)) Cyc_Dict_insert)(({ struct Cyc_Dict_Dict** _temp75= Cyc_CfAbsexp_mallocpt_dict;
if( _temp75 == 0){ _throw( Null_Exception);}* _temp75;}), e,( i=( ++ Cyc_CfAbsexp_num_mallocpts)));});}
return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){ return( void*) Cyc_CfAbsexp_UnknownOp;}
void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg( ao);
return( void*)({ struct Cyc_CfAbsexp_AddressOp_struct* _temp76=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp76[ 0]=({ struct
Cyc_CfAbsexp_AddressOp_struct _temp77; _temp77.tag= Cyc_CfAbsexp_AddressOp;
_temp77.f1=( void*) ao; _temp77;}); _temp76;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return( void*)({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp78=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp78[ 0]=({ struct Cyc_CfAbsexp_LocalOp_struct _temp79; _temp79.tag= Cyc_CfAbsexp_LocalOp;
_temp79.f1= vd; _temp79;}); _temp78;});} void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f){ Cyc_CfAbsexp_ok_member_arg( ao); return( void*)({ struct Cyc_CfAbsexp_MemberOp_struct*
_temp80=( struct Cyc_CfAbsexp_MemberOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct));
_temp80[ 0]=({ struct Cyc_CfAbsexp_MemberOp_struct _temp81; _temp81.tag= Cyc_CfAbsexp_MemberOp;
_temp81.f1=( void*) ao; _temp81.f2=( void*) f; _temp81;}); _temp80;});} void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_CfAbsexp_MallocOp_struct*
_temp82=( struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp82[ 0]=({ struct Cyc_CfAbsexp_MallocOp_struct _temp83; _temp83.tag= Cyc_CfAbsexp_MallocOp;
_temp83.f1= e; _temp83.f2= Cyc_CfAbsexp_mallocpt_int( e); _temp83;}); _temp82;});}
void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg( ao); return(
void*)({ struct Cyc_CfAbsexp_DerefOp_struct* _temp84=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp84[ 0]=({ struct
Cyc_CfAbsexp_DerefOp_struct _temp85; _temp85.tag= Cyc_CfAbsexp_DerefOp; _temp85.f1=(
void*) ao; _temp85;}); _temp84;});} void* Cyc_CfAbsexp_mkBottomAE(){ return(
void*) Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return( void*) Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return( void*)({ struct
Cyc_CfAbsexp_MallocAE_struct* _temp86=( struct Cyc_CfAbsexp_MallocAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct)); _temp86[ 0]=({ struct
Cyc_CfAbsexp_MallocAE_struct _temp87; _temp87.tag= Cyc_CfAbsexp_MallocAE;
_temp87.f1=( void*)(( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp88=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp88[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp89; _temp89.tag= Cyc_CfFlowInfo_MallocPt;
_temp89.f1= e; _temp89;}); _temp88;})); _temp87.f2= Cyc_CfAbsexp_mallocpt_int( e);
_temp87;}); _temp86;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return( void*)({ struct Cyc_CfAbsexp_StmtAE_struct* _temp90=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp90[ 0]=({ struct
Cyc_CfAbsexp_StmtAE_struct _temp91; _temp91.tag= Cyc_CfAbsexp_StmtAE; _temp91.f1=
s; _temp91;}); _temp90;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp92=
ao; _LL94: if( _temp92 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL95;} else{
goto _LL96;} _LL96: goto _LL97; _LL95: return( void*) Cyc_CfAbsexp_SkipAE; _LL97:
return( void*)({ struct Cyc_CfAbsexp_UseAE_struct* _temp98=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp98[ 0]=({ struct Cyc_CfAbsexp_UseAE_struct
_temp99; _temp99.tag= Cyc_CfAbsexp_UseAE; _temp99.f1=( void*) ao; _temp99;});
_temp98;}); _LL93:;} struct _tuple3{ void* f1; void* f2; } ; void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp101=({ struct _tuple3 _temp100; _temp100.f1=
l; _temp100.f2= r; _temp100;}); void* _temp109; void* _temp111; void* _temp113;
void* _temp115; _LL103: _LL112: _temp111= _temp101.f1; if( _temp111 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL110;} else{ goto _LL105;} _LL110: _temp109=
_temp101.f2; if( _temp109 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL104;}
else{ goto _LL105;} _LL105: _LL116: _temp115= _temp101.f1; if( _temp115 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL114;} else{ goto _LL107;} _LL114: _temp113=
_temp101.f2; goto _LL106; _LL107: goto _LL108; _LL104: return( void*) Cyc_CfAbsexp_SkipAE;
_LL106: return Cyc_CfAbsexp_mkUseAE( r); _LL108: Cyc_CfAbsexp_ok_lvalue( l);
return( void*)({ struct Cyc_CfAbsexp_AssignAE_struct* _temp117=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp117[ 0]=({ struct
Cyc_CfAbsexp_AssignAE_struct _temp118; _temp118.tag= Cyc_CfAbsexp_AssignAE;
_temp118.f1=( void*) l; _temp118.f2=( void*) r; _temp118;}); _temp117;}); _LL102:;}
struct _tuple4{ void* f1; void* f2; void* f3; } ; void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2){ struct _tuple4 _temp120=({ struct _tuple4
_temp119; _temp119.f1= g; _temp119.f2= ae1; _temp119.f3= ae2; _temp119;}); void*
_temp143; void* _temp145; void* _temp147; void* _temp150; void* _temp152; void*
_temp154; void* _temp156; void* _temp158; void* _temp160; void* _temp162; void*
_temp164; void* _temp166; void* _temp168; void* _temp170; void* _temp172; void*
_temp175; struct Cyc_List_List* _temp177; void* _temp179; void* _temp181; struct
Cyc_List_List* _temp183; void* _temp185; void* _temp187; void* _temp190; void*
_temp192; struct Cyc_List_List* _temp194; void* _temp196; void* _temp198; void*
_temp200; void* _temp202; struct Cyc_List_List* _temp204; void* _temp206; void*
_temp208; void* _temp211; struct Cyc_List_List* _temp213; void* _temp215; void*
_temp217; void* _temp219; _LL122: _LL148: _temp147= _temp120.f1; goto _LL146;
_LL146: _temp145= _temp120.f2; goto _LL144; _LL144: _temp143= _temp120.f3; if(
_temp143 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL142;} else{ goto _LL124;}
_LL142: if( g !=( void*) Cyc_CfAbsexp_OneofG){ goto _LL123;} else{ goto _LL124;}
_LL124: _LL155: _temp154= _temp120.f1; goto _LL153; _LL153: _temp152= _temp120.f2;
if( _temp152 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL151;} else{ goto _LL126;}
_LL151: _temp150= _temp120.f3; goto _LL149; _LL149: if( g !=( void*) Cyc_CfAbsexp_OneofG){
goto _LL125;} else{ goto _LL126;} _LL126: _LL161: _temp160= _temp120.f1; if(
_temp160 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL159;} else{ goto _LL128;}
_LL159: _temp158= _temp120.f2; goto _LL157; _LL157: _temp156= _temp120.f3; if(
_temp156 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL127;} else{ goto _LL128;}
_LL128: _LL167: _temp166= _temp120.f1; if( _temp166 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL165;} else{ goto _LL130;} _LL165: _temp164= _temp120.f2; if( _temp164 ==(
void*) Cyc_CfAbsexp_BottomAE){ goto _LL163;} else{ goto _LL130;} _LL163:
_temp162= _temp120.f3; goto _LL129; _LL130: _LL173: _temp172= _temp120.f1; if(
_temp172 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL171;} else{ goto _LL132;}
_LL171: _temp170= _temp120.f2; if( _temp170 ==( void*) Cyc_CfAbsexp_BottomAE){
goto _LL169;} else{ goto _LL132;} _LL169: _temp168= _temp120.f3; goto _LL131;
_LL132: _LL188: _temp187= _temp120.f1; goto _LL182; _LL182: _temp181= _temp120.f2;
if(( unsigned int) _temp181 > 2u?*(( int*) _temp181) == Cyc_CfAbsexp_GroupAE: 0){
_LL186: _temp185=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp181)->f1;
goto _LL184; _LL184: _temp183=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp181)->f2; goto _LL176;} else{ goto _LL134;} _LL176: _temp175= _temp120.f3;
if(( unsigned int) _temp175 > 2u?*(( int*) _temp175) == Cyc_CfAbsexp_GroupAE: 0){
_LL180: _temp179=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp175)->f1;
goto _LL178; _LL178: _temp177=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp175)->f2; goto _LL174;} else{ goto _LL134;} _LL174: if( _temp187 ==
_temp185? _temp187 == _temp179: 0){ goto _LL133;} else{ goto _LL134;} _LL134:
_LL199: _temp198= _temp120.f1; goto _LL193; _LL193: _temp192= _temp120.f2; if((
unsigned int) _temp192 > 2u?*(( int*) _temp192) == Cyc_CfAbsexp_GroupAE: 0){
_LL197: _temp196=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp192)->f1;
goto _LL195; _LL195: _temp194=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp192)->f2; goto _LL191;} else{ goto _LL136;} _LL191: _temp190= _temp120.f3;
goto _LL189; _LL189: if( _temp198 == _temp196? _temp198 !=( void*) Cyc_CfAbsexp_OrderedG:
0){ goto _LL135;} else{ goto _LL136;} _LL136: _LL209: _temp208= _temp120.f1; if(
_temp208 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL203;} else{ goto _LL138;}
_LL203: _temp202= _temp120.f2; if(( unsigned int) _temp202 > 2u?*(( int*)
_temp202) == Cyc_CfAbsexp_GroupAE: 0){ _LL207: _temp206=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp202)->f1; if( _temp206 ==( void*) Cyc_CfAbsexp_OrderedG){ goto _LL205;}
else{ goto _LL138;} _LL205: _temp204=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp202)->f2; goto _LL201;} else{ goto _LL138;} _LL201: _temp200= _temp120.f3;
goto _LL137; _LL138: _LL220: _temp219= _temp120.f1; goto _LL218; _LL218:
_temp217= _temp120.f2; goto _LL212; _LL212: _temp211= _temp120.f3; if((
unsigned int) _temp211 > 2u?*(( int*) _temp211) == Cyc_CfAbsexp_GroupAE: 0){
_LL216: _temp215=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp211)->f1;
goto _LL214; _LL214: _temp213=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp211)->f2; goto _LL210;} else{ goto _LL140;} _LL210: if( _temp219 ==
_temp215){ goto _LL139;} else{ goto _LL140;} _LL140: goto _LL141; _LL123: return
ae1; _LL125: return ae2; _LL127: return ae1; _LL129: return ae2; _LL131: return(
void*) Cyc_CfAbsexp_BottomAE; _LL133: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp221=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp221[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp222; _temp222.tag= Cyc_CfAbsexp_GroupAE;
_temp222.f1=( void*) _temp187; _temp222.f2=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp183, _temp177);
_temp222;}); _temp221;}); _LL135: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp223=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp223[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp224; _temp224.tag= Cyc_CfAbsexp_GroupAE;
_temp224.f1=( void*) _temp198; _temp224.f2=({ struct Cyc_List_List* _temp225=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp225[
0]=({ struct Cyc_List_List _temp226; _temp226.hd=( void*) ae2; _temp226.tl=
_temp194; _temp226;}); _temp225;}); _temp224;}); _temp223;}); _LL137: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp227=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp227[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp228; _temp228.tag= Cyc_CfAbsexp_GroupAE;
_temp228.f1=( void*)(( void*) Cyc_CfAbsexp_OrderedG); _temp228.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp204,({
struct Cyc_List_List* _temp229=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp229[ 0]=({ struct Cyc_List_List _temp230;
_temp230.hd=( void*) ae2; _temp230.tl= 0; _temp230;}); _temp229;})); _temp228;});
_temp227;}); _LL139: return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp231=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp231[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp232; _temp232.tag= Cyc_CfAbsexp_GroupAE;
_temp232.f1=( void*) _temp219; _temp232.f2=({ struct Cyc_List_List* _temp233=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp233[
0]=({ struct Cyc_List_List _temp234; _temp234.hd=( void*) ae1; _temp234.tl=
_temp213; _temp234;}); _temp233;}); _temp232;}); _temp231;}); _LL141: return(
void*)({ struct Cyc_CfAbsexp_GroupAE_struct* _temp235=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp235[ 0]=({ struct
Cyc_CfAbsexp_GroupAE_struct _temp236; _temp236.tag= Cyc_CfAbsexp_GroupAE;
_temp236.f1=( void*) g; _temp236.f2=({ struct Cyc_List_List* _temp237=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp237[ 0]=({
struct Cyc_List_List _temp238; _temp238.hd=( void*) ae1; _temp238.tl=({ struct
Cyc_List_List* _temp239=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp239[ 0]=({ struct Cyc_List_List _temp240; _temp240.hd=( void*) ae2;
_temp240.tl= 0; _temp240;}); _temp239;}); _temp238;}); _temp237;}); _temp236;});
_temp235;}); _LL121:;} void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List*
ael){ if( ael == 0){ return( void*) Cyc_CfAbsexp_SkipAE;} if(({ struct Cyc_List_List*
_temp241= ael; if( _temp241 == 0){ _throw( Null_Exception);} _temp241->tl;}) ==
0){ return( void*)({ struct Cyc_List_List* _temp242= ael; if( _temp242 == 0){
_throw( Null_Exception);} _temp242->hd;});} return( void*)({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp243=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp243[ 0]=({ struct Cyc_CfAbsexp_GroupAE_struct _temp244; _temp244.tag= Cyc_CfAbsexp_GroupAE;
_temp244.f1=( void*) g; _temp244.f2= ael; _temp244;}); _temp243;});} int Cyc_CfAbsexp_isUnknownOp(
void* ao){ return ao ==( void*) Cyc_CfAbsexp_UnknownOp;} struct _tagged_string
Cyc_CfAbsexp_absop2string( void* ao){ void* _temp245= ao; void* _temp259; struct
Cyc_Absyn_Vardecl* _temp261; void* _temp263; void* _temp265; int _temp267;
struct Cyc_Absyn_Exp* _temp269; void* _temp271; _LL247: if( _temp245 ==( void*)
Cyc_CfAbsexp_UnknownOp){ goto _LL248;} else{ goto _LL249;} _LL249: if((
unsigned int) _temp245 > 1u?*(( int*) _temp245) == Cyc_CfAbsexp_AddressOp: 0){
_LL260: _temp259=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp245)->f1;
goto _LL250;} else{ goto _LL251;} _LL251: if(( unsigned int) _temp245 > 1u?*((
int*) _temp245) == Cyc_CfAbsexp_LocalOp: 0){ _LL262: _temp261=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfAbsexp_LocalOp_struct*) _temp245)->f1; goto _LL252;} else{ goto
_LL253;} _LL253: if(( unsigned int) _temp245 > 1u?*(( int*) _temp245) == Cyc_CfAbsexp_MemberOp:
0){ _LL266: _temp265=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp245)->f1;
goto _LL264; _LL264: _temp263=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp245)->f2; goto _LL254;} else{ goto _LL255;} _LL255: if(( unsigned int)
_temp245 > 1u?*(( int*) _temp245) == Cyc_CfAbsexp_MallocOp: 0){ _LL270: _temp269=(
struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp245)->f1;
goto _LL268; _LL268: _temp267=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp245)->f2; goto _LL256;} else{ goto _LL257;} _LL257: if(( unsigned int)
_temp245 > 1u?*(( int*) _temp245) == Cyc_CfAbsexp_DerefOp: 0){ _LL272: _temp271=(
void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp245)->f1; goto _LL258;} else{
goto _LL246;} _LL248: return( struct _tagged_string)({ char* _temp273=( char*)"Unknown";
struct _tagged_string _temp274; _temp274.curr= _temp273; _temp274.base= _temp273;
_temp274.last_plus_one= _temp273 + 8; _temp274;}); _LL250: return({ struct
_tagged_string _temp275= Cyc_CfAbsexp_absop2string( _temp259); xprintf("& %.*s",
_temp275.last_plus_one - _temp275.curr, _temp275.curr);}); _LL252: return*(*
_temp261->name).f2; _LL254: { struct _tagged_string fs;{ void* _temp276=
_temp263; struct _tagged_string* _temp282; int _temp284; _LL278: if(*(( int*)
_temp276) == Cyc_CfFlowInfo_StructF){ _LL283: _temp282=( struct _tagged_string*)((
struct Cyc_CfFlowInfo_StructF_struct*) _temp276)->f1; goto _LL279;} else{ goto
_LL280;} _LL280: if(*(( int*) _temp276) == Cyc_CfFlowInfo_TupleF){ _LL285:
_temp284=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp276)->f1; goto
_LL281;} else{ goto _LL277;} _LL279: fs=* _temp282; goto _LL277; _LL281: fs=
xprintf("%d", _temp284); goto _LL277; _LL277:;} return({ struct _tagged_string
_temp286= Cyc_CfAbsexp_absop2string( _temp265); struct _tagged_string _temp287=
fs; xprintf("%.*s.%.*s", _temp286.last_plus_one - _temp286.curr, _temp286.curr,
_temp287.last_plus_one - _temp287.curr, _temp287.curr);});} _LL256: return
xprintf("<mpt%d>", _temp267); _LL258: return({ struct _tagged_string _temp288=
Cyc_CfAbsexp_absop2string( _temp271); xprintf("(*%.*s)", _temp288.last_plus_one
- _temp288.curr, _temp288.curr);}); _LL246:;} struct _tagged_string Cyc_CfAbsexp_absexp2string(
void* ae, int depth){ struct _tagged_string ans= Cyc_Core_new_string( depth);{
int i= 0; for( 0; i <({ struct _tagged_string _temp289= ans;( unsigned int)(
_temp289.last_plus_one - _temp289.curr);}); ++ i){({ struct _tagged_string
_temp290= ans; char* _temp292= _temp290.curr + i; if( _temp290.base == 0? 1:(
_temp292 < _temp290.base? 1: _temp292 >= _temp290.last_plus_one)){ _throw(
Null_Exception);}* _temp292=' ';});}}{ void* _temp293= ae; void* _temp311; int
_temp313; void* _temp315; struct Cyc_Absyn_Exp* _temp317; void* _temp319; void*
_temp321; struct Cyc_List_List* _temp323; void* _temp325; struct Cyc_Absyn_Stmt*
_temp327; _LL295: if( _temp293 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL296;}
else{ goto _LL297;} _LL297: if(( unsigned int) _temp293 > 2u?*(( int*) _temp293)
== Cyc_CfAbsexp_UseAE: 0){ _LL312: _temp311=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*)
_temp293)->f1; goto _LL298;} else{ goto _LL299;} _LL299: if(( unsigned int)
_temp293 > 2u?*(( int*) _temp293) == Cyc_CfAbsexp_MallocAE: 0){ _LL316: _temp315=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp293)->f1; if(*(( int*)
_temp315) == Cyc_CfFlowInfo_MallocPt){ _LL318: _temp317=( struct Cyc_Absyn_Exp*)((
struct Cyc_CfFlowInfo_MallocPt_struct*) _temp315)->f1; goto _LL314;} else{ goto
_LL301;} _LL314: _temp313=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp293)->f2; goto _LL300;} else{ goto _LL301;} _LL301: if(( unsigned int)
_temp293 > 2u?*(( int*) _temp293) == Cyc_CfAbsexp_AssignAE: 0){ _LL322: _temp321=(
void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp293)->f1; goto _LL320;
_LL320: _temp319=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp293)->f2;
goto _LL302;} else{ goto _LL303;} _LL303: if(( unsigned int) _temp293 > 2u?*((
int*) _temp293) == Cyc_CfAbsexp_GroupAE: 0){ _LL326: _temp325=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp293)->f1; goto _LL324; _LL324: _temp323=(
struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp293)->f2;
goto _LL304;} else{ goto _LL305;} _LL305: if(( unsigned int) _temp293 > 2u?*((
int*) _temp293) == Cyc_CfAbsexp_StmtAE: 0){ _LL328: _temp327=( struct Cyc_Absyn_Stmt*)((
struct Cyc_CfAbsexp_StmtAE_struct*) _temp293)->f1; goto _LL306;} else{ goto
_LL307;} _LL307: if( _temp293 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL308;}
else{ goto _LL309;} _LL309: goto _LL310; _LL296: ans=({ struct _tagged_string
_temp329= ans; xprintf("%.*sskip", _temp329.last_plus_one - _temp329.curr,
_temp329.curr);}); goto _LL294; _LL298: ans=({ struct _tagged_string _temp330=
ans; struct _tagged_string _temp331= Cyc_CfAbsexp_absop2string( _temp311);
xprintf("%.*suse %.*s", _temp330.last_plus_one - _temp330.curr, _temp330.curr,
_temp331.last_plus_one - _temp331.curr, _temp331.curr);}); goto _LL294; _LL300:
ans=({ struct _tagged_string _temp332= ans; int _temp333= _temp313; xprintf("%.*smalloc %i",
_temp332.last_plus_one - _temp332.curr, _temp332.curr, _temp333);}); goto _LL294;
_LL302: ans=({ struct _tagged_string _temp334= ans; struct _tagged_string
_temp335= Cyc_CfAbsexp_absop2string( _temp321); struct _tagged_string _temp336=
Cyc_CfAbsexp_absop2string( _temp319); xprintf("%.*sassign %.*s %.*s", _temp334.last_plus_one
- _temp334.curr, _temp334.curr, _temp335.last_plus_one - _temp335.curr, _temp335.curr,
_temp336.last_plus_one - _temp336.curr, _temp336.curr);}); goto _LL294; _LL304:{
void* _temp337= _temp325; _LL339: if( _temp337 ==( void*) Cyc_CfAbsexp_OrderedG){
goto _LL340;} else{ goto _LL341;} _LL341: if( _temp337 ==( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL342;} else{ goto _LL343;} _LL343: if( _temp337 ==( void*) Cyc_CfAbsexp_OneofG){
goto _LL344;} else{ goto _LL338;} _LL340: ans=({ struct _tagged_string _temp345=
ans; xprintf("%.*sordered", _temp345.last_plus_one - _temp345.curr, _temp345.curr);});
goto _LL338; _LL342: ans=({ struct _tagged_string _temp346= ans; xprintf("%.*sunordered",
_temp346.last_plus_one - _temp346.curr, _temp346.curr);}); goto _LL338; _LL344:
ans=({ struct _tagged_string _temp347= ans; xprintf("%.*soneof", _temp347.last_plus_one
- _temp347.curr, _temp347.curr);}); goto _LL338; _LL338:;} for( 0; _temp323 != 0;
_temp323=({ struct Cyc_List_List* _temp348= _temp323; if( _temp348 == 0){ _throw(
Null_Exception);} _temp348->tl;})){ ans=({ struct _tagged_string _temp350= ans;
struct _tagged_string _temp351= Cyc_CfAbsexp_absexp2string(( void*)({ struct Cyc_List_List*
_temp349= _temp323; if( _temp349 == 0){ _throw( Null_Exception);} _temp349->hd;}),
depth + 1); xprintf("%.*s\n%.*s", _temp350.last_plus_one - _temp350.curr,
_temp350.curr, _temp351.last_plus_one - _temp351.curr, _temp351.curr);});} goto
_LL294; _LL306: ans=({ struct _tagged_string _temp352= ans; xprintf("%.*s<<stmt>>",
_temp352.last_plus_one - _temp352.curr, _temp352.curr);}); goto _LL294; _LL308:
ans=({ struct _tagged_string _temp353= ans; xprintf("%.*sbottom", _temp353.last_plus_one
- _temp353.curr, _temp353.curr);}); goto _LL294; _LL310:( void) _throw(( void*)
Cyc_CfAbsexp_BadAbsexp); _LL294:;} return ans;} static void Cyc_CfAbsexp_check_absop(
void* ao){ void* _temp354= ao; void* _temp368; void* _temp370; void* _temp372;
void* _temp374; struct Cyc_Absyn_Vardecl* _temp376; int _temp378; struct Cyc_Absyn_Exp*
_temp380; _LL356: if( _temp354 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL357;}
else{ goto _LL358;} _LL358: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354)
== Cyc_CfAbsexp_AddressOp: 0){ _LL369: _temp368=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp354)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if(( unsigned int)
_temp354 > 1u?*(( int*) _temp354) == Cyc_CfAbsexp_DerefOp: 0){ _LL371: _temp370=(
void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp354)->f1; goto _LL361;} else{
goto _LL362;} _LL362: if(( unsigned int) _temp354 > 1u?*(( int*) _temp354) ==
Cyc_CfAbsexp_MemberOp: 0){ _LL375: _temp374=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp354)->f1; goto _LL373; _LL373: _temp372=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp354)->f2; goto _LL363;} else{ goto _LL364;} _LL364: if(( unsigned int)
_temp354 > 1u?*(( int*) _temp354) == Cyc_CfAbsexp_LocalOp: 0){ _LL377: _temp376=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp354)->f1;
goto _LL365;} else{ goto _LL366;} _LL366: if(( unsigned int) _temp354 > 1u?*((
int*) _temp354) == Cyc_CfAbsexp_MallocOp: 0){ _LL381: _temp380=( struct Cyc_Absyn_Exp*)((
struct Cyc_CfAbsexp_MallocOp_struct*) _temp354)->f1; goto _LL379; _LL379:
_temp378=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp354)->f2; goto
_LL367;} else{ goto _LL355;} _LL357: return; _LL359: Cyc_CfAbsexp_check_absop(
_temp368); Cyc_CfAbsexp_ok_address_arg( _temp368); return; _LL361: Cyc_CfAbsexp_check_absop(
_temp370); Cyc_CfAbsexp_ok_deref_arg( _temp370); return; _LL363: Cyc_CfAbsexp_check_absop(
_temp374); Cyc_CfAbsexp_ok_member_arg( _temp374); return; _LL365: return; _LL367:
return; _LL355:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp382= ae;
int _temp400; void* _temp402; struct Cyc_Absyn_Vardecl* _temp404; int _temp406;
void* _temp408; struct Cyc_Absyn_Stmt* _temp410; struct Cyc_List_List* _temp412;
void* _temp414; void* _temp416; void* _temp418; void* _temp420; _LL384: if(
_temp382 ==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL385;} else{ goto _LL386;}
_LL386: if( _temp382 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL387;} else{ goto
_LL388;} _LL388: if(( unsigned int) _temp382 > 2u?*(( int*) _temp382) == Cyc_CfAbsexp_MallocAE:
0){ _LL403: _temp402=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp382)->f1;
if(*(( int*) _temp402) == Cyc_CfFlowInfo_VarRoot){ _LL405: _temp404=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfFlowInfo_VarRoot_struct*) _temp402)->f1; goto _LL401;} else{ goto
_LL390;} _LL401: _temp400=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp382)->f2; goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int)
_temp382 > 2u?*(( int*) _temp382) == Cyc_CfAbsexp_MallocAE: 0){ _LL409: _temp408=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp382)->f1; goto _LL407;
_LL407: _temp406=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp382)->f2;
goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int) _temp382 > 2u?*((
int*) _temp382) == Cyc_CfAbsexp_StmtAE: 0){ _LL411: _temp410=( struct Cyc_Absyn_Stmt*)((
struct Cyc_CfAbsexp_StmtAE_struct*) _temp382)->f1; goto _LL393;} else{ goto
_LL394;} _LL394: if(( unsigned int) _temp382 > 2u?*(( int*) _temp382) == Cyc_CfAbsexp_GroupAE:
0){ _LL415: _temp414=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp382)->f1;
goto _LL413; _LL413: _temp412=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp382)->f2; goto _LL395;} else{ goto _LL396;} _LL396: if(( unsigned int)
_temp382 > 2u?*(( int*) _temp382) == Cyc_CfAbsexp_UseAE: 0){ _LL417: _temp416=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp382)->f1; goto _LL397;} else{
goto _LL398;} _LL398: if(( unsigned int) _temp382 > 2u?*(( int*) _temp382) ==
Cyc_CfAbsexp_AssignAE: 0){ _LL421: _temp420=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp382)->f1; goto _LL419; _LL419: _temp418=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp382)->f2; goto _LL399;} else{ goto _LL383;} _LL385: return; _LL387: return;
_LL389:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL391: return; _LL393:
return; _LL395: for( 0; _temp412 != 0; _temp412=({ struct Cyc_List_List*
_temp422= _temp412; if( _temp422 == 0){ _throw( Null_Exception);} _temp422->tl;})){
Cyc_CfAbsexp_check_absexp(( void*)({ struct Cyc_List_List* _temp423= _temp412;
if( _temp423 == 0){ _throw( Null_Exception);} _temp423->hd;}));} return; _LL397:
Cyc_CfAbsexp_check_absop( _temp416); return; _LL399: Cyc_CfAbsexp_check_absop(
_temp420); Cyc_CfAbsexp_check_absop( _temp418); Cyc_CfAbsexp_ok_lvalue( _temp420);
return; _LL383:;} void* Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict*
pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l( struct Cyc_Dict_Dict*
pinfo_dict, void* ao){ void* _temp424= ao; struct Cyc_Absyn_Vardecl* _temp438;
int _temp440; struct Cyc_Absyn_Exp* _temp442; void* _temp444; void* _temp446;
void* _temp448; void* _temp450; _LL426: if(( unsigned int) _temp424 > 1u?*(( int*)
_temp424) == Cyc_CfAbsexp_LocalOp: 0){ _LL439: _temp438=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfAbsexp_LocalOp_struct*) _temp424)->f1; goto _LL427;} else{ goto
_LL428;} _LL428: if(( unsigned int) _temp424 > 1u?*(( int*) _temp424) == Cyc_CfAbsexp_MallocOp:
0){ _LL443: _temp442=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp424)->f1; goto _LL441; _LL441: _temp440=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp424)->f2; goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int)
_temp424 > 1u?*(( int*) _temp424) == Cyc_CfAbsexp_MemberOp: 0){ _LL447: _temp446=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp424)->f1; goto _LL445;
_LL445: _temp444=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp424)->f2;
goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp424 > 1u?*((
int*) _temp424) == Cyc_CfAbsexp_DerefOp: 0){ _LL449: _temp448=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp424)->f1; goto _LL433;} else{ goto _LL434;}
_LL434: if( _temp424 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL435;} else{
goto _LL436;} _LL436: if(( unsigned int) _temp424 > 1u?*(( int*) _temp424) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL451: _temp450=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp424)->f1; goto _LL437;} else{ goto _LL425;} _LL427: return( void*)({ struct
Cyc_CfFlowInfo_MustPointTo_struct* _temp452=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp452[ 0]=({
struct Cyc_CfFlowInfo_MustPointTo_struct _temp453; _temp453.tag= Cyc_CfFlowInfo_MustPointTo;
_temp453.f1=({ struct Cyc_CfFlowInfo_Place* _temp454=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp454[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp455; _temp455.root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp456=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp456[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp457; _temp457.tag= Cyc_CfFlowInfo_VarRoot;
_temp457.f1= _temp438; _temp457;}); _temp456;})); _temp455.fields= 0; _temp455;});
_temp454;}); _temp453;}); _temp452;}); _LL429: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp458=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp458[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp459; _temp459.tag= Cyc_CfFlowInfo_MustPointTo; _temp459.f1=({ struct Cyc_CfFlowInfo_Place*
_temp460=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp460[ 0]=({ struct Cyc_CfFlowInfo_Place _temp461; _temp461.root=( void*)((
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp462=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp462[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp463; _temp463.tag= Cyc_CfFlowInfo_MallocPt;
_temp463.f1= _temp442; _temp463;}); _temp462;})); _temp461.fields= 0; _temp461;});
_temp460;}); _temp459;}); _temp458;}); _LL431: { void* _temp464= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp446); goto _LL465; _LL465: { void* _temp466= _temp464; void*
_temp472; void* _temp474; struct Cyc_CfFlowInfo_Place* _temp476; struct Cyc_CfFlowInfo_Place
_temp478; struct Cyc_List_List* _temp479; void* _temp481; _LL468: if(*(( int*)
_temp466) == Cyc_CfFlowInfo_UnknownIS){ _LL475: _temp474=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp466)->f1; goto _LL473; _LL473: _temp472=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp466)->f2; goto _LL469;} else{ goto _LL470;} _LL470: if(*(( int*) _temp466)
== Cyc_CfFlowInfo_MustPointTo){ _LL477: _temp476=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp466)->f1; _temp478=* _temp476;
_LL482: _temp481=( void*) _temp478.root; goto _LL480; _LL480: _temp479=( struct
Cyc_List_List*) _temp478.fields; goto _LL471;} else{ goto _LL467;} _LL469:
return _temp464; _LL471: return( void*)({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp483=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp483[ 0]=({ struct Cyc_CfFlowInfo_MustPointTo_struct
_temp484; _temp484.tag= Cyc_CfFlowInfo_MustPointTo; _temp484.f1=({ struct Cyc_CfFlowInfo_Place*
_temp485=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp485[ 0]=({ struct Cyc_CfFlowInfo_Place _temp486; _temp486.root=( void*)
_temp481; _temp486.fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp479,({ struct Cyc_List_List*
_temp487=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp487[ 0]=({ struct Cyc_List_List _temp488; _temp488.hd=( void*) _temp444;
_temp488.tl= 0; _temp488;}); _temp487;})); _temp486;}); _temp485;}); _temp484;});
_temp483;}); _LL467:;}} _LL433: { void* _temp489= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp448); goto _LL490; _LL490: { void* _temp491= _temp489; void*
_temp499; struct Cyc_Dict_Dict* _temp501; struct Cyc_Dict_Dict* _temp503; _LL493:
if(*(( int*) _temp491) == Cyc_CfFlowInfo_LeafPI){ _LL500: _temp499=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp491)->f1; goto _LL494;} else{ goto
_LL495;} _LL495: if(*(( int*) _temp491) == Cyc_CfFlowInfo_TuplePI){ _LL502:
_temp501=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp491)->f1; goto _LL496;} else{ goto _LL497;} _LL497: if(*(( int*) _temp491)
== Cyc_CfFlowInfo_StructPI){ _LL504: _temp503=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_StructPI_struct*) _temp491)->f1; goto _LL498;} else{ goto _LL492;}
_LL494: return _temp499; _LL496: goto _LL498; _LL498:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL492:;}} _LL435: goto _LL437; _LL437:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL425:;} void* Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict,
void* ao){ void* _temp505= ao; struct Cyc_Absyn_Vardecl* _temp519; int _temp521;
struct Cyc_Absyn_Exp* _temp523; void* _temp525; void* _temp527; void* _temp529;
void* _temp531; _LL507: if(( unsigned int) _temp505 > 1u?*(( int*) _temp505) ==
Cyc_CfAbsexp_LocalOp: 0){ _LL520: _temp519=( struct Cyc_Absyn_Vardecl*)(( struct
Cyc_CfAbsexp_LocalOp_struct*) _temp505)->f1; goto _LL508;} else{ goto _LL509;}
_LL509: if(( unsigned int) _temp505 > 1u?*(( int*) _temp505) == Cyc_CfAbsexp_MallocOp:
0){ _LL524: _temp523=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp505)->f1; goto _LL522; _LL522: _temp521=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp505)->f2; goto _LL510;} else{ goto _LL511;} _LL511: if(( unsigned int)
_temp505 > 1u?*(( int*) _temp505) == Cyc_CfAbsexp_MemberOp: 0){ _LL528: _temp527=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp505)->f1; goto _LL526;
_LL526: _temp525=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp505)->f2;
goto _LL512;} else{ goto _LL513;} _LL513: if(( unsigned int) _temp505 > 1u?*((
int*) _temp505) == Cyc_CfAbsexp_DerefOp: 0){ _LL530: _temp529=( void*)(( struct
Cyc_CfAbsexp_DerefOp_struct*) _temp505)->f1; goto _LL514;} else{ goto _LL515;}
_LL515: if( _temp505 ==( void*) Cyc_CfAbsexp_UnknownOp){ goto _LL516;} else{
goto _LL517;} _LL517: if(( unsigned int) _temp505 > 1u?*(( int*) _temp505) ==
Cyc_CfAbsexp_AddressOp: 0){ _LL532: _temp531=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp505)->f1; goto _LL518;} else{ goto _LL506;} _LL508: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp533=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp533[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp534; _temp534.tag= Cyc_CfFlowInfo_VarRoot;
_temp534.f1= _temp519; _temp534;}); _temp533;})); _LL510: return(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( pinfo_dict,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp535=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp535[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp536; _temp536.tag= Cyc_CfFlowInfo_MallocPt;
_temp536.f1= _temp523; _temp536;}); _temp535;})); _LL512: { void* _temp537= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp527); goto _LL538; _LL538: { struct _tuple3 _temp540=({ struct
_tuple3 _temp539; _temp539.f1= _temp537; _temp539.f2= _temp525; _temp539;});
void* _temp550; void* _temp552; void* _temp554; void* _temp556; void* _temp558;
void* _temp560; int _temp562; void* _temp564; struct Cyc_Dict_Dict* _temp566;
void* _temp568; struct _tagged_string* _temp570; void* _temp572; struct Cyc_Dict_Dict*
_temp574; _LL542: _LL553: _temp552= _temp540.f1; if(*(( int*) _temp552) == Cyc_CfFlowInfo_LeafPI){
_LL555: _temp554=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp552)->f1;
if(*(( int*) _temp554) == Cyc_CfFlowInfo_UnknownIS){ _LL559: _temp558=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp554)->f1; goto _LL557; _LL557:
_temp556=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp554)->f2; goto
_LL551;} else{ goto _LL544;}} else{ goto _LL544;} _LL551: _temp550= _temp540.f2;
goto _LL543; _LL544: _LL565: _temp564= _temp540.f1; if(*(( int*) _temp564) ==
Cyc_CfFlowInfo_TuplePI){ _LL567: _temp566=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp564)->f1; goto _LL561;} else{ goto _LL546;} _LL561: _temp560= _temp540.f2;
if(*(( int*) _temp560) == Cyc_CfFlowInfo_TupleF){ _LL563: _temp562=( int)((
struct Cyc_CfFlowInfo_TupleF_struct*) _temp560)->f1; goto _LL545;} else{ goto
_LL546;} _LL546: _LL573: _temp572= _temp540.f1; if(*(( int*) _temp572) == Cyc_CfFlowInfo_StructPI){
_LL575: _temp574=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp572)->f1; goto _LL569;} else{ goto _LL548;} _LL569: _temp568= _temp540.f2;
if(*(( int*) _temp568) == Cyc_CfFlowInfo_StructF){ _LL571: _temp570=( struct
_tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp568)->f1; goto
_LL547;} else{ goto _LL548;} _LL548: goto _LL549; _LL543: return _temp537;
_LL545: return(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp566, _temp562); _LL547: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp574, _temp570); _LL549:( void)
_throw(( void*) Cyc_CfAbsexp_BadAbsexp); _LL541:;}} _LL514: { void* _temp576=
Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp529); goto _LL577; _LL577: { void*
_temp578= _temp576; void* _temp588; void* _temp590; void* _temp592; void*
_temp594; void* _temp596; void* _temp598; void* _temp600; struct Cyc_CfFlowInfo_Place*
_temp602; _LL580: if(*(( int*) _temp578) == Cyc_CfFlowInfo_LeafPI){ _LL589:
_temp588=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp578)->f1; if(*((
int*) _temp588) == Cyc_CfFlowInfo_UnknownIS){ _LL593: _temp592=( void*)(( struct
Cyc_CfFlowInfo_UnknownIS_struct*) _temp588)->f1; goto _LL591; _LL591: _temp590=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp588)->f2; if( _temp590 ==(
void*) Cyc_CfFlowInfo_ThisIL){ goto _LL581;} else{ goto _LL582;}} else{ goto
_LL582;}} else{ goto _LL582;} _LL582: if(*(( int*) _temp578) == Cyc_CfFlowInfo_LeafPI){
_LL595: _temp594=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp578)->f1;
if(*(( int*) _temp594) == Cyc_CfFlowInfo_UnknownIS){ _LL599: _temp598=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp594)->f1; goto _LL597; _LL597:
_temp596=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp594)->f2; goto
_LL583;} else{ goto _LL584;}} else{ goto _LL584;} _LL584: if(*(( int*) _temp578)
== Cyc_CfFlowInfo_LeafPI){ _LL601: _temp600=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp578)->f1; if(*(( int*) _temp600) == Cyc_CfFlowInfo_MustPointTo){ _LL603:
_temp602=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp600)->f1; goto _LL585;} else{ goto _LL586;}} else{ goto _LL586;} _LL586:
goto _LL587; _LL581: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_NoneIL); _LL583: return _temp576; _LL585: return Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp602); _LL587:( void) _throw(( void*) Cyc_CfAbsexp_BadAbsexp);
_LL579:;}} _LL516: return Cyc_CfFlowInfo_mkLeafPI(( void*) Cyc_CfFlowInfo_Esc,(
void*) Cyc_CfFlowInfo_AllIL); _LL518: return( void*)({ struct Cyc_CfFlowInfo_LeafPI_struct*
_temp604=( struct Cyc_CfFlowInfo_LeafPI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct));
_temp604[ 0]=({ struct Cyc_CfFlowInfo_LeafPI_struct _temp605; _temp605.tag= Cyc_CfFlowInfo_LeafPI;
_temp605.f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp531); _temp605;});
_temp604;}); _LL506:;} void Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp607=({
struct _tuple3 _temp606; _temp606.f1= old_pinfo; _temp606.f2= new_pinfo;
_temp606;}); void* _temp619; void* _temp621; void* _temp623; void* _temp625;
void* _temp627; void* _temp629; void* _temp631; void* _temp633; void* _temp635;
void* _temp637; void* _temp639; struct Cyc_Dict_Dict* _temp641; void* _temp643;
struct Cyc_Dict_Dict* _temp645; void* _temp647; struct Cyc_Dict_Dict* _temp649;
void* _temp651; struct Cyc_Dict_Dict* _temp653; _LL609: _LL624: _temp623=
_temp607.f1; if(*(( int*) _temp623) == Cyc_CfFlowInfo_LeafPI){ _LL626: _temp625=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp623)->f1; if(*(( int*)
_temp625) == Cyc_CfFlowInfo_UnknownIS){ _LL630: _temp629=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp625)->f1; if( _temp629 ==( void*) Cyc_CfFlowInfo_Esc){ goto _LL628;} else{
goto _LL611;} _LL628: _temp627=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp625)->f2; goto _LL620;} else{ goto _LL611;}} else{ goto _LL611;} _LL620:
_temp619= _temp607.f2; if(*(( int*) _temp619) == Cyc_CfFlowInfo_LeafPI){ _LL622:
_temp621=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp619)->f1; goto
_LL610;} else{ goto _LL611;} _LL611: _LL636: _temp635= _temp607.f1; if(*(( int*)
_temp635) == Cyc_CfFlowInfo_LeafPI){ _LL638: _temp637=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp635)->f1; goto _LL632;} else{ goto _LL613;} _LL632: _temp631= _temp607.f2;
if(*(( int*) _temp631) == Cyc_CfFlowInfo_LeafPI){ _LL634: _temp633=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp631)->f1; goto _LL612;} else{ goto
_LL613;} _LL613: _LL644: _temp643= _temp607.f1; if(*(( int*) _temp643) == Cyc_CfFlowInfo_TuplePI){
_LL646: _temp645=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp643)->f1; goto _LL640;} else{ goto _LL615;} _LL640: _temp639= _temp607.f2;
if(*(( int*) _temp639) == Cyc_CfFlowInfo_TuplePI){ _LL642: _temp641=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp639)->f1; goto _LL614;} else{ goto
_LL615;} _LL615: _LL652: _temp651= _temp607.f1; if(*(( int*) _temp651) == Cyc_CfFlowInfo_StructPI){
_LL654: _temp653=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp651)->f1; goto _LL648;} else{ goto _LL617;} _LL648: _temp647= _temp607.f2;
if(*(( int*) _temp647) == Cyc_CfFlowInfo_StructPI){ _LL650: _temp649=( struct
Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp647)->f1; goto
_LL616;} else{ goto _LL617;} _LL617: goto _LL618; _LL610:* escaping_states=({
struct Cyc_List_List* _temp655=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp655[ 0]=({ struct Cyc_List_List _temp656;
_temp656.hd=( void*) new_pinfo; _temp656.tl=* escaping_states; _temp656;});
_temp655;}); return; _LL612: return; _LL614:(( void(*)( void(* f)( struct Cyc_List_List**,
void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp645,
_temp641); return; _LL616:(( void(*)( void(* f)( struct Cyc_List_List**, void*,
void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp653,
_temp649); return; _LL618:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp657=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp657[ 0]=({ struct Cyc_Core_Impossible_struct _temp658; _temp658.tag= Cyc_Core_Impossible;
_temp658.f1=( struct _tagged_string)({ char* _temp659=( char*)"bad pinfos in assign_escape";
struct _tagged_string _temp660; _temp660.curr= _temp659; _temp660.base= _temp659;
_temp660.last_plus_one= _temp659 + 28; _temp660;}); _temp658;}); _temp657;}));
_LL608:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct Cyc_List_List*
ok_mallocs){ void* _temp661= lval; int _temp669; struct Cyc_Absyn_Exp* _temp671;
void* _temp673; void* _temp675; _LL663: if(( unsigned int) _temp661 > 1u?*(( int*)
_temp661) == Cyc_CfAbsexp_MallocOp: 0){ _LL672: _temp671=( struct Cyc_Absyn_Exp*)((
struct Cyc_CfAbsexp_MallocOp_struct*) _temp661)->f1; goto _LL670; _LL670:
_temp669=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp661)->f2; goto
_LL664;} else{ goto _LL665;} _LL665: if(( unsigned int) _temp661 > 1u?*(( int*)
_temp661) == Cyc_CfAbsexp_MemberOp: 0){ _LL676: _temp675=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp661)->f1; goto _LL674; _LL674: _temp673=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp661)->f2; goto _LL666;} else{ goto _LL667;} _LL667: goto _LL668; _LL664:
return(( int(*)( struct Cyc_List_List* l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)(
ok_mallocs, _temp671); _LL666: return Cyc_CfAbsexp_is_ok_malloc_assign( _temp675,
ok_mallocs); _LL668: return 0; _LL662:;} struct _tuple5{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; static struct _tuple5 Cyc_CfAbsexp_unordered_mallocs(
struct _RegionHandle* rgn, void* ae, struct Cyc_List_List* ok_mallocs){ struct
Cyc_List_List* _temp677= 0; goto _LL678; _LL678: { struct Cyc_List_List*
_temp679= 0; goto _LL680; _LL680:{ void* _temp681= ae; void* _temp703; void*
_temp705; void* _temp707; int _temp709; void* _temp711; struct Cyc_Absyn_Exp*
_temp713; int _temp715; void* _temp717; struct Cyc_Absyn_Stmt* _temp719; struct
Cyc_List_List* _temp721; void* _temp723; struct Cyc_List_List* _temp725; void*
_temp727; struct Cyc_List_List* _temp729; void* _temp731; _LL683: if( _temp681
==( void*) Cyc_CfAbsexp_BottomAE){ goto _LL684;} else{ goto _LL685;} _LL685: if(
_temp681 ==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL686;} else{ goto _LL687;}
_LL687: if(( unsigned int) _temp681 > 2u?*(( int*) _temp681) == Cyc_CfAbsexp_UseAE:
0){ _LL704: _temp703=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp681)->f1;
goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int) _temp681 > 2u?*((
int*) _temp681) == Cyc_CfAbsexp_AssignAE: 0){ _LL708: _temp707=( void*)(( struct
Cyc_CfAbsexp_AssignAE_struct*) _temp681)->f1; goto _LL706; _LL706: _temp705=(
void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp681)->f2; goto _LL690;}
else{ goto _LL691;} _LL691: if(( unsigned int) _temp681 > 2u?*(( int*) _temp681)
== Cyc_CfAbsexp_MallocAE: 0){ _LL712: _temp711=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp681)->f1; if(*(( int*) _temp711) == Cyc_CfFlowInfo_MallocPt){ _LL714:
_temp713=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp711)->f1; goto _LL710;} else{ goto _LL693;} _LL710: _temp709=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp681)->f2; goto _LL692;} else{ goto
_LL693;} _LL693: if(( unsigned int) _temp681 > 2u?*(( int*) _temp681) == Cyc_CfAbsexp_MallocAE:
0){ _LL718: _temp717=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp681)->f1;
goto _LL716; _LL716: _temp715=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp681)->f2; goto _LL694;} else{ goto _LL695;} _LL695: if(( unsigned int)
_temp681 > 2u?*(( int*) _temp681) == Cyc_CfAbsexp_StmtAE: 0){ _LL720: _temp719=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp681)->f1;
goto _LL696;} else{ goto _LL697;} _LL697: if(( unsigned int) _temp681 > 2u?*((
int*) _temp681) == Cyc_CfAbsexp_GroupAE: 0){ _LL724: _temp723=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp681)->f1; if( _temp723 ==( void*) Cyc_CfAbsexp_OrderedG){
goto _LL722;} else{ goto _LL699;} _LL722: _temp721=( struct Cyc_List_List*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp681)->f2; goto _LL698;} else{ goto
_LL699;} _LL699: if(( unsigned int) _temp681 > 2u?*(( int*) _temp681) == Cyc_CfAbsexp_GroupAE:
0){ _LL728: _temp727=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp681)->f1;
if( _temp727 ==( void*) Cyc_CfAbsexp_OneofG){ goto _LL726;} else{ goto _LL701;}
_LL726: _temp725=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp681)->f2; goto _LL700;} else{ goto _LL701;} _LL701: if(( unsigned int)
_temp681 > 2u?*(( int*) _temp681) == Cyc_CfAbsexp_GroupAE: 0){ _LL732: _temp731=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp681)->f1; if( _temp731 ==(
void*) Cyc_CfAbsexp_UnorderedG){ goto _LL730;} else{ goto _LL682;} _LL730:
_temp729=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp681)->f2; goto _LL702;} else{ goto _LL682;} _LL684: goto _LL682; _LL686:
goto _LL682; _LL688: goto _LL682; _LL690: if( Cyc_CfAbsexp_is_ok_malloc_assign(
_temp707, ok_mallocs)){ _temp677=({ struct Cyc_List_List* _temp733=( struct Cyc_List_List*)
_region_malloc( rgn, sizeof( struct Cyc_List_List)); _temp733->hd=( void*) ae;
_temp733->tl= _temp677; _temp733;});} goto _LL682; _LL692: _temp679=({ struct
Cyc_List_List* _temp734=( struct Cyc_List_List*) _region_malloc( rgn, sizeof(
struct Cyc_List_List)); _temp734->hd=( void*) _temp713; _temp734->tl= _temp679;
_temp734;}); goto _LL682; _LL694:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp735=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp735[ 0]=({ struct Cyc_Core_Impossible_struct _temp736; _temp736.tag= Cyc_Core_Impossible;
_temp736.f1=( struct _tagged_string)({ char* _temp737=( char*)"local variable in MallocAE";
struct _tagged_string _temp738; _temp738.curr= _temp737; _temp738.base= _temp737;
_temp738.last_plus_one= _temp737 + 27; _temp738;}); _temp736;}); _temp735;}));
_LL696: goto _LL682; _LL698: for( 0; _temp721 != 0; _temp721=({ struct Cyc_List_List*
_temp739= _temp721; if( _temp739 == 0){ _throw( Null_Exception);} _temp739->tl;})){
struct Cyc_List_List* _temp743; struct Cyc_List_List* _temp745; struct _tuple5
_temp741= Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)({ struct Cyc_List_List*
_temp740= _temp721; if( _temp740 == 0){ _throw( Null_Exception);} _temp740->hd;}),((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp679, ok_mallocs)); _LL746:
_temp745= _temp741.f1; goto _LL744; _LL744: _temp743= _temp741.f2; goto _LL742;
_LL742: _temp677=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp745, _temp677);
_temp679=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp743, _temp679);} goto
_LL682; _LL700: goto _LL682; _LL702: for( 0; _temp729 != 0; _temp729=({ struct
Cyc_List_List* _temp747= _temp729; if( _temp747 == 0){ _throw( Null_Exception);}
_temp747->tl;})){ struct Cyc_List_List* _temp751; struct Cyc_List_List* _temp753;
struct _tuple5 _temp749= Cyc_CfAbsexp_unordered_mallocs( rgn,( void*)({ struct
Cyc_List_List* _temp748= _temp729; if( _temp748 == 0){ _throw( Null_Exception);}
_temp748->hd;}), ok_mallocs); _LL754: _temp753= _temp749.f1; goto _LL752; _LL752:
_temp751= _temp749.f2; goto _LL750; _LL750: _temp677=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp753, _temp677); _temp679=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rappend)(
rgn, _temp751, _temp679);} goto _LL682; _LL682:;} return({ struct _tuple5
_temp755; _temp755.f1= _temp677; _temp755.f2= _temp679; _temp755;});}} void* Cyc_CfAbsexp_eval_absexp(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
ae, void* in_flow){ struct Cyc_Dict_Dict* pinfo_dict;{ void* _temp756= in_flow;
struct Cyc_Dict_Dict* _temp762; _LL758: if( _temp756 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL759;} else{ goto _LL760;} _LL760: if(( unsigned int) _temp756 > 1u?*((
int*) _temp756) == Cyc_CfFlowInfo_InitsFL: 0){ _LL763: _temp762=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp756)->f1; goto _LL761;} else{ goto
_LL757;} _LL759: return( void*) Cyc_CfFlowInfo_BottomFL; _LL761: pinfo_dict=
_temp762; goto _LL757; _LL757:;}{ void* _temp764= ae; void* _temp784; void*
_temp786; void* _temp788; int _temp790; void* _temp792; struct Cyc_Absyn_Stmt*
_temp794; struct Cyc_List_List* _temp796; void* _temp798; struct Cyc_List_List*
_temp800; void* _temp802; struct Cyc_List_List* _temp804; void* _temp806; _LL766:
if(( unsigned int) _temp764 > 2u?*(( int*) _temp764) == Cyc_CfAbsexp_AssignAE: 0){
_LL787: _temp786=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp764)->f1;
goto _LL785; _LL785: _temp784=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp764)->f2; goto _LL767;} else{ goto _LL768;} _LL768: if(( unsigned int)
_temp764 > 2u?*(( int*) _temp764) == Cyc_CfAbsexp_UseAE: 0){ _LL789: _temp788=(
void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp764)->f1; goto _LL769;} else{
goto _LL770;} _LL770: if(( unsigned int) _temp764 > 2u?*(( int*) _temp764) ==
Cyc_CfAbsexp_MallocAE: 0){ _LL793: _temp792=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp764)->f1; goto _LL791; _LL791: _temp790=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp764)->f2; goto _LL771;} else{ goto _LL772;} _LL772: if( _temp764 ==( void*)
Cyc_CfAbsexp_BottomAE){ goto _LL773;} else{ goto _LL774;} _LL774: if( _temp764
==( void*) Cyc_CfAbsexp_SkipAE){ goto _LL775;} else{ goto _LL776;} _LL776: if((
unsigned int) _temp764 > 2u?*(( int*) _temp764) == Cyc_CfAbsexp_StmtAE: 0){
_LL795: _temp794=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp764)->f1; goto _LL777;} else{ goto _LL778;} _LL778: if(( unsigned int)
_temp764 > 2u?*(( int*) _temp764) == Cyc_CfAbsexp_GroupAE: 0){ _LL799: _temp798=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp764)->f1; if( _temp798 ==(
void*) Cyc_CfAbsexp_OrderedG){ goto _LL797;} else{ goto _LL780;} _LL797:
_temp796=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp764)->f2; goto _LL779;} else{ goto _LL780;} _LL780: if(( unsigned int)
_temp764 > 2u?*(( int*) _temp764) == Cyc_CfAbsexp_GroupAE: 0){ _LL803: _temp802=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp764)->f1; if( _temp802 ==(
void*) Cyc_CfAbsexp_OneofG){ goto _LL801;} else{ goto _LL782;} _LL801: _temp800=(
struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp764)->f2;
goto _LL781;} else{ goto _LL782;} _LL782: if(( unsigned int) _temp764 > 2u?*((
int*) _temp764) == Cyc_CfAbsexp_GroupAE: 0){ _LL807: _temp806=( void*)(( struct
Cyc_CfAbsexp_GroupAE_struct*) _temp764)->f1; if( _temp806 ==( void*) Cyc_CfAbsexp_UnorderedG){
goto _LL805;} else{ goto _LL765;} _LL805: _temp804=( struct Cyc_List_List*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp764)->f2; goto _LL783;} else{ goto
_LL765;} _LL767:{ void* _temp808= Cyc_CfAbsexp_eval_absop_l( pinfo_dict,
_temp786); void* _temp814; void* _temp816; struct Cyc_CfFlowInfo_Place* _temp818;
_LL810: if(*(( int*) _temp808) == Cyc_CfFlowInfo_UnknownIS){ _LL817: _temp816=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp808)->f1; goto _LL815;
_LL815: _temp814=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp808)->f2;
goto _LL811;} else{ goto _LL812;} _LL812: if(*(( int*) _temp808) == Cyc_CfFlowInfo_MustPointTo){
_LL819: _temp818=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp808)->f1; goto _LL813;} else{ goto _LL809;} _LL811: goto _LL809; _LL813: {
void* _temp820= Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp818); goto _LL821;
_LL821: { void* _temp822= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp784); goto
_LL823; _LL823: { struct _tuple3 _temp825=({ struct _tuple3 _temp824; _temp824.f1=
_temp820; _temp824.f2= _temp822; _temp824;}); void* _temp833; void* _temp835;
void* _temp837; void* _temp839; void* _temp841; struct Cyc_Dict_Dict* _temp843;
void* _temp845; void* _temp847; void* _temp849; void* _temp851; void* _temp853;
struct Cyc_Dict_Dict* _temp855; _LL827: _LL842: _temp841= _temp825.f1; if(*((
int*) _temp841) == Cyc_CfFlowInfo_TuplePI){ _LL844: _temp843=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp841)->f1; goto _LL834;} else{ goto
_LL829;} _LL834: _temp833= _temp825.f2; if(*(( int*) _temp833) == Cyc_CfFlowInfo_LeafPI){
_LL836: _temp835=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp833)->f1;
if(*(( int*) _temp835) == Cyc_CfFlowInfo_UnknownIS){ _LL840: _temp839=( void*)((
struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp835)->f1; goto _LL838; _LL838:
_temp837=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp835)->f2; goto
_LL828;} else{ goto _LL829;}} else{ goto _LL829;} _LL829: _LL854: _temp853=
_temp825.f1; if(*(( int*) _temp853) == Cyc_CfFlowInfo_StructPI){ _LL856:
_temp855=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp853)->f1; goto _LL846;} else{ goto _LL831;} _LL846: _temp845= _temp825.f2;
if(*(( int*) _temp845) == Cyc_CfFlowInfo_LeafPI){ _LL848: _temp847=( void*)((
struct Cyc_CfFlowInfo_LeafPI_struct*) _temp845)->f1; if(*(( int*) _temp847) ==
Cyc_CfFlowInfo_UnknownIS){ _LL852: _temp851=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp847)->f1; goto _LL850; _LL850: _temp849=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp847)->f2; goto _LL830;} else{ goto _LL831;}} else{ goto _LL831;} _LL831:
goto _LL832; _LL828: _temp849= _temp837; goto _LL830; _LL830: _temp822= Cyc_CfFlowInfo_assign_unknown_dict(
_temp849, _temp820); pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp818, _temp822);{ struct Cyc_CfFlowInfo_InitsFL_struct* _temp859=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp857=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp857[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp858; _temp858.tag= Cyc_CfFlowInfo_InitsFL;
_temp858.f1= pinfo_dict; _temp858;}); _temp857;}); goto _LL860; _LL860: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp859); return( void*) _temp859;} _LL832: { struct Cyc_List_List*
_temp861= 0; goto _LL862; _LL862: Cyc_CfAbsexp_assign_escape(& _temp861,
_temp820, _temp822); for( 0; _temp861 != 0; _temp861=({ struct Cyc_List_List*
_temp863= _temp861; if( _temp863 == 0){ _throw( Null_Exception);} _temp863->tl;})){
void* _temp865=( void*)({ struct Cyc_List_List* _temp864= _temp861; if( _temp864
== 0){ _throw( Null_Exception);} _temp864->hd;}); goto _LL866; _LL866: if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, _temp865)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp867=( char*)"uninitialized value assigned to ambiguous place"; struct
_tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 48; _temp868;})); return( void*) Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( _temp865, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp818, _temp822);{ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp871=({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp869=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp869[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp870; _temp870.tag= Cyc_CfFlowInfo_InitsFL;
_temp870.f1= pinfo_dict; _temp870;}); _temp869;}); goto _LL872; _LL872: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp871); return( void*) _temp871;}} _LL826:;}}} _LL809:;}
_temp788= _temp784; goto _LL769; _LL769: { void* _temp873= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp788); goto _LL874; _LL874: if( ! Cyc_CfFlowInfo_isAllInit(
pinfo_dict, _temp873)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp875=
Cyc_CfAbsexp_absop2string( _temp788); xprintf("%.*s must be fully initialized to be used here",
_temp875.last_plus_one - _temp875.curr, _temp875.curr);})); return( void*) Cyc_CfFlowInfo_BottomFL;}{
struct Cyc_Dict_Dict* _temp876= Cyc_CfFlowInfo_escape_pointsto( _temp873,
pinfo_dict); goto _LL877; _LL877: if( _temp876 == pinfo_dict){ return in_flow;}{
struct Cyc_CfFlowInfo_InitsFL_struct* _temp880=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp878=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp878[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp879; _temp879.tag= Cyc_CfFlowInfo_InitsFL;
_temp879.f1= pinfo_dict; _temp879;}); _temp878;}); goto _LL881; _LL881: Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp880); return( void*) _temp880;}}} _LL771: return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp882=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp882[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp883; _temp883.tag= Cyc_CfFlowInfo_InitsFL;
_temp883.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key,
void* data)) Cyc_Dict_insert)( pinfo_dict, _temp792,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots, _temp792)); _temp883;}); _temp882;});
_LL773: return( void*) Cyc_CfFlowInfo_BottomFL; _LL775: return in_flow; _LL777:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp794, in_flow); _LL779: for(
0; _temp796 != 0; _temp796=({ struct Cyc_List_List* _temp884= _temp796; if(
_temp884 == 0){ _throw( Null_Exception);} _temp884->tl;})){ in_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp885= _temp796; if( _temp885 == 0){
_throw( Null_Exception);} _temp885->hd;}), in_flow);} return in_flow; _LL781: {
void* out_flow=( void*) Cyc_CfFlowInfo_BottomFL; for( 0; _temp800 != 0; _temp800=({
struct Cyc_List_List* _temp886= _temp800; if( _temp886 == 0){ _throw(
Null_Exception);} _temp886->tl;})){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow,
Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({ struct Cyc_List_List* _temp887=
_temp800; if( _temp887 == 0){ _throw( Null_Exception);} _temp887->hd;}), in_flow));}
return out_flow;} _LL783: { void* out_flow= in_flow; do { in_flow= out_flow;
out_flow= in_flow;{ struct Cyc_List_List* ael2= _temp804; for( 0; ael2 != 0;
ael2=({ struct Cyc_List_List* _temp888= ael2; if( _temp888 == 0){ _throw(
Null_Exception);} _temp888->tl;})){ out_flow= Cyc_CfFlowInfo_join_flow( out_flow,
Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({ struct Cyc_List_List* _temp889=
ael2; if( _temp889 == 0){ _throw( Null_Exception);} _temp889->hd;}), out_flow));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( out_flow, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp890= in_flow; struct Cyc_Dict_Dict* _temp896; _LL892:
if( _temp890 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL893;} else{ goto
_LL894;} _LL894: if(( unsigned int) _temp890 > 1u?*(( int*) _temp890) == Cyc_CfFlowInfo_InitsFL:
0){ _LL897: _temp896=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp890)->f1; goto _LL895;} else{ goto _LL891;} _LL893: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL895: pinfo_dict= _temp896; goto _LL891; _LL891:;}{ struct _RegionHandle
_temp898= _new_region(); struct _RegionHandle* spine_rgn=& _temp898;
_push_region( spine_rgn);{ struct Cyc_List_List* _temp901; struct Cyc_List_List*
_temp903; struct _tuple5 _temp899= Cyc_CfAbsexp_unordered_mallocs( spine_rgn, ae,
0); _LL904: _temp903= _temp899.f1; goto _LL902; _LL902: _temp901= _temp899.f2;
goto _LL900; _LL900: for( 0; _temp901 != 0; _temp901=({ struct Cyc_List_List*
_temp905= _temp901; if( _temp905 == 0){ _throw( Null_Exception);} _temp905->tl;})){
struct Cyc_CfFlowInfo_MallocPt_struct* _temp909=({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp906=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp906[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp907; _temp907.tag=
Cyc_CfFlowInfo_MallocPt; _temp907.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp908= _temp901; if( _temp908 == 0){ _throw( Null_Exception);} _temp908->hd;});
_temp907;}); _temp906;}); goto _LL910; _LL910: pinfo_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( pinfo_dict,(
void*) _temp909,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots,( void*) _temp909));} out_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp911=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp911[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp912; _temp912.tag= Cyc_CfFlowInfo_InitsFL;
_temp912.f1= pinfo_dict; _temp912;}); _temp911;}); for( 0; _temp903 != 0;
_temp903=({ struct Cyc_List_List* _temp913= _temp903; if( _temp913 == 0){ _throw(
Null_Exception);} _temp913->tl;})){ out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,(
void*)({ struct Cyc_List_List* _temp914= _temp903; if( _temp914 == 0){ _throw(
Null_Exception);} _temp914->hd;}), out_flow);}}; _pop_region(& _temp898);}
return out_flow;}} _LL765:;}}