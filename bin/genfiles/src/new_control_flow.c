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
env, void* ae){ if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(({
struct Cyc_List_List* _temp49= env.lhsides; if( _temp49 == 0){ _throw(
Null_Exception);} _temp49->tl;}) == 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)({
struct Cyc_List_List* _temp50= env.lhsides; if( _temp50 == 0){ _throw(
Null_Exception);} _temp50->hd;}), ae);}{ struct Cyc_List_List* _temp51= 0; goto
_LL52; _LL52:{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides != 0;
lhsides=({ struct Cyc_List_List* _temp53= lhsides; if( _temp53 == 0){ _throw(
Null_Exception);} _temp53->tl;})){ _temp51=({ struct Cyc_List_List* _temp54=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp54->hd=(
void*) Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List* _temp55= lhsides;
if( _temp55 == 0){ _throw( Null_Exception);} _temp55->hd;}), ae); _temp54->tl=
_temp51; _temp54;});}} return Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_OneofG,
_temp51);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=({ struct Cyc_List_List*
_temp56= es; if( _temp56 == 0){ _throw( Null_Exception);} _temp56->tl;})){
struct Cyc_NewControlFlow_AbsSyn _temp58= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp57= es; if(
_temp57 == 0){ _throw( Null_Exception);} _temp57->hd;})); goto _LL59; _LL59: {
void* _temp62; void* _temp64; struct Cyc_NewControlFlow_AbsSynOne _temp60= Cyc_NewControlFlow_meet_absexp(
_temp58); _LL65: _temp64=( void*) _temp60.inner_exp; goto _LL63; _LL63: _temp62=(
void*) _temp60.assigns; goto _LL61; _LL61: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, _temp64, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp62, uses_exp);}} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp66=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp66->hd=( void*) path_e; _temp66->tl=
env.access_path; _temp66;});{ struct Cyc_NewControlFlow_AbsSyn _temp67= Cyc_NewControlFlow_abstract_exp(
env, inner_e); goto _LL68; _LL68: if( _temp67.lvalues == 0){ _temp67.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp67;}} void* Cyc_NewControlFlow_destruct_path(
struct Cyc_NewControlFlow_AbsEnv env, void* absop){{ struct Cyc_List_List* path=
env.access_path; for( 0; path != 0; path=({ struct Cyc_List_List* _temp69= path;
if( _temp69 == 0){ _throw( Null_Exception);} _temp69->tl;})){ void* _temp71=(
void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp70= path; if(
_temp70 == 0){ _throw( Null_Exception);} _temp70->hd;}))->r; struct Cyc_Absyn_Exp*
_temp85; struct Cyc_Absyn_Exp* _temp87; struct _tagged_string* _temp89; struct
Cyc_Absyn_Exp* _temp91; struct _tagged_string* _temp93; struct Cyc_Absyn_Exp*
_temp95; struct Cyc_Absyn_Exp* _temp97; struct Cyc_Absyn_Exp* _temp99; _LL73:
if(*(( int*) _temp71) == Cyc_Absyn_Address_e){ _LL86: _temp85=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp71)->f1; goto _LL74;} else{ goto _LL75;}
_LL75: if(*(( int*) _temp71) == Cyc_Absyn_Deref_e){ _LL88: _temp87=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp71)->f1; goto _LL76;} else{ goto _LL77;}
_LL77: if(*(( int*) _temp71) == Cyc_Absyn_StructArrow_e){ _LL92: _temp91=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp71)->f1;
goto _LL90; _LL90: _temp89=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp71)->f2; goto _LL78;} else{ goto _LL79;} _LL79: if(*(( int*) _temp71) ==
Cyc_Absyn_StructMember_e){ _LL96: _temp95=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp71)->f1; goto _LL94; _LL94: _temp93=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp71)->f2; goto _LL80;} else{ goto _LL81;} _LL81: if(*(( int*) _temp71) ==
Cyc_Absyn_Subscript_e){ _LL100: _temp99=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp71)->f1; goto _LL98; _LL98: _temp97=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp71)->f2; goto _LL82;} else{ goto _LL83;} _LL83: goto _LL84; _LL74:({ struct
Cyc_List_List* _temp101= path; if( _temp101 == 0){ _throw( Null_Exception);}
_temp101->tl;}) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp102=( char*)"new_control_flow.cyc:219 path->tl == null";
struct _tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 42; _temp103;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL76: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL72; _LL78: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp93= _temp89; goto _LL80; _LL80: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp104=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp104[ 0]=({
struct Cyc_CfFlowInfo_StructF_struct _temp105; _temp105.tag= Cyc_CfFlowInfo_StructF;
_temp105.f1= _temp93; _temp105;}); _temp104;})); goto _LL72; _LL82: {
unsigned int _temp106= Cyc_Evexp_eval_const_uint_exp( _temp97); goto _LL107;
_LL107: absop= Cyc_CfAbsexp_mkMemberOp( absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp108=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp108[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp109; _temp109.tag= Cyc_CfFlowInfo_TupleF;
_temp109.f1=( int) _temp106; _temp109;}); _temp108;})); goto _LL72;} _LL84:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp110=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp110[ 0]=({ struct
Cyc_Core_Impossible_struct _temp111; _temp111.tag= Cyc_Core_Impossible; _temp111.f1=(
struct _tagged_string)({ char* _temp112=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 44; _temp113;}); _temp111;}); _temp110;}));
_LL72:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp*
rgn_exp, struct Cyc_Absyn_Exp* init_exp){ void* _temp114= Cyc_CfAbsexp_mkMallocOp(
outer_e); goto _LL115; _LL115: { void* _temp116= Cyc_CfAbsexp_mkSkipAE(); goto
_LL117; _LL117: { void* _temp118= Cyc_CfAbsexp_mkSkipAE(); goto _LL119; _LL119:
if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv _temp121= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp120=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp120->hd=( void*) _temp114; _temp120->tl= 0;
_temp120;})); goto _LL122; _LL122: _temp121.access_path= 0;{ void* _temp126;
void* _temp128; struct Cyc_NewControlFlow_AbsSynOne _temp124= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp121,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp123= init_exp; if( _temp123 == 0){ _throw( Null_Exception);} _temp123;})));
_LL129: _temp128=( void*) _temp124.inner_exp; goto _LL127; _LL127: _temp126=(
void*) _temp124.assigns; goto _LL125; _LL125: _temp116= _temp128; _temp118=
_temp126;}} if( rgn_exp != 0){ struct Cyc_NewControlFlow_AbsSyn _temp131= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp130= rgn_exp; if( _temp130 == 0){ _throw( Null_Exception);} _temp130;}));
goto _LL132; _LL132: { void* _temp135; void* _temp137; struct Cyc_NewControlFlow_AbsSynOne
_temp133= Cyc_NewControlFlow_meet_absexp( _temp131); _LL138: _temp137=( void*)
_temp133.inner_exp; goto _LL136; _LL136: _temp135=( void*) _temp133.assigns;
goto _LL134; _LL134: _temp116= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp116, _temp137); _temp118= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp118, _temp135);}}{ void* _temp139= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp116, _temp118)); goto _LL140; _LL140: { struct Cyc_NewControlFlow_AbsSyn
_temp141= Cyc_NewControlFlow_mkSyn_tf( _temp139, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( _temp114))); goto _LL142; _LL142: if( env.access_path
!= 0){ void* _temp144=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp143= env.access_path; if( _temp143 == 0){ _throw( Null_Exception);}
_temp143->hd;}))->r; struct Cyc_Absyn_Exp* _temp152; struct _tagged_string*
_temp154; struct Cyc_Absyn_Exp* _temp156; _LL146: if(*(( int*) _temp144) == Cyc_Absyn_Deref_e){
_LL153: _temp152=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp144)->f1; goto _LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp144)
== Cyc_Absyn_StructArrow_e){ _LL157: _temp156=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp144)->f1; goto _LL155; _LL155: _temp154=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp144)->f2;
goto _LL149;} else{ goto _LL150;} _LL150: goto _LL151; _LL147: env.access_path=({
struct Cyc_List_List* _temp158= env.access_path; if( _temp158 == 0){ _throw(
Null_Exception);} _temp158->tl;}); _temp141.lvalues=({ struct Cyc_List_List*
_temp159=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp159->hd=( void*) Cyc_NewControlFlow_destruct_path( env, _temp114); _temp159->tl=
0; _temp159;}); goto _LL145; _LL149: env.access_path=({ struct Cyc_List_List*
_temp160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp160->hd=( void*) Cyc_Absyn_structmember_exp( _temp156, _temp154, 0);
_temp160->tl=({ struct Cyc_List_List* _temp161= env.access_path; if( _temp161 ==
0){ _throw( Null_Exception);} _temp161->tl;}); _temp160;}); _temp141.lvalues=({
struct Cyc_List_List* _temp162=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp162->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp114); _temp162->tl= 0; _temp162;}); goto _LL145; _LL151:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp163=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp163[ 0]=({ struct
Cyc_Core_Impossible_struct _temp164; _temp164.tag= Cyc_Core_Impossible; _temp164.f1=(
struct _tagged_string)({ char* _temp165=( char*)"bad malloc access path"; struct
_tagged_string _temp166; _temp166.curr= _temp165; _temp166.base= _temp165;
_temp166.last_plus_one= _temp165 + 23; _temp166;}); _temp164;}); _temp163;}));
_LL145:;} return _temp141;}}}}} struct _tuple4{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp167=(
void*) e->r; struct Cyc_Absyn_Exp* _temp255; struct Cyc_Absyn_Exp* _temp257;
struct Cyc_List_List* _temp259; struct Cyc_Absyn_Exp* _temp261; void* _temp263;
int _temp265; void* _temp267; void* _temp269; int _temp271; void* _temp273; void*
_temp275; void* _temp277; struct Cyc_Absyn_Enumfield* _temp279; struct Cyc_Absyn_Enumdecl*
_temp281; struct _tuple1* _temp283; void* _temp285; struct Cyc_Absyn_Fndecl*
_temp287; struct _tuple1* _temp289; void* _temp291; struct Cyc_Absyn_Vardecl*
_temp293; struct _tuple1* _temp295; void* _temp297; struct Cyc_Absyn_Vardecl*
_temp299; struct _tuple1* _temp301; void* _temp303; struct Cyc_Absyn_Vardecl*
_temp305; struct _tuple1* _temp307; void* _temp309; struct Cyc_Absyn_Vardecl*
_temp311; struct _tuple1* _temp313; struct Cyc_Absyn_Exp* _temp315; struct Cyc_Core_Opt*
_temp317; struct Cyc_Absyn_Exp* _temp319; struct Cyc_Absyn_Exp* _temp321; struct
Cyc_Core_Opt* _temp323; struct Cyc_Absyn_Exp* _temp325; struct Cyc_List_List*
_temp327; void* _temp329; struct Cyc_Absyn_Exp* _temp331; void* _temp333; struct
Cyc_List_List* _temp335; struct Cyc_Absyn_Exp* _temp337; struct Cyc_Absyn_Exp*
_temp339; struct Cyc_Absyn_Exp* _temp341; struct Cyc_Absyn_Exp* _temp343; struct
Cyc_Absyn_Exp* _temp345; struct Cyc_Absyn_Exp* _temp347; void* _temp349; struct
Cyc_Absyn_Exp* _temp351; struct Cyc_Absyn_Exp* _temp353; struct _tagged_string*
_temp355; struct Cyc_Absyn_Exp* _temp357; struct _tagged_string* _temp359;
struct Cyc_Absyn_Exp* _temp361; struct Cyc_Absyn_Tunionfield* _temp363; struct
Cyc_Absyn_Tuniondecl* _temp365; struct Cyc_List_List* _temp367; struct Cyc_Core_Opt*
_temp369; struct Cyc_Core_Opt* _temp371; struct Cyc_List_List* _temp373; struct
Cyc_List_List* _temp375; void* _temp377; struct Cyc_Absyn_Structdecl* _temp379;
struct Cyc_List_List* _temp381; struct Cyc_Core_Opt* _temp383; struct _tuple1*
_temp385; struct Cyc_List_List* _temp387; void* _temp389; struct Cyc_Absyn_Exp*
_temp391; struct Cyc_Absyn_Exp* _temp393; struct Cyc_Absyn_Exp* _temp395; struct
Cyc_Absyn_Exp* _temp397; struct Cyc_Absyn_Exp* _temp399; struct Cyc_Absyn_Stmt*
_temp401; struct Cyc_Absyn_Exp* _temp403; struct Cyc_Absyn_Exp* _temp405; struct
Cyc_Absyn_Exp* _temp407; struct Cyc_Absyn_Exp* _temp409; struct Cyc_Absyn_Vardecl*
_temp411; void* _temp413; struct _tuple1* _temp415; struct _tuple1* _temp417;
struct Cyc_List_List* _temp419; struct Cyc_Absyn_Exp* _temp421; struct Cyc_List_List*
_temp423; struct Cyc_Core_Opt* _temp425; struct Cyc_List_List* _temp427; struct
_tuple2* _temp429; struct Cyc_Absyn_Fndecl* _temp431; struct Cyc_Absyn_Exp*
_temp433; _LL169: if(*(( int*) _temp167) == Cyc_Absyn_NoInstantiate_e){ _LL256:
_temp255=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp167)->f1; goto _LL170;} else{ goto _LL171;} _LL171: if(*(( int*) _temp167)
== Cyc_Absyn_Sizeofexp_e){ _LL258: _temp257=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Sizeofexp_e_struct*) _temp167)->f1; goto _LL172;} else{ goto _LL173;}
_LL173: if(*(( int*) _temp167) == Cyc_Absyn_Instantiate_e){ _LL262: _temp261=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp167)->f1;
goto _LL260; _LL260: _temp259=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp167)->f2; goto _LL174;} else{ goto _LL175;} _LL175: if(*(( int*) _temp167)
== Cyc_Absyn_Const_e){ _LL264: _temp263=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp167)->f1; if(( unsigned int) _temp263 > 1u?*(( int*) _temp263) == Cyc_Absyn_Int_c:
0){ _LL268: _temp267=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp263)->f1;
if( _temp267 ==( void*) Cyc_Absyn_Signed){ goto _LL266;} else{ goto _LL177;}
_LL266: _temp265=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp263)->f2; if(
_temp265 == 0){ goto _LL176;} else{ goto _LL177;}} else{ goto _LL177;}} else{
goto _LL177;} _LL177: if(*(( int*) _temp167) == Cyc_Absyn_Const_e){ _LL270:
_temp269=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp167)->f1; if((
unsigned int) _temp269 > 1u?*(( int*) _temp269) == Cyc_Absyn_Int_c: 0){ _LL274:
_temp273=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp269)->f1; if( _temp273
==( void*) Cyc_Absyn_Signed){ goto _LL272;} else{ goto _LL179;} _LL272: _temp271=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp269)->f2; if( _temp271 == 1){ goto
_LL178;} else{ goto _LL179;}} else{ goto _LL179;}} else{ goto _LL179;} _LL179:
if(*(( int*) _temp167) == Cyc_Absyn_Sizeoftyp_e){ _LL276: _temp275=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp167)->f1; goto _LL180;} else{ goto
_LL181;} _LL181: if(*(( int*) _temp167) == Cyc_Absyn_Const_e){ _LL278: _temp277=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp167)->f1; goto _LL182;} else{
goto _LL183;} _LL183: if(*(( int*) _temp167) == Cyc_Absyn_Enum_e){ _LL284:
_temp283=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp167)->f1;
goto _LL282; _LL282: _temp281=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp167)->f2; goto _LL280; _LL280: _temp279=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp167)->f3; goto _LL184;} else{ goto _LL185;}
_LL185: if(*(( int*) _temp167) == Cyc_Absyn_Var_e){ _LL290: _temp289=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp167)->f1; goto _LL286; _LL286:
_temp285=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp167)->f2; if((
unsigned int) _temp285 > 1u?*(( int*) _temp285) == Cyc_Absyn_Funname_b: 0){
_LL288: _temp287=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp285)->f1; goto _LL186;} else{ goto _LL187;}} else{ goto _LL187;} _LL187:
if(*(( int*) _temp167) == Cyc_Absyn_Var_e){ _LL296: _temp295=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp167)->f1; goto _LL292; _LL292: _temp291=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp167)->f2; if(( unsigned int)
_temp291 > 1u?*(( int*) _temp291) == Cyc_Absyn_Global_b: 0){ _LL294: _temp293=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp291)->f1;
goto _LL188;} else{ goto _LL189;}} else{ goto _LL189;} _LL189: if(*(( int*)
_temp167) == Cyc_Absyn_Var_e){ _LL302: _temp301=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp167)->f1; goto _LL298; _LL298: _temp297=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp167)->f2; if(( unsigned int) _temp297 > 1u?*(( int*) _temp297) == Cyc_Absyn_Param_b:
0){ _LL300: _temp299=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp297)->f1; goto _LL190;} else{ goto _LL191;}} else{ goto _LL191;} _LL191:
if(*(( int*) _temp167) == Cyc_Absyn_Var_e){ _LL308: _temp307=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp167)->f1; goto _LL304; _LL304: _temp303=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp167)->f2; if(( unsigned int)
_temp303 > 1u?*(( int*) _temp303) == Cyc_Absyn_Local_b: 0){ _LL306: _temp305=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp303)->f1;
goto _LL192;} else{ goto _LL193;}} else{ goto _LL193;} _LL193: if(*(( int*)
_temp167) == Cyc_Absyn_Var_e){ _LL314: _temp313=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp167)->f1; goto _LL310; _LL310: _temp309=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp167)->f2; if(( unsigned int) _temp309 > 1u?*(( int*) _temp309) == Cyc_Absyn_Pat_b:
0){ _LL312: _temp311=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp309)->f1; goto _LL194;} else{ goto _LL195;}} else{ goto _LL195;} _LL195:
if(*(( int*) _temp167) == Cyc_Absyn_AssignOp_e){ _LL320: _temp319=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp167)->f1; goto _LL318; _LL318:
_temp317=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp167)->f2;
if( _temp317 == 0){ goto _LL316;} else{ goto _LL197;} _LL316: _temp315=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp167)->f3; goto
_LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp167) == Cyc_Absyn_AssignOp_e){
_LL326: _temp325=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp167)->f1; goto _LL324; _LL324: _temp323=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp167)->f2; goto _LL322; _LL322: _temp321=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp167)->f3;
goto _LL198;} else{ goto _LL199;} _LL199: if(*(( int*) _temp167) == Cyc_Absyn_Primop_e){
_LL330: _temp329=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp167)->f1;
goto _LL328; _LL328: _temp327=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp167)->f2; goto _LL200;} else{ goto _LL201;} _LL201: if(*(( int*) _temp167)
== Cyc_Absyn_Cast_e){ _LL334: _temp333=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp167)->f1; goto _LL332; _LL332: _temp331=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp167)->f2; goto _LL202;} else{ goto _LL203;}
_LL203: if(*(( int*) _temp167) == Cyc_Absyn_FnCall_e){ _LL338: _temp337=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp167)->f1; goto _LL336;
_LL336: _temp335=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp167)->f2; goto _LL204;} else{ goto _LL205;} _LL205: if(*(( int*) _temp167)
== Cyc_Absyn_Subscript_e){ _LL342: _temp341=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp167)->f1; goto _LL340; _LL340: _temp339=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp167)->f2;
goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp167) == Cyc_Absyn_Address_e){
_LL344: _temp343=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp167)->f1; goto _LL208;} else{ goto _LL209;} _LL209: if(*(( int*) _temp167)
== Cyc_Absyn_New_e){ _LL348: _temp347=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp167)->f1; goto _LL346; _LL346: _temp345=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp167)->f2; goto _LL210;} else{ goto _LL211;} _LL211:
if(*(( int*) _temp167) == Cyc_Absyn_Malloc_e){ _LL352: _temp351=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp167)->f1; goto _LL350; _LL350: _temp349=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp167)->f2; goto _LL212;} else{
goto _LL213;} _LL213: if(*(( int*) _temp167) == Cyc_Absyn_Deref_e){ _LL354:
_temp353=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp167)->f1;
goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp167) == Cyc_Absyn_StructArrow_e){
_LL358: _temp357=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp167)->f1; goto _LL356; _LL356: _temp355=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp167)->f2; goto _LL216;} else{ goto _LL217;}
_LL217: if(*(( int*) _temp167) == Cyc_Absyn_StructMember_e){ _LL362: _temp361=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp167)->f1;
goto _LL360; _LL360: _temp359=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp167)->f2; goto _LL218;} else{ goto _LL219;} _LL219: if(*(( int*) _temp167)
== Cyc_Absyn_Tunion_e){ _LL372: _temp371=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp167)->f1; goto _LL370; _LL370: _temp369=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp167)->f2; goto _LL368; _LL368: _temp367=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp167)->f3; goto
_LL366; _LL366: _temp365=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp167)->f4; goto _LL364; _LL364: _temp363=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp167)->f5; goto _LL220;} else{ goto
_LL221;} _LL221: if(*(( int*) _temp167) == Cyc_Absyn_Tuple_e){ _LL374: _temp373=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp167)->f1; goto
_LL222;} else{ goto _LL223;} _LL223: if(*(( int*) _temp167) == Cyc_Absyn_AnonStruct_e){
_LL378: _temp377=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp167)->f1;
goto _LL376; _LL376: _temp375=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp167)->f2; goto _LL224;} else{ goto _LL225;} _LL225: if(*(( int*) _temp167)
== Cyc_Absyn_Struct_e){ _LL386: _temp385=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp167)->f1; goto _LL384; _LL384: _temp383=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp167)->f2; goto _LL382; _LL382: _temp381=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp167)->f3; goto
_LL380; _LL380: _temp379=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp167)->f4; goto _LL226;} else{ goto _LL227;} _LL227: if(*(( int*) _temp167)
== Cyc_Absyn_Array_e){ _LL388: _temp387=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp167)->f1; goto _LL228;} else{ goto _LL229;} _LL229: if(*(( int*) _temp167)
== Cyc_Absyn_Increment_e){ _LL392: _temp391=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Increment_e_struct*) _temp167)->f1; goto _LL390; _LL390: _temp389=(
void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp167)->f2; goto _LL230;}
else{ goto _LL231;} _LL231: if(*(( int*) _temp167) == Cyc_Absyn_Throw_e){ _LL394:
_temp393=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp167)->f1;
goto _LL232;} else{ goto _LL233;} _LL233: if(*(( int*) _temp167) == Cyc_Absyn_Conditional_e){
_LL400: _temp399=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp167)->f1; goto _LL398; _LL398: _temp397=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp167)->f2; goto _LL396; _LL396: _temp395=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp167)->f3;
goto _LL234;} else{ goto _LL235;} _LL235: if(*(( int*) _temp167) == Cyc_Absyn_StmtExp_e){
_LL402: _temp401=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp167)->f1; goto _LL236;} else{ goto _LL237;} _LL237: if(*(( int*) _temp167)
== Cyc_Absyn_SeqExp_e){ _LL406: _temp405=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp167)->f1; goto _LL404; _LL404: _temp403=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp167)->f2; goto _LL238;} else{ goto _LL239;}
_LL239: if(*(( int*) _temp167) == Cyc_Absyn_Comprehension_e){ _LL412: _temp411=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp167)->f1;
goto _LL410; _LL410: _temp409=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp167)->f2; goto _LL408; _LL408: _temp407=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp167)->f3; goto _LL240;} else{ goto
_LL241;} _LL241: if(*(( int*) _temp167) == Cyc_Absyn_Var_e){ _LL416: _temp415=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp167)->f1; goto _LL414;
_LL414: _temp413=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp167)->f2; if(
_temp413 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL242;} else{ goto _LL243;}}
else{ goto _LL243;} _LL243: if(*(( int*) _temp167) == Cyc_Absyn_UnknownId_e){
_LL418: _temp417=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp167)->f1; goto _LL244;} else{ goto _LL245;} _LL245: if(*(( int*) _temp167)
== Cyc_Absyn_UnknownCall_e){ _LL422: _temp421=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp167)->f1; goto _LL420; _LL420: _temp419=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp167)->f2;
goto _LL246;} else{ goto _LL247;} _LL247: if(*(( int*) _temp167) == Cyc_Absyn_UnresolvedMem_e){
_LL426: _temp425=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp167)->f1; goto _LL424; _LL424: _temp423=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp167)->f2; goto _LL248;} else{ goto
_LL249;} _LL249: if(*(( int*) _temp167) == Cyc_Absyn_CompoundLit_e){ _LL430:
_temp429=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp167)->f1;
goto _LL428; _LL428: _temp427=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp167)->f2; goto _LL250;} else{ goto _LL251;} _LL251: if(*(( int*) _temp167)
== Cyc_Absyn_Codegen_e){ _LL432: _temp431=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp167)->f1; goto _LL252;} else{ goto _LL253;}
_LL253: if(*(( int*) _temp167) == Cyc_Absyn_Fill_e){ _LL434: _temp433=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp167)->f1; goto _LL254;}
else{ goto _LL168;} _LL170: _temp257= _temp255; goto _LL172; _LL172: _temp261=
_temp257; goto _LL174; _LL174: return Cyc_NewControlFlow_abstract_exp( env,
_temp261); _LL176: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp435=( char*)"new_control_flow.cyc:284 env.access_path==null";
struct _tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 47; _temp436;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp437; _temp437.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp439;
_temp439.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp439.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp439;}); _temp437.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp438; _temp438.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp438.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp438;}); _temp437.lvalues= 0; _temp437;}); _LL178: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp440=( char*)"new_control_flow.cyc:289 env.access_path==null";
struct _tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 47; _temp441;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp442; _temp442.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp444;
_temp444.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp444.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp444;});
_temp442.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp443; _temp443.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp443.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp443;}); _temp442.lvalues= 0; _temp442;}); _LL180: goto _LL182; _LL182: goto
_LL184; _LL184: goto _LL186; _LL186: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL188:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL190:
_temp305= _temp299; goto _LL192; _LL192: _temp311= _temp305; goto _LL194; _LL194: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp311)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp445=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp445->hd=( void*) absop; _temp445->tl= 0; _temp445;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL196: { struct Cyc_NewControlFlow_AbsSyn
_temp446= Cyc_NewControlFlow_abstract_exp( env, _temp319); goto _LL447; _LL447: {
struct Cyc_List_List* _temp448= _temp446.lvalues == 0? Cyc_NewControlFlow_use_it_list():
_temp446.lvalues; goto _LL449; _LL449: { struct Cyc_NewControlFlow_AbsSyn
_temp450= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp448), _temp315); goto _LL451; _LL451: { void* _temp454; void* _temp456;
struct Cyc_NewControlFlow_AbsSynOne _temp452= Cyc_NewControlFlow_meet_absexp(
_temp446); _LL457: _temp456=( void*) _temp452.inner_exp; goto _LL455; _LL455:
_temp454=( void*) _temp452.assigns; goto _LL453; _LL453: { void* _temp460; void*
_temp462; struct Cyc_NewControlFlow_AbsSynOne _temp458= Cyc_NewControlFlow_meet_absexp(
_temp450); _LL463: _temp462=( void*) _temp458.inner_exp; goto _LL461; _LL461:
_temp460=( void*) _temp458.assigns; goto _LL459; _LL459: { void* _temp464= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp456, _temp462), _temp460); goto _LL465; _LL465: return Cyc_NewControlFlow_mkSyn_tf(
_temp464, 0, _temp454);}}}}}} _LL198: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp466=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp466->hd=( void*) _temp325; _temp466->tl=({
struct Cyc_List_List* _temp467=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp467->hd=( void*) _temp321; _temp467->tl= 0;
_temp467;}); _temp466;}), 0); _LL200: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp327, 0); _LL202: { int both_leaf= 1;{ void* _temp469=( void*)({ struct
Cyc_Core_Opt* _temp468= _temp331->topt; if( _temp468 == 0){ _throw(
Null_Exception);} _temp468->v;}); struct Cyc_List_List* _temp479; struct Cyc_Absyn_Structdecl**
_temp481; struct Cyc_List_List* _temp483; struct _tuple1* _temp485; struct Cyc_List_List*
_temp487; _LL471: if(( unsigned int) _temp469 > 4u?*(( int*) _temp469) == Cyc_Absyn_AnonStructType:
0){ _LL480: _temp479=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp469)->f1; goto _LL472;} else{ goto _LL473;} _LL473: if(( unsigned int)
_temp469 > 4u?*(( int*) _temp469) == Cyc_Absyn_StructType: 0){ _LL486: _temp485=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp469)->f1; goto
_LL484; _LL484: _temp483=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp469)->f2; goto _LL482; _LL482: _temp481=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp469)->f3; goto _LL474;} else{ goto
_LL475;} _LL475: if(( unsigned int) _temp469 > 4u?*(( int*) _temp469) == Cyc_Absyn_TupleType:
0){ _LL488: _temp487=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp469)->f1; goto _LL476;} else{ goto _LL477;} _LL477: goto _LL478; _LL472:
goto _LL474; _LL474: goto _LL476; _LL476: both_leaf= 0; goto _LL470; _LL478:
goto _LL470; _LL470:;}{ void* _temp490=( void*)({ struct Cyc_Core_Opt* _temp489=
e->topt; if( _temp489 == 0){ _throw( Null_Exception);} _temp489->v;}); struct
Cyc_List_List* _temp500; struct Cyc_Absyn_Structdecl** _temp502; struct Cyc_List_List*
_temp504; struct _tuple1* _temp506; struct Cyc_List_List* _temp508; _LL492: if((
unsigned int) _temp490 > 4u?*(( int*) _temp490) == Cyc_Absyn_AnonStructType: 0){
_LL501: _temp500=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp490)->f1; goto _LL493;} else{ goto _LL494;} _LL494: if(( unsigned int)
_temp490 > 4u?*(( int*) _temp490) == Cyc_Absyn_StructType: 0){ _LL507: _temp506=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp490)->f1; goto
_LL505; _LL505: _temp504=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp490)->f2; goto _LL503; _LL503: _temp502=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp490)->f3; goto _LL495;} else{ goto
_LL496;} _LL496: if(( unsigned int) _temp490 > 4u?*(( int*) _temp490) == Cyc_Absyn_TupleType:
0){ _LL509: _temp508=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp490)->f1; goto _LL497;} else{ goto _LL498;} _LL498: goto _LL499; _LL493:
goto _LL495; _LL495: goto _LL497; _LL497: both_leaf= 0; goto _LL491; _LL499:
goto _LL491; _LL491:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp(
env, _temp331);} return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp510=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp510->hd=( void*) _temp331; _temp510->tl= 0; _temp510;}), env.lhsides);}
_LL204: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp511=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp511->hd=( void*) _temp337; _temp511->tl= _temp335; _temp511;}), 0); _LL206: {
void* _temp513= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp512=
_temp341->topt; if( _temp512 == 0){ _throw( Null_Exception);} _temp512->v;}));
struct Cyc_List_List* _temp519; _LL515: if(( unsigned int) _temp513 > 4u?*(( int*)
_temp513) == Cyc_Absyn_TupleType: 0){ _LL520: _temp519=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp513)->f1; goto _LL516;} else{ goto
_LL517;} _LL517: goto _LL518; _LL516: return Cyc_NewControlFlow_descend_path(
env, e, _temp341); _LL518: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp521=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp521->hd=( void*) _temp341; _temp521->tl=({ struct
Cyc_List_List* _temp522=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp522->hd=( void*) _temp339; _temp522->tl= 0; _temp522;}); _temp521;}), Cyc_NewControlFlow_use_it_list());
_LL514:;} _LL208: { int is_malloc= 0;{ void* _temp523=( void*) _temp343->r;
struct Cyc_Absyn_Structdecl* _temp531; struct Cyc_List_List* _temp533; struct
Cyc_Core_Opt* _temp535; struct _tuple1* _temp537; struct Cyc_List_List* _temp539;
_LL525: if(*(( int*) _temp523) == Cyc_Absyn_Struct_e){ _LL538: _temp537=( struct
_tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp523)->f1; goto _LL536;
_LL536: _temp535=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp523)->f2; goto _LL534; _LL534: _temp533=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp523)->f3; goto _LL532; _LL532: _temp531=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp523)->f4;
goto _LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp523) == Cyc_Absyn_Tuple_e){
_LL540: _temp539=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp523)->f1; goto _LL528;} else{ goto _LL529;} _LL529: goto _LL530; _LL526:
goto _LL528; _LL528: is_malloc= 1; goto _LL524; _LL530: goto _LL524; _LL524:;}
if( is_malloc){ _temp347= 0; _temp345= _temp343; goto _LL210;}{ struct Cyc_List_List*
_temp541= env.access_path; struct Cyc_List_List _temp553; struct Cyc_List_List*
_temp554; struct Cyc_Absyn_Exp* _temp556; struct Cyc_Absyn_Exp _temp558; struct
Cyc_Position_Segment* _temp559; void* _temp561; struct Cyc_Absyn_Exp* _temp563;
struct Cyc_Absyn_Exp* _temp565; struct Cyc_Core_Opt* _temp567; struct Cyc_List_List
_temp569; struct Cyc_List_List* _temp570; struct Cyc_Absyn_Exp* _temp572; struct
Cyc_Absyn_Exp _temp574; struct Cyc_Position_Segment* _temp575; void* _temp577;
struct Cyc_Absyn_Exp* _temp579; struct Cyc_Core_Opt* _temp581; struct Cyc_List_List
_temp583; struct Cyc_List_List* _temp584; struct Cyc_Absyn_Exp* _temp586; struct
Cyc_Absyn_Exp _temp588; struct Cyc_Position_Segment* _temp589; void* _temp591;
struct _tagged_string* _temp593; struct Cyc_Absyn_Exp* _temp595; struct Cyc_Core_Opt*
_temp597; _LL543: if( _temp541 == 0){ goto _LL544;} else{ goto _LL545;} _LL545:
if( _temp541 == 0){ goto _LL547;} else{ _temp553=* _temp541; _LL557: _temp556=(
struct Cyc_Absyn_Exp*) _temp553.hd; _temp558=* _temp556; _LL568: _temp567=(
struct Cyc_Core_Opt*) _temp558.topt; goto _LL562; _LL562: _temp561=( void*)
_temp558.r; if(*(( int*) _temp561) == Cyc_Absyn_Subscript_e){ _LL566: _temp565=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp561)->f1;
goto _LL564; _LL564: _temp563=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp561)->f2; goto _LL560;} else{ goto _LL547;} _LL560: _temp559=( struct Cyc_Position_Segment*)
_temp558.loc; goto _LL555; _LL555: _temp554=( struct Cyc_List_List*) _temp553.tl;
goto _LL546;} _LL547: if( _temp541 == 0){ goto _LL549;} else{ _temp569=*
_temp541; _LL573: _temp572=( struct Cyc_Absyn_Exp*) _temp569.hd; _temp574=*
_temp572; _LL582: _temp581=( struct Cyc_Core_Opt*) _temp574.topt; goto _LL578;
_LL578: _temp577=( void*) _temp574.r; if(*(( int*) _temp577) == Cyc_Absyn_Deref_e){
_LL580: _temp579=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp577)->f1; goto _LL576;} else{ goto _LL549;} _LL576: _temp575=( struct Cyc_Position_Segment*)
_temp574.loc; goto _LL571; _LL571: _temp570=( struct Cyc_List_List*) _temp569.tl;
goto _LL548;} _LL549: if( _temp541 == 0){ goto _LL551;} else{ _temp583=*
_temp541; _LL587: _temp586=( struct Cyc_Absyn_Exp*) _temp583.hd; _temp588=*
_temp586; _LL598: _temp597=( struct Cyc_Core_Opt*) _temp588.topt; goto _LL592;
_LL592: _temp591=( void*) _temp588.r; if(*(( int*) _temp591) == Cyc_Absyn_StructArrow_e){
_LL596: _temp595=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp591)->f1; goto _LL594; _LL594: _temp593=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp591)->f2; goto _LL590;} else{ goto _LL551;}
_LL590: _temp589=( struct Cyc_Position_Segment*) _temp588.loc; goto _LL585;
_LL585: _temp584=( struct Cyc_List_List*) _temp583.tl; goto _LL550;} _LL551:
goto _LL552; _LL544: env.access_path=({ struct Cyc_List_List* _temp599=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp599->hd=( void*)
e; _temp599->tl= env.access_path; _temp599;}); goto _LL542; _LL546: Cyc_Evexp_eval_const_uint_exp(
_temp563) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp600=( char*)"new_control_flow.cyc:410 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp601; _temp601.curr= _temp600; _temp601.base= _temp600;
_temp601.last_plus_one= _temp600 + 61; _temp601;})); _temp570= _temp554; goto
_LL548; _LL548: env.access_path= _temp570; goto _LL542; _LL550: env.access_path=({
struct Cyc_List_List* _temp602=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp602->hd=( void*) Cyc_Absyn_structmember_exp(
_temp595, _temp593, 0); _temp602->tl= _temp584; _temp602;}); goto _LL542; _LL552:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp603=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp603[ 0]=({ struct
Cyc_Core_Impossible_struct _temp604; _temp604.tag= Cyc_Core_Impossible; _temp604.f1=(
struct _tagged_string)({ char* _temp605=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp606; _temp606.curr= _temp605; _temp606.base= _temp605;
_temp606.last_plus_one= _temp605 + 36; _temp606;}); _temp604;}); _temp603;}));
_LL542:;} return Cyc_NewControlFlow_abstract_exp( env, _temp343);} _LL210: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp607= _temp345->topt; if( _temp607 ==
0){ _throw( Null_Exception);} _temp607->v;})); return Cyc_NewControlFlow_abstract_malloc(
env, e, _temp347,( struct Cyc_Absyn_Exp*) _temp345); _LL212: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp349); return Cyc_NewControlFlow_abstract_malloc( env, e, _temp351,
0); _LL214: { struct Cyc_List_List* _temp608= env.access_path; struct Cyc_List_List
_temp614; struct Cyc_List_List* _temp615; struct Cyc_Absyn_Exp* _temp617; struct
Cyc_Absyn_Exp _temp619; struct Cyc_Position_Segment* _temp620; void* _temp622;
struct Cyc_Absyn_Exp* _temp624; struct Cyc_Core_Opt* _temp626; _LL610: if(
_temp608 == 0){ goto _LL612;} else{ _temp614=* _temp608; _LL618: _temp617=(
struct Cyc_Absyn_Exp*) _temp614.hd; _temp619=* _temp617; _LL627: _temp626=(
struct Cyc_Core_Opt*) _temp619.topt; goto _LL623; _LL623: _temp622=( void*)
_temp619.r; if(*(( int*) _temp622) == Cyc_Absyn_Address_e){ _LL625: _temp624=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp622)->f1; goto
_LL621;} else{ goto _LL612;} _LL621: _temp620=( struct Cyc_Position_Segment*)
_temp619.loc; goto _LL616; _LL616: _temp615=( struct Cyc_List_List*) _temp614.tl;
goto _LL611;} _LL612: goto _LL613; _LL611: env.access_path= _temp615;{ struct
Cyc_NewControlFlow_AbsSyn _temp628= Cyc_NewControlFlow_abstract_exp( env,
_temp353); goto _LL629; _LL629: if( _temp628.lvalues == 0){ _temp628.lvalues=
Cyc_NewControlFlow_use_it_list();} return _temp628;} _LL613: return Cyc_NewControlFlow_descend_path(
env, e, _temp353); _LL609:;} _LL216: { void* _temp631= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp630= _temp357->topt; if( _temp630 == 0){
_throw( Null_Exception);} _temp630->v;})); struct Cyc_Absyn_PtrInfo _temp637;
struct Cyc_Absyn_Conref* _temp639; struct Cyc_Absyn_Tqual _temp641; struct Cyc_Absyn_Conref*
_temp643; void* _temp645; void* _temp647; _LL633: if(( unsigned int) _temp631 >
4u?*(( int*) _temp631) == Cyc_Absyn_PointerType: 0){ _LL638: _temp637=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp631)->f1; _LL648:
_temp647=( void*) _temp637.elt_typ; goto _LL646; _LL646: _temp645=( void*)
_temp637.rgn_typ; goto _LL644; _LL644: _temp643=( struct Cyc_Absyn_Conref*)
_temp637.nullable; goto _LL642; _LL642: _temp641=( struct Cyc_Absyn_Tqual)
_temp637.tq; goto _LL640; _LL640: _temp639=( struct Cyc_Absyn_Conref*) _temp637.bounds;
goto _LL634;} else{ goto _LL635;} _LL635: goto _LL636; _LL634:{ void* _temp649=
Cyc_Tcutil_compress( _temp647); struct Cyc_List_List* _temp661; struct Cyc_Absyn_Structdecl**
_temp663; struct Cyc_List_List* _temp665; struct _tuple1* _temp667; struct Cyc_List_List*
_temp669; struct Cyc_Absyn_Uniondecl** _temp671; struct Cyc_List_List* _temp673;
struct _tuple1* _temp675; _LL651: if(( unsigned int) _temp649 > 4u?*(( int*)
_temp649) == Cyc_Absyn_AnonStructType: 0){ _LL662: _temp661=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp649)->f1; goto _LL652;} else{ goto
_LL653;} _LL653: if(( unsigned int) _temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_StructType:
0){ _LL668: _temp667=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp649)->f1; goto _LL666; _LL666: _temp665=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp649)->f2; goto _LL664; _LL664: _temp663=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp649)->f3;
goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int) _temp649 > 4u?*((
int*) _temp649) == Cyc_Absyn_AnonUnionType: 0){ _LL670: _temp669=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp649)->f1; goto _LL656;} else{ goto
_LL657;} _LL657: if(( unsigned int) _temp649 > 4u?*(( int*) _temp649) == Cyc_Absyn_UnionType:
0){ _LL676: _temp675=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp649)->f1; goto _LL674; _LL674: _temp673=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp649)->f2; goto _LL672; _LL672: _temp671=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp649)->f3;
goto _LL658;} else{ goto _LL659;} _LL659: goto _LL660; _LL652: goto _LL654;
_LL654: return Cyc_NewControlFlow_descend_path( env, e, _temp357); _LL656: goto
_LL658; _LL658: { struct Cyc_NewControlFlow_AbsSyn _temp677= Cyc_NewControlFlow_abstract_exp(
env, _temp357); goto _LL678; _LL678: _temp677.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp677;} _LL660: goto _LL650; _LL650:;} goto _LL636; _LL636:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp679=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp679[ 0]=({ struct
Cyc_Core_Impossible_struct _temp680; _temp680.tag= Cyc_Core_Impossible; _temp680.f1=(
struct _tagged_string)({ char* _temp681=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 40; _temp682;}); _temp680;}); _temp679;}));
_LL632:;} _LL218: { void* _temp684= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp683= _temp361->topt; if( _temp683 == 0){ _throw( Null_Exception);} _temp683->v;}));
struct Cyc_List_List* _temp696; struct Cyc_Absyn_Structdecl** _temp698; struct
Cyc_List_List* _temp700; struct _tuple1* _temp702; struct Cyc_List_List*
_temp704; struct Cyc_Absyn_Uniondecl** _temp706; struct Cyc_List_List* _temp708;
struct _tuple1* _temp710; _LL686: if(( unsigned int) _temp684 > 4u?*(( int*)
_temp684) == Cyc_Absyn_AnonStructType: 0){ _LL697: _temp696=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp684)->f1; goto _LL687;} else{ goto
_LL688;} _LL688: if(( unsigned int) _temp684 > 4u?*(( int*) _temp684) == Cyc_Absyn_StructType:
0){ _LL703: _temp702=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp684)->f1; goto _LL701; _LL701: _temp700=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp684)->f2; goto _LL699; _LL699: _temp698=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp684)->f3;
goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int) _temp684 > 4u?*((
int*) _temp684) == Cyc_Absyn_AnonUnionType: 0){ _LL705: _temp704=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp684)->f1; goto _LL691;} else{ goto
_LL692;} _LL692: if(( unsigned int) _temp684 > 4u?*(( int*) _temp684) == Cyc_Absyn_UnionType:
0){ _LL711: _temp710=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp684)->f1; goto _LL709; _LL709: _temp708=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp684)->f2; goto _LL707; _LL707: _temp706=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp684)->f3;
goto _LL693;} else{ goto _LL694;} _LL694: goto _LL695; _LL687: goto _LL689;
_LL689: return Cyc_NewControlFlow_descend_path( env, e, _temp361); _LL691: goto
_LL693; _LL693: { struct Cyc_NewControlFlow_AbsSyn _temp712= Cyc_NewControlFlow_abstract_exp(
env, _temp361); goto _LL713; _LL713: _temp712.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp712;} _LL695:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp714=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp714[ 0]=({ struct Cyc_Core_Impossible_struct _temp715; _temp715.tag= Cyc_Core_Impossible;
_temp715.f1=( struct _tagged_string)({ char* _temp716=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 41; _temp717;}); _temp715;}); _temp714;}));
_LL685:;} _LL220: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp718=( char*)"new_control_flow.cyc:480 env.access_path == null";
struct _tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 49; _temp719;})); if( _temp367 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));} _temp373=
_temp367; goto _LL222; _LL222: { struct Cyc_List_List* _temp720= env.access_path;
struct Cyc_List_List _temp728; struct Cyc_List_List* _temp729; struct Cyc_Absyn_Exp*
_temp731; struct Cyc_Absyn_Exp _temp733; struct Cyc_Position_Segment* _temp734;
void* _temp736; struct Cyc_Absyn_Exp* _temp738; struct Cyc_Absyn_Exp* _temp740;
struct Cyc_Core_Opt* _temp742; _LL722: if( _temp720 == 0){ goto _LL723;} else{
goto _LL724;} _LL724: if( _temp720 == 0){ goto _LL726;} else{ _temp728=*
_temp720; _LL732: _temp731=( struct Cyc_Absyn_Exp*) _temp728.hd; _temp733=*
_temp731; _LL743: _temp742=( struct Cyc_Core_Opt*) _temp733.topt; goto _LL737;
_LL737: _temp736=( void*) _temp733.r; if(*(( int*) _temp736) == Cyc_Absyn_Subscript_e){
_LL741: _temp740=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp736)->f1; goto _LL739; _LL739: _temp738=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp736)->f2; goto _LL735;} else{ goto _LL726;}
_LL735: _temp734=( struct Cyc_Position_Segment*) _temp733.loc; goto _LL730;
_LL730: _temp729=( struct Cyc_List_List*) _temp728.tl; goto _LL725;} _LL726:
goto _LL727; _LL723: { void* _temp744= Cyc_CfAbsexp_mkSkipAE(); goto _LL745;
_LL745: { void* _temp746= Cyc_CfAbsexp_mkSkipAE(); goto _LL747; _LL747:{ int j=
0; for( 0; _temp373 != 0; _temp373=({ struct Cyc_List_List* _temp748= _temp373;
if( _temp748 == 0){ _throw( Null_Exception);} _temp748->tl;}), ++ j){ struct Cyc_List_List*
_temp749= 0; goto _LL750; _LL750:{ struct Cyc_List_List* ls= env.lhsides; for( 0;
ls != 0; ls=({ struct Cyc_List_List* _temp751= ls; if( _temp751 == 0){ _throw(
Null_Exception);} _temp751->tl;})){ if( Cyc_CfAbsexp_isUnknownOp(( void*)({
struct Cyc_List_List* _temp752= ls; if( _temp752 == 0){ _throw( Null_Exception);}
_temp752->hd;}))){ _temp749=({ struct Cyc_List_List* _temp753=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp753->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp753->tl= _temp749; _temp753;});} else{ _temp749=({ struct Cyc_List_List*
_temp754=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp754->hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*)({ struct Cyc_List_List*
_temp755= ls; if( _temp755 == 0){ _throw( Null_Exception);} _temp755->hd;}),(
void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp756=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp756[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp757; _temp757.tag= Cyc_CfFlowInfo_TupleF;
_temp757.f1= j; _temp757;}); _temp756;})); _temp754->tl= _temp749; _temp754;});}}}{
struct Cyc_NewControlFlow_AbsSyn _temp759= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp749),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp758=
_temp373; if( _temp758 == 0){ _throw( Null_Exception);} _temp758->hd;})); goto
_LL760; _LL760: { void* _temp763; void* _temp765; struct Cyc_NewControlFlow_AbsSynOne
_temp761= Cyc_NewControlFlow_meet_absexp( _temp759); _LL766: _temp765=( void*)
_temp761.inner_exp; goto _LL764; _LL764: _temp763=( void*) _temp761.assigns;
goto _LL762; _LL762: _temp744= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp744, _temp765); _temp746= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp746, _temp763);}}}} return Cyc_NewControlFlow_mkSyn_tf( _temp744, 0,
_temp746);}} _LL725: { int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp738);
struct Cyc_NewControlFlow_AbsEnv _temp767= Cyc_NewControlFlow_change_lhs( env, 0);
goto _LL768; _LL768: _temp767.access_path= 0; env.access_path= _temp729;{ void*
_temp769= Cyc_CfAbsexp_mkSkipAE(); goto _LL770; _LL770: { void* _temp771= Cyc_CfAbsexp_mkSkipAE();
goto _LL772; _LL772:{ int j= 0; for( 0; _temp373 != 0; _temp373=({ struct Cyc_List_List*
_temp773= _temp373; if( _temp773 == 0){ _throw( Null_Exception);} _temp773->tl;}),
++ j){ if( i == j){ void* _temp777; void* _temp779; struct Cyc_NewControlFlow_AbsSynOne
_temp775= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp774= _temp373; if( _temp774
== 0){ _throw( Null_Exception);} _temp774->hd;}))); _LL780: _temp779=( void*)
_temp775.inner_exp; goto _LL778; _LL778: _temp777=( void*) _temp775.assigns;
goto _LL776; _LL776: _temp771= _temp777; _temp769= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, _temp769, _temp779);} else{ void* _temp784; void*
_temp786; struct Cyc_NewControlFlow_AbsSynOne _temp782= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp767,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp781= _temp373; if( _temp781 == 0){ _throw( Null_Exception);} _temp781->hd;})));
_LL787: _temp786=( void*) _temp782.inner_exp; goto _LL785; _LL785: _temp784=(
void*) _temp782.assigns; goto _LL783; _LL783: _temp769= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp769, _temp786);}}} return Cyc_NewControlFlow_mkSyn_tf(
_temp769,({ struct Cyc_List_List* _temp788=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp788->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp788->tl= 0; _temp788;}), _temp771);}}} _LL727:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp789=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp789[ 0]=({ struct
Cyc_Core_Impossible_struct _temp790; _temp790.tag= Cyc_Core_Impossible; _temp790.f1=(
struct _tagged_string)({ char* _temp791=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 46; _temp792;}); _temp790;}); _temp789;}));
_LL721:;} _LL224: _temp381= _temp375; goto _LL226; _LL226: { struct Cyc_List_List*
_temp793= env.access_path; struct Cyc_List_List _temp801; struct Cyc_List_List*
_temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Exp _temp806; struct
Cyc_Position_Segment* _temp807; void* _temp809; struct _tagged_string* _temp811;
struct Cyc_Absyn_Exp* _temp813; struct Cyc_Core_Opt* _temp815; _LL795: if(
_temp793 == 0){ goto _LL796;} else{ goto _LL797;} _LL797: if( _temp793 == 0){
goto _LL799;} else{ _temp801=* _temp793; _LL805: _temp804=( struct Cyc_Absyn_Exp*)
_temp801.hd; _temp806=* _temp804; _LL816: _temp815=( struct Cyc_Core_Opt*)
_temp806.topt; goto _LL810; _LL810: _temp809=( void*) _temp806.r; if(*(( int*)
_temp809) == Cyc_Absyn_StructMember_e){ _LL814: _temp813=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp809)->f1; goto _LL812; _LL812:
_temp811=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp809)->f2; goto _LL808;} else{ goto _LL799;} _LL808: _temp807=( struct Cyc_Position_Segment*)
_temp806.loc; goto _LL803; _LL803: _temp802=( struct Cyc_List_List*) _temp801.tl;
goto _LL798;} _LL799: goto _LL800; _LL796: { void* _temp817= Cyc_CfAbsexp_mkSkipAE();
goto _LL818; _LL818: { void* _temp819= Cyc_CfAbsexp_mkSkipAE(); goto _LL820;
_LL820: for( 0; _temp381 != 0; _temp381=({ struct Cyc_List_List* _temp821=
_temp381; if( _temp821 == 0){ _throw( Null_Exception);} _temp821->tl;})){ struct
Cyc_List_List* _temp822= 0; goto _LL823; _LL823:{ struct Cyc_List_List* ls= env.lhsides;
for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp824= ls; if( _temp824 == 0){
_throw( Null_Exception);} _temp824->tl;})){ if( Cyc_CfAbsexp_isUnknownOp(( void*)({
struct Cyc_List_List* _temp825= ls; if( _temp825 == 0){ _throw( Null_Exception);}
_temp825->hd;}))){ _temp822=({ struct Cyc_List_List* _temp826=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp826->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp826->tl= _temp822; _temp826;});} else{ struct Cyc_List_List* ds=(*(( struct
_tuple4*)({ struct Cyc_List_List* _temp847= _temp381; if( _temp847 == 0){ _throw(
Null_Exception);} _temp847->hd;}))).f1; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp827= ds; if( _temp827 == 0){ _throw( Null_Exception);} _temp827->tl;})){
void* _temp829=( void*)({ struct Cyc_List_List* _temp828= ds; if( _temp828 == 0){
_throw( Null_Exception);} _temp828->hd;}); struct Cyc_Absyn_Exp* _temp835;
struct _tagged_string* _temp837; _LL831: if(*(( int*) _temp829) == Cyc_Absyn_ArrayElement){
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
_LL834: _temp822=({ struct Cyc_List_List* _temp843=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp843->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp844= ls; if( _temp844 == 0){ _throw(
Null_Exception);} _temp844->hd;}),( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp845=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp845[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp846; _temp846.tag= Cyc_CfFlowInfo_StructF;
_temp846.f1= _temp837; _temp846;}); _temp845;})); _temp843->tl= _temp822;
_temp843;}); goto _LL830; _LL830:;}}}}{ void** _temp850=( void**)(( void*)({
struct Cyc_Core_Opt* _temp849=((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp848= _temp381; if( _temp848 == 0){ _throw( Null_Exception);} _temp848->hd;}))).f2)->topt;
if( _temp849 == 0){ _throw( Null_Exception);}& _temp849->v;})); goto _LL851;
_LL851: { struct Cyc_NewControlFlow_AbsSyn _temp853= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, _temp822),(*(( struct _tuple4*)({ struct Cyc_List_List*
_temp852= _temp381; if( _temp852 == 0){ _throw( Null_Exception);} _temp852->hd;}))).f2);
goto _LL854; _LL854: { void* _temp857; void* _temp859; struct Cyc_NewControlFlow_AbsSynOne
_temp855= Cyc_NewControlFlow_meet_absexp( _temp853); _LL860: _temp859=( void*)
_temp855.inner_exp; goto _LL858; _LL858: _temp857=( void*) _temp855.assigns;
goto _LL856; _LL856: _temp817= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp817, _temp859); _temp819= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp819, _temp857);}}}} return Cyc_NewControlFlow_mkSyn_tf( _temp817, 0,
_temp819);}} _LL798: { struct Cyc_NewControlFlow_AbsEnv _temp861= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL862; _LL862: _temp861.access_path= 0; env.access_path= _temp802;{
void* _temp863= Cyc_CfAbsexp_mkSkipAE(); goto _LL864; _LL864: { void* _temp865=
Cyc_CfAbsexp_mkSkipAE(); goto _LL866; _LL866: for( 0; _temp381 != 0; _temp381=({
struct Cyc_List_List* _temp867= _temp381; if( _temp867 == 0){ _throw(
Null_Exception);} _temp867->tl;})){ int used= 0;{ struct Cyc_List_List* ds=(*((
struct _tuple4*)({ struct Cyc_List_List* _temp884= _temp381; if( _temp884 == 0){
_throw( Null_Exception);} _temp884->hd;}))).f1; for( 0; ds != 0; ds=({ struct
Cyc_List_List* _temp868= ds; if( _temp868 == 0){ _throw( Null_Exception);}
_temp868->tl;})){ void* _temp870=( void*)({ struct Cyc_List_List* _temp869= ds;
if( _temp869 == 0){ _throw( Null_Exception);} _temp869->hd;}); struct Cyc_Absyn_Exp*
_temp876; struct _tagged_string* _temp878; _LL872: if(*(( int*) _temp870) == Cyc_Absyn_ArrayElement){
_LL877: _temp876=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp870)->f1; goto _LL873;} else{ goto _LL874;} _LL874: if(*(( int*) _temp870)
== Cyc_Absyn_FieldName){ _LL879: _temp878=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp870)->f1; goto _LL875;} else{ goto _LL871;} _LL873:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp880=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp880[ 0]=({ struct
Cyc_Core_Impossible_struct _temp881; _temp881.tag= Cyc_Core_Impossible; _temp881.f1=(
struct _tagged_string)({ char* _temp882=( char*)"bad struct designator"; struct
_tagged_string _temp883; _temp883.curr= _temp882; _temp883.base= _temp882;
_temp883.last_plus_one= _temp882 + 22; _temp883;}); _temp881;}); _temp880;}));
_LL875: if( Cyc_String_zstrptrcmp( _temp811, _temp878) == 0){ used= 1;} goto
_LL871; _LL871:;}} if( used){ void* _temp888; void* _temp890; struct Cyc_NewControlFlow_AbsSynOne
_temp886= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp885= _temp381; if( _temp885 == 0){
_throw( Null_Exception);} _temp885->hd;}))).f2)); _LL891: _temp890=( void*)
_temp886.inner_exp; goto _LL889; _LL889: _temp888=( void*) _temp886.assigns;
goto _LL887; _LL887: _temp865= _temp888; _temp863= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, _temp863, _temp890);} else{ void* _temp895; void*
_temp897; struct Cyc_NewControlFlow_AbsSynOne _temp893= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp861,(*(( struct _tuple4*)({ struct Cyc_List_List*
_temp892= _temp381; if( _temp892 == 0){ _throw( Null_Exception);} _temp892->hd;}))).f2));
_LL898: _temp897=( void*) _temp893.inner_exp; goto _LL896; _LL896: _temp895=(
void*) _temp893.assigns; goto _LL894; _LL894: _temp863= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp863, _temp897);}} return Cyc_NewControlFlow_mkSyn_tf(
_temp863, 0, _temp865);}}} _LL800:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp899=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp899[ 0]=({ struct Cyc_Core_Impossible_struct _temp900; _temp900.tag= Cyc_Core_Impossible;
_temp900.f1=( struct _tagged_string)({ char* _temp901=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 47; _temp902;}); _temp900;}); _temp899;}));
_LL794:;} _LL228: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd, _temp387), 0); _LL230:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp903->hd=( void*) _temp391; _temp903->tl= 0; _temp903;}), 0); _LL232: {
struct Cyc_NewControlFlow_AbsSyn _temp904= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp393); goto _LL905; _LL905: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( _temp904)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL234: { struct Cyc_List_List*
_temp908; struct Cyc_NewControlFlow_AbsSynOne _temp910; struct Cyc_NewControlFlow_AbsSynOne
_temp912; struct Cyc_NewControlFlow_AbsSyn _temp906= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp399); _LL913: _temp912=( struct Cyc_NewControlFlow_AbsSynOne)
_temp906.when_true; goto _LL911; _LL911: _temp910=( struct Cyc_NewControlFlow_AbsSynOne)
_temp906.when_false; goto _LL909; _LL909: _temp908=( struct Cyc_List_List*)
_temp906.lvalues; goto _LL907; _LL907: { struct Cyc_List_List* _temp916; struct
Cyc_NewControlFlow_AbsSynOne _temp918; struct Cyc_NewControlFlow_AbsSynOne
_temp920; struct Cyc_NewControlFlow_AbsSyn _temp914= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp397); _LL921: _temp920=( struct Cyc_NewControlFlow_AbsSynOne)
_temp914.when_true; goto _LL919; _LL919: _temp918=( struct Cyc_NewControlFlow_AbsSynOne)
_temp914.when_false; goto _LL917; _LL917: _temp916=( struct Cyc_List_List*)
_temp914.lvalues; goto _LL915; _LL915: { struct Cyc_List_List* _temp924; struct
Cyc_NewControlFlow_AbsSynOne _temp926; struct Cyc_NewControlFlow_AbsSynOne
_temp928; struct Cyc_NewControlFlow_AbsSyn _temp922= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp395); _LL929: _temp928=( struct Cyc_NewControlFlow_AbsSynOne)
_temp922.when_true; goto _LL927; _LL927: _temp926=( struct Cyc_NewControlFlow_AbsSynOne)
_temp922.when_false; goto _LL925; _LL925: _temp924=( struct Cyc_List_List*)
_temp922.lvalues; goto _LL923; _LL923: { void* _temp930= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp912.inner_exp,(
void*) _temp912.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp920.inner_exp,( void*) _temp920.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp910.inner_exp,( void*) _temp910.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp928.inner_exp,( void*) _temp928.assigns)));
goto _LL931; _LL931: { void* _temp932= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp912.inner_exp,( void*) _temp912.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp918.inner_exp,(
void*) _temp918.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp910.inner_exp,(
void*) _temp910.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp926.inner_exp,( void*) _temp926.assigns))); goto _LL933; _LL933:
return({ struct Cyc_NewControlFlow_AbsSyn _temp934; _temp934.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp936; _temp936.inner_exp=( void*) _temp930;
_temp936.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp936;}); _temp934.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp935;
_temp935.inner_exp=( void*) _temp932; _temp935.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp935;}); _temp934.lvalues= 0; _temp934;});}}}}}
_LL236: { struct Cyc_NewControlFlow_AbsEnv _temp937= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL938; _LL938: _temp937.access_path= 0; Cyc_NewControlFlow_cf_prepass(
_temp937, _temp401);{ struct Cyc_Absyn_Stmt* _temp939= _temp401; goto _LL940;
_LL940: while( 1) { void* _temp941=( void*) _temp939->r; struct Cyc_Absyn_Stmt*
_temp951; struct Cyc_Absyn_Stmt* _temp953; struct Cyc_Absyn_Stmt* _temp955;
struct Cyc_Absyn_Decl* _temp957; struct Cyc_Absyn_Exp* _temp959; _LL943: if((
unsigned int) _temp941 > 1u?*(( int*) _temp941) == Cyc_Absyn_Seq_s: 0){ _LL954:
_temp953=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp941)->f1;
goto _LL952; _LL952: _temp951=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp941)->f2; goto _LL944;} else{ goto _LL945;} _LL945: if(( unsigned int)
_temp941 > 1u?*(( int*) _temp941) == Cyc_Absyn_Decl_s: 0){ _LL958: _temp957=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp941)->f1; goto
_LL956; _LL956: _temp955=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp941)->f2; goto _LL946;} else{ goto _LL947;} _LL947: if(( unsigned int)
_temp941 > 1u?*(( int*) _temp941) == Cyc_Absyn_Exp_s: 0){ _LL960: _temp959=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp941)->f1; goto
_LL948;} else{ goto _LL949;} _LL949: goto _LL950; _LL944: _temp939= _temp951;
continue; _LL946: _temp939= _temp955; continue; _LL948: { struct Cyc_NewControlFlow_AbsSyn
_temp961= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp959); goto _LL962; _LL962: { void* _temp965; void* _temp967; struct Cyc_NewControlFlow_AbsSynOne
_temp963= Cyc_NewControlFlow_meet_absexp( _temp961); _LL968: _temp967=( void*)
_temp963.inner_exp; goto _LL966; _LL966: _temp965=( void*) _temp963.assigns;
goto _LL964; _LL964:( void*)({ struct Cyc_List_List* _temp969=( Cyc_NewControlFlow_get_stmt_annot(
_temp939))->absexps; if( _temp969 == 0){ _throw( Null_Exception);} _temp969->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp967, _temp965);});
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp401), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}} _LL950:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp970=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp970[ 0]=({ struct Cyc_Core_Impossible_struct _temp971; _temp971.tag= Cyc_Core_Impossible;
_temp971.f1=( struct _tagged_string)({ char* _temp972=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp973; _temp973.curr= _temp972; _temp973.base= _temp972;
_temp973.last_plus_one= _temp972 + 33; _temp973;}); _temp971;}); _temp970;}));
_LL942:;}}} _LL238: { struct Cyc_NewControlFlow_AbsEnv _temp974= Cyc_NewControlFlow_change_lhs(
env, 0); goto _LL975; _LL975: _temp974.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn
_temp976= Cyc_NewControlFlow_abstract_exp( _temp974, _temp405); goto _LL977;
_LL977: { struct Cyc_NewControlFlow_AbsSyn _temp978= Cyc_NewControlFlow_abstract_exp(
env, _temp403); goto _LL979; _LL979: { struct Cyc_NewControlFlow_AbsSynOne
_temp980= Cyc_NewControlFlow_meet_absexp( _temp976); goto _LL981; _LL981: return({
struct Cyc_NewControlFlow_AbsSyn _temp982; _temp982.when_true=({ struct Cyc_NewControlFlow_AbsSynOne
_temp984; _temp984.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp980.inner_exp,( void*)( _temp978.when_true).inner_exp); _temp984.assigns=(
void*)(( void*)( _temp978.when_true).assigns); _temp984;}); _temp982.when_false=({
struct Cyc_NewControlFlow_AbsSynOne _temp983; _temp983.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp980.inner_exp,( void*)( _temp978.when_false).inner_exp);
_temp983.assigns=( void*)(( void*)( _temp978.when_false).assigns); _temp983;});
_temp982.lvalues= 0; _temp982;});}}}} _LL240: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp985=( char*)"new_control_flow.cyc:700 env.access_path==null"; struct
_tagged_string _temp986; _temp986.curr= _temp985; _temp986.base= _temp985;
_temp986.last_plus_one= _temp985 + 47; _temp986;})); Cyc_NewControlFlow_add_var_root(
env, _temp411);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp987=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp987->hd=( void*) _temp411; _temp987->tl=( env.shared)->comprehension_vars;
_temp987;});{ struct Cyc_NewControlFlow_AbsEnv _temp988= Cyc_NewControlFlow_use_it_env(
env); goto _LL989; _LL989: { void* _temp992; void* _temp994; struct Cyc_NewControlFlow_AbsSynOne
_temp990= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp988, _temp409)); _LL995: _temp994=( void*) _temp990.inner_exp; goto _LL993;
_LL993: _temp992=( void*) _temp990.assigns; goto _LL991; _LL991: { void*
_temp998; void* _temp1000; struct Cyc_NewControlFlow_AbsSynOne _temp996= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp988, _temp407)); _LL1001: _temp1000=( void*)
_temp996.inner_exp; goto _LL999; _LL999: _temp998=( void*) _temp996.assigns;
goto _LL997; _LL997: { void* _temp1002= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp994, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp992, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp411), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1000, _temp998), Cyc_CfAbsexp_mkSkipAE()))));
goto _LL1003; _LL1003: return Cyc_NewControlFlow_mkSyn_tf( _temp1002, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}}} _LL242: goto _LL244; _LL244: goto _LL246;
_LL246: goto _LL248; _LL248: goto _LL250; _LL250: goto _LL252; _LL252: goto
_LL254; _LL254:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1004=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1004[ 0]=({ struct Cyc_Core_Impossible_struct _temp1005; _temp1005.tag= Cyc_Core_Impossible;
_temp1005.f1=( struct _tagged_string)({ char* _temp1006=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp1007; _temp1007.curr= _temp1006; _temp1007.base=
_temp1006; _temp1007.last_plus_one= _temp1006 + 34; _temp1007;}); _temp1005;});
_temp1004;})); _LL168:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp1010; void* _temp1012; struct Cyc_NewControlFlow_AbsSynOne
_temp1008= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL1013: _temp1012=( void*) _temp1008.inner_exp; goto _LL1011; _LL1011:
_temp1010=( void*) _temp1008.assigns; goto _LL1009; _LL1009: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp1012, _temp1010);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp1014= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
e); goto _LL1015; _LL1015: return({ struct _tuple0 _temp1016; _temp1016.f1= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp1014.when_true).inner_exp,( void*)(
_temp1014.when_true).assigns); _temp1016.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*)( _temp1014.when_false).inner_exp,( void*)( _temp1014.when_false).assigns);
_temp1016;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp1017= 0; goto _LL1018; _LL1018: for( 0; scs != 0; scs=({ struct Cyc_List_List*
_temp1019= scs; if( _temp1019 == 0){ _throw( Null_Exception);} _temp1019->tl;})){
struct Cyc_Absyn_Switch_clause _temp1023; struct Cyc_Position_Segment* _temp1024;
struct Cyc_Absyn_Stmt* _temp1026; struct Cyc_Absyn_Exp* _temp1028; struct Cyc_Core_Opt*
_temp1030; struct Cyc_Absyn_Pat* _temp1032; struct Cyc_Absyn_Switch_clause*
_temp1021=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1020=
scs; if( _temp1020 == 0){ _throw( Null_Exception);} _temp1020->hd;}); _temp1023=*
_temp1021; _LL1033: _temp1032=( struct Cyc_Absyn_Pat*) _temp1023.pattern; goto
_LL1031; _LL1031: _temp1030=( struct Cyc_Core_Opt*) _temp1023.pat_vars; goto
_LL1029; _LL1029: _temp1028=( struct Cyc_Absyn_Exp*) _temp1023.where_clause;
goto _LL1027; _LL1027: _temp1026=( struct Cyc_Absyn_Stmt*) _temp1023.body; goto
_LL1025; _LL1025: _temp1024=( struct Cyc_Position_Segment*) _temp1023.loc; goto
_LL1022; _LL1022: if( _temp1030 == 0){( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1034=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1034[ 0]=({ struct Cyc_Core_Impossible_struct _temp1035; _temp1035.tag= Cyc_Core_Impossible;
_temp1035.f1=( struct _tagged_string)({ char* _temp1036=( char*)"switch clause vds not set";
struct _tagged_string _temp1037; _temp1037.curr= _temp1036; _temp1037.base=
_temp1036; _temp1037.last_plus_one= _temp1036 + 26; _temp1037;}); _temp1035;});
_temp1034;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1040= _temp1030; if( _temp1040 == 0){ _throw( Null_Exception);} _temp1040->v;});
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1038= vds; if( _temp1038 ==
0){ _throw( Null_Exception);} _temp1038->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1039= vds; if(
_temp1039 == 0){ _throw( Null_Exception);} _temp1039->hd;}));}} if( _temp1028 !=
0){ void* _temp1044; void* _temp1046; struct _tuple0 _temp1042= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1041= _temp1028; if(
_temp1041 == 0){ _throw( Null_Exception);} _temp1041;})); _LL1047: _temp1046=
_temp1042.f1; goto _LL1045; _LL1045: _temp1044= _temp1042.f2; goto _LL1043;
_LL1043: _temp1017=({ struct Cyc_List_List* _temp1048=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1048->hd=( void*) _temp1046;
_temp1048->tl=({ struct Cyc_List_List* _temp1049=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1049->hd=( void*) _temp1044;
_temp1049->tl= _temp1017; _temp1049;}); _temp1048;});} Cyc_NewControlFlow_cf_prepass(
env, _temp1026);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1017);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp1050=( char*)"new_control_flow.cyc:752 env.lhsides == null"; struct
_tagged_string _temp1051; _temp1051.curr= _temp1050; _temp1051.base= _temp1050;
_temp1051.last_plus_one= _temp1050 + 45; _temp1051;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1052=( char*)"new_control_flow.cyc:753 env.access_path == null";
struct _tagged_string _temp1053; _temp1053.curr= _temp1052; _temp1053.base=
_temp1052; _temp1053.last_plus_one= _temp1052 + 49; _temp1053;}));{ void*
_temp1054=( void*) s->r; struct Cyc_Absyn_Exp* _temp1102; struct Cyc_Absyn_Stmt*
_temp1104; struct Cyc_Absyn_Stmt* _temp1106; struct Cyc_Absyn_Exp* _temp1108;
struct Cyc_Absyn_Exp* _temp1110; struct Cyc_Absyn_Stmt* _temp1112; struct Cyc_Absyn_Stmt*
_temp1114; struct Cyc_Absyn_Exp* _temp1116; struct _tuple3 _temp1118; struct Cyc_Absyn_Stmt*
_temp1120; struct Cyc_Absyn_Exp* _temp1122; struct Cyc_Absyn_Stmt* _temp1124;
struct Cyc_Absyn_Stmt* _temp1126; struct _tuple3 _temp1128; struct Cyc_Absyn_Stmt*
_temp1130; struct Cyc_Absyn_Exp* _temp1132; struct Cyc_Absyn_Stmt* _temp1134;
struct Cyc_Absyn_Stmt* _temp1136; struct Cyc_Absyn_Stmt* _temp1138; struct
_tagged_string* _temp1140; struct Cyc_Absyn_Stmt* _temp1142; struct
_tagged_string* _temp1144; struct Cyc_Absyn_Stmt* _temp1146; struct _tuple3
_temp1148; struct Cyc_Absyn_Stmt* _temp1150; struct Cyc_Absyn_Exp* _temp1152;
struct _tuple3 _temp1154; struct Cyc_Absyn_Stmt* _temp1156; struct Cyc_Absyn_Exp*
_temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_List_List* _temp1162;
struct Cyc_Absyn_Exp* _temp1164; struct Cyc_Absyn_Switch_clause** _temp1166;
struct Cyc_List_List* _temp1168; struct Cyc_Absyn_Stmt* _temp1170; struct Cyc_Absyn_Decl*
_temp1172; struct Cyc_Absyn_Decl _temp1174; struct Cyc_Position_Segment*
_temp1175; void* _temp1177; struct Cyc_Absyn_Vardecl* _temp1179; struct Cyc_Absyn_Stmt*
_temp1181; struct Cyc_Absyn_Decl* _temp1183; struct Cyc_Absyn_Decl _temp1185;
struct Cyc_Position_Segment* _temp1186; void* _temp1188; int _temp1190; struct
Cyc_Absyn_Exp* _temp1192; struct Cyc_Core_Opt* _temp1194; struct Cyc_Core_Opt*
_temp1196; struct Cyc_Core_Opt _temp1198; struct Cyc_List_List* _temp1199;
struct Cyc_Absyn_Pat* _temp1201; struct Cyc_Absyn_Stmt* _temp1203; struct Cyc_Absyn_Decl*
_temp1205; struct Cyc_Absyn_Stmt* _temp1207; struct Cyc_Absyn_Stmt* _temp1209;
struct Cyc_Absyn_Stmt* _temp1211; struct _tagged_string* _temp1213; struct Cyc_List_List*
_temp1215; struct Cyc_Absyn_Stmt* _temp1217; struct Cyc_Absyn_Stmt* _temp1219;
struct Cyc_Absyn_Vardecl* _temp1221; struct Cyc_Absyn_Tvar* _temp1223; _LL1056:
if( _temp1054 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1057;} else{ goto _LL1058;}
_LL1058: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Exp_s:
0){ _LL1103: _temp1102=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1054)->f1; goto _LL1059;} else{ goto _LL1060;} _LL1060: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Seq_s: 0){ _LL1107: _temp1106=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1054)->f1; goto
_LL1105; _LL1105: _temp1104=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1054)->f2; goto _LL1061;} else{ goto _LL1062;} _LL1062: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Return_s: 0){ _LL1109:
_temp1108=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1054)->f1; if( _temp1108 == 0){ goto _LL1063;} else{ goto _LL1064;}} else{
goto _LL1064;} _LL1064: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054)
== Cyc_Absyn_Return_s: 0){ _LL1111: _temp1110=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1054)->f1; goto _LL1065;} else{ goto _LL1066;}
_LL1066: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_IfThenElse_s:
0){ _LL1117: _temp1116=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1054)->f1; goto _LL1115; _LL1115: _temp1114=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1054)->f2; goto _LL1113; _LL1113:
_temp1112=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1054)->f3; goto _LL1067;} else{ goto _LL1068;} _LL1068: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Do_s: 0){ _LL1125: _temp1124=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1054)->f1; goto
_LL1119; _LL1119: _temp1118=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1054)->f2; _LL1123: _temp1122= _temp1118.f1; goto _LL1121; _LL1121:
_temp1120= _temp1118.f2; goto _LL1069;} else{ goto _LL1070;} _LL1070: if((
unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_While_s: 0){
_LL1129: _temp1128=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1054)->f1; _LL1133: _temp1132= _temp1128.f1; goto _LL1131; _LL1131:
_temp1130= _temp1128.f2; goto _LL1127; _LL1127: _temp1126=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1054)->f2; goto _LL1071;} else{ goto
_LL1072;} _LL1072: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Break_s:
0){ _LL1135: _temp1134=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1054)->f1; goto _LL1073;} else{ goto _LL1074;} _LL1074: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Continue_s: 0){ _LL1137:
_temp1136=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1054)->f1; goto _LL1075;} else{ goto _LL1076;} _LL1076: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Goto_s: 0){ _LL1141: _temp1140=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1054)->f1; goto
_LL1139; _LL1139: _temp1138=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1054)->f2; if( _temp1138 == 0){ goto _LL1077;} else{ goto _LL1078;}} else{
goto _LL1078;} _LL1078: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054)
== Cyc_Absyn_Goto_s: 0){ _LL1145: _temp1144=( struct _tagged_string*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp1054)->f1; goto _LL1143; _LL1143: _temp1142=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1054)->f2; goto
_LL1079;} else{ goto _LL1080;} _LL1080: if(( unsigned int) _temp1054 > 1u?*((
int*) _temp1054) == Cyc_Absyn_For_s: 0){ _LL1161: _temp1160=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp1054)->f1; goto _LL1155; _LL1155: _temp1154=(
struct _tuple3)(( struct Cyc_Absyn_For_s_struct*) _temp1054)->f2; _LL1159:
_temp1158= _temp1154.f1; goto _LL1157; _LL1157: _temp1156= _temp1154.f2; goto
_LL1149; _LL1149: _temp1148=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1054)->f3; _LL1153: _temp1152= _temp1148.f1; goto _LL1151; _LL1151:
_temp1150= _temp1148.f2; goto _LL1147; _LL1147: _temp1146=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_For_s_struct*) _temp1054)->f4; goto _LL1081;} else{ goto
_LL1082;} _LL1082: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Switch_s:
0){ _LL1165: _temp1164=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1054)->f1; goto _LL1163; _LL1163: _temp1162=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1054)->f2; goto _LL1083;} else{ goto
_LL1084;} _LL1084: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Fallthru_s:
0){ _LL1169: _temp1168=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1054)->f1; goto _LL1167; _LL1167: _temp1166=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1054)->f2; goto _LL1085;} else{ goto
_LL1086;} _LL1086: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Decl_s:
0){ _LL1173: _temp1172=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1054)->f1; _temp1174=* _temp1172; _LL1178: _temp1177=( void*) _temp1174.r;
if(*(( int*) _temp1177) == Cyc_Absyn_Var_d){ _LL1180: _temp1179=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1177)->f1; goto _LL1176;} else{ goto
_LL1088;} _LL1176: _temp1175=( struct Cyc_Position_Segment*) _temp1174.loc; goto
_LL1171; _LL1171: _temp1170=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1054)->f2; goto _LL1087;} else{ goto _LL1088;} _LL1088: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Decl_s: 0){ _LL1184: _temp1183=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1054)->f1;
_temp1185=* _temp1183; _LL1189: _temp1188=( void*) _temp1185.r; if(*(( int*)
_temp1188) == Cyc_Absyn_Let_d){ _LL1202: _temp1201=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1188)->f1; goto _LL1197; _LL1197: _temp1196=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1188)->f2; if(
_temp1196 == 0){ goto _LL1090;} else{ _temp1198=* _temp1196; _LL1200: _temp1199=(
struct Cyc_List_List*) _temp1198.v; goto _LL1195;} _LL1195: _temp1194=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1188)->f3; goto _LL1193;
_LL1193: _temp1192=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1188)->f4; goto _LL1191; _LL1191: _temp1190=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1188)->f5; goto _LL1187;} else{ goto _LL1090;} _LL1187: _temp1186=( struct
Cyc_Position_Segment*) _temp1185.loc; goto _LL1182; _LL1182: _temp1181=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1054)->f2; goto _LL1089;}
else{ goto _LL1090;} _LL1090: if(( unsigned int) _temp1054 > 1u?*(( int*)
_temp1054) == Cyc_Absyn_Decl_s: 0){ _LL1206: _temp1205=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1054)->f1; goto _LL1204; _LL1204:
_temp1203=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1054)->f2;
goto _LL1091;} else{ goto _LL1092;} _LL1092: if(( unsigned int) _temp1054 > 1u?*((
int*) _temp1054) == Cyc_Absyn_Cut_s: 0){ _LL1208: _temp1207=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1054)->f1; goto _LL1093;} else{ goto
_LL1094;} _LL1094: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Splice_s:
0){ _LL1210: _temp1209=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1054)->f1; goto _LL1095;} else{ goto _LL1096;} _LL1096: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Label_s: 0){ _LL1214: _temp1213=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1054)->f1; goto
_LL1212; _LL1212: _temp1211=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1054)->f2; goto _LL1097;} else{ goto _LL1098;} _LL1098: if(( unsigned int)
_temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_TryCatch_s: 0){ _LL1218:
_temp1217=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1054)->f1; goto _LL1216; _LL1216: _temp1215=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1054)->f2; goto _LL1099;} else{ goto
_LL1100;} _LL1100: if(( unsigned int) _temp1054 > 1u?*(( int*) _temp1054) == Cyc_Absyn_Region_s:
0){ _LL1224: _temp1223=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1054)->f1; goto _LL1222; _LL1222: _temp1221=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1054)->f2; goto _LL1220; _LL1220:
_temp1219=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1054)->f3; goto _LL1101;} else{ goto _LL1055;} _LL1057: return; _LL1059: {
void* _temp1225= Cyc_NewControlFlow_abstract_exp_top( env, _temp1102); goto
_LL1226; _LL1226: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1227=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1227->hd=( void*) _temp1225; _temp1227->tl= 0; _temp1227;})); return;}
_LL1061: Cyc_NewControlFlow_cf_prepass( env, _temp1106); Cyc_NewControlFlow_cf_prepass(
env, _temp1104); return; _LL1063: return; _LL1065: { void* _temp1229= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1228= _temp1110; if( _temp1228 == 0){ _throw( Null_Exception);} _temp1228;}));
goto _LL1230; _LL1230: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1231=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1231->hd=( void*) _temp1229; _temp1231->tl= 0; _temp1231;})); return;}
_LL1067: { void* _temp1234; void* _temp1236; struct _tuple0 _temp1232= Cyc_NewControlFlow_abstract_guard(
env, _temp1116); _LL1237: _temp1236= _temp1232.f1; goto _LL1235; _LL1235:
_temp1234= _temp1232.f2; goto _LL1233; _LL1233: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1238=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1238->hd=( void*) _temp1236; _temp1238->tl=({
struct Cyc_List_List* _temp1239=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1239->hd=( void*) _temp1234; _temp1239->tl= 0;
_temp1239;}); _temp1238;})); Cyc_NewControlFlow_cf_prepass( env, _temp1114); Cyc_NewControlFlow_cf_prepass(
env, _temp1112); return;} _LL1069: _temp1132= _temp1122; _temp1130= _temp1120;
_temp1126= _temp1124; goto _LL1071; _LL1071: { void* _temp1242; void* _temp1244;
struct _tuple0 _temp1240= Cyc_NewControlFlow_abstract_guard( env, _temp1132);
_LL1245: _temp1244= _temp1240.f1; goto _LL1243; _LL1243: _temp1242= _temp1240.f2;
goto _LL1241; _LL1241: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1246=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1246->hd=( void*) _temp1244; _temp1246->tl=({ struct Cyc_List_List*
_temp1247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1247->hd=( void*) _temp1242; _temp1247->tl= 0; _temp1247;}); _temp1246;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1126); return;} _LL1073: return;
_LL1075: return; _LL1077:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1248=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1248[ 0]=({ struct Cyc_Core_Impossible_struct _temp1249; _temp1249.tag= Cyc_Core_Impossible;
_temp1249.f1=( struct _tagged_string)({ char* _temp1250=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base=
_temp1250; _temp1251.last_plus_one= _temp1250 + 33; _temp1251;}); _temp1249;});
_temp1248;})); _LL1079: { struct Cyc_Absyn_Stmt* _temp1252=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; goto _LL1253; _LL1253: { struct Cyc_Absyn_Stmt* _temp1255=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1254= _temp1142; if(
_temp1254 == 0){ _throw( Null_Exception);} _temp1254;})))->encloser; goto
_LL1256; _LL1256: while( _temp1255 != _temp1252) { struct Cyc_Absyn_Stmt*
_temp1257=( Cyc_NewControlFlow_get_stmt_annot( _temp1252))->encloser; goto
_LL1258; _LL1258: if( _temp1257 == _temp1252){ Cyc_Tcutil_terr( s->loc,( struct
_tagged_string)({ char* _temp1259=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1260; _temp1260.curr= _temp1259; _temp1260.base=
_temp1259; _temp1260.last_plus_one= _temp1259 + 45; _temp1260;})); break;}
_temp1252= _temp1257;} return;}} _LL1081: { void* _temp1261= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1160); goto _LL1262; _LL1262: { void* _temp1265; void* _temp1267;
struct _tuple0 _temp1263= Cyc_NewControlFlow_abstract_guard( env, _temp1158);
_LL1268: _temp1267= _temp1263.f1; goto _LL1266; _LL1266: _temp1265= _temp1263.f2;
goto _LL1264; _LL1264: { void* _temp1269= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1152); goto _LL1270; _LL1270: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1271=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1271->hd=( void*) _temp1261; _temp1271->tl=({
struct Cyc_List_List* _temp1272=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1272->hd=( void*) _temp1267; _temp1272->tl=({
struct Cyc_List_List* _temp1273=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1273->hd=( void*) _temp1265; _temp1273->tl=({
struct Cyc_List_List* _temp1274=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1274->hd=( void*) _temp1269; _temp1274->tl= 0;
_temp1274;}); _temp1273;}); _temp1272;}); _temp1271;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1146); return;}}} _LL1083: { void* _temp1275= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1164); goto _LL1276; _LL1276: { struct
Cyc_List_List* _temp1277= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1162);
goto _LL1278; _LL1278: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1279=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1279->hd=( void*) _temp1275; _temp1279->tl= _temp1277; _temp1279;}));
return;}} _LL1085: { void* _temp1282; void* _temp1284; struct Cyc_NewControlFlow_AbsSynOne
_temp1280= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp1168, 0)); _LL1285: _temp1284=( void*) _temp1280.inner_exp; goto
_LL1283; _LL1283: _temp1282=( void*) _temp1280.assigns; goto _LL1281; _LL1281: {
void* _temp1286= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp1284, _temp1282); goto _LL1287; _LL1287: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1288=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1288->hd=( void*) _temp1286; _temp1288->tl= 0;
_temp1288;})); return;}} _LL1087: Cyc_NewControlFlow_add_var_root( env,
_temp1179); if( _temp1179->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
_temp1290= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1289=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1289->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1179); _temp1289->tl= 0; _temp1289;})); goto
_LL1291; _LL1291: { void* _temp1293= Cyc_NewControlFlow_abstract_exp_top(
_temp1290,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1292= _temp1179->initializer;
if( _temp1292 == 0){ _throw( Null_Exception);} _temp1292;})); goto _LL1294;
_LL1294: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1295=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1295->hd=(
void*) _temp1293; _temp1295->tl= 0; _temp1295;}));}} Cyc_NewControlFlow_cf_prepass(
env, _temp1170); return; _LL1089: for( 0; _temp1199 != 0; _temp1199=({ struct
Cyc_List_List* _temp1296= _temp1199; if( _temp1296 == 0){ _throw( Null_Exception);}
_temp1296->tl;})){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)({
struct Cyc_List_List* _temp1297= _temp1199; if( _temp1297 == 0){ _throw(
Null_Exception);} _temp1297->hd;}));}{ void* _temp1298= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1192); goto _LL1299; _LL1299: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1300=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1300->hd=( void*) _temp1298; _temp1300->tl= 0;
_temp1300;})); Cyc_NewControlFlow_cf_prepass( env, _temp1181); return;} _LL1091:
goto _LL1093; _LL1093: goto _LL1095; _LL1095:( void) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp1301=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1301[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1302; _temp1302.tag= Cyc_Core_Impossible;
_temp1302.f1=( struct _tagged_string)({ char* _temp1303=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1304; _temp1304.curr= _temp1303; _temp1304.base=
_temp1303; _temp1304.last_plus_one= _temp1303 + 26; _temp1304;}); _temp1302;});
_temp1301;})); _LL1097: Cyc_NewControlFlow_cf_prepass( env, _temp1211); return;
_LL1099: Cyc_NewControlFlow_cf_prepass( env, _temp1217);{ struct Cyc_List_List*
_temp1305= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1215); goto _LL1306;
_LL1306: Cyc_NewControlFlow_cf_set_absexps( s, _temp1305); return;} _LL1101: Cyc_NewControlFlow_add_var_root(
env, _temp1221); Cyc_NewControlFlow_cf_prepass( env, _temp1219); return; _LL1055:;}}
static int Cyc_NewControlFlow_iterate_cf_check= 0; static int Cyc_NewControlFlow_iteration_num=
0; struct Cyc_NewControlFlow_AnalEnv; void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow){ if( env->in_try){( void*)(
env->tryflow=( void*) Cyc_CfFlowInfo_join_flow( new_flow,( void*) env->tryflow));}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
void* in_flow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1307= Cyc_NewControlFlow_get_stmt_annot( s); goto _LL1308; _LL1308:( void*)(
_temp1307->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*) _temp1307->flow));
++ _temp1307->visited; _temp1307->visited == Cyc_NewControlFlow_iteration_num? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1309=( char*)"new_control_flow.cyc:885 annot->visited == iteration_num";
struct _tagged_string _temp1310; _temp1310.curr= _temp1309; _temp1310.base=
_temp1309; _temp1310.last_plus_one= _temp1309 + 57; _temp1310;})); return
_temp1307;} static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s,
void* flow){ struct Cyc_NewControlFlow_CFStmtAnnot* _temp1311= Cyc_NewControlFlow_get_stmt_annot(
s); goto _LL1312; _LL1312: { void* _temp1313= Cyc_CfFlowInfo_join_flow( flow,(
void*) _temp1311->flow); goto _LL1314; _LL1314: if( ! Cyc_CfFlowInfo_flow_lessthan_approx(
_temp1313,( void*) _temp1311->flow)){( void*)( _temp1311->flow=( void*)
_temp1313); if( _temp1311->visited == Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check=
1;}}}} static void* Cyc_NewControlFlow_add_init_vars_flow( struct Cyc_NewControlFlow_AnalEnv*
env, void* flow, struct Cyc_List_List* vds){ void* _temp1315= flow; struct Cyc_Dict_Dict*
_temp1321; _LL1317: if( _temp1315 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1318;} else{ goto _LL1319;} _LL1319: if(( unsigned int) _temp1315 > 1u?*((
int*) _temp1315) == Cyc_CfFlowInfo_InitsFL: 0){ _LL1322: _temp1321=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp1315)->f1; goto _LL1320;} else{ goto
_LL1316;} _LL1318: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1320: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1323= vds; if( _temp1323 == 0){ _throw(
Null_Exception);} _temp1323->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1327=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1324=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1324[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1325; _temp1325.tag= Cyc_CfFlowInfo_VarRoot;
_temp1325.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1326= vds;
if( _temp1326 == 0){ _throw( Null_Exception);} _temp1326->hd;}); _temp1325;});
_temp1324;}); goto _LL1328; _LL1328: { void* _temp1329= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( env->roots,( void*) _temp1327)); goto _LL1330; _LL1330:
_temp1321=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( _temp1321,( void*) _temp1327, _temp1329);}} return(
void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1331=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1331[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1332; _temp1332.tag= Cyc_CfFlowInfo_InitsFL;
_temp1332.f1= _temp1321; _temp1332;}); _temp1331;}); _LL1316:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1333= flow; struct Cyc_Dict_Dict* _temp1339; _LL1335: if( _temp1333
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1336;} else{ goto _LL1337;} _LL1337:
if(( unsigned int) _temp1333 > 1u?*(( int*) _temp1333) == Cyc_CfFlowInfo_InitsFL:
0){ _LL1340: _temp1339=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1333)->f1; goto _LL1338;} else{ goto _LL1334;} _LL1336: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1338: for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1341= vds; if(
_temp1341 == 0){ _throw( Null_Exception);} _temp1341->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1345=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1342=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1342[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1343; _temp1343.tag= Cyc_CfFlowInfo_VarRoot;
_temp1343.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1344= vds;
if( _temp1344 == 0){ _throw( Null_Exception);} _temp1344->hd;}); _temp1343;});
_temp1342;}); goto _LL1346; _LL1346: _temp1339=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( _temp1339,(
void*) _temp1345,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots,( void*) _temp1345));} return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1347=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1347[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1348; _temp1348.tag=
Cyc_CfFlowInfo_InitsFL; _temp1348.f1= _temp1339; _temp1348;}); _temp1347;});
_LL1334:;} static struct _tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){
if(( void*)({ struct Cyc_List_List* _temp1349= aes; if( _temp1349 == 0){ _throw(
Null_Exception);} _temp1349->hd;}) ==( void*)({ struct Cyc_List_List* _temp1351=({
struct Cyc_List_List* _temp1350= aes; if( _temp1350 == 0){ _throw(
Null_Exception);} _temp1350->tl;}); if( _temp1351 == 0){ _throw( Null_Exception);}
_temp1351->hd;})){ void* _temp1353= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1352= aes; if( _temp1352 == 0){ _throw(
Null_Exception);} _temp1352->hd;}), in_flow); goto _LL1354; _LL1354: return({
struct _tuple0 _temp1355; _temp1355.f1= _temp1353; _temp1355.f2= _temp1353;
_temp1355;});} return({ struct _tuple0 _temp1356; _temp1356.f1= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1359= aes; if( _temp1359 == 0){
_throw( Null_Exception);} _temp1359->hd;}), in_flow); _temp1356.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1358=({ struct Cyc_List_List*
_temp1357= aes; if( _temp1357 == 0){ _throw( Null_Exception);} _temp1357->tl;});
if( _temp1358 == 0){ _throw( Null_Exception);} _temp1358->hd;}), in_flow);
_temp1356;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp1360=
scs; if( _temp1360 == 0){ _throw( Null_Exception);} _temp1360->tl;})){ struct
Cyc_Absyn_Switch_clause _temp1364; struct Cyc_Position_Segment* _temp1365;
struct Cyc_Absyn_Stmt* _temp1367; struct Cyc_Absyn_Exp* _temp1369; struct Cyc_Core_Opt*
_temp1371; struct Cyc_Absyn_Pat* _temp1373; struct Cyc_Absyn_Switch_clause*
_temp1362=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1361=
scs; if( _temp1361 == 0){ _throw( Null_Exception);} _temp1361->hd;}); _temp1364=*
_temp1362; _LL1374: _temp1373=( struct Cyc_Absyn_Pat*) _temp1364.pattern; goto
_LL1372; _LL1372: _temp1371=( struct Cyc_Core_Opt*) _temp1364.pat_vars; goto
_LL1370; _LL1370: _temp1369=( struct Cyc_Absyn_Exp*) _temp1364.where_clause;
goto _LL1368; _LL1368: _temp1367=( struct Cyc_Absyn_Stmt*) _temp1364.body; goto
_LL1366; _LL1366: _temp1365=( struct Cyc_Position_Segment*) _temp1364.loc; goto
_LL1363; _LL1363: { void* _temp1376= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1375= _temp1371;
if( _temp1375 == 0){ _throw( Null_Exception);} _temp1375->v;})); goto _LL1377;
_LL1377: _temp1376= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1376,(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1378= _temp1371; if(
_temp1378 == 0){ _throw( Null_Exception);} _temp1378->v;}));{ void* body_outflow;
if( _temp1369 != 0){ void* _temp1382; void* _temp1384; struct _tuple0 _temp1380=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1379= _temp1369; if( _temp1379 == 0){ _throw( Null_Exception);} _temp1379;}))->loc,
aes, _temp1376); _LL1385: _temp1384= _temp1380.f1; goto _LL1383; _LL1383:
_temp1382= _temp1380.f2; goto _LL1381; _LL1381: aes=({ struct Cyc_List_List*
_temp1387=({ struct Cyc_List_List* _temp1386= aes; if( _temp1386 == 0){ _throw(
Null_Exception);} _temp1386->tl;}); if( _temp1387 == 0){ _throw( Null_Exception);}
_temp1387->tl;}); in_flow= _temp1382; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1367, _temp1384);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1367, _temp1376);}{ void* _temp1388= body_outflow; _LL1390: if(
_temp1388 ==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1391;} else{ goto _LL1392;}
_LL1392: goto _LL1393; _LL1391: goto _LL1389; _LL1393: Cyc_Tcutil_terr(
_temp1367->loc,( struct _tagged_string)({ char* _temp1394=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1395; _temp1395.curr= _temp1394; _temp1395.base=
_temp1394; _temp1395.last_plus_one= _temp1394 + 38; _temp1395;})); goto _LL1389;
_LL1389:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1396= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); goto _LL1397; _LL1397:
in_flow=( void*) _temp1396->flow;{ struct Cyc_List_List* _temp1398= _temp1396->absexps;
goto _LL1399; _LL1399: { void* _temp1400=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1444; struct Cyc_Absyn_Exp* _temp1446; struct Cyc_Absyn_Exp* _temp1448;
struct Cyc_Absyn_Stmt* _temp1450; struct Cyc_Absyn_Stmt* _temp1452; struct Cyc_Absyn_Stmt*
_temp1454; struct Cyc_Absyn_Stmt* _temp1456; struct Cyc_Absyn_Exp* _temp1458;
struct Cyc_Absyn_Stmt* _temp1460; struct _tuple3 _temp1462; struct Cyc_Absyn_Stmt*
_temp1464; struct Cyc_Absyn_Exp* _temp1466; struct _tuple3 _temp1468; struct Cyc_Absyn_Stmt*
_temp1470; struct Cyc_Absyn_Exp* _temp1472; struct Cyc_Absyn_Stmt* _temp1474;
struct Cyc_Absyn_Stmt* _temp1476; struct _tuple3 _temp1478; struct Cyc_Absyn_Stmt*
_temp1480; struct Cyc_Absyn_Exp* _temp1482; struct _tuple3 _temp1484; struct Cyc_Absyn_Stmt*
_temp1486; struct Cyc_Absyn_Exp* _temp1488; struct Cyc_Absyn_Exp* _temp1490;
struct Cyc_Absyn_Stmt* _temp1492; struct Cyc_Absyn_Switch_clause** _temp1494;
struct Cyc_Absyn_Switch_clause* _temp1496; struct Cyc_List_List* _temp1497;
struct Cyc_Absyn_Stmt* _temp1499; struct Cyc_Absyn_Stmt* _temp1501; struct Cyc_Absyn_Stmt*
_temp1503; struct _tagged_string* _temp1505; struct Cyc_List_List* _temp1507;
struct Cyc_Absyn_Exp* _temp1509; struct Cyc_List_List* _temp1511; struct Cyc_Absyn_Stmt*
_temp1513; struct Cyc_Absyn_Stmt* _temp1515; struct Cyc_Absyn_Decl* _temp1517;
struct Cyc_Absyn_Decl _temp1519; struct Cyc_Position_Segment* _temp1520; void*
_temp1522; struct Cyc_Absyn_Vardecl* _temp1524; struct Cyc_Absyn_Stmt* _temp1526;
struct Cyc_Absyn_Decl* _temp1528; struct Cyc_Absyn_Decl _temp1530; struct Cyc_Position_Segment*
_temp1531; void* _temp1533; int _temp1535; struct Cyc_Absyn_Exp* _temp1537;
struct Cyc_Core_Opt* _temp1539; struct Cyc_Core_Opt* _temp1541; struct Cyc_Core_Opt
_temp1543; struct Cyc_List_List* _temp1544; struct Cyc_Absyn_Pat* _temp1546;
struct Cyc_Absyn_Stmt* _temp1548; struct _tagged_string* _temp1550; struct Cyc_Absyn_Stmt*
_temp1552; struct Cyc_Absyn_Vardecl* _temp1554; struct Cyc_Absyn_Tvar* _temp1556;
_LL1402: if( _temp1400 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1403;} else{ goto
_LL1404;} _LL1404: if(( unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Return_s:
0){ _LL1445: _temp1444=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1400)->f1; if( _temp1444 == 0){ goto _LL1405;} else{ goto _LL1406;}} else{
goto _LL1406;} _LL1406: if(( unsigned int) _temp1400 > 1u?*(( int*) _temp1400)
== Cyc_Absyn_Return_s: 0){ _LL1447: _temp1446=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1400)->f1; goto _LL1407;} else{ goto _LL1408;}
_LL1408: if(( unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Exp_s:
0){ _LL1449: _temp1448=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1400)->f1; goto _LL1409;} else{ goto _LL1410;} _LL1410: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Seq_s: 0){ _LL1453: _temp1452=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1400)->f1; goto
_LL1451; _LL1451: _temp1450=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1400)->f2; goto _LL1411;} else{ goto _LL1412;} _LL1412: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_IfThenElse_s: 0){ _LL1459:
_temp1458=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1400)->f1; goto _LL1457; _LL1457: _temp1456=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1400)->f2; goto _LL1455; _LL1455:
_temp1454=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1400)->f3; goto _LL1413;} else{ goto _LL1414;} _LL1414: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_While_s: 0){ _LL1463: _temp1462=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1400)->f1; _LL1467:
_temp1466= _temp1462.f1; goto _LL1465; _LL1465: _temp1464= _temp1462.f2; goto
_LL1461; _LL1461: _temp1460=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1400)->f2; goto _LL1415;} else{ goto _LL1416;} _LL1416: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Do_s: 0){ _LL1475: _temp1474=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1400)->f1; goto
_LL1469; _LL1469: _temp1468=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1400)->f2; _LL1473: _temp1472= _temp1468.f1; goto _LL1471; _LL1471:
_temp1470= _temp1468.f2; goto _LL1417;} else{ goto _LL1418;} _LL1418: if((
unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_For_s: 0){
_LL1491: _temp1490=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1400)->f1; goto _LL1485; _LL1485: _temp1484=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1400)->f2; _LL1489: _temp1488= _temp1484.f1; goto _LL1487; _LL1487:
_temp1486= _temp1484.f2; goto _LL1479; _LL1479: _temp1478=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1400)->f3; _LL1483: _temp1482= _temp1478.f1;
goto _LL1481; _LL1481: _temp1480= _temp1478.f2; goto _LL1477; _LL1477: _temp1476=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1400)->f4; goto
_LL1419;} else{ goto _LL1420;} _LL1420: if(( unsigned int) _temp1400 > 1u?*((
int*) _temp1400) == Cyc_Absyn_Break_s: 0){ _LL1493: _temp1492=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1400)->f1; if( _temp1492 == 0){ goto
_LL1421;} else{ goto _LL1422;}} else{ goto _LL1422;} _LL1422: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Fallthru_s: 0){ _LL1498:
_temp1497=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1400)->f1; goto _LL1495; _LL1495: _temp1494=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1400)->f2; if( _temp1494 == 0){ goto
_LL1424;} else{ _temp1496=* _temp1494; goto _LL1423;}} else{ goto _LL1424;}
_LL1424: if(( unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Break_s:
0){ _LL1500: _temp1499=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1400)->f1; goto _LL1425;} else{ goto _LL1426;} _LL1426: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Continue_s: 0){ _LL1502:
_temp1501=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1400)->f1; goto _LL1427;} else{ goto _LL1428;} _LL1428: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Goto_s: 0){ _LL1506: _temp1505=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1400)->f1; goto
_LL1504; _LL1504: _temp1503=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1400)->f2; goto _LL1429;} else{ goto _LL1430;} _LL1430: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Switch_s: 0){ _LL1510:
_temp1509=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1400)->f1; goto _LL1508; _LL1508: _temp1507=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1400)->f2; goto _LL1431;} else{ goto
_LL1432;} _LL1432: if(( unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_TryCatch_s:
0){ _LL1514: _temp1513=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1400)->f1; goto _LL1512; _LL1512: _temp1511=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1400)->f2; goto _LL1433;} else{ goto
_LL1434;} _LL1434: if(( unsigned int) _temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Decl_s:
0){ _LL1518: _temp1517=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1400)->f1; _temp1519=* _temp1517; _LL1523: _temp1522=( void*) _temp1519.r;
if(*(( int*) _temp1522) == Cyc_Absyn_Var_d){ _LL1525: _temp1524=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1522)->f1; goto _LL1521;} else{ goto
_LL1436;} _LL1521: _temp1520=( struct Cyc_Position_Segment*) _temp1519.loc; goto
_LL1516; _LL1516: _temp1515=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1400)->f2; goto _LL1435;} else{ goto _LL1436;} _LL1436: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Decl_s: 0){ _LL1529: _temp1528=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1400)->f1;
_temp1530=* _temp1528; _LL1534: _temp1533=( void*) _temp1530.r; if(*(( int*)
_temp1533) == Cyc_Absyn_Let_d){ _LL1547: _temp1546=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1533)->f1; goto _LL1542; _LL1542: _temp1541=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1533)->f2; if(
_temp1541 == 0){ goto _LL1438;} else{ _temp1543=* _temp1541; _LL1545: _temp1544=(
struct Cyc_List_List*) _temp1543.v; goto _LL1540;} _LL1540: _temp1539=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1533)->f3; goto _LL1538;
_LL1538: _temp1537=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1533)->f4; goto _LL1536; _LL1536: _temp1535=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1533)->f5; goto _LL1532;} else{ goto _LL1438;} _LL1532: _temp1531=( struct
Cyc_Position_Segment*) _temp1530.loc; goto _LL1527; _LL1527: _temp1526=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1400)->f2; goto _LL1437;}
else{ goto _LL1438;} _LL1438: if(( unsigned int) _temp1400 > 1u?*(( int*)
_temp1400) == Cyc_Absyn_Label_s: 0){ _LL1551: _temp1550=( struct _tagged_string*)((
struct Cyc_Absyn_Label_s_struct*) _temp1400)->f1; goto _LL1549; _LL1549:
_temp1548=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1400)->f2; goto _LL1439;} else{ goto _LL1440;} _LL1440: if(( unsigned int)
_temp1400 > 1u?*(( int*) _temp1400) == Cyc_Absyn_Region_s: 0){ _LL1557:
_temp1556=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1400)->f1; goto _LL1555; _LL1555: _temp1554=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1400)->f2; goto _LL1553; _LL1553:
_temp1552=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1400)->f3; goto _LL1441;} else{ goto _LL1442;} _LL1442: goto _LL1443;
_LL1403: return in_flow; _LL1405: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1407: Cyc_CfAbsexp_eval_absexp( env,({ struct Cyc_Absyn_Exp* _temp1558=
_temp1446; if( _temp1558 == 0){ _throw( Null_Exception);} _temp1558->loc;}),(
void*)({ struct Cyc_List_List* _temp1559= _temp1398; if( _temp1559 == 0){ _throw(
Null_Exception);} _temp1559->hd;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1409: return Cyc_CfAbsexp_eval_absexp( env, _temp1448->loc,( void*)({ struct
Cyc_List_List* _temp1560= _temp1398; if( _temp1560 == 0){ _throw( Null_Exception);}
_temp1560->hd;}), in_flow); _LL1411: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1450, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1452, in_flow));
_LL1413: { void* _temp1563; void* _temp1565; struct _tuple0 _temp1561= Cyc_NewControlFlow_cf_evalguard(
env, _temp1458->loc, _temp1398, in_flow); _LL1566: _temp1565= _temp1561.f1; goto
_LL1564; _LL1564: _temp1563= _temp1561.f2; goto _LL1562; _LL1562: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1456, _temp1565), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1454, _temp1563));} _LL1415: { void* _temp1567=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1464))->flow; goto _LL1568; _LL1568: { void* _temp1571; void*
_temp1573; struct _tuple0 _temp1569= Cyc_NewControlFlow_cf_evalguard( env,
_temp1466->loc, _temp1398, _temp1567); _LL1574: _temp1573= _temp1569.f1; goto
_LL1572; _LL1572: _temp1571= _temp1569.f2; goto _LL1570; _LL1570: { void*
_temp1575= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1460, _temp1573); goto
_LL1576; _LL1576: Cyc_NewControlFlow_update_flow( _temp1464, _temp1575); return
_temp1571;}}} _LL1417: { void* _temp1577= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1474, in_flow); goto _LL1578; _LL1578: { void* _temp1579=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1577, _temp1470))->flow; goto _LL1580; _LL1580: { void* _temp1583; void*
_temp1585; struct _tuple0 _temp1581= Cyc_NewControlFlow_cf_evalguard( env,
_temp1472->loc, _temp1398, _temp1579); _LL1586: _temp1585= _temp1581.f1; goto
_LL1584; _LL1584: _temp1583= _temp1581.f2; goto _LL1582; _LL1582: Cyc_NewControlFlow_update_flow(
_temp1474, _temp1585); return _temp1583;}}} _LL1419: { void* _temp1588= Cyc_CfAbsexp_eval_absexp(
env, _temp1490->loc,( void*)({ struct Cyc_List_List* _temp1587= _temp1398; if(
_temp1587 == 0){ _throw( Null_Exception);} _temp1587->hd;}), in_flow); goto
_LL1589; _LL1589: { void* _temp1590=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1588, _temp1486))->flow; goto _LL1591; _LL1591: { void* _temp1595; void*
_temp1597; struct _tuple0 _temp1593= Cyc_NewControlFlow_cf_evalguard( env,
_temp1488->loc,({ struct Cyc_List_List* _temp1592= _temp1398; if( _temp1592 == 0){
_throw( Null_Exception);} _temp1592->tl;}), _temp1590); _LL1598: _temp1597=
_temp1593.f1; goto _LL1596; _LL1596: _temp1595= _temp1593.f2; goto _LL1594;
_LL1594: { void* _temp1599= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1476,
_temp1597); goto _LL1600; _LL1600: { void* _temp1601=( void*)( Cyc_NewControlFlow_pre_stmt_check(
_temp1599, _temp1480))->flow; goto _LL1602; _LL1602: { void* _temp1607= Cyc_CfAbsexp_eval_absexp(
env, _temp1482->loc,( void*)({ struct Cyc_List_List* _temp1606=({ struct Cyc_List_List*
_temp1605=({ struct Cyc_List_List* _temp1604=({ struct Cyc_List_List* _temp1603=
_temp1398; if( _temp1603 == 0){ _throw( Null_Exception);} _temp1603->tl;}); if(
_temp1604 == 0){ _throw( Null_Exception);} _temp1604->tl;}); if( _temp1605 == 0){
_throw( Null_Exception);} _temp1605->tl;}); if( _temp1606 == 0){ _throw(
Null_Exception);} _temp1606->hd;}), _temp1601); goto _LL1608; _LL1608: Cyc_NewControlFlow_update_flow(
_temp1486, _temp1607); return _temp1595;}}}}}} _LL1421: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1423: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)({ struct Cyc_List_List*
_temp1609= _temp1398; if( _temp1609 == 0){ _throw( Null_Exception);} _temp1609->hd;}),
in_flow);{ struct Cyc_List_List* _temp1611=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1610= _temp1496->pat_vars; if( _temp1610 == 0){ _throw( Null_Exception);}
_temp1610->v;}); goto _LL1612; _LL1612: in_flow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1611); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env,
in_flow, _temp1611); _temp1499=( struct Cyc_Absyn_Stmt*) _temp1496->body; goto
_LL1425;} _LL1425: _temp1501= _temp1499; goto _LL1427; _LL1427: _temp1503=
_temp1501; goto _LL1429; _LL1429: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Absyn_Stmt* _temp1613= _temp1503; if( _temp1613 == 0){ _throw(
Null_Exception);} _temp1613;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1431: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1509->loc,( void*)({
struct Cyc_List_List* _temp1614= _temp1398; if( _temp1614 == 0){ _throw(
Null_Exception);} _temp1614->hd;}), in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1507,({ struct Cyc_List_List* _temp1615= _temp1398; if( _temp1615 == 0){
_throw( Null_Exception);} _temp1615->tl;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1433: { struct Cyc_NewControlFlow_AnalEnv _temp1618; void* _temp1619; int
_temp1621; struct Cyc_Dict_Dict* _temp1623; struct Cyc_NewControlFlow_AnalEnv*
_temp1616= env; _temp1618=* _temp1616; _LL1624: _temp1623=( struct Cyc_Dict_Dict*)
_temp1618.roots; goto _LL1622; _LL1622: _temp1621=( int) _temp1618.in_try; goto
_LL1620; _LL1620: _temp1619=( void*) _temp1618.tryflow; goto _LL1617; _LL1617:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* _temp1625= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1513, in_flow); goto _LL1626; _LL1626: { void* _temp1627=( void*) env->tryflow;
goto _LL1628; _LL1628: env->in_try= _temp1621;( void*)( env->tryflow=( void*)
_temp1619); Cyc_NewControlFlow_update_tryflow( env, _temp1627); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1511, _temp1398, _temp1627); return _temp1625;}}} _LL1435: { struct
Cyc_List_List _temp1630=({ struct Cyc_List_List _temp1629; _temp1629.hd=( void*)
_temp1524; _temp1629.tl= 0; _temp1629;}); goto _LL1631; _LL1631: { void*
_temp1632= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)&
_temp1630); goto _LL1633; _LL1633: { struct Cyc_Absyn_Exp* _temp1634= _temp1524->initializer;
goto _LL1635; _LL1635: if( _temp1634 != 0){ _temp1632= Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1636= _temp1634; if(
_temp1636 == 0){ _throw( Null_Exception);} _temp1636;}))->loc,( void*)({ struct
Cyc_List_List* _temp1637= _temp1398; if( _temp1637 == 0){ _throw( Null_Exception);}
_temp1637->hd;}), _temp1632);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1515, _temp1632);}}} _LL1437: { void* _temp1638= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1544); goto _LL1639; _LL1639: _temp1638= Cyc_CfAbsexp_eval_absexp(
env, _temp1537->loc,( void*)({ struct Cyc_List_List* _temp1640= _temp1398; if(
_temp1640 == 0){ _throw( Null_Exception);} _temp1640->hd;}), _temp1638);
_temp1638= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1638, _temp1544);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1526, _temp1638);} _LL1439:
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1548, in_flow); _LL1441: {
struct Cyc_List_List* _temp1642=({ struct Cyc_List_List* _temp1641=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1641->hd=( void*) _temp1554;
_temp1641->tl= 0; _temp1641;}); goto _LL1643; _LL1643: { void* _temp1644= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1642); goto _LL1645; _LL1645: _temp1644= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1644, _temp1642); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1552, _temp1644);}} _LL1443:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1646=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1646[ 0]=({ struct Cyc_Core_Impossible_struct _temp1647; _temp1647.tag= Cyc_Core_Impossible;
_temp1647.f1=( struct _tagged_string)({ char* _temp1648=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1649; _temp1649.curr= _temp1648; _temp1649.base=
_temp1648; _temp1649.last_plus_one= _temp1648 + 62; _temp1649;}); _temp1647;});
_temp1646;})); _LL1401:;}}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1650=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1674; struct Cyc_Absyn_Stmt* _temp1676;
struct Cyc_Absyn_Stmt* _temp1678; struct Cyc_Absyn_Stmt* _temp1680; struct Cyc_Absyn_Exp*
_temp1682; struct Cyc_Absyn_Stmt* _temp1684; struct _tuple3 _temp1686; struct
Cyc_Absyn_Stmt* _temp1688; struct Cyc_Absyn_Exp* _temp1690; struct _tuple3
_temp1692; struct Cyc_Absyn_Stmt* _temp1694; struct Cyc_Absyn_Exp* _temp1696;
struct Cyc_Absyn_Exp* _temp1698; struct Cyc_Absyn_Stmt* _temp1700; struct
_tuple3 _temp1702; struct Cyc_Absyn_Stmt* _temp1704; struct Cyc_Absyn_Exp*
_temp1706; struct _tuple3 _temp1708; struct Cyc_Absyn_Stmt* _temp1710; struct
Cyc_Absyn_Exp* _temp1712; struct Cyc_Absyn_Stmt* _temp1714; struct Cyc_Absyn_Stmt*
_temp1716; struct Cyc_Absyn_Vardecl* _temp1718; struct Cyc_Absyn_Tvar* _temp1720;
struct Cyc_Absyn_Stmt* _temp1722; struct _tagged_string* _temp1724; struct Cyc_Absyn_Stmt*
_temp1726; struct Cyc_Absyn_Decl* _temp1728; struct Cyc_List_List* _temp1730;
struct Cyc_Absyn_Stmt* _temp1732; struct Cyc_List_List* _temp1734; struct Cyc_Absyn_Exp*
_temp1736; _LL1652: if(( unsigned int) _temp1650 > 1u?*(( int*) _temp1650) ==
Cyc_Absyn_Seq_s: 0){ _LL1677: _temp1676=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1650)->f1; goto _LL1675; _LL1675: _temp1674=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1650)->f2; goto _LL1653;} else{ goto
_LL1654;} _LL1654: if(( unsigned int) _temp1650 > 1u?*(( int*) _temp1650) == Cyc_Absyn_IfThenElse_s:
0){ _LL1683: _temp1682=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1650)->f1; goto _LL1681; _LL1681: _temp1680=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1650)->f2; goto _LL1679; _LL1679:
_temp1678=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1650)->f3; goto _LL1655;} else{ goto _LL1656;} _LL1656: if(( unsigned int)
_temp1650 > 1u?*(( int*) _temp1650) == Cyc_Absyn_For_s: 0){ _LL1699: _temp1698=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1650)->f1; goto
_LL1693; _LL1693: _temp1692=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1650)->f2; _LL1697: _temp1696= _temp1692.f1; goto _LL1695; _LL1695:
_temp1694= _temp1692.f2; goto _LL1687; _LL1687: _temp1686=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1650)->f3; _LL1691: _temp1690= _temp1686.f1;
goto _LL1689; _LL1689: _temp1688= _temp1686.f2; goto _LL1685; _LL1685: _temp1684=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1650)->f4; goto
_LL1657;} else{ goto _LL1658;} _LL1658: if(( unsigned int) _temp1650 > 1u?*((
int*) _temp1650) == Cyc_Absyn_While_s: 0){ _LL1703: _temp1702=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp1650)->f1; _LL1707: _temp1706= _temp1702.f1;
goto _LL1705; _LL1705: _temp1704= _temp1702.f2; goto _LL1701; _LL1701: _temp1700=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp1650)->f2; goto
_LL1659;} else{ goto _LL1660;} _LL1660: if(( unsigned int) _temp1650 > 1u?*((
int*) _temp1650) == Cyc_Absyn_Do_s: 0){ _LL1715: _temp1714=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1650)->f1; goto _LL1709; _LL1709: _temp1708=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1650)->f2; _LL1713:
_temp1712= _temp1708.f1; goto _LL1711; _LL1711: _temp1710= _temp1708.f2; goto
_LL1661;} else{ goto _LL1662;} _LL1662: if(( unsigned int) _temp1650 > 1u?*((
int*) _temp1650) == Cyc_Absyn_Region_s: 0){ _LL1721: _temp1720=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1650)->f1; goto _LL1719; _LL1719:
_temp1718=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1650)->f2; goto _LL1717; _LL1717: _temp1716=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1650)->f3; goto _LL1663;} else{ goto
_LL1664;} _LL1664: if(( unsigned int) _temp1650 > 1u?*(( int*) _temp1650) == Cyc_Absyn_Label_s:
0){ _LL1725: _temp1724=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1650)->f1; goto _LL1723; _LL1723: _temp1722=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1650)->f2; goto _LL1665;} else{ goto
_LL1666;} _LL1666: if(( unsigned int) _temp1650 > 1u?*(( int*) _temp1650) == Cyc_Absyn_Decl_s:
0){ _LL1729: _temp1728=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1650)->f1; goto _LL1727; _LL1727: _temp1726=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1650)->f2; goto _LL1667;} else{ goto
_LL1668;} _LL1668: if(( unsigned int) _temp1650 > 1u?*(( int*) _temp1650) == Cyc_Absyn_TryCatch_s:
0){ _LL1733: _temp1732=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1650)->f1; goto _LL1731; _LL1731: _temp1730=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1650)->f2; goto _LL1669;} else{ goto
_LL1670;} _LL1670: if(( unsigned int) _temp1650 > 1u?*(( int*) _temp1650) == Cyc_Absyn_Switch_s:
0){ _LL1737: _temp1736=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1650)->f1; goto _LL1735; _LL1735: _temp1734=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1650)->f2; goto _LL1671;} else{ goto
_LL1672;} _LL1672: goto _LL1673; _LL1653: _temp1680= _temp1676; _temp1678=
_temp1674; goto _LL1655; _LL1655: Cyc_NewControlFlow_cf_postpass( _temp1680);
Cyc_NewControlFlow_cf_postpass( _temp1678); return; _LL1657: Cyc_NewControlFlow_cf_postpass(
_temp1688); _temp1704= _temp1684; _temp1700= _temp1694; goto _LL1659; _LL1659:
_temp1714= _temp1700; _temp1710= _temp1704; goto _LL1661; _LL1661: Cyc_NewControlFlow_cf_postpass(
_temp1710); _temp1716= _temp1714; goto _LL1663; _LL1663: _temp1722= _temp1716;
goto _LL1665; _LL1665: _temp1726= _temp1722; goto _LL1667; _LL1667: Cyc_NewControlFlow_cf_postpass(
_temp1726); return; _LL1669: Cyc_NewControlFlow_cf_postpass( _temp1732);
_temp1734= _temp1730; goto _LL1671; _LL1671: for( 0; _temp1734 != 0; _temp1734=({
struct Cyc_List_List* _temp1738= _temp1734; if( _temp1738 == 0){ _throw(
Null_Exception);} _temp1738->tl;})){ Cyc_NewControlFlow_cf_postpass((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1739= _temp1734; if(
_temp1739 == 0){ _throw( Null_Exception);} _temp1739->hd;}))->body);} return;
_LL1673: return; _LL1651:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1771; _temp1771.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1772=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1772->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1772->comprehension_vars=
0; _temp1772;}); _temp1771.lhsides= 0; _temp1771.access_path= 0; _temp1771;});
struct Cyc_List_List* _temp1741=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1740= fd->param_vardecls; if( _temp1740 == 0){ _throw( Null_Exception);}
_temp1740->v;}); goto _LL1742; _LL1742:{ struct Cyc_List_List* vds= _temp1741;
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1743= vds; if( _temp1743 ==
0){ _throw( Null_Exception);} _temp1743->tl;})){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1744= vds; if(
_temp1744 == 0){ _throw( Null_Exception);} _temp1744->hd;}));}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1747; struct Cyc_List_List*
_temp1749; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1751; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1753; struct Cyc_List_List* _temp1754; struct Cyc_Dict_Dict* _temp1756;
struct Cyc_NewControlFlow_AbsEnv _temp1745= absenv; _LL1752: _temp1751=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1745.shared; _temp1753=* _temp1751;
_LL1757: _temp1756=( struct Cyc_Dict_Dict*) _temp1753.all_roots; goto _LL1755;
_LL1755: _temp1754=( struct Cyc_List_List*) _temp1753.comprehension_vars; goto
_LL1750; _LL1750: _temp1749=( struct Cyc_List_List*) _temp1745.lhsides; goto
_LL1748; _LL1748: _temp1747=( struct Cyc_List_List*) _temp1745.access_path; goto
_LL1746; _LL1746: { struct Cyc_NewControlFlow_AnalEnv* _temp1759=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1758=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1758->roots= _temp1756; _temp1758->in_try= 0; _temp1758->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1758;}); goto _LL1760; _LL1760: { void*
in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1769=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1769[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1770; _temp1770.tag= Cyc_CfFlowInfo_InitsFL;
_temp1770.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1770;}); _temp1769;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
_temp1759, in_flow, _temp1741); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
_temp1759, in_flow, _temp1754);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( _temp1759, fd->body, in_flow);}{
void* _temp1761= out_flow; _LL1763: if( _temp1761 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1764;} else{ goto _LL1765;} _LL1765: goto _LL1766; _LL1764: goto _LL1762;
_LL1766: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1767=( char*)"function may complete without returning a value";
struct _tagged_string _temp1768; _temp1768.curr= _temp1767; _temp1768.base=
_temp1767; _temp1768.last_plus_one= _temp1767 + 48; _temp1768;}));} goto _LL1762;
_LL1762:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1773= ds; if( _temp1773 == 0){ _throw( Null_Exception);} _temp1773->tl;})){
void* _temp1775=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1774= ds; if( _temp1774 == 0){ _throw( Null_Exception);} _temp1774->hd;}))->r;
struct Cyc_Absyn_Fndecl* _temp1785; struct Cyc_List_List* _temp1787; struct
_tuple1* _temp1789; struct Cyc_List_List* _temp1791; struct _tagged_string*
_temp1793; _LL1777: if(*(( int*) _temp1775) == Cyc_Absyn_Fn_d){ _LL1786:
_temp1785=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1775)->f1;
goto _LL1778;} else{ goto _LL1779;} _LL1779: if(*(( int*) _temp1775) == Cyc_Absyn_Using_d){
_LL1790: _temp1789=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1775)->f1; goto _LL1788; _LL1788: _temp1787=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1775)->f2; goto _LL1780;} else{ goto
_LL1781;} _LL1781: if(*(( int*) _temp1775) == Cyc_Absyn_Namespace_d){ _LL1794:
_temp1793=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1775)->f1; goto _LL1792; _LL1792: _temp1791=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1775)->f2; goto _LL1782;} else{ goto
_LL1783;} _LL1783: goto _LL1784; _LL1778: Cyc_NewControlFlow_cf_check_fun(
_temp1785); goto _LL1776; _LL1780: _temp1791= _temp1787; goto _LL1782; _LL1782:
Cyc_NewControlFlow_cf_check( _temp1791); goto _LL1776; _LL1784: goto _LL1776;
_LL1776:;}}