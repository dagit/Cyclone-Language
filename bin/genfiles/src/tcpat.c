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
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap(
struct _RegionHandle*, void*(* f)( void*), struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_rmap_c( struct
_RegionHandle*, void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev( struct
Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_rappend(
struct _RegionHandle*, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_rzip( struct
_RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*,
int), void* env); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present[ 12u];
extern char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* comp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set* s); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
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
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1*
Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( void*); extern
void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl( struct _tuple1* x, void* t, struct Cyc_Absyn_Exp* init);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_kind2string( void*); extern
struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern char
Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string);
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar*
f1; void* f2; } ; extern struct _tuple4* Cyc_Tcutil_r_make_inst_var( struct
_RegionHandle*, struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List* vs, struct Cyc_Position_Segment* loc, struct
_tagged_string err_msg); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
des, struct Cyc_List_List* fields); struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2; } ; extern struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 > 2u?*(( int*) _temp0)
== Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=( struct _tuple1*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp0)->f1; goto _LL15; _LL15: _temp14=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp0)->f2;
goto _LL13; _LL13: _temp12=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 2u?*((
int*) _temp0) == Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=( struct _tuple1*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp0)->f1; goto _LL21; _LL21:
_temp20=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Structdecl* _temp37; struct Cyc_Absyn_Tunionfield*
_temp39; struct Cyc_Absyn_Tuniondecl* _temp41; struct Cyc_Absyn_Enumfield*
_temp43; struct Cyc_Absyn_Enumdecl* _temp45; void* _temp47; _LL29: if(*(( int*)
_temp27) == Cyc_Tcenv_StructRes){ _LL38: _temp37=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp27)->f1; goto _LL30;} else{ goto _LL31;}
_LL31: if(*(( int*) _temp27) == Cyc_Tcenv_TunionRes){ _LL42: _temp41=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f1; goto
_LL40; _LL40: _temp39=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) ==
Cyc_Tcenv_EnumRes){ _LL46: _temp45=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp27)->f1; goto _LL44; _LL44: _temp43=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp27)->f2; goto _LL34;} else{ goto _LL35;} _LL35:
if(*(( int*) _temp27) == Cyc_Tcenv_VarRes){ _LL48: _temp47=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp27)->f1; goto _LL36;} else{ goto _LL28;} _LL30: Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp49=( char*)"struct tag used without arguments in pattern";
struct _tagged_string _temp50; _temp50.curr= _temp49; _temp50.base= _temp49;
_temp50.last_plus_one= _temp49 + 45; _temp50;})); _npop_handler( 0u); return;
_LL32:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp51=( struct Cyc_Absyn_Tunion_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp51[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp52; _temp52.tag= Cyc_Absyn_Tunion_p;
_temp52.f1= _temp41; _temp52.f2= _temp39; _temp52.f3= 0; _temp52.f4= 0; _temp52;});
_temp51;}))); _npop_handler( 0u); return; _LL34:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp53=( struct Cyc_Absyn_Enum_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp53[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp54; _temp54.tag= Cyc_Absyn_Enum_p; _temp54.f1= _temp45; _temp54.f2= _temp43;
_temp54;}); _temp53;}))); _npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;};
_pop_handler();} else{ void* _temp25=( void*) _exn_thrown; void* _temp56=
_temp25; _LL58: if( _temp56 == Cyc_Dict_Absent){ goto _LL59;} else{ goto _LL60;}
_LL60: goto _LL61; _LL59: goto _LL57; _LL61:( void) _throw( _temp56); _LL57:;}}}{
void* _temp62=(* _temp10).f1; struct Cyc_List_List* _temp70; _LL64: if( _temp62
==( void*) Cyc_Absyn_Loc_n){ goto _LL65;} else{ goto _LL66;} _LL66: if((
unsigned int) _temp62 > 1u?*(( int*) _temp62) == Cyc_Absyn_Rel_n: 0){ _LL71:
_temp70=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp62)->f1;
if( _temp70 == 0){ goto _LL67;} else{ goto _LL68;}} else{ goto _LL68;} _LL68:
goto _LL69; _LL65: goto _LL67; _LL67:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;(
void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp72=( struct
Cyc_Absyn_Var_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_p_struct));
_temp72[ 0]=({ struct Cyc_Absyn_Var_p_struct _temp73; _temp73.tag= Cyc_Absyn_Var_p;
_temp73.f1= Cyc_Absyn_new_vardecl( _temp10,( void*) Cyc_Absyn_VoidType, 0);
_temp73;}); _temp72;}))); return; _LL69: Cyc_Tcutil_terr( p->loc,( struct
_tagged_string)({ char* _temp74=( char*)"qualified variable in pattern"; struct
_tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one=
_temp74 + 30; _temp75;})); return; _LL63:;} _LL5:{ struct _handler_cons _temp76;
_push_handler(& _temp76);{ int _temp78= 0; if( setjmp( _temp76.handler)){
_temp78= 1;} if( ! _temp78){{ void* _temp79= Cyc_Tcenv_lookup_ordinary( te, p->loc,
_temp16); struct Cyc_Absyn_Structdecl* _temp89; struct Cyc_Absyn_Tunionfield*
_temp91; struct Cyc_Absyn_Tuniondecl* _temp93; struct Cyc_Absyn_Enumfield*
_temp95; struct Cyc_Absyn_Enumdecl* _temp97; void* _temp99; _LL81: if(*(( int*)
_temp79) == Cyc_Tcenv_StructRes){ _LL90: _temp89=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp79)->f1; goto _LL82;} else{ goto _LL83;}
_LL83: if(*(( int*) _temp79) == Cyc_Tcenv_TunionRes){ _LL94: _temp93=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp79)->f1; goto
_LL92; _LL92: _temp91=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*)
_temp79)->f2; goto _LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp79) ==
Cyc_Tcenv_EnumRes){ _LL98: _temp97=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp79)->f1; goto _LL96; _LL96: _temp95=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp79)->f2; goto _LL86;} else{ goto _LL87;} _LL87:
if(*(( int*) _temp79) == Cyc_Tcenv_VarRes){ _LL100: _temp99=( void*)(( struct
Cyc_Tcenv_VarRes_struct*) _temp79)->f1; goto _LL88;} else{ goto _LL80;} _LL82: {
struct Cyc_List_List* new_ps= 0; for( 0; _temp12 != 0; _temp12=({ struct Cyc_List_List*
_temp101= _temp12; if( _temp101 == 0){ _throw( Null_Exception);} _temp101->tl;})){
new_ps=({ struct Cyc_List_List* _temp102=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp102->hd=( void*)({ struct _tuple6* _temp103=(
struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp103->f1= 0; _temp103->f2=(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp104= _temp12; if( _temp104
== 0){ _throw( Null_Exception);} _temp104->hd;}); _temp103;}); _temp102->tl=
new_ps; _temp102;});}( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp105=( struct Cyc_Absyn_Struct_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp105[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp106; _temp106.tag= Cyc_Absyn_Struct_p;
_temp106.f1= _temp89; _temp106.f2= 0; _temp106.f3= _temp14; _temp106.f4=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( new_ps);
_temp106;}); _temp105;}))); _npop_handler( 0u); return;} _LL84:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp107=( struct Cyc_Absyn_Tunion_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_Tunion_p_struct _temp108; _temp108.tag= Cyc_Absyn_Tunion_p; _temp108.f1=
_temp93; _temp108.f2= _temp91; _temp108.f3= _temp14; _temp108.f4= _temp12;
_temp108;}); _temp107;}))); _npop_handler( 0u); return; _LL86: Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp109= Cyc_Absynpp_qvar2string( _temp16);
xprintf("bad enum constructor %.*s in pattern", _temp109.last_plus_one -
_temp109.curr, _temp109.curr);})); _npop_handler( 0u); return; _LL88: goto _LL80;
_LL80:;}; _pop_handler();} else{ void* _temp77=( void*) _exn_thrown; void*
_temp111= _temp77; _LL113: if( _temp111 == Cyc_Dict_Absent){ goto _LL114;} else{
goto _LL115;} _LL115: goto _LL116; _LL114: goto _LL112; _LL116:( void) _throw(
_temp111); _LL112:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp117=
Cyc_Absynpp_qvar2string( _temp16); xprintf("%.*s is not a constructor in pattern",
_temp117.last_plus_one - _temp117.curr, _temp117.curr);})); return; _LL7:{
struct _handler_cons _temp118; _push_handler(& _temp118);{ int _temp120= 0; if(
setjmp( _temp118.handler)){ _temp120= 1;} if( ! _temp120){{ void* _temp121= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp22); struct Cyc_Absyn_Structdecl* _temp127; _LL123: if(*(( int*)
_temp121) == Cyc_Tcenv_StructRes){ _LL128: _temp127=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp121)->f1; goto _LL124;} else{ goto
_LL125;} _LL125: goto _LL126; _LL124:( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp129=( struct Cyc_Absyn_Struct_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp129[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp130; _temp130.tag= Cyc_Absyn_Struct_p; _temp130.f1=
_temp127; _temp130.f2= 0; _temp130.f3= _temp20; _temp130.f4= _temp18; _temp130;});
_temp129;}))); _npop_handler( 0u); return; _LL126: goto _LL122; _LL122:;};
_pop_handler();} else{ void* _temp119=( void*) _exn_thrown; void* _temp132=
_temp119; _LL134: if( _temp132 == Cyc_Dict_Absent){ goto _LL135;} else{ goto
_LL136;} _LL136: goto _LL137; _LL135: goto _LL133; _LL137:( void) _throw(
_temp132); _LL133:;}}} Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp138=
Cyc_Absynpp_qvar2string( _temp22); xprintf("%.*s is not a struct name, but pattern has designators",
_temp138.last_plus_one - _temp138.curr, _temp138.curr);})); return; _LL9:
return; _LL1:;} static struct _tagged_string* Cyc_Tcpat_get_name( struct Cyc_Absyn_Vardecl*
vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type( void** topt){ if(
topt != 0){ return({ void** _temp139= topt; if( _temp139 == 0){ _throw(
Null_Exception);}* _temp139;});} return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);}
static void* Cyc_Tcpat_num_type( void** topt, void* numt){ if( topt != 0? Cyc_Tcutil_coerceable(({
void** _temp140= topt; if( _temp140 == 0){ _throw( Null_Exception);}* _temp140;})):
0){ return({ void** _temp141= topt; if( _temp141 == 0){ _throw( Null_Exception);}*
_temp141;});} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=({ struct Cyc_List_List* _temp142=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp142->hd=(
void*) vd; _temp142->tl=* v_result_ptr; _temp142;});} struct _tuple7{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; struct _tuple8{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat*
f2; } ; struct _tuple5 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp143=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp177; struct Cyc_Absyn_Vardecl*
_temp179; int _temp181; void* _temp183; int _temp185; void* _temp187; char
_temp189; struct _tagged_string _temp191; struct Cyc_Absyn_Enumfield* _temp193;
struct Cyc_Absyn_Enumdecl* _temp195; struct Cyc_Absyn_Pat* _temp197; struct Cyc_List_List*
_temp199; struct Cyc_List_List* _temp201; struct Cyc_List_List* _temp203; struct
Cyc_Core_Opt* _temp205; struct Cyc_Core_Opt** _temp207; struct Cyc_Absyn_Structdecl*
_temp208; struct Cyc_List_List* _temp210; struct Cyc_List_List* _temp212; struct
Cyc_Absyn_Tunionfield* _temp214; struct Cyc_Absyn_Tuniondecl* _temp216; struct
_tuple1* _temp218; struct Cyc_List_List* _temp220; struct Cyc_List_List*
_temp222; struct _tuple1* _temp224; struct Cyc_List_List* _temp226; struct Cyc_List_List*
_temp228; struct _tuple1* _temp230; _LL145: if( _temp143 ==( void*) Cyc_Absyn_Wild_p){
goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int) _temp143 > 2u?*((
int*) _temp143) == Cyc_Absyn_Var_p: 0){ _LL178: _temp177=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp143)->f1; goto _LL148;} else{ goto _LL149;}
_LL149: if(( unsigned int) _temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Reference_p:
0){ _LL180: _temp179=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp143)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Int_p: 0){ _LL184: _temp183=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp143)->f1; if( _temp183 ==( void*)
Cyc_Absyn_Unsigned){ goto _LL182;} else{ goto _LL153;} _LL182: _temp181=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp143)->f2; goto _LL152;} else{ goto _LL153;}
_LL153: if(( unsigned int) _temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Int_p:
0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp143)->f1;
if( _temp187 ==( void*) Cyc_Absyn_Signed){ goto _LL186;} else{ goto _LL155;}
_LL186: _temp185=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp143)->f2; goto
_LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp143 > 2u?*(( int*)
_temp143) == Cyc_Absyn_Char_p: 0){ _LL190: _temp189=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp143)->f1; goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Float_p: 0){ _LL192: _temp191=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp143)->f1; goto
_LL158;} else{ goto _LL159;} _LL159: if(( unsigned int) _temp143 > 2u?*(( int*)
_temp143) == Cyc_Absyn_Enum_p: 0){ _LL196: _temp195=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp143)->f1; goto _LL194; _LL194: _temp193=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*) _temp143)->f2;
goto _LL160;} else{ goto _LL161;} _LL161: if( _temp143 ==( void*) Cyc_Absyn_Null_p){
goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp143 > 2u?*((
int*) _temp143) == Cyc_Absyn_Pointer_p: 0){ _LL198: _temp197=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp143)->f1; goto _LL164;} else{ goto
_LL165;} _LL165: if(( unsigned int) _temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Tuple_p:
0){ _LL200: _temp199=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp143)->f1; goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int)
_temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Struct_p: 0){ _LL209: _temp208=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp143)->f1;
goto _LL206; _LL206: _temp205=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp143)->f2; _temp207=&(( struct Cyc_Absyn_Struct_p_struct*) _temp143)->f2;
goto _LL204; _LL204: _temp203=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp143)->f3; goto _LL202; _LL202: _temp201=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp143)->f4; goto _LL168;} else{ goto _LL169;}
_LL169: if(( unsigned int) _temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_Tunion_p:
0){ _LL217: _temp216=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp143)->f1; goto _LL215; _LL215: _temp214=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp143)->f2; goto _LL213; _LL213: _temp212=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp143)->f3; goto
_LL211; _LL211: _temp210=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp143)->f4; goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int)
_temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_UnknownId_p: 0){ _LL219: _temp218=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp143)->f1; goto
_LL172;} else{ goto _LL173;} _LL173: if(( unsigned int) _temp143 > 2u?*(( int*)
_temp143) == Cyc_Absyn_UnknownCall_p: 0){ _LL225: _temp224=( struct _tuple1*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp143)->f1; goto _LL223; _LL223:
_temp222=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp143)->f2; goto _LL221; _LL221: _temp220=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp143)->f3; goto _LL174;} else{ goto _LL175;}
_LL175: if(( unsigned int) _temp143 > 2u?*(( int*) _temp143) == Cyc_Absyn_UnknownFields_p:
0){ _LL231: _temp230=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp143)->f1; goto _LL229; _LL229: _temp228=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp143)->f2; goto _LL227; _LL227: _temp226=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp143)->f3;
goto _LL176;} else{ goto _LL144;} _LL146: t= Cyc_Tcpat_any_type( topt); goto
_LL144; _LL148: t= Cyc_Tcpat_any_type( topt); Cyc_Tcpat_set_vd( _temp177,&
v_result, t); goto _LL144; _LL150: t= Cyc_Tcpat_any_type( topt); if( region_opt
== 0){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char* _temp232=( char*)"* pattern would point into an unknown region";
struct _tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 45; _temp233;})); goto _LL144;} Cyc_Tcpat_set_vd(
_temp179,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct* _temp234=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp234[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp235; _temp235.tag= Cyc_Absyn_PointerType;
_temp235.f1=({ struct Cyc_Absyn_PtrInfo _temp236; _temp236.elt_typ=( void*) t;
_temp236.rgn_typ=( void*)({ void** _temp237= region_opt; if( _temp237 == 0){
_throw( Null_Exception);}* _temp237;}); _temp236.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp236.tq= Cyc_Absyn_empty_tqual();
_temp236.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp236;}); _temp235;}); _temp234;})); goto _LL144; _LL152: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_uint_t); goto _LL144; _LL154: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t);
goto _LL144; _LL156: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto
_LL144; _LL158: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_float_t); goto _LL144;
_LL160: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp238=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp238[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp239; _temp239.tag= Cyc_Absyn_EnumType; _temp239.f1=
_temp195->name; _temp239.f2=( struct Cyc_Absyn_Enumdecl*) _temp195; _temp239;});
_temp238;}); goto _LL144; _LL162: if( topt != 0){ void* _temp241= Cyc_Tcutil_compress(({
void** _temp240= topt; if( _temp240 == 0){ _throw( Null_Exception);}* _temp240;}));
struct Cyc_Absyn_PtrInfo _temp247; _LL243: if(( unsigned int) _temp241 > 4u?*((
int*) _temp241) == Cyc_Absyn_PointerType: 0){ _LL248: _temp247=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp241)->f1; goto _LL244;} else{ goto
_LL245;} _LL245: goto _LL246; _LL244: t=({ void** _temp249= topt; if( _temp249
== 0){ _throw( Null_Exception);}* _temp249;}); goto tcpat_end; _LL246: goto
_LL242; _LL242:;} t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp250=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp250[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp251; _temp251.tag= Cyc_Absyn_PointerType;
_temp251.f1=({ struct Cyc_Absyn_PtrInfo _temp252; _temp252.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_AnyKind); _temp252.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp252.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp252.tq= Cyc_Absyn_empty_tqual(); _temp252.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp252;}); _temp251;});
_temp250;}); goto _LL144; _LL164: { void* inner_typ=( void*) Cyc_Absyn_VoidType;
void** inner_topt= 0; if( topt != 0){ void* _temp254= Cyc_Tcutil_compress(({
void** _temp253= topt; if( _temp253 == 0){ _throw( Null_Exception);}* _temp253;}));
struct Cyc_Absyn_PtrInfo _temp260; struct Cyc_Absyn_Conref* _temp262; struct Cyc_Absyn_Tqual
_temp264; struct Cyc_Absyn_Conref* _temp266; void* _temp268; void* _temp270;
_LL256: if(( unsigned int) _temp254 > 4u?*(( int*) _temp254) == Cyc_Absyn_PointerType:
0){ _LL261: _temp260=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp254)->f1; _LL271: _temp270=( void*) _temp260.elt_typ; goto _LL269; _LL269:
_temp268=( void*) _temp260.rgn_typ; goto _LL267; _LL267: _temp266=( struct Cyc_Absyn_Conref*)
_temp260.nullable; goto _LL265; _LL265: _temp264=( struct Cyc_Absyn_Tqual)
_temp260.tq; goto _LL263; _LL263: _temp262=( struct Cyc_Absyn_Conref*) _temp260.bounds;
goto _LL257;} else{ goto _LL258;} _LL258: goto _LL259; _LL257: inner_typ=
_temp270; inner_topt=( void**)& inner_typ; goto _LL255; _LL259: goto _LL255;
_LL255:;}{ void* ptr_rgn= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); struct
Cyc_List_List* _temp274; struct Cyc_List_List* _temp276; struct _tuple5 _temp272=
Cyc_Tcpat_tcPatRec( te, _temp197, inner_topt,( void**)& ptr_rgn); _LL277:
_temp276= _temp272.f1; goto _LL275; _LL275: _temp274= _temp272.f2; goto _LL273;
_LL273: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp276); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp274); if( inner_topt == 0){ void* _temp279= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp278= _temp197->topt; if( _temp278 == 0){
_throw( Null_Exception);} _temp278->v;})); struct Cyc_Absyn_TunionFieldInfo
_temp285; struct Cyc_List_List* _temp287; void* _temp289; struct Cyc_Absyn_Tunionfield*
_temp291; struct Cyc_Absyn_Tuniondecl* _temp293; _LL281: if(( unsigned int)
_temp279 > 4u?*(( int*) _temp279) == Cyc_Absyn_TunionFieldType: 0){ _LL286:
_temp285=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp279)->f1; _LL290: _temp289=( void*) _temp285.field_info; if(*(( int*)
_temp289) == Cyc_Absyn_KnownTunionfield){ _LL294: _temp293=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunionfield_struct*) _temp289)->f1; goto _LL292; _LL292:
_temp291=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp289)->f2; goto _LL288;} else{ goto _LL283;} _LL288: _temp287=( struct Cyc_List_List*)
_temp285.targs; goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284; _LL282: t=(
void*)({ struct Cyc_Absyn_TunionType_struct* _temp295=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp295[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp296; _temp296.tag= Cyc_Absyn_TunionType;
_temp296.f1=({ struct Cyc_Absyn_TunionInfo _temp297; _temp297.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp298=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp298[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp299; _temp299.tag= Cyc_Absyn_KnownTunion;
_temp299.f1= _temp293; _temp299;}); _temp298;})); _temp297.targs= _temp287;
_temp297.rgn=( void*) ptr_rgn; _temp297;}); _temp296;}); _temp295;}); goto
_LL280; _LL284: t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp300=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp300[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp301; _temp301.tag= Cyc_Absyn_PointerType;
_temp301.f1=({ struct Cyc_Absyn_PtrInfo _temp302; _temp302.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp303= _temp197->topt; if( _temp303 == 0){
_throw( Null_Exception);} _temp303->v;})); _temp302.rgn_typ=( void*) ptr_rgn;
_temp302.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp302.tq= Cyc_Absyn_empty_tqual(); _temp302.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp302;}); _temp301;}); _temp300;}); goto _LL280;
_LL280:;} else{ t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp304=(
struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp304[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp305; _temp305.tag= Cyc_Absyn_PointerType;
_temp305.f1=({ struct Cyc_Absyn_PtrInfo _temp306; _temp306.elt_typ=( void*)((
void*)({ struct Cyc_Core_Opt* _temp307= _temp197->topt; if( _temp307 == 0){
_throw( Null_Exception);} _temp307->v;})); _temp306.rgn_typ=( void*) ptr_rgn;
_temp306.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp306.tq= Cyc_Absyn_empty_tqual(); _temp306.bounds=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp306;}); _temp305;}); _temp304;});} goto _LL144;}}
_LL166: { struct Cyc_List_List* pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if(
topt != 0){ void* _temp309= Cyc_Tcutil_compress(({ void** _temp308= topt; if(
_temp308 == 0){ _throw( Null_Exception);}* _temp308;})); struct Cyc_List_List*
_temp315; _LL311: if(( unsigned int) _temp309 > 4u?*(( int*) _temp309) == Cyc_Absyn_TupleType:
0){ _LL316: _temp315=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp309)->f1; goto _LL312;} else{ goto _LL313;} _LL313: goto _LL314; _LL312:
topt_ts= _temp315; goto _LL310; _LL314: goto _LL310; _LL310:;} for( 0; _temp199
!= 0; _temp199=({ struct Cyc_List_List* _temp317= _temp199; if( _temp317 == 0){
_throw( Null_Exception);} _temp317->tl;})){ void** inner_topt= 0; if( topt_ts !=
0){ inner_topt=( void**)&(*(( struct _tuple7*)({ struct Cyc_List_List* _temp318=
topt_ts; if( _temp318 == 0){ _throw( Null_Exception);} _temp318->hd;}))).f2;
topt_ts=({ struct Cyc_List_List* _temp319= topt_ts; if( _temp319 == 0){ _throw(
Null_Exception);} _temp319->tl;});}{ struct Cyc_List_List* _temp323; struct Cyc_List_List*
_temp325; struct _tuple5 _temp321= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)({
struct Cyc_List_List* _temp320= _temp199; if( _temp320 == 0){ _throw(
Null_Exception);} _temp320->hd;}), inner_topt, region_opt); _LL326: _temp325=
_temp321.f1; goto _LL324; _LL324: _temp323= _temp321.f2; goto _LL322; _LL322:
tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tv_result, _temp325); v_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( v_result,
_temp323); pat_ts=({ struct Cyc_List_List* _temp327=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp327->hd=( void*)({ struct
_tuple7* _temp328=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp328->f1= Cyc_Absyn_empty_tqual(); _temp328->f2=( void*)({ struct Cyc_Core_Opt*
_temp330=(( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp329= _temp199;
if( _temp329 == 0){ _throw( Null_Exception);} _temp329->hd;}))->topt; if(
_temp330 == 0){ _throw( Null_Exception);} _temp330->v;}); _temp328;}); _temp327->tl=
pat_ts; _temp327;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct* _temp331=(
struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp331[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp332; _temp332.tag= Cyc_Absyn_TupleType;
_temp332.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp332;}); _temp331;}); goto _LL144;} _LL168: if( _temp203 != 0){(
void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp333=( char*)"tcPat: struct<...> not implemented";
struct _tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 35; _temp334;})));}{ struct _RegionHandle
_temp335= _new_region(); struct _RegionHandle* rgn=& _temp335; _push_region( rgn);{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*), struct
_RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp208->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, inst); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp336=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp336[ 0]=({ struct Cyc_Absyn_StructType_struct _temp337; _temp337.tag= Cyc_Absyn_StructType;
_temp337.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp338= _temp208->name; if( _temp338 == 0){ _throw( Null_Exception);} _temp338->v;}));
_temp337.f2= all_typs; _temp337.f3=({ struct Cyc_Absyn_Structdecl** _temp339=(
struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp339[ 0]= _temp208; _temp339;}); _temp337;}); _temp336;}); if( topt != 0){
Cyc_Tcutil_unify( t,({ void** _temp340= topt; if( _temp340 == 0){ _throw(
Null_Exception);}* _temp340;}));}* _temp207=({ struct Cyc_Core_Opt* _temp341=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp341->v=(
void*) all_typs; _temp341;}); if( _temp208->fields == 0){ Cyc_Tcutil_terr( p->loc,(
struct _tagged_string)({ char* _temp342=( char*)"can't destructure an abstract struct";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 37; _temp343;})); _npop_handler( 0u); goto
_LL144;}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc,
_temp201,( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp364= _temp208->fields;
if( _temp364 == 0){ _throw( Null_Exception);} _temp364->v;})); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp344= fields; if( _temp344 == 0){ _throw(
Null_Exception);} _temp344->tl;})){ struct _tuple8 _temp348; struct Cyc_Absyn_Pat*
_temp349; struct Cyc_Absyn_Structfield* _temp351; struct _tuple8* _temp346=(
struct _tuple8*)({ struct Cyc_List_List* _temp345= fields; if( _temp345 == 0){
_throw( Null_Exception);} _temp345->hd;}); _temp348=* _temp346; _LL352: _temp351=
_temp348.f1; goto _LL350; _LL350: _temp349= _temp348.f2; goto _LL347; _LL347: {
void* inst_fieldtyp= Cyc_Tcutil_rsubstitute( rgn, inst,( void*) _temp351->type);
struct Cyc_List_List* _temp355; struct Cyc_List_List* _temp357; struct _tuple5
_temp353= Cyc_Tcpat_tcPatRec( te, _temp349,( void**)& inst_fieldtyp, region_opt);
_LL358: _temp357= _temp353.f1; goto _LL356; _LL356: _temp355= _temp353.f2; goto
_LL354; _LL354: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp357); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp355); if( ! Cyc_Tcutil_unify(( void*)({ struct Cyc_Core_Opt*
_temp359= _temp349->topt; if( _temp359 == 0){ _throw( Null_Exception);} _temp359->v;}),
inst_fieldtyp)){ Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp361=*
_temp351->name; struct _tagged_string _temp362= Cyc_Absynpp_typ2string(
inst_fieldtyp); struct _tagged_string _temp363= Cyc_Absynpp_typ2string(( void*)({
struct Cyc_Core_Opt* _temp360= _temp349->topt; if( _temp360 == 0){ _throw(
Null_Exception);} _temp360->v;})); xprintf("field %.*s of struct pattern expects type %.*s != %.*s",
_temp361.last_plus_one - _temp361.curr, _temp361.curr, _temp362.last_plus_one -
_temp362.curr, _temp362.curr, _temp363.last_plus_one - _temp363.curr, _temp363.curr);}));}}}}};
_pop_region(& _temp335);} goto _LL144; _LL170: if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp212) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp214->tvs)){ Cyc_Tcutil_terr( p->loc,( struct _tagged_string)({ char*
_temp365=( char*)"wrong number of existential type variables in pattern"; struct
_tagged_string _temp366; _temp366.curr= _temp365; _temp366.base= _temp365;
_temp366.last_plus_one= _temp365 + 54; _temp366;}));}{ struct _RegionHandle
_temp367= _new_region(); struct _RegionHandle* rgn=& _temp367; _push_region( rgn);{
struct Cyc_List_List* var_tvs= 0;{ struct Cyc_List_List* t= _temp212; for( 0; t
!= 0; t=({ struct Cyc_List_List* _temp368= t; if( _temp368 == 0){ _throw(
Null_Exception);} _temp368->tl;})){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp395= t; if( _temp395 == 0){ _throw( Null_Exception);}
_temp395->hd;});{ void* _temp369=( void*)((( struct Cyc_Absyn_Conref*(*)( struct
Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp379;
void* _temp381; struct Cyc_Absyn_Conref* _temp383; _LL371: if(( unsigned int)
_temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Eq_constr: 0){ _LL380: _temp379=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp369)->f1; if( _temp379 ==(
void*) Cyc_Absyn_BoxKind){ goto _LL372;} else{ goto _LL373;}} else{ goto _LL373;}
_LL373: if(( unsigned int) _temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Eq_constr:
0){ _LL382: _temp381=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp369)->f1;
goto _LL374;} else{ goto _LL375;} _LL375: if( _temp369 ==( void*) Cyc_Absyn_No_constr){
goto _LL376;} else{ goto _LL377;} _LL377: if(( unsigned int) _temp369 > 1u?*((
int*) _temp369) == Cyc_Absyn_Forward_constr: 0){ _LL384: _temp383=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp369)->f1; goto _LL378;} else{ goto
_LL370;} _LL372: goto _LL370; _LL374: Cyc_Tcutil_terr( p->loc,({ struct
_tagged_string _temp385= Cyc_Absynpp_qvar2string( _temp216->name); struct
_tagged_string _temp386=* tv->name; struct _tagged_string _temp387= Cyc_Absynpp_kind2string(
_temp381); xprintf("tunion member %.*s abstracts type variable %.*s of kind %.*s != B",
_temp385.last_plus_one - _temp385.curr, _temp385.curr, _temp386.last_plus_one -
_temp386.curr, _temp386.curr, _temp387.last_plus_one - _temp387.curr, _temp387.curr);}));
goto _LL370; _LL376:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp388=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp388[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp389; _temp389.tag= Cyc_Absyn_Eq_constr;
_temp389.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp389;}); _temp388;})));
goto _LL370; _LL378:( void) _throw((( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp390=( char*)"forward after compress in conref";
struct _tagged_string _temp391; _temp391.curr= _temp390; _temp391.base= _temp390;
_temp391.last_plus_one= _temp390 + 33; _temp391;}))); _LL370:;} var_tvs=({
struct Cyc_List_List* _temp392=( struct Cyc_List_List*) _region_malloc( rgn,
sizeof( struct Cyc_List_List)); _temp392->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct*
_temp393=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp393[ 0]=({ struct Cyc_Absyn_VarType_struct _temp394; _temp394.tag= Cyc_Absyn_VarType;
_temp394.f1= tv; _temp394;}); _temp393;})); _temp392->tl= var_tvs; _temp392;});}}
var_tvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
var_tvs); tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp212);{ struct Cyc_Tcenv_Tenv*
te2= Cyc_Tcenv_add_type_vars( p->loc, te, _temp212); struct Cyc_List_List* tqts=
_temp214->typs; struct Cyc_List_List* all_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple4*(* f)( struct _RegionHandle*, struct Cyc_Absyn_Tvar*),
struct _RegionHandle* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,
rgn, _temp216->tvs); struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r3, struct _RegionHandle* r4, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp214->tvs, var_tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, all_inst, exist_inst); if( tqts == 0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp396=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp396[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp397; _temp397.tag= Cyc_Absyn_TunionType;
_temp397.f1=({ struct Cyc_Absyn_TunionInfo _temp398; _temp398.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp399=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp399[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp400; _temp400.tag= Cyc_Absyn_KnownTunion;
_temp400.f1= _temp216; _temp400;}); _temp399;})); _temp398.targs= all_typs;
_temp398.rgn=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp398;});
_temp397;}); _temp396;});} else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp401=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp401[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp402; _temp402.tag= Cyc_Absyn_TunionFieldType; _temp402.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp403; _temp403.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp404=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp404[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp405; _temp405.tag= Cyc_Absyn_KnownTunionfield; _temp405.f1= _temp216;
_temp405.f2= _temp214; _temp405;}); _temp404;})); _temp403.targs= all_typs;
_temp403;}); _temp402;}); _temp401;});} if( topt != 0? tqts == 0: 0){ void*
_temp407= Cyc_Tcutil_compress(({ void** _temp406= topt; if( _temp406 == 0){
_throw( Null_Exception);}* _temp406;})); struct Cyc_Absyn_TunionFieldInfo
_temp415; struct Cyc_Absyn_TunionInfo _temp417; void* _temp419; struct Cyc_List_List*
_temp421; void* _temp423; _LL409: if(( unsigned int) _temp407 > 4u?*(( int*)
_temp407) == Cyc_Absyn_TunionFieldType: 0){ _LL416: _temp415=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp407)->f1; goto _LL410;} else{
goto _LL411;} _LL411: if(( unsigned int) _temp407 > 4u?*(( int*) _temp407) ==
Cyc_Absyn_TunionType: 0){ _LL418: _temp417=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp407)->f1; _LL424: _temp423=( void*)
_temp417.tunion_info; goto _LL422; _LL422: _temp421=( struct Cyc_List_List*)
_temp417.targs; goto _LL420; _LL420: _temp419=( void*) _temp417.rgn; goto _LL412;}
else{ goto _LL413;} _LL413: goto _LL414; _LL410: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp425=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp425[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp426; _temp426.tag= Cyc_Absyn_TunionFieldType; _temp426.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp427; _temp427.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp428=( struct Cyc_Absyn_KnownTunionfield_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp428[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp429; _temp429.tag= Cyc_Absyn_KnownTunionfield; _temp429.f1= _temp216;
_temp429.f2= _temp214; _temp429;}); _temp428;})); _temp427.targs= all_typs;
_temp427;}); _temp426;}); _temp425;}); goto _LL408; _LL412: { struct Cyc_List_List*
a= all_typs; for( 0; a != 0? _temp421 != 0: 0; a=({ struct Cyc_List_List*
_temp430= a; if( _temp430 == 0){ _throw( Null_Exception);} _temp430->tl;}),
_temp421=({ struct Cyc_List_List* _temp431= _temp421; if( _temp431 == 0){ _throw(
Null_Exception);} _temp431->tl;})){ Cyc_Tcutil_unify(( void*)({ struct Cyc_List_List*
_temp432= a; if( _temp432 == 0){ _throw( Null_Exception);} _temp432->hd;}),(
void*)({ struct Cyc_List_List* _temp433= _temp421; if( _temp433 == 0){ _throw(
Null_Exception);} _temp433->hd;}));} goto _LL408;} _LL414: goto _LL408; _LL408:;}
for( 0; _temp210 != 0? tqts != 0: 0; _temp210=({ struct Cyc_List_List* _temp434=
_temp210; if( _temp434 == 0){ _throw( Null_Exception);} _temp434->tl;}), tqts=({
struct Cyc_List_List* _temp435= tqts; if( _temp435 == 0){ _throw( Null_Exception);}
_temp435->tl;})){ struct Cyc_Absyn_Pat* p2=( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp448= _temp210; if( _temp448 == 0){ _throw( Null_Exception);} _temp448->hd;});
void* field_typ= Cyc_Tcutil_rsubstitute( rgn, inst,(*(( struct _tuple7*)({
struct Cyc_List_List* _temp447= tqts; if( _temp447 == 0){ _throw( Null_Exception);}
_temp447->hd;}))).f2); struct Cyc_List_List* _temp438; struct Cyc_List_List*
_temp440; struct _tuple5 _temp436= Cyc_Tcpat_tcPatRec( te2, p2,( void**)&
field_typ, region_opt); _LL441: _temp440= _temp436.f1; goto _LL439; _LL439:
_temp438= _temp436.f2; goto _LL437; _LL437: tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp440); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( v_result, _temp438); if( ! Cyc_Tcutil_unify((
void*)({ struct Cyc_Core_Opt* _temp442= p2->topt; if( _temp442 == 0){ _throw(
Null_Exception);} _temp442->v;}), field_typ)){ Cyc_Tcutil_terr( p2->loc,({
struct _tagged_string _temp444= Cyc_Absynpp_qvar2string( _temp216->name); struct
_tagged_string _temp445= Cyc_Absynpp_typ2string( field_typ); struct
_tagged_string _temp446= Cyc_Absynpp_typ2string(( void*)({ struct Cyc_Core_Opt*
_temp443= p2->topt; if( _temp443 == 0){ _throw( Null_Exception);} _temp443->v;}));
xprintf("%.*s expects argument type %.*s, not %.*s", _temp444.last_plus_one -
_temp444.curr, _temp444.curr, _temp445.last_plus_one - _temp445.curr, _temp445.curr,
_temp446.last_plus_one - _temp446.curr, _temp446.curr);}));}} if( _temp210 != 0){
Cyc_Tcutil_terr( p->loc,({ struct _tagged_string _temp449= Cyc_Absynpp_qvar2string(
_temp216->name); xprintf("too many arguments for tunion constructor %.*s",
_temp449.last_plus_one - _temp449.curr, _temp449.curr);}));} if( tqts != 0){ Cyc_Tcutil_terr(
p->loc,({ struct _tagged_string _temp450= Cyc_Absynpp_qvar2string( _temp216->name);
xprintf("too few arguments for tunion constructor %.*s", _temp450.last_plus_one
- _temp450.curr, _temp450.curr);}));}}}; _pop_region(& _temp367);} goto _LL144;
_LL172: goto _LL174; _LL174: goto _LL176; _LL176: t= Cyc_Absyn_wildtyp(); goto
_LL144; _LL144:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt* _temp451=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp451->v=( void*) t;
_temp451;}); return({ struct _tuple5 _temp452; _temp452.f1= tv_result; _temp452.f2=
v_result; _temp452;});}} struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ struct _tuple5 ans=
Cyc_Tcpat_tcPatRec( te, p, topt, region_opt);{ struct _RegionHandle _temp453=
_new_region(); struct _RegionHandle* r=& _temp453; _push_region( r); Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct _tagged_string*(* f)(
struct Cyc_Absyn_Vardecl*), struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name,
ans.f2), p->loc,( struct _tagged_string)({ char* _temp454=( char*)"pattern contains a repeated variable";
struct _tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 37; _temp455;}));; _pop_region(& _temp453);}
return ans;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Pat* p){ void* _temp456=( void*) p->r; struct Cyc_Absyn_Pat* _temp468;
struct Cyc_List_List* _temp470; struct Cyc_List_List* _temp472; struct Cyc_Absyn_Tunionfield*
_temp474; struct Cyc_Absyn_Tuniondecl* _temp476; struct Cyc_List_List* _temp478;
struct Cyc_List_List* _temp480; struct Cyc_Core_Opt* _temp482; struct Cyc_Absyn_Structdecl*
_temp484; struct Cyc_List_List* _temp486; _LL458: if(( unsigned int) _temp456 >
2u?*(( int*) _temp456) == Cyc_Absyn_Pointer_p: 0){ _LL469: _temp468=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Pointer_p_struct*) _temp456)->f1; goto _LL459;} else{ goto
_LL460;} _LL460: if(( unsigned int) _temp456 > 2u?*(( int*) _temp456) == Cyc_Absyn_Tunion_p:
0){ _LL477: _temp476=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp456)->f1; goto _LL475; _LL475: _temp474=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f2; goto _LL473; _LL473: _temp472=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp456)->f3; goto
_LL471; _LL471: _temp470=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp456)->f4; goto _LL461;} else{ goto _LL462;} _LL462: if(( unsigned int)
_temp456 > 2u?*(( int*) _temp456) == Cyc_Absyn_Struct_p: 0){ _LL485: _temp484=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*) _temp456)->f1;
goto _LL483; _LL483: _temp482=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp456)->f2; goto _LL481; _LL481: _temp480=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp456)->f3; goto _LL479; _LL479: _temp478=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*) _temp456)->f4; goto
_LL463;} else{ goto _LL464;} _LL464: if(( unsigned int) _temp456 > 2u?*(( int*)
_temp456) == Cyc_Absyn_Tuple_p: 0){ _LL487: _temp486=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_p_struct*) _temp456)->f1; goto _LL465;} else{ goto _LL466;}
_LL466: goto _LL467; _LL459: Cyc_Tcpat_check_pat_regions( te, _temp468);{ void*
_temp489=( void*)({ struct Cyc_Core_Opt* _temp488= p->topt; if( _temp488 == 0){
_throw( Null_Exception);} _temp488->v;}); struct Cyc_Absyn_PtrInfo _temp497;
struct Cyc_Absyn_Conref* _temp499; struct Cyc_Absyn_Tqual _temp501; struct Cyc_Absyn_Conref*
_temp503; void* _temp505; void* _temp507; struct Cyc_Absyn_TunionInfo _temp509;
void* _temp511; struct Cyc_List_List* _temp513; void* _temp515; _LL491: if((
unsigned int) _temp489 > 4u?*(( int*) _temp489) == Cyc_Absyn_PointerType: 0){
_LL498: _temp497=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp489)->f1; _LL508: _temp507=( void*) _temp497.elt_typ; goto _LL506; _LL506:
_temp505=( void*) _temp497.rgn_typ; goto _LL504; _LL504: _temp503=( struct Cyc_Absyn_Conref*)
_temp497.nullable; goto _LL502; _LL502: _temp501=( struct Cyc_Absyn_Tqual)
_temp497.tq; goto _LL500; _LL500: _temp499=( struct Cyc_Absyn_Conref*) _temp497.bounds;
goto _LL492;} else{ goto _LL493;} _LL493: if(( unsigned int) _temp489 > 4u?*((
int*) _temp489) == Cyc_Absyn_TunionType: 0){ _LL510: _temp509=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp489)->f1; _LL516: _temp515=( void*)
_temp509.tunion_info; goto _LL514; _LL514: _temp513=( struct Cyc_List_List*)
_temp509.targs; goto _LL512; _LL512: _temp511=( void*) _temp509.rgn; goto _LL494;}
else{ goto _LL495;} _LL495: goto _LL496; _LL492: _temp511= _temp505; goto _LL494;
_LL494: Cyc_Tcenv_check_rgn_accessible( te, p->loc, _temp511); return; _LL496:(
void) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp517=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp517[ 0]=({ struct
Cyc_Core_Impossible_struct _temp518; _temp518.tag= Cyc_Core_Impossible; _temp518.f1=(
struct _tagged_string)({ char* _temp519=( char*)"check_pat_regions: bad pointer type";
struct _tagged_string _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 36; _temp520;}); _temp518;}); _temp517;}));
_LL490:;} _LL461: for( 0; _temp470 != 0; _temp470=({ struct Cyc_List_List*
_temp521= _temp470; if( _temp521 == 0){ _throw( Null_Exception);} _temp521->tl;})){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)({ struct Cyc_List_List*
_temp522= _temp470; if( _temp522 == 0){ _throw( Null_Exception);} _temp522->hd;}));}{
void* _temp524=( void*)({ struct Cyc_Core_Opt* _temp523= p->topt; if( _temp523
== 0){ _throw( Null_Exception);} _temp523->v;}); struct Cyc_Absyn_TunionInfo
_temp532; void* _temp534; struct Cyc_List_List* _temp536; void* _temp538; struct
Cyc_Absyn_TunionFieldInfo _temp540; _LL526: if(( unsigned int) _temp524 > 4u?*((
int*) _temp524) == Cyc_Absyn_TunionType: 0){ _LL533: _temp532=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp524)->f1; _LL539: _temp538=( void*)
_temp532.tunion_info; goto _LL537; _LL537: _temp536=( struct Cyc_List_List*)
_temp532.targs; goto _LL535; _LL535: _temp534=( void*) _temp532.rgn; goto _LL527;}
else{ goto _LL528;} _LL528: if(( unsigned int) _temp524 > 4u?*(( int*) _temp524)
== Cyc_Absyn_TunionFieldType: 0){ _LL541: _temp540=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp524)->f1; goto _LL529;} else{
goto _LL530;} _LL530: goto _LL531; _LL527: Cyc_Tcenv_check_rgn_accessible( te, p->loc,
_temp534); return; _LL529: return; _LL531:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp542=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp542[ 0]=({ struct Cyc_Core_Impossible_struct _temp543; _temp543.tag= Cyc_Core_Impossible;
_temp543.f1=( struct _tagged_string)({ char* _temp544=( char*)"check_pat_regions: bad tunion type";
struct _tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 35; _temp545;}); _temp543;}); _temp542;}));
_LL525:;} _LL463: _temp486=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Pat*(*
f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Pat*(*)(
struct _tuple6*)) Cyc_Core_snd, _temp478); goto _LL465; _LL465: for( 0; _temp486
!= 0; _temp486=({ struct Cyc_List_List* _temp546= _temp486; if( _temp546 == 0){
_throw( Null_Exception);} _temp546->tl;})){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*)({ struct Cyc_List_List* _temp547= _temp486; if( _temp547
== 0){ _throw( Null_Exception);} _temp547->hd;}));} return; _LL467: return;
_LL457:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Tcpat_Int_v= 1;
struct Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; typedef void* Cyc_Tcpat_name_value_t;
struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt* span; } ;
typedef struct Cyc_Tcpat_Con_s* Cyc_Tcpat_con_t; static const int Cyc_Tcpat_Any=
0; static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag;
struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; typedef void* Cyc_Tcpat_simple_pat_t;
static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp548=( void*) c1->name; struct _tagged_string _temp554; int
_temp556; _LL550: if(*(( int*) _temp548) == Cyc_Tcpat_Name_v){ _LL555: _temp554=(
struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*) _temp548)->f1; goto
_LL551;} else{ goto _LL552;} _LL552: if(*(( int*) _temp548) == Cyc_Tcpat_Int_v){
_LL557: _temp556=( int)(( struct Cyc_Tcpat_Int_v_struct*) _temp548)->f1; goto
_LL553;} else{ goto _LL549;} _LL551: { void* _temp558=( void*) c2->name; struct
_tagged_string _temp564; int _temp566; _LL560: if(*(( int*) _temp558) == Cyc_Tcpat_Name_v){
_LL565: _temp564=( struct _tagged_string)(( struct Cyc_Tcpat_Name_v_struct*)
_temp558)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if(*(( int*) _temp558)
== Cyc_Tcpat_Int_v){ _LL567: _temp566=( int)(( struct Cyc_Tcpat_Int_v_struct*)
_temp558)->f1; goto _LL563;} else{ goto _LL559;} _LL561: return Cyc_String_zstrcmp(
_temp554, _temp564); _LL563: return - 1; _LL559:;} _LL553: { void* _temp568=(
void*) c2->name; struct _tagged_string _temp574; int _temp576; _LL570: if(*((
int*) _temp568) == Cyc_Tcpat_Name_v){ _LL575: _temp574=( struct _tagged_string)((
struct Cyc_Tcpat_Name_v_struct*) _temp568)->f1; goto _LL571;} else{ goto _LL572;}
_LL572: if(*(( int*) _temp568) == Cyc_Tcpat_Int_v){ _LL577: _temp576=( int)((
struct Cyc_Tcpat_Int_v_struct*) _temp568)->f1; goto _LL573;} else{ goto _LL569;}
_LL571: return 1; _LL573: return _temp556 - _temp576; _LL569:;} _LL549:;} static
struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct Cyc_Set_Set*(*)(
int(* comp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*))) Cyc_Set_empty)(
Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt=( struct
Cyc_Core_Opt){.v=( void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt=(
struct Cyc_Core_Opt){.v=( void*) 2}; static struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=(
struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt; static char _temp581[ 5u]="null";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_null_name_value={ 0u,( struct
_tagged_string){ _temp581, _temp581, _temp581 + 5u}}; static char _temp585[ 4u]="ptr";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={ 0u,( struct
_tagged_string){ _temp585, _temp585, _temp585 + 4u}}; static char _temp589[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,( struct
_tagged_string){ _temp589, _temp589, _temp589 + 6u}}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_null_name_value),.arity=
0,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_null_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s
Cyc_Tcpat_ptr_con0=( struct Cyc_Tcpat_Con_s){.name=( void*)(( void*)& Cyc_Tcpat_ptr_name_value),.arity=
1,.span=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt}; static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=&
Cyc_Tcpat_null_ptr_con0; static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp590=( struct Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp590->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp591=(
struct Cyc_Tcpat_Int_v_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp591[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp592; _temp592.tag= Cyc_Tcpat_Int_v;
_temp592.f1= i; _temp592;}); _temp591;})); _temp590->arity= 0; _temp590->span= 0;
_temp590;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_string f){ return({ struct Cyc_Tcpat_Con_s* _temp593=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp593->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp594=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp594[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp595; _temp595.tag= Cyc_Tcpat_Name_v; _temp595.f1= f; _temp595;}); _temp594;}));
_temp593->arity= 0; _temp593->span= 0; _temp593;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( char c){ return({ struct Cyc_Tcpat_Con_s* _temp596=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp596->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp598=( struct Cyc_Tcpat_Int_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp598[ 0]=({ struct Cyc_Tcpat_Int_v_struct
_temp599; _temp599.tag= Cyc_Tcpat_Int_v; _temp599.f1=( int) c; _temp599;});
_temp598;})); _temp596->arity= 0; _temp596->span=({ struct Cyc_Core_Opt*
_temp597=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp597->v=( void*) 256; _temp597;}); _temp596;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s* _temp600=( struct
Cyc_Tcpat_Con_s*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp600->name=(
void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp600->arity= i; _temp600->span=
Cyc_Tcpat_one_opt_ptr; _temp600;});} static void* Cyc_Tcpat_null_pat(){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp601=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp601[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp602; _temp602.tag= Cyc_Tcpat_Con; _temp602.f1= Cyc_Tcpat_null_con; _temp602.f2=
0; _temp602;}); _temp601;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp603=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp603[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp604; _temp604.tag= Cyc_Tcpat_Con; _temp604.f1= Cyc_Tcpat_int_con( i);
_temp604.f2= 0; _temp604;}); _temp603;});} static void* Cyc_Tcpat_char_pat( char
c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp605=( struct Cyc_Tcpat_Con_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp605[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp606; _temp606.tag= Cyc_Tcpat_Con; _temp606.f1= Cyc_Tcpat_char_con( c);
_temp606.f2= 0; _temp606;}); _temp605;});} static void* Cyc_Tcpat_float_pat(
struct _tagged_string f){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp607=(
struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp607[ 0]=({ struct Cyc_Tcpat_Con_struct _temp608; _temp608.tag= Cyc_Tcpat_Con;
_temp608.f1= Cyc_Tcpat_float_con( f); _temp608.f2= 0; _temp608;}); _temp607;});}
static void* Cyc_Tcpat_null_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp609=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp609[ 0]=({ struct Cyc_Tcpat_Con_struct _temp610; _temp610.tag= Cyc_Tcpat_Con;
_temp610.f1= Cyc_Tcpat_null_ptr_con; _temp610.f2=({ struct Cyc_List_List*
_temp611=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp611->hd=( void*) p; _temp611->tl= 0; _temp611;}); _temp610;}); _temp609;});}
static void* Cyc_Tcpat_ptr_pat( void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp612=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp612[ 0]=({ struct Cyc_Tcpat_Con_struct _temp613; _temp613.tag= Cyc_Tcpat_Con;
_temp613.f1= Cyc_Tcpat_ptr_con; _temp613.f2=({ struct Cyc_List_List* _temp614=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp614->hd=(
void*) p; _temp614->tl= 0; _temp614;}); _temp613;}); _temp612;});} static void*
Cyc_Tcpat_tuple_pat( struct Cyc_List_List* ss){ return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp615=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp615[ 0]=({ struct Cyc_Tcpat_Con_struct _temp616; _temp616.tag= Cyc_Tcpat_Con;
_temp616.f1= Cyc_Tcpat_tuple_con((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ss)); _temp616.f2= ss; _temp616;}); _temp615;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_string con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List*
ps){ struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp619=( struct Cyc_Tcpat_Con_s*)
GC_malloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp619->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp620=( struct Cyc_Tcpat_Name_v_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp620[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp621; _temp621.tag= Cyc_Tcpat_Name_v; _temp621.f1= con_name; _temp621;});
_temp620;})); _temp619->arity=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
ps); _temp619->span= span; _temp619;}); return( void*)({ struct Cyc_Tcpat_Con_struct*
_temp617=( struct Cyc_Tcpat_Con_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp617[ 0]=({ struct Cyc_Tcpat_Con_struct _temp618; _temp618.tag= Cyc_Tcpat_Con;
_temp618.f1= c; _temp618.f2= ps; _temp618;}); _temp617;});} static void* Cyc_Tcpat_compile_pat(
struct Cyc_Absyn_Pat* p){ void* s;{ void* _temp622=( void*) p->r; struct Cyc_Absyn_Vardecl*
_temp656; int _temp658; void* _temp660; char _temp662; struct _tagged_string
_temp664; struct Cyc_Absyn_Vardecl* _temp666; struct Cyc_Absyn_Pat* _temp668;
struct Cyc_List_List* _temp670; struct Cyc_List_List* _temp672; struct Cyc_Absyn_Tunionfield*
_temp674; struct Cyc_Absyn_Tuniondecl* _temp676; struct Cyc_List_List* _temp678;
struct Cyc_List_List* _temp680; struct Cyc_Absyn_Tunionfield* _temp682; struct
Cyc_Absyn_Tuniondecl* _temp684; struct Cyc_List_List* _temp686; struct Cyc_List_List*
_temp688; struct Cyc_List_List* _temp690; struct Cyc_Core_Opt* _temp692; struct
Cyc_Absyn_Structdecl* _temp694; struct Cyc_Absyn_Enumfield* _temp696; struct Cyc_Absyn_Enumdecl*
_temp698; struct _tuple1* _temp700; struct Cyc_List_List* _temp702; struct Cyc_List_List*
_temp704; struct _tuple1* _temp706; struct Cyc_List_List* _temp708; struct Cyc_List_List*
_temp710; struct _tuple1* _temp712; _LL624: if( _temp622 ==( void*) Cyc_Absyn_Wild_p){
goto _LL625;} else{ goto _LL626;} _LL626: if(( unsigned int) _temp622 > 2u?*((
int*) _temp622) == Cyc_Absyn_Var_p: 0){ _LL657: _temp656=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_p_struct*) _temp622)->f1; goto _LL627;} else{ goto _LL628;}
_LL628: if( _temp622 ==( void*) Cyc_Absyn_Null_p){ goto _LL629;} else{ goto
_LL630;} _LL630: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Int_p:
0){ _LL661: _temp660=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp622)->f1;
goto _LL659; _LL659: _temp658=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp622)->f2;
goto _LL631;} else{ goto _LL632;} _LL632: if(( unsigned int) _temp622 > 2u?*((
int*) _temp622) == Cyc_Absyn_Char_p: 0){ _LL663: _temp662=( char)(( struct Cyc_Absyn_Char_p_struct*)
_temp622)->f1; goto _LL633;} else{ goto _LL634;} _LL634: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Float_p: 0){ _LL665: _temp664=(
struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp622)->f1; goto
_LL635;} else{ goto _LL636;} _LL636: if(( unsigned int) _temp622 > 2u?*(( int*)
_temp622) == Cyc_Absyn_Reference_p: 0){ _LL667: _temp666=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Reference_p_struct*) _temp622)->f1; goto _LL637;} else{ goto
_LL638;} _LL638: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Pointer_p:
0){ _LL669: _temp668=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp622)->f1; goto _LL639;} else{ goto _LL640;} _LL640: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Tunion_p: 0){ _LL677: _temp676=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f1;
goto _LL675; _LL675: _temp674=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp622)->f2; goto _LL673; _LL673: _temp672=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_p_struct*) _temp622)->f3; goto _LL671; _LL671: _temp670=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f4; if(
_temp670 == 0){ goto _LL641;} else{ goto _LL642;}} else{ goto _LL642;} _LL642:
if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Tunion_p: 0){
_LL685: _temp684=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp622)->f1; goto _LL683; _LL683: _temp682=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f2; goto _LL681; _LL681: _temp680=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp622)->f3; goto
_LL679; _LL679: _temp678=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp622)->f4; goto _LL643;} else{ goto _LL644;} _LL644: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Tuple_p: 0){ _LL687: _temp686=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp622)->f1; goto
_LL645;} else{ goto _LL646;} _LL646: if(( unsigned int) _temp622 > 2u?*(( int*)
_temp622) == Cyc_Absyn_Struct_p: 0){ _LL695: _temp694=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp622)->f1; goto _LL693; _LL693: _temp692=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp622)->f2; goto
_LL691; _LL691: _temp690=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp622)->f3; goto _LL689; _LL689: _temp688=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_p_struct*) _temp622)->f4; goto _LL647;} else{ goto _LL648;}
_LL648: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_Enum_p:
0){ _LL699: _temp698=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp622)->f1; goto _LL697; _LL697: _temp696=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp622)->f2; goto _LL649;} else{ goto _LL650;}
_LL650: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_UnknownId_p:
0){ _LL701: _temp700=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp622)->f1; goto _LL651;} else{ goto _LL652;} _LL652: if(( unsigned int)
_temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_UnknownCall_p: 0){ _LL707:
_temp706=( struct _tuple1*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp622)->f1;
goto _LL705; _LL705: _temp704=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp622)->f2; goto _LL703; _LL703: _temp702=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp622)->f3; goto _LL653;} else{ goto _LL654;}
_LL654: if(( unsigned int) _temp622 > 2u?*(( int*) _temp622) == Cyc_Absyn_UnknownFields_p:
0){ _LL713: _temp712=( struct _tuple1*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp622)->f1; goto _LL711; _LL711: _temp710=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownFields_p_struct*) _temp622)->f2; goto _LL709; _LL709: _temp708=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*) _temp622)->f3;
goto _LL655;} else{ goto _LL623;} _LL625: s=( void*) Cyc_Tcpat_Any; goto _LL623;
_LL627: s=( void*) Cyc_Tcpat_Any; goto _LL623; _LL629: s= Cyc_Tcpat_null_pat();
goto _LL623; _LL631: s= Cyc_Tcpat_int_pat( _temp658); goto _LL623; _LL633: s=
Cyc_Tcpat_char_pat( _temp662); goto _LL623; _LL635: s= Cyc_Tcpat_float_pat(
_temp664); goto _LL623; _LL637: s=( void*) Cyc_Tcpat_Any; goto _LL623; _LL639:{
void* _temp715= Cyc_Tcutil_compress(( void*)({ struct Cyc_Core_Opt* _temp714= p->topt;
if( _temp714 == 0){ _throw( Null_Exception);} _temp714->v;})); struct Cyc_Absyn_PtrInfo
_temp723; struct Cyc_Absyn_Conref* _temp725; struct Cyc_Absyn_Tqual _temp727;
struct Cyc_Absyn_Conref* _temp729; void* _temp731; void* _temp733; struct Cyc_Absyn_TunionInfo
_temp735; _LL717: if(( unsigned int) _temp715 > 4u?*(( int*) _temp715) == Cyc_Absyn_PointerType:
0){ _LL724: _temp723=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp715)->f1; _LL734: _temp733=( void*) _temp723.elt_typ; goto _LL732; _LL732:
_temp731=( void*) _temp723.rgn_typ; goto _LL730; _LL730: _temp729=( struct Cyc_Absyn_Conref*)
_temp723.nullable; goto _LL728; _LL728: _temp727=( struct Cyc_Absyn_Tqual)
_temp723.tq; goto _LL726; _LL726: _temp725=( struct Cyc_Absyn_Conref*) _temp723.bounds;
goto _LL718;} else{ goto _LL719;} _LL719: if(( unsigned int) _temp715 > 4u?*((
int*) _temp715) == Cyc_Absyn_TunionType: 0){ _LL736: _temp735=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp715)->f1; goto _LL720;} else{ goto
_LL721;} _LL721: goto _LL722; _LL718: { int is_nullable= 0; int still_working= 1;
while( still_working) { void* _temp737=( void*) _temp729->v; struct Cyc_Absyn_Conref*
_temp745; int _temp747; _LL739: if(( unsigned int) _temp737 > 1u?*(( int*)
_temp737) == Cyc_Absyn_Forward_constr: 0){ _LL746: _temp745=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp737)->f1; goto _LL740;} else{ goto
_LL741;} _LL741: if( _temp737 ==( void*) Cyc_Absyn_No_constr){ goto _LL742;}
else{ goto _LL743;} _LL743: if(( unsigned int) _temp737 > 1u?*(( int*) _temp737)
== Cyc_Absyn_Eq_constr: 0){ _LL748: _temp747=( int)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp737)->f1; goto _LL744;} else{ goto _LL738;} _LL740:( void*)( _temp729->v=(
void*)(( void*) _temp745->v)); continue; _LL742:( void*)( _temp729->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp749=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp749[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp750; _temp750.tag= Cyc_Absyn_Eq_constr; _temp750.f1=(
void*) 0; _temp750;}); _temp749;}))); is_nullable= 0; still_working= 0; goto
_LL738; _LL744: is_nullable=( int) _temp747; still_working= 0; goto _LL738;
_LL738:;}{ void* ss= Cyc_Tcpat_compile_pat( _temp668); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat(
ss);} else{ s= Cyc_Tcpat_ptr_pat( ss);} goto _LL716;}} _LL720:{ void* _temp751=(
void*) _temp668->r; struct Cyc_List_List* _temp757; struct Cyc_List_List*
_temp759; struct Cyc_Absyn_Tunionfield* _temp761; struct Cyc_Absyn_Tuniondecl*
_temp763; _LL753: if(( unsigned int) _temp751 > 2u?*(( int*) _temp751) == Cyc_Absyn_Tunion_p:
0){ _LL764: _temp763=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp751)->f1; goto _LL762; _LL762: _temp761=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp751)->f2; goto _LL760; _LL760: _temp759=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp751)->f3; goto
_LL758; _LL758: _temp757=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp751)->f4; goto _LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL754: {
struct Cyc_Core_Opt* span; if( _temp763->is_xtunion){ span= 0;} else{ span=({
struct Cyc_Core_Opt* _temp765=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp765->v=( void*)(( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp766=
_temp763->fields; if( _temp766 == 0){ _throw( Null_Exception);} _temp766->v;}));
_temp765;});} s= Cyc_Tcpat_con_pat(*(* _temp761->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp757)); goto _LL752;} _LL756: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp767=( char*)"non-[x]tunion pattern has tunion type";
struct _tagged_string _temp768; _temp768.curr= _temp767; _temp768.base= _temp767;
_temp768.last_plus_one= _temp767 + 38; _temp768;})); goto _LL752; _LL752:;} goto
_LL716; _LL722: s=(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp769=( char*)"pointer pattern does not have pointer type!";
struct _tagged_string _temp770; _temp770.curr= _temp769; _temp770.base= _temp769;
_temp770.last_plus_one= _temp769 + 44; _temp770;})); goto _LL716; _LL716:;} goto
_LL623; _LL641: { struct Cyc_Core_Opt* span;{ void* _temp772= Cyc_Tcutil_compress((
void*)({ struct Cyc_Core_Opt* _temp771= p->topt; if( _temp771 == 0){ _throw(
Null_Exception);} _temp771->v;})); struct Cyc_Absyn_TunionInfo _temp780; struct
Cyc_Absyn_TunionFieldInfo _temp782; _LL774: if(( unsigned int) _temp772 > 4u?*((
int*) _temp772) == Cyc_Absyn_TunionType: 0){ _LL781: _temp780=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp772)->f1; goto _LL775;} else{ goto
_LL776;} _LL776: if(( unsigned int) _temp772 > 4u?*(( int*) _temp772) == Cyc_Absyn_TunionFieldType:
0){ _LL783: _temp782=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp772)->f1; goto _LL777;} else{ goto _LL778;} _LL778: goto _LL779; _LL775:
if( _temp676->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt* _temp784=(
struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp784->v=(
void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp785= _temp676->fields; if( _temp785 == 0){ _throw(
Null_Exception);} _temp785->v;})); _temp784;});} goto _LL773; _LL777: span=({
struct Cyc_Core_Opt* _temp786=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp786->v=( void*) 1; _temp786;}); goto _LL773; _LL779:
span=(( struct Cyc_Core_Opt*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp787=( char*)"void tunion pattern has bad type";
struct _tagged_string _temp788; _temp788.curr= _temp787; _temp788.base= _temp787;
_temp788.last_plus_one= _temp787 + 33; _temp788;})); goto _LL773; _LL773:;} s=
Cyc_Tcpat_con_pat(*(* _temp674->name).f2, span, 0); goto _LL623;} _LL643:
_temp686= _temp678; goto _LL645; _LL645: s= Cyc_Tcpat_tuple_pat((( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp686)); goto _LL623; _LL647: { struct Cyc_List_List* ps= 0;{ struct Cyc_List_List*
fields=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp821= _temp694->fields;
if( _temp821 == 0){ _throw( Null_Exception);} _temp821->v;}); for( 0; fields !=
0; fields=({ struct Cyc_List_List* _temp789= fields; if( _temp789 == 0){ _throw(
Null_Exception);} _temp789->tl;})){ int found= Cyc_String_strcmp(*(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp818= fields; if( _temp818 == 0){ _throw(
Null_Exception);} _temp818->hd;}))->name,( struct _tagged_string)({ char*
_temp819=( char*)""; struct _tagged_string _temp820; _temp820.curr= _temp819;
_temp820.base= _temp819; _temp820.last_plus_one= _temp819 + 1; _temp820;})) == 0;{
struct Cyc_List_List* dlps0= _temp688; for( 0; ! found? dlps0 != 0: 0; dlps0=({
struct Cyc_List_List* _temp790= dlps0; if( _temp790 == 0){ _throw(
Null_Exception);} _temp790->tl;})){ struct _tuple6 _temp794; struct Cyc_Absyn_Pat*
_temp795; struct Cyc_List_List* _temp797; struct _tuple6* _temp792=( struct
_tuple6*)({ struct Cyc_List_List* _temp791= dlps0; if( _temp791 == 0){ _throw(
Null_Exception);} _temp791->hd;}); _temp794=* _temp792; _LL798: _temp797=
_temp794.f1; goto _LL796; _LL796: _temp795= _temp794.f2; goto _LL793; _LL793: {
struct Cyc_List_List* _temp799= _temp797; struct Cyc_List_List _temp805; struct
Cyc_List_List* _temp806; void* _temp808; struct _tagged_string* _temp810; _LL801:
if( _temp799 == 0){ goto _LL803;} else{ _temp805=* _temp799; _LL809: _temp808=(
void*) _temp805.hd; if(*(( int*) _temp808) == Cyc_Absyn_FieldName){ _LL811:
_temp810=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp808)->f1; goto _LL807;} else{ goto _LL803;} _LL807: _temp806=( struct Cyc_List_List*)
_temp805.tl; if( _temp806 == 0){ goto _LL802;} else{ goto _LL803;}} _LL803: goto
_LL804; _LL802: if( Cyc_String_zstrptrcmp( _temp810,(( struct Cyc_Absyn_Structfield*)({
struct Cyc_List_List* _temp812= fields; if( _temp812 == 0){ _throw(
Null_Exception);} _temp812->hd;}))->name) == 0){ ps=({ struct Cyc_List_List*
_temp813=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp813->hd=( void*) Cyc_Tcpat_compile_pat( _temp795); _temp813->tl= ps;
_temp813;}); found= 1;} goto _LL800; _LL804:(( void(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp814=( char*)"bad designator(s)";
struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 18; _temp815;})); goto _LL800; _LL800:;}}}
if( ! found){(( void(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp816=( char*)"bad designator"; struct _tagged_string
_temp817; _temp817.curr= _temp816; _temp817.base= _temp816; _temp817.last_plus_one=
_temp816 + 15; _temp817;}));}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL623;}
_LL649: { int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp823= _temp698->fields; if(
_temp823 == 0){ _throw( Null_Exception);} _temp823->v;})); s= Cyc_Tcpat_con_pat(*(*
_temp696->name).f2,({ struct Cyc_Core_Opt* _temp822=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp822->v=( void*) span;
_temp822;}), 0); goto _LL623;} _LL651: s=(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp824=( char*)"compile_pat: unknownid";
struct _tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 23; _temp825;})); goto _LL623; _LL653: s=((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp826=( char*)"compile_pat: unknowncall"; struct _tagged_string
_temp827; _temp827.curr= _temp826; _temp827.base= _temp826; _temp827.last_plus_one=
_temp826 + 25; _temp827;})); goto _LL623; _LL655: s=(( void*(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp828=(
char*)"compile_pat: unknownfields"; struct _tagged_string _temp829; _temp829.curr=
_temp828; _temp829.base= _temp828; _temp829.last_plus_one= _temp828 + 27;
_temp829;})); goto _LL623; _LL623:;} return s;} typedef struct Cyc_List_List*
Cyc_Tcpat_match_t; static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set*
f1; } ; typedef void* Cyc_Tcpat_term_desc_t; typedef struct Cyc_List_List* Cyc_Tcpat_context_t;
typedef struct Cyc_List_List* Cyc_Tcpat_access_t; static const int Cyc_Tcpat_Failure=
0; static const int Cyc_Tcpat_Success= 0; struct Cyc_Tcpat_Success_struct{ int
tag; void* f1; } ; static const int Cyc_Tcpat_IfEq= 1; struct Cyc_Tcpat_IfEq_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s* f2; void* f3; void*
f4; } ; typedef void* Cyc_Tcpat_decision_t; struct _tuple9{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; typedef struct
_tuple9* Cyc_Tcpat_work_stack_frame_t; typedef struct Cyc_List_List* Cyc_Tcpat_work_stack_t;
static const int Cyc_Tcpat_Yes= 0; static const int Cyc_Tcpat_No= 1; static
const int Cyc_Tcpat_Maybe= 2; typedef void* Cyc_Tcpat_answer_t; static void* Cyc_Tcpat_add_neg(
void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp830= td; struct Cyc_Set_Set*
_temp836; struct Cyc_List_List* _temp838; struct Cyc_Tcpat_Con_s* _temp840;
_LL832: if(*(( int*) _temp830) == Cyc_Tcpat_Neg){ _LL837: _temp836=( struct Cyc_Set_Set*)((
struct Cyc_Tcpat_Neg_struct*) _temp830)->f1; goto _LL833;} else{ goto _LL834;}
_LL834: if(*(( int*) _temp830) == Cyc_Tcpat_Pos){ _LL841: _temp840=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Pos_struct*) _temp830)->f1; goto _LL839; _LL839: _temp838=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp830)->f2; goto
_LL835;} else{ goto _LL831;} _LL833: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp836, c)){(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp842=(
char*)"add_neg called when constructor already in set"; struct _tagged_string
_temp843; _temp843.curr= _temp842; _temp843.base= _temp842; _temp843.last_plus_one=
_temp842 + 47; _temp843;}));} if( c->span != 0?(( int(*)( struct Cyc_Set_Set* s))
Cyc_Set_cardinality)( _temp836) + 1 >=( int)({ struct Cyc_Core_Opt* _temp844= c->span;
if( _temp844 == 0){ _throw( Null_Exception);} _temp844->v;}): 0){(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp845=( char*)"add_neg called when |cs U {c}| >= span(c)"; struct
_tagged_string _temp846; _temp846.curr= _temp845; _temp846.base= _temp845;
_temp846.last_plus_one= _temp845 + 42; _temp846;}));} return( void*)({ struct
Cyc_Tcpat_Neg_struct* _temp847=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp847[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp848; _temp848.tag= Cyc_Tcpat_Neg; _temp848.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp836,
c); _temp848;}); _temp847;}); _LL835: return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp849=( char*)"add_neg called when td is Positive";
struct _tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 35; _temp850;})); _LL831:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp851= td; struct Cyc_List_List*
_temp857; struct Cyc_Tcpat_Con_s* _temp859; struct Cyc_Set_Set* _temp861; _LL853:
if(*(( int*) _temp851) == Cyc_Tcpat_Pos){ _LL860: _temp859=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Pos_struct*) _temp851)->f1; goto _LL858; _LL858: _temp857=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*) _temp851)->f2; goto
_LL854;} else{ goto _LL855;} _LL855: if(*(( int*) _temp851) == Cyc_Tcpat_Neg){
_LL862: _temp861=( struct Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp851)->f1;
goto _LL856;} else{ goto _LL852;} _LL854: if( Cyc_Tcpat_compare_con( c, _temp859)
== 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL856:
if((( int(*)( struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)(
_temp861, c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span != 0?( int)({
struct Cyc_Core_Opt* _temp863= c->span; if( _temp863 == 0){ _throw(
Null_Exception);} _temp863->v;}) ==(( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp861) + 1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL852:;} struct _tuple10{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp864= ctxt; struct Cyc_List_List _temp870;
struct Cyc_List_List* _temp871; struct _tuple10* _temp873; struct _tuple10
_temp875; struct Cyc_List_List* _temp876; struct Cyc_Tcpat_Con_s* _temp878;
_LL866: if( _temp864 == 0){ goto _LL867;} else{ goto _LL868;} _LL868: if(
_temp864 == 0){ goto _LL865;} else{ _temp870=* _temp864; _LL874: _temp873=(
struct _tuple10*) _temp870.hd; _temp875=* _temp873; _LL879: _temp878= _temp875.f1;
goto _LL877; _LL877: _temp876= _temp875.f2; goto _LL872; _LL872: _temp871=(
struct Cyc_List_List*) _temp870.tl; goto _LL869;} _LL867: return 0; _LL869:
return({ struct Cyc_List_List* _temp880=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp880->hd=( void*)({ struct _tuple10*
_temp881=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp881->f1=
_temp878; _temp881->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp882=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp882->hd=(
void*) dsc; _temp882->tl= _temp876; _temp882;}); _temp881;}); _temp880->tl=
_temp871; _temp880;}); _LL865:;} static struct Cyc_List_List* Cyc_Tcpat_norm_context(
struct Cyc_List_List* ctxt){ struct Cyc_List_List* _temp883= ctxt; struct Cyc_List_List
_temp889; struct Cyc_List_List* _temp890; struct _tuple10* _temp892; struct
_tuple10 _temp894; struct Cyc_List_List* _temp895; struct Cyc_Tcpat_Con_s*
_temp897; _LL885: if( _temp883 == 0){ goto _LL886;} else{ goto _LL887;} _LL887:
if( _temp883 == 0){ goto _LL884;} else{ _temp889=* _temp883; _LL893: _temp892=(
struct _tuple10*) _temp889.hd; _temp894=* _temp892; _LL898: _temp897= _temp894.f1;
goto _LL896; _LL896: _temp895= _temp894.f2; goto _LL891; _LL891: _temp890=(
struct Cyc_List_List*) _temp889.tl; goto _LL888;} _LL886: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp899=( char*)"norm_context: empty context"; struct _tagged_string _temp900;
_temp900.curr= _temp899; _temp900.base= _temp899; _temp900.last_plus_one=
_temp899 + 28; _temp900;})); _LL888: return Cyc_Tcpat_augment( _temp890,( void*)({
struct Cyc_Tcpat_Pos_struct* _temp901=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp901[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp902; _temp902.tag= Cyc_Tcpat_Pos; _temp902.f1= _temp897; _temp902.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp895);
_temp902;}); _temp901;})); _LL884:;} static void* Cyc_Tcpat_build_desc( struct
Cyc_List_List* ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple5
_temp904=({ struct _tuple5 _temp903; _temp903.f1= ctxt; _temp903.f2= work;
_temp903;}); struct Cyc_List_List* _temp914; struct Cyc_List_List* _temp916;
struct Cyc_List_List* _temp918; struct Cyc_List_List* _temp920; struct Cyc_List_List*
_temp922; struct Cyc_List_List* _temp924; struct Cyc_List_List* _temp926; struct
Cyc_List_List _temp928; struct Cyc_List_List* _temp929; struct _tuple9* _temp931;
struct _tuple9 _temp933; struct Cyc_List_List* _temp934; struct Cyc_List_List*
_temp936; struct Cyc_List_List* _temp938; struct Cyc_List_List* _temp940; struct
Cyc_List_List _temp942; struct Cyc_List_List* _temp943; struct _tuple10*
_temp945; struct _tuple10 _temp947; struct Cyc_List_List* _temp948; struct Cyc_Tcpat_Con_s*
_temp950; _LL906: _LL917: _temp916= _temp904.f1; if( _temp916 == 0){ goto _LL915;}
else{ goto _LL908;} _LL915: _temp914= _temp904.f2; if( _temp914 == 0){ goto
_LL907;} else{ goto _LL908;} _LL908: _LL921: _temp920= _temp904.f1; if( _temp920
== 0){ goto _LL919;} else{ goto _LL910;} _LL919: _temp918= _temp904.f2; goto
_LL909; _LL910: _LL925: _temp924= _temp904.f1; goto _LL923; _LL923: _temp922=
_temp904.f2; if( _temp922 == 0){ goto _LL911;} else{ goto _LL912;} _LL912:
_LL941: _temp940= _temp904.f1; if( _temp940 == 0){ goto _LL905;} else{ _temp942=*
_temp940; _LL946: _temp945=( struct _tuple10*) _temp942.hd; _temp947=* _temp945;
_LL951: _temp950= _temp947.f1; goto _LL949; _LL949: _temp948= _temp947.f2; goto
_LL944; _LL944: _temp943=( struct Cyc_List_List*) _temp942.tl; goto _LL927;}
_LL927: _temp926= _temp904.f2; if( _temp926 == 0){ goto _LL905;} else{ _temp928=*
_temp926; _LL932: _temp931=( struct _tuple9*) _temp928.hd; _temp933=* _temp931;
_LL939: _temp938= _temp933.f1; goto _LL937; _LL937: _temp936= _temp933.f2; goto
_LL935; _LL935: _temp934= _temp933.f3; goto _LL930; _LL930: _temp929=( struct
Cyc_List_List*) _temp928.tl; goto _LL913;} _LL907: return dsc; _LL909: return((
void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({
char* _temp952=( char*)"build_desc: ctxt and work don't match"; struct
_tagged_string _temp953; _temp953.curr= _temp952; _temp953.base= _temp952;
_temp953.last_plus_one= _temp952 + 38; _temp953;})); _LL911: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp954=( char*)"build_desc: ctxt and work don't match"; struct _tagged_string
_temp955; _temp955.curr= _temp954; _temp955.base= _temp954; _temp955.last_plus_one=
_temp954 + 38; _temp955;})); _LL913: { struct Cyc_Tcpat_Pos_struct* td=({ struct
Cyc_Tcpat_Pos_struct* _temp956=( struct Cyc_Tcpat_Pos_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Pos_struct)); _temp956[ 0]=({ struct Cyc_Tcpat_Pos_struct
_temp957; _temp957.tag= Cyc_Tcpat_Pos; _temp957.f1= _temp950; _temp957.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( _temp948),({
struct Cyc_List_List* _temp958=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp958->hd=( void*) dsc; _temp958->tl= _temp934;
_temp958;})); _temp957;}); _temp956;}); return Cyc_Tcpat_build_desc( _temp943,(
void*) td, _temp929);} _LL905:;} static void* Cyc_Tcpat_match( void* p, struct
Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp959=
allmrules; struct Cyc_List_List _temp965; struct Cyc_List_List* _temp966; struct
_tuple0* _temp968; struct _tuple0 _temp970; void* _temp971; void* _temp973;
_LL961: if( _temp959 == 0){ goto _LL962;} else{ goto _LL963;} _LL963: if(
_temp959 == 0){ goto _LL960;} else{ _temp965=* _temp959; _LL969: _temp968=(
struct _tuple0*) _temp965.hd; _temp970=* _temp968; _LL974: _temp973= _temp970.f1;
goto _LL972; _LL972: _temp971= _temp970.f2; goto _LL967; _LL967: _temp966=(
struct Cyc_List_List*) _temp965.tl; goto _LL964;} _LL962: return( void*) Cyc_Tcpat_Failure;
_LL964: return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules)) Cyc_Tcpat_match)( _temp973, 0, dsc, 0, 0, _temp971,
_temp966); _LL960:;} static void* Cyc_Tcpat_match_compile( struct Cyc_List_List*
allmrules){ return(( void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)((
void*)({ struct Cyc_Tcpat_Neg_struct* _temp975=( struct Cyc_Tcpat_Neg_struct*)
GC_malloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp975[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp976; _temp976.tag= Cyc_Tcpat_Neg; _temp976.f1= Cyc_Tcpat_empty_con_set();
_temp976;}); _temp975;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp977= work; struct Cyc_List_List
_temp985; struct Cyc_List_List* _temp986; struct _tuple9* _temp988; struct
_tuple9 _temp990; struct Cyc_List_List* _temp991; struct Cyc_List_List* _temp993;
struct Cyc_List_List* _temp995; struct Cyc_List_List _temp997; struct Cyc_List_List*
_temp998; struct _tuple9* _temp1000; struct _tuple9 _temp1002; struct Cyc_List_List*
_temp1003; struct Cyc_List_List* _temp1005; struct Cyc_List_List* _temp1007;
_LL979: if( _temp977 == 0){ goto _LL980;} else{ goto _LL981;} _LL981: if(
_temp977 == 0){ goto _LL983;} else{ _temp985=* _temp977; _LL989: _temp988=(
struct _tuple9*) _temp985.hd; _temp990=* _temp988; _LL996: _temp995= _temp990.f1;
if( _temp995 == 0){ goto _LL994;} else{ goto _LL983;} _LL994: _temp993= _temp990.f2;
if( _temp993 == 0){ goto _LL992;} else{ goto _LL983;} _LL992: _temp991= _temp990.f3;
if( _temp991 == 0){ goto _LL987;} else{ goto _LL983;} _LL987: _temp986=( struct
Cyc_List_List*) _temp985.tl; goto _LL982;} _LL983: if( _temp977 == 0){ goto
_LL978;} else{ _temp997=* _temp977; _LL1001: _temp1000=( struct _tuple9*)
_temp997.hd; _temp1002=* _temp1000; _LL1008: _temp1007= _temp1002.f1; goto
_LL1006; _LL1006: _temp1005= _temp1002.f2; goto _LL1004; _LL1004: _temp1003=
_temp1002.f3; goto _LL999; _LL999: _temp998=( struct Cyc_List_List*) _temp997.tl;
goto _LL984;} _LL980: return( void*)({ struct Cyc_Tcpat_Success_struct*
_temp1009=( struct Cyc_Tcpat_Success_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_Success_struct));
_temp1009[ 0]=({ struct Cyc_Tcpat_Success_struct _temp1010; _temp1010.tag= Cyc_Tcpat_Success;
_temp1010.f1=( void*) right_hand_side; _temp1010;}); _temp1009;}); _LL982:
return(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_norm_context(
ctx), _temp986, right_hand_side, rules); _LL984: if(( _temp1007 == 0? 1:
_temp1005 == 0)? 1: _temp1003 == 0){ return(( void*(*)( struct _tagged_string))
Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp1011=( char*)"tcpat:and_match: malformed work frame";
struct _tagged_string _temp1012; _temp1012.curr= _temp1011; _temp1012.base=
_temp1011; _temp1012.last_plus_one= _temp1011 + 38; _temp1012;}));}{ struct Cyc_List_List
_temp1016; struct Cyc_List_List* _temp1017; void* _temp1019; struct Cyc_List_List*
_temp1014=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1013= _temp1007;
if( _temp1013 == 0){ _throw( Null_Exception);} _temp1013;}); _temp1016=*
_temp1014; _LL1020: _temp1019=( void*) _temp1016.hd; goto _LL1018; _LL1018:
_temp1017=( struct Cyc_List_List*) _temp1016.tl; goto _LL1015; _LL1015: { struct
Cyc_List_List _temp1024; struct Cyc_List_List* _temp1025; struct Cyc_List_List*
_temp1027; struct Cyc_List_List* _temp1022=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1021= _temp1005; if( _temp1021 == 0){ _throw( Null_Exception);} _temp1021;});
_temp1024=* _temp1022; _LL1028: _temp1027=( struct Cyc_List_List*) _temp1024.hd;
goto _LL1026; _LL1026: _temp1025=( struct Cyc_List_List*) _temp1024.tl; goto
_LL1023; _LL1023: { struct Cyc_List_List _temp1032; struct Cyc_List_List*
_temp1033; void* _temp1035; struct Cyc_List_List* _temp1030=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1029= _temp1003; if( _temp1029 == 0){ _throw(
Null_Exception);} _temp1029;}); _temp1032=* _temp1030; _LL1036: _temp1035=( void*)
_temp1032.hd; goto _LL1034; _LL1034: _temp1033=( struct Cyc_List_List*)
_temp1032.tl; goto _LL1031; _LL1031: { struct _tuple9* wf=({ struct _tuple9*
_temp1038=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp1038->f1=
_temp1017; _temp1038->f2= _temp1025; _temp1038->f3= _temp1033; _temp1038;});
return(( void*(*)( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_match)( _temp1019, _temp1027, _temp1035, ctx,({ struct Cyc_List_List*
_temp1037=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1037->hd=( void*) wf; _temp1037->tl= _temp998; _temp1037;}),
right_hand_side, rules);}}}} _LL978:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp1039= dsc; struct Cyc_Set_Set*
_temp1045; struct Cyc_List_List* _temp1047; struct Cyc_Tcpat_Con_s* _temp1049;
_LL1041: if(*(( int*) _temp1039) == Cyc_Tcpat_Neg){ _LL1046: _temp1045=( struct
Cyc_Set_Set*)(( struct Cyc_Tcpat_Neg_struct*) _temp1039)->f1; goto _LL1042;}
else{ goto _LL1043;} _LL1043: if(*(( int*) _temp1039) == Cyc_Tcpat_Pos){ _LL1050:
_temp1049=( struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_Pos_struct*) _temp1039)->f1;
goto _LL1048; _LL1048: _temp1047=( struct Cyc_List_List*)(( struct Cyc_Tcpat_Pos_struct*)
_temp1039)->f2; goto _LL1044;} else{ goto _LL1040;} _LL1042: { void* any=( void*)({
struct Cyc_Tcpat_Neg_struct* _temp1052=( struct Cyc_Tcpat_Neg_struct*) GC_malloc(
sizeof( struct Cyc_Tcpat_Neg_struct)); _temp1052[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp1053; _temp1053.tag= Cyc_Tcpat_Neg; _temp1053.f1= Cyc_Tcpat_empty_con_set();
_temp1053;}); _temp1052;}); struct Cyc_List_List* res= 0;{ int i= 0; for( 0; i <
pcon->arity; ++ i){ res=({ struct Cyc_List_List* _temp1051=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1051->hd=( void*) any; _temp1051->tl=
res; _temp1051;});}} return res;} _LL1044: return _temp1047; _LL1040:;} static
struct Cyc_List_List* Cyc_Tcpat_getoarg( struct Cyc_List_List* obj, int i){
return({ struct Cyc_List_List* _temp1054=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1054->hd=( void*)( i + 1); _temp1054->tl=
obj; _temp1054;});} static struct Cyc_List_List* Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s*
pcon, struct Cyc_List_List* obj){ return(( struct Cyc_List_List*(*)( int n,
struct Cyc_List_List*(* f)( struct Cyc_List_List*, int), struct Cyc_List_List*
env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg, obj);} static void*
Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules){ void* _temp1055= p; struct Cyc_List_List* _temp1061; struct Cyc_Tcpat_Con_s*
_temp1063; _LL1057: if( _temp1055 ==( void*) Cyc_Tcpat_Any){ goto _LL1058;}
else{ goto _LL1059;} _LL1059: if(( unsigned int) _temp1055 > 1u?*(( int*)
_temp1055) == Cyc_Tcpat_Con: 0){ _LL1064: _temp1063=( struct Cyc_Tcpat_Con_s*)((
struct Cyc_Tcpat_Con_struct*) _temp1055)->f1; goto _LL1062; _LL1062: _temp1061=(
struct Cyc_List_List*)(( struct Cyc_Tcpat_Con_struct*) _temp1055)->f2; goto
_LL1060;} else{ goto _LL1056;} _LL1058: return(( void*(*)( struct Cyc_List_List*
ctx, struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List*
rules)) Cyc_Tcpat_and_match)( Cyc_Tcpat_augment( ctx, dsc), work,
right_hand_side, rules); _LL1060: { void* _temp1065= Cyc_Tcpat_static_match(
_temp1063, dsc); _LL1067: if( _temp1065 ==( void*) Cyc_Tcpat_Yes){ goto _LL1068;}
else{ goto _LL1069;} _LL1069: if( _temp1065 ==( void*) Cyc_Tcpat_No){ goto
_LL1070;} else{ goto _LL1071;} _LL1071: if( _temp1065 ==( void*) Cyc_Tcpat_Maybe){
goto _LL1072;} else{ goto _LL1066;} _LL1068: { struct Cyc_List_List* ctx2=({
struct Cyc_List_List* _temp1075=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1075->hd=( void*)({ struct _tuple10* _temp1076=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp1076->f1= _temp1063;
_temp1076->f2= 0; _temp1076;}); _temp1075->tl= ctx; _temp1075;}); struct _tuple9*
work_frame=({ struct _tuple9* _temp1074=( struct _tuple9*) GC_malloc( sizeof(
struct _tuple9)); _temp1074->f1= _temp1061; _temp1074->f2= Cyc_Tcpat_getoargs(
_temp1063, obj); _temp1074->f3= Cyc_Tcpat_getdargs( _temp1063, dsc); _temp1074;});
struct Cyc_List_List* work2=({ struct Cyc_List_List* _temp1073=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1073->hd=( void*) work_frame;
_temp1073->tl= work; _temp1073;}); return(( void*(*)( struct Cyc_List_List* ctx,
struct Cyc_List_List* work, void* right_hand_side, struct Cyc_List_List* rules))
Cyc_Tcpat_and_match)( ctx2, work2, right_hand_side, rules);} _LL1070: return((
void*(*)( void* dsc, struct Cyc_List_List* allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL1072: { struct Cyc_List_List* ctx2=({ struct Cyc_List_List*
_temp1081=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1081->hd=( void*)({ struct _tuple10* _temp1082=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp1082->f1= _temp1063; _temp1082->f2= 0;
_temp1082;}); _temp1081->tl= ctx; _temp1081;}); struct _tuple9* work_frame=({
struct _tuple9* _temp1080=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9));
_temp1080->f1= _temp1061; _temp1080->f2= Cyc_Tcpat_getoargs( _temp1063, obj);
_temp1080->f3= Cyc_Tcpat_getdargs( _temp1063, dsc); _temp1080;}); struct Cyc_List_List*
work2=({ struct Cyc_List_List* _temp1079=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp1079->hd=( void*) work_frame; _temp1079->tl=
work; _temp1079;}); void* s=(( void*(*)( struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules)) Cyc_Tcpat_and_match)(
ctx2, work2, right_hand_side, rules); void* f=(( void*(*)( void* dsc, struct Cyc_List_List*
allmrules)) Cyc_Tcpat_or_match)( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg(
dsc, _temp1063), work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct*
_temp1077=( struct Cyc_Tcpat_IfEq_struct*) GC_malloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp1077[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp1078; _temp1078.tag= Cyc_Tcpat_IfEq;
_temp1078.f1= obj; _temp1078.f2= _temp1063; _temp1078.f3=( void*) s; _temp1078.f4=(
void*) f; _temp1078;}); _temp1077;});} _LL1066:;} _LL1056:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp1083= d; void* _temp1091; void* _temp1093; void*
_temp1095; struct Cyc_Tcpat_Con_s* _temp1097; struct Cyc_List_List* _temp1099;
_LL1085: if( _temp1083 ==( void*) Cyc_Tcpat_Failure){ goto _LL1086;} else{ goto
_LL1087;} _LL1087: if(( unsigned int) _temp1083 > 1u?*(( int*) _temp1083) == Cyc_Tcpat_Success:
0){ _LL1092: _temp1091=( void*)(( struct Cyc_Tcpat_Success_struct*) _temp1083)->f1;
goto _LL1088;} else{ goto _LL1089;} _LL1089: if(( unsigned int) _temp1083 > 1u?*((
int*) _temp1083) == Cyc_Tcpat_IfEq: 0){ _LL1100: _temp1099=( struct Cyc_List_List*)((
struct Cyc_Tcpat_IfEq_struct*) _temp1083)->f1; goto _LL1098; _LL1098: _temp1097=(
struct Cyc_Tcpat_Con_s*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1083)->f2; goto
_LL1096; _LL1096: _temp1095=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp1083)->f3;
goto _LL1094; _LL1094: _temp1093=( void*)(( struct Cyc_Tcpat_IfEq_struct*)
_temp1083)->f4; goto _LL1090;} else{ goto _LL1084;} _LL1086: not_exhaust( env1);
goto _LL1084; _LL1088: rhs_appears( env2, _temp1091); goto _LL1084; _LL1090:((
void(*)( void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)(
void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp1095,
not_exhaust, env1, rhs_appears, env2);(( void(*)( void* d, void(* not_exhaust)(
void*), void* env1, void(* rhs_appears)( void*, void*), void* env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp1093, not_exhaust, env1, rhs_appears, env2); goto _LL1084; _LL1084:;}
struct _tuple11{ int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; struct _tuple11* f2; } ; static struct _tuple12* Cyc_Tcpat_get_match(
int* r, struct Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat(
swc->pattern); struct _tuple11* rhs=({ struct _tuple11* _temp1106=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp1106->f1= 0; _temp1106->f2=(
swc->pattern)->loc; _temp1106;}); void* sp; if( swc->where_clause != 0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp1101=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1101->hd=( void*) sp0; _temp1101->tl=({ struct Cyc_List_List*
_temp1102=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1102->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp1102->tl= 0; _temp1102;});
_temp1101;}));* r=* r + 1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp1103=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1103->hd=( void*) sp0; _temp1103->tl=({ struct Cyc_List_List* _temp1104=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1104->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp1104->tl= 0; _temp1104;}); _temp1103;}));}
return({ struct _tuple12* _temp1105=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1105->f1= sp; _temp1105->f2= rhs; _temp1105;});} static
void Cyc_Tcpat_not_exhaust_err( struct Cyc_Position_Segment* loc){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1107=( char*)"patterns may not be exhaustive";
struct _tagged_string _temp1108; _temp1108.curr= _temp1107; _temp1108.base=
_temp1107; _temp1108.last_plus_one= _temp1107 + 31; _temp1108;}));} static void
Cyc_Tcpat_rule_occurs( int dummy, struct _tuple11* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int where_ctr= 0;
struct Cyc_List_List* match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(*
f)( int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcpat_get_match,& where_ctr, swcs); void* dec_tree=(( void*(*)(
struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( match_rules);(( void(*)(
void* d, void(* not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment*
env1, void(* rhs_appears)( int, struct _tuple11*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
dec_tree, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
match_rules != 0; match_rules=({ struct Cyc_List_List* _temp1109= match_rules;
if( _temp1109 == 0){ _throw( Null_Exception);} _temp1109->tl;})){ struct
_tuple12 _temp1113; struct _tuple11* _temp1114; struct _tuple11 _temp1116;
struct Cyc_Position_Segment* _temp1117; int _temp1119; void* _temp1121; struct
_tuple12* _temp1111=( struct _tuple12*)({ struct Cyc_List_List* _temp1110=
match_rules; if( _temp1110 == 0){ _throw( Null_Exception);} _temp1110->hd;});
_temp1113=* _temp1111; _LL1122: _temp1121= _temp1113.f1; goto _LL1115; _LL1115:
_temp1114= _temp1113.f2; _temp1116=* _temp1114; _LL1120: _temp1119= _temp1116.f1;
goto _LL1118; _LL1118: _temp1117= _temp1116.f2; goto _LL1112; _LL1112: if( !
_temp1119){ Cyc_Tcutil_terr( _temp1117,( struct _tagged_string)({ char*
_temp1123=( char*)"redundant pattern"; struct _tagged_string _temp1124;
_temp1124.curr= _temp1123; _temp1124.base= _temp1123; _temp1124.last_plus_one=
_temp1123 + 18; _temp1124;}));}}} struct _tuple13{ struct Cyc_Position_Segment*
f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple13* pr){(*
pr).f2= 0; Cyc_Tcutil_warn((* pr).f1,( struct _tagged_string)({ char* _temp1125=(
char*)"pattern not exhaustive"; struct _tagged_string _temp1126; _temp1126.curr=
_temp1125; _temp1126.base= _temp1125; _temp1126.last_plus_one= _temp1125 + 23;
_temp1126;}));} static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} struct
_tuple14{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
match_rules=({ struct Cyc_List_List* _temp1128=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1128->hd=( void*)({ struct
_tuple14* _temp1129=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14));
_temp1129->f1= Cyc_Tcpat_compile_pat( p); _temp1129->f2= 0; _temp1129;});
_temp1128->tl= 0; _temp1128;}); void* dec_tree=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( match_rules); struct _tuple13* exhaust_env=({
struct _tuple13* _temp1127=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp1127->f1= loc; _temp1127->f2= 1; _temp1127;});(( void(*)( void*
d, void(* not_exhaust)( struct _tuple13*), struct _tuple13* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree,
Cyc_Tcpat_not_exhaust_warn, exhaust_env, Cyc_Tcpat_dummy_fn, 0); return(*
exhaust_env).f2;} static struct _tuple12* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple11* rhs=({
struct _tuple11* _temp1131=( struct _tuple11*) GC_malloc( sizeof( struct
_tuple11)); _temp1131->f1= 0; _temp1131->f2=( swc->pattern)->loc; _temp1131;});
return({ struct _tuple12* _temp1130=( struct _tuple12*) GC_malloc( sizeof(
struct _tuple12)); _temp1130->f1= sp0; _temp1130->f2= rhs; _temp1130;});} static
void Cyc_Tcpat_not_exhaust_err2( struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
match_rules=(( struct Cyc_List_List*(*)( struct _tuple12*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
dec_tree=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
match_rules);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple11*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( dec_tree, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; match_rules != 0; match_rules=({ struct
Cyc_List_List* _temp1132= match_rules; if( _temp1132 == 0){ _throw(
Null_Exception);} _temp1132->tl;})){ struct _tuple12 _temp1136; struct _tuple11*
_temp1137; struct _tuple11 _temp1139; struct Cyc_Position_Segment* _temp1140;
int _temp1142; void* _temp1144; struct _tuple12* _temp1134=( struct _tuple12*)({
struct Cyc_List_List* _temp1133= match_rules; if( _temp1133 == 0){ _throw(
Null_Exception);} _temp1133->hd;}); _temp1136=* _temp1134; _LL1145: _temp1144=
_temp1136.f1; goto _LL1138; _LL1138: _temp1137= _temp1136.f2; _temp1139=*
_temp1137; _LL1143: _temp1142= _temp1139.f1; goto _LL1141; _LL1141: _temp1140=
_temp1139.f2; goto _LL1135; _LL1135: if( ! _temp1142){ Cyc_Tcutil_terr(
_temp1140,( struct _tagged_string)({ char* _temp1146=( char*)"redundant pattern";
struct _tagged_string _temp1147; _temp1147.curr= _temp1146; _temp1147.base=
_temp1146; _temp1147.last_plus_one= _temp1146 + 18; _temp1147;}));}}}