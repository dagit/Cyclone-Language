#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; struct _tuple0{ void*
f1; void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
src){ struct Cyc_NewControlFlow_AbsEnv ans= Cyc_NewControlFlow_change_lhs( src,
Cyc_NewControlFlow_use_it_list()); ans.access_path= 0; return ans;} struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn syn){ struct
Cyc_List_List* _temp24; struct Cyc_NewControlFlow_AbsSynOne _temp26; void*
_temp28; void* _temp30; struct Cyc_NewControlFlow_AbsSynOne _temp32; void*
_temp34; void* _temp36; struct Cyc_NewControlFlow_AbsSyn _temp22= syn; _LL33:
_temp32=( struct Cyc_NewControlFlow_AbsSynOne) _temp22.when_true; _LL37: _temp36=(
void*) _temp32.inner_exp; goto _LL35; _LL35: _temp34=( void*) _temp32.assigns;
goto _LL27; _LL27: _temp26=( struct Cyc_NewControlFlow_AbsSynOne) _temp22.when_false;
_LL31: _temp30=( void*) _temp26.inner_exp; goto _LL29; _LL29: _temp28=( void*)
_temp26.assigns; goto _LL25; _LL25: _temp24=( struct Cyc_List_List*) _temp22.lvalues;
goto _LL23; _LL23: { void* inn= _temp36 == _temp30? _temp36: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp36, _temp30); void* ass= _temp34 == _temp28?
_temp34: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp34, _temp28);
return({ struct Cyc_NewControlFlow_AbsSynOne _temp38; _temp38.inner_exp=( void*)
inn; _temp38.assigns=( void*) ass; _temp38;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne one_way=({ struct Cyc_NewControlFlow_AbsSynOne
_temp40; _temp40.inner_exp=( void*) ae; _temp40.assigns=( void*) assigns;
_temp40;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp39; _temp39.when_true=
one_way; _temp39.when_false= one_way; _temp39.lvalues= lvals; _temp39;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(({ struct Cyc_List_List*
_temp41= env.lhsides; if( _temp41 == 0){ _throw( Null_Exception);} _temp41->tl;})
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List* _temp42=
env.lhsides; if( _temp42 == 0){ _throw( Null_Exception);} _temp42->hd;}), ae);}{
struct Cyc_List_List* l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0;
lhsides != 0; lhsides=({ struct Cyc_List_List* _temp43= lhsides; if( _temp43 ==
0){ _throw( Null_Exception);} _temp43->tl;})){ l=({ struct Cyc_List_List*
_temp44=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp44->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)({ struct Cyc_List_List*
_temp45= lhsides; if( _temp45 == 0){ _throw( Null_Exception);} _temp45->hd;}),
ae); _temp44->tl= l; _temp44;});}} return Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_OneofG,
l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es=({ struct Cyc_List_List*
_temp46= es; if( _temp46 == 0){ _throw( Null_Exception);} _temp46->tl;})){
struct Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp53= es; if( _temp53 == 0){
_throw( Null_Exception);} _temp53->hd;})); void* _temp49; void* _temp51; struct
Cyc_NewControlFlow_AbsSynOne _temp47= Cyc_NewControlFlow_meet_absexp( syn);
_LL52: _temp51=( void*) _temp47.inner_exp; goto _LL50; _LL50: _temp49=( void*)
_temp47.assigns; goto _LL48; _LL48: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp51, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
_temp49, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_descend_path(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp*
inner_e){ env.access_path=({ struct Cyc_List_List* _temp54=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp54->hd=( void*) path_e; _temp54->tl=
env.access_path; _temp54;});{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, inner_e); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path=({ struct Cyc_List_List* _temp55= path; if( _temp55 == 0){ _throw(
Null_Exception);} _temp55->tl;})){ void* _temp57=( void*)(( struct Cyc_Absyn_Exp*)({
struct Cyc_List_List* _temp56= path; if( _temp56 == 0){ _throw( Null_Exception);}
_temp56->hd;}))->r; struct Cyc_Absyn_Exp* _temp71; struct Cyc_Absyn_Exp* _temp73;
struct _tagged_string* _temp75; struct Cyc_Absyn_Exp* _temp77; struct
_tagged_string* _temp79; struct Cyc_Absyn_Exp* _temp81; struct Cyc_Absyn_Exp*
_temp83; struct Cyc_Absyn_Exp* _temp85; _LL59: if(*(( int*) _temp57) == Cyc_Absyn_Address_e){
_LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp57)->f1; goto _LL60;} else{ goto _LL61;} _LL61: if(*(( int*) _temp57) ==
Cyc_Absyn_Deref_e){ _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp57)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if(*(( int*) _temp57) ==
Cyc_Absyn_StructArrow_e){ _LL78: _temp77=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp57)->f1; goto _LL76; _LL76: _temp75=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp57)->f2; goto _LL64;} else{ goto _LL65;} _LL65: if(*(( int*) _temp57) ==
Cyc_Absyn_StructMember_e){ _LL82: _temp81=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp57)->f1; goto _LL80; _LL80: _temp79=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp57)->f2; goto _LL66;} else{ goto _LL67;} _LL67: if(*(( int*) _temp57) ==
Cyc_Absyn_Subscript_e){ _LL86: _temp85=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp57)->f1; goto _LL84; _LL84: _temp83=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp57)->f2; goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70; _LL60:({ struct
Cyc_List_List* _temp87= path; if( _temp87 == 0){ _throw( Null_Exception);}
_temp87->tl;}) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp88=( char*)"new_control_flow.cyc:219 path->tl == null";
struct _tagged_string _temp89; _temp89.curr= _temp88; _temp89.base= _temp88;
_temp89.last_plus_one= _temp88 + 42; _temp89;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL62: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL58; _LL64: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp79= _temp75; goto _LL66; _LL66: absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_StructF_struct* _temp90=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp90[ 0]=({ struct
Cyc_CfFlowInfo_StructF_struct _temp91; _temp91.tag= Cyc_CfFlowInfo_StructF;
_temp91.f1= _temp79; _temp91;}); _temp90;})); goto _LL58; _LL68: { unsigned int
i= Cyc_Evexp_eval_const_uint_exp( _temp83); absop= Cyc_CfAbsexp_mkMemberOp(
absop,( void*)({ struct Cyc_CfFlowInfo_TupleF_struct* _temp92=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp92[ 0]=({ struct
Cyc_CfFlowInfo_TupleF_struct _temp93; _temp93.tag= Cyc_CfFlowInfo_TupleF;
_temp93.f1=( int) i; _temp93;}); _temp92;})); goto _LL58;} _LL70:( void) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp94=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp94[ 0]=({ struct
Cyc_Core_Impossible_struct _temp95; _temp95.tag= Cyc_Core_Impossible; _temp95.f1=(
struct _tagged_string)({ char* _temp96=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp97; _temp97.curr= _temp96; _temp97.base= _temp96;
_temp97.last_plus_one= _temp96 + 44; _temp97;}); _temp95;}); _temp94;})); _LL58:;}}
return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp*
rgn_exp, struct Cyc_Absyn_Exp* init_exp){ void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
outer_e); void* inner= Cyc_CfAbsexp_mkSkipAE(); void* assigns= Cyc_CfAbsexp_mkSkipAE();
if( init_exp != 0){ struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs(
env,({ struct Cyc_List_List* _temp105=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp105->hd=( void*) malloc_op; _temp105->tl= 0;
_temp105;})); inner_env.access_path= 0;{ void* _temp101; void* _temp103; struct
Cyc_NewControlFlow_AbsSynOne _temp99= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp98= init_exp;
if( _temp98 == 0){ _throw( Null_Exception);} _temp98;}))); _LL104: _temp103=(
void*) _temp99.inner_exp; goto _LL102; _LL102: _temp101=( void*) _temp99.assigns;
goto _LL100; _LL100: inner= _temp103; assigns= _temp101;}} if( rgn_exp != 0){
struct Cyc_NewControlFlow_AbsSyn r_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp112= rgn_exp; if(
_temp112 == 0){ _throw( Null_Exception);} _temp112;})); void* _temp108; void*
_temp110; struct Cyc_NewControlFlow_AbsSynOne _temp106= Cyc_NewControlFlow_meet_absexp(
r_syn); _LL111: _temp110=( void*) _temp106.inner_exp; goto _LL109; _LL109:
_temp108=( void*) _temp106.assigns; goto _LL107; _LL107: inner= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, inner, _temp110); assigns= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assigns, _temp108);}{ void* ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, inner, assigns)); struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkAddressOp( malloc_op))); if( env.access_path != 0){ void*
_temp114=( void*)(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp113= env.access_path;
if( _temp113 == 0){ _throw( Null_Exception);} _temp113->hd;}))->r; struct Cyc_Absyn_Exp*
_temp122; struct _tagged_string* _temp124; struct Cyc_Absyn_Exp* _temp126;
_LL116: if(*(( int*) _temp114) == Cyc_Absyn_Deref_e){ _LL123: _temp122=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp114)->f1; goto _LL117;}
else{ goto _LL118;} _LL118: if(*(( int*) _temp114) == Cyc_Absyn_StructArrow_e){
_LL127: _temp126=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp114)->f1; goto _LL125; _LL125: _temp124=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp114)->f2; goto _LL119;} else{ goto _LL120;}
_LL120: goto _LL121; _LL117: env.access_path=({ struct Cyc_List_List* _temp128=
env.access_path; if( _temp128 == 0){ _throw( Null_Exception);} _temp128->tl;});
ans.lvalues=({ struct Cyc_List_List* _temp129=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp129->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp129->tl= 0; _temp129;}); goto _LL115; _LL119: env.access_path=({
struct Cyc_List_List* _temp130=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp130->hd=( void*) Cyc_Absyn_structmember_exp(
_temp126, _temp124, 0); _temp130->tl=({ struct Cyc_List_List* _temp131= env.access_path;
if( _temp131 == 0){ _throw( Null_Exception);} _temp131->tl;}); _temp130;}); ans.lvalues=({
struct Cyc_List_List* _temp132=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp132->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp132->tl= 0; _temp132;}); goto _LL115; _LL121:( void)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp133=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp133[ 0]=({ struct
Cyc_Core_Impossible_struct _temp134; _temp134.tag= Cyc_Core_Impossible; _temp134.f1=(
struct _tagged_string)({ char* _temp135=( char*)"bad malloc access path"; struct
_tagged_string _temp136; _temp136.curr= _temp135; _temp136.base= _temp135;
_temp136.last_plus_one= _temp135 + 23; _temp136;}); _temp134;}); _temp133;}));
_LL115:;} return ans;}} struct _tuple4{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp137=( void*)
e->r; struct Cyc_Absyn_Exp* _temp225; struct Cyc_Absyn_Exp* _temp227; struct Cyc_List_List*
_temp229; struct Cyc_Absyn_Exp* _temp231; void* _temp233; int _temp235; void*
_temp237; void* _temp239; int _temp241; void* _temp243; void* _temp245; void*
_temp247; struct Cyc_Absyn_Enumfield* _temp249; struct Cyc_Absyn_Enumdecl*
_temp251; struct _tuple1* _temp253; void* _temp255; struct Cyc_Absyn_Fndecl*
_temp257; struct _tuple1* _temp259; void* _temp261; struct Cyc_Absyn_Vardecl*
_temp263; struct _tuple1* _temp265; void* _temp267; struct Cyc_Absyn_Vardecl*
_temp269; struct _tuple1* _temp271; void* _temp273; struct Cyc_Absyn_Vardecl*
_temp275; struct _tuple1* _temp277; void* _temp279; struct Cyc_Absyn_Vardecl*
_temp281; struct _tuple1* _temp283; struct Cyc_Absyn_Exp* _temp285; struct Cyc_Core_Opt*
_temp287; struct Cyc_Absyn_Exp* _temp289; struct Cyc_Absyn_Exp* _temp291; struct
Cyc_Core_Opt* _temp293; struct Cyc_Absyn_Exp* _temp295; struct Cyc_List_List*
_temp297; void* _temp299; struct Cyc_Absyn_Exp* _temp301; void* _temp303; struct
Cyc_List_List* _temp305; struct Cyc_Absyn_Exp* _temp307; struct Cyc_Absyn_Exp*
_temp309; struct Cyc_Absyn_Exp* _temp311; struct Cyc_Absyn_Exp* _temp313; struct
Cyc_Absyn_Exp* _temp315; struct Cyc_Absyn_Exp* _temp317; void* _temp319; struct
Cyc_Absyn_Exp* _temp321; struct Cyc_Absyn_Exp* _temp323; struct _tagged_string*
_temp325; struct Cyc_Absyn_Exp* _temp327; struct _tagged_string* _temp329;
struct Cyc_Absyn_Exp* _temp331; struct Cyc_Absyn_Tunionfield* _temp333; struct
Cyc_Absyn_Tuniondecl* _temp335; struct Cyc_List_List* _temp337; struct Cyc_Core_Opt*
_temp339; struct Cyc_Core_Opt* _temp341; struct Cyc_List_List* _temp343; struct
Cyc_List_List* _temp345; void* _temp347; struct Cyc_Absyn_Structdecl* _temp349;
struct Cyc_List_List* _temp351; struct Cyc_Core_Opt* _temp353; struct _tuple1*
_temp355; struct Cyc_List_List* _temp357; void* _temp359; struct Cyc_Absyn_Exp*
_temp361; struct Cyc_Absyn_Exp* _temp363; struct Cyc_Absyn_Exp* _temp365; struct
Cyc_Absyn_Exp* _temp367; struct Cyc_Absyn_Exp* _temp369; struct Cyc_Absyn_Stmt*
_temp371; struct Cyc_Absyn_Exp* _temp373; struct Cyc_Absyn_Exp* _temp375; struct
Cyc_Absyn_Exp* _temp377; struct Cyc_Absyn_Exp* _temp379; struct Cyc_Absyn_Vardecl*
_temp381; void* _temp383; struct _tuple1* _temp385; struct _tuple1* _temp387;
struct Cyc_List_List* _temp389; struct Cyc_Absyn_Exp* _temp391; struct Cyc_List_List*
_temp393; struct Cyc_Core_Opt* _temp395; struct Cyc_List_List* _temp397; struct
_tuple2* _temp399; struct Cyc_Absyn_Fndecl* _temp401; struct Cyc_Absyn_Exp*
_temp403; _LL139: if(*(( int*) _temp137) == Cyc_Absyn_NoInstantiate_e){ _LL226:
_temp225=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp137)->f1; goto _LL140;} else{ goto _LL141;} _LL141: if(*(( int*) _temp137)
== Cyc_Absyn_Sizeofexp_e){ _LL228: _temp227=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Sizeofexp_e_struct*) _temp137)->f1; goto _LL142;} else{ goto _LL143;}
_LL143: if(*(( int*) _temp137) == Cyc_Absyn_Instantiate_e){ _LL232: _temp231=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp137)->f1;
goto _LL230; _LL230: _temp229=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp137)->f2; goto _LL144;} else{ goto _LL145;} _LL145: if(*(( int*) _temp137)
== Cyc_Absyn_Const_e){ _LL234: _temp233=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp137)->f1; if(( unsigned int) _temp233 > 1u?*(( int*) _temp233) == Cyc_Absyn_Int_c:
0){ _LL238: _temp237=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp233)->f1;
if( _temp237 ==( void*) Cyc_Absyn_Signed){ goto _LL236;} else{ goto _LL147;}
_LL236: _temp235=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp233)->f2; if(
_temp235 == 0){ goto _LL146;} else{ goto _LL147;}} else{ goto _LL147;}} else{
goto _LL147;} _LL147: if(*(( int*) _temp137) == Cyc_Absyn_Const_e){ _LL240:
_temp239=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp137)->f1; if((
unsigned int) _temp239 > 1u?*(( int*) _temp239) == Cyc_Absyn_Int_c: 0){ _LL244:
_temp243=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp239)->f1; if( _temp243
==( void*) Cyc_Absyn_Signed){ goto _LL242;} else{ goto _LL149;} _LL242: _temp241=(
int)(( struct Cyc_Absyn_Int_c_struct*) _temp239)->f2; if( _temp241 == 1){ goto
_LL148;} else{ goto _LL149;}} else{ goto _LL149;}} else{ goto _LL149;} _LL149:
if(*(( int*) _temp137) == Cyc_Absyn_Sizeoftyp_e){ _LL246: _temp245=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp137)->f1; goto _LL150;} else{ goto
_LL151;} _LL151: if(*(( int*) _temp137) == Cyc_Absyn_Const_e){ _LL248: _temp247=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp137)->f1; goto _LL152;} else{
goto _LL153;} _LL153: if(*(( int*) _temp137) == Cyc_Absyn_Enum_e){ _LL254:
_temp253=( struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp137)->f1;
goto _LL252; _LL252: _temp251=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp137)->f2; goto _LL250; _LL250: _temp249=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp137)->f3; goto _LL154;} else{ goto _LL155;}
_LL155: if(*(( int*) _temp137) == Cyc_Absyn_Var_e){ _LL260: _temp259=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp137)->f1; goto _LL256; _LL256:
_temp255=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp137)->f2; if((
unsigned int) _temp255 > 1u?*(( int*) _temp255) == Cyc_Absyn_Funname_b: 0){
_LL258: _temp257=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp255)->f1; goto _LL156;} else{ goto _LL157;}} else{ goto _LL157;} _LL157:
if(*(( int*) _temp137) == Cyc_Absyn_Var_e){ _LL266: _temp265=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp137)->f1; goto _LL262; _LL262: _temp261=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp137)->f2; if(( unsigned int)
_temp261 > 1u?*(( int*) _temp261) == Cyc_Absyn_Global_b: 0){ _LL264: _temp263=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*) _temp261)->f1;
goto _LL158;} else{ goto _LL159;}} else{ goto _LL159;} _LL159: if(*(( int*)
_temp137) == Cyc_Absyn_Var_e){ _LL272: _temp271=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp137)->f1; goto _LL268; _LL268: _temp267=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp137)->f2; if(( unsigned int) _temp267 > 1u?*(( int*) _temp267) == Cyc_Absyn_Param_b:
0){ _LL270: _temp269=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp267)->f1; goto _LL160;} else{ goto _LL161;}} else{ goto _LL161;} _LL161:
if(*(( int*) _temp137) == Cyc_Absyn_Var_e){ _LL278: _temp277=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp137)->f1; goto _LL274; _LL274: _temp273=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp137)->f2; if(( unsigned int)
_temp273 > 1u?*(( int*) _temp273) == Cyc_Absyn_Local_b: 0){ _LL276: _temp275=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*) _temp273)->f1;
goto _LL162;} else{ goto _LL163;}} else{ goto _LL163;} _LL163: if(*(( int*)
_temp137) == Cyc_Absyn_Var_e){ _LL284: _temp283=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp137)->f1; goto _LL280; _LL280: _temp279=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp137)->f2; if(( unsigned int) _temp279 > 1u?*(( int*) _temp279) == Cyc_Absyn_Pat_b:
0){ _LL282: _temp281=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp279)->f1; goto _LL164;} else{ goto _LL165;}} else{ goto _LL165;} _LL165:
if(*(( int*) _temp137) == Cyc_Absyn_AssignOp_e){ _LL290: _temp289=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp137)->f1; goto _LL288; _LL288:
_temp287=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp137)->f2;
if( _temp287 == 0){ goto _LL286;} else{ goto _LL167;} _LL286: _temp285=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp137)->f3; goto
_LL166;} else{ goto _LL167;} _LL167: if(*(( int*) _temp137) == Cyc_Absyn_AssignOp_e){
_LL296: _temp295=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp137)->f1; goto _LL294; _LL294: _temp293=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp137)->f2; goto _LL292; _LL292: _temp291=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp137)->f3;
goto _LL168;} else{ goto _LL169;} _LL169: if(*(( int*) _temp137) == Cyc_Absyn_Primop_e){
_LL300: _temp299=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp137)->f1;
goto _LL298; _LL298: _temp297=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp137)->f2; goto _LL170;} else{ goto _LL171;} _LL171: if(*(( int*) _temp137)
== Cyc_Absyn_Cast_e){ _LL304: _temp303=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp137)->f1; goto _LL302; _LL302: _temp301=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp137)->f2; goto _LL172;} else{ goto _LL173;}
_LL173: if(*(( int*) _temp137) == Cyc_Absyn_FnCall_e){ _LL308: _temp307=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp137)->f1; goto _LL306;
_LL306: _temp305=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp137)->f2; goto _LL174;} else{ goto _LL175;} _LL175: if(*(( int*) _temp137)
== Cyc_Absyn_Subscript_e){ _LL312: _temp311=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp137)->f1; goto _LL310; _LL310: _temp309=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp137)->f2;
goto _LL176;} else{ goto _LL177;} _LL177: if(*(( int*) _temp137) == Cyc_Absyn_Address_e){
_LL314: _temp313=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp137)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(*(( int*) _temp137)
== Cyc_Absyn_New_e){ _LL318: _temp317=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp137)->f1; goto _LL316; _LL316: _temp315=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_New_e_struct*) _temp137)->f2; goto _LL180;} else{ goto _LL181;} _LL181:
if(*(( int*) _temp137) == Cyc_Absyn_Malloc_e){ _LL322: _temp321=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Malloc_e_struct*) _temp137)->f1; goto _LL320; _LL320: _temp319=(
void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp137)->f2; goto _LL182;} else{
goto _LL183;} _LL183: if(*(( int*) _temp137) == Cyc_Absyn_Deref_e){ _LL324:
_temp323=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp137)->f1;
goto _LL184;} else{ goto _LL185;} _LL185: if(*(( int*) _temp137) == Cyc_Absyn_StructArrow_e){
_LL328: _temp327=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp137)->f1; goto _LL326; _LL326: _temp325=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp137)->f2; goto _LL186;} else{ goto _LL187;}
_LL187: if(*(( int*) _temp137) == Cyc_Absyn_StructMember_e){ _LL332: _temp331=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp137)->f1;
goto _LL330; _LL330: _temp329=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp137)->f2; goto _LL188;} else{ goto _LL189;} _LL189: if(*(( int*) _temp137)
== Cyc_Absyn_Tunion_e){ _LL342: _temp341=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp137)->f1; goto _LL340; _LL340: _temp339=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp137)->f2; goto _LL338; _LL338: _temp337=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp137)->f3; goto
_LL336; _LL336: _temp335=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp137)->f4; goto _LL334; _LL334: _temp333=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp137)->f5; goto _LL190;} else{ goto
_LL191;} _LL191: if(*(( int*) _temp137) == Cyc_Absyn_Tuple_e){ _LL344: _temp343=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp137)->f1; goto
_LL192;} else{ goto _LL193;} _LL193: if(*(( int*) _temp137) == Cyc_Absyn_AnonStruct_e){
_LL348: _temp347=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp137)->f1;
goto _LL346; _LL346: _temp345=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp137)->f2; goto _LL194;} else{ goto _LL195;} _LL195: if(*(( int*) _temp137)
== Cyc_Absyn_Struct_e){ _LL356: _temp355=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp137)->f1; goto _LL354; _LL354: _temp353=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp137)->f2; goto _LL352; _LL352: _temp351=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp137)->f3; goto
_LL350; _LL350: _temp349=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp137)->f4; goto _LL196;} else{ goto _LL197;} _LL197: if(*(( int*) _temp137)
== Cyc_Absyn_Array_e){ _LL358: _temp357=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp137)->f1; goto _LL198;} else{ goto _LL199;} _LL199: if(*(( int*) _temp137)
== Cyc_Absyn_Increment_e){ _LL362: _temp361=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Increment_e_struct*) _temp137)->f1; goto _LL360; _LL360: _temp359=(
void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp137)->f2; goto _LL200;}
else{ goto _LL201;} _LL201: if(*(( int*) _temp137) == Cyc_Absyn_Throw_e){ _LL364:
_temp363=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp137)->f1;
goto _LL202;} else{ goto _LL203;} _LL203: if(*(( int*) _temp137) == Cyc_Absyn_Conditional_e){
_LL370: _temp369=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp137)->f1; goto _LL368; _LL368: _temp367=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp137)->f2; goto _LL366; _LL366: _temp365=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp137)->f3;
goto _LL204;} else{ goto _LL205;} _LL205: if(*(( int*) _temp137) == Cyc_Absyn_StmtExp_e){
_LL372: _temp371=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp137)->f1; goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp137)
== Cyc_Absyn_SeqExp_e){ _LL376: _temp375=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp137)->f1; goto _LL374; _LL374: _temp373=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp137)->f2; goto _LL208;} else{ goto _LL209;}
_LL209: if(*(( int*) _temp137) == Cyc_Absyn_Comprehension_e){ _LL382: _temp381=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp137)->f1;
goto _LL380; _LL380: _temp379=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp137)->f2; goto _LL378; _LL378: _temp377=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp137)->f3; goto _LL210;} else{ goto
_LL211;} _LL211: if(*(( int*) _temp137) == Cyc_Absyn_Var_e){ _LL386: _temp385=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp137)->f1; goto _LL384;
_LL384: _temp383=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp137)->f2; if(
_temp383 ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL212;} else{ goto _LL213;}}
else{ goto _LL213;} _LL213: if(*(( int*) _temp137) == Cyc_Absyn_UnknownId_e){
_LL388: _temp387=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp137)->f1; goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*) _temp137)
== Cyc_Absyn_UnknownCall_e){ _LL392: _temp391=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp137)->f1; goto _LL390; _LL390: _temp389=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp137)->f2;
goto _LL216;} else{ goto _LL217;} _LL217: if(*(( int*) _temp137) == Cyc_Absyn_UnresolvedMem_e){
_LL396: _temp395=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp137)->f1; goto _LL394; _LL394: _temp393=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp137)->f2; goto _LL218;} else{ goto
_LL219;} _LL219: if(*(( int*) _temp137) == Cyc_Absyn_CompoundLit_e){ _LL400:
_temp399=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp137)->f1;
goto _LL398; _LL398: _temp397=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp137)->f2; goto _LL220;} else{ goto _LL221;} _LL221: if(*(( int*) _temp137)
== Cyc_Absyn_Codegen_e){ _LL402: _temp401=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Codegen_e_struct*) _temp137)->f1; goto _LL222;} else{ goto _LL223;}
_LL223: if(*(( int*) _temp137) == Cyc_Absyn_Fill_e){ _LL404: _temp403=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp137)->f1; goto _LL224;}
else{ goto _LL138;} _LL140: _temp227= _temp225; goto _LL142; _LL142: _temp231=
_temp227; goto _LL144; _LL144: return Cyc_NewControlFlow_abstract_exp( env,
_temp231); _LL146: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp405=( char*)"new_control_flow.cyc:284 env.access_path==null";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 47; _temp406;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp407; _temp407.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp409;
_temp409.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp409.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp409;}); _temp407.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp408; _temp408.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp408.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp408;}); _temp407.lvalues= 0; _temp407;}); _LL148: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp410=( char*)"new_control_flow.cyc:289 env.access_path==null";
struct _tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 47; _temp411;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp412; _temp412.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp414;
_temp414.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp414.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp414;});
_temp412.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp413; _temp413.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp413.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp413;}); _temp412.lvalues= 0; _temp412;}); _LL150: goto _LL152; _LL152: goto
_LL154; _LL154: goto _LL156; _LL156: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL158:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL160:
_temp275= _temp269; goto _LL162; _LL162: _temp281= _temp275; goto _LL164; _LL164: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp281)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp415=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp415->hd=( void*) absop; _temp415->tl= 0; _temp415;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL166: { struct Cyc_NewControlFlow_AbsSyn
syn1= Cyc_NewControlFlow_abstract_exp( env, _temp289); struct Cyc_List_List* lhs=
syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list(): syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn
syn2= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, lhs),
_temp285); void* _temp418; void* _temp420; struct Cyc_NewControlFlow_AbsSynOne
_temp416= Cyc_NewControlFlow_meet_absexp( syn1); _LL421: _temp420=( void*)
_temp416.inner_exp; goto _LL419; _LL419: _temp418=( void*) _temp416.assigns;
goto _LL417; _LL417: { void* _temp424; void* _temp426; struct Cyc_NewControlFlow_AbsSynOne
_temp422= Cyc_NewControlFlow_meet_absexp( syn2); _LL427: _temp426=( void*)
_temp422.inner_exp; goto _LL425; _LL425: _temp424=( void*) _temp422.assigns;
goto _LL423; _LL423: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp420, _temp426),
_temp424); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp418);}}} _LL168:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp428=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp428->hd=( void*) _temp295; _temp428->tl=({ struct Cyc_List_List* _temp429=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp429->hd=(
void*) _temp291; _temp429->tl= 0; _temp429;}); _temp428;}), 0); _LL170: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp297, 0); _LL172: { int both_leaf=
1;{ void* _temp431=( void*)({ struct Cyc_Core_Opt* _temp430= _temp301->topt; if(
_temp430 == 0){ _throw( Null_Exception);} _temp430->v;}); struct Cyc_List_List*
_temp441; struct Cyc_Absyn_Structdecl** _temp443; struct Cyc_List_List* _temp445;
struct _tuple1* _temp447; struct Cyc_List_List* _temp449; _LL433: if((
unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_AnonStructType: 0){
_LL442: _temp441=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp431)->f1; goto _LL434;} else{ goto _LL435;} _LL435: if(( unsigned int)
_temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_StructType: 0){ _LL448: _temp447=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp431)->f1; goto
_LL446; _LL446: _temp445=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp431)->f2; goto _LL444; _LL444: _temp443=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp431)->f3; goto _LL436;} else{ goto
_LL437;} _LL437: if(( unsigned int) _temp431 > 4u?*(( int*) _temp431) == Cyc_Absyn_TupleType:
0){ _LL450: _temp449=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp431)->f1; goto _LL438;} else{ goto _LL439;} _LL439: goto _LL440; _LL434:
goto _LL436; _LL436: goto _LL438; _LL438: both_leaf= 0; goto _LL432; _LL440:
goto _LL432; _LL432:;}{ void* _temp452=( void*)({ struct Cyc_Core_Opt* _temp451=
e->topt; if( _temp451 == 0){ _throw( Null_Exception);} _temp451->v;}); struct
Cyc_List_List* _temp462; struct Cyc_Absyn_Structdecl** _temp464; struct Cyc_List_List*
_temp466; struct _tuple1* _temp468; struct Cyc_List_List* _temp470; _LL454: if((
unsigned int) _temp452 > 4u?*(( int*) _temp452) == Cyc_Absyn_AnonStructType: 0){
_LL463: _temp462=( struct Cyc_List_List*)(( struct Cyc_Absyn_AnonStructType_struct*)
_temp452)->f1; goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int)
_temp452 > 4u?*(( int*) _temp452) == Cyc_Absyn_StructType: 0){ _LL469: _temp468=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp452)->f1; goto
_LL467; _LL467: _temp466=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp452)->f2; goto _LL465; _LL465: _temp464=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp452)->f3; goto _LL457;} else{ goto
_LL458;} _LL458: if(( unsigned int) _temp452 > 4u?*(( int*) _temp452) == Cyc_Absyn_TupleType:
0){ _LL471: _temp470=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp452)->f1; goto _LL459;} else{ goto _LL460;} _LL460: goto _LL461; _LL455:
goto _LL457; _LL457: goto _LL459; _LL459: both_leaf= 0; goto _LL453; _LL461:
goto _LL453; _LL453:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp(
env, _temp301);} return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp472=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp472->hd=( void*) _temp301; _temp472->tl= 0; _temp472;}), env.lhsides);}
_LL174: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp473=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp473->hd=( void*) _temp307; _temp473->tl= _temp305; _temp473;}), 0); _LL176: {
void* _temp475= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp474=
_temp311->topt; if( _temp474 == 0){ _throw( Null_Exception);} _temp474->v;}));
struct Cyc_List_List* _temp481; _LL477: if(( unsigned int) _temp475 > 4u?*(( int*)
_temp475) == Cyc_Absyn_TupleType: 0){ _LL482: _temp481=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp475)->f1; goto _LL478;} else{ goto
_LL479;} _LL479: goto _LL480; _LL478: return Cyc_NewControlFlow_descend_path(
env, e, _temp311); _LL480: return Cyc_NewControlFlow_use_exp_unordered( env,({
struct Cyc_List_List* _temp483=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp483->hd=( void*) _temp311; _temp483->tl=({ struct
Cyc_List_List* _temp484=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp484->hd=( void*) _temp309; _temp484->tl= 0; _temp484;}); _temp483;}), Cyc_NewControlFlow_use_it_list());
_LL476:;} _LL178: { int is_malloc= 0;{ void* _temp485=( void*) _temp313->r;
struct Cyc_Absyn_Structdecl* _temp493; struct Cyc_List_List* _temp495; struct
Cyc_Core_Opt* _temp497; struct _tuple1* _temp499; struct Cyc_List_List* _temp501;
_LL487: if(*(( int*) _temp485) == Cyc_Absyn_Struct_e){ _LL500: _temp499=( struct
_tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp485)->f1; goto _LL498;
_LL498: _temp497=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp485)->f2; goto _LL496; _LL496: _temp495=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp485)->f3; goto _LL494; _LL494: _temp493=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp485)->f4;
goto _LL488;} else{ goto _LL489;} _LL489: if(*(( int*) _temp485) == Cyc_Absyn_Tuple_e){
_LL502: _temp501=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp485)->f1; goto _LL490;} else{ goto _LL491;} _LL491: goto _LL492; _LL488:
goto _LL490; _LL490: is_malloc= 1; goto _LL486; _LL492: goto _LL486; _LL486:;}
if( is_malloc){ _temp317= 0; _temp315= _temp313; goto _LL180;}{ struct Cyc_List_List*
_temp503= env.access_path; struct Cyc_List_List _temp515; struct Cyc_List_List*
_temp516; struct Cyc_Absyn_Exp* _temp518; struct Cyc_Absyn_Exp _temp520; struct
Cyc_Position_Segment* _temp521; void* _temp523; struct Cyc_Absyn_Exp* _temp525;
struct Cyc_Absyn_Exp* _temp527; struct Cyc_Core_Opt* _temp529; struct Cyc_List_List
_temp531; struct Cyc_List_List* _temp532; struct Cyc_Absyn_Exp* _temp534; struct
Cyc_Absyn_Exp _temp536; struct Cyc_Position_Segment* _temp537; void* _temp539;
struct Cyc_Absyn_Exp* _temp541; struct Cyc_Core_Opt* _temp543; struct Cyc_List_List
_temp545; struct Cyc_List_List* _temp546; struct Cyc_Absyn_Exp* _temp548; struct
Cyc_Absyn_Exp _temp550; struct Cyc_Position_Segment* _temp551; void* _temp553;
struct _tagged_string* _temp555; struct Cyc_Absyn_Exp* _temp557; struct Cyc_Core_Opt*
_temp559; _LL505: if( _temp503 == 0){ goto _LL506;} else{ goto _LL507;} _LL507:
if( _temp503 == 0){ goto _LL509;} else{ _temp515=* _temp503; _LL519: _temp518=(
struct Cyc_Absyn_Exp*) _temp515.hd; _temp520=* _temp518; _LL530: _temp529=(
struct Cyc_Core_Opt*) _temp520.topt; goto _LL524; _LL524: _temp523=( void*)
_temp520.r; if(*(( int*) _temp523) == Cyc_Absyn_Subscript_e){ _LL528: _temp527=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp523)->f1;
goto _LL526; _LL526: _temp525=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp523)->f2; goto _LL522;} else{ goto _LL509;} _LL522: _temp521=( struct Cyc_Position_Segment*)
_temp520.loc; goto _LL517; _LL517: _temp516=( struct Cyc_List_List*) _temp515.tl;
goto _LL508;} _LL509: if( _temp503 == 0){ goto _LL511;} else{ _temp531=*
_temp503; _LL535: _temp534=( struct Cyc_Absyn_Exp*) _temp531.hd; _temp536=*
_temp534; _LL544: _temp543=( struct Cyc_Core_Opt*) _temp536.topt; goto _LL540;
_LL540: _temp539=( void*) _temp536.r; if(*(( int*) _temp539) == Cyc_Absyn_Deref_e){
_LL542: _temp541=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp539)->f1; goto _LL538;} else{ goto _LL511;} _LL538: _temp537=( struct Cyc_Position_Segment*)
_temp536.loc; goto _LL533; _LL533: _temp532=( struct Cyc_List_List*) _temp531.tl;
goto _LL510;} _LL511: if( _temp503 == 0){ goto _LL513;} else{ _temp545=*
_temp503; _LL549: _temp548=( struct Cyc_Absyn_Exp*) _temp545.hd; _temp550=*
_temp548; _LL560: _temp559=( struct Cyc_Core_Opt*) _temp550.topt; goto _LL554;
_LL554: _temp553=( void*) _temp550.r; if(*(( int*) _temp553) == Cyc_Absyn_StructArrow_e){
_LL558: _temp557=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp553)->f1; goto _LL556; _LL556: _temp555=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp553)->f2; goto _LL552;} else{ goto _LL513;}
_LL552: _temp551=( struct Cyc_Position_Segment*) _temp550.loc; goto _LL547;
_LL547: _temp546=( struct Cyc_List_List*) _temp545.tl; goto _LL512;} _LL513:
goto _LL514; _LL506: env.access_path=({ struct Cyc_List_List* _temp561=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp561->hd=( void*)
e; _temp561->tl= env.access_path; _temp561;}); goto _LL504; _LL508: Cyc_Evexp_eval_const_uint_exp(
_temp525) == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp562=( char*)"new_control_flow.cyc:410 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp563; _temp563.curr= _temp562; _temp563.base= _temp562;
_temp563.last_plus_one= _temp562 + 61; _temp563;})); _temp532= _temp516; goto
_LL510; _LL510: env.access_path= _temp532; goto _LL504; _LL512: env.access_path=({
struct Cyc_List_List* _temp564=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp564->hd=( void*) Cyc_Absyn_structmember_exp(
_temp557, _temp555, 0); _temp564->tl= _temp546; _temp564;}); goto _LL504; _LL514:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp565=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp565[ 0]=({ struct
Cyc_Core_Impossible_struct _temp566; _temp566.tag= Cyc_Core_Impossible; _temp566.f1=(
struct _tagged_string)({ char* _temp567=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp568; _temp568.curr= _temp567; _temp568.base= _temp567;
_temp568.last_plus_one= _temp567 + 36; _temp568;}); _temp566;}); _temp565;}));
_LL504:;} return Cyc_NewControlFlow_abstract_exp( env, _temp313);} _LL180: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)({ struct Cyc_Core_Opt* _temp569= _temp315->topt; if( _temp569 ==
0){ _throw( Null_Exception);} _temp569->v;})); return Cyc_NewControlFlow_abstract_malloc(
env, e, _temp317,( struct Cyc_Absyn_Exp*) _temp315); _LL182: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp319); return Cyc_NewControlFlow_abstract_malloc( env, e, _temp321,
0); _LL184: { struct Cyc_List_List* _temp570= env.access_path; struct Cyc_List_List
_temp576; struct Cyc_List_List* _temp577; struct Cyc_Absyn_Exp* _temp579; struct
Cyc_Absyn_Exp _temp581; struct Cyc_Position_Segment* _temp582; void* _temp584;
struct Cyc_Absyn_Exp* _temp586; struct Cyc_Core_Opt* _temp588; _LL572: if(
_temp570 == 0){ goto _LL574;} else{ _temp576=* _temp570; _LL580: _temp579=(
struct Cyc_Absyn_Exp*) _temp576.hd; _temp581=* _temp579; _LL589: _temp588=(
struct Cyc_Core_Opt*) _temp581.topt; goto _LL585; _LL585: _temp584=( void*)
_temp581.r; if(*(( int*) _temp584) == Cyc_Absyn_Address_e){ _LL587: _temp586=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp584)->f1; goto
_LL583;} else{ goto _LL574;} _LL583: _temp582=( struct Cyc_Position_Segment*)
_temp581.loc; goto _LL578; _LL578: _temp577=( struct Cyc_List_List*) _temp576.tl;
goto _LL573;} _LL574: goto _LL575; _LL573: env.access_path= _temp577;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp323);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL575: return Cyc_NewControlFlow_descend_path( env, e, _temp323); _LL571:;}
_LL186: { void* _temp591= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp590= _temp327->topt; if( _temp590 == 0){ _throw( Null_Exception);} _temp590->v;}));
struct Cyc_Absyn_PtrInfo _temp597; struct Cyc_Absyn_Conref* _temp599; struct Cyc_Absyn_Tqual
_temp601; struct Cyc_Absyn_Conref* _temp603; void* _temp605; void* _temp607;
_LL593: if(( unsigned int) _temp591 > 4u?*(( int*) _temp591) == Cyc_Absyn_PointerType:
0){ _LL598: _temp597=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp591)->f1; _LL608: _temp607=( void*) _temp597.elt_typ; goto _LL606; _LL606:
_temp605=( void*) _temp597.rgn_typ; goto _LL604; _LL604: _temp603=( struct Cyc_Absyn_Conref*)
_temp597.nullable; goto _LL602; _LL602: _temp601=( struct Cyc_Absyn_Tqual)
_temp597.tq; goto _LL600; _LL600: _temp599=( struct Cyc_Absyn_Conref*) _temp597.bounds;
goto _LL594;} else{ goto _LL595;} _LL595: goto _LL596; _LL594:{ void* _temp609=
Cyc_Tcutil_compress( _temp607); struct Cyc_List_List* _temp621; struct Cyc_Absyn_Structdecl**
_temp623; struct Cyc_List_List* _temp625; struct _tuple1* _temp627; struct Cyc_List_List*
_temp629; struct Cyc_Absyn_Uniondecl** _temp631; struct Cyc_List_List* _temp633;
struct _tuple1* _temp635; _LL611: if(( unsigned int) _temp609 > 4u?*(( int*)
_temp609) == Cyc_Absyn_AnonStructType: 0){ _LL622: _temp621=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp609)->f1; goto _LL612;} else{ goto
_LL613;} _LL613: if(( unsigned int) _temp609 > 4u?*(( int*) _temp609) == Cyc_Absyn_StructType:
0){ _LL628: _temp627=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp609)->f1; goto _LL626; _LL626: _temp625=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp609)->f2; goto _LL624; _LL624: _temp623=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp609)->f3;
goto _LL614;} else{ goto _LL615;} _LL615: if(( unsigned int) _temp609 > 4u?*((
int*) _temp609) == Cyc_Absyn_AnonUnionType: 0){ _LL630: _temp629=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp609)->f1; goto _LL616;} else{ goto
_LL617;} _LL617: if(( unsigned int) _temp609 > 4u?*(( int*) _temp609) == Cyc_Absyn_UnionType:
0){ _LL636: _temp635=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp609)->f1; goto _LL634; _LL634: _temp633=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp609)->f2; goto _LL632; _LL632: _temp631=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp609)->f3;
goto _LL618;} else{ goto _LL619;} _LL619: goto _LL620; _LL612: goto _LL614;
_LL614: return Cyc_NewControlFlow_descend_path( env, e, _temp327); _LL616: goto
_LL618; _LL618: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp327); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL620: goto _LL610; _LL610:;} goto _LL596; _LL596:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp637=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp637[ 0]=({ struct
Cyc_Core_Impossible_struct _temp638; _temp638.tag= Cyc_Core_Impossible; _temp638.f1=(
struct _tagged_string)({ char* _temp639=( char*)"NewControlFlow: bad type in StructArrow";
struct _tagged_string _temp640; _temp640.curr= _temp639; _temp640.base= _temp639;
_temp640.last_plus_one= _temp639 + 40; _temp640;}); _temp638;}); _temp637;}));
_LL592:;} _LL188: { void* _temp642= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt*
_temp641= _temp331->topt; if( _temp641 == 0){ _throw( Null_Exception);} _temp641->v;}));
struct Cyc_List_List* _temp654; struct Cyc_Absyn_Structdecl** _temp656; struct
Cyc_List_List* _temp658; struct _tuple1* _temp660; struct Cyc_List_List*
_temp662; struct Cyc_Absyn_Uniondecl** _temp664; struct Cyc_List_List* _temp666;
struct _tuple1* _temp668; _LL644: if(( unsigned int) _temp642 > 4u?*(( int*)
_temp642) == Cyc_Absyn_AnonStructType: 0){ _LL655: _temp654=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonStructType_struct*) _temp642)->f1; goto _LL645;} else{ goto
_LL646;} _LL646: if(( unsigned int) _temp642 > 4u?*(( int*) _temp642) == Cyc_Absyn_StructType:
0){ _LL661: _temp660=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp642)->f1; goto _LL659; _LL659: _temp658=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp642)->f2; goto _LL657; _LL657: _temp656=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp642)->f3;
goto _LL647;} else{ goto _LL648;} _LL648: if(( unsigned int) _temp642 > 4u?*((
int*) _temp642) == Cyc_Absyn_AnonUnionType: 0){ _LL663: _temp662=( struct Cyc_List_List*)((
struct Cyc_Absyn_AnonUnionType_struct*) _temp642)->f1; goto _LL649;} else{ goto
_LL650;} _LL650: if(( unsigned int) _temp642 > 4u?*(( int*) _temp642) == Cyc_Absyn_UnionType:
0){ _LL669: _temp668=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp642)->f1; goto _LL667; _LL667: _temp666=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp642)->f2; goto _LL665; _LL665: _temp664=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp642)->f3;
goto _LL651;} else{ goto _LL652;} _LL652: goto _LL653; _LL645: goto _LL647;
_LL647: return Cyc_NewControlFlow_descend_path( env, e, _temp331); _LL649: goto
_LL651; _LL651: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp331); syn1.lvalues= Cyc_NewControlFlow_use_it_list(); return syn1;}
_LL653:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp670=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp670[ 0]=({ struct Cyc_Core_Impossible_struct _temp671; _temp671.tag= Cyc_Core_Impossible;
_temp671.f1=( struct _tagged_string)({ char* _temp672=( char*)"NewControlFlow: bad type in StructMember";
struct _tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 41; _temp673;}); _temp671;}); _temp670;}));
_LL643:;} _LL190: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg))
Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp674=( char*)"new_control_flow.cyc:480 env.access_path == null";
struct _tagged_string _temp675; _temp675.curr= _temp674; _temp675.base= _temp674;
_temp675.last_plus_one= _temp674 + 49; _temp675;})); if( _temp337 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));} _temp343=
_temp337; goto _LL192; _LL192: { struct Cyc_List_List* _temp676= env.access_path;
struct Cyc_List_List _temp684; struct Cyc_List_List* _temp685; struct Cyc_Absyn_Exp*
_temp687; struct Cyc_Absyn_Exp _temp689; struct Cyc_Position_Segment* _temp690;
void* _temp692; struct Cyc_Absyn_Exp* _temp694; struct Cyc_Absyn_Exp* _temp696;
struct Cyc_Core_Opt* _temp698; _LL678: if( _temp676 == 0){ goto _LL679;} else{
goto _LL680;} _LL680: if( _temp676 == 0){ goto _LL682;} else{ _temp684=*
_temp676; _LL688: _temp687=( struct Cyc_Absyn_Exp*) _temp684.hd; _temp689=*
_temp687; _LL699: _temp698=( struct Cyc_Core_Opt*) _temp689.topt; goto _LL693;
_LL693: _temp692=( void*) _temp689.r; if(*(( int*) _temp692) == Cyc_Absyn_Subscript_e){
_LL697: _temp696=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp692)->f1; goto _LL695; _LL695: _temp694=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp692)->f2; goto _LL691;} else{ goto _LL682;}
_LL691: _temp690=( struct Cyc_Position_Segment*) _temp689.loc; goto _LL686;
_LL686: _temp685=( struct Cyc_List_List*) _temp684.tl; goto _LL681;} _LL682:
goto _LL683; _LL679: { void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp=
Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp343 != 0; _temp343=({ struct
Cyc_List_List* _temp700= _temp343; if( _temp700 == 0){ _throw( Null_Exception);}
_temp700->tl;}), ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp701= ls; if(
_temp701 == 0){ _throw( Null_Exception);} _temp701->tl;})){ if( Cyc_CfAbsexp_isUnknownOp((
void*)({ struct Cyc_List_List* _temp702= ls; if( _temp702 == 0){ _throw(
Null_Exception);} _temp702->hd;}))){ ith_lvals=({ struct Cyc_List_List* _temp703=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp703->hd=(
void*) Cyc_CfAbsexp_mkUnknownOp(); _temp703->tl= ith_lvals; _temp703;});} else{
ith_lvals=({ struct Cyc_List_List* _temp704=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp704->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp705= ls; if( _temp705 == 0){ _throw(
Null_Exception);} _temp705->hd;}),( void*)({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp706=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp706[ 0]=({ struct Cyc_CfFlowInfo_TupleF_struct _temp707; _temp707.tag= Cyc_CfFlowInfo_TupleF;
_temp707.f1= j; _temp707;}); _temp706;})); _temp704->tl= ith_lvals; _temp704;});}}}{
struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, ith_lvals),( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp714=
_temp343; if( _temp714 == 0){ _throw( Null_Exception);} _temp714->hd;})); void*
_temp710; void* _temp712; struct Cyc_NewControlFlow_AbsSynOne _temp708= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL713: _temp712=( void*) _temp708.inner_exp; goto _LL711; _LL711:
_temp710=( void*) _temp708.assigns; goto _LL709; _LL709: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp712); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp710);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL681: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp694); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp685;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp343 != 0;
_temp343=({ struct Cyc_List_List* _temp715= _temp343; if( _temp715 == 0){ _throw(
Null_Exception);} _temp715->tl;}), ++ j){ if( i == j){ void* _temp719; void*
_temp721; struct Cyc_NewControlFlow_AbsSynOne _temp717= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp716= _temp343; if( _temp716 == 0){ _throw( Null_Exception);} _temp716->hd;})));
_LL722: _temp721=( void*) _temp717.inner_exp; goto _LL720; _LL720: _temp719=(
void*) _temp717.assigns; goto _LL718; _LL718: assignexp= _temp719; ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp721);} else{ void* _temp726; void*
_temp728; struct Cyc_NewControlFlow_AbsSynOne _temp724= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List*
_temp723= _temp343; if( _temp723 == 0){ _throw( Null_Exception);} _temp723->hd;})));
_LL729: _temp728=( void*) _temp724.inner_exp; goto _LL727; _LL727: _temp726=(
void*) _temp724.assigns; goto _LL725; _LL725: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp728);}}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp,({ struct Cyc_List_List* _temp730=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp730->hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp730->tl= 0; _temp730;}), assignexp);}} _LL683:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp731=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp731[ 0]=({ struct
Cyc_Core_Impossible_struct _temp732; _temp732.tag= Cyc_Core_Impossible; _temp732.f1=(
struct _tagged_string)({ char* _temp733=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp734; _temp734.curr= _temp733; _temp734.base= _temp733;
_temp734.last_plus_one= _temp733 + 46; _temp734;}); _temp732;}); _temp731;}));
_LL677:;} _LL194: _temp351= _temp345; goto _LL196; _LL196: { struct Cyc_List_List*
_temp735= env.access_path; struct Cyc_List_List _temp743; struct Cyc_List_List*
_temp744; struct Cyc_Absyn_Exp* _temp746; struct Cyc_Absyn_Exp _temp748; struct
Cyc_Position_Segment* _temp749; void* _temp751; struct _tagged_string* _temp753;
struct Cyc_Absyn_Exp* _temp755; struct Cyc_Core_Opt* _temp757; _LL737: if(
_temp735 == 0){ goto _LL738;} else{ goto _LL739;} _LL739: if( _temp735 == 0){
goto _LL741;} else{ _temp743=* _temp735; _LL747: _temp746=( struct Cyc_Absyn_Exp*)
_temp743.hd; _temp748=* _temp746; _LL758: _temp757=( struct Cyc_Core_Opt*)
_temp748.topt; goto _LL752; _LL752: _temp751=( void*) _temp748.r; if(*(( int*)
_temp751) == Cyc_Absyn_StructMember_e){ _LL756: _temp755=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp751)->f1; goto _LL754; _LL754:
_temp753=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp751)->f2; goto _LL750;} else{ goto _LL741;} _LL750: _temp749=( struct Cyc_Position_Segment*)
_temp748.loc; goto _LL745; _LL745: _temp744=( struct Cyc_List_List*) _temp743.tl;
goto _LL740;} _LL741: goto _LL742; _LL738: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp351 != 0; _temp351=({
struct Cyc_List_List* _temp759= _temp351; if( _temp759 == 0){ _throw(
Null_Exception);} _temp759->tl;})){ struct Cyc_List_List* ith_lvals= 0;{ struct
Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls=({ struct Cyc_List_List*
_temp760= ls; if( _temp760 == 0){ _throw( Null_Exception);} _temp760->tl;})){
if( Cyc_CfAbsexp_isUnknownOp(( void*)({ struct Cyc_List_List* _temp761= ls; if(
_temp761 == 0){ _throw( Null_Exception);} _temp761->hd;}))){ ith_lvals=({ struct
Cyc_List_List* _temp762=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp762->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp762->tl= ith_lvals;
_temp762;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)({ struct Cyc_List_List*
_temp783= _temp351; if( _temp783 == 0){ _throw( Null_Exception);} _temp783->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp763= ds; if( _temp763 == 0){
_throw( Null_Exception);} _temp763->tl;})){ void* _temp765=( void*)({ struct Cyc_List_List*
_temp764= ds; if( _temp764 == 0){ _throw( Null_Exception);} _temp764->hd;});
struct Cyc_Absyn_Exp* _temp771; struct _tagged_string* _temp773; _LL767: if(*((
int*) _temp765) == Cyc_Absyn_ArrayElement){ _LL772: _temp771=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp765)->f1; goto _LL768;} else{ goto
_LL769;} _LL769: if(*(( int*) _temp765) == Cyc_Absyn_FieldName){ _LL774:
_temp773=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp765)->f1; goto _LL770;} else{ goto _LL766;} _LL768:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp775=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp775[ 0]=({ struct
Cyc_Core_Impossible_struct _temp776; _temp776.tag= Cyc_Core_Impossible; _temp776.f1=(
struct _tagged_string)({ char* _temp777=( char*)"bad struct designator"; struct
_tagged_string _temp778; _temp778.curr= _temp777; _temp778.base= _temp777;
_temp778.last_plus_one= _temp777 + 22; _temp778;}); _temp776;}); _temp775;}));
_LL770: ith_lvals=({ struct Cyc_List_List* _temp779=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp779->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)({ struct Cyc_List_List* _temp780= ls; if( _temp780 == 0){ _throw(
Null_Exception);} _temp780->hd;}),( void*)({ struct Cyc_CfFlowInfo_StructF_struct*
_temp781=( struct Cyc_CfFlowInfo_StructF_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct));
_temp781[ 0]=({ struct Cyc_CfFlowInfo_StructF_struct _temp782; _temp782.tag= Cyc_CfFlowInfo_StructF;
_temp782.f1= _temp773; _temp782;}); _temp781;})); _temp779->tl= ith_lvals;
_temp779;}); goto _LL766; _LL766:;}}}}{ void** ith_topt=( void**)(( void*)({
struct Cyc_Core_Opt* _temp792=((*(( struct _tuple4*)({ struct Cyc_List_List*
_temp791= _temp351; if( _temp791 == 0){ _throw( Null_Exception);} _temp791->hd;}))).f2)->topt;
if( _temp792 == 0){ _throw( Null_Exception);}& _temp792->v;})); struct Cyc_NewControlFlow_AbsSyn
ith_syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
ith_lvals),(*(( struct _tuple4*)({ struct Cyc_List_List* _temp790= _temp351; if(
_temp790 == 0){ _throw( Null_Exception);} _temp790->hd;}))).f2); void* _temp786;
void* _temp788; struct Cyc_NewControlFlow_AbsSynOne _temp784= Cyc_NewControlFlow_meet_absexp(
ith_syn); _LL789: _temp788=( void*) _temp784.inner_exp; goto _LL787; _LL787:
_temp786=( void*) _temp784.assigns; goto _LL785; _LL785: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp788); assignexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), assignexp, _temp786);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);} _LL740: { struct Cyc_NewControlFlow_AbsEnv other_env=
Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp744;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp351 != 0; _temp351=({ struct Cyc_List_List* _temp793= _temp351; if(
_temp793 == 0){ _throw( Null_Exception);} _temp793->tl;})){ int used= 0;{ struct
Cyc_List_List* ds=(*(( struct _tuple4*)({ struct Cyc_List_List* _temp810=
_temp351; if( _temp810 == 0){ _throw( Null_Exception);} _temp810->hd;}))).f1;
for( 0; ds != 0; ds=({ struct Cyc_List_List* _temp794= ds; if( _temp794 == 0){
_throw( Null_Exception);} _temp794->tl;})){ void* _temp796=( void*)({ struct Cyc_List_List*
_temp795= ds; if( _temp795 == 0){ _throw( Null_Exception);} _temp795->hd;});
struct Cyc_Absyn_Exp* _temp802; struct _tagged_string* _temp804; _LL798: if(*((
int*) _temp796) == Cyc_Absyn_ArrayElement){ _LL803: _temp802=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp796)->f1; goto _LL799;} else{ goto
_LL800;} _LL800: if(*(( int*) _temp796) == Cyc_Absyn_FieldName){ _LL805:
_temp804=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp796)->f1; goto _LL801;} else{ goto _LL797;} _LL799:( void) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp806=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp806[ 0]=({ struct
Cyc_Core_Impossible_struct _temp807; _temp807.tag= Cyc_Core_Impossible; _temp807.f1=(
struct _tagged_string)({ char* _temp808=( char*)"bad struct designator"; struct
_tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 22; _temp809;}); _temp807;}); _temp806;}));
_LL801: if( Cyc_String_zstrptrcmp( _temp753, _temp804) == 0){ used= 1;} goto
_LL797; _LL797:;}} if( used){ void* _temp814; void* _temp816; struct Cyc_NewControlFlow_AbsSynOne
_temp812= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,(*((
struct _tuple4*)({ struct Cyc_List_List* _temp811= _temp351; if( _temp811 == 0){
_throw( Null_Exception);} _temp811->hd;}))).f2)); _LL817: _temp816=( void*)
_temp812.inner_exp; goto _LL815; _LL815: _temp814=( void*) _temp812.assigns;
goto _LL813; _LL813: assignexp= _temp814; ansexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, ansexp, _temp816);} else{ void* _temp821; void*
_temp823; struct Cyc_NewControlFlow_AbsSynOne _temp819= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( other_env,(*(( struct _tuple4*)({ struct Cyc_List_List*
_temp818= _temp351; if( _temp818 == 0){ _throw( Null_Exception);} _temp818->hd;}))).f2));
_LL824: _temp823=( void*) _temp819.inner_exp; goto _LL822; _LL822: _temp821=(
void*) _temp819.assigns; goto _LL820; _LL820: ansexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, ansexp, _temp823);}} return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, assignexp);}} _LL742:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp825=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp825[ 0]=({ struct Cyc_Core_Impossible_struct _temp826; _temp826.tag= Cyc_Core_Impossible;
_temp826.f1=( struct _tagged_string)({ char* _temp827=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp828; _temp828.curr= _temp827; _temp828.base= _temp827;
_temp828.last_plus_one= _temp827 + 47; _temp828;}); _temp826;}); _temp825;}));
_LL736:;} _LL198: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd, _temp357), 0); _LL200:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp829=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp829->hd=( void*) _temp361; _temp829->tl= 0; _temp829;}), 0); _LL202: {
struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp363); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp( esyn)).inner_exp,
Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());} _LL204: { struct Cyc_List_List*
_temp832; struct Cyc_NewControlFlow_AbsSynOne _temp834; struct Cyc_NewControlFlow_AbsSynOne
_temp836; struct Cyc_NewControlFlow_AbsSyn _temp830= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp369); _LL837: _temp836=( struct Cyc_NewControlFlow_AbsSynOne)
_temp830.when_true; goto _LL835; _LL835: _temp834=( struct Cyc_NewControlFlow_AbsSynOne)
_temp830.when_false; goto _LL833; _LL833: _temp832=( struct Cyc_List_List*)
_temp830.lvalues; goto _LL831; _LL831: { struct Cyc_List_List* _temp840; struct
Cyc_NewControlFlow_AbsSynOne _temp842; struct Cyc_NewControlFlow_AbsSynOne
_temp844; struct Cyc_NewControlFlow_AbsSyn _temp838= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp367); _LL845: _temp844=( struct Cyc_NewControlFlow_AbsSynOne)
_temp838.when_true; goto _LL843; _LL843: _temp842=( struct Cyc_NewControlFlow_AbsSynOne)
_temp838.when_false; goto _LL841; _LL841: _temp840=( struct Cyc_List_List*)
_temp838.lvalues; goto _LL839; _LL839: { struct Cyc_List_List* _temp848; struct
Cyc_NewControlFlow_AbsSynOne _temp850; struct Cyc_NewControlFlow_AbsSynOne
_temp852; struct Cyc_NewControlFlow_AbsSyn _temp846= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp365); _LL853: _temp852=( struct Cyc_NewControlFlow_AbsSynOne)
_temp846.when_true; goto _LL851; _LL851: _temp850=( struct Cyc_NewControlFlow_AbsSynOne)
_temp846.when_false; goto _LL849; _LL849: _temp848=( struct Cyc_List_List*)
_temp846.lvalues; goto _LL847; _LL847: { void* true_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp836.inner_exp,(
void*) _temp836.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp844.inner_exp,( void*) _temp844.assigns)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp834.inner_exp,( void*) _temp834.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp852.inner_exp,( void*) _temp852.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp836.inner_exp,( void*) _temp836.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp842.inner_exp,( void*) _temp842.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp834.inner_exp,( void*) _temp834.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp850.inner_exp,(
void*) _temp850.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn _temp854;
_temp854.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp856; _temp856.inner_exp=(
void*) true_exp; _temp856.assigns=( void*) Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()); _temp856;}); _temp854.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp855; _temp855.inner_exp=( void*) false_exp; _temp855.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp855;}); _temp854.lvalues= 0; _temp854;});}}}}
_LL206: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp371);{
struct Cyc_Absyn_Stmt* last_s= _temp371; while( 1) { void* _temp857=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp867; struct Cyc_Absyn_Stmt* _temp869;
struct Cyc_Absyn_Stmt* _temp871; struct Cyc_Absyn_Decl* _temp873; struct Cyc_Absyn_Exp*
_temp875; _LL859: if(( unsigned int) _temp857 > 1u?*(( int*) _temp857) == Cyc_Absyn_Seq_s:
0){ _LL870: _temp869=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp857)->f1; goto _LL868; _LL868: _temp867=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp857)->f2; goto _LL860;} else{ goto _LL861;} _LL861:
if(( unsigned int) _temp857 > 1u?*(( int*) _temp857) == Cyc_Absyn_Decl_s: 0){
_LL874: _temp873=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp857)->f1; goto _LL872; _LL872: _temp871=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp857)->f2; goto _LL862;} else{ goto _LL863;}
_LL863: if(( unsigned int) _temp857 > 1u?*(( int*) _temp857) == Cyc_Absyn_Exp_s:
0){ _LL876: _temp875=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp857)->f1; goto _LL864;} else{ goto _LL865;} _LL865: goto _LL866; _LL860:
last_s= _temp867; continue; _LL862: last_s= _temp871; continue; _LL864: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp875); void* _temp879; void* _temp881; struct Cyc_NewControlFlow_AbsSynOne
_temp877= Cyc_NewControlFlow_meet_absexp( syn); _LL882: _temp881=( void*)
_temp877.inner_exp; goto _LL880; _LL880: _temp879=( void*) _temp877.assigns;
goto _LL878; _LL878:( void*)({ struct Cyc_List_List* _temp883=( Cyc_NewControlFlow_get_stmt_annot(
last_s))->absexps; if( _temp883 == 0){ _throw( Null_Exception);} _temp883->hd=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp881, _temp879);});
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp371), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL866:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp884=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp884[ 0]=({ struct Cyc_Core_Impossible_struct _temp885; _temp885.tag= Cyc_Core_Impossible;
_temp885.f1=( struct _tagged_string)({ char* _temp886=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp887; _temp887.curr= _temp886; _temp887.base= _temp886;
_temp887.last_plus_one= _temp886 + 33; _temp887;}); _temp885;}); _temp884;}));
_LL858:;}}} _LL208: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp375); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp373); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp888; _temp888.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp890; _temp890.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp890.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp890;});
_temp888.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp889; _temp889.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp889.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp889;}); _temp888.lvalues= 0; _temp888;});}} _LL210: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp891=( char*)"new_control_flow.cyc:700 env.access_path==null";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 47; _temp892;})); Cyc_NewControlFlow_add_var_root(
env, _temp381);( env.shared)->comprehension_vars=({ struct Cyc_List_List*
_temp893=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp893->hd=( void*) _temp381; _temp893->tl=( env.shared)->comprehension_vars;
_temp893;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp896; void* _temp898; struct Cyc_NewControlFlow_AbsSynOne
_temp894= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp379)); _LL899: _temp898=( void*) _temp894.inner_exp; goto _LL897;
_LL897: _temp896=( void*) _temp894.assigns; goto _LL895; _LL895: { void*
_temp902; void* _temp904; struct Cyc_NewControlFlow_AbsSynOne _temp900= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp377)); _LL905: _temp904=( void*)
_temp900.inner_exp; goto _LL903; _LL903: _temp902=( void*) _temp900.assigns;
goto _LL901; _LL901: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp898, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp896, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp(
_temp381), Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp904, _temp902), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL212: goto _LL214; _LL214: goto _LL216;
_LL216: goto _LL218; _LL218: goto _LL220; _LL220: goto _LL222; _LL222: goto
_LL224; _LL224:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp906=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp906[ 0]=({ struct Cyc_Core_Impossible_struct _temp907; _temp907.tag= Cyc_Core_Impossible;
_temp907.f1=( struct _tagged_string)({ char* _temp908=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp909; _temp909.curr= _temp908; _temp909.base= _temp908;
_temp909.last_plus_one= _temp908 + 34; _temp909;}); _temp907;}); _temp906;}));
_LL138:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp912; void* _temp914; struct Cyc_NewControlFlow_AbsSynOne
_temp910= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL915: _temp914=( void*) _temp910.inner_exp; goto _LL913; _LL913: _temp912=(
void*) _temp910.assigns; goto _LL911; _LL911: return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp914, _temp912);} struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp916; _temp916.f1= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns);
_temp916.f2= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp916;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp917= scs; if(
_temp917 == 0){ _throw( Null_Exception);} _temp917->tl;})){ struct Cyc_Absyn_Switch_clause
_temp921; struct Cyc_Position_Segment* _temp922; struct Cyc_Absyn_Stmt* _temp924;
struct Cyc_Absyn_Exp* _temp926; struct Cyc_Core_Opt* _temp928; struct Cyc_Absyn_Pat*
_temp930; struct Cyc_Absyn_Switch_clause* _temp919=( struct Cyc_Absyn_Switch_clause*)({
struct Cyc_List_List* _temp918= scs; if( _temp918 == 0){ _throw( Null_Exception);}
_temp918->hd;}); _temp921=* _temp919; _LL931: _temp930=( struct Cyc_Absyn_Pat*)
_temp921.pattern; goto _LL929; _LL929: _temp928=( struct Cyc_Core_Opt*) _temp921.pat_vars;
goto _LL927; _LL927: _temp926=( struct Cyc_Absyn_Exp*) _temp921.where_clause;
goto _LL925; _LL925: _temp924=( struct Cyc_Absyn_Stmt*) _temp921.body; goto
_LL923; _LL923: _temp922=( struct Cyc_Position_Segment*) _temp921.loc; goto
_LL920; _LL920: if( _temp928 == 0){( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp932=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp932[ 0]=({ struct Cyc_Core_Impossible_struct _temp933; _temp933.tag= Cyc_Core_Impossible;
_temp933.f1=( struct _tagged_string)({ char* _temp934=( char*)"switch clause vds not set";
struct _tagged_string _temp935; _temp935.curr= _temp934; _temp935.base= _temp934;
_temp935.last_plus_one= _temp934 + 26; _temp935;}); _temp933;}); _temp932;}));}{
struct Cyc_List_List* vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp938= _temp928; if( _temp938 == 0){ _throw( Null_Exception);} _temp938->v;});
for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp936= vds; if( _temp936 == 0){
_throw( Null_Exception);} _temp936->tl;})){ Cyc_NewControlFlow_add_var_root( env,(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp937= vds; if( _temp937
== 0){ _throw( Null_Exception);} _temp937->hd;}));}} if( _temp926 != 0){ void*
_temp942; void* _temp944; struct _tuple0 _temp940= Cyc_NewControlFlow_abstract_guard(
env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp939= _temp926; if(
_temp939 == 0){ _throw( Null_Exception);} _temp939;})); _LL945: _temp944=
_temp940.f1; goto _LL943; _LL943: _temp942= _temp940.f2; goto _LL941; _LL941:
ans=({ struct Cyc_List_List* _temp946=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp946->hd=( void*) _temp944; _temp946->tl=({
struct Cyc_List_List* _temp947=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp947->hd=( void*) _temp942; _temp947->tl= ans;
_temp947;}); _temp946;});} Cyc_NewControlFlow_cf_prepass( env, _temp924);}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp948=( char*)"new_control_flow.cyc:752 env.lhsides == null";
struct _tagged_string _temp949; _temp949.curr= _temp948; _temp949.base= _temp948;
_temp949.last_plus_one= _temp948 + 45; _temp949;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp950=( char*)"new_control_flow.cyc:753 env.access_path == null";
struct _tagged_string _temp951; _temp951.curr= _temp950; _temp951.base= _temp950;
_temp951.last_plus_one= _temp950 + 49; _temp951;}));{ void* _temp952=( void*) s->r;
struct Cyc_Absyn_Exp* _temp1000; struct Cyc_Absyn_Stmt* _temp1002; struct Cyc_Absyn_Stmt*
_temp1004; struct Cyc_Absyn_Exp* _temp1006; struct Cyc_Absyn_Exp* _temp1008;
struct Cyc_Absyn_Stmt* _temp1010; struct Cyc_Absyn_Stmt* _temp1012; struct Cyc_Absyn_Exp*
_temp1014; struct _tuple3 _temp1016; struct Cyc_Absyn_Stmt* _temp1018; struct
Cyc_Absyn_Exp* _temp1020; struct Cyc_Absyn_Stmt* _temp1022; struct Cyc_Absyn_Stmt*
_temp1024; struct _tuple3 _temp1026; struct Cyc_Absyn_Stmt* _temp1028; struct
Cyc_Absyn_Exp* _temp1030; struct Cyc_Absyn_Stmt* _temp1032; struct Cyc_Absyn_Stmt*
_temp1034; struct Cyc_Absyn_Stmt* _temp1036; struct _tagged_string* _temp1038;
struct Cyc_Absyn_Stmt* _temp1040; struct _tagged_string* _temp1042; struct Cyc_Absyn_Stmt*
_temp1044; struct _tuple3 _temp1046; struct Cyc_Absyn_Stmt* _temp1048; struct
Cyc_Absyn_Exp* _temp1050; struct _tuple3 _temp1052; struct Cyc_Absyn_Stmt*
_temp1054; struct Cyc_Absyn_Exp* _temp1056; struct Cyc_Absyn_Exp* _temp1058;
struct Cyc_List_List* _temp1060; struct Cyc_Absyn_Exp* _temp1062; struct Cyc_Absyn_Switch_clause**
_temp1064; struct Cyc_List_List* _temp1066; struct Cyc_Absyn_Stmt* _temp1068;
struct Cyc_Absyn_Decl* _temp1070; struct Cyc_Absyn_Decl _temp1072; struct Cyc_Position_Segment*
_temp1073; void* _temp1075; struct Cyc_Absyn_Vardecl* _temp1077; struct Cyc_Absyn_Stmt*
_temp1079; struct Cyc_Absyn_Decl* _temp1081; struct Cyc_Absyn_Decl _temp1083;
struct Cyc_Position_Segment* _temp1084; void* _temp1086; int _temp1088; struct
Cyc_Absyn_Exp* _temp1090; struct Cyc_Core_Opt* _temp1092; struct Cyc_Core_Opt*
_temp1094; struct Cyc_Core_Opt _temp1096; struct Cyc_List_List* _temp1097;
struct Cyc_Absyn_Pat* _temp1099; struct Cyc_Absyn_Stmt* _temp1101; struct Cyc_Absyn_Decl*
_temp1103; struct Cyc_Absyn_Stmt* _temp1105; struct Cyc_Absyn_Stmt* _temp1107;
struct Cyc_Absyn_Stmt* _temp1109; struct _tagged_string* _temp1111; struct Cyc_List_List*
_temp1113; struct Cyc_Absyn_Stmt* _temp1115; struct Cyc_Absyn_Stmt* _temp1117;
struct Cyc_Absyn_Vardecl* _temp1119; struct Cyc_Absyn_Tvar* _temp1121; _LL954:
if( _temp952 ==( void*) Cyc_Absyn_Skip_s){ goto _LL955;} else{ goto _LL956;}
_LL956: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Exp_s:
0){ _LL1001: _temp1000=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp952)->f1; goto _LL957;} else{ goto _LL958;} _LL958: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Seq_s: 0){ _LL1005: _temp1004=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp952)->f1; goto
_LL1003; _LL1003: _temp1002=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp952)->f2; goto _LL959;} else{ goto _LL960;} _LL960: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Return_s: 0){ _LL1007: _temp1006=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp952)->f1; if(
_temp1006 == 0){ goto _LL961;} else{ goto _LL962;}} else{ goto _LL962;} _LL962:
if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Return_s: 0){
_LL1009: _temp1008=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp952)->f1; goto _LL963;} else{ goto _LL964;} _LL964: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_IfThenElse_s: 0){ _LL1015:
_temp1014=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp952)->f1; goto _LL1013; _LL1013: _temp1012=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp952)->f2; goto _LL1011; _LL1011:
_temp1010=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp952)->f3; goto _LL965;} else{ goto _LL966;} _LL966: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Do_s: 0){ _LL1023: _temp1022=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp952)->f1; goto
_LL1017; _LL1017: _temp1016=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp952)->f2; _LL1021: _temp1020= _temp1016.f1; goto _LL1019; _LL1019:
_temp1018= _temp1016.f2; goto _LL967;} else{ goto _LL968;} _LL968: if((
unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_While_s: 0){
_LL1027: _temp1026=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp952)->f1; _LL1031: _temp1030= _temp1026.f1; goto _LL1029; _LL1029:
_temp1028= _temp1026.f2; goto _LL1025; _LL1025: _temp1024=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp952)->f2; goto _LL969;} else{ goto _LL970;}
_LL970: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Break_s:
0){ _LL1033: _temp1032=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp952)->f1; goto _LL971;} else{ goto _LL972;} _LL972: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Continue_s: 0){ _LL1035:
_temp1034=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp952)->f1; goto _LL973;} else{ goto _LL974;} _LL974: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Goto_s: 0){ _LL1039: _temp1038=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp952)->f1; goto
_LL1037; _LL1037: _temp1036=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp952)->f2; if( _temp1036 == 0){ goto _LL975;} else{ goto _LL976;}} else{
goto _LL976;} _LL976: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) ==
Cyc_Absyn_Goto_s: 0){ _LL1043: _temp1042=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp952)->f1; goto _LL1041; _LL1041: _temp1040=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp952)->f2; goto _LL977;} else{ goto _LL978;}
_LL978: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_For_s:
0){ _LL1059: _temp1058=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp952)->f1; goto _LL1053; _LL1053: _temp1052=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp952)->f2; _LL1057: _temp1056= _temp1052.f1; goto _LL1055; _LL1055:
_temp1054= _temp1052.f2; goto _LL1047; _LL1047: _temp1046=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp952)->f3; _LL1051: _temp1050= _temp1046.f1;
goto _LL1049; _LL1049: _temp1048= _temp1046.f2; goto _LL1045; _LL1045: _temp1044=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp952)->f4; goto
_LL979;} else{ goto _LL980;} _LL980: if(( unsigned int) _temp952 > 1u?*(( int*)
_temp952) == Cyc_Absyn_Switch_s: 0){ _LL1063: _temp1062=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp952)->f1; goto _LL1061; _LL1061:
_temp1060=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp952)->f2;
goto _LL981;} else{ goto _LL982;} _LL982: if(( unsigned int) _temp952 > 1u?*((
int*) _temp952) == Cyc_Absyn_Fallthru_s: 0){ _LL1067: _temp1066=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp952)->f1; goto _LL1065; _LL1065:
_temp1064=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp952)->f2; goto _LL983;} else{ goto _LL984;} _LL984: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Decl_s: 0){ _LL1071: _temp1070=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp952)->f1;
_temp1072=* _temp1070; _LL1076: _temp1075=( void*) _temp1072.r; if(*(( int*)
_temp1075) == Cyc_Absyn_Var_d){ _LL1078: _temp1077=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1075)->f1; goto _LL1074;} else{ goto _LL986;}
_LL1074: _temp1073=( struct Cyc_Position_Segment*) _temp1072.loc; goto _LL1069;
_LL1069: _temp1068=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp952)->f2; goto _LL985;} else{ goto _LL986;} _LL986: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Decl_s: 0){ _LL1082: _temp1081=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp952)->f1;
_temp1083=* _temp1081; _LL1087: _temp1086=( void*) _temp1083.r; if(*(( int*)
_temp1086) == Cyc_Absyn_Let_d){ _LL1100: _temp1099=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1086)->f1; goto _LL1095; _LL1095: _temp1094=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1086)->f2; if(
_temp1094 == 0){ goto _LL988;} else{ _temp1096=* _temp1094; _LL1098: _temp1097=(
struct Cyc_List_List*) _temp1096.v; goto _LL1093;} _LL1093: _temp1092=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1086)->f3; goto _LL1091;
_LL1091: _temp1090=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1086)->f4; goto _LL1089; _LL1089: _temp1088=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1086)->f5; goto _LL1085;} else{ goto _LL988;} _LL1085: _temp1084=( struct
Cyc_Position_Segment*) _temp1083.loc; goto _LL1080; _LL1080: _temp1079=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp952)->f2; goto _LL987;}
else{ goto _LL988;} _LL988: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952)
== Cyc_Absyn_Decl_s: 0){ _LL1104: _temp1103=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp952)->f1; goto _LL1102; _LL1102: _temp1101=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp952)->f2; goto
_LL989;} else{ goto _LL990;} _LL990: if(( unsigned int) _temp952 > 1u?*(( int*)
_temp952) == Cyc_Absyn_Cut_s: 0){ _LL1106: _temp1105=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp952)->f1; goto _LL991;} else{ goto _LL992;}
_LL992: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Splice_s:
0){ _LL1108: _temp1107=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp952)->f1; goto _LL993;} else{ goto _LL994;} _LL994: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Label_s: 0){ _LL1112: _temp1111=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp952)->f1; goto
_LL1110; _LL1110: _temp1109=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp952)->f2; goto _LL995;} else{ goto _LL996;} _LL996: if(( unsigned int)
_temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_TryCatch_s: 0){ _LL1116:
_temp1115=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp952)->f1; goto _LL1114; _LL1114: _temp1113=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp952)->f2; goto _LL997;} else{ goto
_LL998;} _LL998: if(( unsigned int) _temp952 > 1u?*(( int*) _temp952) == Cyc_Absyn_Region_s:
0){ _LL1122: _temp1121=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp952)->f1; goto _LL1120; _LL1120: _temp1119=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp952)->f2; goto _LL1118; _LL1118:
_temp1117=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp952)->f3; goto _LL999;} else{ goto _LL953;} _LL955: return; _LL957: { void*
ae= Cyc_NewControlFlow_abstract_exp_top( env, _temp1000); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp1123=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1123->hd=( void*) ae; _temp1123->tl= 0; _temp1123;}));
return;} _LL959: Cyc_NewControlFlow_cf_prepass( env, _temp1004); Cyc_NewControlFlow_cf_prepass(
env, _temp1002); return; _LL961: return; _LL963: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1125= _temp1008; if( _temp1125 == 0){ _throw( Null_Exception);} _temp1125;}));
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1124=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1124->hd=( void*)
ae; _temp1124->tl= 0; _temp1124;})); return;} _LL965: { void* _temp1128; void*
_temp1130; struct _tuple0 _temp1126= Cyc_NewControlFlow_abstract_guard( env,
_temp1014); _LL1131: _temp1130= _temp1126.f1; goto _LL1129; _LL1129: _temp1128=
_temp1126.f2; goto _LL1127; _LL1127: Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1132=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1132->hd=( void*) _temp1130; _temp1132->tl=({
struct Cyc_List_List* _temp1133=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1133->hd=( void*) _temp1128; _temp1133->tl= 0;
_temp1133;}); _temp1132;})); Cyc_NewControlFlow_cf_prepass( env, _temp1012); Cyc_NewControlFlow_cf_prepass(
env, _temp1010); return;} _LL967: _temp1030= _temp1020; _temp1028= _temp1018;
_temp1024= _temp1022; goto _LL969; _LL969: { void* _temp1136; void* _temp1138;
struct _tuple0 _temp1134= Cyc_NewControlFlow_abstract_guard( env, _temp1030);
_LL1139: _temp1138= _temp1134.f1; goto _LL1137; _LL1137: _temp1136= _temp1134.f2;
goto _LL1135; _LL1135: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1140=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1140->hd=( void*) _temp1138; _temp1140->tl=({ struct Cyc_List_List*
_temp1141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1141->hd=( void*) _temp1136; _temp1141->tl= 0; _temp1141;}); _temp1140;}));
Cyc_NewControlFlow_cf_prepass( env, _temp1024); return;} _LL971: return; _LL973:
return; _LL975:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1142=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1142[ 0]=({ struct Cyc_Core_Impossible_struct _temp1143; _temp1143.tag= Cyc_Core_Impossible;
_temp1143.f1=( struct _tagged_string)({ char* _temp1144=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1145; _temp1145.curr= _temp1144; _temp1145.base=
_temp1144; _temp1145.last_plus_one= _temp1144 + 33; _temp1145;}); _temp1143;});
_temp1142;})); _LL977: { struct Cyc_Absyn_Stmt* my_encloser=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* dest_encloser=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1148= _temp1040; if(
_temp1148 == 0){ _throw( Null_Exception);} _temp1148;})))->encloser; while(
dest_encloser != my_encloser) { struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1146=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1147; _temp1147.curr= _temp1146; _temp1147.base=
_temp1146; _temp1147.last_plus_one= _temp1146 + 45; _temp1147;})); break;}
my_encloser= next_encloser;} return;} _LL979: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1058); void* _temp1151; void* _temp1153; struct _tuple0 _temp1149= Cyc_NewControlFlow_abstract_guard(
env, _temp1056); _LL1154: _temp1153= _temp1149.f1; goto _LL1152; _LL1152:
_temp1151= _temp1149.f2; goto _LL1150; _LL1150: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp1050); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1155=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1155->hd=( void*) ae1; _temp1155->tl=({ struct Cyc_List_List* _temp1156=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1156->hd=(
void*) _temp1153; _temp1156->tl=({ struct Cyc_List_List* _temp1157=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1157->hd=( void*) _temp1151;
_temp1157->tl=({ struct Cyc_List_List* _temp1158=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1158->hd=( void*) ae3; _temp1158->tl=
0; _temp1158;}); _temp1157;}); _temp1156;}); _temp1155;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1044); return;}} _LL981: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp1062); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp1060); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1159=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1159->hd=( void*) ae; _temp1159->tl= ael; _temp1159;})); return;} _LL983: {
void* _temp1162; void* _temp1164; struct Cyc_NewControlFlow_AbsSynOne _temp1160=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered( env,
_temp1066, 0)); _LL1165: _temp1164=( void*) _temp1160.inner_exp; goto _LL1163;
_LL1163: _temp1162=( void*) _temp1160.assigns; goto _LL1161; _LL1161: { void* ae=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp1164, _temp1162);
Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List* _temp1166=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1166->hd=( void*)
ae; _temp1166->tl= 0; _temp1166;})); return;}} _LL985: Cyc_NewControlFlow_add_var_root(
env, _temp1077); if( _temp1077->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp1169=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1169->hd=(
void*) Cyc_CfAbsexp_mkLocalOp( _temp1077); _temp1169->tl= 0; _temp1169;})); void*
ae= Cyc_NewControlFlow_abstract_exp_top( init_env,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1168= _temp1077->initializer; if( _temp1168 == 0){
_throw( Null_Exception);} _temp1168;})); Cyc_NewControlFlow_cf_set_absexps( s,({
struct Cyc_List_List* _temp1167=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1167->hd=( void*) ae; _temp1167->tl= 0; _temp1167;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp1068); return; _LL987: for( 0;
_temp1097 != 0; _temp1097=({ struct Cyc_List_List* _temp1170= _temp1097; if(
_temp1170 == 0){ _throw( Null_Exception);} _temp1170->tl;})){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1171= _temp1097;
if( _temp1171 == 0){ _throw( Null_Exception);} _temp1171->hd;}));}{ void* ae=
Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env( env),
_temp1090); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp1172=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1172->hd=( void*) ae; _temp1172->tl= 0; _temp1172;})); Cyc_NewControlFlow_cf_prepass(
env, _temp1079); return;} _LL989: goto _LL991; _LL991: goto _LL993; _LL993:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1173=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1173[ 0]=({ struct Cyc_Core_Impossible_struct _temp1174; _temp1174.tag= Cyc_Core_Impossible;
_temp1174.f1=( struct _tagged_string)({ char* _temp1175=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1176; _temp1176.curr= _temp1175; _temp1176.base=
_temp1175; _temp1176.last_plus_one= _temp1175 + 26; _temp1176;}); _temp1174;});
_temp1173;})); _LL995: Cyc_NewControlFlow_cf_prepass( env, _temp1109); return;
_LL997: Cyc_NewControlFlow_cf_prepass( env, _temp1115);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp1113); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL999: Cyc_NewControlFlow_add_var_root( env, _temp1119); Cyc_NewControlFlow_cf_prepass(
env, _temp1117); return; _LL953:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1177=( char*)"new_control_flow.cyc:885 annot->visited == iteration_num";
struct _tagged_string _temp1178; _temp1178.curr= _temp1177; _temp1178.base=
_temp1177; _temp1178.last_plus_one= _temp1177 + 57; _temp1178;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1179= flow; struct Cyc_Dict_Dict* _temp1185; _LL1181: if( _temp1179
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1182;} else{ goto _LL1183;} _LL1183:
if(( unsigned int) _temp1179 > 1u?*(( int*) _temp1179) == Cyc_CfFlowInfo_InitsFL:
0){ _LL1186: _temp1185=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1179)->f1; goto _LL1184;} else{ goto _LL1180;} _LL1182: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1184: for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1187= vds; if(
_temp1187 == 0){ _throw( Null_Exception);} _temp1187->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct*
root=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1188=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1188[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1189; _temp1189.tag= Cyc_CfFlowInfo_VarRoot;
_temp1189.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1190= vds;
if( _temp1190 == 0){ _throw( Null_Exception);} _temp1190->hd;}); _temp1189;});
_temp1188;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(( void*) Cyc_CfFlowInfo_AllIL,((
void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,(
void*) root)); _temp1185=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1185,( void*) root, pinfo);}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1191=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1191[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1192; _temp1192.tag= Cyc_CfFlowInfo_InitsFL;
_temp1192.f1= _temp1185; _temp1192;}); _temp1191;}); _LL1180:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1193= flow; struct Cyc_Dict_Dict* _temp1199; _LL1195: if( _temp1193
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1196;} else{ goto _LL1197;} _LL1197:
if(( unsigned int) _temp1193 > 1u?*(( int*) _temp1193) == Cyc_CfFlowInfo_InitsFL:
0){ _LL1200: _temp1199=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1193)->f1; goto _LL1198;} else{ goto _LL1194;} _LL1196: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1198: for( 0; vds != 0; vds=({ struct Cyc_List_List* _temp1201= vds; if(
_temp1201 == 0){ _throw( Null_Exception);} _temp1201->tl;})){ struct Cyc_CfFlowInfo_VarRoot_struct*
root=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp1202=( struct Cyc_CfFlowInfo_VarRoot_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1202[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1203; _temp1203.tag= Cyc_CfFlowInfo_VarRoot;
_temp1203.f1=( struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1204= vds;
if( _temp1204 == 0){ _throw( Null_Exception);} _temp1204->hd;}); _temp1203;});
_temp1202;}); _temp1199=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
void* key, void* data)) Cyc_Dict_insert)( _temp1199,( void*) root,(( void*(*)(
struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,( void*) root));}
return( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct* _temp1205=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1205[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1206; _temp1206.tag= Cyc_CfFlowInfo_InitsFL;
_temp1206.f1= _temp1199; _temp1206;}); _temp1205;}); _LL1194:;} static struct
_tuple0 Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if((
void*)({ struct Cyc_List_List* _temp1207= aes; if( _temp1207 == 0){ _throw(
Null_Exception);} _temp1207->hd;}) ==( void*)({ struct Cyc_List_List* _temp1209=({
struct Cyc_List_List* _temp1208= aes; if( _temp1208 == 0){ _throw(
Null_Exception);} _temp1208->tl;}); if( _temp1209 == 0){ _throw( Null_Exception);}
_temp1209->hd;})){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1211= aes; if( _temp1211 == 0){ _throw(
Null_Exception);} _temp1211->hd;}), in_flow); return({ struct _tuple0 _temp1210;
_temp1210.f1= out_flow; _temp1210.f2= out_flow; _temp1210;});} return({ struct
_tuple0 _temp1212; _temp1212.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)({
struct Cyc_List_List* _temp1215= aes; if( _temp1215 == 0){ _throw(
Null_Exception);} _temp1215->hd;}), in_flow); _temp1212.f2= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*)({ struct Cyc_List_List* _temp1214=({ struct Cyc_List_List*
_temp1213= aes; if( _temp1213 == 0){ _throw( Null_Exception);} _temp1213->tl;});
if( _temp1214 == 0){ _throw( Null_Exception);} _temp1214->hd;}), in_flow);
_temp1212;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs=({ struct Cyc_List_List* _temp1216=
scs; if( _temp1216 == 0){ _throw( Null_Exception);} _temp1216->tl;})){ struct
Cyc_Absyn_Switch_clause _temp1220; struct Cyc_Position_Segment* _temp1221;
struct Cyc_Absyn_Stmt* _temp1223; struct Cyc_Absyn_Exp* _temp1225; struct Cyc_Core_Opt*
_temp1227; struct Cyc_Absyn_Pat* _temp1229; struct Cyc_Absyn_Switch_clause*
_temp1218=( struct Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1217=
scs; if( _temp1217 == 0){ _throw( Null_Exception);} _temp1217->hd;}); _temp1220=*
_temp1218; _LL1230: _temp1229=( struct Cyc_Absyn_Pat*) _temp1220.pattern; goto
_LL1228; _LL1228: _temp1227=( struct Cyc_Core_Opt*) _temp1220.pat_vars; goto
_LL1226; _LL1226: _temp1225=( struct Cyc_Absyn_Exp*) _temp1220.where_clause;
goto _LL1224; _LL1224: _temp1223=( struct Cyc_Absyn_Stmt*) _temp1220.body; goto
_LL1222; _LL1222: _temp1221=( struct Cyc_Position_Segment*) _temp1220.loc; goto
_LL1219; _LL1219: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1249= _temp1227;
if( _temp1249 == 0){ _throw( Null_Exception);} _temp1249->v;})); body_inflow=
Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow,( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1231= _temp1227; if( _temp1231 == 0){ _throw(
Null_Exception);} _temp1231->v;}));{ void* body_outflow; if( _temp1225 != 0){
void* _temp1235; void* _temp1237; struct _tuple0 _temp1233= Cyc_NewControlFlow_cf_evalguard(
env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1232= _temp1225; if(
_temp1232 == 0){ _throw( Null_Exception);} _temp1232;}))->loc, aes, body_inflow);
_LL1238: _temp1237= _temp1233.f1; goto _LL1236; _LL1236: _temp1235= _temp1233.f2;
goto _LL1234; _LL1234: aes=({ struct Cyc_List_List* _temp1240=({ struct Cyc_List_List*
_temp1239= aes; if( _temp1239 == 0){ _throw( Null_Exception);} _temp1239->tl;});
if( _temp1240 == 0){ _throw( Null_Exception);} _temp1240->tl;}); in_flow=
_temp1235; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1223,
_temp1237);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1223, body_inflow);}{ void* _temp1241= body_outflow; _LL1243: if( _temp1241
==( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1244;} else{ goto _LL1245;} _LL1245:
goto _LL1246; _LL1244: goto _LL1242; _LL1246: Cyc_Tcutil_terr( _temp1223->loc,(
struct _tagged_string)({ char* _temp1247=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1248; _temp1248.curr= _temp1247; _temp1248.base=
_temp1247; _temp1248.last_plus_one= _temp1247 + 38; _temp1248;})); goto _LL1242;
_LL1242:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1250=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1294; struct Cyc_Absyn_Exp* _temp1296; struct Cyc_Absyn_Exp*
_temp1298; struct Cyc_Absyn_Stmt* _temp1300; struct Cyc_Absyn_Stmt* _temp1302;
struct Cyc_Absyn_Stmt* _temp1304; struct Cyc_Absyn_Stmt* _temp1306; struct Cyc_Absyn_Exp*
_temp1308; struct Cyc_Absyn_Stmt* _temp1310; struct _tuple3 _temp1312; struct
Cyc_Absyn_Stmt* _temp1314; struct Cyc_Absyn_Exp* _temp1316; struct _tuple3
_temp1318; struct Cyc_Absyn_Stmt* _temp1320; struct Cyc_Absyn_Exp* _temp1322;
struct Cyc_Absyn_Stmt* _temp1324; struct Cyc_Absyn_Stmt* _temp1326; struct
_tuple3 _temp1328; struct Cyc_Absyn_Stmt* _temp1330; struct Cyc_Absyn_Exp*
_temp1332; struct _tuple3 _temp1334; struct Cyc_Absyn_Stmt* _temp1336; struct
Cyc_Absyn_Exp* _temp1338; struct Cyc_Absyn_Exp* _temp1340; struct Cyc_Absyn_Stmt*
_temp1342; struct Cyc_Absyn_Switch_clause** _temp1344; struct Cyc_Absyn_Switch_clause*
_temp1346; struct Cyc_List_List* _temp1347; struct Cyc_Absyn_Stmt* _temp1349;
struct Cyc_Absyn_Stmt* _temp1351; struct Cyc_Absyn_Stmt* _temp1353; struct
_tagged_string* _temp1355; struct Cyc_List_List* _temp1357; struct Cyc_Absyn_Exp*
_temp1359; struct Cyc_List_List* _temp1361; struct Cyc_Absyn_Stmt* _temp1363;
struct Cyc_Absyn_Stmt* _temp1365; struct Cyc_Absyn_Decl* _temp1367; struct Cyc_Absyn_Decl
_temp1369; struct Cyc_Position_Segment* _temp1370; void* _temp1372; struct Cyc_Absyn_Vardecl*
_temp1374; struct Cyc_Absyn_Stmt* _temp1376; struct Cyc_Absyn_Decl* _temp1378;
struct Cyc_Absyn_Decl _temp1380; struct Cyc_Position_Segment* _temp1381; void*
_temp1383; int _temp1385; struct Cyc_Absyn_Exp* _temp1387; struct Cyc_Core_Opt*
_temp1389; struct Cyc_Core_Opt* _temp1391; struct Cyc_Core_Opt _temp1393; struct
Cyc_List_List* _temp1394; struct Cyc_Absyn_Pat* _temp1396; struct Cyc_Absyn_Stmt*
_temp1398; struct _tagged_string* _temp1400; struct Cyc_Absyn_Stmt* _temp1402;
struct Cyc_Absyn_Vardecl* _temp1404; struct Cyc_Absyn_Tvar* _temp1406; _LL1252:
if( _temp1250 ==( void*) Cyc_Absyn_Skip_s){ goto _LL1253;} else{ goto _LL1254;}
_LL1254: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Return_s:
0){ _LL1295: _temp1294=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1250)->f1; if( _temp1294 == 0){ goto _LL1255;} else{ goto _LL1256;}} else{
goto _LL1256;} _LL1256: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250)
== Cyc_Absyn_Return_s: 0){ _LL1297: _temp1296=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp1250)->f1; goto _LL1257;} else{ goto _LL1258;}
_LL1258: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Exp_s:
0){ _LL1299: _temp1298=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1250)->f1; goto _LL1259;} else{ goto _LL1260;} _LL1260: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Seq_s: 0){ _LL1303: _temp1302=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1250)->f1; goto
_LL1301; _LL1301: _temp1300=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1250)->f2; goto _LL1261;} else{ goto _LL1262;} _LL1262: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_IfThenElse_s: 0){ _LL1309:
_temp1308=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1250)->f1; goto _LL1307; _LL1307: _temp1306=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1250)->f2; goto _LL1305; _LL1305:
_temp1304=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1250)->f3; goto _LL1263;} else{ goto _LL1264;} _LL1264: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_While_s: 0){ _LL1313: _temp1312=(
struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1250)->f1; _LL1317:
_temp1316= _temp1312.f1; goto _LL1315; _LL1315: _temp1314= _temp1312.f2; goto
_LL1311; _LL1311: _temp1310=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1250)->f2; goto _LL1265;} else{ goto _LL1266;} _LL1266: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Do_s: 0){ _LL1325: _temp1324=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp1250)->f1; goto
_LL1319; _LL1319: _temp1318=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1250)->f2; _LL1323: _temp1322= _temp1318.f1; goto _LL1321; _LL1321:
_temp1320= _temp1318.f2; goto _LL1267;} else{ goto _LL1268;} _LL1268: if((
unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_For_s: 0){
_LL1341: _temp1340=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1250)->f1; goto _LL1335; _LL1335: _temp1334=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1250)->f2; _LL1339: _temp1338= _temp1334.f1; goto _LL1337; _LL1337:
_temp1336= _temp1334.f2; goto _LL1329; _LL1329: _temp1328=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1250)->f3; _LL1333: _temp1332= _temp1328.f1;
goto _LL1331; _LL1331: _temp1330= _temp1328.f2; goto _LL1327; _LL1327: _temp1326=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1250)->f4; goto
_LL1269;} else{ goto _LL1270;} _LL1270: if(( unsigned int) _temp1250 > 1u?*((
int*) _temp1250) == Cyc_Absyn_Break_s: 0){ _LL1343: _temp1342=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1250)->f1; if( _temp1342 == 0){ goto
_LL1271;} else{ goto _LL1272;}} else{ goto _LL1272;} _LL1272: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Fallthru_s: 0){ _LL1348:
_temp1347=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1250)->f1; goto _LL1345; _LL1345: _temp1344=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1250)->f2; if( _temp1344 == 0){ goto
_LL1274;} else{ _temp1346=* _temp1344; goto _LL1273;}} else{ goto _LL1274;}
_LL1274: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Break_s:
0){ _LL1350: _temp1349=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1250)->f1; goto _LL1275;} else{ goto _LL1276;} _LL1276: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Continue_s: 0){ _LL1352:
_temp1351=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1250)->f1; goto _LL1277;} else{ goto _LL1278;} _LL1278: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Goto_s: 0){ _LL1356: _temp1355=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1250)->f1; goto
_LL1354; _LL1354: _temp1353=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1250)->f2; goto _LL1279;} else{ goto _LL1280;} _LL1280: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Switch_s: 0){ _LL1360:
_temp1359=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1250)->f1; goto _LL1358; _LL1358: _temp1357=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1250)->f2; goto _LL1281;} else{ goto
_LL1282;} _LL1282: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_TryCatch_s:
0){ _LL1364: _temp1363=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1250)->f1; goto _LL1362; _LL1362: _temp1361=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1250)->f2; goto _LL1283;} else{ goto
_LL1284;} _LL1284: if(( unsigned int) _temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Decl_s:
0){ _LL1368: _temp1367=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1250)->f1; _temp1369=* _temp1367; _LL1373: _temp1372=( void*) _temp1369.r;
if(*(( int*) _temp1372) == Cyc_Absyn_Var_d){ _LL1375: _temp1374=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1372)->f1; goto _LL1371;} else{ goto
_LL1286;} _LL1371: _temp1370=( struct Cyc_Position_Segment*) _temp1369.loc; goto
_LL1366; _LL1366: _temp1365=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1250)->f2; goto _LL1285;} else{ goto _LL1286;} _LL1286: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Decl_s: 0){ _LL1379: _temp1378=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1250)->f1;
_temp1380=* _temp1378; _LL1384: _temp1383=( void*) _temp1380.r; if(*(( int*)
_temp1383) == Cyc_Absyn_Let_d){ _LL1397: _temp1396=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1383)->f1; goto _LL1392; _LL1392: _temp1391=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1383)->f2; if(
_temp1391 == 0){ goto _LL1288;} else{ _temp1393=* _temp1391; _LL1395: _temp1394=(
struct Cyc_List_List*) _temp1393.v; goto _LL1390;} _LL1390: _temp1389=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1383)->f3; goto _LL1388;
_LL1388: _temp1387=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1383)->f4; goto _LL1386; _LL1386: _temp1385=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1383)->f5; goto _LL1382;} else{ goto _LL1288;} _LL1382: _temp1381=( struct
Cyc_Position_Segment*) _temp1380.loc; goto _LL1377; _LL1377: _temp1376=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1250)->f2; goto _LL1287;}
else{ goto _LL1288;} _LL1288: if(( unsigned int) _temp1250 > 1u?*(( int*)
_temp1250) == Cyc_Absyn_Label_s: 0){ _LL1401: _temp1400=( struct _tagged_string*)((
struct Cyc_Absyn_Label_s_struct*) _temp1250)->f1; goto _LL1399; _LL1399:
_temp1398=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1250)->f2; goto _LL1289;} else{ goto _LL1290;} _LL1290: if(( unsigned int)
_temp1250 > 1u?*(( int*) _temp1250) == Cyc_Absyn_Region_s: 0){ _LL1407:
_temp1406=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1250)->f1; goto _LL1405; _LL1405: _temp1404=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Region_s_struct*) _temp1250)->f2; goto _LL1403; _LL1403:
_temp1402=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1250)->f3; goto _LL1291;} else{ goto _LL1292;} _LL1292: goto _LL1293;
_LL1253: return in_flow; _LL1255: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1257: Cyc_CfAbsexp_eval_absexp( env,({ struct Cyc_Absyn_Exp* _temp1408=
_temp1296; if( _temp1408 == 0){ _throw( Null_Exception);} _temp1408->loc;}),(
void*)({ struct Cyc_List_List* _temp1409= aes; if( _temp1409 == 0){ _throw(
Null_Exception);} _temp1409->hd;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1259: return Cyc_CfAbsexp_eval_absexp( env, _temp1298->loc,( void*)({ struct
Cyc_List_List* _temp1410= aes; if( _temp1410 == 0){ _throw( Null_Exception);}
_temp1410->hd;}), in_flow); _LL1261: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1300, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1302, in_flow));
_LL1263: { void* _temp1413; void* _temp1415; struct _tuple0 _temp1411= Cyc_NewControlFlow_cf_evalguard(
env, _temp1308->loc, aes, in_flow); _LL1416: _temp1415= _temp1411.f1; goto
_LL1414; _LL1414: _temp1413= _temp1411.f2; goto _LL1412; _LL1412: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1306, _temp1415), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1304, _temp1413));} _LL1265: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1314))->flow; void* _temp1419; void* _temp1421; struct _tuple0
_temp1417= Cyc_NewControlFlow_cf_evalguard( env, _temp1316->loc, aes, e_inflow);
_LL1422: _temp1421= _temp1417.f1; goto _LL1420; _LL1420: _temp1419= _temp1417.f2;
goto _LL1418; _LL1418: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1310, _temp1421); Cyc_NewControlFlow_update_flow( _temp1314,
body_outflow); return _temp1419;}} _LL1267: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1324, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1320))->flow; void* _temp1425; void* _temp1427; struct
_tuple0 _temp1423= Cyc_NewControlFlow_cf_evalguard( env, _temp1322->loc, aes,
e_inflow); _LL1428: _temp1427= _temp1423.f1; goto _LL1426; _LL1426: _temp1425=
_temp1423.f2; goto _LL1424; _LL1424: Cyc_NewControlFlow_update_flow( _temp1324,
_temp1427); return _temp1425;} _LL1269: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1340->loc,( void*)({ struct Cyc_List_List* _temp1440= aes; if(
_temp1440 == 0){ _throw( Null_Exception);} _temp1440->hd;}), in_flow); void*
e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( e1_outflow, _temp1336))->flow;
void* _temp1432; void* _temp1434; struct _tuple0 _temp1430= Cyc_NewControlFlow_cf_evalguard(
env, _temp1338->loc,({ struct Cyc_List_List* _temp1429= aes; if( _temp1429 == 0){
_throw( Null_Exception);} _temp1429->tl;}), e2_inflow); _LL1435: _temp1434=
_temp1430.f1; goto _LL1433; _LL1433: _temp1432= _temp1430.f2; goto _LL1431;
_LL1431: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1326, _temp1434); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1330))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1332->loc,( void*)({ struct Cyc_List_List* _temp1439=({ struct Cyc_List_List*
_temp1438=({ struct Cyc_List_List* _temp1437=({ struct Cyc_List_List* _temp1436=
aes; if( _temp1436 == 0){ _throw( Null_Exception);} _temp1436->tl;}); if(
_temp1437 == 0){ _throw( Null_Exception);} _temp1437->tl;}); if( _temp1438 == 0){
_throw( Null_Exception);} _temp1438->tl;}); if( _temp1439 == 0){ _throw(
Null_Exception);} _temp1439->hd;}), e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1336, e3_outflow); return _temp1432;}} _LL1271: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1273: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)({ struct Cyc_List_List*
_temp1441= aes; if( _temp1441 == 0){ _throw( Null_Exception);} _temp1441->hd;}),
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1442= _temp1346->pat_vars; if( _temp1442 == 0){ _throw( Null_Exception);}
_temp1442->v;}); in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
ftvars); in_flow= Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars);
_temp1349=( struct Cyc_Absyn_Stmt*) _temp1346->body; goto _LL1275;} _LL1275:
_temp1351= _temp1349; goto _LL1277; _LL1277: _temp1353= _temp1351; goto _LL1279;
_LL1279: Cyc_NewControlFlow_update_flow(( struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt*
_temp1443= _temp1353; if( _temp1443 == 0){ _throw( Null_Exception);} _temp1443;}),
in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL1281: in_flow= Cyc_CfAbsexp_eval_absexp(
env, _temp1359->loc,( void*)({ struct Cyc_List_List* _temp1444= aes; if(
_temp1444 == 0){ _throw( Null_Exception);} _temp1444->hd;}), in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1357,({ struct Cyc_List_List* _temp1445= aes; if( _temp1445 == 0){
_throw( Null_Exception);} _temp1445->tl;}), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1283: { struct Cyc_NewControlFlow_AnalEnv _temp1448; void* _temp1449; int
_temp1451; struct Cyc_Dict_Dict* _temp1453; struct Cyc_NewControlFlow_AnalEnv*
_temp1446= env; _temp1448=* _temp1446; _LL1454: _temp1453=( struct Cyc_Dict_Dict*)
_temp1448.roots; goto _LL1452; _LL1452: _temp1451=( int) _temp1448.in_try; goto
_LL1450; _LL1450: _temp1449=( void*) _temp1448.tryflow; goto _LL1447; _LL1447:
env->in_try= 1;( void*)( env->tryflow=( void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1363, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1451;( void*)( env->tryflow=( void*) _temp1449); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1361,
aes, scs_inflow); return s_outflow;}} _LL1285: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1457; _temp1457.hd=( void*) _temp1374; _temp1457.tl= 0;
_temp1457;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1374->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1455= e; if( _temp1455 == 0){ _throw( Null_Exception);}
_temp1455;}))->loc,( void*)({ struct Cyc_List_List* _temp1456= aes; if(
_temp1456 == 0){ _throw( Null_Exception);} _temp1456->hd;}), body_inflow);}
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1365, body_inflow);}
_LL1287: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,
_temp1394); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1387->loc,( void*)({
struct Cyc_List_List* _temp1458= aes; if( _temp1458 == 0){ _throw(
Null_Exception);} _temp1458->hd;}), body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1394); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1376, body_inflow);} _LL1289: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1398, in_flow); _LL1291: { struct Cyc_List_List* vds=({ struct Cyc_List_List*
_temp1459=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1459->hd=( void*) _temp1404; _temp1459->tl= 0; _temp1459;}); void*
body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, vds);
body_inflow= Cyc_NewControlFlow_add_init_vars_flow( env, body_inflow, vds);
return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1402, body_inflow);}
_LL1293:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1460=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1460[ 0]=({ struct Cyc_Core_Impossible_struct _temp1461; _temp1461.tag= Cyc_Core_Impossible;
_temp1461.f1=( struct _tagged_string)({ char* _temp1462=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1463; _temp1463.curr= _temp1462; _temp1463.base=
_temp1462; _temp1463.last_plus_one= _temp1462 + 62; _temp1463;}); _temp1461;});
_temp1460;})); _LL1251:;}} void Cyc_NewControlFlow_cf_postpass( struct Cyc_Absyn_Stmt*
s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{ void* _temp1464=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp1488; struct Cyc_Absyn_Stmt* _temp1490;
struct Cyc_Absyn_Stmt* _temp1492; struct Cyc_Absyn_Stmt* _temp1494; struct Cyc_Absyn_Exp*
_temp1496; struct Cyc_Absyn_Stmt* _temp1498; struct _tuple3 _temp1500; struct
Cyc_Absyn_Stmt* _temp1502; struct Cyc_Absyn_Exp* _temp1504; struct _tuple3
_temp1506; struct Cyc_Absyn_Stmt* _temp1508; struct Cyc_Absyn_Exp* _temp1510;
struct Cyc_Absyn_Exp* _temp1512; struct Cyc_Absyn_Stmt* _temp1514; struct
_tuple3 _temp1516; struct Cyc_Absyn_Stmt* _temp1518; struct Cyc_Absyn_Exp*
_temp1520; struct _tuple3 _temp1522; struct Cyc_Absyn_Stmt* _temp1524; struct
Cyc_Absyn_Exp* _temp1526; struct Cyc_Absyn_Stmt* _temp1528; struct Cyc_Absyn_Stmt*
_temp1530; struct Cyc_Absyn_Vardecl* _temp1532; struct Cyc_Absyn_Tvar* _temp1534;
struct Cyc_Absyn_Stmt* _temp1536; struct _tagged_string* _temp1538; struct Cyc_Absyn_Stmt*
_temp1540; struct Cyc_Absyn_Decl* _temp1542; struct Cyc_List_List* _temp1544;
struct Cyc_Absyn_Stmt* _temp1546; struct Cyc_List_List* _temp1548; struct Cyc_Absyn_Exp*
_temp1550; _LL1466: if(( unsigned int) _temp1464 > 1u?*(( int*) _temp1464) ==
Cyc_Absyn_Seq_s: 0){ _LL1491: _temp1490=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1464)->f1; goto _LL1489; _LL1489: _temp1488=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1464)->f2; goto _LL1467;} else{ goto
_LL1468;} _LL1468: if(( unsigned int) _temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_IfThenElse_s:
0){ _LL1497: _temp1496=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1464)->f1; goto _LL1495; _LL1495: _temp1494=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1464)->f2; goto _LL1493; _LL1493:
_temp1492=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1464)->f3; goto _LL1469;} else{ goto _LL1470;} _LL1470: if(( unsigned int)
_temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_For_s: 0){ _LL1513: _temp1512=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1464)->f1; goto
_LL1507; _LL1507: _temp1506=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1464)->f2; _LL1511: _temp1510= _temp1506.f1; goto _LL1509; _LL1509:
_temp1508= _temp1506.f2; goto _LL1501; _LL1501: _temp1500=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1464)->f3; _LL1505: _temp1504= _temp1500.f1;
goto _LL1503; _LL1503: _temp1502= _temp1500.f2; goto _LL1499; _LL1499: _temp1498=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1464)->f4; goto
_LL1471;} else{ goto _LL1472;} _LL1472: if(( unsigned int) _temp1464 > 1u?*((
int*) _temp1464) == Cyc_Absyn_While_s: 0){ _LL1517: _temp1516=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp1464)->f1; _LL1521: _temp1520= _temp1516.f1;
goto _LL1519; _LL1519: _temp1518= _temp1516.f2; goto _LL1515; _LL1515: _temp1514=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp1464)->f2; goto
_LL1473;} else{ goto _LL1474;} _LL1474: if(( unsigned int) _temp1464 > 1u?*((
int*) _temp1464) == Cyc_Absyn_Do_s: 0){ _LL1529: _temp1528=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1464)->f1; goto _LL1523; _LL1523: _temp1522=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1464)->f2; _LL1527:
_temp1526= _temp1522.f1; goto _LL1525; _LL1525: _temp1524= _temp1522.f2; goto
_LL1475;} else{ goto _LL1476;} _LL1476: if(( unsigned int) _temp1464 > 1u?*((
int*) _temp1464) == Cyc_Absyn_Region_s: 0){ _LL1535: _temp1534=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_Region_s_struct*) _temp1464)->f1; goto _LL1533; _LL1533:
_temp1532=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Region_s_struct*)
_temp1464)->f2; goto _LL1531; _LL1531: _temp1530=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Region_s_struct*) _temp1464)->f3; goto _LL1477;} else{ goto
_LL1478;} _LL1478: if(( unsigned int) _temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_Label_s:
0){ _LL1539: _temp1538=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1464)->f1; goto _LL1537; _LL1537: _temp1536=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1464)->f2; goto _LL1479;} else{ goto
_LL1480;} _LL1480: if(( unsigned int) _temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_Decl_s:
0){ _LL1543: _temp1542=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1464)->f1; goto _LL1541; _LL1541: _temp1540=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1464)->f2; goto _LL1481;} else{ goto
_LL1482;} _LL1482: if(( unsigned int) _temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_TryCatch_s:
0){ _LL1547: _temp1546=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1464)->f1; goto _LL1545; _LL1545: _temp1544=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1464)->f2; goto _LL1483;} else{ goto
_LL1484;} _LL1484: if(( unsigned int) _temp1464 > 1u?*(( int*) _temp1464) == Cyc_Absyn_Switch_s:
0){ _LL1551: _temp1550=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1464)->f1; goto _LL1549; _LL1549: _temp1548=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1464)->f2; goto _LL1485;} else{ goto
_LL1486;} _LL1486: goto _LL1487; _LL1467: _temp1494= _temp1490; _temp1492=
_temp1488; goto _LL1469; _LL1469: Cyc_NewControlFlow_cf_postpass( _temp1494);
Cyc_NewControlFlow_cf_postpass( _temp1492); return; _LL1471: Cyc_NewControlFlow_cf_postpass(
_temp1502); _temp1518= _temp1498; _temp1514= _temp1508; goto _LL1473; _LL1473:
_temp1528= _temp1514; _temp1524= _temp1518; goto _LL1475; _LL1475: Cyc_NewControlFlow_cf_postpass(
_temp1524); _temp1530= _temp1528; goto _LL1477; _LL1477: _temp1536= _temp1530;
goto _LL1479; _LL1479: _temp1540= _temp1536; goto _LL1481; _LL1481: Cyc_NewControlFlow_cf_postpass(
_temp1540); return; _LL1483: Cyc_NewControlFlow_cf_postpass( _temp1546);
_temp1548= _temp1544; goto _LL1485; _LL1485: for( 0; _temp1548 != 0; _temp1548=({
struct Cyc_List_List* _temp1552= _temp1548; if( _temp1552 == 0){ _throw(
Null_Exception);} _temp1552->tl;})){ Cyc_NewControlFlow_cf_postpass((( struct
Cyc_Absyn_Switch_clause*)({ struct Cyc_List_List* _temp1553= _temp1548; if(
_temp1553 == 0){ _throw( Null_Exception);} _temp1553->hd;}))->body);} return;
_LL1487: return; _LL1465:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1581; _temp1581.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1582=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv));
_temp1582->all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1582->comprehension_vars=
0; _temp1582;}); _temp1581.lhsides= 0; _temp1581.access_path= 0; _temp1581;});
struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp1580= fd->param_vardecls; if( _temp1580 == 0){ _throw( Null_Exception);}
_temp1580->v;});{ struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds=({
struct Cyc_List_List* _temp1554= vds; if( _temp1554 == 0){ _throw(
Null_Exception);} _temp1554->tl;})){ Cyc_NewControlFlow_add_var_root( absenv,(
struct Cyc_Absyn_Vardecl*)({ struct Cyc_List_List* _temp1555= vds; if( _temp1555
== 0){ _throw( Null_Exception);} _temp1555->hd;}));}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1558; struct Cyc_List_List*
_temp1560; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1562; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1564; struct Cyc_List_List* _temp1565; struct Cyc_Dict_Dict* _temp1567;
struct Cyc_NewControlFlow_AbsEnv _temp1556= absenv; _LL1563: _temp1562=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1556.shared; _temp1564=* _temp1562;
_LL1568: _temp1567=( struct Cyc_Dict_Dict*) _temp1564.all_roots; goto _LL1566;
_LL1566: _temp1565=( struct Cyc_List_List*) _temp1564.comprehension_vars; goto
_LL1561; _LL1561: _temp1560=( struct Cyc_List_List*) _temp1556.lhsides; goto
_LL1559; _LL1559: _temp1558=( struct Cyc_List_List*) _temp1556.access_path; goto
_LL1557; _LL1557: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1579=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1579->roots= _temp1567; _temp1579->in_try= 0; _temp1579->tryflow=( void*)((
void*) Cyc_CfFlowInfo_BottomFL); _temp1579;}); void* in_flow=( void*)({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp1577=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1577[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp1578; _temp1578.tag= Cyc_CfFlowInfo_InitsFL;
_temp1578.f1=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)(
Cyc_CfFlowInfo_local_root_cmp); _temp1578;}); _temp1577;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
analenv, in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
analenv, in_flow, _temp1565);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1569= out_flow; _LL1571: if( _temp1569 ==( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1572;} else{ goto _LL1573;} _LL1573: goto _LL1574; _LL1572: goto _LL1570;
_LL1574: if(( void*) fd->ret_type !=( void*) Cyc_Absyn_VoidType){ Cyc_Tcutil_terr((
fd->body)->loc,( struct _tagged_string)({ char* _temp1575=( char*)"function may complete without returning a value";
struct _tagged_string _temp1576; _temp1576.curr= _temp1575; _temp1576.base=
_temp1575; _temp1576.last_plus_one= _temp1575 + 48; _temp1576;}));} goto _LL1570;
_LL1570:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1583= ds; if( _temp1583 == 0){ _throw( Null_Exception);} _temp1583->tl;})){
void* _temp1585=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1584= ds; if( _temp1584 == 0){ _throw( Null_Exception);} _temp1584->hd;}))->r;
struct Cyc_Absyn_Fndecl* _temp1595; struct Cyc_List_List* _temp1597; struct
_tuple1* _temp1599; struct Cyc_List_List* _temp1601; struct _tagged_string*
_temp1603; _LL1587: if(*(( int*) _temp1585) == Cyc_Absyn_Fn_d){ _LL1596:
_temp1595=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp1585)->f1;
goto _LL1588;} else{ goto _LL1589;} _LL1589: if(*(( int*) _temp1585) == Cyc_Absyn_Using_d){
_LL1600: _temp1599=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1585)->f1; goto _LL1598; _LL1598: _temp1597=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1585)->f2; goto _LL1590;} else{ goto
_LL1591;} _LL1591: if(*(( int*) _temp1585) == Cyc_Absyn_Namespace_d){ _LL1604:
_temp1603=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1585)->f1; goto _LL1602; _LL1602: _temp1601=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1585)->f2; goto _LL1592;} else{ goto
_LL1593;} _LL1593: goto _LL1594; _LL1588: Cyc_NewControlFlow_cf_check_fun(
_temp1595); goto _LL1586; _LL1590: _temp1601= _temp1597; goto _LL1592; _LL1592:
Cyc_NewControlFlow_cf_check( _temp1601); goto _LL1586; _LL1594: goto _LL1586;
_LL1586:;}}