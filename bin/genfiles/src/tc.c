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
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileCloseError[ 19u]; extern
char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2); extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
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
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple0* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*, void*)); extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[
11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict* d);
extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes= 0;
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_string*, struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type( void* t); extern
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default(
void*); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**,
struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, int var_default_init, struct Cyc_List_List* ds); extern
struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*);
extern char Cyc_Tcdecl_Incompatible[ 17u]; extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl*
d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg); extern struct
Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl( struct Cyc_Absyn_Uniondecl* d0,
struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern void* Cyc_Tcdecl_merge_binding( void*
d0, void* d1, struct Cyc_Position_Segment* loc, struct _tagged_string* msg);
extern struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List*
f, int* res, struct _tagged_string* v, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); static char _temp2[ 1u]=""; static struct _tagged_string
Cyc_Tc_tc_msg_c=( struct _tagged_string){ _temp2, _temp2, _temp2 + 1u}; static
struct _tagged_string* Cyc_Tc_tc_msg=( struct _tagged_string*)& Cyc_Tc_tc_msg_c;
static struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp3= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp9;
struct Cyc_List_List* _temp11; struct Cyc_List_List** _temp13; int _temp14;
struct Cyc_List_List* _temp16; void* _temp18; struct Cyc_Core_Opt* _temp20;
struct Cyc_List_List* _temp22; _LL5: if(( unsigned int) _temp3 > 4u?*(( int*)
_temp3) == Cyc_Absyn_FnType: 0){ _LL10: _temp9=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp3)->f1; _LL23: _temp22=( struct Cyc_List_List*)
_temp9.tvars; goto _LL21; _LL21: _temp20=( struct Cyc_Core_Opt*) _temp9.effect;
goto _LL19; _LL19: _temp18=( void*) _temp9.ret_typ; goto _LL17; _LL17: _temp16=(
struct Cyc_List_List*) _temp9.args; goto _LL15; _LL15: _temp14=( int) _temp9.varargs;
goto _LL12; _LL12: _temp11=( struct Cyc_List_List*) _temp9.attributes; _temp13=&(((
struct Cyc_Absyn_FnType_struct*) _temp3)->f1).attributes; goto _LL6;} else{ goto
_LL7;} _LL7: goto _LL8; _LL6: { struct Cyc_List_List* res_atts= 0; for( 0; atts
!= 0; atts=({ struct Cyc_List_List* _temp24= atts; if( _temp24 == 0){ _throw(
Null_Exception);} _temp24->tl;})){ void* _temp26=( void*)({ struct Cyc_List_List*
_temp25= atts; if( _temp25 == 0){ _throw( Null_Exception);} _temp25->hd;}); int
_temp40; _LL28: if(( unsigned int) _temp26 > 15u?*(( int*) _temp26) == Cyc_Absyn_Regparm_att:
0){ _LL41: _temp40=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp26)->f1;
goto _LL29;} else{ goto _LL30;} _LL30: if( _temp26 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL31;} else{ goto _LL32;} _LL32: if( _temp26 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL33;} else{ goto _LL34;} _LL34: if( _temp26 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL35;} else{ goto _LL36;} _LL36: if( _temp26 ==( void*) Cyc_Absyn_Const_att){
goto _LL37;} else{ goto _LL38;} _LL38: goto _LL39; _LL29: goto _LL31; _LL31:
goto _LL33; _LL33: goto _LL35; _LL35: goto _LL37; _LL37:* _temp13=({ struct Cyc_List_List*
_temp42=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp42->hd=( void*)(( void*)({ struct Cyc_List_List* _temp43= atts; if( _temp43
== 0){ _throw( Null_Exception);} _temp43->hd;})); _temp42->tl=* _temp13; _temp42;});
continue; _LL39: res_atts=({ struct Cyc_List_List* _temp44=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp44->hd=( void*)(( void*)({
struct Cyc_List_List* _temp45= atts; if( _temp45 == 0){ _throw( Null_Exception);}
_temp45->hd;})); _temp44->tl= res_atts; _temp44;}); continue; _LL27:;} return
res_atts;} _LL8: return(( struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp46=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp47; _temp47.curr= _temp46; _temp47.base= _temp46; _temp47.last_plus_one=
_temp46 + 22; _temp47;})); _LL4:;} struct _tuple3{ void* f1; int f2; } ; static
void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl* vd, int
check_var_init){ struct _tagged_string* v=(* vd->name).f2; void* t=( void*) vd->type;
void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{ void*
_temp48=(* vd->name).f1; struct Cyc_List_List* _temp56; struct Cyc_List_List*
_temp58; _LL50: if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Rel_n:
0){ _LL57: _temp56=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp48)->f1; if( _temp56 == 0){ goto _LL51;} else{ goto _LL52;}} else{ goto
_LL52;} _LL52: if(( unsigned int) _temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Abs_n:
0){ _LL59: _temp58=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp48)->f1; if( _temp58 == 0){ goto _LL53;} else{ goto _LL54;}} else{ goto
_LL54;} _LL54: goto _LL55; _LL51: goto _LL49; _LL53: goto _LL49; _LL55: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp60=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 43; _temp61;})); return; _LL49:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp62=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp62[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp63; _temp63.tag= Cyc_Absyn_Abs_n; _temp63.f1= te->ns; _temp63;}); _temp62;});{
void* _temp64= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp71; struct
Cyc_Absyn_Tqual _temp73; void* _temp75; _LL66: if(( unsigned int) _temp64 > 4u?*((
int*) _temp64) == Cyc_Absyn_ArrayType: 0){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp64)->f1; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp64)->f2; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp64)->f3; if( _temp71 == 0){ goto _LL70;} else{ goto _LL68;}} else{ goto
_LL68;} _LL70: if( vd->initializer != 0){ goto _LL67;} else{ goto _LL68;} _LL68:
goto _LL69; _LL67:{ void* _temp78=( void*)({ struct Cyc_Absyn_Exp* _temp77= vd->initializer;
if( _temp77 == 0){ _throw( Null_Exception);} _temp77->r;}); void* _temp88;
struct _tagged_string _temp90; struct Cyc_List_List* _temp92; struct Cyc_Core_Opt*
_temp94; struct Cyc_List_List* _temp96; _LL80: if(*(( int*) _temp78) == Cyc_Absyn_Const_e){
_LL89: _temp88=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp78)->f1; if((
unsigned int) _temp88 > 1u?*(( int*) _temp88) == Cyc_Absyn_String_c: 0){ _LL91:
_temp90=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp88)->f1;
goto _LL81;} else{ goto _LL82;}} else{ goto _LL82;} _LL82: if(*(( int*) _temp78)
== Cyc_Absyn_UnresolvedMem_e){ _LL95: _temp94=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp78)->f1; goto _LL93; _LL93: _temp92=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp78)->f2;
goto _LL83;} else{ goto _LL84;} _LL84: if(*(( int*) _temp78) == Cyc_Absyn_Array_e){
_LL97: _temp96=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp78)->f1; goto _LL85;} else{ goto _LL86;} _LL86: goto _LL87; _LL81: t=( void*)(
vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp98=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp98[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp99; _temp99.tag= Cyc_Absyn_ArrayType;
_temp99.f1=( void*) _temp75; _temp99.f2= _temp73; _temp99.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp100= _temp90;( unsigned int)(
_temp100.last_plus_one - _temp100.curr);}), 0); _temp99;}); _temp98;}))); goto
_LL79; _LL83: _temp96= _temp92; goto _LL85; _LL85: t=( void*)( vd->type=( void*)((
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp101=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp101[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp102; _temp102.tag= Cyc_Absyn_ArrayType; _temp102.f1=(
void*) _temp75; _temp102.f2= _temp73; _temp102.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp96), 0);
_temp102;}); _temp101;}))); goto _LL79; _LL87: goto _LL79; _LL79:;} goto _LL65;
_LL69: goto _LL65; _LL65:;} Cyc_Tcutil_check_valid_toplevel_type( loc, te, t);
if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts( t,
atts);} if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){
if( vd->initializer != 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp103=( char*)"extern declaration should not have initializer"; struct
_tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 47; _temp104;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp105= atts; if(
_temp105 == 0){ _throw( Null_Exception);} _temp105->tl;})){ void* _temp107=(
void*)({ struct Cyc_List_List* _temp106= atts; if( _temp106 == 0){ _throw(
Null_Exception);} _temp106->hd;}); int _temp127; struct _tagged_string _temp129;
_LL109: if(( unsigned int) _temp107 > 15u?*(( int*) _temp107) == Cyc_Absyn_Aligned_att:
0){ _LL128: _temp127=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp107)->f1;
goto _LL110;} else{ goto _LL111;} _LL111: if(( unsigned int) _temp107 > 15u?*((
int*) _temp107) == Cyc_Absyn_Section_att: 0){ _LL130: _temp129=( struct
_tagged_string)(( struct Cyc_Absyn_Section_att_struct*) _temp107)->f1; goto
_LL112;} else{ goto _LL113;} _LL113: if( _temp107 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp107 ==( void*) Cyc_Absyn_Shared_att){
goto _LL116;} else{ goto _LL117;} _LL117: if( _temp107 ==( void*) Cyc_Absyn_Unused_att){
goto _LL118;} else{ goto _LL119;} _LL119: if( _temp107 ==( void*) Cyc_Absyn_Weak_att){
goto _LL120;} else{ goto _LL121;} _LL121: if( _temp107 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL122;} else{ goto _LL123;} _LL123: if( _temp107 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL124;} else{ goto _LL125;} _LL125: goto _LL126; _LL110: continue; _LL112:
continue; _LL114: continue; _LL116: continue; _LL118: continue; _LL120:
continue; _LL122: continue; _LL124: continue; _LL126: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp132= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp131= atts; if( _temp131 == 0){ _throw( Null_Exception);} _temp131->hd;}));
struct _tagged_string _temp133= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp132.last_plus_one - _temp132.curr, _temp132.curr, _temp133.last_plus_one -
_temp133.curr, _temp133.curr);})); goto _LL108; _LL108:;} if( vd->initializer ==
0){ if( check_var_init? ! Cyc_Tcutil_supports_default( t): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp134= Cyc_Absynpp_qvar2string( vd->name);
struct _tagged_string _temp135= Cyc_Absynpp_typ2string( t); xprintf("initializer required for variable %.*s of type %.*s",
_temp134.last_plus_one - _temp134.curr, _temp134.curr, _temp135.last_plus_one -
_temp135.curr, _temp135.curr);}));}} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp141= vd->initializer; if( _temp141 == 0){ _throw(
Null_Exception);} _temp141;}); void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)&
t, e); if( ! Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp136= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp137= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp138= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp136.last_plus_one - _temp136.curr, _temp136.curr, _temp137.last_plus_one -
_temp137.curr, _temp137.curr, _temp138.last_plus_one - _temp138.curr, _temp138.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp139=( char*)"initializer is not a constant expression";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 41; _temp140;}));}}} else{ for( 0; atts != 0;
atts=({ struct Cyc_List_List* _temp142= atts; if( _temp142 == 0){ _throw(
Null_Exception);} _temp142->tl;})){ void* _temp144=( void*)({ struct Cyc_List_List*
_temp143= atts; if( _temp143 == 0){ _throw( Null_Exception);} _temp143->hd;});
int _temp162; int _temp164; _LL146: if(( unsigned int) _temp144 > 15u?*(( int*)
_temp144) == Cyc_Absyn_Regparm_att: 0){ _LL163: _temp162=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp144)->f1; goto _LL147;} else{ goto _LL148;} _LL148: if( _temp144 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL149;} else{ goto _LL150;} _LL150: if( _temp144
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL151;} else{ goto _LL152;} _LL152: if(
_temp144 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL153;} else{ goto _LL154;}
_LL154: if( _temp144 ==( void*) Cyc_Absyn_Const_att){ goto _LL155;} else{ goto
_LL156;} _LL156: if(( unsigned int) _temp144 > 15u?*(( int*) _temp144) == Cyc_Absyn_Aligned_att:
0){ _LL165: _temp164=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp144)->f1;
goto _LL157;} else{ goto _LL158;} _LL158: if( _temp144 ==( void*) Cyc_Absyn_Packed_att){
goto _LL159;} else{ goto _LL160;} _LL160: goto _LL161; _LL147: goto _LL149;
_LL149: goto _LL151; _LL151: goto _LL153; _LL153: goto _LL155; _LL155:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp166=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 51; _temp167;})); goto _LL145; _LL157: goto
_LL159; _LL159: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp169= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp168= atts; if( _temp168 == 0){ _throw(
Null_Exception);} _temp168->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp169.last_plus_one - _temp169.curr, _temp169.curr);})); goto _LL145; _LL161:
continue; _LL145:;}}}{ struct _handler_cons _temp170; _push_handler(& _temp170);{
int _temp172= 0; if( setjmp( _temp170.handler)){ _temp172= 1;} if( ! _temp172){{
struct _tuple3* ans=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp173=(*
ans).f1; void* _temp179; _LL175: if(*(( int*) _temp173) == Cyc_Tcenv_VarRes){
_LL180: _temp179=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp173)->f1; goto
_LL176;} else{ goto _LL177;} _LL177: goto _LL178; _LL176: { struct Cyc_Absyn_Global_b_struct*
b1=({ struct Cyc_Absyn_Global_b_struct* _temp184=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp184[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp185; _temp185.tag= Cyc_Absyn_Global_b; _temp185.f1=
vd; _temp185;}); _temp184;}); void* b= Cyc_Tcdecl_merge_binding( _temp179,( void*)
b1, loc, Cyc_Tc_tc_msg); if( b ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL174;}
if( b == _temp179?(* ans).f2: 0){ goto _LL174;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp181=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp181->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp182=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp182[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp183; _temp183.tag= Cyc_Tcenv_VarRes;
_temp183.f1=( void*) b; _temp183;}); _temp182;}); _temp181->f2= 1; _temp181;}));
goto _LL174;} _LL178:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp186=( char*)"tcVardecl: a variable is being declared with a wrong name";
struct _tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 58; _temp187;})); goto _LL174; _LL174:;};
_pop_handler();} else{ void* _temp171=( void*) _exn_thrown; void* _temp189=
_temp171; _LL191: if( _temp189 == Cyc_Dict_Absent){ goto _LL192;} else{ goto
_LL193;} _LL193: goto _LL194; _LL192: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp195=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp195->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp196=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp196[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp197; _temp197.tag= Cyc_Tcenv_VarRes;
_temp197.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp198=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp198[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp199; _temp199.tag= Cyc_Absyn_Global_b;
_temp199.f1= vd; _temp199;}); _temp198;})); _temp197;}); _temp196;}); _temp195->f2=
0; _temp195;})); goto _LL190; _LL194:( void) _throw( _temp189); _LL190:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp200=(*
fd->name).f1; struct Cyc_List_List* _temp208; struct Cyc_List_List* _temp210;
_LL202: if(( unsigned int) _temp200 > 1u?*(( int*) _temp200) == Cyc_Absyn_Rel_n:
0){ _LL209: _temp208=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp200)->f1; if( _temp208 == 0){ goto _LL203;} else{ goto _LL204;}} else{ goto
_LL204;} _LL204: if(( unsigned int) _temp200 > 1u?*(( int*) _temp200) == Cyc_Absyn_Abs_n:
0){ _LL211: _temp210=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp200)->f1; goto _LL205;} else{ goto _LL206;} _LL206: goto _LL207; _LL203:
goto _LL201; _LL205:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp212=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp212[ 0]=({ struct Cyc_Core_Impossible_struct _temp213; _temp213.tag= Cyc_Core_Impossible;
_temp213.f1=( struct _tagged_string)({ char* _temp214=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp215; _temp215.curr= _temp214; _temp215.base= _temp214;
_temp215.last_plus_one= _temp214 + 22; _temp215;}); _temp213;}); _temp212;}));
_LL207: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp216=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp217; _temp217.curr= _temp216; _temp217.base= _temp216;
_temp217.last_plus_one= _temp216 + 43; _temp217;})); return; _LL201:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp218=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp218[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp219; _temp219.tag= Cyc_Absyn_Abs_n; _temp219.f1= te->ns; _temp219;});
_temp218;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({
struct Cyc_List_List* _temp220= atts; if( _temp220 == 0){ _throw( Null_Exception);}
_temp220->tl;})){ void* _temp222=( void*)({ struct Cyc_List_List* _temp221= atts;
if( _temp221 == 0){ _throw( Null_Exception);} _temp221->hd;}); int _temp230;
_LL224: if( _temp222 ==( void*) Cyc_Absyn_Packed_att){ goto _LL225;} else{ goto
_LL226;} _LL226: if(( unsigned int) _temp222 > 15u?*(( int*) _temp222) == Cyc_Absyn_Aligned_att:
0){ _LL231: _temp230=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp222)->f1;
goto _LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL225: goto _LL227;
_LL227: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp233= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp232= atts; if( _temp232 == 0){ _throw(
Null_Exception);} _temp232->hd;})); xprintf("bad attribute %.*s for function",
_temp233.last_plus_one - _temp233.curr, _temp233.curr);})); goto _LL223; _LL229:
goto _LL223; _LL223:;}}{ struct _handler_cons _temp234; _push_handler(& _temp234);{
int _temp236= 0; if( setjmp( _temp234.handler)){ _temp236= 1;} if( ! _temp236){{
struct _tuple3* ans=(( struct _tuple3*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp237=(*
ans).f1; void* _temp243; _LL239: if(*(( int*) _temp237) == Cyc_Tcenv_VarRes){
_LL244: _temp243=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp237)->f1; goto
_LL240;} else{ goto _LL241;} _LL241: goto _LL242; _LL240: { struct Cyc_Absyn_Funname_b_struct*
b1=({ struct Cyc_Absyn_Funname_b_struct* _temp248=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp248[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp249; _temp249.tag= Cyc_Absyn_Funname_b; _temp249.f1=
fd; _temp249;}); _temp248;}); void* b= Cyc_Tcdecl_merge_binding( _temp243,( void*)
b1, loc, Cyc_Tc_tc_msg); if( b ==( void*) Cyc_Absyn_Unresolved_b){ goto _LL238;}
if( b == _temp243?(* ans).f2: 0){ goto _LL238;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp245=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp245->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp246=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp246[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp247; _temp247.tag= Cyc_Tcenv_VarRes;
_temp247.f1=( void*) b; _temp247;}); _temp246;}); _temp245->f2= 1; _temp245;}));
goto _LL238;} _LL242:(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp250=( char*)"tcFndecl: a variable is being declared with a wrong name";
struct _tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 57; _temp251;})); goto _LL238; _LL238:;};
_pop_handler();} else{ void* _temp235=( void*) _exn_thrown; void* _temp253=
_temp235; _LL255: if( _temp253 == Cyc_Dict_Absent){ goto _LL256;} else{ goto
_LL257;} _LL257: goto _LL258; _LL256: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple3* _temp259=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3)); _temp259->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp260=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp260[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp261; _temp261.tag= Cyc_Tcenv_VarRes;
_temp261.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp262=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp262[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp263; _temp263.tag= Cyc_Absyn_Funname_b;
_temp263.f1= fd; _temp263;}); _temp262;})); _temp261;}); _temp260;}); _temp259->f2=
0; _temp259;})); goto _LL254; _LL258:( void) _throw( _temp253); _LL254:;}}} te->le=({
struct Cyc_Core_Opt* _temp264=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp264->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp265=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp265[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp266; _temp266.tag= Cyc_Tcenv_Outermost;
_temp266.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp266;}); _temp265;}));
_temp264;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp267=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 49; _temp268;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp269=(* td->name).f1; struct Cyc_List_List*
_temp277; struct Cyc_List_List* _temp279; _LL271: if(( unsigned int) _temp269 >
1u?*(( int*) _temp269) == Cyc_Absyn_Rel_n: 0){ _LL278: _temp277=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp269)->f1; if( _temp277 == 0){ goto _LL272;}
else{ goto _LL273;}} else{ goto _LL273;} _LL273: if(( unsigned int) _temp269 > 1u?*((
int*) _temp269) == Cyc_Absyn_Abs_n: 0){ _LL280: _temp279=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp269)->f1; if( _temp279 == 0){ goto _LL274;}
else{ goto _LL275;}} else{ goto _LL275;} _LL275: goto _LL276; _LL272: goto
_LL270; _LL274: goto _LL270; _LL276: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp281=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp282; _temp282.curr= _temp281; _temp282.base= _temp281;
_temp282.last_plus_one= _temp281 + 43; _temp282;})); return; _LL270:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp283=* v; xprintf("redeclaration of typedef %.*s",
_temp283.last_plus_one - _temp283.curr, _temp283.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp284=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp284[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp285; _temp285.tag= Cyc_Absyn_Abs_n; _temp285.f1= te->ns; _temp285;});
_temp284;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct _tagged_string obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp286= _new_region();
struct _RegionHandle* uprev_rgn=& _temp286; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* fs= fields; for( 0; fs != 0;
fs=({ struct Cyc_List_List* _temp287= fs; if( _temp287 == 0){ _throw(
Null_Exception);} _temp287->tl;})){ struct Cyc_Absyn_Structfield _temp291;
struct Cyc_List_List* _temp292; struct Cyc_Core_Opt* _temp294; void* _temp296;
struct Cyc_Absyn_Tqual _temp298; struct _tagged_string* _temp300; struct Cyc_Absyn_Structfield*
_temp289=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp288= fs;
if( _temp288 == 0){ _throw( Null_Exception);} _temp288->hd;}); _temp291=*
_temp289; _LL301: _temp300=( struct _tagged_string*) _temp291.name; goto _LL299;
_LL299: _temp298=( struct Cyc_Absyn_Tqual) _temp291.tq; goto _LL297; _LL297:
_temp296=( void*) _temp291.type; goto _LL295; _LL295: _temp294=( struct Cyc_Core_Opt*)
_temp291.width; goto _LL293; _LL293: _temp292=( struct Cyc_List_List*) _temp291.attributes;
goto _LL290; _LL290: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp300)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp302=* _temp300; struct _tagged_string _temp303= obj; xprintf("duplicate field %.*s in %.*s",
_temp302.last_plus_one - _temp302.curr, _temp302.curr, _temp303.last_plus_one -
_temp303.curr, _temp303.curr);}));} if( Cyc_String_strcmp(* _temp300,( struct
_tagged_string)({ char* _temp304=( char*)""; struct _tagged_string _temp305;
_temp305.curr= _temp304; _temp305.base= _temp304; _temp305.last_plus_one=
_temp304 + 1; _temp305;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp306=(
struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp306->hd=( void*) _temp300; _temp306->tl= prev_fields; _temp306;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp296); if( _temp294 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Core_Opt* _temp307= _temp294; if( _temp307 == 0){ _throw(
Null_Exception);} _temp307->v;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp308=* _temp300; xprintf("bitfield %.*s does not have constant width",
_temp308.last_plus_one - _temp308.curr, _temp308.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp309= _temp294; if( _temp309
== 0){ _throw( Null_Exception);} _temp309->v;}));}{ void* _temp310= Cyc_Tcutil_compress(
_temp296); void* _temp316; void* _temp318; _LL312: if(( unsigned int) _temp310 >
4u?*(( int*) _temp310) == Cyc_Absyn_IntType: 0){ _LL319: _temp318=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp310)->f1; goto _LL317; _LL317: _temp316=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp310)->f2; goto _LL313;} else{
goto _LL314;} _LL314: goto _LL315; _LL313:{ void* _temp320= _temp316; _LL322:
if( _temp320 ==( void*) Cyc_Absyn_B1){ goto _LL323;} else{ goto _LL324;} _LL324:
if( _temp320 ==( void*) Cyc_Absyn_B2){ goto _LL325;} else{ goto _LL326;} _LL326:
if( _temp320 ==( void*) Cyc_Absyn_B4){ goto _LL327;} else{ goto _LL328;} _LL328:
if( _temp320 ==( void*) Cyc_Absyn_B8){ goto _LL329;} else{ goto _LL321;} _LL323:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp330=(
char*)"bitfield larger than type"; struct _tagged_string _temp331; _temp331.curr=
_temp330; _temp331.base= _temp330; _temp331.last_plus_one= _temp330 + 26;
_temp331;}));} goto _LL321; _LL325: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp332=( char*)"bitfield larger than type"; struct
_tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 26; _temp333;}));} goto _LL321; _LL327: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp334=( char*)"bitfield larger than type";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 26; _temp335;}));} goto _LL321; _LL329: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp336=( char*)"bitfield larger than type";
struct _tagged_string _temp337; _temp337.curr= _temp336; _temp337.base= _temp336;
_temp337.last_plus_one= _temp336 + 26; _temp337;}));} goto _LL321; _LL321:;}
goto _LL311; _LL315: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp338=*
_temp300; struct _tagged_string _temp339= Cyc_Absynpp_typ2string( _temp296);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp338.last_plus_one
- _temp338.curr, _temp338.curr, _temp339.last_plus_one - _temp339.curr, _temp339.curr);}));
goto _LL311; _LL311:;}}}}; _pop_region(& _temp286);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_string obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp393= sd->name;
if( _temp393 == 0){ _throw( Null_Exception);} _temp393->v;}))).f2; if( sd->name
== 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp340= obj; xprintf("anonymous %.*ss are not allowed at top level",
_temp340.last_plus_one - _temp340.curr, _temp340.curr);})); return;}{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp341=
atts; if( _temp341 == 0){ _throw( Null_Exception);} _temp341->tl;})){ void*
_temp343=( void*)({ struct Cyc_List_List* _temp342= atts; if( _temp342 == 0){
_throw( Null_Exception);} _temp342->hd;}); int _temp351; _LL345: if( _temp343 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL346;} else{ goto _LL347;} _LL347: if((
unsigned int) _temp343 > 15u?*(( int*) _temp343) == Cyc_Absyn_Aligned_att: 0){
_LL352: _temp351=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp343)->f1;
goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL346: continue; _LL348:
continue; _LL350: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp354= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp353= atts; if( _temp353 == 0){ _throw(
Null_Exception);} _temp353->hd;})); struct _tagged_string _temp355= obj; struct
_tagged_string _temp356=* v; xprintf("bad attribute %.*s in %.*s %.*s definition",
_temp354.last_plus_one - _temp354.curr, _temp354.curr, _temp355.last_plus_one -
_temp355.curr, _temp355.curr, _temp356.last_plus_one - _temp356.curr, _temp356.curr);}));
goto _LL344; _LL344:;}}{ struct Cyc_List_List* tvs= sd->tvs; for( 0; tvs != 0;
tvs=({ struct Cyc_List_List* _temp357= tvs; if( _temp357 == 0){ _throw(
Null_Exception);} _temp357->tl;})){ struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp374= tvs; if( _temp374 == 0){ _throw( Null_Exception);}
_temp374->hd;}))->kind); void* _temp358=( void*) c->v; void* _temp366; _LL360:
if( _temp358 ==( void*) Cyc_Absyn_No_constr){ goto _LL361;} else{ goto _LL362;}
_LL362: if(( unsigned int) _temp358 > 1u?*(( int*) _temp358) == Cyc_Absyn_Eq_constr:
0){ _LL367: _temp366=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp358)->f1;
if( _temp366 ==( void*) Cyc_Absyn_MemKind){ goto _LL363;} else{ goto _LL364;}}
else{ goto _LL364;} _LL364: goto _LL365; _LL361:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp368=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp368[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp369; _temp369.tag= Cyc_Absyn_Eq_constr; _temp369.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp369;}); _temp368;}))); continue; _LL363:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp371= obj; struct
_tagged_string _temp372=* v; struct _tagged_string _temp373=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp370= tvs; if( _temp370 == 0){ _throw( Null_Exception);}
_temp370->hd;}))->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp371.last_plus_one - _temp371.curr, _temp371.curr, _temp372.last_plus_one -
_temp372.curr, _temp372.curr, _temp373.last_plus_one - _temp373.curr, _temp373.curr);}));
continue; _LL365: continue; _LL359:;}}{ void* _temp376=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp375= sd->name; if( _temp375 == 0){ _throw(
Null_Exception);} _temp375->v;}))).f1; struct Cyc_List_List* _temp384; struct
Cyc_List_List* _temp386; _LL378: if(( unsigned int) _temp376 > 1u?*(( int*)
_temp376) == Cyc_Absyn_Rel_n: 0){ _LL385: _temp384=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp376)->f1; if( _temp384 == 0){ goto _LL379;}
else{ goto _LL380;}} else{ goto _LL380;} _LL380: if(( unsigned int) _temp376 > 1u?*((
int*) _temp376) == Cyc_Absyn_Abs_n: 0){ _LL387: _temp386=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp376)->f1; if( _temp386 == 0){ goto _LL381;}
else{ goto _LL382;}} else{ goto _LL382;} _LL382: goto _LL383; _LL379: goto
_LL377; _LL381: goto _LL377; _LL383: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp388=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 43; _temp389;})); return; _LL377:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp390= sd->name; if( _temp390 == 0){
_throw( Null_Exception);} _temp390->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp391=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp391[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp392; _temp392.tag= Cyc_Absyn_Abs_n;
_temp392.f1= te->ns; _temp392;}); _temp391;}); Cyc_Tcutil_check_unique_tvars(
loc, sd->tvs);} struct _tuple4{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp444= sd->name;
if( _temp444 == 0){ _throw( Null_Exception);} _temp444->v;}))).f2; struct
_tagged_string obj=( struct _tagged_string)({ char* _temp442=( char*)"struct";
struct _tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 7; _temp443;}); Cyc_Tc_tcStructOrUniondecl(
te, ge, obj, loc, sd);{ struct Cyc_List_List* tvs= sd->tvs;{ struct _tuple4
_temp395=({ struct _tuple4 _temp394; _temp394.f1= sd->fields; _temp394.f2=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->structdecls, v); _temp394;}); struct Cyc_Core_Opt*
_temp405; struct Cyc_Core_Opt* _temp407; struct Cyc_Core_Opt* _temp409; struct
Cyc_Core_Opt* _temp411; struct Cyc_Core_Opt _temp413; struct Cyc_List_List*
_temp414; struct Cyc_Core_Opt* _temp416; struct Cyc_Core_Opt _temp418; struct
Cyc_Absyn_Structdecl** _temp419; struct Cyc_Core_Opt* _temp421; struct Cyc_Core_Opt
_temp423; struct Cyc_List_List* _temp424; struct Cyc_Core_Opt* _temp426; struct
Cyc_Core_Opt _temp428; struct Cyc_Absyn_Structdecl** _temp429; struct Cyc_Core_Opt*
_temp431; _LL397: _LL408: _temp407= _temp395.f1; if( _temp407 == 0){ goto _LL406;}
else{ goto _LL399;} _LL406: _temp405= _temp395.f2; if( _temp405 == 0){ goto
_LL398;} else{ goto _LL399;} _LL399: _LL412: _temp411= _temp395.f1; if( _temp411
== 0){ goto _LL401;} else{ _temp413=* _temp411; _LL415: _temp414=( struct Cyc_List_List*)
_temp413.v; goto _LL410;} _LL410: _temp409= _temp395.f2; if( _temp409 == 0){
goto _LL400;} else{ goto _LL401;} _LL401: _LL422: _temp421= _temp395.f1; if(
_temp421 == 0){ goto _LL403;} else{ _temp423=* _temp421; _LL425: _temp424=(
struct Cyc_List_List*) _temp423.v; goto _LL417;} _LL417: _temp416= _temp395.f2;
if( _temp416 == 0){ goto _LL403;} else{ _temp418=* _temp416; _LL420: _temp419=(
struct Cyc_Absyn_Structdecl**) _temp418.v; goto _LL402;} _LL403: _LL432:
_temp431= _temp395.f1; if( _temp431 == 0){ goto _LL427;} else{ goto _LL396;}
_LL427: _temp426= _temp395.f2; if( _temp426 == 0){ goto _LL396;} else{ _temp428=*
_temp426; _LL430: _temp429=( struct Cyc_Absyn_Structdecl**) _temp428.v; goto
_LL404;} _LL398: ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, v,({ struct Cyc_Absyn_Structdecl** _temp433=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp433[ 0]= sd; _temp433;}));
goto _LL396; _LL400: { struct Cyc_Absyn_Structdecl** sdp=({ struct Cyc_Absyn_Structdecl**
_temp434=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp434[ 0]=({ struct Cyc_Absyn_Structdecl* _temp435=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp435->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp435->name= sd->name; _temp435->tvs= tvs; _temp435->fields=
0; _temp435->attributes= 0; _temp435;}); _temp434;}); ge->structdecls=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct
Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)( ge->structdecls, v, sdp); Cyc_Tc_tcStructFields(
te, ge, loc, obj, _temp414, tvs);* sdp= sd; goto _LL396;} _LL402: { struct Cyc_Absyn_Structdecl*
sd0=* _temp419;* _temp419=({ struct Cyc_Absyn_Structdecl* _temp436=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp436->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp436->name= sd->name; _temp436->tvs= tvs; _temp436->fields=
0; _temp436->attributes= 0; _temp436;}); Cyc_Tc_tcStructFields( te, ge, loc, obj,
_temp424, tvs);* _temp419= sd0; _temp429= _temp419; goto _LL404;} _LL404: {
struct Cyc_Absyn_Structdecl* sd2= Cyc_Tcdecl_merge_structdecl(* _temp429, sd,
loc, Cyc_Tc_tc_msg); if( sd2 == 0){ return;} else{* _temp429=( struct Cyc_Absyn_Structdecl*)({
struct Cyc_Absyn_Structdecl* _temp437= sd2; if( _temp437 == 0){ _throw(
Null_Exception);} _temp437;}); sd=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp438= sd2; if( _temp438 == 0){ _throw( Null_Exception);} _temp438;}); goto
_LL396;}} _LL396:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple3* _temp439=( struct _tuple3*) GC_malloc( sizeof( struct
_tuple3)); _temp439->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct* _temp440=(
struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp440[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp441; _temp441.tag= Cyc_Tcenv_StructRes;
_temp441.f1= sd; _temp441;}); _temp440;}); _temp439->f2= 1; _temp439;}));}}
static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp491= ud->name;
if( _temp491 == 0){ _throw( Null_Exception);} _temp491->v;}))).f2; struct
_tagged_string obj=( struct _tagged_string)({ char* _temp489=( char*)"union";
struct _tagged_string _temp490; _temp490.curr= _temp489; _temp490.base= _temp489;
_temp490.last_plus_one= _temp489 + 6; _temp490;}); Cyc_Tc_tcStructOrUniondecl(
te, ge, obj, loc,( struct Cyc_Absyn_Structdecl*) ud);{ struct Cyc_List_List* tvs=
ud->tvs; struct _tuple4 _temp446=({ struct _tuple4 _temp445; _temp445.f1= ud->fields;
_temp445.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); _temp445;});
struct Cyc_Core_Opt* _temp456; struct Cyc_Core_Opt* _temp458; struct Cyc_Core_Opt*
_temp460; struct Cyc_Core_Opt* _temp462; struct Cyc_Core_Opt _temp464; struct
Cyc_List_List* _temp465; struct Cyc_Core_Opt* _temp467; struct Cyc_Core_Opt
_temp469; struct Cyc_Absyn_Uniondecl** _temp470; struct Cyc_Core_Opt* _temp472;
struct Cyc_Core_Opt _temp474; struct Cyc_List_List* _temp475; struct Cyc_Core_Opt*
_temp477; struct Cyc_Core_Opt _temp479; struct Cyc_Absyn_Uniondecl** _temp480;
struct Cyc_Core_Opt* _temp482; _LL448: _LL459: _temp458= _temp446.f1; if(
_temp458 == 0){ goto _LL457;} else{ goto _LL450;} _LL457: _temp456= _temp446.f2;
if( _temp456 == 0){ goto _LL449;} else{ goto _LL450;} _LL450: _LL463: _temp462=
_temp446.f1; if( _temp462 == 0){ goto _LL452;} else{ _temp464=* _temp462; _LL466:
_temp465=( struct Cyc_List_List*) _temp464.v; goto _LL461;} _LL461: _temp460=
_temp446.f2; if( _temp460 == 0){ goto _LL451;} else{ goto _LL452;} _LL452:
_LL473: _temp472= _temp446.f1; if( _temp472 == 0){ goto _LL454;} else{ _temp474=*
_temp472; _LL476: _temp475=( struct Cyc_List_List*) _temp474.v; goto _LL468;}
_LL468: _temp467= _temp446.f2; if( _temp467 == 0){ goto _LL454;} else{ _temp469=*
_temp467; _LL471: _temp470=( struct Cyc_Absyn_Uniondecl**) _temp469.v; goto
_LL453;} _LL454: _LL483: _temp482= _temp446.f1; if( _temp482 == 0){ goto _LL478;}
else{ goto _LL447;} _LL478: _temp477= _temp446.f2; if( _temp477 == 0){ goto
_LL447;} else{ _temp479=* _temp477; _LL481: _temp480=( struct Cyc_Absyn_Uniondecl**)
_temp479.v; goto _LL455;} _LL449: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp484=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp484[ 0]= ud; _temp484;})); goto _LL447; _LL451: { struct Cyc_Absyn_Uniondecl**
udp=({ struct Cyc_Absyn_Uniondecl** _temp485=( struct Cyc_Absyn_Uniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*)); _temp485[ 0]=({ struct Cyc_Absyn_Uniondecl*
_temp486=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp486->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp486->name= ud->name;
_temp486->tvs= tvs; _temp486->fields= 0; _temp486->attributes= ud->attributes;
_temp486;}); _temp485;}); ge->uniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl** data)) Cyc_Dict_insert)(
ge->uniondecls, v, udp); Cyc_Tc_tcStructFields( te, ge, loc, obj, _temp465, tvs);*
udp= ud; goto _LL447;} _LL453: { struct Cyc_Absyn_Uniondecl* ud0=* _temp470;*
_temp470=({ struct Cyc_Absyn_Uniondecl* _temp487=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp487->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp487->name= ud->name; _temp487->tvs= tvs; _temp487->fields=
0; _temp487->attributes= ud->attributes; _temp487;}); Cyc_Tc_tcStructFields( te,
ge, loc, obj, _temp475, tvs);* _temp470= ud0; _temp480= _temp470; goto _LL455;}
_LL455: { struct Cyc_Absyn_Uniondecl* ud2= Cyc_Tcdecl_merge_uniondecl(* _temp480,
ud, loc, Cyc_Tc_tc_msg); if( ud2 == 0){ return;} else{* _temp480=( struct Cyc_Absyn_Uniondecl*)({
struct Cyc_Absyn_Uniondecl* _temp488= ud2; if( _temp488 == 0){ _throw(
Null_Exception);} _temp488;}); goto _LL447;}} _LL447:;}} struct _tuple5{ struct
Cyc_Absyn_Tqual f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct _tagged_string obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* fs= fields; for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp492=
fs; if( _temp492 == 0){ _throw( Null_Exception);} _temp492->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp534= fs; if(
_temp534 == 0){ _throw( Null_Exception);} _temp534->hd;});{ struct Cyc_List_List*
tvs= f->tvs; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp493= tvs; if(
_temp493 == 0){ _throw( Null_Exception);} _temp493->tl;})){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp507= tvs; if( _temp507
== 0){ _throw( Null_Exception);} _temp507->hd;}); struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp494=( void*) c->v; void* _temp502; _LL496: if( _temp494 ==(
void*) Cyc_Absyn_No_constr){ goto _LL497;} else{ goto _LL498;} _LL498: if((
unsigned int) _temp494 > 1u?*(( int*) _temp494) == Cyc_Absyn_Eq_constr: 0){
_LL503: _temp502=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp494)->f1;
if( _temp502 ==( void*) Cyc_Absyn_MemKind){ goto _LL499;} else{ goto _LL500;}}
else{ goto _LL500;} _LL500: goto _LL501; _LL497:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp504=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp504[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp505; _temp505.tag= Cyc_Absyn_Eq_constr; _temp505.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp505;}); _temp504;}))); goto _LL495;
_LL499: Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp506=*(* f->name).f2;
xprintf("field %.*s abstracts type variable of kind M", _temp506.last_plus_one -
_temp506.curr, _temp506.curr);})); goto _LL495; _LL501: goto _LL495; _LL495:;}}{
struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp508= typs; if( _temp508 == 0){ _throw( Null_Exception);}
_temp508->tl;})){ Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp509= typs; if( _temp509 == 0){
_throw( Null_Exception);} _temp509->hd;}))).f2);}}{ void* _temp510=(* f->name).f1;
struct Cyc_List_List* _temp520; struct Cyc_List_List* _temp522; struct Cyc_List_List*
_temp524; _LL512: if(( unsigned int) _temp510 > 1u?*(( int*) _temp510) == Cyc_Absyn_Rel_n:
0){ _LL521: _temp520=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp510)->f1; if( _temp520 == 0){ goto _LL513;} else{ goto _LL514;}} else{ goto
_LL514;} _LL514: if(( unsigned int) _temp510 > 1u?*(( int*) _temp510) == Cyc_Absyn_Rel_n:
0){ _LL523: _temp522=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp510)->f1; goto _LL515;} else{ goto _LL516;} _LL516: if(( unsigned int)
_temp510 > 1u?*(( int*) _temp510) == Cyc_Absyn_Abs_n: 0){ _LL525: _temp524=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp510)->f1; goto
_LL517;} else{ goto _LL518;} _LL518: if( _temp510 ==( void*) Cyc_Absyn_Loc_n){
goto _LL519;} else{ goto _LL511;} _LL513: if( is_xtunion){(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp526=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp526[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp527; _temp527.tag= Cyc_Absyn_Abs_n; _temp527.f1= te->ns; _temp527;});
_temp526;});} else{(* f->name).f1=(* name).f1;} goto _LL511; _LL515: Cyc_Tcutil_terr(
f->loc,( struct _tagged_string)({ char* _temp528=( char*)"qualified declarations are not allowed";
struct _tagged_string _temp529; _temp529.curr= _temp528; _temp529.base= _temp528;
_temp529.last_plus_one= _temp528 + 39; _temp529;})); goto _LL511; _LL517: goto
_LL511; _LL519:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp530=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp530[ 0]=({ struct Cyc_Core_Impossible_struct _temp531; _temp531.tag= Cyc_Core_Impossible;
_temp531.f1=( struct _tagged_string)({ char* _temp532=( char*)"tcTunionFields: Loc_n";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 22; _temp533;}); _temp531;}); _temp530;}));
goto _LL511; _LL511:;}}}}{ struct Cyc_List_List* fields2; if( is_xtunion){ int
res= 1; struct Cyc_List_List* fs= Cyc_Tcdecl_sort_xtunion_fields( fields,& res,(*
name).f2, loc, Cyc_Tc_tc_msg); if( res){ fields2= fs;} else{ fields2= 0;}} else{
struct _RegionHandle _temp535= _new_region(); struct _RegionHandle* uprev_rgn=&
_temp535; _push_region( uprev_rgn);{ struct Cyc_List_List* prev_fields= 0;{
struct Cyc_List_List* fs= fields; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp536= fs; if( _temp536 == 0){ _throw( Null_Exception);} _temp536->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp540= fs; if( _temp540 == 0){ _throw( Null_Exception);} _temp540->hd;}); if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp537=*(* f->name).f2; struct _tagged_string _temp538= obj; xprintf("duplicate field name %.*s in %.*s",
_temp537.last_plus_one - _temp537.curr, _temp537.curr, _temp538.last_plus_one -
_temp538.curr, _temp538.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp539=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp539->hd=( void*)(* f->name).f2; _temp539->tl= prev_fields; _temp539;});}}}
fields2= fields;}; _pop_region(& _temp535);}{ struct Cyc_List_List* fs= fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp541= fs; if( _temp541 == 0){
_throw( Null_Exception);} _temp541->tl;})){ struct Cyc_Absyn_Tunionfield* f=(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp545= fs; if(
_temp545 == 0){ _throw( Null_Exception);} _temp545->hd;}); ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(* f->name).f2,({ struct
_tuple3* _temp542=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp542->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp543=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp543[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp544; _temp544.tag= Cyc_Tcenv_TunionRes; _temp544.f1=
tudres; _temp544.f2= f; _temp544;}); _temp543;}); _temp542->f2= 1; _temp542;}));}}
return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=( struct _tagged_string)({ char* _temp546=( char*)"xtunion";
struct _tagged_string _temp547; _temp547.curr= _temp546; _temp547.base= _temp546;
_temp547.last_plus_one= _temp546 + 8; _temp547;});} else{ obj=( struct
_tagged_string)({ char* _temp548=( char*)"tunion"; struct _tagged_string
_temp549; _temp549.curr= _temp548; _temp549.base= _temp548; _temp549.last_plus_one=
_temp548 + 7; _temp549;});}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp550= tvs2; if(
_temp550 == 0){ _throw( Null_Exception);} _temp550->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp567= tvs2; if( _temp567 ==
0){ _throw( Null_Exception);} _temp567->hd;}))->kind); void* _temp551=( void*) c->v;
void* _temp559; _LL553: if( _temp551 ==( void*) Cyc_Absyn_No_constr){ goto
_LL554;} else{ goto _LL555;} _LL555: if(( unsigned int) _temp551 > 1u?*(( int*)
_temp551) == Cyc_Absyn_Eq_constr: 0){ _LL560: _temp559=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp551)->f1; if( _temp559 ==( void*) Cyc_Absyn_MemKind){ goto _LL556;} else{
goto _LL557;}} else{ goto _LL557;} _LL557: goto _LL558; _LL554:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp561=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp561[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp562; _temp562.tag= Cyc_Absyn_Eq_constr; _temp562.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp562;}); _temp561;}))); goto _LL552;
_LL556: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp564= obj; struct
_tagged_string _temp565=* v; struct _tagged_string _temp566=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp563= tvs2; if( _temp563 == 0){ _throw( Null_Exception);}
_temp563->hd;}))->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp564.last_plus_one - _temp564.curr, _temp564.curr, _temp565.last_plus_one -
_temp565.curr, _temp565.curr, _temp566.last_plus_one - _temp566.curr, _temp566.curr);}));
goto _LL552; _LL558: goto _LL552; _LL552:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp568; _push_handler(& _temp568);{ int _temp570= 0; if( setjmp( _temp568.handler)){
_temp570= 1;} if( ! _temp570){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp569=( void*) _exn_thrown; void*
_temp572= _temp569; _LL574: if( _temp572 == Cyc_Dict_Absent){ goto _LL575;}
else{ goto _LL576;} _LL576: goto _LL577; _LL575: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp578= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp578.last_plus_one - _temp578.curr, _temp578.curr);})); return; _LL577:(
void) _throw( _temp572); _LL573:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp579= tud_opt; if( _temp579
== 0){ _throw( Null_Exception);} _temp579->v;})))->name;}} else{{ void* _temp580=(*
tud->name).f1; struct Cyc_List_List* _temp588; struct Cyc_List_List* _temp590;
_LL582: if(( unsigned int) _temp580 > 1u?*(( int*) _temp580) == Cyc_Absyn_Rel_n:
0){ _LL589: _temp588=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp580)->f1; if( _temp588 == 0){ goto _LL583;} else{ goto _LL584;}} else{ goto
_LL584;} _LL584: if(( unsigned int) _temp580 > 1u?*(( int*) _temp580) == Cyc_Absyn_Abs_n:
0){ _LL591: _temp590=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp580)->f1; goto _LL585;} else{ goto _LL586;} _LL586: goto _LL587; _LL583:(*
tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp592=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp592[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp593; _temp593.tag= Cyc_Absyn_Abs_n; _temp593.f1= te->ns; _temp593;});
_temp592;}); goto _LL581; _LL585: goto _LL587; _LL587: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp594=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp595; _temp595.curr= _temp594; _temp595.base= _temp594;
_temp595.last_plus_one= _temp594 + 43; _temp595;})); return; _LL581:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp597=({ struct
_tuple4 _temp596; _temp596.f1= tud->fields; _temp596.f2= tud_opt; _temp596;});
struct Cyc_Core_Opt* _temp607; struct Cyc_Core_Opt* _temp609; struct Cyc_Core_Opt*
_temp611; struct Cyc_Core_Opt* _temp613; struct Cyc_Core_Opt _temp615; struct
Cyc_List_List* _temp616; struct Cyc_List_List** _temp618; struct Cyc_Core_Opt*
_temp619; struct Cyc_Core_Opt _temp621; struct Cyc_Absyn_Tuniondecl** _temp622;
struct Cyc_Core_Opt* _temp624; struct Cyc_Core_Opt _temp626; struct Cyc_List_List*
_temp627; struct Cyc_List_List** _temp629; struct Cyc_Core_Opt* _temp630; struct
Cyc_Core_Opt _temp632; struct Cyc_Absyn_Tuniondecl** _temp633; struct Cyc_Core_Opt*
_temp635; _LL599: _LL610: _temp609= _temp597.f1; if( _temp609 == 0){ goto _LL608;}
else{ goto _LL601;} _LL608: _temp607= _temp597.f2; if( _temp607 == 0){ goto
_LL600;} else{ goto _LL601;} _LL601: _LL614: _temp613= _temp597.f1; if( _temp613
== 0){ goto _LL603;} else{ _temp615=* _temp613; _LL617: _temp616=( struct Cyc_List_List*)
_temp615.v; _temp618=&(* _temp597.f1).v; goto _LL612;} _LL612: _temp611=
_temp597.f2; if( _temp611 == 0){ goto _LL602;} else{ goto _LL603;} _LL603:
_LL625: _temp624= _temp597.f1; if( _temp624 == 0){ goto _LL605;} else{ _temp626=*
_temp624; _LL628: _temp627=( struct Cyc_List_List*) _temp626.v; _temp629=&(*
_temp597.f1).v; goto _LL620;} _LL620: _temp619= _temp597.f2; if( _temp619 == 0){
goto _LL605;} else{ _temp621=* _temp619; _LL623: _temp622=( struct Cyc_Absyn_Tuniondecl**)
_temp621.v; goto _LL604;} _LL605: _LL636: _temp635= _temp597.f1; if( _temp635 ==
0){ goto _LL631;} else{ goto _LL598;} _LL631: _temp630= _temp597.f2; if(
_temp630 == 0){ goto _LL598;} else{ _temp632=* _temp630; _LL634: _temp633=(
struct Cyc_Absyn_Tuniondecl**) _temp632.v; goto _LL606;} _LL600: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp637=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp637[ 0]= tud; _temp637;}));
goto _LL598; _LL602: { struct Cyc_Absyn_Tuniondecl** tudp=({ struct Cyc_Absyn_Tuniondecl**
_temp638=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp638[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp639=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp639->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp639->name= tud->name; _temp639->tvs= tvs; _temp639->fields=
0; _temp639->is_xtunion= tud->is_xtunion; _temp639;}); _temp638;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v, tudp);*
_temp618= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion, tud->name,*
_temp618, tvs, tud);* tudp= tud; goto _LL598;} _LL604: { struct Cyc_Absyn_Tuniondecl*
tud0=* _temp622;* _temp622=({ struct Cyc_Absyn_Tuniondecl* _temp640=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp640->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp640->name= tud->name; _temp640->tvs= tvs; _temp640->fields=
0; _temp640->is_xtunion= tud->is_xtunion; _temp640;});* _temp629= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp629, tvs, tud);* _temp622=
tud0; _temp633= _temp622; goto _LL606;} _LL606: { struct Cyc_Absyn_Tuniondecl*
tud2= Cyc_Tcdecl_merge_tuniondecl(* _temp633, tud, loc, Cyc_Tc_tc_msg); if( tud2
== 0){ return;} else{* _temp633=( struct Cyc_Absyn_Tuniondecl*)({ struct Cyc_Absyn_Tuniondecl*
_temp641= tud2; if( _temp641 == 0){ _throw( Null_Exception);} _temp641;}); goto
_LL598;}} _LL598:;}}}} static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl*
ed){ struct _tagged_string* v=(* ed->name).f2;{ void* _temp642=(* ed->name).f1;
struct Cyc_List_List* _temp650; struct Cyc_List_List* _temp652; _LL644: if((
unsigned int) _temp642 > 1u?*(( int*) _temp642) == Cyc_Absyn_Rel_n: 0){ _LL651:
_temp650=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp642)->f1;
if( _temp650 == 0){ goto _LL645;} else{ goto _LL646;}} else{ goto _LL646;}
_LL646: if(( unsigned int) _temp642 > 1u?*(( int*) _temp642) == Cyc_Absyn_Abs_n:
0){ _LL653: _temp652=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp642)->f1; if( _temp652 == 0){ goto _LL647;} else{ goto _LL648;}} else{ goto
_LL648;} _LL648: goto _LL649; _LL645: goto _LL643; _LL647: goto _LL643; _LL649:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp654=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp655; _temp655.curr= _temp654; _temp655.base= _temp654;
_temp655.last_plus_one= _temp654 + 43; _temp655;})); return; _LL643:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp656=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp656[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp657; _temp657.tag= Cyc_Absyn_Abs_n; _temp657.f1= te->ns; _temp657;});
_temp656;}); if( ed->fields != 0){ struct _RegionHandle _temp658= _new_region();
struct _RegionHandle* uprev_rgn=& _temp658; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp669= ed->fields; if(
_temp669 == 0){ _throw( Null_Exception);} _temp669->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp659= fs; if( _temp659 == 0){ _throw( Null_Exception);}
_temp659->tl;})){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp668= fs; if( _temp668 == 0){ _throw( Null_Exception);}
_temp668->hd;}); if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp660=*(* f->name).f2; xprintf("duplicate field name %.*s",
_temp660.last_plus_one - _temp660.curr, _temp660.curr);}));} else{ prev_fields=({
struct Cyc_List_List* _temp661=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp661->hd=( void*)(* f->name).f2;
_temp661->tl= prev_fields; _temp661;});} if( f->tag == 0){ f->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp662= f->tag; if( _temp662 ==
0){ _throw( Null_Exception);} _temp662;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp663=* v; struct _tagged_string _temp664=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp663.last_plus_one
- _temp663.curr, _temp663.curr, _temp664.last_plus_one - _temp664.curr, _temp664.curr);}));}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp667= f->tag; if( _temp667 == 0){ _throw( Null_Exception);}
_temp667;})); tag_count= t1 + 1;(* f->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp665=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp665[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp666; _temp666.tag= Cyc_Absyn_Abs_n;
_temp666.f1= te->ns; _temp666;}); _temp665;});}}}; _pop_region(& _temp658);}{
struct _handler_cons _temp670; _push_handler(& _temp670);{ int _temp672= 0; if(
setjmp( _temp670.handler)){ _temp672= 1;} if( ! _temp672){{ struct Cyc_Absyn_Enumdecl**
edp=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v); struct Cyc_Absyn_Enumdecl*
ed2= Cyc_Tcdecl_merge_enumdecl(* edp, ed, loc, Cyc_Tc_tc_msg); if( ed2 == 0){
_npop_handler( 0u); return;}* edp=( struct Cyc_Absyn_Enumdecl*)({ struct Cyc_Absyn_Enumdecl*
_temp673= ed2; if( _temp673 == 0){ _throw( Null_Exception);} _temp673;});};
_pop_handler();} else{ void* _temp671=( void*) _exn_thrown; void* _temp675=
_temp671; _LL677: if( _temp675 == Cyc_Dict_Absent){ goto _LL678;} else{ goto
_LL679;} _LL679: goto _LL680; _LL678: { struct Cyc_Absyn_Enumdecl** edp=({
struct Cyc_Absyn_Enumdecl** _temp681=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp681[ 0]= ed; _temp681;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, edp);
goto _LL676;} _LL680:( void) _throw( _temp675); _LL676:;}}} if( ed->fields != 0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp687= ed->fields; if( _temp687 == 0){ _throw( Null_Exception);} _temp687->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp682= fs; if( _temp682 == 0){
_throw( Null_Exception);} _temp682->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp686= fs; if( _temp686
== 0){ _throw( Null_Exception);} _temp686->hd;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple3* _temp683=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp683->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp684=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp684[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp685; _temp685.tag= Cyc_Tcenv_EnumRes;
_temp685.f1= ed; _temp685.f2= f; _temp685;}); _temp684;}); _temp683->f2= 1;
_temp683;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp688= sc; _LL690: if( _temp688 ==( void*) Cyc_Absyn_Static){
goto _LL691;} else{ goto _LL692;} _LL692: if( _temp688 ==( void*) Cyc_Absyn_Abstract){
goto _LL693;} else{ goto _LL694;} _LL694: if( _temp688 ==( void*) Cyc_Absyn_Public){
goto _LL695;} else{ goto _LL696;} _LL696: if( _temp688 ==( void*) Cyc_Absyn_Extern){
goto _LL697;} else{ goto _LL698;} _LL698: if( _temp688 ==( void*) Cyc_Absyn_ExternC){
goto _LL699;} else{ goto _LL689;} _LL691: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp700=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp701; _temp701.curr= _temp700; _temp701.base= _temp700;
_temp701.last_plus_one= _temp700 + 44; _temp701;})); return 0; _LL693: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp702=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp703; _temp703.curr= _temp702; _temp703.base= _temp702;
_temp703.last_plus_one= _temp702 + 46; _temp703;})); return 0; _LL695: return 1;
_LL697: return 1; _LL699: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp704=( char*)"nested extern \"C\" declaration"; struct _tagged_string
_temp705; _temp705.curr= _temp704; _temp705.base= _temp704; _temp705.last_plus_one=
_temp704 + 30; _temp705;})); return 1; _LL689:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp706= ds; if( _temp706 == 0){ _throw( Null_Exception);} _temp706->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp806= ds; if( _temp806 == 0){ _throw( Null_Exception);} _temp806->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp707=( void*) d->r; int
_temp731; struct Cyc_Absyn_Exp* _temp733; struct Cyc_Core_Opt* _temp735; struct
Cyc_Core_Opt* _temp737; struct Cyc_Absyn_Pat* _temp739; struct Cyc_Absyn_Vardecl*
_temp741; struct Cyc_Absyn_Fndecl* _temp743; struct Cyc_Absyn_Typedefdecl*
_temp745; struct Cyc_Absyn_Structdecl* _temp747; struct Cyc_Absyn_Uniondecl*
_temp749; struct Cyc_Absyn_Tuniondecl* _temp751; struct Cyc_Absyn_Enumdecl*
_temp753; struct Cyc_List_List* _temp755; struct _tagged_string* _temp757;
struct Cyc_List_List* _temp759; struct _tuple0* _temp761; struct _tuple0
_temp763; struct _tagged_string* _temp764; void* _temp766; struct Cyc_List_List*
_temp768; _LL709: if(*(( int*) _temp707) == Cyc_Absyn_Let_d){ _LL740: _temp739=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp707)->f1; goto
_LL738; _LL738: _temp737=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp707)->f2; goto _LL736; _LL736: _temp735=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp707)->f3; goto _LL734; _LL734: _temp733=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp707)->f4; goto _LL732;
_LL732: _temp731=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp707)->f5; goto
_LL710;} else{ goto _LL711;} _LL711: if(*(( int*) _temp707) == Cyc_Absyn_Var_d){
_LL742: _temp741=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp707)->f1; goto _LL712;} else{ goto _LL713;} _LL713: if(*(( int*) _temp707)
== Cyc_Absyn_Fn_d){ _LL744: _temp743=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp707)->f1; goto _LL714;} else{ goto _LL715;} _LL715: if(*(( int*) _temp707)
== Cyc_Absyn_Typedef_d){ _LL746: _temp745=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp707)->f1; goto _LL716;} else{ goto
_LL717;} _LL717: if(*(( int*) _temp707) == Cyc_Absyn_Struct_d){ _LL748: _temp747=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp707)->f1;
goto _LL718;} else{ goto _LL719;} _LL719: if(*(( int*) _temp707) == Cyc_Absyn_Union_d){
_LL750: _temp749=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp707)->f1; goto _LL720;} else{ goto _LL721;} _LL721: if(*(( int*) _temp707)
== Cyc_Absyn_Tunion_d){ _LL752: _temp751=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp707)->f1; goto _LL722;} else{ goto
_LL723;} _LL723: if(*(( int*) _temp707) == Cyc_Absyn_Enum_d){ _LL754: _temp753=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp707)->f1;
goto _LL724;} else{ goto _LL725;} _LL725: if(*(( int*) _temp707) == Cyc_Absyn_Namespace_d){
_LL758: _temp757=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp707)->f1; goto _LL756; _LL756: _temp755=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp707)->f2; goto _LL726;} else{ goto _LL727;}
_LL727: if(*(( int*) _temp707) == Cyc_Absyn_Using_d){ _LL762: _temp761=( struct
_tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp707)->f1; _temp763=* _temp761;
_LL767: _temp766= _temp763.f1; goto _LL765; _LL765: _temp764= _temp763.f2; goto
_LL760; _LL760: _temp759=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp707)->f2; goto _LL728;} else{ goto _LL729;} _LL729: if(*(( int*) _temp707)
== Cyc_Absyn_ExternC_d){ _LL769: _temp768=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp707)->f1; goto _LL730;} else{ goto _LL708;} _LL710: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp770=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp771; _temp771.curr= _temp770; _temp771.base= _temp770;
_temp771.last_plus_one= _temp770 + 47; _temp771;})); goto _LL708; _LL712: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp741->sc): 0){( void*)(
_temp741->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp741, check_var_init); goto _LL708; _LL714: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp743->sc): 0){( void*)( _temp743->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp743); goto _LL708; _LL716: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp745); goto _LL708; _LL718: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp747->sc): 0){( void*)( _temp747->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp747); goto _LL708; _LL720: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp749->sc): 0){( void*)( _temp749->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp749);
goto _LL708; _LL722: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp751->sc): 0){( void*)( _temp751->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp751); goto _LL708; _LL724: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp753->sc): 0){( void*)( _temp753->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp753);
goto _LL708; _LL726: { struct Cyc_List_List* ns= te->ns; struct Cyc_List_List*
ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp772=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp772->hd=( void*) _temp757;
_temp772->tl= 0; _temp772;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, _temp757)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
ge->namespaces, _temp757); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
ns2, Cyc_Tcenv_empty_genv());} te->ns= ns2; Cyc_Tc_tc_decls( te, _temp755,
in_externC, check_var_init); te->ns= ns; goto _LL708;} _LL728: { struct
_tagged_string* first; struct Cyc_List_List* rest;{ void* _temp773= _temp766;
struct Cyc_List_List* _temp785; struct Cyc_List_List* _temp787; struct Cyc_List_List*
_temp789; struct Cyc_List_List _temp791; struct Cyc_List_List* _temp792; struct
_tagged_string* _temp794; struct Cyc_List_List* _temp796; struct Cyc_List_List
_temp798; struct Cyc_List_List* _temp799; struct _tagged_string* _temp801;
_LL775: if( _temp773 ==( void*) Cyc_Absyn_Loc_n){ goto _LL776;} else{ goto
_LL777;} _LL777: if(( unsigned int) _temp773 > 1u?*(( int*) _temp773) == Cyc_Absyn_Rel_n:
0){ _LL786: _temp785=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp773)->f1; if( _temp785 == 0){ goto _LL778;} else{ goto _LL779;}} else{ goto
_LL779;} _LL779: if(( unsigned int) _temp773 > 1u?*(( int*) _temp773) == Cyc_Absyn_Abs_n:
0){ _LL788: _temp787=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp773)->f1; if( _temp787 == 0){ goto _LL780;} else{ goto _LL781;}} else{ goto
_LL781;} _LL781: if(( unsigned int) _temp773 > 1u?*(( int*) _temp773) == Cyc_Absyn_Rel_n:
0){ _LL790: _temp789=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp773)->f1; if( _temp789 == 0){ goto _LL783;} else{ _temp791=* _temp789;
_LL795: _temp794=( struct _tagged_string*) _temp791.hd; goto _LL793; _LL793:
_temp792=( struct Cyc_List_List*) _temp791.tl; goto _LL782;}} else{ goto _LL783;}
_LL783: if(( unsigned int) _temp773 > 1u?*(( int*) _temp773) == Cyc_Absyn_Abs_n:
0){ _LL797: _temp796=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp773)->f1; if( _temp796 == 0){ goto _LL774;} else{ _temp798=* _temp796;
_LL802: _temp801=( struct _tagged_string*) _temp798.hd; goto _LL800; _LL800:
_temp799=( struct Cyc_List_List*) _temp798.tl; goto _LL784;}} else{ goto _LL774;}
_LL776: goto _LL778; _LL778: goto _LL780; _LL780: first= _temp764; rest= 0; goto
_LL774; _LL782: _temp801= _temp794; _temp799= _temp792; goto _LL784; _LL784:
first= _temp801; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp799,({ struct Cyc_List_List*
_temp803=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp803->hd=( void*) _temp764; _temp803->tl= 0; _temp803;})); goto _LL774;
_LL774:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace( te, loc,
first, rest); ge->availables=({ struct Cyc_List_List* _temp804=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp804->hd=( void*) ns2; _temp804->tl=
ge->availables; _temp804;}); Cyc_Tc_tc_decls( te, _temp759, in_externC,
check_var_init); ge->availables=({ struct Cyc_List_List* _temp805= ge->availables;
if( _temp805 == 0){ _throw( Null_Exception);} _temp805->tl;}); goto _LL708;}}
_LL730: Cyc_Tc_tc_decls( te, _temp768, 1, check_var_init); goto _LL708; _LL708:;}}
static char _temp809[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp809, _temp809, _temp809 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, int check_var_init, struct Cyc_List_List* ds){ if(
add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp810->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp811=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp811->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp812=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp812[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp813; _temp813.tag= Cyc_Absyn_Namespace_d;
_temp813.f1= Cyc_Tc_cyc_ns; _temp813.f2= ds; _temp813;}); _temp812;})); _temp811->loc=
0; _temp811;}); _temp810->tl= 0; _temp810;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t; static
int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d);
static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple6{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp814=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp824; struct Cyc_List_List* _temp826; struct Cyc_List_List**
_temp828; struct _tuple0* _temp829; struct Cyc_List_List* _temp831; struct Cyc_List_List**
_temp833; struct _tagged_string* _temp834; _LL816: if(*(( int*) _temp814) == Cyc_Absyn_Var_d){
_LL825: _temp824=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp814)->f1; goto _LL817;} else{ goto _LL818;} _LL818: if(*(( int*) _temp814)
== Cyc_Absyn_Using_d){ _LL830: _temp829=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp814)->f1; goto _LL827; _LL827: _temp826=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Using_d_struct*) _temp814)->f2; _temp828=&(( struct Cyc_Absyn_Using_d_struct*)
_temp814)->f2; goto _LL819;} else{ goto _LL820;} _LL820: if(*(( int*) _temp814)
== Cyc_Absyn_Namespace_d){ _LL835: _temp834=( struct _tagged_string*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp814)->f1; goto _LL832; _LL832: _temp831=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp814)->f2;
_temp833=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp814)->f2; goto _LL821;}
else{ goto _LL822;} _LL822: goto _LL823; _LL817: if(( void*) _temp824->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp838; struct
_tagged_string* _temp839; void* _temp841; struct _tuple0* _temp836= _temp824->name;
_temp838=* _temp836; _LL842: _temp841= _temp838.f1; goto _LL840; _LL840:
_temp839= _temp838.f2; goto _LL837; _LL837: { struct Cyc_List_List* ns;{ void*
_temp843= _temp841; struct Cyc_List_List* _temp851; struct Cyc_List_List*
_temp853; _LL845: if( _temp843 ==( void*) Cyc_Absyn_Loc_n){ goto _LL846;} else{
goto _LL847;} _LL847: if(( unsigned int) _temp843 > 1u?*(( int*) _temp843) ==
Cyc_Absyn_Rel_n: 0){ _LL852: _temp851=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp843)->f1; goto _LL848;} else{ goto _LL849;} _LL849: if(( unsigned int)
_temp843 > 1u?*(( int*) _temp843) == Cyc_Absyn_Abs_n: 0){ _LL854: _temp853=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp843)->f1; goto
_LL850;} else{ goto _LL844;} _LL846: ns= 0; goto _LL844; _LL848: ns= _temp851;
goto _LL844; _LL850: ns= _temp853; goto _LL844; _LL844:;}{ struct _tuple6* nsenv=((
struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
env, ns); struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; int needed=(*(( struct _tuple3*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries,
_temp839)).f2; if( ! needed){(* nsenv).f2=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)((* nsenv).f2, _temp839);} return
needed;}}} _LL819: _temp833= _temp828; goto _LL821; _LL821:* _temp833= Cyc_Tc_treeshake_f(
env,* _temp833); return 1; _LL823: return 1; _LL815:;} static int Cyc_Tc_treeshake_remove_f(
struct Cyc_Set_Set* set, struct _tagged_string* x, struct _tuple3* y){ return !((
int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( set,
x);} static void Cyc_Tc_treeshake_remove( struct Cyc_List_List* ns, struct
_tuple6* nsenv){ struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; struct Cyc_Set_Set* set=(*
nsenv).f2; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*,
struct _tagged_string*, struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f, set, ge->ordinaries);} static
struct _tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp855=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp855->f1= ge; _temp855->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp855;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* env=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* ds1= Cyc_Tc_treeshake_f(
env, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, env); return ds1;}