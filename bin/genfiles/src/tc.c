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
extern char Cyc_Tcdecl_Incompatible[ 17u]; struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl* base; struct Cyc_Absyn_Tunionfield* field; } ;
typedef struct Cyc_Tcdecl_Xtunionfielddecl* Cyc_Tcdecl_xtunionfielddecl_t;
extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl( struct Cyc_Absyn_Structdecl*
d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment* loc, struct
_tagged_string* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_string* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
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
_temp541= fs; if( _temp541 == 0){ _throw( Null_Exception);} _temp541->hd;}); if(((
int(*)( int(* compare)( struct _tagged_string*, struct _tagged_string*), struct
Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp,
prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp537=*(* f->name).f2; struct _tagged_string _temp538= obj; xprintf("duplicate field name %.*s in %.*s",
_temp537.last_plus_one - _temp537.curr, _temp537.curr, _temp538.last_plus_one -
_temp538.curr, _temp538.curr);}));} else{ prev_fields=({ struct Cyc_List_List*
_temp539=( struct Cyc_List_List*) _region_malloc( uprev_rgn, sizeof( struct Cyc_List_List));
_temp539->hd=( void*)(* f->name).f2; _temp539->tl= prev_fields; _temp539;});}
if(( void*) f->sc !=( void*) Cyc_Absyn_Public){ Cyc_Tcutil_warn( loc,({ struct
_tagged_string _temp540=*(* f->name).f2; xprintf("ignoring scope of field %.*s",
_temp540.last_plus_one - _temp540.curr, _temp540.curr);}));( void*)( f->sc=(
void*)(( void*) Cyc_Absyn_Public));}}} fields2= fields;}; _pop_region(& _temp535);}{
struct Cyc_List_List* fs= fields; for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp542= fs; if( _temp542 == 0){ _throw( Null_Exception);} _temp542->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp546= fs; if( _temp546 == 0){ _throw( Null_Exception);} _temp546->hd;}); ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple3* data)) Cyc_Dict_insert)( ge->ordinaries,(* f->name).f2,({ struct
_tuple3* _temp543=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp543->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp544=( struct Cyc_Tcenv_TunionRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp544[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp545; _temp545.tag= Cyc_Tcenv_TunionRes; _temp545.f1=
tudres; _temp545.f2= f; _temp545;}); _temp544;}); _temp543->f2= 1; _temp543;}));}}
return fields2;}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct _tagged_string obj; if(
tud->is_xtunion){ obj=( struct _tagged_string)({ char* _temp547=( char*)"xtunion";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 8; _temp548;});} else{ obj=( struct
_tagged_string)({ char* _temp549=( char*)"tunion"; struct _tagged_string
_temp550; _temp550.curr= _temp549; _temp550.base= _temp549; _temp550.last_plus_one=
_temp549 + 7; _temp550;});}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp551= tvs2; if(
_temp551 == 0){ _throw( Null_Exception);} _temp551->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp568= tvs2; if( _temp568 ==
0){ _throw( Null_Exception);} _temp568->hd;}))->kind); void* _temp552=( void*) c->v;
void* _temp560; _LL554: if( _temp552 ==( void*) Cyc_Absyn_No_constr){ goto
_LL555;} else{ goto _LL556;} _LL556: if(( unsigned int) _temp552 > 1u?*(( int*)
_temp552) == Cyc_Absyn_Eq_constr: 0){ _LL561: _temp560=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp552)->f1; if( _temp560 ==( void*) Cyc_Absyn_MemKind){ goto _LL557;} else{
goto _LL558;}} else{ goto _LL558;} _LL558: goto _LL559; _LL555:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp562=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp562[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp563; _temp563.tag= Cyc_Absyn_Eq_constr; _temp563.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp563;}); _temp562;}))); goto _LL553;
_LL557: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp565= obj; struct
_tagged_string _temp566=* v; struct _tagged_string _temp567=*(( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp564= tvs2; if( _temp564 == 0){ _throw( Null_Exception);}
_temp564->hd;}))->name; xprintf("%.*s %.*s attempts to abstract type variable %.*s of kind M",
_temp565.last_plus_one - _temp565.curr, _temp565.curr, _temp566.last_plus_one -
_temp566.curr, _temp566.curr, _temp567.last_plus_one - _temp567.curr, _temp567.curr);}));
goto _LL553; _LL559: goto _LL553; _LL553:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp569; _push_handler(& _temp569);{ int _temp571= 0; if( setjmp( _temp569.handler)){
_temp571= 1;} if( ! _temp571){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc,
tud->name);; _pop_handler();} else{ void* _temp570=( void*) _exn_thrown; void*
_temp573= _temp570; _LL575: if( _temp573 == Cyc_Dict_Absent){ goto _LL576;}
else{ goto _LL577;} _LL577: goto _LL578; _LL576: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp579= Cyc_Absynpp_qvar2string( tud->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp579.last_plus_one - _temp579.curr, _temp579.curr);})); return; _LL578:(
void) _throw( _temp573); _LL574:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp580= tud_opt; if( _temp580
== 0){ _throw( Null_Exception);} _temp580->v;})))->name;} else{(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp581=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp581[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp582; _temp582.tag= Cyc_Absyn_Abs_n; _temp582.f1= te->ns; _temp582;});
_temp581;});}} else{{ void* _temp583=(* tud->name).f1; struct Cyc_List_List*
_temp591; struct Cyc_List_List* _temp593; _LL585: if(( unsigned int) _temp583 >
1u?*(( int*) _temp583) == Cyc_Absyn_Rel_n: 0){ _LL592: _temp591=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp583)->f1; if( _temp591 == 0){ goto _LL586;}
else{ goto _LL587;}} else{ goto _LL587;} _LL587: if(( unsigned int) _temp583 > 1u?*((
int*) _temp583) == Cyc_Absyn_Abs_n: 0){ _LL594: _temp593=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp583)->f1; goto _LL588;} else{ goto _LL589;}
_LL589: goto _LL590; _LL586:(* tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp595=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp595[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp596; _temp596.tag= Cyc_Absyn_Abs_n;
_temp596.f1= te->ns; _temp596;}); _temp595;}); goto _LL584; _LL588: goto _LL590;
_LL590: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp597=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp598; _temp598.curr= _temp597; _temp598.base= _temp597;
_temp598.last_plus_one= _temp597 + 43; _temp598;})); return; _LL584:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct _tuple4 _temp600=({ struct
_tuple4 _temp599; _temp599.f1= tud->fields; _temp599.f2= tud_opt; _temp599;});
struct Cyc_Core_Opt* _temp610; struct Cyc_Core_Opt* _temp612; struct Cyc_Core_Opt*
_temp614; struct Cyc_Core_Opt* _temp616; struct Cyc_Core_Opt _temp618; struct
Cyc_List_List* _temp619; struct Cyc_List_List** _temp621; struct Cyc_Core_Opt*
_temp622; struct Cyc_Core_Opt _temp624; struct Cyc_Absyn_Tuniondecl** _temp625;
struct Cyc_Core_Opt* _temp627; struct Cyc_Core_Opt _temp629; struct Cyc_List_List*
_temp630; struct Cyc_List_List** _temp632; struct Cyc_Core_Opt* _temp633; struct
Cyc_Core_Opt _temp635; struct Cyc_Absyn_Tuniondecl** _temp636; struct Cyc_Core_Opt*
_temp638; _LL602: _LL613: _temp612= _temp600.f1; if( _temp612 == 0){ goto _LL611;}
else{ goto _LL604;} _LL611: _temp610= _temp600.f2; if( _temp610 == 0){ goto
_LL603;} else{ goto _LL604;} _LL604: _LL617: _temp616= _temp600.f1; if( _temp616
== 0){ goto _LL606;} else{ _temp618=* _temp616; _LL620: _temp619=( struct Cyc_List_List*)
_temp618.v; _temp621=&(* _temp600.f1).v; goto _LL615;} _LL615: _temp614=
_temp600.f2; if( _temp614 == 0){ goto _LL605;} else{ goto _LL606;} _LL606:
_LL628: _temp627= _temp600.f1; if( _temp627 == 0){ goto _LL608;} else{ _temp629=*
_temp627; _LL631: _temp630=( struct Cyc_List_List*) _temp629.v; _temp632=&(*
_temp600.f1).v; goto _LL623;} _LL623: _temp622= _temp600.f2; if( _temp622 == 0){
goto _LL608;} else{ _temp624=* _temp622; _LL626: _temp625=( struct Cyc_Absyn_Tuniondecl**)
_temp624.v; goto _LL607;} _LL608: _LL639: _temp638= _temp600.f1; if( _temp638 ==
0){ goto _LL634;} else{ goto _LL601;} _LL634: _temp633= _temp600.f2; if(
_temp633 == 0){ goto _LL601;} else{ _temp635=* _temp633; _LL637: _temp636=(
struct Cyc_Absyn_Tuniondecl**) _temp635.v; goto _LL609;} _LL603: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp640=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp640[ 0]= tud; _temp640;}));
goto _LL601; _LL605: { struct Cyc_Absyn_Tuniondecl** tudp=({ struct Cyc_Absyn_Tuniondecl**
_temp641=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp641[ 0]=({ struct Cyc_Absyn_Tuniondecl* _temp642=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp642->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp642->name= tud->name; _temp642->tvs= tvs; _temp642->fields=
0; _temp642->is_xtunion= tud->is_xtunion; _temp642;}); _temp641;}); ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v, tudp);*
_temp621= Cyc_Tc_tcTunionFields( te, ge, loc, obj, tud->is_xtunion, tud->name,*
_temp621, tvs, tud);* tudp= tud; goto _LL601;} _LL607: { struct Cyc_Absyn_Tuniondecl*
tud0=* _temp625;* _temp625=({ struct Cyc_Absyn_Tuniondecl* _temp643=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp643->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp643->name= tud->name; _temp643->tvs= tvs; _temp643->fields=
0; _temp643->is_xtunion= tud->is_xtunion; _temp643;});* _temp632= Cyc_Tc_tcTunionFields(
te, ge, loc, obj, tud->is_xtunion, tud->name,* _temp632, tvs, tud);* _temp625=
tud0; _temp636= _temp625; goto _LL609;} _LL609: { struct Cyc_Absyn_Tuniondecl*
tud2= Cyc_Tcdecl_merge_tuniondecl(* _temp636, tud, loc, Cyc_Tc_tc_msg); if( tud2
== 0){ return;} else{* _temp636=( struct Cyc_Absyn_Tuniondecl*)({ struct Cyc_Absyn_Tuniondecl*
_temp644= tud2; if( _temp644 == 0){ _throw( Null_Exception);} _temp644;}); goto
_LL601;}} _LL601:;}}}} static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl*
ed){ struct _tagged_string* v=(* ed->name).f2;{ void* _temp645=(* ed->name).f1;
struct Cyc_List_List* _temp653; struct Cyc_List_List* _temp655; _LL647: if((
unsigned int) _temp645 > 1u?*(( int*) _temp645) == Cyc_Absyn_Rel_n: 0){ _LL654:
_temp653=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp645)->f1;
if( _temp653 == 0){ goto _LL648;} else{ goto _LL649;}} else{ goto _LL649;}
_LL649: if(( unsigned int) _temp645 > 1u?*(( int*) _temp645) == Cyc_Absyn_Abs_n:
0){ _LL656: _temp655=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp645)->f1; if( _temp655 == 0){ goto _LL650;} else{ goto _LL651;}} else{ goto
_LL651;} _LL651: goto _LL652; _LL648: goto _LL646; _LL650: goto _LL646; _LL652:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp657=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp658; _temp658.curr= _temp657; _temp658.base= _temp657;
_temp658.last_plus_one= _temp657 + 43; _temp658;})); return; _LL646:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp659=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp659[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp660; _temp660.tag= Cyc_Absyn_Abs_n; _temp660.f1= te->ns; _temp660;});
_temp659;}); if( ed->fields != 0){ struct _RegionHandle _temp661= _new_region();
struct _RegionHandle* uprev_rgn=& _temp661; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List*
fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp672= ed->fields; if(
_temp672 == 0){ _throw( Null_Exception);} _temp672->v;}); for( 0; fs != 0; fs=({
struct Cyc_List_List* _temp662= fs; if( _temp662 == 0){ _throw( Null_Exception);}
_temp662->tl;})){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp671= fs; if( _temp671 == 0){ _throw( Null_Exception);}
_temp671->hd;}); if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp663=*(* f->name).f2; xprintf("duplicate field name %.*s",
_temp663.last_plus_one - _temp663.curr, _temp663.curr);}));} else{ prev_fields=({
struct Cyc_List_List* _temp664=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp664->hd=( void*)(* f->name).f2;
_temp664->tl= prev_fields; _temp664;});} if( f->tag == 0){ f->tag=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp665= f->tag; if( _temp665 ==
0){ _throw( Null_Exception);} _temp665;}))){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp666=* v; struct _tagged_string _temp667=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp666.last_plus_one
- _temp666.curr, _temp666.curr, _temp667.last_plus_one - _temp667.curr, _temp667.curr);}));}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp670= f->tag; if( _temp670 == 0){ _throw( Null_Exception);}
_temp670;})); tag_count= t1 + 1;(* f->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp668=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp668[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp669; _temp669.tag= Cyc_Absyn_Abs_n;
_temp669.f1= te->ns; _temp669;}); _temp668;});}}}; _pop_region(& _temp661);}{
struct _handler_cons _temp673; _push_handler(& _temp673);{ int _temp675= 0; if(
setjmp( _temp673.handler)){ _temp675= 1;} if( ! _temp675){{ struct Cyc_Absyn_Enumdecl**
edp=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v); struct Cyc_Absyn_Enumdecl*
ed2= Cyc_Tcdecl_merge_enumdecl(* edp, ed, loc, Cyc_Tc_tc_msg); if( ed2 == 0){
_npop_handler( 0u); return;}* edp=( struct Cyc_Absyn_Enumdecl*)({ struct Cyc_Absyn_Enumdecl*
_temp676= ed2; if( _temp676 == 0){ _throw( Null_Exception);} _temp676;});};
_pop_handler();} else{ void* _temp674=( void*) _exn_thrown; void* _temp678=
_temp674; _LL680: if( _temp678 == Cyc_Dict_Absent){ goto _LL681;} else{ goto
_LL682;} _LL682: goto _LL683; _LL681: { struct Cyc_Absyn_Enumdecl** edp=({
struct Cyc_Absyn_Enumdecl** _temp684=( struct Cyc_Absyn_Enumdecl**) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp684[ 0]= ed; _temp684;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v, edp);
goto _LL679;} _LL683:( void) _throw( _temp678); _LL679:;}}} if( ed->fields != 0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp690= ed->fields; if( _temp690 == 0){ _throw( Null_Exception);} _temp690->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp685= fs; if( _temp685 == 0){
_throw( Null_Exception);} _temp685->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp689= fs; if( _temp689
== 0){ _throw( Null_Exception);} _temp689->hd;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple3* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple3* _temp686=( struct _tuple3*)
GC_malloc( sizeof( struct _tuple3)); _temp686->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp687=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp687[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp688; _temp688.tag= Cyc_Tcenv_EnumRes;
_temp688.f1= ed; _temp688.f2= f; _temp688;}); _temp687;}); _temp686->f2= 1;
_temp686;}));}}} static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment*
loc, void* sc){ void* _temp691= sc; _LL693: if( _temp691 ==( void*) Cyc_Absyn_Static){
goto _LL694;} else{ goto _LL695;} _LL695: if( _temp691 ==( void*) Cyc_Absyn_Abstract){
goto _LL696;} else{ goto _LL697;} _LL697: if( _temp691 ==( void*) Cyc_Absyn_Public){
goto _LL698;} else{ goto _LL699;} _LL699: if( _temp691 ==( void*) Cyc_Absyn_Extern){
goto _LL700;} else{ goto _LL701;} _LL701: if( _temp691 ==( void*) Cyc_Absyn_ExternC){
goto _LL702;} else{ goto _LL692;} _LL694: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp703=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp704; _temp704.curr= _temp703; _temp704.base= _temp703;
_temp704.last_plus_one= _temp703 + 44; _temp704;})); return 0; _LL696: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp705=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 46; _temp706;})); return 0; _LL698: return 1;
_LL700: return 1; _LL702: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp707=( char*)"nested extern \"C\" declaration"; struct _tagged_string
_temp708; _temp708.curr= _temp707; _temp708.base= _temp707; _temp708.last_plus_one=
_temp707 + 30; _temp708;})); return 1; _LL692:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC, int
check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct
Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns);
struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp709= ds; if( _temp709 == 0){ _throw( Null_Exception);} _temp709->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp809= ds; if( _temp809 == 0){ _throw( Null_Exception);} _temp809->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp710=( void*) d->r; int
_temp734; struct Cyc_Absyn_Exp* _temp736; struct Cyc_Core_Opt* _temp738; struct
Cyc_Core_Opt* _temp740; struct Cyc_Absyn_Pat* _temp742; struct Cyc_Absyn_Vardecl*
_temp744; struct Cyc_Absyn_Fndecl* _temp746; struct Cyc_Absyn_Typedefdecl*
_temp748; struct Cyc_Absyn_Structdecl* _temp750; struct Cyc_Absyn_Uniondecl*
_temp752; struct Cyc_Absyn_Tuniondecl* _temp754; struct Cyc_Absyn_Enumdecl*
_temp756; struct Cyc_List_List* _temp758; struct _tagged_string* _temp760;
struct Cyc_List_List* _temp762; struct _tuple0* _temp764; struct _tuple0
_temp766; struct _tagged_string* _temp767; void* _temp769; struct Cyc_List_List*
_temp771; _LL712: if(*(( int*) _temp710) == Cyc_Absyn_Let_d){ _LL743: _temp742=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp710)->f1; goto
_LL741; _LL741: _temp740=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp710)->f2; goto _LL739; _LL739: _temp738=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp710)->f3; goto _LL737; _LL737: _temp736=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp710)->f4; goto _LL735;
_LL735: _temp734=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp710)->f5; goto
_LL713;} else{ goto _LL714;} _LL714: if(*(( int*) _temp710) == Cyc_Absyn_Var_d){
_LL745: _temp744=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp710)->f1; goto _LL715;} else{ goto _LL716;} _LL716: if(*(( int*) _temp710)
== Cyc_Absyn_Fn_d){ _LL747: _temp746=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp710)->f1; goto _LL717;} else{ goto _LL718;} _LL718: if(*(( int*) _temp710)
== Cyc_Absyn_Typedef_d){ _LL749: _temp748=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp710)->f1; goto _LL719;} else{ goto
_LL720;} _LL720: if(*(( int*) _temp710) == Cyc_Absyn_Struct_d){ _LL751: _temp750=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp710)->f1;
goto _LL721;} else{ goto _LL722;} _LL722: if(*(( int*) _temp710) == Cyc_Absyn_Union_d){
_LL753: _temp752=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp710)->f1; goto _LL723;} else{ goto _LL724;} _LL724: if(*(( int*) _temp710)
== Cyc_Absyn_Tunion_d){ _LL755: _temp754=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp710)->f1; goto _LL725;} else{ goto
_LL726;} _LL726: if(*(( int*) _temp710) == Cyc_Absyn_Enum_d){ _LL757: _temp756=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp710)->f1;
goto _LL727;} else{ goto _LL728;} _LL728: if(*(( int*) _temp710) == Cyc_Absyn_Namespace_d){
_LL761: _temp760=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp710)->f1; goto _LL759; _LL759: _temp758=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp710)->f2; goto _LL729;} else{ goto _LL730;}
_LL730: if(*(( int*) _temp710) == Cyc_Absyn_Using_d){ _LL765: _temp764=( struct
_tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp710)->f1; _temp766=* _temp764;
_LL770: _temp769= _temp766.f1; goto _LL768; _LL768: _temp767= _temp766.f2; goto
_LL763; _LL763: _temp762=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp710)->f2; goto _LL731;} else{ goto _LL732;} _LL732: if(*(( int*) _temp710)
== Cyc_Absyn_ExternC_d){ _LL772: _temp771=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp710)->f1; goto _LL733;} else{ goto _LL711;} _LL713: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp773=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp774; _temp774.curr= _temp773; _temp774.base= _temp773;
_temp774.last_plus_one= _temp773 + 47; _temp774;})); goto _LL711; _LL715: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp744->sc): 0){( void*)(
_temp744->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp744, check_var_init); goto _LL711; _LL717: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp746->sc): 0){( void*)( _temp746->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp746); goto _LL711; _LL719: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp748); goto _LL711; _LL721: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp750->sc): 0){( void*)( _temp750->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp750); goto _LL711; _LL723: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp752->sc): 0){( void*)( _temp752->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp752);
goto _LL711; _LL725: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp754->sc): 0){( void*)( _temp754->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp754); goto _LL711; _LL727: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp756->sc): 0){( void*)( _temp756->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp756);
goto _LL711; _LL729: { struct Cyc_List_List* ns= te->ns; struct Cyc_List_List*
ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp775->hd=( void*) _temp760;
_temp775->tl= 0; _temp775;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, _temp760)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
ge->namespaces, _temp760); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
ns2, Cyc_Tcenv_empty_genv());} te->ns= ns2; Cyc_Tc_tc_decls( te, _temp758,
in_externC, check_var_init); te->ns= ns; goto _LL711;} _LL731: { struct
_tagged_string* first; struct Cyc_List_List* rest;{ void* _temp776= _temp769;
struct Cyc_List_List* _temp788; struct Cyc_List_List* _temp790; struct Cyc_List_List*
_temp792; struct Cyc_List_List _temp794; struct Cyc_List_List* _temp795; struct
_tagged_string* _temp797; struct Cyc_List_List* _temp799; struct Cyc_List_List
_temp801; struct Cyc_List_List* _temp802; struct _tagged_string* _temp804;
_LL778: if( _temp776 ==( void*) Cyc_Absyn_Loc_n){ goto _LL779;} else{ goto
_LL780;} _LL780: if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Rel_n:
0){ _LL789: _temp788=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp776)->f1; if( _temp788 == 0){ goto _LL781;} else{ goto _LL782;}} else{ goto
_LL782;} _LL782: if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Abs_n:
0){ _LL791: _temp790=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp776)->f1; if( _temp790 == 0){ goto _LL783;} else{ goto _LL784;}} else{ goto
_LL784;} _LL784: if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Rel_n:
0){ _LL793: _temp792=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp776)->f1; if( _temp792 == 0){ goto _LL786;} else{ _temp794=* _temp792;
_LL798: _temp797=( struct _tagged_string*) _temp794.hd; goto _LL796; _LL796:
_temp795=( struct Cyc_List_List*) _temp794.tl; goto _LL785;}} else{ goto _LL786;}
_LL786: if(( unsigned int) _temp776 > 1u?*(( int*) _temp776) == Cyc_Absyn_Abs_n:
0){ _LL800: _temp799=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp776)->f1; if( _temp799 == 0){ goto _LL777;} else{ _temp801=* _temp799;
_LL805: _temp804=( struct _tagged_string*) _temp801.hd; goto _LL803; _LL803:
_temp802=( struct Cyc_List_List*) _temp801.tl; goto _LL787;}} else{ goto _LL777;}
_LL779: goto _LL781; _LL781: goto _LL783; _LL783: first= _temp767; rest= 0; goto
_LL777; _LL785: _temp804= _temp797; _temp802= _temp795; goto _LL787; _LL787:
first= _temp804; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp802,({ struct Cyc_List_List*
_temp806=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp806->hd=( void*) _temp767; _temp806->tl= 0; _temp806;})); goto _LL777;
_LL777:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace( te, loc,
first, rest); ge->availables=({ struct Cyc_List_List* _temp807=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp807->hd=( void*) ns2; _temp807->tl=
ge->availables; _temp807;}); Cyc_Tc_tc_decls( te, _temp762, in_externC,
check_var_init); ge->availables=({ struct Cyc_List_List* _temp808= ge->availables;
if( _temp808 == 0){ _throw( Null_Exception);} _temp808->tl;}); goto _LL711;}}
_LL733: Cyc_Tc_tc_decls( te, _temp771, 1, check_var_init); goto _LL711; _LL711:;}}
static char _temp812[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp812, _temp812, _temp812 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, int check_var_init, struct Cyc_List_List* ds){ if(
add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp813=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp813->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp814=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp814->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp815=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp815[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp816; _temp816.tag= Cyc_Absyn_Namespace_d;
_temp816.f1= Cyc_Tc_cyc_ns; _temp816.f2= ds; _temp816;}); _temp815;})); _temp814->loc=
0; _temp814;}); _temp813->tl= 0; _temp813;});} Cyc_Tc_tc_decls( te, ds, 0,
check_var_init);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t; static
int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d);
static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple6{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp817=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp827; struct Cyc_List_List* _temp829; struct Cyc_List_List**
_temp831; struct _tuple0* _temp832; struct Cyc_List_List* _temp834; struct Cyc_List_List**
_temp836; struct _tagged_string* _temp837; _LL819: if(*(( int*) _temp817) == Cyc_Absyn_Var_d){
_LL828: _temp827=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp817)->f1; goto _LL820;} else{ goto _LL821;} _LL821: if(*(( int*) _temp817)
== Cyc_Absyn_Using_d){ _LL833: _temp832=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp817)->f1; goto _LL830; _LL830: _temp829=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Using_d_struct*) _temp817)->f2; _temp831=&(( struct Cyc_Absyn_Using_d_struct*)
_temp817)->f2; goto _LL822;} else{ goto _LL823;} _LL823: if(*(( int*) _temp817)
== Cyc_Absyn_Namespace_d){ _LL838: _temp837=( struct _tagged_string*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp817)->f1; goto _LL835; _LL835: _temp834=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp817)->f2;
_temp836=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp817)->f2; goto _LL824;}
else{ goto _LL825;} _LL825: goto _LL826; _LL820: if(( void*) _temp827->sc !=(
void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp841; struct
_tagged_string* _temp842; void* _temp844; struct _tuple0* _temp839= _temp827->name;
_temp841=* _temp839; _LL845: _temp844= _temp841.f1; goto _LL843; _LL843:
_temp842= _temp841.f2; goto _LL840; _LL840: { struct Cyc_List_List* ns;{ void*
_temp846= _temp844; struct Cyc_List_List* _temp854; struct Cyc_List_List*
_temp856; _LL848: if( _temp846 ==( void*) Cyc_Absyn_Loc_n){ goto _LL849;} else{
goto _LL850;} _LL850: if(( unsigned int) _temp846 > 1u?*(( int*) _temp846) ==
Cyc_Absyn_Rel_n: 0){ _LL855: _temp854=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp846)->f1; goto _LL851;} else{ goto _LL852;} _LL852: if(( unsigned int)
_temp846 > 1u?*(( int*) _temp846) == Cyc_Absyn_Abs_n: 0){ _LL857: _temp856=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp846)->f1; goto
_LL853;} else{ goto _LL847;} _LL849: ns= 0; goto _LL847; _LL851: ns= _temp854;
goto _LL847; _LL853: ns= _temp856; goto _LL847; _LL847:;}{ struct _tuple6* nsenv=((
struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
env, ns); struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; int needed=(*(( struct _tuple3*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries,
_temp842)).f2; if( ! needed){(* nsenv).f2=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)((* nsenv).f2, _temp842);} return
needed;}}} _LL822: _temp836= _temp831; goto _LL824; _LL824:* _temp836= Cyc_Tc_treeshake_f(
env,* _temp836); return 1; _LL826: return 1; _LL818:;} static int Cyc_Tc_treeshake_remove_f(
struct Cyc_Set_Set* set, struct _tagged_string* x, struct _tuple3* y){ return !((
int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( set,
x);} static void Cyc_Tc_treeshake_remove( struct Cyc_List_List* ns, struct
_tuple6* nsenv){ struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; struct Cyc_Set_Set* set=(*
nsenv).f2; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*,
struct _tagged_string*, struct _tuple3*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict*
d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f, set, ge->ordinaries);} static
struct _tuple6* Cyc_Tc_treeshake_make_env_f( struct Cyc_Tcenv_Genv* ge){ return({
struct _tuple6* _temp858=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp858->f1= ge; _temp858->f2=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_strptrcmp);
_temp858;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* env=(( struct Cyc_Dict_Dict*(*)(
struct _tuple6*(* f)( struct Cyc_Tcenv_Genv*), struct Cyc_Dict_Dict* d)) Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f, te->ae); struct Cyc_List_List* ds1= Cyc_Tc_treeshake_f(
env, ds);(( void(*)( void(* f)( struct Cyc_List_List*, struct _tuple6*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove, env); return ds1;}