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
extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg[ 15u];
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
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
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
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
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
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp20; _temp20.shared= src.shared; _temp20.lhsides= new_lhs; _temp20.access_path=
src.access_path; _temp20;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
== 0){ Cyc_NewControlFlow_unknown_list=({ struct Cyc_List_List* _temp21=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp21->hd=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp21->tl= 0; _temp21;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv _temp22= Cyc_NewControlFlow_change_lhs(
src, Cyc_NewControlFlow_use_it_list()); goto _LL23; _LL23: _temp22.access_path=
0; return _temp22;} struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ struct Cyc_List_List* _temp26; struct Cyc_NewControlFlow_AbsSynOne
_temp28; void* _temp30; void* _temp32; struct Cyc_NewControlFlow_AbsSynOne
_temp34; void* _temp36; void* _temp38; struct Cyc_NewControlFlow_AbsSyn _temp24=
syn; _LL35: _temp34=( struct Cyc_NewControlFlow_AbsSynOne) _temp24.when_true;
_LL39: _temp38=( void*) _temp34.inner_exp; goto _LL37; _LL37: _temp36=( void*)
_temp34.assigns; goto _LL29; _LL29: _temp28=( struct Cyc_NewControlFlow_AbsSynOne)
_temp24.when_false; _LL33: _temp32=( void*) _temp28.inner_exp; goto _LL31; _LL31:
_temp30=( void*) _temp28.assigns; goto _LL27; _LL27: _temp26=( struct Cyc_List_List*)
_temp24.lvalues; goto _LL25; _LL25: { void* _temp40= _temp38 == _temp32? _temp38:
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp38, _temp32); goto
_LL41; _LL41: { void* _temp42= _temp36 == _temp30? _temp36: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp36, _temp30); goto _LL43; _LL43: return({
struct Cyc_NewControlFlow_AbsSynOne _temp44; _temp44.inner_exp=( void*) _temp40;
_temp44.assigns=( void*) _temp42; _temp44;});}}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne _temp46=({ struct Cyc_NewControlFlow_AbsSynOne
_temp45; _temp45.inner_exp=( void*) ae; _temp45.assigns=( void*) assigns;
_temp45;}); goto _LL47; _LL47: return({ struct Cyc_NewControlFlow_AbsSyn _temp48;
_temp48.when_true= _temp46; _temp48.when_false= _temp46; _temp48.lvalues= lvals;
_temp48;});} void* Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv
env, void* ae){ if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(((
struct Cyc_List_List*) _check_null( env.lhsides))->tl == 0){ return Cyc_CfAbsexp_mkAssignAE((
void*)(( struct Cyc_List_List*) _check_null( env.lhsides))->hd, ae);}{ struct
Cyc_List_List* _temp49= 0; goto _LL50; _LL50:{ struct Cyc_List_List* lhsides=
env.lhsides; for( 0; lhsides != 0; lhsides=(( struct Cyc_List_List*) _check_null(
lhsides))->tl){ _temp49=({ struct Cyc_List_List* _temp51=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp51->hd=( void*) Cyc_CfAbsexp_mkAssignAE((
void*)(( struct Cyc_List_List*) _check_null( lhsides))->hd, ae); _temp51->tl=
_temp49; _temp51;});}} return Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_OneofG,
_temp49);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp52= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
goto _LL53; _LL53: { void* _temp56; void* _temp58; struct Cyc_NewControlFlow_AbsSynOne
_temp54= Cyc_NewControlFlow_meet_absexp( _temp52); _LL59: _temp58=( void*)
_temp54.inner_exp; goto _LL57; _LL57: _temp56=( void*) _temp54.assigns; goto
_LL55; _LL55: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp58, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp56, uses_exp);}} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp60=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp60->hd=( void*) path_e; _temp60->tl=
env.access_path; _temp60;});{ struct Cyc_NewControlFlow_AbsSyn _temp61= Cyc_NewControlFlow_abstract_exp(
env, inner_e); goto _LL62; _LL62: if( _temp61.lvalues == 0){ _temp61.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp61;}} void* Cyc_NewControlFlow_destruct_path(
struct Cyc_NewControlFlow_AbsEnv env, void* absop){{ struct Cyc_List_List* path=
env.access_path; for( 0; path != 0; path=(( struct Cyc_List_List*) _check_null(
path))->tl){ void* _temp63=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( path))->hd)->r; struct Cyc_Absyn_Exp* _temp77; struct Cyc_Absyn_Exp*
_temp79; struct _tagged_string* _temp81; struct Cyc_Absyn_Exp* _temp83; struct
_tagged_string* _temp85; struct Cyc_Absyn_Exp* _temp87; struct Cyc_Absyn_Exp*
_temp89; struct Cyc_Absyn_Exp* _temp91; _LL65: if(*(( int*) _temp63) == Cyc_Absyn_Address_e){
_LL78: _temp77=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp63)->f1; goto _LL66;} else{ goto _LL67;} _LL67: if(*(( int*) _temp63) ==
Cyc_Absyn_Deref_e){ _LL80: _temp79=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp63)->f1; goto _LL68;} else{ goto _LL69;} _LL69: if(*(( int*) _temp63) ==
Cyc_Absyn_StructArrow_e){ _LL84: _temp83=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp63)->f1; goto _LL82; _LL82: _temp81=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp63)->f2; goto _LL70;} else{ goto _LL71;} _LL71: if(*(( int*) _temp63) ==
Cyc_Absyn_StructMember_e){ _LL88: _temp87=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp63)->f1; goto _LL86; _LL86: _temp85=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp63)->f2; goto _LL72;} else{ goto _LL73;} _LL73: if(*(( int*) _temp63) ==
Cyc_Absyn_Subscript_e){ _LL92: _temp91=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp63)->f1; goto _LL90; _LL90: _temp89=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp63)->f2; goto _LL74;} else{ goto _LL75;} _LL75: goto _LL76; _LL66:(( struct
Cyc_List_List*) _check_null( path))->tl == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp93=( char*)"new_control_flow.cyc:219 path->tl == null";
struct _tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93;
_temp94.last_plus_one= _temp93 + 42; _temp94;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL68: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL64; _LL70: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp85= _temp81; goto _LL72; _LL72: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp95=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp95[ 0]=({ struct
Cyc_CfFlowInfo_StructF_struct _temp96; _temp96.tag= Cyc_CfFlowInfo_StructF;
_temp96.f1= _temp85; _temp96;}); _temp95;})); goto _LL64; _LL74: { unsigned int
_temp97= Cyc_Evexp_eval_const_uint_exp( _temp89); goto _LL98; _LL98: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp99=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp99[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp100; _temp100.tag= Cyc_CfFlowInfo_TupleF;
_temp100.f1=( int) _temp97; _temp100;}); _temp99;})); goto _LL64;} _LL76:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp101=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp101[ 0]=({ struct
Cyc_Core_Impossible_struct _temp102; _temp102.tag= Cyc_Core_Impossible; _temp102.f1=(
struct _tagged_string)({ char* _temp103=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 44; _temp104;}); _temp102;}); _temp101;}));
_LL64:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp*
rgn_exp, struct Cyc_Absyn_Exp* init_exp){ void* _temp105= Cyc_CfAbsexp_mkMallocOp(
outer_e); goto _LL106; _LL106: { void* _temp107= Cyc_CfAbsexp_mkSkipAE(); goto
_LL108; _LL108: { void* _temp109= Cyc_CfAbsexp_mkSkipAE(); goto _LL110; _LL110:
if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv _temp112= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp111=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp111->hd=( void*) _temp105; _temp111->tl= 0;
_temp111;})); goto _LL113; _LL113: _temp112.access_path= 0;{ void* _temp116;
void* _temp118; struct Cyc_NewControlFlow_AbsSynOne _temp114= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp112,( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( init_exp)))); _LL119: _temp118=( void*) _temp114.inner_exp; goto
_LL117; _LL117: _temp116=( void*) _temp114.assigns; goto _LL115; _LL115:
_temp107= _temp118; _temp109= _temp116;}} if( rgn_exp != 0){ struct Cyc_NewControlFlow_AbsSyn
_temp120= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( rgn_exp))); goto
_LL121; _LL121: { void* _temp124; void* _temp126; struct Cyc_NewControlFlow_AbsSynOne
_temp122= Cyc_NewControlFlow_meet_absexp( _temp120); _LL127: _temp126=( void*)
_temp122.inner_exp; goto _LL125; _LL125: _temp124=( void*) _temp122.assigns;
goto _LL123; _LL123: _temp107= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp107, _temp126); _temp109= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp109, _temp124);}}{ void* _temp128= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp107, _temp109)); goto _LL129; _LL129: { struct Cyc_NewControlFlow_AbsSyn
_temp130= Cyc_NewControlFlow_mkSyn_tf( _temp128, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( _temp105))); goto _LL131; _LL131: if( env.access_path
!= 0){ void* _temp132=( void*)(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( env.access_path))->hd)->r; struct Cyc_Absyn_Exp* _temp140; struct
_tagged_string* _temp142; struct Cyc_Absyn_Exp* _temp144; _LL134: if(*(( int*)
_temp132) == Cyc_Absyn_Deref_e){ _LL141: _temp140=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp132)->f1; goto _LL135;} else{ goto _LL136;}
_LL136: if(*(( int*) _temp132) == Cyc_Absyn_StructArrow_e){ _LL145: _temp144=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp132)->f1;
goto _LL143; _LL143: _temp142=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp132)->f2; goto _LL137;} else{ goto _LL138;} _LL138: goto _LL139; _LL135:
env.access_path=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp130.lvalues=({ struct Cyc_List_List* _temp146=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp146->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp105); _temp146->tl= 0; _temp146;}); goto _LL133; _LL137: env.access_path=({
struct Cyc_List_List* _temp147=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp147->hd=( void*) Cyc_Absyn_structmember_exp(
_temp144, _temp142, 0); _temp147->tl=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp147;}); _temp130.lvalues=({ struct Cyc_List_List* _temp148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp148->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp105); _temp148->tl= 0; _temp148;}); goto _LL133; _LL139:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp149=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp149[ 0]=({ struct
Cyc_Core_Impossible_struct _temp150; _temp150.tag= Cyc_Core_Impossible; _temp150.f1=(
struct _tagged_string)({ char* _temp151=( char*)"bad malloc access path"; struct
_tagged_string _temp152; _temp152.curr= _temp151; _temp152.base= _temp151;
_temp152.last_plus_one= _temp151 + 23; _temp152;}); _temp150;}); _temp149;}));
_LL133:;} return _temp130;}}}}} struct _tuple4{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp153=(
void*) e->r; struct Cyc_Absyn_Exp* _temp241; struct Cyc_Absyn_Exp* _temp243;
struct Cyc_List_List* _temp245; struct Cyc_Absyn_Exp* _temp247; void* _temp249;
int _temp251; void* _temp253; void* _temp255; int _temp257; void* _temp259; void*
_temp261; void* _temp263; struct Cyc_Absyn_Enumfield* _temp265; struct Cyc_Absyn_Enumdecl*
_temp267; struct _tuple1* _temp269; void* _temp271; struct Cyc_Absyn_Fndecl*
_temp273; struct _tuple1* _temp275; void* _temp277; struct Cyc_Absyn_Vardecl*
_temp279; struct _tuple1* _temp281; void* _temp283; struct Cyc_Absyn_Vardecl*
_temp285; struct _tuple1* _temp287; void* _temp289; struct Cyc_Absyn_Vardecl*
_temp291; struct _tuple1* _temp293; void* _temp295; struct Cyc_Absyn_Vardecl*
_temp297; struct _tuple1* _temp299; struct Cyc_Absyn_Exp* _temp301; struct Cyc_Core_Opt*
_temp303; struct Cyc_Absyn_Exp* _temp305; struct Cyc_Absyn_Exp* _temp307; struct
Cyc_Core_Opt* _temp309; struct Cyc_Absyn_Exp* _temp311; struct Cyc_List_List*
_temp313; void* _temp315; struct Cyc_Absyn_Exp* _temp317; void* _temp319; struct
Cyc_List_List* _temp321; struct Cyc_Absyn_Exp* _temp323; struct Cyc_Absyn_Exp*
_temp325; struct Cyc_Absyn_Exp* _temp327; struct Cyc_Absyn_Exp* _temp329; struct
Cyc_Absyn_Exp* _temp331; struct Cyc_Absyn_Exp* _temp333; void* _temp335; struct
Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Exp* _temp339; struct _tagged_string*
_temp341; struct Cyc_Absyn_Exp* _temp343; struct _tagged_string* _temp345;
struct Cyc_Absyn_Exp* _temp347; struct Cyc_Absyn_Tunionfield* _temp349; struct
Cyc_Absyn_Tuniondecl* _temp351; struct Cyc_List_List* _temp353; struct Cyc_Core_Opt*
_temp355; struct Cyc_Core_Opt* _temp357; struct Cyc_List_List* _temp359; struct
Cyc_List_List* _temp361; void* _temp363; struct Cyc_Absyn_Structdecl* _temp365;
struct Cyc_List_List* _temp367; struct Cyc_Core_Opt* _temp369; struct _tuple1*
_temp371; struct Cyc_List_List* _temp373; void* _temp375; struct Cyc_Absyn_Exp*
_temp377; struct Cyc_Absyn_Exp* _temp379; struct Cyc_Absyn_Exp* _temp381; struct
Cyc_Absyn_Exp* _temp383; struct Cyc_Absyn_Exp* _temp385; struct Cyc_Absyn_Stmt*
_temp387; struct Cyc_Absyn_Exp* _temp389; struct Cyc_Absyn_Exp* _temp391; struct
Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Exp* _temp395; struct Cyc_Absyn_Vardecl*
_temp397; void* _temp399; struct _tuple1* _temp401; struct _tuple1* _temp403;
struct Cyc_List_List* _temp405; struct Cyc_Absyn_Exp* _temp407; struct Cyc_List_List*
_temp409; struct Cyc_Core_Opt* _temp411; struct Cyc_List_List* _temp413; struct
_tuple2* _temp415; struct Cyc_Absyn_Fndecl* _temp417; struct Cyc_Absyn_Exp*
_temp419; _LL155: if(*(( int*) _temp153) == Cyc_Absyn_NoInstantiate_e){ _LL242:
_temp241=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp153)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if(*(( int*) _temp153)
== Cyc_Absyn_Sizeofexp_e){ _LL244: _temp243=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Sizeofexp_e_struct*) _temp153)->f1; goto _LL158;} else{ goto _LL159;}
_LL159: if(*(( int*) _temp153) == Cyc_Absyn_Instantiate_e){ _LL248: _temp247=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp153)->f1;
goto _LL246; _LL246: _temp245=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp153)->f2; goto _LL160;} else{ goto _LL161;} _LL161: if(*(( int*) _temp153)
== Cyc_Absyn_Const_e){ _LL250: _temp249=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp153)->f1; if(( unsigned int) _temp249 > 1u?*(( int*) _temp249) == Cyc_Absyn_Int_c:
0){ _LL254: _temp253=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp249)->f1;
if( _temp253 ==( void*) Cyc_Absyn_Signed){ goto _LL252;} else{ goto _LL163;}
_LL252: _temp251=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp249)->f2; if(
_temp251 == 0){ goto _LL162;} else{ goto _LL163;}} else{ goto _LL163;}} else{
goto _LL163;} _LL163: if(*(( int*) _temp153) == Cyc_Absyn_Const_e){ _LL256:
_temp255=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp153)->f1; if((
unsigned int) _temp255 > 1u?*(( int*) _temp255) == Cyc_Absyn_Int_c: 0){ _LL260:
_temp259=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp255)->f1; if( _temp259
==( void*) Cyc_Absyn_Signed){ goto _LL258;} else{ goto _LL165;} _LL258: _temp257=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp255)->f2; if( _temp257 == 1){ goto
_LL164;} else{ goto _LL165;}} else{ goto _LL165;}} else{ goto _LL165;} _LL165:
if(*(( int*) _temp153) == Cyc_Absyn_Sizeoftyp_e){ _LL262: _temp261=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp153)->f1; goto _LL166;} else{ goto
_LL167;} _LL167: if(*(( int*) _temp153) == Cyc_Absyn_Const_e){ _LL264: _temp263=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp153)->f1; goto _LL168;} else{
goto _LL169;} _LL169: if(*(( int*) _temp153) == Cyc_Absyn_Enum_e){ _LL270:
_temp269=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp153)->f1;
goto _LL268; _LL268: _temp267=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp153)->f2; goto _LL266; _LL266: _temp265=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp153)->f3; goto _LL170;} else{ goto _LL171;}
_LL171: if(*(( int*) _temp153) == Cyc_Absyn_Var_e){ _LL276: _temp275=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f1; goto _LL272; _LL272:
_temp271=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f2; if((
unsigned int) _temp271 > 1u?*(( int*) _temp271) == Cyc_Absyn_Funname_b: 0){
_LL274: _temp273=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp271)->f1; goto _LL172;} else{ goto _LL173;}} else{ goto _LL173;} _LL173:
if(*(( int*) _temp153) == Cyc_Absyn_Var_e){ _LL282: _temp281=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp153)->f1; goto _LL278; _LL278: _temp277=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f2; if(( unsigned int)
_temp277 > 1u?*(( int*) _temp277) == Cyc_Absyn_Global_b: 0){ _LL280: _temp279=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp277)->f1;
goto _LL174;} else{ goto _LL175;}} else{ goto _LL175;} _LL175: if(*(( int*)
_temp153) == Cyc_Absyn_Var_e){ _LL288: _temp287=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL284; _LL284: _temp283=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp283 > 1u?*(( int*) _temp283) == Cyc_Absyn_Param_b:
0){ _LL286: _temp285=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp283)->f1; goto _LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177:
if(*(( int*) _temp153) == Cyc_Absyn_Var_e){ _LL294: _temp293=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp153)->f1; goto _LL290; _LL290: _temp289=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f2; if(( unsigned int)
_temp289 > 1u?*(( int*) _temp289) == Cyc_Absyn_Local_b: 0){ _LL292: _temp291=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp289)->f1;
goto _LL178;} else{ goto _LL179;}} else{ goto _LL179;} _LL179: if(*(( int*)
_temp153) == Cyc_Absyn_Var_e){ _LL300: _temp299=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f1; goto _LL296; _LL296: _temp295=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp153)->f2; if(( unsigned int) _temp295 > 1u?*(( int*) _temp295) == Cyc_Absyn_Pat_b:
0){ _LL298: _temp297=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp295)->f1; goto _LL180;} else{ goto _LL181;}} else{ goto _LL181;} _LL181:
if(*(( int*) _temp153) == Cyc_Absyn_AssignOp_e){ _LL306: _temp305=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f1; goto _LL304; _LL304:
_temp303=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f2;
if( _temp303 == 0){ goto _LL302;} else{ goto _LL183;} _LL302: _temp301=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f3; goto
_LL182;} else{ goto _LL183;} _LL183: if(*(( int*) _temp153) == Cyc_Absyn_AssignOp_e){
_LL312: _temp311=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp153)->f1; goto _LL310; _LL310: _temp309=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp153)->f2; goto _LL308; _LL308: _temp307=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp153)->f3;
goto _LL184;} else{ goto _LL185;} _LL185: if(*(( int*) _temp153) == Cyc_Absyn_Primop_e){
_LL316: _temp315=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp153)->f1;
goto _LL314; _LL314: _temp313=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp153)->f2; goto _LL186;} else{ goto _LL187;} _LL187: if(*(( int*) _temp153)
== Cyc_Absyn_Cast_e){ _LL320: _temp319=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp153)->f1; goto _LL318; _LL318: _temp317=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp153)->f2; goto _LL188;} else{ goto _LL189;}
_LL189: if(*(( int*) _temp153) == Cyc_Absyn_FnCall_e){ _LL324: _temp323=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp153)->f1; goto _LL322;
_LL322: _temp321=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp153)->f2; goto _LL190;} else{ goto _LL191;} _LL191: if(*(( int*) _temp153)
== Cyc_Absyn_Subscript_e){ _LL328: _temp327=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp153)->f1; goto _LL326; _LL326: _temp325=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp153)->f2;
goto _LL192;} else{ goto _LL193;} _LL193: if(*(( int*) _temp153) == Cyc_Absyn_Address_e){
_LL330: _temp329=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp153)->f1; goto _LL194;} else{ goto _LL195;} _LL195: if(*(( int*) _temp153)
== Cyc_Absyn_New_e){ _LL334: _temp333=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp153)->f1; goto _LL332; _LL332: _temp331=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp153)->f2; goto _LL196;} else{ goto _LL197;} _LL197:
if(*(( int*) _temp153) == Cyc_Absyn_Malloc_e){ _LL338: _temp337=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp153)->f1; goto _LL336; _LL336: _temp335=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp153)->f2; goto _LL198;} else{
goto _LL199;} _LL199: if(*(( int*) _temp153) == Cyc_Absyn_Deref_e){ _LL340:
_temp339=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp153)->f1;
goto _LL200;} else{ goto _LL201;} _LL201: if(*(( int*) _temp153) == Cyc_Absyn_StructArrow_e){
_LL344: _temp343=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp153)->f1; goto _LL342; _LL342: _temp341=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp153)->f2; goto _LL202;} else{ goto _LL203;}
_LL203: if(*(( int*) _temp153) == Cyc_Absyn_StructMember_e){ _LL348: _temp347=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp153)->f1;
goto _LL346; _LL346: _temp345=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp153)->f2; goto _LL204;} else{ goto _LL205;} _LL205: if(*(( int*) _temp153)
== Cyc_Absyn_Tunion_e){ _LL358: _temp357=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp153)->f1; goto _LL356; _LL356: _temp355=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp153)->f2; goto _LL354; _LL354: _temp353=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f3; goto
_LL352; _LL352: _temp351=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp153)->f4; goto _LL350; _LL350: _temp349=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp153)->f5; goto _LL206;} else{ goto
_LL207;} _LL207: if(*(( int*) _temp153) == Cyc_Absyn_Tuple_e){ _LL360: _temp359=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp153)->f1; goto
_LL208;} else{ goto _LL209;} _LL209: if(*(( int*) _temp153) == Cyc_Absyn_AnonStruct_e){
_LL364: _temp363=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp153)->f1;
goto _LL362; _LL362: _temp361=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp153)->f2; goto _LL210;} else{ goto _LL211;} _LL211: if(*(( int*) _temp153)
== Cyc_Absyn_Struct_e){ _LL372: _temp371=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp153)->f1; goto _LL370; _LL370: _temp369=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp153)->f2; goto _LL368; _LL368: _temp367=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp153)->f3; goto
_LL366; _LL366: _temp365=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp153)->f4; goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp153)
== Cyc_Absyn_Array_e){ _LL374: _temp373=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp153)->f1; goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp153)
== Cyc_Absyn_Increment_e){ _LL378: _temp377=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Increment_e_struct*) _temp153)->f1; goto _LL376; _LL376: _temp375=(
void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp153)->f2; goto _LL216;}
else{ goto _LL217;} _LL217: if(*(( int*) _temp153) == Cyc_Absyn_Throw_e){ _LL380:
_temp379=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp153)->f1;
goto _LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp153) == Cyc_Absyn_Conditional_e){
_LL386: _temp385=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp153)->f1; goto _LL384; _LL384: _temp383=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp153)->f2; goto _LL382; _LL382: _temp381=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp153)->f3;
goto _LL220;} else{ goto _LL221;} _LL221: if(*(( int*) _temp153) == Cyc_Absyn_StmtExp_e){
_LL388: _temp387=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp153)->f1; goto _LL222;} else{ goto _LL223;} _LL223: if(*(( int*) _temp153)
== Cyc_Absyn_SeqExp_e){ _LL392: _temp391=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp153)->f1; goto _LL390; _LL390: _temp389=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp153)->f2; goto _LL224;} else{ goto _LL225;}
_LL225: if(*(( int*) _temp153) == Cyc_Absyn_Comprehension_e){ _LL398: _temp397=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp153)->f1;
goto _LL396; _LL396: _temp395=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp153)->f2; goto _LL394; _LL394: _temp393=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp153)->f3; goto _LL226;} else{ goto
_LL227;} _LL227: if(*(( int*) _temp153) == Cyc_Absyn_Var_e){ _LL402: _temp401=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f1; goto _LL400;
_LL400: _temp399=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp153)->f2; if(
_temp399 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL228;} else{ goto _LL229;}}
else{ goto _LL229;} _LL229: if(*(( int*) _temp153) == Cyc_Absyn_UnknownId_e){
_LL404: _temp403=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp153)->f1; goto _LL230;} else{ goto _LL231;} _LL231: if(*(( int*) _temp153)
== Cyc_Absyn_UnknownCall_e){ _LL408: _temp407=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp153)->f1; goto _LL406; _LL406: _temp405=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp153)->f2;
goto _LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp153) == Cyc_Absyn_UnresolvedMem_e){
_LL412: _temp411=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp153)->f1; goto _LL410; _LL410: _temp409=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp153)->f2; goto _LL234;} else{ goto
_LL235;} _LL235: if(*(( int*) _temp153) == Cyc_Absyn_CompoundLit_e){ _LL416:
_temp415=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp153)->f1;
goto _LL414; _LL414: _temp413=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp153)->f2; goto _LL236;} else{ goto _LL237;} _LL237: if(*(( int*) _temp153)
== Cyc_Absyn_Codegen_e){ _LL418: _temp417=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp153)->f1; goto _LL238;} else{ goto _LL239;}
_LL239: if(*(( int*) _temp153) == Cyc_Absyn_Fill_e){ _LL420: _temp419=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp153)->f1; goto _LL240;}
else{ goto _LL154;} _LL156: _temp243= _temp241; goto _LL158; _LL158: _temp247=
_temp243; goto _LL160; _LL160: return Cyc_NewControlFlow_abstract_exp( env,
_temp247); _LL162: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp421=( char*)"new_control_flow.cyc:284 env.access_path==null";
struct _tagged_string _temp422; _temp422.curr= _temp421; _temp422.base= _temp421;
_temp422.last_plus_one= _temp421 + 47; _temp422;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp423; _temp423.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp425;
_temp425.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp425.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp425;}); _temp423.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp424; _temp424.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp424.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp424;}); _temp423.lvalues= 0; _temp423;}); _LL164: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp426=( char*)"new_control_flow.cyc:289 env.access_path==null";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 47; _temp427;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp428; _temp428.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp430;
_temp430.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp430.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp430;});
_temp428.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp429; _temp429.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp429.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp429;}); _temp428.lvalues= 0; _temp428;}); _LL166: goto _LL168; _LL168: goto
_LL170; _LL170: goto _LL172; _LL172: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL174:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL176:
_temp291= _temp285; goto _LL178; _LL178: _temp297= _temp291; goto _LL180; _LL180: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp297)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp431=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp431->hd=( void*) absop; _temp431->tl= 0; _temp431;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL182: { struct Cyc_NewControlFlow_AbsSyn
_temp432= Cyc_NewControlFlow_abstract_exp( env, _temp305); goto _LL433; _LL433: {
struct Cyc_List_List* _temp434= _temp432.lvalues == 0? Cyc_NewControlFlow_use_it_list():
_temp432.lvalues; goto _LL435; _LL435: { struct Cyc_NewControlFlow_AbsSyn
_temp436= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp434), _temp301); goto _LL437; _LL437: { void* _temp440; void* _temp442;
struct Cyc_NewControlFlow_AbsSynOne _temp438= Cyc_NewControlFlow_meet_absexp(
_temp432); _LL443: _temp442=( void*) _temp438.inner_exp; goto _LL441; _LL441:
_temp440=( void*) _temp438.assigns; goto _LL439; _LL439: { void* _temp446; void*
_temp448; struct Cyc_NewControlFlow_AbsSynOne _temp444= Cyc_NewControlFlow_meet_absexp(
_temp436); _LL449: _temp448=( void*) _temp444.inner_exp; goto _LL447; _LL447:
_temp446=( void*) _temp444.assigns; goto _LL445; _LL445: { void* _temp450= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp442, _temp448), _temp446); goto _LL451; _LL451: return Cyc_NewControlFlow_mkSyn_tf(
_temp450, 0, _temp440);}}}}}} _LL184: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp452=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp452->hd=( void*) _temp311; _temp452->tl=({
struct Cyc_List_List* _temp453=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp453->hd=( void*) _temp307; _temp453->tl= 0;
_temp453;}); _temp452;}), 0); _LL186: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp313, 0); _LL188: { int both_leaf= 1;{ void* _temp454=( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp317->topt))->v; struct Cyc_List_List* _temp464;
struct Cyc_Absyn_Structdecl** _temp466; struct Cyc_List_List* _temp468; struct
_tuple1* _temp470; struct Cyc_List_List* _temp472; _LL456: if(( unsigned int)
_temp454 > 4u?*(( int*) _temp454) == Cyc_Absyn_AnonStructType: 0){ _LL465:
_temp464=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp454)->f1; goto _LL457;} else{ goto _LL458;} _LL458: if(( unsigned int)
_temp454 > 4u?*(( int*) _temp454) == Cyc_Absyn_StructType: 0){ _LL471: _temp470=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp454)->f1; goto
_LL469; _LL469: _temp468=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp454)->f2; goto _LL467; _LL467: _temp466=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp454)->f3; goto _LL459;} else{ goto
_LL460;} _LL460: if(( unsigned int) _temp454 > 4u?*(( int*) _temp454) == Cyc_Absyn_TupleType:
0){ _LL473: _temp472=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp454)->f1; goto _LL461;} else{ goto _LL462;} _LL462: goto _LL463; _LL457:
goto _LL459; _LL459: goto _LL461; _LL461: both_leaf= 0; goto _LL455; _LL463:
goto _LL455; _LL455:;}{ void* _temp474=( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v; struct Cyc_List_List* _temp484; struct Cyc_Absyn_Structdecl**
_temp486; struct Cyc_List_List* _temp488; struct _tuple1* _temp490; struct Cyc_List_List*
_temp492; _LL476: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_AnonStructType:
0){ _LL485: _temp484=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp474)->f1; goto _LL477;} else{ goto _LL478;} _LL478: if(( unsigned int)
_temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_StructType: 0){ _LL491: _temp490=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp474)->f1; goto
_LL489; _LL489: _temp488=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp474)->f2; goto _LL487; _LL487: _temp486=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp474)->f3; goto _LL479;} else{ goto
_LL480;} _LL480: if(( unsigned int) _temp474 > 4u?*(( int*) _temp474) == Cyc_Absyn_TupleType:
0){ _LL493: _temp492=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp474)->f1; goto _LL481;} else{ goto _LL482;} _LL482: goto _LL483; _LL477:
goto _LL479; _LL479: goto _LL481; _LL481: both_leaf= 0; goto _LL475; _LL483:
goto _LL475; _LL475:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp(
env, _temp317);} return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp494=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp494->hd=( void*) _temp317; _temp494->tl= 0; _temp494;}), env.lhsides);}
_LL190: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp495=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp495->hd=( void*) _temp323; _temp495->tl= _temp321; _temp495;}), 0); _LL192: {
void* _temp496= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp327->topt))->v); struct Cyc_List_List* _temp502; _LL498: if(( unsigned int)
_temp496 > 4u?*(( int*) _temp496) == Cyc_Absyn_TupleType: 0){ _LL503: _temp502=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp496)->f1; goto
_LL499;} else{ goto _LL500;} _LL500: goto _LL501; _LL499: return Cyc_NewControlFlow_descend_path(
env, e, _temp327); _LL501: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp504=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp504->hd=( void*) _temp327; _temp504->tl=({ struct
Cyc_List_List* _temp505=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp505->hd=( void*) _temp325; _temp505->tl= 0; _temp505;}); _temp504;}), Cyc_NewControlFlow_use_it_list());
_LL497:;} _LL194: { int is_malloc= 0;{ void* _temp506=( void*) _temp329->r;
struct Cyc_Absyn_Structdecl* _temp514; struct Cyc_List_List* _temp516; struct
Cyc_Core_Opt* _temp518; struct _tuple1* _temp520; struct Cyc_List_List* _temp522;
_LL508: if(*(( int*) _temp506) == Cyc_Absyn_Struct_e){ _LL521: _temp520=( struct
_tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp506)->f1; goto _LL519;
_LL519: _temp518=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp506)->f2; goto _LL517; _LL517: _temp516=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp506)->f3; goto _LL515; _LL515: _temp514=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp506)->f4;
goto _LL509;} else{ goto _LL510;} _LL510: if(*(( int*) _temp506) == Cyc_Absyn_Tuple_e){
_LL523: _temp522=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp506)->f1; goto _LL511;} else{ goto _LL512;} _LL512: goto _LL513; _LL509:
goto _LL511; _LL511: is_malloc= 1; goto _LL507; _LL513: goto _LL507; _LL507:;}
if( is_malloc){ _temp333= 0; _temp331= _temp329; goto _LL196;}{ struct Cyc_List_List*
_temp524= env.access_path; struct Cyc_List_List _temp536; struct Cyc_List_List*
_temp537; struct Cyc_Absyn_Exp* _temp539; struct Cyc_Absyn_Exp _temp541; struct
Cyc_Position_Segment* _temp542; void* _temp544; struct Cyc_Absyn_Exp* _temp546;
struct Cyc_Absyn_Exp* _temp548; struct Cyc_Core_Opt* _temp550; struct Cyc_List_List
_temp552; struct Cyc_List_List* _temp553; struct Cyc_Absyn_Exp* _temp555; struct
Cyc_Absyn_Exp _temp557; struct Cyc_Position_Segment* _temp558; void* _temp560;
struct Cyc_Absyn_Exp* _temp562; struct Cyc_Core_Opt* _temp564; struct Cyc_List_List
_temp566; struct Cyc_List_List* _temp567; struct Cyc_Absyn_Exp* _temp569; struct
Cyc_Absyn_Exp _temp571; struct Cyc_Position_Segment* _temp572; void* _temp574;
struct _tagged_string* _temp576; struct Cyc_Absyn_Exp* _temp578; struct Cyc_Core_Opt*
_temp580; _LL526: if( _temp524 == 0){ goto _LL527;} else{ goto _LL528;} _LL528:
if( _temp524 == 0){ goto _LL530;} else{ _temp536=* _temp524; _LL540: _temp539=(
struct Cyc_Absyn_Exp*) _temp536.hd; _temp541=* _temp539; _LL551: _temp550=(
struct Cyc_Core_Opt*) _temp541.topt; goto _LL545; _LL545: _temp544=( void*)
_temp541.r; if(*(( int*) _temp544) == Cyc_Absyn_Subscript_e){ _LL549: _temp548=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp544)->f1;
goto _LL547; _LL547: _temp546=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp544)->f2; goto _LL543;} else{ goto _LL530;} _LL543: _temp542=( struct Cyc_Position_Segment*)
_temp541.loc; goto _LL538; _LL538: _temp537=( struct Cyc_List_List*) _temp536.tl;
goto _LL529;} _LL530: if( _temp524 == 0){ goto _LL532;} else{ _temp552=*
_temp524; _LL556: _temp555=( struct Cyc_Absyn_Exp*) _temp552.hd; _temp557=*
_temp555; _LL565: _temp564=( struct Cyc_Core_Opt*) _temp557.topt; goto _LL561;
_LL561: _temp560=( void*) _temp557.r; if(*(( int*) _temp560) == Cyc_Absyn_Deref_e){
_LL563: _temp562=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp560)->f1; goto _LL559;} else{ goto _LL532;} _LL559: _temp558=( struct Cyc_Position_Segment*)
_temp557.loc; goto _LL554; _LL554: _temp553=( struct Cyc_List_List*) _temp552.tl;
goto _LL531;} _LL532: if( _temp524 == 0){ goto _LL534;} else{ _temp566=*
_temp524; _LL570: _temp569=( struct Cyc_Absyn_Exp*) _temp566.hd; _temp571=*
_temp569; _LL581: _temp580=( struct Cyc_Core_Opt*) _temp571.topt; goto _LL575;
_LL575: _temp574=( void*) _temp571.r; if(*(( int*) _temp574) == Cyc_Absyn_StructArrow_e){
_LL579: _temp578=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp574)->f1; goto _LL577; _LL577: _temp576=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp574)->f2; goto _LL573;} else{ goto _LL534;}
_LL573: _temp572=( struct Cyc_Position_Segment*) _temp571.loc; goto _LL568;
_LL568: _temp567=( struct Cyc_List_List*) _temp566.tl; goto _LL533;} _LL534:
goto _LL535; _LL527: env.access_path=({ struct Cyc_List_List* _temp582=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp582->hd=( void*)
e; _temp582->tl= env.access_path; _temp582;}); goto _LL525; _LL529: Cyc_Evexp_eval_const_uint_exp(
_temp546) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp583=( char*)"new_control_flow.cyc:410 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 61; _temp584;})); _temp553= _temp537; goto
_LL531; _LL531: env.access_path= _temp553; goto _LL525; _LL533: env.access_path=({
struct Cyc_List_List* _temp585=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp585->hd=( void*) Cyc_Absyn_structmember_exp(
_temp578, _temp576, 0); _temp585->tl= _temp567; _temp585;}); goto _LL525; _LL535:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp586=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp586[ 0]=({ struct
Cyc_Core_Impossible_struct _temp587; _temp587.tag= Cyc_Core_Impossible; _temp587.f1=(
struct _tagged_string)({ char* _temp588=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp589; _temp589.curr= _temp588; _temp589.base= _temp588;
_temp589.last_plus_one= _temp588 + 36; _temp589;}); _temp587;}); _temp586;}));
_LL525:;} return Cyc_NewControlFlow_abstract_exp( env, _temp329);} _LL196: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp331->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, _temp333,( struct Cyc_Absyn_Exp*)
_temp331); _LL198: Cyc_NewControlFlow_add_malloc_root( env, e, _temp335); return
Cyc_NewControlFlow_abstract_malloc( env, e, _temp337, 0); _LL200: { struct Cyc_List_List*
_temp590= env.access_path; struct Cyc_List_List _temp596; struct Cyc_List_List*
_temp597; struct Cyc_Absyn_Exp* _temp599; struct Cyc_Absyn_Exp _temp601; struct
Cyc_Position_Segment* _temp602; void* _temp604; struct Cyc_Absyn_Exp* _temp606;
struct Cyc_Core_Opt* _temp608; _LL592: if( _temp590 == 0){ goto _LL594;} else{
_temp596=* _temp590; _LL600: _temp599=( struct Cyc_Absyn_Exp*) _temp596.hd;
_temp601=* _temp599; _LL609: _temp608=( struct Cyc_Core_Opt*) _temp601.topt;
goto _LL605; _LL605: _temp604=( void*) _temp601.r; if(*(( int*) _temp604) == Cyc_Absyn_Address_e){
_LL607: _temp606=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp604)->f1; goto _LL603;} else{ goto _LL594;} _LL603: _temp602=( struct Cyc_Position_Segment*)
_temp601.loc; goto _LL598; _LL598: _temp597=( struct Cyc_List_List*) _temp596.tl;
goto _LL593;} _LL594: goto _LL595; _LL593: env.access_path= _temp597;{ struct
Cyc_NewControlFlow_AbsSyn _temp610= Cyc_NewControlFlow_abstract_exp( env,
_temp339); goto _LL611; _LL611: if( _temp610.lvalues == 0){ _temp610.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp610;} _LL595: return Cyc_NewControlFlow_descend_path(
env, e, _temp339); _LL591:;} _LL202: { void* _temp612= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp343->topt))->v); struct Cyc_Absyn_PtrInfo
_temp618; struct Cyc_Absyn_Conref* _temp620; struct Cyc_Absyn_Tqual _temp622;
struct Cyc_Absyn_Conref* _temp624; void* _temp626; void* _temp628; _LL614: if((
unsigned int) _temp612 > 4u?*(( int*) _temp612) == Cyc_Absyn_PointerType: 0){
_LL619: _temp618=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp612)->f1; _LL629: _temp628=( void*) _temp618.elt_typ; goto _LL627; _LL627:
_temp626=( void*) _temp618.rgn_typ; goto _LL625; _LL625: _temp624=( struct Cyc_Absyn_Conref*)
_temp618.nullable; goto _LL623; _LL623: _temp622=( struct Cyc_Absyn_Tqual)
_temp618.tq; goto _LL621; _LL621: _temp620=( struct Cyc_Absyn_Conref*) _temp618.bounds;
goto _LL615;} else{ goto _LL616;} _LL616: goto _LL617; _LL615:{ void* _temp630=
Cyc_Tcutil_compress( _temp628); struct Cyc_List_List* _temp642; struct Cyc_Absyn_Structdecl**
_temp644; struct Cyc_List_List* _temp646; struct _tuple1* _temp648; struct Cyc_List_List*
_temp650; struct Cyc_Absyn_Uniondecl** _temp652; struct Cyc_List_List* _temp654;
struct _tuple1* _temp656; _LL632: if(( unsigned int) _temp630 > 4u?*(( int*)
_temp630) == Cyc_Absyn_AnonStructType: 0){ _LL643: _temp642=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp630)->f1; goto _LL633;} else{ goto
_LL634;} _LL634: if(( unsigned int) _temp630 > 4u?*(( int*) _temp630) == Cyc_Absyn_StructType:
0){ _LL649: _temp648=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp630)->f1; goto _LL647; _LL647: _temp646=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp630)->f2; goto _LL645; _LL645: _temp644=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp630)->f3;
goto _LL635;} else{ goto _LL636;} _LL636: if(( unsigned int) _temp630 > 4u?*((
int*) _temp630) == Cyc_Absyn_AnonUnionType: 0){ _LL651: _temp650=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp630)->f1; goto _LL637;} else{ goto
_LL638;} _LL638: if(( unsigned int) _temp630 > 4u?*(( int*) _temp630) == Cyc_Absyn_UnionType:
0){ _LL657: _temp656=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp630)->f1; goto _LL655; _LL655: _temp654=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp630)->f2; goto _LL653; _LL653: _temp652=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp630)->f3;
goto _LL639;} else{ goto _LL640;} _LL640: goto _LL641; _LL633: goto _LL635;
_LL635: return Cyc_NewControlFlow_descend_path( env, e, _temp343); _LL637: goto
_LL639; _LL639: { struct Cyc_NewControlFlow_AbsSyn _temp658= Cyc_NewControlFlow_abstract_exp(
env, _temp343); goto _LL659; _LL659: _temp658.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp658;} _LL641: goto _LL631; _LL631:;} goto _LL617; _LL617:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp660=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp660[ 0]=({ struct
Cyc_Core_Impossible_struct _temp661; _temp661.tag= Cyc_Core_Impossible; _temp661.f1=(
struct _tagged_string)({ char* _temp662=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp663; _temp663.curr= _temp662; _temp663.base= _temp662;
_temp663.last_plus_one= _temp662 + 40; _temp663;}); _temp661;}); _temp660;}));
_LL613:;} _LL204: { void* _temp664= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp347->topt))->v); struct Cyc_List_List* _temp676; struct Cyc_Absyn_Structdecl**
_temp678; struct Cyc_List_List* _temp680; struct _tuple1* _temp682; struct Cyc_List_List*
_temp684; struct Cyc_Absyn_Uniondecl** _temp686; struct Cyc_List_List* _temp688;
struct _tuple1* _temp690; _LL666: if(( unsigned int) _temp664 > 4u?*(( int*)
_temp664) == Cyc_Absyn_AnonStructType: 0){ _LL677: _temp676=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp664)->f1; goto _LL667;} else{ goto
_LL668;} _LL668: if(( unsigned int) _temp664 > 4u?*(( int*) _temp664) == Cyc_Absyn_StructType:
0){ _LL683: _temp682=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp664)->f1; goto _LL681; _LL681: _temp680=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp664)->f2; goto _LL679; _LL679: _temp678=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp664)->f3;
goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int) _temp664 > 4u?*((
int*) _temp664) == Cyc_Absyn_AnonUnionType: 0){ _LL685: _temp684=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp664)->f1; goto _LL671;} else{ goto
_LL672;} _LL672: if(( unsigned int) _temp664 > 4u?*(( int*) _temp664) == Cyc_Absyn_UnionType:
0){ _LL691: _temp690=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp664)->f1; goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp664)->f2; goto _LL687; _LL687: _temp686=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp664)->f3;
goto _LL673;} else{ goto _LL674;} _LL674: goto _LL675; _LL667: goto _LL669;
_LL669: return Cyc_NewControlFlow_descend_path( env, e, _temp347); _LL671: goto
_LL673; _LL673: { struct Cyc_NewControlFlow_AbsSyn _temp692= Cyc_NewControlFlow_abstract_exp(
env, _temp347); goto _LL693; _LL693: _temp692.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp692;} _LL675:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp694=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp694[ 0]=({ struct Cyc_Core_Impossible_struct _temp695; _temp695.tag= Cyc_Core_Impossible;
_temp695.f1=( struct _tagged_string)({ char* _temp696=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 41; _temp697;}); _temp695;}); _temp694;}));
_LL665:;} _LL206: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp698=( char*)"new_control_flow.cyc:480 env.access_path == null";
struct _tagged_string _temp699; _temp699.curr= _temp698; _temp699.base= _temp698;
_temp699.last_plus_one= _temp698 + 49; _temp699;})); if( _temp353 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));} _temp359=
_temp353; goto _LL208; _LL208: { struct Cyc_List_List* _temp700= env.access_path;
struct Cyc_List_List _temp708; struct Cyc_List_List* _temp709; struct Cyc_Absyn_Exp*
_temp711; struct Cyc_Absyn_Exp _temp713; struct Cyc_Position_Segment* _temp714;
void* _temp716; struct Cyc_Absyn_Exp* _temp718; struct Cyc_Absyn_Exp* _temp720;
struct Cyc_Core_Opt* _temp722; _LL702: if( _temp700 == 0){ goto _LL703;} else{
goto _LL704;} _LL704: if( _temp700 == 0){ goto _LL706;} else{ _temp708=*
_temp700; _LL712: _temp711=( struct Cyc_Absyn_Exp*) _temp708.hd; _temp713=*
_temp711; _LL723: _temp722=( struct Cyc_Core_Opt*) _temp713.topt; goto _LL717;
_LL717: _temp716=( void*) _temp713.r; if(*(( int*) _temp716) == Cyc_Absyn_Subscript_e){
_LL721: _temp720=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp716)->f1; goto _LL719; _LL719: _temp718=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp716)->f2; goto _LL715;} else{ goto _LL706;}
_LL715: _temp714=( struct Cyc_Position_Segment*) _temp713.loc; goto _LL710;
_LL710: _temp709=( struct Cyc_List_List*) _temp708.tl; goto _LL705;} _LL706:
goto _LL707; _LL703: { void* _temp724= Cyc_CfAbsexp_mkSkipAE(); goto _LL725;
_LL725: { void* _temp726= Cyc_CfAbsexp_mkSkipAE(); goto _LL727; _LL727:{ int j=
0; for( 0; _temp359 != 0;( _temp359=(( struct Cyc_List_List*) _check_null(
_temp359))->tl, ++ j)){ struct Cyc_List_List* _temp728= 0; goto _LL729; _LL729:{
struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=(( struct Cyc_List_List*)
_check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)(( struct Cyc_List_List*)
_check_null( ls))->hd)){ _temp728=({ struct Cyc_List_List* _temp730=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp730->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp730->tl= _temp728; _temp730;});} else{ _temp728=({ struct Cyc_List_List*
_temp731=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp731->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*)(( struct Cyc_List_List*)
_check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp732=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp732[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp733; _temp733.tag= Cyc_CfFlowInfo_TupleF;
_temp733.f1= j; _temp733;}); _temp732;})); _temp731->tl= _temp728; _temp731;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp734= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp728),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp359))->hd); goto _LL735; _LL735: { void* _temp738; void* _temp740; struct
Cyc_NewControlFlow_AbsSynOne _temp736= Cyc_NewControlFlow_meet_absexp( _temp734);
_LL741: _temp740=( void*) _temp736.inner_exp; goto _LL739; _LL739: _temp738=(
void*) _temp736.assigns; goto _LL737; _LL737: _temp724= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp724, _temp740); _temp726= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp726, _temp738);}}}} return Cyc_NewControlFlow_mkSyn_tf(
_temp724, 0, _temp726);}} _LL705: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp718); struct Cyc_NewControlFlow_AbsEnv _temp742= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL743; _LL743: _temp742.access_path= 0; env.access_path= _temp709;{
void* _temp744= Cyc_CfAbsexp_mkSkipAE(); goto _LL745; _LL745: { void* _temp746=
Cyc_CfAbsexp_mkSkipAE(); goto _LL747; _LL747:{ int j= 0; for( 0; _temp359 != 0;(
_temp359=(( struct Cyc_List_List*) _check_null( _temp359))->tl, ++ j)){ if( i ==
j){ void* _temp750; void* _temp752; struct Cyc_NewControlFlow_AbsSynOne _temp748=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp359))->hd)); _LL753:
_temp752=( void*) _temp748.inner_exp; goto _LL751; _LL751: _temp750=( void*)
_temp748.assigns; goto _LL749; _LL749: _temp746= _temp750; _temp744= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp744, _temp752);} else{ void* _temp756; void*
_temp758; struct Cyc_NewControlFlow_AbsSynOne _temp754= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp742,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp359))->hd)); _LL759: _temp758=( void*) _temp754.inner_exp;
goto _LL757; _LL757: _temp756=( void*) _temp754.assigns; goto _LL755; _LL755:
_temp744= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp744,
_temp758);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp744,({ struct Cyc_List_List*
_temp760=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp760->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp760->tl= 0; _temp760;}),
_temp746);}}} _LL707:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp761=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp761[ 0]=({ struct Cyc_Core_Impossible_struct _temp762; _temp762.tag= Cyc_Core_Impossible;
_temp762.f1=( struct _tagged_string)({ char* _temp763=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp764; _temp764.curr= _temp763; _temp764.base= _temp763;
_temp764.last_plus_one= _temp763 + 46; _temp764;}); _temp762;}); _temp761;}));
_LL701:;} _LL210: _temp367= _temp361; goto _LL212; _LL212: { struct Cyc_List_List*
_temp765= env.access_path; struct Cyc_List_List _temp773; struct Cyc_List_List*
_temp774; struct Cyc_Absyn_Exp* _temp776; struct Cyc_Absyn_Exp _temp778; struct
Cyc_Position_Segment* _temp779; void* _temp781; struct _tagged_string* _temp783;
struct Cyc_Absyn_Exp* _temp785; struct Cyc_Core_Opt* _temp787; _LL767: if(
_temp765 == 0){ goto _LL768;} else{ goto _LL769;} _LL769: if( _temp765 == 0){
goto _LL771;} else{ _temp773=* _temp765; _LL777: _temp776=( struct Cyc_Absyn_Exp*)
_temp773.hd; _temp778=* _temp776; _LL788: _temp787=( struct Cyc_Core_Opt*)
_temp778.topt; goto _LL782; _LL782: _temp781=( void*) _temp778.r; if(*(( int*)
_temp781) == Cyc_Absyn_StructMember_e){ _LL786: _temp785=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp781)->f1; goto _LL784; _LL784:
_temp783=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp781)->f2; goto _LL780;} else{ goto _LL771;} _LL780: _temp779=( struct Cyc_Position_Segment*)
_temp778.loc; goto _LL775; _LL775: _temp774=( struct Cyc_List_List*) _temp773.tl;
goto _LL770;} _LL771: goto _LL772; _LL768: { void* _temp789= Cyc_CfAbsexp_mkSkipAE();
goto _LL790; _LL790: { void* _temp791= Cyc_CfAbsexp_mkSkipAE(); goto _LL792;
_LL792: for( 0; _temp367 != 0; _temp367=(( struct Cyc_List_List*) _check_null(
_temp367))->tl){ struct Cyc_List_List* _temp793= 0; goto _LL794; _LL794:{ struct
Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=(( struct Cyc_List_List*)
_check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)(( struct Cyc_List_List*)
_check_null( ls))->hd)){ _temp793=({ struct Cyc_List_List* _temp795=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp795->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp795->tl= _temp793; _temp795;});} else{ struct Cyc_List_List* ds=(*(( struct
_tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f1; for( 0; ds
!= 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp796=( void*)((
struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp* _temp802;
struct _tagged_string* _temp804; _LL798: if(*(( int*) _temp796) == Cyc_Absyn_ArrayElement){
_LL803: _temp802=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp796)->f1; goto _LL799;} else{ goto _LL800;} _LL800: if(*(( int*) _temp796)
== Cyc_Absyn_FieldName){ _LL805: _temp804=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp796)->f1; goto _LL801;} else{ goto _LL797;} _LL799:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp806=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp806[ 0]=({ struct
Cyc_Core_Impossible_struct _temp807; _temp807.tag= Cyc_Core_Impossible; _temp807.f1=(
struct _tagged_string)({ char* _temp808=( char*)"bad struct designator"; struct
_tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 22; _temp809;}); _temp807;}); _temp806;}));
_LL801: _temp793=({ struct Cyc_List_List* _temp810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp810->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd,( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp811=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp811[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp812; _temp812.tag= Cyc_CfFlowInfo_StructF;
_temp812.f1= _temp804; _temp812;}); _temp811;})); _temp810->tl= _temp793;
_temp810;}); goto _LL797; _LL797:;}}}}{ void** _temp813=( void**)(( void*)&((
struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp367))->hd)).f2)->topt))->v); goto _LL814; _LL814: { struct Cyc_NewControlFlow_AbsSyn
_temp815= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp793),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f2);
goto _LL816; _LL816: { void* _temp819; void* _temp821; struct Cyc_NewControlFlow_AbsSynOne
_temp817= Cyc_NewControlFlow_meet_absexp( _temp815); _LL822: _temp821=( void*)
_temp817.inner_exp; goto _LL820; _LL820: _temp819=( void*) _temp817.assigns;
goto _LL818; _LL818: _temp789= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp789, _temp821); _temp791= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp791, _temp819);}}}} return Cyc_NewControlFlow_mkSyn_tf( _temp789, 0,
_temp791);}} _LL770: { struct Cyc_NewControlFlow_AbsEnv _temp823= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL824; _LL824: _temp823.access_path= 0; env.access_path= _temp774;{
void* _temp825= Cyc_CfAbsexp_mkSkipAE(); goto _LL826; _LL826: { void* _temp827=
Cyc_CfAbsexp_mkSkipAE(); goto _LL828; _LL828: for( 0; _temp367 != 0; _temp367=((
struct Cyc_List_List*) _check_null( _temp367))->tl){ int used= 0;{ struct Cyc_List_List*
ds=(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f1;
for( 0; ds != 0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void*
_temp829=( void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct Cyc_Absyn_Exp*
_temp835; struct _tagged_string* _temp837; _LL831: if(*(( int*) _temp829) == Cyc_Absyn_ArrayElement){
_LL836: _temp835=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp829)->f1; goto _LL832;} else{ goto _LL833;} _LL833: if(*(( int*) _temp829)
== Cyc_Absyn_FieldName){ _LL838: _temp837=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp829)->f1; goto _LL834;} else{ goto _LL830;} _LL832:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp839=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp839[ 0]=({ struct
Cyc_Core_Impossible_struct _temp840; _temp840.tag= Cyc_Core_Impossible; _temp840.f1=(
struct _tagged_string)({ char* _temp841=( char*)"bad struct designator"; struct
_tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 22; _temp842;}); _temp840;}); _temp839;}));
_LL834: if( Cyc_String_zstrptrcmp( _temp783, _temp837) == 0){ used= 1;} goto
_LL830; _LL830:;}} if( used){ void* _temp845; void* _temp847; struct Cyc_NewControlFlow_AbsSynOne
_temp843= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp367))->hd)).f2));
_LL848: _temp847=( void*) _temp843.inner_exp; goto _LL846; _LL846: _temp845=(
void*) _temp843.assigns; goto _LL844; _LL844: _temp827= _temp845; _temp825= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp825, _temp847);} else{ void* _temp851; void*
_temp853; struct Cyc_NewControlFlow_AbsSynOne _temp849= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp823,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp367))->hd)).f2)); _LL854: _temp853=( void*) _temp849.inner_exp;
goto _LL852; _LL852: _temp851=( void*) _temp849.assigns; goto _LL850; _LL850:
_temp825= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp825,
_temp853);}} return Cyc_NewControlFlow_mkSyn_tf( _temp825, 0, _temp827);}}}
_LL772:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp855=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp855[ 0]=({ struct Cyc_Core_Impossible_struct _temp856; _temp856.tag= Cyc_Core_Impossible;
_temp856.f1=( struct _tagged_string)({ char* _temp857=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp858; _temp858.curr= _temp857; _temp858.base= _temp857;
_temp858.last_plus_one= _temp857 + 47; _temp858;}); _temp856;}); _temp855;}));
_LL766:;} _LL214: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd, _temp373), 0); _LL216:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp859=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp859->hd=( void*) _temp377; _temp859->tl= 0; _temp859;}), 0); _LL218: {
struct Cyc_NewControlFlow_AbsSyn _temp860= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp379); goto _LL861; _LL861: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( _temp860)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL220: { struct Cyc_List_List*
_temp864; struct Cyc_NewControlFlow_AbsSynOne _temp866; struct Cyc_NewControlFlow_AbsSynOne
_temp868; struct Cyc_NewControlFlow_AbsSyn _temp862= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp385); _LL869: _temp868=( struct Cyc_NewControlFlow_AbsSynOne)
_temp862.when_true; goto _LL867; _LL867: _temp866=( struct Cyc_NewControlFlow_AbsSynOne)
_temp862.when_false; goto _LL865; _LL865: _temp864=( struct Cyc_List_List*)
_temp862.lvalues; goto _LL863; _LL863: { struct Cyc_List_List* _temp872; struct
Cyc_NewControlFlow_AbsSynOne _temp874; struct Cyc_NewControlFlow_AbsSynOne
_temp876; struct Cyc_NewControlFlow_AbsSyn _temp870= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp383); _LL877: _temp876=( struct Cyc_NewControlFlow_AbsSynOne)
_temp870.when_true; goto _LL875; _LL875: _temp874=( struct Cyc_NewControlFlow_AbsSynOne)
_temp870.when_false; goto _LL873; _LL873: _temp872=( struct Cyc_List_List*)
_temp870.lvalues; goto _LL871; _LL871: { struct Cyc_List_List* _temp880; struct
Cyc_NewControlFlow_AbsSynOne _temp882; struct Cyc_NewControlFlow_AbsSynOne
_temp884; struct Cyc_NewControlFlow_AbsSyn _temp878= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp381); _LL885: _temp884=( struct Cyc_NewControlFlow_AbsSynOne)
_temp878.when_true; goto _LL883; _LL883: _temp882=( struct Cyc_NewControlFlow_AbsSynOne)
_temp878.when_false; goto _LL881; _LL881: _temp880=( struct Cyc_List_List*)
_temp878.lvalues; goto _LL879; _LL879: { void* _temp886= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp868.inner_exp,(
void*) _temp868.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp876.inner_exp,( void*) _temp876.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp866.inner_exp,( void*) _temp866.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp884.inner_exp,( void*) _temp884.assigns)));
goto _LL887; _LL887: { void* _temp888= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp868.inner_exp,( void*) _temp868.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp874.inner_exp,(
void*) _temp874.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp866.inner_exp,(
void*) _temp866.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp882.inner_exp,( void*) _temp882.assigns))); goto _LL889; _LL889:
return({ struct Cyc_NewControlFlow_AbsSyn _temp890; _temp890.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp892; _temp892.inner_exp=( void*) _temp886;
_temp892.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp892;}); _temp890.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp891;
_temp891.inner_exp=( void*) _temp888; _temp891.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp891;}); _temp890.lvalues= 0; _temp890;});}}}}}
_LL222: { struct Cyc_NewControlFlow_AbsEnv _temp893= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL894; _LL894: _temp893.access_path= 0; Cyc_NewControlFlow_cf_prepass(
_temp893, _temp387);{ struct Cyc_Absyn_Stmt* _temp895= _temp387; goto _LL896;
_LL896: while( 1) { void* _temp897=( void*) _temp895->r; struct Cyc_Absyn_Stmt*
_temp907; struct Cyc_Absyn_Stmt* _temp909; struct Cyc_Absyn_Stmt* _temp911;
struct Cyc_Absyn_Decl* _temp913; struct Cyc_Absyn_Exp* _temp915; _LL899: if((
unsigned int) _temp897 > 1u?*(( int*) _temp897) == Cyc_Absyn_Seq_s: 0){ _LL910:
_temp909=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp897)->f1;
goto _LL908; _LL908: _temp907=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp897)->f2; goto _LL900;} else{ goto _LL901;} _LL901: if(( unsigned int)
_temp897 > 1u?*(( int*) _temp897) == Cyc_Absyn_Decl_s: 0){ _LL914: _temp913=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp897)->f1; goto
_LL912; _LL912: _temp911=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp897)->f2; goto _LL902;} else{ goto _LL903;} _LL903: if(( unsigned int)
_temp897 > 1u?*(( int*) _temp897) == Cyc_Absyn_Exp_s: 0){ _LL916: _temp915=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp897)->f1; goto
_LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL900: _temp895= _temp907;
continue; _LL902: _temp895= _temp911; continue; _LL904: { struct Cyc_NewControlFlow_AbsSyn
_temp917= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp915); goto _LL918; _LL918: { void* _temp921; void* _temp923; struct Cyc_NewControlFlow_AbsSynOne
_temp919= Cyc_NewControlFlow_meet_absexp( _temp917); _LL924: _temp923=( void*)
_temp919.inner_exp; goto _LL922; _LL922: _temp921=( void*) _temp919.assigns;
goto _LL920; _LL920:( void*)((( struct Cyc_List_List*) _check_null(( Cyc_NewControlFlow_get_stmt_annot(
_temp895))->absexps))->hd=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp923, _temp921)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE(
_temp387), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}}
_LL906:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp925=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp925[ 0]=({ struct Cyc_Core_Impossible_struct _temp926; _temp926.tag= Cyc_Core_Impossible;
_temp926.f1=( struct _tagged_string)({ char* _temp927=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp928; _temp928.curr= _temp927; _temp928.base= _temp927;
_temp928.last_plus_one= _temp927 + 33; _temp928;}); _temp926;}); _temp925;}));
_LL898:;}}} _LL224: { struct Cyc_NewControlFlow_AbsEnv _temp929= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL930; _LL930: _temp929.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn
_temp931= Cyc_NewControlFlow_abstract_exp( _temp929, _temp391); goto _LL932;
_LL932: { struct Cyc_NewControlFlow_AbsSyn _temp933= Cyc_NewControlFlow_abstract_exp(
env, _temp389); goto _LL934; _LL934: { struct Cyc_NewControlFlow_AbsSynOne
_temp935= Cyc_NewControlFlow_meet_absexp( _temp931); goto _LL936; _LL936: return({
struct Cyc_NewControlFlow_AbsSyn _temp937; _temp937.when_true=({ struct Cyc_NewControlFlow_AbsSynOne
_temp939; _temp939.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp935.inner_exp,( void*)( _temp933.when_true).inner_exp); _temp939.assigns=(
void*)(( void*)( _temp933.when_true).assigns); _temp939;}); _temp937.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp938; _temp938.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp935.inner_exp,( void*)( _temp933.when_false).inner_exp);
_temp938.assigns=( void*)(( void*)( _temp933.when_false).assigns); _temp938;});
_temp937.lvalues= 0; _temp937;});}}}} _LL226: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp940=( char*)"new_control_flow.cyc:700 env.access_path==null"; struct
_tagged_string _temp941; _temp941.curr= _temp940; _temp941.base= _temp940;
_temp941.last_plus_one= _temp940 + 47; _temp941;})); Cyc_NewControlFlow_add_var_root(
env, _temp397);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp942=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp942->hd=( void*) _temp397; _temp942->tl=( env.shared)->comprehension_vars;
_temp942;});{ struct Cyc_NewControlFlow_AbsEnv _temp943= Cyc_NewControlFlow_use_it_env(
env); goto _LL944; _LL944: { void* _temp947; void* _temp949; struct Cyc_NewControlFlow_AbsSynOne
_temp945= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp943, _temp395)); _LL950: _temp949=( void*) _temp945.inner_exp; goto _LL948;
_LL948: _temp947=( void*) _temp945.assigns; goto _LL946; _LL946: { void*
_temp953; void* _temp955; struct Cyc_NewControlFlow_AbsSynOne _temp951= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp943, _temp393)); _LL956: _temp955=( void*)
_temp951.inner_exp; goto _LL954; _LL954: _temp953=( void*) _temp951.assigns;
goto _LL952; _LL952: { void* _temp957= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp949, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp947, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp397), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp955, _temp953), Cyc_CfAbsexp_mkSkipAE()))));
goto _LL958; _LL958: return Cyc_NewControlFlow_mkSyn_tf( _temp957, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}}} _LL228: goto _LL230; _LL230: goto _LL232;
_LL232: goto _LL234; _LL234: goto _LL236; _LL236: goto _LL238; _LL238: goto
_LL240; _LL240:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp959=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp959[ 0]=({ struct Cyc_Core_Impossible_struct _temp960; _temp960.tag= Cyc_Core_Impossible;
_temp960.f1=( struct _tagged_string)({ char* _temp961=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp962; _temp962.curr= _temp961; _temp962.base= _temp961;
_temp962.last_plus_one= _temp961 + 34; _temp962;}); _temp960;}); _temp959;}));
_LL154:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp965; void* _temp967; struct Cyc_NewControlFlow_AbsSynOne
_temp963= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL968: _temp967=( void*) _temp963.inner_exp; goto _LL966; _LL966: _temp965=(
void*) _temp963.assigns; goto _LL964; _LL964: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp967, _temp965);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp969= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
e); goto _LL970; _LL970: return({ struct _tuple0 _temp971; _temp971.f1= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp969.when_true).inner_exp,( void*)(
_temp969.when_true).assigns); _temp971.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*)( _temp969.when_false).inner_exp,( void*)( _temp969.when_false).assigns);
_temp971;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp972= 0; goto _LL973; _LL973: for( 0; scs != 0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp976; struct Cyc_Position_Segment*
_temp977; struct Cyc_Absyn_Stmt* _temp979; struct Cyc_Absyn_Exp* _temp981;
struct Cyc_Core_Opt* _temp983; struct Cyc_Absyn_Pat* _temp985; struct Cyc_Absyn_Switch_clause*
_temp974=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp976=* _temp974; _LL986: _temp985=( struct Cyc_Absyn_Pat*)
_temp976.pattern; goto _LL984; _LL984: _temp983=( struct Cyc_Core_Opt*) _temp976.pat_vars;
goto _LL982; _LL982: _temp981=( struct Cyc_Absyn_Exp*) _temp976.where_clause;
goto _LL980; _LL980: _temp979=( struct Cyc_Absyn_Stmt*) _temp976.body; goto
_LL978; _LL978: _temp977=( struct Cyc_Position_Segment*) _temp976.loc; goto
_LL975; _LL975: if( _temp983 == 0){( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp987=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp987[ 0]=({ struct Cyc_Core_Impossible_struct _temp988; _temp988.tag= Cyc_Core_Impossible;
_temp988.f1=( struct _tagged_string)({ char* _temp989=( char*)"switch clause vds not set";
struct _tagged_string _temp990; _temp990.curr= _temp989; _temp990.base= _temp989;
_temp990.last_plus_one= _temp989 + 26; _temp990;}); _temp988;}); _temp987;}));}{
struct Cyc_List_List* vds=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp983))->v; for( 0; vds != 0; vds=(( struct Cyc_List_List*)
_check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd);}} if( _temp981 != 0){ void*
_temp993; void* _temp995; struct _tuple0 _temp991= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp981)));
_LL996: _temp995= _temp991.f1; goto _LL994; _LL994: _temp993= _temp991.f2; goto
_LL992; _LL992: _temp972=({ struct Cyc_List_List* _temp997=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp997->hd=( void*) _temp995;
_temp997->tl=({ struct Cyc_List_List* _temp998=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp998->hd=( void*) _temp993;
_temp998->tl= _temp972; _temp998;}); _temp997;});} Cyc_NewControlFlow_cf_prepass(
env, _temp979);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp972);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp999=( char*)"new_control_flow.cyc:752 env.lhsides == null"; struct
_tagged_string _temp1000; _temp1000.curr= _temp999; _temp1000.base= _temp999;
_temp1000.last_plus_one= _temp999 + 45; _temp1000;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1001=( char*)"new_control_flow.cyc:753 env.access_path == null";
struct _tagged_string _temp1002; _temp1002.curr= _temp1001; _temp1002.base=
_temp1001; _temp1002.last_plus_one= _temp1001 + 49; _temp1002;}));{ void*
_temp1003=( void*) s->r; struct Cyc_Absyn_Exp* _temp1051; struct Cyc_Absyn_Stmt*
_temp1053; struct Cyc_Absyn_Stmt* _temp1055; struct Cyc_Absyn_Exp* _temp1057;
struct Cyc_Absyn_Exp* _temp1059; struct Cyc_Absyn_Stmt* _temp1061; struct Cyc_Absyn_Stmt*
_temp1063; struct Cyc_Absyn_Exp* _temp1065; struct _tuple3 _temp1067; struct Cyc_Absyn_Stmt*
_temp1069; struct Cyc_Absyn_Exp* _temp1071; struct Cyc_Absyn_Stmt* _temp1073;
struct Cyc_Absyn_Stmt* _temp1075; struct _tuple3 _temp1077; struct Cyc_Absyn_Stmt*
_temp1079; struct Cyc_Absyn_Exp* _temp1081; struct Cyc_Absyn_Stmt* _temp1083;
struct Cyc_Absyn_Stmt* _temp1085; struct Cyc_Absyn_Stmt* _temp1087; struct
_tagged_string* _temp1089; struct Cyc_Absyn_Stmt* _temp1091; struct
_tagged_string* _temp1093; struct Cyc_Absyn_Stmt* _temp1095; struct _tuple3
_temp1097; struct Cyc_Absyn_Stmt* _temp1099; struct Cyc_Absyn_Exp* _temp1101;
struct _tuple3 _temp1103; struct Cyc_Absyn_Stmt* _temp1105; struct Cyc_Absyn_Exp*
_temp1107; struct Cyc_Absyn_Exp* _temp1109; struct Cyc_List_List* _temp1111;
struct Cyc_Absyn_Exp* _temp1113; struct Cyc_Absyn_Switch_clause** _temp1115;
struct Cyc_List_List* _temp1117; struct Cyc_Absyn_Stmt* _temp1119; struct Cyc_Absyn_Decl*
_temp1121; struct Cyc_Absyn_Decl _temp1123; struct Cyc_Position_Segment*
_temp1124; void* _temp1126; struct Cyc_Absyn_Vardecl* _temp1128; struct Cyc_Absyn_Stmt*
_temp1130; struct Cyc_Absyn_Decl* _temp1132; struct Cyc_Absyn_Decl _temp1134;
struct Cyc_Position_Segment* _temp1135; void* _temp1137; int _temp1139; struct
Cyc_Absyn_Exp* _temp1141; struct Cyc_Core_Opt* _temp1143; struct Cyc_Core_Opt*
_temp1145; struct Cyc_Core_Opt _temp1147; struct Cyc_List_List* _temp1148;
struct Cyc_Absyn_Pat* _temp1150; struct Cyc_Absyn_Stmt* _temp1152; struct Cyc_Absyn_Decl*
_temp1154; struct Cyc_Absyn_Stmt* _temp1156; struct Cyc_Absyn_Stmt* _temp1158;
struct Cyc_Absyn_Stmt* _temp1160; struct _tagged_string* _temp1162; struct Cyc_List_List*
_temp1164; struct Cyc_Absyn_Stmt* _temp1166; struct Cyc_Absyn_Stmt* _temp1168;
struct Cyc_Absyn_Vardecl* _temp1170; struct Cyc_Absyn_Tvar* _temp1172; _LL1005:
if( _temp1003 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1006;} else{ goto _LL1007;}
_LL1007: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Exp_s:
0){ _LL1052: _temp1051=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1003)->f1; goto _LL1008;} else{ goto _LL1009;} _LL1009: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Seq_s: 0){ _LL1056: _temp1055=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1003)->f1; goto
_LL1054; _LL1054: _temp1053=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1003)->f2; goto _LL1010;} else{ goto _LL1011;} _LL1011: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Return_s: 0){ _LL1058:
_temp1057=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1003)->f1; if( _temp1057 == 0){ goto _LL1012;} else{ goto _LL1013;}} else{
goto _LL1013;} _LL1013: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003)
== Cyc_Absyn_Return_s: 0){ _LL1060: _temp1059=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1003)->f1; goto _LL1014;} else{ goto _LL1015;}
_LL1015: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_IfThenElse_s:
0){ _LL1066: _temp1065=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1003)->f1; goto _LL1064; _LL1064: _temp1063=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1003)->f2; goto _LL1062; _LL1062:
_temp1061=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1003)->f3; goto _LL1016;} else{ goto _LL1017;} _LL1017: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Do_s: 0){ _LL1074: _temp1073=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1003)->f1; goto
_LL1068; _LL1068: _temp1067=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1003)->f2; _LL1072: _temp1071= _temp1067.f1; goto _LL1070; _LL1070:
_temp1069= _temp1067.f2; goto _LL1018;} else{ goto _LL1019;} _LL1019: if((
unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_While_s: 0){
_LL1078: _temp1077=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1003)->f1; _LL1082: _temp1081= _temp1077.f1; goto _LL1080; _LL1080:
_temp1079= _temp1077.f2; goto _LL1076; _LL1076: _temp1075=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1003)->f2; goto _LL1020;} else{ goto
_LL1021;} _LL1021: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Break_s:
0){ _LL1084: _temp1083=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1003)->f1; goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Continue_s: 0){ _LL1086:
_temp1085=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1003)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Goto_s: 0){ _LL1090: _temp1089=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1003)->f1; goto
_LL1088; _LL1088: _temp1087=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1003)->f2; if( _temp1087 == 0){ goto _LL1026;} else{ goto _LL1027;}} else{
goto _LL1027;} _LL1027: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003)
== Cyc_Absyn_Goto_s: 0){ _LL1094: _temp1093=( struct _tagged_string*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp1003)->f1; goto _LL1092; _LL1092: _temp1091=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1003)->f2; goto
_LL1028;} else{ goto _LL1029;} _LL1029: if(( unsigned int) _temp1003 > 1u?*((
int*) _temp1003) == Cyc_Absyn_For_s: 0){ _LL1110: _temp1109=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp1003)->f1; goto _LL1104; _LL1104: _temp1103=(
struct _tuple3)(( struct Cyc_Absyn_For_s_struct*) _temp1003)->f2; _LL1108:
_temp1107= _temp1103.f1; goto _LL1106; _LL1106: _temp1105= _temp1103.f2; goto
_LL1098; _LL1098: _temp1097=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1003)->f3; _LL1102: _temp1101= _temp1097.f1; goto _LL1100; _LL1100:
_temp1099= _temp1097.f2; goto _LL1096; _LL1096: _temp1095=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_For_s_struct*) _temp1003)->f4; goto _LL1030;} else{ goto
_LL1031;} _LL1031: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Switch_s:
0){ _LL1114: _temp1113=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1003)->f1; goto _LL1112; _LL1112: _temp1111=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1003)->f2; goto _LL1032;} else{ goto
_LL1033;} _LL1033: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Fallthru_s:
0){ _LL1118: _temp1117=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1003)->f1; goto _LL1116; _LL1116: _temp1115=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1003)->f2; goto _LL1034;} else{ goto
_LL1035;} _LL1035: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Decl_s:
0){ _LL1122: _temp1121=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1003)->f1; _temp1123=* _temp1121; _LL1127: _temp1126=( void*) _temp1123.r;
if(*(( int*) _temp1126) == Cyc_Absyn_Var_d){ _LL1129: _temp1128=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1126)->f1; goto _LL1125;} else{ goto
_LL1037;} _LL1125: _temp1124=( struct Cyc_Position_Segment*) _temp1123.loc; goto
_LL1120; _LL1120: _temp1119=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1003)->f2; goto _LL1036;} else{ goto _LL1037;} _LL1037: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Decl_s: 0){ _LL1133: _temp1132=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1003)->f1;
_temp1134=* _temp1132; _LL1138: _temp1137=( void*) _temp1134.r; if(*(( int*)
_temp1137) == Cyc_Absyn_Let_d){ _LL1151: _temp1150=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1137)->f1; goto _LL1146; _LL1146: _temp1145=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1137)->f2; if(
_temp1145 == 0){ goto _LL1039;} else{ _temp1147=* _temp1145; _LL1149: _temp1148=(
struct Cyc_List_List*) _temp1147.v; goto _LL1144;} _LL1144: _temp1143=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1137)->f3; goto _LL1142;
_LL1142: _temp1141=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1137)->f4; goto _LL1140; _LL1140: _temp1139=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1137)->f5; goto _LL1136;} else{ goto _LL1039;} _LL1136: _temp1135=( struct
Cyc_Position_Segment*) _temp1134.loc; goto _LL1131; _LL1131: _temp1130=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1003)->f2; goto _LL1038;}
else{ goto _LL1039;} _LL1039: if(( unsigned int) _temp1003 > 1u?*(( int*)
_temp1003) == Cyc_Absyn_Decl_s: 0){ _LL1155: _temp1154=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1003)->f1; goto _LL1153; _LL1153:
_temp1152=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1003)->f2;
goto _LL1040;} else{ goto _LL1041;} _LL1041: if(( unsigned int) _temp1003 > 1u?*((
int*) _temp1003) == Cyc_Absyn_Cut_s: 0){ _LL1157: _temp1156=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1003)->f1; goto _LL1042;} else{ goto
_LL1043;} _LL1043: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Splice_s:
0){ _LL1159: _temp1158=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1003)->f1; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Label_s: 0){ _LL1163: _temp1162=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1003)->f1; goto
_LL1161; _LL1161: _temp1160=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1003)->f2; goto _LL1046;} else{ goto _LL1047;} _LL1047: if(( unsigned int)
_temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_TryCatch_s: 0){ _LL1167:
_temp1166=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1003)->f1; goto _LL1165; _LL1165: _temp1164=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1003)->f2; goto _LL1048;} else{ goto
_LL1049;} _LL1049: if(( unsigned int) _temp1003 > 1u?*(( int*) _temp1003) == Cyc_Absyn_Region_s:
0){ _LL1173: _temp1172=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1003)->f1; goto _LL1171; _LL1171: _temp1170=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1003)->f2; goto _LL1169; _LL1169:
_temp1168=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1003)->f3; goto _LL1050;} else{ goto _LL1004;} _LL1006: return; _LL1008: {
void* _temp1174= Cyc_NewControlFlow_abstract_exp_top( env, _temp1051); goto
_LL1175; _LL1175: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1176->hd=( void*) _temp1174; _temp1176->tl= 0; _temp1176;})); return;}
_LL1010: Cyc_NewControlFlow_cf_prepass( env, _temp1055); Cyc_NewControlFlow_cf_prepass(
env, _temp1053); return; _LL1012: return; _LL1014: { void* _temp1177= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp1059))); goto _LL1178; _LL1178: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1179=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1179->hd=( void*) _temp1177; _temp1179->tl= 0;
_temp1179;})); return;} _LL1016: { void* _temp1182; void* _temp1184; struct
_tuple0 _temp1180= Cyc_NewControlFlow_abstract_guard( env, _temp1065); _LL1185:
_temp1184= _temp1180.f1; goto _LL1183; _LL1183: _temp1182= _temp1180.f2; goto
_LL1181; _LL1181: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1186=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1186->hd=( void*) _temp1184; _temp1186->tl=({ struct Cyc_List_List*
_temp1187=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1187->hd=( void*) _temp1182; _temp1187->tl= 0; _temp1187;}); _temp1186;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1063); Cyc_NewControlFlow_cf_prepass(
env, _temp1061); return;} _LL1018: _temp1081= _temp1071; _temp1079= _temp1069;
_temp1075= _temp1073; goto _LL1020; _LL1020: { void* _temp1190; void* _temp1192;
struct _tuple0 _temp1188= Cyc_NewControlFlow_abstract_guard( env, _temp1081);
_LL1193: _temp1192= _temp1188.f1; goto _LL1191; _LL1191: _temp1190= _temp1188.f2;
goto _LL1189; _LL1189: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1194=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1194->hd=( void*) _temp1192; _temp1194->tl=({ struct Cyc_List_List*
_temp1195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1195->hd=( void*) _temp1190; _temp1195->tl= 0; _temp1195;}); _temp1194;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1075); return;} _LL1022: return;
_LL1024: return; _LL1026:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1196=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1196[ 0]=({ struct Cyc_Core_Impossible_struct _temp1197; _temp1197.tag= Cyc_Core_Impossible;
_temp1197.f1=( struct _tagged_string)({ char* _temp1198=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1199; _temp1199.curr= _temp1198; _temp1199.base=
_temp1198; _temp1199.last_plus_one= _temp1198 + 33; _temp1199;}); _temp1197;});
_temp1196;})); _LL1028: { struct Cyc_Absyn_Stmt* _temp1200=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; goto _LL1201; _LL1201: { struct Cyc_Absyn_Stmt* _temp1202=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Stmt*) _check_null( _temp1091))))->encloser;
goto _LL1203; _LL1203: while( _temp1202 != _temp1200) { struct Cyc_Absyn_Stmt*
_temp1204=( Cyc_NewControlFlow_get_stmt_annot( _temp1200))->encloser; goto
_LL1205; _LL1205: if( _temp1204 == _temp1200){ Cyc_Tcutil_terr( s->loc,( struct
_tagged_string)({ char* _temp1206=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1207; _temp1207.curr= _temp1206; _temp1207.base=
_temp1206; _temp1207.last_plus_one= _temp1206 + 45; _temp1207;})); break;}
_temp1200= _temp1204;} return;}} _LL1030: { void* _temp1208= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1109); goto _LL1209; _LL1209: { void* _temp1212; void* _temp1214;
struct _tuple0 _temp1210= Cyc_NewControlFlow_abstract_guard( env, _temp1107);
_LL1215: _temp1214= _temp1210.f1; goto _LL1213; _LL1213: _temp1212= _temp1210.f2;
goto _LL1211; _LL1211: { void* _temp1216= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1101); goto _LL1217; _LL1217: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1218=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1218->hd=( void*) _temp1208; _temp1218->tl=({
struct Cyc_List_List* _temp1219=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1219->hd=( void*) _temp1214; _temp1219->tl=({
struct Cyc_List_List* _temp1220=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1220->hd=( void*) _temp1212; _temp1220->tl=({
struct Cyc_List_List* _temp1221=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1221->hd=( void*) _temp1216; _temp1221->tl= 0;
_temp1221;}); _temp1220;}); _temp1219;}); _temp1218;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1095); return;}}} _LL1032: { void* _temp1222= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1113); goto _LL1223; _LL1223: { struct
Cyc_List_List* _temp1224= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1111);
goto _LL1225; _LL1225: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1226=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1226->hd=( void*) _temp1222; _temp1226->tl= _temp1224; _temp1226;}));
return;}} _LL1034: { void* _temp1229; void* _temp1231; struct Cyc_NewControlFlow_AbsSynOne
_temp1227= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1117, 0)); _LL1232: _temp1231=( void*) _temp1227.inner_exp; goto
_LL1230; _LL1230: _temp1229=( void*) _temp1227.assigns; goto _LL1228; _LL1228: {
void* _temp1233= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1231, _temp1229); goto _LL1234; _LL1234: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1235=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1235->hd=( void*) _temp1233; _temp1235->tl= 0;
_temp1235;})); return;}} _LL1036: Cyc_NewControlFlow_add_var_root( env,
_temp1128); if( _temp1128->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp1237= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1236=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1236->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1128); _temp1236->tl= 0; _temp1236;})); goto
_LL1238; _LL1238: { void* _temp1239= Cyc_NewControlFlow_abstract_exp_top(
_temp1237,( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp1128->initializer))); goto _LL1240; _LL1240: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1241=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1241->hd=( void*) _temp1239; _temp1241->tl= 0;
_temp1241;}));}} Cyc_NewControlFlow_cf_prepass( env, _temp1119); return; _LL1038:
for( 0; _temp1148 != 0; _temp1148=(( struct Cyc_List_List*) _check_null(
_temp1148))->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( _temp1148))->hd);}{ void* _temp1242= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1141); goto _LL1243; _LL1243: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1244=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1244->hd=( void*) _temp1242; _temp1244->tl= 0;
_temp1244;})); Cyc_NewControlFlow_cf_prepass( env, _temp1130); return;} _LL1040:
goto _LL1042; _LL1042: goto _LL1044; _LL1044:( void) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp1245=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1245[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1246; _temp1246.tag= Cyc_Core_Impossible;
_temp1246.f1=( struct _tagged_string)({ char* _temp1247=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1248; _temp1248.curr= _temp1247; _temp1248.base=
_temp1247; _temp1248.last_plus_one= _temp1247 + 26; _temp1248;}); _temp1246;});
_temp1245;})); _LL1046: Cyc_NewControlFlow_cf_prepass( env, _temp1160); return;
_LL1048: Cyc_NewControlFlow_cf_prepass( env, _temp1166);{ struct Cyc_List_List*
_temp1249= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1164); goto _LL1250;
_LL1250: Cyc_NewControlFlow_cf_set_absexps( s, _temp1249); return;} _LL1050: Cyc_NewControlFlow_add_var_root(
env, _temp1170); Cyc_NewControlFlow_cf_prepass( env, _temp1168); return; _LL1004:;}}
static int Cyc_NewControlFlow_iterate_cf_check= 0; static int Cyc_NewControlFlow_iteration_num=
0; struct Cyc_NewControlFlow_AnalEnv; void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow){ if( env->in_try){( void*)(
env->tryflow=( void*) Cyc_CfFlowInfo_join_flow( new_flow,( void*) env->tryflow));}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
void* in_flow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1251= Cyc_NewControlFlow_get_stmt_annot( s); goto _LL1252; _LL1252:( void*)(
_temp1251->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*) _temp1251->flow));
++ _temp1251->visited; _temp1251->visited == Cyc_NewControlFlow_iteration_num? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1253=( char*)"new_control_flow.cyc:885 annot->visited == iteration_num";
struct _tagged_string _temp1254; _temp1254.curr= _temp1253; _temp1254.base=
_temp1253; _temp1254.last_plus_one= _temp1253 + 57; _temp1254;})); return
_temp1251;} static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s,
void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot* _temp1255= Cyc_NewControlFlow_get_stmt_annot(
s); goto _LL1256; _LL1256: { void* _temp1257= Cyc_CfFlowInfo_join_flow( flow,(
void*) _temp1255->flow); goto _LL1258; _LL1258: if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp1257,( void*) _temp1255->flow)){( void*)( _temp1255->flow=( void*)
_temp1257); if( _temp1255->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1259= flow; struct Cyc_Dict_Dict*
_temp1265; _LL1261: if( _temp1259 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1262;} else{ goto _LL1263;} _LL1263: if(( unsigned int) _temp1259 > 1u?*((
int*) _temp1259) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1266: _temp1265=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1259)->f1; goto _LL1264;} else{ goto
_LL1260;} _LL1262: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1264: for( 0; vds
!= 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1269=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1267=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1267[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1268; _temp1268.tag= Cyc_CfFlowInfo_VarRoot;
_temp1268.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp1268;}); _temp1267;}); goto _LL1270; _LL1270: { void* _temp1271=
Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,( void*)
_temp1269)); goto _LL1272; _LL1272: _temp1265=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1265,(
void*) _temp1269, _temp1271);}} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1273=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1273[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1274; _temp1274.tag=
Cyc_CfFlowInfo_InitsFL; _temp1274.f1= _temp1265; _temp1274;}); _temp1273;});
_LL1260:;} static void* Cyc_NewControlFlow_add_vardecls_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1275= flow; struct Cyc_Dict_Dict*
_temp1281; _LL1277: if( _temp1275 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1278;} else{ goto _LL1279;} _LL1279: if(( unsigned int) _temp1275 > 1u?*((
int*) _temp1275) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1282: _temp1281=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1275)->f1; goto _LL1280;} else{ goto
_LL1276;} _LL1278: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1280: for( 0; vds
!= 0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1285=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1283=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1283[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1284; _temp1284.tag= Cyc_CfFlowInfo_VarRoot;
_temp1284.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp1284;}); _temp1283;}); goto _LL1286; _LL1286: _temp1281=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)(
_temp1281,( void*) _temp1285,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) _temp1285));} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1287=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1287[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1288; _temp1288.tag=
Cyc_CfFlowInfo_InitsFL; _temp1288.f1= _temp1281; _temp1288;}); _temp1287;});
_LL1276:;} static struct _tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){
if(( void*)(( struct Cyc_List_List*) _check_null( aes))->hd ==( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){
void* _temp1289= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); goto _LL1290; _LL1290: return({ struct _tuple0
_temp1291; _temp1291.f1= _temp1289; _temp1291.f2= _temp1289; _temp1291;});}
return({ struct _tuple0 _temp1292; _temp1292.f1= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null( aes))->hd, in_flow);
_temp1292.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd, in_flow);
_temp1292;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=(( struct Cyc_List_List*) _check_null(
scs))->tl){ struct Cyc_Absyn_Switch_clause _temp1295; struct Cyc_Position_Segment*
_temp1296; struct Cyc_Absyn_Stmt* _temp1298; struct Cyc_Absyn_Exp* _temp1300;
struct Cyc_Core_Opt* _temp1302; struct Cyc_Absyn_Pat* _temp1304; struct Cyc_Absyn_Switch_clause*
_temp1293=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs))->hd; _temp1295=* _temp1293; _LL1305: _temp1304=( struct Cyc_Absyn_Pat*)
_temp1295.pattern; goto _LL1303; _LL1303: _temp1302=( struct Cyc_Core_Opt*)
_temp1295.pat_vars; goto _LL1301; _LL1301: _temp1300=( struct Cyc_Absyn_Exp*)
_temp1295.where_clause; goto _LL1299; _LL1299: _temp1298=( struct Cyc_Absyn_Stmt*)
_temp1295.body; goto _LL1297; _LL1297: _temp1296=( struct Cyc_Position_Segment*)
_temp1295.loc; goto _LL1294; _LL1294: { void* _temp1306= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1302))->v); goto _LL1307; _LL1307: _temp1306= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1306,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1302))->v);{ void* body_outflow; if( _temp1300 != 0){ void* _temp1310; void*
_temp1312; struct _tuple0 _temp1308= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp1300)))->loc,
aes, _temp1306); _LL1313: _temp1312= _temp1308.f1; goto _LL1311; _LL1311:
_temp1310= _temp1308.f2; goto _LL1309; _LL1309: aes=(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->tl; in_flow=
_temp1310; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1298,
_temp1312);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1298, _temp1306);}{ void* _temp1314= body_outflow; _LL1316: if( _temp1314
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1317;} else{ goto _LL1318;} _LL1318:
goto _LL1319; _LL1317: goto _LL1315; _LL1319: Cyc_Tcutil_terr( _temp1298->loc,(
struct _tagged_string)({ char* _temp1320=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1321; _temp1321.curr= _temp1320; _temp1321.base=
_temp1320; _temp1321.last_plus_one= _temp1320 + 38; _temp1321;})); goto _LL1315;
_LL1315:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1322= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); goto _LL1323; _LL1323:
in_flow=( void*) _temp1322->flow;{ struct Cyc_List_List* _temp1324= _temp1322->absexps;
goto _LL1325; _LL1325: { void* _temp1326=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1370; struct Cyc_Absyn_Exp* _temp1372; struct Cyc_Absyn_Exp* _temp1374;
struct Cyc_Absyn_Stmt* _temp1376; struct Cyc_Absyn_Stmt* _temp1378; struct Cyc_Absyn_Stmt*
_temp1380; struct Cyc_Absyn_Stmt* _temp1382; struct Cyc_Absyn_Exp* _temp1384;
struct Cyc_Absyn_Stmt* _temp1386; struct _tuple3 _temp1388; struct Cyc_Absyn_Stmt*
_temp1390; struct Cyc_Absyn_Exp* _temp1392; struct _tuple3 _temp1394; struct Cyc_Absyn_Stmt*
_temp1396; struct Cyc_Absyn_Exp* _temp1398; struct Cyc_Absyn_Stmt* _temp1400;
struct Cyc_Absyn_Stmt* _temp1402; struct _tuple3 _temp1404; struct Cyc_Absyn_Stmt*
_temp1406; struct Cyc_Absyn_Exp* _temp1408; struct _tuple3 _temp1410; struct Cyc_Absyn_Stmt*
_temp1412; struct Cyc_Absyn_Exp* _temp1414; struct Cyc_Absyn_Exp* _temp1416;
struct Cyc_Absyn_Stmt* _temp1418; struct Cyc_Absyn_Switch_clause** _temp1420;
struct Cyc_Absyn_Switch_clause* _temp1422; struct Cyc_List_List* _temp1423;
struct Cyc_Absyn_Stmt* _temp1425; struct Cyc_Absyn_Stmt* _temp1427; struct Cyc_Absyn_Stmt*
_temp1429; struct _tagged_string* _temp1431; struct Cyc_List_List* _temp1433;
struct Cyc_Absyn_Exp* _temp1435; struct Cyc_List_List* _temp1437; struct Cyc_Absyn_Stmt*
_temp1439; struct Cyc_Absyn_Stmt* _temp1441; struct Cyc_Absyn_Decl* _temp1443;
struct Cyc_Absyn_Decl _temp1445; struct Cyc_Position_Segment* _temp1446; void*
_temp1448; struct Cyc_Absyn_Vardecl* _temp1450; struct Cyc_Absyn_Stmt* _temp1452;
struct Cyc_Absyn_Decl* _temp1454; struct Cyc_Absyn_Decl _temp1456; struct Cyc_Position_Segment*
_temp1457; void* _temp1459; int _temp1461; struct Cyc_Absyn_Exp* _temp1463;
struct Cyc_Core_Opt* _temp1465; struct Cyc_Core_Opt* _temp1467; struct Cyc_Core_Opt
_temp1469; struct Cyc_List_List* _temp1470; struct Cyc_Absyn_Pat* _temp1472;
struct Cyc_Absyn_Stmt* _temp1474; struct _tagged_string* _temp1476; struct Cyc_Absyn_Stmt*
_temp1478; struct Cyc_Absyn_Vardecl* _temp1480; struct Cyc_Absyn_Tvar* _temp1482;
_LL1328: if( _temp1326 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1329;} else{ goto
_LL1330;} _LL1330: if(( unsigned int) _temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Return_s:
0){ _LL1371: _temp1370=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1326)->f1; if( _temp1370 == 0){ goto _LL1331;} else{ goto _LL1332;}} else{
goto _LL1332;} _LL1332: if(( unsigned int) _temp1326 > 1u?*(( int*) _temp1326)
== Cyc_Absyn_Return_s: 0){ _LL1373: _temp1372=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1326)->f1; goto _LL1333;} else{ goto _LL1334;}
_LL1334: if(( unsigned int) _temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Exp_s:
0){ _LL1375: _temp1374=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1326)->f1; goto _LL1335;} else{ goto _LL1336;} _LL1336: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Seq_s: 0){ _LL1379: _temp1378=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1326)->f1; goto
_LL1377; _LL1377: _temp1376=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1326)->f2; goto _LL1337;} else{ goto _LL1338;} _LL1338: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_IfThenElse_s: 0){ _LL1385:
_temp1384=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1326)->f1; goto _LL1383; _LL1383: _temp1382=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1326)->f2; goto _LL1381; _LL1381:
_temp1380=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1326)->f3; goto _LL1339;} else{ goto _LL1340;} _LL1340: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_While_s: 0){ _LL1389: _temp1388=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1326)->f1; _LL1393:
_temp1392= _temp1388.f1; goto _LL1391; _LL1391: _temp1390= _temp1388.f2; goto
_LL1387; _LL1387: _temp1386=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1326)->f2; goto _LL1341;} else{ goto _LL1342;} _LL1342: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Do_s: 0){ _LL1401: _temp1400=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1326)->f1; goto
_LL1395; _LL1395: _temp1394=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1326)->f2; _LL1399: _temp1398= _temp1394.f1; goto _LL1397; _LL1397:
_temp1396= _temp1394.f2; goto _LL1343;} else{ goto _LL1344;} _LL1344: if((
unsigned int) _temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_For_s: 0){
_LL1417: _temp1416=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1326)->f1; goto _LL1411; _LL1411: _temp1410=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1326)->f2; _LL1415: _temp1414= _temp1410.f1; goto _LL1413; _LL1413:
_temp1412= _temp1410.f2; goto _LL1405; _LL1405: _temp1404=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1326)->f3; _LL1409: _temp1408= _temp1404.f1;
goto _LL1407; _LL1407: _temp1406= _temp1404.f2; goto _LL1403; _LL1403: _temp1402=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1326)->f4; goto
_LL1345;} else{ goto _LL1346;} _LL1346: if(( unsigned int) _temp1326 > 1u?*((
int*) _temp1326) == Cyc_Absyn_Break_s: 0){ _LL1419: _temp1418=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1326)->f1; if( _temp1418 == 0){ goto
_LL1347;} else{ goto _LL1348;}} else{ goto _LL1348;} _LL1348: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Fallthru_s: 0){ _LL1424:
_temp1423=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1326)->f1; goto _LL1421; _LL1421: _temp1420=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1326)->f2; if( _temp1420 == 0){ goto
_LL1350;} else{ _temp1422=* _temp1420; goto _LL1349;}} else{ goto _LL1350;}
_LL1350: if(( unsigned int) _temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Break_s:
0){ _LL1426: _temp1425=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1326)->f1; goto _LL1351;} else{ goto _LL1352;} _LL1352: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Continue_s: 0){ _LL1428:
_temp1427=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1326)->f1; goto _LL1353;} else{ goto _LL1354;} _LL1354: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Goto_s: 0){ _LL1432: _temp1431=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1326)->f1; goto
_LL1430; _LL1430: _temp1429=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1326)->f2; goto _LL1355;} else{ goto _LL1356;} _LL1356: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Switch_s: 0){ _LL1436:
_temp1435=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1326)->f1; goto _LL1434; _LL1434: _temp1433=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1326)->f2; goto _LL1357;} else{ goto
_LL1358;} _LL1358: if(( unsigned int) _temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_TryCatch_s:
0){ _LL1440: _temp1439=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1326)->f1; goto _LL1438; _LL1438: _temp1437=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1326)->f2; goto _LL1359;} else{ goto
_LL1360;} _LL1360: if(( unsigned int) _temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Decl_s:
0){ _LL1444: _temp1443=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1326)->f1; _temp1445=* _temp1443; _LL1449: _temp1448=( void*) _temp1445.r;
if(*(( int*) _temp1448) == Cyc_Absyn_Var_d){ _LL1451: _temp1450=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1448)->f1; goto _LL1447;} else{ goto
_LL1362;} _LL1447: _temp1446=( struct Cyc_Position_Segment*) _temp1445.loc; goto
_LL1442; _LL1442: _temp1441=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1326)->f2; goto _LL1361;} else{ goto _LL1362;} _LL1362: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Decl_s: 0){ _LL1455: _temp1454=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1326)->f1;
_temp1456=* _temp1454; _LL1460: _temp1459=( void*) _temp1456.r; if(*(( int*)
_temp1459) == Cyc_Absyn_Let_d){ _LL1473: _temp1472=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1459)->f1; goto _LL1468; _LL1468: _temp1467=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1459)->f2; if(
_temp1467 == 0){ goto _LL1364;} else{ _temp1469=* _temp1467; _LL1471: _temp1470=(
struct Cyc_List_List*) _temp1469.v; goto _LL1466;} _LL1466: _temp1465=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1459)->f3; goto _LL1464;
_LL1464: _temp1463=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1459)->f4; goto _LL1462; _LL1462: _temp1461=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1459)->f5; goto _LL1458;} else{ goto _LL1364;} _LL1458: _temp1457=( struct
Cyc_Position_Segment*) _temp1456.loc; goto _LL1453; _LL1453: _temp1452=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1326)->f2; goto _LL1363;}
else{ goto _LL1364;} _LL1364: if(( unsigned int) _temp1326 > 1u?*(( int*)
_temp1326) == Cyc_Absyn_Label_s: 0){ _LL1477: _temp1476=( struct _tagged_string*)((
struct Cyc_Absyn_Label_s_struct*) _temp1326)->f1; goto _LL1475; _LL1475:
_temp1474=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1326)->f2; goto _LL1365;} else{ goto _LL1366;} _LL1366: if(( unsigned int)
_temp1326 > 1u?*(( int*) _temp1326) == Cyc_Absyn_Region_s: 0){ _LL1483:
_temp1482=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1326)->f1; goto _LL1481; _LL1481: _temp1480=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1326)->f2; goto _LL1479; _LL1479:
_temp1478=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1326)->f3; goto _LL1367;} else{ goto _LL1368;} _LL1368: goto _LL1369;
_LL1329: return in_flow; _LL1331: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1333: Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*) _check_null(
_temp1372))->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1324))->hd,
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1335: return Cyc_CfAbsexp_eval_absexp(
env, _temp1374->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1324))->hd,
in_flow); _LL1337: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1376,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1378, in_flow)); _LL1339: { void*
_temp1486; void* _temp1488; struct _tuple0 _temp1484= Cyc_NewControlFlow_cf_evalguard(
env, _temp1384->loc, _temp1324, in_flow); _LL1489: _temp1488= _temp1484.f1; goto
_LL1487; _LL1487: _temp1486= _temp1484.f2; goto _LL1485; _LL1485: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1382, _temp1488), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1380, _temp1486));} _LL1341: { void* _temp1490=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1390))->flow; goto _LL1491; _LL1491: { void* _temp1494; void*
_temp1496; struct _tuple0 _temp1492= Cyc_NewControlFlow_cf_evalguard( env,
_temp1392->loc, _temp1324, _temp1490); _LL1497: _temp1496= _temp1492.f1; goto
_LL1495; _LL1495: _temp1494= _temp1492.f2; goto _LL1493; _LL1493: { void*
_temp1498= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1386, _temp1496); goto
_LL1499; _LL1499: Cyc_NewControlFlow_update_flow( _temp1390, _temp1498); return
_temp1494;}}} _LL1343: { void* _temp1500= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1400, in_flow); goto _LL1501; _LL1501: { void* _temp1502=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1500, _temp1396))->flow; goto _LL1503; _LL1503: { void* _temp1506; void*
_temp1508; struct _tuple0 _temp1504= Cyc_NewControlFlow_cf_evalguard( env,
_temp1398->loc, _temp1324, _temp1502); _LL1509: _temp1508= _temp1504.f1; goto
_LL1507; _LL1507: _temp1506= _temp1504.f2; goto _LL1505; _LL1505: Cyc_NewControlFlow_update_flow(
_temp1400, _temp1508); return _temp1506;}}} _LL1345: { void* _temp1510= Cyc_CfAbsexp_eval_absexp(
env, _temp1416->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1324))->hd,
in_flow); goto _LL1511; _LL1511: { void* _temp1512=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1510, _temp1412))->flow; goto _LL1513; _LL1513: { void* _temp1516; void*
_temp1518; struct _tuple0 _temp1514= Cyc_NewControlFlow_cf_evalguard( env,
_temp1414->loc,(( struct Cyc_List_List*) _check_null( _temp1324))->tl, _temp1512);
_LL1519: _temp1518= _temp1514.f1; goto _LL1517; _LL1517: _temp1516= _temp1514.f2;
goto _LL1515; _LL1515: { void* _temp1520= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1402, _temp1518); goto _LL1521; _LL1521: { void* _temp1522=( void*)(
Cyc_NewControlFlow_pre_stmt_check( _temp1520, _temp1406))->flow; goto _LL1523;
_LL1523: { void* _temp1524= Cyc_CfAbsexp_eval_absexp( env, _temp1408->loc,( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1324))->tl))->tl))->tl))->hd, _temp1522); goto _LL1525; _LL1525: Cyc_NewControlFlow_update_flow(
_temp1412, _temp1524); return _temp1516;}}}}}} _LL1347: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1349: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp1324))->hd, in_flow);{ struct Cyc_List_List* _temp1526=(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1422->pat_vars))->v;
goto _LL1527; _LL1527: in_flow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow, _temp1526); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env,
in_flow, _temp1526); _temp1425=( struct Cyc_Absyn_Stmt*) _temp1422->body; goto
_LL1351;} _LL1351: _temp1427= _temp1425; goto _LL1353; _LL1353: _temp1429=
_temp1427; goto _LL1355; _LL1355: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*) _check_null( _temp1429)), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1357: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1435->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp1324))->hd, in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1433,(( struct Cyc_List_List*) _check_null( _temp1324))->tl, in_flow);
return( void*) Cyc_CfFlowInfo_BottomFL; _LL1359: { struct Cyc_NewControlFlow_AnalEnv
_temp1530; void* _temp1531; int _temp1533; struct Cyc_Dict_Dict* _temp1535;
struct Cyc_NewControlFlow_AnalEnv* _temp1528= env; _temp1530=* _temp1528;
_LL1536: _temp1535=( struct Cyc_Dict_Dict*) _temp1530.roots; goto _LL1534;
_LL1534: _temp1533=( int) _temp1530.in_try; goto _LL1532; _LL1532: _temp1531=(
void*) _temp1530.tryflow; goto _LL1529; _LL1529: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* _temp1537= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1439, in_flow); goto _LL1538; _LL1538: { void* _temp1539=( void*) env->tryflow;
goto _LL1540; _LL1540: env->in_try= _temp1533;( void*)( env->tryflow=( void*)
_temp1531); Cyc_NewControlFlow_update_tryflow( env, _temp1539); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1437, _temp1324, _temp1539); return _temp1537;}}} _LL1361: { struct
Cyc_List_List _temp1542=({ struct Cyc_List_List _temp1541; _temp1541.hd=( void*)
_temp1450; _temp1541.tl= 0; _temp1541;}); goto _LL1543; _LL1543: { void*
_temp1544= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)&
_temp1542); goto _LL1545; _LL1545: { struct Cyc_Absyn_Exp* _temp1546= _temp1450->initializer;
goto _LL1547; _LL1547: if( _temp1546 != 0){ _temp1544= Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null( _temp1546)))->loc,(
void*)(( struct Cyc_List_List*) _check_null( _temp1324))->hd, _temp1544);}
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1441, _temp1544);}}}
_LL1363: { void* _temp1548= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1470); goto _LL1549; _LL1549: _temp1548= Cyc_CfAbsexp_eval_absexp( env,
_temp1463->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp1324))->hd,
_temp1548); _temp1548= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1548,
_temp1470); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1452, _temp1548);}
_LL1365: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1474, in_flow);
_LL1367: { struct Cyc_List_List* _temp1551=({ struct Cyc_List_List* _temp1550=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1550->hd=(
void*) _temp1480; _temp1550->tl= 0; _temp1550;}); goto _LL1552; _LL1552: { void*
_temp1553= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1551); goto
_LL1554; _LL1554: _temp1553= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp1553, _temp1551); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1478,
_temp1553);}} _LL1369:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1555=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1555[ 0]=({ struct Cyc_Core_Impossible_struct _temp1556; _temp1556.tag= Cyc_Core_Impossible;
_temp1556.f1=( struct _tagged_string)({ char* _temp1557=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1558; _temp1558.curr= _temp1557; _temp1558.base=
_temp1557; _temp1558.last_plus_one= _temp1557 + 62; _temp1558;}); _temp1556;});
_temp1555;})); _LL1327:;}}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1559=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1583; struct Cyc_Absyn_Stmt* _temp1585;
struct Cyc_Absyn_Stmt* _temp1587; struct Cyc_Absyn_Stmt* _temp1589; struct Cyc_Absyn_Exp*
_temp1591; struct Cyc_Absyn_Stmt* _temp1593; struct _tuple3 _temp1595; struct
Cyc_Absyn_Stmt* _temp1597; struct Cyc_Absyn_Exp* _temp1599; struct _tuple3
_temp1601; struct Cyc_Absyn_Stmt* _temp1603; struct Cyc_Absyn_Exp* _temp1605;
struct Cyc_Absyn_Exp* _temp1607; struct Cyc_Absyn_Stmt* _temp1609; struct
_tuple3 _temp1611; struct Cyc_Absyn_Stmt* _temp1613; struct Cyc_Absyn_Exp*
_temp1615; struct _tuple3 _temp1617; struct Cyc_Absyn_Stmt* _temp1619; struct
Cyc_Absyn_Exp* _temp1621; struct Cyc_Absyn_Stmt* _temp1623; struct Cyc_Absyn_Stmt*
_temp1625; struct Cyc_Absyn_Vardecl* _temp1627; struct Cyc_Absyn_Tvar* _temp1629;
struct Cyc_Absyn_Stmt* _temp1631; struct _tagged_string* _temp1633; struct Cyc_Absyn_Stmt*
_temp1635; struct Cyc_Absyn_Decl* _temp1637; struct Cyc_List_List* _temp1639;
struct Cyc_Absyn_Stmt* _temp1641; struct Cyc_List_List* _temp1643; struct Cyc_Absyn_Exp*
_temp1645; _LL1561: if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) ==
Cyc_Absyn_Seq_s: 0){ _LL1586: _temp1585=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1559)->f1; goto _LL1584; _LL1584: _temp1583=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1559)->f2; goto _LL1562;} else{ goto
_LL1563;} _LL1563: if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_IfThenElse_s:
0){ _LL1592: _temp1591=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1559)->f1; goto _LL1590; _LL1590: _temp1589=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1559)->f2; goto _LL1588; _LL1588:
_temp1587=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1559)->f3; goto _LL1564;} else{ goto _LL1565;} _LL1565: if(( unsigned int)
_temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_For_s: 0){ _LL1608: _temp1607=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1559)->f1; goto
_LL1602; _LL1602: _temp1601=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1559)->f2; _LL1606: _temp1605= _temp1601.f1; goto _LL1604; _LL1604:
_temp1603= _temp1601.f2; goto _LL1596; _LL1596: _temp1595=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1559)->f3; _LL1600: _temp1599= _temp1595.f1;
goto _LL1598; _LL1598: _temp1597= _temp1595.f2; goto _LL1594; _LL1594: _temp1593=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1559)->f4; goto
_LL1566;} else{ goto _LL1567;} _LL1567: if(( unsigned int) _temp1559 > 1u?*((
int*) _temp1559) == Cyc_Absyn_While_s: 0){ _LL1612: _temp1611=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp1559)->f1; _LL1616: _temp1615= _temp1611.f1;
goto _LL1614; _LL1614: _temp1613= _temp1611.f2; goto _LL1610; _LL1610: _temp1609=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp1559)->f2; goto
_LL1568;} else{ goto _LL1569;} _LL1569: if(( unsigned int) _temp1559 > 1u?*((
int*) _temp1559) == Cyc_Absyn_Do_s: 0){ _LL1624: _temp1623=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1559)->f1; goto _LL1618; _LL1618: _temp1617=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1559)->f2; _LL1622:
_temp1621= _temp1617.f1; goto _LL1620; _LL1620: _temp1619= _temp1617.f2; goto
_LL1570;} else{ goto _LL1571;} _LL1571: if(( unsigned int) _temp1559 > 1u?*((
int*) _temp1559) == Cyc_Absyn_Region_s: 0){ _LL1630: _temp1629=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1559)->f1; goto _LL1628; _LL1628:
_temp1627=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1559)->f2; goto _LL1626; _LL1626: _temp1625=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1559)->f3; goto _LL1572;} else{ goto
_LL1573;} _LL1573: if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_Label_s:
0){ _LL1634: _temp1633=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1559)->f1; goto _LL1632; _LL1632: _temp1631=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1559)->f2; goto _LL1574;} else{ goto
_LL1575;} _LL1575: if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_Decl_s:
0){ _LL1638: _temp1637=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1559)->f1; goto _LL1636; _LL1636: _temp1635=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1559)->f2; goto _LL1576;} else{ goto
_LL1577;} _LL1577: if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_TryCatch_s:
0){ _LL1642: _temp1641=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1559)->f1; goto _LL1640; _LL1640: _temp1639=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1559)->f2; goto _LL1578;} else{ goto
_LL1579;} _LL1579: if(( unsigned int) _temp1559 > 1u?*(( int*) _temp1559) == Cyc_Absyn_Switch_s:
0){ _LL1646: _temp1645=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1559)->f1; goto _LL1644; _LL1644: _temp1643=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1559)->f2; goto _LL1580;} else{ goto
_LL1581;} _LL1581: goto _LL1582; _LL1562: _temp1589= _temp1585; _temp1587=
_temp1583; goto _LL1564; _LL1564: Cyc_NewControlFlow_cf_postpass( _temp1589);
Cyc_NewControlFlow_cf_postpass( _temp1587); return; _LL1566: Cyc_NewControlFlow_cf_postpass(
_temp1597); _temp1613= _temp1593; _temp1609= _temp1603; goto _LL1568; _LL1568:
_temp1623= _temp1609; _temp1619= _temp1613; goto _LL1570; _LL1570: Cyc_NewControlFlow_cf_postpass(
_temp1619); _temp1625= _temp1623; goto _LL1572; _LL1572: _temp1631= _temp1625;
goto _LL1574; _LL1574: _temp1635= _temp1631; goto _LL1576; _LL1576: Cyc_NewControlFlow_cf_postpass(
_temp1635); return; _LL1578: Cyc_NewControlFlow_cf_postpass( _temp1641);
_temp1643= _temp1639; goto _LL1580; _LL1580: for( 0; _temp1643 != 0; _temp1643=((
struct Cyc_List_List*) _check_null( _temp1643))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1643))->hd)->body);}
return; _LL1582: return; _LL1560:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1675; _temp1675.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1676=( struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1676->all_roots=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp1676->comprehension_vars= 0; _temp1676;}); _temp1675.lhsides= 0; _temp1675.access_path=
0; _temp1675;}); struct Cyc_List_List* _temp1647=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v; goto _LL1648;
_LL1648:{ struct Cyc_List_List* vds= _temp1647; for( 0; vds != 0; vds=(( struct
Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_List_List*
_temp1651; struct Cyc_List_List* _temp1653; struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1655; struct Cyc_NewControlFlow_SharedAbsEnv _temp1657; struct Cyc_List_List*
_temp1658; struct Cyc_Dict_Dict* _temp1660; struct Cyc_NewControlFlow_AbsEnv
_temp1649= absenv; _LL1656: _temp1655=( struct Cyc_NewControlFlow_SharedAbsEnv*)
_temp1649.shared; _temp1657=* _temp1655; _LL1661: _temp1660=( struct Cyc_Dict_Dict*)
_temp1657.all_roots; goto _LL1659; _LL1659: _temp1658=( struct Cyc_List_List*)
_temp1657.comprehension_vars; goto _LL1654; _LL1654: _temp1653=( struct Cyc_List_List*)
_temp1649.lhsides; goto _LL1652; _LL1652: _temp1651=( struct Cyc_List_List*)
_temp1649.access_path; goto _LL1650; _LL1650: { struct Cyc_NewControlFlow_AnalEnv*
_temp1663=({ struct Cyc_NewControlFlow_AnalEnv* _temp1662=( struct Cyc_NewControlFlow_AnalEnv*)
GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp1662->roots=
_temp1660; _temp1662->in_try= 0; _temp1662->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp1662;}); goto _LL1664; _LL1664: { void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1673=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1673[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1674; _temp1674.tag=
Cyc_CfFlowInfo_InitsFL; _temp1674.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)(
void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1674;});
_temp1673;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp1663, in_flow,
_temp1647); in_flow= Cyc_NewControlFlow_add_vardecls_flow( _temp1663, in_flow,
_temp1658);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp1663, fd->body, in_flow);}{ void* _temp1665= out_flow; _LL1667: if(
_temp1665 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1668;} else{ goto _LL1669;}
_LL1669: goto _LL1670; _LL1668: goto _LL1666; _LL1670: if(( void*) fd->ret_type
!=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,( struct
_tagged_string)({ char* _temp1671=( char*)"function may complete without returning a value";
struct _tagged_string _temp1672; _temp1672.curr= _temp1671; _temp1672.base=
_temp1671; _temp1672.last_plus_one= _temp1671 + 48; _temp1672;}));} goto _LL1666;
_LL1666:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp1677=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp1687; struct Cyc_List_List* _temp1689; struct _tuple1* _temp1691; struct
Cyc_List_List* _temp1693; struct _tagged_string* _temp1695; _LL1679: if(*(( int*)
_temp1677) == Cyc_Absyn_Fn_d){ _LL1688: _temp1687=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1677)->f1; goto _LL1680;} else{ goto _LL1681;}
_LL1681: if(*(( int*) _temp1677) == Cyc_Absyn_Using_d){ _LL1692: _temp1691=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp1677)->f1; goto
_LL1690; _LL1690: _temp1689=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1677)->f2; goto _LL1682;} else{ goto _LL1683;} _LL1683: if(*(( int*)
_temp1677) == Cyc_Absyn_Namespace_d){ _LL1696: _temp1695=( struct _tagged_string*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1677)->f1; goto _LL1694; _LL1694:
_temp1693=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1677)->f2; goto _LL1684;} else{ goto _LL1685;} _LL1685: goto _LL1686;
_LL1680: Cyc_NewControlFlow_cf_check_fun( _temp1687); goto _LL1678; _LL1682:
_temp1693= _temp1689; goto _LL1684; _LL1684: Cyc_NewControlFlow_cf_check(
_temp1693); goto _LL1678; _LL1686: goto _LL1678; _LL1678:;}}