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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
14; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 15; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 29; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
30; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 31; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 32; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 33; struct Cyc_Absyn_Fill_e_struct{
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
Cyc_NewControlFlow_CFAnnot[ 12u]="CFAnnot"; struct Cyc_NewControlFlow_CFAnnot_struct{
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
_temp9; struct Cyc_NewControlFlow_CFStmtAnnot* _temp11; _LL5: if(( void*)(*((
struct _xtunion_struct*) _temp3)).tag == Cyc_NewControlFlow_CFAnnot){ _LL10:
_temp9=( struct Cyc_NewControlFlow_CFStmtAnnot)(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; _temp11=&(( struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1;
goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11; _LL8:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp12=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp12[ 0]=({ struct
Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible; _temp13.f1=(
struct _tagged_string)({ char* _temp14=( char*)"ControlFlow -- wrong stmt annotation";
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
_temp107; struct Cyc_Absyn_Exp* _temp109; _LL83: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Address_e){ _LL96: _temp95=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp81)->f1; goto _LL84;} else{ goto _LL85;}
_LL85: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Deref_e){ _LL98:
_temp97=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp81)->f1;
goto _LL86;} else{ goto _LL87;} _LL87: if((( struct _tunion_struct*) _temp81)->tag
== Cyc_Absyn_StructArrow_e){ _LL102: _temp101=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp81)->f1; goto _LL100; _LL100: _temp99=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp81)->f2;
goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _tunion_struct*) _temp81)->tag
== Cyc_Absyn_StructMember_e){ _LL106: _temp105=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp81)->f1; goto _LL104; _LL104: _temp103=(
struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp81)->f2;
goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _tunion_struct*) _temp81)->tag
== Cyc_Absyn_Subscript_e){ _LL110: _temp109=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp81)->f1; goto _LL108; _LL108: _temp107=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp81)->f2;
goto _LL92;} else{ goto _LL93;} _LL93: goto _LL94; _LL84:({ struct Cyc_List_List*
_temp111= path; if( _temp111 == 0){ _throw( Null_Exception);} _temp111->tl;}) ==
0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp112=( char*)"new_control_flow.cyc:239 path->tl == null";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
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
e->r; struct Cyc_Absyn_Exp* _temp208; struct Cyc_Absyn_Exp* _temp210; struct Cyc_List_List*
_temp212; struct Cyc_Absyn_Exp* _temp214; void* _temp216; int _temp218; void*
_temp220; void* _temp222; int _temp224; void* _temp226; void* _temp228; void*
_temp230; struct Cyc_Absyn_Enumfield* _temp232; struct Cyc_Absyn_Enumdecl*
_temp234; struct _tuple1* _temp236; void* _temp238; struct Cyc_Absyn_Fndecl*
_temp240; struct _tuple1* _temp242; void* _temp244; struct Cyc_Absyn_Vardecl*
_temp246; struct _tuple1* _temp248; void* _temp250; struct Cyc_Absyn_Vardecl*
_temp252; struct _tuple1* _temp254; void* _temp256; struct Cyc_Absyn_Vardecl*
_temp258; struct _tuple1* _temp260; void* _temp262; struct Cyc_Absyn_Vardecl*
_temp264; struct _tuple1* _temp266; struct Cyc_Absyn_Tunionfield* _temp268;
struct Cyc_Absyn_Tuniondecl* _temp270; struct Cyc_List_List* _temp272; struct
Cyc_Core_Opt* _temp274; struct Cyc_Core_Opt* _temp276; void* _temp278; struct
Cyc_Absyn_Exp* _temp280; struct Cyc_Absyn_Exp* _temp282; struct Cyc_Core_Opt*
_temp284; struct Cyc_Absyn_Exp* _temp286; struct Cyc_Absyn_Exp* _temp288; struct
Cyc_Core_Opt* _temp290; struct Cyc_Absyn_Exp* _temp292; struct Cyc_List_List*
_temp294; void* _temp296; struct Cyc_Absyn_Exp* _temp298; void* _temp300; struct
Cyc_List_List* _temp302; struct Cyc_Absyn_Exp* _temp304; struct Cyc_Absyn_Exp*
_temp306; struct Cyc_Absyn_Exp* _temp308; struct Cyc_Absyn_Exp* _temp310; struct
Cyc_Absyn_Exp* _temp312; struct Cyc_Absyn_Exp* _temp314; struct Cyc_Absyn_Exp*
_temp316; struct _tagged_string* _temp318; struct Cyc_Absyn_Exp* _temp320;
struct _tagged_string* _temp322; struct Cyc_Absyn_Exp* _temp324; struct Cyc_List_List*
_temp326; struct Cyc_Absyn_Structdecl* _temp328; struct Cyc_List_List* _temp330;
struct Cyc_Core_Opt* _temp332; struct _tuple1* _temp334; struct Cyc_List_List*
_temp336; void* _temp338; struct Cyc_Absyn_Exp* _temp340; struct Cyc_Absyn_Exp*
_temp342; struct Cyc_Absyn_Exp* _temp344; struct Cyc_Absyn_Exp* _temp346; struct
Cyc_Absyn_Exp* _temp348; struct Cyc_Absyn_Stmt* _temp350; struct Cyc_Absyn_Exp*
_temp352; struct Cyc_Absyn_Exp* _temp354; struct Cyc_Absyn_Exp* _temp356; struct
Cyc_Absyn_Exp* _temp358; struct Cyc_Absyn_Vardecl* _temp360; void* _temp362;
struct _tuple1* _temp364; struct _tuple1* _temp366; struct Cyc_List_List*
_temp368; struct Cyc_Absyn_Exp* _temp370; struct Cyc_List_List* _temp372; struct
Cyc_Core_Opt* _temp374; struct Cyc_List_List* _temp376; struct _tuple2* _temp378;
struct Cyc_Absyn_Fndecl* _temp380; struct Cyc_Absyn_Exp* _temp382; _LL124: if(((
struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_NoInstantiate_e){ _LL209:
_temp208=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp122)->f1; goto _LL125;} else{ goto _LL126;} _LL126: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Sizeofexp_e){ _LL211: _temp210=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*) _temp122)->f1;
goto _LL127;} else{ goto _LL128;} _LL128: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Instantiate_e){ _LL215: _temp214=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp122)->f1; goto _LL213; _LL213: _temp212=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp122)->f2;
goto _LL129;} else{ goto _LL130;} _LL130: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Const_e){ _LL217: _temp216=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp122)->f1; if(( unsigned int) _temp216 > 1u?(( struct _tunion_struct*)
_temp216)->tag == Cyc_Absyn_Int_c: 0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp216)->f1; if( _temp220 ==( void*) Cyc_Absyn_Signed){ goto _LL219;} else{
goto _LL132;} _LL219: _temp218=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp216)->f2;
if( _temp218 == 0){ goto _LL131;} else{ goto _LL132;}} else{ goto _LL132;}}
else{ goto _LL132;} _LL132: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Const_e){
_LL223: _temp222=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp122)->f1; if((
unsigned int) _temp222 > 1u?(( struct _tunion_struct*) _temp222)->tag == Cyc_Absyn_Int_c:
0){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp222)->f1;
if( _temp226 ==( void*) Cyc_Absyn_Signed){ goto _LL225;} else{ goto _LL134;}
_LL225: _temp224=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp222)->f2; if(
_temp224 == 1){ goto _LL133;} else{ goto _LL134;}} else{ goto _LL134;}} else{
goto _LL134;} _LL134: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Sizeoftyp_e){
_LL229: _temp228=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp122)->f1;
goto _LL135;} else{ goto _LL136;} _LL136: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Const_e){ _LL231: _temp230=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp122)->f1; goto _LL137;} else{ goto _LL138;} _LL138: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Enum_e){ _LL237: _temp236=( struct
_tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp122)->f1; goto _LL235; _LL235:
_temp234=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp122)->f2; goto _LL233; _LL233: _temp232=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp122)->f3; goto _LL139;} else{ goto _LL140;}
_LL140: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e){ _LL243:
_temp242=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL239; _LL239: _temp238=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp238 > 1u?(( struct _tunion_struct*)
_temp238)->tag == Cyc_Absyn_Funname_b: 0){ _LL241: _temp240=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp238)->f1; goto _LL141;} else{ goto
_LL142;}} else{ goto _LL142;} _LL142: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Var_e){ _LL249: _temp248=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL245; _LL245: _temp244=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp244 > 1u?(( struct _tunion_struct*)
_temp244)->tag == Cyc_Absyn_Global_b: 0){ _LL247: _temp246=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp244)->f1; goto _LL143;} else{ goto
_LL144;}} else{ goto _LL144;} _LL144: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Var_e){ _LL255: _temp254=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f1; goto _LL251; _LL251: _temp250=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp250 > 1u?(( struct _tunion_struct*)
_temp250)->tag == Cyc_Absyn_Param_b: 0){ _LL253: _temp252=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp250)->f1; goto _LL145;} else{ goto _LL146;}}
else{ goto _LL146;} _LL146: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e){
_LL261: _temp260=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL257; _LL257: _temp256=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp256 > 1u?(( struct _tunion_struct*)
_temp256)->tag == Cyc_Absyn_Local_b: 0){ _LL259: _temp258=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp256)->f1; goto _LL147;} else{ goto _LL148;}}
else{ goto _LL148;} _LL148: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e){
_LL267: _temp266=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL263; _LL263: _temp262=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if(( unsigned int) _temp262 > 1u?(( struct _tunion_struct*)
_temp262)->tag == Cyc_Absyn_Pat_b: 0){ _LL265: _temp264=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp262)->f1; goto _LL149;} else{ goto _LL150;}}
else{ goto _LL150;} _LL150: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Tunion_e){
_LL277: _temp276=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp122)->f1; goto _LL275; _LL275: _temp274=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp122)->f2; goto _LL273; _LL273: _temp272=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f3; goto
_LL271; _LL271: _temp270=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp122)->f4; goto _LL269; _LL269: _temp268=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp122)->f5; goto _LL151;} else{ goto
_LL152;} _LL152: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Malloc_e){
_LL281: _temp280=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp122)->f1; goto _LL279; _LL279: _temp278=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp122)->f2; goto _LL153;} else{ goto _LL154;} _LL154: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_AssignOp_e){ _LL287: _temp286=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f1;
goto _LL285; _LL285: _temp284=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f2; if( _temp284 == 0){ goto _LL283;} else{ goto _LL156;} _LL283:
_temp282=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f3; goto _LL155;} else{ goto _LL156;} _LL156: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_AssignOp_e){ _LL293: _temp292=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp122)->f1;
goto _LL291; _LL291: _temp290=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp122)->f2; goto _LL289; _LL289: _temp288=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp122)->f3; goto _LL157;} else{ goto _LL158;}
_LL158: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Primop_e){
_LL297: _temp296=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp122)->f1;
goto _LL295; _LL295: _temp294=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp122)->f2; goto _LL159;} else{ goto _LL160;} _LL160: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Cast_e){ _LL301: _temp300=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp122)->f1; goto _LL299; _LL299: _temp298=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp122)->f2; goto
_LL161;} else{ goto _LL162;} _LL162: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_FnCall_e){ _LL305: _temp304=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp122)->f1; goto _LL303; _LL303: _temp302=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp122)->f2; goto _LL163;} else{ goto _LL164;}
_LL164: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Subscript_e){
_LL309: _temp308=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp122)->f1; goto _LL307; _LL307: _temp306=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp122)->f2; goto _LL165;} else{ goto _LL166;}
_LL166: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_New_e){ _LL313:
_temp312=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*) _temp122)->f1;
goto _LL311; _LL311: _temp310=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp122)->f2; goto _LL167;} else{ goto _LL168;} _LL168: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Address_e){ _LL315: _temp314=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp122)->f1; goto
_LL169;} else{ goto _LL170;} _LL170: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_Deref_e){ _LL317: _temp316=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp122)->f1; goto _LL171;} else{ goto _LL172;} _LL172: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_StructArrow_e){ _LL321: _temp320=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp122)->f1;
goto _LL319; _LL319: _temp318=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp122)->f2; goto _LL173;} else{ goto _LL174;} _LL174: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_StructMember_e){ _LL325: _temp324=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp122)->f1;
goto _LL323; _LL323: _temp322=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp122)->f2; goto _LL175;} else{ goto _LL176;} _LL176: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Tuple_e){ _LL327: _temp326=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp122)->f1; goto _LL177;}
else{ goto _LL178;} _LL178: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Struct_e){
_LL335: _temp334=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp122)->f1; goto _LL333; _LL333: _temp332=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp122)->f2; goto _LL331; _LL331: _temp330=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp122)->f3; goto
_LL329; _LL329: _temp328=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp122)->f4; goto _LL179;} else{ goto _LL180;} _LL180: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Array_e){ _LL337: _temp336=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp122)->f1; goto _LL181;}
else{ goto _LL182;} _LL182: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Increment_e){
_LL341: _temp340=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp122)->f1; goto _LL339; _LL339: _temp338=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp122)->f2; goto _LL183;} else{ goto _LL184;} _LL184: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Throw_e){ _LL343: _temp342=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp122)->f1; goto _LL185;}
else{ goto _LL186;} _LL186: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Conditional_e){
_LL349: _temp348=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp122)->f1; goto _LL347; _LL347: _temp346=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp122)->f2; goto _LL345; _LL345: _temp344=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp122)->f3;
goto _LL187;} else{ goto _LL188;} _LL188: if((( struct _tunion_struct*) _temp122)->tag
== Cyc_Absyn_StmtExp_e){ _LL351: _temp350=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp122)->f1; goto _LL189;} else{ goto _LL190;} _LL190: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_SeqExp_e){ _LL355: _temp354=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp122)->f1; goto
_LL353; _LL353: _temp352=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp122)->f2; goto _LL191;} else{ goto _LL192;} _LL192: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Comprehension_e){ _LL361: _temp360=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp122)->f1;
goto _LL359; _LL359: _temp358=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp122)->f2; goto _LL357; _LL357: _temp356=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp122)->f3; goto _LL193;} else{ goto
_LL194;} _LL194: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Var_e){
_LL365: _temp364=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp122)->f1;
goto _LL363; _LL363: _temp362=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp122)->f2; if( _temp362 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL195;}
else{ goto _LL196;}} else{ goto _LL196;} _LL196: if((( struct _tunion_struct*)
_temp122)->tag == Cyc_Absyn_UnknownId_e){ _LL367: _temp366=( struct _tuple1*)((
struct Cyc_Absyn_UnknownId_e_struct*) _temp122)->f1; goto _LL197;} else{ goto
_LL198;} _LL198: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_UnknownCall_e){
_LL371: _temp370=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp122)->f1; goto _LL369; _LL369: _temp368=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp122)->f2; goto _LL199;} else{ goto _LL200;}
_LL200: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_UnresolvedMem_e){
_LL375: _temp374=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp122)->f1; goto _LL373; _LL373: _temp372=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp122)->f2; goto _LL201;} else{ goto
_LL202;} _LL202: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_CompoundLit_e){
_LL379: _temp378=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp122)->f1; goto _LL377; _LL377: _temp376=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp122)->f2; goto _LL203;} else{ goto _LL204;}
_LL204: if((( struct _tunion_struct*) _temp122)->tag == Cyc_Absyn_Codegen_e){
_LL381: _temp380=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp122)->f1; goto _LL205;} else{ goto _LL206;} _LL206: if((( struct
_tunion_struct*) _temp122)->tag == Cyc_Absyn_Fill_e){ _LL383: _temp382=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp122)->f1; goto _LL207;}
else{ goto _LL123;} _LL125: _temp210= _temp208; goto _LL127; _LL127: _temp214=
_temp210; goto _LL129; _LL129: return Cyc_NewControlFlow_abstract_exp( env,
_temp214); _LL131: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp384=( char*)"new_control_flow.cyc:263 env.access_path==null";
struct _tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 47; _temp385;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp386; _temp386.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp388;
_temp388.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp388.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp388;}); _temp386.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp387; _temp387.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp387.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp387;}); _temp386.lvalues= 0; _temp386;}); _LL133: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp389=( char*)"new_control_flow.cyc:268 env.access_path==null";
struct _tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 47; _temp390;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp391; _temp391.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp393;
_temp393.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp393.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp393;});
_temp391.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp392; _temp392.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp392.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp392;}); _temp391.lvalues= 0; _temp391;}); _LL135: goto _LL137; _LL137: goto
_LL139; _LL139: goto _LL141; _LL141: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL143:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL145:
_temp258= _temp252; goto _LL147; _LL147: _temp264= _temp258; goto _LL149; _LL149: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp264)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp394=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp394->hd=( void*) absop; _temp394->tl= 0; _temp394;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL151: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp395=( char*)"new_control_flow.cyc:298 env.access_path == null";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 49; _temp396;})); if( _temp272 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_tunion_root( env, e,
_temp268);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp272 != 0; _temp272=({ struct Cyc_List_List* _temp397=
_temp272; if( _temp397 == 0){ _throw( Null_Exception);} _temp397->tl;}), ++ i){
struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp405=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp405->hd=( void*) Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkMallocOp(
e),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp406=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp406[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp407; _temp407.tag= Cyc_CfFlowInfo_TupleF;
_temp407.f1= i; _temp407;}); _temp406;})); _temp405->tl= 0; _temp405;})); struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp404= _temp272; if( _temp404 == 0){
_throw( Null_Exception);} _temp404->hd;})); void* _temp400; void* _temp402;
struct Cyc_NewControlFlow_AbsSynOne _temp398= Cyc_NewControlFlow_meet_absexp(
syn); _LL403: _temp402=( void*) _temp398.inner_exp; goto _LL401; _LL401:
_temp400=( void*) _temp398.assigns; goto _LL399; _LL399: es_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp402, es_exp); useexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp400, useexp);}} return Cyc_NewControlFlow_malloc_and_init(
env, e, es_exp, useexp);} _LL153: Cyc_NewControlFlow_add_malloc_root( env, e,
_temp278); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkMallocAE( e), 0,
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e)))); _LL155: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp286); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp282); void* _temp410; void*
_temp412; struct Cyc_NewControlFlow_AbsSynOne _temp408= Cyc_NewControlFlow_meet_absexp(
syn1); _LL413: _temp412=( void*) _temp408.inner_exp; goto _LL411; _LL411:
_temp410=( void*) _temp408.assigns; goto _LL409; _LL409: { void* _temp416; void*
_temp418; struct Cyc_NewControlFlow_AbsSynOne _temp414= Cyc_NewControlFlow_meet_absexp(
syn2); _LL419: _temp418=( void*) _temp414.inner_exp; goto _LL417; _LL417:
_temp416=( void*) _temp414.assigns; goto _LL415; _LL415: { void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp412, _temp418), _temp416); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0,
_temp410);}}} _LL157: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp420=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp420->hd=( void*) _temp292; _temp420->tl=({ struct Cyc_List_List* _temp421=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp421->hd=(
void*) _temp288; _temp421->tl= 0; _temp421;}); _temp420;}), 0); _LL159: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp294, 0); _LL161: { int both_leaf=
1;{ void* _temp423=( void*)({ struct Cyc_Core_Opt* _temp422= _temp298->topt; if(
_temp422 == 0){ _throw( Null_Exception);} _temp422->v;}); struct Cyc_Absyn_Structdecl**
_temp431; struct Cyc_List_List* _temp433; struct _tuple1* _temp435; struct Cyc_List_List*
_temp437; _LL425: if(( unsigned int) _temp423 > 4u?(( struct _tunion_struct*)
_temp423)->tag == Cyc_Absyn_StructType: 0){ _LL436: _temp435=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp423)->f1; goto _LL434; _LL434:
_temp433=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp423)->f2; goto _LL432; _LL432: _temp431=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp423)->f3; goto _LL426;} else{ goto
_LL427;} _LL427: if(( unsigned int) _temp423 > 4u?(( struct _tunion_struct*)
_temp423)->tag == Cyc_Absyn_TupleType: 0){ _LL438: _temp437=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp423)->f1; goto _LL428;} else{ goto
_LL429;} _LL429: goto _LL430; _LL426: goto _LL428; _LL428: both_leaf= 0; goto
_LL424; _LL430: goto _LL424; _LL424:;}{ void* _temp440=( void*)({ struct Cyc_Core_Opt*
_temp439= e->topt; if( _temp439 == 0){ _throw( Null_Exception);} _temp439->v;});
struct Cyc_Absyn_Structdecl** _temp448; struct Cyc_List_List* _temp450; struct
_tuple1* _temp452; struct Cyc_List_List* _temp454; _LL442: if(( unsigned int)
_temp440 > 4u?(( struct _tunion_struct*) _temp440)->tag == Cyc_Absyn_StructType:
0){ _LL453: _temp452=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp440)->f1; goto _LL451; _LL451: _temp450=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp440)->f2; goto _LL449; _LL449: _temp448=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp440)->f3;
goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int) _temp440 > 4u?((
struct _tunion_struct*) _temp440)->tag == Cyc_Absyn_TupleType: 0){ _LL455:
_temp454=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp440)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL443: goto _LL445;
_LL445: both_leaf= 0; goto _LL441; _LL447: goto _LL441; _LL441:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp298);} return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp456=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp456->hd=( void*) _temp298; _temp456->tl= 0;
_temp456;}), env.lhsides);} _LL163: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp457=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp457->hd=( void*) _temp304; _temp457->tl=
_temp302; _temp457;}), 0); _LL165: { void* _temp459= Cyc_Tcutil_compress(( void*)({
struct Cyc_Core_Opt* _temp458= _temp308->topt; if( _temp458 == 0){ _throw(
Null_Exception);} _temp458->v;})); struct Cyc_List_List* _temp465; _LL461: if((
unsigned int) _temp459 > 4u?(( struct _tunion_struct*) _temp459)->tag == Cyc_Absyn_TupleType:
0){ _LL466: _temp465=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp459)->f1; goto _LL462;} else{ goto _LL463;} _LL463: goto _LL464; _LL462:
return Cyc_NewControlFlow_descend_path( env, e, _temp308); _LL464: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp467=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp467->hd=( void*) _temp308; _temp467->tl=({
struct Cyc_List_List* _temp468=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp468->hd=( void*) _temp306; _temp468->tl= 0;
_temp468;}); _temp467;}), Cyc_NewControlFlow_use_it_list()); _LL460:;} _LL167: {
void* _temp469=( void*) _temp310->r; struct Cyc_Absyn_Structdecl* _temp477;
struct Cyc_List_List* _temp479; struct Cyc_Core_Opt* _temp481; struct _tuple1*
_temp483; struct Cyc_List_List* _temp485; _LL471: if((( struct _tunion_struct*)
_temp469)->tag == Cyc_Absyn_Struct_e){ _LL484: _temp483=( struct _tuple1*)((
struct Cyc_Absyn_Struct_e_struct*) _temp469)->f1; goto _LL482; _LL482: _temp481=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*) _temp469)->f2; goto
_LL480; _LL480: _temp479=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp469)->f3; goto _LL478; _LL478: _temp477=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp469)->f4; goto _LL472;} else{ goto
_LL473;} _LL473: if((( struct _tunion_struct*) _temp469)->tag == Cyc_Absyn_Tuple_e){
_LL486: _temp485=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp469)->f1; goto _LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL472:
goto _LL474; _LL474: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp487= _temp310->topt; if( _temp487 ==
0){ _throw( Null_Exception);} _temp487->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp526=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp526->hd=(
void*) malloc_op; _temp526->tl= 0; _temp526;})); inner_env.access_path= 0;{ void*
_temp490; void* _temp492; struct Cyc_NewControlFlow_AbsSynOne _temp488= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp310)); _LL493: _temp492=( void*)
_temp488.inner_exp; goto _LL491; _LL491: _temp490=( void*) _temp488.assigns;
goto _LL489; _LL489: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp492, _temp490); if( env.access_path != 0){ void* _temp495=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp494= env.access_path; if(
_temp494 == 0){ _throw( Null_Exception);} _temp494->hd;}))->r; struct Cyc_Absyn_Exp*
_temp505; struct Cyc_Absyn_Exp* _temp507; struct Cyc_Absyn_Exp* _temp509; struct
_tagged_string* _temp511; struct Cyc_Absyn_Exp* _temp513; _LL497: if((( struct
_tunion_struct*) _temp495)->tag == Cyc_Absyn_Subscript_e){ _LL508: _temp507=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp495)->f1;
goto _LL506; _LL506: _temp505=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp495)->f2; goto _LL498;} else{ goto _LL499;} _LL499: if((( struct
_tunion_struct*) _temp495)->tag == Cyc_Absyn_Deref_e){ _LL510: _temp509=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp495)->f1; goto _LL500;}
else{ goto _LL501;} _LL501: if((( struct _tunion_struct*) _temp495)->tag == Cyc_Absyn_StructArrow_e){
_LL514: _temp513=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp495)->f1; goto _LL512; _LL512: _temp511=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp495)->f2; goto _LL502;} else{ goto _LL503;}
_LL503: goto _LL504; _LL498: Cyc_Evexp_eval_const_uint_exp( _temp505) == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp515=( char*)"new_control_flow.cyc:411 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp516; _temp516.curr= _temp515; _temp516.base= _temp515;
_temp516.last_plus_one= _temp515 + 61; _temp516;})); goto _LL500; _LL500: env.access_path=({
struct Cyc_List_List* _temp517= env.access_path; if( _temp517 == 0){ _throw(
Null_Exception);} _temp517->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp518=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp518->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp518->tl= 0;
_temp518;}); goto _LL496; _LL502: env.access_path=({ struct Cyc_List_List*
_temp519=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp519->hd=( void*) Cyc_Absyn_structmember_exp( _temp513, _temp511, 0);
_temp519->tl=({ struct Cyc_List_List* _temp520= env.access_path; if( _temp520 ==
0){ _throw( Null_Exception);} _temp520->tl;}); _temp519;}); ans.lvalues=({
struct Cyc_List_List* _temp521=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp521->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp521->tl= 0; _temp521;}); goto _LL496; _LL504:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp522=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp522[ 0]=({ struct
Cyc_Core_Impossible_struct _temp523; _temp523.tag= Cyc_Core_Impossible; _temp523.f1=(
struct _tagged_string)({ char* _temp524=( char*)"bad malloc access path"; struct
_tagged_string _temp525; _temp525.curr= _temp524; _temp525.base= _temp524;
_temp525.last_plus_one= _temp524 + 23; _temp525;}); _temp523;}); _temp522;}));
_LL496:;} return ans;}}}} _LL476: if( _temp312 == 0){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp527=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp527->hd=( void*) _temp310; _temp527->tl= 0;
_temp527;}), 0);} else{ return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp528=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp528->hd=( void*)(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp530= _temp312; if( _temp530 == 0){ _throw( Null_Exception);}
_temp530;})); _temp528->tl=({ struct Cyc_List_List* _temp529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp529->hd=( void*) _temp310;
_temp529->tl= 0; _temp529;}); _temp528;}), 0);} _LL470:;} _LL169:{ void*
_temp531=( void*) _temp314->r; struct Cyc_Absyn_Structdecl* _temp539; struct Cyc_List_List*
_temp541; struct Cyc_Core_Opt* _temp543; struct _tuple1* _temp545; struct Cyc_List_List*
_temp547; _LL533: if((( struct _tunion_struct*) _temp531)->tag == Cyc_Absyn_Struct_e){
_LL546: _temp545=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp531)->f1; goto _LL544; _LL544: _temp543=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp531)->f2; goto _LL542; _LL542: _temp541=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp531)->f3; goto
_LL540; _LL540: _temp539=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp531)->f4; goto _LL534;} else{ goto _LL535;} _LL535: if((( struct
_tunion_struct*) _temp531)->tag == Cyc_Absyn_Tuple_e){ _LL548: _temp547=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp531)->f1; goto _LL536;}
else{ goto _LL537;} _LL537: goto _LL538; _LL534: goto _LL536; _LL536: { void*
malloc_op= Cyc_CfAbsexp_mkMallocOp( e); Cyc_NewControlFlow_add_malloc_root( env,
e,( void*)({ struct Cyc_Core_Opt* _temp549= _temp314->topt; if( _temp549 == 0){
_throw( Null_Exception);} _temp549->v;}));{ struct Cyc_NewControlFlow_AbsEnv
inner_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp588=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp588->hd=(
void*) malloc_op; _temp588->tl= 0; _temp588;})); inner_env.access_path= 0;{ void*
_temp552; void* _temp554; struct Cyc_NewControlFlow_AbsSynOne _temp550= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( inner_env, _temp314)); _LL555: _temp554=( void*)
_temp550.inner_exp; goto _LL553; _LL553: _temp552=( void*) _temp550.assigns;
goto _LL551; _LL551: { struct Cyc_NewControlFlow_AbsSyn ans= Cyc_NewControlFlow_malloc_and_init(
env, e, _temp554, _temp552); if( env.access_path != 0){ void* _temp557=( void*)((
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp556= env.access_path; if(
_temp556 == 0){ _throw( Null_Exception);} _temp556->hd;}))->r; struct Cyc_Absyn_Exp*
_temp567; struct Cyc_Absyn_Exp* _temp569; struct Cyc_Absyn_Exp* _temp571; struct
_tagged_string* _temp573; struct Cyc_Absyn_Exp* _temp575; _LL559: if((( struct
_tunion_struct*) _temp557)->tag == Cyc_Absyn_Subscript_e){ _LL570: _temp569=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp557)->f1;
goto _LL568; _LL568: _temp567=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp557)->f2; goto _LL560;} else{ goto _LL561;} _LL561: if((( struct
_tunion_struct*) _temp557)->tag == Cyc_Absyn_Deref_e){ _LL572: _temp571=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp557)->f1; goto _LL562;}
else{ goto _LL563;} _LL563: if((( struct _tunion_struct*) _temp557)->tag == Cyc_Absyn_StructArrow_e){
_LL576: _temp575=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp557)->f1; goto _LL574; _LL574: _temp573=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp557)->f2; goto _LL564;} else{ goto _LL565;}
_LL565: goto _LL566; _LL560: Cyc_Evexp_eval_const_uint_exp( _temp567) == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp577=( char*)"new_control_flow.cyc:451 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 61; _temp578;})); goto _LL562; _LL562: env.access_path=({
struct Cyc_List_List* _temp579= env.access_path; if( _temp579 == 0){ _throw(
Null_Exception);} _temp579->tl;}); ans.lvalues=({ struct Cyc_List_List* _temp580=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp580->hd=(
void*) Cyc_NewControlFlow_destruct_path( env, malloc_op); _temp580->tl= 0;
_temp580;}); goto _LL558; _LL564: env.access_path=({ struct Cyc_List_List*
_temp581=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp581->hd=( void*) Cyc_Absyn_structmember_exp( _temp575, _temp573, 0);
_temp581->tl=({ struct Cyc_List_List* _temp582= env.access_path; if( _temp582 ==
0){ _throw( Null_Exception);} _temp582->tl;}); _temp581;}); ans.lvalues=({
struct Cyc_List_List* _temp583=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp583->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp583->tl= 0; _temp583;}); goto _LL558; _LL566:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp584=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp584[ 0]=({ struct
Cyc_Core_Impossible_struct _temp585; _temp585.tag= Cyc_Core_Impossible; _temp585.f1=(
struct _tagged_string)({ char* _temp586=( char*)"bad malloc access path"; struct
_tagged_string _temp587; _temp587.curr= _temp586; _temp587.base= _temp586;
_temp587.last_plus_one= _temp586 + 23; _temp587;}); _temp585;}); _temp584;}));
_LL558:;} return ans;}}}} _LL538: goto _LL532; _LL532:;}{ struct Cyc_List_List*
_temp589= env.access_path; struct Cyc_List_List _temp601; struct Cyc_List_List*
_temp602; struct Cyc_Absyn_Exp* _temp604; struct Cyc_Absyn_Exp _temp606; struct
Cyc_Position_Segment* _temp607; void* _temp609; struct Cyc_Absyn_Exp* _temp611;
struct Cyc_Absyn_Exp* _temp613; struct Cyc_Core_Opt* _temp615; struct Cyc_List_List
_temp617; struct Cyc_List_List* _temp618; struct Cyc_Absyn_Exp* _temp620; struct
Cyc_Absyn_Exp _temp622; struct Cyc_Position_Segment* _temp623; void* _temp625;
struct Cyc_Absyn_Exp* _temp627; struct Cyc_Core_Opt* _temp629; struct Cyc_List_List
_temp631; struct Cyc_List_List* _temp632; struct Cyc_Absyn_Exp* _temp634; struct
Cyc_Absyn_Exp _temp636; struct Cyc_Position_Segment* _temp637; void* _temp639;
struct _tagged_string* _temp641; struct Cyc_Absyn_Exp* _temp643; struct Cyc_Core_Opt*
_temp645; _LL591: if( _temp589 == 0){ goto _LL592;} else{ goto _LL593;} _LL593:
if( _temp589 == 0){ goto _LL595;} else{ _temp601=* _temp589; _LL605: _temp604=(
struct Cyc_Absyn_Exp*) _temp601.hd; _temp606=* _temp604; _LL616: _temp615=(
struct Cyc_Core_Opt*) _temp606.topt; goto _LL610; _LL610: _temp609=( void*)
_temp606.r; if((( struct _tunion_struct*) _temp609)->tag == Cyc_Absyn_Subscript_e){
_LL614: _temp613=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp609)->f1; goto _LL612; _LL612: _temp611=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp609)->f2; goto _LL608;} else{ goto _LL595;}
_LL608: _temp607=( struct Cyc_Position_Segment*) _temp606.loc; goto _LL603;
_LL603: _temp602=( struct Cyc_List_List*) _temp601.tl; goto _LL594;} _LL595: if(
_temp589 == 0){ goto _LL597;} else{ _temp617=* _temp589; _LL621: _temp620=(
struct Cyc_Absyn_Exp*) _temp617.hd; _temp622=* _temp620; _LL630: _temp629=(
struct Cyc_Core_Opt*) _temp622.topt; goto _LL626; _LL626: _temp625=( void*)
_temp622.r; if((( struct _tunion_struct*) _temp625)->tag == Cyc_Absyn_Deref_e){
_LL628: _temp627=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp625)->f1; goto _LL624;} else{ goto _LL597;} _LL624: _temp623=( struct Cyc_Position_Segment*)
_temp622.loc; goto _LL619; _LL619: _temp618=( struct Cyc_List_List*) _temp617.tl;
goto _LL596;} _LL597: if( _temp589 == 0){ goto _LL599;} else{ _temp631=*
_temp589; _LL635: _temp634=( struct Cyc_Absyn_Exp*) _temp631.hd; _temp636=*
_temp634; _LL646: _temp645=( struct Cyc_Core_Opt*) _temp636.topt; goto _LL640;
_LL640: _temp639=( void*) _temp636.r; if((( struct _tunion_struct*) _temp639)->tag
== Cyc_Absyn_StructArrow_e){ _LL644: _temp643=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp639)->f1; goto _LL642; _LL642: _temp641=(
struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp639)->f2;
goto _LL638;} else{ goto _LL599;} _LL638: _temp637=( struct Cyc_Position_Segment*)
_temp636.loc; goto _LL633; _LL633: _temp632=( struct Cyc_List_List*) _temp631.tl;
goto _LL598;} _LL599: goto _LL600; _LL592: env.access_path=({ struct Cyc_List_List*
_temp647=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp647->hd=( void*) e; _temp647->tl= env.access_path; _temp647;}); goto _LL590;
_LL594: Cyc_Evexp_eval_const_uint_exp( _temp611) == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp648=( char*)"new_control_flow.cyc:479 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp649; _temp649.curr= _temp648; _temp649.base= _temp648;
_temp649.last_plus_one= _temp648 + 61; _temp649;})); _temp618= _temp602; goto
_LL596; _LL596: env.access_path= _temp618; goto _LL590; _LL598: env.access_path=({
struct Cyc_List_List* _temp650=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp650->hd=( void*) Cyc_Absyn_structmember_exp(
_temp643, _temp641, 0); _temp650->tl= _temp632; _temp650;}); goto _LL590; _LL600:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp651=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp651[ 0]=({ struct
Cyc_Core_Impossible_struct _temp652; _temp652.tag= Cyc_Core_Impossible; _temp652.f1=(
struct _tagged_string)({ char* _temp653=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp654; _temp654.curr= _temp653; _temp654.base= _temp653;
_temp654.last_plus_one= _temp653 + 36; _temp654;}); _temp652;}); _temp651;}));
_LL590:;} return Cyc_NewControlFlow_abstract_exp( env, _temp314); _LL171: {
struct Cyc_List_List* _temp655= env.access_path; struct Cyc_List_List _temp661;
struct Cyc_List_List* _temp662; struct Cyc_Absyn_Exp* _temp664; struct Cyc_Absyn_Exp
_temp666; struct Cyc_Position_Segment* _temp667; void* _temp669; struct Cyc_Absyn_Exp*
_temp671; struct Cyc_Core_Opt* _temp673; _LL657: if( _temp655 == 0){ goto _LL659;}
else{ _temp661=* _temp655; _LL665: _temp664=( struct Cyc_Absyn_Exp*) _temp661.hd;
_temp666=* _temp664; _LL674: _temp673=( struct Cyc_Core_Opt*) _temp666.topt;
goto _LL670; _LL670: _temp669=( void*) _temp666.r; if((( struct _tunion_struct*)
_temp669)->tag == Cyc_Absyn_Address_e){ _LL672: _temp671=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp669)->f1; goto _LL668;} else{ goto
_LL659;} _LL668: _temp667=( struct Cyc_Position_Segment*) _temp666.loc; goto
_LL663; _LL663: _temp662=( struct Cyc_List_List*) _temp661.tl; goto _LL658;}
_LL659: goto _LL660; _LL658: env.access_path= _temp662;{ struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp316); if( syn1.lvalues == 0){
syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return syn1;} _LL660: return
Cyc_NewControlFlow_descend_path( env, e, _temp316); _LL656:;} _LL173: { void*
_temp676= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp675= _temp320->topt;
if( _temp675 == 0){ _throw( Null_Exception);} _temp675->v;})); struct Cyc_Absyn_PtrInfo
_temp682; struct Cyc_Absyn_Conref* _temp684; struct Cyc_Absyn_Tqual _temp686;
struct Cyc_Absyn_Conref* _temp688; void* _temp690; void* _temp692; _LL678: if((
unsigned int) _temp676 > 4u?(( struct _tunion_struct*) _temp676)->tag == Cyc_Absyn_PointerType:
0){ _LL683: _temp682=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp676)->f1; _LL693: _temp692=( void*) _temp682.elt_typ; goto _LL691; _LL691:
_temp690=( void*) _temp682.rgn_typ; goto _LL689; _LL689: _temp688=( struct Cyc_Absyn_Conref*)
_temp682.nullable; goto _LL687; _LL687: _temp686=( struct Cyc_Absyn_Tqual)
_temp682.tq; goto _LL685; _LL685: _temp684=( struct Cyc_Absyn_Conref*) _temp682.bounds;
goto _LL679;} else{ goto _LL680;} _LL680: goto _LL681; _LL679:{ void* _temp694=
Cyc_Tcutil_compress( _temp692); struct Cyc_Absyn_Structdecl** _temp702; struct
Cyc_List_List* _temp704; struct _tuple1* _temp706; struct Cyc_Absyn_Uniondecl**
_temp708; struct Cyc_List_List* _temp710; struct _tuple1* _temp712; _LL696: if((
unsigned int) _temp694 > 4u?(( struct _tunion_struct*) _temp694)->tag == Cyc_Absyn_StructType:
0){ _LL707: _temp706=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp694)->f1; goto _LL705; _LL705: _temp704=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp694)->f2; goto _LL703; _LL703: _temp702=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp694)->f3;
goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int) _temp694 > 4u?((
struct _tunion_struct*) _temp694)->tag == Cyc_Absyn_UnionType: 0){ _LL713:
_temp712=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp694)->f1;
goto _LL711; _LL711: _temp710=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp694)->f2; goto _LL709; _LL709: _temp708=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp694)->f3; goto _LL699;} else{ goto
_LL700;} _LL700: goto _LL701; _LL697: return Cyc_NewControlFlow_descend_path(
env, e, _temp320); _LL699: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp320); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL701: goto _LL695; _LL695:;} goto _LL681; _LL681:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp714=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp714[ 0]=({ struct
Cyc_Core_Impossible_struct _temp715; _temp715.tag= Cyc_Core_Impossible; _temp715.f1=(
struct _tagged_string)({ char* _temp716=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 40; _temp717;}); _temp715;}); _temp714;}));
_LL677:;} _LL175: { void* _temp719= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp718= _temp324->topt; if( _temp718 == 0){ _throw( Null_Exception);} _temp718->v;}));
struct Cyc_Absyn_Structdecl** _temp727; struct Cyc_List_List* _temp729; struct
_tuple1* _temp731; struct Cyc_Absyn_Uniondecl** _temp733; struct Cyc_List_List*
_temp735; struct _tuple1* _temp737; _LL721: if(( unsigned int) _temp719 > 4u?((
struct _tunion_struct*) _temp719)->tag == Cyc_Absyn_StructType: 0){ _LL732:
_temp731=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp719)->f1;
goto _LL730; _LL730: _temp729=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp719)->f2; goto _LL728; _LL728: _temp727=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp719)->f3; goto _LL722;} else{ goto
_LL723;} _LL723: if(( unsigned int) _temp719 > 4u?(( struct _tunion_struct*)
_temp719)->tag == Cyc_Absyn_UnionType: 0){ _LL738: _temp737=( struct _tuple1*)((
struct Cyc_Absyn_UnionType_struct*) _temp719)->f1; goto _LL736; _LL736: _temp735=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp719)->f2; goto
_LL734; _LL734: _temp733=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp719)->f3; goto _LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL722:
return Cyc_NewControlFlow_descend_path( env, e, _temp324); _LL724: { struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp324); syn1.lvalues= Cyc_NewControlFlow_use_it_list();
return syn1;} _LL726:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp739=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp739[ 0]=({ struct Cyc_Core_Impossible_struct _temp740; _temp740.tag= Cyc_Core_Impossible;
_temp740.f1=( struct _tagged_string)({ char* _temp741=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp742; _temp742.curr= _temp741; _temp742.base= _temp741;
_temp742.last_plus_one= _temp741 + 41; _temp742;}); _temp740;}); _temp739;}));
_LL720:;} _LL177: { struct Cyc_List_List* _temp743= env.access_path; struct Cyc_List_List
_temp751; struct Cyc_List_List* _temp752; struct Cyc_Absyn_Exp* _temp754; struct
Cyc_Absyn_Exp _temp756; struct Cyc_Position_Segment* _temp757; void* _temp759;
struct Cyc_Absyn_Exp* _temp761; struct Cyc_Absyn_Exp* _temp763; struct Cyc_Core_Opt*
_temp765; _LL745: if( _temp743 == 0){ goto _LL746;} else{ goto _LL747;} _LL747:
if( _temp743 == 0){ goto _LL749;} else{ _temp751=* _temp743; _LL755: _temp754=(
struct Cyc_Absyn_Exp*) _temp751.hd; _temp756=* _temp754; _LL766: _temp765=(
struct Cyc_Core_Opt*) _temp756.topt; goto _LL760; _LL760: _temp759=( void*)
_temp756.r; if((( struct _tunion_struct*) _temp759)->tag == Cyc_Absyn_Subscript_e){
_LL764: _temp763=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp759)->f1; goto _LL762; _LL762: _temp761=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp759)->f2; goto _LL758;} else{ goto _LL749;}
_LL758: _temp757=( struct Cyc_Position_Segment*) _temp756.loc; goto _LL753;
_LL753: _temp752=( struct Cyc_List_List*) _temp751.tl; goto _LL748;} _LL749:
goto _LL750; _LL746: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp326 != 0; _temp326=({ struct
Cyc_List_List* _temp767= _temp326; if( _temp767 == 0){ _throw( Null_Exception);}
_temp767->tl;}), ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp768= ls; if(
_temp768 == 0){ _throw( Null_Exception);} _temp768->tl;})){ if( Cyc_CfAbsexp_isUnknownOp((
void*)({ struct Cyc_List_List* _temp769= ls; if( _temp769 == 0){ _throw(
Null_Exception);} _temp769->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp770=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp770->hd=(
void*) Cyc_CfAbsexp_mkUnknownOp(); _temp770->tl= ith_lvals; _temp770;});} else{
ith_lvals=({ struct Cyc_List_List* _temp771=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp771->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp772= ls; if( _temp772 == 0){ _throw(
Null_Exception);} _temp772->hd;}),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp773=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp773[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp774; _temp774.tag= Cyc_CfFlowInfo_TupleF;
_temp774.f1= j; _temp774;}); _temp773;})); _temp771->tl= ith_lvals; _temp771;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp781=
_temp326; if( _temp781 == 0){ _throw( Null_Exception);} _temp781->hd;})); void*
_temp777; void* _temp779; struct Cyc_NewControlFlow_AbsSynOne _temp775= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL780: _temp779=( void*) _temp775.inner_exp; goto _LL778; _LL778:
_temp777=( void*) _temp775.assigns; goto _LL776; _LL776: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp779); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp777);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL748: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp761); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp752;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp326 != 0;
_temp326=({ struct Cyc_List_List* _temp782= _temp326; if( _temp782 == 0){ _throw(
Null_Exception);} _temp782->tl;}), ++ j){ if( i == j){ void* _temp786; void*
_temp788; struct Cyc_NewControlFlow_AbsSynOne _temp784= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp783= _temp326; if( _temp783 == 0){ _throw( Null_Exception);} _temp783->hd;})));
_LL789: _temp788=( void*) _temp784.inner_exp; goto _LL787; _LL787: _temp786=(
void*) _temp784.assigns; goto _LL785; _LL785: assignexp= _temp786; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp788);} else{ void* _temp793; void*
_temp795; struct Cyc_NewControlFlow_AbsSynOne _temp791= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp790= _temp326; if( _temp790 == 0){ _throw( Null_Exception);} _temp790->hd;})));
_LL796: _temp795=( void*) _temp791.inner_exp; goto _LL794; _LL794: _temp793=(
void*) _temp791.assigns; goto _LL792; _LL792: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp795);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp797=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp797->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp797->tl= 0; _temp797;}), assignexp);}} _LL750:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp798=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp798[ 0]=({ struct
Cyc_Core_Impossible_struct _temp799; _temp799.tag= Cyc_Core_Impossible; _temp799.f1=(
struct _tagged_string)({ char* _temp800=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 46; _temp801;}); _temp799;}); _temp798;}));
_LL744:;} _LL179: { struct Cyc_List_List* _temp802= env.access_path; struct Cyc_List_List
_temp810; struct Cyc_List_List* _temp811; struct Cyc_Absyn_Exp* _temp813; struct
Cyc_Absyn_Exp _temp815; struct Cyc_Position_Segment* _temp816; void* _temp818;
struct _tagged_string* _temp820; struct Cyc_Absyn_Exp* _temp822; struct Cyc_Core_Opt*
_temp824; _LL804: if( _temp802 == 0){ goto _LL805;} else{ goto _LL806;} _LL806:
if( _temp802 == 0){ goto _LL808;} else{ _temp810=* _temp802; _LL814: _temp813=(
struct Cyc_Absyn_Exp*) _temp810.hd; _temp815=* _temp813; _LL825: _temp824=(
struct Cyc_Core_Opt*) _temp815.topt; goto _LL819; _LL819: _temp818=( void*)
_temp815.r; if((( struct _tunion_struct*) _temp818)->tag == Cyc_Absyn_StructMember_e){
_LL823: _temp822=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp818)->f1; goto _LL821; _LL821: _temp820=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp818)->f2; goto _LL817;} else{ goto _LL808;}
_LL817: _temp816=( struct Cyc_Position_Segment*) _temp815.loc; goto _LL812;
_LL812: _temp811=( struct Cyc_List_List*) _temp810.tl; goto _LL807;} _LL808:
goto _LL809; _LL805: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp330 != 0; _temp330=({ struct Cyc_List_List*
_temp826= _temp330; if( _temp826 == 0){ _throw( Null_Exception);} _temp826->tl;})){
struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls != 0; ls=({ struct Cyc_List_List* _temp827= ls; if( _temp827 == 0){ _throw(
Null_Exception);} _temp827->tl;})){ if( Cyc_CfAbsexp_isUnknownOp(( void*)({
struct Cyc_List_List* _temp828= ls; if( _temp828 == 0){ _throw( Null_Exception);}
_temp828->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp829=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp829->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp829->tl= ith_lvals; _temp829;});} else{ struct Cyc_List_List* ds=(*((
struct _tuple5*)({ struct Cyc_List_List* _temp850= _temp330; if( _temp850 == 0){
_throw( Null_Exception);} _temp850->hd;}))).f1; for( 0; ds != 0; ds=({ struct
Cyc_List_List* _temp830= ds; if( _temp830 == 0){ _throw( Null_Exception);}
_temp830->tl;})){ void* _temp832=( void*)({ struct Cyc_List_List* _temp831= ds;
if( _temp831 == 0){ _throw( Null_Exception);} _temp831->hd;}); struct Cyc_Absyn_Exp*
_temp838; struct _tagged_string* _temp840; _LL834: if((( struct _tunion_struct*)
_temp832)->tag == Cyc_Absyn_ArrayElement){ _LL839: _temp838=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp832)->f1; goto _LL835;} else{ goto
_LL836;} _LL836: if((( struct _tunion_struct*) _temp832)->tag == Cyc_Absyn_FieldName){
_LL841: _temp840=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp832)->f1; goto _LL837;} else{ goto _LL833;} _LL835:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp842=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp842[ 0]=({ struct
Cyc_Core_Impossible_struct _temp843; _temp843.tag= Cyc_Core_Impossible; _temp843.f1=(
struct _tagged_string)({ char* _temp844=( char*)"bad struct designator"; struct
_tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 22; _temp845;}); _temp843;}); _temp842;}));
_LL837: ith_lvals=({ struct Cyc_List_List* _temp846=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp846->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp847= ls; if( _temp847 == 0){ _throw(
Null_Exception);} _temp847->hd;}),( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp848=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp848[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp849; _temp849.tag= Cyc_CfFlowInfo_StructF;
_temp849.f1= _temp840; _temp849;}); _temp848;})); _temp846->tl= ith_lvals;
_temp846;}); goto _LL833; _LL833:;}}}}{ void** ith_topt=( void*)({ struct Cyc_Core_Opt*
_temp859=((*(( struct _tuple5*)({ struct Cyc_List_List* _temp858= _temp330; if(
_temp858 == 0){ _throw( Null_Exception);} _temp858->hd;}))).f2)->topt; if(
_temp859 == 0){ _throw( Null_Exception);}& _temp859->v;}); struct Cyc_NewControlFlow_AbsSyn
ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
ith_lvals),(*(( struct _tuple5*)({ struct Cyc_List_List* _temp857= _temp330; if(
_temp857 == 0){ _throw( Null_Exception);} _temp857->hd;}))).f2); void* _temp853;
void* _temp855; struct Cyc_NewControlFlow_AbsSynOne _temp851= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL856: _temp855=( void*) _temp851.inner_exp; goto _LL854; _LL854:
_temp853=( void*) _temp851.assigns; goto _LL852; _LL852: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp855); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp853);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL807: { struct Cyc_NewControlFlow_AbsEnv other_env=
Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp811;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp330 != 0; _temp330=({ struct Cyc_List_List* _temp860= _temp330; if(
_temp860 == 0){ _throw( Null_Exception);} _temp860->tl;})){ int used= 0;{ struct
Cyc_List_List* ds=(*(( struct _tuple5*)({ struct Cyc_List_List* _temp877=
_temp330; if( _temp877 == 0){ _throw( Null_Exception);} _temp877->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp861= ds; if( _temp861 == 0){
_throw( Null_Exception);} _temp861->tl;})){ void* _temp863=( void*)({ struct Cyc_List_List*
_temp862= ds; if( _temp862 == 0){ _throw( Null_Exception);} _temp862->hd;});
struct Cyc_Absyn_Exp* _temp869; struct _tagged_string* _temp871; _LL865: if(((
struct _tunion_struct*) _temp863)->tag == Cyc_Absyn_ArrayElement){ _LL870:
_temp869=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp863)->f1; goto _LL866;} else{ goto _LL867;} _LL867: if((( struct
_tunion_struct*) _temp863)->tag == Cyc_Absyn_FieldName){ _LL872: _temp871=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp863)->f1;
goto _LL868;} else{ goto _LL864;} _LL866:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp873=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp873[ 0]=({ struct Cyc_Core_Impossible_struct _temp874; _temp874.tag= Cyc_Core_Impossible;
_temp874.f1=( struct _tagged_string)({ char* _temp875=( char*)"bad struct designator";
struct _tagged_string _temp876; _temp876.curr= _temp875; _temp876.base= _temp875;
_temp876.last_plus_one= _temp875 + 22; _temp876;}); _temp874;}); _temp873;}));
_LL868: if( Cyc_String_zstrptrcmp( _temp820, _temp871) == 0){ used= 1;} goto
_LL864; _LL864:;}} if( used){ void* _temp881; void* _temp883; struct Cyc_NewControlFlow_AbsSynOne
_temp879= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp878= _temp330; if( _temp878 == 0){
_throw( Null_Exception);} _temp878->hd;}))).f2)); _LL884: _temp883=( void*)
_temp879.inner_exp; goto _LL882; _LL882: _temp881=( void*) _temp879.assigns;
goto _LL880; _LL880: assignexp= _temp881; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, ansexp, _temp883);} else{ void* _temp888; void*
_temp890; struct Cyc_NewControlFlow_AbsSynOne _temp886= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp885= _temp330; if( _temp885 == 0){ _throw( Null_Exception);} _temp885->hd;}))).f2));
_LL891: _temp890=( void*) _temp886.inner_exp; goto _LL889; _LL889: _temp888=(
void*) _temp886.assigns; goto _LL887; _LL887: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp890);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);}} _LL809:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp892=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp892[ 0]=({ struct Cyc_Core_Impossible_struct _temp893; _temp893.tag= Cyc_Core_Impossible;
_temp893.f1=( struct _tagged_string)({ char* _temp894=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp895; _temp895.curr= _temp894; _temp895.base= _temp894;
_temp895.last_plus_one= _temp894 + 47; _temp895;}); _temp893;}); _temp892;}));
_LL803:;} _LL181: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp336), 0); _LL183:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp896=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp896->hd=( void*) _temp340; _temp896->tl= 0; _temp896;}), 0); _LL185: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp342); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL187: { struct Cyc_List_List*
_temp899; struct Cyc_NewControlFlow_AbsSynOne _temp901; struct Cyc_NewControlFlow_AbsSynOne
_temp903; struct Cyc_NewControlFlow_AbsSyn _temp897= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp348); _LL904: _temp903=( struct Cyc_NewControlFlow_AbsSynOne)
_temp897.when_true; goto _LL902; _LL902: _temp901=( struct Cyc_NewControlFlow_AbsSynOne)
_temp897.when_false; goto _LL900; _LL900: _temp899=( struct Cyc_List_List*)
_temp897.lvalues; goto _LL898; _LL898: { struct Cyc_List_List* _temp907; struct
Cyc_NewControlFlow_AbsSynOne _temp909; struct Cyc_NewControlFlow_AbsSynOne
_temp911; struct Cyc_NewControlFlow_AbsSyn _temp905= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp346); _LL912: _temp911=( struct Cyc_NewControlFlow_AbsSynOne)
_temp905.when_true; goto _LL910; _LL910: _temp909=( struct Cyc_NewControlFlow_AbsSynOne)
_temp905.when_false; goto _LL908; _LL908: _temp907=( struct Cyc_List_List*)
_temp905.lvalues; goto _LL906; _LL906: { struct Cyc_List_List* _temp915; struct
Cyc_NewControlFlow_AbsSynOne _temp917; struct Cyc_NewControlFlow_AbsSynOne
_temp919; struct Cyc_NewControlFlow_AbsSyn _temp913= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp344); _LL920: _temp919=( struct Cyc_NewControlFlow_AbsSynOne)
_temp913.when_true; goto _LL918; _LL918: _temp917=( struct Cyc_NewControlFlow_AbsSynOne)
_temp913.when_false; goto _LL916; _LL916: _temp915=( struct Cyc_List_List*)
_temp913.lvalues; goto _LL914; _LL914: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp903.inner_exp,(
void*) _temp903.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp911.inner_exp,( void*) _temp911.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp901.inner_exp,( void*) _temp901.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp919.inner_exp,( void*) _temp919.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp903.inner_exp,( void*) _temp903.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp909.inner_exp,( void*) _temp909.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp901.inner_exp,( void*) _temp901.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp917.inner_exp,(
void*) _temp917.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp921;
_temp921.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp923; _temp923.inner_exp=(
void*) true_exp; _temp923.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp923;}); _temp921.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp922; _temp922.inner_exp=( void*) false_exp; _temp922.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp922;}); _temp921.lvalues= 0; _temp921;});}}}}
_LL189: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp350);{
struct Cyc_Absyn_Stmt* last_s= _temp350; while( 1) { void* _temp924=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp934; struct Cyc_Absyn_Stmt* _temp936;
struct Cyc_Absyn_Stmt* _temp938; struct Cyc_Absyn_Decl* _temp940; struct Cyc_Absyn_Exp*
_temp942; _LL926: if(( unsigned int) _temp924 > 1u?(( struct _tunion_struct*)
_temp924)->tag == Cyc_Absyn_Seq_s: 0){ _LL937: _temp936=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp924)->f1; goto _LL935; _LL935: _temp934=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp924)->f2; goto
_LL927;} else{ goto _LL928;} _LL928: if(( unsigned int) _temp924 > 1u?(( struct
_tunion_struct*) _temp924)->tag == Cyc_Absyn_Decl_s: 0){ _LL941: _temp940=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp924)->f1; goto
_LL939; _LL939: _temp938=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp924)->f2; goto _LL929;} else{ goto _LL930;} _LL930: if(( unsigned int)
_temp924 > 1u?(( struct _tunion_struct*) _temp924)->tag == Cyc_Absyn_Exp_s: 0){
_LL943: _temp942=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp924)->f1; goto _LL931;} else{ goto _LL932;} _LL932: goto _LL933; _LL927:
last_s= _temp934; continue; _LL929: last_s= _temp938; continue; _LL931: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp942); void* _temp946; void* _temp948; struct Cyc_NewControlFlow_AbsSynOne
_temp944= Cyc_NewControlFlow_meet_absexp( syn); _LL949: _temp948=( void*)
_temp944.inner_exp; goto _LL947; _LL947: _temp946=( void*) _temp944.assigns;
goto _LL945; _LL945:( void*)({ struct Cyc_List_List* _temp950=( Cyc_NewControlFlow_get_stmt_annot(
last_s))->absexps; if( _temp950 == 0){ _throw( Null_Exception);} _temp950->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp948, _temp946);});
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp350), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL933:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp951=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp951[ 0]=({ struct Cyc_Core_Impossible_struct _temp952; _temp952.tag= Cyc_Core_Impossible;
_temp952.f1=( struct _tagged_string)({ char* _temp953=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp954; _temp954.curr= _temp953; _temp954.base= _temp953;
_temp954.last_plus_one= _temp953 + 33; _temp954;}); _temp952;}); _temp951;}));
_LL925:;}}} _LL191: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp354); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp352); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp955; _temp955.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp957; _temp957.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp957.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp957;});
_temp955.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp956; _temp956.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp956.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp956;}); _temp955.lvalues= 0; _temp955;});}} _LL193: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp958=( char*)"new_control_flow.cyc:750 env.access_path==null";
struct _tagged_string _temp959; _temp959.curr= _temp958; _temp959.base= _temp958;
_temp959.last_plus_one= _temp958 + 47; _temp959;})); Cyc_NewControlFlow_add_var_root(
env, _temp360);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp960=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp960->hd=( void*) _temp360; _temp960->tl=( env.shared)->comprehension_vars;
_temp960;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp963; void* _temp965; struct Cyc_NewControlFlow_AbsSynOne
_temp961= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp358)); _LL966: _temp965=( void*) _temp961.inner_exp; goto _LL964;
_LL964: _temp963=( void*) _temp961.assigns; goto _LL962; _LL962: { void*
_temp969; void* _temp971; struct Cyc_NewControlFlow_AbsSynOne _temp967= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp356)); _LL972: _temp971=( void*)
_temp967.inner_exp; goto _LL970; _LL970: _temp969=( void*) _temp967.assigns;
goto _LL968; _LL968: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp965, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp963, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp360), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp971, _temp969), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL195: goto _LL197; _LL197: goto _LL199;
_LL199: goto _LL201; _LL201: goto _LL203; _LL203: goto _LL205; _LL205: goto
_LL207; _LL207:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp973=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp973[ 0]=({ struct Cyc_Core_Impossible_struct _temp974; _temp974.tag= Cyc_Core_Impossible;
_temp974.f1=( struct _tagged_string)({ char* _temp975=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp976; _temp976.curr= _temp975; _temp976.base= _temp975;
_temp976.last_plus_one= _temp975 + 34; _temp976;}); _temp974;}); _temp973;}));
_LL123:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp979; void* _temp981; struct Cyc_NewControlFlow_AbsSynOne
_temp977= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL982: _temp981=( void*) _temp977.inner_exp; goto _LL980; _LL980: _temp979=(
void*) _temp977.assigns; goto _LL978; _LL978: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp981, _temp979);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp983; _temp983.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp983.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp983;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp984= scs; if(
_temp984 == 0){ _throw( Null_Exception);} _temp984->tl;})){ struct Cyc_Absyn_Switch_clause
_temp988; struct Cyc_Position_Segment* _temp989; struct Cyc_Absyn_Stmt* _temp991;
struct Cyc_Absyn_Exp* _temp993; struct Cyc_Core_Opt* _temp995; struct Cyc_Absyn_Pat*
_temp997; struct Cyc_Absyn_Switch_clause* _temp986=( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp985= scs; if( _temp985 == 0){ _throw( Null_Exception);}
_temp985->hd;}); _temp988=* _temp986; _LL998: _temp997=( struct Cyc_Absyn_Pat*)
_temp988.pattern; goto _LL996; _LL996: _temp995=( struct Cyc_Core_Opt*) _temp988.pat_vars;
goto _LL994; _LL994: _temp993=( struct Cyc_Absyn_Exp*) _temp988.where_clause;
goto _LL992; _LL992: _temp991=( struct Cyc_Absyn_Stmt*) _temp988.body; goto
_LL990; _LL990: _temp989=( struct Cyc_Position_Segment*) _temp988.loc; goto
_LL987; _LL987: if( _temp995 == 0){( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp999=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp999[ 0]=({ struct Cyc_Core_Impossible_struct _temp1000; _temp1000.tag= Cyc_Core_Impossible;
_temp1000.f1=( struct _tagged_string)({ char* _temp1001=( char*)"switch clause vds not set";
struct _tagged_string _temp1002; _temp1002.curr= _temp1001; _temp1002.base=
_temp1001; _temp1002.last_plus_one= _temp1001 + 26; _temp1002;}); _temp1000;});
_temp999;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1005= _temp995; if( _temp1005 == 0){ _throw( Null_Exception);} _temp1005->v;});
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1003= vds; if( _temp1003 ==
0){ _throw( Null_Exception);} _temp1003->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1004= vds; if(
_temp1004 == 0){ _throw( Null_Exception);} _temp1004->hd;}));}} if( _temp993 !=
0){ void* _temp1009; void* _temp1011; struct _tuple0 _temp1007= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1006= _temp993; if(
_temp1006 == 0){ _throw( Null_Exception);} _temp1006;})); _LL1012: _temp1011=
_temp1007.f1; goto _LL1010; _LL1010: _temp1009= _temp1007.f2; goto _LL1008;
_LL1008: ans=({ struct Cyc_List_List* _temp1013=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1013->hd=( void*) _temp1011;
_temp1013->tl=({ struct Cyc_List_List* _temp1014=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1014->hd=( void*) _temp1009;
_temp1014->tl= ans; _temp1014;}); _temp1013;});} Cyc_NewControlFlow_cf_prepass(
env, _temp991);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp1015=( char*)"new_control_flow.cyc:802 env.lhsides == null"; struct
_tagged_string _temp1016; _temp1016.curr= _temp1015; _temp1016.base= _temp1015;
_temp1016.last_plus_one= _temp1015 + 45; _temp1016;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1017=( char*)"new_control_flow.cyc:803 env.access_path == null";
struct _tagged_string _temp1018; _temp1018.curr= _temp1017; _temp1018.base=
_temp1017; _temp1018.last_plus_one= _temp1017 + 49; _temp1018;}));{ void*
_temp1019=( void*) s->r; struct Cyc_Absyn_Exp* _temp1067; struct Cyc_Absyn_Stmt*
_temp1069; struct Cyc_Absyn_Stmt* _temp1071; struct Cyc_Absyn_Exp* _temp1073;
struct Cyc_Absyn_Exp* _temp1075; struct Cyc_Absyn_Stmt* _temp1077; struct Cyc_Absyn_Stmt*
_temp1079; struct Cyc_Absyn_Exp* _temp1081; struct _tuple3 _temp1083; struct Cyc_Absyn_Stmt*
_temp1085; struct Cyc_Absyn_Exp* _temp1087; struct Cyc_Absyn_Stmt* _temp1089;
struct Cyc_Absyn_Stmt* _temp1091; struct _tuple3 _temp1093; struct Cyc_Absyn_Stmt*
_temp1095; struct Cyc_Absyn_Exp* _temp1097; struct Cyc_Absyn_Stmt* _temp1099;
struct Cyc_Absyn_Stmt* _temp1101; struct Cyc_Absyn_Stmt* _temp1103; struct
_tagged_string* _temp1105; struct Cyc_Absyn_Stmt* _temp1107; struct
_tagged_string* _temp1109; struct Cyc_Absyn_Stmt* _temp1111; struct _tuple3
_temp1113; struct Cyc_Absyn_Stmt* _temp1115; struct Cyc_Absyn_Exp* _temp1117;
struct _tuple3 _temp1119; struct Cyc_Absyn_Stmt* _temp1121; struct Cyc_Absyn_Exp*
_temp1123; struct Cyc_Absyn_Exp* _temp1125; struct Cyc_List_List* _temp1127;
struct Cyc_Absyn_Exp* _temp1129; struct Cyc_Absyn_Switch_clause** _temp1131;
struct Cyc_List_List* _temp1133; struct Cyc_Absyn_Stmt* _temp1135; struct Cyc_Absyn_Decl*
_temp1137; struct Cyc_Absyn_Decl _temp1139; struct Cyc_Position_Segment*
_temp1140; void* _temp1142; struct Cyc_Absyn_Vardecl* _temp1144; struct Cyc_Absyn_Stmt*
_temp1146; struct Cyc_Absyn_Decl* _temp1148; struct Cyc_Absyn_Decl _temp1150;
struct Cyc_Position_Segment* _temp1151; void* _temp1153; int _temp1155; struct
Cyc_Absyn_Exp* _temp1157; struct Cyc_Core_Opt* _temp1159; struct Cyc_Core_Opt*
_temp1161; struct Cyc_Core_Opt _temp1163; struct Cyc_List_List* _temp1164;
struct Cyc_Absyn_Pat* _temp1166; struct Cyc_Absyn_Stmt* _temp1168; struct Cyc_Absyn_Decl*
_temp1170; struct Cyc_Absyn_Stmt* _temp1172; struct Cyc_Absyn_Stmt* _temp1174;
struct Cyc_Absyn_Stmt* _temp1176; struct _tagged_string* _temp1178; struct Cyc_List_List*
_temp1180; struct Cyc_Absyn_Stmt* _temp1182; struct Cyc_Absyn_Stmt* _temp1184;
struct Cyc_Absyn_Vardecl* _temp1186; struct Cyc_Absyn_Tvar* _temp1188; _LL1021:
if( _temp1019 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1022;} else{ goto _LL1023;}
_LL1023: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag
== Cyc_Absyn_Exp_s: 0){ _LL1068: _temp1067=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1019)->f1; goto _LL1024;} else{ goto _LL1025;} _LL1025: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Seq_s: 0){
_LL1072: _temp1071=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1019)->f1; goto _LL1070; _LL1070: _temp1069=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1019)->f2; goto _LL1026;} else{ goto
_LL1027;} _LL1027: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*)
_temp1019)->tag == Cyc_Absyn_Return_s: 0){ _LL1074: _temp1073=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1019)->f1; if( _temp1073 == 0){ goto
_LL1028;} else{ goto _LL1029;}} else{ goto _LL1029;} _LL1029: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Return_s:
0){ _LL1076: _temp1075=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1019)->f1; goto _LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_IfThenElse_s:
0){ _LL1082: _temp1081=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1019)->f1; goto _LL1080; _LL1080: _temp1079=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1019)->f2; goto _LL1078; _LL1078:
_temp1077=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1019)->f3; goto _LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Do_s: 0){
_LL1090: _temp1089=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1019)->f1; goto _LL1084; _LL1084: _temp1083=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1019)->f2; _LL1088: _temp1087= _temp1083.f1; goto _LL1086; _LL1086:
_temp1085= _temp1083.f2; goto _LL1034;} else{ goto _LL1035;} _LL1035: if((
unsigned int) _temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_While_s:
0){ _LL1094: _temp1093=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1019)->f1; _LL1098: _temp1097= _temp1093.f1; goto _LL1096; _LL1096:
_temp1095= _temp1093.f2; goto _LL1092; _LL1092: _temp1091=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1019)->f2; goto _LL1036;} else{ goto
_LL1037;} _LL1037: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*)
_temp1019)->tag == Cyc_Absyn_Break_s: 0){ _LL1100: _temp1099=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1019)->f1; goto _LL1038;} else{ goto
_LL1039;} _LL1039: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*)
_temp1019)->tag == Cyc_Absyn_Continue_s: 0){ _LL1102: _temp1101=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1019)->f1; goto _LL1040;} else{ goto
_LL1041;} _LL1041: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*)
_temp1019)->tag == Cyc_Absyn_Goto_s: 0){ _LL1106: _temp1105=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1019)->f1; goto _LL1104;
_LL1104: _temp1103=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1019)->f2; if( _temp1103 == 0){ goto _LL1042;} else{ goto _LL1043;}} else{
goto _LL1043;} _LL1043: if(( unsigned int) _temp1019 > 1u?(( struct
_tunion_struct*) _temp1019)->tag == Cyc_Absyn_Goto_s: 0){ _LL1110: _temp1109=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1019)->f1; goto
_LL1108; _LL1108: _temp1107=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1019)->f2; goto _LL1044;} else{ goto _LL1045;} _LL1045: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_For_s: 0){
_LL1126: _temp1125=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1019)->f1; goto _LL1120; _LL1120: _temp1119=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1019)->f2; _LL1124: _temp1123= _temp1119.f1; goto _LL1122; _LL1122:
_temp1121= _temp1119.f2; goto _LL1114; _LL1114: _temp1113=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1019)->f3; _LL1118: _temp1117= _temp1113.f1;
goto _LL1116; _LL1116: _temp1115= _temp1113.f2; goto _LL1112; _LL1112: _temp1111=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1019)->f4; goto
_LL1046;} else{ goto _LL1047;} _LL1047: if(( unsigned int) _temp1019 > 1u?((
struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Switch_s: 0){ _LL1130:
_temp1129=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1019)->f1; goto _LL1128; _LL1128: _temp1127=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1019)->f2; goto _LL1048;} else{ goto
_LL1049;} _LL1049: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*)
_temp1019)->tag == Cyc_Absyn_Fallthru_s: 0){ _LL1134: _temp1133=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1019)->f1; goto _LL1132; _LL1132:
_temp1131=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1019)->f2; goto _LL1050;} else{ goto _LL1051;} _LL1051: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Decl_s: 0){
_LL1138: _temp1137=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1019)->f1; _temp1139=* _temp1137; _LL1143: _temp1142=( void*) _temp1139.r;
if((( struct _tunion_struct*) _temp1142)->tag == Cyc_Absyn_Var_d){ _LL1145:
_temp1144=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1142)->f1; goto _LL1141;} else{ goto _LL1053;} _LL1141: _temp1140=( struct
Cyc_Position_Segment*) _temp1139.loc; goto _LL1136; _LL1136: _temp1135=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1019)->f2; goto _LL1052;}
else{ goto _LL1053;} _LL1053: if(( unsigned int) _temp1019 > 1u?(( struct
_tunion_struct*) _temp1019)->tag == Cyc_Absyn_Decl_s: 0){ _LL1149: _temp1148=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1019)->f1;
_temp1150=* _temp1148; _LL1154: _temp1153=( void*) _temp1150.r; if((( struct
_tunion_struct*) _temp1153)->tag == Cyc_Absyn_Let_d){ _LL1167: _temp1166=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1153)->f1; goto
_LL1162; _LL1162: _temp1161=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1153)->f2; if( _temp1161 == 0){ goto _LL1055;} else{ _temp1163=* _temp1161;
_LL1165: _temp1164=( struct Cyc_List_List*) _temp1163.v; goto _LL1160;} _LL1160:
_temp1159=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1153)->f3;
goto _LL1158; _LL1158: _temp1157=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1153)->f4; goto _LL1156; _LL1156: _temp1155=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1153)->f5; goto _LL1152;} else{ goto _LL1055;} _LL1152: _temp1151=( struct
Cyc_Position_Segment*) _temp1150.loc; goto _LL1147; _LL1147: _temp1146=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1019)->f2; goto _LL1054;}
else{ goto _LL1055;} _LL1055: if(( unsigned int) _temp1019 > 1u?(( struct
_tunion_struct*) _temp1019)->tag == Cyc_Absyn_Decl_s: 0){ _LL1171: _temp1170=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1019)->f1; goto
_LL1169; _LL1169: _temp1168=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1019)->f2; goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Cut_s: 0){
_LL1173: _temp1172=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp1019)->f1; goto _LL1058;} else{ goto _LL1059;} _LL1059: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Splice_s:
0){ _LL1175: _temp1174=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp1019)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Label_s:
0){ _LL1179: _temp1178=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1019)->f1; goto _LL1177; _LL1177: _temp1176=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1019)->f2; goto _LL1062;} else{ goto
_LL1063;} _LL1063: if(( unsigned int) _temp1019 > 1u?(( struct _tunion_struct*)
_temp1019)->tag == Cyc_Absyn_TryCatch_s: 0){ _LL1183: _temp1182=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1019)->f1; goto _LL1181; _LL1181:
_temp1180=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1019)->f2; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(( unsigned int)
_temp1019 > 1u?(( struct _tunion_struct*) _temp1019)->tag == Cyc_Absyn_Region_s:
0){ _LL1189: _temp1188=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1019)->f1; goto _LL1187; _LL1187: _temp1186=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1019)->f2; goto _LL1185; _LL1185:
_temp1184=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1019)->f3; goto _LL1066;} else{ goto _LL1020;} _LL1022: return; _LL1024: {
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1067); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1190=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1190->hd=( void*) ae; _temp1190->tl= 0; _temp1190;}));
return;} _LL1026: Cyc_NewControlFlow_cf_prepass( env, _temp1071); Cyc_NewControlFlow_cf_prepass(
env, _temp1069); return; _LL1028: return; _LL1030: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1192= _temp1075; if( _temp1192 == 0){ _throw( Null_Exception);} _temp1192;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1191=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1191->hd=( void*)
ae; _temp1191->tl= 0; _temp1191;})); return;} _LL1032: { void* _temp1195; void*
_temp1197; struct _tuple0 _temp1193= Cyc_NewControlFlow_abstract_guard( env,
_temp1081); _LL1198: _temp1197= _temp1193.f1; goto _LL1196; _LL1196: _temp1195=
_temp1193.f2; goto _LL1194; _LL1194: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1199=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1199->hd=( void*) _temp1197; _temp1199->tl=({
struct Cyc_List_List* _temp1200=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1200->hd=( void*) _temp1195; _temp1200->tl= 0;
_temp1200;}); _temp1199;})); Cyc_NewControlFlow_cf_prepass( env, _temp1079); Cyc_NewControlFlow_cf_prepass(
env, _temp1077); return;} _LL1034: _temp1097= _temp1087; _temp1095= _temp1085;
_temp1091= _temp1089; goto _LL1036; _LL1036: { void* _temp1203; void* _temp1205;
struct _tuple0 _temp1201= Cyc_NewControlFlow_abstract_guard( env, _temp1097);
_LL1206: _temp1205= _temp1201.f1; goto _LL1204; _LL1204: _temp1203= _temp1201.f2;
goto _LL1202; _LL1202: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1207=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1207->hd=( void*) _temp1205; _temp1207->tl=({ struct Cyc_List_List*
_temp1208=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1208->hd=( void*) _temp1203; _temp1208->tl= 0; _temp1208;}); _temp1207;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1091); return;} _LL1038: return;
_LL1040: return; _LL1042:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1209=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1209[ 0]=({ struct Cyc_Core_Impossible_struct _temp1210; _temp1210.tag= Cyc_Core_Impossible;
_temp1210.f1=( struct _tagged_string)({ char* _temp1211=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1212; _temp1212.curr= _temp1211; _temp1212.base=
_temp1211; _temp1212.last_plus_one= _temp1211 + 33; _temp1212;}); _temp1210;});
_temp1209;})); _LL1044: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1215= _temp1107; if(
_temp1215 == 0){ _throw( Null_Exception);} _temp1215;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1213=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1214; _temp1214.curr= _temp1213; _temp1214.base=
_temp1213; _temp1214.last_plus_one= _temp1213 + 45; _temp1214;})); break;}
my_encloser= next_encloser;} return;} _LL1046: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1125); void* _temp1218; void* _temp1220; struct _tuple0 _temp1216= Cyc_NewControlFlow_abstract_guard(
env, _temp1123); _LL1221: _temp1220= _temp1216.f1; goto _LL1219; _LL1219:
_temp1218= _temp1216.f2; goto _LL1217; _LL1217: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1117); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1222=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1222->hd=( void*) ae1; _temp1222->tl=({ struct Cyc_List_List* _temp1223=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1223->hd=(
void*) _temp1220; _temp1223->tl=({ struct Cyc_List_List* _temp1224=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1224->hd=( void*) _temp1218;
_temp1224->tl=({ struct Cyc_List_List* _temp1225=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1225->hd=( void*) ae3; _temp1225->tl=
0; _temp1225;}); _temp1224;}); _temp1223;}); _temp1222;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1111); return;}} _LL1048: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1129); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1127); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1226=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1226->hd=( void*) ae; _temp1226->tl= ael; _temp1226;})); return;} _LL1050: {
void* _temp1229; void* _temp1231; struct Cyc_NewControlFlow_AbsSynOne _temp1227=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1133, 0)); _LL1232: _temp1231=( void*) _temp1227.inner_exp; goto _LL1230;
_LL1230: _temp1229=( void*) _temp1227.assigns; goto _LL1228; _LL1228: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1231, _temp1229);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1233=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1233->hd=( void*)
ae; _temp1233->tl= 0; _temp1233;})); return;}} _LL1052: Cyc_NewControlFlow_add_var_root(
env, _temp1144); if( _temp1144->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1236=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1236->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1144); _temp1236->tl= 0; _temp1236;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1235= _temp1144->initializer; if( _temp1235 == 0){
_throw( Null_Exception);} _temp1235;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1234=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1234->hd=( void*) ae; _temp1234->tl= 0; _temp1234;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1135); return; _LL1054: for( 0;
_temp1164 != 0; _temp1164=({ struct Cyc_List_List* _temp1237= _temp1164; if(
_temp1237 == 0){ _throw( Null_Exception);} _temp1237->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1238= _temp1164;
if( _temp1238 == 0){ _throw( Null_Exception);} _temp1238->hd;}));}{ void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1157); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1239=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1239->hd=( void*) ae; _temp1239->tl= 0; _temp1239;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1146); return;} _LL1056: goto _LL1058; _LL1058: goto _LL1060; _LL1060:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1240=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1240[ 0]=({ struct Cyc_Core_Impossible_struct _temp1241; _temp1241.tag= Cyc_Core_Impossible;
_temp1241.f1=( struct _tagged_string)({ char* _temp1242=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1243; _temp1243.curr= _temp1242; _temp1243.base=
_temp1242; _temp1243.last_plus_one= _temp1242 + 26; _temp1243;}); _temp1241;});
_temp1240;})); _LL1062: Cyc_NewControlFlow_cf_prepass( env, _temp1176); return;
_LL1064: Cyc_NewControlFlow_cf_prepass( env, _temp1182);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1180); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL1066: Cyc_NewControlFlow_add_var_root( env, _temp1186);{
void* ae= Cyc_NewControlFlow_abstract_exp_top( env, Cyc_Absyn_uint_exp( 0, 0));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1244=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1244->hd=( void*)
ae; _temp1244->tl= 0; _temp1244;})); Cyc_NewControlFlow_cf_prepass( env,
_temp1184); return;} _LL1020:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1245=( char*)"new_control_flow.cyc:941 annot->visited == iteration_num";
struct _tagged_string _temp1246; _temp1246.curr= _temp1245; _temp1246.base=
_temp1245; _temp1246.last_plus_one= _temp1245 + 57; _temp1246;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1247= flow; struct Cyc_Dict_Dict* _temp1253; _LL1249: if( _temp1247
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1250;} else{ goto _LL1251;} _LL1251:
if(( unsigned int) _temp1247 > 1u?(( struct _tunion_struct*) _temp1247)->tag ==
Cyc_CfFlowInfo_InitsFL: 0){ _LL1254: _temp1253=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_InitsFL_struct*) _temp1247)->f1; goto _LL1252;} else{ goto
_LL1248;} _LL1250: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1252: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1255= vds; if( _temp1255 == 0){ _throw(
Null_Exception);} _temp1255->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1256=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1256[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1257; _temp1257.tag= Cyc_CfFlowInfo_VarRoot;
_temp1257.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1258= vds;
if( _temp1258 == 0){ _throw( Null_Exception);} _temp1258->hd;}); _temp1257;});
_temp1256;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) root)); _temp1253=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1253,( void*) root, pinfo);}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1259=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1259[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1260; _temp1260.tag= Cyc_CfFlowInfo_InitsFL;
_temp1260.f1= _temp1253; _temp1260;}); _temp1259;}); _LL1248:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1261= flow; struct Cyc_Dict_Dict* _temp1267; _LL1263: if( _temp1261
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1264;} else{ goto _LL1265;} _LL1265:
if(( unsigned int) _temp1261 > 1u?(( struct _tunion_struct*) _temp1261)->tag ==
Cyc_CfFlowInfo_InitsFL: 0){ _LL1268: _temp1267=( struct Cyc_Dict_Dict*)(( struct
Cyc_CfFlowInfo_InitsFL_struct*) _temp1261)->f1; goto _LL1266;} else{ goto
_LL1262;} _LL1264: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1266: for( 0; vds
!= 0; vds=({ struct Cyc_List_List* _temp1269= vds; if( _temp1269 == 0){ _throw(
Null_Exception);} _temp1269->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct* root=({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1270=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1270[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1271; _temp1271.tag= Cyc_CfFlowInfo_VarRoot;
_temp1271.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1272= vds;
if( _temp1272 == 0){ _throw( Null_Exception);} _temp1272->hd;}); _temp1271;});
_temp1270;}); _temp1267=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1267,( void*) root,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1273=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1273[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1274; _temp1274.tag= Cyc_CfFlowInfo_InitsFL;
_temp1274.f1= _temp1267; _temp1274;}); _temp1273;}); _LL1262:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)({ struct Cyc_List_List* _temp1275= aes; if( _temp1275 == 0){ _throw(
Null_Exception);} _temp1275->hd;}) ==( void*)({ struct Cyc_List_List* _temp1277=({
struct Cyc_List_List* _temp1276= aes; if( _temp1276 == 0){ _throw(
Null_Exception);} _temp1276->tl;}); if( _temp1277 == 0){ _throw( Null_Exception);}
_temp1277->hd;})){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1279= aes; if( _temp1279 == 0){ _throw(
Null_Exception);} _temp1279->hd;}), in_flow); return({ struct _tuple0 _temp1278;
_temp1278.f1= out_flow; _temp1278.f2= out_flow; _temp1278;});} return({ struct
_tuple0 _temp1280; _temp1280.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1283= aes; if( _temp1283 == 0){ _throw(
Null_Exception);} _temp1283->hd;}), in_flow); _temp1280.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1282=({ struct Cyc_List_List*
_temp1281= aes; if( _temp1281 == 0){ _throw( Null_Exception);} _temp1281->tl;});
if( _temp1282 == 0){ _throw( Null_Exception);} _temp1282->hd;}), in_flow);
_temp1280;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp1284=
scs; if( _temp1284 == 0){ _throw( Null_Exception);} _temp1284->tl;})){ struct
Cyc_Absyn_Switch_clause _temp1288; struct Cyc_Position_Segment* _temp1289;
struct Cyc_Absyn_Stmt* _temp1291; struct Cyc_Absyn_Exp* _temp1293; struct Cyc_Core_Opt*
_temp1295; struct Cyc_Absyn_Pat* _temp1297; struct Cyc_Absyn_Switch_clause*
_temp1286=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1285=
scs; if( _temp1285 == 0){ _throw( Null_Exception);} _temp1285->hd;}); _temp1288=*
_temp1286; _LL1298: _temp1297=( struct Cyc_Absyn_Pat*) _temp1288.pattern; goto
_LL1296; _LL1296: _temp1295=( struct Cyc_Core_Opt*) _temp1288.pat_vars; goto
_LL1294; _LL1294: _temp1293=( struct Cyc_Absyn_Exp*) _temp1288.where_clause;
goto _LL1292; _LL1292: _temp1291=( struct Cyc_Absyn_Stmt*) _temp1288.body; goto
_LL1290; _LL1290: _temp1289=( struct Cyc_Position_Segment*) _temp1288.loc; goto
_LL1287; _LL1287: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1317= _temp1295;
if( _temp1317 == 0){ _throw( Null_Exception);} _temp1317->v;})); body_inflow=
Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1299= _temp1295; if( _temp1299 == 0){ _throw(
Null_Exception);} _temp1299->v;}));{ void* body_outflow; if( _temp1293 != 0){
void* _temp1303; void* _temp1305; struct _tuple0 _temp1301= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1300= _temp1293; if(
_temp1300 == 0){ _throw( Null_Exception);} _temp1300;}))->loc, aes, body_inflow);
_LL1306: _temp1305= _temp1301.f1; goto _LL1304; _LL1304: _temp1303= _temp1301.f2;
goto _LL1302; _LL1302: aes=({ struct Cyc_List_List* _temp1308=({ struct Cyc_List_List*
_temp1307= aes; if( _temp1307 == 0){ _throw( Null_Exception);} _temp1307->tl;});
if( _temp1308 == 0){ _throw( Null_Exception);} _temp1308->tl;}); in_flow=
_temp1303; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1291,
_temp1305);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1291, body_inflow);}{ void* _temp1309= body_outflow; _LL1311: if( _temp1309
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1312;} else{ goto _LL1313;} _LL1313:
goto _LL1314; _LL1312: goto _LL1310; _LL1314: Cyc_Tcutil_terr( _temp1291->loc,(
struct _tagged_string)({ char* _temp1315=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1316; _temp1316.curr= _temp1315; _temp1316.base=
_temp1315; _temp1316.last_plus_one= _temp1315 + 38; _temp1316;})); goto _LL1310;
_LL1310:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1318=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1362; struct Cyc_Absyn_Exp* _temp1364; struct Cyc_Absyn_Exp*
_temp1366; struct Cyc_Absyn_Stmt* _temp1368; struct Cyc_Absyn_Stmt* _temp1370;
struct Cyc_Absyn_Stmt* _temp1372; struct Cyc_Absyn_Stmt* _temp1374; struct Cyc_Absyn_Exp*
_temp1376; struct Cyc_Absyn_Stmt* _temp1378; struct _tuple3 _temp1380; struct
Cyc_Absyn_Stmt* _temp1382; struct Cyc_Absyn_Exp* _temp1384; struct _tuple3
_temp1386; struct Cyc_Absyn_Stmt* _temp1388; struct Cyc_Absyn_Exp* _temp1390;
struct Cyc_Absyn_Stmt* _temp1392; struct Cyc_Absyn_Stmt* _temp1394; struct
_tuple3 _temp1396; struct Cyc_Absyn_Stmt* _temp1398; struct Cyc_Absyn_Exp*
_temp1400; struct _tuple3 _temp1402; struct Cyc_Absyn_Stmt* _temp1404; struct
Cyc_Absyn_Exp* _temp1406; struct Cyc_Absyn_Exp* _temp1408; struct Cyc_Absyn_Stmt*
_temp1410; struct Cyc_Absyn_Switch_clause** _temp1412; struct Cyc_Absyn_Switch_clause*
_temp1414; struct Cyc_List_List* _temp1415; struct Cyc_Absyn_Stmt* _temp1417;
struct Cyc_Absyn_Stmt* _temp1419; struct Cyc_Absyn_Stmt* _temp1421; struct
_tagged_string* _temp1423; struct Cyc_List_List* _temp1425; struct Cyc_Absyn_Exp*
_temp1427; struct Cyc_List_List* _temp1429; struct Cyc_Absyn_Stmt* _temp1431;
struct Cyc_Absyn_Stmt* _temp1433; struct Cyc_Absyn_Decl* _temp1435; struct Cyc_Absyn_Decl
_temp1437; struct Cyc_Position_Segment* _temp1438; void* _temp1440; struct Cyc_Absyn_Vardecl*
_temp1442; struct Cyc_Absyn_Stmt* _temp1444; struct Cyc_Absyn_Decl* _temp1446;
struct Cyc_Absyn_Decl _temp1448; struct Cyc_Position_Segment* _temp1449; void*
_temp1451; int _temp1453; struct Cyc_Absyn_Exp* _temp1455; struct Cyc_Core_Opt*
_temp1457; struct Cyc_Core_Opt* _temp1459; struct Cyc_Core_Opt _temp1461; struct
Cyc_List_List* _temp1462; struct Cyc_Absyn_Pat* _temp1464; struct Cyc_Absyn_Stmt*
_temp1466; struct _tagged_string* _temp1468; struct Cyc_Absyn_Stmt* _temp1470;
struct Cyc_Absyn_Vardecl* _temp1472; struct Cyc_Absyn_Tvar* _temp1474; _LL1320:
if( _temp1318 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1321;} else{ goto _LL1322;}
_LL1322: if(( unsigned int) _temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag
== Cyc_Absyn_Return_s: 0){ _LL1363: _temp1362=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1318)->f1; if( _temp1362 == 0){ goto _LL1323;}
else{ goto _LL1324;}} else{ goto _LL1324;} _LL1324: if(( unsigned int) _temp1318
> 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Return_s: 0){
_LL1365: _temp1364=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1318)->f1; goto _LL1325;} else{ goto _LL1326;} _LL1326: if(( unsigned int)
_temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Exp_s: 0){
_LL1367: _temp1366=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1318)->f1; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(( unsigned int)
_temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Seq_s: 0){
_LL1371: _temp1370=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1318)->f1; goto _LL1369; _LL1369: _temp1368=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1318)->f2; goto _LL1329;} else{ goto
_LL1330;} _LL1330: if(( unsigned int) _temp1318 > 1u?(( struct _tunion_struct*)
_temp1318)->tag == Cyc_Absyn_IfThenElse_s: 0){ _LL1377: _temp1376=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1318)->f1; goto _LL1375; _LL1375:
_temp1374=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1318)->f2; goto _LL1373; _LL1373: _temp1372=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1318)->f3; goto _LL1331;} else{ goto
_LL1332;} _LL1332: if(( unsigned int) _temp1318 > 1u?(( struct _tunion_struct*)
_temp1318)->tag == Cyc_Absyn_While_s: 0){ _LL1381: _temp1380=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp1318)->f1; _LL1385: _temp1384= _temp1380.f1;
goto _LL1383; _LL1383: _temp1382= _temp1380.f2; goto _LL1379; _LL1379: _temp1378=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp1318)->f2; goto
_LL1333;} else{ goto _LL1334;} _LL1334: if(( unsigned int) _temp1318 > 1u?((
struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Do_s: 0){ _LL1393:
_temp1392=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1318)->f1;
goto _LL1387; _LL1387: _temp1386=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1318)->f2; _LL1391: _temp1390= _temp1386.f1; goto _LL1389; _LL1389:
_temp1388= _temp1386.f2; goto _LL1335;} else{ goto _LL1336;} _LL1336: if((
unsigned int) _temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_For_s:
0){ _LL1409: _temp1408=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1318)->f1; goto _LL1403; _LL1403: _temp1402=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1318)->f2; _LL1407: _temp1406= _temp1402.f1; goto _LL1405; _LL1405:
_temp1404= _temp1402.f2; goto _LL1397; _LL1397: _temp1396=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1318)->f3; _LL1401: _temp1400= _temp1396.f1;
goto _LL1399; _LL1399: _temp1398= _temp1396.f2; goto _LL1395; _LL1395: _temp1394=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1318)->f4; goto
_LL1337;} else{ goto _LL1338;} _LL1338: if(( unsigned int) _temp1318 > 1u?((
struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Break_s: 0){ _LL1411:
_temp1410=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1318)->f1; if( _temp1410 == 0){ goto _LL1339;} else{ goto _LL1340;}} else{
goto _LL1340;} _LL1340: if(( unsigned int) _temp1318 > 1u?(( struct
_tunion_struct*) _temp1318)->tag == Cyc_Absyn_Fallthru_s: 0){ _LL1416: _temp1415=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp1318)->f1;
goto _LL1413; _LL1413: _temp1412=( struct Cyc_Absyn_Switch_clause**)(( struct
Cyc_Absyn_Fallthru_s_struct*) _temp1318)->f2; if( _temp1412 == 0){ goto _LL1342;}
else{ _temp1414=* _temp1412; goto _LL1341;}} else{ goto _LL1342;} _LL1342: if((
unsigned int) _temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Break_s:
0){ _LL1418: _temp1417=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1318)->f1; goto _LL1343;} else{ goto _LL1344;} _LL1344: if(( unsigned int)
_temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Continue_s:
0){ _LL1420: _temp1419=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1318)->f1; goto _LL1345;} else{ goto _LL1346;} _LL1346: if(( unsigned int)
_temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Goto_s: 0){
_LL1424: _temp1423=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1318)->f1; goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1318)->f2; goto _LL1347;} else{ goto
_LL1348;} _LL1348: if(( unsigned int) _temp1318 > 1u?(( struct _tunion_struct*)
_temp1318)->tag == Cyc_Absyn_Switch_s: 0){ _LL1428: _temp1427=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1318)->f1; goto _LL1426; _LL1426:
_temp1425=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1318)->f2; goto _LL1349;} else{ goto _LL1350;} _LL1350: if(( unsigned int)
_temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_TryCatch_s:
0){ _LL1432: _temp1431=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1318)->f1; goto _LL1430; _LL1430: _temp1429=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1318)->f2; goto _LL1351;} else{ goto
_LL1352;} _LL1352: if(( unsigned int) _temp1318 > 1u?(( struct _tunion_struct*)
_temp1318)->tag == Cyc_Absyn_Decl_s: 0){ _LL1436: _temp1435=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1318)->f1; _temp1437=* _temp1435; _LL1441:
_temp1440=( void*) _temp1437.r; if((( struct _tunion_struct*) _temp1440)->tag ==
Cyc_Absyn_Var_d){ _LL1443: _temp1442=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1440)->f1; goto _LL1439;} else{ goto _LL1354;} _LL1439: _temp1438=( struct
Cyc_Position_Segment*) _temp1437.loc; goto _LL1434; _LL1434: _temp1433=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1318)->f2; goto _LL1353;}
else{ goto _LL1354;} _LL1354: if(( unsigned int) _temp1318 > 1u?(( struct
_tunion_struct*) _temp1318)->tag == Cyc_Absyn_Decl_s: 0){ _LL1447: _temp1446=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1318)->f1;
_temp1448=* _temp1446; _LL1452: _temp1451=( void*) _temp1448.r; if((( struct
_tunion_struct*) _temp1451)->tag == Cyc_Absyn_Let_d){ _LL1465: _temp1464=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1451)->f1; goto
_LL1460; _LL1460: _temp1459=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1451)->f2; if( _temp1459 == 0){ goto _LL1356;} else{ _temp1461=* _temp1459;
_LL1463: _temp1462=( struct Cyc_List_List*) _temp1461.v; goto _LL1458;} _LL1458:
_temp1457=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1451)->f3;
goto _LL1456; _LL1456: _temp1455=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1451)->f4; goto _LL1454; _LL1454: _temp1453=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1451)->f5; goto _LL1450;} else{ goto _LL1356;} _LL1450: _temp1449=( struct
Cyc_Position_Segment*) _temp1448.loc; goto _LL1445; _LL1445: _temp1444=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1318)->f2; goto _LL1355;}
else{ goto _LL1356;} _LL1356: if(( unsigned int) _temp1318 > 1u?(( struct
_tunion_struct*) _temp1318)->tag == Cyc_Absyn_Label_s: 0){ _LL1469: _temp1468=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1318)->f1; goto
_LL1467; _LL1467: _temp1466=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1318)->f2; goto _LL1357;} else{ goto _LL1358;} _LL1358: if(( unsigned int)
_temp1318 > 1u?(( struct _tunion_struct*) _temp1318)->tag == Cyc_Absyn_Region_s:
0){ _LL1475: _temp1474=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1318)->f1; goto _LL1473; _LL1473: _temp1472=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1318)->f2; goto _LL1471; _LL1471:
_temp1470=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1318)->f3; goto _LL1359;} else{ goto _LL1360;} _LL1360: goto _LL1361;
_LL1321: return in_flow; _LL1323: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1325: Cyc_CfAbsexp_eval_absexp( env,({ struct Cyc_Absyn_Exp* _temp1476=
_temp1364; if( _temp1476 == 0){ _throw( Null_Exception);} _temp1476->loc;}),(
void*)({ struct Cyc_List_List* _temp1477= aes; if( _temp1477 == 0){ _throw(
Null_Exception);} _temp1477->hd;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1327: return Cyc_CfAbsexp_eval_absexp( env, _temp1366->loc,( void*)({ struct
Cyc_List_List* _temp1478= aes; if( _temp1478 == 0){ _throw( Null_Exception);}
_temp1478->hd;}), in_flow); _LL1329: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1368, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1370, in_flow));
_LL1331: { void* _temp1481; void* _temp1483; struct _tuple0 _temp1479= Cyc_NewControlFlow_cf_evalguard(
env, _temp1376->loc, aes, in_flow); _LL1484: _temp1483= _temp1479.f1; goto
_LL1482; _LL1482: _temp1481= _temp1479.f2; goto _LL1480; _LL1480: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1374, _temp1483), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1372, _temp1481));} _LL1333: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1382))->flow; void* _temp1487; void* _temp1489; struct _tuple0
_temp1485= Cyc_NewControlFlow_cf_evalguard( env, _temp1384->loc, aes, e_inflow);
_LL1490: _temp1489= _temp1485.f1; goto _LL1488; _LL1488: _temp1487= _temp1485.f2;
goto _LL1486; _LL1486: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1378, _temp1489); Cyc_NewControlFlow_update_flow( _temp1382,
body_outflow); return _temp1487;}} _LL1335: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1392, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1388))->flow; void* _temp1493; void* _temp1495; struct
_tuple0 _temp1491= Cyc_NewControlFlow_cf_evalguard( env, _temp1390->loc, aes,
e_inflow); _LL1496: _temp1495= _temp1491.f1; goto _LL1494; _LL1494: _temp1493=
_temp1491.f2; goto _LL1492; _LL1492: Cyc_NewControlFlow_update_flow( _temp1392,
_temp1495); return _temp1493;} _LL1337: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1408->loc,( void*)({ struct Cyc_List_List* _temp1508= aes; if(
_temp1508 == 0){ _throw( Null_Exception);} _temp1508->hd;}), in_flow); void*
e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( e1_outflow, _temp1404))->flow;
void* _temp1500; void* _temp1502; struct _tuple0 _temp1498= Cyc_NewControlFlow_cf_evalguard(
env, _temp1406->loc,({ struct Cyc_List_List* _temp1497= aes; if( _temp1497 == 0){
_throw( Null_Exception);} _temp1497->tl;}), e2_inflow); _LL1503: _temp1502=
_temp1498.f1; goto _LL1501; _LL1501: _temp1500= _temp1498.f2; goto _LL1499;
_LL1499: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1394, _temp1502); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1398))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1400->loc,( void*)({ struct Cyc_List_List* _temp1507=({ struct Cyc_List_List*
_temp1506=({ struct Cyc_List_List* _temp1505=({ struct Cyc_List_List* _temp1504=
aes; if( _temp1504 == 0){ _throw( Null_Exception);} _temp1504->tl;}); if(
_temp1505 == 0){ _throw( Null_Exception);} _temp1505->tl;}); if( _temp1506 == 0){
_throw( Null_Exception);} _temp1506->tl;}); if( _temp1507 == 0){ _throw(
Null_Exception);} _temp1507->hd;}), e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1404, e3_outflow); return _temp1500;}} _LL1339: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1341: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)({ struct Cyc_List_List*
_temp1509= aes; if( _temp1509 == 0){ _throw( Null_Exception);} _temp1509->hd;}),
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1510= _temp1414->pat_vars; if( _temp1510 == 0){ _throw( Null_Exception);}
_temp1510->v;}); in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
ftvars); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars);
_temp1417=( struct Cyc_Absyn_Stmt*) _temp1414->body; goto _LL1343;} _LL1343:
_temp1419= _temp1417; goto _LL1345; _LL1345: _temp1421= _temp1419; goto _LL1347;
_LL1347: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt*
_temp1511= _temp1421; if( _temp1511 == 0){ _throw( Null_Exception);} _temp1511;}),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1349: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1427->loc,( void*)({ struct Cyc_List_List* _temp1512= aes; if(
_temp1512 == 0){ _throw( Null_Exception);} _temp1512->hd;}), in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1425,({ struct Cyc_List_List* _temp1513= aes; if( _temp1513 == 0){
_throw( Null_Exception);} _temp1513->tl;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1351: { struct Cyc_NewControlFlow_AnalEnv _temp1516; void* _temp1517; int
_temp1519; struct Cyc_Dict_Dict* _temp1521; struct Cyc_NewControlFlow_AnalEnv*
_temp1514= env; _temp1516=* _temp1514; _LL1522: _temp1521=( struct Cyc_Dict_Dict*)
_temp1516.roots; goto _LL1520; _LL1520: _temp1519=( int) _temp1516.in_try; goto
_LL1518; _LL1518: _temp1517=( void*) _temp1516.tryflow; goto _LL1515; _LL1515:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1431, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1519;( void*)( env->tryflow=( void*) _temp1517); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1429,
aes, scs_inflow); return s_outflow;}} _LL1353: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1525; _temp1525.hd=( void*) _temp1442; _temp1525.tl= 0;
_temp1525;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1442->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1523= e; if( _temp1523 == 0){ _throw( Null_Exception);}
_temp1523;}))->loc,( void*)({ struct Cyc_List_List* _temp1524= aes; if(
_temp1524 == 0){ _throw( Null_Exception);} _temp1524->hd;}), body_inflow);}
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1433, body_inflow);}
_LL1355: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1462); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1455->loc,( void*)({
struct Cyc_List_List* _temp1526= aes; if( _temp1526 == 0){ _throw(
Null_Exception);} _temp1526->hd;}), body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1462); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1444, body_inflow);} _LL1357: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1466, in_flow); _LL1359: { struct Cyc_List_List* vds=({ struct Cyc_List_List*
_temp1527=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1527->hd=( void*) _temp1472; _temp1527->tl= 0; _temp1527;}); void*
body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, vds);
body_inflow= Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow, vds);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1470, body_inflow);}
_LL1361:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1528=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1528[ 0]=({ struct Cyc_Core_Impossible_struct _temp1529; _temp1529.tag= Cyc_Core_Impossible;
_temp1529.f1=( struct _tagged_string)({ char* _temp1530=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1531; _temp1531.curr= _temp1530; _temp1531.base=
_temp1530; _temp1531.last_plus_one= _temp1530 + 62; _temp1531;}); _temp1529;});
_temp1528;})); _LL1319:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1532=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1556; struct Cyc_Absyn_Stmt* _temp1558;
struct Cyc_Absyn_Stmt* _temp1560; struct Cyc_Absyn_Stmt* _temp1562; struct Cyc_Absyn_Exp*
_temp1564; struct Cyc_Absyn_Stmt* _temp1566; struct _tuple3 _temp1568; struct
Cyc_Absyn_Stmt* _temp1570; struct Cyc_Absyn_Exp* _temp1572; struct _tuple3
_temp1574; struct Cyc_Absyn_Stmt* _temp1576; struct Cyc_Absyn_Exp* _temp1578;
struct Cyc_Absyn_Exp* _temp1580; struct Cyc_Absyn_Stmt* _temp1582; struct
_tuple3 _temp1584; struct Cyc_Absyn_Stmt* _temp1586; struct Cyc_Absyn_Exp*
_temp1588; struct _tuple3 _temp1590; struct Cyc_Absyn_Stmt* _temp1592; struct
Cyc_Absyn_Exp* _temp1594; struct Cyc_Absyn_Stmt* _temp1596; struct Cyc_Absyn_Stmt*
_temp1598; struct Cyc_Absyn_Vardecl* _temp1600; struct Cyc_Absyn_Tvar* _temp1602;
struct Cyc_Absyn_Stmt* _temp1604; struct _tagged_string* _temp1606; struct Cyc_Absyn_Stmt*
_temp1608; struct Cyc_Absyn_Decl* _temp1610; struct Cyc_List_List* _temp1612;
struct Cyc_Absyn_Stmt* _temp1614; struct Cyc_List_List* _temp1616; struct Cyc_Absyn_Exp*
_temp1618; _LL1534: if(( unsigned int) _temp1532 > 1u?(( struct _tunion_struct*)
_temp1532)->tag == Cyc_Absyn_Seq_s: 0){ _LL1559: _temp1558=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1532)->f1; goto _LL1557; _LL1557: _temp1556=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1532)->f2; goto
_LL1535;} else{ goto _LL1536;} _LL1536: if(( unsigned int) _temp1532 > 1u?((
struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_IfThenElse_s: 0){ _LL1565:
_temp1564=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1532)->f1; goto _LL1563; _LL1563: _temp1562=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1532)->f2; goto _LL1561; _LL1561:
_temp1560=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1532)->f3; goto _LL1537;} else{ goto _LL1538;} _LL1538: if(( unsigned int)
_temp1532 > 1u?(( struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_For_s: 0){
_LL1581: _temp1580=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1532)->f1; goto _LL1575; _LL1575: _temp1574=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1532)->f2; _LL1579: _temp1578= _temp1574.f1; goto _LL1577; _LL1577:
_temp1576= _temp1574.f2; goto _LL1569; _LL1569: _temp1568=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1532)->f3; _LL1573: _temp1572= _temp1568.f1;
goto _LL1571; _LL1571: _temp1570= _temp1568.f2; goto _LL1567; _LL1567: _temp1566=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1532)->f4; goto
_LL1539;} else{ goto _LL1540;} _LL1540: if(( unsigned int) _temp1532 > 1u?((
struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_While_s: 0){ _LL1585:
_temp1584=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1532)->f1;
_LL1589: _temp1588= _temp1584.f1; goto _LL1587; _LL1587: _temp1586= _temp1584.f2;
goto _LL1583; _LL1583: _temp1582=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1532)->f2; goto _LL1541;} else{ goto _LL1542;} _LL1542: if(( unsigned int)
_temp1532 > 1u?(( struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_Do_s: 0){
_LL1597: _temp1596=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1532)->f1; goto _LL1591; _LL1591: _temp1590=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1532)->f2; _LL1595: _temp1594= _temp1590.f1; goto _LL1593; _LL1593:
_temp1592= _temp1590.f2; goto _LL1543;} else{ goto _LL1544;} _LL1544: if((
unsigned int) _temp1532 > 1u?(( struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_Region_s:
0){ _LL1603: _temp1602=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1532)->f1; goto _LL1601; _LL1601: _temp1600=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1532)->f2; goto _LL1599; _LL1599:
_temp1598=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1532)->f3; goto _LL1545;} else{ goto _LL1546;} _LL1546: if(( unsigned int)
_temp1532 > 1u?(( struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_Label_s:
0){ _LL1607: _temp1606=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1532)->f1; goto _LL1605; _LL1605: _temp1604=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1532)->f2; goto _LL1547;} else{ goto
_LL1548;} _LL1548: if(( unsigned int) _temp1532 > 1u?(( struct _tunion_struct*)
_temp1532)->tag == Cyc_Absyn_Decl_s: 0){ _LL1611: _temp1610=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1532)->f1; goto _LL1609; _LL1609:
_temp1608=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1532)->f2;
goto _LL1549;} else{ goto _LL1550;} _LL1550: if(( unsigned int) _temp1532 > 1u?((
struct _tunion_struct*) _temp1532)->tag == Cyc_Absyn_TryCatch_s: 0){ _LL1615:
_temp1614=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1532)->f1; goto _LL1613; _LL1613: _temp1612=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1532)->f2; goto _LL1551;} else{ goto
_LL1552;} _LL1552: if(( unsigned int) _temp1532 > 1u?(( struct _tunion_struct*)
_temp1532)->tag == Cyc_Absyn_Switch_s: 0){ _LL1619: _temp1618=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1532)->f1; goto _LL1617; _LL1617:
_temp1616=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1532)->f2; goto _LL1553;} else{ goto _LL1554;} _LL1554: goto _LL1555;
_LL1535: _temp1562= _temp1558; _temp1560= _temp1556; goto _LL1537; _LL1537: Cyc_NewControlFlow_cf_postpass(
_temp1562); Cyc_NewControlFlow_cf_postpass( _temp1560); return; _LL1539: Cyc_NewControlFlow_cf_postpass(
_temp1570); _temp1586= _temp1566; _temp1582= _temp1576; goto _LL1541; _LL1541:
_temp1596= _temp1582; _temp1592= _temp1586; goto _LL1543; _LL1543: Cyc_NewControlFlow_cf_postpass(
_temp1592); _temp1598= _temp1596; goto _LL1545; _LL1545: _temp1604= _temp1598;
goto _LL1547; _LL1547: _temp1608= _temp1604; goto _LL1549; _LL1549: Cyc_NewControlFlow_cf_postpass(
_temp1608); return; _LL1551: Cyc_NewControlFlow_cf_postpass( _temp1614);
_temp1616= _temp1612; goto _LL1553; _LL1553: for( 0; _temp1616 != 0; _temp1616=({
struct Cyc_List_List* _temp1620= _temp1616; if( _temp1620 == 0){ _throw(
Null_Exception);} _temp1620->tl;})){ Cyc_NewControlFlow_cf_postpass((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1621= _temp1616; if(
_temp1621 == 0){ _throw( Null_Exception);} _temp1621->hd;}))->body);} return;
_LL1555: return; _LL1533:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1649; _temp1649.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1650=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1650->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1650->comprehension_vars=
0; _temp1650;}); _temp1649.lhsides= 0; _temp1649.access_path= 0; _temp1649;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1648= fd->param_vardecls; if( _temp1648 == 0){ _throw( Null_Exception);}
_temp1648->v;});{ struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds=({
struct Cyc_List_List* _temp1622= vds; if( _temp1622 == 0){ _throw(
Null_Exception);} _temp1622->tl;})){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1623= vds; if( _temp1623
== 0){ _throw( Null_Exception);} _temp1623->hd;}));}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1626; struct Cyc_List_List*
_temp1628; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1630; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1632; struct Cyc_List_List* _temp1633; struct Cyc_Dict_Dict* _temp1635;
struct Cyc_NewControlFlow_AbsEnv _temp1624= absenv; _LL1631: _temp1630=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1624.shared; _temp1632=* _temp1630;
_LL1636: _temp1635=( struct Cyc_Dict_Dict*) _temp1632.all_roots; goto _LL1634;
_LL1634: _temp1633=( struct Cyc_List_List*) _temp1632.comprehension_vars; goto
_LL1629; _LL1629: _temp1628=( struct Cyc_List_List*) _temp1624.lhsides; goto
_LL1627; _LL1627: _temp1626=( struct Cyc_List_List*) _temp1624.access_path; goto
_LL1625; _LL1625: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1647=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1647->roots= _temp1635; _temp1647->in_try= 0; _temp1647->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1647;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1645=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1645[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1646; _temp1646.tag= Cyc_CfFlowInfo_InitsFL;
_temp1646.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1646;}); _temp1645;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1633);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1637= out_flow; _LL1639: if( _temp1637 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1640;} else{ goto _LL1641;} _LL1641: goto _LL1642; _LL1640: goto _LL1638;
_LL1642: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1643=( char*)"function may complete without returning a value";
struct _tagged_string _temp1644; _temp1644.curr= _temp1643; _temp1644.base=
_temp1643; _temp1644.last_plus_one= _temp1643 + 48; _temp1644;}));} goto _LL1638;
_LL1638:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1651= ds; if( _temp1651 == 0){ _throw( Null_Exception);} _temp1651->tl;})){
void* _temp1653=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1652= ds; if( _temp1652 == 0){ _throw( Null_Exception);} _temp1652->hd;}))->r;
struct Cyc_Absyn_Fndecl* _temp1663; struct Cyc_List_List* _temp1665; struct
_tuple1* _temp1667; struct Cyc_List_List* _temp1669; struct _tagged_string*
_temp1671; _LL1655: if((( struct _tunion_struct*) _temp1653)->tag == Cyc_Absyn_Fn_d){
_LL1664: _temp1663=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1653)->f1; goto _LL1656;} else{ goto _LL1657;} _LL1657: if((( struct
_tunion_struct*) _temp1653)->tag == Cyc_Absyn_Using_d){ _LL1668: _temp1667=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp1653)->f1; goto
_LL1666; _LL1666: _temp1665=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1653)->f2; goto _LL1658;} else{ goto _LL1659;} _LL1659: if((( struct
_tunion_struct*) _temp1653)->tag == Cyc_Absyn_Namespace_d){ _LL1672: _temp1671=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1653)->f1;
goto _LL1670; _LL1670: _temp1669=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1653)->f2; goto _LL1660;} else{ goto _LL1661;} _LL1661: goto _LL1662;
_LL1656: Cyc_NewControlFlow_cf_check_fun( _temp1663); goto _LL1654; _LL1658:
_temp1669= _temp1665; goto _LL1660; _LL1660: Cyc_NewControlFlow_cf_check(
_temp1669); goto _LL1654; _LL1662: goto _LL1654; _LL1654:;}}