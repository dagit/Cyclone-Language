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
struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void* f1; void* f2; } ;
extern void* Cyc_Core_snd( struct _tuple0*); extern int Cyc_Core_intcmp( int,
int); extern char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure[ 12u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u]; extern char
Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char* tag;
struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern char Cyc_List_Nth[ 8u]; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[
12u]; extern char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key, void* data); extern
void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); extern void* Cyc_Assert_AssertFail( struct _tagged_string msg);
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
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t; struct _tuple1{ void*
f1; struct _tagged_string* f2; } ; typedef struct _tuple1* Cyc_Absyn_qvar_t;
typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
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
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
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
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
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
f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
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
12u]; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_CfFlowInfo_Place;
typedef void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t;
typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF=
0; struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp[ 14u]; static
const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG= 1;
static const int Cyc_CfAbsexp_OneofG= 2; typedef void* Cyc_CfAbsexp_absexp_t;
typedef void* Cyc_CfAbsexp_absexpgroup_t; typedef void* Cyc_CfAbsexp_absop_t;
extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkAssignAE( void*
l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern int Cyc_CfAbsexp_isUnknownOp( void*); extern
void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, void* ae, void* in_flow); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt*
encloser; int visited; void* flow; struct Cyc_List_List* absexps; } ; typedef
struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t; char
Cyc_NewControlFlow_CFAnnot[ 12u]="\000\000\000\000CFAnnot"; struct Cyc_NewControlFlow_CFAnnot_struct{
char* tag; struct Cyc_NewControlFlow_CFStmtAnnot f1; } ; void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser){( void*)(
enclosee->annot=( void*)(( void*)({ struct Cyc_NewControlFlow_CFAnnot_struct*
_temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({ struct Cyc_NewControlFlow_CFAnnot_struct
_temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot; _temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot
_temp2; _temp2.encloser= encloser; _temp2.visited= 0; _temp2.flow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;})));}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt* s){ void* _temp3=( void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp9; struct Cyc_NewControlFlow_CFStmtAnnot* _temp11; _LL5: if(*(( void**)
_temp3) == Cyc_NewControlFlow_CFAnnot){ _LL10: _temp9=( struct Cyc_NewControlFlow_CFStmtAnnot)((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11;
_LL8:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp12=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp12[ 0]=({ struct Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible;
_temp13.f1=( struct _tagged_string)({ char* _temp14=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14;
_temp15.last_plus_one= _temp14 + 37; _temp15;}); _temp13;}); _temp12;})); _LL4:;}
void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt* s, struct Cyc_List_List*
ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{
struct Cyc_Dict_Dict* all_roots; struct Cyc_List_List* comprehension_vars; } ;
struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_NewControlFlow_SharedAbsEnv* shared;
struct Cyc_List_List* lhsides; struct Cyc_List_List* access_path; } ; typedef
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_absenv_t; struct Cyc_NewControlFlow_AbsSynOne{
void* inner_exp; void* assigns; } ; typedef struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_abssyn_one_t; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; typedef struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abssyn_t;
void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp16=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp16[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_VarRoot;
_temp17.f1= vd; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp18=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp18[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp19; _temp19.tag= Cyc_CfFlowInfo_MallocPt;
_temp19.f1= e; _temp19;}); _temp18;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
struct _tuple4{ struct Cyc_Absyn_Tqual f1; void* f2; } ; void Cyc_NewControlFlow_add_tunion_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Tunionfield*
ef){ struct Cyc_Absyn_Tunionfield _temp22; struct Cyc_Position_Segment* _temp23;
struct Cyc_List_List* _temp25; struct Cyc_List_List* _temp27; struct _tuple1*
_temp29; struct Cyc_Absyn_Tunionfield* _temp20= ef; _temp22=* _temp20; _LL30:
_temp29=( struct _tuple1*) _temp22.name; goto _LL28; _LL28: _temp27=( struct Cyc_List_List*)
_temp22.tvs; goto _LL26; _LL26: _temp25=( struct Cyc_List_List*) _temp22.typs;
goto _LL24; _LL24: _temp23=( struct Cyc_Position_Segment*) _temp22.loc; goto
_LL21; _LL21: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0; _temp25 != 0;
_temp25=({ struct Cyc_List_List* _temp31= _temp25; if( _temp31 == 0){ _throw(
Null_Exception);} _temp31->tl;}), ++ i){ struct _tuple4 _temp35; void* _temp36;
struct Cyc_Absyn_Tqual _temp38; struct _tuple4* _temp33=( struct _tuple4*)({
struct Cyc_List_List* _temp32= _temp25; if( _temp32 == 0){ _throw(
Null_Exception);} _temp32->hd;}); _temp35=* _temp33; _LL39: _temp38= _temp35.f1;
goto _LL37; _LL37: _temp36= _temp35.f2; goto _LL34; _LL34: d=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp36));}}( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp40=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp40[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp41; _temp41.tag= Cyc_CfFlowInfo_MallocPt;
_temp41.f1= e; _temp41;}); _temp40;}),( void*)({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp42=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp42[ 0]=({ struct Cyc_CfFlowInfo_TuplePI_struct _temp43; _temp43.tag= Cyc_CfFlowInfo_TuplePI;
_temp43.f1= d; _temp43;}); _temp42;}));}} struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs(
struct Cyc_NewControlFlow_AbsEnv src, struct Cyc_List_List* new_lhs){ return({
struct Cyc_NewControlFlow_AbsEnv _temp44; _temp44.shared= src.shared; _temp44.lhsides=
new_lhs; _temp44.access_path= src.access_path; _temp44;});} static struct Cyc_List_List*
Cyc_NewControlFlow_unknown_list= 0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){
if( Cyc_NewControlFlow_unknown_list == 0){ Cyc_NewControlFlow_unknown_list=({
struct Cyc_List_List* _temp45=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp45->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp45->tl= 0;
_temp45;});} return Cyc_NewControlFlow_unknown_list;} struct Cyc_NewControlFlow_AbsEnv
Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv src){ struct Cyc_NewControlFlow_AbsEnv
ans= Cyc_NewControlFlow_change_lhs( src, Cyc_NewControlFlow_use_it_list()); ans.access_path=
0; return ans;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp48; struct Cyc_NewControlFlow_AbsSynOne
_temp50; void* _temp52; void* _temp54; struct Cyc_NewControlFlow_AbsSynOne
_temp56; void* _temp58; void* _temp60; struct Cyc_NewControlFlow_AbsSyn _temp46=
syn; _LL57: _temp56=( struct Cyc_NewControlFlow_AbsSynOne) _temp46.when_true;
_LL61: _temp60=( void*) _temp56.inner_exp; goto _LL59; _LL59: _temp58=( void*)
_temp56.assigns; goto _LL51; _LL51: _temp50=( struct Cyc_NewControlFlow_AbsSynOne)
_temp46.when_false; _LL55: _temp54=( void*) _temp50.inner_exp; goto _LL53; _LL53:
_temp52=( void*) _temp50.assigns; goto _LL49; _LL49: _temp48=( struct Cyc_List_List*)
_temp46.lvalues; goto _LL47; _LL47: { void* inn= _temp60 == _temp54? _temp60:
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp60, _temp54); void*
ass= _temp58 == _temp52? _temp58: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
_temp58, _temp52); return({ struct Cyc_NewControlFlow_AbsSynOne _temp62; _temp62.inner_exp=(
void*) inn; _temp62.assigns=( void*) ass; _temp62;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne one_way=({ struct Cyc_NewControlFlow_AbsSynOne
_temp64; _temp64.inner_exp=( void*) ae; _temp64.assigns=( void*) assigns;
_temp64;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp63; _temp63.when_true=
one_way; _temp63.when_false= one_way; _temp63.lvalues= lvals; _temp63;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(({ struct Cyc_List_List*
_temp65= env.lhsides; if( _temp65 == 0){ _throw( Null_Exception);} _temp65->tl;})
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List* _temp66=
env.lhsides; if( _temp66 == 0){ _throw( Null_Exception);} _temp66->hd;}), ae);}{
struct Cyc_List_List* l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0;
lhsides != 0; lhsides=({ struct Cyc_List_List* _temp67= lhsides; if( _temp67 ==
0){ _throw( Null_Exception);} _temp67->tl;})){ l=({ struct Cyc_List_List*
_temp68=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp68->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List*
_temp69= lhsides; if( _temp69 == 0){ _throw( Null_Exception);} _temp69->hd;}),
ae); _temp68->tl= l; _temp68;});}} return Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_OneofG,
l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=({ struct Cyc_List_List*
_temp70= es; if( _temp70 == 0){ _throw( Null_Exception);} _temp70->tl;})){
struct Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp77= es; if( _temp77 == 0){
_throw( Null_Exception);} _temp77->hd;})); void* _temp73; void* _temp75; struct
Cyc_NewControlFlow_AbsSynOne _temp71= Cyc_NewControlFlow_meet_absexp( syn);
_LL76: _temp75=( void*) _temp71.inner_exp; goto _LL74; _LL74: _temp73=( void*)
_temp71.assigns; goto _LL72; _LL72: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp75, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp73, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* es_exp,
void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp( e))));} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_descend_path( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp* inner_e){ env.access_path=({ struct
Cyc_List_List* _temp78=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp78->hd=( void*) path_e; _temp78->tl= env.access_path; _temp78;});{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, inner_e);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path=({ struct Cyc_List_List* _temp79= path; if( _temp79 == 0){ _throw(
Null_Exception);} _temp79->tl;})){ void* _temp81=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp80= path; if( _temp80 == 0){ _throw( Null_Exception);}
_temp80->hd;}))->r; struct Cyc_Absyn_Exp* _temp95; struct Cyc_Absyn_Exp* _temp97;
struct _tagged_string* _temp99; struct Cyc_Absyn_Exp* _temp101; struct
_tagged_string* _temp103; struct Cyc_Absyn_Exp* _temp105; struct Cyc_Absyn_Exp*
_temp107; struct Cyc_Absyn_Exp* _temp109; _LL83: if(*(( int*) _temp81) == Cyc_Absyn_Address_e){
_LL96: _temp95=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp81)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp81) ==
Cyc_Absyn_Deref_e){ _LL98: _temp97=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp81)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if(*(( int*) _temp81) ==
Cyc_Absyn_StructArrow_e){ _LL102: _temp101=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp81)->f1; goto _LL100; _LL100: _temp99=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp81)->f2; goto _LL88;} else{ goto _LL89;}
_LL89: if(*(( int*) _temp81) == Cyc_Absyn_StructMember_e){ _LL106: _temp105=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp81)->f1;
goto _LL104; _LL104: _temp103=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp81)->f2; goto _LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp81) ==
Cyc_Absyn_Subscript_e){ _LL110: _temp109=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp81)->f1; goto _LL108; _LL108: _temp107=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp81)->f2; goto _LL92;} else{ goto _LL93;}
_LL93: goto _LL94; _LL84:({ struct Cyc_List_List* _temp111= path; if( _temp111
== 0){ _throw( Null_Exception);} _temp111->tl;}) == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp112=( char*)"new_control_flow.cyc:239 path->tl == null"; struct
_tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 42; _temp113;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL86: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL82; _LL88: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp103= _temp99; goto _LL90; _LL90: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp114=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp114[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp115; _temp115.tag= Cyc_CfFlowInfo_StructF;
_temp115.f1= _temp103; _temp115;}); _temp114;})); goto _LL82; _LL92: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp107); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp116=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp116[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp117; _temp117.tag= Cyc_CfFlowInfo_TupleF;
_temp117.f1=( int) i; _temp117;}); _temp116;})); goto _LL82;} _LL94:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp118=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp118[ 0]=({ struct
Cyc_Core_Impossible_struct _temp119; _temp119.tag= Cyc_Core_Impossible; _temp119.f1=(
struct _tagged_string)({ char* _temp120=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp121; _temp121.curr= _temp120; _temp121.base= _temp120;
_temp121.last_plus_one= _temp120 + 44; _temp121;}); _temp119;}); _temp118;}));
_LL82:;}} return absop;} struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp122=( void*)
e->r; struct Cyc_Absyn_Exp* _temp210; struct Cyc_Absyn_Exp* _temp212; struct Cyc_List_List*
_temp214; struct Cyc_Absyn_Exp* _temp216; void* _temp218; int _temp220; void*
_temp222; void* _temp224; int _temp226; void* _temp228; void* _temp230; void*
_temp232; struct Cyc_Absyn_Enumfield* _temp234; struct Cyc_Absyn_Enumdecl*
_temp236; struct _tuple1* _temp238; void* _temp240; struct Cyc_Absyn_Fndecl*
_temp242; struct _tuple1* _temp244; void* _temp246; struct Cyc_Absyn_Vardecl*
_temp248; struct _tuple1* _temp250; void* _temp252; struct Cyc_Absyn_Vardecl*
_temp254; struct _tuple1* _temp256; void* _temp258; struct Cyc_Absyn_Vardecl*
_temp260; struct _tuple1* _temp262; void* _temp264; struct Cyc_Absyn_Vardecl*
_temp266; struct _tuple1* _temp268; struct Cyc_Absyn_Tunionfield* _temp270;
struct Cyc_Absyn_Tuniondecl* _temp272; struct Cyc_List_List* _temp274; struct
Cyc_Core_Opt* _temp276; struct Cyc_Core_Opt* _temp278; void* _temp280; struct
Cyc_Absyn_Exp* _temp282; struct Cyc_Absyn_Exp* _temp284; struct Cyc_Core_Opt*
_temp286; struct Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct
Cyc_Core_Opt* _temp292; struct Cyc_Absyn_Exp* _temp294; struct Cyc_List_List*
_temp296; void* _temp298; struct Cyc_Absyn_Exp* _temp300; void* _temp302; struct
Cyc_List_List* _temp304; struct Cyc_Absyn_Exp* _temp306; struct Cyc_Absyn_Exp*
_temp308; struct Cyc_Absyn_Exp* _temp310; struct Cyc_Absyn_Exp* _temp312; struct
Cyc_Absyn_Exp* _temp314; struct Cyc_Absyn_Exp* _temp316; struct Cyc_Absyn_Exp*
_temp318; struct _tagged_string* _temp320; struct Cyc_Absyn_Exp* _temp322;
struct _tagged_string* _temp324; struct Cyc_Absyn_Exp* _temp326; struct Cyc_List_List*
_temp328; struct Cyc_List_List* _temp330; void* _temp332; struct Cyc_Absyn_Structdecl*
_temp334; struct Cyc_List_List* _temp336; struct Cyc_Core_Opt* _temp338; struct
_tuple1* _temp340; struct Cyc_List_List* _temp342; void* _temp344; struct Cyc_Absyn_Exp*
_temp346; struct Cyc_Absyn_Exp* _temp348; struct Cyc_Absyn_Exp* _temp350; struct
Cyc_Absyn_Exp* _temp352; struct Cyc_Absyn_Exp* _temp354; struct Cyc_Absyn_Stmt*
_temp356; struct Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Exp* _temp360; struct
Cyc_Absyn_Exp* _temp362; struct Cyc_Absyn_Exp* _temp364; struct Cyc_Absyn_Vardecl*
_temp366; void* _temp368; struct _tuple1* _temp370; struct _tuple1* _temp372;
struct Cyc_List_List* _temp374; struct Cyc_Absyn_Exp* _temp376; struct Cyc_List_List*
_temp378; struct Cyc_Core_Opt* _temp380; struct Cyc_List_List* _temp382; struct
_tuple2* _temp384; struct Cyc_Absyn_Fndecl* _temp386; struct Cyc_Absyn_Exp*
_temp388; _LL124: if(*(( int*) _temp122) == Cyc_Absyn_NoInstantiate_e){ _LL211:
_temp210=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp122)->f1; goto _LL125;} else{ goto _LL126;} _LL126: if(*(( int*) _temp122)
== Cyc_Absyn_Sizeofexp_e){ _LL213: _temp212=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Sizeofexp_e_struct*) _temp122)->f1; goto _LL127;} else{ goto _LL128;}
_LL128: if(*(( int*) _temp122) == Cyc_Absyn_Instantiate_e){ _LL217: _temp216=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp122)->f1;
goto _LL215; _LL215: _temp214=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp122)->f2; goto _LL129;} else{ goto _LL130;} _LL130: if(*(( int*) _temp122)
== Cyc_Absyn_Const_e){ _LL219: _temp218=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp122)->f1; if(( unsigned int) _temp218 > 1u?*(( int*) _temp218) == Cyc_Absyn_Int_c:
0){ _LL223: _temp222=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp218)->f1;
if( _temp222 ==( void*) Cyc_Absyn_Signed){ goto _LL221;} else{ goto _LL132;}
_LL221: _temp220=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp218)->f2; if(
_temp220 == 0){ goto _LL131;} else{ goto _LL132;}} else{ goto _LL132;}} else{
goto _LL132;} _LL132: if(*(( int*) _temp122) == Cyc_Absyn_Const_e){ _LL225:
_temp224=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp122)->f1; if((
unsigned int) _temp224 > 1u?*(( int*) _temp224) == Cyc_Absyn_Int_c: 0){ _LL229:
_temp228=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp224)->f1; if( _temp228
==( void*) Cyc_Absyn_Signed){ goto _LL227;} else{ goto _LL134;} _LL227: _temp226=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp224)->f2; if( _temp226 == 1){ goto
_LL133;} else{ goto _LL134;}} else{ goto _LL134;}} else{ goto _LL134;} _LL134:
if(*(( int*) _temp122) == Cyc_Absyn_Sizeoftyp_e){ _LL231: _temp230=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp122)->f1; goto _LL135;} else{ goto
_LL136;} _LL136: if(*(( int*) _temp122) == Cyc_Absyn_Const_e){ _LL233: _temp232=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp122)->f1; goto _LL137;} else{
goto _LL138;} _LL138: if(*(( int*) _temp122) == Cyc_Absyn_Enum_e){ _LL239:
_temp238=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp122)->f1;
goto _LL237; _LL237: _temp236=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp122)->f2; goto _LL235; _LL235: _temp234=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp122)->f3; goto _LL139;} else{ goto _LL140;}
_LL140: if(*(( int*) _temp122) == Cyc_Absyn_Var_e){ _LL245: _temp244=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1; goto _LL241; _LL241:
_temp240=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f2; if((
unsigned int) _temp240 > 1u?*(( int*) _temp240) == Cyc_Absyn_Funname_b: 0){
_LL243: _temp242=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp240)->f1; goto _LL141;} else{ goto _LL142;}} else{ goto _LL142;} _LL142:
if(*(( int*) _temp122) == Cyc_Absyn_Var_e){ _LL251: _temp250=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp122)->f1; goto _LL247; _LL247: _temp246=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f2; if(( unsigned int)
_temp246 > 1u?*(( int*) _temp246) == Cyc_Absyn_Global_b: 0){ _LL249: _temp248=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp246)->f1;
goto _LL143;} else{ goto _LL144;}} else{ goto _LL144;} _LL144: if(*(( int*)
_temp122) == Cyc_Absyn_Var_e){ _LL257: _temp256=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL253; _LL253: _temp252=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp252 > 1u?*(( int*) _temp252) == Cyc_Absyn_Param_b:
0){ _LL255: _temp254=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp252)->f1; goto _LL145;} else{ goto _LL146;}} else{ goto _LL146;} _LL146:
if(*(( int*) _temp122) == Cyc_Absyn_Var_e){ _LL263: _temp262=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp122)->f1; goto _LL259; _LL259: _temp258=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f2; if(( unsigned int)
_temp258 > 1u?*(( int*) _temp258) == Cyc_Absyn_Local_b: 0){ _LL261: _temp260=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp258)->f1;
goto _LL147;} else{ goto _LL148;}} else{ goto _LL148;} _LL148: if(*(( int*)
_temp122) == Cyc_Absyn_Var_e){ _LL269: _temp268=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL265; _LL265: _temp264=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp264 > 1u?*(( int*) _temp264) == Cyc_Absyn_Pat_b:
0){ _LL267: _temp266=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp264)->f1; goto _LL149;} else{ goto _LL150;}} else{ goto _LL150;} _LL150:
if(*(( int*) _temp122) == Cyc_Absyn_Tunion_e){ _LL279: _temp278=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f1; goto _LL277; _LL277: _temp276=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f2; goto
_LL275; _LL275: _temp274=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp122)->f3; goto _LL273; _LL273: _temp272=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f4; goto _LL271; _LL271: _temp270=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f5;
goto _LL151;} else{ goto _LL152;} _LL152: if(*(( int*) _temp122) == Cyc_Absyn_Malloc_e){
_LL283: _temp282=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp122)->f1; goto _LL281; _LL281: _temp280=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp122)->f2; goto _LL153;} else{ goto _LL154;} _LL154: if(*(( int*) _temp122)
== Cyc_Absyn_AssignOp_e){ _LL289: _temp288=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f1; goto _LL287; _LL287: _temp286=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f2; if( _temp286 == 0){ goto _LL285;}
else{ goto _LL156;} _LL285: _temp284=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f3; goto _LL155;} else{ goto _LL156;} _LL156: if(*(( int*) _temp122)
== Cyc_Absyn_AssignOp_e){ _LL295: _temp294=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f1; goto _LL293; _LL293: _temp292=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f2; goto _LL291; _LL291: _temp290=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f3;
goto _LL157;} else{ goto _LL158;} _LL158: if(*(( int*) _temp122) == Cyc_Absyn_Primop_e){
_LL299: _temp298=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp122)->f1;
goto _LL297; _LL297: _temp296=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp122)->f2; goto _LL159;} else{ goto _LL160;} _LL160: if(*(( int*) _temp122)
== Cyc_Absyn_Cast_e){ _LL303: _temp302=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp122)->f1; goto _LL301; _LL301: _temp300=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp122)->f2; goto _LL161;} else{ goto _LL162;}
_LL162: if(*(( int*) _temp122) == Cyc_Absyn_FnCall_e){ _LL307: _temp306=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp122)->f1; goto _LL305;
_LL305: _temp304=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp122)->f2; goto _LL163;} else{ goto _LL164;} _LL164: if(*(( int*) _temp122)
== Cyc_Absyn_Subscript_e){ _LL311: _temp310=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp122)->f1; goto _LL309; _LL309: _temp308=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp122)->f2;
goto _LL165;} else{ goto _LL166;} _LL166: if(*(( int*) _temp122) == Cyc_Absyn_New_e){
_LL315: _temp314=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp122)->f1; goto _LL313; _LL313: _temp312=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp122)->f2; goto _LL167;} else{ goto _LL168;} _LL168:
if(*(( int*) _temp122) == Cyc_Absyn_Address_e){ _LL317: _temp316=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp122)->f1; goto _LL169;} else{ goto
_LL170;} _LL170: if(*(( int*) _temp122) == Cyc_Absyn_Deref_e){ _LL319: _temp318=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp122)->f1; goto
_LL171;} else{ goto _LL172;} _LL172: if(*(( int*) _temp122) == Cyc_Absyn_StructArrow_e){
_LL323: _temp322=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp122)->f1; goto _LL321; _LL321: _temp320=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp122)->f2; goto _LL173;} else{ goto _LL174;}
_LL174: if(*(( int*) _temp122) == Cyc_Absyn_StructMember_e){ _LL327: _temp326=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp122)->f1;
goto _LL325; _LL325: _temp324=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp122)->f2; goto _LL175;} else{ goto _LL176;} _LL176: if(*(( int*) _temp122)
== Cyc_Absyn_Tuple_e){ _LL329: _temp328=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp122)->f1; goto _LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp122)
== Cyc_Absyn_AnonStruct_e){ _LL333: _temp332=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp122)->f1; goto _LL331; _LL331: _temp330=( struct Cyc_List_List*)(( struct
Cyc_Absyn_AnonStruct_e_struct*) _temp122)->f2; goto _LL179;} else{ goto _LL180;}
_LL180: if(*(( int*) _temp122) == Cyc_Absyn_Struct_e){ _LL341: _temp340=( struct
_tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f1; goto _LL339;
_LL339: _temp338=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp122)->f2; goto _LL337; _LL337: _temp336=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp122)->f3; goto _LL335; _LL335: _temp334=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f4;
goto _LL181;} else{ goto _LL182;} _LL182: if(*(( int*) _temp122) == Cyc_Absyn_Array_e){
_LL343: _temp342=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp122)->f1; goto _LL183;} else{ goto _LL184;} _LL184: if(*(( int*) _temp122)
== Cyc_Absyn_Increment_e){ _LL347: _temp346=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Increment_e_struct*) _temp122)->f1; goto _LL345; _LL345: _temp344=(
void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp122)->f2; goto _LL185;}
else{ goto _LL186;} _LL186: if(*(( int*) _temp122) == Cyc_Absyn_Throw_e){ _LL349:
_temp348=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp122)->f1;
goto _LL187;} else{ goto _LL188;} _LL188: if(*(( int*) _temp122) == Cyc_Absyn_Conditional_e){
_LL355: _temp354=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp122)->f1; goto _LL353; _LL353: _temp352=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp122)->f2; goto _LL351; _LL351: _temp350=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp122)->f3;
goto _LL189;} else{ goto _LL190;} _LL190: if(*(( int*) _temp122) == Cyc_Absyn_StmtExp_e){
_LL357: _temp356=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp122)->f1; goto _LL191;} else{ goto _LL192;} _LL192: if(*(( int*) _temp122)
== Cyc_Absyn_SeqExp_e){ _LL361: _temp360=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp122)->f1; goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp122)->f2; goto _LL193;} else{ goto _LL194;}
_LL194: if(*(( int*) _temp122) == Cyc_Absyn_Comprehension_e){ _LL367: _temp366=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp122)->f1;
goto _LL365; _LL365: _temp364=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp122)->f2; goto _LL363; _LL363: _temp362=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp122)->f3; goto _LL195;} else{ goto
_LL196;} _LL196: if(*(( int*) _temp122) == Cyc_Absyn_Var_e){ _LL371: _temp370=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1; goto _LL369;
_LL369: _temp368=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f2; if(
_temp368 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL197;} else{ goto _LL198;}}
else{ goto _LL198;} _LL198: if(*(( int*) _temp122) == Cyc_Absyn_UnknownId_e){
_LL373: _temp372=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp122)->f1; goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*) _temp122)
== Cyc_Absyn_UnknownCall_e){ _LL377: _temp376=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp122)->f1; goto _LL375; _LL375: _temp374=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp122)->f2;
goto _LL201;} else{ goto _LL202;} _LL202: if(*(( int*) _temp122) == Cyc_Absyn_UnresolvedMem_e){
_LL381: _temp380=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp122)->f1; goto _LL379; _LL379: _temp378=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp122)->f2; goto _LL203;} else{ goto
_LL204;} _LL204: if(*(( int*) _temp122) == Cyc_Absyn_CompoundLit_e){ _LL385:
_temp384=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp122)->f1;
goto _LL383; _LL383: _temp382=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp122)->f2; goto _LL205;} else{ goto _LL206;} _LL206: if(*(( int*) _temp122)
== Cyc_Absyn_Codegen_e){ _LL387: _temp386=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp122)->f1; goto _LL207;} else{ goto _LL208;}
_LL208: if(*(( int*) _temp122) == Cyc_Absyn_Fill_e){ _LL389: _temp388=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp122)->f1; goto _LL209;}
else{ goto _LL123;} _LL125: _temp212= _temp210; goto _LL127; _LL127: _temp216=
_temp212; goto _LL129; _LL129: return Cyc_NewControlFlow_abstract_exp( env,
_temp216); _LL131: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp390=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 47; _temp391;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp392; _temp392.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp394;
_temp394.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp394.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp394;}); _temp392.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp393; _temp393.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp393.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp393;}); _temp392.lvalues= 0; _temp392;}); _LL133: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp395=( char*)"new_control_flow.cyc:268 env.access_path==null";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 47; _temp396;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp397; _temp397.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp399;
_temp399.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp399.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp399;});
_temp397.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp398; _temp398.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp398.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp398;}); _temp397.lvalues= 0; _temp397;}); _LL135: goto _LL137; _LL137: goto
_LL139; _LL139: goto _LL141; _LL141: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL143:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL145:
_temp260= _temp254; goto _LL147; _LL147: _temp266= _temp260; goto _LL149; _LL149: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp266)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp400=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp400->hd=( void*) absop; _temp400->tl= 0; _temp400;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL151: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp401=( char*)"new_control_flow.cyc:298 env.access_path == null";
struct _tagged_string _temp402; _temp402.curr= _temp401; _temp402.base= _temp401;
_temp402.last_plus_one= _temp401 + 49; _temp402;})); if( _temp274 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_tunion_root( env, e,
_temp270);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp274 != 0; _temp274=({ struct Cyc_List_List* _temp403=
_temp274; if( _temp403 == 0){ _throw( Null_Exception);} _temp403->tl;}), ++ i){
struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp411=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp411->hd=( void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp(
e),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp412=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp412[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp413; _temp413.tag= Cyc_CfFlowInfo_TupleF;
_temp413.f1= i; _temp413;}); _temp412;})); _temp411->tl= 0; _temp411;})); struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp410= _temp274; if( _temp410 == 0){
_throw( Null_Exception);} _temp410->hd;})); void* _temp406; void* _temp408;
struct Cyc_NewControlFlow_AbsSynOne _temp404= Cyc_NewControlFlow_meet_absexp(
syn); _LL409: _temp408=( void*) _temp404.inner_exp; goto _LL407; _LL407:
_temp406=( void*) _temp404.assigns; goto _LL405; _LL405: es_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp408, es_exp); useexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp406, useexp);}} return Cyc_NewControlFlow_malloc_and_init(
env, e, es_exp, useexp);} _LL153: Cyc_NewControlFlow_add_malloc_root( env, e,
_temp280); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE( e), 0,
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL155: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp288); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp284); void* _temp416; void*
_temp418; struct Cyc_NewControlFlow_AbsSynOne _temp414= Cyc_NewControlFlow_meet_absexp(
syn1); _LL419: _temp418=( void*) _temp414.inner_exp; goto _LL417; _LL417:
_temp416=( void*) _temp414.assigns; goto _LL415; _LL415: { void* _temp422; void*
_temp424; struct Cyc_NewControlFlow_AbsSynOne _temp420= Cyc_NewControlFlow_meet_absexp(
syn2); _LL425: _temp424=( void*) _temp420.inner_exp; goto _LL423; _LL423:
_temp422=( void*) _temp420.assigns; goto _LL421; _LL421: { void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp418, _temp424), _temp422); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0,
_temp416);}}} _LL157: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp426=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp426->hd=( void*) _temp294; _temp426->tl=({ struct Cyc_List_List* _temp427=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp427->hd=(
void*) _temp290; _temp427->tl= 0; _temp427;}); _temp426;}), 0); _LL159: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp296, 0); _LL161: { int both_leaf=
1;{ void* _temp429=( void*)({ struct Cyc_Core_Opt* _temp428= _temp300->topt; if(
_temp428 == 0){ _throw( Null_Exception);} _temp428->v;}); struct Cyc_List_List*
_temp439; struct Cyc_Absyn_Structdecl** _temp441; struct Cyc_List_List* _temp443;
struct _tuple1* _temp445; struct Cyc_List_List* _temp447; _LL431: if((
unsigned int) _temp429 > 4u?*(( int*) _temp429) == Cyc_Absyn_AnonStructType: 0){
_LL440: _temp439=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp429)->f1; goto _LL432;} else{ goto _LL433;} _LL433: if(( unsigned int)
_temp429 > 4u?*(( int*) _temp429) == Cyc_Absyn_StructType: 0){ _LL446: _temp445=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp429)->f1; goto
_LL444; _LL444: _temp443=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp429)->f2; goto _LL442; _LL442: _temp441=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp429)->f3; goto _LL434;} else{ goto
_LL435;} _LL435: if(( unsigned int) _temp429 > 4u?*(( int*) _temp429) == Cyc_Absyn_TupleType:
0){ _LL448: _temp447=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp429)->f1; goto _LL436;} else{ goto _LL437;} _LL437: goto _LL438; _LL432:
goto _LL434; _LL434: goto _LL436; _LL436: both_leaf= 0; goto _LL430; _LL438:
goto _LL430; _LL430:;}{ void* _temp450=( void*)({ struct Cyc_Core_Opt* _temp449=
e->topt; if( _temp449 == 0){ _throw( Null_Exception);} _temp449->v;}); struct
Cyc_List_List* _temp460; struct Cyc_Absyn_Structdecl** _temp462; struct Cyc_List_List*
_temp464; struct _tuple1* _temp466; struct Cyc_List_List* _temp468; _LL452: if((
unsigned int) _temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_AnonStructType: 0){
_LL461: _temp460=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp450)->f1; goto _LL453;} else{ goto _LL454;} _LL454: if(( unsigned int)
_temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_StructType: 0){ _LL467: _temp466=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp450)->f1; goto
_LL465; _LL465: _temp464=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp450)->f2; goto _LL463; _LL463: _temp462=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp450)->f3; goto _LL455;} else{ goto
_LL456;} _LL456: if(( unsigned int) _temp450 > 4u?*(( int*) _temp450) == Cyc_Absyn_TupleType:
0){ _LL469: _temp468=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp450)->f1; goto _LL457;} else{ goto _LL458;} _LL458: goto _LL459; _LL453:
goto _LL455; _LL455: goto _LL457; _LL457: both_leaf= 0; goto _LL451; _LL459:
goto _LL451; _LL451:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp(
env, _temp300);} return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp470=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp470->hd=( void*) _temp300; _temp470->tl= 0; _temp470;}), env.lhsides);}
_LL163: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp471=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp471->hd=( void*) _temp306; _temp471->tl= _temp304; _temp471;}), 0); _LL165: {
void* _temp473= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp472=
_temp310->topt; if( _temp472 == 0){ _throw( Null_Exception);} _temp472->v;}));
struct Cyc_List_List* _temp479; _LL475: if(( unsigned int) _temp473 > 4u?*(( int*)
_temp473) == Cyc_Absyn_TupleType: 0){ _LL480: _temp479=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp473)->f1; goto _LL476;} else{ goto
_LL477;} _LL477: goto _LL478; _LL476: return Cyc_NewControlFlow_descend_path(
env, e, _temp310); _LL478: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp481=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp481->hd=( void*) _temp310; _temp481->tl=({ struct
Cyc_List_List* _temp482=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp482->hd=( void*) _temp308; _temp482->tl= 0; _temp482;}); _temp481;}), Cyc_NewControlFlow_use_it_list());
_LL474:;} _LL167: { void* _temp483=( void*) _temp312->r; struct Cyc_Absyn_Structdecl*
_temp491; struct Cyc_List_List* _temp493; struct Cyc_Core_Opt* _temp495; struct
_tuple1* _temp497; struct Cyc_List_List* _temp499; _LL485: if(*(( int*) _temp483)
== Cyc_Absyn_Struct_e){ _LL498: _temp497=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp483)->f1; goto _LL496; _LL496: _temp495=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp483)->f2; goto _LL494; _LL494: _temp493=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp483)->f3; goto
_LL492; _LL492: _temp491=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp483)->f4; goto _LL486;} else{ goto _LL487;} _LL487: if(*(( int*) _temp483)
== Cyc_Absyn_Tuple_e){ _LL500: _temp499=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp483)->f1; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL486:
goto _LL488; _LL488: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp501= _temp312->topt; if( _temp501 ==
0){ _throw( Null_Exception);} _temp501->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp540=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp540->hd=(
void*) malloc_op; _temp540->tl= 0; _temp540;})); inner_env.access_path= 0;{ void*
_temp504; void* _temp506; struct Cyc_NewControlFlow_AbsSynOne _temp502= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp312)); _LL507: _temp506=( void*)
_temp502.inner_exp; goto _LL505; _LL505: _temp504=( void*) _temp502.assigns;
goto _LL503; _LL503: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp506, _temp504); if( env.access_path != 0){ void* _temp509=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp508= env.access_path; if(
_temp508 == 0){ _throw( Null_Exception);} _temp508->hd;}))->r; struct Cyc_Absyn_Exp*
_temp519; struct Cyc_Absyn_Exp* _temp521; struct Cyc_Absyn_Exp* _temp523; struct
_tagged_string* _temp525; struct Cyc_Absyn_Exp* _temp527; _LL511: if(*(( int*)
_temp509) == Cyc_Absyn_Subscript_e){ _LL522: _temp521=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp509)->f1; goto _LL520; _LL520:
_temp519=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp509)->f2; goto _LL512;} else{ goto _LL513;} _LL513: if(*(( int*) _temp509)
== Cyc_Absyn_Deref_e){ _LL524: _temp523=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp509)->f1; goto _LL514;} else{ goto _LL515;} _LL515: if(*(( int*) _temp509)
== Cyc_Absyn_StructArrow_e){ _LL528: _temp527=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp509)->f1; goto _LL526; _LL526: _temp525=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp509)->f2;
goto _LL516;} else{ goto _LL517;} _LL517: goto _LL518; _LL512: Cyc_Evexp_eval_const_uint_exp(
_temp519) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp529=( char*)"new_control_flow.cyc:413 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 61; _temp530;})); goto _LL514; _LL514: env.access_path=({
struct Cyc_List_List* _temp531= env.access_path; if( _temp531 == 0){ _throw(
Null_Exception);} _temp531->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp532=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp532->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp532->tl= 0;
_temp532;}); goto _LL510; _LL516: env.access_path=({ struct Cyc_List_List*
_temp533=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp533->hd=( void*) Cyc_Absyn_structmember_exp( _temp527, _temp525, 0);
_temp533->tl=({ struct Cyc_List_List* _temp534= env.access_path; if( _temp534 ==
0){ _throw( Null_Exception);} _temp534->tl;}); _temp533;}); ans.lvalues=({
struct Cyc_List_List* _temp535=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp535->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp535->tl= 0; _temp535;}); goto _LL510; _LL518:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp536=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp536[ 0]=({ struct
Cyc_Core_Impossible_struct _temp537; _temp537.tag= Cyc_Core_Impossible; _temp537.f1=(
struct _tagged_string)({ char* _temp538=( char*)"bad malloc access path"; struct
_tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 23; _temp539;}); _temp537;}); _temp536;}));
_LL510:;} return ans;}}}} _LL490: if( _temp314 == 0){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp541=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp541->hd=( void*) _temp312; _temp541->tl= 0;
_temp541;}), 0);} else{ return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp542=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp542->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp544= _temp314; if( _temp544 == 0){ _throw( Null_Exception);}
_temp544;})); _temp542->tl=({ struct Cyc_List_List* _temp543=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp543->hd=( void*) _temp312;
_temp543->tl= 0; _temp543;}); _temp542;}), 0);} _LL484:;} _LL169:{ void*
_temp545=( void*) _temp316->r; struct Cyc_Absyn_Structdecl* _temp553; struct Cyc_List_List*
_temp555; struct Cyc_Core_Opt* _temp557; struct _tuple1* _temp559; struct Cyc_List_List*
_temp561; _LL547: if(*(( int*) _temp545) == Cyc_Absyn_Struct_e){ _LL560:
_temp559=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp545)->f1;
goto _LL558; _LL558: _temp557=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp545)->f2; goto _LL556; _LL556: _temp555=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp545)->f3; goto _LL554; _LL554: _temp553=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp545)->f4;
goto _LL548;} else{ goto _LL549;} _LL549: if(*(( int*) _temp545) == Cyc_Absyn_Tuple_e){
_LL562: _temp561=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp545)->f1; goto _LL550;} else{ goto _LL551;} _LL551: goto _LL552; _LL548:
goto _LL550; _LL550: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp563= _temp316->topt; if( _temp563 ==
0){ _throw( Null_Exception);} _temp563->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp602=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp602->hd=(
void*) malloc_op; _temp602->tl= 0; _temp602;})); inner_env.access_path= 0;{ void*
_temp566; void* _temp568; struct Cyc_NewControlFlow_AbsSynOne _temp564= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp316)); _LL569: _temp568=( void*)
_temp564.inner_exp; goto _LL567; _LL567: _temp566=( void*) _temp564.assigns;
goto _LL565; _LL565: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp568, _temp566); if( env.access_path != 0){ void* _temp571=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp570= env.access_path; if(
_temp570 == 0){ _throw( Null_Exception);} _temp570->hd;}))->r; struct Cyc_Absyn_Exp*
_temp581; struct Cyc_Absyn_Exp* _temp583; struct Cyc_Absyn_Exp* _temp585; struct
_tagged_string* _temp587; struct Cyc_Absyn_Exp* _temp589; _LL573: if(*(( int*)
_temp571) == Cyc_Absyn_Subscript_e){ _LL584: _temp583=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp571)->f1; goto _LL582; _LL582:
_temp581=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp571)->f2; goto _LL574;} else{ goto _LL575;} _LL575: if(*(( int*) _temp571)
== Cyc_Absyn_Deref_e){ _LL586: _temp585=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp571)->f1; goto _LL576;} else{ goto _LL577;} _LL577: if(*(( int*) _temp571)
== Cyc_Absyn_StructArrow_e){ _LL590: _temp589=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp571)->f1; goto _LL588; _LL588: _temp587=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp571)->f2;
goto _LL578;} else{ goto _LL579;} _LL579: goto _LL580; _LL574: Cyc_Evexp_eval_const_uint_exp(
_temp581) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp591=( char*)"new_control_flow.cyc:453 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp592; _temp592.curr= _temp591; _temp592.base= _temp591;
_temp592.last_plus_one= _temp591 + 61; _temp592;})); goto _LL576; _LL576: env.access_path=({
struct Cyc_List_List* _temp593= env.access_path; if( _temp593 == 0){ _throw(
Null_Exception);} _temp593->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp594=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp594->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp594->tl= 0;
_temp594;}); goto _LL572; _LL578: env.access_path=({ struct Cyc_List_List*
_temp595=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp595->hd=( void*) Cyc_Absyn_structmember_exp( _temp589, _temp587, 0);
_temp595->tl=({ struct Cyc_List_List* _temp596= env.access_path; if( _temp596 ==
0){ _throw( Null_Exception);} _temp596->tl;}); _temp595;}); ans.lvalues=({
struct Cyc_List_List* _temp597=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp597->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp597->tl= 0; _temp597;}); goto _LL572; _LL580:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp598=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp598[ 0]=({ struct
Cyc_Core_Impossible_struct _temp599; _temp599.tag= Cyc_Core_Impossible; _temp599.f1=(
struct _tagged_string)({ char* _temp600=( char*)"bad malloc access path"; struct
_tagged_string _temp601; _temp601.curr= _temp600; _temp601.base= _temp600;
_temp601.last_plus_one= _temp600 + 23; _temp601;}); _temp599;}); _temp598;}));
_LL572:;} return ans;}}}} _LL552: goto _LL546; _LL546:;}{ struct Cyc_List_List*
_temp603= env.access_path; struct Cyc_List_List _temp615; struct Cyc_List_List*
_temp616; struct Cyc_Absyn_Exp* _temp618; struct Cyc_Absyn_Exp _temp620; struct
Cyc_Position_Segment* _temp621; void* _temp623; struct Cyc_Absyn_Exp* _temp625;
struct Cyc_Absyn_Exp* _temp627; struct Cyc_Core_Opt* _temp629; struct Cyc_List_List
_temp631; struct Cyc_List_List* _temp632; struct Cyc_Absyn_Exp* _temp634; struct
Cyc_Absyn_Exp _temp636; struct Cyc_Position_Segment* _temp637; void* _temp639;
struct Cyc_Absyn_Exp* _temp641; struct Cyc_Core_Opt* _temp643; struct Cyc_List_List
_temp645; struct Cyc_List_List* _temp646; struct Cyc_Absyn_Exp* _temp648; struct
Cyc_Absyn_Exp _temp650; struct Cyc_Position_Segment* _temp651; void* _temp653;
struct _tagged_string* _temp655; struct Cyc_Absyn_Exp* _temp657; struct Cyc_Core_Opt*
_temp659; _LL605: if( _temp603 == 0){ goto _LL606;} else{ goto _LL607;} _LL607:
if( _temp603 == 0){ goto _LL609;} else{ _temp615=* _temp603; _LL619: _temp618=(
struct Cyc_Absyn_Exp*) _temp615.hd; _temp620=* _temp618; _LL630: _temp629=(
struct Cyc_Core_Opt*) _temp620.topt; goto _LL624; _LL624: _temp623=( void*)
_temp620.r; if(*(( int*) _temp623) == Cyc_Absyn_Subscript_e){ _LL628: _temp627=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp623)->f1;
goto _LL626; _LL626: _temp625=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp623)->f2; goto _LL622;} else{ goto _LL609;} _LL622: _temp621=( struct Cyc_Position_Segment*)
_temp620.loc; goto _LL617; _LL617: _temp616=( struct Cyc_List_List*) _temp615.tl;
goto _LL608;} _LL609: if( _temp603 == 0){ goto _LL611;} else{ _temp631=*
_temp603; _LL635: _temp634=( struct Cyc_Absyn_Exp*) _temp631.hd; _temp636=*
_temp634; _LL644: _temp643=( struct Cyc_Core_Opt*) _temp636.topt; goto _LL640;
_LL640: _temp639=( void*) _temp636.r; if(*(( int*) _temp639) == Cyc_Absyn_Deref_e){
_LL642: _temp641=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp639)->f1; goto _LL638;} else{ goto _LL611;} _LL638: _temp637=( struct Cyc_Position_Segment*)
_temp636.loc; goto _LL633; _LL633: _temp632=( struct Cyc_List_List*) _temp631.tl;
goto _LL610;} _LL611: if( _temp603 == 0){ goto _LL613;} else{ _temp645=*
_temp603; _LL649: _temp648=( struct Cyc_Absyn_Exp*) _temp645.hd; _temp650=*
_temp648; _LL660: _temp659=( struct Cyc_Core_Opt*) _temp650.topt; goto _LL654;
_LL654: _temp653=( void*) _temp650.r; if(*(( int*) _temp653) == Cyc_Absyn_StructArrow_e){
_LL658: _temp657=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp653)->f1; goto _LL656; _LL656: _temp655=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp653)->f2; goto _LL652;} else{ goto _LL613;}
_LL652: _temp651=( struct Cyc_Position_Segment*) _temp650.loc; goto _LL647;
_LL647: _temp646=( struct Cyc_List_List*) _temp645.tl; goto _LL612;} _LL613:
goto _LL614; _LL606: env.access_path=({ struct Cyc_List_List* _temp661=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp661->hd=( void*)
e; _temp661->tl= env.access_path; _temp661;}); goto _LL604; _LL608: Cyc_Evexp_eval_const_uint_exp(
_temp625) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp662=( char*)"new_control_flow.cyc:481 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 61; _temp663;})); _temp632= _temp616; goto
_LL610; _LL610: env.access_path= _temp632; goto _LL604; _LL612: env.access_path=({
struct Cyc_List_List* _temp664=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp664->hd=( void*) Cyc_Absyn_structmember_exp(
_temp657, _temp655, 0); _temp664->tl= _temp646; _temp664;}); goto _LL604; _LL614:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp665=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp665[ 0]=({ struct
Cyc_Core_Impossible_struct _temp666; _temp666.tag= Cyc_Core_Impossible; _temp666.f1=(
struct _tagged_string)({ char* _temp667=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp668; _temp668.curr= _temp667; _temp668.base= _temp667;
_temp668.last_plus_one= _temp667 + 36; _temp668;}); _temp666;}); _temp665;}));
_LL604:;} return Cyc_NewControlFlow_abstract_exp( env, _temp316); _LL171: {
struct Cyc_List_List* _temp669= env.access_path; struct Cyc_List_List _temp675;
struct Cyc_List_List* _temp676; struct Cyc_Absyn_Exp* _temp678; struct Cyc_Absyn_Exp
_temp680; struct Cyc_Position_Segment* _temp681; void* _temp683; struct Cyc_Absyn_Exp*
_temp685; struct Cyc_Core_Opt* _temp687; _LL671: if( _temp669 == 0){ goto _LL673;}
else{ _temp675=* _temp669; _LL679: _temp678=( struct Cyc_Absyn_Exp*) _temp675.hd;
_temp680=* _temp678; _LL688: _temp687=( struct Cyc_Core_Opt*) _temp680.topt;
goto _LL684; _LL684: _temp683=( void*) _temp680.r; if(*(( int*) _temp683) == Cyc_Absyn_Address_e){
_LL686: _temp685=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp683)->f1; goto _LL682;} else{ goto _LL673;} _LL682: _temp681=( struct Cyc_Position_Segment*)
_temp680.loc; goto _LL677; _LL677: _temp676=( struct Cyc_List_List*) _temp675.tl;
goto _LL672;} _LL673: goto _LL674; _LL672: env.access_path= _temp676;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp318);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL674: return Cyc_NewControlFlow_descend_path( env, e, _temp318); _LL670:;}
_LL173: { void* _temp690= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp689= _temp322->topt; if( _temp689 == 0){ _throw( Null_Exception);} _temp689->v;}));
struct Cyc_Absyn_PtrInfo _temp696; struct Cyc_Absyn_Conref* _temp698; struct Cyc_Absyn_Tqual
_temp700; struct Cyc_Absyn_Conref* _temp702; void* _temp704; void* _temp706;
_LL692: if(( unsigned int) _temp690 > 4u?*(( int*) _temp690) == Cyc_Absyn_PointerType:
0){ _LL697: _temp696=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp690)->f1; _LL707: _temp706=( void*) _temp696.elt_typ; goto _LL705; _LL705:
_temp704=( void*) _temp696.rgn_typ; goto _LL703; _LL703: _temp702=( struct Cyc_Absyn_Conref*)
_temp696.nullable; goto _LL701; _LL701: _temp700=( struct Cyc_Absyn_Tqual)
_temp696.tq; goto _LL699; _LL699: _temp698=( struct Cyc_Absyn_Conref*) _temp696.bounds;
goto _LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL693:{ void* _temp708=
Cyc_Tcutil_compress( _temp706); struct Cyc_List_List* _temp720; struct Cyc_Absyn_Structdecl**
_temp722; struct Cyc_List_List* _temp724; struct _tuple1* _temp726; struct Cyc_List_List*
_temp728; struct Cyc_Absyn_Uniondecl** _temp730; struct Cyc_List_List* _temp732;
struct _tuple1* _temp734; _LL710: if(( unsigned int) _temp708 > 4u?*(( int*)
_temp708) == Cyc_Absyn_AnonStructType: 0){ _LL721: _temp720=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp708)->f1; goto _LL711;} else{ goto
_LL712;} _LL712: if(( unsigned int) _temp708 > 4u?*(( int*) _temp708) == Cyc_Absyn_StructType:
0){ _LL727: _temp726=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp708)->f1; goto _LL725; _LL725: _temp724=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp708)->f2; goto _LL723; _LL723: _temp722=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp708)->f3;
goto _LL713;} else{ goto _LL714;} _LL714: if(( unsigned int) _temp708 > 4u?*((
int*) _temp708) == Cyc_Absyn_AnonUnionType: 0){ _LL729: _temp728=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp708)->f1; goto _LL715;} else{ goto
_LL716;} _LL716: if(( unsigned int) _temp708 > 4u?*(( int*) _temp708) == Cyc_Absyn_UnionType:
0){ _LL735: _temp734=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp708)->f1; goto _LL733; _LL733: _temp732=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp708)->f2; goto _LL731; _LL731: _temp730=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp708)->f3;
goto _LL717;} else{ goto _LL718;} _LL718: goto _LL719; _LL711: goto _LL713;
_LL713: return Cyc_NewControlFlow_descend_path( env, e, _temp322); _LL715: goto
_LL717; _LL717: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp322); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL719: goto _LL709; _LL709:;} goto _LL695; _LL695:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp736=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp736[ 0]=({ struct
Cyc_Core_Impossible_struct _temp737; _temp737.tag= Cyc_Core_Impossible; _temp737.f1=(
struct _tagged_string)({ char* _temp738=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp739; _temp739.curr= _temp738; _temp739.base= _temp738;
_temp739.last_plus_one= _temp738 + 40; _temp739;}); _temp737;}); _temp736;}));
_LL691:;} _LL175: { void* _temp741= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp740= _temp326->topt; if( _temp740 == 0){ _throw( Null_Exception);} _temp740->v;}));
struct Cyc_List_List* _temp753; struct Cyc_Absyn_Structdecl** _temp755; struct
Cyc_List_List* _temp757; struct _tuple1* _temp759; struct Cyc_List_List*
_temp761; struct Cyc_Absyn_Uniondecl** _temp763; struct Cyc_List_List* _temp765;
struct _tuple1* _temp767; _LL743: if(( unsigned int) _temp741 > 4u?*(( int*)
_temp741) == Cyc_Absyn_AnonStructType: 0){ _LL754: _temp753=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp741)->f1; goto _LL744;} else{ goto
_LL745;} _LL745: if(( unsigned int) _temp741 > 4u?*(( int*) _temp741) == Cyc_Absyn_StructType:
0){ _LL760: _temp759=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp741)->f1; goto _LL758; _LL758: _temp757=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp741)->f2; goto _LL756; _LL756: _temp755=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp741)->f3;
goto _LL746;} else{ goto _LL747;} _LL747: if(( unsigned int) _temp741 > 4u?*((
int*) _temp741) == Cyc_Absyn_AnonUnionType: 0){ _LL762: _temp761=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp741)->f1; goto _LL748;} else{ goto
_LL749;} _LL749: if(( unsigned int) _temp741 > 4u?*(( int*) _temp741) == Cyc_Absyn_UnionType:
0){ _LL768: _temp767=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp741)->f1; goto _LL766; _LL766: _temp765=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp741)->f2; goto _LL764; _LL764: _temp763=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp741)->f3;
goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL744: goto _LL746;
_LL746: return Cyc_NewControlFlow_descend_path( env, e, _temp326); _LL748: goto
_LL750; _LL750: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp326); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL752:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp769=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp769[ 0]=({ struct Cyc_Core_Impossible_struct _temp770; _temp770.tag= Cyc_Core_Impossible;
_temp770.f1=( struct _tagged_string)({ char* _temp771=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp772; _temp772.curr= _temp771; _temp772.base= _temp771;
_temp772.last_plus_one= _temp771 + 41; _temp772;}); _temp770;}); _temp769;}));
_LL742:;} _LL177: { struct Cyc_List_List* _temp773= env.access_path; struct Cyc_List_List
_temp781; struct Cyc_List_List* _temp782; struct Cyc_Absyn_Exp* _temp784; struct
Cyc_Absyn_Exp _temp786; struct Cyc_Position_Segment* _temp787; void* _temp789;
struct Cyc_Absyn_Exp* _temp791; struct Cyc_Absyn_Exp* _temp793; struct Cyc_Core_Opt*
_temp795; _LL775: if( _temp773 == 0){ goto _LL776;} else{ goto _LL777;} _LL777:
if( _temp773 == 0){ goto _LL779;} else{ _temp781=* _temp773; _LL785: _temp784=(
struct Cyc_Absyn_Exp*) _temp781.hd; _temp786=* _temp784; _LL796: _temp795=(
struct Cyc_Core_Opt*) _temp786.topt; goto _LL790; _LL790: _temp789=( void*)
_temp786.r; if(*(( int*) _temp789) == Cyc_Absyn_Subscript_e){ _LL794: _temp793=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp789)->f1;
goto _LL792; _LL792: _temp791=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp789)->f2; goto _LL788;} else{ goto _LL779;} _LL788: _temp787=( struct Cyc_Position_Segment*)
_temp786.loc; goto _LL783; _LL783: _temp782=( struct Cyc_List_List*) _temp781.tl;
goto _LL778;} _LL779: goto _LL780; _LL776: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp328 != 0;
_temp328=({ struct Cyc_List_List* _temp797= _temp328; if( _temp797 == 0){ _throw(
Null_Exception);} _temp797->tl;}), ++ j){ struct Cyc_List_List* ith_lvals= 0;{
struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List*
_temp798= ls; if( _temp798 == 0){ _throw( Null_Exception);} _temp798->tl;})){
if( Cyc_CfAbsexp_isUnknownOp(( void*)({ struct Cyc_List_List* _temp799= ls; if(
_temp799 == 0){ _throw( Null_Exception);} _temp799->hd;}))){ ith_lvals=({ struct
Cyc_List_List* _temp800=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp800->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp800->tl= ith_lvals;
_temp800;});} else{ ith_lvals=({ struct Cyc_List_List* _temp801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp801->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp802= ls; if( _temp802 == 0){ _throw(
Null_Exception);} _temp802->hd;}),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp803=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp803[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp804; _temp804.tag= Cyc_CfFlowInfo_TupleF;
_temp804.f1= j; _temp804;}); _temp803;})); _temp801->tl= ith_lvals; _temp801;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp811=
_temp328; if( _temp811 == 0){ _throw( Null_Exception);} _temp811->hd;})); void*
_temp807; void* _temp809; struct Cyc_NewControlFlow_AbsSynOne _temp805= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL810: _temp809=( void*) _temp805.inner_exp; goto _LL808; _LL808:
_temp807=( void*) _temp805.assigns; goto _LL806; _LL806: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp809); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp807);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL778: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp791); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp782;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp328 != 0;
_temp328=({ struct Cyc_List_List* _temp812= _temp328; if( _temp812 == 0){ _throw(
Null_Exception);} _temp812->tl;}), ++ j){ if( i == j){ void* _temp816; void*
_temp818; struct Cyc_NewControlFlow_AbsSynOne _temp814= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp813= _temp328; if( _temp813 == 0){ _throw( Null_Exception);} _temp813->hd;})));
_LL819: _temp818=( void*) _temp814.inner_exp; goto _LL817; _LL817: _temp816=(
void*) _temp814.assigns; goto _LL815; _LL815: assignexp= _temp816; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp818);} else{ void* _temp823; void*
_temp825; struct Cyc_NewControlFlow_AbsSynOne _temp821= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp820= _temp328; if( _temp820 == 0){ _throw( Null_Exception);} _temp820->hd;})));
_LL826: _temp825=( void*) _temp821.inner_exp; goto _LL824; _LL824: _temp823=(
void*) _temp821.assigns; goto _LL822; _LL822: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp825);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp827=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp827->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp827->tl= 0; _temp827;}), assignexp);}} _LL780:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp828=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp828[ 0]=({ struct
Cyc_Core_Impossible_struct _temp829; _temp829.tag= Cyc_Core_Impossible; _temp829.f1=(
struct _tagged_string)({ char* _temp830=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 46; _temp831;}); _temp829;}); _temp828;}));
_LL774:;} _LL179: _temp336= _temp330; goto _LL181; _LL181: { struct Cyc_List_List*
_temp832= env.access_path; struct Cyc_List_List _temp840; struct Cyc_List_List*
_temp841; struct Cyc_Absyn_Exp* _temp843; struct Cyc_Absyn_Exp _temp845; struct
Cyc_Position_Segment* _temp846; void* _temp848; struct _tagged_string* _temp850;
struct Cyc_Absyn_Exp* _temp852; struct Cyc_Core_Opt* _temp854; _LL834: if(
_temp832 == 0){ goto _LL835;} else{ goto _LL836;} _LL836: if( _temp832 == 0){
goto _LL838;} else{ _temp840=* _temp832; _LL844: _temp843=( struct Cyc_Absyn_Exp*)
_temp840.hd; _temp845=* _temp843; _LL855: _temp854=( struct Cyc_Core_Opt*)
_temp845.topt; goto _LL849; _LL849: _temp848=( void*) _temp845.r; if(*(( int*)
_temp848) == Cyc_Absyn_StructMember_e){ _LL853: _temp852=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp848)->f1; goto _LL851; _LL851:
_temp850=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp848)->f2; goto _LL847;} else{ goto _LL838;} _LL847: _temp846=( struct Cyc_Position_Segment*)
_temp845.loc; goto _LL842; _LL842: _temp841=( struct Cyc_List_List*) _temp840.tl;
goto _LL837;} _LL838: goto _LL839; _LL835: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp336 != 0; _temp336=({
struct Cyc_List_List* _temp856= _temp336; if( _temp856 == 0){ _throw(
Null_Exception);} _temp856->tl;})){ struct Cyc_List_List* ith_lvals= 0;{ struct
Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List*
_temp857= ls; if( _temp857 == 0){ _throw( Null_Exception);} _temp857->tl;})){
if( Cyc_CfAbsexp_isUnknownOp(( void*)({ struct Cyc_List_List* _temp858= ls; if(
_temp858 == 0){ _throw( Null_Exception);} _temp858->hd;}))){ ith_lvals=({ struct
Cyc_List_List* _temp859=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp859->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp859->tl= ith_lvals;
_temp859;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp880= _temp336; if( _temp880 == 0){ _throw( Null_Exception);} _temp880->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp860= ds; if( _temp860 == 0){
_throw( Null_Exception);} _temp860->tl;})){ void* _temp862=( void*)({ struct Cyc_List_List*
_temp861= ds; if( _temp861 == 0){ _throw( Null_Exception);} _temp861->hd;});
struct Cyc_Absyn_Exp* _temp868; struct _tagged_string* _temp870; _LL864: if(*((
int*) _temp862) == Cyc_Absyn_ArrayElement){ _LL869: _temp868=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp862)->f1; goto _LL865;} else{ goto
_LL866;} _LL866: if(*(( int*) _temp862) == Cyc_Absyn_FieldName){ _LL871:
_temp870=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp862)->f1; goto _LL867;} else{ goto _LL863;} _LL865:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp872=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp872[ 0]=({ struct
Cyc_Core_Impossible_struct _temp873; _temp873.tag= Cyc_Core_Impossible; _temp873.f1=(
struct _tagged_string)({ char* _temp874=( char*)"bad struct designator"; struct
_tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 22; _temp875;}); _temp873;}); _temp872;}));
_LL867: ith_lvals=({ struct Cyc_List_List* _temp876=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp876->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp877= ls; if( _temp877 == 0){ _throw(
Null_Exception);} _temp877->hd;}),( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp878=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp878[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp879; _temp879.tag= Cyc_CfFlowInfo_StructF;
_temp879.f1= _temp870; _temp879;}); _temp878;})); _temp876->tl= ith_lvals;
_temp876;}); goto _LL863; _LL863:;}}}}{ void** ith_topt=( void**)(( void*)({
struct Cyc_Core_Opt* _temp889=((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp888= _temp336; if( _temp888 == 0){ _throw( Null_Exception);} _temp888->hd;}))).f2)->topt;
if( _temp889 == 0){ _throw( Null_Exception);}& _temp889->v;})); struct Cyc_NewControlFlow_AbsSyn
ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
ith_lvals),(*(( struct _tuple5*)({ struct Cyc_List_List* _temp887= _temp336; if(
_temp887 == 0){ _throw( Null_Exception);} _temp887->hd;}))).f2); void* _temp883;
void* _temp885; struct Cyc_NewControlFlow_AbsSynOne _temp881= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL886: _temp885=( void*) _temp881.inner_exp; goto _LL884; _LL884:
_temp883=( void*) _temp881.assigns; goto _LL882; _LL882: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp885); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp883);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL837: { struct Cyc_NewControlFlow_AbsEnv other_env=
Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp841;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp336 != 0; _temp336=({ struct Cyc_List_List* _temp890= _temp336; if(
_temp890 == 0){ _throw( Null_Exception);} _temp890->tl;})){ int used= 0;{ struct
Cyc_List_List* ds=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp907=
_temp336; if( _temp907 == 0){ _throw( Null_Exception);} _temp907->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp891= ds; if( _temp891 == 0){
_throw( Null_Exception);} _temp891->tl;})){ void* _temp893=( void*)({ struct Cyc_List_List*
_temp892= ds; if( _temp892 == 0){ _throw( Null_Exception);} _temp892->hd;});
struct Cyc_Absyn_Exp* _temp899; struct _tagged_string* _temp901; _LL895: if(*((
int*) _temp893) == Cyc_Absyn_ArrayElement){ _LL900: _temp899=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp893)->f1; goto _LL896;} else{ goto
_LL897;} _LL897: if(*(( int*) _temp893) == Cyc_Absyn_FieldName){ _LL902:
_temp901=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp893)->f1; goto _LL898;} else{ goto _LL894;} _LL896:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp903=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp903[ 0]=({ struct
Cyc_Core_Impossible_struct _temp904; _temp904.tag= Cyc_Core_Impossible; _temp904.f1=(
struct _tagged_string)({ char* _temp905=( char*)"bad struct designator"; struct
_tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 22; _temp906;}); _temp904;}); _temp903;}));
_LL898: if( Cyc_String_zstrptrcmp( _temp850, _temp901) == 0){ used= 1;} goto
_LL894; _LL894:;}} if( used){ void* _temp911; void* _temp913; struct Cyc_NewControlFlow_AbsSynOne
_temp909= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp908= _temp336; if( _temp908 == 0){
_throw( Null_Exception);} _temp908->hd;}))).f2)); _LL914: _temp913=( void*)
_temp909.inner_exp; goto _LL912; _LL912: _temp911=( void*) _temp909.assigns;
goto _LL910; _LL910: assignexp= _temp911; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, ansexp, _temp913);} else{ void* _temp918; void*
_temp920; struct Cyc_NewControlFlow_AbsSynOne _temp916= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp915= _temp336; if( _temp915 == 0){ _throw( Null_Exception);} _temp915->hd;}))).f2));
_LL921: _temp920=( void*) _temp916.inner_exp; goto _LL919; _LL919: _temp918=(
void*) _temp916.assigns; goto _LL917; _LL917: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp920);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);}} _LL839:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp922=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp922[ 0]=({ struct Cyc_Core_Impossible_struct _temp923; _temp923.tag= Cyc_Core_Impossible;
_temp923.f1=( struct _tagged_string)({ char* _temp924=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp925; _temp925.curr= _temp924; _temp925.base= _temp924;
_temp925.last_plus_one= _temp924 + 47; _temp925;}); _temp923;}); _temp922;}));
_LL833:;} _LL183: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp342), 0); _LL185:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp926=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp926->hd=( void*) _temp346; _temp926->tl= 0; _temp926;}), 0); _LL187: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp348); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL189: { struct Cyc_List_List*
_temp929; struct Cyc_NewControlFlow_AbsSynOne _temp931; struct Cyc_NewControlFlow_AbsSynOne
_temp933; struct Cyc_NewControlFlow_AbsSyn _temp927= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp354); _LL934: _temp933=( struct Cyc_NewControlFlow_AbsSynOne)
_temp927.when_true; goto _LL932; _LL932: _temp931=( struct Cyc_NewControlFlow_AbsSynOne)
_temp927.when_false; goto _LL930; _LL930: _temp929=( struct Cyc_List_List*)
_temp927.lvalues; goto _LL928; _LL928: { struct Cyc_List_List* _temp937; struct
Cyc_NewControlFlow_AbsSynOne _temp939; struct Cyc_NewControlFlow_AbsSynOne
_temp941; struct Cyc_NewControlFlow_AbsSyn _temp935= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp352); _LL942: _temp941=( struct Cyc_NewControlFlow_AbsSynOne)
_temp935.when_true; goto _LL940; _LL940: _temp939=( struct Cyc_NewControlFlow_AbsSynOne)
_temp935.when_false; goto _LL938; _LL938: _temp937=( struct Cyc_List_List*)
_temp935.lvalues; goto _LL936; _LL936: { struct Cyc_List_List* _temp945; struct
Cyc_NewControlFlow_AbsSynOne _temp947; struct Cyc_NewControlFlow_AbsSynOne
_temp949; struct Cyc_NewControlFlow_AbsSyn _temp943= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp350); _LL950: _temp949=( struct Cyc_NewControlFlow_AbsSynOne)
_temp943.when_true; goto _LL948; _LL948: _temp947=( struct Cyc_NewControlFlow_AbsSynOne)
_temp943.when_false; goto _LL946; _LL946: _temp945=( struct Cyc_List_List*)
_temp943.lvalues; goto _LL944; _LL944: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp933.inner_exp,(
void*) _temp933.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp941.inner_exp,( void*) _temp941.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp931.inner_exp,( void*) _temp931.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp949.inner_exp,( void*) _temp949.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp933.inner_exp,( void*) _temp933.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp939.inner_exp,( void*) _temp939.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp931.inner_exp,( void*) _temp931.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp947.inner_exp,(
void*) _temp947.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp951;
_temp951.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp953; _temp953.inner_exp=(
void*) true_exp; _temp953.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp953;}); _temp951.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp952; _temp952.inner_exp=( void*) false_exp; _temp952.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp952;}); _temp951.lvalues= 0; _temp951;});}}}}
_LL191: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp356);{
struct Cyc_Absyn_Stmt* last_s= _temp356; while( 1) { void* _temp954=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp964; struct Cyc_Absyn_Stmt* _temp966;
struct Cyc_Absyn_Stmt* _temp968; struct Cyc_Absyn_Decl* _temp970; struct Cyc_Absyn_Exp*
_temp972; _LL956: if(( unsigned int) _temp954 > 1u?*(( int*) _temp954) == Cyc_Absyn_Seq_s:
0){ _LL967: _temp966=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp954)->f1; goto _LL965; _LL965: _temp964=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp954)->f2; goto _LL957;} else{ goto _LL958;} _LL958:
if(( unsigned int) _temp954 > 1u?*(( int*) _temp954) == Cyc_Absyn_Decl_s: 0){
_LL971: _temp970=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp954)->f1; goto _LL969; _LL969: _temp968=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp954)->f2; goto _LL959;} else{ goto _LL960;}
_LL960: if(( unsigned int) _temp954 > 1u?*(( int*) _temp954) == Cyc_Absyn_Exp_s:
0){ _LL973: _temp972=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp954)->f1; goto _LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL957:
last_s= _temp964; continue; _LL959: last_s= _temp968; continue; _LL961: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp972); void* _temp976; void* _temp978; struct Cyc_NewControlFlow_AbsSynOne
_temp974= Cyc_NewControlFlow_meet_absexp( syn); _LL979: _temp978=( void*)
_temp974.inner_exp; goto _LL977; _LL977: _temp976=( void*) _temp974.assigns;
goto _LL975; _LL975:( void*)({ struct Cyc_List_List* _temp980=( Cyc_NewControlFlow_get_stmt_annot(
last_s))->absexps; if( _temp980 == 0){ _throw( Null_Exception);} _temp980->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp978, _temp976);});
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp356), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL963:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp981=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp981[ 0]=({ struct Cyc_Core_Impossible_struct _temp982; _temp982.tag= Cyc_Core_Impossible;
_temp982.f1=( struct _tagged_string)({ char* _temp983=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp984; _temp984.curr= _temp983; _temp984.base= _temp983;
_temp984.last_plus_one= _temp983 + 33; _temp984;}); _temp982;}); _temp981;}));
_LL955:;}}} _LL193: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp360); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp358); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp985; _temp985.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp987; _temp987.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp987.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp987;});
_temp985.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp986; _temp986.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp986.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp986;}); _temp985.lvalues= 0; _temp985;});}} _LL195: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp988=( char*)"new_control_flow.cyc:757 env.access_path==null";
struct _tagged_string _temp989; _temp989.curr= _temp988; _temp989.base= _temp988;
_temp989.last_plus_one= _temp988 + 47; _temp989;})); Cyc_NewControlFlow_add_var_root(
env, _temp366);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp990=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp990->hd=( void*) _temp366; _temp990->tl=( env.shared)->comprehension_vars;
_temp990;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp993; void* _temp995; struct Cyc_NewControlFlow_AbsSynOne
_temp991= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp364)); _LL996: _temp995=( void*) _temp991.inner_exp; goto _LL994;
_LL994: _temp993=( void*) _temp991.assigns; goto _LL992; _LL992: { void*
_temp999; void* _temp1001; struct Cyc_NewControlFlow_AbsSynOne _temp997= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp362)); _LL1002: _temp1001=( void*)
_temp997.inner_exp; goto _LL1000; _LL1000: _temp999=( void*) _temp997.assigns;
goto _LL998; _LL998: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp995, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp993, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp366), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1001, _temp999), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL197: goto _LL199; _LL199: goto _LL201;
_LL201: goto _LL203; _LL203: goto _LL205; _LL205: goto _LL207; _LL207: goto
_LL209; _LL209:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1003=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1003[ 0]=({ struct Cyc_Core_Impossible_struct _temp1004; _temp1004.tag= Cyc_Core_Impossible;
_temp1004.f1=( struct _tagged_string)({ char* _temp1005=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp1006; _temp1006.curr= _temp1005; _temp1006.base=
_temp1005; _temp1006.last_plus_one= _temp1005 + 34; _temp1006;}); _temp1004;});
_temp1003;})); _LL123:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp1009; void* _temp1011; struct Cyc_NewControlFlow_AbsSynOne
_temp1007= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL1012: _temp1011=( void*) _temp1007.inner_exp; goto _LL1010; _LL1010:
_temp1009=( void*) _temp1007.assigns; goto _LL1008; _LL1008: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp1011, _temp1009);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp1013; _temp1013.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp1013.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)(
syn.when_false).inner_exp,( void*)( syn.when_false).assigns); _temp1013;});}
struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_List_List* scs){ struct Cyc_List_List* ans= 0; for( 0; scs != 0;
scs=({ struct Cyc_List_List* _temp1014= scs; if( _temp1014 == 0){ _throw(
Null_Exception);} _temp1014->tl;})){ struct Cyc_Absyn_Switch_clause _temp1018;
struct Cyc_Position_Segment* _temp1019; struct Cyc_Absyn_Stmt* _temp1021; struct
Cyc_Absyn_Exp* _temp1023; struct Cyc_Core_Opt* _temp1025; struct Cyc_Absyn_Pat*
_temp1027; struct Cyc_Absyn_Switch_clause* _temp1016=( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp1015= scs; if( _temp1015 == 0){ _throw(
Null_Exception);} _temp1015->hd;}); _temp1018=* _temp1016; _LL1028: _temp1027=(
struct Cyc_Absyn_Pat*) _temp1018.pattern; goto _LL1026; _LL1026: _temp1025=(
struct Cyc_Core_Opt*) _temp1018.pat_vars; goto _LL1024; _LL1024: _temp1023=(
struct Cyc_Absyn_Exp*) _temp1018.where_clause; goto _LL1022; _LL1022: _temp1021=(
struct Cyc_Absyn_Stmt*) _temp1018.body; goto _LL1020; _LL1020: _temp1019=(
struct Cyc_Position_Segment*) _temp1018.loc; goto _LL1017; _LL1017: if(
_temp1025 == 0){( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1029=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1029[ 0]=({ struct Cyc_Core_Impossible_struct _temp1030; _temp1030.tag= Cyc_Core_Impossible;
_temp1030.f1=( struct _tagged_string)({ char* _temp1031=( char*)"switch clause vds not set";
struct _tagged_string _temp1032; _temp1032.curr= _temp1031; _temp1032.base=
_temp1031; _temp1032.last_plus_one= _temp1031 + 26; _temp1032;}); _temp1030;});
_temp1029;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1035= _temp1025; if( _temp1035 == 0){ _throw( Null_Exception);} _temp1035->v;});
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1033= vds; if( _temp1033 ==
0){ _throw( Null_Exception);} _temp1033->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1034= vds; if(
_temp1034 == 0){ _throw( Null_Exception);} _temp1034->hd;}));}} if( _temp1023 !=
0){ void* _temp1039; void* _temp1041; struct _tuple0 _temp1037= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1036= _temp1023; if(
_temp1036 == 0){ _throw( Null_Exception);} _temp1036;})); _LL1042: _temp1041=
_temp1037.f1; goto _LL1040; _LL1040: _temp1039= _temp1037.f2; goto _LL1038;
_LL1038: ans=({ struct Cyc_List_List* _temp1043=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1043->hd=( void*) _temp1041;
_temp1043->tl=({ struct Cyc_List_List* _temp1044=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1044->hd=( void*) _temp1039;
_temp1044->tl= ans; _temp1044;}); _temp1043;});} Cyc_NewControlFlow_cf_prepass(
env, _temp1021);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp1045=( char*)"new_control_flow.cyc:809 env.lhsides == null"; struct
_tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base= _temp1045;
_temp1046.last_plus_one= _temp1045 + 45; _temp1046;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1047=( char*)"new_control_flow.cyc:810 env.access_path == null";
struct _tagged_string _temp1048; _temp1048.curr= _temp1047; _temp1048.base=
_temp1047; _temp1048.last_plus_one= _temp1047 + 49; _temp1048;}));{ void*
_temp1049=( void*) s->r; struct Cyc_Absyn_Exp* _temp1097; struct Cyc_Absyn_Stmt*
_temp1099; struct Cyc_Absyn_Stmt* _temp1101; struct Cyc_Absyn_Exp* _temp1103;
struct Cyc_Absyn_Exp* _temp1105; struct Cyc_Absyn_Stmt* _temp1107; struct Cyc_Absyn_Stmt*
_temp1109; struct Cyc_Absyn_Exp* _temp1111; struct _tuple3 _temp1113; struct Cyc_Absyn_Stmt*
_temp1115; struct Cyc_Absyn_Exp* _temp1117; struct Cyc_Absyn_Stmt* _temp1119;
struct Cyc_Absyn_Stmt* _temp1121; struct _tuple3 _temp1123; struct Cyc_Absyn_Stmt*
_temp1125; struct Cyc_Absyn_Exp* _temp1127; struct Cyc_Absyn_Stmt* _temp1129;
struct Cyc_Absyn_Stmt* _temp1131; struct Cyc_Absyn_Stmt* _temp1133; struct
_tagged_string* _temp1135; struct Cyc_Absyn_Stmt* _temp1137; struct
_tagged_string* _temp1139; struct Cyc_Absyn_Stmt* _temp1141; struct _tuple3
_temp1143; struct Cyc_Absyn_Stmt* _temp1145; struct Cyc_Absyn_Exp* _temp1147;
struct _tuple3 _temp1149; struct Cyc_Absyn_Stmt* _temp1151; struct Cyc_Absyn_Exp*
_temp1153; struct Cyc_Absyn_Exp* _temp1155; struct Cyc_List_List* _temp1157;
struct Cyc_Absyn_Exp* _temp1159; struct Cyc_Absyn_Switch_clause** _temp1161;
struct Cyc_List_List* _temp1163; struct Cyc_Absyn_Stmt* _temp1165; struct Cyc_Absyn_Decl*
_temp1167; struct Cyc_Absyn_Decl _temp1169; struct Cyc_Position_Segment*
_temp1170; void* _temp1172; struct Cyc_Absyn_Vardecl* _temp1174; struct Cyc_Absyn_Stmt*
_temp1176; struct Cyc_Absyn_Decl* _temp1178; struct Cyc_Absyn_Decl _temp1180;
struct Cyc_Position_Segment* _temp1181; void* _temp1183; int _temp1185; struct
Cyc_Absyn_Exp* _temp1187; struct Cyc_Core_Opt* _temp1189; struct Cyc_Core_Opt*
_temp1191; struct Cyc_Core_Opt _temp1193; struct Cyc_List_List* _temp1194;
struct Cyc_Absyn_Pat* _temp1196; struct Cyc_Absyn_Stmt* _temp1198; struct Cyc_Absyn_Decl*
_temp1200; struct Cyc_Absyn_Stmt* _temp1202; struct Cyc_Absyn_Stmt* _temp1204;
struct Cyc_Absyn_Stmt* _temp1206; struct _tagged_string* _temp1208; struct Cyc_List_List*
_temp1210; struct Cyc_Absyn_Stmt* _temp1212; struct Cyc_Absyn_Stmt* _temp1214;
struct Cyc_Absyn_Vardecl* _temp1216; struct Cyc_Absyn_Tvar* _temp1218; _LL1051:
if( _temp1049 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1052;} else{ goto _LL1053;}
_LL1053: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Exp_s:
0){ _LL1098: _temp1097=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1049)->f1; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Seq_s: 0){ _LL1102: _temp1101=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1049)->f1; goto
_LL1100; _LL1100: _temp1099=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1049)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Return_s: 0){ _LL1104:
_temp1103=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1049)->f1; if( _temp1103 == 0){ goto _LL1058;} else{ goto _LL1059;}} else{
goto _LL1059;} _LL1059: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049)
== Cyc_Absyn_Return_s: 0){ _LL1106: _temp1105=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1049)->f1; goto _LL1060;} else{ goto _LL1061;}
_LL1061: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_IfThenElse_s:
0){ _LL1112: _temp1111=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1049)->f1; goto _LL1110; _LL1110: _temp1109=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1049)->f2; goto _LL1108; _LL1108:
_temp1107=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1049)->f3; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Do_s: 0){ _LL1120: _temp1119=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1049)->f1; goto
_LL1114; _LL1114: _temp1113=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1049)->f2; _LL1118: _temp1117= _temp1113.f1; goto _LL1116; _LL1116:
_temp1115= _temp1113.f2; goto _LL1064;} else{ goto _LL1065;} _LL1065: if((
unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_While_s: 0){
_LL1124: _temp1123=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1049)->f1; _LL1128: _temp1127= _temp1123.f1; goto _LL1126; _LL1126:
_temp1125= _temp1123.f2; goto _LL1122; _LL1122: _temp1121=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1049)->f2; goto _LL1066;} else{ goto
_LL1067;} _LL1067: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Break_s:
0){ _LL1130: _temp1129=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1049)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Continue_s: 0){ _LL1132:
_temp1131=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1049)->f1; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Goto_s: 0){ _LL1136: _temp1135=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1049)->f1; goto
_LL1134; _LL1134: _temp1133=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1049)->f2; if( _temp1133 == 0){ goto _LL1072;} else{ goto _LL1073;}} else{
goto _LL1073;} _LL1073: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049)
== Cyc_Absyn_Goto_s: 0){ _LL1140: _temp1139=( struct _tagged_string*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp1049)->f1; goto _LL1138; _LL1138: _temp1137=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1049)->f2; goto
_LL1074;} else{ goto _LL1075;} _LL1075: if(( unsigned int) _temp1049 > 1u?*((
int*) _temp1049) == Cyc_Absyn_For_s: 0){ _LL1156: _temp1155=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp1049)->f1; goto _LL1150; _LL1150: _temp1149=(
struct _tuple3)(( struct Cyc_Absyn_For_s_struct*) _temp1049)->f2; _LL1154:
_temp1153= _temp1149.f1; goto _LL1152; _LL1152: _temp1151= _temp1149.f2; goto
_LL1144; _LL1144: _temp1143=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1049)->f3; _LL1148: _temp1147= _temp1143.f1; goto _LL1146; _LL1146:
_temp1145= _temp1143.f2; goto _LL1142; _LL1142: _temp1141=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_For_s_struct*) _temp1049)->f4; goto _LL1076;} else{ goto
_LL1077;} _LL1077: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Switch_s:
0){ _LL1160: _temp1159=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1049)->f1; goto _LL1158; _LL1158: _temp1157=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1049)->f2; goto _LL1078;} else{ goto
_LL1079;} _LL1079: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Fallthru_s:
0){ _LL1164: _temp1163=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1049)->f1; goto _LL1162; _LL1162: _temp1161=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1049)->f2; goto _LL1080;} else{ goto
_LL1081;} _LL1081: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Decl_s:
0){ _LL1168: _temp1167=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1049)->f1; _temp1169=* _temp1167; _LL1173: _temp1172=( void*) _temp1169.r;
if(*(( int*) _temp1172) == Cyc_Absyn_Var_d){ _LL1175: _temp1174=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1172)->f1; goto _LL1171;} else{ goto
_LL1083;} _LL1171: _temp1170=( struct Cyc_Position_Segment*) _temp1169.loc; goto
_LL1166; _LL1166: _temp1165=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1049)->f2; goto _LL1082;} else{ goto _LL1083;} _LL1083: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Decl_s: 0){ _LL1179: _temp1178=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1049)->f1;
_temp1180=* _temp1178; _LL1184: _temp1183=( void*) _temp1180.r; if(*(( int*)
_temp1183) == Cyc_Absyn_Let_d){ _LL1197: _temp1196=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1183)->f1; goto _LL1192; _LL1192: _temp1191=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1183)->f2; if(
_temp1191 == 0){ goto _LL1085;} else{ _temp1193=* _temp1191; _LL1195: _temp1194=(
struct Cyc_List_List*) _temp1193.v; goto _LL1190;} _LL1190: _temp1189=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1183)->f3; goto _LL1188;
_LL1188: _temp1187=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1183)->f4; goto _LL1186; _LL1186: _temp1185=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1183)->f5; goto _LL1182;} else{ goto _LL1085;} _LL1182: _temp1181=( struct
Cyc_Position_Segment*) _temp1180.loc; goto _LL1177; _LL1177: _temp1176=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1049)->f2; goto _LL1084;}
else{ goto _LL1085;} _LL1085: if(( unsigned int) _temp1049 > 1u?*(( int*)
_temp1049) == Cyc_Absyn_Decl_s: 0){ _LL1201: _temp1200=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1049)->f1; goto _LL1199; _LL1199:
_temp1198=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1049)->f2;
goto _LL1086;} else{ goto _LL1087;} _LL1087: if(( unsigned int) _temp1049 > 1u?*((
int*) _temp1049) == Cyc_Absyn_Cut_s: 0){ _LL1203: _temp1202=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1049)->f1; goto _LL1088;} else{ goto
_LL1089;} _LL1089: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Splice_s:
0){ _LL1205: _temp1204=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1049)->f1; goto _LL1090;} else{ goto _LL1091;} _LL1091: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Label_s: 0){ _LL1209: _temp1208=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1049)->f1; goto
_LL1207; _LL1207: _temp1206=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1049)->f2; goto _LL1092;} else{ goto _LL1093;} _LL1093: if(( unsigned int)
_temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_TryCatch_s: 0){ _LL1213:
_temp1212=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1049)->f1; goto _LL1211; _LL1211: _temp1210=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1049)->f2; goto _LL1094;} else{ goto
_LL1095;} _LL1095: if(( unsigned int) _temp1049 > 1u?*(( int*) _temp1049) == Cyc_Absyn_Region_s:
0){ _LL1219: _temp1218=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1049)->f1; goto _LL1217; _LL1217: _temp1216=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1049)->f2; goto _LL1215; _LL1215:
_temp1214=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1049)->f3; goto _LL1096;} else{ goto _LL1050;} _LL1052: return; _LL1054: {
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1097); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1220=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1220->hd=( void*) ae; _temp1220->tl= 0; _temp1220;}));
return;} _LL1056: Cyc_NewControlFlow_cf_prepass( env, _temp1101); Cyc_NewControlFlow_cf_prepass(
env, _temp1099); return; _LL1058: return; _LL1060: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1222= _temp1105; if( _temp1222 == 0){ _throw( Null_Exception);} _temp1222;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1221=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1221->hd=( void*)
ae; _temp1221->tl= 0; _temp1221;})); return;} _LL1062: { void* _temp1225; void*
_temp1227; struct _tuple0 _temp1223= Cyc_NewControlFlow_abstract_guard( env,
_temp1111); _LL1228: _temp1227= _temp1223.f1; goto _LL1226; _LL1226: _temp1225=
_temp1223.f2; goto _LL1224; _LL1224: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1229=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1229->hd=( void*) _temp1227; _temp1229->tl=({
struct Cyc_List_List* _temp1230=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1230->hd=( void*) _temp1225; _temp1230->tl= 0;
_temp1230;}); _temp1229;})); Cyc_NewControlFlow_cf_prepass( env, _temp1109); Cyc_NewControlFlow_cf_prepass(
env, _temp1107); return;} _LL1064: _temp1127= _temp1117; _temp1125= _temp1115;
_temp1121= _temp1119; goto _LL1066; _LL1066: { void* _temp1233; void* _temp1235;
struct _tuple0 _temp1231= Cyc_NewControlFlow_abstract_guard( env, _temp1127);
_LL1236: _temp1235= _temp1231.f1; goto _LL1234; _LL1234: _temp1233= _temp1231.f2;
goto _LL1232; _LL1232: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1237=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1237->hd=( void*) _temp1235; _temp1237->tl=({ struct Cyc_List_List*
_temp1238=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1238->hd=( void*) _temp1233; _temp1238->tl= 0; _temp1238;}); _temp1237;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1121); return;} _LL1068: return;
_LL1070: return; _LL1072:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1239=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1239[ 0]=({ struct Cyc_Core_Impossible_struct _temp1240; _temp1240.tag= Cyc_Core_Impossible;
_temp1240.f1=( struct _tagged_string)({ char* _temp1241=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1242; _temp1242.curr= _temp1241; _temp1242.base=
_temp1241; _temp1242.last_plus_one= _temp1241 + 33; _temp1242;}); _temp1240;});
_temp1239;})); _LL1074: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1245= _temp1137; if(
_temp1245 == 0){ _throw( Null_Exception);} _temp1245;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1243=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1244; _temp1244.curr= _temp1243; _temp1244.base=
_temp1243; _temp1244.last_plus_one= _temp1243 + 45; _temp1244;})); break;}
my_encloser= next_encloser;} return;} _LL1076: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1155); void* _temp1248; void* _temp1250; struct _tuple0 _temp1246= Cyc_NewControlFlow_abstract_guard(
env, _temp1153); _LL1251: _temp1250= _temp1246.f1; goto _LL1249; _LL1249:
_temp1248= _temp1246.f2; goto _LL1247; _LL1247: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1147); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1252=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1252->hd=( void*) ae1; _temp1252->tl=({ struct Cyc_List_List* _temp1253=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1253->hd=(
void*) _temp1250; _temp1253->tl=({ struct Cyc_List_List* _temp1254=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1254->hd=( void*) _temp1248;
_temp1254->tl=({ struct Cyc_List_List* _temp1255=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1255->hd=( void*) ae3; _temp1255->tl=
0; _temp1255;}); _temp1254;}); _temp1253;}); _temp1252;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1141); return;}} _LL1078: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1159); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1157); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1256=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1256->hd=( void*) ae; _temp1256->tl= ael; _temp1256;})); return;} _LL1080: {
void* _temp1259; void* _temp1261; struct Cyc_NewControlFlow_AbsSynOne _temp1257=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1163, 0)); _LL1262: _temp1261=( void*) _temp1257.inner_exp; goto _LL1260;
_LL1260: _temp1259=( void*) _temp1257.assigns; goto _LL1258; _LL1258: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1261, _temp1259);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1263=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1263->hd=( void*)
ae; _temp1263->tl= 0; _temp1263;})); return;}} _LL1082: Cyc_NewControlFlow_add_var_root(
env, _temp1174); if( _temp1174->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1266=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1266->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1174); _temp1266->tl= 0; _temp1266;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1265= _temp1174->initializer; if( _temp1265 == 0){
_throw( Null_Exception);} _temp1265;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1264=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1264->hd=( void*) ae; _temp1264->tl= 0; _temp1264;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1165); return; _LL1084: for( 0;
_temp1194 != 0; _temp1194=({ struct Cyc_List_List* _temp1267= _temp1194; if(
_temp1267 == 0){ _throw( Null_Exception);} _temp1267->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1268= _temp1194;
if( _temp1268 == 0){ _throw( Null_Exception);} _temp1268->hd;}));}{ void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1187); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1269=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1269->hd=( void*) ae; _temp1269->tl= 0; _temp1269;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1176); return;} _LL1086: goto _LL1088; _LL1088: goto _LL1090; _LL1090:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1270=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1270[ 0]=({ struct Cyc_Core_Impossible_struct _temp1271; _temp1271.tag= Cyc_Core_Impossible;
_temp1271.f1=( struct _tagged_string)({ char* _temp1272=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1273; _temp1273.curr= _temp1272; _temp1273.base=
_temp1272; _temp1273.last_plus_one= _temp1272 + 26; _temp1273;}); _temp1271;});
_temp1270;})); _LL1092: Cyc_NewControlFlow_cf_prepass( env, _temp1206); return;
_LL1094: Cyc_NewControlFlow_cf_prepass( env, _temp1212);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1210); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL1096: Cyc_NewControlFlow_add_var_root( env, _temp1216);{
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, Cyc_Absyn_uint_exp( 0, 0));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1274=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1274->hd=( void*)
ae; _temp1274->tl= 0; _temp1274;})); Cyc_NewControlFlow_cf_prepass( env,
_temp1214); return;} _LL1050:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1275=( char*)"new_control_flow.cyc:948 annot->visited == iteration_num";
struct _tagged_string _temp1276; _temp1276.curr= _temp1275; _temp1276.base=
_temp1275; _temp1276.last_plus_one= _temp1275 + 57; _temp1276;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1277= flow; struct Cyc_Dict_Dict* _temp1283; _LL1279: if( _temp1277
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1280;} else{ goto _LL1281;} _LL1281:
if(( unsigned int) _temp1277 > 1u?*(( int*) _temp1277) == Cyc_CfFlowInfo_InitsFL:
0){ _LL1284: _temp1283=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1277)->f1; goto _LL1282;} else{ goto _LL1278;} _LL1280: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1282: for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1285= vds; if(
_temp1285 == 0){ _throw( Null_Exception);} _temp1285->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct*
root=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1286=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1286[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1287; _temp1287.tag= Cyc_CfFlowInfo_VarRoot;
_temp1287.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1288= vds;
if( _temp1288 == 0){ _throw( Null_Exception);} _temp1288->hd;}); _temp1287;});
_temp1286;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) root)); _temp1283=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1283,( void*) root, pinfo);}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1289=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1289[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1290; _temp1290.tag= Cyc_CfFlowInfo_InitsFL;
_temp1290.f1= _temp1283; _temp1290;}); _temp1289;}); _LL1278:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1291= flow; struct Cyc_Dict_Dict* _temp1297; _LL1293: if( _temp1291
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1294;} else{ goto _LL1295;} _LL1295:
if(( unsigned int) _temp1291 > 1u?*(( int*) _temp1291) == Cyc_CfFlowInfo_InitsFL:
0){ _LL1298: _temp1297=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1291)->f1; goto _LL1296;} else{ goto _LL1292;} _LL1294: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1296: for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1299= vds; if(
_temp1299 == 0){ _throw( Null_Exception);} _temp1299->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct*
root=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1300=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1300[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1301; _temp1301.tag= Cyc_CfFlowInfo_VarRoot;
_temp1301.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1302= vds;
if( _temp1302 == 0){ _throw( Null_Exception);} _temp1302->hd;}); _temp1301;});
_temp1300;}); _temp1297=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1297,( void*) root,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1303=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1303[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1304; _temp1304.tag= Cyc_CfFlowInfo_InitsFL;
_temp1304.f1= _temp1297; _temp1304;}); _temp1303;}); _LL1292:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)({ struct Cyc_List_List* _temp1305= aes; if( _temp1305 == 0){ _throw(
Null_Exception);} _temp1305->hd;}) ==( void*)({ struct Cyc_List_List* _temp1307=({
struct Cyc_List_List* _temp1306= aes; if( _temp1306 == 0){ _throw(
Null_Exception);} _temp1306->tl;}); if( _temp1307 == 0){ _throw( Null_Exception);}
_temp1307->hd;})){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1309= aes; if( _temp1309 == 0){ _throw(
Null_Exception);} _temp1309->hd;}), in_flow); return({ struct _tuple0 _temp1308;
_temp1308.f1= out_flow; _temp1308.f2= out_flow; _temp1308;});} return({ struct
_tuple0 _temp1310; _temp1310.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1313= aes; if( _temp1313 == 0){ _throw(
Null_Exception);} _temp1313->hd;}), in_flow); _temp1310.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1312=({ struct Cyc_List_List*
_temp1311= aes; if( _temp1311 == 0){ _throw( Null_Exception);} _temp1311->tl;});
if( _temp1312 == 0){ _throw( Null_Exception);} _temp1312->hd;}), in_flow);
_temp1310;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp1314=
scs; if( _temp1314 == 0){ _throw( Null_Exception);} _temp1314->tl;})){ struct
Cyc_Absyn_Switch_clause _temp1318; struct Cyc_Position_Segment* _temp1319;
struct Cyc_Absyn_Stmt* _temp1321; struct Cyc_Absyn_Exp* _temp1323; struct Cyc_Core_Opt*
_temp1325; struct Cyc_Absyn_Pat* _temp1327; struct Cyc_Absyn_Switch_clause*
_temp1316=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1315=
scs; if( _temp1315 == 0){ _throw( Null_Exception);} _temp1315->hd;}); _temp1318=*
_temp1316; _LL1328: _temp1327=( struct Cyc_Absyn_Pat*) _temp1318.pattern; goto
_LL1326; _LL1326: _temp1325=( struct Cyc_Core_Opt*) _temp1318.pat_vars; goto
_LL1324; _LL1324: _temp1323=( struct Cyc_Absyn_Exp*) _temp1318.where_clause;
goto _LL1322; _LL1322: _temp1321=( struct Cyc_Absyn_Stmt*) _temp1318.body; goto
_LL1320; _LL1320: _temp1319=( struct Cyc_Position_Segment*) _temp1318.loc; goto
_LL1317; _LL1317: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1347= _temp1325;
if( _temp1347 == 0){ _throw( Null_Exception);} _temp1347->v;})); body_inflow=
Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1329= _temp1325; if( _temp1329 == 0){ _throw(
Null_Exception);} _temp1329->v;}));{ void* body_outflow; if( _temp1323 != 0){
void* _temp1333; void* _temp1335; struct _tuple0 _temp1331= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1330= _temp1323; if(
_temp1330 == 0){ _throw( Null_Exception);} _temp1330;}))->loc, aes, body_inflow);
_LL1336: _temp1335= _temp1331.f1; goto _LL1334; _LL1334: _temp1333= _temp1331.f2;
goto _LL1332; _LL1332: aes=({ struct Cyc_List_List* _temp1338=({ struct Cyc_List_List*
_temp1337= aes; if( _temp1337 == 0){ _throw( Null_Exception);} _temp1337->tl;});
if( _temp1338 == 0){ _throw( Null_Exception);} _temp1338->tl;}); in_flow=
_temp1333; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1321,
_temp1335);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1321, body_inflow);}{ void* _temp1339= body_outflow; _LL1341: if( _temp1339
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1342;} else{ goto _LL1343;} _LL1343:
goto _LL1344; _LL1342: goto _LL1340; _LL1344: Cyc_Tcutil_terr( _temp1321->loc,(
struct _tagged_string)({ char* _temp1345=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1346; _temp1346.curr= _temp1345; _temp1346.base=
_temp1345; _temp1346.last_plus_one= _temp1345 + 38; _temp1346;})); goto _LL1340;
_LL1340:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1348=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1392; struct Cyc_Absyn_Exp* _temp1394; struct Cyc_Absyn_Exp*
_temp1396; struct Cyc_Absyn_Stmt* _temp1398; struct Cyc_Absyn_Stmt* _temp1400;
struct Cyc_Absyn_Stmt* _temp1402; struct Cyc_Absyn_Stmt* _temp1404; struct Cyc_Absyn_Exp*
_temp1406; struct Cyc_Absyn_Stmt* _temp1408; struct _tuple3 _temp1410; struct
Cyc_Absyn_Stmt* _temp1412; struct Cyc_Absyn_Exp* _temp1414; struct _tuple3
_temp1416; struct Cyc_Absyn_Stmt* _temp1418; struct Cyc_Absyn_Exp* _temp1420;
struct Cyc_Absyn_Stmt* _temp1422; struct Cyc_Absyn_Stmt* _temp1424; struct
_tuple3 _temp1426; struct Cyc_Absyn_Stmt* _temp1428; struct Cyc_Absyn_Exp*
_temp1430; struct _tuple3 _temp1432; struct Cyc_Absyn_Stmt* _temp1434; struct
Cyc_Absyn_Exp* _temp1436; struct Cyc_Absyn_Exp* _temp1438; struct Cyc_Absyn_Stmt*
_temp1440; struct Cyc_Absyn_Switch_clause** _temp1442; struct Cyc_Absyn_Switch_clause*
_temp1444; struct Cyc_List_List* _temp1445; struct Cyc_Absyn_Stmt* _temp1447;
struct Cyc_Absyn_Stmt* _temp1449; struct Cyc_Absyn_Stmt* _temp1451; struct
_tagged_string* _temp1453; struct Cyc_List_List* _temp1455; struct Cyc_Absyn_Exp*
_temp1457; struct Cyc_List_List* _temp1459; struct Cyc_Absyn_Stmt* _temp1461;
struct Cyc_Absyn_Stmt* _temp1463; struct Cyc_Absyn_Decl* _temp1465; struct Cyc_Absyn_Decl
_temp1467; struct Cyc_Position_Segment* _temp1468; void* _temp1470; struct Cyc_Absyn_Vardecl*
_temp1472; struct Cyc_Absyn_Stmt* _temp1474; struct Cyc_Absyn_Decl* _temp1476;
struct Cyc_Absyn_Decl _temp1478; struct Cyc_Position_Segment* _temp1479; void*
_temp1481; int _temp1483; struct Cyc_Absyn_Exp* _temp1485; struct Cyc_Core_Opt*
_temp1487; struct Cyc_Core_Opt* _temp1489; struct Cyc_Core_Opt _temp1491; struct
Cyc_List_List* _temp1492; struct Cyc_Absyn_Pat* _temp1494; struct Cyc_Absyn_Stmt*
_temp1496; struct _tagged_string* _temp1498; struct Cyc_Absyn_Stmt* _temp1500;
struct Cyc_Absyn_Vardecl* _temp1502; struct Cyc_Absyn_Tvar* _temp1504; _LL1350:
if( _temp1348 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1351;} else{ goto _LL1352;}
_LL1352: if(( unsigned int) _temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Return_s:
0){ _LL1393: _temp1392=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1348)->f1; if( _temp1392 == 0){ goto _LL1353;} else{ goto _LL1354;}} else{
goto _LL1354;} _LL1354: if(( unsigned int) _temp1348 > 1u?*(( int*) _temp1348)
== Cyc_Absyn_Return_s: 0){ _LL1395: _temp1394=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1348)->f1; goto _LL1355;} else{ goto _LL1356;}
_LL1356: if(( unsigned int) _temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Exp_s:
0){ _LL1397: _temp1396=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1348)->f1; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Seq_s: 0){ _LL1401: _temp1400=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1348)->f1; goto
_LL1399; _LL1399: _temp1398=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1348)->f2; goto _LL1359;} else{ goto _LL1360;} _LL1360: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_IfThenElse_s: 0){ _LL1407:
_temp1406=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1348)->f1; goto _LL1405; _LL1405: _temp1404=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1348)->f2; goto _LL1403; _LL1403:
_temp1402=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1348)->f3; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_While_s: 0){ _LL1411: _temp1410=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1348)->f1; _LL1415:
_temp1414= _temp1410.f1; goto _LL1413; _LL1413: _temp1412= _temp1410.f2; goto
_LL1409; _LL1409: _temp1408=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1348)->f2; goto _LL1363;} else{ goto _LL1364;} _LL1364: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Do_s: 0){ _LL1423: _temp1422=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1348)->f1; goto
_LL1417; _LL1417: _temp1416=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1348)->f2; _LL1421: _temp1420= _temp1416.f1; goto _LL1419; _LL1419:
_temp1418= _temp1416.f2; goto _LL1365;} else{ goto _LL1366;} _LL1366: if((
unsigned int) _temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_For_s: 0){
_LL1439: _temp1438=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1348)->f1; goto _LL1433; _LL1433: _temp1432=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1348)->f2; _LL1437: _temp1436= _temp1432.f1; goto _LL1435; _LL1435:
_temp1434= _temp1432.f2; goto _LL1427; _LL1427: _temp1426=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1348)->f3; _LL1431: _temp1430= _temp1426.f1;
goto _LL1429; _LL1429: _temp1428= _temp1426.f2; goto _LL1425; _LL1425: _temp1424=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1348)->f4; goto
_LL1367;} else{ goto _LL1368;} _LL1368: if(( unsigned int) _temp1348 > 1u?*((
int*) _temp1348) == Cyc_Absyn_Break_s: 0){ _LL1441: _temp1440=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1348)->f1; if( _temp1440 == 0){ goto
_LL1369;} else{ goto _LL1370;}} else{ goto _LL1370;} _LL1370: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Fallthru_s: 0){ _LL1446:
_temp1445=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1348)->f1; goto _LL1443; _LL1443: _temp1442=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1348)->f2; if( _temp1442 == 0){ goto
_LL1372;} else{ _temp1444=* _temp1442; goto _LL1371;}} else{ goto _LL1372;}
_LL1372: if(( unsigned int) _temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Break_s:
0){ _LL1448: _temp1447=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1348)->f1; goto _LL1373;} else{ goto _LL1374;} _LL1374: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Continue_s: 0){ _LL1450:
_temp1449=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1348)->f1; goto _LL1375;} else{ goto _LL1376;} _LL1376: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Goto_s: 0){ _LL1454: _temp1453=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1348)->f1; goto
_LL1452; _LL1452: _temp1451=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1348)->f2; goto _LL1377;} else{ goto _LL1378;} _LL1378: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Switch_s: 0){ _LL1458:
_temp1457=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1348)->f1; goto _LL1456; _LL1456: _temp1455=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1348)->f2; goto _LL1379;} else{ goto
_LL1380;} _LL1380: if(( unsigned int) _temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_TryCatch_s:
0){ _LL1462: _temp1461=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1348)->f1; goto _LL1460; _LL1460: _temp1459=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1348)->f2; goto _LL1381;} else{ goto
_LL1382;} _LL1382: if(( unsigned int) _temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Decl_s:
0){ _LL1466: _temp1465=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1348)->f1; _temp1467=* _temp1465; _LL1471: _temp1470=( void*) _temp1467.r;
if(*(( int*) _temp1470) == Cyc_Absyn_Var_d){ _LL1473: _temp1472=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1470)->f1; goto _LL1469;} else{ goto
_LL1384;} _LL1469: _temp1468=( struct Cyc_Position_Segment*) _temp1467.loc; goto
_LL1464; _LL1464: _temp1463=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1348)->f2; goto _LL1383;} else{ goto _LL1384;} _LL1384: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Decl_s: 0){ _LL1477: _temp1476=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1348)->f1;
_temp1478=* _temp1476; _LL1482: _temp1481=( void*) _temp1478.r; if(*(( int*)
_temp1481) == Cyc_Absyn_Let_d){ _LL1495: _temp1494=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1481)->f1; goto _LL1490; _LL1490: _temp1489=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1481)->f2; if(
_temp1489 == 0){ goto _LL1386;} else{ _temp1491=* _temp1489; _LL1493: _temp1492=(
struct Cyc_List_List*) _temp1491.v; goto _LL1488;} _LL1488: _temp1487=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1481)->f3; goto _LL1486;
_LL1486: _temp1485=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1481)->f4; goto _LL1484; _LL1484: _temp1483=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1481)->f5; goto _LL1480;} else{ goto _LL1386;} _LL1480: _temp1479=( struct
Cyc_Position_Segment*) _temp1478.loc; goto _LL1475; _LL1475: _temp1474=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1348)->f2; goto _LL1385;}
else{ goto _LL1386;} _LL1386: if(( unsigned int) _temp1348 > 1u?*(( int*)
_temp1348) == Cyc_Absyn_Label_s: 0){ _LL1499: _temp1498=( struct _tagged_string*)((
struct Cyc_Absyn_Label_s_struct*) _temp1348)->f1; goto _LL1497; _LL1497:
_temp1496=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1348)->f2; goto _LL1387;} else{ goto _LL1388;} _LL1388: if(( unsigned int)
_temp1348 > 1u?*(( int*) _temp1348) == Cyc_Absyn_Region_s: 0){ _LL1505:
_temp1504=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1348)->f1; goto _LL1503; _LL1503: _temp1502=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1348)->f2; goto _LL1501; _LL1501:
_temp1500=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1348)->f3; goto _LL1389;} else{ goto _LL1390;} _LL1390: goto _LL1391;
_LL1351: return in_flow; _LL1353: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1355: Cyc_CfAbsexp_eval_absexp( env,({ struct Cyc_Absyn_Exp* _temp1506=
_temp1394; if( _temp1506 == 0){ _throw( Null_Exception);} _temp1506->loc;}),(
void*)({ struct Cyc_List_List* _temp1507= aes; if( _temp1507 == 0){ _throw(
Null_Exception);} _temp1507->hd;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1357: return Cyc_CfAbsexp_eval_absexp( env, _temp1396->loc,( void*)({ struct
Cyc_List_List* _temp1508= aes; if( _temp1508 == 0){ _throw( Null_Exception);}
_temp1508->hd;}), in_flow); _LL1359: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1398, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1400, in_flow));
_LL1361: { void* _temp1511; void* _temp1513; struct _tuple0 _temp1509= Cyc_NewControlFlow_cf_evalguard(
env, _temp1406->loc, aes, in_flow); _LL1514: _temp1513= _temp1509.f1; goto
_LL1512; _LL1512: _temp1511= _temp1509.f2; goto _LL1510; _LL1510: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1404, _temp1513), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1402, _temp1511));} _LL1363: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1412))->flow; void* _temp1517; void* _temp1519; struct _tuple0
_temp1515= Cyc_NewControlFlow_cf_evalguard( env, _temp1414->loc, aes, e_inflow);
_LL1520: _temp1519= _temp1515.f1; goto _LL1518; _LL1518: _temp1517= _temp1515.f2;
goto _LL1516; _LL1516: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1408, _temp1519); Cyc_NewControlFlow_update_flow( _temp1412,
body_outflow); return _temp1517;}} _LL1365: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1422, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1418))->flow; void* _temp1523; void* _temp1525; struct
_tuple0 _temp1521= Cyc_NewControlFlow_cf_evalguard( env, _temp1420->loc, aes,
e_inflow); _LL1526: _temp1525= _temp1521.f1; goto _LL1524; _LL1524: _temp1523=
_temp1521.f2; goto _LL1522; _LL1522: Cyc_NewControlFlow_update_flow( _temp1422,
_temp1525); return _temp1523;} _LL1367: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1438->loc,( void*)({ struct Cyc_List_List* _temp1538= aes; if(
_temp1538 == 0){ _throw( Null_Exception);} _temp1538->hd;}), in_flow); void*
e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( e1_outflow, _temp1434))->flow;
void* _temp1530; void* _temp1532; struct _tuple0 _temp1528= Cyc_NewControlFlow_cf_evalguard(
env, _temp1436->loc,({ struct Cyc_List_List* _temp1527= aes; if( _temp1527 == 0){
_throw( Null_Exception);} _temp1527->tl;}), e2_inflow); _LL1533: _temp1532=
_temp1528.f1; goto _LL1531; _LL1531: _temp1530= _temp1528.f2; goto _LL1529;
_LL1529: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1424, _temp1532); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1428))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1430->loc,( void*)({ struct Cyc_List_List* _temp1537=({ struct Cyc_List_List*
_temp1536=({ struct Cyc_List_List* _temp1535=({ struct Cyc_List_List* _temp1534=
aes; if( _temp1534 == 0){ _throw( Null_Exception);} _temp1534->tl;}); if(
_temp1535 == 0){ _throw( Null_Exception);} _temp1535->tl;}); if( _temp1536 == 0){
_throw( Null_Exception);} _temp1536->tl;}); if( _temp1537 == 0){ _throw(
Null_Exception);} _temp1537->hd;}), e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1434, e3_outflow); return _temp1530;}} _LL1369: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1371: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)({ struct Cyc_List_List*
_temp1539= aes; if( _temp1539 == 0){ _throw( Null_Exception);} _temp1539->hd;}),
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1540= _temp1444->pat_vars; if( _temp1540 == 0){ _throw( Null_Exception);}
_temp1540->v;}); in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
ftvars); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars);
_temp1447=( struct Cyc_Absyn_Stmt*) _temp1444->body; goto _LL1373;} _LL1373:
_temp1449= _temp1447; goto _LL1375; _LL1375: _temp1451= _temp1449; goto _LL1377;
_LL1377: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt*
_temp1541= _temp1451; if( _temp1541 == 0){ _throw( Null_Exception);} _temp1541;}),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1379: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1457->loc,( void*)({ struct Cyc_List_List* _temp1542= aes; if(
_temp1542 == 0){ _throw( Null_Exception);} _temp1542->hd;}), in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1455,({ struct Cyc_List_List* _temp1543= aes; if( _temp1543 == 0){
_throw( Null_Exception);} _temp1543->tl;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1381: { struct Cyc_NewControlFlow_AnalEnv _temp1546; void* _temp1547; int
_temp1549; struct Cyc_Dict_Dict* _temp1551; struct Cyc_NewControlFlow_AnalEnv*
_temp1544= env; _temp1546=* _temp1544; _LL1552: _temp1551=( struct Cyc_Dict_Dict*)
_temp1546.roots; goto _LL1550; _LL1550: _temp1549=( int) _temp1546.in_try; goto
_LL1548; _LL1548: _temp1547=( void*) _temp1546.tryflow; goto _LL1545; _LL1545:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1461, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1549;( void*)( env->tryflow=( void*) _temp1547); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1459,
aes, scs_inflow); return s_outflow;}} _LL1383: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1555; _temp1555.hd=( void*) _temp1472; _temp1555.tl= 0;
_temp1555;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1472->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1553= e; if( _temp1553 == 0){ _throw( Null_Exception);}
_temp1553;}))->loc,( void*)({ struct Cyc_List_List* _temp1554= aes; if(
_temp1554 == 0){ _throw( Null_Exception);} _temp1554->hd;}), body_inflow);}
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1463, body_inflow);}
_LL1385: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1492); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1485->loc,( void*)({
struct Cyc_List_List* _temp1556= aes; if( _temp1556 == 0){ _throw(
Null_Exception);} _temp1556->hd;}), body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1492); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1474, body_inflow);} _LL1387: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1496, in_flow); _LL1389: { struct Cyc_List_List* vds=({ struct Cyc_List_List*
_temp1557=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1557->hd=( void*) _temp1502; _temp1557->tl= 0; _temp1557;}); void*
body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, vds);
body_inflow= Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow, vds);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1500, body_inflow);}
_LL1391:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1558=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1558[ 0]=({ struct Cyc_Core_Impossible_struct _temp1559; _temp1559.tag= Cyc_Core_Impossible;
_temp1559.f1=( struct _tagged_string)({ char* _temp1560=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1561; _temp1561.curr= _temp1560; _temp1561.base=
_temp1560; _temp1561.last_plus_one= _temp1560 + 62; _temp1561;}); _temp1559;});
_temp1558;})); _LL1349:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1562=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1586; struct Cyc_Absyn_Stmt* _temp1588;
struct Cyc_Absyn_Stmt* _temp1590; struct Cyc_Absyn_Stmt* _temp1592; struct Cyc_Absyn_Exp*
_temp1594; struct Cyc_Absyn_Stmt* _temp1596; struct _tuple3 _temp1598; struct
Cyc_Absyn_Stmt* _temp1600; struct Cyc_Absyn_Exp* _temp1602; struct _tuple3
_temp1604; struct Cyc_Absyn_Stmt* _temp1606; struct Cyc_Absyn_Exp* _temp1608;
struct Cyc_Absyn_Exp* _temp1610; struct Cyc_Absyn_Stmt* _temp1612; struct
_tuple3 _temp1614; struct Cyc_Absyn_Stmt* _temp1616; struct Cyc_Absyn_Exp*
_temp1618; struct _tuple3 _temp1620; struct Cyc_Absyn_Stmt* _temp1622; struct
Cyc_Absyn_Exp* _temp1624; struct Cyc_Absyn_Stmt* _temp1626; struct Cyc_Absyn_Stmt*
_temp1628; struct Cyc_Absyn_Vardecl* _temp1630; struct Cyc_Absyn_Tvar* _temp1632;
struct Cyc_Absyn_Stmt* _temp1634; struct _tagged_string* _temp1636; struct Cyc_Absyn_Stmt*
_temp1638; struct Cyc_Absyn_Decl* _temp1640; struct Cyc_List_List* _temp1642;
struct Cyc_Absyn_Stmt* _temp1644; struct Cyc_List_List* _temp1646; struct Cyc_Absyn_Exp*
_temp1648; _LL1564: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) ==
Cyc_Absyn_Seq_s: 0){ _LL1589: _temp1588=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1562)->f1; goto _LL1587; _LL1587: _temp1586=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1562)->f2; goto _LL1565;} else{ goto
_LL1566;} _LL1566: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_IfThenElse_s:
0){ _LL1595: _temp1594=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1562)->f1; goto _LL1593; _LL1593: _temp1592=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1562)->f2; goto _LL1591; _LL1591:
_temp1590=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1562)->f3; goto _LL1567;} else{ goto _LL1568;} _LL1568: if(( unsigned int)
_temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_For_s: 0){ _LL1611: _temp1610=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1562)->f1; goto
_LL1605; _LL1605: _temp1604=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1562)->f2; _LL1609: _temp1608= _temp1604.f1; goto _LL1607; _LL1607:
_temp1606= _temp1604.f2; goto _LL1599; _LL1599: _temp1598=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1562)->f3; _LL1603: _temp1602= _temp1598.f1;
goto _LL1601; _LL1601: _temp1600= _temp1598.f2; goto _LL1597; _LL1597: _temp1596=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1562)->f4; goto
_LL1569;} else{ goto _LL1570;} _LL1570: if(( unsigned int) _temp1562 > 1u?*((
int*) _temp1562) == Cyc_Absyn_While_s: 0){ _LL1615: _temp1614=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp1562)->f1; _LL1619: _temp1618= _temp1614.f1;
goto _LL1617; _LL1617: _temp1616= _temp1614.f2; goto _LL1613; _LL1613: _temp1612=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp1562)->f2; goto
_LL1571;} else{ goto _LL1572;} _LL1572: if(( unsigned int) _temp1562 > 1u?*((
int*) _temp1562) == Cyc_Absyn_Do_s: 0){ _LL1627: _temp1626=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1562)->f1; goto _LL1621; _LL1621: _temp1620=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1562)->f2; _LL1625:
_temp1624= _temp1620.f1; goto _LL1623; _LL1623: _temp1622= _temp1620.f2; goto
_LL1573;} else{ goto _LL1574;} _LL1574: if(( unsigned int) _temp1562 > 1u?*((
int*) _temp1562) == Cyc_Absyn_Region_s: 0){ _LL1633: _temp1632=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1562)->f1; goto _LL1631; _LL1631:
_temp1630=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1562)->f2; goto _LL1629; _LL1629: _temp1628=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1562)->f3; goto _LL1575;} else{ goto
_LL1576;} _LL1576: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Label_s:
0){ _LL1637: _temp1636=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1562)->f1; goto _LL1635; _LL1635: _temp1634=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1562)->f2; goto _LL1577;} else{ goto
_LL1578;} _LL1578: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Decl_s:
0){ _LL1641: _temp1640=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1562)->f1; goto _LL1639; _LL1639: _temp1638=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1562)->f2; goto _LL1579;} else{ goto
_LL1580;} _LL1580: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_TryCatch_s:
0){ _LL1645: _temp1644=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1562)->f1; goto _LL1643; _LL1643: _temp1642=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1562)->f2; goto _LL1581;} else{ goto
_LL1582;} _LL1582: if(( unsigned int) _temp1562 > 1u?*(( int*) _temp1562) == Cyc_Absyn_Switch_s:
0){ _LL1649: _temp1648=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1562)->f1; goto _LL1647; _LL1647: _temp1646=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1562)->f2; goto _LL1583;} else{ goto
_LL1584;} _LL1584: goto _LL1585; _LL1565: _temp1592= _temp1588; _temp1590=
_temp1586; goto _LL1567; _LL1567: Cyc_NewControlFlow_cf_postpass( _temp1592);
Cyc_NewControlFlow_cf_postpass( _temp1590); return; _LL1569: Cyc_NewControlFlow_cf_postpass(
_temp1600); _temp1616= _temp1596; _temp1612= _temp1606; goto _LL1571; _LL1571:
_temp1626= _temp1612; _temp1622= _temp1616; goto _LL1573; _LL1573: Cyc_NewControlFlow_cf_postpass(
_temp1622); _temp1628= _temp1626; goto _LL1575; _LL1575: _temp1634= _temp1628;
goto _LL1577; _LL1577: _temp1638= _temp1634; goto _LL1579; _LL1579: Cyc_NewControlFlow_cf_postpass(
_temp1638); return; _LL1581: Cyc_NewControlFlow_cf_postpass( _temp1644);
_temp1646= _temp1642; goto _LL1583; _LL1583: for( 0; _temp1646 != 0; _temp1646=({
struct Cyc_List_List* _temp1650= _temp1646; if( _temp1650 == 0){ _throw(
Null_Exception);} _temp1650->tl;})){ Cyc_NewControlFlow_cf_postpass((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1651= _temp1646; if(
_temp1651 == 0){ _throw( Null_Exception);} _temp1651->hd;}))->body);} return;
_LL1585: return; _LL1563:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1679; _temp1679.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1680=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1680->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1680->comprehension_vars=
0; _temp1680;}); _temp1679.lhsides= 0; _temp1679.access_path= 0; _temp1679;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1678= fd->param_vardecls; if( _temp1678 == 0){ _throw( Null_Exception);}
_temp1678->v;});{ struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds=({
struct Cyc_List_List* _temp1652= vds; if( _temp1652 == 0){ _throw(
Null_Exception);} _temp1652->tl;})){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1653= vds; if( _temp1653
== 0){ _throw( Null_Exception);} _temp1653->hd;}));}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1656; struct Cyc_List_List*
_temp1658; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1660; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1662; struct Cyc_List_List* _temp1663; struct Cyc_Dict_Dict* _temp1665;
struct Cyc_NewControlFlow_AbsEnv _temp1654= absenv; _LL1661: _temp1660=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1654.shared; _temp1662=* _temp1660;
_LL1666: _temp1665=( struct Cyc_Dict_Dict*) _temp1662.all_roots; goto _LL1664;
_LL1664: _temp1663=( struct Cyc_List_List*) _temp1662.comprehension_vars; goto
_LL1659; _LL1659: _temp1658=( struct Cyc_List_List*) _temp1654.lhsides; goto
_LL1657; _LL1657: _temp1656=( struct Cyc_List_List*) _temp1654.access_path; goto
_LL1655; _LL1655: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1677=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1677->roots= _temp1665; _temp1677->in_try= 0; _temp1677->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1677;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1675=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1675[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1676; _temp1676.tag= Cyc_CfFlowInfo_InitsFL;
_temp1676.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1676;}); _temp1675;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1663);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1667= out_flow; _LL1669: if( _temp1667 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1670;} else{ goto _LL1671;} _LL1671: goto _LL1672; _LL1670: goto _LL1668;
_LL1672: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1673=( char*)"function may complete without returning a value";
struct _tagged_string _temp1674; _temp1674.curr= _temp1673; _temp1674.base=
_temp1673; _temp1674.last_plus_one= _temp1673 + 48; _temp1674;}));} goto _LL1668;
_LL1668:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1681= ds; if( _temp1681 == 0){ _throw( Null_Exception);} _temp1681->tl;})){
void* _temp1683=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1682= ds; if( _temp1682 == 0){ _throw( Null_Exception);} _temp1682->hd;}))->r;
struct Cyc_Absyn_Fndecl* _temp1693; struct Cyc_List_List* _temp1695; struct
_tuple1* _temp1697; struct Cyc_List_List* _temp1699; struct _tagged_string*
_temp1701; _LL1685: if(*(( int*) _temp1683) == Cyc_Absyn_Fn_d){ _LL1694:
_temp1693=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1683)->f1;
goto _LL1686;} else{ goto _LL1687;} _LL1687: if(*(( int*) _temp1683) == Cyc_Absyn_Using_d){
_LL1698: _temp1697=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1683)->f1; goto _LL1696; _LL1696: _temp1695=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1683)->f2; goto _LL1688;} else{ goto
_LL1689;} _LL1689: if(*(( int*) _temp1683) == Cyc_Absyn_Namespace_d){ _LL1702:
_temp1701=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1683)->f1; goto _LL1700; _LL1700: _temp1699=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1683)->f2; goto _LL1690;} else{ goto
_LL1691;} _LL1691: goto _LL1692; _LL1686: Cyc_NewControlFlow_cf_check_fun(
_temp1693); goto _LL1684; _LL1688: _temp1699= _temp1695; goto _LL1690; _LL1690:
Cyc_NewControlFlow_cf_check( _temp1699); goto _LL1684; _LL1692: goto _LL1684;
_LL1684:;}}